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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];extern char Cyc_Lexing_Error[10];
struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct
_tagged_arr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct
Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*
s);extern char Cyc_Set_Absent[11];unsigned int Cyc_strlen(struct _tagged_arr s);int
Cyc_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_str_sepstr(
struct Cyc_List_List*,struct _tagged_arr);struct _tagged_arr Cyc_zstrncpy(struct
_tagged_arr,struct _tagged_arr,unsigned int);struct _tagged_arr Cyc_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Xarray_Xarray{struct
_tagged_arr elmts;int num_elmts;};void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);void Cyc_Xarray_add(struct
Cyc_Xarray_Xarray*,void*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*,
struct _tagged_arr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,
struct _tagged_arr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;
struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern struct Cyc_Core_Opt*Cyc_Parse_lbuf;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[
27];struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Abstractdeclarator*
f1;};extern char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AggrFieldDeclList_tok[26];struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{char*tag;void*f1;};
extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;int f1;};extern char
Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};extern char Cyc_DeclList_tok[
17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;struct Cyc_Declaration_spec*
f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{char*tag;
struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{
char*tag;struct Cyc_Absyn_Enumfield*f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{
char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct
Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[
21];struct _tuple3{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple3*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple4{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple4*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple5{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple5*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple6{struct Cyc_List_List*f1;int
f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};
struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple6*f1;};extern char Cyc_ParamDeclList_tok[
22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple1*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple0*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple7{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple7*f1;};extern char Cyc_Rgnorder_tok[
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
extern char Cyc_YY1[8];struct _tuple8{struct Cyc_Absyn_Conref*f1;struct Cyc_Absyn_Conref*
f2;};struct Cyc_YY1_struct{char*tag;struct _tuple8*f1;};extern char Cyc_YY2[8];
struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YY3[8];
struct Cyc_YY3_struct{char*tag;struct _tuple6*f1;};extern char Cyc_YY4[8];struct Cyc_YY4_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};extern
struct Cyc_Yyltype Cyc_yylloc;extern void*Cyc_yylval;struct Cyc_Dict_Dict;extern char
Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);void Cyc_yyerror(struct _tagged_arr s){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),
s));}struct Cyc_Lex_Trie;struct Cyc_Lex_One_struct{int tag;int f1;struct Cyc_Lex_Trie*
f2;};struct Cyc_Lex_Many_struct{int tag;struct Cyc_Lex_Trie**f1;};struct Cyc_Lex_Trie{
void*children;int shared_str;};static int Cyc_Lex_num_kws=0;static struct _tagged_arr
Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};static struct Cyc_Xarray_Xarray*
Cyc_Lex_symbols=0;static struct Cyc_Lex_Trie*Cyc_Lex_ids_trie=0;static struct Cyc_Lex_Trie*
Cyc_Lex_typedefs_trie=0;static int Cyc_Lex_comment_depth=0;static struct _tuple5 Cyc_Lex_token_int_pair={(
void*)0,0};static char _tmp0[8]="*bogus*";static struct _tagged_arr Cyc_Lex_bogus_string={
_tmp0,_tmp0,_tmp0 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Lex_bogus_string};static char Cyc_Lex_token_char='\000';static char _tmp1[1]="";
static struct _tagged_arr Cyc_Lex_token_string={_tmp1,_tmp1,_tmp1 + 1};static struct
_tuple5*Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;static struct _tuple0*Cyc_Lex_token_qvar=&
Cyc_Lex_token_id_pair;static int Cyc_Lex_runaway_start=0;static void Cyc_Lex_err(
struct _tagged_arr msg,struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=
Cyc_Position_segment_of_abs(Cyc_Lexing_lexeme_start(lb),Cyc_Lexing_lexeme_end(lb));
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}static void Cyc_Lex_runaway_err(
struct _tagged_arr msg,struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=
Cyc_Position_segment_of_abs(Cyc_Lex_runaway_start,Cyc_Lexing_lexeme_start(lb));
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}struct _tuple10{struct
_tagged_arr f1;short f2;};static char _tmp2[14]="__attribute__";static char _tmp3[9]="abstract";
static char _tmp4[5]="auto";static char _tmp5[6]="break";static char _tmp6[7]="calloc";
static char _tmp7[5]="case";static char _tmp8[6]="catch";static char _tmp9[5]="char";
static char _tmpA[8]="codegen";static char _tmpB[6]="const";static char _tmpC[9]="continue";
static char _tmpD[4]="cut";static char _tmpE[8]="default";static char _tmpF[3]="do";
static char _tmp10[7]="double";static char _tmp11[5]="else";static char _tmp12[5]="enum";
static char _tmp13[7]="extern";static char _tmp14[9]="fallthru";static char _tmp15[5]="fill";
static char _tmp16[6]="float";static char _tmp17[4]="for";static char _tmp18[9]="forarray";
static char _tmp19[6]="__gen";static char _tmp1A[5]="goto";static char _tmp1B[3]="if";
static char _tmp1C[7]="inline";static char _tmp1D[4]="int";static char _tmp1E[4]="let";
static char _tmp1F[5]="long";static char _tmp20[7]="malloc";static char _tmp21[10]="namespace";
static char _tmp22[4]="new";static char _tmp23[11]="NOZEROTERM";static char _tmp24[5]="NULL";
static char _tmp25[9]="offsetof";static char _tmp26[8]="rcalloc";static char _tmp27[9]="region_t";
static char _tmp28[7]="region";static char _tmp29[8]="regions";static char _tmp2A[9]="register";
static char _tmp2B[13]="reset_region";static char _tmp2C[9]="restrict";static char
_tmp2D[7]="return";static char _tmp2E[8]="rmalloc";static char _tmp2F[5]="rnew";
static char _tmp30[6]="short";static char _tmp31[7]="signed";static char _tmp32[7]="sizeof";
static char _tmp33[9]="sizeof_t";static char _tmp34[7]="splice";static char _tmp35[7]="static";
static char _tmp36[7]="struct";static char _tmp37[7]="switch";static char _tmp38[6]="tag_t";
static char _tmp39[6]="throw";static char _tmp3A[4]="try";static char _tmp3B[7]="tunion";
static char _tmp3C[8]="typedef";static char _tmp3D[6]="union";static char _tmp3E[9]="unsigned";
static char _tmp3F[6]="using";static char _tmp40[5]="void";static char _tmp41[9]="volatile";
static char _tmp42[6]="while";static char _tmp43[8]="xtunion";static char _tmp44[9]="ZEROTERM";
static struct _tuple10 Cyc_Lex_rw_array[67]={{{_tmp2,_tmp2,_tmp2 + 14},358},{{_tmp3,
_tmp3,_tmp3 + 9},299},{{_tmp4,_tmp4,_tmp4 + 5},258},{{_tmp5,_tmp5,_tmp5 + 6},290},{{
_tmp6,_tmp6,_tmp6 + 7},312},{{_tmp7,_tmp7,_tmp7 + 5},277},{{_tmp8,_tmp8,_tmp8 + 6},
297},{{_tmp9,_tmp9,_tmp9 + 5},264},{{_tmpA,_tmpA,_tmpA + 8},307},{{_tmpB,_tmpB,
_tmpB + 6},272},{{_tmpC,_tmpC,_tmpC + 9},289},{{_tmpD,_tmpD,_tmpD + 4},308},{{_tmpE,
_tmpE,_tmpE + 8},278},{{_tmpF,_tmpF,_tmpF + 3},286},{{_tmp10,_tmp10,_tmp10 + 7},269},{{
_tmp11,_tmp11,_tmp11 + 5},283},{{_tmp12,_tmp12,_tmp12 + 5},292},{{_tmp13,_tmp13,
_tmp13 + 7},261},{{_tmp14,_tmp14,_tmp14 + 9},300},{{_tmp15,_tmp15,_tmp15 + 5},306},{{
_tmp16,_tmp16,_tmp16 + 6},268},{{_tmp17,_tmp17,_tmp17 + 4},287},{{_tmp18,_tmp18,
_tmp18 + 9},305},{{_tmp19,_tmp19,_tmp19 + 6},321},{{_tmp1A,_tmp1A,_tmp1A + 5},288},{{
_tmp1B,_tmp1B,_tmp1B + 3},282},{{_tmp1C,_tmp1C,_tmp1C + 7},279},{{_tmp1D,_tmp1D,
_tmp1D + 4},266},{{_tmp1E,_tmp1E,_tmp1E + 4},294},{{_tmp1F,_tmp1F,_tmp1F + 5},267},{{
_tmp20,_tmp20,_tmp20 + 7},310},{{_tmp21,_tmp21,_tmp21 + 10},302},{{_tmp22,_tmp22,
_tmp22 + 4},298},{{_tmp23,_tmp23,_tmp23 + 11},322},{{_tmp24,_tmp24,_tmp24 + 5},293},{{
_tmp25,_tmp25,_tmp25 + 9},281},{{_tmp26,_tmp26,_tmp26 + 8},313},{{_tmp27,_tmp27,
_tmp27 + 9},314},{{_tmp28,_tmp28,_tmp28 + 7},317},{{_tmp29,_tmp29,_tmp29 + 8},319},{{
_tmp2A,_tmp2A,_tmp2A + 9},259},{{_tmp2B,_tmp2B,_tmp2B + 13},320},{{_tmp2C,_tmp2C,
_tmp2C + 9},274},{{_tmp2D,_tmp2D,_tmp2D + 7},291},{{_tmp2E,_tmp2E,_tmp2E + 8},311},{{
_tmp2F,_tmp2F,_tmp2F + 5},318},{{_tmp30,_tmp30,_tmp30 + 6},265},{{_tmp31,_tmp31,
_tmp31 + 7},270},{{_tmp32,_tmp32,_tmp32 + 7},280},{{_tmp33,_tmp33,_tmp33 + 9},315},{{
_tmp34,_tmp34,_tmp34 + 7},309},{{_tmp35,_tmp35,_tmp35 + 7},260},{{_tmp36,_tmp36,
_tmp36 + 7},275},{{_tmp37,_tmp37,_tmp37 + 7},284},{{_tmp38,_tmp38,_tmp38 + 6},316},{{
_tmp39,_tmp39,_tmp39 + 6},295},{{_tmp3A,_tmp3A,_tmp3A + 4},296},{{_tmp3B,_tmp3B,
_tmp3B + 7},303},{{_tmp3C,_tmp3C,_tmp3C + 8},262},{{_tmp3D,_tmp3D,_tmp3D + 6},276},{{
_tmp3E,_tmp3E,_tmp3E + 9},271},{{_tmp3F,_tmp3F,_tmp3F + 6},301},{{_tmp40,_tmp40,
_tmp40 + 5},263},{{_tmp41,_tmp41,_tmp41 + 9},273},{{_tmp42,_tmp42,_tmp42 + 6},285},{{
_tmp43,_tmp43,_tmp43 + 8},304},{{_tmp44,_tmp44,_tmp44 + 9},323}};static int Cyc_Lex_trie_char(
int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}return c - 48;}static struct
Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct Cyc_Lex_Trie*t,struct _tagged_arr buff,int
offset,int len){int i=offset;int last=(offset + len)- 1;while(i <= last){void*_tmp45=(
void*)((struct Cyc_Lex_Trie*)_check_null(t))->children;struct Cyc_Lex_Trie**_tmp46;
int _tmp47;struct Cyc_Lex_Trie*_tmp48;_LL1: if(_tmp45 <= (void*)1?1:*((int*)_tmp45)
!= 1)goto _LL3;_tmp46=((struct Cyc_Lex_Many_struct*)_tmp45)->f1;_LL2: {int ch=Cyc_Lex_trie_char((
int)*((const char*)_check_unknown_subscript(buff,sizeof(char),i)));if(_tmp46[
_check_known_subscript_notnull(64,ch)]== 0)_tmp46[_check_known_subscript_notnull(
64,ch)]=({struct Cyc_Lex_Trie*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->children=(
void*)((void*)0);_tmp49->shared_str=0;_tmp49;});t=_tmp46[
_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if(_tmp45 <= (void*)1?1:*((
int*)_tmp45)!= 0)goto _LL5;_tmp47=((struct Cyc_Lex_One_struct*)_tmp45)->f1;_tmp48=((
struct Cyc_Lex_One_struct*)_tmp45)->f2;_LL4: if(_tmp47 == *((const char*)
_check_unknown_subscript(buff,sizeof(char),i)))t=_tmp48;else{struct Cyc_Lex_Trie**
_tmp4A=({unsigned int _tmp4F=64;struct Cyc_Lex_Trie**_tmp50=(struct Cyc_Lex_Trie**)
_cycalloc(_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp4F));{unsigned int _tmp51=
_tmp4F;unsigned int j;for(j=0;j < _tmp51;j ++){_tmp50[j]=0;}}_tmp50;});_tmp4A[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp47))]=_tmp48;{int _tmp4B=
Cyc_Lex_trie_char((int)*((const char*)_check_unknown_subscript(buff,sizeof(char),
i)));_tmp4A[_check_known_subscript_notnull(64,_tmp4B)]=({struct Cyc_Lex_Trie*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->children=(void*)((void*)0);_tmp4C->shared_str=
0;_tmp4C;});(void*)(t->children=(void*)((void*)({struct Cyc_Lex_Many_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Lex_Many_struct _tmp4E;
_tmp4E.tag=1;_tmp4E.f1=_tmp4A;_tmp4E;});_tmp4D;})));t=_tmp4A[
_check_known_subscript_notnull(64,_tmp4B)];}}++ i;goto _LL0;_LL5: if((int)_tmp45 != 
0)goto _LL0;_LL6: while(i <= last){struct Cyc_Lex_Trie*_tmp52=({struct Cyc_Lex_Trie*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->children=(void*)((void*)0);_tmp55->shared_str=
0;_tmp55;});(void*)(((struct Cyc_Lex_Trie*)_check_null(t))->children=(void*)((
void*)({struct Cyc_Lex_One_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({
struct Cyc_Lex_One_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(int)*((const char*)
_check_unknown_subscript(buff,sizeof(char),i ++));_tmp54.f2=_tmp52;_tmp54;});
_tmp53;})));t=_tmp52;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(struct
_tagged_arr buff,int offset,int len){struct Cyc_Lex_Trie*_tmp56=Cyc_Lex_trie_lookup(
Cyc_Lex_ids_trie,buff,offset,len);if(_tmp56->shared_str == 0){struct _tagged_arr
_tmp57=Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(_tmp57,(struct
_tagged_arr)_tagged_arr_plus(buff,sizeof(char),offset),(unsigned int)len);{int
ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _tagged_arr*))Cyc_Xarray_add_ind)((
struct Cyc_Xarray_Xarray*)_check_null(Cyc_Lex_symbols),({struct _tagged_arr*_tmp58=
_cycalloc(sizeof(*_tmp58));_tmp58[0]=(struct _tagged_arr)_tmp57;_tmp58;}));_tmp56->shared_str=
ans;}}return _tmp56->shared_str;}static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*
lbuf){return Cyc_Lex_str_index((struct _tagged_arr)lbuf->lex_buffer,lbuf->lex_start_pos,
lbuf->lex_curr_pos - lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(
struct _tagged_arr*sptr){struct _tagged_arr _tmp59=*sptr;struct Cyc_Lex_Trie*_tmp5A=
Cyc_Lex_trie_lookup(Cyc_Lex_typedefs_trie,_tmp59,0,(int)(_get_arr_size(_tmp59,
sizeof(char))- 1));((struct Cyc_Lex_Trie*)_check_null(_tmp5A))->shared_str=1;
return;}static struct _tagged_arr*Cyc_Lex_get_symbol(int symbol_num){return((struct
_tagged_arr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)((struct Cyc_Xarray_Xarray*)
_check_null(Cyc_Lex_symbols),symbol_num);}static int Cyc_Lex_int_of_char(char c){
if('0' <= c?c <= '9': 0)return c - '0';else{if('a' <= c?c <= 'f': 0)return(10 + c)- 'a';
else{if('A' <= c?c <= 'F': 0)return(10 + c)- 'A';else{(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp5C;_tmp5C.tag=Cyc_Core_Invalid_argument;_tmp5C.f1=({const char*_tmp5D="string to integer conversion";
_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,29));});_tmp5C;});_tmp5B;}));}}}}
static struct _tuple5*Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int
end,int base){unsigned int n=0;int end2=lbuf->lex_curr_pos - end;struct _tagged_arr
buff=lbuf->lex_buffer;int i=start + lbuf->lex_start_pos;{int i=start + lbuf->lex_start_pos;
for(0;i < end2;++ i){char c=*((char*)_check_unknown_subscript(buff,sizeof(char),i));
switch(c){case 'u': _LL7: goto _LL8;case 'U': _LL8: return({struct _tuple5*_tmp5E=
_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=(void*)1;_tmp5E->f2=(int)n;_tmp5E;});case 'l':
_LL9: break;case 'L': _LLA: break;default: _LLB: n=n * base + (unsigned int)Cyc_Lex_int_of_char(
c);break;}}}return({struct _tuple5*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=(
void*)0;_tmp5F->f2=(int)n;_tmp5F;});}char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _tagged_arr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)
Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};int Cyc_Lex_string_pos=
0;void Cyc_Lex_store_string_char(char c){int sz=(int)(_get_arr_size(Cyc_Lex_string_buffer,
sizeof(char))- 1);if(Cyc_Lex_string_pos >= sz){int newsz=sz;while(Cyc_Lex_string_pos
>= newsz){newsz=newsz * 2;}{struct _tagged_arr str=({unsigned int _tmp60=(
unsigned int)newsz;char*_tmp61=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp60 + 1));struct _tagged_arr _tmp63=_tag_arr(_tmp61,sizeof(char),_tmp60 + 1);{
unsigned int _tmp62=_tmp60;unsigned int i;for(i=0;i < _tmp62;i ++){_tmp61[i]=i < sz?*((
char*)_check_unknown_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):'\000';}
_tmp61[_tmp62]=(char)0;}_tmp63;});Cyc_Lex_string_buffer=str;}}({struct
_tagged_arr _tmp64=_tagged_arr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);
char _tmp65=*((char*)_check_unknown_subscript(_tmp64,sizeof(char),0));char _tmp66=
c;if(_get_arr_size(_tmp64,sizeof(char))== 1?_tmp65 == '\000'?_tmp66 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp64.curr)=_tmp66;});++ Cyc_Lex_string_pos;}
struct _tagged_arr Cyc_Lex_get_stored_string(){struct _tagged_arr str=Cyc_substring((
struct _tagged_arr)Cyc_Lex_string_buffer,0,(unsigned int)Cyc_Lex_string_pos);Cyc_Lex_string_pos=
0;return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*
namespaces;};struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct Cyc_List_List*
imported_namespaces;};struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict*
decls;};static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp67=({struct Cyc_Lex_Lvis*_tmp6D=_cycalloc(sizeof(*_tmp6D));
_tmp6D->current_namespace=0;_tmp6D->imported_namespaces=0;_tmp6D;});struct Cyc_List_List*
_tmp68=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=_tmp67;
_tmp6C->tl=0;_tmp6C;});struct Cyc_Dict_Dict*init_decls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))
Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,({struct Cyc_Lex_Ldecls*_tmp6B=_cycalloc(
sizeof(*_tmp6B));_tmp6B->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp6B->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_zstrptrcmp);_tmp6B;}));Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmp69=_cycalloc(
sizeof(*_tmp69));_tmp69->v=({struct Cyc_Lex_Lstate*_tmp6A=_cycalloc(sizeof(*
_tmp6A));_tmp6A->lstack=_tmp68;_tmp6A->decls=init_decls;_tmp6A;});_tmp69;});}
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
struct _tagged_arr*n=(struct _tagged_arr*)ns->hd;{struct Cyc_List_List*ls=(struct
Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=
lv->current_namespace;_tmp6E->tl=lv->imported_namespaces;_tmp6E;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);if(((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(ld->namespaces,n))return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd,(struct Cyc_List_List*)ns);}}}Cyc_yyerror((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_str_sepstr((struct Cyc_List_List*)ns,({
const char*_tmp72="::";_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,3));})));{
void*_tmp6F[1]={& _tmp71};Cyc_aprintf(({const char*_tmp70="undeclared namespace %s";
_tag_arr(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,24));}),_tag_arr(_tmp6F,
sizeof(void*),1));}}));return 0;}static void Cyc_Lex_recompute_typedefs(){Cyc_Lex_typedefs_trie=({
struct Cyc_Lex_Trie*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->children=(void*)((
void*)0);_tmp73->shared_str=0;_tmp73;});{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->hd=
lv->current_namespace;_tmp74->tl=lv->imported_namespaces;_tmp74;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);((void(*)(void(*f)(struct _tagged_arr*),
struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}}}
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct
_tagged_arr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);struct
_handler_cons _tmp75;_push_handler(& _tmp75);{int _tmp77=0;if(setjmp(_tmp75.handler))
_tmp77=1;if(!_tmp77){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp78=((int(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp78;};_pop_handler();}else{void*_tmp76=(void*)
_exn_thrown;void*_tmp7A=_tmp76;_LLE: if(_tmp7A != Cyc_Dict_Absent)goto _LL10;_LLF:
return 0;_LL10:;_LL11:(void)_throw(_tmp7A);_LLD:;}}}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _tagged_arr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)_check_null(ns),v);{struct _tagged_arr _tmp7B=*v;int len=(int)(
_get_arr_size(_tmp7B,sizeof(char))- 1);struct Cyc_Lex_Trie*t=Cyc_Lex_typedefs_trie;{
int i=0;for(0;i < len;++ i){void*_tmp7C=(void*)((struct Cyc_Lex_Trie*)_check_null(t))->children;
int _tmp7D;struct Cyc_Lex_Trie*_tmp7E;struct Cyc_Lex_Trie*_tmp7F;struct Cyc_Lex_Trie**
_tmp80;_LL13: if((int)_tmp7C != 0)goto _LL15;_LL14: return 0;_LL15: if(_tmp7C <= (void*)
1?1:*((int*)_tmp7C)!= 0)goto _LL17;_tmp7D=((struct Cyc_Lex_One_struct*)_tmp7C)->f1;
_tmp7E=((struct Cyc_Lex_One_struct*)_tmp7C)->f2;if(!(_tmp7D != *((const char*)
_check_unknown_subscript(_tmp7B,sizeof(char),i))))goto _LL17;_LL16: return 0;_LL17:
if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)!= 0)goto _LL19;_tmp7F=((struct Cyc_Lex_One_struct*)
_tmp7C)->f2;_LL18: t=_tmp7F;goto _LL12;_LL19: if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)
!= 1)goto _LL12;_tmp80=((struct Cyc_Lex_Many_struct*)_tmp7C)->f1;_LL1A: {struct Cyc_Lex_Trie*
_tmp81=_tmp80[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const
char*)_check_unknown_subscript(_tmp7B,sizeof(char),i))))];if(_tmp81 == 0)return 0;
t=_tmp81;goto _LL12;}_LL12:;}}return((struct Cyc_Lex_Trie*)_check_null(t))->shared_str;}}
void Cyc_Lex_enter_namespace(struct _tagged_arr*s){struct Cyc_List_List*ns=((struct
Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(ns,({struct Cyc_List_List*_tmp86=
_cycalloc(sizeof(*_tmp86));_tmp86->hd=s;_tmp86->tl=0;_tmp86;}));((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=({struct Cyc_List_List*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=({struct Cyc_Lex_Lvis*_tmp83=
_cycalloc(sizeof(*_tmp83));_tmp83->current_namespace=new_ns;_tmp83->imported_namespaces=
0;_tmp83;});_tmp82->tl=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;_tmp82;});{struct Cyc_Lex_Ldecls*ld=((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
ns);if(!((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ld->namespaces,
s)){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,({struct Cyc_Lex_Ldecls*_tmp84=_cycalloc(sizeof(*
_tmp84));_tmp84->typedefs=ld->typedefs;_tmp84->namespaces=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(ld->namespaces,s);
_tmp84;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,new_ns,({struct Cyc_Lex_Ldecls*_tmp85=_cycalloc(
sizeof(*_tmp85));_tmp85->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp85->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_zstrptrcmp);_tmp85;}));}((void(*)(void(*f)(struct _tagged_arr*),struct Cyc_Set_Set*
s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);}}void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
struct Cyc_List_List*)_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*ns;{void*_tmp87=(*q).f1;
struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp89;_LL1C: if((int)_tmp87 != 0)
goto _LL1E;_LL1D: ns=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A->hd=(*q).f2;_tmp8A->tl=0;_tmp8A;});goto _LL1B;_LL1E: if(
_tmp87 <= (void*)1?1:*((int*)_tmp87)!= 0)goto _LL20;_tmp88=((struct Cyc_Absyn_Rel_n_struct*)
_tmp87)->f1;_LL1F: _tmp89=_tmp88;goto _LL21;_LL20: if(_tmp87 <= (void*)1?1:*((int*)
_tmp87)!= 1)goto _LL1B;_tmp89=((struct Cyc_Absyn_Abs_n_struct*)_tmp87)->f1;_LL21:
ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp89,({struct Cyc_List_List*_tmp8B=
_cycalloc(sizeof(*_tmp8B));_tmp8B->hd=(*q).f2;_tmp8B->tl=0;_tmp8B;})));goto _LL1B;
_LL1B:;}{struct Cyc_List_List*_tmp8C=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*
_tmp8D=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->hd=_tmp8C;_tmp8E->tl=
_tmp8D;_tmp8E;});((void(*)(void(*f)(struct _tagged_arr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(
Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls,_tmp8C))->typedefs);}}void Cyc_Lex_leave_using(){
struct Cyc_List_List*_tmp8F=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)
_check_null(_tmp8F))->tl;Cyc_Lex_recompute_typedefs();}void Cyc_Lex_register_typedef(
struct _tuple0*q){struct Cyc_List_List*_tmp90=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict*_tmp91=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmp92=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp91,_tmp90);
struct Cyc_Lex_Ldecls*_tmp93=({struct Cyc_Lex_Ldecls*_tmp94=_cycalloc(sizeof(*
_tmp94));_tmp94->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(_tmp92->typedefs,(*q).f2);_tmp94->namespaces=
_tmp92->namespaces;_tmp94;});((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmp91,_tmp90,
_tmp93);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws)
return(short)*((int*)_check_unknown_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));{struct _tagged_arr*_tmp95=Cyc_Lex_get_symbol(symbol_num);Cyc_Lex_token_string=*
_tmp95;if(Cyc_Lex_is_typedef(0,_tmp95))return 354;return 348;}}static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf*lbuf){int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;
struct _tagged_arr s=lbuf->lex_buffer;struct Cyc_List_List*rev_vs=0;while(i < end){
int start=i;for(0;i < end?*((char*)_check_unknown_subscript(s,sizeof(char),i))!= ':':
0;i ++){;}if(start == i)(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Core_Impossible_struct
_tmp97;_tmp97.tag=Cyc_Core_Impossible;_tmp97.f1=({const char*_tmp98="bad namespace";
_tag_arr(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,14));});_tmp97;});_tmp96;}));{
int vlen=i - start;struct _tagged_arr*v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((
struct _tagged_arr)s,start,vlen));rev_vs=({struct Cyc_List_List*_tmp99=_cycalloc(
sizeof(*_tmp99));_tmp99->hd=v;_tmp99->tl=rev_vs;_tmp99;});i +=2;}}if(rev_vs == 0)(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp9A=_cycalloc(sizeof(*
_tmp9A));_tmp9A[0]=({struct Cyc_Core_Impossible_struct _tmp9B;_tmp9B.tag=Cyc_Core_Impossible;
_tmp9B.f1=({const char*_tmp9C="bad namespace";_tag_arr(_tmp9C,sizeof(char),
_get_zero_arr_size(_tmp9C,14));});_tmp9B;});_tmp9A;}));{struct _tagged_arr*v=(
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(rev_vs))->hd;struct Cyc_List_List*
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs->tl);
Cyc_Lex_token_qvar=({struct _tuple0*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=(
void*)({struct Cyc_Absyn_Rel_n_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Absyn_Rel_n_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=vs;_tmp9F;});_tmp9E;});
_tmp9D->f2=v;_tmp9D;});if(Cyc_Lex_is_typedef(vs,v))return 356;return 355;}}int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf*);int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*);int Cyc_yylex(){int ans=((int(*)(struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);
Cyc_yylloc.first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)((
struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);
Cyc_yylloc.last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)((
struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);
switch(ans){case 348: _LL22: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_String_tok_struct _tmpA1;_tmpA1.tag=
Cyc_String_tok;_tmpA1.f1=Cyc_Lex_token_string;_tmpA1;});_tmpA0;});break;case 354:
_LL23: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpA2=_cycalloc(sizeof(*
_tmpA2));_tmpA2[0]=({struct Cyc_String_tok_struct _tmpA3;_tmpA3.tag=Cyc_String_tok;
_tmpA3.f1=Cyc_Lex_token_string;_tmpA3;});_tmpA2;});break;case 355: _LL24: Cyc_yylval=(
void*)({struct Cyc_QualId_tok_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_QualId_tok_struct _tmpA5;_tmpA5.tag=Cyc_QualId_tok;_tmpA5.f1=Cyc_Lex_token_qvar;
_tmpA5;});_tmpA4;});break;case 356: _LL25: Cyc_yylval=(void*)({struct Cyc_QualId_tok_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_QualId_tok_struct _tmpA7;
_tmpA7.tag=Cyc_QualId_tok;_tmpA7.f1=Cyc_Lex_token_qvar;_tmpA7;});_tmpA6;});
break;case 353: _LL26: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpA8=
_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_String_tok_struct _tmpA9;_tmpA9.tag=
Cyc_String_tok;_tmpA9.f1=Cyc_Lex_token_string;_tmpA9;});_tmpA8;});break;case 357:
_LL27: Cyc_yylval=(void*)({struct Cyc_Int_tok_struct*_tmpAA=_cycalloc(sizeof(*
_tmpAA));_tmpAA[0]=({struct Cyc_Int_tok_struct _tmpAB;_tmpAB.tag=Cyc_Int_tok;
_tmpAB.f1=Cyc_Lex_token_int;_tmpAB;});_tmpAA;});break;case 349: _LL28: Cyc_yylval=(
void*)({struct Cyc_Int_tok_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({
struct Cyc_Int_tok_struct _tmpAD;_tmpAD.tag=Cyc_Int_tok;_tmpAD.f1=Cyc_Lex_token_int;
_tmpAD;});_tmpAC;});break;case 351: _LL29: Cyc_yylval=(void*)({struct Cyc_Char_tok_struct*
_tmpAE=_cycalloc_atomic(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Char_tok_struct
_tmpAF;_tmpAF.tag=Cyc_Char_tok;_tmpAF.f1=Cyc_Lex_token_char;_tmpAF;});_tmpAE;});
break;case 352: _LL2A: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpB0=
_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_String_tok_struct _tmpB1;_tmpB1.tag=
Cyc_String_tok;_tmpB1.f1=Cyc_Lex_token_string;_tmpB1;});_tmpB0;});break;case 350:
_LL2B: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpB2=_cycalloc(sizeof(*
_tmpB2));_tmpB2[0]=({struct Cyc_String_tok_struct _tmpB3;_tmpB3.tag=Cyc_String_tok;
_tmpB3.f1=Cyc_Lex_token_string;_tmpB3;});_tmpB2;});break;default: _LL2C: break;}
return ans;}const int Cyc_Lex_lex_base[171]=(const int[171]){0,113,17,83,16,2,- 3,- 1,
- 2,- 17,- 18,4,118,119,- 19,5,- 13,- 12,85,- 14,- 11,- 4,- 5,- 6,- 7,- 8,- 9,- 10,192,215,111,
- 15,166,- 18,- 59,8,30,- 43,18,31,116,139,32,138,135,137,159,250,325,83,82,84,94,
395,85,470,545,90,- 58,- 26,- 32,620,630,700,110,126,775,785,119,142,855,893,134,
146,138,200,192,202,- 5,963,- 27,1038,90,1096,1171,1246,99,123,- 30,142,- 17,- 35,- 29,
- 38,825,1323,363,194,210,292,1333,1363,668,335,1396,1427,1465,203,219,1535,1573,
212,245,237,247,239,249,- 10,- 42,199,- 24,- 41,6,161,1505,- 37,- 20,- 22,- 36,- 21,- 23,2,
1613,169,171,311,174,176,185,186,187,189,191,193,199,1686,1770,- 56,- 50,- 49,- 48,-
47,- 46,- 45,- 44,- 51,- 54,- 55,386,213,- 52,- 53,- 57,- 31,- 28,- 25,383,- 39,12,- 16,680};
const int Cyc_Lex_lex_backtrk[171]=(const int[171]){- 1,- 1,- 1,5,3,4,- 1,- 1,- 1,- 1,- 1,
16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,14,- 1,- 1,- 1,- 1,39,58,- 1,
58,58,58,58,58,58,58,58,58,9,11,58,58,58,58,0,58,58,58,58,- 1,- 1,- 1,4,6,7,6,6,4,5,
4,4,- 1,3,3,3,5,5,4,4,- 1,1,- 1,0,- 1,- 1,2,2,- 1,33,- 1,32,- 1,- 1,- 1,- 1,13,11,- 1,11,11,
- 1,12,13,- 1,- 1,13,9,10,9,9,- 1,8,8,8,10,10,9,9,- 1,- 1,- 1,- 1,- 1,40,- 1,13,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,39};const int Cyc_Lex_lex_default[171]=(
const int[171]){34,9,3,3,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,0,0,0,0,0,-
1,- 1,- 1,0,- 1,0,0,- 1,- 1,0,166,- 1,- 1,131,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,-
1,- 1,- 1,- 1,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,0,119,0,0,- 1,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,166,0,- 1,0,- 1};const int Cyc_Lex_lex_trans[
2027]=(const int[2027]){0,0,0,0,0,0,0,0,0,35,35,35,35,35,33,6,121,170,170,170,170,
170,167,0,0,0,0,0,167,0,0,168,35,36,37,38,0,39,40,41,170,162,42,43,7,44,45,46,47,
48,48,48,48,48,48,48,48,48,49,4,50,51,52,8,5,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,169,165,130,54,55,56,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,10,57,- 1,11,32,32,6,
- 1,32,15,30,30,30,30,30,30,30,30,93,89,90,91,31,80,12,83,- 1,32,59,7,16,163,33,87,
83,17,31,31,31,31,31,31,31,31,18,18,18,18,18,18,18,18,32,32,164,- 1,32,126,14,19,
123,88,124,124,124,124,124,124,124,124,124,124,24,127,128,32,129,7,118,24,92,22,
13,119,125,161,121,160,20,122,156,60,155,21,22,22,21,120,23,21,23,154,153,152,24,
151,24,150,132,149,25,24,26,22,27,148,28,29,29,29,29,29,29,29,29,29,29,22,21,157,
0,21,23,58,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,6,- 1,0,23,78,78,17,29,
29,29,29,29,29,17,0,27,29,29,29,29,29,29,27,94,26,105,105,105,105,105,105,105,
105,106,106,23,78,78,17,29,29,29,29,29,29,17,96,27,26,20,20,117,117,107,27,0,26,
0,0,0,0,0,108,0,0,109,- 1,100,100,100,100,100,100,100,100,100,100,157,96,0,26,20,
20,117,117,107,158,158,158,158,158,158,158,158,108,0,14,109,94,0,95,95,95,95,95,
95,95,95,95,95,104,104,104,104,104,104,104,104,104,104,167,96,- 1,168,0,0,0,0,97,
0,0,0,0,99,0,99,0,98,100,100,100,100,100,100,100,100,100,100,0,0,0,0,157,96,0,0,
0,0,0,0,97,159,159,159,159,159,159,159,159,98,81,81,81,81,81,81,81,81,81,81,82,0,
- 1,0,0,0,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,0,0,0,0,81,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,79,79,79,79,79,79,79,79,79,79,0,0,0,0,0,0,0,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,0,0,0,0,79,0,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,61,62,
62,62,62,62,62,62,62,62,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,- 1,63,0,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,66,66,66,66,66,66,66,66,67,67,62,
62,62,62,62,62,62,62,62,62,0,170,170,170,170,170,0,0,68,0,0,0,0,0,0,0,0,69,64,0,
70,0,0,103,170,103,0,65,104,104,104,104,104,104,104,104,104,104,0,0,68,0,0,0,0,0,
0,0,0,69,64,0,70,0,0,0,0,0,0,65,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,
0,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,66,66,66,66,66,66,66,66,67,67,67,67,67,67,67,67,67,67,67,67,0,0,0,0,0,0,0,
0,76,0,0,0,0,0,0,0,0,77,74,0,0,0,0,0,0,0,0,75,0,0,101,101,101,101,101,101,101,
101,101,101,76,0,0,0,0,0,0,0,0,77,74,102,19,0,0,0,0,0,19,75,71,71,71,71,71,71,71,
71,71,71,0,0,0,0,0,0,0,71,71,71,71,71,71,102,19,0,0,0,0,0,19,0,0,0,0,0,0,0,71,71,
71,71,71,71,71,71,71,71,0,71,71,71,71,71,71,71,71,71,71,71,71,0,0,0,0,0,72,0,0,0,
0,0,0,0,0,73,0,0,0,0,0,0,0,0,0,0,0,71,71,71,71,71,71,0,0,0,0,0,72,0,0,0,0,0,0,0,
0,73,79,79,79,79,79,79,79,79,79,79,0,0,0,0,0,0,0,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,0,0,0,0,79,0,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,81,81,81,81,81,81,81,
81,81,81,82,0,0,0,0,0,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,0,0,0,0,81,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,81,81,81,81,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,0,0,0,0,85,0,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,86,0,0,0,
0,0,0,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,0,0,0,0,84,0,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,84,85,85,85,85,85,85,85,85,85,85,0,0,0,0,0,0,0,85,85,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,0,0,0,0,85,0,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,94,0,95,95,
95,95,95,95,95,95,95,95,100,100,100,100,100,100,100,100,100,100,0,96,0,0,0,0,0,0,
97,0,0,0,16,0,0,0,0,98,16,0,101,101,101,101,101,101,101,101,101,101,0,0,0,96,0,0,
0,0,0,0,97,102,19,0,16,0,0,0,19,98,16,0,0,104,104,104,104,104,104,104,104,104,
104,0,0,0,0,0,0,0,0,0,0,102,19,19,0,0,0,0,19,19,94,0,105,105,105,105,105,105,105,
105,106,106,0,0,0,0,0,0,0,0,0,0,0,96,0,19,0,0,0,0,115,19,0,0,0,0,0,0,94,116,106,
106,106,106,106,106,106,106,106,106,0,0,0,0,0,96,0,0,0,0,0,96,115,0,0,0,0,0,113,
0,0,116,0,0,0,0,0,114,0,0,124,124,124,124,124,124,124,124,124,124,0,0,0,96,0,0,0,
0,0,0,113,102,19,0,0,0,0,0,19,114,110,110,110,110,110,110,110,110,110,110,0,0,0,
0,0,0,0,110,110,110,110,110,110,102,19,0,0,0,0,0,19,0,0,0,0,0,0,0,110,110,110,
110,110,110,110,110,110,110,0,110,110,110,110,110,110,110,110,110,110,110,110,0,
0,0,133,0,111,0,0,134,0,0,0,0,0,112,0,0,135,135,135,135,135,135,135,135,0,110,
110,110,110,110,110,136,0,0,0,0,111,0,0,0,0,0,0,0,0,112,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,137,0,0,0,0,138,139,0,0,0,140,0,0,0,0,0,0,0,141,0,0,0,142,0,143,0,144,0,145,
146,146,146,146,146,146,146,146,146,146,0,0,0,0,0,0,0,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
0,0,0,0,0,0,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,147,0,0,0,0,0,0,0,0,146,146,146,146,146,146,
146,146,146,146,0,0,0,0,0,0,0,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,0,0,0,0,0,0,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[2027]=(const
int[2027]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,15,122,35,35,35,35,35,168,- 1,
- 1,- 1,- 1,- 1,38,- 1,- 1,38,0,0,0,0,- 1,0,0,0,35,131,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,2,0,0,0,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,39,42,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,1,12,12,13,3,12,13,18,
18,18,18,18,18,18,18,49,50,50,50,51,54,1,82,41,12,57,12,13,40,52,52,86,13,30,30,
30,30,30,30,30,30,13,13,13,13,13,13,13,13,32,32,40,41,32,44,43,13,45,87,45,45,45,
45,45,45,45,45,45,45,64,44,44,32,43,32,46,65,89,68,1,46,123,133,119,134,13,119,
136,57,137,13,13,69,72,46,13,73,74,138,139,140,64,141,13,142,41,143,13,65,13,68,
13,144,13,28,28,28,28,28,28,28,28,28,28,69,72,159,- 1,73,74,0,28,28,28,28,28,28,
29,29,29,29,29,29,29,29,29,29,2,38,- 1,75,76,77,97,29,29,29,29,29,29,98,- 1,107,28,
28,28,28,28,28,108,47,111,47,47,47,47,47,47,47,47,47,47,75,76,77,97,29,29,29,29,
29,29,98,47,107,112,113,114,115,116,47,108,- 1,111,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,47,3,
99,99,99,99,99,99,99,99,99,99,135,47,- 1,112,113,114,115,116,47,135,135,135,135,
135,135,135,135,47,- 1,1,47,48,- 1,48,48,48,48,48,48,48,48,48,48,103,103,103,103,
103,103,103,103,103,103,166,48,41,166,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,96,- 1,96,- 1,48,
96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,158,48,- 1,- 1,- 1,- 1,- 1,- 1,48,158,158,
158,158,158,158,158,158,48,53,53,53,53,53,53,53,53,53,53,53,- 1,119,- 1,- 1,- 1,- 1,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,- 1,
- 1,- 1,- 1,53,- 1,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,55,55,55,55,55,55,55,55,55,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,55,55,55,55,55,
55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,- 1,- 1,- 1,- 1,55,- 1,55,
55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,56,56,
56,56,56,56,56,56,56,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,56,56,56,- 1,- 1,- 1,166,56,- 1,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,61,61,61,61,61,61,61,
61,61,61,62,62,62,62,62,62,62,62,62,62,- 1,170,170,170,170,170,- 1,- 1,61,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,61,62,- 1,61,- 1,- 1,102,170,102,- 1,62,102,102,102,102,102,102,102,
102,102,102,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,62,63,
63,63,63,63,63,63,63,63,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,- 1,- 1,- 1,- 1,63,- 1,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,66,66,66,66,66,66,66,
66,66,66,67,67,67,67,67,67,67,67,67,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,94,94,94,94,94,94,94,94,94,94,66,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,94,94,- 1,- 1,- 1,- 1,- 1,94,67,70,70,70,70,70,70,70,70,
70,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,70,70,70,70,70,94,94,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,71,71,71,71,71,71,71,71,71,71,- 1,70,70,70,70,70,70,71,71,71,71,71,71,- 1,
- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,71,
71,71,71,71,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,79,79,79,79,79,79,79,79,
79,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,- 1,- 1,- 1,- 1,79,- 1,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,
- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,
83,83,83,83,83,83,- 1,- 1,- 1,- 1,83,- 1,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,
83,83,83,83,83,83,83,83,83,83,83,84,84,84,84,84,84,84,84,84,84,84,- 1,- 1,- 1,- 1,- 1,
- 1,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
- 1,- 1,- 1,- 1,84,- 1,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,84,85,85,85,85,85,85,85,85,85,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,- 1,- 1,- 1,- 1,85,- 1,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,95,
- 1,95,95,95,95,95,95,95,95,95,95,100,100,100,100,100,100,100,100,100,100,- 1,95,-
1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,95,100,- 1,101,101,101,101,101,101,
101,101,101,101,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,95,101,101,- 1,100,- 1,- 1,- 1,101,95,
100,- 1,- 1,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
101,101,104,- 1,- 1,- 1,- 1,101,104,105,- 1,105,105,105,105,105,105,105,105,105,105,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,104,- 1,- 1,- 1,- 1,105,104,- 1,- 1,- 1,- 1,- 1,- 1,
106,105,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,-
1,106,105,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,124,124,124,124,
124,124,124,124,124,124,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,106,124,124,- 1,- 1,- 1,- 1,-
1,124,106,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,109,
109,109,109,109,124,124,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,
110,110,110,110,110,110,- 1,109,109,109,109,109,109,110,110,110,110,110,110,- 1,- 1,
- 1,132,- 1,110,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,132,132,132,132,132,132,132,132,
- 1,110,110,110,110,110,110,132,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,132,132,- 1,- 1,- 1,132,- 1,- 1,-
1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,132,- 1,132,- 1,132,- 1,132,145,145,145,145,145,145,145,
145,145,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,145,145,145,145,145,145,145,145,145,145,145,145,
145,145,145,145,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,- 1,- 1,145,
145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
145,145,145,145,145,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,
146,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf){int state;int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(
lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(-
state)- 1;}while(1){base=Cyc_Lex_lex_base[_check_known_subscript_notnull(171,
state)];if(base < 0)return(- base)- 1;backtrk=Cyc_Lex_lex_backtrk[
_check_known_subscript_notnull(171,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_Lex_lex_check[_check_known_subscript_notnull(2027,base + 
c)]== state)state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(2027,base + c)];
else{state=Cyc_Lex_lex_default[_check_known_subscript_notnull(171,state)];}if(
state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB4=_cycalloc(sizeof(*
_tmpB4));_tmpB4[0]=({struct Cyc_Lexing_Error_struct _tmpB5;_tmpB5.tag=Cyc_Lexing_Error;
_tmpB5.f1=({const char*_tmpB6="empty token";_tag_arr(_tmpB6,sizeof(char),
_get_zero_arr_size(_tmpB6,12));});_tmpB5;});_tmpB4;}));else{return lbuf->lex_last_action;}}
else{if(c == 256)lbuf->lex_eof_reached=0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL2E: return(int)Cyc_Lex_process_id(lexbuf);case 1: _LL2F: return(int)Cyc_Lex_process_id(
lexbuf);case 2: _LL30: return(int)Cyc_Lex_process_qual_id(lexbuf);case 3: _LL31: Cyc_Lex_token_int=
Cyc_Lex_intconst(lexbuf,3,0,16);return 357;case 4: _LL32: Cyc_Lex_token_int=Cyc_Lex_intconst(
lexbuf,1,0,8);return 357;case 5: _LL33: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,1,
0,10);return 357;case 6: _LL34: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,1,0,10);
return 357;case 7: _LL35: Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(
lexbuf));return 353;case 8: _LL36: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 349;case 9: _LL37: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);return 349;
case 10: _LL38: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 349;case 11:
_LL39: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 349;case 12: _LL3A:
Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);return 352;case
13: _LL3B: Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);
return 352;case 14: _LL3C: return 331;case 15: _LL3D: return 332;case 16: _LL3E: return 329;
case 17: _LL3F: return 330;case 18: _LL40: return 325;case 19: _LL41: return 326;case 20:
_LL42: return 338;case 21: _LL43: return 339;case 22: _LL44: return 335;case 23: _LL45:
return 336;case 24: _LL46: return 337;case 25: _LL47: return 344;case 26: _LL48: return 343;
case 27: _LL49: return 342;case 28: _LL4A: return 340;case 29: _LL4B: return 341;case 30:
_LL4C: return 333;case 31: _LL4D: return 334;case 32: _LL4E: return 327;case 33: _LL4F:
return 328;case 34: _LL50: return 346;case 35: _LL51: return 324;case 36: _LL52: return 345;
case 37: _LL53: return 347;case 38: _LL54: return Cyc_Lex_token(lexbuf);case 39: _LL55:
return Cyc_Lex_token(lexbuf);case 40: _LL56: return Cyc_Lex_token(lexbuf);case 41:
_LL57: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 42: _LL58: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lex_get_stored_string();
return 350;case 43: _LL59: Cyc_Lex_token_char='\a';return 351;case 44: _LL5A: Cyc_Lex_token_char='\b';
return 351;case 45: _LL5B: Cyc_Lex_token_char='\f';return 351;case 46: _LL5C: Cyc_Lex_token_char='\n';
return 351;case 47: _LL5D: Cyc_Lex_token_char='\r';return 351;case 48: _LL5E: Cyc_Lex_token_char='\t';
return 351;case 49: _LL5F: Cyc_Lex_token_char='\v';return 351;case 50: _LL60: Cyc_Lex_token_char='\\';
return 351;case 51: _LL61: Cyc_Lex_token_char='\'';return 351;case 52: _LL62: Cyc_Lex_token_char='"';
return 351;case 53: _LL63: Cyc_Lex_token_char='?';return 351;case 54: _LL64: Cyc_Lex_token_char=(
char)(*Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 351;case 55: _LL65: Cyc_Lex_token_char=(
char)(*Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 351;case 56: _LL66: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 351;case 57: _LL67: return - 1;case 58: _LL68:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL69:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Lexing_Error_struct
_tmpB8;_tmpB8.tag=Cyc_Lexing_Error;_tmpB8.f1=({const char*_tmpB9="some action didn't return!";
_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,27));});_tmpB8;});_tmpB7;}));}
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,
0);}int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6B: return 1;case 1:
_LL6C: return 0;case 2: _LL6D: return 1;case 3: _LL6E: Cyc_Lex_store_string_char('\a');
return 1;case 4: _LL6F: Cyc_Lex_store_string_char('\b');return 1;case 5: _LL70: Cyc_Lex_store_string_char('\f');
return 1;case 6: _LL71: Cyc_Lex_store_string_char('\n');return 1;case 7: _LL72: Cyc_Lex_store_string_char('\r');
return 1;case 8: _LL73: Cyc_Lex_store_string_char('\t');return 1;case 9: _LL74: Cyc_Lex_store_string_char('\v');
return 1;case 10: _LL75: Cyc_Lex_store_string_char('\\');return 1;case 11: _LL76: Cyc_Lex_store_string_char('\'');
return 1;case 12: _LL77: Cyc_Lex_store_string_char('"');return 1;case 13: _LL78: Cyc_Lex_store_string_char('?');
return 1;case 14: _LL79: Cyc_Lex_store_string_char((char)(*Cyc_Lex_intconst(lexbuf,1,
0,8)).f2);return 1;case 15: _LL7A: Cyc_Lex_store_string_char((char)(*Cyc_Lex_intconst(
lexbuf,2,0,16)).f2);return 1;case 16: _LL7B: Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(
lexbuf,0));return 1;case 17: _LL7C: Cyc_Lex_runaway_err(({const char*_tmpBA="string ends in newline";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,23));}),lexbuf);return 0;
case 18: _LL7D: Cyc_Lex_runaway_err(({const char*_tmpBB="unterminated string";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,20));}),lexbuf);return 0;
case 19: _LL7E: Cyc_Lex_err(({const char*_tmpBC="bad character following backslash in string";
_tag_arr(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,44));}),lexbuf);return 1;
default: _LL7F:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_strng_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Lexing_Error_struct _tmpBE;_tmpBE.tag=Cyc_Lexing_Error;
_tmpBE.f1=({const char*_tmpBF="some action didn't return!";_tag_arr(_tmpBF,
sizeof(char),_get_zero_arr_size(_tmpBF,27));});_tmpBE;});_tmpBD;}));}int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,1);}int Cyc_Lex_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL81: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(
lexbuf);case 1: _LL82: -- Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0)return
Cyc_Lex_comment(lexbuf);return 0;case 2: _LL83: Cyc_Lex_runaway_err(({const char*
_tmpC0="unterminated comment";_tag_arr(_tmpC0,sizeof(char),_get_zero_arr_size(
_tmpC0,21));}),lexbuf);return 0;case 3: _LL84: return Cyc_Lex_comment(lexbuf);case 4:
_LL85: return Cyc_Lex_comment(lexbuf);case 5: _LL86: return Cyc_Lex_comment(lexbuf);
default: _LL87:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_comment_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpC1=_cycalloc(
sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Lexing_Error_struct _tmpC2;_tmpC2.tag=Cyc_Lexing_Error;
_tmpC2.f1=({const char*_tmpC3="some action didn't return!";_tag_arr(_tmpC3,
sizeof(char),_get_zero_arr_size(_tmpC3,27));});_tmpC2;});_tmpC1;}));}int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,2);}void Cyc_Lex_lex_init(){
Cyc_Lex_ids_trie=({struct Cyc_Lex_Trie*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->children=(
void*)((void*)0);_tmpC4->shared_str=0;_tmpC4;});Cyc_Lex_typedefs_trie=({struct
Cyc_Lex_Trie*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->children=(void*)((void*)0);
_tmpC5->shared_str=0;_tmpC5;});Cyc_Lex_symbols=(struct Cyc_Xarray_Xarray*)((
struct Cyc_Xarray_Xarray*(*)(int,struct _tagged_arr*))Cyc_Xarray_create)(101,
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1));((void(*)(
struct Cyc_Xarray_Xarray*,struct _tagged_arr*))Cyc_Xarray_add)((struct Cyc_Xarray_Xarray*)
_check_null(Cyc_Lex_symbols),& Cyc_Lex_bogus_string);Cyc_Lex_num_kws=(int)67;Cyc_Lex_kw_nums=({
unsigned int _tmpC7=(unsigned int)Cyc_Lex_num_kws;int*_tmpC8=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmpC7));struct _tagged_arr _tmpCA=
_tag_arr(_tmpC8,sizeof(int),_tmpC7);{unsigned int _tmpC9=_tmpC7;unsigned int i;
for(i=0;i < _tmpC9;i ++){_tmpC8[i]=0;}}_tmpCA;});{int i=0;for(0;i < Cyc_Lex_num_kws;
++ i){struct _tagged_arr _tmpCB=(Cyc_Lex_rw_array[_check_known_subscript_notnull(67,
i)]).f1;Cyc_Lex_str_index(_tmpCB,0,(int)Cyc_strlen((struct _tagged_arr)_tmpCB));*((
int*)_check_unknown_subscript(Cyc_Lex_kw_nums,sizeof(int),i))=(int)(Cyc_Lex_rw_array[
_check_known_subscript_notnull(67,i)]).f2;}}Cyc_Lex_typedef_init();Cyc_Lex_comment_depth=
0;}
