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
 void exit(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;
};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{char*tag;
struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct _tagged_arr Cstring_to_string(char*);
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fputc(int,struct Cyc_Std___cycFILE*);
int Cyc_Std_fputs(struct _tagged_arr,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};int Cyc_Std_remove(struct _tagged_arr);int Cyc_Std_vfprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct
Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct _tagged_arr Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,
int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*
Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct Cyc_Set_Set*
Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct Cyc_Set_Set*Cyc_Set_delete(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*
s2);struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);
struct _tagged_arr Cyc_Std_strdup(struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_tagged_arr*p);struct _tagged_arr Cyc_Filename_concat(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);
struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);struct _tagged_arr Cyc_Filename_basename(
struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _tagged_arr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;
};extern char Cyc_Position_Nocontext[14];extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};
struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_usage(struct Cyc_List_List*,struct _tagged_arr);void Cyc_Arg_parse(
struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),struct _tagged_arr
errmsg,struct _tagged_arr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void Cyc_Buffer_add_char(
struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct
_tagged_arr);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;
struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;
int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct _tuple3{void*f1;struct
_tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);void Cyc_Absyn_print_decls(
struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;
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
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
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
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple1*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple0*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple8{
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
first_column;int last_line;int last_column;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple9*Cyc_Dict_rchoose(
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();void Cyc_Tc_tc(
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);typedef
struct{int quot;int rem;}Cyc_Std_div_t;typedef struct{int quot;int rem;}Cyc_Std_ldiv_t;
int Cyc_Std_system(struct _tagged_arr);struct Cyc_Std_stat{short st_dev;unsigned int
st_ino;int st_mode;unsigned short st_nlink;unsigned short st_uid;unsigned short
st_gid;short st_rdev;int st_size;int st_atime;int st_spare1;int st_mtime;int st_spare2;
int st_ctime;int st_spare3;int st_blksize;int st_blocks;int st_spare4[2];};int Cyc_Std_mkdir(
struct _tagged_arr pathname,int mode);struct Cyc_Std_flock{short l_type;short l_whence;
int l_start;int l_len;short l_pid;short l_xxx;};struct Cyc_Std_Flock_struct{int tag;
struct Cyc_Std_flock*f1;};struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_open(
struct _tagged_arr,int,struct _tagged_arr);int Cyc_Std_chdir(struct _tagged_arr);int
Cyc_Std_chroot(struct _tagged_arr);int close(int);struct _tagged_arr Cyc_Std_getcwd(
struct _tagged_arr buf,unsigned int size);struct _tagged_arr Cyc_Std_getpass(struct
_tagged_arr);int isspace(int);int toupper(int);void Cyc_Lex_lex_init();extern char*
Ccomp;struct Cyc_Std___cycFILE*Cyc_log_file=0;struct Cyc_Std___cycFILE*Cyc_cstubs_file=
0;struct Cyc_Std___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_log_file == 0){({void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp1="Internal error: log file is NULL\n";_tag_arr(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,34));}),_tag_arr(_tmp0,sizeof(void*),0));});exit(1);}{
int _tmp2=Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)((struct Cyc_Std___cycFILE*)
_check_null(Cyc_log_file)));return _tmp2;}}static struct _tagged_arr*Cyc_current_source=
0;static struct Cyc_List_List*Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=
0;static void Cyc_add_target(struct _tagged_arr*sptr){Cyc_current_targets=({struct
Cyc_Set_Set**_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),sptr);_tmp3;});}struct _tuple10{struct
_tagged_arr*f1;struct Cyc_Set_Set*f2;};struct _tuple10*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);int Cyc_string(struct Cyc_Lexing_lexbuf*);
struct Cyc_Std___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);
struct _tuple11{struct _tagged_arr f1;struct _tagged_arr*f2;};struct _tuple11*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*);struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(
void*)0,(void*)0,(void*)(0 + 0)};struct _tuple12{struct _tagged_arr f1;struct
_tagged_arr f2;};struct _tuple13{struct _tagged_arr f1;struct Cyc_List_List*f2;struct
Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*
f6;struct Cyc_List_List*f7;};struct _tuple13*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);struct _tagged_arr Cyc_current_header=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=
0;struct Cyc_List_List*Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=
0;struct Cyc_List_List*Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_prologue=
0;struct Cyc_List_List*Cyc_current_epilogue=0;struct Cyc_List_List*Cyc_current_omit_symbols=
0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=
0;int Cyc_parens_to_match=0;int Cyc_numdef=0;const int Cyc_lex_base[420]=(const int[
420]){0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,
- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 16,
223,12,- 2,216,23,26,28,24,28,58,56,65,52,70,370,1024,1139,386,93,83,90,112,96,
106,115,124,115,126,113,375,414,152,180,209,227,228,377,525,227,226,229,238,226,
248,498,1253,1368,654,- 8,238,235,254,239,239,239,259,508,1482,1597,- 11,684,- 12,
241,270,510,1711,1826,- 9,720,- 10,746,- 13,432,515,550,1903,1980,2057,2138,434,465,
667,2213,269,269,279,277,279,290,0,13,4,2294,5,660,2302,2367,822,49,467,6,2328,7,
990,2390,2428,998,- 21,1051,1056,297,314,278,281,289,281,300,313,301,305,311,321,
330,314,- 19,323,371,400,411,418,419,- 14,399,415,412,411,422,418,421,446,448,434,
430,430,453,454,445,443,436,448,444,461,440,465,469,457,478,469,469,- 18,477,471,
500,510,518,500,503,548,556,557,1,4,6,570,571,559,567,579,580,587,588,2,21,590,
592,654,19,20,21,616,580,578,595,603,605,23,660,676,623,624,679,686,690,637,639,
698,699,700,648,649,704,705,709,656,658,713,716,717,665,666,1144,- 20,1165,665,
677,674,673,701,697,699,703,730,728,742,1258,734,736,744,757,1260,1284,760,761,
751,752,750,769,1375,770,771,769,782,1484,- 7,- 8,8,1149,2460,9,1227,2484,2522,
1450,1377,- 49,1168,- 2,821,- 4,865,997,1369,866,993,1023,1563,867,2549,2592,871,
908,870,911,2662,913,943,- 36,- 42,- 37,2737,- 28,940,- 40,- 25,941,- 27,- 45,- 39,- 48,
2812,2841,1583,970,1010,1677,2851,2881,1696,1792,2914,2945,2983,1081,1091,3053,
3091,1084,1127,1148,1207,1227,1237,- 6,- 34,957,3023,- 47,- 30,- 32,- 46,- 29,- 31,- 33,
967,3131,1018,1020,1811,1023,1024,1031,1032,1043,1076,1077,1078,1081,3204,3288,-
23,- 17,- 15,- 22,2357,1112,- 24,- 41,- 38,- 35,- 26,1380,3370,4,3453,1115,15,1061,1062,
1062,1063,1062,1072,1150};const int Cyc_lex_backtrk[420]=(const int[420]){- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,
- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,13,14,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,
8,3,5,- 1,6,5,- 1,20,20,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,-
1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,
42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
420]=(const int[420]){- 1,- 1,- 1,311,300,154,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,126,126,126,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,380,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3710]=(const int[3710]){0,0,0,0,0,
0,0,0,0,0,22,19,28,412,19,28,19,28,95,19,45,45,45,45,45,22,45,0,0,0,0,0,21,218,
229,413,21,22,- 1,- 1,22,- 1,- 1,45,219,45,220,22,410,410,410,410,410,410,410,410,
410,410,31,119,22,230,130,40,243,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,31,260,255,250,410,146,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,408,408,408,408,408,408,408,408,408,408,137,20,85,78,65,
55,56,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,57,58,59,60,408,61,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,66,
67,37,312,313,312,312,313,39,22,68,69,70,71,72,145,34,34,34,34,34,34,34,34,73,74,
312,314,315,75,76,316,317,318,120,120,319,320,120,321,322,323,324,325,325,325,
325,325,325,325,325,325,326,79,327,328,329,120,19,330,330,330,330,330,330,330,
330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,19,-
1,- 1,331,330,80,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,
330,330,330,330,330,330,330,330,330,330,301,332,37,302,155,155,24,24,155,147,138,
81,82,96,83,38,97,86,28,87,24,29,98,25,303,88,89,155,90,22,26,26,21,21,91,117,
109,156,99,100,101,102,103,110,26,35,35,35,35,35,35,35,35,111,131,132,30,30,30,
30,30,30,30,30,62,62,133,134,62,77,77,84,84,77,135,84,136,158,207,208,64,64,304,
193,64,188,179,62,148,139,157,39,77,22,84,172,159,160,161,165,162,166,27,64,163,
31,167,21,77,77,168,169,77,164,170,171,173,129,129,129,129,129,129,129,129,129,
129,- 1,32,- 1,- 1,77,- 1,22,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,- 1,174,- 1,- 1,129,- 1,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,92,92,35,175,92,176,177,178,180,181,104,104,112,112,104,182,112,120,
120,183,184,120,185,92,186,121,121,84,84,121,28,84,21,104,187,112,95,189,190,191,
120,192,46,194,195,196,197,198,121,199,84,200,128,128,114,201,128,202,203,21,21,
21,122,123,122,122,122,122,122,122,122,122,122,122,21,128,204,205,206,265,209,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,210,211,212,213,122,214,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,47,47,19,
215,47,216,217,33,33,33,33,33,33,33,33,33,33,94,94,221,222,94,223,47,33,33,33,33,
33,33,128,128,224,225,128,226,227,228,48,231,94,232,- 1,245,- 1,238,239,107,107,
240,49,107,241,128,242,244,33,33,33,33,33,33,144,144,144,144,144,144,144,144,107,
118,246,247,248,- 1,233,- 1,41,41,234,249,41,115,115,116,251,115,252,235,50,236,51,
253,254,106,52,256,257,258,259,41,53,54,114,261,115,262,263,117,117,264,108,117,
266,267,282,42,42,42,42,42,42,42,42,42,42,237,277,273,274,95,117,28,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,275,108,276,36,
42,278,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,279,280,43,281,283,116,284,33,33,33,33,33,33,33,33,33,33,285,286,295,290,
291,292,293,33,33,33,33,33,33,294,118,35,35,35,35,35,35,35,35,296,297,298,299,
406,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,405,379,370,345,44,340,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,341,342,343,28,337,338,335,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,339,344,373,333,44,402,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,403,
377,62,153,153,153,153,153,153,153,153,35,35,35,35,35,35,35,35,378,95,62,268,404,
171,287,287,154,206,287,269,269,334,374,269,106,108,63,63,63,63,63,63,63,63,63,
63,118,287,375,376,95,95,269,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,178,398,46,95,63,397,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,31,64,64,288,399,64,267,267,270,21,
267,310,310,289,414,310,415,416,271,35,35,417,31,272,64,418,419,269,269,267,407,
269,407,407,310,21,21,268,0,0,63,63,63,63,63,63,63,63,63,63,269,35,35,407,31,0,
31,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,0,0,36,63,31,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,92,92,270,36,92,281,281,286,286,281,0,286,271,309,309,309,309,309,
309,309,309,36,0,92,0,0,0,0,281,0,286,287,287,0,0,287,19,0,31,93,93,93,93,93,93,
93,93,93,93,0,369,369,0,36,287,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,369,369,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,36,94,94,- 1,0,94,0,288,294,294,310,
310,294,407,310,407,407,289,0,0,0,0,0,0,94,0,0,0,0,0,0,294,- 1,310,0,21,407,0,0,
28,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,381,0,93,0,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,104,104,299,299,104,0,
299,31,31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,104,0,299,0,0,0,0,0,0,0,35,0,0,0,0,0,
105,105,105,105,105,105,105,105,105,105,0,0,0,0,0,0,0,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,106,107,107,0,371,107,372,372,372,372,
372,372,372,372,372,372,0,0,0,0,- 1,351,0,351,107,0,352,352,352,352,352,352,352,
352,352,352,0,0,0,0,105,105,105,105,105,105,105,105,105,105,0,0,0,0,0,0,0,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,112,112,0,0,112,352,
352,352,352,352,352,352,352,352,352,0,0,0,0,355,0,355,0,112,356,356,356,356,356,
356,356,356,356,356,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,
0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,114,115,115,
0,0,115,356,356,356,356,356,356,356,356,356,356,399,0,0,0,0,0,0,0,115,400,400,
400,400,400,400,400,400,0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,
0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,124,
124,124,124,124,124,124,124,124,124,124,124,22,0,0,0,0,0,0,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,0,0,0,0,124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,125,124,124,124,124,124,124,124,
124,124,124,22,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,22,0,0,0,0,0,0,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,- 1,0,0,- 1,124,0,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,
125,125,125,125,125,125,125,125,125,127,0,0,0,0,0,0,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,
0,0,125,0,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,0,0,
0,0,0,0,0,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,0,0,
28,0,0,149,141,141,141,141,141,141,141,141,143,143,143,143,143,143,143,143,143,
143,0,0,0,0,0,0,0,143,143,143,143,143,143,0,0,0,150,150,150,150,150,150,150,150,
0,0,0,0,0,0,0,31,0,0,- 1,0,399,0,0,143,143,143,143,143,143,401,401,401,401,401,
401,401,401,0,142,143,143,143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,
143,143,143,143,152,152,152,152,152,152,152,152,152,152,151,0,0,0,0,0,0,152,152,
152,152,152,152,0,0,0,143,143,143,143,143,143,19,0,0,305,0,0,152,152,152,152,152,
152,152,152,152,152,0,152,152,152,152,152,152,152,152,152,152,152,152,0,0,0,0,0,
0,0,0,0,306,306,306,306,306,306,306,306,0,0,0,0,0,0,0,0,0,152,152,152,152,152,
152,0,308,308,308,308,308,308,308,308,308,308,0,0,0,0,0,0,0,308,308,308,308,308,
308,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,308,308,308,308,308,308,308,308,308,308,307,
308,308,308,308,308,308,308,308,308,308,308,308,0,0,346,0,357,357,357,357,357,
357,357,357,358,358,0,0,0,0,0,0,0,0,0,0,0,348,308,308,308,308,308,308,359,0,0,0,
0,0,0,0,0,360,0,0,361,346,0,347,347,347,347,347,347,347,347,347,347,348,0,0,0,0,
0,0,359,0,0,0,348,0,0,0,0,360,0,349,361,0,0,0,0,0,0,0,350,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,348,0,0,0,0,0,0,349,0,0,0,0,0,0,0,0,350,336,336,336,336,336,336,336,336,
336,336,0,0,0,0,0,0,0,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,0,0,0,0,336,0,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,0,0,0,0,0,0,0,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,0,0,0,0,336,0,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,353,353,353,353,353,353,353,353,
353,353,0,0,0,0,0,0,0,0,0,0,0,354,116,0,0,0,0,346,116,347,347,347,347,347,347,
347,347,347,347,352,352,352,352,352,352,352,352,352,352,0,348,0,0,354,116,0,0,
349,0,0,116,114,0,0,0,0,350,114,0,353,353,353,353,353,353,353,353,353,353,0,0,0,
348,0,0,0,0,0,0,349,354,116,0,114,0,0,0,116,350,114,0,0,356,356,356,356,356,356,
356,356,356,356,0,0,0,0,0,0,0,0,0,0,354,116,116,0,0,0,0,116,116,346,0,357,357,
357,357,357,357,357,357,358,358,0,0,0,0,0,0,0,0,0,0,0,348,0,116,0,0,0,0,367,116,
0,0,0,0,0,0,346,368,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,348,0,0,0,
0,0,348,367,0,0,0,0,0,365,0,0,368,0,0,0,0,0,366,0,0,372,372,372,372,372,372,372,
372,372,372,0,0,0,348,0,0,0,0,0,0,365,354,116,0,0,0,0,0,116,366,362,362,362,362,
362,362,362,362,362,362,0,0,0,0,0,0,0,362,362,362,362,362,362,354,116,0,0,0,0,0,
116,0,0,0,0,0,0,0,362,362,362,362,362,362,362,362,362,362,0,362,362,362,362,362,
362,362,362,362,362,362,362,0,0,0,382,0,363,0,0,383,0,0,0,0,0,364,0,0,384,384,
384,384,384,384,384,384,0,362,362,362,362,362,362,385,0,0,0,0,363,0,0,0,0,0,0,0,
0,364,0,0,0,0,0,0,0,0,0,0,0,0,0,0,386,0,0,0,0,387,388,0,0,0,389,0,0,0,0,0,0,0,
390,0,0,0,391,0,392,0,393,0,394,395,395,395,395,395,395,395,395,395,395,0,0,0,0,
0,0,0,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,0,0,0,0,0,0,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,396,0,0,0,0,
0,0,0,0,395,395,395,395,395,395,395,395,395,395,0,0,0,0,0,0,0,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,0,0,0,0,0,0,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,22,0,0,409,0,0,0,408,408,408,408,408,
408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,408,0,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,411,0,0,0,0,0,0,0,410,410,410,410,410,410,410,410,410,410,0,
0,0,0,0,0,0,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,0,0,0,0,410,0,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3710]=(const int[3710]){- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,138,140,147,149,302,305,41,41,45,45,41,412,45,- 1,-
1,- 1,- 1,- 1,136,217,228,0,409,10,12,40,10,12,40,41,218,45,219,20,1,1,1,1,1,1,1,1,
1,1,38,48,137,229,10,38,242,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
145,233,234,235,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,
7,3,3,3,3,3,7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,
47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,80,81,50,82,16,50,85,27,86,17,27,50,17,4,87,
88,5,89,5,6,8,9,18,90,96,97,5,98,99,100,101,102,109,17,34,34,34,34,34,34,34,34,
110,130,131,27,27,27,27,27,27,27,27,61,61,132,133,61,76,76,83,83,76,134,83,135,
157,159,159,64,64,4,160,64,161,162,61,6,8,5,16,76,16,83,163,158,158,158,164,158,
165,17,64,158,27,166,7,77,77,167,168,77,158,169,170,172,11,11,11,11,11,11,11,11,
11,11,119,27,126,119,77,126,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,127,173,146,127,11,146,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,64,174,91,175,176,177,179,
180,103,103,111,111,103,181,111,120,120,182,183,120,184,91,185,13,13,84,84,13,77,
84,16,103,186,111,187,188,189,190,120,191,192,193,194,195,196,197,13,198,84,199,
121,121,4,200,121,201,202,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,121,203,
204,205,207,208,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,209,210,211,212,13,213,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,14,14,84,214,14,215,216,32,32,32,32,32,32,32,
32,32,32,94,94,220,221,94,222,14,32,32,32,32,32,32,128,128,223,224,128,225,226,
227,14,230,94,231,119,236,126,237,238,107,107,239,14,107,240,128,241,243,32,32,
32,32,32,32,141,141,141,141,141,141,141,141,107,244,245,246,247,127,232,146,15,
15,232,248,15,115,115,249,250,115,251,232,14,232,14,252,253,254,14,255,256,257,
258,15,14,14,259,260,115,261,262,117,117,263,264,117,265,266,270,15,15,15,15,15,
15,15,15,15,15,232,271,272,273,94,117,13,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,274,107,275,276,15,277,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,278,279,15,280,282,
115,283,33,33,33,33,33,33,33,33,33,33,284,285,288,289,290,291,292,33,33,33,33,33,
33,293,117,144,144,144,144,144,144,144,144,295,296,297,298,314,42,42,42,42,42,42,
42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,316,319,323,326,42,328,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,
327,327,327,15,329,329,331,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,338,341,371,332,44,380,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,317,320,62,150,150,150,150,
150,150,150,150,153,153,153,153,153,153,153,153,320,349,62,382,317,383,155,155,
385,386,155,156,156,332,321,156,387,388,62,62,62,62,62,62,62,62,62,62,389,155,
321,321,350,349,156,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,390,391,392,350,62,393,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,63,63,155,401,63,267,267,156,411,267,
303,303,155,413,303,414,415,156,359,360,416,363,156,63,417,418,269,269,267,312,
269,312,312,303,419,303,267,- 1,- 1,63,63,63,63,63,63,63,63,63,63,269,359,360,312,
363,- 1,364,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,- 1,- 1,- 1,365,63,364,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,92,92,269,365,92,281,281,286,286,281,- 1,286,269,306,
306,306,306,306,306,306,306,366,- 1,92,- 1,- 1,- 1,- 1,281,- 1,286,287,287,- 1,- 1,287,
281,- 1,286,92,92,92,92,92,92,92,92,92,92,- 1,367,368,- 1,366,287,- 1,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,367,368,- 1,- 1,92,
- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,93,93,318,- 1,93,- 1,287,294,294,310,310,294,407,310,407,407,287,- 1,- 1,- 1,- 1,- 1,
- 1,93,- 1,- 1,- 1,- 1,- 1,- 1,294,318,310,- 1,310,407,- 1,- 1,294,93,93,93,93,93,93,93,93,
93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,- 1,- 1,318,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,104,104,299,299,104,- 1,299,309,309,309,
309,309,309,309,309,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,299,- 1,- 1,- 1,- 1,- 1,- 1,- 1,299,
- 1,- 1,- 1,- 1,- 1,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,- 1,
322,105,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,318,348,- 1,348,105,-
1,348,348,348,348,348,348,348,348,348,348,- 1,- 1,- 1,- 1,105,105,105,105,105,105,
105,105,105,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,112,112,- 1,- 1,112,351,351,351,351,351,351,351,351,351,
351,- 1,- 1,- 1,- 1,354,- 1,354,- 1,112,354,354,354,354,354,354,354,354,354,354,- 1,- 1,
- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,113,113,- 1,- 1,
113,355,355,355,355,355,355,355,355,355,355,384,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,384,384,
384,384,384,384,384,384,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,- 1,-
1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,
123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,
124,124,124,124,124,124,- 1,- 1,- 1,- 1,- 1,- 1,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,125,- 1,- 1,
125,124,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,
125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,125,
- 1,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,139,- 1,- 1,139,129,- 1,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,- 1,-
1,148,- 1,- 1,148,139,139,139,139,139,139,139,139,142,142,142,142,142,142,142,142,
142,142,- 1,- 1,- 1,- 1,- 1,- 1,- 1,142,142,142,142,142,142,- 1,- 1,- 1,148,148,148,148,
148,148,148,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,- 1,- 1,125,- 1,400,- 1,- 1,142,142,142,142,
142,142,400,400,400,400,400,400,400,400,- 1,139,143,143,143,143,143,143,143,143,
143,143,148,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,151,151,151,151,151,151,
151,151,151,151,148,- 1,- 1,- 1,- 1,- 1,- 1,151,151,151,151,151,151,- 1,- 1,- 1,143,143,
143,143,143,143,304,- 1,- 1,304,- 1,- 1,152,152,152,152,152,152,152,152,152,152,- 1,
151,151,151,151,151,151,152,152,152,152,152,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,304,
304,304,304,304,304,304,304,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,152,152,152,152,152,152,-
1,307,307,307,307,307,307,307,307,307,307,- 1,- 1,- 1,- 1,- 1,- 1,- 1,307,307,307,307,
307,307,- 1,- 1,304,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,308,308,308,308,308,308,
308,308,308,308,304,307,307,307,307,307,307,308,308,308,308,308,308,- 1,- 1,324,- 1,
324,324,324,324,324,324,324,324,324,324,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,324,308,
308,308,308,308,308,324,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,324,- 1,- 1,324,325,- 1,325,325,325,
325,325,325,325,325,325,325,324,- 1,- 1,- 1,- 1,- 1,- 1,324,- 1,- 1,- 1,325,- 1,- 1,- 1,- 1,
324,- 1,325,324,- 1,- 1,- 1,- 1,- 1,- 1,- 1,325,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,325,- 1,- 1,- 1,- 1,- 1,- 1,325,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,325,330,330,330,330,330,
330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,330,330,330,330,330,330,330,330,330,
330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,330,
- 1,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,
330,330,330,330,330,330,330,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,346,
346,346,346,346,346,346,346,346,346,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,346,346,- 1,
- 1,- 1,- 1,347,346,347,347,347,347,347,347,347,347,347,347,352,352,352,352,352,352,
352,352,352,352,- 1,347,- 1,- 1,346,346,- 1,- 1,347,- 1,- 1,346,352,- 1,- 1,- 1,- 1,347,352,
- 1,353,353,353,353,353,353,353,353,353,353,- 1,- 1,- 1,347,- 1,- 1,- 1,- 1,- 1,- 1,347,
353,353,- 1,352,- 1,- 1,- 1,353,347,352,- 1,- 1,356,356,356,356,356,356,356,356,356,
356,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,353,353,356,- 1,- 1,- 1,- 1,353,356,357,- 1,357,357,
357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,357,- 1,356,- 1,-
1,- 1,- 1,357,356,- 1,- 1,- 1,- 1,- 1,- 1,358,357,358,358,358,358,358,358,358,358,358,
358,- 1,- 1,- 1,- 1,- 1,357,- 1,- 1,- 1,- 1,- 1,358,357,- 1,- 1,- 1,- 1,- 1,358,- 1,- 1,357,- 1,- 1,
- 1,- 1,- 1,358,- 1,- 1,372,372,372,372,372,372,372,372,372,372,- 1,- 1,- 1,358,- 1,- 1,- 1,
- 1,- 1,- 1,358,372,372,- 1,- 1,- 1,- 1,- 1,372,358,361,361,361,361,361,361,361,361,361,
361,- 1,- 1,- 1,- 1,- 1,- 1,- 1,361,361,361,361,361,361,372,372,- 1,- 1,- 1,- 1,- 1,372,- 1,-
1,- 1,- 1,- 1,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,361,361,361,361,361,
361,362,362,362,362,362,362,- 1,- 1,- 1,381,- 1,362,- 1,- 1,381,- 1,- 1,- 1,- 1,- 1,362,- 1,
- 1,381,381,381,381,381,381,381,381,- 1,362,362,362,362,362,362,381,- 1,- 1,- 1,- 1,
362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,- 1,
- 1,- 1,- 1,381,381,- 1,- 1,- 1,381,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,- 1,- 1,- 1,381,- 1,381,- 1,
381,- 1,381,394,394,394,394,394,394,394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,395,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,395,395,395,395,395,395,395,395,395,395,- 1,- 1,- 1,- 1,- 1,- 1,- 1,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,- 1,- 1,- 1,- 1,- 1,- 1,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,408,- 1,- 1,408,- 1,- 1,- 1,
408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,- 1,- 1,- 1,- 1,408,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,410,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,410,
410,410,410,410,410,410,410,410,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,- 1,-
1,- 1,- 1,410,- 1,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int start_state,struct
Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;if(
state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(420,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(420,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3710,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3710,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(420,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=({
struct Cyc_Lexing_Error_struct _tmp5;_tmp5.tag=Cyc_Lexing_Error;_tmp5.f1=({const
char*_tmp6="empty token";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,12));});
_tmp5;});_tmp4;}));else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=
0;}}}struct _tuple10*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: Cyc_macroname(
lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)
_check_null(Cyc_current_args))->tl){Cyc_current_targets=({struct Cyc_Set_Set**
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);
_tmp7;});}return({struct _tuple10*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->f1=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp8->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp8;});case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp9=
_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Lexing_Error_struct _tmpA;_tmpA.tag=
Cyc_Lexing_Error;_tmpA.f1=({const char*_tmpB="some action didn't return!";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,27));});_tmpA;});_tmp9;}));}
struct _tuple10*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(
lexbuf,0);}int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL5: Cyc_current_source=({
struct _tagged_arr*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=(struct _tagged_arr)
Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpC;});
Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpD;});Cyc_token(lexbuf);
return 0;case 1: _LL6: Cyc_current_source=({struct _tagged_arr*_tmpE=_cycalloc(
sizeof(*_tmpE));_tmpE[0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));_tmpE;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpF;});
Cyc_args(lexbuf);return 0;case 2: _LL7: Cyc_current_source=({struct _tagged_arr*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp10;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmp11;});
Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);return Cyc_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp12=
_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Lexing_Error_struct _tmp13;
_tmp13.tag=Cyc_Lexing_Error;_tmp13.f1=({const char*_tmp14="some action didn't return!";
_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,27));});_tmp13;});_tmp12;}));}
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,
1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LLA: {struct _tagged_arr*_tmp15=({struct
_tagged_arr*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp17;});Cyc_current_args=({
struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=_tmp15;_tmp16->tl=
Cyc_current_args;_tmp16;});return Cyc_args(lexbuf);}case 1: _LLB: {struct
_tagged_arr*_tmp18=({struct _tagged_arr*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[
0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));_tmp1A;});Cyc_current_args=({struct Cyc_List_List*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19->hd=_tmp18;_tmp19->tl=Cyc_current_args;_tmp19;});return
Cyc_token(lexbuf);}default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp1B=
_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Lexing_Error_struct _tmp1C;
_tmp1C.tag=Cyc_Lexing_Error;_tmp1C.f1=({const char*_tmp1D="some action didn't return!";
_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,27));});_tmp1C;});_tmp1B;}));}
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLE: Cyc_add_target(({struct _tagged_arr*_tmp1E=
_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);
_tmp1E;}));return Cyc_token(lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(
lexbuf);case 3: _LL11: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL12:
return Cyc_token(lexbuf);case 5: _LL13: return Cyc_token(lexbuf);case 6: _LL14: return
Cyc_token(lexbuf);case 7: _LL15: return Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(
lexbuf);case 9: _LL17: return Cyc_token(lexbuf);case 10: _LL18: return Cyc_token(lexbuf);
case 11: _LL19: return Cyc_token(lexbuf);case 12: _LL1A: return Cyc_token(lexbuf);case 13:
_LL1B: return Cyc_token(lexbuf);case 14: _LL1C: return Cyc_token(lexbuf);case 15: _LL1D:
return Cyc_token(lexbuf);case 16: _LL1E: return Cyc_token(lexbuf);case 17: _LL1F: return
Cyc_token(lexbuf);case 18: _LL20: return Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(
lexbuf);case 20: _LL22: return Cyc_token(lexbuf);case 21: _LL23: return Cyc_token(lexbuf);
case 22: _LL24: return Cyc_token(lexbuf);case 23: _LL25: return Cyc_token(lexbuf);case 24:
_LL26: return Cyc_token(lexbuf);case 25: _LL27: return Cyc_token(lexbuf);case 26: _LL28:
return Cyc_token(lexbuf);case 27: _LL29: return Cyc_token(lexbuf);case 28: _LL2A: return
Cyc_token(lexbuf);case 29: _LL2B: return Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(
lexbuf);case 31: _LL2D: return Cyc_token(lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);
case 33: _LL2F: return Cyc_token(lexbuf);case 34: _LL30: return Cyc_token(lexbuf);case 35:
_LL31: return Cyc_token(lexbuf);case 36: _LL32: return Cyc_token(lexbuf);case 37: _LL33:
return Cyc_token(lexbuf);case 38: _LL34: return Cyc_token(lexbuf);case 39: _LL35: return
Cyc_token(lexbuf);case 40: _LL36: return Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(
lexbuf);case 42: _LL38: return Cyc_token(lexbuf);case 43: _LL39: return Cyc_token(lexbuf);
case 44: _LL3A: return Cyc_token(lexbuf);case 45: _LL3B: return Cyc_token(lexbuf);case 46:
_LL3C: return Cyc_token(lexbuf);case 47: _LL3D: return Cyc_token(lexbuf);case 48: _LL3E:
return Cyc_token(lexbuf);default: _LL3F:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp1F=
_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=({struct Cyc_Lexing_Error_struct _tmp20;
_tmp20.tag=Cyc_Lexing_Error;_tmp20.f1=({const char*_tmp21="some action didn't return!";
_tag_arr(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,27));});_tmp20;});_tmp1F;}));}
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}int
Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL41: return Cyc_string(lexbuf);case 1:
_LL42: return 0;case 2: _LL43: return Cyc_string(lexbuf);case 3: _LL44: return Cyc_string(
lexbuf);case 4: _LL45: return Cyc_string(lexbuf);case 5: _LL46: return Cyc_string(lexbuf);
case 6: _LL47: return Cyc_string(lexbuf);case 7: _LL48: return 0;case 8: _LL49: return 0;
case 9: _LL4A: return Cyc_string(lexbuf);default: _LL4B:(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Lexing_Error_struct
_tmp23;_tmp23.tag=Cyc_Lexing_Error;_tmp23.f1=({const char*_tmp24="some action didn't return!";
_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,27));});_tmp23;});_tmp22;}));}
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL4D: return 0;case 1: _LL4E: Cyc_Std_fputc((
int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(
lexbuf)){;}return 1;case 2: _LL4F: Cyc_Std_fputs(({const char*_tmp25="*__IGNORE_FOR_CYCLONE_MALLOC(";
_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,30));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp26[0]={};Cyc_log(({const char*_tmp27="Warning: declaration of malloc sidestepped\n";
_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,44));}),_tag_arr(_tmp26,
sizeof(void*),0));});return 1;case 3: _LL50: Cyc_Std_fputs(({const char*_tmp28=" __IGNORE_FOR_CYCLONE_MALLOC(";
_tag_arr(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,30));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp29[0]={};Cyc_log(({const char*_tmp2A="Warning: declaration of malloc sidestepped\n";
_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,44));}),_tag_arr(_tmp29,
sizeof(void*),0));});return 1;case 4: _LL51: Cyc_Std_fputs(({const char*_tmp2B="*__IGNORE_FOR_CYCLONE_CALLOC(";
_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,30));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp2C[0]={};Cyc_log(({const char*_tmp2D="Warning: declaration of calloc sidestepped\n";
_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,44));}),_tag_arr(_tmp2C,
sizeof(void*),0));});return 1;case 5: _LL52: Cyc_Std_fputs(({const char*_tmp2E=" __IGNORE_FOR_CYCLONE_CALLOC(";
_tag_arr(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,30));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp2F[0]={};Cyc_log(({const char*_tmp30="Warning: declaration of calloc sidestepped\n";
_tag_arr(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,44));}),_tag_arr(_tmp2F,
sizeof(void*),0));});return 1;case 6: _LL53: Cyc_Std_fputs(({const char*_tmp31="__region";
_tag_arr(_tmp31,sizeof(char),_get_zero_arr_size(_tmp31,9));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp32[0]={};Cyc_log(({const char*_tmp33="Warning: use of region sidestepped\n";
_tag_arr(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,36));}),_tag_arr(_tmp32,
sizeof(void*),0));});return 1;case 7: _LL54: return 1;case 8: _LL55: return 1;case 9: _LL56:
return 1;case 10: _LL57: return 1;case 11: _LL58: return 1;case 12: _LL59: return 1;case 13:
_LL5A: Cyc_Std_fputs(({const char*_tmp34="inline";_tag_arr(_tmp34,sizeof(char),
_get_zero_arr_size(_tmp34,7));}),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 14: _LL5B: Cyc_Std_fputs(({const char*_tmp35="inline";_tag_arr(_tmp35,
sizeof(char),_get_zero_arr_size(_tmp35,7));}),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5C: Cyc_Std_fputs(({const char*
_tmp36="const";_tag_arr(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,6));}),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5D: Cyc_Std_fputs(({
const char*_tmp37="const";_tag_arr(_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,
6));}),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17:
_LL5E: Cyc_Std_fputs(({const char*_tmp38="int";_tag_arr(_tmp38,sizeof(char),
_get_zero_arr_size(_tmp38,4));}),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL5F: return 1;case 19: _LL60: Cyc_parens_to_match=1;while(Cyc_asm(
lexbuf)){;}Cyc_Std_fputs(({const char*_tmp39="0";_tag_arr(_tmp39,sizeof(char),
_get_zero_arr_size(_tmp39,2));}),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp3A[0]={};Cyc_log(({const char*_tmp3B="Warning: replacing use of __asm__ with 0\n";
_tag_arr(_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,42));}),_tag_arr(_tmp3A,
sizeof(void*),0));});return 1;case 20: _LL61: Cyc_Std_fputc((int)Cyc_Lexing_lexeme_char(
lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;default:
_LL62:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));
_tmp3C[0]=({struct Cyc_Lexing_Error_struct _tmp3D;_tmp3D.tag=Cyc_Lexing_Error;
_tmp3D.f1=({const char*_tmp3E="some action didn't return!";_tag_arr(_tmp3E,
sizeof(char),_get_zero_arr_size(_tmp3E,27));});_tmp3D;});_tmp3C;}));}int Cyc_slurp(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL64: return 0;case 1: _LL65: Cyc_Std_fputc((int)'"',(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL66:({void*
_tmp3F[0]={};Cyc_log(({const char*_tmp40="Warning: unclosed string\n";_tag_arr(
_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,26));}),_tag_arr(_tmp3F,sizeof(
void*),0));});({struct Cyc_Std_String_pa_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp41[1]={& _tmp43};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp42="%s";
_tag_arr(_tmp42,sizeof(char),_get_zero_arr_size(_tmp42,3));}),_tag_arr(_tmp41,
sizeof(void*),1));}});return 1;case 3: _LL67:({struct Cyc_Std_String_pa_struct _tmp46;
_tmp46.tag=0;_tmp46.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp44[
1]={& _tmp46};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp45="%s";_tag_arr(_tmp45,sizeof(char),_get_zero_arr_size(_tmp45,3));}),
_tag_arr(_tmp44,sizeof(void*),1));}});return 1;case 4: _LL68:({struct Cyc_Std_String_pa_struct
_tmp49;_tmp49.tag=0;_tmp49.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp47[1]={& _tmp49};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp48="%s";_tag_arr(_tmp48,sizeof(char),_get_zero_arr_size(_tmp48,3));}),
_tag_arr(_tmp47,sizeof(void*),1));}});return 1;case 5: _LL69:({struct Cyc_Std_String_pa_struct
_tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4A[1]={& _tmp4C};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp4B="%s";_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,3));}),
_tag_arr(_tmp4A,sizeof(void*),1));}});return 1;case 6: _LL6A:({struct Cyc_Std_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4D[1]={& _tmp4F};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp4E="%s";_tag_arr(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,3));}),
_tag_arr(_tmp4D,sizeof(void*),1));}});return 1;case 7: _LL6B:({struct Cyc_Std_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp50[1]={& _tmp52};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp51="%s";_tag_arr(_tmp51,sizeof(char),_get_zero_arr_size(_tmp51,3));}),
_tag_arr(_tmp50,sizeof(void*),1));}});return 1;case 8: _LL6C:({struct Cyc_Std_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp53[1]={& _tmp55};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp54="%s";_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,3));}),
_tag_arr(_tmp53,sizeof(void*),1));}});return 1;default: _LL6D:(lexbuf->refill_buff)(
lexbuf);return Cyc_slurp_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Lexing_Error_struct
_tmp57;_tmp57.tag=Cyc_Lexing_Error;_tmp57.f1=({const char*_tmp58="some action didn't return!";
_tag_arr(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,27));});_tmp57;});_tmp56;}));}
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70:
if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match --;return 1;case 2: _LL71: Cyc_parens_to_match
++;return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73:
while(Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp59=_cycalloc(sizeof(*
_tmp59));_tmp59[0]=({struct Cyc_Lexing_Error_struct _tmp5A;_tmp5A.tag=Cyc_Lexing_Error;
_tmp5A.f1=({const char*_tmp5B="some action didn't return!";_tag_arr(_tmp5B,
sizeof(char),_get_zero_arr_size(_tmp5B,27));});_tmp5A;});_tmp59;}));}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL78:({void*_tmp5C[0]={};Cyc_log(({const char*
_tmp5D="Warning: unclosed string\n";_tag_arr(_tmp5D,sizeof(char),
_get_zero_arr_size(_tmp5D,26));}),_tag_arr(_tmp5C,sizeof(void*),0));});return 0;
case 1: _LL79: return 0;case 2: _LL7A:({void*_tmp5E[0]={};Cyc_log(({const char*_tmp5F="Warning: unclosed string\n";
_tag_arr(_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,26));}),_tag_arr(_tmp5E,
sizeof(void*),0));});return 1;case 3: _LL7B: return 1;case 4: _LL7C: return 1;case 5: _LL7D:
return 1;case 6: _LL7E: return 1;case 7: _LL7F: return 1;case 8: _LL80: return 1;default:
_LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60[0]=({struct Cyc_Lexing_Error_struct _tmp61;_tmp61.tag=Cyc_Lexing_Error;
_tmp61.f1=({const char*_tmp62="some action didn't return!";_tag_arr(_tmp62,
sizeof(char),_get_zero_arr_size(_tmp62,27));});_tmp61;});_tmp60;}));}int Cyc_asm_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL83:({void*_tmp63[0]={};Cyc_log(({const char*
_tmp64="Warning: unclosed comment\n";_tag_arr(_tmp64,sizeof(char),
_get_zero_arr_size(_tmp64,27));}),_tag_arr(_tmp63,sizeof(void*),0));});return 0;
case 1: _LL84: return 0;case 2: _LL85: return 1;default: _LL86:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Lexing_Error_struct
_tmp66;_tmp66.tag=Cyc_Lexing_Error;_tmp66.f1=({const char*_tmp67="some action didn't return!";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,27));});_tmp66;});_tmp65;}));}
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple11*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL88:
Cyc_current_line=({const char*_tmp68="#define ";_tag_arr(_tmp68,sizeof(char),
_get_zero_arr_size(_tmp68,9));});Cyc_suck_macroname(lexbuf);return({struct
_tuple11*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->f1=Cyc_current_line;_tmp69->f2=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp69;});case 1: _LL89: return
Cyc_suck_line(lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Lexing_Error_struct
_tmp6B;_tmp6B.tag=Cyc_Lexing_Error;_tmp6B.f1=({const char*_tmp6C="some action didn't return!";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,27));});_tmp6B;});_tmp6A;}));}
struct _tuple11*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8D: Cyc_current_source=({
struct _tagged_arr*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);_tmp6D;});Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(
Cyc_current_line,*((struct _tagged_arr*)_check_null(Cyc_current_source)));return
Cyc_suck_restofline(lexbuf);default: _LL8E:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_macroname_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Lexing_Error_struct
_tmp6F;_tmp6F.tag=Cyc_Lexing_Error;_tmp6F.f1=({const char*_tmp70="some action didn't return!";
_tag_arr(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,27));});_tmp6F;});_tmp6E;}));}
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL90:
Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL91:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({
struct Cyc_Lexing_Error_struct _tmp72;_tmp72.tag=Cyc_Lexing_Error;_tmp72.f1=({
const char*_tmp73="some action didn't return!";_tag_arr(_tmp73,sizeof(char),
_get_zero_arr_size(_tmp73,27));});_tmp72;});_tmp71;}));}int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}struct
_tuple13*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL93: return Cyc_spec(lexbuf);case 1: _LL94:
Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);Cyc_current_cstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);return({struct _tuple13*_tmp74=_cycalloc(sizeof(*_tmp74));
_tmp74->f1=Cyc_current_header;_tmp74->f2=Cyc_current_symbols;_tmp74->f3=Cyc_current_omit_symbols;
_tmp74->f4=Cyc_current_prologue;_tmp74->f5=Cyc_current_epilogue;_tmp74->f6=Cyc_current_cstubs;
_tmp74->f7=Cyc_current_cycstubs;_tmp74;});case 2: _LL95: return Cyc_spec(lexbuf);
case 3: _LL96: return 0;case 4: _LL97:({struct Cyc_Std_Int_pa_struct _tmp77;_tmp77.tag=1;
_tmp77.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*
_tmp75[1]={& _tmp77};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp76="Error in .cys file: expected header file name, found '%c' instead\n";
_tag_arr(_tmp76,sizeof(char),_get_zero_arr_size(_tmp76,67));}),_tag_arr(_tmp75,
sizeof(void*),1));}});return 0;default: _LL98:(lexbuf->refill_buff)(lexbuf);return
Cyc_spec_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Lexing_Error_struct
_tmp79;_tmp79.tag=Cyc_Lexing_Error;_tmp79.f1=({const char*_tmp7A="some action didn't return!";
_tag_arr(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,27));});_tmp79;});_tmp78;}));}
struct _tuple13*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9A: return 0;
case 1: _LL9B: return 0;case 2: _LL9C: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(
lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9D: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LL9E: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple12*x=({struct _tuple12*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp7C->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7C;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=x;_tmp7B->tl=
Cyc_current_prologue;_tmp7B;});return 1;}case 5: _LL9F: {struct _tagged_arr _tmp7D=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp7D,sizeof(char),(int)Cyc_Std_strlen(({
const char*_tmp7E="prologue";_tag_arr(_tmp7E,sizeof(char),_get_zero_arr_size(
_tmp7E,9));})));while(isspace((int)*((char*)_check_unknown_subscript(_tmp7D,
sizeof(char),0)))){_tagged_arr_inplace_plus_post(& _tmp7D,sizeof(char),1);}{
struct _tagged_arr t=_tmp7D;while(!isspace((int)*((char*)_check_unknown_subscript(
t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct
_tagged_arr _tmp7F=Cyc_Std_substring((struct _tagged_arr)_tmp7D,0,(unsigned int)((
t.curr - _tmp7D.curr)/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*
x=({struct _tuple12*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=(struct
_tagged_arr)_tmp7F;_tmp81->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp81;});Cyc_current_prologue=({struct Cyc_List_List*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->hd=x;_tmp80->tl=Cyc_current_prologue;
_tmp80;});return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct
_tuple12*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp83->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp83;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=x;_tmp82->tl=Cyc_current_epilogue;
_tmp82;});return 1;}case 7: _LLA1: {struct _tagged_arr _tmp84=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp84,sizeof(char),(int)Cyc_Std_strlen(({
const char*_tmp85="epilogue";_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(
_tmp85,9));})));while(isspace((int)*((char*)_check_unknown_subscript(_tmp84,
sizeof(char),0)))){_tagged_arr_inplace_plus_post(& _tmp84,sizeof(char),1);}{
struct _tagged_arr t=_tmp84;while(!isspace((int)*((char*)_check_unknown_subscript(
t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct
_tagged_arr _tmp86=Cyc_Std_substring((struct _tagged_arr)_tmp84,0,(unsigned int)((
t.curr - _tmp84.curr)/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*
x=({struct _tuple12*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=(struct
_tagged_arr)_tmp86;_tmp88->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp88;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->hd=x;_tmp87->tl=Cyc_current_epilogue;
_tmp87;});return 1;}}}}case 8: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct
_tuple12*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp8A->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp8A;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=x;_tmp89->tl=Cyc_current_cstubs;
_tmp89;});return 1;}case 9: _LLA3: {struct _tagged_arr _tmp8B=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp8B,sizeof(char),(int)Cyc_Std_strlen(({
const char*_tmp8C="cstub";_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,
6));})));while(isspace((int)*((char*)_check_unknown_subscript(_tmp8B,sizeof(char),
0)))){_tagged_arr_inplace_plus_post(& _tmp8B,sizeof(char),1);}{struct _tagged_arr t=
_tmp8B;while(!isspace((int)*((char*)_check_unknown_subscript(t,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct _tagged_arr _tmp8D=Cyc_Std_substring((
struct _tagged_arr)_tmp8B,0,(unsigned int)((t.curr - _tmp8B.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp8F=_cycalloc(
sizeof(*_tmp8F));_tmp8F->f1=(struct _tagged_arr)_tmp8D;_tmp8F->f2=(struct
_tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp8F;});Cyc_current_cstubs=({struct Cyc_List_List*_tmp8E=_cycalloc(sizeof(*
_tmp8E));_tmp8E->hd=x;_tmp8E->tl=Cyc_current_cstubs;_tmp8E;});return 1;}}}}case 10:
_LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91->f1=(struct _tagged_arr)_tag_arr(0,0,0);_tmp91->f2=(
struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp91;});Cyc_current_cycstubs=({struct Cyc_List_List*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90->hd=x;_tmp90->tl=Cyc_current_cycstubs;_tmp90;});return 1;}case 11:
_LLA5: {struct _tagged_arr _tmp92=Cyc_Lexing_lexeme(lexbuf);
_tagged_arr_inplace_plus(& _tmp92,sizeof(char),(int)Cyc_Std_strlen(({const char*
_tmp93="cycstub";_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,8));})));
while(isspace((int)*((char*)_check_unknown_subscript(_tmp92,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& _tmp92,sizeof(char),1);}{struct _tagged_arr t=
_tmp92;while(!isspace((int)*((char*)_check_unknown_subscript(t,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct _tagged_arr _tmp94=Cyc_Std_substring((
struct _tagged_arr)_tmp92,0,(unsigned int)((t.curr - _tmp92.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96->f1=(struct _tagged_arr)_tmp94;_tmp96->f2=(struct
_tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp96;});Cyc_current_cycstubs=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->hd=x;_tmp95->tl=Cyc_current_cycstubs;_tmp95;});return 1;}}}}case
12: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tagged_arr*x=({struct _tagged_arr*_tmp98=
_cycalloc(sizeof(*_tmp98));_tmp98[0]=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98;});Cyc_current_cpp=({struct
Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x;_tmp97->tl=Cyc_current_cpp;
_tmp97;});return 1;}case 13: _LLA7: return 1;case 14: _LLA8: return 1;case 15: _LLA9:({
struct Cyc_Std_Int_pa_struct _tmp9B;_tmp9B.tag=1;_tmp9B.f1=(int)((unsigned int)((
int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp99[1]={& _tmp9B};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp9A="Error in .cys file: expected command, found '%c' instead\n";
_tag_arr(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,58));}),_tag_arr(_tmp99,
sizeof(void*),1));}});return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return
Cyc_commands_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Lexing_Error_struct
_tmp9D;_tmp9D.tag=Cyc_Lexing_Error;_tmp9D.f1=({const char*_tmp9E="some action didn't return!";
_tag_arr(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,27));});_tmp9D;});_tmp9C;}));}
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LLAC: Cyc_snarfed_symbols=({struct Cyc_List_List*
_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->hd=({struct _tagged_arr*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);
_tmpA0;});_tmp9F->tl=Cyc_snarfed_symbols;_tmp9F;});return 1;case 1: _LLAD: return 1;
case 2: _LLAE: return 0;case 3: _LLAF:({void*_tmpA1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmpA2="Error in .cys file: unexpected end-of-file\n";_tag_arr(_tmpA2,
sizeof(char),_get_zero_arr_size(_tmpA2,44));}),_tag_arr(_tmpA1,sizeof(void*),0));});
return 0;case 4: _LLB0:({struct Cyc_Std_Int_pa_struct _tmpA5;_tmpA5.tag=1;_tmpA5.f1=(
int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmpA3[1]={&
_tmpA5};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpA4="Error in .cys file: expected symbol, found '%c' instead\n";
_tag_arr(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,57));}),_tag_arr(_tmpA3,
sizeof(void*),1));}});return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return
Cyc_snarfsymbols_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Lexing_Error_struct
_tmpA7;_tmpA7.tag=Cyc_Lexing_Error;_tmpA7.f1=({const char*_tmpA8="some action didn't return!";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,27));});_tmpA7;});_tmpA6;}));}
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB3:({void*
_tmpA9[0]={};Cyc_log(({const char*_tmpAA="Warning: unclosed brace\n";_tag_arr(
_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,25));}),_tag_arr(_tmpA9,sizeof(
void*),0));});return 0;case 1: _LLB4: if(Cyc_braces_to_match == 1)return 0;Cyc_braces_to_match
--;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return
1;case 2: _LLB5: Cyc_braces_to_match ++;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB6: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}return 1;case 4:
_LLB7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({
const char*_tmpAB="/*";_tag_arr(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,3));}));
while(Cyc_block_comment(lexbuf)){;}return 1;case 5: _LLB8: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLBA:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({
struct Cyc_Lexing_Error_struct _tmpAD;_tmpAD.tag=Cyc_Lexing_Error;_tmpAD.f1=({
const char*_tmpAE="some action didn't return!";_tag_arr(_tmpAE,sizeof(char),
_get_zero_arr_size(_tmpAE,27));});_tmpAD;});_tmpAC;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBC:({void*_tmpAF[0]={};Cyc_log(({const char*_tmpB0="Warning: unclosed string\n";
_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,26));}),_tag_arr(_tmpAF,
sizeof(void*),0));});return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBE:({void*_tmpB1[0]={};Cyc_log(({
const char*_tmpB2="Warning: unclosed string\n";_tag_arr(_tmpB2,sizeof(char),
_get_zero_arr_size(_tmpB2,26));}),_tag_arr(_tmpB1,sizeof(void*),0));});Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 3: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 5: _LLC1: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 7: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 8: _LLC4: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;default: _LLC5:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB3=
_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Lexing_Error_struct _tmpB4;
_tmpB4.tag=Cyc_Lexing_Error;_tmpB4.f1=({const char*_tmpB5="some action didn't return!";
_tag_arr(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,27));});_tmpB4;});_tmpB3;}));}
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:({void*
_tmpB6[0]={};Cyc_log(({const char*_tmpB7="Warning: unclosed comment\n";_tag_arr(
_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,27));}),_tag_arr(_tmpB6,sizeof(
void*),0));});return 0;case 1: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),({const char*_tmpB8="*/";_tag_arr(_tmpB8,sizeof(char),
_get_zero_arr_size(_tmpB8,3));}));return 0;case 2: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Lexing_Error_struct _tmpBA;
_tmpBA.tag=Cyc_Lexing_Error;_tmpBA.f1=({const char*_tmpBB="some action didn't return!";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,27));});_tmpBA;});_tmpB9;}));}
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmpBC=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmpBD;struct
_tuple0 _tmpBE;struct _tagged_arr*_tmpBF;struct _tuple0*_tmpC0;struct _tuple0 _tmpC1;
struct _tagged_arr*_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;
struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;
struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;
struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;
struct Cyc_List_List*_tmpD4;void*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_MallocInfo
_tmpD7;int _tmpD8;struct Cyc_Absyn_Exp*_tmpD9;void**_tmpDA;struct Cyc_Absyn_Exp*
_tmpDB;void*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct _tagged_arr*_tmpDE;struct Cyc_Absyn_Exp*
_tmpDF;struct _tagged_arr*_tmpE0;void*_tmpE1;void*_tmpE2;struct Cyc_List_List*
_tmpE3;_LLCD: if(*((int*)_tmpBC)!= 1)goto _LLCF;_tmpBD=((struct Cyc_Absyn_Var_e_struct*)
_tmpBC)->f1;_tmpBE=*_tmpBD;_tmpBF=_tmpBE.f2;_LLCE: _tmpC2=_tmpBF;goto _LLD0;_LLCF:
if(*((int*)_tmpBC)!= 2)goto _LLD1;_tmpC0=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpBC)->f1;_tmpC1=*_tmpC0;_tmpC2=_tmpC1.f2;_LLD0: Cyc_add_target(_tmpC2);return;
_LLD1: if(*((int*)_tmpBC)!= 3)goto _LLD3;_tmpC3=((struct Cyc_Absyn_Primop_e_struct*)
_tmpBC)->f2;_LLD2: for(0;_tmpC3 != 0;_tmpC3=_tmpC3->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpC3->hd));}return;_LLD3: if(*((int*)_tmpBC)!= 23)goto
_LLD5;_tmpC4=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBC)->f1;_tmpC5=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpBC)->f2;_LLD4: _tmpC6=_tmpC4;_tmpC7=_tmpC5;goto
_LLD6;_LLD5: if(*((int*)_tmpBC)!= 7)goto _LLD7;_tmpC6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBC)->f1;_tmpC7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpBC)->f2;_LLD6: _tmpC8=
_tmpC6;_tmpC9=_tmpC7;goto _LLD8;_LLD7: if(*((int*)_tmpBC)!= 4)goto _LLD9;_tmpC8=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpBC)->f1;_tmpC9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBC)->f3;_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC9);return;_LLD9: if(*((int*)_tmpBC)!= 20)goto _LLDB;
_tmpCA=((struct Cyc_Absyn_Deref_e_struct*)_tmpBC)->f1;_LLDA: _tmpCB=_tmpCA;goto
_LLDC;_LLDB: if(*((int*)_tmpBC)!= 17)goto _LLDD;_tmpCB=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpBC)->f1;_LLDC: _tmpCC=_tmpCB;goto _LLDE;_LLDD: if(*((int*)_tmpBC)!= 14)goto
_LLDF;_tmpCC=((struct Cyc_Absyn_Address_e_struct*)_tmpBC)->f1;_LLDE: _tmpCD=_tmpCC;
goto _LLE0;_LLDF: if(*((int*)_tmpBC)!= 5)goto _LLE1;_tmpCD=((struct Cyc_Absyn_Increment_e_struct*)
_tmpBC)->f1;_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCD);return;_LLE1: if(*((
int*)_tmpBC)!= 6)goto _LLE3;_tmpCE=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBC)->f1;
_tmpCF=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBC)->f2;_tmpD0=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBC)->f3;_LLE2: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCE);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpCF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD0);return;
_LLE3: if(*((int*)_tmpBC)!= 9)goto _LLE5;_tmpD1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpBC)->f1;_tmpD2=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBC)->f2;_LLE4: _tmpD3=
_tmpD1;_tmpD4=_tmpD2;goto _LLE6;_LLE5: if(*((int*)_tmpBC)!= 8)goto _LLE7;_tmpD3=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBC)->f1;_tmpD4=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpBC)->f2;_LLE6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD3);for(0;_tmpD4 != 0;
_tmpD4=_tmpD4->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpD4->hd));}return;_LLE7: if(*((int*)_tmpBC)!= 13)goto _LLE9;_tmpD5=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpBC)->f1;_tmpD6=((struct Cyc_Absyn_Cast_e_struct*)
_tmpBC)->f2;_LLE8: Cyc_scan_type(_tmpD5);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpD6);return;_LLE9: if(*((int*)_tmpBC)!= 33)goto _LLEB;_tmpD7=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpBC)->f1;_tmpD8=_tmpD7.is_calloc;_tmpD9=_tmpD7.rgn;_tmpDA=_tmpD7.elt_type;
_tmpDB=_tmpD7.num_elts;_LLEA: if(_tmpD9 != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpD9)));if(_tmpDA != 0)Cyc_scan_type(*((void**)
_tmpDA));Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDB);return;_LLEB: if(*((int*)
_tmpBC)!= 16)goto _LLED;_tmpDC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpBC)->f1;_LLEC: Cyc_scan_type(_tmpDC);return;_LLED: if(*((int*)_tmpBC)!= 21)
goto _LLEF;_tmpDD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpBC)->f1;_tmpDE=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpBC)->f2;_LLEE: _tmpDF=_tmpDD;_tmpE0=
_tmpDE;goto _LLF0;_LLEF: if(*((int*)_tmpBC)!= 22)goto _LLF1;_tmpDF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpBC)->f1;_tmpE0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBC)->f2;_LLF0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDF);Cyc_add_target(_tmpE0);return;_LLF1: if(*((int*)
_tmpBC)!= 18)goto _LLF3;_tmpE1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBC)->f1;
_tmpE2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBC)->f2;_LLF2: Cyc_scan_type(
_tmpE1);{void*_tmpE4=_tmpE2;struct _tagged_arr*_tmpE5;_LL11A: if(*((int*)_tmpE4)!= 
0)goto _LL11C;_tmpE5=((struct Cyc_Absyn_StructField_struct*)_tmpE4)->f1;_LL11B: Cyc_add_target(
_tmpE5);goto _LL119;_LL11C: if(*((int*)_tmpE4)!= 1)goto _LL119;_LL11D: goto _LL119;
_LL119:;}return;_LLF3: if(*((int*)_tmpBC)!= 0)goto _LLF5;_LLF4: return;_LLF5: if(*((
int*)_tmpBC)!= 34)goto _LLF7;_tmpE3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpBC)->f2;_LLF6: for(0;_tmpE3 != 0;_tmpE3=_tmpE3->tl){struct _tuple14 _tmpE7;
struct Cyc_Absyn_Exp*_tmpE8;struct _tuple14*_tmpE6=(struct _tuple14*)_tmpE3->hd;
_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE8);}
return;_LLF7: if(*((int*)_tmpBC)!= 35)goto _LLF9;_LLF8:({void*_tmpE9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmpEA="Error: unexpected Stmt_e\n";_tag_arr(_tmpEA,
sizeof(char),_get_zero_arr_size(_tmpEA,26));}),_tag_arr(_tmpE9,sizeof(void*),0));});
exit(1);return;_LLF9: if(*((int*)_tmpBC)!= 10)goto _LLFB;_LLFA:({void*_tmpEB[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpEC="Error: unexpected Throw_e\n";
_tag_arr(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,27));}),_tag_arr(_tmpEB,
sizeof(void*),0));});exit(1);return;_LLFB: if(*((int*)_tmpBC)!= 11)goto _LLFD;
_LLFC:({void*_tmpED[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpEE="Error: unexpected NoInstantiate_e\n";
_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,35));}),_tag_arr(_tmpED,
sizeof(void*),0));});exit(1);return;_LLFD: if(*((int*)_tmpBC)!= 12)goto _LLFF;
_LLFE:({void*_tmpEF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpF0="Error: unexpected Instantiate_e\n";
_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,33));}),_tag_arr(_tmpEF,
sizeof(void*),0));});exit(1);return;_LLFF: if(*((int*)_tmpBC)!= 15)goto _LL101;
_LL100:({void*_tmpF1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpF2="Error: unexpected New_e\n";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,25));}),_tag_arr(_tmpF1,
sizeof(void*),0));});exit(1);return;_LL101: if(*((int*)_tmpBC)!= 19)goto _LL103;
_LL102:({void*_tmpF3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpF4="Error: unexpected Gentyp_e\n";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,28));}),_tag_arr(_tmpF3,
sizeof(void*),0));});exit(1);return;_LL103: if(*((int*)_tmpBC)!= 24)goto _LL105;
_LL104:({void*_tmpF5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpF6="Error: unexpected Tuple_e\n";
_tag_arr(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,27));}),_tag_arr(_tmpF5,
sizeof(void*),0));});exit(1);return;_LL105: if(*((int*)_tmpBC)!= 25)goto _LL107;
_LL106:({void*_tmpF7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpF8="Error: unexpected CompoundLit_e\n";
_tag_arr(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,33));}),_tag_arr(_tmpF7,
sizeof(void*),0));});exit(1);return;_LL107: if(*((int*)_tmpBC)!= 26)goto _LL109;
_LL108:({void*_tmpF9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpFA="Error: unexpected Array_e\n";
_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,27));}),_tag_arr(_tmpF9,
sizeof(void*),0));});exit(1);return;_LL109: if(*((int*)_tmpBC)!= 27)goto _LL10B;
_LL10A:({void*_tmpFB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpFC="Error: unexpected Comprehension_e\n";
_tag_arr(_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,35));}),_tag_arr(_tmpFB,
sizeof(void*),0));});exit(1);return;_LL10B: if(*((int*)_tmpBC)!= 28)goto _LL10D;
_LL10C:({void*_tmpFD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpFE="Error: unexpected Struct_e\n";
_tag_arr(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,28));}),_tag_arr(_tmpFD,
sizeof(void*),0));});exit(1);return;_LL10D: if(*((int*)_tmpBC)!= 29)goto _LL10F;
_LL10E:({void*_tmpFF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp100="Error: unexpected AnonStruct_e\n";
_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,32));}),_tag_arr(_tmpFF,
sizeof(void*),0));});exit(1);return;_LL10F: if(*((int*)_tmpBC)!= 30)goto _LL111;
_LL110:({void*_tmp101[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp102="Error: unexpected Tunion_e\n";
_tag_arr(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,28));}),_tag_arr(
_tmp101,sizeof(void*),0));});exit(1);return;_LL111: if(*((int*)_tmpBC)!= 31)goto
_LL113;_LL112:({void*_tmp103[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp104="Error: unexpected Enum_e\n";_tag_arr(_tmp104,sizeof(char),
_get_zero_arr_size(_tmp104,26));}),_tag_arr(_tmp103,sizeof(void*),0));});exit(1);
return;_LL113: if(*((int*)_tmpBC)!= 32)goto _LL115;_LL114:({void*_tmp105[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp106="Error: unexpected AnonEnum_e\n";_tag_arr(
_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,30));}),_tag_arr(_tmp105,sizeof(
void*),0));});exit(1);return;_LL115: if(*((int*)_tmpBC)!= 36)goto _LL117;_LL116:({
void*_tmp107[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp108="Error: unexpected Codegen_e\n";
_tag_arr(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,29));}),_tag_arr(
_tmp107,sizeof(void*),0));});exit(1);return;_LL117: if(*((int*)_tmpBC)!= 37)goto
_LLCC;_LL118:({void*_tmp109[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp10A="Error: unexpected Fill_e\n";_tag_arr(_tmp10A,sizeof(char),
_get_zero_arr_size(_tmp10A,26));}),_tag_arr(_tmp109,sizeof(void*),0));});exit(1);
return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)
Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*_tmp10B=t;struct Cyc_Absyn_PtrInfo
_tmp10C;struct Cyc_Absyn_ArrayInfo _tmp10D;void*_tmp10E;struct Cyc_Absyn_Exp*
_tmp10F;struct Cyc_Absyn_Conref*_tmp110;struct Cyc_Absyn_FnInfo _tmp111;struct Cyc_List_List*
_tmp112;struct Cyc_Absyn_AggrInfo _tmp113;void*_tmp114;struct _tuple0*_tmp115;
struct _tuple0 _tmp116;struct _tagged_arr*_tmp117;struct _tuple0*_tmp118;struct
_tuple0 _tmp119;struct _tagged_arr*_tmp11A;_LL11F: if((int)_tmp10B != 0)goto _LL121;
_LL120: goto _LL122;_LL121: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 5)goto _LL123;
_LL122: goto _LL124;_LL123: if((int)_tmp10B != 1)goto _LL125;_LL124: goto _LL126;_LL125:
if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 6)goto _LL127;_LL126: return;_LL127: if(
_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 4)goto _LL129;_tmp10C=((struct Cyc_Absyn_PointerType_struct*)
_tmp10B)->f1;_LL128: Cyc_scan_type((void*)_tmp10C.elt_typ);return;_LL129: if(
_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 7)goto _LL12B;_tmp10D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10B)->f1;_tmp10E=(void*)_tmp10D.elt_type;_tmp10F=_tmp10D.num_elts;_tmp110=
_tmp10D.zero_term;_LL12A: Cyc_scan_type(_tmp10E);Cyc_scan_exp_opt(_tmp10F);
return;_LL12B: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 8)goto _LL12D;_tmp111=((
struct Cyc_Absyn_FnType_struct*)_tmp10B)->f1;_LL12C: Cyc_scan_type((void*)_tmp111.ret_typ);{
struct Cyc_List_List*_tmp11B=_tmp111.args;for(0;_tmp11B != 0;_tmp11B=_tmp11B->tl){
struct _tuple1 _tmp11D;void*_tmp11E;struct _tuple1*_tmp11C=(struct _tuple1*)_tmp11B->hd;
_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f3;Cyc_scan_type(_tmp11E);}}if(_tmp111.cyc_varargs
!= 0)Cyc_scan_type((void*)(_tmp111.cyc_varargs)->type);return;_LL12D: if(_tmp10B
<= (void*)3?1:*((int*)_tmp10B)!= 11)goto _LL12F;_tmp112=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10B)->f2;_LL12E: for(0;_tmp112 != 0;_tmp112=_tmp112->tl){Cyc_scan_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)
_tmp112->hd)->width);}return;_LL12F: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 
13)goto _LL131;_LL130: return;_LL131: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 10)
goto _LL133;_tmp113=((struct Cyc_Absyn_AggrType_struct*)_tmp10B)->f1;_tmp114=(void*)
_tmp113.aggr_info;_LL132: {struct _tuple0*_tmp120;struct _tuple0 _tmp121;struct
_tagged_arr*_tmp122;struct _tuple3 _tmp11F=Cyc_Absyn_aggr_kinded_name(_tmp114);
_tmp120=_tmp11F.f2;_tmp121=*_tmp120;_tmp122=_tmp121.f2;_tmp117=_tmp122;goto
_LL134;}_LL133: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 12)goto _LL135;_tmp115=((
struct Cyc_Absyn_EnumType_struct*)_tmp10B)->f1;_tmp116=*_tmp115;_tmp117=_tmp116.f2;
_LL134: _tmp11A=_tmp117;goto _LL136;_LL135: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)
!= 16)goto _LL137;_tmp118=((struct Cyc_Absyn_TypedefType_struct*)_tmp10B)->f1;
_tmp119=*_tmp118;_tmp11A=_tmp119.f2;_LL136: Cyc_add_target(_tmp11A);return;_LL137:
if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 0)goto _LL139;_LL138:({void*_tmp123[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp124="Error: unexpected Evar\n";
_tag_arr(_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,24));}),_tag_arr(
_tmp123,sizeof(void*),0));});exit(1);return;_LL139: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 1)goto _LL13B;_LL13A:({void*_tmp125[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp126="Error: unexpected VarType\n";_tag_arr(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,27));}),_tag_arr(_tmp125,sizeof(void*),0));});exit(1);
return;_LL13B: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 2)goto _LL13D;_LL13C:({
void*_tmp127[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp128="Error: unexpected TunionType\n";
_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,30));}),_tag_arr(
_tmp127,sizeof(void*),0));});exit(1);return;_LL13D: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 3)goto _LL13F;_LL13E:({void*_tmp129[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp12A="Error: unexpected TunionFieldType\n";_tag_arr(_tmp12A,sizeof(
char),_get_zero_arr_size(_tmp12A,35));}),_tag_arr(_tmp129,sizeof(void*),0));});
exit(1);return;_LL13F: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 9)goto _LL141;
_LL140:({void*_tmp12B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp12C="Error: unexpected TupleType\n";
_tag_arr(_tmp12C,sizeof(char),_get_zero_arr_size(_tmp12C,29));}),_tag_arr(
_tmp12B,sizeof(void*),0));});exit(1);return;_LL141: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 15)goto _LL143;_LL142:({void*_tmp12D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp12E="Error: unexpected RgnHandleType\n";_tag_arr(_tmp12E,sizeof(
char),_get_zero_arr_size(_tmp12E,33));}),_tag_arr(_tmp12D,sizeof(void*),0));});
exit(1);return;_LL143: if((int)_tmp10B != 2)goto _LL145;_LL144:({void*_tmp12F[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp130="Error: unexpected HeapRgn\n";
_tag_arr(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,27));}),_tag_arr(
_tmp12F,sizeof(void*),0));});exit(1);return;_LL145: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 19)goto _LL147;_LL146:({void*_tmp131[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp132="Error: unexpected AccessEff\n";_tag_arr(_tmp132,sizeof(char),
_get_zero_arr_size(_tmp132,29));}),_tag_arr(_tmp131,sizeof(void*),0));});exit(1);
return;_LL147: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 20)goto _LL149;_LL148:({
void*_tmp133[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp134="Error: unexpected JoinEff\n";
_tag_arr(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,27));}),_tag_arr(
_tmp133,sizeof(void*),0));});exit(1);return;_LL149: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 21)goto _LL14B;_LL14A:({void*_tmp135[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp136="Error: unexpected RgnsEff\n";_tag_arr(_tmp136,sizeof(char),
_get_zero_arr_size(_tmp136,27));}),_tag_arr(_tmp135,sizeof(void*),0));});exit(1);
return;_LL14B: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 14)goto _LL14D;_LL14C:({
void*_tmp137[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp138="Error: unexpected sizeof_t\n";
_tag_arr(_tmp138,sizeof(char),_get_zero_arr_size(_tmp138,28));}),_tag_arr(
_tmp137,sizeof(void*),0));});exit(1);return;_LL14D: if(_tmp10B <= (void*)3?1:*((
int*)_tmp10B)!= 18)goto _LL14F;_LL14E:({void*_tmp139[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp13A="Error: unexpected type integer\n";_tag_arr(_tmp13A,sizeof(
char),_get_zero_arr_size(_tmp13A,32));}),_tag_arr(_tmp139,sizeof(void*),0));});
exit(1);return;_LL14F: if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 17)goto _LL11E;
_LL150:({void*_tmp13B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp13C="Error: unexpected tag_t\n";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,25));}),_tag_arr(
_tmp13B,sizeof(void*),0));});exit(1);return;_LL11E:;}struct _tuple15{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple10*Cyc_scan_decl(
struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**_tmp13D=
_cycalloc(sizeof(*_tmp13D));_tmp13D[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmp13D;});{
void*_tmp13E=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp13F;struct Cyc_Absyn_Fndecl*
_tmp140;struct Cyc_Absyn_Aggrdecl*_tmp141;struct Cyc_Absyn_Enumdecl*_tmp142;struct
Cyc_Absyn_Typedefdecl*_tmp143;_LL152: if(*((int*)_tmp13E)!= 0)goto _LL154;_tmp13F=((
struct Cyc_Absyn_Var_d_struct*)_tmp13E)->f1;_LL153: {struct _tuple0 _tmp145;struct
_tagged_arr*_tmp146;struct _tuple0*_tmp144=_tmp13F->name;_tmp145=*_tmp144;_tmp146=
_tmp145.f2;Cyc_current_source=(struct _tagged_arr*)_tmp146;Cyc_scan_type((void*)
_tmp13F->type);Cyc_scan_exp_opt(_tmp13F->initializer);goto _LL151;}_LL154: if(*((
int*)_tmp13E)!= 1)goto _LL156;_tmp140=((struct Cyc_Absyn_Fn_d_struct*)_tmp13E)->f1;
_LL155: {struct _tuple0 _tmp148;struct _tagged_arr*_tmp149;struct _tuple0*_tmp147=
_tmp140->name;_tmp148=*_tmp147;_tmp149=_tmp148.f2;Cyc_current_source=(struct
_tagged_arr*)_tmp149;Cyc_scan_type((void*)_tmp140->ret_type);{struct Cyc_List_List*
_tmp14A=_tmp140->args;for(0;_tmp14A != 0;_tmp14A=_tmp14A->tl){struct _tuple15
_tmp14C;void*_tmp14D;struct _tuple15*_tmp14B=(struct _tuple15*)_tmp14A->hd;_tmp14C=*
_tmp14B;_tmp14D=_tmp14C.f3;Cyc_scan_type(_tmp14D);}}if(_tmp140->cyc_varargs != 0)
Cyc_scan_type((void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp140->cyc_varargs))->type);
if(_tmp140->is_inline)({void*_tmp14E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp14F="Warning: ignoring inline function\n";_tag_arr(_tmp14F,sizeof(
char),_get_zero_arr_size(_tmp14F,35));}),_tag_arr(_tmp14E,sizeof(void*),0));});
goto _LL151;}_LL156: if(*((int*)_tmp13E)!= 4)goto _LL158;_tmp141=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp13E)->f1;_LL157: {struct _tuple0 _tmp151;struct _tagged_arr*_tmp152;struct
_tuple0*_tmp150=_tmp141->name;_tmp151=*_tmp150;_tmp152=_tmp151.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp152;if((unsigned int)_tmp141->impl){{struct Cyc_List_List*
_tmp153=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp141->impl))->fields;for(
0;_tmp153 != 0;_tmp153=_tmp153->tl){struct Cyc_Absyn_Aggrfield*_tmp154=(struct Cyc_Absyn_Aggrfield*)
_tmp153->hd;Cyc_scan_type((void*)_tmp154->type);Cyc_scan_exp_opt(_tmp154->width);}}{
struct Cyc_List_List*_tmp155=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp141->impl))->fields;
for(0;_tmp155 != 0;_tmp155=_tmp155->tl){;}}}goto _LL151;}_LL158: if(*((int*)_tmp13E)
!= 6)goto _LL15A;_tmp142=((struct Cyc_Absyn_Enum_d_struct*)_tmp13E)->f1;_LL159: {
struct _tuple0 _tmp157;struct _tagged_arr*_tmp158;struct _tuple0*_tmp156=_tmp142->name;
_tmp157=*_tmp156;_tmp158=_tmp157.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp158;if((unsigned int)_tmp142->fields){{struct Cyc_List_List*_tmp159=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp142->fields))->v;for(0;
_tmp159 != 0;_tmp159=_tmp159->tl){struct Cyc_Absyn_Enumfield*_tmp15A=(struct Cyc_Absyn_Enumfield*)
_tmp159->hd;Cyc_scan_exp_opt(_tmp15A->tag);}}{struct Cyc_List_List*_tmp15B=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp142->fields))->v;for(
0;_tmp15B != 0;_tmp15B=_tmp15B->tl){;}}}goto _LL151;}_LL15A: if(*((int*)_tmp13E)!= 
7)goto _LL15C;_tmp143=((struct Cyc_Absyn_Typedef_d_struct*)_tmp13E)->f1;_LL15B: {
struct _tuple0 _tmp15D;struct _tagged_arr*_tmp15E;struct _tuple0*_tmp15C=_tmp143->name;
_tmp15D=*_tmp15C;_tmp15E=_tmp15D.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp15E;if((unsigned int)_tmp143->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp143->defn))->v);goto _LL151;}_LL15C: if(*((int*)_tmp13E)!= 2)goto
_LL15E;_LL15D:({void*_tmp15F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp160="Error: unexpected let declaration\n";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,35));}),_tag_arr(_tmp15F,sizeof(void*),0));});exit(1);
return 0;_LL15E: if(*((int*)_tmp13E)!= 5)goto _LL160;_LL15F:({void*_tmp161[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp162="Error: unexpected tunion declaration\n";
_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,38));}),_tag_arr(
_tmp161,sizeof(void*),0));});exit(1);return 0;_LL160: if(*((int*)_tmp13E)!= 3)goto
_LL162;_LL161:({void*_tmp163[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp164="Error: unexpected let declaration\n";_tag_arr(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,35));}),_tag_arr(_tmp163,sizeof(void*),0));});exit(1);
return 0;_LL162: if(*((int*)_tmp13E)!= 8)goto _LL164;_LL163:({void*_tmp165[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp166="Error: unexpected namespace declaration\n";
_tag_arr(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,41));}),_tag_arr(
_tmp165,sizeof(void*),0));});exit(1);return 0;_LL164: if(*((int*)_tmp13E)!= 9)goto
_LL166;_LL165:({void*_tmp167[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp168="Error: unexpected using declaration\n";_tag_arr(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,37));}),_tag_arr(_tmp167,sizeof(void*),0));});exit(1);
return 0;_LL166: if(*((int*)_tmp13E)!= 10)goto _LL151;_LL167:({void*_tmp169[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp16A="Error: unexpected extern \"C\" declaration\n";
_tag_arr(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,42));}),_tag_arr(
_tmp169,sizeof(void*),0));});exit(1);return 0;_LL151:;}return({struct _tuple10*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->f1=(struct _tagged_arr*)_check_null(
Cyc_current_source);_tmp16B->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));
_tmp16B;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),int(*hash)(struct
_tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _tagged_arr*x){
struct _handler_cons _tmp16C;_push_handler(& _tmp16C);{int _tmp16E=0;if(setjmp(
_tmp16C.handler))_tmp16E=1;if(!_tmp16E){{struct Cyc_Set_Set*_tmp16F=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp16F;};_pop_handler();}else{void*_tmp16D=(void*)
_exn_thrown;void*_tmp171=_tmp16D;_LL169: if(_tmp171 != Cyc_Core_Not_found)goto
_LL16B;_LL16A: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL16B:;_LL16C:(void)_throw(
_tmp171);_LL168:;}}}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,
struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(curr,(
struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp172=curr;struct _tagged_arr*
_tmp173=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp172)> 0){struct Cyc_Set_Set*
_tmp174=emptyset;struct Cyc_Iter_Iter _tmp175=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp172);while(((int(*)(struct Cyc_Iter_Iter,struct _tagged_arr**))Cyc_Iter_next)(
_tmp175,& _tmp173)){_tmp174=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp174,Cyc_find(t,_tmp173));}_tmp172=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp174,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp172);}return curr;}}static void*Cyc_mode=(void*)0;
static int Cyc_gathering(){return Cyc_mode == (void*)1?1: Cyc_mode == (void*)2;}static
struct Cyc_Std___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_script_file == 0){({void*_tmp177[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp178="Internal error: script file is NULL\n";
_tag_arr(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,37));}),_tag_arr(
_tmp177,sizeof(void*),0));});exit(1);}return Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file),fmt,ap);}int Cyc_force_directory(struct _tagged_arr d){
if(Cyc_mode == (void*)2)({struct Cyc_Std_String_pa_struct _tmp17C;_tmp17C.tag=0;
_tmp17C.f1=(struct _tagged_arr)d;{struct Cyc_Std_String_pa_struct _tmp17B;_tmp17B.tag=
0;_tmp17B.f1=(struct _tagged_arr)d;{void*_tmp179[2]={& _tmp17B,& _tmp17C};Cyc_prscript(({
const char*_tmp17A="if ! test -e %s; then mkdir %s; fi\n";_tag_arr(_tmp17A,
sizeof(char),_get_zero_arr_size(_tmp17A,36));}),_tag_arr(_tmp179,sizeof(void*),2));}}});
else{int _tmp17D=({int _tmp181[0];Cyc_Std_open(d,0,_tag_arr(_tmp181,sizeof(int),0));});
if(_tmp17D == - 1){if(Cyc_Std_mkdir(d,448)== - 1){({struct Cyc_Std_String_pa_struct
_tmp180;_tmp180.tag=0;_tmp180.f1=(struct _tagged_arr)d;{void*_tmp17E[1]={& _tmp180};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp17F="Error: could not create directory %s\n";
_tag_arr(_tmp17F,sizeof(char),_get_zero_arr_size(_tmp17F,38));}),_tag_arr(
_tmp17E,sizeof(void*),1));}});return 1;}}else{close(_tmp17D);}}return 0;}int Cyc_force_directory_prefixes(
struct _tagged_arr file){struct _tagged_arr _tmp182=Cyc_Std_strdup(file);struct Cyc_List_List*
_tmp183=0;while(1){_tmp182=Cyc_Filename_dirname((struct _tagged_arr)_tmp182);if(
Cyc_Std_strlen((struct _tagged_arr)_tmp182)== 0)break;_tmp183=({struct Cyc_List_List*
_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->hd=({struct _tagged_arr*_tmp185=
_cycalloc(sizeof(*_tmp185));_tmp185[0]=(struct _tagged_arr)_tmp182;_tmp185;});
_tmp184->tl=_tmp183;_tmp184;});}for(0;_tmp183 != 0;_tmp183=_tmp183->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp183->hd))))return 1;}return 0;}char Cyc_NO_SUPPORT[
15]="\000\000\000\000NO_SUPPORT\000";struct Cyc_NO_SUPPORT_struct{char*tag;struct
_tagged_arr f1;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
int Cyc_process_file(struct _tagged_arr filename,struct Cyc_List_List*start_symbols,
struct Cyc_List_List*omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*
epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*
maybe;struct Cyc_Std___cycFILE*in_file;struct Cyc_Std___cycFILE*out_file;int
errorcode=0;({struct Cyc_Std_String_pa_struct _tmp188;_tmp188.tag=0;_tmp188.f1=(
struct _tagged_arr)filename;{void*_tmp186[1]={& _tmp188};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp187="********************************* %s...\n";_tag_arr(_tmp187,
sizeof(char),_get_zero_arr_size(_tmp187,41));}),_tag_arr(_tmp186,sizeof(void*),1));}});
if(!Cyc_gathering())({struct Cyc_Std_String_pa_struct _tmp18B;_tmp18B.tag=0;
_tmp18B.f1=(struct _tagged_arr)filename;{void*_tmp189[1]={& _tmp18B};Cyc_log(({
const char*_tmp18A="\n%s:\n";_tag_arr(_tmp18A,sizeof(char),_get_zero_arr_size(
_tmp18A,6));}),_tag_arr(_tmp189,sizeof(void*),1));}});{struct _tagged_arr _tmp18C=
Cyc_Filename_basename(filename);struct _tagged_arr _tmp18D=Cyc_Filename_dirname(
filename);struct _tagged_arr _tmp18E=Cyc_Filename_chop_extension((struct
_tagged_arr)_tmp18C);struct _tagged_arr _tmp18F=Cyc_Std_strconcat((struct
_tagged_arr)_tmp18E,({const char*_tmp2BC=".iA";_tag_arr(_tmp2BC,sizeof(char),
_get_zero_arr_size(_tmp2BC,4));}));struct _tagged_arr _tmp190=_get_arr_size(
_tmp18D,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp2B8;_tmp2B8.tag=0;
_tmp2B8.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2B6[1]={& _tmp2B8};Cyc_Std_aprintf(({
const char*_tmp2B7="%s.iB";_tag_arr(_tmp2B7,sizeof(char),_get_zero_arr_size(
_tmp2B7,6));}),_tag_arr(_tmp2B6,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp18D,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2B9[1]={&
_tmp2BB};Cyc_Std_aprintf(({const char*_tmp2BA="%s.iB";_tag_arr(_tmp2BA,sizeof(
char),_get_zero_arr_size(_tmp2BA,6));}),_tag_arr(_tmp2B9,sizeof(void*),1));}}));
struct _tagged_arr _tmp191=_get_arr_size(_tmp18D,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2B0[1]={&
_tmp2B2};Cyc_Std_aprintf(({const char*_tmp2B1="%s.iC";_tag_arr(_tmp2B1,sizeof(
char),_get_zero_arr_size(_tmp2B1,6));}),_tag_arr(_tmp2B0,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp18D,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2B3[1]={&
_tmp2B5};Cyc_Std_aprintf(({const char*_tmp2B4="%s.iC";_tag_arr(_tmp2B4,sizeof(
char),_get_zero_arr_size(_tmp2B4,6));}),_tag_arr(_tmp2B3,sizeof(void*),1));}}));
struct _tagged_arr _tmp192=_get_arr_size(_tmp18D,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct
_tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2AA[1]={&
_tmp2AC};Cyc_Std_aprintf(({const char*_tmp2AB="%s.iD";_tag_arr(_tmp2AB,sizeof(
char),_get_zero_arr_size(_tmp2AB,6));}),_tag_arr(_tmp2AA,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp18D,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _tagged_arr)_tmp18E;{void*_tmp2AD[1]={&
_tmp2AF};Cyc_Std_aprintf(({const char*_tmp2AE="%s.iD";_tag_arr(_tmp2AE,sizeof(
char),_get_zero_arr_size(_tmp2AE,6));}),_tag_arr(_tmp2AD,sizeof(void*),1));}}));
struct _tagged_arr _tmp193=_get_arr_size(_tmp18D,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct
_tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct _tagged_arr)_tmp18C;{void*_tmp2A4[1]={&
_tmp2A6};Cyc_Std_aprintf(({const char*_tmp2A5="c%s";_tag_arr(_tmp2A5,sizeof(char),
_get_zero_arr_size(_tmp2A5,4));}),_tag_arr(_tmp2A4,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp18D,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _tagged_arr)_tmp18C;{void*_tmp2A7[1]={&
_tmp2A9};Cyc_Std_aprintf(({const char*_tmp2A8="c%s";_tag_arr(_tmp2A8,sizeof(char),
_get_zero_arr_size(_tmp2A8,4));}),_tag_arr(_tmp2A7,sizeof(void*),1));}}));struct
_handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))
_tmp196=1;if(!_tmp196){if(Cyc_force_directory_prefixes(filename)){int _tmp197=1;
_npop_handler(0);return _tmp197;}if(Cyc_mode != (void*)3){Cyc_current_cpp=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);if(
Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct _tmp19A;_tmp19A.tag=0;
_tmp19A.f1=(struct _tagged_arr)_tmp18F;{void*_tmp198[1]={& _tmp19A};Cyc_prscript(({
const char*_tmp199="cat >%s <<XXX\n";_tag_arr(_tmp199,sizeof(char),
_get_zero_arr_size(_tmp199,15));}),_tag_arr(_tmp198,sizeof(void*),1));}});{
struct Cyc_List_List*_tmp19B=Cyc_current_cpp;for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){({
struct Cyc_Std_String_pa_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct
_tagged_arr)*((struct _tagged_arr*)((struct _tagged_arr*)_tmp19B->hd));{void*
_tmp19C[1]={& _tmp19E};Cyc_prscript(({const char*_tmp19D="%s";_tag_arr(_tmp19D,
sizeof(char),_get_zero_arr_size(_tmp19D,3));}),_tag_arr(_tmp19C,sizeof(void*),1));}});}}({
struct Cyc_Std_String_pa_struct _tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(struct
_tagged_arr)filename;{void*_tmp19F[1]={& _tmp1A1};Cyc_prscript(({const char*
_tmp1A0="#include <%s>\n";_tag_arr(_tmp1A0,sizeof(char),_get_zero_arr_size(
_tmp1A0,15));}),_tag_arr(_tmp19F,sizeof(void*),1));}});({void*_tmp1A2[0]={};Cyc_prscript(({
const char*_tmp1A3="XXX\n";_tag_arr(_tmp1A3,sizeof(char),_get_zero_arr_size(
_tmp1A3,5));}),_tag_arr(_tmp1A2,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(struct _tagged_arr)_tmp18F;{struct Cyc_Std_String_pa_struct
_tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _tagged_arr)_tmp190;{void*_tmp1A4[2]={&
_tmp1A6,& _tmp1A7};Cyc_prscript(({const char*_tmp1A5="$GCC -E -dM -o %s -x c %s && \\\n";
_tag_arr(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,32));}),_tag_arr(
_tmp1A4,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct _tmp1AB;_tmp1AB.tag=
0;_tmp1AB.f1=(struct _tagged_arr)_tmp18F;{struct Cyc_Std_String_pa_struct _tmp1AA;
_tmp1AA.tag=0;_tmp1AA.f1=(struct _tagged_arr)_tmp191;{void*_tmp1A8[2]={& _tmp1AA,&
_tmp1AB};Cyc_prscript(({const char*_tmp1A9="$GCC -E     -o %s -x c %s;\n";
_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,28));}),_tag_arr(
_tmp1A8,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct _tmp1AE;_tmp1AE.tag=
0;_tmp1AE.f1=(struct _tagged_arr)_tmp18F;{void*_tmp1AC[1]={& _tmp1AE};Cyc_prscript(({
const char*_tmp1AD="rm %s\n";_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(
_tmp1AD,7));}),_tag_arr(_tmp1AC,sizeof(void*),1));}});}else{maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp18F,({const char*_tmp1AF="w";_tag_arr(_tmp1AF,sizeof(char),
_get_zero_arr_size(_tmp1AF,2));}));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(struct _tagged_arr)_tmp18F;{void*_tmp1B0[1]={&
_tmp1B2};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1B1="Error: could not create file %s\n";
_tag_arr(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,33));}),_tag_arr(
_tmp1B0,sizeof(void*),1));}});{int _tmp1B3=1;_npop_handler(0);return _tmp1B3;}}
out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{struct Cyc_List_List*
_tmp1B4=Cyc_current_cpp;for(0;_tmp1B4 != 0;_tmp1B4=_tmp1B4->tl){Cyc_Std_fputs(*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp1B4->hd)),out_file);}}({struct Cyc_Std_String_pa_struct
_tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(struct _tagged_arr)filename;{void*_tmp1B5[1]={&
_tmp1B7};Cyc_Std_fprintf(out_file,({const char*_tmp1B6="#include <%s>\n";_tag_arr(
_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,15));}),_tag_arr(_tmp1B5,sizeof(
void*),1));}});Cyc_Std_fclose(out_file);{struct _tagged_arr _tmp1B8=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp1B9=({struct Cyc_Std_String_pa_struct
_tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct _tagged_arr)_tmp18F;{struct Cyc_Std_String_pa_struct
_tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=(struct _tagged_arr)_tmp190;{struct Cyc_Std_String_pa_struct
_tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct _tagged_arr)_tmp1B8;{void*_tmp1BF[3]={&
_tmp1C1,& _tmp1C2,& _tmp1C3};Cyc_Std_aprintf(({const char*_tmp1C0="%s -E -dM -o %s -x c %s";
_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,24));}),_tag_arr(
_tmp1BF,sizeof(void*),3));}}}});if(!Cyc_Std_system((struct _tagged_arr)_tmp1B9)){
_tmp1B9=({struct Cyc_Std_String_pa_struct _tmp1BE;_tmp1BE.tag=0;_tmp1BE.f1=(struct
_tagged_arr)_tmp18F;{struct Cyc_Std_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(
struct _tagged_arr)_tmp191;{struct Cyc_Std_String_pa_struct _tmp1BC;_tmp1BC.tag=0;
_tmp1BC.f1=(struct _tagged_arr)_tmp1B8;{void*_tmp1BA[3]={& _tmp1BC,& _tmp1BD,&
_tmp1BE};Cyc_Std_aprintf(({const char*_tmp1BB="%s -E -o %s -x c %s";_tag_arr(
_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,20));}),_tag_arr(_tmp1BA,sizeof(
void*),3));}}}});Cyc_Std_system((struct _tagged_arr)_tmp1B9);}Cyc_Std_remove((
struct _tagged_arr)_tmp18F);}}}if(Cyc_gathering()){int _tmp1C4=0;_npop_handler(0);
return _tmp1C4;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp190,({const char*_tmp1C5="r";_tag_arr(_tmp1C5,sizeof(char),
_get_zero_arr_size(_tmp1C5,2));}));if(!((unsigned int)maybe))(int)_throw((void*)({
struct Cyc_NO_SUPPORT_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({
struct Cyc_NO_SUPPORT_struct _tmp1C7;_tmp1C7.tag=Cyc_NO_SUPPORT;_tmp1C7.f1=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(
struct _tagged_arr)_tmp190;{void*_tmp1C8[1]={& _tmp1CA};Cyc_Std_aprintf(({const
char*_tmp1C9="can't open macrosfile %s";_tag_arr(_tmp1C9,sizeof(char),
_get_zero_arr_size(_tmp1C9,25));}),_tag_arr(_tmp1C8,sizeof(void*),1));}});
_tmp1C7;});_tmp1C6;}));in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{
struct Cyc_Lexing_lexbuf*_tmp1CB=Cyc_Lexing_from_file(in_file);struct _tuple10*
entry;while((entry=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(
_tmp1CB))!= 0){struct _tuple10 _tmp1CD;struct _tagged_arr*_tmp1CE;struct Cyc_Set_Set*
_tmp1CF;struct _tuple10*_tmp1CC=(struct _tuple10*)_check_null(entry);_tmp1CD=*
_tmp1CC;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1CE,
_tmp1CF);}Cyc_Std_fclose(in_file);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp191,({
const char*_tmp1D0="r";_tag_arr(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,2));}));
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_NO_SUPPORT_struct
_tmp1D2;_tmp1D2.tag=Cyc_NO_SUPPORT;_tmp1D2.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _tagged_arr)_tmp191;{void*_tmp1D3[1]={&
_tmp1D5};Cyc_Std_aprintf(({const char*_tmp1D4="can't open declsfile %s";_tag_arr(
_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,24));}),_tag_arr(_tmp1D3,sizeof(
void*),1));}});_tmp1D2;});_tmp1D1;}));in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);_tmp1CB=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_Std_fopen((
struct _tagged_arr)_tmp192,({const char*_tmp1D6="w";_tag_arr(_tmp1D6,sizeof(char),
_get_zero_arr_size(_tmp1D6,2));}));if(!((unsigned int)Cyc_slurp_out)){int _tmp1D7=
1;_npop_handler(0);return _tmp1D7;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp1CB)){;}Cyc_Std_fclose(in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)Cyc_Std_remove((struct
_tagged_arr)_tmp191);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp192,({const char*
_tmp1D8="r";_tag_arr(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,2));}));if(
!((unsigned int)maybe)){int _tmp1D9=1;_npop_handler(0);return _tmp1D9;}in_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_Position_reset_position((struct
_tagged_arr)_tmp192);Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp1DA=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp192);{struct Cyc_List_List*_tmp1DB=_tmp1DA;for(0;_tmp1DB != 0;
_tmp1DB=_tmp1DB->tl){struct _tuple10*_tmp1DC=Cyc_scan_decl((struct Cyc_Absyn_Decl*)
_tmp1DB->hd);if(_tmp1DC == 0)continue;{struct _tuple10 _tmp1DE;struct _tagged_arr*
_tmp1DF;struct Cyc_Set_Set*_tmp1E0;struct _tuple10*_tmp1DD=(struct _tuple10*)
_check_null(_tmp1DC);_tmp1DE=*_tmp1DD;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;{
struct Cyc_Set_Set*old;{struct _handler_cons _tmp1E1;_push_handler(& _tmp1E1);{int
_tmp1E3=0;if(setjmp(_tmp1E1.handler))_tmp1E3=1;if(!_tmp1E3){old=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,
_tmp1DF);;_pop_handler();}else{void*_tmp1E2=(void*)_exn_thrown;void*_tmp1E5=
_tmp1E2;_LL16E: if(_tmp1E5 != Cyc_Core_Not_found)goto _LL170;_LL16F: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL16D;_LL170:;_LL171:(void)_throw(_tmp1E5);_LL16D:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1DF,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp1E0,old));}}}}{struct Cyc_Set_Set*_tmp1E6=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp1E7=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp1E8=_tmp1DA;for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){
struct Cyc_Absyn_Decl*_tmp1E9=(struct Cyc_Absyn_Decl*)_tmp1E8->hd;struct
_tagged_arr*name;{void*_tmp1EA=(void*)_tmp1E9->r;struct Cyc_Absyn_Vardecl*_tmp1EB;
struct Cyc_Absyn_Fndecl*_tmp1EC;struct Cyc_Absyn_Aggrdecl*_tmp1ED;struct Cyc_Absyn_Enumdecl*
_tmp1EE;struct Cyc_Absyn_Typedefdecl*_tmp1EF;_LL173: if(*((int*)_tmp1EA)!= 0)goto
_LL175;_tmp1EB=((struct Cyc_Absyn_Var_d_struct*)_tmp1EA)->f1;_LL174: {struct
_tuple0 _tmp1F1;struct _tagged_arr*_tmp1F2;struct _tuple0*_tmp1F0=_tmp1EB->name;
_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp1F2);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp1F2))name=0;else{name=(struct _tagged_arr*)_tmp1F2;}goto _LL172;}
_LL175: if(*((int*)_tmp1EA)!= 1)goto _LL177;_tmp1EC=((struct Cyc_Absyn_Fn_d_struct*)
_tmp1EA)->f1;_LL176: {struct _tuple0 _tmp1F4;struct _tagged_arr*_tmp1F5;struct
_tuple0*_tmp1F3=_tmp1EC->name;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp1F5);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp1F5))name=0;else{name=(struct _tagged_arr*)
_tmp1F5;}goto _LL172;}_LL177: if(*((int*)_tmp1EA)!= 4)goto _LL179;_tmp1ED=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1EA)->f1;_LL178: {struct _tuple0 _tmp1F7;struct
_tagged_arr*_tmp1F8;struct _tuple0*_tmp1F6=_tmp1ED->name;_tmp1F7=*_tmp1F6;_tmp1F8=
_tmp1F7.f2;name=(struct _tagged_arr*)_tmp1F8;goto _LL172;}_LL179: if(*((int*)
_tmp1EA)!= 6)goto _LL17B;_tmp1EE=((struct Cyc_Absyn_Enum_d_struct*)_tmp1EA)->f1;
_LL17A: {struct _tuple0 _tmp1FA;struct _tagged_arr*_tmp1FB;struct _tuple0*_tmp1F9=
_tmp1EE->name;_tmp1FA=*_tmp1F9;_tmp1FB=_tmp1FA.f2;name=(struct _tagged_arr*)
_tmp1FB;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp1E6,(struct _tagged_arr*)_check_null(name)): 0)_tmp1E7=({struct Cyc_List_List*
_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=_tmp1E9;_tmp1FC->tl=_tmp1E7;
_tmp1FC;});else{if((unsigned int)_tmp1EE->fields){struct Cyc_List_List*_tmp1FD=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1EE->fields))->v;for(
0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){struct Cyc_Absyn_Enumfield*_tmp1FE=(struct Cyc_Absyn_Enumfield*)
_tmp1FD->hd;struct _tuple0 _tmp200;struct _tagged_arr*_tmp201;struct _tuple0*_tmp1FF=
_tmp1FE->name;_tmp200=*_tmp1FF;_tmp201=_tmp200.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp1E6,_tmp201)){_tmp1E7=({struct Cyc_List_List*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=_tmp1E9;_tmp202->tl=_tmp1E7;
_tmp202;});break;}}}}name=0;goto _LL172;}_LL17B: if(*((int*)_tmp1EA)!= 7)goto
_LL17D;_tmp1EF=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1EA)->f1;_LL17C: {struct
_tuple0 _tmp204;struct _tagged_arr*_tmp205;struct _tuple0*_tmp203=_tmp1EF->name;
_tmp204=*_tmp203;_tmp205=_tmp204.f2;name=(struct _tagged_arr*)_tmp205;goto _LL172;}
_LL17D: if(*((int*)_tmp1EA)!= 2)goto _LL17F;_LL17E: goto _LL180;_LL17F: if(*((int*)
_tmp1EA)!= 5)goto _LL181;_LL180: goto _LL182;_LL181: if(*((int*)_tmp1EA)!= 3)goto
_LL183;_LL182: goto _LL184;_LL183: if(*((int*)_tmp1EA)!= 8)goto _LL185;_LL184: goto
_LL186;_LL185: if(*((int*)_tmp1EA)!= 9)goto _LL187;_LL186: goto _LL188;_LL187: if(*((
int*)_tmp1EA)!= 10)goto _LL172;_LL188: name=0;goto _LL172;_LL172:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp1E6,(
struct _tagged_arr*)_check_null(name)): 0)_tmp1E7=({struct Cyc_List_List*_tmp206=
_cycalloc(sizeof(*_tmp206));_tmp206->hd=_tmp1E9;_tmp206->tl=_tmp1E7;_tmp206;});}}
maybe=Cyc_Std_fopen(filename,({const char*_tmp207="w";_tag_arr(_tmp207,sizeof(
char),_get_zero_arr_size(_tmp207,2));}));if(!((unsigned int)maybe)){int _tmp208=1;
_npop_handler(0);return _tmp208;}out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);({struct Cyc_Std_String_pa_struct _tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct
_tagged_arr)_tmp193;{void*_tmp209[1]={& _tmp20B};Cyc_Std_fprintf(out_file,({const
char*_tmp20A="#include <%s>\nusing Std;\n";_tag_arr(_tmp20A,sizeof(char),
_get_zero_arr_size(_tmp20A,26));}),_tag_arr(_tmp209,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp193,({const char*_tmp20C="w";
_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,2));}));if(!((
unsigned int)maybe)){int _tmp20D=1;_npop_handler(0);return _tmp20D;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);{struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct
_tagged_arr)filename;{void*_tmp288[1]={& _tmp28A};Cyc_Std_aprintf(({const char*
_tmp289="_%s_";_tag_arr(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,5));}),
_tag_arr(_tmp288,sizeof(void*),1));}});{int _tmp20E=0;for(0;_tmp20E < 
_get_arr_size(ifdefmacro,sizeof(char));_tmp20E ++){if(((char*)ifdefmacro.curr)[
_tmp20E]== '.'?1:((char*)ifdefmacro.curr)[_tmp20E]== '/')({struct _tagged_arr
_tmp20F=_tagged_arr_plus(ifdefmacro,sizeof(char),_tmp20E);char _tmp210=*((char*)
_check_unknown_subscript(_tmp20F,sizeof(char),0));char _tmp211='_';if(
_get_arr_size(_tmp20F,sizeof(char))== 1?_tmp210 == '\000'?_tmp211 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp20F.curr)=_tmp211;});else{if(((char*)
ifdefmacro.curr)[_tmp20E]!= '_'?((char*)ifdefmacro.curr)[_tmp20E]!= '/': 0)({
struct _tagged_arr _tmp212=_tagged_arr_plus(ifdefmacro,sizeof(char),_tmp20E);char
_tmp213=*((char*)_check_unknown_subscript(_tmp212,sizeof(char),0));char _tmp214=(
char)toupper((int)((char*)ifdefmacro.curr)[_tmp20E]);if(_get_arr_size(_tmp212,
sizeof(char))== 1?_tmp213 == '\000'?_tmp214 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp212.curr)=_tmp214;});}}}({struct Cyc_Std_String_pa_struct _tmp218;
_tmp218.tag=0;_tmp218.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp215[2]={&
_tmp217,& _tmp218};Cyc_Std_fprintf(out_file,({const char*_tmp216="#ifndef %s\n#define %s\n";
_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,23));}),_tag_arr(
_tmp215,sizeof(void*),2));}}});if(prologue != 0){struct Cyc_List_List*_tmp219=
prologue;for(0;_tmp219 != 0;_tmp219=_tmp219->tl){struct _tuple12 _tmp21B;struct
_tagged_arr _tmp21C;struct _tagged_arr _tmp21D;struct _tuple12*_tmp21A=(struct
_tuple12*)_tmp219->hd;_tmp21B=*_tmp21A;_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;if(
_tmp21D.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp21C.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp21E=
_cycalloc(sizeof(*_tmp21E));_tmp21E[0]=_tmp21C;_tmp21E;})): 0)Cyc_Std_fputs(
_tmp21D,out_file);else{({struct Cyc_Std_String_pa_struct _tmp221;_tmp221.tag=0;
_tmp221.f1=(struct _tagged_arr)_tmp21C;{void*_tmp21F[1]={& _tmp221};Cyc_log(({
const char*_tmp220="%s is not supported on this platform\n";_tag_arr(_tmp220,
sizeof(char),_get_zero_arr_size(_tmp220,38));}),_tag_arr(_tmp21F,sizeof(void*),1));}});}}}{
struct Cyc_List_List*_tmp222=0;struct Cyc_List_List*_tmp223=0;{struct Cyc_List_List*
_tmp224=_tmp1E7;for(0;_tmp224 != 0;_tmp224=_tmp224->tl){struct Cyc_Absyn_Decl*
_tmp225=(struct Cyc_Absyn_Decl*)_tmp224->hd;int _tmp226=0;struct _tagged_arr*name;{
void*_tmp227=(void*)_tmp225->r;struct Cyc_Absyn_Vardecl*_tmp228;struct Cyc_Absyn_Fndecl*
_tmp229;struct Cyc_Absyn_Aggrdecl*_tmp22A;struct Cyc_Absyn_Enumdecl*_tmp22B;struct
Cyc_Absyn_Typedefdecl*_tmp22C;_LL18A: if(*((int*)_tmp227)!= 0)goto _LL18C;_tmp228=((
struct Cyc_Absyn_Var_d_struct*)_tmp227)->f1;_LL18B: {struct _tuple0 _tmp22E;struct
_tagged_arr*_tmp22F;struct _tuple0*_tmp22D=_tmp228->name;_tmp22E=*_tmp22D;_tmp22F=
_tmp22E.f2;name=(struct _tagged_arr*)_tmp22F;goto _LL189;}_LL18C: if(*((int*)
_tmp227)!= 1)goto _LL18E;_tmp229=((struct Cyc_Absyn_Fn_d_struct*)_tmp227)->f1;
_LL18D: {struct _tuple0 _tmp231;struct _tagged_arr*_tmp232;struct _tuple0*_tmp230=
_tmp229->name;_tmp231=*_tmp230;_tmp232=_tmp231.f2;name=(struct _tagged_arr*)
_tmp232;goto _LL189;}_LL18E: if(*((int*)_tmp227)!= 4)goto _LL190;_tmp22A=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp227)->f1;_LL18F: {struct _tuple0 _tmp234;struct
_tagged_arr*_tmp235;struct _tuple0*_tmp233=_tmp22A->name;_tmp234=*_tmp233;_tmp235=
_tmp234.f2;name=(struct _tagged_arr*)_tmp235;goto _LL189;}_LL190: if(*((int*)
_tmp227)!= 6)goto _LL192;_tmp22B=((struct Cyc_Absyn_Enum_d_struct*)_tmp227)->f1;
_LL191: {struct _tuple0 _tmp237;struct _tagged_arr*_tmp238;struct _tuple0*_tmp236=
_tmp22B->name;_tmp237=*_tmp236;_tmp238=_tmp237.f2;name=(struct _tagged_arr*)
_tmp238;goto _LL189;}_LL192: if(*((int*)_tmp227)!= 7)goto _LL194;_tmp22C=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp227)->f1;_LL193: {struct _tuple0 _tmp23A;struct
_tagged_arr*_tmp23B;struct _tuple0*_tmp239=_tmp22C->name;_tmp23A=*_tmp239;_tmp23B=
_tmp23A.f2;name=(struct _tagged_arr*)_tmp23B;goto _LL189;}_LL194: if(*((int*)
_tmp227)!= 2)goto _LL196;_LL195: goto _LL197;_LL196: if(*((int*)_tmp227)!= 5)goto
_LL198;_LL197: goto _LL199;_LL198: if(*((int*)_tmp227)!= 3)goto _LL19A;_LL199: goto
_LL19B;_LL19A: if(*((int*)_tmp227)!= 8)goto _LL19C;_LL19B: goto _LL19D;_LL19C: if(*((
int*)_tmp227)!= 9)goto _LL19E;_LL19D: goto _LL19F;_LL19E: if(*((int*)_tmp227)!= 10)
goto _LL189;_LL19F: name=0;goto _LL189;_LL189:;}if(!((unsigned int)name)?!_tmp226: 0)
continue;_tmp222=({struct Cyc_List_List*_tmp23C=_cycalloc(sizeof(*_tmp23C));
_tmp23C->hd=_tmp225;_tmp23C->tl=_tmp222;_tmp23C;});_tmp223=({struct Cyc_List_List*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->hd=name;_tmp23D->tl=_tmp223;_tmp23D;});}}{
struct Cyc_Tcenv_Tenv*_tmp23E=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp23E,1,_tmp222);({
void*_tmp23F[0]={};Cyc_Std_fprintf(out_file,({const char*_tmp240="\nnamespace Std {\n";
_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,18));}),_tag_arr(
_tmp23F,sizeof(void*),0));});{struct Cyc_List_List*_tmp243;struct Cyc_List_List*
_tmp244;struct _tuple16 _tmp242=({struct _tuple16 _tmp241;_tmp241.f1=_tmp222;_tmp241.f2=
_tmp223;_tmp241;});_tmp243=_tmp242.f1;_tmp244=_tmp242.f2;for(0;_tmp243 != 0?
_tmp244 != 0: 0;(_tmp243=_tmp243->tl,_tmp244=_tmp244->tl)){struct Cyc_Absyn_Decl*
_tmp245=(struct Cyc_Absyn_Decl*)_tmp243->hd;struct _tagged_arr*_tmp246=(struct
_tagged_arr*)_tmp244->hd;int _tmp247=0;if(!((unsigned int)_tmp246))_tmp247=1;if((
unsigned int)_tmp246){ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp24A;_tmp24A.tag=
0;_tmp24A.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp246);{void*_tmp248[1]={&
_tmp24A};Cyc_Std_aprintf(({const char*_tmp249="_%s_def_";_tag_arr(_tmp249,sizeof(
char),_get_zero_arr_size(_tmp249,9));}),_tag_arr(_tmp248,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp24B[1]={& _tmp24D};Cyc_Std_fprintf(out_file,({
const char*_tmp24C="#ifndef %s\n";_tag_arr(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,12));}),_tag_arr(_tmp24B,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp250;_tmp250.tag=0;_tmp250.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp24E[1]={& _tmp250};Cyc_Std_fprintf(out_file,({
const char*_tmp24F="#define %s\n";_tag_arr(_tmp24F,sizeof(char),
_get_zero_arr_size(_tmp24F,12));}),_tag_arr(_tmp24E,sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({
struct Cyc_Absyn_Decl*_tmp251[1];_tmp251[0]=_tmp245;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp251,sizeof(struct Cyc_Absyn_Decl*),
1));}),out_file);({void*_tmp252[0]={};Cyc_Std_fprintf(out_file,({const char*
_tmp253="#endif\n";_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,8));}),
_tag_arr(_tmp252,sizeof(void*),0));});}else{Cyc_Absynpp_decllist2file(({struct
Cyc_Absyn_Decl*_tmp254[1];_tmp254[0]=_tmp245;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp254,sizeof(struct Cyc_Absyn_Decl*),1));}),
out_file);}}}maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp190,({const char*_tmp255="r";
_tag_arr(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,2));}));if(!((
unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_struct*_tmp256=
_cycalloc(sizeof(*_tmp256));_tmp256[0]=({struct Cyc_NO_SUPPORT_struct _tmp257;
_tmp257.tag=Cyc_NO_SUPPORT;_tmp257.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp25A;_tmp25A.tag=0;_tmp25A.f1=(struct _tagged_arr)_tmp190;{void*_tmp258[1]={&
_tmp25A};Cyc_Std_aprintf(({const char*_tmp259="can't open macrosfile %s";_tag_arr(
_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,25));}),_tag_arr(_tmp258,sizeof(
void*),1));}});_tmp257;});_tmp256;}));in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);_tmp1CB=Cyc_Lexing_from_file(in_file);{struct _tuple11*entry2;
while((entry2=((struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(
_tmp1CB))!= 0){struct _tuple11 _tmp25C;struct _tagged_arr _tmp25D;struct _tagged_arr*
_tmp25E;struct _tuple11*_tmp25B=(struct _tuple11*)_check_null(entry2);_tmp25C=*
_tmp25B;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;if(((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(_tmp1E6,_tmp25E)){({struct Cyc_Std_String_pa_struct
_tmp261;_tmp261.tag=0;_tmp261.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp25E);{void*_tmp25F[1]={& _tmp261};Cyc_Std_fprintf(out_file,({const char*
_tmp260="#ifndef %s\n";_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,
12));}),_tag_arr(_tmp25F,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp264;_tmp264.tag=0;_tmp264.f1=(struct _tagged_arr)_tmp25D;{void*_tmp262[1]={&
_tmp264};Cyc_Std_fprintf(out_file,({const char*_tmp263="%s\n";_tag_arr(_tmp263,
sizeof(char),_get_zero_arr_size(_tmp263,4));}),_tag_arr(_tmp262,sizeof(void*),1));}});({
void*_tmp265[0]={};Cyc_Std_fprintf(out_file,({const char*_tmp266="#endif\n";
_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,8));}),_tag_arr(_tmp265,
sizeof(void*),0));});}}Cyc_Std_fclose(in_file);if(Cyc_mode != (void*)3)Cyc_Std_remove((
struct _tagged_arr)_tmp190);if(epilogue != 0){struct Cyc_List_List*_tmp267=epilogue;
for(0;_tmp267 != 0;_tmp267=_tmp267->tl){struct _tuple12 _tmp269;struct _tagged_arr
_tmp26A;struct _tagged_arr _tmp26B;struct _tuple12*_tmp268=(struct _tuple12*)_tmp267->hd;
_tmp269=*_tmp268;_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;if(_tmp26B.curr != ((
struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp26A.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[
0]=_tmp26A;_tmp26C;})): 0)Cyc_Std_fputs(_tmp26B,out_file);else{({struct Cyc_Std_String_pa_struct
_tmp26F;_tmp26F.tag=0;_tmp26F.f1=(struct _tagged_arr)_tmp26A;{void*_tmp26D[1]={&
_tmp26F};Cyc_log(({const char*_tmp26E="%s is not supported on this platform\n";
_tag_arr(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,38));}),_tag_arr(
_tmp26D,sizeof(void*),1));}});}}}({void*_tmp270[0]={};Cyc_Std_fprintf(out_file,({
const char*_tmp271="}\n";_tag_arr(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,
3));}),_tag_arr(_tmp270,sizeof(void*),0));});({void*_tmp272[0]={};Cyc_Std_fprintf(
out_file,({const char*_tmp273="#endif\n";_tag_arr(_tmp273,sizeof(char),
_get_zero_arr_size(_tmp273,8));}),_tag_arr(_tmp272,sizeof(void*),0));});Cyc_Std_fclose(
out_file);if(cstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp274=cstubs;for(0;_tmp274 != 0;_tmp274=_tmp274->tl){struct
_tuple12 _tmp276;struct _tagged_arr _tmp277;struct _tagged_arr _tmp278;struct _tuple12*
_tmp275=(struct _tuple12*)_tmp274->hd;_tmp276=*_tmp275;_tmp277=_tmp276.f1;_tmp278=
_tmp276.f2;if(_tmp278.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp277.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=_tmp277;_tmp279;})): 0)Cyc_Std_fputs(
_tmp278,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _tmp27C;_tmp27C.tag=0;_tmp27C.f1=(struct
_tagged_arr)_tmp193;{void*_tmp27A[1]={& _tmp27C};Cyc_Std_fprintf(out_file,({const
char*_tmp27B="#include <%s>\n\n";_tag_arr(_tmp27B,sizeof(char),
_get_zero_arr_size(_tmp27B,16));}),_tag_arr(_tmp27A,sizeof(void*),1));}});if(
cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
void*_tmp27D[0]={};Cyc_Std_fprintf(out_file,({const char*_tmp27E="namespace Std {\n";
_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,17));}),_tag_arr(
_tmp27D,sizeof(void*),0));});{struct Cyc_List_List*_tmp27F=cycstubs;for(0;_tmp27F
!= 0;_tmp27F=_tmp27F->tl){struct _tuple12 _tmp281;struct _tagged_arr _tmp282;struct
_tagged_arr _tmp283;struct _tuple12*_tmp280=(struct _tuple12*)_tmp27F->hd;_tmp281=*
_tmp280;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;if(_tmp283.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp282.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284[
0]=_tmp282;_tmp284;})): 0)Cyc_Std_fputs(_tmp283,out_file);}}({void*_tmp285[0]={};
Cyc_Std_fprintf(out_file,({const char*_tmp286="}\n\n";_tag_arr(_tmp286,sizeof(
char),_get_zero_arr_size(_tmp286,4));}),_tag_arr(_tmp285,sizeof(void*),0));});}{
int _tmp287=0;_npop_handler(0);return _tmp287;}}}}}}}}};_pop_handler();}else{void*
_tmp195=(void*)_exn_thrown;void*_tmp28C=_tmp195;struct _tagged_arr _tmp28D;_LL1A1:
if(*((void**)_tmp28C)!= Cyc_NO_SUPPORT)goto _LL1A3;_tmp28D=((struct Cyc_NO_SUPPORT_struct*)
_tmp28C)->f1;_LL1A2:({struct Cyc_Std_String_pa_struct _tmp290;_tmp290.tag=0;
_tmp290.f1=(struct _tagged_arr)_tmp28D;{void*_tmp28E[1]={& _tmp290};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp28F="No support because %s\n";_tag_arr(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,23));}),_tag_arr(_tmp28E,sizeof(void*),1));}});
goto _LL1A4;_LL1A3:;_LL1A4: maybe=Cyc_Std_fopen(filename,({const char*_tmp291="w";
_tag_arr(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,2));}));if(!((
unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp294;_tmp294.tag=0;
_tmp294.f1=(struct _tagged_arr)filename;{void*_tmp292[1]={& _tmp294};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp293="Error: could not create file %s\n";_tag_arr(
_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,33));}),_tag_arr(_tmp292,sizeof(
void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);({
struct Cyc_Std_String_pa_struct _tmp297;_tmp297.tag=0;_tmp297.f1=(struct
_tagged_arr)filename;{void*_tmp295[1]={& _tmp297};Cyc_Std_fprintf(out_file,({
const char*_tmp296="#error -- %s is not supported on this platform\n";_tag_arr(
_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,48));}),_tag_arr(_tmp295,sizeof(
void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp193,({const char*_tmp298="w";_tag_arr(_tmp298,sizeof(char),_get_zero_arr_size(
_tmp298,2));}));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp29B;_tmp29B.tag=0;_tmp29B.f1=(struct _tagged_arr)_tmp193;{void*_tmp299[1]={&
_tmp29B};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp29A="Error: could not create file %s\n";
_tag_arr(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,33));}),_tag_arr(
_tmp299,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(
struct _tagged_arr)_tmp193;{void*_tmp29C[1]={& _tmp29E};Cyc_Std_fprintf(out_file,({
const char*_tmp29D="#error -- %s is not supported on this platform\n";_tag_arr(
_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,48));}),_tag_arr(_tmp29C,sizeof(
void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct _tmp2A1;
_tmp2A1.tag=0;_tmp2A1.f1=(struct _tagged_arr)filename;{void*_tmp29F[1]={& _tmp2A1};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2A0="Warning: %s will not be supported on this platform\n";
_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,52));}),_tag_arr(
_tmp29F,sizeof(void*),1));}});({void*_tmp2A2[0]={};Cyc_log(({const char*_tmp2A3="Not supported on this platform\n";
_tag_arr(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,32));}),_tag_arr(
_tmp2A2,sizeof(void*),0));});Cyc_Std_remove((struct _tagged_arr)_tmp190);Cyc_Std_remove((
struct _tagged_arr)_tmp191);Cyc_Std_remove((struct _tagged_arr)_tmp192);return 0;
_LL1A5:;_LL1A6:(void)_throw(_tmp28C);_LL1A0:;}}}}int Cyc_process_specfile(struct
_tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp2BD=Cyc_Std_fopen(
file,({const char*_tmp2DB="r";_tag_arr(_tmp2DB,sizeof(char),_get_zero_arr_size(
_tmp2DB,2));}));if(!((unsigned int)_tmp2BD)){({struct Cyc_Std_String_pa_struct
_tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=(struct _tagged_arr)file;{void*_tmp2BE[1]={&
_tmp2C0};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2BF="Error: could not open %s\n";
_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,26));}),_tag_arr(
_tmp2BE,sizeof(void*),1));}});return 1;}{struct Cyc_Std___cycFILE*_tmp2C1=(struct
Cyc_Std___cycFILE*)_check_null(_tmp2BD);struct _tagged_arr buf=({char*_tmp2DA=({
unsigned int _tmp2D7=(unsigned int)1024;char*_tmp2D8=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp2D7 + 1));{unsigned int _tmp2D9=_tmp2D7;unsigned int
i;for(i=0;i < _tmp2D9;i ++){_tmp2D8[i]='\000';}_tmp2D8[_tmp2D9]=(char)0;}_tmp2D8;});
_tag_arr(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,(unsigned int)1024 + 1));});
struct _tagged_arr _tmp2C2=Cyc_Std_getcwd(buf,_get_arr_size(buf,sizeof(char)));if(
Cyc_mode != (void*)2){if(Cyc_Std_chdir(dir)){({struct Cyc_Std_String_pa_struct
_tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct _tagged_arr)dir;{void*_tmp2C3[1]={&
_tmp2C5};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2C4="Error: can't change directory to %s\n";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,37));}),_tag_arr(
_tmp2C3,sizeof(void*),1));}});return 1;}}if(Cyc_mode == (void*)1){struct
_tagged_arr _tmp2C6=Cstring_to_string(Ccomp);Cyc_Std_system((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=(struct
_tagged_arr)_tmp2C6;{void*_tmp2C7[1]={& _tmp2C9};Cyc_Std_aprintf(({const char*
_tmp2C8="echo | %s -E -dM - -o INITMACROS.h\n";_tag_arr(_tmp2C8,sizeof(char),
_get_zero_arr_size(_tmp2C8,36));}),_tag_arr(_tmp2C7,sizeof(void*),1));}}));}{
struct Cyc_Lexing_lexbuf*_tmp2CA=Cyc_Lexing_from_file(_tmp2C1);struct _tuple13*
entry;while((entry=((struct _tuple13*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp2CA))!= 0){struct _tuple13 _tmp2CC;struct _tagged_arr _tmp2CD;struct Cyc_List_List*
_tmp2CE;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*
_tmp2D1;struct Cyc_List_List*_tmp2D2;struct Cyc_List_List*_tmp2D3;struct _tuple13*
_tmp2CB=(struct _tuple13*)_check_null(entry);_tmp2CC=*_tmp2CB;_tmp2CD=_tmp2CC.f1;
_tmp2CE=_tmp2CC.f2;_tmp2CF=_tmp2CC.f3;_tmp2D0=_tmp2CC.f4;_tmp2D1=_tmp2CC.f5;
_tmp2D2=_tmp2CC.f6;_tmp2D3=_tmp2CC.f7;if(Cyc_process_file(_tmp2CD,_tmp2CE,
_tmp2CF,_tmp2D0,_tmp2D1,_tmp2D2,_tmp2D3))return 1;}Cyc_Std_fclose(_tmp2C1);if(Cyc_mode
!= (void*)2){if(Cyc_Std_chdir((struct _tagged_arr)_tmp2C2)){({struct Cyc_Std_String_pa_struct
_tmp2D6;_tmp2D6.tag=0;_tmp2D6.f1=(struct _tagged_arr)_tmp2C2;{void*_tmp2D4[1]={&
_tmp2D6};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2D5="Error: could not change directory to %s\n";
_tag_arr(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,41));}),_tag_arr(
_tmp2D4,sizeof(void*),1));}});return 1;}}return 0;}}}static char _tmp2DC[13]="BUILDLIB.OUT";
static struct _tagged_arr Cyc_output_dir={_tmp2DC,_tmp2DC,_tmp2DC + 13};static void
Cyc_set_output_dir(struct _tagged_arr s){Cyc_output_dir=s;}static struct Cyc_List_List*
Cyc_spec_files=0;static void Cyc_add_spec_file(struct _tagged_arr s){Cyc_spec_files=({
struct Cyc_List_List*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->hd=({struct
_tagged_arr*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE[0]=s;_tmp2DE;});_tmp2DD->tl=
Cyc_spec_files;_tmp2DD;});}static void Cyc_set_GATHER(){Cyc_mode=(void*)1;}static
void Cyc_set_GATHERSCRIPT(){Cyc_mode=(void*)2;}static void Cyc_set_FINISH(){Cyc_mode=(
void*)3;}static int Cyc_badparse=0;static void Cyc_unsupported_option(struct
_tagged_arr s){({struct Cyc_Std_String_pa_struct _tmp2E1;_tmp2E1.tag=0;_tmp2E1.f1=(
struct _tagged_arr)s;{void*_tmp2DF[1]={& _tmp2E1};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp2E0="Unsupported option %s\n";_tag_arr(_tmp2E0,sizeof(char),
_get_zero_arr_size(_tmp2E0,23));}),_tag_arr(_tmp2DF,sizeof(void*),1));}});Cyc_badparse=
1;}void GC_blacklist_warn_clear();struct _tuple17{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*options=({struct
_tuple17*_tmp307[5];_tmp307[4]=({struct _tuple17*_tmp320=_cycalloc(sizeof(*
_tmp320));_tmp320->f1=({const char*_tmp325="-";_tag_arr(_tmp325,sizeof(char),
_get_zero_arr_size(_tmp325,2));});_tmp320->f2=1;_tmp320->f3=({const char*_tmp324="";
_tag_arr(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,1));});_tmp320->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp322=_cycalloc(sizeof(*_tmp322));
_tmp322[0]=({struct Cyc_Arg_Flag_spec_struct _tmp323;_tmp323.tag=1;_tmp323.f1=Cyc_unsupported_option;
_tmp323;});_tmp322;});_tmp320->f5=({const char*_tmp321="";_tag_arr(_tmp321,
sizeof(char),_get_zero_arr_size(_tmp321,1));});_tmp320;});_tmp307[3]=({struct
_tuple17*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->f1=({const char*_tmp31F="-finish";
_tag_arr(_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,8));});_tmp31A->f2=0;
_tmp31A->f3=({const char*_tmp31E="";_tag_arr(_tmp31E,sizeof(char),
_get_zero_arr_size(_tmp31E,1));});_tmp31A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp31D;_tmp31D.tag=0;_tmp31D.f1=Cyc_set_FINISH;_tmp31D;});_tmp31C;});_tmp31A->f5=({
const char*_tmp31B="Produce Cyclone headers from pre-gathered C library info";
_tag_arr(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,57));});_tmp31A;});
_tmp307[2]=({struct _tuple17*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->f1=({
const char*_tmp319="-gatherscript";_tag_arr(_tmp319,sizeof(char),
_get_zero_arr_size(_tmp319,14));});_tmp314->f2=0;_tmp314->f3=({const char*_tmp318="";
_tag_arr(_tmp318,sizeof(char),_get_zero_arr_size(_tmp318,1));});_tmp314->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp316=_cycalloc(sizeof(*_tmp316));
_tmp316[0]=({struct Cyc_Arg_Unit_spec_struct _tmp317;_tmp317.tag=0;_tmp317.f1=Cyc_set_GATHERSCRIPT;
_tmp317;});_tmp316;});_tmp314->f5=({const char*_tmp315="Produce a script to gather C library info";
_tag_arr(_tmp315,sizeof(char),_get_zero_arr_size(_tmp315,42));});_tmp314;});
_tmp307[1]=({struct _tuple17*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->f1=({
const char*_tmp313="-gather";_tag_arr(_tmp313,sizeof(char),_get_zero_arr_size(
_tmp313,8));});_tmp30E->f2=0;_tmp30E->f3=({const char*_tmp312="";_tag_arr(_tmp312,
sizeof(char),_get_zero_arr_size(_tmp312,1));});_tmp30E->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp311;_tmp311.tag=0;_tmp311.f1=Cyc_set_GATHER;_tmp311;});_tmp310;});_tmp30E->f5=({
const char*_tmp30F="Gather C library info but don't produce Cyclone headers";
_tag_arr(_tmp30F,sizeof(char),_get_zero_arr_size(_tmp30F,56));});_tmp30E;});
_tmp307[0]=({struct _tuple17*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->f1=({
const char*_tmp30D="-d";_tag_arr(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,
3));});_tmp308->f2=0;_tmp308->f3=({const char*_tmp30C=" <file>";_tag_arr(_tmp30C,
sizeof(char),_get_zero_arr_size(_tmp30C,8));});_tmp308->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=({struct Cyc_Arg_String_spec_struct
_tmp30B;_tmp30B.tag=5;_tmp30B.f1=Cyc_set_output_dir;_tmp30B;});_tmp30A;});
_tmp308->f5=({const char*_tmp309="Set the output directory to <file>";_tag_arr(
_tmp309,sizeof(char),_get_zero_arr_size(_tmp309,35));});_tmp308;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp307,sizeof(struct _tuple17*),5));});
Cyc_Arg_parse(options,Cyc_add_spec_file,({const char*_tmp2E2="Options:";_tag_arr(
_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,9));}),argv);if(Cyc_badparse?1:
Cyc_spec_files == 0){Cyc_Arg_usage(options,({const char*_tmp2E3="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";
_tag_arr(_tmp2E3,sizeof(char),_get_zero_arr_size(_tmp2E3,59));}));return 1;}if(
Cyc_mode == (void*)2){Cyc_script_file=Cyc_Std_fopen(({const char*_tmp2E4="BUILDLIB.sh";
_tag_arr(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,12));}),({const char*
_tmp2E5="w";_tag_arr(_tmp2E5,sizeof(char),_get_zero_arr_size(_tmp2E5,2));}));if(
!((unsigned int)Cyc_script_file)){({void*_tmp2E6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp2E7="Could not create file BUILDLIB.sh\n";_tag_arr(_tmp2E7,sizeof(
char),_get_zero_arr_size(_tmp2E7,35));}),_tag_arr(_tmp2E6,sizeof(void*),0));});
exit(1);}({void*_tmp2E8[0]={};Cyc_prscript(({const char*_tmp2E9="#!/bin/sh\n";
_tag_arr(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,11));}),_tag_arr(
_tmp2E8,sizeof(void*),0));});({void*_tmp2EA[0]={};Cyc_prscript(({const char*
_tmp2EB="GCC=\"gcc\"\n";_tag_arr(_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,
11));}),_tag_arr(_tmp2EA,sizeof(void*),0));});}if(Cyc_force_directory_prefixes(
Cyc_output_dir)?1: Cyc_force_directory(Cyc_output_dir)){({struct Cyc_Std_String_pa_struct
_tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2EC[
1]={& _tmp2EE};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2ED="Error: could not create directory %s\n";
_tag_arr(_tmp2ED,sizeof(char),_get_zero_arr_size(_tmp2ED,38));}),_tag_arr(
_tmp2EC,sizeof(void*),1));}});return 1;}if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct
_tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2EF[
1]={& _tmp2F1};Cyc_prscript(({const char*_tmp2F0="cd %s\n";_tag_arr(_tmp2F0,
sizeof(char),_get_zero_arr_size(_tmp2F0,7));}),_tag_arr(_tmp2EF,sizeof(void*),1));}});({
void*_tmp2F2[0]={};Cyc_prscript(({const char*_tmp2F3="echo | $GCC -E -dM - -o INITMACROS.h\n";
_tag_arr(_tmp2F3,sizeof(char),_get_zero_arr_size(_tmp2F3,38));}),_tag_arr(
_tmp2F2,sizeof(void*),0));});}if(!Cyc_gathering()){Cyc_log_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2F4="BUILDLIB.LOG";
_tag_arr(_tmp2F4,sizeof(char),_get_zero_arr_size(_tmp2F4,13));})),({const char*
_tmp2F5="w";_tag_arr(_tmp2F5,sizeof(char),_get_zero_arr_size(_tmp2F5,2));}));if(
!((unsigned int)Cyc_log_file)){({struct Cyc_Std_String_pa_struct _tmp2F8;_tmp2F8.tag=
0;_tmp2F8.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2F6[1]={& _tmp2F8};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp2F7="Error: could not create log file in directory %s\n";
_tag_arr(_tmp2F7,sizeof(char),_get_zero_arr_size(_tmp2F7,50));}),_tag_arr(
_tmp2F6,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=Cyc_Std_fopen((struct
_tagged_arr)Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2F9="cstubs.c";
_tag_arr(_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,9));})),({const char*
_tmp2FA="w";_tag_arr(_tmp2FA,sizeof(char),_get_zero_arr_size(_tmp2FA,2));}));if(
!((unsigned int)Cyc_cstubs_file)){({struct Cyc_Std_String_pa_struct _tmp2FD;
_tmp2FD.tag=0;_tmp2FD.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2FB[1]={&
_tmp2FD};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2FC="Error: could not create cstubs.c in directory %s\n";
_tag_arr(_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,50));}),_tag_arr(
_tmp2FB,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=Cyc_Std_fopen((struct
_tagged_arr)Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2FE="cycstubs.cyc";
_tag_arr(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,13));})),({const char*
_tmp2FF="w";_tag_arr(_tmp2FF,sizeof(char),_get_zero_arr_size(_tmp2FF,2));}));if(
!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_Std_String_pa_struct _tmp302;
_tmp302.tag=0;_tmp302.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp300[1]={&
_tmp302};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp301="Error: could not create cycstubs.c in directory %s\n";
_tag_arr(_tmp301,sizeof(char),_get_zero_arr_size(_tmp301,52));}),_tag_arr(
_tmp300,sizeof(void*),1));}});return 1;}({void*_tmp303[0]={};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),({const char*_tmp304="#include <core.h>\nusing Core;\n\n";
_tag_arr(_tmp304,sizeof(char),_get_zero_arr_size(_tmp304,32));}),_tag_arr(
_tmp303,sizeof(void*),0));});}for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct
Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(Cyc_process_specfile(*((
struct _tagged_arr*)((struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd)),
Cyc_output_dir)){({void*_tmp305[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp306="FATAL ERROR -- QUIT!\n";_tag_arr(_tmp306,sizeof(char),
_get_zero_arr_size(_tmp306,22));}),_tag_arr(_tmp305,sizeof(void*),0));});exit(1);}}
if(Cyc_mode == (void*)2)Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_script_file));
else{if(!Cyc_gathering()){Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(
Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file));}}
return 0;}}
