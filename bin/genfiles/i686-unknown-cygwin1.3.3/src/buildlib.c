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
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct
Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(
struct _tagged_arr,struct _tagged_arr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int
Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);int Cyc_fputc(
int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct _tagged_arr Cyc_Lexing_lexeme(
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
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned int Cyc_strlen(
struct _tagged_arr s);int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);
struct _tagged_arr Cyc_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_strdup(struct _tagged_arr src);struct _tagged_arr Cyc_substring(
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
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
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
struct{int quot;int rem;}Cyc_div_t;typedef struct{int quot;int rem;}Cyc_ldiv_t;int
system(const char*);struct Cyc_stat{short st_dev;unsigned int st_ino;int st_mode;
unsigned short st_nlink;unsigned short st_uid;unsigned short st_gid;short st_rdev;
int st_size;int st_atime;int st_spare1;int st_mtime;int st_spare2;int st_ctime;int
st_spare3;int st_blksize;int st_blocks;int st_spare4[2];};int mkdir(const char*
pathname,int mode);struct Cyc_flock{short l_type;short l_whence;int l_start;int l_len;
short l_pid;short l_xxx;};struct Cyc_Flock_struct{int tag;struct Cyc_flock*f1;};
struct Cyc_Long_struct{int tag;int f1;};int Cyc_open(const char*,int,struct
_tagged_arr);int chdir(const char*);int close(int);struct _tagged_arr Cyc_getcwd(
struct _tagged_arr buf,unsigned int size);int isspace(int);int toupper(int);void Cyc_Lex_lex_init();
extern char*Ccomp;struct Cyc___cycFILE*Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=
0;struct Cyc___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,struct
_tagged_arr ap){if(Cyc_log_file == 0){({void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp1="Internal error: log file is NULL\n";_tag_arr(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,34));}),_tag_arr(_tmp0,sizeof(void*),0));});exit(1);}{
int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;}}static struct _tagged_arr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _tagged_arr*sptr){Cyc_current_targets=({struct Cyc_Set_Set**_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr);_tmp3;});}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Set_Set*f2;};
struct _tuple10*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple11{struct _tagged_arr f1;struct _tagged_arr*
f2;};struct _tuple11*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct _tuple12{struct _tagged_arr f1;struct _tagged_arr f2;};struct _tuple13{
struct _tagged_arr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};struct
_tuple13*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_header=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*Cyc_current_symbols=
0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*Cyc_current_cycstubs=
0;struct Cyc_List_List*Cyc_current_prologue=0;struct Cyc_List_List*Cyc_current_epilogue=
0;struct Cyc_List_List*Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=
0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=
0;int Cyc_numdef=0;const int Cyc_lex_base[420]=(const int[420]){0,0,75,192,305,310,
311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,
166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 16,223,12,- 2,216,23,26,28,
24,28,58,56,65,52,70,370,1024,1139,386,93,83,90,112,96,106,115,124,115,126,113,
375,414,152,180,209,227,228,377,525,227,226,229,238,226,248,498,1253,1368,654,- 8,
238,235,254,239,239,239,259,508,1482,1597,- 11,684,- 12,241,270,510,1711,1826,- 9,
720,- 10,746,- 13,432,515,550,1903,1980,2057,2138,434,465,667,2213,269,269,279,277,
279,290,0,13,4,2294,5,660,2302,2367,822,49,467,6,2328,7,990,2390,2428,998,- 21,
1051,1056,297,314,278,281,289,281,300,313,301,305,311,321,330,314,- 19,323,371,
400,411,418,419,- 14,399,415,412,411,422,418,421,446,448,434,430,430,453,454,445,
443,436,448,444,461,440,465,469,457,478,469,469,- 18,477,471,500,510,518,500,503,
548,556,557,1,4,6,570,571,559,567,579,580,587,588,2,21,590,592,654,19,20,21,616,
580,578,595,603,605,23,660,676,623,624,679,686,690,637,639,698,699,700,648,649,
704,705,709,656,658,713,716,717,665,666,1144,- 20,1165,665,677,674,673,701,697,
699,703,730,728,742,1258,734,736,744,757,1260,1284,760,761,751,752,750,769,1375,
770,771,769,782,1484,- 7,- 8,8,1149,2460,9,1227,2484,2522,1450,1377,- 49,1168,- 2,
821,- 4,865,997,1369,866,993,1023,1563,867,2549,2592,871,908,870,911,2662,913,943,
- 36,- 42,- 37,2737,- 28,940,- 40,- 25,941,- 27,- 45,- 39,- 48,2812,2841,1583,970,1010,
1677,2851,2881,1696,1792,2914,2945,2983,1081,1091,3053,3091,1084,1127,1148,1207,
1227,1237,- 6,- 34,957,3023,- 47,- 30,- 32,- 46,- 29,- 31,- 33,967,3131,1018,1020,1811,
1023,1024,1031,1032,1043,1076,1077,1078,1081,3204,3288,- 23,- 17,- 15,- 22,2357,1112,
- 24,- 41,- 38,- 35,- 26,1380,3370,4,3453,1115,15,1061,1062,1062,1063,1062,1072,1150};
const int Cyc_lex_backtrk[420]=(const int[420]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,
- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,14,2,4,4,- 1,
0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,20,20,20,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,
48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,-
1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,
2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[420]=(const int[420]){- 1,- 1,-
1,311,300,154,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,-
1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,380,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3710]=(const int[3710]){0,0,0,0,0,0,0,0,0,0,22,19,28,412,19,
28,19,28,95,19,45,45,45,45,45,22,45,0,0,0,0,0,21,218,229,413,21,22,- 1,- 1,22,- 1,-
1,45,219,45,220,22,410,410,410,410,410,410,410,410,410,410,31,119,22,230,130,40,
243,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,31,260,255,250,410,146,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,408,
408,408,408,408,408,408,408,408,408,137,20,85,78,65,55,56,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,57,58,59,60,408,61,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,66,67,37,312,313,312,312,313,39,
22,68,69,70,71,72,145,34,34,34,34,34,34,34,34,73,74,312,314,315,75,76,316,317,
318,120,120,319,320,120,321,322,323,324,325,325,325,325,325,325,325,325,325,326,
79,327,328,329,120,19,330,330,330,330,330,330,330,330,330,330,330,330,330,330,
330,330,330,330,330,330,330,330,330,330,330,330,19,- 1,- 1,331,330,80,330,330,330,
330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,330,
330,330,330,301,332,37,302,155,155,24,24,155,147,138,81,82,96,83,38,97,86,28,87,
24,29,98,25,303,88,89,155,90,22,26,26,21,21,91,117,109,156,99,100,101,102,103,
110,26,35,35,35,35,35,35,35,35,111,131,132,30,30,30,30,30,30,30,30,62,62,133,134,
62,77,77,84,84,77,135,84,136,158,207,208,64,64,304,193,64,188,179,62,148,139,157,
39,77,22,84,172,159,160,161,165,162,166,27,64,163,31,167,21,77,77,168,169,77,164,
170,171,173,129,129,129,129,129,129,129,129,129,129,- 1,32,- 1,- 1,77,- 1,22,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,- 1,174,- 1,- 1,129,- 1,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,92,92,35,175,92,176,
177,178,180,181,104,104,112,112,104,182,112,120,120,183,184,120,185,92,186,121,
121,84,84,121,28,84,21,104,187,112,95,189,190,191,120,192,46,194,195,196,197,198,
121,199,84,200,128,128,114,201,128,202,203,21,21,21,122,123,122,122,122,122,122,
122,122,122,122,122,21,128,204,205,206,265,209,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,210,211,
212,213,122,214,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,47,47,19,215,47,216,217,33,33,33,33,33,
33,33,33,33,33,94,94,221,222,94,223,47,33,33,33,33,33,33,128,128,224,225,128,226,
227,228,48,231,94,232,- 1,245,- 1,238,239,107,107,240,49,107,241,128,242,244,33,33,
33,33,33,33,144,144,144,144,144,144,144,144,107,118,246,247,248,- 1,233,- 1,41,41,
234,249,41,115,115,116,251,115,252,235,50,236,51,253,254,106,52,256,257,258,259,
41,53,54,114,261,115,262,263,117,117,264,108,117,266,267,282,42,42,42,42,42,42,
42,42,42,42,237,277,273,274,95,117,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,275,108,276,36,42,278,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,279,280,43,281,283,116,
284,33,33,33,33,33,33,33,33,33,33,285,286,295,290,291,292,293,33,33,33,33,33,33,
294,118,35,35,35,35,35,35,35,35,296,297,298,299,406,44,44,44,44,44,44,44,44,44,
44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,405,379,370,345,44,340,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,341,342,
343,28,337,338,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,339,344,373,333,44,402,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,403,377,62,153,153,153,153,153,153,
153,153,35,35,35,35,35,35,35,35,378,95,62,268,404,171,287,287,154,206,287,269,
269,334,374,269,106,108,63,63,63,63,63,63,63,63,63,63,118,287,375,376,95,95,269,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
178,398,46,95,63,397,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,31,64,64,288,399,64,267,267,270,21,267,310,310,289,414,310,415,
416,271,35,35,417,31,272,64,418,419,269,269,267,407,269,407,407,310,21,21,268,0,
0,63,63,63,63,63,63,63,63,63,63,269,35,35,407,31,0,31,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,36,63,31,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,92,92,270,36,92,
281,281,286,286,281,0,286,271,309,309,309,309,309,309,309,309,36,0,92,0,0,0,0,
281,0,286,287,287,0,0,287,19,0,31,93,93,93,93,93,93,93,93,93,93,0,369,369,0,36,
287,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,369,369,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,36,94,94,- 1,0,94,0,288,294,294,310,310,294,407,310,407,407,289,
0,0,0,0,0,0,94,0,0,0,0,0,0,294,- 1,310,0,21,407,0,0,28,93,93,93,93,93,93,93,93,93,
93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,0,0,381,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,104,104,299,299,104,0,299,31,31,31,31,31,31,31,31,0,0,
0,0,0,0,0,0,104,0,299,0,0,0,0,0,0,0,35,0,0,0,0,0,105,105,105,105,105,105,105,105,
105,105,0,0,0,0,0,0,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,105,0,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,106,107,107,0,371,107,372,372,372,372,372,372,372,372,372,372,0,0,0,0,- 1,
351,0,351,107,0,352,352,352,352,352,352,352,352,352,352,0,0,0,0,105,105,105,105,
105,105,105,105,105,105,0,0,0,0,0,0,0,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,105,0,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,112,112,0,0,112,352,352,352,352,352,352,352,352,352,352,
0,0,0,0,355,0,355,0,112,356,356,356,356,356,356,356,356,356,356,0,0,0,0,0,113,
113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,
0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,114,115,115,0,0,115,356,356,356,356,356,356,
356,356,356,356,399,0,0,0,0,0,0,0,115,400,400,400,400,400,400,400,400,0,0,0,0,0,
0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,124,124,124,124,124,124,124,124,124,
124,124,124,22,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,125,124,124,124,124,124,124,124,124,124,124,22,0,0,0,0,0,0,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,0,0,0,0,124,0,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,22,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,- 1,0,0,- 1,
124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,125,125,125,125,125,125,125,
125,125,127,0,0,0,0,0,0,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,125,0,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,129,129,129,129,129,129,129,129,129,129,0,0,0,0,0,0,0,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,0,0,28,0,0,149,141,141,141,141,
141,141,141,141,143,143,143,143,143,143,143,143,143,143,0,0,0,0,0,0,0,143,143,
143,143,143,143,0,0,0,150,150,150,150,150,150,150,150,0,0,0,0,0,0,0,31,0,0,- 1,0,
399,0,0,143,143,143,143,143,143,401,401,401,401,401,401,401,401,0,142,143,143,
143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,143,143,143,143,152,152,
152,152,152,152,152,152,152,152,151,0,0,0,0,0,0,152,152,152,152,152,152,0,0,0,
143,143,143,143,143,143,19,0,0,305,0,0,152,152,152,152,152,152,152,152,152,152,0,
152,152,152,152,152,152,152,152,152,152,152,152,0,0,0,0,0,0,0,0,0,306,306,306,
306,306,306,306,306,0,0,0,0,0,0,0,0,0,152,152,152,152,152,152,0,308,308,308,308,
308,308,308,308,308,308,0,0,0,0,0,0,0,308,308,308,308,308,308,0,0,28,0,0,0,0,0,0,
0,0,0,0,0,0,308,308,308,308,308,308,308,308,308,308,307,308,308,308,308,308,308,
308,308,308,308,308,308,0,0,346,0,357,357,357,357,357,357,357,357,358,358,0,0,0,
0,0,0,0,0,0,0,0,348,308,308,308,308,308,308,359,0,0,0,0,0,0,0,0,360,0,0,361,346,
0,347,347,347,347,347,347,347,347,347,347,348,0,0,0,0,0,0,359,0,0,0,348,0,0,0,0,
360,0,349,361,0,0,0,0,0,0,0,350,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,348,0,0,0,0,0,0,
349,0,0,0,0,0,0,0,0,350,336,336,336,336,336,336,336,336,336,336,0,0,0,0,0,0,0,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,0,0,0,0,336,0,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,0,0,0,0,0,0,0,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,0,0,0,0,336,0,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,353,353,353,353,353,353,353,353,353,353,0,0,0,0,0,0,0,0,
0,0,0,354,116,0,0,0,0,346,116,347,347,347,347,347,347,347,347,347,347,352,352,
352,352,352,352,352,352,352,352,0,348,0,0,354,116,0,0,349,0,0,116,114,0,0,0,0,
350,114,0,353,353,353,353,353,353,353,353,353,353,0,0,0,348,0,0,0,0,0,0,349,354,
116,0,114,0,0,0,116,350,114,0,0,356,356,356,356,356,356,356,356,356,356,0,0,0,0,
0,0,0,0,0,0,354,116,116,0,0,0,0,116,116,346,0,357,357,357,357,357,357,357,357,
358,358,0,0,0,0,0,0,0,0,0,0,0,348,0,116,0,0,0,0,367,116,0,0,0,0,0,0,346,368,358,
358,358,358,358,358,358,358,358,358,0,0,0,0,0,348,0,0,0,0,0,348,367,0,0,0,0,0,
365,0,0,368,0,0,0,0,0,366,0,0,372,372,372,372,372,372,372,372,372,372,0,0,0,348,
0,0,0,0,0,0,365,354,116,0,0,0,0,0,116,366,362,362,362,362,362,362,362,362,362,
362,0,0,0,0,0,0,0,362,362,362,362,362,362,354,116,0,0,0,0,0,116,0,0,0,0,0,0,0,
362,362,362,362,362,362,362,362,362,362,0,362,362,362,362,362,362,362,362,362,
362,362,362,0,0,0,382,0,363,0,0,383,0,0,0,0,0,364,0,0,384,384,384,384,384,384,
384,384,0,362,362,362,362,362,362,385,0,0,0,0,363,0,0,0,0,0,0,0,0,364,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,386,0,0,0,0,387,388,0,0,0,389,0,0,0,0,0,0,0,390,0,0,0,391,0,
392,0,393,0,394,395,395,395,395,395,395,395,395,395,395,0,0,0,0,0,0,0,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,0,0,0,0,0,0,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,396,0,0,0,0,0,0,0,0,395,395,
395,395,395,395,395,395,395,395,0,0,0,0,0,0,0,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,0,0,0,0,
0,0,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,395,
395,395,395,395,395,395,395,22,0,0,409,0,0,0,408,408,408,408,408,408,408,408,408,
408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,408,0,408,408,408,408,408,
408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,
408,411,0,0,0,0,0,0,0,410,410,410,410,410,410,410,410,410,410,0,0,0,0,0,0,0,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,0,0,0,0,410,0,410,410,410,410,410,410,410,410,410,410,410,
410,410,410,410,410,410,410,410,410,410,410,410,410,410,410,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3710]=(const int[3710]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,138,140,147,149,302,305,41,41,45,45,41,412,45,- 1,- 1,- 1,- 1,- 1,136,
217,228,0,409,10,12,40,10,12,40,41,218,45,219,20,1,1,1,1,1,1,1,1,1,1,38,48,137,
229,10,38,242,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,145,233,234,
235,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,7,3,3,3,3,3,
7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,16,4,5,5,6,8,5,6,8,80,81,50,82,16,50,85,27,86,17,27,50,17,4,87,88,5,89,5,
6,8,9,18,90,96,97,5,98,99,100,101,102,109,17,34,34,34,34,34,34,34,34,110,130,131,
27,27,27,27,27,27,27,27,61,61,132,133,61,76,76,83,83,76,134,83,135,157,159,159,
64,64,4,160,64,161,162,61,6,8,5,16,76,16,83,163,158,158,158,164,158,165,17,64,
158,27,166,7,77,77,167,168,77,158,169,170,172,11,11,11,11,11,11,11,11,11,11,119,
27,126,119,77,126,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,127,173,146,127,11,146,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,64,174,91,175,176,177,179,180,103,
103,111,111,103,181,111,120,120,182,183,120,184,91,185,13,13,84,84,13,77,84,16,
103,186,111,187,188,189,190,120,191,192,193,194,195,196,197,13,198,84,199,121,
121,4,200,121,201,202,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,121,203,204,
205,207,208,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,209,210,211,212,13,213,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,14,14,84,214,14,215,216,32,32,32,32,32,32,32,32,32,
32,94,94,220,221,94,222,14,32,32,32,32,32,32,128,128,223,224,128,225,226,227,14,
230,94,231,119,236,126,237,238,107,107,239,14,107,240,128,241,243,32,32,32,32,32,
32,141,141,141,141,141,141,141,141,107,244,245,246,247,127,232,146,15,15,232,248,
15,115,115,249,250,115,251,232,14,232,14,252,253,254,14,255,256,257,258,15,14,14,
259,260,115,261,262,117,117,263,264,117,265,266,270,15,15,15,15,15,15,15,15,15,
15,232,271,272,273,94,117,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,274,107,275,276,15,277,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,278,279,15,280,282,115,283,33,33,
33,33,33,33,33,33,33,33,284,285,288,289,290,291,292,33,33,33,33,33,33,293,117,
144,144,144,144,144,144,144,144,295,296,297,298,314,42,42,42,42,42,42,42,42,42,
42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,316,319,323,326,42,328,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,327,327,
327,15,329,329,331,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,338,341,371,332,44,380,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,317,320,62,150,150,150,150,150,150,
150,150,153,153,153,153,153,153,153,153,320,349,62,382,317,383,155,155,385,386,
155,156,156,332,321,156,387,388,62,62,62,62,62,62,62,62,62,62,389,155,321,321,
350,349,156,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,390,391,392,350,62,393,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,63,63,155,401,63,267,267,156,411,267,303,303,
155,413,303,414,415,156,359,360,416,363,156,63,417,418,269,269,267,312,269,312,
312,303,419,303,267,- 1,- 1,63,63,63,63,63,63,63,63,63,63,269,359,360,312,363,- 1,
364,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,- 1,- 1,- 1,365,63,364,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,92,92,269,365,92,281,281,286,286,281,- 1,286,269,306,306,306,
306,306,306,306,306,366,- 1,92,- 1,- 1,- 1,- 1,281,- 1,286,287,287,- 1,- 1,287,281,- 1,
286,92,92,92,92,92,92,92,92,92,92,- 1,367,368,- 1,366,287,- 1,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,367,368,- 1,- 1,92,- 1,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,
93,318,- 1,93,- 1,287,294,294,310,310,294,407,310,407,407,287,- 1,- 1,- 1,- 1,- 1,- 1,93,
- 1,- 1,- 1,- 1,- 1,- 1,294,318,310,- 1,310,407,- 1,- 1,294,93,93,93,93,93,93,93,93,93,93,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,- 1,- 1,318,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,104,104,299,299,104,- 1,299,309,309,309,309,
309,309,309,309,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,299,- 1,- 1,- 1,- 1,- 1,- 1,- 1,299,- 1,-
1,- 1,- 1,- 1,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,- 1,322,
105,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,318,348,- 1,348,105,- 1,
348,348,348,348,348,348,348,348,348,348,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,
105,105,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,112,112,- 1,- 1,112,351,351,351,351,351,351,351,351,351,351,- 1,
- 1,- 1,- 1,354,- 1,354,- 1,112,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,-
1,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,113,113,- 1,- 1,113,355,
355,355,355,355,355,355,355,355,355,384,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,384,384,384,384,
384,384,384,384,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,- 1,- 1,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,
123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,124,
124,124,124,124,124,- 1,- 1,- 1,- 1,- 1,- 1,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,125,- 1,- 1,125,
124,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,
125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,125,- 1,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,-
1,- 1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,139,- 1,- 1,139,129,- 1,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,- 1,- 1,
148,- 1,- 1,148,139,139,139,139,139,139,139,139,142,142,142,142,142,142,142,142,
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
Cyc_substring((struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpC;});Cyc_current_args=0;Cyc_current_targets=({
struct Cyc_Set_Set**_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);
_tmpD;});Cyc_token(lexbuf);return 0;case 1: _LL6: Cyc_current_source=({struct
_tagged_arr*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=(struct _tagged_arr)Cyc_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpE;});Cyc_current_args=0;Cyc_current_targets=({
struct Cyc_Set_Set**_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);
_tmpF;});Cyc_args(lexbuf);return 0;case 2: _LL7: Cyc_current_source=({struct
_tagged_arr*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp10;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp11;});
Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);return Cyc_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp12=
_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Lexing_Error_struct _tmp13;
_tmp13.tag=Cyc_Lexing_Error;_tmp13.f1=({const char*_tmp14="some action didn't return!";
_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,27));});_tmp13;});_tmp12;}));}
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,
1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LLA: {struct _tagged_arr*_tmp15=({struct
_tagged_arr*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=(struct _tagged_arr)Cyc_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp17;});Cyc_current_args=({
struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=_tmp15;_tmp16->tl=
Cyc_current_args;_tmp16;});return Cyc_args(lexbuf);}case 1: _LLB: {struct
_tagged_arr*_tmp18=({struct _tagged_arr*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[
0]=(struct _tagged_arr)Cyc_substring((struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),
0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))
- 1));_tmp1A;});Cyc_current_args=({struct Cyc_List_List*_tmp19=_cycalloc(sizeof(*
_tmp19));_tmp19->hd=_tmp18;_tmp19->tl=Cyc_current_args;_tmp19;});return Cyc_token(
lexbuf);}default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp1B=_cycalloc(
sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Lexing_Error_struct _tmp1C;_tmp1C.tag=Cyc_Lexing_Error;
_tmp1C.f1=({const char*_tmp1D="some action didn't return!";_tag_arr(_tmp1D,
sizeof(char),_get_zero_arr_size(_tmp1D,27));});_tmp1C;});_tmp1B;}));}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(
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
lexstate,lexbuf);switch(lexstate){case 0: _LL4D: return 0;case 1: _LL4E: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL4F: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp25[0]={};Cyc_log(({
const char*_tmp26="Warning: declaration of malloc sidestepped\n";_tag_arr(_tmp26,
sizeof(char),_get_zero_arr_size(_tmp26,44));}),_tag_arr(_tmp25,sizeof(void*),0));});
return 1;case 3: _LL50: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp27[0]={};Cyc_log(({
const char*_tmp28="Warning: declaration of malloc sidestepped\n";_tag_arr(_tmp28,
sizeof(char),_get_zero_arr_size(_tmp28,44));}),_tag_arr(_tmp27,sizeof(void*),0));});
return 1;case 4: _LL51: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp29[0]={};Cyc_log(({
const char*_tmp2A="Warning: declaration of calloc sidestepped\n";_tag_arr(_tmp2A,
sizeof(char),_get_zero_arr_size(_tmp2A,44));}),_tag_arr(_tmp29,sizeof(void*),0));});
return 1;case 5: _LL52: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp2B[0]={};Cyc_log(({
const char*_tmp2C="Warning: declaration of calloc sidestepped\n";_tag_arr(_tmp2C,
sizeof(char),_get_zero_arr_size(_tmp2C,44));}),_tag_arr(_tmp2B,sizeof(void*),0));});
return 1;case 6: _LL53: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp2D[0]={};Cyc_log(({const char*_tmp2E="Warning: use of region sidestepped\n";
_tag_arr(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,36));}),_tag_arr(_tmp2D,
sizeof(void*),0));});return 1;case 7: _LL54: return 1;case 8: _LL55: return 1;case 9: _LL56:
return 1;case 10: _LL57: return 1;case 11: _LL58: return 1;case 12: _LL59: return 1;case 13:
_LL5A: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 14: _LL5B: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5C: Cyc_fputs((const char*)"const",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5D: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 17: _LL5E: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL5F: return 1;case 19: _LL60: Cyc_parens_to_match=1;while(Cyc_asm(
lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp2F[0]={};Cyc_log(({const char*_tmp30="Warning: replacing use of __asm__ with 0\n";
_tag_arr(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,42));}),_tag_arr(_tmp2F,
sizeof(void*),0));});return 1;case 20: _LL61: Cyc_fputc((int)Cyc_Lexing_lexeme_char(
lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default:
_LL62:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31[0]=({struct Cyc_Lexing_Error_struct _tmp32;_tmp32.tag=Cyc_Lexing_Error;
_tmp32.f1=({const char*_tmp33="some action didn't return!";_tag_arr(_tmp33,
sizeof(char),_get_zero_arr_size(_tmp33,27));});_tmp32;});_tmp31;}));}int Cyc_slurp(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL64: return 0;case 1: _LL65: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL66:({void*
_tmp34[0]={};Cyc_log(({const char*_tmp35="Warning: unclosed string\n";_tag_arr(
_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,26));}),_tag_arr(_tmp34,sizeof(
void*),0));});({struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp36[1]={& _tmp38};Cyc_fprintf((
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp37="%s";_tag_arr(
_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,3));}),_tag_arr(_tmp36,sizeof(void*),
1));}});return 1;case 3: _LL67:({struct Cyc_String_pa_struct _tmp3B;_tmp3B.tag=0;
_tmp3B.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp39[1]={& _tmp3B};
Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp3A="%s";
_tag_arr(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,3));}),_tag_arr(_tmp39,
sizeof(void*),1));}});return 1;case 4: _LL68:({struct Cyc_String_pa_struct _tmp3E;
_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp3C[
1]={& _tmp3E};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp3D="%s";_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,3));}),
_tag_arr(_tmp3C,sizeof(void*),1));}});return 1;case 5: _LL69:({struct Cyc_String_pa_struct
_tmp41;_tmp41.tag=0;_tmp41.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp3F[1]={& _tmp41};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp40="%s";_tag_arr(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,3));}),
_tag_arr(_tmp3F,sizeof(void*),1));}});return 1;case 6: _LL6A:({struct Cyc_String_pa_struct
_tmp44;_tmp44.tag=0;_tmp44.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp42[1]={& _tmp44};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp43="%s";_tag_arr(_tmp43,sizeof(char),_get_zero_arr_size(_tmp43,3));}),
_tag_arr(_tmp42,sizeof(void*),1));}});return 1;case 7: _LL6B:({struct Cyc_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp45[1]={& _tmp47};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp46="%s";_tag_arr(_tmp46,sizeof(char),_get_zero_arr_size(_tmp46,3));}),
_tag_arr(_tmp45,sizeof(void*),1));}});return 1;case 8: _LL6C:({struct Cyc_String_pa_struct
_tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp48[1]={& _tmp4A};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp49="%s";_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,3));}),
_tag_arr(_tmp48,sizeof(void*),1));}});return 1;default: _LL6D:(lexbuf->refill_buff)(
lexbuf);return Cyc_slurp_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Lexing_Error_struct
_tmp4C;_tmp4C.tag=Cyc_Lexing_Error;_tmp4C.f1=({const char*_tmp4D="some action didn't return!";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,27));});_tmp4C;});_tmp4B;}));}
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70:
if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match --;return 1;case 2: _LL71: Cyc_parens_to_match
++;return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73:
while(Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp4E=_cycalloc(sizeof(*
_tmp4E));_tmp4E[0]=({struct Cyc_Lexing_Error_struct _tmp4F;_tmp4F.tag=Cyc_Lexing_Error;
_tmp4F.f1=({const char*_tmp50="some action didn't return!";_tag_arr(_tmp50,
sizeof(char),_get_zero_arr_size(_tmp50,27));});_tmp4F;});_tmp4E;}));}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL78:({void*_tmp51[0]={};Cyc_log(({const char*
_tmp52="Warning: unclosed string\n";_tag_arr(_tmp52,sizeof(char),
_get_zero_arr_size(_tmp52,26));}),_tag_arr(_tmp51,sizeof(void*),0));});return 0;
case 1: _LL79: return 0;case 2: _LL7A:({void*_tmp53[0]={};Cyc_log(({const char*_tmp54="Warning: unclosed string\n";
_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,26));}),_tag_arr(_tmp53,
sizeof(void*),0));});return 1;case 3: _LL7B: return 1;case 4: _LL7C: return 1;case 5: _LL7D:
return 1;case 6: _LL7E: return 1;case 7: _LL7F: return 1;case 8: _LL80: return 1;default:
_LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp55=_cycalloc(sizeof(*
_tmp55));_tmp55[0]=({struct Cyc_Lexing_Error_struct _tmp56;_tmp56.tag=Cyc_Lexing_Error;
_tmp56.f1=({const char*_tmp57="some action didn't return!";_tag_arr(_tmp57,
sizeof(char),_get_zero_arr_size(_tmp57,27));});_tmp56;});_tmp55;}));}int Cyc_asm_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL83:({void*_tmp58[0]={};Cyc_log(({const char*
_tmp59="Warning: unclosed comment\n";_tag_arr(_tmp59,sizeof(char),
_get_zero_arr_size(_tmp59,27));}),_tag_arr(_tmp58,sizeof(void*),0));});return 0;
case 1: _LL84: return 0;case 2: _LL85: return 1;default: _LL86:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_Lexing_Error_struct
_tmp5B;_tmp5B.tag=Cyc_Lexing_Error;_tmp5B.f1=({const char*_tmp5C="some action didn't return!";
_tag_arr(_tmp5C,sizeof(char),_get_zero_arr_size(_tmp5C,27));});_tmp5B;});_tmp5A;}));}
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple11*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL88:
Cyc_current_line=({const char*_tmp5D="#define ";_tag_arr(_tmp5D,sizeof(char),
_get_zero_arr_size(_tmp5D,9));});Cyc_suck_macroname(lexbuf);return({struct
_tuple11*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=Cyc_current_line;_tmp5E->f2=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp5E;});case 1: _LL89: return
Cyc_suck_line(lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Lexing_Error_struct
_tmp60;_tmp60.tag=Cyc_Lexing_Error;_tmp60.f1=({const char*_tmp61="some action didn't return!";
_tag_arr(_tmp61,sizeof(char),_get_zero_arr_size(_tmp61,27));});_tmp60;});_tmp5F;}));}
struct _tuple11*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8D: Cyc_current_source=({
struct _tagged_arr*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);_tmp62;});Cyc_current_line=(struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_current_line,(struct _tagged_arr)*((struct _tagged_arr*)
_check_null(Cyc_current_source)));return Cyc_suck_restofline(lexbuf);default:
_LL8E:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp63=_cycalloc(sizeof(*
_tmp63));_tmp63[0]=({struct Cyc_Lexing_Error_struct _tmp64;_tmp64.tag=Cyc_Lexing_Error;
_tmp64.f1=({const char*_tmp65="some action didn't return!";_tag_arr(_tmp65,
sizeof(char),_get_zero_arr_size(_tmp65,27));});_tmp64;});_tmp63;}));}int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL90: Cyc_current_line=(struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_current_line,(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));
return 0;default: _LL91:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp66=
_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Lexing_Error_struct _tmp67;
_tmp67.tag=Cyc_Lexing_Error;_tmp67.f1=({const char*_tmp68="some action didn't return!";
_tag_arr(_tmp68,sizeof(char),_get_zero_arr_size(_tmp68,27));});_tmp67;});_tmp66;}));}
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple13*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL93:
return Cyc_spec(lexbuf);case 1: _LL94: Cyc_current_header=(struct _tagged_arr)Cyc_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=
0;while(Cyc_commands(lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);return({struct _tuple13*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->f1=Cyc_current_header;_tmp69->f2=Cyc_current_symbols;
_tmp69->f3=Cyc_current_omit_symbols;_tmp69->f4=Cyc_current_prologue;_tmp69->f5=
Cyc_current_epilogue;_tmp69->f6=Cyc_current_cstubs;_tmp69->f7=Cyc_current_cycstubs;
_tmp69;});case 2: _LL95: return Cyc_spec(lexbuf);case 3: _LL96: return 0;case 4: _LL97:({
struct Cyc_Int_pa_struct _tmp6C;_tmp6C.tag=1;_tmp6C.f1=(int)((unsigned int)((int)
Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp6A[1]={& _tmp6C};Cyc_fprintf(Cyc_stderr,({
const char*_tmp6B="Error in .cys file: expected header file name, found '%c' instead\n";
_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,67));}),_tag_arr(_tmp6A,
sizeof(void*),1));}});return 0;default: _LL98:(lexbuf->refill_buff)(lexbuf);return
Cyc_spec_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Lexing_Error_struct
_tmp6E;_tmp6E.tag=Cyc_Lexing_Error;_tmp6E.f1=({const char*_tmp6F="some action didn't return!";
_tag_arr(_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,27));});_tmp6E;});_tmp6D;}));}
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
struct _tuple12*x=({struct _tuple12*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp71->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp71;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->hd=x;_tmp70->tl=
Cyc_current_prologue;_tmp70;});return 1;}case 5: _LL9F: {struct _tagged_arr _tmp72=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp72,sizeof(char),(int)Cyc_strlen(({
const char*_tmp73="prologue";_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(
_tmp73,9));})));while(isspace((int)*((char*)_check_unknown_subscript(_tmp72,
sizeof(char),0)))){_tagged_arr_inplace_plus_post(& _tmp72,sizeof(char),1);}{
struct _tagged_arr t=_tmp72;while(!isspace((int)*((char*)_check_unknown_subscript(
t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct
_tagged_arr _tmp74=Cyc_substring((struct _tagged_arr)_tmp72,0,(unsigned int)((t.curr
- _tmp72.curr)/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct
_tuple12*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=(struct _tagged_arr)_tmp74;
_tmp76->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp76;});Cyc_current_prologue=({struct Cyc_List_List*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=x;_tmp75->tl=Cyc_current_prologue;
_tmp75;});return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct
_tuple12*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp78->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp78;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=x;_tmp77->tl=Cyc_current_epilogue;
_tmp77;});return 1;}case 7: _LLA1: {struct _tagged_arr _tmp79=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp79,sizeof(char),(int)Cyc_strlen(({const
char*_tmp7A="epilogue";_tag_arr(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,9));})));
while(isspace((int)*((char*)_check_unknown_subscript(_tmp79,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& _tmp79,sizeof(char),1);}{struct _tagged_arr t=
_tmp79;while(!isspace((int)*((char*)_check_unknown_subscript(t,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct _tagged_arr _tmp7B=Cyc_substring((
struct _tagged_arr)_tmp79,0,(unsigned int)((t.curr - _tmp79.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp7D=_cycalloc(
sizeof(*_tmp7D));_tmp7D->f1=(struct _tagged_arr)_tmp7B;_tmp7D->f2=(struct
_tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp7D;});Cyc_current_epilogue=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*
_tmp7C));_tmp7C->hd=x;_tmp7C->tl=Cyc_current_epilogue;_tmp7C;});return 1;}}}}case
8: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp7F=
_cycalloc(sizeof(*_tmp7F));_tmp7F->f1=(struct _tagged_arr)_tag_arr(0,0,0);_tmp7F->f2=(
struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp7F;});Cyc_current_cstubs=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*
_tmp7E));_tmp7E->hd=x;_tmp7E->tl=Cyc_current_cstubs;_tmp7E;});return 1;}case 9:
_LLA3: {struct _tagged_arr _tmp80=Cyc_Lexing_lexeme(lexbuf);
_tagged_arr_inplace_plus(& _tmp80,sizeof(char),(int)Cyc_strlen(({const char*_tmp81="cstub";
_tag_arr(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,6));})));while(isspace((
int)*((char*)_check_unknown_subscript(_tmp80,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& _tmp80,sizeof(char),1);}{struct _tagged_arr t=
_tmp80;while(!isspace((int)*((char*)_check_unknown_subscript(t,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct _tagged_arr _tmp82=Cyc_substring((
struct _tagged_arr)_tmp80,0,(unsigned int)((t.curr - _tmp80.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp84=_cycalloc(
sizeof(*_tmp84));_tmp84->f1=(struct _tagged_arr)_tmp82;_tmp84->f2=(struct
_tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp84;});Cyc_current_cstubs=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83->hd=x;_tmp83->tl=Cyc_current_cstubs;_tmp83;});return 1;}}}}case 10:
_LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp86=
_cycalloc(sizeof(*_tmp86));_tmp86->f1=(struct _tagged_arr)_tag_arr(0,0,0);_tmp86->f2=(
struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp86;});Cyc_current_cycstubs=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*
_tmp85));_tmp85->hd=x;_tmp85->tl=Cyc_current_cycstubs;_tmp85;});return 1;}case 11:
_LLA5: {struct _tagged_arr _tmp87=Cyc_Lexing_lexeme(lexbuf);
_tagged_arr_inplace_plus(& _tmp87,sizeof(char),(int)Cyc_strlen(({const char*_tmp88="cycstub";
_tag_arr(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,8));})));while(isspace((
int)*((char*)_check_unknown_subscript(_tmp87,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& _tmp87,sizeof(char),1);}{struct _tagged_arr t=
_tmp87;while(!isspace((int)*((char*)_check_unknown_subscript(t,sizeof(char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);}{struct _tagged_arr _tmp89=Cyc_substring((
struct _tagged_arr)_tmp87,0,(unsigned int)((t.curr - _tmp87.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple12*x=({struct _tuple12*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B->f1=(struct _tagged_arr)_tmp89;_tmp8B->f2=(struct
_tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp8B;});Cyc_current_cycstubs=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->hd=x;_tmp8A->tl=Cyc_current_cycstubs;_tmp8A;});return 1;}}}}case
12: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tagged_arr*x=({struct _tagged_arr*_tmp8D=
_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp8D;});Cyc_current_cpp=({struct
Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=x;_tmp8C->tl=Cyc_current_cpp;
_tmp8C;});return 1;}case 13: _LLA7: return 1;case 14: _LLA8: return 1;case 15: _LLA9:({
struct Cyc_Int_pa_struct _tmp90;_tmp90.tag=1;_tmp90.f1=(int)((unsigned int)((int)
Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp8E[1]={& _tmp90};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8F="Error in .cys file: expected command, found '%c' instead\n";
_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,58));}),_tag_arr(_tmp8E,
sizeof(void*),1));}});return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return
Cyc_commands_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Lexing_Error_struct
_tmp92;_tmp92.tag=Cyc_Lexing_Error;_tmp92.f1=({const char*_tmp93="some action didn't return!";
_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,27));});_tmp92;});_tmp91;}));}
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LLAC: Cyc_snarfed_symbols=({struct Cyc_List_List*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->hd=({struct _tagged_arr*_tmp95=
_cycalloc(sizeof(*_tmp95));_tmp95[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);
_tmp95;});_tmp94->tl=Cyc_snarfed_symbols;_tmp94;});return 1;case 1: _LLAD: return 1;
case 2: _LLAE: return 0;case 3: _LLAF:({void*_tmp96[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp97="Error in .cys file: unexpected end-of-file\n";_tag_arr(_tmp97,
sizeof(char),_get_zero_arr_size(_tmp97,44));}),_tag_arr(_tmp96,sizeof(void*),0));});
return 0;case 4: _LLB0:({struct Cyc_Int_pa_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=(int)((
unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp98[1]={& _tmp9A};
Cyc_fprintf(Cyc_stderr,({const char*_tmp99="Error in .cys file: expected symbol, found '%c' instead\n";
_tag_arr(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,57));}),_tag_arr(_tmp98,
sizeof(void*),1));}});return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return
Cyc_snarfsymbols_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Lexing_Error_struct
_tmp9C;_tmp9C.tag=Cyc_Lexing_Error;_tmp9C.f1=({const char*_tmp9D="some action didn't return!";
_tag_arr(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,27));});_tmp9C;});_tmp9B;}));}
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB3:({void*
_tmp9E[0]={};Cyc_log(({const char*_tmp9F="Warning: unclosed brace\n";_tag_arr(
_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,25));}),_tag_arr(_tmp9E,sizeof(
void*),0));});return 0;case 1: _LLB4: if(Cyc_braces_to_match == 1)return 0;Cyc_braces_to_match
--;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return
1;case 2: _LLB5: Cyc_braces_to_match ++;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB6: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}return 1;case 4:
_LLB7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({
const char*_tmpA0="/*";_tag_arr(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,3));}));
while(Cyc_block_comment(lexbuf)){;}return 1;case 5: _LLB8: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLBA:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Lexing_Error_struct _tmpA2;_tmpA2.tag=Cyc_Lexing_Error;_tmpA2.f1=({
const char*_tmpA3="some action didn't return!";_tag_arr(_tmpA3,sizeof(char),
_get_zero_arr_size(_tmpA3,27));});_tmpA2;});_tmpA1;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBC:({void*_tmpA4[0]={};Cyc_log(({const char*_tmpA5="Warning: unclosed string\n";
_tag_arr(_tmpA5,sizeof(char),_get_zero_arr_size(_tmpA5,26));}),_tag_arr(_tmpA4,
sizeof(void*),0));});return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBE:({void*_tmpA6[0]={};Cyc_log(({
const char*_tmpA7="Warning: unclosed string\n";_tag_arr(_tmpA7,sizeof(char),
_get_zero_arr_size(_tmpA7,26));}),_tag_arr(_tmpA6,sizeof(void*),0));});Cyc_Buffer_add_string((
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
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpA8=
_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Lexing_Error_struct _tmpA9;
_tmpA9.tag=Cyc_Lexing_Error;_tmpA9.f1=({const char*_tmpAA="some action didn't return!";
_tag_arr(_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,27));});_tmpA9;});_tmpA8;}));}
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:({void*
_tmpAB[0]={};Cyc_log(({const char*_tmpAC="Warning: unclosed comment\n";_tag_arr(
_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,27));}),_tag_arr(_tmpAB,sizeof(
void*),0));});return 0;case 1: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),({const char*_tmpAD="*/";_tag_arr(_tmpAD,sizeof(char),
_get_zero_arr_size(_tmpAD,3));}));return 0;case 2: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Lexing_Error_struct _tmpAF;
_tmpAF.tag=Cyc_Lexing_Error;_tmpAF.f1=({const char*_tmpB0="some action didn't return!";
_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,27));});_tmpAF;});_tmpAE;}));}
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmpB1=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmpB2;struct
_tuple0 _tmpB3;struct _tagged_arr*_tmpB4;struct _tuple0*_tmpB5;struct _tuple0 _tmpB6;
struct _tagged_arr*_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;
struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;
struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;
struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;
struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;
struct Cyc_List_List*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_MallocInfo
_tmpCC;int _tmpCD;struct Cyc_Absyn_Exp*_tmpCE;void**_tmpCF;struct Cyc_Absyn_Exp*
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct _tagged_arr*_tmpD3;struct Cyc_Absyn_Exp*
_tmpD4;struct _tagged_arr*_tmpD5;void*_tmpD6;void*_tmpD7;struct Cyc_List_List*
_tmpD8;_LLCD: if(*((int*)_tmpB1)!= 1)goto _LLCF;_tmpB2=((struct Cyc_Absyn_Var_e_struct*)
_tmpB1)->f1;_tmpB3=*_tmpB2;_tmpB4=_tmpB3.f2;_LLCE: _tmpB7=_tmpB4;goto _LLD0;_LLCF:
if(*((int*)_tmpB1)!= 2)goto _LLD1;_tmpB5=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpB1)->f1;_tmpB6=*_tmpB5;_tmpB7=_tmpB6.f2;_LLD0: Cyc_add_target(_tmpB7);return;
_LLD1: if(*((int*)_tmpB1)!= 3)goto _LLD3;_tmpB8=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB1)->f2;_LLD2: for(0;_tmpB8 != 0;_tmpB8=_tmpB8->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpB8->hd));}return;_LLD3: if(*((int*)_tmpB1)!= 23)goto
_LLD5;_tmpB9=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB1)->f1;_tmpBA=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpB1)->f2;_LLD4: _tmpBB=_tmpB9;_tmpBC=_tmpBA;goto
_LLD6;_LLD5: if(*((int*)_tmpB1)!= 7)goto _LLD7;_tmpBB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB1)->f1;_tmpBC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB1)->f2;_LLD6: _tmpBD=
_tmpBB;_tmpBE=_tmpBC;goto _LLD8;_LLD7: if(*((int*)_tmpB1)!= 4)goto _LLD9;_tmpBD=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpB1)->f1;_tmpBE=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB1)->f3;_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBD);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpBE);return;_LLD9: if(*((int*)_tmpB1)!= 20)goto _LLDB;
_tmpBF=((struct Cyc_Absyn_Deref_e_struct*)_tmpB1)->f1;_LLDA: _tmpC0=_tmpBF;goto
_LLDC;_LLDB: if(*((int*)_tmpB1)!= 17)goto _LLDD;_tmpC0=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpB1)->f1;_LLDC: _tmpC1=_tmpC0;goto _LLDE;_LLDD: if(*((int*)_tmpB1)!= 14)goto
_LLDF;_tmpC1=((struct Cyc_Absyn_Address_e_struct*)_tmpB1)->f1;_LLDE: _tmpC2=_tmpC1;
goto _LLE0;_LLDF: if(*((int*)_tmpB1)!= 5)goto _LLE1;_tmpC2=((struct Cyc_Absyn_Increment_e_struct*)
_tmpB1)->f1;_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC2);return;_LLE1: if(*((
int*)_tmpB1)!= 6)goto _LLE3;_tmpC3=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB1)->f1;
_tmpC4=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB1)->f2;_tmpC5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB1)->f3;_LLE2: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC3);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC4);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC5);return;
_LLE3: if(*((int*)_tmpB1)!= 9)goto _LLE5;_tmpC6=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpB1)->f1;_tmpC7=((struct Cyc_Absyn_FnCall_e_struct*)_tmpB1)->f2;_LLE4: _tmpC8=
_tmpC6;_tmpC9=_tmpC7;goto _LLE6;_LLE5: if(*((int*)_tmpB1)!= 8)goto _LLE7;_tmpC8=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpB1)->f1;_tmpC9=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpB1)->f2;_LLE6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);for(0;_tmpC9 != 0;
_tmpC9=_tmpC9->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpC9->hd));}return;_LLE7: if(*((int*)_tmpB1)!= 13)goto _LLE9;_tmpCA=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB1)->f1;_tmpCB=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB1)->f2;_LLE8: Cyc_scan_type(_tmpCA);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpCB);return;_LLE9: if(*((int*)_tmpB1)!= 33)goto _LLEB;_tmpCC=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpB1)->f1;_tmpCD=_tmpCC.is_calloc;_tmpCE=_tmpCC.rgn;_tmpCF=_tmpCC.elt_type;
_tmpD0=_tmpCC.num_elts;_LLEA: if(_tmpCE != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpCE)));if(_tmpCF != 0)Cyc_scan_type(*((void**)
_tmpCF));Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD0);return;_LLEB: if(*((int*)
_tmpB1)!= 16)goto _LLED;_tmpD1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpB1)->f1;_LLEC: Cyc_scan_type(_tmpD1);return;_LLED: if(*((int*)_tmpB1)!= 21)
goto _LLEF;_tmpD2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB1)->f1;_tmpD3=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpB1)->f2;_LLEE: _tmpD4=_tmpD2;_tmpD5=
_tmpD3;goto _LLF0;_LLEF: if(*((int*)_tmpB1)!= 22)goto _LLF1;_tmpD4=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpB1)->f1;_tmpD5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpB1)->f2;_LLF0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD4);Cyc_add_target(_tmpD5);return;_LLF1: if(*((int*)
_tmpB1)!= 18)goto _LLF3;_tmpD6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB1)->f1;
_tmpD7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB1)->f2;_LLF2: Cyc_scan_type(
_tmpD6);{void*_tmpD9=_tmpD7;struct _tagged_arr*_tmpDA;_LL11A: if(*((int*)_tmpD9)!= 
0)goto _LL11C;_tmpDA=((struct Cyc_Absyn_StructField_struct*)_tmpD9)->f1;_LL11B: Cyc_add_target(
_tmpDA);goto _LL119;_LL11C: if(*((int*)_tmpD9)!= 1)goto _LL119;_LL11D: goto _LL119;
_LL119:;}return;_LLF3: if(*((int*)_tmpB1)!= 0)goto _LLF5;_LLF4: return;_LLF5: if(*((
int*)_tmpB1)!= 34)goto _LLF7;_tmpD8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB1)->f2;_LLF6: for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){struct _tuple14 _tmpDC;
struct Cyc_Absyn_Exp*_tmpDD;struct _tuple14*_tmpDB=(struct _tuple14*)_tmpD8->hd;
_tmpDC=*_tmpDB;_tmpDD=_tmpDC.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDD);}
return;_LLF7: if(*((int*)_tmpB1)!= 35)goto _LLF9;_LLF8:({void*_tmpDE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpDF="Error: unexpected Stmt_e\n";_tag_arr(_tmpDF,
sizeof(char),_get_zero_arr_size(_tmpDF,26));}),_tag_arr(_tmpDE,sizeof(void*),0));});
exit(1);return;_LLF9: if(*((int*)_tmpB1)!= 10)goto _LLFB;_LLFA:({void*_tmpE0[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmpE1="Error: unexpected Throw_e\n";_tag_arr(
_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,27));}),_tag_arr(_tmpE0,sizeof(
void*),0));});exit(1);return;_LLFB: if(*((int*)_tmpB1)!= 11)goto _LLFD;_LLFC:({
void*_tmpE2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpE3="Error: unexpected NoInstantiate_e\n";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,35));}),_tag_arr(_tmpE2,
sizeof(void*),0));});exit(1);return;_LLFD: if(*((int*)_tmpB1)!= 12)goto _LLFF;
_LLFE:({void*_tmpE4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpE5="Error: unexpected Instantiate_e\n";
_tag_arr(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,33));}),_tag_arr(_tmpE4,
sizeof(void*),0));});exit(1);return;_LLFF: if(*((int*)_tmpB1)!= 15)goto _LL101;
_LL100:({void*_tmpE6[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpE7="Error: unexpected New_e\n";
_tag_arr(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,25));}),_tag_arr(_tmpE6,
sizeof(void*),0));});exit(1);return;_LL101: if(*((int*)_tmpB1)!= 19)goto _LL103;
_LL102:({void*_tmpE8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpE9="Error: unexpected Gentyp_e\n";
_tag_arr(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,28));}),_tag_arr(_tmpE8,
sizeof(void*),0));});exit(1);return;_LL103: if(*((int*)_tmpB1)!= 24)goto _LL105;
_LL104:({void*_tmpEA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpEB="Error: unexpected Tuple_e\n";
_tag_arr(_tmpEB,sizeof(char),_get_zero_arr_size(_tmpEB,27));}),_tag_arr(_tmpEA,
sizeof(void*),0));});exit(1);return;_LL105: if(*((int*)_tmpB1)!= 25)goto _LL107;
_LL106:({void*_tmpEC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpED="Error: unexpected CompoundLit_e\n";
_tag_arr(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,33));}),_tag_arr(_tmpEC,
sizeof(void*),0));});exit(1);return;_LL107: if(*((int*)_tmpB1)!= 26)goto _LL109;
_LL108:({void*_tmpEE[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpEF="Error: unexpected Array_e\n";
_tag_arr(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,27));}),_tag_arr(_tmpEE,
sizeof(void*),0));});exit(1);return;_LL109: if(*((int*)_tmpB1)!= 27)goto _LL10B;
_LL10A:({void*_tmpF0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF1="Error: unexpected Comprehension_e\n";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,35));}),_tag_arr(_tmpF0,
sizeof(void*),0));});exit(1);return;_LL10B: if(*((int*)_tmpB1)!= 28)goto _LL10D;
_LL10C:({void*_tmpF2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF3="Error: unexpected Struct_e\n";
_tag_arr(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,28));}),_tag_arr(_tmpF2,
sizeof(void*),0));});exit(1);return;_LL10D: if(*((int*)_tmpB1)!= 29)goto _LL10F;
_LL10E:({void*_tmpF4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF5="Error: unexpected AnonStruct_e\n";
_tag_arr(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,32));}),_tag_arr(_tmpF4,
sizeof(void*),0));});exit(1);return;_LL10F: if(*((int*)_tmpB1)!= 30)goto _LL111;
_LL110:({void*_tmpF6[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF7="Error: unexpected Tunion_e\n";
_tag_arr(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,28));}),_tag_arr(_tmpF6,
sizeof(void*),0));});exit(1);return;_LL111: if(*((int*)_tmpB1)!= 31)goto _LL113;
_LL112:({void*_tmpF8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF9="Error: unexpected Enum_e\n";
_tag_arr(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,26));}),_tag_arr(_tmpF8,
sizeof(void*),0));});exit(1);return;_LL113: if(*((int*)_tmpB1)!= 32)goto _LL115;
_LL114:({void*_tmpFA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpFB="Error: unexpected AnonEnum_e\n";
_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,30));}),_tag_arr(_tmpFA,
sizeof(void*),0));});exit(1);return;_LL115: if(*((int*)_tmpB1)!= 36)goto _LL117;
_LL116:({void*_tmpFC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpFD="Error: unexpected Codegen_e\n";
_tag_arr(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,29));}),_tag_arr(_tmpFC,
sizeof(void*),0));});exit(1);return;_LL117: if(*((int*)_tmpB1)!= 37)goto _LLCC;
_LL118:({void*_tmpFE[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpFF="Error: unexpected Fill_e\n";
_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,26));}),_tag_arr(_tmpFE,
sizeof(void*),0));});exit(1);return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*
eo){if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*
_tmp100=t;struct Cyc_Absyn_PtrInfo _tmp101;struct Cyc_Absyn_ArrayInfo _tmp102;void*
_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Conref*_tmp105;struct Cyc_Absyn_FnInfo
_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_AggrInfo _tmp108;void*_tmp109;
struct _tuple0*_tmp10A;struct _tuple0 _tmp10B;struct _tagged_arr*_tmp10C;struct
_tuple0*_tmp10D;struct _tuple0 _tmp10E;struct _tagged_arr*_tmp10F;_LL11F: if((int)
_tmp100 != 0)goto _LL121;_LL120: goto _LL122;_LL121: if(_tmp100 <= (void*)3?1:*((int*)
_tmp100)!= 5)goto _LL123;_LL122: goto _LL124;_LL123: if((int)_tmp100 != 1)goto _LL125;
_LL124: goto _LL126;_LL125: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 6)goto _LL127;
_LL126: return;_LL127: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 4)goto _LL129;
_tmp101=((struct Cyc_Absyn_PointerType_struct*)_tmp100)->f1;_LL128: Cyc_scan_type((
void*)_tmp101.elt_typ);return;_LL129: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 
7)goto _LL12B;_tmp102=((struct Cyc_Absyn_ArrayType_struct*)_tmp100)->f1;_tmp103=(
void*)_tmp102.elt_type;_tmp104=_tmp102.num_elts;_tmp105=_tmp102.zero_term;_LL12A:
Cyc_scan_type(_tmp103);Cyc_scan_exp_opt(_tmp104);return;_LL12B: if(_tmp100 <= (
void*)3?1:*((int*)_tmp100)!= 8)goto _LL12D;_tmp106=((struct Cyc_Absyn_FnType_struct*)
_tmp100)->f1;_LL12C: Cyc_scan_type((void*)_tmp106.ret_typ);{struct Cyc_List_List*
_tmp110=_tmp106.args;for(0;_tmp110 != 0;_tmp110=_tmp110->tl){struct _tuple1 _tmp112;
void*_tmp113;struct _tuple1*_tmp111=(struct _tuple1*)_tmp110->hd;_tmp112=*_tmp111;
_tmp113=_tmp112.f3;Cyc_scan_type(_tmp113);}}if(_tmp106.cyc_varargs != 0)Cyc_scan_type((
void*)(_tmp106.cyc_varargs)->type);return;_LL12D: if(_tmp100 <= (void*)3?1:*((int*)
_tmp100)!= 11)goto _LL12F;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp100)->f2;
_LL12E: for(0;_tmp107 != 0;_tmp107=_tmp107->tl){Cyc_scan_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp107->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp107->hd)->width);}
return;_LL12F: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 13)goto _LL131;_LL130:
return;_LL131: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 10)goto _LL133;_tmp108=((
struct Cyc_Absyn_AggrType_struct*)_tmp100)->f1;_tmp109=(void*)_tmp108.aggr_info;
_LL132: {struct _tuple0*_tmp115;struct _tuple0 _tmp116;struct _tagged_arr*_tmp117;
struct _tuple3 _tmp114=Cyc_Absyn_aggr_kinded_name(_tmp109);_tmp115=_tmp114.f2;
_tmp116=*_tmp115;_tmp117=_tmp116.f2;_tmp10C=_tmp117;goto _LL134;}_LL133: if(
_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 12)goto _LL135;_tmp10A=((struct Cyc_Absyn_EnumType_struct*)
_tmp100)->f1;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.f2;_LL134: _tmp10F=_tmp10C;goto
_LL136;_LL135: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 16)goto _LL137;_tmp10D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp100)->f1;_tmp10E=*_tmp10D;_tmp10F=
_tmp10E.f2;_LL136: Cyc_add_target(_tmp10F);return;_LL137: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 0)goto _LL139;_LL138:({void*_tmp118[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp119="Error: unexpected Evar\n";_tag_arr(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,24));}),_tag_arr(_tmp118,sizeof(void*),0));});exit(1);
return;_LL139: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 1)goto _LL13B;_LL13A:({
void*_tmp11A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11B="Error: unexpected VarType\n";
_tag_arr(_tmp11B,sizeof(char),_get_zero_arr_size(_tmp11B,27));}),_tag_arr(
_tmp11A,sizeof(void*),0));});exit(1);return;_LL13B: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 2)goto _LL13D;_LL13C:({void*_tmp11C[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp11D="Error: unexpected TunionType\n";_tag_arr(_tmp11D,sizeof(char),
_get_zero_arr_size(_tmp11D,30));}),_tag_arr(_tmp11C,sizeof(void*),0));});exit(1);
return;_LL13D: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 3)goto _LL13F;_LL13E:({
void*_tmp11E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11F="Error: unexpected TunionFieldType\n";
_tag_arr(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,35));}),_tag_arr(
_tmp11E,sizeof(void*),0));});exit(1);return;_LL13F: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 9)goto _LL141;_LL140:({void*_tmp120[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp121="Error: unexpected TupleType\n";_tag_arr(_tmp121,sizeof(char),
_get_zero_arr_size(_tmp121,29));}),_tag_arr(_tmp120,sizeof(void*),0));});exit(1);
return;_LL141: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 15)goto _LL143;_LL142:({
void*_tmp122[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp123="Error: unexpected RgnHandleType\n";
_tag_arr(_tmp123,sizeof(char),_get_zero_arr_size(_tmp123,33));}),_tag_arr(
_tmp122,sizeof(void*),0));});exit(1);return;_LL143: if((int)_tmp100 != 2)goto
_LL145;_LL144:({void*_tmp124[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp125="Error: unexpected HeapRgn\n";
_tag_arr(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,27));}),_tag_arr(
_tmp124,sizeof(void*),0));});exit(1);return;_LL145: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 19)goto _LL147;_LL146:({void*_tmp126[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp127="Error: unexpected AccessEff\n";_tag_arr(_tmp127,sizeof(char),
_get_zero_arr_size(_tmp127,29));}),_tag_arr(_tmp126,sizeof(void*),0));});exit(1);
return;_LL147: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 20)goto _LL149;_LL148:({
void*_tmp128[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp129="Error: unexpected JoinEff\n";
_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,27));}),_tag_arr(
_tmp128,sizeof(void*),0));});exit(1);return;_LL149: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 21)goto _LL14B;_LL14A:({void*_tmp12A[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp12B="Error: unexpected RgnsEff\n";_tag_arr(_tmp12B,sizeof(char),
_get_zero_arr_size(_tmp12B,27));}),_tag_arr(_tmp12A,sizeof(void*),0));});exit(1);
return;_LL14B: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 14)goto _LL14D;_LL14C:({
void*_tmp12C[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp12D="Error: unexpected sizeof_t\n";
_tag_arr(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,28));}),_tag_arr(
_tmp12C,sizeof(void*),0));});exit(1);return;_LL14D: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 18)goto _LL14F;_LL14E:({void*_tmp12E[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp12F="Error: unexpected type integer\n";_tag_arr(_tmp12F,sizeof(
char),_get_zero_arr_size(_tmp12F,32));}),_tag_arr(_tmp12E,sizeof(void*),0));});
exit(1);return;_LL14F: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 17)goto _LL11E;
_LL150:({void*_tmp130[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp131="Error: unexpected tag_t\n";
_tag_arr(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,25));}),_tag_arr(
_tmp130,sizeof(void*),0));});exit(1);return;_LL11E:;}struct _tuple15{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple10*Cyc_scan_decl(
struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**_tmp132=
_cycalloc(sizeof(*_tmp132));_tmp132[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp132;});{void*
_tmp133=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp134;struct Cyc_Absyn_Fndecl*
_tmp135;struct Cyc_Absyn_Aggrdecl*_tmp136;struct Cyc_Absyn_Enumdecl*_tmp137;struct
Cyc_Absyn_Typedefdecl*_tmp138;_LL152: if(*((int*)_tmp133)!= 0)goto _LL154;_tmp134=((
struct Cyc_Absyn_Var_d_struct*)_tmp133)->f1;_LL153: {struct _tuple0 _tmp13A;struct
_tagged_arr*_tmp13B;struct _tuple0*_tmp139=_tmp134->name;_tmp13A=*_tmp139;_tmp13B=
_tmp13A.f2;Cyc_current_source=(struct _tagged_arr*)_tmp13B;Cyc_scan_type((void*)
_tmp134->type);Cyc_scan_exp_opt(_tmp134->initializer);goto _LL151;}_LL154: if(*((
int*)_tmp133)!= 1)goto _LL156;_tmp135=((struct Cyc_Absyn_Fn_d_struct*)_tmp133)->f1;
_LL155: {struct _tuple0 _tmp13D;struct _tagged_arr*_tmp13E;struct _tuple0*_tmp13C=
_tmp135->name;_tmp13D=*_tmp13C;_tmp13E=_tmp13D.f2;Cyc_current_source=(struct
_tagged_arr*)_tmp13E;Cyc_scan_type((void*)_tmp135->ret_type);{struct Cyc_List_List*
_tmp13F=_tmp135->args;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){struct _tuple15
_tmp141;void*_tmp142;struct _tuple15*_tmp140=(struct _tuple15*)_tmp13F->hd;_tmp141=*
_tmp140;_tmp142=_tmp141.f3;Cyc_scan_type(_tmp142);}}if(_tmp135->cyc_varargs != 0)
Cyc_scan_type((void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp135->cyc_varargs))->type);
if(_tmp135->is_inline)({void*_tmp143[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp144="Warning: ignoring inline function\n";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,35));}),_tag_arr(_tmp143,sizeof(void*),0));});goto
_LL151;}_LL156: if(*((int*)_tmp133)!= 4)goto _LL158;_tmp136=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp133)->f1;_LL157: {struct _tuple0 _tmp146;struct _tagged_arr*_tmp147;struct
_tuple0*_tmp145=_tmp136->name;_tmp146=*_tmp145;_tmp147=_tmp146.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp147;if((unsigned int)_tmp136->impl){{struct Cyc_List_List*
_tmp148=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp136->impl))->fields;for(
0;_tmp148 != 0;_tmp148=_tmp148->tl){struct Cyc_Absyn_Aggrfield*_tmp149=(struct Cyc_Absyn_Aggrfield*)
_tmp148->hd;Cyc_scan_type((void*)_tmp149->type);Cyc_scan_exp_opt(_tmp149->width);}}{
struct Cyc_List_List*_tmp14A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp136->impl))->fields;
for(0;_tmp14A != 0;_tmp14A=_tmp14A->tl){;}}}goto _LL151;}_LL158: if(*((int*)_tmp133)
!= 6)goto _LL15A;_tmp137=((struct Cyc_Absyn_Enum_d_struct*)_tmp133)->f1;_LL159: {
struct _tuple0 _tmp14C;struct _tagged_arr*_tmp14D;struct _tuple0*_tmp14B=_tmp137->name;
_tmp14C=*_tmp14B;_tmp14D=_tmp14C.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp14D;if((unsigned int)_tmp137->fields){{struct Cyc_List_List*_tmp14E=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp137->fields))->v;for(0;
_tmp14E != 0;_tmp14E=_tmp14E->tl){struct Cyc_Absyn_Enumfield*_tmp14F=(struct Cyc_Absyn_Enumfield*)
_tmp14E->hd;Cyc_scan_exp_opt(_tmp14F->tag);}}{struct Cyc_List_List*_tmp150=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp137->fields))->v;for(
0;_tmp150 != 0;_tmp150=_tmp150->tl){;}}}goto _LL151;}_LL15A: if(*((int*)_tmp133)!= 
7)goto _LL15C;_tmp138=((struct Cyc_Absyn_Typedef_d_struct*)_tmp133)->f1;_LL15B: {
struct _tuple0 _tmp152;struct _tagged_arr*_tmp153;struct _tuple0*_tmp151=_tmp138->name;
_tmp152=*_tmp151;_tmp153=_tmp152.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp153;if((unsigned int)_tmp138->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp138->defn))->v);goto _LL151;}_LL15C: if(*((int*)_tmp133)!= 2)goto
_LL15E;_LL15D:({void*_tmp154[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp155="Error: unexpected let declaration\n";
_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,35));}),_tag_arr(
_tmp154,sizeof(void*),0));});exit(1);return 0;_LL15E: if(*((int*)_tmp133)!= 5)goto
_LL160;_LL15F:({void*_tmp156[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp157="Error: unexpected tunion declaration\n";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,38));}),_tag_arr(
_tmp156,sizeof(void*),0));});exit(1);return 0;_LL160: if(*((int*)_tmp133)!= 3)goto
_LL162;_LL161:({void*_tmp158[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp159="Error: unexpected let declaration\n";
_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,35));}),_tag_arr(
_tmp158,sizeof(void*),0));});exit(1);return 0;_LL162: if(*((int*)_tmp133)!= 8)goto
_LL164;_LL163:({void*_tmp15A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp15B="Error: unexpected namespace declaration\n";
_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,41));}),_tag_arr(
_tmp15A,sizeof(void*),0));});exit(1);return 0;_LL164: if(*((int*)_tmp133)!= 9)goto
_LL166;_LL165:({void*_tmp15C[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp15D="Error: unexpected using declaration\n";
_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,37));}),_tag_arr(
_tmp15C,sizeof(void*),0));});exit(1);return 0;_LL166: if(*((int*)_tmp133)!= 10)
goto _LL151;_LL167:({void*_tmp15E[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp15F="Error: unexpected extern \"C\" declaration\n";_tag_arr(_tmp15F,sizeof(
char),_get_zero_arr_size(_tmp15F,42));}),_tag_arr(_tmp15E,sizeof(void*),0));});
exit(1);return 0;_LL151:;}return({struct _tuple10*_tmp160=_cycalloc(sizeof(*
_tmp160));_tmp160->f1=(struct _tagged_arr*)_check_null(Cyc_current_source);
_tmp160->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp160;});}
struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),int(*hash)(struct
_tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _tagged_arr*x){
struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(
_tmp161.handler))_tmp163=1;if(!_tmp163){{struct Cyc_Set_Set*_tmp164=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp164;};_pop_handler();}else{void*_tmp162=(void*)
_exn_thrown;void*_tmp166=_tmp162;_LL169: if(_tmp166 != Cyc_Core_Not_found)goto
_LL16B;_LL16A: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL16B:;_LL16C:(void)_throw(_tmp166);
_LL168:;}}}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*
t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);struct Cyc_Set_Set*
curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(curr,(struct
_tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp167=curr;struct _tagged_arr*
_tmp168=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp167)> 0){struct Cyc_Set_Set*
_tmp169=emptyset;struct Cyc_Iter_Iter _tmp16A=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp167);while(((int(*)(struct Cyc_Iter_Iter,struct _tagged_arr**))Cyc_Iter_next)(
_tmp16A,& _tmp168)){_tmp169=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp169,Cyc_find(t,_tmp168));}_tmp167=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp169,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp167);}return curr;}}static void*Cyc_mode=(void*)0;
static int Cyc_gathering(){return Cyc_mode == (void*)1?1: Cyc_mode == (void*)2;}static
struct Cyc___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_script_file == 0){({void*_tmp16C[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp16D="Internal error: script file is NULL\n";_tag_arr(
_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,37));}),_tag_arr(_tmp16C,sizeof(
void*),0));});exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _tagged_arr d){if(Cyc_mode == (void*)2)({
struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(struct _tagged_arr)d;{
struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(struct _tagged_arr)d;{
void*_tmp16E[2]={& _tmp170,& _tmp171};Cyc_prscript(({const char*_tmp16F="if ! test -e %s; then mkdir %s; fi\n";
_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,36));}),_tag_arr(
_tmp16E,sizeof(void*),2));}}});else{int _tmp172=({int _tmp176[0];Cyc_open((const
char*)_check_null(_untag_arr(d,sizeof(char),1)),0,_tag_arr(_tmp176,sizeof(int),0));});
if(_tmp172 == - 1){if(mkdir((const char*)_untag_arr(d,sizeof(char),1),448)== - 1){({
struct Cyc_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _tagged_arr)d;{
void*_tmp173[1]={& _tmp175};Cyc_fprintf(Cyc_stderr,({const char*_tmp174="Error: could not create directory %s\n";
_tag_arr(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,38));}),_tag_arr(
_tmp173,sizeof(void*),1));}});return 1;}}else{close(_tmp172);}}return 0;}int Cyc_force_directory_prefixes(
struct _tagged_arr file){struct _tagged_arr _tmp177=Cyc_strdup((struct _tagged_arr)
file);struct Cyc_List_List*_tmp178=0;while(1){_tmp177=Cyc_Filename_dirname((
struct _tagged_arr)_tmp177);if(Cyc_strlen((struct _tagged_arr)_tmp177)== 0)break;
_tmp178=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=({
struct _tagged_arr*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=(struct
_tagged_arr)_tmp177;_tmp17A;});_tmp179->tl=_tmp178;_tmp179;});}for(0;_tmp178 != 0;
_tmp178=_tmp178->tl){if(Cyc_force_directory(*((struct _tagged_arr*)((struct
_tagged_arr*)_tmp178->hd))))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT\000";
struct Cyc_NO_SUPPORT_struct{char*tag;struct _tagged_arr f1;};struct _tuple16{struct
Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(const char*filename,
struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*
prologue,struct Cyc_List_List*epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;({struct Cyc_String_pa_struct _tmp17D;_tmp17D.tag=0;
_tmp17D.f1=({const char*_tmp17E=filename;_tag_arr(_tmp17E,sizeof(char),
_get_zero_arr_size(_tmp17E,1));});{void*_tmp17B[1]={& _tmp17D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp17C="********************************* %s...\n";_tag_arr(_tmp17C,
sizeof(char),_get_zero_arr_size(_tmp17C,41));}),_tag_arr(_tmp17B,sizeof(void*),1));}});
if(!Cyc_gathering())({struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;_tmp181.f1=({
const char*_tmp182=filename;_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(
_tmp182,1));});{void*_tmp17F[1]={& _tmp181};Cyc_log(({const char*_tmp180="\n%s:\n";
_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,6));}),_tag_arr(_tmp17F,
sizeof(void*),1));}});{struct _tagged_arr _tmp183=Cyc_Filename_basename(({const
char*_tmp2AA=filename;_tag_arr(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,1));}));
struct _tagged_arr _tmp184=Cyc_Filename_dirname(({const char*_tmp2A9=filename;
_tag_arr(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,1));}));struct
_tagged_arr _tmp185=Cyc_Filename_chop_extension((struct _tagged_arr)_tmp183);const
char*_tmp186=(const char*)_untag_arr(Cyc_strconcat((struct _tagged_arr)_tmp185,({
const char*_tmp2A8=".iA";_tag_arr(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,
4));})),sizeof(char),1);const char*_tmp187=(const char*)_untag_arr(_get_arr_size(
_tmp184,sizeof(char))== 0?({struct Cyc_String_pa_struct _tmp2A4;_tmp2A4.tag=0;
_tmp2A4.f1=(struct _tagged_arr)_tmp185;{void*_tmp2A2[1]={& _tmp2A4};Cyc_aprintf(({
const char*_tmp2A3="%s.iB";_tag_arr(_tmp2A3,sizeof(char),_get_zero_arr_size(
_tmp2A3,6));}),_tag_arr(_tmp2A2,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp184,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2A7;
_tmp2A7.tag=0;_tmp2A7.f1=(struct _tagged_arr)_tmp185;{void*_tmp2A5[1]={& _tmp2A7};
Cyc_aprintf(({const char*_tmp2A6="%s.iB";_tag_arr(_tmp2A6,sizeof(char),
_get_zero_arr_size(_tmp2A6,6));}),_tag_arr(_tmp2A5,sizeof(void*),1));}})),
sizeof(char),1);const char*_tmp188=(const char*)_untag_arr(_get_arr_size(_tmp184,
sizeof(char))== 0?({struct Cyc_String_pa_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(
struct _tagged_arr)_tmp185;{void*_tmp29C[1]={& _tmp29E};Cyc_aprintf(({const char*
_tmp29D="%s.iC";_tag_arr(_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,6));}),
_tag_arr(_tmp29C,sizeof(void*),1));}}): Cyc_Filename_concat((struct _tagged_arr)
_tmp184,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2A1;_tmp2A1.tag=0;
_tmp2A1.f1=(struct _tagged_arr)_tmp185;{void*_tmp29F[1]={& _tmp2A1};Cyc_aprintf(({
const char*_tmp2A0="%s.iC";_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(
_tmp2A0,6));}),_tag_arr(_tmp29F,sizeof(void*),1));}})),sizeof(char),1);const char*
_tmp189=(const char*)_untag_arr(_get_arr_size(_tmp184,sizeof(char))== 0?({struct
Cyc_String_pa_struct _tmp298;_tmp298.tag=0;_tmp298.f1=(struct _tagged_arr)_tmp185;{
void*_tmp296[1]={& _tmp298};Cyc_aprintf(({const char*_tmp297="%s.iD";_tag_arr(
_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,6));}),_tag_arr(_tmp296,sizeof(
void*),1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp184,(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp29B;_tmp29B.tag=0;_tmp29B.f1=(struct _tagged_arr)
_tmp185;{void*_tmp299[1]={& _tmp29B};Cyc_aprintf(({const char*_tmp29A="%s.iD";
_tag_arr(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,6));}),_tag_arr(_tmp299,
sizeof(void*),1));}})),sizeof(char),1);struct _handler_cons _tmp18A;_push_handler(&
_tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){if(Cyc_force_directory_prefixes(({
const char*_tmp18D=filename;_tag_arr(_tmp18D,sizeof(char),_get_zero_arr_size(
_tmp18D,1));}))){int _tmp18E=1;_npop_handler(0);return _tmp18E;}if(Cyc_mode != (
void*)3){Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){({struct Cyc_String_pa_struct _tmp191;
_tmp191.tag=0;_tmp191.f1=({const char*_tmp192=_tmp186;_tag_arr(_tmp192,sizeof(
char),_get_zero_arr_size(_tmp192,1));});{void*_tmp18F[1]={& _tmp191};Cyc_prscript(({
const char*_tmp190="cat >%s <<XXX\n";_tag_arr(_tmp190,sizeof(char),
_get_zero_arr_size(_tmp190,15));}),_tag_arr(_tmp18F,sizeof(void*),1));}});{
struct Cyc_List_List*_tmp193=Cyc_current_cpp;for(0;_tmp193 != 0;_tmp193=_tmp193->tl){({
struct Cyc_String_pa_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _tagged_arr)*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp193->hd));{void*_tmp194[1]={& _tmp196};
Cyc_prscript(({const char*_tmp195="%s";_tag_arr(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,3));}),_tag_arr(_tmp194,sizeof(void*),1));}});}}({
struct Cyc_String_pa_struct _tmp199;_tmp199.tag=0;_tmp199.f1=({const char*_tmp19A=
filename;_tag_arr(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,1));});{void*
_tmp197[1]={& _tmp199};Cyc_prscript(({const char*_tmp198="#include <%s>\n";
_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,15));}),_tag_arr(
_tmp197,sizeof(void*),1));}});({void*_tmp19B[0]={};Cyc_prscript(({const char*
_tmp19C="XXX\n";_tag_arr(_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,5));}),
_tag_arr(_tmp19B,sizeof(void*),0));});({struct Cyc_String_pa_struct _tmp1A0;
_tmp1A0.tag=0;_tmp1A0.f1=({const char*_tmp1A2=_tmp186;_tag_arr(_tmp1A2,sizeof(
char),_get_zero_arr_size(_tmp1A2,1));});{struct Cyc_String_pa_struct _tmp19F;
_tmp19F.tag=0;_tmp19F.f1=({const char*_tmp1A1=_tmp187;_tag_arr(_tmp1A1,sizeof(
char),_get_zero_arr_size(_tmp1A1,1));});{void*_tmp19D[2]={& _tmp19F,& _tmp1A0};Cyc_prscript(({
const char*_tmp19E="$GCC -E -dM -o %s -x c %s && \\\n";_tag_arr(_tmp19E,sizeof(
char),_get_zero_arr_size(_tmp19E,32));}),_tag_arr(_tmp19D,sizeof(void*),2));}}});({
struct Cyc_String_pa_struct _tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=({const char*_tmp1A8=
_tmp186;_tag_arr(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,1));});{struct
Cyc_String_pa_struct _tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=({const char*_tmp1A7=_tmp188;
_tag_arr(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,1));});{void*_tmp1A3[2]={&
_tmp1A5,& _tmp1A6};Cyc_prscript(({const char*_tmp1A4="$GCC -E     -o %s -x c %s;\n";
_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,28));}),_tag_arr(
_tmp1A3,sizeof(void*),2));}}});({struct Cyc_String_pa_struct _tmp1AB;_tmp1AB.tag=0;
_tmp1AB.f1=({const char*_tmp1AC=_tmp186;_tag_arr(_tmp1AC,sizeof(char),
_get_zero_arr_size(_tmp1AC,1));});{void*_tmp1A9[1]={& _tmp1AB};Cyc_prscript(({
const char*_tmp1AA="rm %s\n";_tag_arr(_tmp1AA,sizeof(char),_get_zero_arr_size(
_tmp1AA,7));}),_tag_arr(_tmp1A9,sizeof(void*),1));}});}else{maybe=Cyc_fopen(
_tmp186,(const char*)"w");if(!((unsigned int)maybe)){({struct Cyc_String_pa_struct
_tmp1AF;_tmp1AF.tag=0;_tmp1AF.f1=({const char*_tmp1B0=_tmp186;_tag_arr(_tmp1B0,
sizeof(char),_get_zero_arr_size(_tmp1B0,1));});{void*_tmp1AD[1]={& _tmp1AF};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1AE="Error: could not create file %s\n";_tag_arr(
_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,33));}),_tag_arr(_tmp1AD,sizeof(
void*),1));}});{int _tmp1B1=1;_npop_handler(0);return _tmp1B1;}}out_file=(struct
Cyc___cycFILE*)_check_null(maybe);{struct Cyc_List_List*_tmp1B2=Cyc_current_cpp;
for(0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){Cyc_fputs((const char*)_untag_arr(*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp1B2->hd)),sizeof(char),1),out_file);}}({
struct Cyc_String_pa_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=({const char*_tmp1B6=
filename;_tag_arr(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,1));});{void*
_tmp1B3[1]={& _tmp1B5};Cyc_fprintf(out_file,({const char*_tmp1B4="#include <%s>\n";
_tag_arr(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,15));}),_tag_arr(
_tmp1B3,sizeof(void*),1));}});Cyc_fclose(out_file);{struct _tagged_arr _tmp1B7=
Cstring_to_string(Ccomp);char*cmd=(char*)_untag_arr(({struct Cyc_String_pa_struct
_tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=({const char*_tmp1C5=_tmp186;_tag_arr(_tmp1C5,
sizeof(char),_get_zero_arr_size(_tmp1C5,1));});{struct Cyc_String_pa_struct
_tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=({const char*_tmp1C4=_tmp187;_tag_arr(_tmp1C4,
sizeof(char),_get_zero_arr_size(_tmp1C4,1));});{struct Cyc_String_pa_struct
_tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct _tagged_arr)_tmp1B7;{void*_tmp1BF[3]={&
_tmp1C1,& _tmp1C2,& _tmp1C3};Cyc_aprintf(({const char*_tmp1C0="%s -E -dM -o %s -x c %s";
_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,24));}),_tag_arr(
_tmp1BF,sizeof(void*),3));}}}}),sizeof(char),1);if(!system((const char*)cmd)){cmd=(
char*)_untag_arr(({struct Cyc_String_pa_struct _tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=({
const char*_tmp1BE=_tmp186;_tag_arr(_tmp1BE,sizeof(char),_get_zero_arr_size(
_tmp1BE,1));});{struct Cyc_String_pa_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=({
const char*_tmp1BD=_tmp188;_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(
_tmp1BD,1));});{struct Cyc_String_pa_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(
struct _tagged_arr)_tmp1B7;{void*_tmp1B8[3]={& _tmp1BA,& _tmp1BB,& _tmp1BC};Cyc_aprintf(({
const char*_tmp1B9="%s -E -o %s -x c %s";_tag_arr(_tmp1B9,sizeof(char),
_get_zero_arr_size(_tmp1B9,20));}),_tag_arr(_tmp1B8,sizeof(void*),3));}}}}),
sizeof(char),1);system((const char*)cmd);}remove(_tmp186);}}}if(Cyc_gathering()){
int _tmp1C6=0;_npop_handler(0);return _tmp1C6;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp187,(const char*)"r");if(!((unsigned int)maybe))(int)_throw((
void*)({struct Cyc_NO_SUPPORT_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[
0]=({struct Cyc_NO_SUPPORT_struct _tmp1C8;_tmp1C8.tag=Cyc_NO_SUPPORT;_tmp1C8.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=({
const char*_tmp1CC=_tmp187;_tag_arr(_tmp1CC,sizeof(char),_get_zero_arr_size(
_tmp1CC,1));});{void*_tmp1C9[1]={& _tmp1CB};Cyc_aprintf(({const char*_tmp1CA="can't open macrosfile %s";
_tag_arr(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,25));}),_tag_arr(
_tmp1C9,sizeof(void*),1));}});_tmp1C8;});_tmp1C7;}));in_file=(struct Cyc___cycFILE*)
_check_null(maybe);{struct Cyc_Lexing_lexbuf*_tmp1CD=Cyc_Lexing_from_file(in_file);
struct _tuple10*entry;while((entry=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp1CD))!= 0){struct _tuple10 _tmp1CF;struct _tagged_arr*_tmp1D0;
struct Cyc_Set_Set*_tmp1D1;struct _tuple10*_tmp1CE=(struct _tuple10*)_check_null(
entry);_tmp1CF=*_tmp1CE;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;((void(*)(struct
Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp1D0,_tmp1D1);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp188,(const char*)"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_struct*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_NO_SUPPORT_struct
_tmp1D3;_tmp1D3.tag=Cyc_NO_SUPPORT;_tmp1D3.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=({const char*_tmp1D7=_tmp188;_tag_arr(_tmp1D7,
sizeof(char),_get_zero_arr_size(_tmp1D7,1));});{void*_tmp1D4[1]={& _tmp1D6};Cyc_aprintf(({
const char*_tmp1D5="can't open declsfile %s";_tag_arr(_tmp1D5,sizeof(char),
_get_zero_arr_size(_tmp1D5,24));}),_tag_arr(_tmp1D4,sizeof(void*),1));}});
_tmp1D3;});_tmp1D2;}));in_file=(struct Cyc___cycFILE*)_check_null(maybe);_tmp1CD=
Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_fopen(_tmp189,(const char*)"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp1D8=1;_npop_handler(0);return _tmp1D8;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp1CD)){;}Cyc_fclose(
in_file);Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));if(Cyc_mode
!= (void*)3)remove(_tmp188);maybe=Cyc_fopen(_tmp189,(const char*)"r");if(!((
unsigned int)maybe)){int _tmp1D9=1;_npop_handler(0);return _tmp1D9;}in_file=(
struct Cyc___cycFILE*)_check_null(maybe);Cyc_Position_reset_position(({const char*
_tmp1DA=_tmp189;_tag_arr(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,1));}));
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp1DB=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init();Cyc_fclose(in_file);remove(_tmp189);{struct Cyc_List_List*
_tmp1DC=_tmp1DB;for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){struct _tuple10*_tmp1DD=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp1DC->hd);if(_tmp1DD == 0)continue;{
struct _tuple10 _tmp1DF;struct _tagged_arr*_tmp1E0;struct Cyc_Set_Set*_tmp1E1;struct
_tuple10*_tmp1DE=(struct _tuple10*)_check_null(_tmp1DD);_tmp1DF=*_tmp1DE;_tmp1E0=
_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp1E2;
_push_handler(& _tmp1E2);{int _tmp1E4=0;if(setjmp(_tmp1E2.handler))_tmp1E4=1;if(!
_tmp1E4){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_tagged_arr*key))Cyc_Hashtable_lookup)(t,_tmp1E0);;_pop_handler();}else{void*
_tmp1E3=(void*)_exn_thrown;void*_tmp1E6=_tmp1E3;_LL16E: if(_tmp1E6 != Cyc_Core_Not_found)
goto _LL170;_LL16F: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL16D;_LL170:;_LL171:(
void)_throw(_tmp1E6);_LL16D:;}}}((void(*)(struct Cyc_Hashtable_Table*t,struct
_tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1E0,((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp1E1,old));}}}}{struct Cyc_Set_Set*_tmp1E7=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp1E8=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp1E9=_tmp1DB;for(0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){
struct Cyc_Absyn_Decl*_tmp1EA=(struct Cyc_Absyn_Decl*)_tmp1E9->hd;struct
_tagged_arr*name;{void*_tmp1EB=(void*)_tmp1EA->r;struct Cyc_Absyn_Vardecl*_tmp1EC;
struct Cyc_Absyn_Fndecl*_tmp1ED;struct Cyc_Absyn_Aggrdecl*_tmp1EE;struct Cyc_Absyn_Enumdecl*
_tmp1EF;struct Cyc_Absyn_Typedefdecl*_tmp1F0;_LL173: if(*((int*)_tmp1EB)!= 0)goto
_LL175;_tmp1EC=((struct Cyc_Absyn_Var_d_struct*)_tmp1EB)->f1;_LL174: {struct
_tuple0 _tmp1F2;struct _tagged_arr*_tmp1F3;struct _tuple0*_tmp1F1=_tmp1EC->name;
_tmp1F2=*_tmp1F1;_tmp1F3=_tmp1F2.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp1F3);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,
_tmp1F3))name=0;else{name=(struct _tagged_arr*)_tmp1F3;}goto _LL172;}_LL175: if(*((
int*)_tmp1EB)!= 1)goto _LL177;_tmp1ED=((struct Cyc_Absyn_Fn_d_struct*)_tmp1EB)->f1;
_LL176: {struct _tuple0 _tmp1F5;struct _tagged_arr*_tmp1F6;struct _tuple0*_tmp1F4=
_tmp1ED->name;_tmp1F5=*_tmp1F4;_tmp1F6=_tmp1F5.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp1F6);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,
_tmp1F6))name=0;else{name=(struct _tagged_arr*)_tmp1F6;}goto _LL172;}_LL177: if(*((
int*)_tmp1EB)!= 4)goto _LL179;_tmp1EE=((struct Cyc_Absyn_Aggr_d_struct*)_tmp1EB)->f1;
_LL178: {struct _tuple0 _tmp1F8;struct _tagged_arr*_tmp1F9;struct _tuple0*_tmp1F7=
_tmp1EE->name;_tmp1F8=*_tmp1F7;_tmp1F9=_tmp1F8.f2;name=(struct _tagged_arr*)
_tmp1F9;goto _LL172;}_LL179: if(*((int*)_tmp1EB)!= 6)goto _LL17B;_tmp1EF=((struct
Cyc_Absyn_Enum_d_struct*)_tmp1EB)->f1;_LL17A: {struct _tuple0 _tmp1FB;struct
_tagged_arr*_tmp1FC;struct _tuple0*_tmp1FA=_tmp1EF->name;_tmp1FB=*_tmp1FA;_tmp1FC=
_tmp1FB.f2;name=(struct _tagged_arr*)_tmp1FC;if(name != 0?((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp1E7,(struct _tagged_arr*)_check_null(
name)): 0)_tmp1E8=({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD->hd=_tmp1EA;_tmp1FD->tl=_tmp1E8;_tmp1FD;});else{if((unsigned int)_tmp1EF->fields){
struct Cyc_List_List*_tmp1FE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1EF->fields))->v;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){struct
Cyc_Absyn_Enumfield*_tmp1FF=(struct Cyc_Absyn_Enumfield*)_tmp1FE->hd;struct
_tuple0 _tmp201;struct _tagged_arr*_tmp202;struct _tuple0*_tmp200=_tmp1FF->name;
_tmp201=*_tmp200;_tmp202=_tmp201.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(_tmp1E7,_tmp202)){_tmp1E8=({struct Cyc_List_List*
_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->hd=_tmp1EA;_tmp203->tl=_tmp1E8;
_tmp203;});break;}}}}name=0;goto _LL172;}_LL17B: if(*((int*)_tmp1EB)!= 7)goto
_LL17D;_tmp1F0=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1EB)->f1;_LL17C: {struct
_tuple0 _tmp205;struct _tagged_arr*_tmp206;struct _tuple0*_tmp204=_tmp1F0->name;
_tmp205=*_tmp204;_tmp206=_tmp205.f2;name=(struct _tagged_arr*)_tmp206;goto _LL172;}
_LL17D: if(*((int*)_tmp1EB)!= 2)goto _LL17F;_LL17E: goto _LL180;_LL17F: if(*((int*)
_tmp1EB)!= 5)goto _LL181;_LL180: goto _LL182;_LL181: if(*((int*)_tmp1EB)!= 3)goto
_LL183;_LL182: goto _LL184;_LL183: if(*((int*)_tmp1EB)!= 8)goto _LL185;_LL184: goto
_LL186;_LL185: if(*((int*)_tmp1EB)!= 9)goto _LL187;_LL186: goto _LL188;_LL187: if(*((
int*)_tmp1EB)!= 10)goto _LL172;_LL188: name=0;goto _LL172;_LL172:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp1E7,(
struct _tagged_arr*)_check_null(name)): 0)_tmp1E8=({struct Cyc_List_List*_tmp207=
_cycalloc(sizeof(*_tmp207));_tmp207->hd=_tmp1EA;_tmp207->tl=_tmp1E8;_tmp207;});}}
maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)maybe)){int _tmp208=
1;_npop_handler(0);return _tmp208;}out_file=(struct Cyc___cycFILE*)_check_null(
maybe);{struct _tagged_arr ifdefmacro=({struct Cyc_String_pa_struct _tmp280;_tmp280.tag=
0;_tmp280.f1=({const char*_tmp281=filename;_tag_arr(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,1));});{void*_tmp27E[1]={& _tmp280};Cyc_aprintf(({
const char*_tmp27F="_%s_";_tag_arr(_tmp27F,sizeof(char),_get_zero_arr_size(
_tmp27F,5));}),_tag_arr(_tmp27E,sizeof(void*),1));}});{int _tmp209=0;for(0;
_tmp209 < _get_arr_size(ifdefmacro,sizeof(char));_tmp209 ++){if(((char*)ifdefmacro.curr)[
_tmp209]== '.'?1:((char*)ifdefmacro.curr)[_tmp209]== '/')({struct _tagged_arr
_tmp20A=_tagged_arr_plus(ifdefmacro,sizeof(char),_tmp209);char _tmp20B=*((char*)
_check_unknown_subscript(_tmp20A,sizeof(char),0));char _tmp20C='_';if(
_get_arr_size(_tmp20A,sizeof(char))== 1?_tmp20B == '\000'?_tmp20C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp20A.curr)=_tmp20C;});else{if(((char*)
ifdefmacro.curr)[_tmp209]!= '_'?((char*)ifdefmacro.curr)[_tmp209]!= '/': 0)({
struct _tagged_arr _tmp20D=_tagged_arr_plus(ifdefmacro,sizeof(char),_tmp209);char
_tmp20E=*((char*)_check_unknown_subscript(_tmp20D,sizeof(char),0));char _tmp20F=(
char)toupper((int)((char*)ifdefmacro.curr)[_tmp209]);if(_get_arr_size(_tmp20D,
sizeof(char))== 1?_tmp20E == '\000'?_tmp20F != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp20D.curr)=_tmp20F;});}}}({struct Cyc_String_pa_struct _tmp213;_tmp213.tag=
0;_tmp213.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_String_pa_struct _tmp212;
_tmp212.tag=0;_tmp212.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp210[2]={&
_tmp212,& _tmp213};Cyc_fprintf(out_file,({const char*_tmp211="#ifndef %s\n#define %s\n";
_tag_arr(_tmp211,sizeof(char),_get_zero_arr_size(_tmp211,23));}),_tag_arr(
_tmp210,sizeof(void*),2));}}});if(prologue != 0){struct Cyc_List_List*_tmp214=
prologue;for(0;_tmp214 != 0;_tmp214=_tmp214->tl){struct _tuple12 _tmp216;struct
_tagged_arr _tmp217;struct _tagged_arr _tmp218;struct _tuple12*_tmp215=(struct
_tuple12*)_tmp214->hd;_tmp216=*_tmp215;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;if(
_tmp218.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp217.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp219=
_cycalloc(sizeof(*_tmp219));_tmp219[0]=_tmp217;_tmp219;})): 0)Cyc_fputs((const
char*)_untag_arr(_tmp218,sizeof(char),1),out_file);else{({struct Cyc_String_pa_struct
_tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _tagged_arr)_tmp217;{void*_tmp21A[1]={&
_tmp21C};Cyc_log(({const char*_tmp21B="%s is not supported on this platform\n";
_tag_arr(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,38));}),_tag_arr(
_tmp21A,sizeof(void*),1));}});}}}{struct Cyc_List_List*_tmp21D=0;struct Cyc_List_List*
_tmp21E=0;{struct Cyc_List_List*_tmp21F=_tmp1E8;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
struct Cyc_Absyn_Decl*_tmp220=(struct Cyc_Absyn_Decl*)_tmp21F->hd;int _tmp221=0;
struct _tagged_arr*name;{void*_tmp222=(void*)_tmp220->r;struct Cyc_Absyn_Vardecl*
_tmp223;struct Cyc_Absyn_Fndecl*_tmp224;struct Cyc_Absyn_Aggrdecl*_tmp225;struct
Cyc_Absyn_Enumdecl*_tmp226;struct Cyc_Absyn_Typedefdecl*_tmp227;_LL18A: if(*((int*)
_tmp222)!= 0)goto _LL18C;_tmp223=((struct Cyc_Absyn_Var_d_struct*)_tmp222)->f1;
_LL18B: {struct _tuple0 _tmp229;struct _tagged_arr*_tmp22A;struct _tuple0*_tmp228=
_tmp223->name;_tmp229=*_tmp228;_tmp22A=_tmp229.f2;name=(struct _tagged_arr*)
_tmp22A;goto _LL189;}_LL18C: if(*((int*)_tmp222)!= 1)goto _LL18E;_tmp224=((struct
Cyc_Absyn_Fn_d_struct*)_tmp222)->f1;_LL18D: {struct _tuple0 _tmp22C;struct
_tagged_arr*_tmp22D;struct _tuple0*_tmp22B=_tmp224->name;_tmp22C=*_tmp22B;_tmp22D=
_tmp22C.f2;name=(struct _tagged_arr*)_tmp22D;goto _LL189;}_LL18E: if(*((int*)
_tmp222)!= 4)goto _LL190;_tmp225=((struct Cyc_Absyn_Aggr_d_struct*)_tmp222)->f1;
_LL18F: {struct _tuple0 _tmp22F;struct _tagged_arr*_tmp230;struct _tuple0*_tmp22E=
_tmp225->name;_tmp22F=*_tmp22E;_tmp230=_tmp22F.f2;name=(struct _tagged_arr*)
_tmp230;goto _LL189;}_LL190: if(*((int*)_tmp222)!= 6)goto _LL192;_tmp226=((struct
Cyc_Absyn_Enum_d_struct*)_tmp222)->f1;_LL191: {struct _tuple0 _tmp232;struct
_tagged_arr*_tmp233;struct _tuple0*_tmp231=_tmp226->name;_tmp232=*_tmp231;_tmp233=
_tmp232.f2;name=(struct _tagged_arr*)_tmp233;goto _LL189;}_LL192: if(*((int*)
_tmp222)!= 7)goto _LL194;_tmp227=((struct Cyc_Absyn_Typedef_d_struct*)_tmp222)->f1;
_LL193: {struct _tuple0 _tmp235;struct _tagged_arr*_tmp236;struct _tuple0*_tmp234=
_tmp227->name;_tmp235=*_tmp234;_tmp236=_tmp235.f2;name=(struct _tagged_arr*)
_tmp236;goto _LL189;}_LL194: if(*((int*)_tmp222)!= 2)goto _LL196;_LL195: goto _LL197;
_LL196: if(*((int*)_tmp222)!= 5)goto _LL198;_LL197: goto _LL199;_LL198: if(*((int*)
_tmp222)!= 3)goto _LL19A;_LL199: goto _LL19B;_LL19A: if(*((int*)_tmp222)!= 8)goto
_LL19C;_LL19B: goto _LL19D;_LL19C: if(*((int*)_tmp222)!= 9)goto _LL19E;_LL19D: goto
_LL19F;_LL19E: if(*((int*)_tmp222)!= 10)goto _LL189;_LL19F: name=0;goto _LL189;
_LL189:;}if(!((unsigned int)name)?!_tmp221: 0)continue;_tmp21D=({struct Cyc_List_List*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->hd=_tmp220;_tmp237->tl=_tmp21D;
_tmp237;});_tmp21E=({struct Cyc_List_List*_tmp238=_cycalloc(sizeof(*_tmp238));
_tmp238->hd=name;_tmp238->tl=_tmp21E;_tmp238;});}}{struct Cyc_Tcenv_Tenv*_tmp239=
Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp239,1,_tmp21D);{struct Cyc_List_List*_tmp23C;
struct Cyc_List_List*_tmp23D;struct _tuple16 _tmp23B=({struct _tuple16 _tmp23A;
_tmp23A.f1=_tmp21D;_tmp23A.f2=_tmp21E;_tmp23A;});_tmp23C=_tmp23B.f1;_tmp23D=
_tmp23B.f2;for(0;_tmp23C != 0?_tmp23D != 0: 0;(_tmp23C=_tmp23C->tl,_tmp23D=_tmp23D->tl)){
struct Cyc_Absyn_Decl*_tmp23E=(struct Cyc_Absyn_Decl*)_tmp23C->hd;struct
_tagged_arr*_tmp23F=(struct _tagged_arr*)_tmp23D->hd;int _tmp240=0;if(!((
unsigned int)_tmp23F))_tmp240=1;if((unsigned int)_tmp23F){ifdefmacro=({struct Cyc_String_pa_struct
_tmp243;_tmp243.tag=0;_tmp243.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp23F);{void*_tmp241[1]={& _tmp243};Cyc_aprintf(({const char*_tmp242="_%s_def_";
_tag_arr(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,9));}),_tag_arr(_tmp241,
sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp246;_tmp246.tag=0;_tmp246.f1=(
struct _tagged_arr)ifdefmacro;{void*_tmp244[1]={& _tmp246};Cyc_fprintf(out_file,({
const char*_tmp245="#ifndef %s\n";_tag_arr(_tmp245,sizeof(char),
_get_zero_arr_size(_tmp245,12));}),_tag_arr(_tmp244,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmp249;_tmp249.tag=0;_tmp249.f1=(struct _tagged_arr)
ifdefmacro;{void*_tmp247[1]={& _tmp249};Cyc_fprintf(out_file,({const char*_tmp248="#define %s\n";
_tag_arr(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,12));}),_tag_arr(
_tmp247,sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp24A[1];_tmp24A[0]=_tmp23E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp24A,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp24B[0]={};
Cyc_fprintf(out_file,({const char*_tmp24C="#endif\n";_tag_arr(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,8));}),_tag_arr(_tmp24B,sizeof(void*),0));});}else{
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp24D[1];_tmp24D[0]=_tmp23E;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp24D,
sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}maybe=Cyc_fopen(_tmp187,(const
char*)"r");if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_struct*
_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_NO_SUPPORT_struct
_tmp24F;_tmp24F.tag=Cyc_NO_SUPPORT;_tmp24F.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp252;_tmp252.tag=0;_tmp252.f1=({const char*_tmp253=_tmp187;_tag_arr(_tmp253,
sizeof(char),_get_zero_arr_size(_tmp253,1));});{void*_tmp250[1]={& _tmp252};Cyc_aprintf(({
const char*_tmp251="can't open macrosfile %s";_tag_arr(_tmp251,sizeof(char),
_get_zero_arr_size(_tmp251,25));}),_tag_arr(_tmp250,sizeof(void*),1));}});
_tmp24F;});_tmp24E;}));in_file=(struct Cyc___cycFILE*)_check_null(maybe);_tmp1CD=
Cyc_Lexing_from_file(in_file);{struct _tuple11*entry2;while((entry2=((struct
_tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp1CD))!= 0){
struct _tuple11 _tmp255;struct _tagged_arr _tmp256;struct _tagged_arr*_tmp257;struct
_tuple11*_tmp254=(struct _tuple11*)_check_null(entry2);_tmp255=*_tmp254;_tmp256=
_tmp255.f1;_tmp257=_tmp255.f2;if(((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_member)(_tmp1E7,_tmp257)){({struct Cyc_String_pa_struct _tmp25A;
_tmp25A.tag=0;_tmp25A.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp257);{void*
_tmp258[1]={& _tmp25A};Cyc_fprintf(out_file,({const char*_tmp259="#ifndef %s\n";
_tag_arr(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,12));}),_tag_arr(
_tmp258,sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp25D;_tmp25D.tag=0;
_tmp25D.f1=(struct _tagged_arr)_tmp256;{void*_tmp25B[1]={& _tmp25D};Cyc_fprintf(
out_file,({const char*_tmp25C="%s\n";_tag_arr(_tmp25C,sizeof(char),
_get_zero_arr_size(_tmp25C,4));}),_tag_arr(_tmp25B,sizeof(void*),1));}});({void*
_tmp25E[0]={};Cyc_fprintf(out_file,({const char*_tmp25F="#endif\n";_tag_arr(
_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,8));}),_tag_arr(_tmp25E,sizeof(
void*),0));});}}Cyc_fclose(in_file);if(Cyc_mode != (void*)3)remove(_tmp187);if(
epilogue != 0){struct Cyc_List_List*_tmp260=epilogue;for(0;_tmp260 != 0;_tmp260=
_tmp260->tl){struct _tuple12 _tmp262;struct _tagged_arr _tmp263;struct _tagged_arr
_tmp264;struct _tuple12*_tmp261=(struct _tuple12*)_tmp260->hd;_tmp262=*_tmp261;
_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;if(_tmp264.curr != ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?_tmp263.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[
0]=_tmp263;_tmp265;})): 0)Cyc_fputs((const char*)_untag_arr(_tmp264,sizeof(char),1),
out_file);else{({struct Cyc_String_pa_struct _tmp268;_tmp268.tag=0;_tmp268.f1=(
struct _tagged_arr)_tmp263;{void*_tmp266[1]={& _tmp268};Cyc_log(({const char*
_tmp267="%s is not supported on this platform\n";_tag_arr(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,38));}),_tag_arr(_tmp266,sizeof(void*),1));}});}}}({
void*_tmp269[0]={};Cyc_fprintf(out_file,({const char*_tmp26A="#endif\n";_tag_arr(
_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,8));}),_tag_arr(_tmp269,sizeof(
void*),0));});Cyc_fclose(out_file);if(cstubs != 0){out_file=(struct Cyc___cycFILE*)
_check_null(Cyc_cstubs_file);{struct Cyc_List_List*_tmp26B=cstubs;for(0;_tmp26B != 
0;_tmp26B=_tmp26B->tl){struct _tuple12 _tmp26D;struct _tagged_arr _tmp26E;struct
_tagged_arr _tmp26F;struct _tuple12*_tmp26C=(struct _tuple12*)_tmp26B->hd;_tmp26D=*
_tmp26C;_tmp26E=_tmp26D.f1;_tmp26F=_tmp26D.f2;if(_tmp26F.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp26E.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[
0]=_tmp26E;_tmp270;})): 0)Cyc_fputs((const char*)_untag_arr(_tmp26F,sizeof(char),1),
out_file);}}}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_String_pa_struct _tmp273;_tmp273.tag=0;_tmp273.f1=({const char*_tmp274=
filename;_tag_arr(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,1));});{void*
_tmp271[1]={& _tmp273};Cyc_fprintf(out_file,({const char*_tmp272="#include <%s>\n\n";
_tag_arr(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,16));}),_tag_arr(
_tmp271,sizeof(void*),1));}});if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file);{struct Cyc_List_List*_tmp275=cycstubs;for(0;
_tmp275 != 0;_tmp275=_tmp275->tl){struct _tuple12 _tmp277;struct _tagged_arr _tmp278;
struct _tagged_arr _tmp279;struct _tuple12*_tmp276=(struct _tuple12*)_tmp275->hd;
_tmp277=*_tmp276;_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;if(_tmp279.curr != ((
struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp278.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A[
0]=_tmp278;_tmp27A;})): 0)Cyc_fputs((const char*)_untag_arr(_tmp279,sizeof(char),1),
out_file);}}({void*_tmp27B[0]={};Cyc_fprintf(out_file,({const char*_tmp27C="\n";
_tag_arr(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,2));}),_tag_arr(_tmp27B,
sizeof(void*),0));});}{int _tmp27D=0;_npop_handler(0);return _tmp27D;}}}}}}}}};
_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*_tmp283=_tmp18B;struct
_tagged_arr _tmp284;_LL1A1: if(*((void**)_tmp283)!= Cyc_NO_SUPPORT)goto _LL1A3;
_tmp284=((struct Cyc_NO_SUPPORT_struct*)_tmp283)->f1;_LL1A2:({struct Cyc_String_pa_struct
_tmp287;_tmp287.tag=0;_tmp287.f1=(struct _tagged_arr)_tmp284;{void*_tmp285[1]={&
_tmp287};Cyc_fprintf(Cyc_stderr,({const char*_tmp286="No support because %s\n";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,23));}),_tag_arr(
_tmp285,sizeof(void*),1));}});goto _LL1A4;_LL1A3:;_LL1A4: maybe=Cyc_fopen(filename,(
const char*)"w");if(!((unsigned int)maybe)){({struct Cyc_String_pa_struct _tmp28A;
_tmp28A.tag=0;_tmp28A.f1=({const char*_tmp28B=filename;_tag_arr(_tmp28B,sizeof(
char),_get_zero_arr_size(_tmp28B,1));});{void*_tmp288[1]={& _tmp28A};Cyc_fprintf(
Cyc_stderr,({const char*_tmp289="Error: could not create file %s\n";_tag_arr(
_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,33));}),_tag_arr(_tmp288,sizeof(
void*),1));}});return 1;}out_file=(struct Cyc___cycFILE*)_check_null(maybe);({
struct Cyc_String_pa_struct _tmp28E;_tmp28E.tag=0;_tmp28E.f1=({const char*_tmp28F=
filename;_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,1));});{void*
_tmp28C[1]={& _tmp28E};Cyc_fprintf(out_file,({const char*_tmp28D="#error -- %s is not supported on this platform\n";
_tag_arr(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,48));}),_tag_arr(
_tmp28C,sizeof(void*),1));}});Cyc_fclose(out_file);({struct Cyc_String_pa_struct
_tmp292;_tmp292.tag=0;_tmp292.f1=({const char*_tmp293=filename;_tag_arr(_tmp293,
sizeof(char),_get_zero_arr_size(_tmp293,1));});{void*_tmp290[1]={& _tmp292};Cyc_fprintf(
Cyc_stderr,({const char*_tmp291="Warning: %s will not be supported on this platform\n";
_tag_arr(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,52));}),_tag_arr(
_tmp290,sizeof(void*),1));}});({void*_tmp294[0]={};Cyc_log(({const char*_tmp295="Not supported on this platform\n";
_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,32));}),_tag_arr(
_tmp294,sizeof(void*),0));});remove(_tmp187);remove(_tmp188);remove(_tmp189);
return 0;_LL1A5:;_LL1A6:(void)_throw(_tmp283);_LL1A0:;}}}}int Cyc_process_specfile(
const char*file,const char*dir){struct Cyc___cycFILE*_tmp2AB=Cyc_fopen(file,(const
char*)"r");if(!((unsigned int)_tmp2AB)){({struct Cyc_String_pa_struct _tmp2AE;
_tmp2AE.tag=0;_tmp2AE.f1=({const char*_tmp2AF=file;_tag_arr(_tmp2AF,sizeof(char),
_get_zero_arr_size(_tmp2AF,1));});{void*_tmp2AC[1]={& _tmp2AE};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2AD="Error: could not open %s\n";_tag_arr(_tmp2AD,sizeof(char),
_get_zero_arr_size(_tmp2AD,26));}),_tag_arr(_tmp2AC,sizeof(void*),1));}});return
1;}{struct Cyc___cycFILE*_tmp2B0=(struct Cyc___cycFILE*)_check_null(_tmp2AB);
struct _tagged_arr buf=({char*_tmp2CA=({unsigned int _tmp2C7=(unsigned int)1024;
char*_tmp2C8=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2C7 + 1));{
unsigned int _tmp2C9=_tmp2C7;unsigned int i;for(i=0;i < _tmp2C9;i ++){_tmp2C8[i]='\000';}
_tmp2C8[_tmp2C9]=(char)0;}_tmp2C8;});_tag_arr(_tmp2CA,sizeof(char),
_get_zero_arr_size(_tmp2CA,(unsigned int)1024 + 1));});struct _tagged_arr _tmp2B1=
Cyc_getcwd(buf,_get_arr_size(buf,sizeof(char)));if(Cyc_mode != (void*)2){if(chdir(
dir)){({struct Cyc_String_pa_struct _tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=({const char*
_tmp2B5=dir;_tag_arr(_tmp2B5,sizeof(char),_get_zero_arr_size(_tmp2B5,1));});{
void*_tmp2B2[1]={& _tmp2B4};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B3="Error: can't change directory to %s\n";
_tag_arr(_tmp2B3,sizeof(char),_get_zero_arr_size(_tmp2B3,37));}),_tag_arr(
_tmp2B2,sizeof(void*),1));}});return 1;}}if(Cyc_mode == (void*)1){struct
_tagged_arr _tmp2B6=Cstring_to_string(Ccomp);system((const char*)_untag_arr(({
struct Cyc_String_pa_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _tagged_arr)
_tmp2B6;{void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({const char*_tmp2B8="echo | %s -E -dM - -o INITMACROS.h\n";
_tag_arr(_tmp2B8,sizeof(char),_get_zero_arr_size(_tmp2B8,36));}),_tag_arr(
_tmp2B7,sizeof(void*),1));}}),sizeof(char),1));}{struct Cyc_Lexing_lexbuf*_tmp2BA=
Cyc_Lexing_from_file(_tmp2B0);struct _tuple13*entry;while((entry=((struct _tuple13*(*)(
struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp2BA))!= 0){struct _tuple13 _tmp2BC;
struct _tagged_arr _tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2BF;
struct Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*
_tmp2C2;struct Cyc_List_List*_tmp2C3;struct _tuple13*_tmp2BB=(struct _tuple13*)
_check_null(entry);_tmp2BC=*_tmp2BB;_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;
_tmp2BF=_tmp2BC.f3;_tmp2C0=_tmp2BC.f4;_tmp2C1=_tmp2BC.f5;_tmp2C2=_tmp2BC.f6;
_tmp2C3=_tmp2BC.f7;if(Cyc_process_file((const char*)_untag_arr(_tmp2BD,sizeof(
char),1),_tmp2BE,_tmp2BF,_tmp2C0,_tmp2C1,_tmp2C2,_tmp2C3))return 1;}Cyc_fclose(
_tmp2B0);if(Cyc_mode != (void*)2){if(chdir((const char*)((char*)_untag_arr(_tmp2B1,
sizeof(char),1)))){({struct Cyc_String_pa_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(
struct _tagged_arr)_tmp2B1;{void*_tmp2C4[1]={& _tmp2C6};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2C5="Error: could not change directory to %s\n";_tag_arr(_tmp2C5,
sizeof(char),_get_zero_arr_size(_tmp2C5,41));}),_tag_arr(_tmp2C4,sizeof(void*),1));}});
return 1;}}return 0;}}}static char _tmp2CB[13]="BUILDLIB.OUT";static struct
_tagged_arr Cyc_output_dir={_tmp2CB,_tmp2CB,_tmp2CB + 13};static void Cyc_set_output_dir(
struct _tagged_arr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _tagged_arr s){Cyc_spec_files=({struct Cyc_List_List*
_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->hd=(const char*)_untag_arr(s,sizeof(
char),1);_tmp2CC->tl=Cyc_spec_files;_tmp2CC;});}static void Cyc_set_GATHER(){Cyc_mode=(
void*)1;}static void Cyc_set_GATHERSCRIPT(){Cyc_mode=(void*)2;}static void Cyc_set_FINISH(){
Cyc_mode=(void*)3;}static int Cyc_badparse=0;static void Cyc_unsupported_option(
struct _tagged_arr s){({struct Cyc_String_pa_struct _tmp2CF;_tmp2CF.tag=0;_tmp2CF.f1=(
struct _tagged_arr)s;{void*_tmp2CD[1]={& _tmp2CF};Cyc_fprintf(Cyc_stderr,({const
char*_tmp2CE="Unsupported option %s\n";_tag_arr(_tmp2CE,sizeof(char),
_get_zero_arr_size(_tmp2CE,23));}),_tag_arr(_tmp2CD,sizeof(void*),1));}});Cyc_badparse=
1;}void GC_blacklist_warn_clear();struct _tuple17{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*options=({struct
_tuple17*_tmp2F0[5];_tmp2F0[4]=({struct _tuple17*_tmp309=_cycalloc(sizeof(*
_tmp309));_tmp309->f1=({const char*_tmp30E="-";_tag_arr(_tmp30E,sizeof(char),
_get_zero_arr_size(_tmp30E,2));});_tmp309->f2=1;_tmp309->f3=({const char*_tmp30D="";
_tag_arr(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,1));});_tmp309->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));
_tmp30B[0]=({struct Cyc_Arg_Flag_spec_struct _tmp30C;_tmp30C.tag=1;_tmp30C.f1=Cyc_unsupported_option;
_tmp30C;});_tmp30B;});_tmp309->f5=({const char*_tmp30A="";_tag_arr(_tmp30A,
sizeof(char),_get_zero_arr_size(_tmp30A,1));});_tmp309;});_tmp2F0[3]=({struct
_tuple17*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->f1=({const char*_tmp308="-finish";
_tag_arr(_tmp308,sizeof(char),_get_zero_arr_size(_tmp308,8));});_tmp303->f2=0;
_tmp303->f3=({const char*_tmp307="";_tag_arr(_tmp307,sizeof(char),
_get_zero_arr_size(_tmp307,1));});_tmp303->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp306;_tmp306.tag=0;_tmp306.f1=Cyc_set_FINISH;_tmp306;});_tmp305;});_tmp303->f5=({
const char*_tmp304="Produce Cyclone headers from pre-gathered C library info";
_tag_arr(_tmp304,sizeof(char),_get_zero_arr_size(_tmp304,57));});_tmp303;});
_tmp2F0[2]=({struct _tuple17*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->f1=({
const char*_tmp302="-gatherscript";_tag_arr(_tmp302,sizeof(char),
_get_zero_arr_size(_tmp302,14));});_tmp2FD->f2=0;_tmp2FD->f3=({const char*_tmp301="";
_tag_arr(_tmp301,sizeof(char),_get_zero_arr_size(_tmp301,1));});_tmp2FD->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));
_tmp2FF[0]=({struct Cyc_Arg_Unit_spec_struct _tmp300;_tmp300.tag=0;_tmp300.f1=Cyc_set_GATHERSCRIPT;
_tmp300;});_tmp2FF;});_tmp2FD->f5=({const char*_tmp2FE="Produce a script to gather C library info";
_tag_arr(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,42));});_tmp2FD;});
_tmp2F0[1]=({struct _tuple17*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->f1=({
const char*_tmp2FC="-gather";_tag_arr(_tmp2FC,sizeof(char),_get_zero_arr_size(
_tmp2FC,8));});_tmp2F7->f2=0;_tmp2F7->f3=({const char*_tmp2FB="";_tag_arr(_tmp2FB,
sizeof(char),_get_zero_arr_size(_tmp2FB,1));});_tmp2F7->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp2FA;_tmp2FA.tag=0;_tmp2FA.f1=Cyc_set_GATHER;_tmp2FA;});_tmp2F9;});_tmp2F7->f5=({
const char*_tmp2F8="Gather C library info but don't produce Cyclone headers";
_tag_arr(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,56));});_tmp2F7;});
_tmp2F0[0]=({struct _tuple17*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->f1=({
const char*_tmp2F6="-d";_tag_arr(_tmp2F6,sizeof(char),_get_zero_arr_size(_tmp2F6,
3));});_tmp2F1->f2=0;_tmp2F1->f3=({const char*_tmp2F5=" <file>";_tag_arr(_tmp2F5,
sizeof(char),_get_zero_arr_size(_tmp2F5,8));});_tmp2F1->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[0]=({struct Cyc_Arg_String_spec_struct
_tmp2F4;_tmp2F4.tag=5;_tmp2F4.f1=Cyc_set_output_dir;_tmp2F4;});_tmp2F3;});
_tmp2F1->f5=({const char*_tmp2F2="Set the output directory to <file>";_tag_arr(
_tmp2F2,sizeof(char),_get_zero_arr_size(_tmp2F2,35));});_tmp2F1;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2F0,sizeof(struct _tuple17*),5));});
Cyc_Arg_parse(options,Cyc_add_spec_file,({const char*_tmp2D0="Options:";_tag_arr(
_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,9));}),argv);if(Cyc_badparse?1:
Cyc_spec_files == 0){Cyc_Arg_usage(options,({const char*_tmp2D1="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";
_tag_arr(_tmp2D1,sizeof(char),_get_zero_arr_size(_tmp2D1,59));}));return 1;}if(
Cyc_mode == (void*)2){Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const
char*)"w");if(!((unsigned int)Cyc_script_file)){({void*_tmp2D2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2D3="Could not create file BUILDLIB.sh\n";_tag_arr(
_tmp2D3,sizeof(char),_get_zero_arr_size(_tmp2D3,35));}),_tag_arr(_tmp2D2,sizeof(
void*),0));});exit(1);}({void*_tmp2D4[0]={};Cyc_prscript(({const char*_tmp2D5="#!/bin/sh\n";
_tag_arr(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,11));}),_tag_arr(
_tmp2D4,sizeof(void*),0));});({void*_tmp2D6[0]={};Cyc_prscript(({const char*
_tmp2D7="GCC=\"gcc\"\n";_tag_arr(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,
11));}),_tag_arr(_tmp2D6,sizeof(void*),0));});}if(Cyc_force_directory_prefixes(
Cyc_output_dir)?1: Cyc_force_directory(Cyc_output_dir)){({struct Cyc_String_pa_struct
_tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2D8[
1]={& _tmp2DA};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D9="Error: could not create directory %s\n";
_tag_arr(_tmp2D9,sizeof(char),_get_zero_arr_size(_tmp2D9,38));}),_tag_arr(
_tmp2D8,sizeof(void*),1));}});return 1;}if(Cyc_mode == (void*)2){({struct Cyc_String_pa_struct
_tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2DB[
1]={& _tmp2DD};Cyc_prscript(({const char*_tmp2DC="cd %s\n";_tag_arr(_tmp2DC,
sizeof(char),_get_zero_arr_size(_tmp2DC,7));}),_tag_arr(_tmp2DB,sizeof(void*),1));}});({
void*_tmp2DE[0]={};Cyc_prscript(({const char*_tmp2DF="echo | $GCC -E -dM - -o INITMACROS.h\n";
_tag_arr(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,38));}),_tag_arr(
_tmp2DE,sizeof(void*),0));});}if(!Cyc_gathering()){Cyc_log_file=Cyc_fopen((const
char*)_untag_arr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2E0="BUILDLIB.LOG";
_tag_arr(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,13));})),sizeof(char),1),(
const char*)"w");if(!((unsigned int)Cyc_log_file)){({struct Cyc_String_pa_struct
_tmp2E3;_tmp2E3.tag=0;_tmp2E3.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2E1[
1]={& _tmp2E3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E2="Error: could not create log file in directory %s\n";
_tag_arr(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,50));}),_tag_arr(
_tmp2E1,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=Cyc_fopen((const char*)
_untag_arr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2E4="cstubs.c";
_tag_arr(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,9));})),sizeof(char),1),(
const char*)"w");if(!((unsigned int)Cyc_cstubs_file)){({struct Cyc_String_pa_struct
_tmp2E7;_tmp2E7.tag=0;_tmp2E7.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2E5[
1]={& _tmp2E7};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E6="Error: could not create cstubs.c in directory %s\n";
_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,50));}),_tag_arr(
_tmp2E5,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_arr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp2E8="cycstubs.cyc";
_tag_arr(_tmp2E8,sizeof(char),_get_zero_arr_size(_tmp2E8,13));})),sizeof(char),1),(
const char*)"w");if(!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_String_pa_struct
_tmp2EB;_tmp2EB.tag=0;_tmp2EB.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp2E9[
1]={& _tmp2EB};Cyc_fprintf(Cyc_stderr,({const char*_tmp2EA="Error: could not create cycstubs.c in directory %s\n";
_tag_arr(_tmp2EA,sizeof(char),_get_zero_arr_size(_tmp2EA,52));}),_tag_arr(
_tmp2E9,sizeof(void*),1));}});return 1;}({void*_tmp2EC[0]={};Cyc_fprintf((struct
Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),({const char*_tmp2ED="#include <core.h>\nusing Core;\n\n";
_tag_arr(_tmp2ED,sizeof(char),_get_zero_arr_size(_tmp2ED,32));}),_tag_arr(
_tmp2EC,sizeof(void*),0));});}{const char*outdir=(const char*)_untag_arr(Cyc_output_dir,
sizeof(char),1);for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)
_check_null(Cyc_spec_files))->tl){if(Cyc_process_specfile((const char*)((struct
Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){({void*_tmp2EE[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp2EF="FATAL ERROR -- QUIT!\n";_tag_arr(
_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,22));}),_tag_arr(_tmp2EE,sizeof(
void*),0));});exit(1);}}if(Cyc_mode == (void*)2)Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_script_file));else{if(!Cyc_gathering()){Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_log_file));Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}return 0;}}}
