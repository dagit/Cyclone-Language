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
 void exit(int);struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(
unsigned int);struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[
12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct Cyc___cycFILE*);
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_fputc(int,struct Cyc___cycFILE*);int
Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_printf(struct _tagged_arr,struct _tagged_arr);int
puts(const char*);int remove(const char*);extern char Cyc_FileCloseError[19];extern
char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};void Cyc_file_close(struct Cyc___cycFILE*);extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc___cycFILE*);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _tagged_arr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,
struct Cyc_List_List*y);unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_strconcat_l(struct Cyc_List_List*);
struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);struct
_tagged_arr Cyc_substring(struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places);struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _tagged_arr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;
};extern char Cyc_Position_Nocontext[14];typedef struct{int quot;int rem;}Cyc_div_t;
typedef struct{int quot;int rem;}Cyc_ldiv_t;int system(const char*);extern int Cyc_PP_tex_output;
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};
struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_usage(struct Cyc_List_List*,struct _tagged_arr);void Cyc_Arg_parse(
struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),struct _tagged_arr
errmsg,struct _tagged_arr args);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
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
Cyc_YY2[8];struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};extern char
Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);void Cyc_Lex_lex_init();struct Cyc_Position_Segment{int
start;int end;};struct Cyc_MatchDecl_struct{int tag;struct _tagged_arr f1;};struct Cyc_Standalone_struct{
int tag;struct _tagged_arr f1;};struct _tuple9{int f1;void*f2;};struct _tuple9*Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf);const int Cyc_lex_base[15]=(const int[15]){0,- 4,0,-
3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[15]=(const int[15]){- 1,- 1,3,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[15]=(const int[15]){1,0,
- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(const int[266]){0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,0,0,4,
5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};const int Cyc_lex_check[
266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,
12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;
int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(15,state)];if(base < 0)return(-
base)- 1;backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(15,state)];if(
backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(-
state)- 1;else{c=256;}}else{c=(int)*((char*)_check_unknown_subscript(lbuf->lex_buffer,
sizeof(char),lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(266,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(266,base + c)];else{state=Cyc_lex_default[
_check_known_subscript_notnull(15,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Lexing_Error_struct _tmp1;
_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=({const char*_tmp2="empty token";_tag_arr(
_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,12));});_tmp1;});_tmp0;}));else{
return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=0;}}}struct
_tuple9*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL0: {int _tmp3=lexbuf->lex_start_pos + 5;
int _tmp4=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 7;return({struct _tuple9*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp5->f2=(
void*)({struct Cyc_Standalone_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({
struct Cyc_Standalone_struct _tmp7;_tmp7.tag=1;_tmp7.f1=Cyc_substring((struct
_tagged_arr)lexbuf->lex_buffer,_tmp3,(unsigned int)_tmp4);_tmp7;});_tmp6;});
_tmp5;});}case 1: _LL1: {int _tmp8=lexbuf->lex_start_pos + 4;int _tmp9=(lexbuf->lex_curr_pos
- lexbuf->lex_start_pos)- 6;return({struct _tuple9*_tmpA=_cycalloc(sizeof(*_tmpA));
_tmpA->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmpA->f2=(void*)({struct Cyc_MatchDecl_struct*
_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_MatchDecl_struct _tmpC;_tmpC.tag=
0;_tmpC.f1=Cyc_substring((struct _tagged_arr)lexbuf->lex_buffer,_tmp8,(
unsigned int)_tmp9);_tmpC;});_tmpB;});_tmpA;});}case 2: _LL2: return 0;case 3: _LL3:
return Cyc_token(lexbuf);default: _LL4:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Lexing_Error_struct _tmpE;_tmpE.tag=
Cyc_Lexing_Error;_tmpE.f1=({const char*_tmpF="some action didn't return!";
_tag_arr(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,27));});_tmpE;});_tmpD;}));}
struct _tuple9*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(
lexbuf,0);}struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
ae;struct Cyc_Core_Opt*le;};static void Cyc_dump_begin(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
static void Cyc_dump_middle(){puts((const char*)"\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
static void Cyc_dump_end(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _tagged_arr s){int depth=0;
int len=(int)Cyc_strlen((struct _tagged_arr)s);int i=0;for(0;i < len;i ++){char c=*((
const char*)_check_unknown_subscript(s,sizeof(char),i));if(c != '['){Cyc_fputc((
int)c,outf);continue;}Cyc_fputs((const char*)"\\texttt{",outf);i ++;depth ++;for(0;
i < len;i ++){char c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(c
== ']'){depth --;if(depth == 0){Cyc_fputc((int)'}',outf);break;}}else{if(c == '[')
depth ++;}Cyc_fputc((int)c,outf);}}}static int Cyc_width=50;static void Cyc_set_width(
int w){Cyc_width=w;}static struct Cyc_List_List*Cyc_cycdoc_files=0;static void Cyc_add_other(
struct _tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*_tmp10=_cycalloc(
sizeof(*_tmp10));_tmp10->hd=({struct _tagged_arr*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=s;_tmp11;});_tmp10->tl=Cyc_cycdoc_files;_tmp10;});}static struct Cyc_List_List*
Cyc_cycargs=0;static void Cyc_add_cycarg(struct _tagged_arr s){Cyc_cycargs=({struct
Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=({struct _tagged_arr*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=s;_tmp13;});_tmp12->tl=Cyc_cycargs;
_tmp12;});}static char _tmp14[8]="cyclone";static struct _tagged_arr Cyc_cyclone_file={
_tmp14,_tmp14,_tmp14 + 8};static void Cyc_set_cyclone_file(struct _tagged_arr s){Cyc_cyclone_file=
s;}static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _tagged_arr comment){Cyc_dump_begin();({
struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(
Cyc_Absynpp_decl2doc(d),50);{void*_tmp15[1]={& _tmp17};Cyc_printf(({const char*
_tmp16="%s";_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,3));}),
_tag_arr(_tmp15,sizeof(void*),1));}});Cyc_dump_middle();Cyc_pr_comment(Cyc_stdout,
comment);({void*_tmp18[0]={};Cyc_printf(({const char*_tmp19="\n";_tag_arr(_tmp19,
sizeof(char),_get_zero_arr_size(_tmp19,2));}),_tag_arr(_tmp18,sizeof(void*),0));});
Cyc_dump_end();}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL6:
goto _LL7;case '"': _LL7: goto _LL8;case ';': _LL8: goto _LL9;case '&': _LL9: goto _LLA;case '(':
_LLA: goto _LLB;case ')': _LLB: goto _LLC;case '|': _LLC: goto _LLD;case '^': _LLD: goto _LLE;
case '<': _LLE: goto _LLF;case '>': _LLF: goto _LL10;case ' ': _LL10: goto _LL11;case '\n':
_LL11: goto _LL12;case '\t': _LL12: return 1;default: _LL13: return 0;}}static struct
_tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int _tmp1A=Cyc_strlen((
struct _tagged_arr)s);int _tmp1B=0;int _tmp1C=0;{int i=0;for(0;i < _tmp1A;i ++){char
_tmp1D=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(_tmp1D == '\'')
_tmp1B ++;else{if(Cyc_is_other_special(_tmp1D))_tmp1C ++;}}}if(_tmp1B == 0?_tmp1C == 
0: 0)return s;if(_tmp1B == 0)return(struct _tagged_arr)Cyc_strconcat_l(({struct
_tagged_arr*_tmp1E[3];_tmp1E[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);_tmp1E[1]=({struct _tagged_arr*_tmp20=_cycalloc(sizeof(*_tmp20));
_tmp20[0]=(struct _tagged_arr)s;_tmp20;});_tmp1E[0]=_init_tag_arr(_cycalloc(
sizeof(struct _tagged_arr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp1E,sizeof(struct _tagged_arr*),3));}));{
unsigned int _tmp22=(_tmp1A + _tmp1B)+ _tmp1C;struct _tagged_arr s2=({unsigned int
_tmp2C=_tmp22 + 1;char*_tmp2D=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp2C + 1));struct _tagged_arr _tmp2F=_tag_arr(_tmp2D,sizeof(char),_tmp2C + 1);{
unsigned int _tmp2E=_tmp2C;unsigned int i;for(i=0;i < _tmp2E;i ++){_tmp2D[i]='\000';}
_tmp2D[_tmp2E]=(char)0;}_tmp2F;});int _tmp23=0;int _tmp24=0;for(0;_tmp23 < _tmp1A;
_tmp23 ++){char _tmp25=*((const char*)_check_unknown_subscript(s,sizeof(char),
_tmp23));if(_tmp25 == '\''?1: Cyc_is_other_special(_tmp25))({struct _tagged_arr
_tmp26=_tagged_arr_plus(s2,sizeof(char),_tmp24 ++);char _tmp27=*((char*)
_check_unknown_subscript(_tmp26,sizeof(char),0));char _tmp28='\\';if(
_get_arr_size(_tmp26,sizeof(char))== 1?_tmp27 == '\000'?_tmp28 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});({struct _tagged_arr _tmp29=
_tagged_arr_plus(s2,sizeof(char),_tmp24 ++);char _tmp2A=*((char*)
_check_unknown_subscript(_tmp29,sizeof(char),0));char _tmp2B=_tmp25;if(
_get_arr_size(_tmp29,sizeof(char))== 1?_tmp2A == '\000'?_tmp2B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});}return(struct _tagged_arr)
s2;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){
return({struct _tagged_arr*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=Cyc_sh_escape_string(*((
struct _tagged_arr*)sp));_tmp30;});}static struct Cyc_Lineno_Pos*Cyc_new_pos(){
return({struct Cyc_Lineno_Pos*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->logical_file=({
const char*_tmp32="";_tag_arr(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,1));});
_tmp31->line=Cyc_Core_new_string(0);_tmp31->line_no=0;_tmp31->col=0;_tmp31;});}
struct _tuple10{int f1;struct Cyc_Lineno_Pos*f2;};static struct _tuple10*Cyc_start2pos(
int x){return({struct _tuple10*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=x;
_tmp33->f2=Cyc_new_pos();_tmp33;});}static int Cyc_decl2start(struct Cyc_Absyn_Decl*
d){return((struct Cyc_Position_Segment*)_check_null(d->loc))->start;}struct
_tuple11{struct Cyc_Lineno_Pos*f1;void*f2;};static struct Cyc_List_List*Cyc_this_file(
struct _tagged_arr file,struct Cyc_List_List*x){struct Cyc_List_List*_tmp34=0;for(0;
x != 0;x=x->tl){if(Cyc_strcmp((struct _tagged_arr)((*((struct _tuple11*)((struct
_tuple11*)x->hd))).f1)->logical_file,(struct _tagged_arr)file)== 0)_tmp34=({
struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=(struct _tuple11*)
x->hd;_tmp35->tl=_tmp34;_tmp35;});}_tmp34=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp34);return _tmp34;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p){return p->line_no;}static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*
decls){struct Cyc_List_List*_tmp36=0;while(decls != 0){void*_tmp37=(void*)((struct
Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp38;struct Cyc_List_List*
_tmp39;struct Cyc_List_List*_tmp3A;_LL16: if(*((int*)_tmp37)!= 8)goto _LL18;_tmp38=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp37)->f2;_LL17: _tmp39=_tmp38;goto _LL19;
_LL18: if(*((int*)_tmp37)!= 9)goto _LL1A;_tmp39=((struct Cyc_Absyn_Using_d_struct*)
_tmp37)->f2;_LL19: _tmp3A=_tmp39;goto _LL1B;_LL1A: if(*((int*)_tmp37)!= 10)goto
_LL1C;_tmp3A=((struct Cyc_Absyn_ExternC_d_struct*)_tmp37)->f1;_LL1B: decls=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp3A,decls->tl);goto _LL15;_LL1C: if(*((int*)_tmp37)!= 4)goto _LL1E;_LL1D: goto
_LL1F;_LL1E: if(*((int*)_tmp37)!= 0)goto _LL20;_LL1F: goto _LL21;_LL20: if(*((int*)
_tmp37)!= 5)goto _LL22;_LL21: goto _LL23;_LL22: if(*((int*)_tmp37)!= 6)goto _LL24;
_LL23: goto _LL25;_LL24: if(*((int*)_tmp37)!= 7)goto _LL26;_LL25: goto _LL27;_LL26: if(*((
int*)_tmp37)!= 1)goto _LL28;_LL27: goto _LL29;_LL28: if(*((int*)_tmp37)!= 2)goto
_LL2A;_LL29: goto _LL2B;_LL2A: if(*((int*)_tmp37)!= 3)goto _LL15;_LL2B: _tmp36=({
struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->hd=(struct Cyc_Absyn_Decl*)
decls->hd;_tmp3B->tl=_tmp36;_tmp3B;});decls=decls->tl;goto _LL15;_LL15:;}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp36);}struct
_tuple12{int f1;struct Cyc_Absyn_Decl*f2;};struct _tuple13{struct Cyc_Lineno_Pos*f1;
struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(struct _tagged_arr filename){
struct _tagged_arr _tmp3C=Cyc_Filename_chop_extension(filename);const char*_tmp3D=(
const char*)_untag_arr(Cyc_strconcat((struct _tagged_arr)_tmp3C,({const char*_tmp69=".cyp";
_tag_arr(_tmp69,sizeof(char),_get_zero_arr_size(_tmp69,5));})),sizeof(char),1);
struct _tagged_arr _tmp3E=Cyc_str_sepstr(({struct Cyc_List_List*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66->hd=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);_tmp66->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));
_tmp66;}),({const char*_tmp68=" ";_tag_arr(_tmp68,sizeof(char),_get_zero_arr_size(
_tmp68,2));}));const char*_tmp3F=(const char*)_untag_arr(({struct Cyc_String_pa_struct
_tmp64;_tmp64.tag=0;_tmp64.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{
struct Cyc_String_pa_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct _tagged_arr)Cyc_sh_escape_string(({
const char*_tmp65=_tmp3D;_tag_arr(_tmp65,sizeof(char),_get_zero_arr_size(_tmp65,1));}));{
struct Cyc_String_pa_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(struct _tagged_arr)
_tmp3E;{struct Cyc_String_pa_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(struct
_tagged_arr)Cyc_cyclone_file;{void*_tmp5F[4]={& _tmp61,& _tmp62,& _tmp63,& _tmp64};
Cyc_aprintf(({const char*_tmp60="%s %s -E -o %s -x cyc %s";_tag_arr(_tmp60,
sizeof(char),_get_zero_arr_size(_tmp60,25));}),_tag_arr(_tmp5F,sizeof(void*),4));}}}}}),
sizeof(char),1);if(system(_tmp3F)!= 0){({void*_tmp40[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp41="\nError: preprocessing\n";_tag_arr(_tmp41,sizeof(char),
_get_zero_arr_size(_tmp41,23));}),_tag_arr(_tmp40,sizeof(void*),0));});return;}
Cyc_Position_reset_position(({const char*_tmp42=_tmp3D;_tag_arr(_tmp42,sizeof(
char),_get_zero_arr_size(_tmp42,1));}));{struct Cyc___cycFILE*in_file=(struct Cyc___cycFILE*)
_check_null(Cyc_fopen(_tmp3D,(const char*)"r"));Cyc_Lex_lex_init();{struct Cyc_List_List*
_tmp43=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_file_close((struct
Cyc___cycFILE*)in_file);_tmp43=Cyc_flatten_decls(_tmp43);{struct Cyc_List_List*
_tmp44=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(int),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,_tmp43));Cyc_Lineno_poss_of_abss(({
const char*_tmp45=_tmp3D;_tag_arr(_tmp45,sizeof(char),_get_zero_arr_size(_tmp45,1));}),
_tmp44);remove(_tmp3D);{struct Cyc_List_List*_tmp46=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple10*))Cyc_Core_snd,_tmp44),_tmp43);_tmp46=((
struct Cyc_List_List*(*)(struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(
filename,_tmp46);{struct Cyc_List_List*_tmp47=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct
Cyc_Lineno_Pos*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_fst,_tmp46)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple13*))Cyc_Core_snd,_tmp46));struct Cyc___cycFILE*
_tmp48=(struct Cyc___cycFILE*)_check_null(Cyc_fopen((const char*)_untag_arr(
filename,sizeof(char),1),(const char*)"r"));struct Cyc_Lexing_lexbuf*_tmp49=Cyc_Lexing_from_file(
_tmp48);struct Cyc_List_List*_tmp4A=0;struct _tuple9*comment;while((comment=((
struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp49))!= 0){
_tmp4A=({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->hd=(
struct _tuple9*)_check_null(comment);_tmp4B->tl=_tmp4A;_tmp4B;});}Cyc_fclose(
_tmp48);_tmp4A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4A);{struct Cyc_List_List*_tmp4C=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct
_tuple9*))Cyc_Core_fst,_tmp4A));Cyc_Lineno_poss_of_abss(filename,_tmp4C);{struct
Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(struct
_tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(struct
_tuple10*))Cyc_Core_snd,_tmp4C),((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple9*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple9*))Cyc_Core_snd,
_tmp4A));_tmp4D=Cyc_this_file(filename,_tmp4D);{struct Cyc_List_List*_tmp4E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple11*))Cyc_Core_fst,_tmp4D)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp4D));while(_tmp47 != 0?_tmp4E != 0: 0){struct _tuple12 _tmp50;int
_tmp51;struct Cyc_Absyn_Decl*_tmp52;struct _tuple12*_tmp4F=(struct _tuple12*)_tmp47->hd;
_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;{struct _tuple9 _tmp54;int _tmp55;
void*_tmp56;struct _tuple9*_tmp53=(struct _tuple9*)_tmp4E->hd;_tmp54=*_tmp53;
_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;{void*_tmp57=_tmp56;struct _tagged_arr _tmp58;
struct _tagged_arr _tmp59;_LL2D: if(*((int*)_tmp57)!= 1)goto _LL2F;_tmp58=((struct
Cyc_Standalone_struct*)_tmp57)->f1;_LL2E: Cyc_pr_comment(Cyc_stdout,(struct
_tagged_arr)_tmp58);({void*_tmp5A[0]={};Cyc_printf(({const char*_tmp5B="\n";
_tag_arr(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,2));}),_tag_arr(_tmp5A,
sizeof(void*),0));});_tmp4E=_tmp4E->tl;goto _LL2C;_LL2F: if(*((int*)_tmp57)!= 0)
goto _LL2C;_tmp59=((struct Cyc_MatchDecl_struct*)_tmp57)->f1;_LL30: if(_tmp55 < 
_tmp51){_tmp4E=_tmp4E->tl;continue;}if(_tmp47->tl != 0){struct _tuple12 _tmp5D;int
_tmp5E;struct _tuple12*_tmp5C=(struct _tuple12*)((struct Cyc_List_List*)_check_null(
_tmp47->tl))->hd;_tmp5D=*_tmp5C;_tmp5E=_tmp5D.f1;if(_tmp5E < _tmp55){_tmp47=
_tmp47->tl;continue;}}Cyc_dumpdecl(_tmp52,(struct _tagged_arr)_tmp59);Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stdout);_tmp47=_tmp47->tl;_tmp4E=_tmp4E->tl;goto _LL2C;
_LL2C:;}}}}}}}}}}}}void GC_blacklist_warn_clear();struct _tuple14{struct
_tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(
int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
options=({struct _tuple14*_tmp6D[5];_tmp6D[4]=({struct _tuple14*_tmp86=_cycalloc(
sizeof(*_tmp86));_tmp86->f1=({const char*_tmp8B="-B";_tag_arr(_tmp8B,sizeof(char),
_get_zero_arr_size(_tmp8B,3));});_tmp86->f2=1;_tmp86->f3=({const char*_tmp8A="<file>";
_tag_arr(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,7));});_tmp86->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp89;_tmp89.tag=1;_tmp89.f1=Cyc_add_cycarg;
_tmp89;});_tmp88;});_tmp86->f5=({const char*_tmp87="Add to the list of directories to search for compiler files";
_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,60));});_tmp86;});_tmp6D[
3]=({struct _tuple14*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=({const char*
_tmp85="-I";_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,3));});_tmp80->f2=
1;_tmp80->f3=({const char*_tmp84="<dir>";_tag_arr(_tmp84,sizeof(char),
_get_zero_arr_size(_tmp84,6));});_tmp80->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp83;_tmp83.tag=1;_tmp83.f1=Cyc_add_cycarg;_tmp83;});_tmp82;});_tmp80->f5=({
const char*_tmp81="Add to the list of directories to search for include files";
_tag_arr(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,59));});_tmp80;});_tmp6D[
2]=({struct _tuple14*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=({const char*
_tmp7F="-D";_tag_arr(_tmp7F,sizeof(char),_get_zero_arr_size(_tmp7F,3));});_tmp7A->f2=
1;_tmp7A->f3=({const char*_tmp7E="<name>[=<value>]";_tag_arr(_tmp7E,sizeof(char),
_get_zero_arr_size(_tmp7E,17));});_tmp7A->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp7D;_tmp7D.tag=1;_tmp7D.f1=Cyc_add_cycarg;_tmp7D;});_tmp7C;});_tmp7A->f5=({
const char*_tmp7B="Pass definition to preprocessor";_tag_arr(_tmp7B,sizeof(char),
_get_zero_arr_size(_tmp7B,32));});_tmp7A;});_tmp6D[1]=({struct _tuple14*_tmp74=
_cycalloc(sizeof(*_tmp74));_tmp74->f1=({const char*_tmp79="-w";_tag_arr(_tmp79,
sizeof(char),_get_zero_arr_size(_tmp79,3));});_tmp74->f2=0;_tmp74->f3=({const
char*_tmp78=" <width>";_tag_arr(_tmp78,sizeof(char),_get_zero_arr_size(_tmp78,9));});
_tmp74->f4=(void*)({struct Cyc_Arg_Int_spec_struct*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76[0]=({struct Cyc_Arg_Int_spec_struct _tmp77;_tmp77.tag=6;_tmp77.f1=
Cyc_set_width;_tmp77;});_tmp76;});_tmp74->f5=({const char*_tmp75="Use <width> as the max width for printing declarations";
_tag_arr(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,55));});_tmp74;});_tmp6D[
0]=({struct _tuple14*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=({const char*
_tmp73="-cyclone";_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,9));});
_tmp6E->f2=0;_tmp6E->f3=({const char*_tmp72=" <file>";_tag_arr(_tmp72,sizeof(char),
_get_zero_arr_size(_tmp72,8));});_tmp6E->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Arg_String_spec_struct
_tmp71;_tmp71.tag=5;_tmp71.f1=Cyc_set_cyclone_file;_tmp71;});_tmp70;});_tmp6E->f5=({
const char*_tmp6F="Use <file> as the cyclone compiler";_tag_arr(_tmp6F,sizeof(
char),_get_zero_arr_size(_tmp6F,35));});_tmp6E;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp6D,sizeof(struct _tuple14*),5));});
Cyc_Arg_parse(options,Cyc_add_other,({const char*_tmp6A="Options:";_tag_arr(
_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,9));}),argv);if(Cyc_cycdoc_files == 
0){Cyc_Arg_usage(options,({const char*_tmp6B="Usage: cycdoc [options] file1 file2 ...\nOptions:";
_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,49));}));exit(1);}Cyc_PP_tex_output=
1;Cyc_Absynpp_print_scopes=0;{struct Cyc_List_List*_tmp6C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycdoc_files);for(0;_tmp6C != 0;_tmp6C=
_tmp6C->tl){Cyc_process_file(*((struct _tagged_arr*)((struct _tagged_arr*)_tmp6C->hd)));}}
return 0;}}
