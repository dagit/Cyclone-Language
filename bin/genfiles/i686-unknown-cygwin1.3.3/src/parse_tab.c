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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _tagged_arr s);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int
Cyc_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_strcat(struct _tagged_arr
dest,struct _tagged_arr src);struct _tagged_arr Cyc_strcpy(struct _tagged_arr dest,
struct _tagged_arr src);struct _tagged_arr Cyc_substring(struct _tagged_arr,int ofs,
unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern void*Cyc_Absyn_rel_ns_null;
int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;void*Cyc_Absyn_starb_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
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
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct
Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts);struct _tagged_arr Cyc_Absyn_attribute2string(void*);int
Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);void Cyc_Lex_register_typedef(struct _tuple1*s);void
Cyc_Lex_enter_namespace(struct _tagged_arr*);void Cyc_Lex_leave_namespace();void
Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Core_Opt*Cyc_Parse_lbuf=0;static struct Cyc_List_List*Cyc_Parse_parse_result=
0;static void Cyc_Parse_err(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((
struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((
void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _tagged_arr msg,struct
Cyc_Position_Segment*sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(
struct _tagged_arr)msg;{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(sg);{void*_tmp0[2]={& _tmp2,&
_tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,46));}),_tag_arr(_tmp0,
sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=0;struct _tuple1*
Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9[0]=({struct Cyc_Absyn_Rel_n_struct _tmpA;_tmpA.tag=0;_tmpA.f1=0;_tmpA;});
_tmp9;});_tmp4->f2=({struct _tagged_arr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(int)((
unsigned int)Cyc_Parse_enum_counter ++);{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({
const char*_tmp7="__anonymous_enum_%d__";_tag_arr(_tmp7,sizeof(char),
_get_zero_arr_size(_tmp7,22));}),_tag_arr(_tmp6,sizeof(void*),1));}});_tmp5;});
_tmp4;});}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct
Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info){struct _tuple8 _tmpC;
struct _tuple7*_tmpD;struct _tuple7 _tmpE;struct _tuple1*_tmpF;struct Cyc_Absyn_Tqual
_tmp10;void*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_Absyn_Exp*_tmp14;struct _tuple8*_tmpB=field_info;_tmpC=*_tmpB;_tmpD=_tmpC.f1;
_tmpE=*_tmpD;_tmpF=_tmpE.f1;_tmp10=_tmpE.f2;_tmp11=_tmpE.f3;_tmp12=_tmpE.f4;
_tmp13=_tmpE.f5;_tmp14=_tmpC.f2;if(_tmp12 != 0)Cyc_Parse_err(({const char*_tmp15="bad type params in struct field";
_tag_arr(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,32));}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmpF))Cyc_Parse_err(({const char*_tmp16="struct or union field cannot be qualified with a namespace";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,59));}),loc);return({
struct Cyc_Absyn_Aggrfield*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->name=(*((
struct _tuple1*)_tmpF)).f2;_tmp17->tq=_tmp10;_tmp17->type=(void*)_tmp11;_tmp17->width=
_tmp14;_tmp17->attributes=_tmp13;_tmp17;});}static void*Cyc_Parse_type_spec(void*
t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Parse_Type_spec_struct
_tmp19;_tmp19.tag=4;_tmp19.f1=(void*)t;_tmp19.f2=loc;_tmp19;});_tmp18;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp1A=t;struct Cyc_Absyn_ArrayInfo
_tmp1B;void*_tmp1C;struct Cyc_Absyn_Tqual _tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct
Cyc_Absyn_Conref*_tmp1F;_LL1: if(_tmp1A <= (void*)3?1:*((int*)_tmp1A)!= 7)goto _LL3;
_tmp1B=((struct Cyc_Absyn_ArrayType_struct*)_tmp1A)->f1;_tmp1C=(void*)_tmp1B.elt_type;
_tmp1D=_tmp1B.tq;_tmp1E=_tmp1B.num_elts;_tmp1F=_tmp1B.zero_term;_LL2: return Cyc_Absyn_starb_typ(
_tmp1C,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*
_tmp20));_tmp20->v=(void*)((void*)3);_tmp20;}),0):(void*)2,_tmp1D,_tmp1E == 0?(
void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp21=_cycalloc(sizeof(*_tmp21));
_tmp21[0]=({struct Cyc_Absyn_Upper_b_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
Cyc_Absyn_Exp*)_check_null(_tmp1E);_tmp22;});_tmp21;}),_tmp1F);_LL3:;_LL4: return
t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*((struct _tuple2*)x)).f3=
Cyc_Parse_array2ptr((*((struct _tuple2*)x)).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp23=_cycalloc(sizeof(*_tmp23));
_tmp23->f1=(*((struct _tuple2*)t)).f2;_tmp23->f2=(*((struct _tuple2*)t)).f3;_tmp23;});}
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp24=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp25;_LL6: if(*((int*)_tmp24)!= 
0)goto _LL8;_tmp25=((struct Cyc_Absyn_Var_d_struct*)_tmp24)->f1;_LL7: return _tmp25;
_LL8:;_LL9:({void*_tmp26[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,({const char*_tmp27="bad declaration in `forarray' statement";
_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,40));}),_tag_arr(_tmp26,
sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp28=
tm;_LLB: if(*((int*)_tmp28)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}
static void*Cyc_Parse_id2type(struct _tagged_arr s,void*k){if(Cyc_zstrcmp((struct
_tagged_arr)s,({const char*_tmp29="`H";_tag_arr(_tmp29,sizeof(char),
_get_zero_arr_size(_tmp29,3));}))== 0)return(void*)2;else{return(void*)({struct
Cyc_Absyn_VarType_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_VarType_struct
_tmp2B;_tmp2B.tag=1;_tmp2B.f1=({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*
_tmp2C));_tmp2C->name=({struct _tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=s;_tmp2D;});_tmp2C->identity=0;_tmp2C->kind=(void*)k;_tmp2C;});_tmp2B;});
_tmp2A;});}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp2F;void*
_tmp30;_LL10: if(*((int*)_tmp2E)!= 0)goto _LL12;_tmp2F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp2E)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp31=_cycalloc(
sizeof(*_tmp31));_tmp31[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp32;_tmp32.tag=0;
_tmp32.f1=(void*)_tmp2F;_tmp32;});_tmp31;});goto _LLF;_LL12: if(*((int*)_tmp2E)!= 
1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp33=_cycalloc(
sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp34;_tmp34.tag=
1;_tmp34.f1=0;_tmp34;});_tmp33;});goto _LLF;_LL14: if(*((int*)_tmp2E)!= 2)goto _LLF;
_tmp30=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL15: k=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp36;_tmp36.tag=2;_tmp36.f1=0;_tmp36.f2=(void*)
_tmp30;_tmp36;});_tmp35;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp37=
_cycalloc(sizeof(*_tmp37));_tmp37->name=t->name;_tmp37->identity=0;_tmp37->kind=(
void*)k;_tmp37;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp38=t;struct Cyc_Absyn_Tvar*_tmp39;_LL17: if(_tmp38 <= (void*)3?
1:*((int*)_tmp38)!= 1)goto _LL19;_tmp39=((struct Cyc_Absyn_VarType_struct*)_tmp38)->f1;
_LL18: return _tmp39;_LL19:;_LL1A:({void*_tmp3A[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp3B="expecting a list of type variables, not types";_tag_arr(_tmp3B,sizeof(
char),_get_zero_arr_size(_tmp3B,46));}),_tag_arr(_tmp3A,sizeof(void*),0));});
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({
struct Cyc_Absyn_VarType_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=pr;_tmp3D;});_tmp3C;});}
static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*_tmp3E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp3F;struct Cyc_Absyn_Tvar _tmp40;void*_tmp41;void**
_tmp42;_LL1C: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 1)goto _LL1E;_tmp3F=((
struct Cyc_Absyn_VarType_struct*)_tmp3E)->f1;_tmp40=*_tmp3F;_tmp41=(void*)_tmp40.kind;
_tmp42=(void**)&(*((struct Cyc_Absyn_VarType_struct*)_tmp3E)->f1).kind;_LL1D: {
void*_tmp43=Cyc_Absyn_compress_kb(*((void**)_tmp42));_LL21: if(*((int*)_tmp43)!= 
1)goto _LL23;_LL22:*((void**)_tmp42)=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp44=
_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp45;_tmp45.tag=
0;_tmp45.f1=(void*)k;_tmp45;});_tmp44;});return;_LL23:;_LL24: return;_LL20:;}
_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp46=(void*)tms->hd;void*_tmp47;
_LL26: if(*((int*)_tmp46)!= 3)goto _LL28;_tmp47=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp46)->f1;_LL27: if(tms->tl == 0?1:(Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd)?((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0:
0)){void*_tmp48=_tmp47;struct Cyc_List_List*_tmp49;_LL2B: if(*((int*)_tmp48)!= 1)
goto _LL2D;_LL2C:({void*_tmp4A[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4B="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,93));}),_tag_arr(_tmp4A,
sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp48)!= 0)goto _LL2A;_tmp49=((
struct Cyc_Absyn_NoTypes_struct*)_tmp48)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp49)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tds))({void*_tmp4C[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp4D="wrong number of parameter declarations in old-style function declaration";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,73));}),_tag_arr(_tmp4C,
sizeof(void*),0));});{struct Cyc_List_List*_tmp4E=0;for(0;_tmp49 != 0;_tmp49=
_tmp49->tl){struct Cyc_List_List*_tmp4F=tds;for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){
struct Cyc_Absyn_Decl*_tmp50=(struct Cyc_Absyn_Decl*)_tmp4F->hd;void*_tmp51=(void*)
_tmp50->r;struct Cyc_Absyn_Vardecl*_tmp52;_LL30: if(*((int*)_tmp51)!= 0)goto _LL32;
_tmp52=((struct Cyc_Absyn_Var_d_struct*)_tmp51)->f1;_LL31: if(Cyc_zstrptrcmp((*((
struct _tuple1*)_tmp52->name)).f2,(struct _tagged_arr*)_tmp49->hd)!= 0)continue;
if(_tmp52->initializer != 0)({void*_tmp53[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp50->loc,({
const char*_tmp54="initializer found in parameter declaration";_tag_arr(_tmp54,
sizeof(char),_get_zero_arr_size(_tmp54,43));}),_tag_arr(_tmp53,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp52->name))({void*_tmp55[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmp50->loc,({const char*_tmp56="namespaces forbidden in parameter declarations";
_tag_arr(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,47));}),_tag_arr(_tmp55,
sizeof(void*),0));});_tmp4E=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*
_tmp57));_tmp57->hd=({struct _tuple2*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=({
struct Cyc_Core_Opt*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->v=(*((struct _tuple1*)
_tmp52->name)).f2;_tmp59;});_tmp58->f2=_tmp52->tq;_tmp58->f3=(void*)_tmp52->type;
_tmp58;});_tmp57->tl=_tmp4E;_tmp57;});goto L;_LL32:;_LL33:({void*_tmp5A[0]={};((
int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Parse_abort)(_tmp50->loc,({const char*_tmp5B="nonvariable declaration in parameter type";
_tag_arr(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,42));}),_tag_arr(_tmp5A,
sizeof(void*),0));});_LL2F:;}L: if(_tmp4F == 0)({struct Cyc_String_pa_struct _tmp5E;
_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
_tagged_arr*)_tmp49->hd));{void*_tmp5C[1]={& _tmp5E};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp5D="%s is not given a type";_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(
_tmp5D,23));}),_tag_arr(_tmp5C,sizeof(void*),1));}});}return({struct Cyc_List_List*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp61;_tmp61.tag=3;_tmp61.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp63;_tmp63.tag=1;_tmp63.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp4E);_tmp63.f2=0;_tmp63.f3=0;_tmp63.f4=0;_tmp63.f5=0;_tmp63;});
_tmp62;}));_tmp61;});_tmp60;}));_tmp5F->tl=0;_tmp5F;});}_LL2A:;}goto _LL29;_LL28:;
_LL29: return({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=(
void*)((void*)tms->hd);_tmp64->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp64;});_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->id=d->id;_tmp65->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp65;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp66=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp66 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp66 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp67="bad storage class on function";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,30));}),loc);goto _LL34;
_LL34:;}}{void*_tmp69;struct Cyc_Core_Opt*_tmp6A;struct _tuple5 _tmp68=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;{struct Cyc_Absyn_Tqual _tmp6C;void*
_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6F;struct _tuple6 _tmp6B=
Cyc_Parse_apply_tms(tq,_tmp69,atts,d->tms);_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;
_tmp6E=_tmp6B.f3;_tmp6F=_tmp6B.f4;if(_tmp6A != 0)({void*_tmp70[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp71="nested type declaration within function prototype";
_tag_arr(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,50));}),_tag_arr(_tmp70,
sizeof(void*),0));});if(_tmp6E != 0)({void*_tmp72[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp73="bad type params, ignoring";_tag_arr(_tmp73,sizeof(char),
_get_zero_arr_size(_tmp73,26));}),_tag_arr(_tmp72,sizeof(void*),0));});{void*
_tmp74=_tmp6D;struct Cyc_Absyn_FnInfo _tmp75;struct Cyc_List_List*_tmp76;struct Cyc_Core_Opt*
_tmp77;void*_tmp78;struct Cyc_List_List*_tmp79;int _tmp7A;struct Cyc_Absyn_VarargInfo*
_tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7D;_LL3C: if(_tmp74 <= (
void*)3?1:*((int*)_tmp74)!= 8)goto _LL3E;_tmp75=((struct Cyc_Absyn_FnType_struct*)
_tmp74)->f1;_tmp76=_tmp75.tvars;_tmp77=_tmp75.effect;_tmp78=(void*)_tmp75.ret_typ;
_tmp79=_tmp75.args;_tmp7A=_tmp75.c_varargs;_tmp7B=_tmp75.cyc_varargs;_tmp7C=
_tmp75.rgn_po;_tmp7D=_tmp75.attributes;_LL3D: {struct Cyc_List_List*_tmp7E=0;{
struct Cyc_List_List*_tmp7F=_tmp79;for(0;_tmp7F != 0;_tmp7F=_tmp7F->tl){struct
_tuple2 _tmp81;struct Cyc_Core_Opt*_tmp82;struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;
struct _tuple2*_tmp80=(struct _tuple2*)_tmp7F->hd;_tmp81=*_tmp80;_tmp82=_tmp81.f1;
_tmp83=_tmp81.f2;_tmp84=_tmp81.f3;if(_tmp82 == 0){Cyc_Parse_err(({const char*
_tmp85="missing argument variable in function prototype";_tag_arr(_tmp85,sizeof(
char),_get_zero_arr_size(_tmp85,48));}),loc);_tmp7E=({struct Cyc_List_List*_tmp86=
_cycalloc(sizeof(*_tmp86));_tmp86->hd=({struct _tuple10*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",
sizeof(char),2);_tmp87->f2=_tmp83;_tmp87->f3=_tmp84;_tmp87;});_tmp86->tl=_tmp7E;
_tmp86;});}else{_tmp7E=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));
_tmp89->hd=({struct _tuple10*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=(struct
_tagged_arr*)_tmp82->v;_tmp8A->f2=_tmp83;_tmp8A->f3=_tmp84;_tmp8A;});_tmp89->tl=
_tmp7E;_tmp89;});}}}return({struct Cyc_Absyn_Fndecl*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->sc=(void*)sc;_tmp8B->name=d->id;_tmp8B->tvs=_tmp76;_tmp8B->is_inline=
is_inline;_tmp8B->effect=_tmp77;_tmp8B->ret_type=(void*)_tmp78;_tmp8B->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7E);_tmp8B->c_varargs=
_tmp7A;_tmp8B->cyc_varargs=_tmp7B;_tmp8B->rgn_po=_tmp7C;_tmp8B->body=body;_tmp8B->cached_typ=
0;_tmp8B->param_vardecls=0;_tmp8B->attributes=Cyc_List_append(_tmp7D,_tmp6F);
_tmp8B;});}_LL3E:;_LL3F:({void*_tmp8C[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp8D="declarator is not a function prototype";_tag_arr(_tmp8D,sizeof(char),
_get_zero_arr_size(_tmp8D,39));}),_tag_arr(_tmp8C,sizeof(void*),0));});_LL3B:;}}}}}
static char _tmp8E[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp8E,_tmp8E,_tmp8E + 52};static char
_tmp8F[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp8F,_tmp8F,_tmp8F + 63};static char
_tmp90[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp90,_tmp90,_tmp90 + 50};static char _tmp91[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp91,_tmp91,_tmp91 + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp92=(void*)ts->hd;void*_tmp93;
struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;struct Cyc_Position_Segment*
_tmp96;struct Cyc_Position_Segment*_tmp97;struct Cyc_Position_Segment*_tmp98;
struct Cyc_Absyn_Decl*_tmp99;_LL41: if(*((int*)_tmp92)!= 4)goto _LL43;_tmp93=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp92)->f1;_tmp94=((struct Cyc_Parse_Type_spec_struct*)
_tmp92)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp94);last_loc=
_tmp94;seen_type=1;t=_tmp93;goto _LL40;_LL43: if(*((int*)_tmp92)!= 0)goto _LL45;
_tmp95=((struct Cyc_Parse_Signed_spec_struct*)_tmp92)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp95);if(seen_type)Cyc_Parse_err(({const char*_tmp9A="signed qualifier must come before type";
_tag_arr(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,39));}),_tmp95);last_loc=
_tmp95;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp92)!= 1)goto _LL47;
_tmp96=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp92)->f1;_LL46: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmp96);if(seen_type)Cyc_Parse_err(({const char*
_tmp9B="signed qualifier must come before type";_tag_arr(_tmp9B,sizeof(char),
_get_zero_arr_size(_tmp9B,39));}),_tmp96);last_loc=_tmp96;seen_sign=1;sgn=(void*)
1;goto _LL40;_LL47: if(*((int*)_tmp92)!= 2)goto _LL49;_tmp97=((struct Cyc_Parse_Short_spec_struct*)
_tmp92)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp9C="integral size may appear only once within a type specifier";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,59));}),_tmp97);if(
seen_type)Cyc_Parse_err(({const char*_tmp9D="short modifier must come before base type";
_tag_arr(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,42));}),_tmp97);last_loc=
_tmp97;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp92)!= 3)goto _LL4B;
_tmp98=((struct Cyc_Parse_Long_spec_struct*)_tmp92)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({
const char*_tmp9E="long modifier must come before base type";_tag_arr(_tmp9E,
sizeof(char),_get_zero_arr_size(_tmp9E,41));}),_tmp98);if(seen_size){void*_tmp9F=
sz;_LL4E: if((int)_tmp9F != 2)goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51:
Cyc_Parse_err(({const char*_tmpA0="extra long in type specifier";_tag_arr(_tmpA0,
sizeof(char),_get_zero_arr_size(_tmpA0,29));}),_tmp98);goto _LL4D;_LL4D:;}
last_loc=_tmp98;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp92)!= 5)goto _LL40;
_tmp99=((struct Cyc_Parse_Decl_spec_struct*)_tmp92)->f1;_LL4C: last_loc=_tmp99->loc;
if(declopt == 0?!seen_type: 0){seen_type=1;{void*_tmpA1=(void*)_tmp99->r;struct Cyc_Absyn_Aggrdecl*
_tmpA2;struct Cyc_Absyn_Tuniondecl*_tmpA3;struct Cyc_Absyn_Enumdecl*_tmpA4;_LL53:
if(*((int*)_tmpA1)!= 4)goto _LL55;_tmpA2=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA1)->f1;
_LL54: {struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA2->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA7;_tmpA7.tag=10;_tmpA7.f1=({struct Cyc_Absyn_AggrInfo _tmpA8;_tmpA8.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpA9=_cycalloc(sizeof(*
_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpAA;_tmpAA.tag=0;
_tmpAA.f1=(void*)((void*)_tmpA2->kind);_tmpAA.f2=_tmpA2->name;_tmpAA;});_tmpA9;}));
_tmpA8.targs=_tmpA5;_tmpA8;});_tmpA7;});_tmpA6;});if(_tmpA2->impl != 0)declopt=({
struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->v=_tmp99;_tmpAB;});
goto _LL52;}_LL55: if(*((int*)_tmpA1)!= 5)goto _LL57;_tmpA3=((struct Cyc_Absyn_Tunion_d_struct*)
_tmpA1)->f1;_LL56: {struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA3->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=({struct Cyc_Absyn_TunionInfo
_tmpAF;_tmpAF.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB1;_tmpB1.tag=1;_tmpB1.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB2=_cycalloc(
sizeof(*_tmpB2));_tmpB2[0]=_tmpA3;_tmpB2;});_tmpB1;});_tmpB0;}));_tmpAF.targs=
_tmpAC;_tmpAF.rgn=(void*)((void*)2);_tmpAF;});_tmpAE;});_tmpAD;});if(_tmpA3->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->v=
_tmp99;_tmpB3;});goto _LL52;}_LL57: if(*((int*)_tmpA1)!= 6)goto _LL59;_tmpA4=((
struct Cyc_Absyn_Enum_d_struct*)_tmpA1)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_EnumType_struct
_tmpB5;_tmpB5.tag=12;_tmpB5.f1=_tmpA4->name;_tmpB5.f2=0;_tmpB5;});_tmpB4;});
declopt=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=_tmp99;
_tmpB6;});goto _LL52;_LL59:;_LL5A:({void*_tmpB7[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp99->loc,({
const char*_tmpB8="bad declaration within type specifier";_tag_arr(_tmpB8,sizeof(
char),_get_zero_arr_size(_tmpB8,38));}),_tag_arr(_tmpB7,sizeof(void*),0));});
_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp99->loc);}goto _LL40;_LL40:;}if(!
seen_type){if(!seen_sign?!seen_size: 0)({void*_tmpB9[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBA="missing type within specifier";_tag_arr(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,30));}),_tag_arr(_tmpB9,sizeof(void*),0));});
t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));
_tmpBB[0]=({struct Cyc_Absyn_IntType_struct _tmpBC;_tmpBC.tag=5;_tmpBC.f1=(void*)
sgn;_tmpBC.f2=(void*)sz;_tmpBC;});_tmpBB;});}else{if(seen_sign){void*_tmpBD=t;
void*_tmpBE;_LL5C: if(_tmpBD <= (void*)3?1:*((int*)_tmpBD)!= 5)goto _LL5E;_tmpBE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f2;_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_IntType_struct
_tmpC0;_tmpC0.tag=5;_tmpC0.f1=(void*)sgn;_tmpC0.f2=(void*)_tmpBE;_tmpC0;});
_tmpBF;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const char*_tmpC1="sign specification on non-integral type";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,40));}),last_loc);goto
_LL5B;_LL5B:;}if(seen_size){void*_tmpC2=t;void*_tmpC3;_LL61: if(_tmpC2 <= (void*)3?
1:*((int*)_tmpC2)!= 5)goto _LL63;_tmpC3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC2)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC4=_cycalloc(
sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_IntType_struct _tmpC5;_tmpC5.tag=5;
_tmpC5.f1=(void*)_tmpC3;_tmpC5.f2=(void*)sz;_tmpC5;});_tmpC4;});goto _LL60;_LL63:
if(_tmpC2 <= (void*)3?1:*((int*)_tmpC2)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpC6=_cycalloc_atomic(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC7;_tmpC7.tag=6;_tmpC7.f1=1;_tmpC7;});_tmpC6;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpC8="size qualifier on non-integral type";_tag_arr(_tmpC8,sizeof(
char),_get_zero_arr_size(_tmpC8,36));}),last_loc);goto _LL60;_LL60:;}}return({
struct _tuple5 _tmpC9;_tmpC9.f1=t;_tmpC9.f2=declopt;_tmpC9;});}static struct Cyc_List_List*
Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCA=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCB=_tmpCA->id;
struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*
_tmpD0;struct _tuple6 _tmpCC=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCA->tms);
_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;return({
struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=({struct _tuple7*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->f1=_tmpCB;_tmpD2->f2=_tmpCD;_tmpD2->f3=
_tmpCE;_tmpD2->f4=_tmpCF;_tmpD2->f5=_tmpD0;_tmpD2;});_tmpD1->tl=Cyc_Parse_apply_tmss(
_tmpCD,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD1;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD3;_tmpD3.f1=
tq;_tmpD3.f2=t;_tmpD3.f3=0;_tmpD3.f4=atts;_tmpD3;});{void*_tmpD4=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;void*_tmpD8;struct Cyc_List_List*_tmpD9;struct Cyc_Position_Segment*_tmpDA;
void*_tmpDB;void*_tmpDC;struct Cyc_Absyn_Tqual _tmpDD;struct Cyc_Position_Segment*
_tmpDE;struct Cyc_List_List*_tmpDF;_LL68: if(*((int*)_tmpD4)!= 0)goto _LL6A;_tmpD5=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpD4)->f1;_LL69: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),Cyc_Absyn_array_typ(t,tq,0,_tmpD5),atts,tms->tl);_LL6A:
if(*((int*)_tmpD4)!= 1)goto _LL6C;_tmpD6=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD4)->f1;_tmpD7=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD4)->f2;_LL6B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmpD6,_tmpD7),atts,tms->tl);_LL6C: if(*((int*)_tmpD4)!= 3)
goto _LL6E;_tmpD8=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpD4)->f1;_LL6D: {
void*_tmpE0=_tmpD8;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_Position_Segment*
_tmpE6;_LL75: if(*((int*)_tmpE0)!= 1)goto _LL77;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f2;_tmpE3=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f5;_LL76: {
struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*
new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((
void*)as->hd))fn_atts=({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));
_tmpE7->hd=(void*)((void*)as->hd);_tmpE7->tl=fn_atts;_tmpE7;});else{new_atts=({
struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(void*)((void*)
as->hd);_tmpE8->tl=new_atts;_tmpE8;});}}}if(tms->tl != 0){void*_tmpE9=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpEA;_LL7A:
if(*((int*)_tmpE9)!= 4)goto _LL7C;_tmpEA=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE9)->f1;_LL7B: typvars=_tmpEA;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;
_LL79:;}if(((((!_tmpE2?_tmpE3 == 0: 0)?_tmpE1 != 0: 0)?_tmpE1->tl == 0: 0)?(*((struct
_tuple2*)((struct _tuple2*)_tmpE1->hd))).f1 == 0: 0)?(*((struct _tuple2*)((struct
_tuple2*)_tmpE1->hd))).f3 == (void*)0: 0)_tmpE1=0;t=Cyc_Parse_array2ptr(t,0);((
void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmpE1);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(
typvars,_tmpE4,t,_tmpE1,_tmpE2,_tmpE3,_tmpE5,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LL77: if(*((int*)_tmpE0)!= 0)goto _LL74;_tmpE6=((struct
Cyc_Absyn_NoTypes_struct*)_tmpE0)->f2;_LL78:({void*_tmpEB[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmpE6,({const char*_tmpEC="function declaration without parameter types";
_tag_arr(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,45));}),_tag_arr(_tmpEB,
sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD4)!= 4)goto _LL70;_tmpD9=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD4)->f1;_tmpDA=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD4)->f2;_LL6F: if(tms->tl == 0)return({struct _tuple6 _tmpED;_tmpED.f1=tq;_tmpED.f2=
t;_tmpED.f3=_tmpD9;_tmpED.f4=atts;_tmpED;});({void*_tmpEE[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmpDA,({const char*_tmpEF="type parameters must appear before function arguments in declarator";
_tag_arr(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,68));}),_tag_arr(_tmpEE,
sizeof(void*),0));});_LL70: if(*((int*)_tmpD4)!= 2)goto _LL72;_tmpDB=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpD4)->f1;_tmpDC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD4)->f2;_tmpDD=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD4)->f3;_LL71: {
void*_tmpF0=_tmpDB;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Conref*_tmpF2;
struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Conref*_tmpF4;struct Cyc_Absyn_Conref*
_tmpF5;_LL7F: if(*((int*)_tmpF0)!= 0)goto _LL81;_tmpF1=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpF0)->f1;_tmpF2=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmpF0)->f2;_LL80:
return Cyc_Parse_apply_tms(_tmpDD,Cyc_Absyn_atb_typ(t,_tmpDC,tq,(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpF7;_tmpF7.tag=0;_tmpF7.f1=_tmpF1;_tmpF7;});_tmpF6;}),_tmpF2),atts,tms->tl);
_LL81: if(*((int*)_tmpF0)!= 1)goto _LL83;_tmpF3=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmpF0)->f1;_tmpF4=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpF0)->f2;_LL82:
return Cyc_Parse_apply_tms(_tmpDD,Cyc_Absyn_starb_typ(t,_tmpDC,tq,(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpF9;_tmpF9.tag=0;_tmpF9.f1=_tmpF3;_tmpF9;});_tmpF8;}),_tmpF4),atts,tms->tl);
_LL83: if(*((int*)_tmpF0)!= 2)goto _LL7E;_tmpF5=((struct Cyc_Absyn_TaggedArray_ps_struct*)
_tmpF0)->f1;_LL84: return Cyc_Parse_apply_tms(_tmpDD,Cyc_Absyn_tagged_typ(t,_tmpDC,
tq,_tmpF5),atts,tms->tl);_LL7E:;}_LL72: if(*((int*)_tmpD4)!= 5)goto _LL67;_tmpDE=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpD4)->f1;_tmpDF=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD4)->f2;_LL73: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpDF),
tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpFB;struct Cyc_Core_Opt*_tmpFC;struct _tuple5 _tmpFA=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;if(_tmpFC != 0)({void*_tmpFD[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmpFE="ignoring nested type declaration(s) in specifier list";
_tag_arr(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,54));}),_tag_arr(_tmpFD,
sizeof(void*),0));});return _tmpFB;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t){struct _tuple7 _tmp100;struct
_tuple1*_tmp101;struct Cyc_Absyn_Tqual _tmp102;void*_tmp103;struct Cyc_List_List*
_tmp104;struct Cyc_List_List*_tmp105;struct _tuple7*_tmpFF=t;_tmp100=*_tmpFF;
_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;_tmp104=_tmp100.f4;
_tmp105=_tmp100.f5;Cyc_Lex_register_typedef(_tmp101);if(_tmp105 != 0)Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp105->hd);{void*_tmp106[1]={&
_tmp108};Cyc_aprintf(({const char*_tmp107="bad attribute %s within typedef";
_tag_arr(_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,32));}),_tag_arr(
_tmp106,sizeof(void*),1));}}),loc);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmp109=_tmp103;struct Cyc_Core_Opt*_tmp10A;_LL86: if(_tmp109 <= (void*)
3?1:*((int*)_tmp109)!= 0)goto _LL88;_tmp10A=((struct Cyc_Absyn_Evar_struct*)
_tmp109)->f1;_LL87: type=0;if(_tmp10A == 0)kind=({struct Cyc_Core_Opt*_tmp10B=
_cycalloc(sizeof(*_tmp10B));_tmp10B->v=(void*)((void*)2);_tmp10B;});else{kind=
_tmp10A;}goto _LL85;_LL88:;_LL89: kind=0;type=({struct Cyc_Core_Opt*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->v=(void*)_tmp103;_tmp10C;});goto _LL85;_LL85:;}
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp10D=
_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp10E;
_tmp10E.tag=7;_tmp10E.f1=({struct Cyc_Absyn_Typedefdecl*_tmp10F=_cycalloc(sizeof(*
_tmp10F));_tmp10F->name=_tmp101;_tmp10F->tvs=_tmp104;_tmp10F->kind=kind;_tmp10F->defn=
type;_tmp10F;});_tmp10E;});_tmp10D;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp111;_tmp111.tag=12;_tmp111.f1=d;_tmp111.f2=s;
_tmp111;});_tmp110;}),Cyc_Position_segment_join(d->loc,s->loc));}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(
Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp113;struct Cyc_Absyn_Tqual _tmp114;struct
Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Parse_Declaration_spec*
_tmp112=ds;_tmp113=*_tmp112;_tmp114=_tmp113.tq;_tmp115=_tmp113.type_specs;
_tmp116=_tmp113.attributes;if(ds->is_inline)Cyc_Parse_err(({const char*_tmp117="inline is allowed only on function definitions";
_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,47));}),loc);if(_tmp115
== 0){Cyc_Parse_err(({const char*_tmp118="missing type specifiers in declaration";
_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,39));}),loc);return 0;}{
void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp119=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL8B: if((int)_tmp119 != 0)goto _LL8D;_LL8C: istypedef=1;
goto _LL8A;_LL8D: if((int)_tmp119 != 1)goto _LL8F;_LL8E: s=(void*)3;goto _LL8A;_LL8F:
if((int)_tmp119 != 2)goto _LL91;_LL90: s=(void*)4;goto _LL8A;_LL91: if((int)_tmp119 != 
3)goto _LL93;_LL92: s=(void*)0;goto _LL8A;_LL93: if((int)_tmp119 != 4)goto _LL95;_LL94:
s=(void*)2;goto _LL8A;_LL95: if((int)_tmp119 != 5)goto _LL97;_LL96: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL8A;_LL97: if((int)_tmp119 != 6)goto _LL8A;_LL98:
s=(void*)1;goto _LL8A;_LL8A:;}{struct Cyc_List_List*_tmp11B;struct Cyc_List_List*
_tmp11C;struct _tuple0 _tmp11A=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmp11B=_tmp11A.f1;_tmp11C=_tmp11A.f2;{int exps_empty=1;{struct Cyc_List_List*
es=_tmp11C;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple5 _tmp11D=Cyc_Parse_collapse_type_specifiers(
_tmp115,loc);if(_tmp11B == 0){void*_tmp11F;struct Cyc_Core_Opt*_tmp120;struct
_tuple5 _tmp11E=_tmp11D;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;if(_tmp120 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp120->v;{void*_tmp121=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp122;struct Cyc_Absyn_Aggrdecl*_tmp123;struct Cyc_Absyn_Tuniondecl*
_tmp124;_LL9A: if(*((int*)_tmp121)!= 6)goto _LL9C;_tmp122=((struct Cyc_Absyn_Enum_d_struct*)
_tmp121)->f1;_LL9B:(void*)(_tmp122->sc=(void*)s);if(_tmp116 != 0)Cyc_Parse_err(({
const char*_tmp125="bad attributes on enum";_tag_arr(_tmp125,sizeof(char),
_get_zero_arr_size(_tmp125,23));}),loc);goto _LL99;_LL9C: if(*((int*)_tmp121)!= 4)
goto _LL9E;_tmp123=((struct Cyc_Absyn_Aggr_d_struct*)_tmp121)->f1;_LL9D:(void*)(
_tmp123->sc=(void*)s);_tmp123->attributes=_tmp116;goto _LL99;_LL9E: if(*((int*)
_tmp121)!= 5)goto _LLA0;_tmp124=((struct Cyc_Absyn_Tunion_d_struct*)_tmp121)->f1;
_LL9F:(void*)(_tmp124->sc=(void*)s);if(_tmp116 != 0)Cyc_Parse_err(({const char*
_tmp126="bad attributes on tunion";_tag_arr(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,25));}),loc);goto _LL99;_LLA0:;_LLA1: Cyc_Parse_err(({
const char*_tmp127="bad declaration";_tag_arr(_tmp127,sizeof(char),
_get_zero_arr_size(_tmp127,16));}),loc);return 0;_LL99:;}return({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=d;_tmp128->tl=0;_tmp128;});}
else{void*_tmp129=_tmp11F;struct Cyc_Absyn_AggrInfo _tmp12A;void*_tmp12B;void*
_tmp12C;struct _tuple1*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_TunionInfo
_tmp12F;void*_tmp130;struct Cyc_Absyn_Tuniondecl**_tmp131;struct Cyc_Absyn_TunionInfo
_tmp132;void*_tmp133;struct Cyc_Absyn_UnknownTunionInfo _tmp134;struct _tuple1*
_tmp135;int _tmp136;struct Cyc_List_List*_tmp137;struct _tuple1*_tmp138;struct Cyc_List_List*
_tmp139;_LLA3: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 10)goto _LLA5;_tmp12A=((
struct Cyc_Absyn_AggrType_struct*)_tmp129)->f1;_tmp12B=(void*)_tmp12A.aggr_info;
if(*((int*)_tmp12B)!= 0)goto _LLA5;_tmp12C=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp12B)->f2;_tmp12E=
_tmp12A.targs;_LLA4: {struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12E);
struct Cyc_Absyn_Aggrdecl*_tmp13B=({struct Cyc_Absyn_Aggrdecl*_tmp140=_cycalloc(
sizeof(*_tmp140));_tmp140->kind=(void*)_tmp12C;_tmp140->sc=(void*)s;_tmp140->name=
_tmp12D;_tmp140->tvs=_tmp13A;_tmp140->impl=0;_tmp140->attributes=0;_tmp140;});
if(_tmp116 != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on type declaration";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,35));}),loc);return({
struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));
_tmp13E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp13F;_tmp13F.tag=4;_tmp13F.f1=
_tmp13B;_tmp13F;});_tmp13E;}),loc);_tmp13D->tl=0;_tmp13D;});}_LLA5: if(_tmp129 <= (
void*)3?1:*((int*)_tmp129)!= 2)goto _LLA7;_tmp12F=((struct Cyc_Absyn_TunionType_struct*)
_tmp129)->f1;_tmp130=(void*)_tmp12F.tunion_info;if(*((int*)_tmp130)!= 1)goto
_LLA7;_tmp131=((struct Cyc_Absyn_KnownTunion_struct*)_tmp130)->f1;_LLA6: if(
_tmp116 != 0)Cyc_Parse_err(({const char*_tmp141="bad attributes on tunion";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,25));}),loc);return({
struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp143=_cycalloc(sizeof(*_tmp143));
_tmp143[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp144;_tmp144.tag=5;_tmp144.f1=*((
struct Cyc_Absyn_Tuniondecl**)_tmp131);_tmp144;});_tmp143;}),loc);_tmp142->tl=0;
_tmp142;});_LLA7: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 2)goto _LLA9;_tmp132=((
struct Cyc_Absyn_TunionType_struct*)_tmp129)->f1;_tmp133=(void*)_tmp132.tunion_info;
if(*((int*)_tmp133)!= 0)goto _LLA9;_tmp134=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp133)->f1;_tmp135=_tmp134.name;_tmp136=_tmp134.is_xtunion;_tmp137=_tmp132.targs;
_LLA8: {struct Cyc_List_List*_tmp145=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp137);struct Cyc_Absyn_Decl*_tmp146=
Cyc_Absyn_tunion_decl(s,_tmp135,_tmp145,0,_tmp136,loc);if(_tmp116 != 0)Cyc_Parse_err(({
const char*_tmp147="bad attributes on tunion";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,25));}),loc);return({struct Cyc_List_List*_tmp148=
_cycalloc(sizeof(*_tmp148));_tmp148->hd=_tmp146;_tmp148->tl=0;_tmp148;});}_LLA9:
if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 12)goto _LLAB;_tmp138=((struct Cyc_Absyn_EnumType_struct*)
_tmp129)->f1;_LLAA: {struct Cyc_Absyn_Enumdecl*_tmp149=({struct Cyc_Absyn_Enumdecl*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->sc=(void*)s;_tmp14F->name=_tmp138;
_tmp14F->fields=0;_tmp14F;});if(_tmp116 != 0)Cyc_Parse_err(({const char*_tmp14A="bad attributes on enum";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,23));}),loc);return({
struct Cyc_List_List*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=({struct Cyc_Absyn_Decl*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14E;_tmp14E.tag=6;_tmp14E.f1=_tmp149;_tmp14E;});_tmp14D;}));_tmp14C->loc=loc;
_tmp14C;});_tmp14B->tl=0;_tmp14B;});}_LLAB: if(_tmp129 <= (void*)3?1:*((int*)
_tmp129)!= 13)goto _LLAD;_tmp139=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp129)->f1;
_LLAC: {struct Cyc_Absyn_Enumdecl*_tmp150=({struct Cyc_Absyn_Enumdecl*_tmp156=
_cycalloc(sizeof(*_tmp156));_tmp156->sc=(void*)s;_tmp156->name=Cyc_Parse_gensym_enum();
_tmp156->fields=({struct Cyc_Core_Opt*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->v=
_tmp139;_tmp157;});_tmp156;});if(_tmp116 != 0)Cyc_Parse_err(({const char*_tmp151="bad attributes on enum";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,23));}),loc);return({
struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->hd=({struct Cyc_Absyn_Decl*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp155;_tmp155.tag=6;_tmp155.f1=_tmp150;_tmp155;});_tmp154;}));_tmp153->loc=loc;
_tmp153;});_tmp152->tl=0;_tmp152;});}_LLAD:;_LLAE: Cyc_Parse_err(({const char*
_tmp158="missing declarator";_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(
_tmp158,19));}),loc);return 0;_LLA2:;}}else{void*t=_tmp11D.f1;struct Cyc_List_List*
_tmp159=Cyc_Parse_apply_tmss(_tmp114,t,_tmp11B,_tmp116);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp15A="initializer in typedef declaration";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,35));}),loc);{struct Cyc_List_List*
decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp159);if(_tmp11D.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp11D.f2)->v;{void*_tmp15B=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp15C;struct Cyc_Absyn_Tuniondecl*_tmp15D;struct Cyc_Absyn_Enumdecl*_tmp15E;
_LLB0: if(*((int*)_tmp15B)!= 4)goto _LLB2;_tmp15C=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp15B)->f1;_LLB1:(void*)(_tmp15C->sc=(void*)s);_tmp15C->attributes=_tmp116;
_tmp116=0;goto _LLAF;_LLB2: if(*((int*)_tmp15B)!= 5)goto _LLB4;_tmp15D=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp15B)->f1;_LLB3:(void*)(_tmp15D->sc=(void*)s);goto _LLAF;_LLB4: if(*((int*)
_tmp15B)!= 6)goto _LLB6;_tmp15E=((struct Cyc_Absyn_Enum_d_struct*)_tmp15B)->f1;
_LLB5:(void*)(_tmp15E->sc=(void*)s);goto _LLAF;_LLB6:;_LLB7: Cyc_Parse_err(({const
char*_tmp15F="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,70));}),loc);goto _LLAF;
_LLAF:;}decls=({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=
d;_tmp160->tl=decls;_tmp160;});}if(_tmp116 != 0)Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(struct _tagged_arr)
Cyc_Absyn_attribute2string((void*)_tmp116->hd);{void*_tmp161[1]={& _tmp163};Cyc_aprintf(({
const char*_tmp162="bad attribute %s in typedef";_tag_arr(_tmp162,sizeof(char),
_get_zero_arr_size(_tmp162,28));}),_tag_arr(_tmp161,sizeof(void*),1));}}),loc);
return decls;}}else{if(_tmp11D.f2 != 0)Cyc_Parse_unimp(({const char*_tmp164="nested type declaration within declarator";
_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,42));}),loc);{struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp165=_tmp159;for(0;_tmp165 != 0;(_tmp165=_tmp165->tl,
_tmp11C=_tmp11C->tl)){struct _tuple7 _tmp167;struct _tuple1*_tmp168;struct Cyc_Absyn_Tqual
_tmp169;void*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16C;
struct _tuple7*_tmp166=(struct _tuple7*)_tmp165->hd;_tmp167=*_tmp166;_tmp168=
_tmp167.f1;_tmp169=_tmp167.f2;_tmp16A=_tmp167.f3;_tmp16B=_tmp167.f4;_tmp16C=
_tmp167.f5;if(_tmp16B != 0)({void*_tmp16D[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp16E="bad type params, ignoring";_tag_arr(_tmp16E,sizeof(char),
_get_zero_arr_size(_tmp16E,26));}),_tag_arr(_tmp16D,sizeof(void*),0));});if(
_tmp11C == 0)({void*_tmp16F[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp170="unexpected NULL in parse!";
_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,26));}),_tag_arr(
_tmp16F,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp171=(struct Cyc_Absyn_Exp*)
_tmp11C->hd;struct Cyc_Absyn_Vardecl*_tmp172=Cyc_Absyn_new_vardecl(_tmp168,
_tmp16A,_tmp171);_tmp172->tq=_tmp169;(void*)(_tmp172->sc=(void*)s);_tmp172->attributes=
_tmp16C;{struct Cyc_Absyn_Decl*_tmp173=({struct Cyc_Absyn_Decl*_tmp175=_cycalloc(
sizeof(*_tmp175));_tmp175->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_Var_d_struct
_tmp177;_tmp177.tag=0;_tmp177.f1=_tmp172;_tmp177;});_tmp176;}));_tmp175->loc=loc;
_tmp175;});decls=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));
_tmp174->hd=_tmp173;_tmp174->tl=decls;_tmp174;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB8: return(void*)0;case 'M': _LLB9: return(void*)1;case 'B': _LLBA:
return(void*)2;case 'R': _LLBB: return(void*)3;case 'E': _LLBC: return(void*)4;case 'I':
_LLBD: return(void*)5;default: _LLBE: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _tagged_arr)s;{
void*_tmp178[1]={& _tmp17A};Cyc_aprintf(({const char*_tmp179="bad kind: %s";
_tag_arr(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,13));}),_tag_arr(
_tmp178,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(atts == 0)return tms;else{return({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*
_tmp17B));_tmp17B->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp17D;_tmp17D.tag=5;_tmp17D.f1=loc;_tmp17D.f2=atts;_tmp17D;});_tmp17C;}));
_tmp17B->tl=tms;_tmp17B;});}}char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok\000";
struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok\000";
struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;};
char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok\000";
struct Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_AttributeList_tok[
22]="\000\000\000\000AttributeList_tok\000";struct Cyc_AttributeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok\000";
struct Cyc_Attribute_tok_struct{char*tag;void*f1;};char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok\000";
struct Cyc_Bool_tok_struct{char*tag;int f1;};char Cyc_Char_tok[13]="\000\000\000\000Char_tok\000";
struct Cyc_Char_tok_struct{char*tag;char f1;};char Cyc_DeclList_tok[17]="\000\000\000\000DeclList_tok\000";
struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_DeclSpec_tok[
17]="\000\000\000\000DeclSpec_tok\000";struct Cyc_DeclSpec_tok_struct{char*tag;
struct Cyc_Parse_Declaration_spec*f1;};char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok\000";
struct Cyc_Declarator_tok_struct{char*tag;struct Cyc_Parse_Declarator*f1;};char Cyc_DesignatorList_tok[
23]="\000\000\000\000DesignatorList_tok\000";struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok\000";
struct Cyc_Designator_tok_struct{char*tag;void*f1;};char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok\000";
struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Enumfield_tok[
18]="\000\000\000\000Enumfield_tok\000";struct Cyc_Enumfield_tok_struct{char*tag;
struct Cyc_Absyn_Enumfield*f1;};char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok\000";
struct Cyc_ExpList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Exp_tok[
12]="\000\000\000\000Exp_tok\000";struct Cyc_Exp_tok_struct{char*tag;struct Cyc_Absyn_Exp*
f1;};char Cyc_FieldPatternList_tok[25]="\000\000\000\000FieldPatternList_tok\000";
struct Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_FieldPattern_tok[
21]="\000\000\000\000FieldPattern_tok\000";struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{char*tag;struct
_tuple11*f1;};char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok\000";struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok\000";
struct Cyc_IdList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_InitDeclList_tok[
21]="\000\000\000\000InitDeclList_tok\000";struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok\000";
struct _tuple12{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok\000";
struct Cyc_InitializerList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Int_tok[
12]="\000\000\000\000Int_tok\000";struct _tuple13{void*f1;int f2;};struct Cyc_Int_tok_struct{
char*tag;struct _tuple13*f1;};char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok\000";
struct Cyc_Kind_tok_struct{char*tag;void*f1;};char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok\000";
struct _tuple14{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
char*tag;struct _tuple14*f1;};char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok\000";
struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_ParamDecl_tok[
18]="\000\000\000\000ParamDecl_tok\000";struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple2*f1;};char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok\000";
struct Cyc_PatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Pattern_tok[
16]="\000\000\000\000Pattern_tok\000";struct Cyc_Pattern_tok_struct{char*tag;
struct Cyc_Absyn_Pat*f1;};char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok\000";
struct Cyc_Pointer_Sort_tok_struct{char*tag;void*f1;};char Cyc_Primop_tok[15]="\000\000\000\000Primop_tok\000";
struct Cyc_Primop_tok_struct{char*tag;void*f1;};char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok\000";
struct Cyc_Primopopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_QualId_tok[
15]="\000\000\000\000QualId_tok\000";struct Cyc_QualId_tok_struct{char*tag;struct
_tuple1*f1;};char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok\000";
struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};char Cyc_Rgnorder_tok[
17]="\000\000\000\000Rgnorder_tok\000";struct Cyc_Rgnorder_tok_struct{char*tag;
struct Cyc_List_List*f1;};char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok\000";
struct Cyc_Scope_tok_struct{char*tag;void*f1;};char Cyc_Short_tok[14]="\000\000\000\000Short_tok\000";
struct Cyc_Short_tok_struct{char*tag;short f1;};char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok\000";
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};char Cyc_StorageClass_tok[
21]="\000\000\000\000StorageClass_tok\000";struct Cyc_StorageClass_tok_struct{
char*tag;void*f1;};char Cyc_String_tok[15]="\000\000\000\000String_tok\000";
struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};char Cyc_Stringopt_tok[
18]="\000\000\000\000Stringopt_tok\000";struct Cyc_Stringopt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok\000";
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchCClauseList_tok[
26]="\000\000\000\000SwitchCClauseList_tok\000";struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_SwitchClauseList_tok[25]="\000\000\000\000SwitchClauseList_tok\000";
struct Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[
24]="\000\000\000\000TunionFieldList_tok\000";struct Cyc_TunionFieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok\000";
struct Cyc_TunionField_tok_struct{char*tag;struct Cyc_Absyn_Tunionfield*f1;};char
Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok\000";struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[25]="\000\000\000\000TypeModifierList_tok\000";
struct Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[
16]="\000\000\000\000TypeOpt_tok\000";struct Cyc_TypeOpt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok\000";
struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[
22]="\000\000\000\000TypeSpecifier_tok\000";struct Cyc_TypeSpecifier_tok_struct{
char*tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok\000";struct Cyc_Type_tok_struct{
char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1\000";struct Cyc_YY1_struct{
char*tag;struct _tuple14*f1;};char Cyc_YY2[8]="\000\000\000\000YY2\000";struct Cyc_YY2_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL\000";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";static char _tmp17F[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp17F,
_tmp17F,_tmp17F + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp180=yy1;struct _tuple13*_tmp181;_LLC1: if(*((void**)_tmp180)!= Cyc_Int_tok)
goto _LLC3;_tmp181=((struct Cyc_Int_tok_struct*)_tmp180)->f1;_LLC2: yyzzz=_tmp181;
goto _LLC0;_LLC3:;_LLC4:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLC0:;}return
yyzzz;}static char _tmp183[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp183,_tmp183,_tmp183 + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp184=yy1;char _tmp185;_LLC6: if(*((void**)_tmp184)!= Cyc_Char_tok)
goto _LLC8;_tmp185=((struct Cyc_Char_tok_struct*)_tmp184)->f1;_LLC7: yyzzz=_tmp185;
goto _LLC5;_LLC8:;_LLC9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLC5:;}return
yyzzz;}static char _tmp187[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp187,_tmp187,_tmp187 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp188=yy1;struct _tagged_arr _tmp189;
_LLCB: if(*((void**)_tmp188)!= Cyc_String_tok)goto _LLCD;_tmp189=((struct Cyc_String_tok_struct*)
_tmp188)->f1;_LLCC: yyzzz=_tmp189;goto _LLCA;_LLCD:;_LLCE:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLCA:;}return yyzzz;}static char _tmp18B[20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{_tmp18B,_tmp18B,_tmp18B + 20}};
void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*yyzzz;{void*_tmp18C=yy1;void*
_tmp18D;_LLD0: if(*((void**)_tmp18C)!= Cyc_Pointer_Sort_tok)goto _LLD2;_tmp18D=(
void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp18C)->f1;_LLD1: yyzzz=_tmp18D;goto
_LLCF;_LLD2:;_LLD3:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LLCF:;}
return yyzzz;}static char _tmp18F[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{_tmp18F,_tmp18F,_tmp18F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(
void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp190=yy1;struct Cyc_Absyn_Exp*
_tmp191;_LLD5: if(*((void**)_tmp190)!= Cyc_Exp_tok)goto _LLD7;_tmp191=((struct Cyc_Exp_tok_struct*)
_tmp190)->f1;_LLD6: yyzzz=_tmp191;goto _LLD4;_LLD7:;_LLD8:(int)_throw((void*)& Cyc_yyfail_Exp_tok);
_LLD4:;}return yyzzz;}static char _tmp193[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp194=yy1;struct Cyc_List_List*_tmp195;_LLDA: if(*((void**)_tmp194)!= Cyc_ExpList_tok)
goto _LLDC;_tmp195=((struct Cyc_ExpList_tok_struct*)_tmp194)->f1;_LLDB: yyzzz=
_tmp195;goto _LLD9;_LLDC:;_LLDD:(int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD9:;}
return yyzzz;}static char _tmp197[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{
_tmp197,_tmp197,_tmp197 + 39}};struct Cyc_List_List*Cyc_yyget_InitializerList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp198=yy1;struct Cyc_List_List*
_tmp199;_LLDF: if(*((void**)_tmp198)!= Cyc_InitializerList_tok)goto _LLE1;_tmp199=((
struct Cyc_InitializerList_tok_struct*)_tmp198)->f1;_LLE0: yyzzz=_tmp199;goto _LLDE;
_LLE1:;_LLE2:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LLDE:;}return
yyzzz;}static char _tmp19B[9]="primop_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 9}};void*Cyc_yyget_Primop_tok(void*
yy1){void*yyzzz;{void*_tmp19C=yy1;void*_tmp19D;_LLE4: if(*((void**)_tmp19C)!= Cyc_Primop_tok)
goto _LLE6;_tmp19D=(void*)((struct Cyc_Primop_tok_struct*)_tmp19C)->f1;_LLE5: yyzzz=
_tmp19D;goto _LLE3;_LLE6:;_LLE7:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LLE3:;}
return yyzzz;}static char _tmp19F[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp19F,_tmp19F,_tmp19F + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp1A0=yy1;struct Cyc_Core_Opt*_tmp1A1;_LLE9: if(*((void**)_tmp1A0)!= Cyc_Primopopt_tok)
goto _LLEB;_tmp1A1=((struct Cyc_Primopopt_tok_struct*)_tmp1A0)->f1;_LLEA: yyzzz=
_tmp1A1;goto _LLE8;_LLEB:;_LLEC:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE8:;}return yyzzz;}static char _tmp1A3[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1A3,_tmp1A3,_tmp1A3 + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp1A4=yy1;
struct _tuple1*_tmp1A5;_LLEE: if(*((void**)_tmp1A4)!= Cyc_QualId_tok)goto _LLF0;
_tmp1A5=((struct Cyc_QualId_tok_struct*)_tmp1A4)->f1;_LLEF: yyzzz=_tmp1A5;goto
_LLED;_LLF0:;_LLF1:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLED:;}return
yyzzz;}static char _tmp1A7[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1A7,_tmp1A7,_tmp1A7 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1A8=yy1;struct Cyc_Absyn_Stmt*
_tmp1A9;_LLF3: if(*((void**)_tmp1A8)!= Cyc_Stmt_tok)goto _LLF5;_tmp1A9=((struct Cyc_Stmt_tok_struct*)
_tmp1A8)->f1;_LLF4: yyzzz=_tmp1A9;goto _LLF2;_LLF5:;_LLF6:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF2:;}return yyzzz;}static char _tmp1AB[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp1AB,_tmp1AB,_tmp1AB + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1AC=yy1;struct Cyc_List_List*
_tmp1AD;_LLF8: if(*((void**)_tmp1AC)!= Cyc_SwitchClauseList_tok)goto _LLFA;_tmp1AD=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp1AC)->f1;_LLF9: yyzzz=_tmp1AD;goto
_LLF7;_LLFA:;_LLFB:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF7:;}
return yyzzz;}static char _tmp1AF[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp1AF,_tmp1AF,_tmp1AF + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1B0=yy1;struct Cyc_List_List*_tmp1B1;_LLFD: if(*((void**)_tmp1B0)
!= Cyc_SwitchCClauseList_tok)goto _LLFF;_tmp1B1=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp1B0)->f1;_LLFE: yyzzz=_tmp1B1;goto _LLFC;_LLFF:;_LL100:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFC:;}return yyzzz;}static char _tmp1B3[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1B4=yy1;
struct Cyc_Absyn_Pat*_tmp1B5;_LL102: if(*((void**)_tmp1B4)!= Cyc_Pattern_tok)goto
_LL104;_tmp1B5=((struct Cyc_Pattern_tok_struct*)_tmp1B4)->f1;_LL103: yyzzz=_tmp1B5;
goto _LL101;_LL104:;_LL105:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL101:;}
return yyzzz;}static char _tmp1B7[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1B7,_tmp1B7,_tmp1B7 + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1B8=yy1;struct Cyc_List_List*_tmp1B9;_LL107: if(*((void**)_tmp1B8)
!= Cyc_PatternList_tok)goto _LL109;_tmp1B9=((struct Cyc_PatternList_tok_struct*)
_tmp1B8)->f1;_LL108: yyzzz=_tmp1B9;goto _LL106;_LL109:;_LL10A:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL106:;}return yyzzz;}static char _tmp1BB[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1BB,_tmp1BB,_tmp1BB + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1BC=yy1;struct _tuple11*_tmp1BD;_LL10C: if(*((void**)
_tmp1BC)!= Cyc_FieldPattern_tok)goto _LL10E;_tmp1BD=((struct Cyc_FieldPattern_tok_struct*)
_tmp1BC)->f1;_LL10D: yyzzz=_tmp1BD;goto _LL10B;_LL10E:;_LL10F:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL10B:;}return yyzzz;}static char _tmp1BF[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1BF,_tmp1BF,_tmp1BF + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C0=yy1;struct Cyc_List_List*
_tmp1C1;_LL111: if(*((void**)_tmp1C0)!= Cyc_FieldPatternList_tok)goto _LL113;
_tmp1C1=((struct Cyc_FieldPatternList_tok_struct*)_tmp1C0)->f1;_LL112: yyzzz=
_tmp1C1;goto _LL110;_LL113:;_LL114:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL110:;}return yyzzz;}static char _tmp1C3[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1C4=yy1;
struct Cyc_Absyn_Fndecl*_tmp1C5;_LL116: if(*((void**)_tmp1C4)!= Cyc_FnDecl_tok)
goto _LL118;_tmp1C5=((struct Cyc_FnDecl_tok_struct*)_tmp1C4)->f1;_LL117: yyzzz=
_tmp1C5;goto _LL115;_LL118:;_LL119:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL115:;}return yyzzz;}static char _tmp1C7[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1C7,_tmp1C7,_tmp1C7 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1C8=yy1;struct Cyc_List_List*_tmp1C9;_LL11B: if(*((void**)_tmp1C8)!= Cyc_DeclList_tok)
goto _LL11D;_tmp1C9=((struct Cyc_DeclList_tok_struct*)_tmp1C8)->f1;_LL11C: yyzzz=
_tmp1C9;goto _LL11A;_LL11D:;_LL11E:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL11A:;}return yyzzz;}static char _tmp1CB[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1CC=yy1;struct Cyc_Parse_Declaration_spec*_tmp1CD;_LL120: if(*((
void**)_tmp1CC)!= Cyc_DeclSpec_tok)goto _LL122;_tmp1CD=((struct Cyc_DeclSpec_tok_struct*)
_tmp1CC)->f1;_LL121: yyzzz=_tmp1CD;goto _LL11F;_LL122:;_LL123:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11F:;}return yyzzz;}static char _tmp1CF[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1CF,_tmp1CF,_tmp1CF + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1D0=yy1;struct _tuple12*_tmp1D1;_LL125: if(*((void**)
_tmp1D0)!= Cyc_InitDecl_tok)goto _LL127;_tmp1D1=((struct Cyc_InitDecl_tok_struct*)
_tmp1D0)->f1;_LL126: yyzzz=_tmp1D1;goto _LL124;_LL127:;_LL128:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL124:;}return yyzzz;}static char _tmp1D3[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1D3,_tmp1D3,_tmp1D3 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D4=yy1;struct Cyc_List_List*
_tmp1D5;_LL12A: if(*((void**)_tmp1D4)!= Cyc_InitDeclList_tok)goto _LL12C;_tmp1D5=((
struct Cyc_InitDeclList_tok_struct*)_tmp1D4)->f1;_LL12B: yyzzz=_tmp1D5;goto _LL129;
_LL12C:;_LL12D:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL129:;}return
yyzzz;}static char _tmp1D7[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1D8=yy1;void*
_tmp1D9;_LL12F: if(*((void**)_tmp1D8)!= Cyc_StorageClass_tok)goto _LL131;_tmp1D9=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1D8)->f1;_LL130: yyzzz=_tmp1D9;
goto _LL12E;_LL131:;_LL132:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12E:;}return yyzzz;}static char _tmp1DB[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1DC=yy1;void*
_tmp1DD;_LL134: if(*((void**)_tmp1DC)!= Cyc_TypeSpecifier_tok)goto _LL136;_tmp1DD=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1DC)->f1;_LL135: yyzzz=_tmp1DD;
goto _LL133;_LL136:;_LL137:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL133:;}return yyzzz;}static char _tmp1DF[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1E0=yy1;void*
_tmp1E1;_LL139: if(*((void**)_tmp1E0)!= Cyc_StructOrUnion_tok)goto _LL13B;_tmp1E1=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1E0)->f1;_LL13A: yyzzz=_tmp1E1;
goto _LL138;_LL13B:;_LL13C:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL138:;}return yyzzz;}static char _tmp1E3[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1E4=yy1;struct Cyc_Absyn_Tqual _tmp1E5;_LL13E: if(*((void**)_tmp1E4)!= Cyc_TypeQual_tok)
goto _LL140;_tmp1E5=((struct Cyc_TypeQual_tok_struct*)_tmp1E4)->f1;_LL13F: yyzzz=
_tmp1E5;goto _LL13D;_LL140:;_LL141:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13D:;}return yyzzz;}static char _tmp1E7[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1E7,_tmp1E7,_tmp1E7 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E8=yy1;struct Cyc_List_List*
_tmp1E9;_LL143: if(*((void**)_tmp1E8)!= Cyc_AggrFieldDeclList_tok)goto _LL145;
_tmp1E9=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1E8)->f1;_LL144: yyzzz=
_tmp1E9;goto _LL142;_LL145:;_LL146:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL142:;}return yyzzz;}static char _tmp1EB[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1EB,_tmp1EB,_tmp1EB + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EC=yy1;struct Cyc_List_List*
_tmp1ED;_LL148: if(*((void**)_tmp1EC)!= Cyc_AggrFieldDeclListList_tok)goto _LL14A;
_tmp1ED=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1EC)->f1;_LL149: yyzzz=
_tmp1ED;goto _LL147;_LL14A:;_LL14B:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL147:;}return yyzzz;}static char _tmp1EF[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1EF,_tmp1EF,_tmp1EF + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F0=yy1;struct Cyc_List_List*
_tmp1F1;_LL14D: if(*((void**)_tmp1F0)!= Cyc_TypeModifierList_tok)goto _LL14F;
_tmp1F1=((struct Cyc_TypeModifierList_tok_struct*)_tmp1F0)->f1;_LL14E: yyzzz=
_tmp1F1;goto _LL14C;_LL14F:;_LL150:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14C:;}return yyzzz;}static char _tmp1F3[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1F4=yy1;struct Cyc_Parse_Declarator*_tmp1F5;_LL152: if(*((void**)
_tmp1F4)!= Cyc_Declarator_tok)goto _LL154;_tmp1F5=((struct Cyc_Declarator_tok_struct*)
_tmp1F4)->f1;_LL153: yyzzz=_tmp1F5;goto _LL151;_LL154:;_LL155:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL151:;}return yyzzz;}static char _tmp1F7[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1F7,_tmp1F7,_tmp1F7 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1F8=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1F9;_LL157: if(*((void**)_tmp1F8)!= Cyc_AbstractDeclarator_tok)goto _LL159;
_tmp1F9=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1F8)->f1;_LL158: yyzzz=
_tmp1F9;goto _LL156;_LL159:;_LL15A:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL156:;}return yyzzz;}static char _tmp1FB[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1FC=yy1;int _tmp1FD;_LL15C: if(*((void**)_tmp1FC)!= 
Cyc_Bool_tok)goto _LL15E;_tmp1FD=((struct Cyc_Bool_tok_struct*)_tmp1FC)->f1;_LL15D:
yyzzz=_tmp1FD;goto _LL15B;_LL15E:;_LL15F:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL15B:;}return yyzzz;}static char _tmp1FF[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp200=yy1;void*_tmp201;_LL161: if(*((void**)_tmp200)
!= Cyc_Scope_tok)goto _LL163;_tmp201=(void*)((struct Cyc_Scope_tok_struct*)_tmp200)->f1;
_LL162: yyzzz=_tmp201;goto _LL160;_LL163:;_LL164:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL160:;}return yyzzz;}static char _tmp203[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp204=yy1;struct Cyc_Absyn_Tunionfield*_tmp205;_LL166: if(*((void**)
_tmp204)!= Cyc_TunionField_tok)goto _LL168;_tmp205=((struct Cyc_TunionField_tok_struct*)
_tmp204)->f1;_LL167: yyzzz=_tmp205;goto _LL165;_LL168:;_LL169:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL165:;}return yyzzz;}static char _tmp207[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp207,_tmp207,_tmp207 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp208=yy1;struct Cyc_List_List*
_tmp209;_LL16B: if(*((void**)_tmp208)!= Cyc_TunionFieldList_tok)goto _LL16D;
_tmp209=((struct Cyc_TunionFieldList_tok_struct*)_tmp208)->f1;_LL16C: yyzzz=
_tmp209;goto _LL16A;_LL16D:;_LL16E:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL16A:;}return yyzzz;}static char _tmp20B[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp20B,_tmp20B,_tmp20B + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp20C=yy1;struct _tuple15*_tmp20D;_LL170: if(*((void**)
_tmp20C)!= Cyc_QualSpecList_tok)goto _LL172;_tmp20D=((struct Cyc_QualSpecList_tok_struct*)
_tmp20C)->f1;_LL171: yyzzz=_tmp20D;goto _LL16F;_LL172:;_LL173:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16F:;}return yyzzz;}static char _tmp20F[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp20F,_tmp20F,_tmp20F + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp210=yy1;struct Cyc_List_List*_tmp211;_LL175:
if(*((void**)_tmp210)!= Cyc_IdList_tok)goto _LL177;_tmp211=((struct Cyc_IdList_tok_struct*)
_tmp210)->f1;_LL176: yyzzz=_tmp211;goto _LL174;_LL177:;_LL178:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL174:;}return yyzzz;}static char _tmp213[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp213,_tmp213,_tmp213 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp214=yy1;struct _tuple2*_tmp215;_LL17A: if(*((void**)
_tmp214)!= Cyc_ParamDecl_tok)goto _LL17C;_tmp215=((struct Cyc_ParamDecl_tok_struct*)
_tmp214)->f1;_LL17B: yyzzz=_tmp215;goto _LL179;_LL17C:;_LL17D:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL179:;}return yyzzz;}static char _tmp217[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp217,_tmp217,_tmp217 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp218=yy1;struct Cyc_List_List*
_tmp219;_LL17F: if(*((void**)_tmp218)!= Cyc_ParamDeclList_tok)goto _LL181;_tmp219=((
struct Cyc_ParamDeclList_tok_struct*)_tmp218)->f1;_LL180: yyzzz=_tmp219;goto _LL17E;
_LL181:;_LL182:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17E:;}return
yyzzz;}static char _tmp21B[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp21B,
_tmp21B,_tmp21B + 107}};struct _tuple14*Cyc_yyget_YY1(void*yy1){struct _tuple14*
yyzzz;{void*_tmp21C=yy1;struct _tuple14*_tmp21D;_LL184: if(*((void**)_tmp21C)!= 
Cyc_YY1)goto _LL186;_tmp21D=((struct Cyc_YY1_struct*)_tmp21C)->f1;_LL185: yyzzz=
_tmp21D;goto _LL183;_LL186:;_LL187:(int)_throw((void*)& Cyc_yyfail_YY1);_LL183:;}
return yyzzz;}static char _tmp21F[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp220=yy1;struct Cyc_List_List*_tmp221;_LL189: if(*((void**)_tmp220)!= Cyc_TypeList_tok)
goto _LL18B;_tmp221=((struct Cyc_TypeList_tok_struct*)_tmp220)->f1;_LL18A: yyzzz=
_tmp221;goto _LL188;_LL18B:;_LL18C:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL188:;}return yyzzz;}static char _tmp223[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp223,
_tmp223,_tmp223 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp224=yy1;struct Cyc_List_List*_tmp225;_LL18E:
if(*((void**)_tmp224)!= Cyc_DesignatorList_tok)goto _LL190;_tmp225=((struct Cyc_DesignatorList_tok_struct*)
_tmp224)->f1;_LL18F: yyzzz=_tmp225;goto _LL18D;_LL190:;_LL191:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18D:;}return yyzzz;}static char _tmp227[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp227,_tmp227,_tmp227 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp228=yy1;void*_tmp229;_LL193: if(*((void**)_tmp228)!= Cyc_Designator_tok)
goto _LL195;_tmp229=(void*)((struct Cyc_Designator_tok_struct*)_tmp228)->f1;_LL194:
yyzzz=_tmp229;goto _LL192;_LL195:;_LL196:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL192:;}return yyzzz;}static char _tmp22B[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp22C=yy1;void*_tmp22D;_LL198: if(*((void**)_tmp22C)
!= Cyc_Kind_tok)goto _LL19A;_tmp22D=(void*)((struct Cyc_Kind_tok_struct*)_tmp22C)->f1;
_LL199: yyzzz=_tmp22D;goto _LL197;_LL19A:;_LL19B:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL197:;}return yyzzz;}static char _tmp22F[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp230=yy1;void*_tmp231;_LL19D: if(*((void**)_tmp230)
!= Cyc_Type_tok)goto _LL19F;_tmp231=(void*)((struct Cyc_Type_tok_struct*)_tmp230)->f1;
_LL19E: yyzzz=_tmp231;goto _LL19C;_LL19F:;_LL1A0:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19C:;}return yyzzz;}static char _tmp233[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp233,
_tmp233,_tmp233 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp234=yy1;struct Cyc_List_List*_tmp235;_LL1A2:
if(*((void**)_tmp234)!= Cyc_AttributeList_tok)goto _LL1A4;_tmp235=((struct Cyc_AttributeList_tok_struct*)
_tmp234)->f1;_LL1A3: yyzzz=_tmp235;goto _LL1A1;_LL1A4:;_LL1A5:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1A1:;}return yyzzz;}static char _tmp237[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp237,_tmp237,_tmp237 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp238=yy1;void*_tmp239;_LL1A7: if(*((void**)_tmp238)!= Cyc_Attribute_tok)
goto _LL1A9;_tmp239=(void*)((struct Cyc_Attribute_tok_struct*)_tmp238)->f1;_LL1A8:
yyzzz=_tmp239;goto _LL1A6;_LL1A9:;_LL1AA:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A6:;}return yyzzz;}static char _tmp23B[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp23C=yy1;struct Cyc_Absyn_Enumfield*_tmp23D;_LL1AC: if(*((void**)
_tmp23C)!= Cyc_Enumfield_tok)goto _LL1AE;_tmp23D=((struct Cyc_Enumfield_tok_struct*)
_tmp23C)->f1;_LL1AD: yyzzz=_tmp23D;goto _LL1AB;_LL1AE:;_LL1AF:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1AB:;}return yyzzz;}static char _tmp23F[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp23F,_tmp23F,_tmp23F + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp240=yy1;struct Cyc_List_List*
_tmp241;_LL1B1: if(*((void**)_tmp240)!= Cyc_EnumfieldList_tok)goto _LL1B3;_tmp241=((
struct Cyc_EnumfieldList_tok_struct*)_tmp240)->f1;_LL1B2: yyzzz=_tmp241;goto _LL1B0;
_LL1B3:;_LL1B4:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1B0:;}return
yyzzz;}static char _tmp243[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp244=yy1;struct Cyc_Core_Opt*_tmp245;_LL1B6: if(*((void**)_tmp244)!= Cyc_TypeOpt_tok)
goto _LL1B8;_tmp245=((struct Cyc_TypeOpt_tok_struct*)_tmp244)->f1;_LL1B7: yyzzz=
_tmp245;goto _LL1B5;_LL1B8:;_LL1B9:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B5:;}return yyzzz;}static char _tmp247[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp247,
_tmp247,_tmp247 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp248=yy1;struct Cyc_List_List*_tmp249;_LL1BB:
if(*((void**)_tmp248)!= Cyc_Rgnorder_tok)goto _LL1BD;_tmp249=((struct Cyc_Rgnorder_tok_struct*)
_tmp248)->f1;_LL1BC: yyzzz=_tmp249;goto _LL1BA;_LL1BD:;_LL1BE:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1BA:;}return yyzzz;}static char _tmp24B[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp24B,
_tmp24B,_tmp24B + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp24C=yy1;struct Cyc_Absyn_Conref*_tmp24D;_LL1C0: if(*((void**)
_tmp24C)!= Cyc_YY2)goto _LL1C2;_tmp24D=((struct Cyc_YY2_struct*)_tmp24C)->f1;
_LL1C1: yyzzz=_tmp24D;goto _LL1BF;_LL1C2:;_LL1C3:(int)_throw((void*)& Cyc_yyfail_YY2);
_LL1BF:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp24E;_tmp24E.timestamp=0;_tmp24E.first_line=0;
_tmp24E.first_column=0;_tmp24E.last_line=0;_tmp24E.last_column=0;_tmp24E;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[359]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,130,2,2,112,128,124,2,
108,109,118,122,110,123,115,127,2,2,2,2,2,2,2,2,2,2,114,104,113,107,121,120,119,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,116,2,117,126,111,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,105,125,106,129,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp24F[
2]="$";static char _tmp250[6]="error";static char _tmp251[12]="$undefined.";static
char _tmp252[5]="AUTO";static char _tmp253[9]="REGISTER";static char _tmp254[7]="STATIC";
static char _tmp255[7]="EXTERN";static char _tmp256[8]="TYPEDEF";static char _tmp257[5]="VOID";
static char _tmp258[5]="CHAR";static char _tmp259[6]="SHORT";static char _tmp25A[4]="INT";
static char _tmp25B[5]="LONG";static char _tmp25C[6]="FLOAT";static char _tmp25D[7]="DOUBLE";
static char _tmp25E[7]="SIGNED";static char _tmp25F[9]="UNSIGNED";static char _tmp260[
6]="CONST";static char _tmp261[9]="VOLATILE";static char _tmp262[9]="RESTRICT";
static char _tmp263[7]="STRUCT";static char _tmp264[6]="UNION";static char _tmp265[5]="CASE";
static char _tmp266[8]="DEFAULT";static char _tmp267[7]="INLINE";static char _tmp268[7]="SIZEOF";
static char _tmp269[9]="OFFSETOF";static char _tmp26A[3]="IF";static char _tmp26B[5]="ELSE";
static char _tmp26C[7]="SWITCH";static char _tmp26D[6]="WHILE";static char _tmp26E[3]="DO";
static char _tmp26F[4]="FOR";static char _tmp270[5]="GOTO";static char _tmp271[9]="CONTINUE";
static char _tmp272[6]="BREAK";static char _tmp273[7]="RETURN";static char _tmp274[5]="ENUM";
static char _tmp275[8]="NULL_kw";static char _tmp276[4]="LET";static char _tmp277[6]="THROW";
static char _tmp278[4]="TRY";static char _tmp279[6]="CATCH";static char _tmp27A[4]="NEW";
static char _tmp27B[9]="ABSTRACT";static char _tmp27C[9]="FALLTHRU";static char
_tmp27D[6]="USING";static char _tmp27E[10]="NAMESPACE";static char _tmp27F[7]="TUNION";
static char _tmp280[8]="XTUNION";static char _tmp281[9]="FORARRAY";static char _tmp282[
5]="FILL";static char _tmp283[8]="CODEGEN";static char _tmp284[4]="CUT";static char
_tmp285[7]="SPLICE";static char _tmp286[7]="MALLOC";static char _tmp287[8]="RMALLOC";
static char _tmp288[7]="CALLOC";static char _tmp289[8]="RCALLOC";static char _tmp28A[9]="REGION_T";
static char _tmp28B[9]="SIZEOF_T";static char _tmp28C[6]="TAG_T";static char _tmp28D[7]="REGION";
static char _tmp28E[5]="RNEW";static char _tmp28F[8]="REGIONS";static char _tmp290[13]="RESET_REGION";
static char _tmp291[4]="GEN";static char _tmp292[14]="NOZEROTERM_kw";static char
_tmp293[12]="ZEROTERM_kw";static char _tmp294[7]="PTR_OP";static char _tmp295[7]="INC_OP";
static char _tmp296[7]="DEC_OP";static char _tmp297[8]="LEFT_OP";static char _tmp298[9]="RIGHT_OP";
static char _tmp299[6]="LE_OP";static char _tmp29A[6]="GE_OP";static char _tmp29B[6]="EQ_OP";
static char _tmp29C[6]="NE_OP";static char _tmp29D[7]="AND_OP";static char _tmp29E[6]="OR_OP";
static char _tmp29F[11]="MUL_ASSIGN";static char _tmp2A0[11]="DIV_ASSIGN";static char
_tmp2A1[11]="MOD_ASSIGN";static char _tmp2A2[11]="ADD_ASSIGN";static char _tmp2A3[11]="SUB_ASSIGN";
static char _tmp2A4[12]="LEFT_ASSIGN";static char _tmp2A5[13]="RIGHT_ASSIGN";static
char _tmp2A6[11]="AND_ASSIGN";static char _tmp2A7[11]="XOR_ASSIGN";static char
_tmp2A8[10]="OR_ASSIGN";static char _tmp2A9[9]="ELLIPSIS";static char _tmp2AA[11]="LEFT_RIGHT";
static char _tmp2AB[12]="COLON_COLON";static char _tmp2AC[11]="IDENTIFIER";static
char _tmp2AD[17]="INTEGER_CONSTANT";static char _tmp2AE[7]="STRING";static char
_tmp2AF[19]="CHARACTER_CONSTANT";static char _tmp2B0[18]="FLOATING_CONSTANT";
static char _tmp2B1[9]="TYPE_VAR";static char _tmp2B2[13]="TYPEDEF_NAME";static char
_tmp2B3[16]="QUAL_IDENTIFIER";static char _tmp2B4[18]="QUAL_TYPEDEF_NAME";static
char _tmp2B5[13]="TYPE_INTEGER";static char _tmp2B6[10]="ATTRIBUTE";static char
_tmp2B7[4]="';'";static char _tmp2B8[4]="'{'";static char _tmp2B9[4]="'}'";static
char _tmp2BA[4]="'='";static char _tmp2BB[4]="'('";static char _tmp2BC[4]="')'";
static char _tmp2BD[4]="','";static char _tmp2BE[4]="'_'";static char _tmp2BF[4]="'$'";
static char _tmp2C0[4]="'<'";static char _tmp2C1[4]="':'";static char _tmp2C2[4]="'.'";
static char _tmp2C3[4]="'['";static char _tmp2C4[4]="']'";static char _tmp2C5[4]="'*'";
static char _tmp2C6[4]="'@'";static char _tmp2C7[4]="'?'";static char _tmp2C8[4]="'>'";
static char _tmp2C9[4]="'+'";static char _tmp2CA[4]="'-'";static char _tmp2CB[4]="'&'";
static char _tmp2CC[4]="'|'";static char _tmp2CD[4]="'^'";static char _tmp2CE[4]="'/'";
static char _tmp2CF[4]="'%'";static char _tmp2D0[4]="'~'";static char _tmp2D1[4]="'!'";
static char _tmp2D2[5]="prog";static char _tmp2D3[17]="translation_unit";static char
_tmp2D4[21]="external_declaration";static char _tmp2D5[20]="function_definition";
static char _tmp2D6[21]="function_definition2";static char _tmp2D7[13]="using_action";
static char _tmp2D8[15]="unusing_action";static char _tmp2D9[17]="namespace_action";
static char _tmp2DA[19]="unnamespace_action";static char _tmp2DB[12]="declaration";
static char _tmp2DC[17]="declaration_list";static char _tmp2DD[23]="declaration_specifiers";
static char _tmp2DE[24]="storage_class_specifier";static char _tmp2DF[15]="attributes_opt";
static char _tmp2E0[11]="attributes";static char _tmp2E1[15]="attribute_list";static
char _tmp2E2[10]="attribute";static char _tmp2E3[15]="type_specifier";static char
_tmp2E4[5]="kind";static char _tmp2E5[15]="type_qualifier";static char _tmp2E6[15]="enum_specifier";
static char _tmp2E7[11]="enum_field";static char _tmp2E8[22]="enum_declaration_list";
static char _tmp2E9[26]="struct_or_union_specifier";static char _tmp2EA[16]="type_params_opt";
static char _tmp2EB[16]="struct_or_union";static char _tmp2EC[24]="struct_declaration_list";
static char _tmp2ED[25]="struct_declaration_list0";static char _tmp2EE[21]="init_declarator_list";
static char _tmp2EF[22]="init_declarator_list0";static char _tmp2F0[16]="init_declarator";
static char _tmp2F1[19]="struct_declaration";static char _tmp2F2[25]="specifier_qualifier_list";
static char _tmp2F3[23]="struct_declarator_list";static char _tmp2F4[24]="struct_declarator_list0";
static char _tmp2F5[18]="struct_declarator";static char _tmp2F6[17]="tunion_specifier";
static char _tmp2F7[18]="tunion_or_xtunion";static char _tmp2F8[17]="tunionfield_list";
static char _tmp2F9[18]="tunionfield_scope";static char _tmp2FA[12]="tunionfield";
static char _tmp2FB[11]="declarator";static char _tmp2FC[18]="direct_declarator";
static char _tmp2FD[18]="zeroterm_qual_opt";static char _tmp2FE[8]="pointer";static
char _tmp2FF[13]="pointer_char";static char _tmp300[8]="rgn_opt";static char _tmp301[
4]="rgn";static char _tmp302[20]="type_qualifier_list";static char _tmp303[20]="parameter_type_list";
static char _tmp304[9]="type_var";static char _tmp305[16]="optional_effect";static
char _tmp306[19]="optional_rgn_order";static char _tmp307[10]="rgn_order";static
char _tmp308[16]="optional_inject";static char _tmp309[11]="effect_set";static char
_tmp30A[14]="atomic_effect";static char _tmp30B[11]="region_set";static char _tmp30C[
15]="parameter_list";static char _tmp30D[22]="parameter_declaration";static char
_tmp30E[16]="identifier_list";static char _tmp30F[17]="identifier_list0";static
char _tmp310[12]="initializer";static char _tmp311[18]="array_initializer";static
char _tmp312[17]="initializer_list";static char _tmp313[12]="designation";static
char _tmp314[16]="designator_list";static char _tmp315[11]="designator";static char
_tmp316[10]="type_name";static char _tmp317[14]="any_type_name";static char _tmp318[
15]="type_name_list";static char _tmp319[20]="abstract_declarator";static char
_tmp31A[27]="direct_abstract_declarator";static char _tmp31B[10]="statement";
static char _tmp31C[18]="labeled_statement";static char _tmp31D[21]="expression_statement";
static char _tmp31E[19]="compound_statement";static char _tmp31F[16]="block_item_list";
static char _tmp320[20]="selection_statement";static char _tmp321[15]="switch_clauses";
static char _tmp322[16]="switchC_clauses";static char _tmp323[20]="iteration_statement";
static char _tmp324[15]="jump_statement";static char _tmp325[8]="pattern";static char
_tmp326[19]="tuple_pattern_list";static char _tmp327[14]="field_pattern";static
char _tmp328[19]="field_pattern_list";static char _tmp329[11]="expression";static
char _tmp32A[22]="assignment_expression";static char _tmp32B[20]="assignment_operator";
static char _tmp32C[23]="conditional_expression";static char _tmp32D[20]="constant_expression";
static char _tmp32E[22]="logical_or_expression";static char _tmp32F[23]="logical_and_expression";
static char _tmp330[24]="inclusive_or_expression";static char _tmp331[24]="exclusive_or_expression";
static char _tmp332[15]="and_expression";static char _tmp333[20]="equality_expression";
static char _tmp334[22]="relational_expression";static char _tmp335[17]="shift_expression";
static char _tmp336[20]="additive_expression";static char _tmp337[26]="multiplicative_expression";
static char _tmp338[16]="cast_expression";static char _tmp339[17]="unary_expression";
static char _tmp33A[15]="unary_operator";static char _tmp33B[19]="postfix_expression";
static char _tmp33C[19]="primary_expression";static char _tmp33D[25]="argument_expression_list";
static char _tmp33E[26]="argument_expression_list0";static char _tmp33F[9]="constant";
static char _tmp340[20]="qual_opt_identifier";static char _tmp341[17]="qual_opt_typedef";
static char _tmp342[18]="struct_union_name";static char _tmp343[11]="field_name";
static char _tmp344[12]="right_angle";static struct _tagged_arr Cyc_yytname[246]={{
_tmp24F,_tmp24F,_tmp24F + 2},{_tmp250,_tmp250,_tmp250 + 6},{_tmp251,_tmp251,
_tmp251 + 12},{_tmp252,_tmp252,_tmp252 + 5},{_tmp253,_tmp253,_tmp253 + 9},{_tmp254,
_tmp254,_tmp254 + 7},{_tmp255,_tmp255,_tmp255 + 7},{_tmp256,_tmp256,_tmp256 + 8},{
_tmp257,_tmp257,_tmp257 + 5},{_tmp258,_tmp258,_tmp258 + 5},{_tmp259,_tmp259,
_tmp259 + 6},{_tmp25A,_tmp25A,_tmp25A + 4},{_tmp25B,_tmp25B,_tmp25B + 5},{_tmp25C,
_tmp25C,_tmp25C + 6},{_tmp25D,_tmp25D,_tmp25D + 7},{_tmp25E,_tmp25E,_tmp25E + 7},{
_tmp25F,_tmp25F,_tmp25F + 9},{_tmp260,_tmp260,_tmp260 + 6},{_tmp261,_tmp261,
_tmp261 + 9},{_tmp262,_tmp262,_tmp262 + 9},{_tmp263,_tmp263,_tmp263 + 7},{_tmp264,
_tmp264,_tmp264 + 6},{_tmp265,_tmp265,_tmp265 + 5},{_tmp266,_tmp266,_tmp266 + 8},{
_tmp267,_tmp267,_tmp267 + 7},{_tmp268,_tmp268,_tmp268 + 7},{_tmp269,_tmp269,
_tmp269 + 9},{_tmp26A,_tmp26A,_tmp26A + 3},{_tmp26B,_tmp26B,_tmp26B + 5},{_tmp26C,
_tmp26C,_tmp26C + 7},{_tmp26D,_tmp26D,_tmp26D + 6},{_tmp26E,_tmp26E,_tmp26E + 3},{
_tmp26F,_tmp26F,_tmp26F + 4},{_tmp270,_tmp270,_tmp270 + 5},{_tmp271,_tmp271,
_tmp271 + 9},{_tmp272,_tmp272,_tmp272 + 6},{_tmp273,_tmp273,_tmp273 + 7},{_tmp274,
_tmp274,_tmp274 + 5},{_tmp275,_tmp275,_tmp275 + 8},{_tmp276,_tmp276,_tmp276 + 4},{
_tmp277,_tmp277,_tmp277 + 6},{_tmp278,_tmp278,_tmp278 + 4},{_tmp279,_tmp279,
_tmp279 + 6},{_tmp27A,_tmp27A,_tmp27A + 4},{_tmp27B,_tmp27B,_tmp27B + 9},{_tmp27C,
_tmp27C,_tmp27C + 9},{_tmp27D,_tmp27D,_tmp27D + 6},{_tmp27E,_tmp27E,_tmp27E + 10},{
_tmp27F,_tmp27F,_tmp27F + 7},{_tmp280,_tmp280,_tmp280 + 8},{_tmp281,_tmp281,
_tmp281 + 9},{_tmp282,_tmp282,_tmp282 + 5},{_tmp283,_tmp283,_tmp283 + 8},{_tmp284,
_tmp284,_tmp284 + 4},{_tmp285,_tmp285,_tmp285 + 7},{_tmp286,_tmp286,_tmp286 + 7},{
_tmp287,_tmp287,_tmp287 + 8},{_tmp288,_tmp288,_tmp288 + 7},{_tmp289,_tmp289,
_tmp289 + 8},{_tmp28A,_tmp28A,_tmp28A + 9},{_tmp28B,_tmp28B,_tmp28B + 9},{_tmp28C,
_tmp28C,_tmp28C + 6},{_tmp28D,_tmp28D,_tmp28D + 7},{_tmp28E,_tmp28E,_tmp28E + 5},{
_tmp28F,_tmp28F,_tmp28F + 8},{_tmp290,_tmp290,_tmp290 + 13},{_tmp291,_tmp291,
_tmp291 + 4},{_tmp292,_tmp292,_tmp292 + 14},{_tmp293,_tmp293,_tmp293 + 12},{_tmp294,
_tmp294,_tmp294 + 7},{_tmp295,_tmp295,_tmp295 + 7},{_tmp296,_tmp296,_tmp296 + 7},{
_tmp297,_tmp297,_tmp297 + 8},{_tmp298,_tmp298,_tmp298 + 9},{_tmp299,_tmp299,
_tmp299 + 6},{_tmp29A,_tmp29A,_tmp29A + 6},{_tmp29B,_tmp29B,_tmp29B + 6},{_tmp29C,
_tmp29C,_tmp29C + 6},{_tmp29D,_tmp29D,_tmp29D + 7},{_tmp29E,_tmp29E,_tmp29E + 6},{
_tmp29F,_tmp29F,_tmp29F + 11},{_tmp2A0,_tmp2A0,_tmp2A0 + 11},{_tmp2A1,_tmp2A1,
_tmp2A1 + 11},{_tmp2A2,_tmp2A2,_tmp2A2 + 11},{_tmp2A3,_tmp2A3,_tmp2A3 + 11},{
_tmp2A4,_tmp2A4,_tmp2A4 + 12},{_tmp2A5,_tmp2A5,_tmp2A5 + 13},{_tmp2A6,_tmp2A6,
_tmp2A6 + 11},{_tmp2A7,_tmp2A7,_tmp2A7 + 11},{_tmp2A8,_tmp2A8,_tmp2A8 + 10},{
_tmp2A9,_tmp2A9,_tmp2A9 + 9},{_tmp2AA,_tmp2AA,_tmp2AA + 11},{_tmp2AB,_tmp2AB,
_tmp2AB + 12},{_tmp2AC,_tmp2AC,_tmp2AC + 11},{_tmp2AD,_tmp2AD,_tmp2AD + 17},{
_tmp2AE,_tmp2AE,_tmp2AE + 7},{_tmp2AF,_tmp2AF,_tmp2AF + 19},{_tmp2B0,_tmp2B0,
_tmp2B0 + 18},{_tmp2B1,_tmp2B1,_tmp2B1 + 9},{_tmp2B2,_tmp2B2,_tmp2B2 + 13},{_tmp2B3,
_tmp2B3,_tmp2B3 + 16},{_tmp2B4,_tmp2B4,_tmp2B4 + 18},{_tmp2B5,_tmp2B5,_tmp2B5 + 13},{
_tmp2B6,_tmp2B6,_tmp2B6 + 10},{_tmp2B7,_tmp2B7,_tmp2B7 + 4},{_tmp2B8,_tmp2B8,
_tmp2B8 + 4},{_tmp2B9,_tmp2B9,_tmp2B9 + 4},{_tmp2BA,_tmp2BA,_tmp2BA + 4},{_tmp2BB,
_tmp2BB,_tmp2BB + 4},{_tmp2BC,_tmp2BC,_tmp2BC + 4},{_tmp2BD,_tmp2BD,_tmp2BD + 4},{
_tmp2BE,_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,_tmp2C0,
_tmp2C0 + 4},{_tmp2C1,_tmp2C1,_tmp2C1 + 4},{_tmp2C2,_tmp2C2,_tmp2C2 + 4},{_tmp2C3,
_tmp2C3,_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 4},{_tmp2C5,_tmp2C5,_tmp2C5 + 4},{
_tmp2C6,_tmp2C6,_tmp2C6 + 4},{_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,_tmp2C8,
_tmp2C8 + 4},{_tmp2C9,_tmp2C9,_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{_tmp2CB,
_tmp2CB,_tmp2CB + 4},{_tmp2CC,_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD,_tmp2CD + 4},{
_tmp2CE,_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF,_tmp2CF + 4},{_tmp2D0,_tmp2D0,
_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 4},{_tmp2D2,_tmp2D2,_tmp2D2 + 5},{_tmp2D3,
_tmp2D3,_tmp2D3 + 17},{_tmp2D4,_tmp2D4,_tmp2D4 + 21},{_tmp2D5,_tmp2D5,_tmp2D5 + 20},{
_tmp2D6,_tmp2D6,_tmp2D6 + 21},{_tmp2D7,_tmp2D7,_tmp2D7 + 13},{_tmp2D8,_tmp2D8,
_tmp2D8 + 15},{_tmp2D9,_tmp2D9,_tmp2D9 + 17},{_tmp2DA,_tmp2DA,_tmp2DA + 19},{
_tmp2DB,_tmp2DB,_tmp2DB + 12},{_tmp2DC,_tmp2DC,_tmp2DC + 17},{_tmp2DD,_tmp2DD,
_tmp2DD + 23},{_tmp2DE,_tmp2DE,_tmp2DE + 24},{_tmp2DF,_tmp2DF,_tmp2DF + 15},{
_tmp2E0,_tmp2E0,_tmp2E0 + 11},{_tmp2E1,_tmp2E1,_tmp2E1 + 15},{_tmp2E2,_tmp2E2,
_tmp2E2 + 10},{_tmp2E3,_tmp2E3,_tmp2E3 + 15},{_tmp2E4,_tmp2E4,_tmp2E4 + 5},{_tmp2E5,
_tmp2E5,_tmp2E5 + 15},{_tmp2E6,_tmp2E6,_tmp2E6 + 15},{_tmp2E7,_tmp2E7,_tmp2E7 + 11},{
_tmp2E8,_tmp2E8,_tmp2E8 + 22},{_tmp2E9,_tmp2E9,_tmp2E9 + 26},{_tmp2EA,_tmp2EA,
_tmp2EA + 16},{_tmp2EB,_tmp2EB,_tmp2EB + 16},{_tmp2EC,_tmp2EC,_tmp2EC + 24},{
_tmp2ED,_tmp2ED,_tmp2ED + 25},{_tmp2EE,_tmp2EE,_tmp2EE + 21},{_tmp2EF,_tmp2EF,
_tmp2EF + 22},{_tmp2F0,_tmp2F0,_tmp2F0 + 16},{_tmp2F1,_tmp2F1,_tmp2F1 + 19},{
_tmp2F2,_tmp2F2,_tmp2F2 + 25},{_tmp2F3,_tmp2F3,_tmp2F3 + 23},{_tmp2F4,_tmp2F4,
_tmp2F4 + 24},{_tmp2F5,_tmp2F5,_tmp2F5 + 18},{_tmp2F6,_tmp2F6,_tmp2F6 + 17},{
_tmp2F7,_tmp2F7,_tmp2F7 + 18},{_tmp2F8,_tmp2F8,_tmp2F8 + 17},{_tmp2F9,_tmp2F9,
_tmp2F9 + 18},{_tmp2FA,_tmp2FA,_tmp2FA + 12},{_tmp2FB,_tmp2FB,_tmp2FB + 11},{
_tmp2FC,_tmp2FC,_tmp2FC + 18},{_tmp2FD,_tmp2FD,_tmp2FD + 18},{_tmp2FE,_tmp2FE,
_tmp2FE + 8},{_tmp2FF,_tmp2FF,_tmp2FF + 13},{_tmp300,_tmp300,_tmp300 + 8},{_tmp301,
_tmp301,_tmp301 + 4},{_tmp302,_tmp302,_tmp302 + 20},{_tmp303,_tmp303,_tmp303 + 20},{
_tmp304,_tmp304,_tmp304 + 9},{_tmp305,_tmp305,_tmp305 + 16},{_tmp306,_tmp306,
_tmp306 + 19},{_tmp307,_tmp307,_tmp307 + 10},{_tmp308,_tmp308,_tmp308 + 16},{
_tmp309,_tmp309,_tmp309 + 11},{_tmp30A,_tmp30A,_tmp30A + 14},{_tmp30B,_tmp30B,
_tmp30B + 11},{_tmp30C,_tmp30C,_tmp30C + 15},{_tmp30D,_tmp30D,_tmp30D + 22},{
_tmp30E,_tmp30E,_tmp30E + 16},{_tmp30F,_tmp30F,_tmp30F + 17},{_tmp310,_tmp310,
_tmp310 + 12},{_tmp311,_tmp311,_tmp311 + 18},{_tmp312,_tmp312,_tmp312 + 17},{
_tmp313,_tmp313,_tmp313 + 12},{_tmp314,_tmp314,_tmp314 + 16},{_tmp315,_tmp315,
_tmp315 + 11},{_tmp316,_tmp316,_tmp316 + 10},{_tmp317,_tmp317,_tmp317 + 14},{
_tmp318,_tmp318,_tmp318 + 15},{_tmp319,_tmp319,_tmp319 + 20},{_tmp31A,_tmp31A,
_tmp31A + 27},{_tmp31B,_tmp31B,_tmp31B + 10},{_tmp31C,_tmp31C,_tmp31C + 18},{
_tmp31D,_tmp31D,_tmp31D + 21},{_tmp31E,_tmp31E,_tmp31E + 19},{_tmp31F,_tmp31F,
_tmp31F + 16},{_tmp320,_tmp320,_tmp320 + 20},{_tmp321,_tmp321,_tmp321 + 15},{
_tmp322,_tmp322,_tmp322 + 16},{_tmp323,_tmp323,_tmp323 + 20},{_tmp324,_tmp324,
_tmp324 + 15},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 19},{_tmp327,
_tmp327,_tmp327 + 14},{_tmp328,_tmp328,_tmp328 + 19},{_tmp329,_tmp329,_tmp329 + 11},{
_tmp32A,_tmp32A,_tmp32A + 22},{_tmp32B,_tmp32B,_tmp32B + 20},{_tmp32C,_tmp32C,
_tmp32C + 23},{_tmp32D,_tmp32D,_tmp32D + 20},{_tmp32E,_tmp32E,_tmp32E + 22},{
_tmp32F,_tmp32F,_tmp32F + 23},{_tmp330,_tmp330,_tmp330 + 24},{_tmp331,_tmp331,
_tmp331 + 24},{_tmp332,_tmp332,_tmp332 + 15},{_tmp333,_tmp333,_tmp333 + 20},{
_tmp334,_tmp334,_tmp334 + 22},{_tmp335,_tmp335,_tmp335 + 17},{_tmp336,_tmp336,
_tmp336 + 20},{_tmp337,_tmp337,_tmp337 + 26},{_tmp338,_tmp338,_tmp338 + 16},{
_tmp339,_tmp339,_tmp339 + 17},{_tmp33A,_tmp33A,_tmp33A + 15},{_tmp33B,_tmp33B,
_tmp33B + 19},{_tmp33C,_tmp33C,_tmp33C + 19},{_tmp33D,_tmp33D,_tmp33D + 25},{
_tmp33E,_tmp33E,_tmp33E + 26},{_tmp33F,_tmp33F,_tmp33F + 9},{_tmp340,_tmp340,
_tmp340 + 20},{_tmp341,_tmp341,_tmp341 + 17},{_tmp342,_tmp342,_tmp342 + 18},{
_tmp343,_tmp343,_tmp343 + 11},{_tmp344,_tmp344,_tmp344 + 12}};static short Cyc_yyr1[
419]={0,131,132,132,132,132,132,132,132,133,133,134,134,134,134,135,135,136,137,
138,139,140,140,140,140,141,141,142,142,142,142,142,142,142,142,142,142,143,143,
143,143,143,143,143,144,144,145,146,146,147,147,147,147,147,148,148,148,148,148,
148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,149,150,150,150,
151,151,151,152,152,153,153,154,154,154,155,155,156,156,157,158,158,159,160,160,
161,161,162,163,163,163,163,163,163,164,165,165,166,166,166,167,167,167,167,168,
168,169,169,169,169,170,170,170,171,171,172,172,173,173,173,173,173,173,173,173,
173,173,173,174,174,174,175,175,175,175,176,176,176,176,176,177,177,178,178,179,
179,180,180,180,180,181,181,182,182,183,183,184,184,185,185,186,186,187,187,187,
187,188,188,189,189,190,190,190,191,192,192,193,193,194,194,194,194,195,195,195,
195,196,197,197,198,198,199,199,200,200,200,200,200,201,201,202,202,202,203,203,
203,203,203,203,203,203,203,203,203,204,204,204,204,204,204,204,204,204,204,204,
204,204,205,206,206,207,207,208,208,208,208,208,208,209,209,209,209,209,210,210,
210,210,210,210,211,211,211,211,212,212,212,212,212,212,212,212,212,212,212,212,
212,212,212,213,213,213,213,213,213,213,213,214,214,214,214,214,214,214,214,214,
214,214,214,214,215,215,216,216,217,217,218,218,219,219,220,220,220,220,220,220,
220,220,220,220,220,221,221,221,221,221,221,221,222,223,223,224,224,225,225,226,
226,227,227,228,228,228,229,229,229,229,229,230,230,230,231,231,231,232,232,232,
232,233,233,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,234,235,
235,235,236,236,236,236,236,236,236,236,236,236,236,236,237,237,237,237,237,237,
237,237,237,238,239,239,240,240,240,240,241,241,242,242,243,243,244,244,245,245};
static short Cyc_yyr2[419]={0,1,2,3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,3,1,1,1,1,1,1,1,1,1,1,
1,1,2,4,4,4,1,4,1,1,1,1,5,2,4,1,3,1,3,4,8,3,0,3,1,1,1,1,2,1,1,3,1,3,3,1,2,1,2,1,
2,1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,5,4,5,4,4,2,2,1,0,1,1,3,4,
4,5,2,2,5,5,2,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,
1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,
2,1,1,1,1,1,1,6,3,9,6,5,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,0,3,4,5,
5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,1,3,1,2,
1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,
2,2,6,7,4,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[
860]={0,141,37,38,39,40,42,54,57,58,59,60,61,62,63,64,76,77,78,91,92,33,0,0,43,0,
0,118,119,0,0,0,409,164,411,410,412,73,0,0,55,0,142,142,142,1,0,9,0,0,10,0,27,35,
29,31,65,66,0,67,0,0,129,0,154,68,131,89,41,40,34,0,80,296,409,292,295,294,0,290,
0,0,0,0,0,187,0,297,17,19,0,0,0,0,0,0,0,0,144,143,0,149,0,150,153,2,0,0,0,0,21,0,
96,97,99,28,36,30,32,0,413,414,89,157,0,156,89,0,25,0,0,11,166,0,0,139,130,44,
155,140,0,69,0,41,84,0,82,0,0,0,302,293,301,24,0,0,89,0,0,0,106,102,104,205,207,
0,0,0,415,416,165,75,0,132,56,0,0,182,0,0,408,0,0,0,0,0,0,0,0,0,89,0,0,405,395,
406,407,0,0,0,0,380,0,378,379,0,311,324,332,334,336,338,340,342,345,350,353,356,
360,0,362,381,394,393,0,3,0,5,0,22,0,0,0,12,0,93,94,0,88,89,0,116,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,409,242,244,0,250,246,0,248,228,229,230,0,231,232,233,0,309,26,
13,99,172,188,0,0,168,166,0,212,0,142,0,145,45,0,0,0,81,0,0,291,303,0,189,0,0,0,
0,208,180,0,107,103,105,166,0,214,206,215,418,417,0,71,72,74,50,49,0,47,0,184,0,
186,70,0,0,370,0,326,360,0,327,328,0,0,0,0,0,0,0,0,0,363,364,0,0,0,0,366,367,365,
142,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,314,315,316,317,318,319,320,321,322,
323,313,0,368,0,387,388,0,0,0,397,0,89,142,18,0,20,0,98,100,191,190,14,86,95,0,0,
108,109,111,89,115,89,124,0,0,0,0,0,0,0,283,284,285,0,0,287,0,0,239,240,0,0,0,0,
0,0,251,247,99,249,245,243,0,173,0,0,0,179,167,174,135,0,0,0,168,137,0,138,133,
142,158,147,146,90,0,85,331,83,79,298,0,23,0,0,0,0,201,305,307,0,299,0,0,209,0,
168,0,142,0,216,166,0,0,227,211,0,0,0,183,0,0,0,192,196,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,396,403,0,402,151,333,0,335,337,339,341,343,344,348,349,346,347,351,352,354,
355,357,358,359,312,386,383,0,385,0,0,0,152,4,6,112,101,0,0,168,117,126,125,0,0,
120,0,0,0,0,0,0,0,0,282,286,0,0,0,0,235,0,0,0,241,0,15,310,166,0,176,0,0,169,0,
136,172,160,213,134,159,148,7,304,204,0,306,200,202,300,0,210,181,223,0,217,218,
142,0,168,0,142,0,0,0,0,46,48,369,0,0,193,0,197,391,392,0,374,0,0,0,0,0,0,0,0,
361,399,0,0,384,382,0,0,110,113,0,114,127,124,124,0,0,0,0,0,0,0,0,0,0,257,288,0,
0,0,0,0,16,168,0,177,175,0,168,0,203,308,222,220,225,0,226,219,142,0,51,52,0,0,0,
194,198,0,0,0,0,329,330,373,401,0,404,325,398,400,0,0,0,123,122,252,0,257,267,0,
0,0,0,0,0,0,0,0,0,0,0,289,0,0,0,0,238,162,178,170,161,166,224,221,0,372,371,0,
199,375,0,0,389,0,87,0,0,263,0,0,269,0,0,0,277,0,0,0,0,0,0,0,0,256,0,234,237,0,0,
168,0,0,0,0,390,128,253,0,0,0,254,268,270,271,0,279,278,0,273,0,0,0,0,257,258,0,
0,171,163,0,0,0,0,0,0,255,272,280,274,275,0,0,257,259,0,0,53,195,376,0,263,264,
276,257,260,281,236,0,263,265,257,261,377,266,262,0,0,0};static short Cyc_yydefgoto[
115]={857,45,46,47,262,48,402,49,404,50,232,51,52,289,53,324,325,54,170,55,56,
144,145,57,141,58,234,235,111,112,113,236,163,413,414,415,59,60,569,570,571,61,
62,101,63,64,137,124,470,492,65,493,460,599,452,456,457,307,283,177,84,85,511,
407,512,513,483,484,164,285,286,494,315,265,266,267,268,269,270,742,801,271,272,
298,299,486,487,273,274,389,206,475,207,208,209,210,211,212,213,214,215,216,217,
218,219,220,221,528,529,222,223,67,122,171,319};static short Cyc_yypact[860]={2248,
- -32768,- -32768,- -32768,- -32768,- 65,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
2871,187,920,- -32768,179,- 47,- -32768,- -32768,- 23,48,63,- -32768,- 19,- -32768,-
-32768,- -32768,- -32768,65,280,101,135,14,122,369,- -32768,2127,- -32768,341,390,-
-32768,244,2871,2871,2871,2871,- -32768,- -32768,260,- -32768,163,2742,202,25,33,-
-32768,179,118,141,181,- -32768,179,153,- -32768,217,- -32768,- -32768,- -32768,940,-
-32768,178,208,215,940,210,207,205,24,- -32768,- -32768,5118,5118,5118,236,211,225,
236,5296,- -32768,- -32768,4615,- -32768,4615,- -32768,- -32768,- -32768,2127,2368,
2127,2368,- -32768,232,245,- -32768,2673,- -32768,- -32768,- -32768,- -32768,5296,-
-32768,- -32768,118,- -32768,179,- -32768,185,1494,- -32768,2742,244,- -32768,5061,
5118,3399,- -32768,202,267,- -32768,- -32768,5118,- -32768,2368,- -32768,272,302,315,
179,322,940,- -32768,- -32768,- -32768,- -32768,363,4615,118,940,358,- 35,5296,5296,
5296,324,- -32768,- 36,- 36,- 36,- -32768,- -32768,- -32768,- -32768,12,- -32768,- -32768,
552,396,- -32768,4843,371,- -32768,4615,4691,383,393,395,400,425,427,433,118,4919,
4919,- -32768,- -32768,- -32768,- -32768,1873,437,4995,4995,- -32768,4995,- -32768,-
-32768,441,- -32768,- 17,484,424,426,442,448,88,457,420,175,- -32768,778,4995,144,-
44,- -32768,464,466,- -32768,472,- -32768,480,- -32768,280,3475,2742,- -32768,483,
5296,- -32768,634,488,118,179,495,503,15,506,2959,523,547,540,551,3551,2959,167,
557,2959,2959,94,572,545,- -32768,- -32768,574,1622,1622,244,1622,- -32768,- -32768,
- -32768,580,- -32768,- -32768,- -32768,237,- -32768,- -32768,- -32768,599,601,- -32768,
2,603,596,246,607,595,- 1,369,604,177,- -32768,- 1,614,179,- -32768,4615,620,- -32768,
- -32768,446,- -32768,247,852,451,5118,- -32768,618,623,- -32768,- -32768,- -32768,
2555,3627,27,- -32768,230,- -32768,- -32768,2,- -32768,- -32768,- -32768,- -32768,622,
624,621,1094,- -32768,418,- -32768,- -32768,5296,1873,- -32768,5296,- -32768,- -32768,
3047,- -32768,656,4615,2488,4615,4615,4615,4615,4615,628,1873,- -32768,- -32768,
1622,629,455,4615,- -32768,- -32768,- -32768,369,4995,4615,4995,4995,4995,4995,4995,
4995,4995,4995,4995,4995,4995,4995,4995,4995,4995,4995,4995,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4615,-
-32768,236,- -32768,- -32768,3703,236,4615,- -32768,627,118,369,- -32768,2127,-
-32768,2127,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4615,633,631,
- -32768,632,118,- -32768,118,570,4615,637,4615,4615,719,1750,646,- -32768,- -32768,
- -32768,273,709,- -32768,3779,2811,- -32768,- -32768,2959,658,664,4615,2959,1996,-
-32768,- -32768,2673,- -32768,- -32768,- -32768,4615,- -32768,5296,650,151,- -32768,-
-32768,638,- -32768,2,652,5191,596,- -32768,5118,- -32768,- -32768,369,- -32768,-
-32768,177,- -32768,2127,- -32768,- -32768,- -32768,- -32768,- -32768,940,- -32768,236,
4615,940,105,- -32768,- -32768,- -32768,262,- -32768,58,661,- -32768,653,596,655,369,
648,230,5176,5118,3855,- -32768,- -32768,419,663,12,- -32768,665,657,683,- -32768,-
-32768,287,3475,471,668,280,474,669,670,674,486,5296,677,672,4767,- -32768,-
-32768,678,681,- -32768,484,344,424,426,442,448,88,88,457,457,457,457,420,420,175,
175,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,684,- -32768,60,5118,3311,-
-32768,- -32768,- -32768,- -32768,- -32768,634,4615,596,- -32768,- -32768,- -32768,686,
179,275,494,4615,507,510,687,3931,4007,291,- -32768,- -32768,691,693,692,696,-
-32768,682,689,513,- -32768,2742,- -32768,- -32768,700,5118,- -32768,701,2,- -32768,
690,- -32768,115,- -32768,595,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,697,
- -32768,- -32768,- -32768,- -32768,852,- -32768,- -32768,- -32768,699,- -32768,- -32768,
369,704,596,- 1,369,698,707,710,716,- -32768,- -32768,705,123,713,- -32768,3135,-
-32768,- -32768,- -32768,2742,- -32768,4615,793,4615,4691,718,705,721,3311,- -32768,
- -32768,4615,4615,- -32768,- -32768,- 1,353,- -32768,- -32768,5296,- -32768,720,44,570,
2959,515,727,2959,4615,4083,306,4159,316,4235,605,- -32768,729,4615,741,- 16,731,-
-32768,596,59,- -32768,- -32768,739,596,5296,- -32768,- -32768,- -32768,- -32768,-
-32768,730,- -32768,- -32768,369,746,- -32768,- -32768,733,734,4615,- -32768,- -32768,
3475,528,736,737,- -32768,656,- -32768,- -32768,355,- -32768,- -32768,- -32768,- -32768,
3311,742,5296,- -32768,- -32768,821,747,605,- -32768,538,2959,541,4311,2959,544,
4387,4463,317,940,754,750,- -32768,320,2959,2959,755,- -32768,- -32768,- -32768,760,
- -32768,700,- -32768,- -32768,761,- -32768,- -32768,361,- -32768,- -32768,5296,847,-
-32768,3223,- -32768,548,2959,651,767,770,- -32768,2959,2959,567,- -32768,2959,2959,
575,2959,581,4539,26,1622,- -32768,4615,- -32768,- -32768,756,2,596,784,4615,771,
773,- -32768,- -32768,- -32768,4615,765,776,- -32768,- -32768,- -32768,- -32768,2959,-
-32768,- -32768,2959,- -32768,2959,2959,587,4615,1215,- -32768,589,791,- -32768,-
-32768,782,380,783,5296,378,1622,- -32768,- -32768,- -32768,- -32768,- -32768,2959,
379,605,- -32768,2959,2959,- -32768,- -32768,- -32768,785,1366,- -32768,- -32768,1215,
- -32768,- -32768,- -32768,786,651,- -32768,605,- -32768,- -32768,- -32768,- -32768,887,
893,- -32768};static short Cyc_yypgoto[115]={- -32768,49,- -32768,555,- -32768,- -32768,
- -32768,- -32768,- -32768,- 60,- 59,- 21,- -32768,- -32768,- 49,392,- -32768,377,802,107,
- -32768,- -32768,- 119,- -32768,168,- -32768,238,- -32768,- -32768,- -32768,671,667,338,
- -32768,- -32768,340,- -32768,- -32768,35,- -32768,- -32768,0,- 39,- 33,- 61,- -32768,-
-32768,840,- -32768,- 124,- 7,- 117,- 437,116,303,309,- 314,- 374,- 94,- 315,779,- -32768,
- 225,- 165,- 534,- 293,- -32768,429,- 183,- 72,- 128,- 37,- 234,77,- -32768,- -32768,- 40,-
227,- -32768,- 674,- 572,- -32768,- -32768,- 18,751,294,- -32768,347,289,- -32768,- 174,-
385,- 160,556,558,554,559,553,339,240,343,345,- 141,803,- -32768,- -32768,- -32768,-
342,- -32768,- -32768,76,860,- -32768,- 327,- 122};static short Cyc_yytable[5409]={70,
128,129,176,502,86,406,335,281,482,103,104,291,135,352,282,506,338,165,166,167,
131,339,659,136,603,1,561,296,322,68,115,116,117,118,444,445,316,447,95,130,160,
160,160,320,321,89,397,160,567,568,114,553,125,128,770,620,125,355,356,148,357,
359,33,551,152,453,263,554,275,160,305,316,93,233,398,66,746,390,497,597,98,99,
160,160,317,318,135,290,276,90,160,584,130,497,105,611,747,72,87,33,88,313,360,
814,323,264,454,130,464,422,160,160,160,328,66,618,717,32,100,317,474,66,423,524,
35,314,66,662,156,277,33,157,39,120,311,126,594,329,66,815,835,139,312,123,600,
506,146,160,507,- 121,508,306,610,87,225,226,227,228,87,846,91,367,368,465,523,
462,617,750,471,450,853,275,94,473,327,92,657,856,661,318,318,66,66,66,66,160,
438,697,98,99,292,409,96,16,17,18,162,162,162,239,369,263,263,162,263,66,439,451,
370,440,130,613,391,392,393,168,704,66,280,480,481,169,146,- 166,87,162,102,469,-
166,277,140,489,87,547,548,549,416,474,162,162,264,264,97,264,1,142,162,749,33,
313,66,394,752,466,160,32,596,147,395,396,33,160,35,446,328,501,162,162,162,851,
433,32,455,123,434,143,160,855,35,32,1,160,160,485,160,149,35,639,136,238,263,71,
375,241,42,43,44,140,160,240,150,376,377,162,38,66,474,705,151,132,455,155,66,
153,133,419,154,134,172,516,- 188,425,482,302,530,95,- 188,432,168,264,436,437,38,
173,169,229,32,498,648,66,449,162,499,35,709,500,450,110,176,280,479,39,32,820,
230,461,450,347,34,35,36,42,43,44,119,578,558,615,146,38,626,616,32,624,585,753,
581,87,665,35,625,293,450,652,666,128,591,39,205,474,224,604,637,160,676,468,638,
42,43,44,450,66,160,66,130,592,418,294,607,734,162,160,708,130,160,450,66,162,1,
737,782,295,288,786,130,450,450,658,709,450,297,311,162,605,175,98,99,162,162,
312,162,42,43,44,106,107,306,501,160,160,559,455,560,450,162,300,237,655,721,609,
764,622,722,612,765,304,161,161,161,175,450,709,160,161,793,600,685,66,334,66,
719,713,306,760,690,839,714,450,450,450,340,842,845,108,109,161,708,308,309,310,
341,301,342,698,330,331,160,343,161,161,32,629,630,631,586,642,161,35,590,408,
608,- 214,686,365,366,326,- 214,- 214,371,372,275,- 214,344,312,345,720,161,161,161,
708,346,373,374,353,354,160,358,66,362,162,684,363,1,87,477,478,816,87,162,488,
478,361,416,526,450,364,557,162,399,130,162,400,237,161,567,568,606,401,794,640,
450,128,643,450,565,403,566,834,410,695,455,66,417,699,647,450,431,485,843,420,
496,233,667,450,162,162,539,540,541,542,421,161,160,424,850,669,450,852,670,450,
130,683,450,728,450,408,740,741,162,767,426,518,519,520,1,791,761,450,66,427,841,
160,527,428,32,664,771,450,175,773,450,35,777,450,429,- 185,797,331,442,326,- 185,
- 185,162,175,435,- 185,755,312,175,42,43,44,799,800,160,806,450,550,353,441,161,
443,527,809,450,448,514,161,517,811,450,87,521,451,353,832,450,836,450,725,726,
162,161,537,538,231,532,161,161,459,161,458,160,543,544,463,318,545,546,472,467,
783,527,263,161,476,32,490,491,503,505,527,504,35,359,522,562,525,593,556,563,39,
555,727,573,564,730,412,576,580,582,42,43,44,263,587,588,595,33,598,601,619,264,
621,623,263,635,572,162,574,575,632,579,634,160,636,641,650,644,645,175,263,455,
646,263,649,653,589,628,175,654,663,656,264,671,677,678,162,175,680,679,408,681,
280,264,682,687,694,772,651,689,776,696,692,700,87,701,711,702,161,264,787,788,
264,703,706,715,724,161,716,162,729,743,745,748,175,751,161,754,756,161,757,758,
762,798,408,763,766,768,804,805,769,789,807,808,785,810,378,379,380,381,382,383,
384,385,386,387,784,162,790,792,795,802,803,161,161,818,821,826,823,824,827,828,
837,388,829,858,830,831,73,838,840,859,849,854,515,633,174,161,723,405,411,660,
138,691,819,688,303,844,693,284,614,847,848,531,534,536,121,533,668,0,535,0,673,
675,0,408,0,0,0,162,0,161,0,712,0,0,0,0,408,0,0,718,0,32,75,0,76,77,0,0,35,0,0,0,
0,0,73,0,78,0,0,79,80,0,0,480,481,0,81,0,161,0,0,82,83,0,73,0,0,333,0,0,336,336,
0,0,0,0,0,710,0,0,349,350,0,0,408,0,237,0,336,336,0,336,0,0,0,0,0,408,0,74,75,0,
76,77,731,733,35,736,336,739,0,0,0,744,78,175,0,79,80,32,75,0,76,77,81,161,35,0,
0,82,83,0,0,0,78,0,0,79,80,759,408,0,0,0,81,0,0,0,175,82,83,0,0,0,161,0,0,0,0,0,
0,0,0,0,0,0,0,775,0,0,779,781,0,0,0,0,0,0,0,0,0,1,0,0,336,0,0,161,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,813,0,22,0,817,0,0,0,0,0,161,
822,0,27,28,0,0,825,0,0,0,0,0,0,29,30,31,0,0,0,0,0,833,336,0,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,0,0,0,278,0,0,32,0,0,0,0,33,34,
35,36,37,38,280,0,0,161,326,- 166,0,40,41,0,- 166,0,312,0,42,43,44,336,0,0,2,3,4,
69,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,740,741,21,178,179,242,0,243,244,245,
246,247,248,249,250,22,180,23,181,251,0,182,24,252,0,0,27,28,253,183,184,254,255,
185,186,187,188,29,30,31,256,189,0,257,190,0,0,336,191,192,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,258,193,194,195,196,33,34,35,36,37,38,259,127,0,0,197,0,0,40,
261,336,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,336,0,2,3,4,69,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,799,800,
21,178,179,242,0,243,244,245,246,247,248,249,250,22,180,23,181,251,0,182,24,252,
0,0,27,28,253,183,184,254,255,185,186,187,188,29,30,31,256,189,0,257,190,0,0,0,
191,192,0,0,0,0,0,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,336,258,193,194,195,196,33,34,
35,36,37,38,259,127,0,0,197,0,0,40,261,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,
203,204,2,3,4,69,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,242,0,
243,244,245,246,247,248,249,250,22,180,23,181,251,0,182,24,252,0,0,27,28,253,183,
184,254,255,185,186,187,188,29,30,31,256,189,0,257,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,193,194,195,196,33,34,35,36,37,38,259,127,260,
0,197,0,0,40,261,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,2,3,4,69,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,242,0,243,244,245,246,247,248,
249,250,22,180,23,181,251,0,182,24,252,0,0,27,28,253,183,184,254,255,185,186,187,
188,29,30,31,256,189,0,257,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,258,193,194,195,196,33,34,35,36,37,38,259,127,0,0,197,0,0,40,261,0,0,0,0,0,
199,0,0,0,200,201,202,0,0,0,0,203,204,2,3,4,69,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,180,23,181,0,0,182,24,0,0,0,27,28,
0,183,184,0,0,185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,577,0,0,0,197,0,
0,40,261,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,
28,0,183,184,0,0,185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,0,351,0,0,
197,0,0,40,261,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,
0,0,27,28,0,183,184,0,0,185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,0,0,0,
0,197,0,0,40,261,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,- 8,1,0,2,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,
0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,
40,41,0,0,0,0,0,42,43,44,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,
0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,
0,0,33,34,35,36,37,38,0,0,0,0,39,0,0,40,41,0,0,0,0,0,42,43,44,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,
0,0,0,0,42,43,44,1,0,2,3,4,69,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,
0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,32,0,0,0,
0,33,34,35,36,37,38,22,0,0,0,39,0,0,40,41,0,0,27,28,0,42,43,44,0,0,0,0,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,0,0,0,0,0,33,
34,0,36,37,38,280,0,0,0,311,0,0,40,41,0,0,0,312,0,42,43,44,2,3,4,69,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,
0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,69,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,127,22,231,23,0,
0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,69,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,127,22,
0,23,0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,4,69,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,33,34,0,36,37,38,
24,0,0,0,27,28,0,40,41,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41,178,179,
242,0,243,244,245,246,247,248,249,250,0,180,0,181,251,0,182,0,252,0,0,0,0,253,
183,184,254,255,185,186,187,188,0,0,0,256,189,0,257,190,0,0,0,191,192,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,193,194,195,196,0,0,35,0,0,0,259,127,0,0,197,
0,0,0,198,178,179,0,0,0,199,0,509,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,
0,0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,337,510,0,197,0,
0,0,198,178,179,480,481,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,
0,0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,337,707,0,197,0,
0,0,198,178,179,480,481,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,
0,0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,337,796,0,197,0,
0,0,198,178,179,480,481,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,
0,0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,337,0,0,197,0,0,
0,198,178,179,480,481,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,
0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,
0,198,0,180,0,181,287,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,
188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,193,194,195,196,0,0,35,178,179,0,0,337,0,0,197,0,0,0,198,0,180,0,181,0,199,
182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,430,0,0,0,197,0,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,
198,0,180,0,181,495,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,
0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,552,0,0,198,0,180,0,181,0,199,182,0,
0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,0,0,0,0,197,583,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,
204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,198,0,180,
0,181,627,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,
189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,
195,196,0,0,35,178,179,0,672,0,0,0,197,0,0,0,198,0,180,0,181,0,199,182,0,0,200,
201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,674,
0,0,0,197,0,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,732,0,0,198,0,180,0,
181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,
0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,
0,0,35,178,179,0,0,0,0,0,197,735,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,
0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,738,0,0,0,197,
0,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,
188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,774,0,0,198,0,180,0,181,0,199,
182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,0,0,0,197,778,0,0,198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,780,0,0,
198,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,
0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,
194,195,196,0,0,35,178,179,0,0,0,0,0,197,812,0,0,198,0,180,0,181,0,199,182,0,0,
200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,0,0,0,0,197,0,0,0,198,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,337,0,0,197,0,0,0,198,0,180,0,0,
0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,
0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,651,0,0,197,0,0,0,198,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,
203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,332,0,0,0,198,0,
180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,
0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,
0,0,35,178,179,0,0,0,0,0,348,0,0,0,198,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,22,0,0,0,0,197,0,
0,0,198,0,27,28,0,0,199,0,0,0,200,201,202,29,30,31,0,203,204,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,278,0,0,279,22,0,0,0,33,34,0,36,37,38,
280,27,28,0,0,0,0,40,41,0,0,0,29,30,31,0,0,158,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,33,34,0,36,37,38,0,159,
27,28,0,0,22,40,41,0,0,0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,
0,0,0,0,0,0,0,0,278,0,0,0,0,0,0,0,33,34,0,36,37,38,280,602,0,0,0,0,0,40,41,33,34,
0,36,37,38,0,0,0,0,0,0,0,40,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,
37,38,0,0,0,0,0,0,0,40,41};static short Cyc_yycheck[5409]={21,61,61,97,318,23,231,
181,132,302,43,44,140,62,197,132,331,182,90,91,92,61,182,557,63,462,1,412,147,17,
95,52,53,54,55,262,263,73,265,39,61,90,91,92,166,167,93,91,97,5,6,51,394,60,114,
729,493,64,199,200,78,202,79,98,391,83,64,127,395,129,119,106,73,92,114,119,0,93,
219,313,454,67,68,132,133,121,122,136,137,129,113,140,434,114,328,46,481,113,22,
23,98,25,163,120,78,93,127,105,129,110,95,160,161,162,175,39,490,651,93,105,121,
295,46,108,351,100,163,51,565,105,130,98,108,108,58,108,60,452,175,63,114,815,66,
116,111,459,461,71,197,332,106,334,159,480,78,106,107,108,109,83,834,113,74,75,
286,348,283,109,109,291,110,845,232,108,293,175,113,117,852,564,122,122,106,107,
108,109,235,93,625,67,68,142,232,92,17,18,19,90,91,92,124,113,262,263,97,265,130,
113,93,121,116,232,107,69,70,71,93,94,142,104,115,116,99,147,109,149,119,105,289,
114,230,113,304,157,375,376,377,237,412,132,133,262,263,108,265,1,105,140,685,98,
311,175,108,690,287,304,93,106,105,115,116,98,311,100,264,326,315,160,161,162,
842,104,93,280,111,108,95,326,850,100,93,1,331,332,302,334,108,100,513,328,122,
351,105,118,126,118,119,120,113,348,115,93,127,128,197,103,230,481,635,94,108,
318,107,237,104,113,240,110,116,108,341,104,245,616,156,358,326,110,251,93,351,
254,255,103,109,99,104,93,108,522,264,104,235,113,100,638,116,110,104,443,104,
104,108,93,791,110,110,110,190,99,100,101,118,119,120,105,426,400,106,293,103,
499,110,93,498,435,691,104,302,104,100,498,110,110,525,110,446,446,108,100,564,
102,464,106,443,104,289,110,118,119,120,110,326,452,328,426,446,239,106,470,104,
304,461,638,435,464,110,341,311,1,104,104,107,134,104,446,110,110,556,722,110,
109,108,326,467,97,67,68,331,332,116,334,118,119,120,104,105,454,497,498,499,402,
459,404,110,348,93,119,114,106,478,106,495,110,482,110,108,90,91,92,132,110,765,
522,97,114,790,594,402,108,404,655,647,490,709,602,106,647,110,110,110,108,114,
114,104,105,119,722,160,161,162,108,155,108,626,109,110,556,108,132,133,93,93,94,
95,438,516,140,100,442,231,472,104,595,76,77,108,109,110,72,73,591,114,108,116,
108,658,160,161,162,765,108,122,123,197,108,595,106,472,125,443,591,126,1,478,
109,110,784,482,452,109,110,78,563,109,110,124,399,461,105,591,464,106,235,197,5,
6,470,106,762,109,110,642,109,110,417,106,419,815,106,623,598,516,105,627,109,
110,250,616,826,105,312,642,109,110,498,499,367,368,369,370,108,235,662,108,842,
109,110,845,109,110,642,109,110,109,110,337,22,23,522,724,108,343,344,345,1,753,
109,110,563,93,824,691,354,104,93,570,109,110,311,109,110,100,109,110,104,104,
109,110,114,108,109,110,556,326,108,114,700,116,331,118,119,120,22,23,724,109,
110,389,332,108,304,108,394,109,110,106,340,311,342,109,110,616,346,93,348,109,
110,109,110,665,666,595,326,365,366,107,360,331,332,114,334,109,762,371,372,109,
122,373,374,106,117,740,434,784,348,106,93,110,106,108,110,443,109,100,79,108,
104,109,450,113,110,108,396,667,108,114,670,114,30,104,42,118,119,120,815,98,93,
108,98,122,109,109,784,109,117,826,110,421,662,423,424,109,426,109,824,93,109,
106,110,110,443,842,790,110,845,109,109,441,500,452,110,106,109,815,108,105,104,
691,461,104,109,513,121,104,826,117,106,109,732,105,121,735,109,117,117,740,110,
25,109,443,842,745,746,845,109,113,109,108,452,109,724,105,104,93,104,498,98,461,
109,94,464,109,109,108,768,557,110,106,28,773,774,105,98,777,778,106,780,80,81,
82,83,84,85,86,87,88,89,114,762,110,110,25,106,104,498,499,121,94,114,109,108,
106,806,93,107,809,0,811,812,38,109,109,0,109,109,341,505,96,522,662,230,235,563,
64,602,790,598,157,832,616,132,483,836,837,359,362,364,58,361,573,- 1,363,- 1,577,
578,- 1,638,- 1,- 1,- 1,824,- 1,556,- 1,646,- 1,- 1,- 1,- 1,651,- 1,- 1,654,- 1,93,94,- 1,96,
97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,38,- 1,108,- 1,- 1,111,112,- 1,- 1,115,116,- 1,118,- 1,595,
- 1,- 1,123,124,- 1,38,- 1,- 1,178,- 1,- 1,181,182,- 1,- 1,- 1,- 1,- 1,644,- 1,- 1,191,192,- 1,
- 1,709,- 1,662,- 1,199,200,- 1,202,- 1,- 1,- 1,- 1,- 1,722,- 1,93,94,- 1,96,97,671,672,100,
674,219,676,- 1,- 1,- 1,680,108,691,- 1,111,112,93,94,- 1,96,97,118,662,100,- 1,- 1,123,
124,- 1,- 1,- 1,108,- 1,- 1,111,112,706,765,- 1,- 1,- 1,118,- 1,- 1,- 1,724,123,124,- 1,- 1,-
1,691,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,734,- 1,- 1,737,738,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,1,- 1,- 1,295,- 1,- 1,724,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,782,- 1,37,- 1,786,- 1,- 1,- 1,- 1,- 1,762,793,- 1,48,49,- 1,-
1,799,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,814,359,- 1,361,362,363,364,365,
366,367,368,369,370,371,372,373,374,375,376,377,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,- 1,
98,99,100,101,102,103,104,- 1,- 1,824,108,109,- 1,111,112,- 1,114,- 1,116,- 1,118,119,
120,412,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,
54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,481,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,
105,- 1,- 1,108,- 1,- 1,111,112,525,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,
129,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,564,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,647,- 1,-
1,- 1,- 1,- 1,- 1,- 1,655,93,94,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,108,- 1,-
1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,
65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,
- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,
- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,
- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,- 1,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,
- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,
58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,
- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,0,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,
- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,
99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,
- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,
100,101,102,103,37,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,48,49,- 1,118,119,120,- 1,- 1,-
1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,
108,- 1,- 1,111,112,- 1,- 1,- 1,116,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,
44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,
- 1,101,102,103,- 1,105,37,107,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,- 1,39,-
1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,98,99,- 1,101,102,103,44,- 1,- 1,- 1,48,49,- 1,111,112,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,
112,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,
50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,-
1,104,105,- 1,- 1,108,- 1,- 1,- 1,112,25,26,- 1,- 1,- 1,118,- 1,32,- 1,122,123,124,- 1,38,-
1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,-
1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,
116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,
105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,
40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,-
1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,- 1,
- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,
130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,-
1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,
- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,
- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,
70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,
122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,
- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,
- 1,- 1,37,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,48,49,- 1,- 1,118,- 1,- 1,- 1,122,123,124,59,
60,61,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,90,- 1,- 1,93,37,- 1,- 1,- 1,98,99,- 1,101,102,103,104,48,49,- 1,- 1,- 1,- 1,111,
112,- 1,- 1,- 1,59,60,61,- 1,- 1,64,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,98,99,- 1,101,102,103,- 1,105,48,49,-
1,- 1,37,111,112,- 1,- 1,- 1,- 1,59,60,61,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,
102,103,104,90,- 1,- 1,- 1,- 1,- 1,111,112,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,
112};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _tagged_arr);
int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;
static int Cyc_yynerrs=0;struct _tuple16{struct _tagged_arr f1;void*f2;};static char
_tmp3D9[8]="stdcall";static char _tmp3DA[6]="cdecl";static char _tmp3DB[9]="fastcall";
static char _tmp3DC[9]="noreturn";static char _tmp3DD[6]="const";static char _tmp3DE[8]="aligned";
static char _tmp3DF[7]="packed";static char _tmp3E0[7]="shared";static char _tmp3E1[7]="unused";
static char _tmp3E2[5]="weak";static char _tmp3E3[10]="dllimport";static char _tmp3E4[
10]="dllexport";static char _tmp3E5[23]="no_instrument_function";static char _tmp3E6[
12]="constructor";static char _tmp3E7[11]="destructor";static char _tmp3E8[22]="no_check_memory_usage";
static char _tmp3E9[5]="pure";struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8FE=10000;short*_tmp8FF=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp8FE));{unsigned int _tmp900=
_tmp8FE;unsigned int i;for(i=0;i < _tmp900;i ++){_tmp8FF[i]=(short)0;}}_tmp8FF;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp8FB=10000;void**_tmp8FC=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp8FB));{unsigned int _tmp8FD=
_tmp8FB;unsigned int i;for(i=0;i < _tmp8FD;i ++){_tmp8FC[i]=Cyc_yylval;}}_tmp8FC;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp8F8=
10000;struct Cyc_Yyltype*_tmp8F9=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp8F8));{unsigned int _tmp8FA=_tmp8F8;
unsigned int i;for(i=0;i < _tmp8FA;i ++){_tmp8F9[i]=Cyc_yynewloc();}}_tmp8F9;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp345="parser stack overflow";_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(
_tmp345,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(860,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar <= 358: 0)?(
int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 246;}yyn +=
yychar1;if((yyn < 0?1: yyn > 5408)?1: Cyc_yycheck[_check_known_subscript_notnull(
5409,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5409,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 859)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(860,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(419,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1C4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1C5: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp346=
_cycalloc(sizeof(*_tmp346));_tmp346[0]=({struct Cyc_DeclList_tok_struct _tmp347;
_tmp347.tag=Cyc_DeclList_tok;_tmp347.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp347;});_tmp346;});
break;case 3: _LL1C6: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp348=
_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_DeclList_tok_struct _tmp349;
_tmp349.tag=Cyc_DeclList_tok;_tmp349.f1=({struct Cyc_List_List*_tmp34A=_cycalloc(
sizeof(*_tmp34A));_tmp34A->hd=({struct Cyc_Absyn_Decl*_tmp34B=_cycalloc(sizeof(*
_tmp34B));_tmp34B->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp34C=
_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Absyn_Using_d_struct _tmp34D;
_tmp34D.tag=9;_tmp34D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp34D.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp34D;});_tmp34C;}));
_tmp34B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp34B;});_tmp34A->tl=0;_tmp34A;});_tmp349;});
_tmp348;});Cyc_Lex_leave_using();break;case 4: _LL1C7: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_DeclList_tok_struct
_tmp34F;_tmp34F.tag=Cyc_DeclList_tok;_tmp34F.f1=({struct Cyc_List_List*_tmp350=
_cycalloc(sizeof(*_tmp350));_tmp350->hd=({struct Cyc_Absyn_Decl*_tmp351=_cycalloc(
sizeof(*_tmp351));_tmp351->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_Using_d_struct
_tmp353;_tmp353.tag=9;_tmp353.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp353.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp353;});_tmp352;}));
_tmp351->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp351;});_tmp350->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp350;});_tmp34F;});
_tmp34E;});break;case 5: _LL1C8: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_DeclList_tok_struct
_tmp355;_tmp355.tag=Cyc_DeclList_tok;_tmp355.f1=({struct Cyc_List_List*_tmp356=
_cycalloc(sizeof(*_tmp356));_tmp356->hd=({struct Cyc_Absyn_Decl*_tmp357=_cycalloc(
sizeof(*_tmp357));_tmp357->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp359;_tmp359.tag=8;_tmp359.f1=({struct _tagged_arr*_tmp35A=_cycalloc(sizeof(*
_tmp35A));_tmp35A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp35A;});_tmp359.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp359;});_tmp358;}));
_tmp357->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp357;});_tmp356->tl=0;_tmp356;});_tmp355;});
_tmp354;});Cyc_Lex_leave_namespace();break;case 6: _LL1C9: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_DeclList_tok_struct
_tmp35C;_tmp35C.tag=Cyc_DeclList_tok;_tmp35C.f1=({struct Cyc_List_List*_tmp35D=
_cycalloc(sizeof(*_tmp35D));_tmp35D->hd=({struct Cyc_Absyn_Decl*_tmp35E=_cycalloc(
sizeof(*_tmp35E));_tmp35E->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp360;_tmp360.tag=8;_tmp360.f1=({struct _tagged_arr*_tmp361=_cycalloc(sizeof(*
_tmp361));_tmp361[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp361;});_tmp360.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp360;});_tmp35F;}));
_tmp35E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp35E;});_tmp35D->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp35D;});_tmp35C;});
_tmp35B;});break;case 7: _LL1CA: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp362="C";
_tag_arr(_tmp362,sizeof(char),_get_zero_arr_size(_tmp362,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp363="only extern \"C\" { ... } is allowed";_tag_arr(_tmp363,
sizeof(char),_get_zero_arr_size(_tmp363,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({
struct Cyc_DeclList_tok_struct _tmp365;_tmp365.tag=Cyc_DeclList_tok;_tmp365.f1=({
struct Cyc_List_List*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->hd=({struct Cyc_Absyn_Decl*
_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp369;_tmp369.tag=10;_tmp369.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp369;});_tmp368;}));
_tmp367->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp367;});_tmp366->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp366;});_tmp365;});
_tmp364;});break;case 8: _LL1CB: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=({struct Cyc_DeclList_tok_struct
_tmp36B;_tmp36B.tag=Cyc_DeclList_tok;_tmp36B.f1=0;_tmp36B;});_tmp36A;});break;
case 9: _LL1CC: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp36C=_cycalloc(
sizeof(*_tmp36C));_tmp36C[0]=({struct Cyc_DeclList_tok_struct _tmp36D;_tmp36D.tag=
Cyc_DeclList_tok;_tmp36D.f1=({struct Cyc_List_List*_tmp36E=_cycalloc(sizeof(*
_tmp36E));_tmp36E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp370;_tmp370.tag=1;_tmp370.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp370;});_tmp36F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp36E->tl=0;
_tmp36E;});_tmp36D;});_tmp36C;});break;case 10: _LL1CD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1CE: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[
0]=({struct Cyc_FnDecl_tok_struct _tmp372;_tmp372.tag=Cyc_FnDecl_tok;_tmp372.f1=
Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp372;});
_tmp371;});break;case 12: _LL1CF: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373[0]=({struct Cyc_FnDecl_tok_struct
_tmp374;_tmp374.tag=Cyc_FnDecl_tok;_tmp374.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp375;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp374;});
_tmp373;});break;case 13: _LL1D0: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376[0]=({struct Cyc_FnDecl_tok_struct
_tmp377;_tmp377.tag=Cyc_FnDecl_tok;_tmp377.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp377;});
_tmp376;});break;case 14: _LL1D1: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_FnDecl_tok_struct
_tmp379;_tmp379.tag=Cyc_FnDecl_tok;_tmp379.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp37A;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp379;});
_tmp378;});break;case 15: _LL1D2: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_FnDecl_tok_struct
_tmp37C;_tmp37C.tag=Cyc_FnDecl_tok;_tmp37C.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp37D;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp37C;});
_tmp37B;});break;case 16: _LL1D3: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_FnDecl_tok_struct
_tmp37F;_tmp37F.tag=Cyc_FnDecl_tok;_tmp37F.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp380;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp37F;});
_tmp37E;});break;case 17: _LL1D4: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL1D5: Cyc_Lex_leave_using();
break;case 19: _LL1D6: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp381=
_cycalloc(sizeof(*_tmp381));_tmp381[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp381;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 20: _LL1D7: Cyc_Lex_leave_namespace();
break;case 21: _LL1D8: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp382=
_cycalloc(sizeof(*_tmp382));_tmp382[0]=({struct Cyc_DeclList_tok_struct _tmp383;
_tmp383.tag=Cyc_DeclList_tok;_tmp383.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp383;});
_tmp382;});break;case 22: _LL1D9: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384[0]=({struct Cyc_DeclList_tok_struct
_tmp385;_tmp385.tag=Cyc_DeclList_tok;_tmp385.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp385;});
_tmp384;});break;case 23: _LL1DA: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_DeclList_tok_struct
_tmp387;_tmp387.tag=Cyc_DeclList_tok;_tmp387.f1=({struct Cyc_List_List*_tmp388=
_cycalloc(sizeof(*_tmp388));_tmp388->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp388->tl=0;
_tmp388;});_tmp387;});_tmp386;});break;case 24: _LL1DB: {struct Cyc_List_List*
_tmp389=0;{struct Cyc_List_List*_tmp38A=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp38A != 0;
_tmp38A=_tmp38A->tl){struct _tagged_arr*_tmp38B=(struct _tagged_arr*)_tmp38A->hd;
struct _tuple1*_tmp38C=({struct _tuple1*_tmp38F=_cycalloc(sizeof(*_tmp38F));
_tmp38F->f1=Cyc_Absyn_rel_ns_null;_tmp38F->f2=_tmp38B;_tmp38F;});struct Cyc_Absyn_Vardecl*
_tmp38D=Cyc_Absyn_new_vardecl(_tmp38C,Cyc_Absyn_wildtyp(0),0);_tmp389=({struct
Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->hd=_tmp38D;_tmp38E->tl=
_tmp389;_tmp38E;});}}_tmp389=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp389);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp390=
_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_DeclList_tok_struct _tmp391;
_tmp391.tag=Cyc_DeclList_tok;_tmp391.f1=({struct Cyc_List_List*_tmp392=_cycalloc(
sizeof(*_tmp392));_tmp392->hd=Cyc_Absyn_letv_decl(_tmp389,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp392->tl=0;
_tmp392;});_tmp391;});_tmp390;});break;}case 25: _LL1DC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL1DD: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp393=_cycalloc(sizeof(*_tmp393));
_tmp393[0]=({struct Cyc_DeclList_tok_struct _tmp394;_tmp394.tag=Cyc_DeclList_tok;
_tmp394.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp394;});_tmp393;});
break;case 27: _LL1DE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp395=
_cycalloc(sizeof(*_tmp395));_tmp395[0]=({struct Cyc_DeclSpec_tok_struct _tmp396;
_tmp396.tag=Cyc_DeclSpec_tok;_tmp396.f1=({struct Cyc_Parse_Declaration_spec*
_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->sc=({struct Cyc_Core_Opt*_tmp398=
_cycalloc(sizeof(*_tmp398));_tmp398->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp398;});_tmp397->tq=Cyc_Absyn_empty_tqual();
_tmp397->type_specs=0;_tmp397->is_inline=0;_tmp397->attributes=0;_tmp397;});
_tmp396;});_tmp395;});break;case 28: _LL1DF: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp399[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp39A="Only one storage class is allowed in a declaration";
_tag_arr(_tmp39A,sizeof(char),_get_zero_arr_size(_tmp39A,51));}),_tag_arr(
_tmp399,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B[0]=({struct Cyc_DeclSpec_tok_struct
_tmp39C;_tmp39C.tag=Cyc_DeclSpec_tok;_tmp39C.f1=({struct Cyc_Parse_Declaration_spec*
_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->sc=({struct Cyc_Core_Opt*_tmp39E=
_cycalloc(sizeof(*_tmp39E));_tmp39E->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp39E;});_tmp39D->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp39D->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp39D->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp39D->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp39D;});_tmp39C;});_tmp39B;});break;case 29: _LL1E0: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3A0;_tmp3A0.tag=Cyc_DeclSpec_tok;_tmp3A0.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->sc=0;_tmp3A1->tq=Cyc_Absyn_empty_tqual();
_tmp3A1->type_specs=({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));
_tmp3A2->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A2->tl=0;_tmp3A2;});
_tmp3A1->is_inline=0;_tmp3A1->attributes=0;_tmp3A1;});_tmp3A0;});_tmp39F;});
break;case 30: _LL1E1: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3A3=
_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_DeclSpec_tok_struct _tmp3A4;
_tmp3A4.tag=Cyc_DeclSpec_tok;_tmp3A4.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3A5->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3A5->type_specs=({
struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A6->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3A6;});
_tmp3A5->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3A5->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3A5;});
_tmp3A4;});_tmp3A3;});break;case 31: _LL1E2: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3A8;_tmp3A8.tag=Cyc_DeclSpec_tok;_tmp3A8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->sc=0;_tmp3A9->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A9->type_specs=0;
_tmp3A9->is_inline=0;_tmp3A9->attributes=0;_tmp3A9;});_tmp3A8;});_tmp3A7;});
break;case 32: _LL1E3: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3AA=
_cycalloc(sizeof(*_tmp3AA));_tmp3AA[0]=({struct Cyc_DeclSpec_tok_struct _tmp3AB;
_tmp3AB.tag=Cyc_DeclSpec_tok;_tmp3AB.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3AC->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3AC->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3AC->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3AC->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3AC;});_tmp3AB;});_tmp3AA;});break;case 33: _LL1E4: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3AE;_tmp3AE.tag=Cyc_DeclSpec_tok;_tmp3AE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->sc=0;_tmp3AF->tq=Cyc_Absyn_empty_tqual();
_tmp3AF->type_specs=0;_tmp3AF->is_inline=1;_tmp3AF->attributes=0;_tmp3AF;});
_tmp3AE;});_tmp3AD;});break;case 34: _LL1E5: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B1;_tmp3B1.tag=Cyc_DeclSpec_tok;_tmp3B1.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3B2->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3B2->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3B2->is_inline=1;_tmp3B2->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3B2;});
_tmp3B1;});_tmp3B0;});break;case 35: _LL1E6: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B4;_tmp3B4.tag=Cyc_DeclSpec_tok;_tmp3B4.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->sc=0;_tmp3B5->tq=Cyc_Absyn_empty_tqual();
_tmp3B5->type_specs=0;_tmp3B5->is_inline=0;_tmp3B5->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B5;});_tmp3B4;});
_tmp3B3;});break;case 36: _LL1E7: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B7;_tmp3B7.tag=Cyc_DeclSpec_tok;_tmp3B7.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3B8->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3B8->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3B8->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3B8->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3B8;});
_tmp3B7;});_tmp3B6;});break;case 37: _LL1E8: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_StorageClass_tok_struct
_tmp3BA;_tmp3BA.tag=Cyc_StorageClass_tok;_tmp3BA.f1=(void*)((void*)4);_tmp3BA;});
_tmp3B9;});break;case 38: _LL1E9: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_StorageClass_tok_struct
_tmp3BC;_tmp3BC.tag=Cyc_StorageClass_tok;_tmp3BC.f1=(void*)((void*)5);_tmp3BC;});
_tmp3BB;});break;case 39: _LL1EA: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_StorageClass_tok_struct
_tmp3BE;_tmp3BE.tag=Cyc_StorageClass_tok;_tmp3BE.f1=(void*)((void*)3);_tmp3BE;});
_tmp3BD;});break;case 40: _LL1EB: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_StorageClass_tok_struct
_tmp3C0;_tmp3C0.tag=Cyc_StorageClass_tok;_tmp3C0.f1=(void*)((void*)1);_tmp3C0;});
_tmp3BF;});break;case 41: _LL1EC: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3C1="C";
_tag_arr(_tmp3C1,sizeof(char),_get_zero_arr_size(_tmp3C1,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3C2="only extern or extern \"C\" is allowed";_tag_arr(_tmp3C2,
sizeof(char),_get_zero_arr_size(_tmp3C2,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({
struct Cyc_StorageClass_tok_struct _tmp3C4;_tmp3C4.tag=Cyc_StorageClass_tok;
_tmp3C4.f1=(void*)((void*)2);_tmp3C4;});_tmp3C3;});break;case 42: _LL1ED: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));
_tmp3C5[0]=({struct Cyc_StorageClass_tok_struct _tmp3C6;_tmp3C6.tag=Cyc_StorageClass_tok;
_tmp3C6.f1=(void*)((void*)0);_tmp3C6;});_tmp3C5;});break;case 43: _LL1EE: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));
_tmp3C7[0]=({struct Cyc_StorageClass_tok_struct _tmp3C8;_tmp3C8.tag=Cyc_StorageClass_tok;
_tmp3C8.f1=(void*)((void*)6);_tmp3C8;});_tmp3C7;});break;case 44: _LL1EF: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));
_tmp3C9[0]=({struct Cyc_AttributeList_tok_struct _tmp3CA;_tmp3CA.tag=Cyc_AttributeList_tok;
_tmp3CA.f1=0;_tmp3CA;});_tmp3C9;});break;case 45: _LL1F0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 46: _LL1F1: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));
_tmp3CB[0]=({struct Cyc_AttributeList_tok_struct _tmp3CC;_tmp3CC.tag=Cyc_AttributeList_tok;
_tmp3CC.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp3CC;});_tmp3CB;});break;case 47: _LL1F2: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({
struct Cyc_AttributeList_tok_struct _tmp3CE;_tmp3CE.tag=Cyc_AttributeList_tok;
_tmp3CE.f1=({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CF->tl=0;_tmp3CF;});_tmp3CE;});_tmp3CD;});break;case 48:
_LL1F3: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3D0=_cycalloc(
sizeof(*_tmp3D0));_tmp3D0[0]=({struct Cyc_AttributeList_tok_struct _tmp3D1;_tmp3D1.tag=
Cyc_AttributeList_tok;_tmp3D1.f1=({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*
_tmp3D2));_tmp3D2->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3D2->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D2;});_tmp3D1;});
_tmp3D0;});break;case 49: _LL1F4: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple16 att_map[17]={{{_tmp3D9,_tmp3D9,_tmp3D9 + 8},(
void*)0},{{_tmp3DA,_tmp3DA,_tmp3DA + 6},(void*)1},{{_tmp3DB,_tmp3DB,_tmp3DB + 9},(
void*)2},{{_tmp3DC,_tmp3DC,_tmp3DC + 9},(void*)3},{{_tmp3DD,_tmp3DD,_tmp3DD + 6},(
void*)4},{{_tmp3DE,_tmp3DE,_tmp3DE + 8},(void*)& att_aligned},{{_tmp3DF,_tmp3DF,
_tmp3DF + 7},(void*)5},{{_tmp3E0,_tmp3E0,_tmp3E0 + 7},(void*)7},{{_tmp3E1,_tmp3E1,
_tmp3E1 + 7},(void*)8},{{_tmp3E2,_tmp3E2,_tmp3E2 + 5},(void*)9},{{_tmp3E3,_tmp3E3,
_tmp3E3 + 10},(void*)10},{{_tmp3E4,_tmp3E4,_tmp3E4 + 10},(void*)11},{{_tmp3E5,
_tmp3E5,_tmp3E5 + 23},(void*)12},{{_tmp3E6,_tmp3E6,_tmp3E6 + 12},(void*)13},{{
_tmp3E7,_tmp3E7,_tmp3E7 + 11},(void*)14},{{_tmp3E8,_tmp3E8,_tmp3E8 + 22},(void*)15},{{
_tmp3E9,_tmp3E9,_tmp3E9 + 5},(void*)16}};struct _tagged_arr _tmp3D3=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp3D3,sizeof(char))> 4?*((const char*)_check_unknown_subscript(_tmp3D3,sizeof(
char),0))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3D3,sizeof(char),1))
== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3D3,sizeof(char),(int)(
_get_arr_size(_tmp3D3,sizeof(char))- 2)))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp3D3,sizeof(char),(int)(_get_arr_size(_tmp3D3,
sizeof(char))- 3)))== '_': 0)_tmp3D3=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp3D3,2,_get_arr_size(_tmp3D3,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp3D3,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3D4=_cycalloc(sizeof(*
_tmp3D4));_tmp3D4[0]=({struct Cyc_Attribute_tok_struct _tmp3D5;_tmp3D5.tag=Cyc_Attribute_tok;
_tmp3D5.f1=(void*)(att_map[i]).f2;_tmp3D5;});_tmp3D4;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp3D6="unrecognized attribute";_tag_arr(_tmp3D6,sizeof(char),
_get_zero_arr_size(_tmp3D6,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({
struct Cyc_Attribute_tok_struct _tmp3D8;_tmp3D8.tag=Cyc_Attribute_tok;_tmp3D8.f1=(
void*)((void*)1);_tmp3D8;});_tmp3D7;});}break;}}case 50: _LL1F5: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({
struct Cyc_Attribute_tok_struct _tmp3EC;_tmp3EC.tag=Cyc_Attribute_tok;_tmp3EC.f1=(
void*)((void*)4);_tmp3EC;});_tmp3EB;});break;case 51: _LL1F6: {struct _tagged_arr
_tmp3ED=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp3EF;int _tmp3F0;struct _tuple13*_tmp3EE=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3EF=*_tmp3EE;
_tmp3F0=_tmp3EF.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({const char*
_tmp3F1="regparm";_tag_arr(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,8));}))
== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({const char*_tmp3F2="__regparm__";
_tag_arr(_tmp3F2,sizeof(char),_get_zero_arr_size(_tmp3F2,12));}))== 0){if(
_tmp3F0 < 0?1: _tmp3F0 > 3)Cyc_Parse_err(({const char*_tmp3F3="regparm requires value between 0 and 3";
_tag_arr(_tmp3F3,sizeof(char),_get_zero_arr_size(_tmp3F3,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3F4=_cycalloc_atomic(sizeof(*_tmp3F4));
_tmp3F4[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=
_tmp3F0;_tmp3F5;});_tmp3F4;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({
const char*_tmp3F6="aligned";_tag_arr(_tmp3F6,sizeof(char),_get_zero_arr_size(
_tmp3F6,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({const char*_tmp3F7="__aligned__";
_tag_arr(_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,12));}))== 0){if(
_tmp3F0 < 0)Cyc_Parse_err(({const char*_tmp3F8="aligned requires positive power of two";
_tag_arr(_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp3F0;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp3F9="aligned requires positive power of two";
_tag_arr(_tmp3F9,sizeof(char),_get_zero_arr_size(_tmp3F9,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3FA=_cycalloc_atomic(sizeof(*_tmp3FA));
_tmp3FA[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3FB;_tmp3FB.tag=1;_tmp3FB.f1=
_tmp3F0;_tmp3FB;});_tmp3FA;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({
const char*_tmp3FC="initializes";_tag_arr(_tmp3FC,sizeof(char),_get_zero_arr_size(
_tmp3FC,12));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3ED,({const char*
_tmp3FD="__initializes__";_tag_arr(_tmp3FD,sizeof(char),_get_zero_arr_size(
_tmp3FD,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp3FE=
_cycalloc_atomic(sizeof(*_tmp3FE));_tmp3FE[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp3FF;_tmp3FF.tag=4;_tmp3FF.f1=_tmp3F0;_tmp3FF;});_tmp3FE;});else{Cyc_Parse_err(({
const char*_tmp400="unrecognized attribute";_tag_arr(_tmp400,sizeof(char),
_get_zero_arr_size(_tmp400,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp401=_cycalloc(sizeof(*_tmp401));
_tmp401[0]=({struct Cyc_Attribute_tok_struct _tmp402;_tmp402.tag=Cyc_Attribute_tok;
_tmp402.f1=(void*)a;_tmp402;});_tmp401;});break;}}case 52: _LL1F7: {struct
_tagged_arr _tmp403=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp404=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp403,({const char*_tmp405="section";_tag_arr(_tmp405,sizeof(
char),_get_zero_arr_size(_tmp405,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)
_tmp403,({const char*_tmp406="__section__";_tag_arr(_tmp406,sizeof(char),
_get_zero_arr_size(_tmp406,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_Absyn_Section_att_struct
_tmp408;_tmp408.tag=2;_tmp408.f1=_tmp404;_tmp408;});_tmp407;});else{Cyc_Parse_err(({
const char*_tmp409="unrecognized attribute";_tag_arr(_tmp409,sizeof(char),
_get_zero_arr_size(_tmp409,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));
_tmp40A[0]=({struct Cyc_Attribute_tok_struct _tmp40B;_tmp40B.tag=Cyc_Attribute_tok;
_tmp40B.f1=(void*)a;_tmp40B;});_tmp40A;});break;}case 53: _LL1F8: {struct
_tagged_arr _tmp40C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp40D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp40F;struct
_tuple13 _tmp40E=*((struct _tuple13*)Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));_tmp40F=_tmp40E.f2;{int
_tmp411;struct _tuple13 _tmp410=*((struct _tuple13*)Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp411=_tmp410.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp40C,({const char*_tmp412="format";
_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,7));}))== 0?1: Cyc_zstrcmp((
struct _tagged_arr)_tmp40C,({const char*_tmp413="__format__";_tag_arr(_tmp413,
sizeof(char),_get_zero_arr_size(_tmp413,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp40D,({const char*_tmp414="printf";_tag_arr(_tmp414,sizeof(char),
_get_zero_arr_size(_tmp414,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_Absyn_Format_att_struct
_tmp416;_tmp416.tag=3;_tmp416.f1=(void*)((void*)0);_tmp416.f2=_tmp40F;_tmp416.f3=
_tmp411;_tmp416;});_tmp415;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp40D,({
const char*_tmp417="scanf";_tag_arr(_tmp417,sizeof(char),_get_zero_arr_size(
_tmp417,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp418=
_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Absyn_Format_att_struct
_tmp419;_tmp419.tag=3;_tmp419.f1=(void*)((void*)1);_tmp419.f2=_tmp40F;_tmp419.f3=
_tmp411;_tmp419;});_tmp418;});else{Cyc_Parse_err(({const char*_tmp41A="unrecognized format type";
_tag_arr(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp41B="unrecognized attribute";_tag_arr(_tmp41B,sizeof(char),
_get_zero_arr_size(_tmp41B,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C[0]=({
struct Cyc_Attribute_tok_struct _tmp41D;_tmp41D.tag=Cyc_Attribute_tok;_tmp41D.f1=(
void*)a;_tmp41D;});_tmp41C;});break;}}}case 54: _LL1F9: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp41F;_tmp41F.tag=Cyc_TypeSpecifier_tok;_tmp41F.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp41F;});_tmp41E;});break;case 55: _LL1FA: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp420=_cycalloc(sizeof(*_tmp420));
_tmp420[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp421;_tmp421.tag=Cyc_TypeSpecifier_tok;
_tmp421.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp421;});
_tmp420;});break;case 56: _LL1FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp423;_tmp423.tag=Cyc_TypeSpecifier_tok;_tmp423.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp424=_cycalloc(sizeof(*_tmp424));
_tmp424->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp424;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp423;});
_tmp422;});break;case 57: _LL1FC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp426;_tmp426.tag=Cyc_TypeSpecifier_tok;_tmp426.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp426;});
_tmp425;});break;case 58: _LL1FD: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp428;_tmp428.tag=Cyc_TypeSpecifier_tok;_tmp428.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Parse_Short_spec_struct
_tmp42A;_tmp42A.tag=2;_tmp42A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp42A;});
_tmp429;}));_tmp428;});_tmp427;});break;case 59: _LL1FE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp42C;_tmp42C.tag=Cyc_TypeSpecifier_tok;_tmp42C.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42C;});
_tmp42B;});break;case 60: _LL1FF: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp42E;_tmp42E.tag=Cyc_TypeSpecifier_tok;_tmp42E.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Parse_Long_spec_struct
_tmp430;_tmp430.tag=3;_tmp430.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp430;});
_tmp42F;}));_tmp42E;});_tmp42D;});break;case 61: _LL200: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp432;_tmp432.tag=Cyc_TypeSpecifier_tok;_tmp432.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp432;});
_tmp431;});break;case 62: _LL201: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp434;_tmp434.tag=Cyc_TypeSpecifier_tok;_tmp434.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp434;});
_tmp433;});break;case 63: _LL202: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp436;_tmp436.tag=Cyc_TypeSpecifier_tok;_tmp436.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp438;_tmp438.tag=0;_tmp438.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp438;});
_tmp437;}));_tmp436;});_tmp435;});break;case 64: _LL203: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43A;_tmp43A.tag=Cyc_TypeSpecifier_tok;_tmp43A.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp43C;_tmp43C.tag=1;_tmp43C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp43C;});
_tmp43B;}));_tmp43A;});_tmp439;});break;case 65: _LL204: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 66: _LL205: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 67: _LL206:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68:
_LL207: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp43D=_cycalloc(
sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp43E;_tmp43E.tag=
Cyc_TypeSpecifier_tok;_tmp43E.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43E;});
_tmp43D;});break;case 69: _LL208: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp440;_tmp440.tag=Cyc_TypeSpecifier_tok;_tmp440.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp441=_cycalloc(sizeof(*_tmp441));
_tmp441[0]=({struct Cyc_Absyn_TypedefType_struct _tmp442;_tmp442.tag=16;_tmp442.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp442.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp442.f3=0;_tmp442.f4=0;_tmp442;});_tmp441;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp440;});
_tmp43F;});break;case 70: _LL209: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp444;_tmp444.tag=Cyc_TypeSpecifier_tok;_tmp444.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp445=_cycalloc(sizeof(*_tmp445));
_tmp445[0]=({struct Cyc_Absyn_TupleType_struct _tmp446;_tmp446.tag=9;_tmp446.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp446;});_tmp445;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp444;});_tmp443;});break;case 71: _LL20A: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp447=_cycalloc(sizeof(*_tmp447));
_tmp447[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp448;_tmp448.tag=Cyc_TypeSpecifier_tok;
_tmp448.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp44A;_tmp44A.tag=15;_tmp44A.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp44A;});_tmp449;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp448;});_tmp447;});break;case 72: _LL20B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));
_tmp44B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp44C;_tmp44C.tag=Cyc_TypeSpecifier_tok;
_tmp44C.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp44E;_tmp44E.tag=14;_tmp44E.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp44E;});_tmp44D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp44C;});_tmp44B;});break;case 73: _LL20C: {struct
_tuple13 _tmp450;int _tmp451;struct _tuple13*_tmp44F=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp450=*_tmp44F;_tmp451=
_tmp450.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp452=_cycalloc(
sizeof(*_tmp452));_tmp452[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp453;_tmp453.tag=
Cyc_TypeSpecifier_tok;_tmp453.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp454=_cycalloc_atomic(sizeof(*_tmp454));_tmp454[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp455;_tmp455.tag=18;_tmp455.f1=_tmp451;_tmp455;});_tmp454;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp453;});
_tmp452;});break;}case 74: _LL20D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp457;_tmp457.tag=Cyc_TypeSpecifier_tok;_tmp457.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp458=_cycalloc(sizeof(*_tmp458));
_tmp458[0]=({struct Cyc_Absyn_TagType_struct _tmp459;_tmp459.tag=17;_tmp459.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp459;});_tmp458;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp457;});
_tmp456;});break;case 75: _LL20E: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp45A=
_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_Kind_tok_struct _tmp45B;
_tmp45B.tag=Cyc_Kind_tok;_tmp45B.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45B;});
_tmp45A;});break;case 76: _LL20F: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp45C=_cycalloc_atomic(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_TypeQual_tok_struct
_tmp45D;_tmp45D.tag=Cyc_TypeQual_tok;_tmp45D.f1=({struct Cyc_Absyn_Tqual _tmp45E;
_tmp45E.q_const=1;_tmp45E.q_volatile=0;_tmp45E.q_restrict=0;_tmp45E;});_tmp45D;});
_tmp45C;});break;case 77: _LL210: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp45F=_cycalloc_atomic(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_TypeQual_tok_struct
_tmp460;_tmp460.tag=Cyc_TypeQual_tok;_tmp460.f1=({struct Cyc_Absyn_Tqual _tmp461;
_tmp461.q_const=0;_tmp461.q_volatile=1;_tmp461.q_restrict=0;_tmp461;});_tmp460;});
_tmp45F;});break;case 78: _LL211: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp462=_cycalloc_atomic(sizeof(*_tmp462));_tmp462[0]=({struct Cyc_TypeQual_tok_struct
_tmp463;_tmp463.tag=Cyc_TypeQual_tok;_tmp463.f1=({struct Cyc_Absyn_Tqual _tmp464;
_tmp464.q_const=0;_tmp464.q_volatile=0;_tmp464.q_restrict=1;_tmp464;});_tmp463;});
_tmp462;});break;case 79: _LL212: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp466;_tmp466.tag=Cyc_TypeSpecifier_tok;_tmp466.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp468;_tmp468.tag=5;_tmp468.f1=({struct Cyc_Absyn_Decl*_tmp469=_cycalloc(
sizeof(*_tmp469));_tmp469->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp46B;_tmp46B.tag=6;_tmp46B.f1=({struct Cyc_Absyn_Enumdecl*_tmp46C=_cycalloc(
sizeof(*_tmp46C));_tmp46C->sc=(void*)((void*)2);_tmp46C->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp46C->fields=({
struct Cyc_Core_Opt*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46D;});_tmp46C;});
_tmp46B;});_tmp46A;}));_tmp469->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp469;});
_tmp468;});_tmp467;}));_tmp466;});_tmp465;});break;case 80: _LL213: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp46F;_tmp46F.tag=Cyc_TypeSpecifier_tok;
_tmp46F.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470[0]=({struct Cyc_Absyn_EnumType_struct
_tmp471;_tmp471.tag=12;_tmp471.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp471.f2=0;_tmp471;});
_tmp470;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46F;});_tmp46E;});break;case 81: _LL214: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp472=_cycalloc(sizeof(*_tmp472));
_tmp472[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp473;_tmp473.tag=Cyc_TypeSpecifier_tok;
_tmp473.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp474=_cycalloc(
sizeof(*_tmp474));_tmp474[0]=({struct Cyc_Parse_Type_spec_struct _tmp475;_tmp475.tag=
4;_tmp475.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp476=
_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp477;_tmp477.tag=13;_tmp477.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp477;});_tmp476;}));
_tmp475.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp475;});_tmp474;}));_tmp473;});_tmp472;});break;
case 82: _LL215: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp478=_cycalloc(
sizeof(*_tmp478));_tmp478[0]=({struct Cyc_Enumfield_tok_struct _tmp479;_tmp479.tag=
Cyc_Enumfield_tok;_tmp479.f1=({struct Cyc_Absyn_Enumfield*_tmp47A=_cycalloc(
sizeof(*_tmp47A));_tmp47A->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp47A->tag=0;_tmp47A->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp47A;});_tmp479;});_tmp478;});break;case 83: _LL216:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));
_tmp47B[0]=({struct Cyc_Enumfield_tok_struct _tmp47C;_tmp47C.tag=Cyc_Enumfield_tok;
_tmp47C.f1=({struct Cyc_Absyn_Enumfield*_tmp47D=_cycalloc(sizeof(*_tmp47D));
_tmp47D->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp47D->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp47D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp47D;});
_tmp47C;});_tmp47B;});break;case 84: _LL217: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp47F;_tmp47F.tag=Cyc_EnumfieldList_tok;_tmp47F.f1=({struct Cyc_List_List*
_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp480->tl=0;_tmp480;});
_tmp47F;});_tmp47E;});break;case 85: _LL218: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp482;_tmp482.tag=Cyc_EnumfieldList_tok;_tmp482.f1=({struct Cyc_List_List*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp483->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp483;});_tmp482;});
_tmp481;});break;case 86: _LL219: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp485;_tmp485.tag=Cyc_TypeSpecifier_tok;_tmp485.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp486=_cycalloc(sizeof(*_tmp486));
_tmp486[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp487;_tmp487.tag=11;_tmp487.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp487.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp487;});_tmp486;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp485;});_tmp484;});break;case 87: _LL21A: {struct
Cyc_List_List*_tmp488=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp489=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp48B;_tmp48B.tag=Cyc_TypeSpecifier_tok;
_tmp48B.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp48C=_cycalloc(
sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Parse_Decl_spec_struct _tmp48D;_tmp48D.tag=
5;_tmp48D.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp488,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp489,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48D;});
_tmp48C;}));_tmp48B;});_tmp48A;});break;}case 88: _LL21B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48F;_tmp48F.tag=Cyc_TypeSpecifier_tok;_tmp48F.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp490=_cycalloc(sizeof(*_tmp490));
_tmp490[0]=({struct Cyc_Absyn_AggrType_struct _tmp491;_tmp491.tag=10;_tmp491.f1=({
struct Cyc_Absyn_AggrInfo _tmp492;_tmp492.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp494;_tmp494.tag=0;_tmp494.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp494.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp494;});_tmp493;}));
_tmp492.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp492;});_tmp491;});_tmp490;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48F;});
_tmp48E;});break;case 89: _LL21C: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_TypeList_tok_struct
_tmp496;_tmp496.tag=Cyc_TypeList_tok;_tmp496.f1=0;_tmp496;});_tmp495;});break;
case 90: _LL21D: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp497=_cycalloc(
sizeof(*_tmp497));_tmp497[0]=({struct Cyc_TypeList_tok_struct _tmp498;_tmp498.tag=
Cyc_TypeList_tok;_tmp498.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp498;});_tmp497;});
break;case 91: _LL21E: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp499=
_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp49A;_tmp49A.tag=Cyc_StructOrUnion_tok;_tmp49A.f1=(void*)((void*)0);_tmp49A;});
_tmp499;});break;case 92: _LL21F: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp49C;_tmp49C.tag=Cyc_StructOrUnion_tok;_tmp49C.f1=(void*)((void*)1);_tmp49C;});
_tmp49B;});break;case 93: _LL220: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp49E;_tmp49E.tag=Cyc_AggrFieldDeclList_tok;_tmp49E.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp49E;});_tmp49D;});
break;case 94: _LL221: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_AggrFieldDeclListList_tok;_tmp4A0.f1=({struct Cyc_List_List*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A1->tl=0;_tmp4A1;});
_tmp4A0;});_tmp49F;});break;case 95: _LL222: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_AggrFieldDeclListList_tok;_tmp4A3.f1=({struct Cyc_List_List*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A4->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A4;});_tmp4A3;});
_tmp4A2;});break;case 96: _LL223: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4A6;_tmp4A6.tag=Cyc_InitDeclList_tok;_tmp4A6.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4A6;});_tmp4A5;});
break;case 97: _LL224: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4A7=
_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4A8;_tmp4A8.tag=Cyc_InitDeclList_tok;_tmp4A8.f1=({struct Cyc_List_List*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A9->tl=0;_tmp4A9;});
_tmp4A8;});_tmp4A7;});break;case 98: _LL225: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4AB;_tmp4AB.tag=Cyc_InitDeclList_tok;_tmp4AB.f1=({struct Cyc_List_List*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4AC->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4AC;});_tmp4AB;});
_tmp4AA;});break;case 99: _LL226: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_InitDecl_tok_struct
_tmp4AE;_tmp4AE.tag=Cyc_InitDecl_tok;_tmp4AE.f1=({struct _tuple12*_tmp4AF=
_cycalloc(sizeof(*_tmp4AF));_tmp4AF->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4AF->f2=0;_tmp4AF;});
_tmp4AE;});_tmp4AD;});break;case 100: _LL227: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_InitDecl_tok_struct
_tmp4B1;_tmp4B1.tag=Cyc_InitDecl_tok;_tmp4B1.f1=({struct _tuple12*_tmp4B2=
_cycalloc(sizeof(*_tmp4B2));_tmp4B2->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B2->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4B2;});_tmp4B1;});_tmp4B0;});break;case 101: _LL228: {struct _tuple15 _tmp4B4;
struct Cyc_Absyn_Tqual _tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_List_List*
_tmp4B7;struct _tuple15*_tmp4B3=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B4=*_tmp4B3;_tmp4B5=
_tmp4B4.f1;_tmp4B6=_tmp4B4.f2;_tmp4B7=_tmp4B4.f3;{struct Cyc_List_List*_tmp4B9;
struct Cyc_List_List*_tmp4BA;struct _tuple0 _tmp4B8=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4B9=_tmp4B8.f1;
_tmp4BA=_tmp4B8.f2;{void*_tmp4BB=Cyc_Parse_speclist2typ(_tmp4B6,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4B5,_tmp4BB,_tmp4B9,_tmp4B7),_tmp4BA);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4BD=_cycalloc(sizeof(*
_tmp4BD));_tmp4BD[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp4BE;_tmp4BE.tag=
Cyc_AggrFieldDeclList_tok;_tmp4BE.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4BC);
_tmp4BE;});_tmp4BD;});break;}}}case 102: _LL229: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4C0;_tmp4C0.tag=Cyc_QualSpecList_tok;_tmp4C0.f1=({struct _tuple15*_tmp4C1=
_cycalloc(sizeof(*_tmp4C1));_tmp4C1->f1=Cyc_Absyn_empty_tqual();_tmp4C1->f2=({
struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C2->tl=0;_tmp4C2;});
_tmp4C1->f3=0;_tmp4C1;});_tmp4C0;});_tmp4BF;});break;case 103: _LL22A: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({
struct Cyc_QualSpecList_tok_struct _tmp4C4;_tmp4C4.tag=Cyc_QualSpecList_tok;
_tmp4C4.f1=({struct _tuple15*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->f1=(*((
struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))).f1;_tmp4C5->f2=({struct Cyc_List_List*_tmp4C6=_cycalloc(
sizeof(*_tmp4C6));_tmp4C6->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C6->tl=(*((struct
_tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f2;_tmp4C6;});_tmp4C5->f3=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f3;_tmp4C5;});
_tmp4C4;});_tmp4C3;});break;case 104: _LL22B: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4C8;_tmp4C8.tag=Cyc_QualSpecList_tok;_tmp4C8.f1=({struct _tuple15*_tmp4C9=
_cycalloc(sizeof(*_tmp4C9));_tmp4C9->f1=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C9->f2=0;_tmp4C9->f3=0;
_tmp4C9;});_tmp4C8;});_tmp4C7;});break;case 105: _LL22C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4CB;_tmp4CB.tag=Cyc_QualSpecList_tok;_tmp4CB.f1=({struct _tuple15*_tmp4CC=
_cycalloc(sizeof(*_tmp4CC));_tmp4CC->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*((struct _tuple15*)
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f1);_tmp4CC->f2=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f2;_tmp4CC->f3=(*((
struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))).f3;_tmp4CC;});_tmp4CB;});_tmp4CA;});break;case 106: _LL22D:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp4CD=_cycalloc(sizeof(*
_tmp4CD));_tmp4CD[0]=({struct Cyc_QualSpecList_tok_struct _tmp4CE;_tmp4CE.tag=Cyc_QualSpecList_tok;
_tmp4CE.f1=({struct _tuple15*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->f1=Cyc_Absyn_empty_tqual();
_tmp4CF->f2=0;_tmp4CF->f3=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CF;});_tmp4CE;});
_tmp4CD;});break;case 107: _LL22E: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4D1;_tmp4D1.tag=Cyc_QualSpecList_tok;_tmp4D1.f1=({struct _tuple15*_tmp4D2=
_cycalloc(sizeof(*_tmp4D2));_tmp4D2->f1=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f1;_tmp4D2->f2=(*((
struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))).f2;_tmp4D2->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*((struct _tuple15*)
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f3);_tmp4D2;});_tmp4D1;});_tmp4D0;});break;case 108: _LL22F:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4D3=_cycalloc(sizeof(*
_tmp4D3));_tmp4D3[0]=({struct Cyc_InitDeclList_tok_struct _tmp4D4;_tmp4D4.tag=Cyc_InitDeclList_tok;
_tmp4D4.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp4D4;});_tmp4D3;});break;case 109: _LL230: yyval=(void*)({
struct Cyc_InitDeclList_tok_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({
struct Cyc_InitDeclList_tok_struct _tmp4D6;_tmp4D6.tag=Cyc_InitDeclList_tok;
_tmp4D6.f1=({struct Cyc_List_List*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7->hd=
Cyc_yyget_InitDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4D7->tl=0;_tmp4D7;});_tmp4D6;});_tmp4D5;});break;case 110: _LL231: yyval=(void*)({
struct Cyc_InitDeclList_tok_struct*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8[0]=({
struct Cyc_InitDeclList_tok_struct _tmp4D9;_tmp4D9.tag=Cyc_InitDeclList_tok;
_tmp4D9.f1=({struct Cyc_List_List*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->hd=
Cyc_yyget_InitDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4DA->tl=Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4DA;});_tmp4D9;});_tmp4D8;});break;case 111: _LL232: yyval=(
void*)({struct Cyc_InitDecl_tok_struct*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));
_tmp4DB[0]=({struct Cyc_InitDecl_tok_struct _tmp4DC;_tmp4DC.tag=Cyc_InitDecl_tok;
_tmp4DC.f1=({struct _tuple12*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4DD->f2=0;_tmp4DD;});
_tmp4DC;});_tmp4DB;});break;case 112: _LL233: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({struct Cyc_InitDecl_tok_struct
_tmp4DF;_tmp4DF.tag=Cyc_InitDecl_tok;_tmp4DF.f1=({struct _tuple12*_tmp4E0=
_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=({struct Cyc_Parse_Declarator*_tmp4E1=
_cycalloc(sizeof(*_tmp4E1));_tmp4E1->id=({struct _tuple1*_tmp4E2=_cycalloc(
sizeof(*_tmp4E2));_tmp4E2->f1=Cyc_Absyn_rel_ns_null;_tmp4E2->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp4E2;});_tmp4E1->tms=
0;_tmp4E1;});_tmp4E0->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E0;});_tmp4DF;});
_tmp4DE;});break;case 113: _LL234: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_InitDecl_tok_struct
_tmp4E5;_tmp4E5.tag=Cyc_InitDecl_tok;_tmp4E5.f1=({struct _tuple12*_tmp4E6=
_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E6->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4E6;});_tmp4E5;});_tmp4E4;});break;case 114: _LL235: {struct Cyc_List_List*
_tmp4E7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4E9;_tmp4E9.tag=Cyc_TypeSpecifier_tok;
_tmp4E9.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4EA=_cycalloc(
sizeof(*_tmp4EA));_tmp4EA[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4EB;_tmp4EB.tag=
5;_tmp4EB.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp4E7,({struct Cyc_Core_Opt*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EC;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4EB;});
_tmp4EA;}));_tmp4E9;});_tmp4E8;});break;}case 115: _LL236: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4EE;_tmp4EE.tag=Cyc_TypeSpecifier_tok;_tmp4EE.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));
_tmp4EF[0]=({struct Cyc_Absyn_TunionType_struct _tmp4F0;_tmp4F0.tag=2;_tmp4F0.f1=({
struct Cyc_Absyn_TunionInfo _tmp4F1;_tmp4F1.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp4F3;_tmp4F3.tag=0;_tmp4F3.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp4F4;
_tmp4F4.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4F4.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4F4;});_tmp4F3;});
_tmp4F2;}));_tmp4F1.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F1.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F1;});_tmp4F0;});
_tmp4EF;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4EE;});_tmp4ED;});break;case 116: _LL237: {void*
_tmp4F5=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp4FE=_cycalloc(sizeof(*
_tmp4FE));_tmp4FE->v=(void*)((void*)3);_tmp4FE;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4F7;_tmp4F7.tag=Cyc_TypeSpecifier_tok;_tmp4F7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));
_tmp4F8[0]=({struct Cyc_Absyn_TunionType_struct _tmp4F9;_tmp4F9.tag=2;_tmp4F9.f1=({
struct Cyc_Absyn_TunionInfo _tmp4FA;_tmp4FA.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp4FC;_tmp4FC.tag=0;_tmp4FC.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp4FD;
_tmp4FD.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4FD.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FD;});_tmp4FC;});
_tmp4FB;}));_tmp4FA.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FA.rgn=(void*)_tmp4F5;
_tmp4FA;});_tmp4F9;});_tmp4F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F7;});
_tmp4F6;});break;}case 117: _LL238: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp500;_tmp500.tag=Cyc_TypeSpecifier_tok;_tmp500.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp501=_cycalloc(sizeof(*_tmp501));
_tmp501[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp502;_tmp502.tag=3;_tmp502.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp503;_tmp503.field_info=(void*)((void*)({
struct Cyc_Absyn_UnknownTunionfield_struct*_tmp504=_cycalloc(sizeof(*_tmp504));
_tmp504[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp505;_tmp505.tag=0;
_tmp505.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp506;_tmp506.tunion_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp506.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp506.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp506;});_tmp505;});
_tmp504;}));_tmp503.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp503;});_tmp502;});
_tmp501;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp500;});_tmp4FF;});break;case 118: _LL239: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp507=_cycalloc_atomic(sizeof(*_tmp507));
_tmp507[0]=({struct Cyc_Bool_tok_struct _tmp508;_tmp508.tag=Cyc_Bool_tok;_tmp508.f1=
0;_tmp508;});_tmp507;});break;case 119: _LL23A: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp509=_cycalloc_atomic(sizeof(*_tmp509));_tmp509[0]=({struct Cyc_Bool_tok_struct
_tmp50A;_tmp50A.tag=Cyc_Bool_tok;_tmp50A.f1=1;_tmp50A;});_tmp509;});break;case
120: _LL23B: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp50B=_cycalloc(
sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_TunionFieldList_tok_struct _tmp50C;
_tmp50C.tag=Cyc_TunionFieldList_tok;_tmp50C.f1=({struct Cyc_List_List*_tmp50D=
_cycalloc(sizeof(*_tmp50D));_tmp50D->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50D->tl=0;_tmp50D;});
_tmp50C;});_tmp50B;});break;case 121: _LL23C: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp50F;_tmp50F.tag=Cyc_TunionFieldList_tok;_tmp50F.f1=({struct Cyc_List_List*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp510->tl=0;_tmp510;});
_tmp50F;});_tmp50E;});break;case 122: _LL23D: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp512;_tmp512.tag=Cyc_TunionFieldList_tok;_tmp512.f1=({struct Cyc_List_List*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp513->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp513;});_tmp512;});
_tmp511;});break;case 123: _LL23E: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp515;_tmp515.tag=Cyc_TunionFieldList_tok;_tmp515.f1=({struct Cyc_List_List*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp516->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516;});_tmp515;});
_tmp514;});break;case 124: _LL23F: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_Scope_tok_struct
_tmp518;_tmp518.tag=Cyc_Scope_tok;_tmp518.f1=(void*)((void*)2);_tmp518;});
_tmp517;});break;case 125: _LL240: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519[0]=({struct Cyc_Scope_tok_struct
_tmp51A;_tmp51A.tag=Cyc_Scope_tok;_tmp51A.f1=(void*)((void*)3);_tmp51A;});
_tmp519;});break;case 126: _LL241: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Scope_tok_struct
_tmp51C;_tmp51C.tag=Cyc_Scope_tok;_tmp51C.f1=(void*)((void*)0);_tmp51C;});
_tmp51B;});break;case 127: _LL242: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_TunionField_tok_struct
_tmp51E;_tmp51E.tag=Cyc_TunionField_tok;_tmp51E.f1=({struct Cyc_Absyn_Tunionfield*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F->typs=0;_tmp51F->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp51F->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51F;});_tmp51E;});
_tmp51D;});break;case 128: _LL243: {struct Cyc_List_List*_tmp520=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({
struct Cyc_TunionField_tok_struct _tmp522;_tmp522.tag=Cyc_TunionField_tok;_tmp522.f1=({
struct Cyc_Absyn_Tunionfield*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp523->typs=_tmp520;_tmp523->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp523->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp523;});_tmp522;});_tmp521;});break;}case 129: _LL244: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 130: _LL245: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp524=_cycalloc(sizeof(*_tmp524));
_tmp524[0]=({struct Cyc_Declarator_tok_struct _tmp525;_tmp525.tag=Cyc_Declarator_tok;
_tmp525.f1=({struct Cyc_Parse_Declarator*_tmp526=_cycalloc(sizeof(*_tmp526));
_tmp526->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp526->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp526;});
_tmp525;});_tmp524;});break;case 131: _LL246: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527[0]=({struct Cyc_Declarator_tok_struct
_tmp528;_tmp528.tag=Cyc_Declarator_tok;_tmp528.f1=({struct Cyc_Parse_Declarator*
_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp529->tms=0;_tmp529;});
_tmp528;});_tmp527;});break;case 132: _LL247: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 133: _LL248:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));
_tmp52A[0]=({struct Cyc_Declarator_tok_struct _tmp52B;_tmp52B.tag=Cyc_Declarator_tok;
_tmp52B.f1=({struct Cyc_Parse_Declarator*_tmp52C=_cycalloc(sizeof(*_tmp52C));
_tmp52C->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp52C->tms=({struct Cyc_List_List*_tmp52D=_cycalloc(
sizeof(*_tmp52D));_tmp52D->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp52F;_tmp52F.tag=0;_tmp52F.f1=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52F;});_tmp52E;}));
_tmp52D->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp52D;});_tmp52C;});_tmp52B;});_tmp52A;});break;case
134: _LL249: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp530=_cycalloc(
sizeof(*_tmp530));_tmp530[0]=({struct Cyc_Declarator_tok_struct _tmp531;_tmp531.tag=
Cyc_Declarator_tok;_tmp531.f1=({struct Cyc_Parse_Declarator*_tmp532=_cycalloc(
sizeof(*_tmp532));_tmp532->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp532->tms=({
struct Cyc_List_List*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp534=_cycalloc(sizeof(*_tmp534));
_tmp534[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp535;_tmp535.tag=1;_tmp535.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp535.f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp535;});_tmp534;}));_tmp533->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp533;});_tmp532;});
_tmp531;});_tmp530;});break;case 135: _LL24A: {struct _tuple14 _tmp537;struct Cyc_List_List*
_tmp538;int _tmp539;struct Cyc_Absyn_VarargInfo*_tmp53A;struct Cyc_Core_Opt*_tmp53B;
struct Cyc_List_List*_tmp53C;struct _tuple14*_tmp536=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp537=*_tmp536;_tmp538=
_tmp537.f1;_tmp539=_tmp537.f2;_tmp53A=_tmp537.f3;_tmp53B=_tmp537.f4;_tmp53C=
_tmp537.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp53D=_cycalloc(
sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Declarator_tok_struct _tmp53E;_tmp53E.tag=
Cyc_Declarator_tok;_tmp53E.f1=({struct Cyc_Parse_Declarator*_tmp53F=_cycalloc(
sizeof(*_tmp53F));_tmp53F->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp53F->tms=({
struct Cyc_List_List*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp541=_cycalloc(sizeof(*_tmp541));
_tmp541[0]=({struct Cyc_Absyn_Function_mod_struct _tmp542;_tmp542.tag=3;_tmp542.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp543=_cycalloc(sizeof(*
_tmp543));_tmp543[0]=({struct Cyc_Absyn_WithTypes_struct _tmp544;_tmp544.tag=1;
_tmp544.f1=_tmp538;_tmp544.f2=_tmp539;_tmp544.f3=_tmp53A;_tmp544.f4=_tmp53B;
_tmp544.f5=_tmp53C;_tmp544;});_tmp543;}));_tmp542;});_tmp541;}));_tmp540->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp540;});_tmp53F;});_tmp53E;});_tmp53D;});break;}case 136: _LL24B:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp545=_cycalloc(sizeof(*_tmp545));
_tmp545[0]=({struct Cyc_Declarator_tok_struct _tmp546;_tmp546.tag=Cyc_Declarator_tok;
_tmp546.f1=({struct Cyc_Parse_Declarator*_tmp547=_cycalloc(sizeof(*_tmp547));
_tmp547->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp547->tms=({struct Cyc_List_List*_tmp548=_cycalloc(
sizeof(*_tmp548));_tmp548->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp54A;_tmp54A.tag=3;_tmp54A.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp54C;_tmp54C.tag=1;_tmp54C.f1=0;_tmp54C.f2=0;_tmp54C.f3=0;_tmp54C.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54C.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54C;});_tmp54B;}));
_tmp54A;});_tmp549;}));_tmp548->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp548;});_tmp547;});
_tmp546;});_tmp545;});break;case 137: _LL24C: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Declarator_tok_struct
_tmp54E;_tmp54E.tag=Cyc_Declarator_tok;_tmp54E.f1=({struct Cyc_Parse_Declarator*
_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp54F->tms=({
struct Cyc_List_List*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp551=_cycalloc(sizeof(*_tmp551));
_tmp551[0]=({struct Cyc_Absyn_Function_mod_struct _tmp552;_tmp552.tag=3;_tmp552.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553[0]=({struct Cyc_Absyn_NoTypes_struct _tmp554;_tmp554.tag=0;_tmp554.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp554.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp554;});
_tmp553;}));_tmp552;});_tmp551;}));_tmp550->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp550;});_tmp54F;});
_tmp54E;});_tmp54D;});break;case 138: _LL24D: {struct Cyc_List_List*_tmp555=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556[0]=({
struct Cyc_Declarator_tok_struct _tmp557;_tmp557.tag=Cyc_Declarator_tok;_tmp557.f1=({
struct Cyc_Parse_Declarator*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp558->tms=({
struct Cyc_List_List*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp55B;_tmp55B.tag=4;_tmp55B.f1=
_tmp555;_tmp55B.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp55B.f3=0;
_tmp55B;});_tmp55A;}));_tmp559->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp559;});_tmp558;});
_tmp557;});_tmp556;});break;}case 139: _LL24E: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Declarator_tok_struct
_tmp55D;_tmp55D.tag=Cyc_Declarator_tok;_tmp55D.f1=({struct Cyc_Parse_Declarator*
_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp55E->tms=({
struct Cyc_List_List*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp560=_cycalloc(sizeof(*_tmp560));
_tmp560[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp561;_tmp561.tag=5;_tmp561.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp561.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp561;});_tmp560;}));
_tmp55F->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp55F;});_tmp55E;});_tmp55D;});_tmp55C;});break;case
140: _LL24F: Cyc_Parse_err(({const char*_tmp562="identifier has not been declared as a typedef";
_tag_arr(_tmp562,sizeof(char),_get_zero_arr_size(_tmp562,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({
struct Cyc_Declarator_tok_struct _tmp564;_tmp564.tag=Cyc_Declarator_tok;_tmp564.f1=({
struct Cyc_Parse_Declarator*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp565->tms=0;_tmp565;});
_tmp564;});_tmp563;});break;case 141: _LL250: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566[0]=({struct Cyc_Declarator_tok_struct
_tmp567;_tmp567.tag=Cyc_Declarator_tok;_tmp567.f1=({struct Cyc_Parse_Declarator*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->id=Cyc_Absyn_exn_name;_tmp568->tms=
0;_tmp568;});_tmp567;});_tmp566;});break;case 142: _LL251: yyval=(void*)({struct Cyc_YY2_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_YY2_struct _tmp56A;
_tmp56A.tag=Cyc_YY2;_tmp56A.f1=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp56A;});_tmp569;});break;case 143: _LL252: yyval=(void*)({struct Cyc_YY2_struct*
_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B[0]=({struct Cyc_YY2_struct _tmp56C;
_tmp56C.tag=Cyc_YY2;_tmp56C.f1=Cyc_Absyn_true_conref;_tmp56C;});_tmp56B;});
break;case 144: _LL253: yyval=(void*)({struct Cyc_YY2_struct*_tmp56D=_cycalloc(
sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_YY2_struct _tmp56E;_tmp56E.tag=Cyc_YY2;
_tmp56E.f1=Cyc_Absyn_false_conref;_tmp56E;});_tmp56D;});break;case 145: _LL254:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp56F=_cycalloc(sizeof(*
_tmp56F));_tmp56F[0]=({struct Cyc_TypeModifierList_tok_struct _tmp570;_tmp570.tag=
Cyc_TypeModifierList_tok;_tmp570.f1=({struct Cyc_List_List*_tmp571=_cycalloc(
sizeof(*_tmp571));_tmp571->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp573;_tmp573.tag=2;_tmp573.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp573.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp573.f3=Cyc_Absyn_empty_tqual();
_tmp573;});_tmp572;}));_tmp571->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),0);_tmp571;});_tmp570;});
_tmp56F;});break;case 146: _LL255: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp575;_tmp575.tag=Cyc_TypeModifierList_tok;_tmp575.f1=({struct Cyc_List_List*
_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp578;_tmp578.tag=2;_tmp578.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp578.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp578.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp578;});_tmp577;}));
_tmp576->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp576;});
_tmp575;});_tmp574;});break;case 147: _LL256: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp57A;_tmp57A.tag=Cyc_TypeModifierList_tok;_tmp57A.f1=({struct Cyc_List_List*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp57D;_tmp57D.tag=2;_tmp57D.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57D.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp57D.f3=Cyc_Absyn_empty_tqual();
_tmp57D;});_tmp57C;}));_tmp57B->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp57B;});_tmp57A;});
_tmp579;});break;case 148: _LL257: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp57F;_tmp57F.tag=Cyc_TypeModifierList_tok;_tmp57F.f1=({struct Cyc_List_List*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp582;_tmp582.tag=2;_tmp582.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp582.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp582.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp582;});_tmp581;}));
_tmp580->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp580;});_tmp57F;});
_tmp57E;});break;case 149: _LL258: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp584;_tmp584.tag=Cyc_Pointer_Sort_tok;_tmp584.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp586;_tmp586.tag=1;_tmp586.f1=Cyc_Absyn_exp_unsigned_one;_tmp586.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp586;});_tmp585;}));
_tmp584;});_tmp583;});break;case 150: _LL259: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp588;_tmp588.tag=Cyc_Pointer_Sort_tok;_tmp588.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp58A;_tmp58A.tag=0;_tmp58A.f1=Cyc_Absyn_exp_unsigned_one;_tmp58A.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58A;});_tmp589;}));
_tmp588;});_tmp587;});break;case 151: _LL25A: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp58C;_tmp58C.tag=Cyc_Pointer_Sort_tok;_tmp58C.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp58E;_tmp58E.tag=1;_tmp58E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp58E.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58E;});_tmp58D;}));
_tmp58C;});_tmp58B;});break;case 152: _LL25B: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp590;_tmp590.tag=Cyc_Pointer_Sort_tok;_tmp590.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp592;_tmp592.tag=0;_tmp592.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp592.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp592;});_tmp591;}));
_tmp590;});_tmp58F;});break;case 153: _LL25C: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp594;_tmp594.tag=Cyc_Pointer_Sort_tok;_tmp594.f1=(void*)((void*)({struct Cyc_Absyn_TaggedArray_ps_struct*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_Absyn_TaggedArray_ps_struct
_tmp596;_tmp596.tag=2;_tmp596.f1=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp596;});_tmp595;}));
_tmp594;});_tmp593;});break;case 154: _LL25D: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597[0]=({struct Cyc_Type_tok_struct
_tmp598;_tmp598.tag=Cyc_Type_tok;_tmp598.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->v=(void*)((void*)3);
_tmp599;}),0);_tmp598;});_tmp597;});break;case 155: _LL25E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL25F: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 157: _LL260: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp59A=_cycalloc(
sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Type_tok_struct _tmp59B;_tmp59B.tag=Cyc_Type_tok;
_tmp59B.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp59C=_cycalloc(
sizeof(*_tmp59C));_tmp59C->v=(void*)((void*)3);_tmp59C;}),0);_tmp59B;});_tmp59A;});
break;case 158: _LL261: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL262: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp59D=_cycalloc_atomic(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_TypeQual_tok_struct
_tmp59E;_tmp59E.tag=Cyc_TypeQual_tok;_tmp59E.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp59E;});_tmp59D;});
break;case 160: _LL263: yyval=(void*)({struct Cyc_YY1_struct*_tmp59F=_cycalloc(
sizeof(*_tmp59F));_tmp59F[0]=({struct Cyc_YY1_struct _tmp5A0;_tmp5A0.tag=Cyc_YY1;
_tmp5A0.f1=({struct _tuple14*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5A1->f2=0;
_tmp5A1->f3=0;_tmp5A1->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A1->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A1;});_tmp5A0;});
_tmp59F;});break;case 161: _LL264: yyval=(void*)({struct Cyc_YY1_struct*_tmp5A2=
_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_YY1_struct _tmp5A3;_tmp5A3.tag=
Cyc_YY1;_tmp5A3.f1=({struct _tuple14*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp5A4->f2=1;
_tmp5A4->f3=0;_tmp5A4->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A4->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A4;});_tmp5A3;});
_tmp5A2;});break;case 162: _LL265: {struct _tuple2 _tmp5A6;struct Cyc_Core_Opt*
_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A9;struct _tuple2*_tmp5A5=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A6=*_tmp5A5;
_tmp5A7=_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;_tmp5A9=_tmp5A6.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5AA=({struct Cyc_Absyn_VarargInfo*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->name=
_tmp5A7;_tmp5AE->tq=_tmp5A8;_tmp5AE->type=(void*)_tmp5A9;_tmp5AE->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5AE;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({
struct Cyc_YY1_struct _tmp5AC;_tmp5AC.tag=Cyc_YY1;_tmp5AC.f1=({struct _tuple14*
_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->f1=0;_tmp5AD->f2=0;_tmp5AD->f3=
_tmp5AA;_tmp5AD->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5AD->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AD;});_tmp5AC;});
_tmp5AB;});break;}}case 163: _LL266: {struct _tuple2 _tmp5B0;struct Cyc_Core_Opt*
_tmp5B1;struct Cyc_Absyn_Tqual _tmp5B2;void*_tmp5B3;struct _tuple2*_tmp5AF=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5B0=*_tmp5AF;
_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;_tmp5B3=_tmp5B0.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5B4=({struct Cyc_Absyn_VarargInfo*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->name=
_tmp5B1;_tmp5B8->tq=_tmp5B2;_tmp5B8->type=(void*)_tmp5B3;_tmp5B8->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5B8;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({
struct Cyc_YY1_struct _tmp5B6;_tmp5B6.tag=Cyc_YY1;_tmp5B6.f1=({struct _tuple14*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5B7->f2=0;_tmp5B7->f3=
_tmp5B4;_tmp5B7->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5B7->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B7;});_tmp5B6;});
_tmp5B5;});break;}}case 164: _LL267: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({struct Cyc_Type_tok_struct
_tmp5BA;_tmp5BA.tag=Cyc_Type_tok;_tmp5BA.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5BC;_tmp5BC.tag=1;_tmp5BC.f1=0;_tmp5BC;});_tmp5BB;}));_tmp5BA;});_tmp5B9;});
break;case 165: _LL268: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5BD=_cycalloc(
sizeof(*_tmp5BD));_tmp5BD[0]=({struct Cyc_Type_tok_struct _tmp5BE;_tmp5BE.tag=Cyc_Type_tok;
_tmp5BE.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5C0;_tmp5C0.tag=0;_tmp5C0.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C0;});_tmp5BF;}));
_tmp5BE;});_tmp5BD;});break;case 166: _LL269: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5C2;_tmp5C2.tag=Cyc_TypeOpt_tok;_tmp5C2.f1=0;_tmp5C2;});_tmp5C1;});break;
case 167: _LL26A: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5C3=_cycalloc(
sizeof(*_tmp5C3));_tmp5C3[0]=({struct Cyc_TypeOpt_tok_struct _tmp5C4;_tmp5C4.tag=
Cyc_TypeOpt_tok;_tmp5C4.f1=({struct Cyc_Core_Opt*_tmp5C5=_cycalloc(sizeof(*
_tmp5C5));_tmp5C5->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5C6=
_cycalloc(sizeof(*_tmp5C6));_tmp5C6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5C7;
_tmp5C7.tag=20;_tmp5C7.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C7;});_tmp5C6;}));
_tmp5C5;});_tmp5C4;});_tmp5C3;});break;case 168: _LL26B: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5C8=_cycalloc(sizeof(*_tmp5C8));_tmp5C8[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5C9;_tmp5C9.tag=Cyc_Rgnorder_tok;_tmp5C9.f1=0;_tmp5C9;});_tmp5C8;});break;
case 169: _LL26C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 170: _LL26D: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5CA=
_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_Rgnorder_tok_struct _tmp5CB;
_tmp5CB.tag=Cyc_Rgnorder_tok;_tmp5CB.f1=({struct Cyc_List_List*_tmp5CC=_cycalloc(
sizeof(*_tmp5CC));_tmp5CC->hd=({struct _tuple4*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));
_tmp5CD->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5D0=_cycalloc(sizeof(*
_tmp5D0));_tmp5D0[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5D1;_tmp5D1.tag=20;
_tmp5D1.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5D1;});_tmp5D0;});_tmp5CD->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5CF;_tmp5CF.tag=0;_tmp5CF.f1=(void*)((void*)3);_tmp5CF;});_tmp5CE;}));
_tmp5CD;});_tmp5CC->tl=0;_tmp5CC;});_tmp5CB;});_tmp5CA;});break;case 171: _LL26E:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));
_tmp5D2[0]=({struct Cyc_Rgnorder_tok_struct _tmp5D3;_tmp5D3.tag=Cyc_Rgnorder_tok;
_tmp5D3.f1=({struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=({
struct _tuple4*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5D9;_tmp5D9.tag=20;_tmp5D9.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5D9;});_tmp5D8;});
_tmp5D5->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5D7;_tmp5D7.tag=0;_tmp5D7.f1=(void*)((void*)3);_tmp5D7;});_tmp5D6;}));
_tmp5D5;});_tmp5D4->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D4;});_tmp5D3;});
_tmp5D2;});break;case 172: _LL26F: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp5DA=
_cycalloc_atomic(sizeof(*_tmp5DA));_tmp5DA[0]=({struct Cyc_Bool_tok_struct _tmp5DB;
_tmp5DB.tag=Cyc_Bool_tok;_tmp5DB.f1=0;_tmp5DB;});_tmp5DA;});break;case 173: _LL270:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5DC="inject";
_tag_arr(_tmp5DC,sizeof(char),_get_zero_arr_size(_tmp5DC,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5DD="missing type in function declaration";_tag_arr(_tmp5DD,
sizeof(char),_get_zero_arr_size(_tmp5DD,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp5DE=_cycalloc_atomic(sizeof(*_tmp5DE));_tmp5DE[0]=({
struct Cyc_Bool_tok_struct _tmp5DF;_tmp5DF.tag=Cyc_Bool_tok;_tmp5DF.f1=1;_tmp5DF;});
_tmp5DE;});break;case 174: _LL271: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 175: _LL272: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0[0]=({struct Cyc_TypeList_tok_struct
_tmp5E1;_tmp5E1.tag=Cyc_TypeList_tok;_tmp5E1.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5E1;});_tmp5E0;});
break;case 176: _LL273: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5E2=
_cycalloc(sizeof(*_tmp5E2));_tmp5E2[0]=({struct Cyc_TypeList_tok_struct _tmp5E3;
_tmp5E3.tag=Cyc_TypeList_tok;_tmp5E3.f1=0;_tmp5E3;});_tmp5E2;});break;case 177:
_LL274: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 178: _LL275: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5E4=_cycalloc(
sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_TypeList_tok_struct _tmp5E5;_tmp5E5.tag=
Cyc_TypeList_tok;_tmp5E5.f1=({struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*
_tmp5E6));_tmp5E6->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5E7=
_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5E8;
_tmp5E8.tag=21;_tmp5E8.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E8;});_tmp5E7;}));
_tmp5E6->tl=0;_tmp5E6;});_tmp5E5;});_tmp5E4;});break;case 179: _LL276: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5E9=_cycalloc(sizeof(*
_tmp5E9));_tmp5E9[0]=({struct Cyc_TypeList_tok_struct _tmp5EA;_tmp5EA.tag=Cyc_TypeList_tok;
_tmp5EA.f1=({struct Cyc_List_List*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5EB->tl=0;_tmp5EB;});_tmp5EA;});_tmp5E9;});break;case 180: _LL277: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5EC=_cycalloc(sizeof(*
_tmp5EC));_tmp5EC[0]=({struct Cyc_TypeList_tok_struct _tmp5ED;_tmp5ED.tag=Cyc_TypeList_tok;
_tmp5ED.f1=({struct Cyc_List_List*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5EF=_cycalloc(sizeof(*
_tmp5EF));_tmp5EF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5F0;_tmp5F0.tag=19;
_tmp5F0.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5F0;});_tmp5EF;}));_tmp5EE->tl=0;_tmp5EE;});_tmp5ED;});
_tmp5EC;});break;case 181: _LL278: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));
_tmp5F1[0]=({struct Cyc_TypeList_tok_struct _tmp5F2;_tmp5F2.tag=Cyc_TypeList_tok;
_tmp5F2.f1=({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5F4=_cycalloc(sizeof(*
_tmp5F4));_tmp5F4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5F5;_tmp5F5.tag=19;
_tmp5F5.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5F5;});_tmp5F4;}));_tmp5F3->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F3;});_tmp5F2;});
_tmp5F1;});break;case 182: _LL279: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5F7;_tmp5F7.tag=Cyc_ParamDeclList_tok;_tmp5F7.f1=({struct Cyc_List_List*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F8->tl=0;_tmp5F8;});
_tmp5F7;});_tmp5F6;});break;case 183: _LL27A: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5FA;_tmp5FA.tag=Cyc_ParamDeclList_tok;_tmp5FA.f1=({struct Cyc_List_List*
_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FB->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FB;});_tmp5FA;});
_tmp5F9;});break;case 184: _LL27B: {struct _tuple15 _tmp5FD;struct Cyc_Absyn_Tqual
_tmp5FE;struct Cyc_List_List*_tmp5FF;struct Cyc_List_List*_tmp600;struct _tuple15*
_tmp5FC=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5FD=*_tmp5FC;_tmp5FE=_tmp5FD.f1;_tmp5FF=_tmp5FD.f2;
_tmp600=_tmp5FD.f3;{struct Cyc_Parse_Declarator _tmp602;struct _tuple1*_tmp603;
struct Cyc_List_List*_tmp604;struct Cyc_Parse_Declarator*_tmp601=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp602=*_tmp601;
_tmp603=_tmp602.id;_tmp604=_tmp602.tms;{void*_tmp605=Cyc_Parse_speclist2typ(
_tmp5FF,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp607;void*_tmp608;struct
Cyc_List_List*_tmp609;struct Cyc_List_List*_tmp60A;struct _tuple6 _tmp606=Cyc_Parse_apply_tms(
_tmp5FE,_tmp605,_tmp600,_tmp604);_tmp607=_tmp606.f1;_tmp608=_tmp606.f2;_tmp609=
_tmp606.f3;_tmp60A=_tmp606.f4;if(_tmp609 != 0)Cyc_Parse_err(({const char*_tmp60B="parameter with bad type params";
_tag_arr(_tmp60B,sizeof(char),_get_zero_arr_size(_tmp60B,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp603))Cyc_Parse_err(({const char*_tmp60C="parameter cannot be qualified with a namespace";
_tag_arr(_tmp60C,sizeof(char),_get_zero_arr_size(_tmp60C,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp60D=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp613=_cycalloc(sizeof(*
_tmp613));_tmp613->v=(*((struct _tuple1*)_tmp603)).f2;_tmp613;});if(_tmp60A != 0)({
void*_tmp60E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp60F="extra attributes on parameter, ignoring";_tag_arr(_tmp60F,sizeof(char),
_get_zero_arr_size(_tmp60F,40));}),_tag_arr(_tmp60E,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp610=_cycalloc(sizeof(*_tmp610));
_tmp610[0]=({struct Cyc_ParamDecl_tok_struct _tmp611;_tmp611.tag=Cyc_ParamDecl_tok;
_tmp611.f1=({struct _tuple2*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->f1=
_tmp60D;_tmp612->f2=_tmp607;_tmp612->f3=_tmp608;_tmp612;});_tmp611;});_tmp610;});
break;}}}}case 185: _LL27C: {struct _tuple15 _tmp615;struct Cyc_Absyn_Tqual _tmp616;
struct Cyc_List_List*_tmp617;struct Cyc_List_List*_tmp618;struct _tuple15*_tmp614=
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp615=*_tmp614;_tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_tmp618=
_tmp615.f3;{void*_tmp619=Cyc_Parse_speclist2typ(_tmp617,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp618 != 0)({
void*_tmp61A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp61B="bad attributes on parameter, ignoring";_tag_arr(_tmp61B,sizeof(char),
_get_zero_arr_size(_tmp61B,38));}),_tag_arr(_tmp61A,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));
_tmp61C[0]=({struct Cyc_ParamDecl_tok_struct _tmp61D;_tmp61D.tag=Cyc_ParamDecl_tok;
_tmp61D.f1=({struct _tuple2*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->f1=0;
_tmp61E->f2=_tmp616;_tmp61E->f3=_tmp619;_tmp61E;});_tmp61D;});_tmp61C;});break;}}
case 186: _LL27D: {struct _tuple15 _tmp620;struct Cyc_Absyn_Tqual _tmp621;struct Cyc_List_List*
_tmp622;struct Cyc_List_List*_tmp623;struct _tuple15*_tmp61F=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp620=*_tmp61F;
_tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;{void*_tmp624=Cyc_Parse_speclist2typ(
_tmp622,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp625=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual
_tmp627;void*_tmp628;struct Cyc_List_List*_tmp629;struct Cyc_List_List*_tmp62A;
struct _tuple6 _tmp626=Cyc_Parse_apply_tms(_tmp621,_tmp624,_tmp623,_tmp625);
_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;_tmp629=_tmp626.f3;_tmp62A=_tmp626.f4;if(
_tmp629 != 0)({void*_tmp62B[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62C="bad type parameters on formal argument, ignoring";_tag_arr(_tmp62C,
sizeof(char),_get_zero_arr_size(_tmp62C,49));}),_tag_arr(_tmp62B,sizeof(void*),0));});
if(_tmp62A != 0)({void*_tmp62D[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62E="bad attributes on parameter, ignoring";_tag_arr(_tmp62E,sizeof(char),
_get_zero_arr_size(_tmp62E,38));}),_tag_arr(_tmp62D,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));
_tmp62F[0]=({struct Cyc_ParamDecl_tok_struct _tmp630;_tmp630.tag=Cyc_ParamDecl_tok;
_tmp630.f1=({struct _tuple2*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->f1=0;
_tmp631->f2=_tmp627;_tmp631->f3=_tmp628;_tmp631;});_tmp630;});_tmp62F;});break;}}
case 187: _LL27E: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp632=_cycalloc(
sizeof(*_tmp632));_tmp632[0]=({struct Cyc_IdList_tok_struct _tmp633;_tmp633.tag=
Cyc_IdList_tok;_tmp633.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp633;});_tmp632;});break;case 188: _LL27F: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_IdList_tok_struct
_tmp635;_tmp635.tag=Cyc_IdList_tok;_tmp635.f1=({struct Cyc_List_List*_tmp636=
_cycalloc(sizeof(*_tmp636));_tmp636->hd=({struct _tagged_arr*_tmp637=_cycalloc(
sizeof(*_tmp637));_tmp637[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp637;});_tmp636->tl=0;
_tmp636;});_tmp635;});_tmp634;});break;case 189: _LL280: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_IdList_tok_struct
_tmp639;_tmp639.tag=Cyc_IdList_tok;_tmp639.f1=({struct Cyc_List_List*_tmp63A=
_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=({struct _tagged_arr*_tmp63B=_cycalloc(
sizeof(*_tmp63B));_tmp63B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63B;});_tmp63A->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp63A;});_tmp639;});
_tmp638;});break;case 190: _LL281: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 191: _LL282: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 192: _LL283: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({struct Cyc_Exp_tok_struct _tmp63D;
_tmp63D.tag=Cyc_Exp_tok;_tmp63D.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp63F;_tmp63F.tag=34;_tmp63F.f1=0;_tmp63F.f2=0;_tmp63F;});_tmp63E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp63D;});
_tmp63C;});break;case 193: _LL284: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp640=
_cycalloc(sizeof(*_tmp640));_tmp640[0]=({struct Cyc_Exp_tok_struct _tmp641;_tmp641.tag=
Cyc_Exp_tok;_tmp641.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp643;_tmp643.tag=34;_tmp643.f1=0;_tmp643.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp643;});_tmp642;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp641;});_tmp640;});break;case 194: _LL285: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644[0]=({
struct Cyc_Exp_tok_struct _tmp645;_tmp645.tag=Cyc_Exp_tok;_tmp645.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp646=_cycalloc(sizeof(*_tmp646));
_tmp646[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp647;_tmp647.tag=34;
_tmp647.f1=0;_tmp647.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp647;});_tmp646;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp645;});
_tmp644;});break;case 195: _LL286: {struct Cyc_Absyn_Vardecl*_tmp648=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->f1=(void*)0;_tmp64E->f2=({
struct _tagged_arr*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp64F;});_tmp64E;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp648->tq=({
struct Cyc_Absyn_Tqual _tmp649;_tmp649.q_const=1;_tmp649.q_volatile=0;_tmp649.q_restrict=
1;_tmp649;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp64A=_cycalloc(sizeof(*
_tmp64A));_tmp64A[0]=({struct Cyc_Exp_tok_struct _tmp64B;_tmp64B.tag=Cyc_Exp_tok;
_tmp64B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp64D;_tmp64D.tag=27;_tmp64D.f1=_tmp648;_tmp64D.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp64D.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64D.f4=0;
_tmp64D;});_tmp64C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp64B;});
_tmp64A;});break;}case 196: _LL287: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_InitializerList_tok_struct
_tmp651;_tmp651.tag=Cyc_InitializerList_tok;_tmp651.f1=({struct Cyc_List_List*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->hd=({struct _tuple17*_tmp653=
_cycalloc(sizeof(*_tmp653));_tmp653->f1=0;_tmp653->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp653;});_tmp652->tl=0;
_tmp652;});_tmp651;});_tmp650;});break;case 197: _LL288: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_InitializerList_tok_struct
_tmp655;_tmp655.tag=Cyc_InitializerList_tok;_tmp655.f1=({struct Cyc_List_List*
_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->hd=({struct _tuple17*_tmp657=
_cycalloc(sizeof(*_tmp657));_tmp657->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp657->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp657;});_tmp656->tl=
0;_tmp656;});_tmp655;});_tmp654;});break;case 198: _LL289: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658[0]=({struct Cyc_InitializerList_tok_struct
_tmp659;_tmp659.tag=Cyc_InitializerList_tok;_tmp659.f1=({struct Cyc_List_List*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->hd=({struct _tuple17*_tmp65B=
_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=0;_tmp65B->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65B;});_tmp65A->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65A;});_tmp659;});
_tmp658;});break;case 199: _LL28A: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_InitializerList_tok_struct
_tmp65D;_tmp65D.tag=Cyc_InitializerList_tok;_tmp65D.f1=({struct Cyc_List_List*
_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->hd=({struct _tuple17*_tmp65F=
_cycalloc(sizeof(*_tmp65F));_tmp65F->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65F;});_tmp65E->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp65E;});_tmp65D;});_tmp65C;});break;case 200: _LL28B: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp660=_cycalloc(sizeof(*_tmp660));
_tmp660[0]=({struct Cyc_DesignatorList_tok_struct _tmp661;_tmp661.tag=Cyc_DesignatorList_tok;
_tmp661.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp661;});_tmp660;});
break;case 201: _LL28C: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp662=
_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_DesignatorList_tok_struct
_tmp663;_tmp663.tag=Cyc_DesignatorList_tok;_tmp663.f1=({struct Cyc_List_List*
_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp664->tl=0;_tmp664;});
_tmp663;});_tmp662;});break;case 202: _LL28D: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_DesignatorList_tok_struct
_tmp666;_tmp666.tag=Cyc_DesignatorList_tok;_tmp666.f1=({struct Cyc_List_List*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp667->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp667;});_tmp666;});
_tmp665;});break;case 203: _LL28E: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({struct Cyc_Designator_tok_struct
_tmp669;_tmp669.tag=Cyc_Designator_tok;_tmp669.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp66B;_tmp66B.tag=0;_tmp66B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66B;});_tmp66A;}));
_tmp669;});_tmp668;});break;case 204: _LL28F: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Designator_tok_struct
_tmp66D;_tmp66D.tag=Cyc_Designator_tok;_tmp66D.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E[0]=({struct Cyc_Absyn_FieldName_struct
_tmp66F;_tmp66F.tag=1;_tmp66F.f1=({struct _tagged_arr*_tmp670=_cycalloc(sizeof(*
_tmp670));_tmp670[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp670;});_tmp66F;});_tmp66E;}));_tmp66D;});_tmp66C;});
break;case 205: _LL290: {void*_tmp671=Cyc_Parse_speclist2typ((*((struct _tuple15*)
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f2,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));struct Cyc_List_List*
_tmp672=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))).f3;if(_tmp672 != 0)({void*
_tmp673[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp674="ignoring attributes in type";_tag_arr(_tmp674,sizeof(char),
_get_zero_arr_size(_tmp674,28));}),_tag_arr(_tmp673,sizeof(void*),0));});{struct
Cyc_Absyn_Tqual _tmp675=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))).f1;yyval=(void*)({struct
Cyc_ParamDecl_tok_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676[0]=({struct
Cyc_ParamDecl_tok_struct _tmp677;_tmp677.tag=Cyc_ParamDecl_tok;_tmp677.f1=({
struct _tuple2*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->f1=0;_tmp678->f2=
_tmp675;_tmp678->f3=_tmp671;_tmp678;});_tmp677;});_tmp676;});break;}}case 206:
_LL291: {void*_tmp679=Cyc_Parse_speclist2typ((*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp67A=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f3;struct Cyc_Absyn_Tqual
_tmp67B=(*((struct _tuple15*)Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f1;struct Cyc_List_List*
_tmp67C=(Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp67D=Cyc_Parse_apply_tms(_tmp67B,
_tmp679,_tmp67A,_tmp67C);if(_tmp67D.f3 != 0)({void*_tmp67E[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp67F="bad type params, ignoring";
_tag_arr(_tmp67F,sizeof(char),_get_zero_arr_size(_tmp67F,26));}),_tag_arr(
_tmp67E,sizeof(void*),0));});if(_tmp67D.f4 != 0)({void*_tmp680[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp681="bad specifiers, ignoring";
_tag_arr(_tmp681,sizeof(char),_get_zero_arr_size(_tmp681,25));}),_tag_arr(
_tmp680,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682[0]=({struct Cyc_ParamDecl_tok_struct
_tmp683;_tmp683.tag=Cyc_ParamDecl_tok;_tmp683.f1=({struct _tuple2*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684->f1=0;_tmp684->f2=_tmp67D.f1;_tmp684->f3=
_tmp67D.f2;_tmp684;});_tmp683;});_tmp682;});break;}case 207: _LL292: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685[0]=({
struct Cyc_Type_tok_struct _tmp686;_tmp686.tag=Cyc_Type_tok;_tmp686.f1=(void*)(*((
struct _tuple2*)Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f3;_tmp686;});_tmp685;});break;case 208: _LL293: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({
struct Cyc_Type_tok_struct _tmp688;_tmp688.tag=Cyc_Type_tok;_tmp688.f1=(void*)((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp689=_cycalloc(sizeof(*_tmp689));
_tmp689[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68A;_tmp68A.tag=20;_tmp68A.f1=0;
_tmp68A;});_tmp689;}));_tmp688;});_tmp687;});break;case 209: _LL294: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({
struct Cyc_Type_tok_struct _tmp68C;_tmp68C.tag=Cyc_Type_tok;_tmp68C.f1=(void*)((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));
_tmp68D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68E;_tmp68E.tag=20;_tmp68E.f1=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68E;});_tmp68D;}));
_tmp68C;});_tmp68B;});break;case 210: _LL295: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Type_tok_struct
_tmp690;_tmp690.tag=Cyc_Type_tok;_tmp690.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp692;_tmp692.tag=21;_tmp692.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp692;});_tmp691;}));
_tmp690;});_tmp68F;});break;case 211: _LL296: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Type_tok_struct
_tmp694;_tmp694.tag=Cyc_Type_tok;_tmp694.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp696;_tmp696.tag=20;_tmp696.f1=({struct Cyc_List_List*_tmp697=_cycalloc(
sizeof(*_tmp697));_tmp697->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp697->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp697;});_tmp696;});
_tmp695;}));_tmp694;});_tmp693;});break;case 212: _LL297: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_TypeList_tok_struct
_tmp699;_tmp699.tag=Cyc_TypeList_tok;_tmp699.f1=({struct Cyc_List_List*_tmp69A=
_cycalloc(sizeof(*_tmp69A));_tmp69A->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69A->tl=0;_tmp69A;});
_tmp699;});_tmp698;});break;case 213: _LL298: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_TypeList_tok_struct
_tmp69C;_tmp69C.tag=Cyc_TypeList_tok;_tmp69C.f1=({struct Cyc_List_List*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69D->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69D;});_tmp69C;});
_tmp69B;});break;case 214: _LL299: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp69F;_tmp69F.tag=Cyc_AbstractDeclarator_tok;_tmp69F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A0;});_tmp69F;});
_tmp69E;});break;case 215: _LL29A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 216: _LL29B: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6A2;_tmp6A2.tag=Cyc_AbstractDeclarator_tok;_tmp6A2.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6A3;});
_tmp6A2;});_tmp6A1;});break;case 217: _LL29C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 218: _LL29D:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6A4=_cycalloc(sizeof(*
_tmp6A4));_tmp6A4[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6A5;_tmp6A5.tag=
Cyc_AbstractDeclarator_tok;_tmp6A5.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->tms=({struct Cyc_List_List*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6A9;_tmp6A9.tag=0;_tmp6A9.f1=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A9;});_tmp6A8;}));
_tmp6A7->tl=0;_tmp6A7;});_tmp6A6;});_tmp6A5;});_tmp6A4;});break;case 219: _LL29E:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6AA=_cycalloc(sizeof(*
_tmp6AA));_tmp6AA[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6AB;_tmp6AB.tag=
Cyc_AbstractDeclarator_tok;_tmp6AB.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->tms=({struct Cyc_List_List*_tmp6AD=
_cycalloc(sizeof(*_tmp6AD));_tmp6AD->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6AF;_tmp6AF.tag=0;_tmp6AF.f1=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AF;});_tmp6AE;}));
_tmp6AD->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6AD;});_tmp6AC;});
_tmp6AB;});_tmp6AA;});break;case 220: _LL29F: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6B1;_tmp6B1.tag=Cyc_AbstractDeclarator_tok;_tmp6B1.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->tms=({struct Cyc_List_List*_tmp6B3=
_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6B5;_tmp6B5.tag=1;_tmp6B5.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B5.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B5;});_tmp6B4;}));
_tmp6B3->tl=0;_tmp6B3;});_tmp6B2;});_tmp6B1;});_tmp6B0;});break;case 221: _LL2A0:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6B6=_cycalloc(sizeof(*
_tmp6B6));_tmp6B6[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6B7;_tmp6B7.tag=
Cyc_AbstractDeclarator_tok;_tmp6B7.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->tms=({struct Cyc_List_List*_tmp6B9=
_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6BB;_tmp6BB.tag=1;_tmp6BB.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BB.f2=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BB;});_tmp6BA;}));
_tmp6B9->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6B9;});_tmp6B8;});
_tmp6B7;});_tmp6B6;});break;case 222: _LL2A1: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6BD;_tmp6BD.tag=Cyc_AbstractDeclarator_tok;_tmp6BD.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->tms=({struct Cyc_List_List*_tmp6BF=
_cycalloc(sizeof(*_tmp6BF));_tmp6BF->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6C1;_tmp6C1.tag=3;_tmp6C1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6C3;_tmp6C3.tag=1;_tmp6C3.f1=0;_tmp6C3.f2=0;_tmp6C3.f3=0;_tmp6C3.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C3.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C3;});_tmp6C2;}));
_tmp6C1;});_tmp6C0;}));_tmp6BF->tl=0;_tmp6BF;});_tmp6BE;});_tmp6BD;});_tmp6BC;});
break;case 223: _LL2A2: {struct _tuple14 _tmp6C5;struct Cyc_List_List*_tmp6C6;int
_tmp6C7;struct Cyc_Absyn_VarargInfo*_tmp6C8;struct Cyc_Core_Opt*_tmp6C9;struct Cyc_List_List*
_tmp6CA;struct _tuple14*_tmp6C4=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6C5=*_tmp6C4;_tmp6C6=_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;
_tmp6C8=_tmp6C5.f3;_tmp6C9=_tmp6C5.f4;_tmp6CA=_tmp6C5.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6CC;_tmp6CC.tag=Cyc_AbstractDeclarator_tok;
_tmp6CC.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CD=_cycalloc(sizeof(*
_tmp6CD));_tmp6CD->tms=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));
_tmp6CE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6CF=
_cycalloc(sizeof(*_tmp6CF));_tmp6CF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D0;_tmp6D0.tag=3;_tmp6D0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D2;_tmp6D2.tag=1;_tmp6D2.f1=_tmp6C6;_tmp6D2.f2=_tmp6C7;_tmp6D2.f3=_tmp6C8;
_tmp6D2.f4=_tmp6C9;_tmp6D2.f5=_tmp6CA;_tmp6D2;});_tmp6D1;}));_tmp6D0;});_tmp6CF;}));
_tmp6CE->tl=0;_tmp6CE;});_tmp6CD;});_tmp6CC;});_tmp6CB;});break;}case 224: _LL2A3:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6D3=_cycalloc(sizeof(*
_tmp6D3));_tmp6D3[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6D4;_tmp6D4.tag=
Cyc_AbstractDeclarator_tok;_tmp6D4.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->tms=({struct Cyc_List_List*_tmp6D6=
_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D8;_tmp6D8.tag=3;_tmp6D8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6DA;_tmp6DA.tag=1;_tmp6DA.f1=0;_tmp6DA.f2=0;_tmp6DA.f3=0;_tmp6DA.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DA.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA;});_tmp6D9;}));
_tmp6D8;});_tmp6D7;}));_tmp6D6->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6D6;});_tmp6D5;});
_tmp6D4;});_tmp6D3;});break;case 225: _LL2A4: {struct _tuple14 _tmp6DC;struct Cyc_List_List*
_tmp6DD;int _tmp6DE;struct Cyc_Absyn_VarargInfo*_tmp6DF;struct Cyc_Core_Opt*_tmp6E0;
struct Cyc_List_List*_tmp6E1;struct _tuple14*_tmp6DB=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DC=*_tmp6DB;_tmp6DD=
_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;_tmp6DF=_tmp6DC.f3;_tmp6E0=_tmp6DC.f4;_tmp6E1=
_tmp6DC.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6E2=
_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6E3;_tmp6E3.tag=Cyc_AbstractDeclarator_tok;_tmp6E3.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->tms=({struct Cyc_List_List*_tmp6E5=
_cycalloc(sizeof(*_tmp6E5));_tmp6E5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E7;_tmp6E7.tag=3;_tmp6E7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E9;_tmp6E9.tag=1;_tmp6E9.f1=_tmp6DD;_tmp6E9.f2=_tmp6DE;_tmp6E9.f3=_tmp6DF;
_tmp6E9.f4=_tmp6E0;_tmp6E9.f5=_tmp6E1;_tmp6E9;});_tmp6E8;}));_tmp6E7;});_tmp6E6;}));
_tmp6E5->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6E5;});_tmp6E4;});
_tmp6E3;});_tmp6E2;});break;}case 226: _LL2A5: {struct Cyc_List_List*_tmp6EA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6EC;_tmp6EC.tag=Cyc_AbstractDeclarator_tok;
_tmp6EC.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6ED=_cycalloc(sizeof(*
_tmp6ED));_tmp6ED->tms=({struct Cyc_List_List*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));
_tmp6EE->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6F0;_tmp6F0.tag=4;_tmp6F0.f1=_tmp6EA;_tmp6F0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F0.f3=0;
_tmp6F0;});_tmp6EF;}));_tmp6EE->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6EE;});_tmp6ED;});
_tmp6EC;});_tmp6EB;});break;}case 227: _LL2A6: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6F2;_tmp6F2.tag=Cyc_AbstractDeclarator_tok;_tmp6F2.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->tms=({struct Cyc_List_List*_tmp6F4=
_cycalloc(sizeof(*_tmp6F4));_tmp6F4->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6F6;_tmp6F6.tag=5;_tmp6F6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F6.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F6;});_tmp6F5;}));
_tmp6F4->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp6F4;});_tmp6F3;});
_tmp6F2;});_tmp6F1;});break;case 228: _LL2A7: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 229: _LL2A8: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230: _LL2A9:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231:
_LL2AA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
232: _LL2AB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 233: _LL2AC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 234: _LL2AD: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp6F7="`H";
_tag_arr(_tmp6F7,sizeof(char),_get_zero_arr_size(_tmp6F7,3));}))== 0)Cyc_Parse_err(({
const char*_tmp6F8="bad occurrence of heap region `H";_tag_arr(_tmp6F8,sizeof(
char),_get_zero_arr_size(_tmp6F8,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->name=({
struct _tagged_arr*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp706;});_tmp703->identity=
0;_tmp703->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp704=_cycalloc(
sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp705;_tmp705.tag=0;
_tmp705.f1=(void*)((void*)3);_tmp705;});_tmp704;}));_tmp703;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({
struct Cyc_Absyn_VarType_struct _tmp702;_tmp702.tag=1;_tmp702.f1=tv;_tmp702;});
_tmp701;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F9=_cycalloc(sizeof(*
_tmp6F9));_tmp6F9[0]=({struct Cyc_Stmt_tok_struct _tmp6FA;_tmp6FA.tag=Cyc_Stmt_tok;
_tmp6FA.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_Region_s_struct _tmp6FC;
_tmp6FC.tag=18;_tmp6FC.f1=tv;_tmp6FC.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->f1=(void*)0;_tmp6FD->f2=({struct
_tagged_arr*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FE;});_tmp6FD;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));
_tmp6FF[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp700;_tmp700.tag=15;_tmp700.f1=(
void*)t;_tmp700;});_tmp6FF;}),0);_tmp6FC.f3=0;_tmp6FC.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FC;});_tmp6FB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6FA;});
_tmp6F9;});break;}case 235: _LL2AE: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp707="H";
_tag_arr(_tmp707,sizeof(char),_get_zero_arr_size(_tmp707,2));}))== 0)Cyc_Parse_err(({
const char*_tmp708="bad occurrence of heap region `H";_tag_arr(_tmp708,sizeof(
char),_get_zero_arr_size(_tmp708,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->name=({
struct _tagged_arr*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp719;_tmp719.tag=0;_tmp719.f1=(struct
_tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp717[1]={& _tmp719};Cyc_aprintf(({const char*_tmp718="`%s";
_tag_arr(_tmp718,sizeof(char),_get_zero_arr_size(_tmp718,4));}),_tag_arr(_tmp717,
sizeof(void*),1));}});_tmp716;});_tmp713->identity=0;_tmp713->kind=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp715;_tmp715.tag=0;_tmp715.f1=(void*)((void*)3);
_tmp715;});_tmp714;}));_tmp713;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Absyn_VarType_struct
_tmp712;_tmp712.tag=1;_tmp712.f1=tv;_tmp712;});_tmp711;});yyval=(void*)({struct
Cyc_Stmt_tok_struct*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Stmt_tok_struct
_tmp70A;_tmp70A.tag=Cyc_Stmt_tok;_tmp70A.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=({
struct Cyc_Absyn_Region_s_struct _tmp70C;_tmp70C.tag=18;_tmp70C.f1=tv;_tmp70C.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp70D=_cycalloc(sizeof(*_tmp70D));
_tmp70D->f1=(void*)0;_tmp70D->f2=({struct _tagged_arr*_tmp70E=_cycalloc(sizeof(*
_tmp70E));_tmp70E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp70E;});_tmp70D;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp710;_tmp710.tag=15;_tmp710.f1=(void*)t;_tmp710;});_tmp70F;}),0);_tmp70C.f3=0;
_tmp70C.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp70C;});_tmp70B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp70A;});
_tmp709;});break;}case 236: _LL2AF: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp71A="resetable";
_tag_arr(_tmp71A,sizeof(char),_get_zero_arr_size(_tmp71A,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp71B="expecting [resetable]";_tag_arr(_tmp71B,sizeof(char),
_get_zero_arr_size(_tmp71B,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp71C="`H";_tag_arr(_tmp71C,sizeof(char),
_get_zero_arr_size(_tmp71C,3));}))== 0)Cyc_Parse_err(({const char*_tmp71D="bad occurrence of heap region `H";
_tag_arr(_tmp71D,sizeof(char),_get_zero_arr_size(_tmp71D,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728->name=({
struct _tagged_arr*_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp72B;});_tmp728->identity=
0;_tmp728->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp729=_cycalloc(
sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp72A;_tmp72A.tag=0;
_tmp72A.f1=(void*)((void*)3);_tmp72A;});_tmp729;}));_tmp728;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726[0]=({
struct Cyc_Absyn_VarType_struct _tmp727;_tmp727.tag=1;_tmp727.f1=tv;_tmp727;});
_tmp726;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp71E=_cycalloc(sizeof(*
_tmp71E));_tmp71E[0]=({struct Cyc_Stmt_tok_struct _tmp71F;_tmp71F.tag=Cyc_Stmt_tok;
_tmp71F.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp720=
_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Absyn_Region_s_struct _tmp721;
_tmp721.tag=18;_tmp721.f1=tv;_tmp721.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->f1=(void*)0;_tmp722->f2=({struct
_tagged_arr*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp723;});_tmp722;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp725;_tmp725.tag=15;_tmp725.f1=(
void*)t;_tmp725;});_tmp724;}),0);_tmp721.f3=1;_tmp721.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp721;});_tmp720;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp71F;});
_tmp71E;});break;}case 237: _LL2B0: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp72C="resetable";
_tag_arr(_tmp72C,sizeof(char),_get_zero_arr_size(_tmp72C,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp72D="expecting `resetable'";_tag_arr(_tmp72D,sizeof(char),
_get_zero_arr_size(_tmp72D,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp72E="H";_tag_arr(_tmp72E,sizeof(char),
_get_zero_arr_size(_tmp72E,2));}))== 0)Cyc_Parse_err(({const char*_tmp72F="bad occurrence of heap region `H";
_tag_arr(_tmp72F,sizeof(char),_get_zero_arr_size(_tmp72F,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A->name=({
struct _tagged_arr*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp740;_tmp740.tag=0;_tmp740.f1=(struct
_tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp73E[1]={& _tmp740};Cyc_aprintf(({const char*_tmp73F="`%s";
_tag_arr(_tmp73F,sizeof(char),_get_zero_arr_size(_tmp73F,4));}),_tag_arr(_tmp73E,
sizeof(void*),1));}});_tmp73D;});_tmp73A->identity=0;_tmp73A->kind=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp73C;_tmp73C.tag=0;_tmp73C.f1=(void*)((void*)3);
_tmp73C;});_tmp73B;}));_tmp73A;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738[0]=({struct Cyc_Absyn_VarType_struct
_tmp739;_tmp739.tag=1;_tmp739.f1=tv;_tmp739;});_tmp738;});yyval=(void*)({struct
Cyc_Stmt_tok_struct*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Stmt_tok_struct
_tmp731;_tmp731.tag=Cyc_Stmt_tok;_tmp731.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({
struct Cyc_Absyn_Region_s_struct _tmp733;_tmp733.tag=18;_tmp733.f1=tv;_tmp733.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp734=_cycalloc(sizeof(*_tmp734));
_tmp734->f1=(void*)0;_tmp734->f2=({struct _tagged_arr*_tmp735=_cycalloc(sizeof(*
_tmp735));_tmp735[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp735;});_tmp734;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp737;_tmp737.tag=15;_tmp737.f1=(void*)t;_tmp737;});_tmp736;}),0);_tmp733.f3=1;
_tmp733.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp733;});_tmp732;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp731;});
_tmp730;});break;}case 238: _LL2B1: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Stmt_tok_struct
_tmp742;_tmp742.tag=Cyc_Stmt_tok;_tmp742.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp744;_tmp744.tag=20;_tmp744.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp744;});_tmp743;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp742;});_tmp741;});break;case 239: _LL2B2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[0]=({
struct Cyc_Stmt_tok_struct _tmp746;_tmp746.tag=Cyc_Stmt_tok;_tmp746.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp748;_tmp748.tag=13;_tmp748.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp748;});_tmp747;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp746;});_tmp745;});break;case 240: _LL2B3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[0]=({
struct Cyc_Stmt_tok_struct _tmp74A;_tmp74A.tag=Cyc_Stmt_tok;_tmp74A.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));
_tmp74B[0]=({struct Cyc_Absyn_Splice_s_struct _tmp74C;_tmp74C.tag=14;_tmp74C.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp74C;});_tmp74B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74A;});
_tmp749;});break;case 241: _LL2B4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp74D=
_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Stmt_tok_struct _tmp74E;
_tmp74E.tag=Cyc_Stmt_tok;_tmp74E.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=({struct Cyc_Absyn_Label_s_struct
_tmp750;_tmp750.tag=15;_tmp750.f1=({struct _tagged_arr*_tmp751=_cycalloc(sizeof(*
_tmp751));_tmp751[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp751;});_tmp750.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp750;});_tmp74F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74E;});
_tmp74D;});break;case 242: _LL2B5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp752=
_cycalloc(sizeof(*_tmp752));_tmp752[0]=({struct Cyc_Stmt_tok_struct _tmp753;
_tmp753.tag=Cyc_Stmt_tok;_tmp753.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp753;});
_tmp752;});break;case 243: _LL2B6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp754=
_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Stmt_tok_struct _tmp755;
_tmp755.tag=Cyc_Stmt_tok;_tmp755.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp755;});
_tmp754;});break;case 244: _LL2B7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp756=
_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Stmt_tok_struct _tmp757;
_tmp757.tag=Cyc_Stmt_tok;_tmp757.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp757;});
_tmp756;});break;case 245: _LL2B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 246: _LL2B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758[0]=({struct Cyc_Stmt_tok_struct
_tmp759;_tmp759.tag=Cyc_Stmt_tok;_tmp759.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp759;});_tmp758;});break;case 247: _LL2BA: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A[0]=({struct Cyc_Stmt_tok_struct
_tmp75B;_tmp75B.tag=Cyc_Stmt_tok;_tmp75B.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp75B;});_tmp75A;});
break;case 248: _LL2BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 249: _LL2BC: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Stmt_tok_struct
_tmp75D;_tmp75D.tag=Cyc_Stmt_tok;_tmp75D.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75D;});
_tmp75C;});break;case 250: _LL2BD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Stmt_tok_struct _tmp75F;
_tmp75F.tag=Cyc_Stmt_tok;_tmp75F.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp761;_tmp761.tag=1;_tmp761.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp761;});_tmp760;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp75F;});_tmp75E;});break;
case 251: _LL2BE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp762=_cycalloc(
sizeof(*_tmp762));_tmp762[0]=({struct Cyc_Stmt_tok_struct _tmp763;_tmp763.tag=Cyc_Stmt_tok;
_tmp763.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp765;_tmp765.tag=1;_tmp765.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp765;});_tmp764;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp763;});_tmp762;});
break;case 252: _LL2BF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp766=_cycalloc(
sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Stmt_tok_struct _tmp767;_tmp767.tag=Cyc_Stmt_tok;
_tmp767.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp767;});
_tmp766;});break;case 253: _LL2C0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp768=
_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Stmt_tok_struct _tmp769;
_tmp769.tag=Cyc_Stmt_tok;_tmp769.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp769;});
_tmp768;});break;case 254: _LL2C1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_Stmt_tok_struct _tmp76B;
_tmp76B.tag=Cyc_Stmt_tok;_tmp76B.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76B;});
_tmp76A;});break;case 255: _LL2C2: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp76C="C";
_tag_arr(_tmp76C,sizeof(char),_get_zero_arr_size(_tmp76C,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp76D="only switch \"C\" { ... } is allowed";_tag_arr(_tmp76D,
sizeof(char),_get_zero_arr_size(_tmp76D,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({
struct Cyc_Stmt_tok_struct _tmp76F;_tmp76F.tag=Cyc_Stmt_tok;_tmp76F.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp770=_cycalloc(sizeof(*_tmp770));
_tmp770[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp771;_tmp771.tag=10;_tmp771.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp771.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp771;});_tmp770;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76F;});
_tmp76E;});break;case 256: _LL2C3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp772=
_cycalloc(sizeof(*_tmp772));_tmp772[0]=({struct Cyc_Stmt_tok_struct _tmp773;
_tmp773.tag=Cyc_Stmt_tok;_tmp773.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp773;});
_tmp772;});break;case 257: _LL2C4: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp775;_tmp775.tag=Cyc_SwitchClauseList_tok;_tmp775.f1=0;_tmp775;});_tmp774;});
break;case 258: _LL2C5: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp777;_tmp777.tag=Cyc_SwitchClauseList_tok;_tmp777.f1=({struct Cyc_List_List*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778->hd=({struct Cyc_Absyn_Switch_clause*
_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp779->pat_vars=0;_tmp779->where_clause=0;
_tmp779->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp779->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp779;});
_tmp778->tl=0;_tmp778;});_tmp777;});_tmp776;});break;case 259: _LL2C6: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp77A=_cycalloc(sizeof(*_tmp77A));
_tmp77A[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp77B;_tmp77B.tag=Cyc_SwitchClauseList_tok;
_tmp77B.f1=({struct Cyc_List_List*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C->hd=({
struct Cyc_Absyn_Switch_clause*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp77D->pat_vars=0;_tmp77D->where_clause=0;_tmp77D->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp77D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77D;});
_tmp77C->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp77C;});_tmp77B;});_tmp77A;});break;case 260: _LL2C7:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp77E=_cycalloc(sizeof(*
_tmp77E));_tmp77E[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp77F;_tmp77F.tag=
Cyc_SwitchClauseList_tok;_tmp77F.f1=({struct Cyc_List_List*_tmp780=_cycalloc(
sizeof(*_tmp780));_tmp780->hd=({struct Cyc_Absyn_Switch_clause*_tmp781=_cycalloc(
sizeof(*_tmp781));_tmp781->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp781->pat_vars=0;
_tmp781->where_clause=0;_tmp781->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp781->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp781;});
_tmp780->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp780;});_tmp77F;});_tmp77E;});break;case 261: _LL2C8:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp782=_cycalloc(sizeof(*
_tmp782));_tmp782[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp783;_tmp783.tag=
Cyc_SwitchClauseList_tok;_tmp783.f1=({struct Cyc_List_List*_tmp784=_cycalloc(
sizeof(*_tmp784));_tmp784->hd=({struct Cyc_Absyn_Switch_clause*_tmp785=_cycalloc(
sizeof(*_tmp785));_tmp785->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp785->pat_vars=0;
_tmp785->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp785->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp785->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp785;});
_tmp784->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp784;});_tmp783;});_tmp782;});break;case 262: _LL2C9:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp786=_cycalloc(sizeof(*
_tmp786));_tmp786[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp787;_tmp787.tag=
Cyc_SwitchClauseList_tok;_tmp787.f1=({struct Cyc_List_List*_tmp788=_cycalloc(
sizeof(*_tmp788));_tmp788->hd=({struct Cyc_Absyn_Switch_clause*_tmp789=_cycalloc(
sizeof(*_tmp789));_tmp789->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp789->pat_vars=0;
_tmp789->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp789->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp789->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp789;});
_tmp788->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp788;});_tmp787;});_tmp786;});break;case 263: _LL2CA:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp78A=_cycalloc(sizeof(*
_tmp78A));_tmp78A[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp78B;_tmp78B.tag=
Cyc_SwitchCClauseList_tok;_tmp78B.f1=0;_tmp78B;});_tmp78A;});break;case 264:
_LL2CB: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp78C=_cycalloc(
sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp78D;
_tmp78D.tag=Cyc_SwitchCClauseList_tok;_tmp78D.f1=({struct Cyc_List_List*_tmp78E=
_cycalloc(sizeof(*_tmp78E));_tmp78E->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp78F=
_cycalloc(sizeof(*_tmp78F));_tmp78F->cnst_exp=0;_tmp78F->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp78F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp78F;});
_tmp78E->tl=0;_tmp78E;});_tmp78D;});_tmp78C;});break;case 265: _LL2CC: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp790=_cycalloc(sizeof(*_tmp790));
_tmp790[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp791;_tmp791.tag=Cyc_SwitchCClauseList_tok;
_tmp791.f1=({struct Cyc_List_List*_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp793->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp793->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp793;});
_tmp792->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp792;});_tmp791;});_tmp790;});break;case 266: _LL2CD:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp794=_cycalloc(sizeof(*
_tmp794));_tmp794[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp795;_tmp795.tag=
Cyc_SwitchCClauseList_tok;_tmp795.f1=({struct Cyc_List_List*_tmp796=_cycalloc(
sizeof(*_tmp796));_tmp796->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp797=_cycalloc(
sizeof(*_tmp797));_tmp797->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp797->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp797->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp797;});
_tmp796->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp796;});_tmp795;});_tmp794;});break;case 267: _LL2CE:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp798=_cycalloc(sizeof(*_tmp798));
_tmp798[0]=({struct Cyc_Stmt_tok_struct _tmp799;_tmp799.tag=Cyc_Stmt_tok;_tmp799.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp799;});_tmp798;});break;case 268: _LL2CF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({
struct Cyc_Stmt_tok_struct _tmp79B;_tmp79B.tag=Cyc_Stmt_tok;_tmp79B.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp79B;});_tmp79A;});break;case 269: _LL2D0: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C[0]=({
struct Cyc_Stmt_tok_struct _tmp79D;_tmp79D.tag=Cyc_Stmt_tok;_tmp79D.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp79D;});
_tmp79C;});break;case 270: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp79E=
_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=({struct Cyc_Stmt_tok_struct _tmp79F;
_tmp79F.tag=Cyc_Stmt_tok;_tmp79F.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp79F;});
_tmp79E;});break;case 271: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A0=
_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Stmt_tok_struct _tmp7A1;
_tmp7A1.tag=Cyc_Stmt_tok;_tmp7A1.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A1;});
_tmp7A0;});break;case 272: _LL2D3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Stmt_tok_struct _tmp7A3;
_tmp7A3.tag=Cyc_Stmt_tok;_tmp7A3.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A3;});
_tmp7A2;});break;case 273: _LL2D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Stmt_tok_struct _tmp7A5;
_tmp7A5.tag=Cyc_Stmt_tok;_tmp7A5.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A5;});
_tmp7A4;});break;case 274: _LL2D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A6=
_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Stmt_tok_struct _tmp7A7;
_tmp7A7.tag=Cyc_Stmt_tok;_tmp7A7.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A7;});
_tmp7A6;});break;case 275: _LL2D6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A8=
_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Stmt_tok_struct _tmp7A9;
_tmp7A9.tag=Cyc_Stmt_tok;_tmp7A9.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A9;});
_tmp7A8;});break;case 276: _LL2D7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AA=
_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Stmt_tok_struct _tmp7AB;
_tmp7AB.tag=Cyc_Stmt_tok;_tmp7AB.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7AB;});
_tmp7AA;});break;case 277: _LL2D8: {struct Cyc_List_List*_tmp7AC=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp7AD=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AE=
_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Stmt_tok_struct _tmp7AF;
_tmp7AF.tag=Cyc_Stmt_tok;_tmp7AF.f1=Cyc_Parse_flatten_declarations(_tmp7AC,
_tmp7AD);_tmp7AF;});_tmp7AE;});break;}case 278: _LL2D9: {struct Cyc_List_List*
_tmp7B0=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7B1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({
struct Cyc_Stmt_tok_struct _tmp7B3;_tmp7B3.tag=Cyc_Stmt_tok;_tmp7B3.f1=Cyc_Parse_flatten_declarations(
_tmp7B0,_tmp7B1);_tmp7B3;});_tmp7B2;});break;}case 279: _LL2DA: {struct Cyc_List_List*
_tmp7B4=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7B5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({
struct Cyc_Stmt_tok_struct _tmp7B7;_tmp7B7.tag=Cyc_Stmt_tok;_tmp7B7.f1=Cyc_Parse_flatten_declarations(
_tmp7B4,_tmp7B5);_tmp7B7;});_tmp7B6;});break;}case 280: _LL2DB: {struct Cyc_List_List*
_tmp7B8=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp7B9=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=({
struct Cyc_Stmt_tok_struct _tmp7BB;_tmp7BB.tag=Cyc_Stmt_tok;_tmp7BB.f1=Cyc_Parse_flatten_declarations(
_tmp7B8,_tmp7B9);_tmp7BB;});_tmp7BA;});break;}case 281: _LL2DC: {struct Cyc_List_List*
_tmp7BC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({
struct Cyc_Stmt_tok_struct _tmp7BE;_tmp7BE.tag=Cyc_Stmt_tok;_tmp7BE.f1=Cyc_Absyn_forarray_stmt(
_tmp7BC,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7BE;});_tmp7BD;});break;}case 282: _LL2DD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({
struct Cyc_Stmt_tok_struct _tmp7C0;_tmp7C0.tag=Cyc_Stmt_tok;_tmp7C0.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7C0;});
_tmp7BF;});break;case 283: _LL2DE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C2=
_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Stmt_tok_struct _tmp7C3;
_tmp7C3.tag=Cyc_Stmt_tok;_tmp7C3.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7C3;});
_tmp7C2;});break;case 284: _LL2DF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C4=
_cycalloc(sizeof(*_tmp7C4));_tmp7C4[0]=({struct Cyc_Stmt_tok_struct _tmp7C5;
_tmp7C5.tag=Cyc_Stmt_tok;_tmp7C5.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7C5;});
_tmp7C4;});break;case 285: _LL2E0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C6=
_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Stmt_tok_struct _tmp7C7;
_tmp7C7.tag=Cyc_Stmt_tok;_tmp7C7.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7C7;});
_tmp7C6;});break;case 286: _LL2E1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C8=
_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Stmt_tok_struct _tmp7C9;
_tmp7C9.tag=Cyc_Stmt_tok;_tmp7C9.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7C9;});_tmp7C8;});break;case 287: _LL2E2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({
struct Cyc_Stmt_tok_struct _tmp7CB;_tmp7CB.tag=Cyc_Stmt_tok;_tmp7CB.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7CB;});_tmp7CA;});break;case 288: _LL2E3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({
struct Cyc_Stmt_tok_struct _tmp7CD;_tmp7CD.tag=Cyc_Stmt_tok;_tmp7CD.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp7CD;});_tmp7CC;});break;case 289: _LL2E4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=({
struct Cyc_Stmt_tok_struct _tmp7CF;_tmp7CF.tag=Cyc_Stmt_tok;_tmp7CF.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp7CF;});_tmp7CE;});break;case 290: _LL2E5: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[
0]=({struct Cyc_Pattern_tok_struct _tmp7D1;_tmp7D1.tag=Cyc_Pattern_tok;_tmp7D1.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D1;});
_tmp7D0;});break;case 291: _LL2E6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 292: _LL2E7: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Pattern_tok_struct
_tmp7D3;_tmp7D3.tag=Cyc_Pattern_tok;_tmp7D3.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({struct
Cyc_Absyn_Int_p_struct _tmp7D5;_tmp7D5.tag=1;_tmp7D5.f1=(void*)(*((struct _tuple13*)
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f1;
_tmp7D5.f2=(*((struct _tuple13*)Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))).f2;_tmp7D5;});_tmp7D4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7D3;});_tmp7D2;});break;case 293: _LL2E8: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));_tmp7D6[
0]=({struct Cyc_Pattern_tok_struct _tmp7D7;_tmp7D7.tag=Cyc_Pattern_tok;_tmp7D7.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp7D8=_cycalloc(
sizeof(*_tmp7D8));_tmp7D8[0]=({struct Cyc_Absyn_Int_p_struct _tmp7D9;_tmp7D9.tag=1;
_tmp7D9.f1=(void*)((void*)0);_tmp7D9.f2=-(*((struct _tuple13*)Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f2;_tmp7D9;});
_tmp7D8;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7D7;});_tmp7D6;});break;case 294: _LL2E9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[
0]=({struct Cyc_Pattern_tok_struct _tmp7DB;_tmp7DB.tag=Cyc_Pattern_tok;_tmp7DB.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp7DC=_cycalloc(
sizeof(*_tmp7DC));_tmp7DC[0]=({struct Cyc_Absyn_Float_p_struct _tmp7DD;_tmp7DD.tag=
3;_tmp7DD.f1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7DD;});_tmp7DC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DB;});
_tmp7DA;});break;case 295: _LL2EA: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7DE=_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({struct Cyc_Pattern_tok_struct
_tmp7DF;_tmp7DF.tag=Cyc_Pattern_tok;_tmp7DF.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp7E0=_cycalloc_atomic(sizeof(*_tmp7E0));_tmp7E0[0]=({
struct Cyc_Absyn_Char_p_struct _tmp7E1;_tmp7E1.tag=2;_tmp7E1.f1=Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E1;});_tmp7E0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7DF;});_tmp7DE;});break;case 296: _LL2EB: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[
0]=({struct Cyc_Pattern_tok_struct _tmp7E3;_tmp7E3.tag=Cyc_Pattern_tok;_tmp7E3.f1=
Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E3;});
_tmp7E2;});break;case 297: _LL2EC: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Pattern_tok_struct
_tmp7E5;_tmp7E5.tag=Cyc_Pattern_tok;_tmp7E5.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp7E7;_tmp7E7.tag=11;_tmp7E7.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E5;});_tmp7E4;});break;case 298: _LL2ED: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[
0]=({struct Cyc_Pattern_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_Pattern_tok;_tmp7E9.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7EA=_cycalloc(
sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7EB;_tmp7EB.tag=
4;_tmp7EB.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]));_tmp7EB;});_tmp7EA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E9;});
_tmp7E8;});break;case 299: _LL2EE: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Pattern_tok_struct
_tmp7ED;_tmp7ED.tag=Cyc_Pattern_tok;_tmp7ED.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownCall_p_struct*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp7EF;_tmp7EF.tag=12;_tmp7EF.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7EF.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7EF;});_tmp7EE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7ED;});_tmp7EC;});break;case 300: _LL2EF: {struct
Cyc_List_List*_tmp7F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({
struct Cyc_Pattern_tok_struct _tmp7F2;_tmp7F2.tag=Cyc_Pattern_tok;_tmp7F2.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));
_tmp7F3[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7F4;_tmp7F4.tag=7;_tmp7F4.f1=({
struct Cyc_Absyn_AggrInfo _tmp7F5;_tmp7F5.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=(void*)((void*)0);_tmp7F7.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7F7;});_tmp7F6;}));
_tmp7F5.targs=0;_tmp7F5;});_tmp7F4.f2=_tmp7F0;_tmp7F4.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7F4;});_tmp7F3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F2;});_tmp7F1;});break;}case 301: _LL2F0: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[
0]=({struct Cyc_Pattern_tok_struct _tmp7F9;_tmp7F9.tag=Cyc_Pattern_tok;_tmp7F9.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7FA=_cycalloc(
sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7FB;_tmp7FB.tag=
5;_tmp7FB.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7FB;});_tmp7FA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F9;});
_tmp7F8;});break;case 302: _LL2F1: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=({struct Cyc_Pattern_tok_struct
_tmp7FD;_tmp7FD.tag=Cyc_Pattern_tok;_tmp7FD.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp7FF;_tmp7FF.tag=6;_tmp7FF.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800->f1=(void*)0;_tmp800->f2=({
struct _tagged_arr*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp801;});_tmp800;}),(
void*)0,0);_tmp7FF;});_tmp7FE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FD;});
_tmp7FC;});break;case 303: _LL2F2: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_PatternList_tok_struct
_tmp803;_tmp803.tag=Cyc_PatternList_tok;_tmp803.f1=({struct Cyc_List_List*_tmp804=
_cycalloc(sizeof(*_tmp804));_tmp804->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp804->tl=0;_tmp804;});
_tmp803;});_tmp802;});break;case 304: _LL2F3: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=({struct Cyc_PatternList_tok_struct
_tmp806;_tmp806.tag=Cyc_PatternList_tok;_tmp806.f1=({struct Cyc_List_List*_tmp807=
_cycalloc(sizeof(*_tmp807));_tmp807->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp807->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp807;});_tmp806;});
_tmp805;});break;case 305: _LL2F4: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_FieldPattern_tok_struct
_tmp809;_tmp809.tag=Cyc_FieldPattern_tok;_tmp809.f1=({struct _tuple11*_tmp80A=
_cycalloc(sizeof(*_tmp80A));_tmp80A->f1=0;_tmp80A->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80A;});_tmp809;});
_tmp808;});break;case 306: _LL2F5: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_FieldPattern_tok_struct
_tmp80C;_tmp80C.tag=Cyc_FieldPattern_tok;_tmp80C.f1=({struct _tuple11*_tmp80D=
_cycalloc(sizeof(*_tmp80D));_tmp80D->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80D->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80D;});_tmp80C;});
_tmp80B;});break;case 307: _LL2F6: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp80F;_tmp80F.tag=Cyc_FieldPatternList_tok;_tmp80F.f1=({struct Cyc_List_List*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp810->tl=0;_tmp810;});
_tmp80F;});_tmp80E;});break;case 308: _LL2F7: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp812;_tmp812.tag=Cyc_FieldPatternList_tok;_tmp812.f1=({struct Cyc_List_List*
_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp813->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp813;});_tmp812;});
_tmp811;});break;case 309: _LL2F8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 310: _LL2F9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Exp_tok_struct _tmp815;
_tmp815.tag=Cyc_Exp_tok;_tmp815.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp815;});
_tmp814;});break;case 311: _LL2FA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 312: _LL2FB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Exp_tok_struct _tmp817;
_tmp817.tag=Cyc_Exp_tok;_tmp817.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp817;});
_tmp816;});break;case 313: _LL2FC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Primopopt_tok_struct
_tmp819;_tmp819.tag=Cyc_Primopopt_tok;_tmp819.f1=0;_tmp819;});_tmp818;});break;
case 314: _LL2FD: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp81A=_cycalloc(
sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Primopopt_tok_struct _tmp81B;_tmp81B.tag=
Cyc_Primopopt_tok;_tmp81B.f1=({struct Cyc_Core_Opt*_tmp81C=_cycalloc(sizeof(*
_tmp81C));_tmp81C->v=(void*)((void*)1);_tmp81C;});_tmp81B;});_tmp81A;});break;
case 315: _LL2FE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp81D=_cycalloc(
sizeof(*_tmp81D));_tmp81D[0]=({struct Cyc_Primopopt_tok_struct _tmp81E;_tmp81E.tag=
Cyc_Primopopt_tok;_tmp81E.f1=({struct Cyc_Core_Opt*_tmp81F=_cycalloc(sizeof(*
_tmp81F));_tmp81F->v=(void*)((void*)3);_tmp81F;});_tmp81E;});_tmp81D;});break;
case 316: _LL2FF: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp820=_cycalloc(
sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Primopopt_tok_struct _tmp821;_tmp821.tag=
Cyc_Primopopt_tok;_tmp821.f1=({struct Cyc_Core_Opt*_tmp822=_cycalloc(sizeof(*
_tmp822));_tmp822->v=(void*)((void*)4);_tmp822;});_tmp821;});_tmp820;});break;
case 317: _LL300: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp823=_cycalloc(
sizeof(*_tmp823));_tmp823[0]=({struct Cyc_Primopopt_tok_struct _tmp824;_tmp824.tag=
Cyc_Primopopt_tok;_tmp824.f1=({struct Cyc_Core_Opt*_tmp825=_cycalloc(sizeof(*
_tmp825));_tmp825->v=(void*)((void*)0);_tmp825;});_tmp824;});_tmp823;});break;
case 318: _LL301: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Primopopt_tok_struct _tmp827;_tmp827.tag=
Cyc_Primopopt_tok;_tmp827.f1=({struct Cyc_Core_Opt*_tmp828=_cycalloc(sizeof(*
_tmp828));_tmp828->v=(void*)((void*)2);_tmp828;});_tmp827;});_tmp826;});break;
case 319: _LL302: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp829=_cycalloc(
sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Primopopt_tok_struct _tmp82A;_tmp82A.tag=
Cyc_Primopopt_tok;_tmp82A.f1=({struct Cyc_Core_Opt*_tmp82B=_cycalloc(sizeof(*
_tmp82B));_tmp82B->v=(void*)((void*)16);_tmp82B;});_tmp82A;});_tmp829;});break;
case 320: _LL303: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp82C=_cycalloc(
sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Primopopt_tok_struct _tmp82D;_tmp82D.tag=
Cyc_Primopopt_tok;_tmp82D.f1=({struct Cyc_Core_Opt*_tmp82E=_cycalloc(sizeof(*
_tmp82E));_tmp82E->v=(void*)((void*)17);_tmp82E;});_tmp82D;});_tmp82C;});break;
case 321: _LL304: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp82F=_cycalloc(
sizeof(*_tmp82F));_tmp82F[0]=({struct Cyc_Primopopt_tok_struct _tmp830;_tmp830.tag=
Cyc_Primopopt_tok;_tmp830.f1=({struct Cyc_Core_Opt*_tmp831=_cycalloc(sizeof(*
_tmp831));_tmp831->v=(void*)((void*)13);_tmp831;});_tmp830;});_tmp82F;});break;
case 322: _LL305: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp832=_cycalloc(
sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Primopopt_tok_struct _tmp833;_tmp833.tag=
Cyc_Primopopt_tok;_tmp833.f1=({struct Cyc_Core_Opt*_tmp834=_cycalloc(sizeof(*
_tmp834));_tmp834->v=(void*)((void*)15);_tmp834;});_tmp833;});_tmp832;});break;
case 323: _LL306: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp835=_cycalloc(
sizeof(*_tmp835));_tmp835[0]=({struct Cyc_Primopopt_tok_struct _tmp836;_tmp836.tag=
Cyc_Primopopt_tok;_tmp836.f1=({struct Cyc_Core_Opt*_tmp837=_cycalloc(sizeof(*
_tmp837));_tmp837->v=(void*)((void*)14);_tmp837;});_tmp836;});_tmp835;});break;
case 324: _LL307: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 325: _LL308: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp838=_cycalloc(
sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Exp_tok_struct _tmp839;_tmp839.tag=Cyc_Exp_tok;
_tmp839.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp839;});
_tmp838;});break;case 326: _LL309: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83A=
_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Exp_tok_struct _tmp83B;_tmp83B.tag=
Cyc_Exp_tok;_tmp83B.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83B;});
_tmp83A;});break;case 327: _LL30A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83C=
_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Exp_tok_struct _tmp83D;_tmp83D.tag=
Cyc_Exp_tok;_tmp83D.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
_tmp83C;});break;case 328: _LL30B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83E=
_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Exp_tok_struct _tmp83F;_tmp83F.tag=
Cyc_Exp_tok;_tmp83F.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
_tmp83E;});break;case 329: _LL30C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp840=
_cycalloc(sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Exp_tok_struct _tmp841;_tmp841.tag=
Cyc_Exp_tok;_tmp841.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
_tmp840;});break;case 330: _LL30D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp842=
_cycalloc(sizeof(*_tmp842));_tmp842[0]=({struct Cyc_Exp_tok_struct _tmp843;_tmp843.tag=
Cyc_Exp_tok;_tmp843.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
_tmp842;});break;case 331: _LL30E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 332: _LL30F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 333: _LL310: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Exp_tok_struct _tmp845;
_tmp845.tag=Cyc_Exp_tok;_tmp845.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
_tmp844;});break;case 334: _LL311: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL312: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Exp_tok_struct _tmp847;
_tmp847.tag=Cyc_Exp_tok;_tmp847.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
_tmp846;});break;case 336: _LL313: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL314: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848[0]=({struct Cyc_Exp_tok_struct _tmp849;
_tmp849.tag=Cyc_Exp_tok;_tmp849.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
_tmp848;});break;case 338: _LL315: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 339: _LL316: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({struct Cyc_Exp_tok_struct _tmp84B;
_tmp84B.tag=Cyc_Exp_tok;_tmp84B.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
_tmp84A;});break;case 340: _LL317: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL318: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_Exp_tok_struct _tmp84D;
_tmp84D.tag=Cyc_Exp_tok;_tmp84D.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
_tmp84C;});break;case 342: _LL319: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 343: _LL31A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({struct Cyc_Exp_tok_struct _tmp84F;
_tmp84F.tag=Cyc_Exp_tok;_tmp84F.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
_tmp84E;});break;case 344: _LL31B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp850=
_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_Exp_tok_struct _tmp851;_tmp851.tag=
Cyc_Exp_tok;_tmp851.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
_tmp850;});break;case 345: _LL31C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL31D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Exp_tok_struct _tmp853;
_tmp853.tag=Cyc_Exp_tok;_tmp853.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
_tmp852;});break;case 347: _LL31E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp854=
_cycalloc(sizeof(*_tmp854));_tmp854[0]=({struct Cyc_Exp_tok_struct _tmp855;_tmp855.tag=
Cyc_Exp_tok;_tmp855.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
_tmp854;});break;case 348: _LL31F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp856=
_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_Exp_tok_struct _tmp857;_tmp857.tag=
Cyc_Exp_tok;_tmp857.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
_tmp856;});break;case 349: _LL320: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp858=
_cycalloc(sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Exp_tok_struct _tmp859;_tmp859.tag=
Cyc_Exp_tok;_tmp859.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
_tmp858;});break;case 350: _LL321: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL322: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Exp_tok_struct _tmp85B;
_tmp85B.tag=Cyc_Exp_tok;_tmp85B.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
_tmp85A;});break;case 352: _LL323: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp85C=
_cycalloc(sizeof(*_tmp85C));_tmp85C[0]=({struct Cyc_Exp_tok_struct _tmp85D;_tmp85D.tag=
Cyc_Exp_tok;_tmp85D.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
_tmp85C;});break;case 353: _LL324: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 354: _LL325: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp85E=_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_Exp_tok_struct _tmp85F;
_tmp85F.tag=Cyc_Exp_tok;_tmp85F.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
_tmp85E;});break;case 355: _LL326: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Exp_tok_struct _tmp861;_tmp861.tag=
Cyc_Exp_tok;_tmp861.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
_tmp860;});break;case 356: _LL327: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 357: _LL328: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Exp_tok_struct _tmp863;
_tmp863.tag=Cyc_Exp_tok;_tmp863.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
_tmp862;});break;case 358: _LL329: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp864=
_cycalloc(sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Exp_tok_struct _tmp865;_tmp865.tag=
Cyc_Exp_tok;_tmp865.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
_tmp864;});break;case 359: _LL32A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Exp_tok_struct _tmp867;_tmp867.tag=
Cyc_Exp_tok;_tmp867.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
_tmp866;});break;case 360: _LL32B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 361: _LL32C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868[0]=({struct Cyc_Exp_tok_struct _tmp869;
_tmp869.tag=Cyc_Exp_tok;_tmp869.f1=Cyc_Absyn_cast_exp((*((struct _tuple2*)Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
_tmp868;});break;case 362: _LL32D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 363: _LL32E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A[0]=({struct Cyc_Exp_tok_struct _tmp86B;
_tmp86B.tag=Cyc_Exp_tok;_tmp86B.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
_tmp86A;});break;case 364: _LL32F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp86C=
_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({struct Cyc_Exp_tok_struct _tmp86D;_tmp86D.tag=
Cyc_Exp_tok;_tmp86D.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86D;});
_tmp86C;});break;case 365: _LL330: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp86E=
_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Exp_tok_struct _tmp86F;_tmp86F.tag=
Cyc_Exp_tok;_tmp86F.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86F;});
_tmp86E;});break;case 366: _LL331: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp870=
_cycalloc(sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Exp_tok_struct _tmp871;_tmp871.tag=
Cyc_Exp_tok;_tmp871.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
_tmp870;});break;case 367: _LL332: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 368: _LL333: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Exp_tok_struct _tmp873;
_tmp873.tag=Cyc_Exp_tok;_tmp873.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
_tmp872;});break;case 369: _LL334: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp874=
_cycalloc(sizeof(*_tmp874));_tmp874[0]=({struct Cyc_Exp_tok_struct _tmp875;_tmp875.tag=
Cyc_Exp_tok;_tmp875.f1=Cyc_Absyn_sizeoftyp_exp((*((struct _tuple2*)Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp875;});
_tmp874;});break;case 370: _LL335: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp876=
_cycalloc(sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Exp_tok_struct _tmp877;_tmp877.tag=
Cyc_Exp_tok;_tmp877.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
_tmp876;});break;case 371: _LL336: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp878=
_cycalloc(sizeof(*_tmp878));_tmp878[0]=({struct Cyc_Exp_tok_struct _tmp879;_tmp879.tag=
Cyc_Exp_tok;_tmp879.f1=Cyc_Absyn_offsetof_exp((*((struct _tuple2*)Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))).f3,(void*)({
struct Cyc_Absyn_StructField_struct*_tmp87A=_cycalloc(sizeof(*_tmp87A));_tmp87A[0]=({
struct Cyc_Absyn_StructField_struct _tmp87B;_tmp87B.tag=0;_tmp87B.f1=({struct
_tagged_arr*_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp87C;});_tmp87B;});
_tmp87A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp879;});_tmp878;});break;case 372: _LL337: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=({
struct Cyc_Exp_tok_struct _tmp87E;_tmp87E.tag=Cyc_Exp_tok;_tmp87E.f1=Cyc_Absyn_offsetof_exp((*((
struct _tuple2*)Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp87F=
_cycalloc_atomic(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp880;_tmp880.tag=1;_tmp880.f1=(unsigned int)(*((struct _tuple13*)Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f2;_tmp880;});
_tmp87F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp87E;});_tmp87D;});break;case 373: _LL338: {struct
Cyc_Position_Segment*_tmp881=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp882=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp881,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp883=_cycalloc(
sizeof(*_tmp883));_tmp883[0]=({struct Cyc_Exp_tok_struct _tmp884;_tmp884.tag=Cyc_Exp_tok;
_tmp884.f1=Cyc_Absyn_gentyp_exp(_tmp882,(*((struct _tuple2*)Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp884;});
_tmp883;});break;}case 374: _LL339: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp885=
_cycalloc(sizeof(*_tmp885));_tmp885[0]=({struct Cyc_Exp_tok_struct _tmp886;_tmp886.tag=
Cyc_Exp_tok;_tmp886.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp888;_tmp888.tag=33;_tmp888.f1=({struct Cyc_Absyn_MallocInfo _tmp889;_tmp889.is_calloc=
0;_tmp889.rgn=0;_tmp889.elt_type=0;_tmp889.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp889.fat_result=0;
_tmp889;});_tmp888;});_tmp887;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
_tmp885;});break;case 375: _LL33A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp88A=
_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=({struct Cyc_Exp_tok_struct _tmp88B;_tmp88B.tag=
Cyc_Exp_tok;_tmp88B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp88D;_tmp88D.tag=33;_tmp88D.f1=({struct Cyc_Absyn_MallocInfo _tmp88E;_tmp88E.is_calloc=
0;_tmp88E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp88E.elt_type=0;
_tmp88E.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp88E.fat_result=0;_tmp88E;});_tmp88D;});_tmp88C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
_tmp88A;});break;case 376: _LL33B: {void*_tmp890;struct _tuple2 _tmp88F=*((struct
_tuple2*)Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp890=_tmp88F.f3;yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Exp_tok_struct _tmp892;
_tmp892.tag=Cyc_Exp_tok;_tmp892.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp894;_tmp894.tag=33;_tmp894.f1=({struct Cyc_Absyn_MallocInfo _tmp895;_tmp895.is_calloc=
1;_tmp895.rgn=0;_tmp895.elt_type=({void**_tmp896=_cycalloc(sizeof(*_tmp896));
_tmp896[0]=_tmp890;_tmp896;});_tmp895.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp895.fat_result=0;
_tmp895;});_tmp894;});_tmp893;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
_tmp891;});break;}case 377: _LL33C: {void*_tmp898;struct _tuple2 _tmp897=*((struct
_tuple2*)Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp898=_tmp897.f3;yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=({struct Cyc_Exp_tok_struct _tmp89A;
_tmp89A.tag=Cyc_Exp_tok;_tmp89A.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp89C;_tmp89C.tag=33;_tmp89C.f1=({struct Cyc_Absyn_MallocInfo _tmp89D;_tmp89D.is_calloc=
1;_tmp89D.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp89D.elt_type=({void**
_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=_tmp898;_tmp89E;});_tmp89D.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp89D.fat_result=0;_tmp89D;});_tmp89C;});_tmp89B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89A;});
_tmp899;});break;}case 378: _LL33D: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Primop_tok_struct
_tmp8A0;_tmp8A0.tag=Cyc_Primop_tok;_tmp8A0.f1=(void*)((void*)12);_tmp8A0;});
_tmp89F;});break;case 379: _LL33E: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct Cyc_Primop_tok_struct
_tmp8A2;_tmp8A2.tag=Cyc_Primop_tok;_tmp8A2.f1=(void*)((void*)11);_tmp8A2;});
_tmp8A1;});break;case 380: _LL33F: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Primop_tok_struct
_tmp8A4;_tmp8A4.tag=Cyc_Primop_tok;_tmp8A4.f1=(void*)((void*)2);_tmp8A4;});
_tmp8A3;});break;case 381: _LL340: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 382: _LL341: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Exp_tok_struct _tmp8A6;
_tmp8A6.tag=Cyc_Exp_tok;_tmp8A6.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
_tmp8A5;});break;case 383: _LL342: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A7=
_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Exp_tok_struct _tmp8A8;_tmp8A8.tag=
Cyc_Exp_tok;_tmp8A8.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A8;});
_tmp8A7;});break;case 384: _LL343: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Exp_tok_struct _tmp8AA;_tmp8AA.tag=
Cyc_Exp_tok;_tmp8AA.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AA;});
_tmp8A9;});break;case 385: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8AB=
_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Exp_tok_struct _tmp8AC;_tmp8AC.tag=
Cyc_Exp_tok;_tmp8AC.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AC;});
_tmp8AB;});break;case 386: _LL345: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8AE=
_cycalloc(sizeof(*_tmp8AE));_tmp8AE[0]=({struct Cyc_Exp_tok_struct _tmp8AF;_tmp8AF.tag=
Cyc_Exp_tok;_tmp8AF.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AF;});
_tmp8AE;});break;case 387: _LL346: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B1=
_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Exp_tok_struct _tmp8B2;_tmp8B2.tag=
Cyc_Exp_tok;_tmp8B2.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
_tmp8B1;});break;case 388: _LL347: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B3=
_cycalloc(sizeof(*_tmp8B3));_tmp8B3[0]=({struct Cyc_Exp_tok_struct _tmp8B4;_tmp8B4.tag=
Cyc_Exp_tok;_tmp8B4.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B4;});
_tmp8B3;});break;case 389: _LL348: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B5=
_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Exp_tok_struct _tmp8B6;_tmp8B6.tag=
Cyc_Exp_tok;_tmp8B6.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp8B8;_tmp8B8.tag=25;_tmp8B8.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8B8.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8B8;});_tmp8B7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8B6;});_tmp8B5;});break;case 390: _LL349: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=({
struct Cyc_Exp_tok_struct _tmp8BA;_tmp8BA.tag=Cyc_Exp_tok;_tmp8BA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8BB=_cycalloc(sizeof(*_tmp8BB));
_tmp8BB[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8BC;_tmp8BC.tag=25;_tmp8BC.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp8BC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp8BC;});_tmp8BB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BA;});
_tmp8B9;});break;case 391: _LL34A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BD=
_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({struct Cyc_Exp_tok_struct _tmp8BE;_tmp8BE.tag=
Cyc_Exp_tok;_tmp8BE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp8BF=_cycalloc(sizeof(*_tmp8BF));_tmp8BF[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp8C0;_tmp8C0.tag=37;_tmp8C0.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8C0;});_tmp8BF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8BE;});_tmp8BD;});break;case 392: _LL34B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8C1=_cycalloc(sizeof(*_tmp8C1));_tmp8C1[0]=({
struct Cyc_Exp_tok_struct _tmp8C2;_tmp8C2.tag=Cyc_Exp_tok;_tmp8C2.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp8C3=_cycalloc(sizeof(*_tmp8C3));
_tmp8C3[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp8C4;_tmp8C4.tag=36;_tmp8C4.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8C4;});_tmp8C3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C2;});
_tmp8C1;});break;case 393: _LL34C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C5=
_cycalloc(sizeof(*_tmp8C5));_tmp8C5[0]=({struct Cyc_Exp_tok_struct _tmp8C6;_tmp8C6.tag=
Cyc_Exp_tok;_tmp8C6.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp8C7=_cycalloc(sizeof(*_tmp8C7));_tmp8C7[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp8C8;_tmp8C8.tag=2;_tmp8C8.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C8;});_tmp8C7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C6;});
_tmp8C5;});break;case 394: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 395: _LL34E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({struct Cyc_Exp_tok_struct _tmp8CA;
_tmp8CA.tag=Cyc_Exp_tok;_tmp8CA.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CA;});
_tmp8C9;});break;case 396: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 397: _LL350: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Exp_tok_struct _tmp8CC;
_tmp8CC.tag=Cyc_Exp_tok;_tmp8CC.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CC;});
_tmp8CB;});break;case 398: _LL351: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CD=
_cycalloc(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Exp_tok_struct _tmp8CE;_tmp8CE.tag=
Cyc_Exp_tok;_tmp8CE.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CE;});
_tmp8CD;});break;case 399: _LL352: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Exp_tok_struct _tmp8D0;_tmp8D0.tag=
Cyc_Exp_tok;_tmp8D0.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D0;});
_tmp8CF;});break;case 400: _LL353: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D1=
_cycalloc(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Exp_tok_struct _tmp8D2;_tmp8D2.tag=
Cyc_Exp_tok;_tmp8D2.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp8D4;_tmp8D4.tag=28;_tmp8D4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8D4.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8D4.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8D4.f4=0;
_tmp8D4;});_tmp8D3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D2;});
_tmp8D1;});break;case 401: _LL354: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D5=
_cycalloc(sizeof(*_tmp8D5));_tmp8D5[0]=({struct Cyc_Exp_tok_struct _tmp8D6;_tmp8D6.tag=
Cyc_Exp_tok;_tmp8D6.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D6;});
_tmp8D5;});break;case 402: _LL355: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp8D7=_cycalloc(sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_ExpList_tok_struct
_tmp8D8;_tmp8D8.tag=Cyc_ExpList_tok;_tmp8D8.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp8D8;});_tmp8D7;});
break;case 403: _LL356: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp8D9=
_cycalloc(sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_ExpList_tok_struct _tmp8DA;
_tmp8DA.tag=Cyc_ExpList_tok;_tmp8DA.f1=({struct Cyc_List_List*_tmp8DB=_cycalloc(
sizeof(*_tmp8DB));_tmp8DB->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8DB->tl=0;_tmp8DB;});
_tmp8DA;});_tmp8D9;});break;case 404: _LL357: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp8DC=_cycalloc(sizeof(*_tmp8DC));_tmp8DC[0]=({struct Cyc_ExpList_tok_struct
_tmp8DD;_tmp8DD.tag=Cyc_ExpList_tok;_tmp8DD.f1=({struct Cyc_List_List*_tmp8DE=
_cycalloc(sizeof(*_tmp8DE));_tmp8DE->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8DE->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8DE;});_tmp8DD;});
_tmp8DC;});break;case 405: _LL358: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8DF=
_cycalloc(sizeof(*_tmp8DF));_tmp8DF[0]=({struct Cyc_Exp_tok_struct _tmp8E0;_tmp8E0.tag=
Cyc_Exp_tok;_tmp8E0.f1=Cyc_Absyn_int_exp((*((struct _tuple13*)Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))).f1,(*((struct
_tuple13*)Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))).f2,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E0;});
_tmp8DF;});break;case 406: _LL359: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8E1=
_cycalloc(sizeof(*_tmp8E1));_tmp8E1[0]=({struct Cyc_Exp_tok_struct _tmp8E2;_tmp8E2.tag=
Cyc_Exp_tok;_tmp8E2.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E2;});
_tmp8E1;});break;case 407: _LL35A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8E3=
_cycalloc(sizeof(*_tmp8E3));_tmp8E3[0]=({struct Cyc_Exp_tok_struct _tmp8E4;_tmp8E4.tag=
Cyc_Exp_tok;_tmp8E4.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E4;});
_tmp8E3;});break;case 408: _LL35B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8E5=
_cycalloc(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Exp_tok_struct _tmp8E6;_tmp8E6.tag=
Cyc_Exp_tok;_tmp8E6.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E6;});
_tmp8E5;});break;case 409: _LL35C: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({struct Cyc_QualId_tok_struct
_tmp8E8;_tmp8E8.tag=Cyc_QualId_tok;_tmp8E8.f1=({struct _tuple1*_tmp8E9=_cycalloc(
sizeof(*_tmp8E9));_tmp8E9->f1=Cyc_Absyn_rel_ns_null;_tmp8E9->f2=({struct
_tagged_arr*_tmp8EA=_cycalloc(sizeof(*_tmp8EA));_tmp8EA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8EA;});_tmp8E9;});
_tmp8E8;});_tmp8E7;});break;case 410: _LL35D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 411: _LL35E: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp8EB=_cycalloc(sizeof(*_tmp8EB));_tmp8EB[
0]=({struct Cyc_QualId_tok_struct _tmp8EC;_tmp8EC.tag=Cyc_QualId_tok;_tmp8EC.f1=({
struct _tuple1*_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED->f1=Cyc_Absyn_rel_ns_null;
_tmp8ED->f2=({struct _tagged_arr*_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp8EE;});_tmp8ED;});_tmp8EC;});_tmp8EB;});break;case 412: _LL35F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 413: _LL360: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 414: _LL361:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 415:
_LL362: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
416: _LL363: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 417: _LL364: break;case 418: _LL365:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL366: break;}
yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(419,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(115,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0?yystate <= 
5408: 0)?Cyc_yycheck[_check_known_subscript_notnull(5409,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)]: 0)yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5409,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(115,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(860,yystate)];if(yyn > - 32768?yyn < 5408: 0){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 246 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5409,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(246,x)])
+ 15,count ++);}msg=({unsigned int _tmp8EF=(unsigned int)(sze + 15);char*_tmp8F0=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp8EF + 1));struct _tagged_arr
_tmp8F2=_tag_arr(_tmp8F0,sizeof(char),_tmp8EF + 1);{unsigned int _tmp8F1=_tmp8EF;
unsigned int i;for(i=0;i < _tmp8F1;i ++){_tmp8F0[i]='\000';}_tmp8F0[_tmp8F1]=(char)
0;}_tmp8F2;});Cyc_strcpy(msg,({const char*_tmp8F3="parse error";_tag_arr(_tmp8F3,
sizeof(char),_get_zero_arr_size(_tmp8F3,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 246 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5409,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?({const char*_tmp8F4=", expecting `";_tag_arr(_tmp8F4,
sizeof(char),_get_zero_arr_size(_tmp8F4,14));}):({const char*_tmp8F5=" or `";
_tag_arr(_tmp8F5,sizeof(char),_get_zero_arr_size(_tmp8F5,6));})));Cyc_strcat(msg,(
struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(246,x)]);Cyc_strcat(
msg,({const char*_tmp8F6="'";_tag_arr(_tmp8F6,sizeof(char),_get_zero_arr_size(
_tmp8F6,2));}));count ++;}}}Cyc_yyerror((struct _tagged_arr)msg);}else{Cyc_yyerror(({
const char*_tmp8F7="parse error";_tag_arr(_tmp8F7,sizeof(char),_get_zero_arr_size(
_tmp8F7,12));}));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)
return 1;Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0)return 1;yyvsp_offset --;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];yylsp_offset --;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(860,yystate)];if(yyn == - 32768)
goto yyerrdefault;yyn +=1;if((yyn < 0?1: yyn > 5408)?1: Cyc_yycheck[
_check_known_subscript_notnull(5409,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5409,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 859)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp901=v;struct _tuple13*_tmp902;
struct _tuple13 _tmp903;int _tmp904;char _tmp905;short _tmp906;struct _tagged_arr
_tmp907;struct Cyc_Core_Opt*_tmp908;struct Cyc_Core_Opt*_tmp909;struct Cyc_Core_Opt
_tmp90A;struct _tagged_arr*_tmp90B;struct _tuple1*_tmp90C;struct _tuple1 _tmp90D;
void*_tmp90E;struct _tagged_arr*_tmp90F;_LL369: if(*((void**)_tmp901)!= Cyc_Int_tok)
goto _LL36B;_tmp902=((struct Cyc_Int_tok_struct*)_tmp901)->f1;_tmp903=*_tmp902;
_tmp904=_tmp903.f2;_LL36A:({struct Cyc_Int_pa_struct _tmp912;_tmp912.tag=1;_tmp912.f1=(
int)((unsigned int)_tmp904);{void*_tmp910[1]={& _tmp912};Cyc_fprintf(Cyc_stderr,({
const char*_tmp911="%d";_tag_arr(_tmp911,sizeof(char),_get_zero_arr_size(_tmp911,
3));}),_tag_arr(_tmp910,sizeof(void*),1));}});goto _LL368;_LL36B: if(*((void**)
_tmp901)!= Cyc_Char_tok)goto _LL36D;_tmp905=((struct Cyc_Char_tok_struct*)_tmp901)->f1;
_LL36C:({struct Cyc_Int_pa_struct _tmp915;_tmp915.tag=1;_tmp915.f1=(int)((
unsigned int)((int)_tmp905));{void*_tmp913[1]={& _tmp915};Cyc_fprintf(Cyc_stderr,({
const char*_tmp914="%c";_tag_arr(_tmp914,sizeof(char),_get_zero_arr_size(_tmp914,
3));}),_tag_arr(_tmp913,sizeof(void*),1));}});goto _LL368;_LL36D: if(*((void**)
_tmp901)!= Cyc_Short_tok)goto _LL36F;_tmp906=((struct Cyc_Short_tok_struct*)
_tmp901)->f1;_LL36E:({struct Cyc_Int_pa_struct _tmp918;_tmp918.tag=1;_tmp918.f1=(
int)((unsigned int)((int)_tmp906));{void*_tmp916[1]={& _tmp918};Cyc_fprintf(Cyc_stderr,({
const char*_tmp917="%ds";_tag_arr(_tmp917,sizeof(char),_get_zero_arr_size(_tmp917,
4));}),_tag_arr(_tmp916,sizeof(void*),1));}});goto _LL368;_LL36F: if(*((void**)
_tmp901)!= Cyc_String_tok)goto _LL371;_tmp907=((struct Cyc_String_tok_struct*)
_tmp901)->f1;_LL370:({struct Cyc_String_pa_struct _tmp91B;_tmp91B.tag=0;_tmp91B.f1=(
struct _tagged_arr)_tmp907;{void*_tmp919[1]={& _tmp91B};Cyc_fprintf(Cyc_stderr,({
const char*_tmp91A="\"%s\"";_tag_arr(_tmp91A,sizeof(char),_get_zero_arr_size(
_tmp91A,5));}),_tag_arr(_tmp919,sizeof(void*),1));}});goto _LL368;_LL371: if(*((
void**)_tmp901)!= Cyc_Stringopt_tok)goto _LL373;_tmp908=((struct Cyc_Stringopt_tok_struct*)
_tmp901)->f1;if(_tmp908 != 0)goto _LL373;_LL372:({void*_tmp91C[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp91D="NULL";_tag_arr(_tmp91D,sizeof(char),
_get_zero_arr_size(_tmp91D,5));}),_tag_arr(_tmp91C,sizeof(void*),0));});goto
_LL368;_LL373: if(*((void**)_tmp901)!= Cyc_Stringopt_tok)goto _LL375;_tmp909=((
struct Cyc_Stringopt_tok_struct*)_tmp901)->f1;if(_tmp909 == 0)goto _LL375;_tmp90A=*
_tmp909;_tmp90B=(struct _tagged_arr*)_tmp90A.v;_LL374:({struct Cyc_String_pa_struct
_tmp920;_tmp920.tag=0;_tmp920.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp90B);{void*_tmp91E[1]={& _tmp920};Cyc_fprintf(Cyc_stderr,({const char*_tmp91F="\"%s\"";
_tag_arr(_tmp91F,sizeof(char),_get_zero_arr_size(_tmp91F,5));}),_tag_arr(_tmp91E,
sizeof(void*),1));}});goto _LL368;_LL375: if(*((void**)_tmp901)!= Cyc_QualId_tok)
goto _LL377;_tmp90C=((struct Cyc_QualId_tok_struct*)_tmp901)->f1;_tmp90D=*_tmp90C;
_tmp90E=_tmp90D.f1;_tmp90F=_tmp90D.f2;_LL376: {struct Cyc_List_List*_tmp921=0;{
void*_tmp922=_tmp90E;struct Cyc_List_List*_tmp923;struct Cyc_List_List*_tmp924;
_LL37A: if(_tmp922 <= (void*)1?1:*((int*)_tmp922)!= 0)goto _LL37C;_tmp923=((struct
Cyc_Absyn_Rel_n_struct*)_tmp922)->f1;_LL37B: _tmp921=_tmp923;goto _LL379;_LL37C:
if(_tmp922 <= (void*)1?1:*((int*)_tmp922)!= 1)goto _LL37E;_tmp924=((struct Cyc_Absyn_Abs_n_struct*)
_tmp922)->f1;_LL37D: _tmp921=_tmp924;goto _LL379;_LL37E: if((int)_tmp922 != 0)goto
_LL379;_LL37F: goto _LL379;_LL379:;}for(0;_tmp921 != 0;_tmp921=_tmp921->tl){({
struct Cyc_String_pa_struct _tmp927;_tmp927.tag=0;_tmp927.f1=(struct _tagged_arr)*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp921->hd));{void*_tmp925[1]={& _tmp927};
Cyc_fprintf(Cyc_stderr,({const char*_tmp926="%s::";_tag_arr(_tmp926,sizeof(char),
_get_zero_arr_size(_tmp926,5));}),_tag_arr(_tmp925,sizeof(void*),1));}});}({
struct Cyc_String_pa_struct _tmp92A;_tmp92A.tag=0;_tmp92A.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp90F);{void*_tmp928[1]={& _tmp92A};Cyc_fprintf(Cyc_stderr,({
const char*_tmp929="%s::";_tag_arr(_tmp929,sizeof(char),_get_zero_arr_size(
_tmp929,5));}),_tag_arr(_tmp928,sizeof(void*),1));}});goto _LL368;}_LL377:;_LL378:({
void*_tmp92B[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp92C="?";_tag_arr(
_tmp92C,sizeof(char),_get_zero_arr_size(_tmp92C,2));}),_tag_arr(_tmp92B,sizeof(
void*),0));});goto _LL368;_LL368:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D->v=Cyc_Lexing_from_file(f);_tmp92D;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
