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
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;
typedef struct{int __pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t
Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(
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
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
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
targs;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo
f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*
f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{int tag;
void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int
tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
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
int Cyc_Std_system(struct _tagged_arr);struct Cyc_Std_stat{unsigned long long st_dev;
unsigned short __pad1;unsigned int st_ino;unsigned int st_mode;unsigned int st_nlink;
unsigned int st_uid;unsigned int st_gid;unsigned long long st_rdev;unsigned short
__pad2;int st_size;int st_blksize;int st_blocks;int st_atime;unsigned int __unused1;
int st_mtime;unsigned int __unused2;int st_ctime;unsigned int __unused3;unsigned int
__unused4;unsigned int __unused5;};int Cyc_Std_mkdir(struct _tagged_arr pathname,
unsigned int mode);struct Cyc_Std_flock{short l_type;short l_whence;int l_start;int
l_len;int l_pid;};struct Cyc_Std_Flock_struct{int tag;struct Cyc_Std_flock*f1;};
struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_open(struct _tagged_arr,int,
struct _tagged_arr);enum Cyc_Std___anonymous_enum_362__{Cyc_Std__PC_LINK_MAX  = 0,
Cyc_Std__PC_MAX_CANON  = 1,Cyc_Std__PC_MAX_INPUT  = 2,Cyc_Std__PC_NAME_MAX  = 3,Cyc_Std__PC_PATH_MAX
 = 4,Cyc_Std__PC_PIPE_BUF  = 5,Cyc_Std__PC_CHOWN_RESTRICTED  = 6,Cyc_Std__PC_NO_TRUNC
 = 7,Cyc_Std__PC_VDISABLE  = 8,Cyc_Std__PC_SYNC_IO  = 9,Cyc_Std__PC_ASYNC_IO  = 10,
Cyc_Std__PC_PRIO_IO  = 11,Cyc_Std__PC_SOCK_MAXBUF  = 12,Cyc_Std__PC_FILESIZEBITS
 = 13,Cyc_Std__PC_REC_INCR_XFER_SIZE  = 14,Cyc_Std__PC_REC_MAX_XFER_SIZE  = 15,Cyc_Std__PC_REC_MIN_XFER_SIZE
 = 16,Cyc_Std__PC_REC_XFER_ALIGN  = 17};enum Cyc_Std___anonymous_enum_364__{Cyc_Std__CS_PATH
 = 0};int Cyc_Std_chdir(struct _tagged_arr);int Cyc_Std_chroot(struct _tagged_arr);
int close(int);struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int
size);struct _tagged_arr Cyc_Std_getpass(struct _tagged_arr);int isspace(int);int
toupper(int);void Cyc_Lex_lex_init();extern char*Ccomp;struct Cyc_Std___cycFILE*Cyc_log_file=
0;struct Cyc_Std___cycFILE*Cyc_cstubs_file=0;struct Cyc_Std___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _tagged_arr fmt,struct _tagged_arr ap){if(Cyc_log_file == 0){({
void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Internal error: log file is NULL\n",
sizeof(char),34),_tag_arr(_tmp0,sizeof(void*),0));});exit(1);}{int _tmp1=Cyc_Std_vfprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file),fmt,ap);Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)((struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file)));
return _tmp1;}}static struct _tagged_arr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _tagged_arr*sptr){Cyc_current_targets=({struct Cyc_Set_Set**_tmp2=_cycalloc(
sizeof(*_tmp2));_tmp2[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr);_tmp2;});}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Set_Set*f2;};
struct _tuple10*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc_Std___cycFILE*Cyc_slurp_out=0;int
Cyc_slurp(struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm(struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple11{struct _tagged_arr f1;
struct _tagged_arr*f2;};struct _tuple11*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
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
void*)({struct Cyc_Lexing_Error_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({
struct Cyc_Lexing_Error_struct _tmp4;_tmp4.tag=Cyc_Lexing_Error;_tmp4.f1=_tag_arr("empty token",
sizeof(char),12);_tmp4;});_tmp3;}));else{return lbuf->lex_last_action;}}else{if(c
== 256)lbuf->lex_eof_reached=0;}}}struct _tuple10*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((
struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){Cyc_current_targets=({
struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),(struct _tagged_arr*)((struct Cyc_List_List*)
_check_null(Cyc_current_args))->hd);_tmp5;});}return({struct _tuple10*_tmp6=
_cycalloc(sizeof(*_tmp6));_tmp6->f1=(struct _tagged_arr*)_check_null(Cyc_current_source);
_tmp6->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp6;});case
1: _LL1: return Cyc_line(lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(
lexbuf);return Cyc_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Lexing_Error_struct _tmp8;
_tmp8.tag=Cyc_Lexing_Error;_tmp8.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp8;});_tmp7;}));}struct _tuple10*Cyc_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5: Cyc_current_source=({struct _tagged_arr*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9[0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 2));_tmp9;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpA;});
Cyc_token(lexbuf);return 0;case 1: _LL6: Cyc_current_source=({struct _tagged_arr*
_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpB;});Cyc_current_args=0;Cyc_current_targets=({
struct Cyc_Set_Set**_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmpC;});Cyc_args(lexbuf);return 0;case 2: _LL7: Cyc_current_source=({struct
_tagged_arr*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmpD;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpE;});
Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);return Cyc_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpF=
_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Lexing_Error_struct _tmp10;_tmp10.tag=
Cyc_Lexing_Error;_tmp10.f1=_tag_arr("some action didn't return!",sizeof(char),27);
_tmp10;});_tmpF;}));}int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(
lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA: {struct _tagged_arr*
_tmp11=({struct _tagged_arr*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=(struct
_tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
2));_tmp13;});Cyc_current_args=({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*
_tmp12));_tmp12->hd=_tmp11;_tmp12->tl=Cyc_current_args;_tmp12;});return Cyc_args(
lexbuf);}case 1: _LLB: {struct _tagged_arr*_tmp14=({struct _tagged_arr*_tmp16=
_cycalloc(sizeof(*_tmp16));_tmp16[0]=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp16;});Cyc_current_args=({
struct Cyc_List_List*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->hd=_tmp14;_tmp15->tl=
Cyc_current_args;_tmp15;});return Cyc_token(lexbuf);}default: _LLC:(lexbuf->refill_buff)(
lexbuf);return Cyc_args_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Lexing_Error_struct
_tmp18;_tmp18.tag=Cyc_Lexing_Error;_tmp18.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp18;});_tmp17;}));}int Cyc_args(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLE: Cyc_add_target(({struct _tagged_arr*_tmp19=_cycalloc(sizeof(*_tmp19));
_tmp19[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp19;}));return Cyc_token(
lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(lexbuf);case 3: _LL11: Cyc_string(
lexbuf);return Cyc_token(lexbuf);case 4: _LL12: return Cyc_token(lexbuf);case 5: _LL13:
return Cyc_token(lexbuf);case 6: _LL14: return Cyc_token(lexbuf);case 7: _LL15: return
Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(lexbuf);case 9: _LL17: return Cyc_token(
lexbuf);case 10: _LL18: return Cyc_token(lexbuf);case 11: _LL19: return Cyc_token(lexbuf);
case 12: _LL1A: return Cyc_token(lexbuf);case 13: _LL1B: return Cyc_token(lexbuf);case 14:
_LL1C: return Cyc_token(lexbuf);case 15: _LL1D: return Cyc_token(lexbuf);case 16: _LL1E:
return Cyc_token(lexbuf);case 17: _LL1F: return Cyc_token(lexbuf);case 18: _LL20: return
Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(lexbuf);case 20: _LL22: return Cyc_token(
lexbuf);case 21: _LL23: return Cyc_token(lexbuf);case 22: _LL24: return Cyc_token(lexbuf);
case 23: _LL25: return Cyc_token(lexbuf);case 24: _LL26: return Cyc_token(lexbuf);case 25:
_LL27: return Cyc_token(lexbuf);case 26: _LL28: return Cyc_token(lexbuf);case 27: _LL29:
return Cyc_token(lexbuf);case 28: _LL2A: return Cyc_token(lexbuf);case 29: _LL2B: return
Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(lexbuf);case 31: _LL2D: return Cyc_token(
lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);case 33: _LL2F: return Cyc_token(lexbuf);
case 34: _LL30: return Cyc_token(lexbuf);case 35: _LL31: return Cyc_token(lexbuf);case 36:
_LL32: return Cyc_token(lexbuf);case 37: _LL33: return Cyc_token(lexbuf);case 38: _LL34:
return Cyc_token(lexbuf);case 39: _LL35: return Cyc_token(lexbuf);case 40: _LL36: return
Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(lexbuf);case 42: _LL38: return Cyc_token(
lexbuf);case 43: _LL39: return Cyc_token(lexbuf);case 44: _LL3A: return Cyc_token(lexbuf);
case 45: _LL3B: return Cyc_token(lexbuf);case 46: _LL3C: return Cyc_token(lexbuf);case 47:
_LL3D: return Cyc_token(lexbuf);case 48: _LL3E: return Cyc_token(lexbuf);default: _LL3F:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({
struct Cyc_Lexing_Error_struct _tmp1B;_tmp1B.tag=Cyc_Lexing_Error;_tmp1B.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp1B;});_tmp1A;}));}int
Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL41: return Cyc_string(lexbuf);case 1: _LL42: return
0;case 2: _LL43: return Cyc_string(lexbuf);case 3: _LL44: return Cyc_string(lexbuf);case
4: _LL45: return Cyc_string(lexbuf);case 5: _LL46: return Cyc_string(lexbuf);case 6:
_LL47: return Cyc_string(lexbuf);case 7: _LL48: return 0;case 8: _LL49: return 0;case 9:
_LL4A: return Cyc_string(lexbuf);default: _LL4B:(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Lexing_Error_struct
_tmp1D;_tmp1D.tag=Cyc_Lexing_Error;_tmp1D.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp1D;});_tmp1C;}));}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL4D: return 0;case 1: _LL4E: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}return 1;case 2:
_LL4F: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC(",sizeof(char),30),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp1E[0]={};Cyc_log(
_tag_arr("Warning: declaration of malloc sidestepped\n",sizeof(char),44),
_tag_arr(_tmp1E,sizeof(void*),0));});return 1;case 3: _LL50: Cyc_Std_fputs(_tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC(",
sizeof(char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp1F[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp1F,sizeof(void*),0));});return 1;case 4: _LL51: Cyc_Std_fputs(
_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp20[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp20,sizeof(void*),0));});return 1;case 5: _LL52: Cyc_Std_fputs(
_tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp21[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp21,sizeof(void*),0));});return 1;case 6: _LL53: Cyc_Std_fputs(
_tag_arr("__region",sizeof(char),9),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp22[0]={};Cyc_log(_tag_arr("Warning: use of region sidestepped\n",
sizeof(char),36),_tag_arr(_tmp22,sizeof(void*),0));});return 1;case 7: _LL54: return
1;case 8: _LL55: return 1;case 9: _LL56: return 1;case 10: _LL57: return 1;case 11: _LL58:
return 1;case 12: _LL59: return 1;case 13: _LL5A: Cyc_Std_fputs(_tag_arr("inline",
sizeof(char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 14: _LL5B: Cyc_Std_fputs(_tag_arr("inline",sizeof(char),7),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5C: Cyc_Std_fputs(_tag_arr("const",
sizeof(char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 16: _LL5D: Cyc_Std_fputs(_tag_arr("const",sizeof(char),6),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 17: _LL5E: Cyc_Std_fputs(_tag_arr("int",
sizeof(char),4),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 18: _LL5F: return 1;case 19: _LL60: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}
Cyc_Std_fputs(_tag_arr("0",sizeof(char),2),(struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out));({void*_tmp23[0]={};Cyc_log(_tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof(char),42),_tag_arr(_tmp23,sizeof(void*),0));});return 1;case 20: _LL61: Cyc_Std_fputc((
int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;default: _LL62:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp24=_cycalloc(
sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Lexing_Error_struct _tmp25;_tmp25.tag=Cyc_Lexing_Error;
_tmp25.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp25;});
_tmp24;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL64: return 0;
case 1: _LL65: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 0;case 2: _LL66:({void*_tmp26[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(char),26),_tag_arr(_tmp26,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp27[1]={& _tmp28};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp27,sizeof(void*),1));}});return 1;case
3: _LL67:({struct Cyc_Std_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp29[1]={& _tmp2A};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp29,sizeof(void*),1));}});return 1;case 4: _LL68:({struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp2B[1]={& _tmp2C};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp2B,sizeof(void*),1));}});return 1;case
5: _LL69:({struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp2D[1]={& _tmp2E};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp2D,sizeof(void*),1));}});return 1;case 6: _LL6A:({struct Cyc_Std_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp2F[1]={& _tmp30};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp2F,sizeof(void*),1));}});return 1;case
7: _LL6B:({struct Cyc_Std_String_pa_struct _tmp32;_tmp32.tag=0;_tmp32.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp31[1]={& _tmp32};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp31,sizeof(void*),1));}});return 1;case 8: _LL6C:({struct Cyc_Std_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp33[1]={& _tmp34};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp33,sizeof(void*),1));}});return 1;
default: _LL6D:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp35=_cycalloc(
sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Lexing_Error_struct _tmp36;_tmp36.tag=Cyc_Lexing_Error;
_tmp36.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp36;});
_tmp35;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70:
if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match --;return 1;case 2: _LL71: Cyc_parens_to_match
++;return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73:
while(Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp37=_cycalloc(sizeof(*
_tmp37));_tmp37[0]=({struct Cyc_Lexing_Error_struct _tmp38;_tmp38.tag=Cyc_Lexing_Error;
_tmp38.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp38;});
_tmp37;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,
7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL78:({void*_tmp39[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp39,
sizeof(void*),0));});return 0;case 1: _LL79: return 0;case 2: _LL7A:({void*_tmp3A[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp3A,
sizeof(void*),0));});return 1;case 3: _LL7B: return 1;case 4: _LL7C: return 1;case 5: _LL7D:
return 1;case 6: _LL7E: return 1;case 7: _LL7F: return 1;case 8: _LL80: return 1;default:
_LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp3B=_cycalloc(sizeof(*
_tmp3B));_tmp3B[0]=({struct Cyc_Lexing_Error_struct _tmp3C;_tmp3C.tag=Cyc_Lexing_Error;
_tmp3C.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp3C;});
_tmp3B;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL83:({void*
_tmp3D[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp3D,sizeof(void*),0));});return 0;case 1: _LL84: return 0;case 2: _LL85:
return 1;default: _LL86:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp3E=
_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Lexing_Error_struct _tmp3F;
_tmp3F.tag=Cyc_Lexing_Error;_tmp3F.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp3F;});_tmp3E;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple11*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL88: Cyc_current_line=_tag_arr("#define ",
sizeof(char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple11*_tmp40=
_cycalloc(sizeof(*_tmp40));_tmp40->f1=Cyc_current_line;_tmp40->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp40;});case 1: _LL89: return Cyc_suck_line(
lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41[0]=({struct Cyc_Lexing_Error_struct
_tmp42;_tmp42.tag=Cyc_Lexing_Error;_tmp42.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp42;});_tmp41;}));}struct _tuple11*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL8D: Cyc_current_source=({struct _tagged_arr*_tmp43=_cycalloc(sizeof(*
_tmp43));_tmp43[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp43;});Cyc_current_line=(
struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((struct _tagged_arr*)
_check_null(Cyc_current_source)));return Cyc_suck_restofline(lexbuf);default:
_LL8E:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44[0]=({struct Cyc_Lexing_Error_struct _tmp45;_tmp45.tag=Cyc_Lexing_Error;
_tmp45.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp45;});
_tmp44;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL90:
Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL91:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({
struct Cyc_Lexing_Error_struct _tmp47;_tmp47.tag=Cyc_Lexing_Error;_tmp47.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp47;});_tmp46;}));}int
Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple13*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL93:
return Cyc_spec(lexbuf);case 1: _LL94: Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=
0;while(Cyc_commands(lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);return({struct _tuple13*_tmp48=
_cycalloc(sizeof(*_tmp48));_tmp48->f1=Cyc_current_header;_tmp48->f2=Cyc_current_symbols;
_tmp48->f3=Cyc_current_omit_symbols;_tmp48->f4=Cyc_current_prologue;_tmp48->f5=
Cyc_current_epilogue;_tmp48->f6=Cyc_current_cstubs;_tmp48->f7=Cyc_current_cycstubs;
_tmp48;});case 2: _LL95: return Cyc_spec(lexbuf);case 3: _LL96: return 0;case 4: _LL97:({
struct Cyc_Std_Int_pa_struct _tmp4A;_tmp4A.tag=1;_tmp4A.f1=(int)((unsigned int)((
int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp49[1]={& _tmp4A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(char),67),_tag_arr(_tmp49,sizeof(void*),1));}});return 0;default: _LL98:(
lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({
struct Cyc_Lexing_Error_struct _tmp4C;_tmp4C.tag=Cyc_Lexing_Error;_tmp4C.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp4C;});_tmp4B;}));}
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
struct _tuple12*x=({struct _tuple12*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp4E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp4E;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=x;_tmp4D->tl=
Cyc_current_prologue;_tmp4D;});return 1;}case 5: _LL9F: {struct _tagged_arr _tmp4F=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp4F,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("prologue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp4F,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp4F,sizeof(char),1);}{struct _tagged_arr t=_tmp4F;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp50=Cyc_Std_substring((struct _tagged_arr)
_tmp4F,0,(unsigned int)((t.curr - _tmp4F.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple12*x=({struct _tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=(
struct _tagged_arr)_tmp50;_tmp52->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp52;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=x;_tmp51->tl=
Cyc_current_prologue;_tmp51;});return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple12*x=({struct _tuple12*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp54->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp54;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=x;_tmp53->tl=
Cyc_current_epilogue;_tmp53;});return 1;}case 7: _LLA1: {struct _tagged_arr _tmp55=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp55,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("epilogue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp55,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp55,sizeof(char),1);}{struct _tagged_arr t=_tmp55;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp56=Cyc_Std_substring((struct _tagged_arr)
_tmp55,0,(unsigned int)((t.curr - _tmp55.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple12*x=({struct _tuple12*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=(
struct _tagged_arr)_tmp56;_tmp58->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp58;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=x;_tmp57->tl=
Cyc_current_epilogue;_tmp57;});return 1;}}}}case 8: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple12*x=({struct _tuple12*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp5A->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp5A;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->hd=x;_tmp59->tl=
Cyc_current_cstubs;_tmp59;});return 1;}case 9: _LLA3: {struct _tagged_arr _tmp5B=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp5B,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cstub",sizeof(char),6)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp5B,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp5B,sizeof(char),1);}{struct _tagged_arr t=_tmp5B;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp5C=Cyc_Std_substring((struct _tagged_arr)
_tmp5B,0,(unsigned int)((t.curr - _tmp5B.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple12*x=({struct _tuple12*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=(
struct _tagged_arr)_tmp5C;_tmp5E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp5E;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=x;_tmp5D->tl=
Cyc_current_cstubs;_tmp5D;});return 1;}}}}case 10: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple12*x=({struct _tuple12*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp60->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp60;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=x;_tmp5F->tl=
Cyc_current_cycstubs;_tmp5F;});return 1;}case 11: _LLA5: {struct _tagged_arr _tmp61=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp61,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cycstub",sizeof(char),8)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp61,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp61,sizeof(char),1);}{struct _tagged_arr t=_tmp61;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp62=Cyc_Std_substring((struct _tagged_arr)
_tmp61,0,(unsigned int)((t.curr - _tmp61.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple12*x=({struct _tuple12*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->f1=(
struct _tagged_arr)_tmp62;_tmp64->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp64;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->hd=x;_tmp63->tl=
Cyc_current_cycstubs;_tmp63;});return 1;}}}}case 12: _LLA6: Cyc_braces_to_match=1;
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tagged_arr*x=({struct _tagged_arr*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[
0]=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp66;});Cyc_current_cpp=({struct Cyc_List_List*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65->hd=x;_tmp65->tl=Cyc_current_cpp;_tmp65;});return 1;}case 13: _LLA7: return 1;
case 14: _LLA8: return 1;case 15: _LLA9:({struct Cyc_Std_Int_pa_struct _tmp68;_tmp68.tag=
1;_tmp68.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*
_tmp67[1]={& _tmp68};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(char),58),_tag_arr(_tmp67,sizeof(void*),1));}});return 0;default: _LLAA:(
lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp69=_cycalloc(sizeof(*_tmp69));
_tmp69[0]=({struct Cyc_Lexing_Error_struct _tmp6A;_tmp6A.tag=Cyc_Lexing_Error;
_tmp6A.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp6A;});
_tmp69;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAC: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->hd=({struct
_tagged_arr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp6C;});_tmp6B->tl=Cyc_snarfed_symbols;_tmp6B;});return 1;case 1: _LLAD:
return 1;case 2: _LLAE: return 0;case 3: _LLAF:({void*_tmp6D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n",sizeof(char),44),
_tag_arr(_tmp6D,sizeof(void*),0));});return 0;case 4: _LLB0:({struct Cyc_Std_Int_pa_struct
_tmp6F;_tmp6F.tag=1;_tmp6F.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp6E[1]={& _tmp6F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(char),57),_tag_arr(_tmp6E,sizeof(void*),1));}});return 0;default: _LLB1:(
lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_Lexing_Error_struct _tmp71;_tmp71.tag=Cyc_Lexing_Error;
_tmp71.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp71;});
_tmp70;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB3:({void*
_tmp72[0]={};Cyc_log(_tag_arr("Warning: unclosed brace\n",sizeof(char),25),
_tag_arr(_tmp72,sizeof(void*),0));});return 0;case 1: _LLB4: if(Cyc_braces_to_match
== 1)return 0;Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB5: Cyc_braces_to_match ++;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB6: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(char),3));while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLB8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLBA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Lexing_Error_struct _tmp74;_tmp74.tag=Cyc_Lexing_Error;
_tmp74.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp74;});
_tmp73;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(
lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBC:({void*
_tmp75[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp75,sizeof(void*),0));});return 0;case 1: _LLBD: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBE:({void*
_tmp76[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp76,sizeof(void*),0));});Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 3: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLC0: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 6: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLC3: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLC4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
default: _LLC5:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp77=_cycalloc(
sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Lexing_Error_struct _tmp78;_tmp78.tag=Cyc_Lexing_Error;
_tmp78.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp78;});
_tmp77;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:({void*
_tmp79[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp79,sizeof(void*),0));});return 0;case 1: _LLC8: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(char),3));
return 0;case 2: _LLC9: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({
struct Cyc_Lexing_Error_struct _tmp7B;_tmp7B.tag=Cyc_Lexing_Error;_tmp7B.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp7B;});_tmp7A;}));}int
Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp7C=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmp7D;struct
_tuple0 _tmp7E;struct _tagged_arr*_tmp7F;struct _tuple0*_tmp80;struct _tuple0 _tmp81;
struct _tagged_arr*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp84;
struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp87;
struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp8A;
struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;
struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;
struct Cyc_Absyn_Exp*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_Absyn_Exp*_tmp93;
struct Cyc_List_List*_tmp94;void*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_MallocInfo
_tmp97;int _tmp98;struct Cyc_Absyn_Exp*_tmp99;void**_tmp9A;struct Cyc_Absyn_Exp*
_tmp9B;void*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct _tagged_arr*_tmp9E;struct Cyc_Absyn_Exp*
_tmp9F;struct _tagged_arr*_tmpA0;void*_tmpA1;void*_tmpA2;struct Cyc_List_List*
_tmpA3;_LLCD: if(*((int*)_tmp7C)!= 1)goto _LLCF;_tmp7D=((struct Cyc_Absyn_Var_e_struct*)
_tmp7C)->f1;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f2;_LLCE: _tmp82=_tmp7F;goto _LLD0;_LLCF:
if(*((int*)_tmp7C)!= 2)goto _LLD1;_tmp80=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp7C)->f1;_tmp81=*_tmp80;_tmp82=_tmp81.f2;_LLD0: Cyc_add_target(_tmp82);return;
_LLD1: if(*((int*)_tmp7C)!= 3)goto _LLD3;_tmp83=((struct Cyc_Absyn_Primop_e_struct*)
_tmp7C)->f2;_LLD2: for(0;_tmp83 != 0;_tmp83=_tmp83->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmp83->hd));}return;_LLD3: if(*((int*)_tmp7C)!= 23)goto
_LLD5;_tmp84=((struct Cyc_Absyn_Subscript_e_struct*)_tmp7C)->f1;_tmp85=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp7C)->f2;_LLD4: _tmp86=_tmp84;_tmp87=_tmp85;goto
_LLD6;_LLD5: if(*((int*)_tmp7C)!= 7)goto _LLD7;_tmp86=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp7C)->f1;_tmp87=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7C)->f2;_LLD6: _tmp88=
_tmp86;_tmp89=_tmp87;goto _LLD8;_LLD7: if(*((int*)_tmp7C)!= 4)goto _LLD9;_tmp88=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp7C)->f1;_tmp89=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp7C)->f3;_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp88);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmp89);return;_LLD9: if(*((int*)_tmp7C)!= 20)goto _LLDB;
_tmp8A=((struct Cyc_Absyn_Deref_e_struct*)_tmp7C)->f1;_LLDA: _tmp8B=_tmp8A;goto
_LLDC;_LLDB: if(*((int*)_tmp7C)!= 17)goto _LLDD;_tmp8B=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp7C)->f1;_LLDC: _tmp8C=_tmp8B;goto _LLDE;_LLDD: if(*((int*)_tmp7C)!= 14)goto
_LLDF;_tmp8C=((struct Cyc_Absyn_Address_e_struct*)_tmp7C)->f1;_LLDE: _tmp8D=_tmp8C;
goto _LLE0;_LLDF: if(*((int*)_tmp7C)!= 5)goto _LLE1;_tmp8D=((struct Cyc_Absyn_Increment_e_struct*)
_tmp7C)->f1;_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp8D);return;_LLE1: if(*((
int*)_tmp7C)!= 6)goto _LLE3;_tmp8E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7C)->f1;
_tmp8F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7C)->f2;_tmp90=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7C)->f3;_LLE2: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp8E);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmp8F);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp90);return;
_LLE3: if(*((int*)_tmp7C)!= 9)goto _LLE5;_tmp91=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp7C)->f1;_tmp92=((struct Cyc_Absyn_FnCall_e_struct*)_tmp7C)->f2;_LLE4: _tmp93=
_tmp91;_tmp94=_tmp92;goto _LLE6;_LLE5: if(*((int*)_tmp7C)!= 8)goto _LLE7;_tmp93=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7C)->f1;_tmp94=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp7C)->f2;_LLE6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp93);for(0;_tmp94 != 0;
_tmp94=_tmp94->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmp94->hd));}return;_LLE7: if(*((int*)_tmp7C)!= 13)goto _LLE9;_tmp95=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp7C)->f1;_tmp96=((struct Cyc_Absyn_Cast_e_struct*)
_tmp7C)->f2;_LLE8: Cyc_scan_type(_tmp95);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp96);return;_LLE9: if(*((int*)_tmp7C)!= 33)goto _LLEB;_tmp97=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp7C)->f1;_tmp98=_tmp97.is_calloc;_tmp99=_tmp97.rgn;_tmp9A=_tmp97.elt_type;
_tmp9B=_tmp97.num_elts;_LLEA: if(_tmp99 != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp99)));if(_tmp9A != 0)Cyc_scan_type(*_tmp9A);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp9B);return;_LLEB: if(*((int*)_tmp7C)!= 16)
goto _LLED;_tmp9C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp7C)->f1;_LLEC:
Cyc_scan_type(_tmp9C);return;_LLED: if(*((int*)_tmp7C)!= 21)goto _LLEF;_tmp9D=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp7C)->f1;_tmp9E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp7C)->f2;_LLEE: _tmp9F=_tmp9D;_tmpA0=_tmp9E;goto _LLF0;_LLEF: if(*((int*)_tmp7C)
!= 22)goto _LLF1;_tmp9F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7C)->f1;_tmpA0=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7C)->f2;_LLF0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp9F);Cyc_add_target(_tmpA0);return;_LLF1: if(*((int*)_tmp7C)!= 18)goto _LLF3;
_tmpA1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp7C)->f1;_tmpA2=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7C)->f2;_LLF2: Cyc_scan_type(_tmpA1);{void*
_tmpA4=_tmpA2;struct _tagged_arr*_tmpA5;_LL11A: if(*((int*)_tmpA4)!= 0)goto _LL11C;
_tmpA5=((struct Cyc_Absyn_StructField_struct*)_tmpA4)->f1;_LL11B: Cyc_add_target(
_tmpA5);goto _LL119;_LL11C: if(*((int*)_tmpA4)!= 1)goto _LL119;_LL11D: goto _LL119;
_LL119:;}return;_LLF3: if(*((int*)_tmp7C)!= 0)goto _LLF5;_LLF4: return;_LLF5: if(*((
int*)_tmp7C)!= 34)goto _LLF7;_tmpA3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp7C)->f2;_LLF6: for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){struct _tuple14 _tmpA7;
struct Cyc_Absyn_Exp*_tmpA8;struct _tuple14*_tmpA6=(struct _tuple14*)_tmpA3->hd;
_tmpA7=*_tmpA6;_tmpA8=_tmpA7.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA8);}
return;_LLF7: if(*((int*)_tmp7C)!= 35)goto _LLF9;_LLF8:({void*_tmpA9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",sizeof(char),26),_tag_arr(
_tmpA9,sizeof(void*),0));});exit(1);return;_LLF9: if(*((int*)_tmp7C)!= 10)goto
_LLFB;_LLFA:({void*_tmpAA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(char),27),_tag_arr(_tmpAA,sizeof(void*),0));});exit(1);return;_LLFB: if(*((
int*)_tmp7C)!= 11)goto _LLFD;_LLFC:({void*_tmpAB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected NoInstantiate_e\n",sizeof(char),35),_tag_arr(_tmpAB,
sizeof(void*),0));});exit(1);return;_LLFD: if(*((int*)_tmp7C)!= 12)goto _LLFF;
_LLFE:({void*_tmpAC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",
sizeof(char),33),_tag_arr(_tmpAC,sizeof(void*),0));});exit(1);return;_LLFF: if(*((
int*)_tmp7C)!= 15)goto _LL101;_LL100:({void*_tmpAD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected New_e\n",sizeof(char),25),_tag_arr(_tmpAD,sizeof(
void*),0));});exit(1);return;_LL101: if(*((int*)_tmp7C)!= 19)goto _LL103;_LL102:({
void*_tmpAE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(char),28),_tag_arr(_tmpAE,sizeof(void*),0));});exit(1);return;_LL103: if(*((
int*)_tmp7C)!= 24)goto _LL105;_LL104:({void*_tmpAF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tuple_e\n",sizeof(char),27),_tag_arr(_tmpAF,sizeof(
void*),0));});exit(1);return;_LL105: if(*((int*)_tmp7C)!= 25)goto _LL107;_LL106:({
void*_tmpB0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(char),33),_tag_arr(_tmpB0,sizeof(void*),0));});exit(1);return;_LL107: if(*((
int*)_tmp7C)!= 26)goto _LL109;_LL108:({void*_tmpB1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Array_e\n",sizeof(char),27),_tag_arr(_tmpB1,sizeof(
void*),0));});exit(1);return;_LL109: if(*((int*)_tmp7C)!= 27)goto _LL10B;_LL10A:({
void*_tmpB2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(char),35),_tag_arr(_tmpB2,sizeof(void*),0));});exit(1);return;_LL10B: if(*((
int*)_tmp7C)!= 28)goto _LL10D;_LL10C:({void*_tmpB3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Struct_e\n",sizeof(char),28),_tag_arr(_tmpB3,sizeof(
void*),0));});exit(1);return;_LL10D: if(*((int*)_tmp7C)!= 29)goto _LL10F;_LL10E:({
void*_tmpB4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(char),32),_tag_arr(_tmpB4,sizeof(void*),0));});exit(1);return;_LL10F: if(*((
int*)_tmp7C)!= 30)goto _LL111;_LL110:({void*_tmpB5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tunion_e\n",sizeof(char),28),_tag_arr(_tmpB5,sizeof(
void*),0));});exit(1);return;_LL111: if(*((int*)_tmp7C)!= 31)goto _LL113;_LL112:({
void*_tmpB6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(char),26),_tag_arr(_tmpB6,sizeof(void*),0));});exit(1);return;_LL113: if(*((
int*)_tmp7C)!= 32)goto _LL115;_LL114:({void*_tmpB7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonEnum_e\n",sizeof(char),30),_tag_arr(_tmpB7,
sizeof(void*),0));});exit(1);return;_LL115: if(*((int*)_tmp7C)!= 36)goto _LL117;
_LL116:({void*_tmpB8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Codegen_e\n",
sizeof(char),29),_tag_arr(_tmpB8,sizeof(void*),0));});exit(1);return;_LL117: if(*((
int*)_tmp7C)!= 37)goto _LLCC;_LL118:({void*_tmpB9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Fill_e\n",sizeof(char),26),_tag_arr(_tmpB9,sizeof(
void*),0));});exit(1);return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){
if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*
_tmpBA=t;struct Cyc_Absyn_PtrInfo _tmpBB;void*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;
struct Cyc_Absyn_FnInfo _tmpBE;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_AggrInfo
_tmpC0;void*_tmpC1;struct _tuple0*_tmpC2;struct _tuple0 _tmpC3;struct _tagged_arr*
_tmpC4;struct _tuple0*_tmpC5;struct _tuple0 _tmpC6;struct _tagged_arr*_tmpC7;_LL11F:
if((int)_tmpBA != 0)goto _LL121;_LL120: goto _LL122;_LL121: if(_tmpBA <= (void*)3?1:*((
int*)_tmpBA)!= 5)goto _LL123;_LL122: goto _LL124;_LL123: if((int)_tmpBA != 1)goto
_LL125;_LL124: goto _LL126;_LL125: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 6)goto
_LL127;_LL126: return;_LL127: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 4)goto
_LL129;_tmpBB=((struct Cyc_Absyn_PointerType_struct*)_tmpBA)->f1;_LL128: Cyc_scan_type((
void*)_tmpBB.elt_typ);return;_LL129: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 7)
goto _LL12B;_tmpBC=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpBA)->f1;_tmpBD=((
struct Cyc_Absyn_ArrayType_struct*)_tmpBA)->f3;_LL12A: Cyc_scan_type(_tmpBC);Cyc_scan_exp_opt(
_tmpBD);return;_LL12B: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 8)goto _LL12D;
_tmpBE=((struct Cyc_Absyn_FnType_struct*)_tmpBA)->f1;_LL12C: Cyc_scan_type((void*)
_tmpBE.ret_typ);{struct Cyc_List_List*_tmpC8=_tmpBE.args;for(0;_tmpC8 != 0;_tmpC8=
_tmpC8->tl){struct _tuple1 _tmpCA;void*_tmpCB;struct _tuple1*_tmpC9=(struct _tuple1*)
_tmpC8->hd;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.f3;Cyc_scan_type(_tmpCB);}}if(_tmpBE.cyc_varargs
!= 0)Cyc_scan_type((void*)(_tmpBE.cyc_varargs)->type);return;_LL12D: if(_tmpBA <= (
void*)3?1:*((int*)_tmpBA)!= 11)goto _LL12F;_tmpBF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpBA)->f2;_LL12E: for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){Cyc_scan_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmpBF->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)
_tmpBF->hd)->width);}return;_LL12F: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 13)
goto _LL131;_LL130: return;_LL131: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 10)goto
_LL133;_tmpC0=((struct Cyc_Absyn_AggrType_struct*)_tmpBA)->f1;_tmpC1=(void*)
_tmpC0.aggr_info;_LL132: {struct _tuple0*_tmpCD;struct _tuple0 _tmpCE;struct
_tagged_arr*_tmpCF;struct _tuple3 _tmpCC=Cyc_Absyn_aggr_kinded_name(_tmpC1);_tmpCD=
_tmpCC.f2;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.f2;_tmpC4=_tmpCF;goto _LL134;}_LL133: if(
_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 12)goto _LL135;_tmpC2=((struct Cyc_Absyn_EnumType_struct*)
_tmpBA)->f1;_tmpC3=*_tmpC2;_tmpC4=_tmpC3.f2;_LL134: _tmpC7=_tmpC4;goto _LL136;
_LL135: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 16)goto _LL137;_tmpC5=((struct Cyc_Absyn_TypedefType_struct*)
_tmpBA)->f1;_tmpC6=*_tmpC5;_tmpC7=_tmpC6.f2;_LL136: Cyc_add_target(_tmpC7);
return;_LL137: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 0)goto _LL139;_LL138:({
void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Evar\n",
sizeof(char),24),_tag_arr(_tmpD0,sizeof(void*),0));});exit(1);return;_LL139: if(
_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 1)goto _LL13B;_LL13A:({void*_tmpD1[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected VarType\n",sizeof(
char),27),_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;_LL13B: if(_tmpBA <= (
void*)3?1:*((int*)_tmpBA)!= 2)goto _LL13D;_LL13C:({void*_tmpD2[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected TunionType\n",sizeof(char),30),
_tag_arr(_tmpD2,sizeof(void*),0));});exit(1);return;_LL13D: if(_tmpBA <= (void*)3?
1:*((int*)_tmpBA)!= 3)goto _LL13F;_LL13E:({void*_tmpD3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionFieldType\n",sizeof(char),35),_tag_arr(_tmpD3,
sizeof(void*),0));});exit(1);return;_LL13F: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)
!= 9)goto _LL141;_LL140:({void*_tmpD4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n",sizeof(char),29),_tag_arr(_tmpD4,
sizeof(void*),0));});exit(1);return;_LL141: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)
!= 15)goto _LL143;_LL142:({void*_tmpD5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnHandleType\n",sizeof(char),33),_tag_arr(_tmpD5,
sizeof(void*),0));});exit(1);return;_LL143: if((int)_tmpBA != 2)goto _LL145;_LL144:({
void*_tmpD6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(char),27),_tag_arr(_tmpD6,sizeof(void*),0));});exit(1);return;_LL145: if(
_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 19)goto _LL147;_LL146:({void*_tmpD7[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",sizeof(
char),29),_tag_arr(_tmpD7,sizeof(void*),0));});exit(1);return;_LL147: if(_tmpBA <= (
void*)3?1:*((int*)_tmpBA)!= 20)goto _LL149;_LL148:({void*_tmpD8[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",sizeof(char),27),_tag_arr(
_tmpD8,sizeof(void*),0));});exit(1);return;_LL149: if(_tmpBA <= (void*)3?1:*((int*)
_tmpBA)!= 21)goto _LL14B;_LL14A:({void*_tmpD9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnsEff\n",sizeof(char),27),_tag_arr(_tmpD9,sizeof(
void*),0));});exit(1);return;_LL14B: if(_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 14)
goto _LL14D;_LL14C:({void*_tmpDA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(char),28),_tag_arr(_tmpDA,sizeof(void*),0));});exit(1);return;_LL14D: if(
_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 18)goto _LL14F;_LL14E:({void*_tmpDB[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected type integer\n",
sizeof(char),32),_tag_arr(_tmpDB,sizeof(void*),0));});exit(1);return;_LL14F: if(
_tmpBA <= (void*)3?1:*((int*)_tmpBA)!= 17)goto _LL11E;_LL150:({void*_tmpDC[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected tag_t\n",sizeof(char),
25),_tag_arr(_tmpDC,sizeof(void*),0));});exit(1);return;_LL11E:;}struct _tuple15{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple10*Cyc_scan_decl(
struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**_tmpDD=
_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpDD;});{
void*_tmpDE=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpDF;struct Cyc_Absyn_Fndecl*
_tmpE0;struct Cyc_Absyn_Aggrdecl*_tmpE1;struct Cyc_Absyn_Enumdecl*_tmpE2;struct Cyc_Absyn_Typedefdecl*
_tmpE3;_LL152: if(*((int*)_tmpDE)!= 0)goto _LL154;_tmpDF=((struct Cyc_Absyn_Var_d_struct*)
_tmpDE)->f1;_LL153: {struct _tuple0 _tmpE5;struct _tagged_arr*_tmpE6;struct _tuple0*
_tmpE4=_tmpDF->name;_tmpE5=*_tmpE4;_tmpE6=_tmpE5.f2;Cyc_current_source=(struct
_tagged_arr*)_tmpE6;Cyc_scan_type((void*)_tmpDF->type);Cyc_scan_exp_opt(_tmpDF->initializer);
goto _LL151;}_LL154: if(*((int*)_tmpDE)!= 1)goto _LL156;_tmpE0=((struct Cyc_Absyn_Fn_d_struct*)
_tmpDE)->f1;_LL155: {struct _tuple0 _tmpE8;struct _tagged_arr*_tmpE9;struct _tuple0*
_tmpE7=_tmpE0->name;_tmpE8=*_tmpE7;_tmpE9=_tmpE8.f2;Cyc_current_source=(struct
_tagged_arr*)_tmpE9;Cyc_scan_type((void*)_tmpE0->ret_type);{struct Cyc_List_List*
_tmpEA=_tmpE0->args;for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){struct _tuple15 _tmpEC;
void*_tmpED;struct _tuple15*_tmpEB=(struct _tuple15*)_tmpEA->hd;_tmpEC=*_tmpEB;
_tmpED=_tmpEC.f3;Cyc_scan_type(_tmpED);}}if(_tmpE0->cyc_varargs != 0)Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpE0->cyc_varargs))->type);if(
_tmpE0->is_inline)({void*_tmpEE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: ignoring inline function\n",
sizeof(char),35),_tag_arr(_tmpEE,sizeof(void*),0));});goto _LL151;}_LL156: if(*((
int*)_tmpDE)!= 4)goto _LL158;_tmpE1=((struct Cyc_Absyn_Aggr_d_struct*)_tmpDE)->f1;
_LL157: {struct _tuple0 _tmpF0;struct _tagged_arr*_tmpF1;struct _tuple0*_tmpEF=
_tmpE1->name;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.f2;Cyc_current_source=(struct
_tagged_arr*)_tmpF1;if((unsigned int)_tmpE1->impl){{struct Cyc_List_List*_tmpF2=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE1->impl))->fields;for(0;_tmpF2 != 
0;_tmpF2=_tmpF2->tl){struct Cyc_Absyn_Aggrfield*_tmpF3=(struct Cyc_Absyn_Aggrfield*)
_tmpF2->hd;Cyc_scan_type((void*)_tmpF3->type);Cyc_scan_exp_opt(_tmpF3->width);}}{
struct Cyc_List_List*_tmpF4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE1->impl))->fields;
for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){;}}}goto _LL151;}_LL158: if(*((int*)_tmpDE)!= 
6)goto _LL15A;_tmpE2=((struct Cyc_Absyn_Enum_d_struct*)_tmpDE)->f1;_LL159: {struct
_tuple0 _tmpF6;struct _tagged_arr*_tmpF7;struct _tuple0*_tmpF5=_tmpE2->name;_tmpF6=*
_tmpF5;_tmpF7=_tmpF6.f2;Cyc_current_source=(struct _tagged_arr*)_tmpF7;if((
unsigned int)_tmpE2->fields){{struct Cyc_List_List*_tmpF8=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpE2->fields))->v;for(0;_tmpF8 != 0;_tmpF8=
_tmpF8->tl){struct Cyc_Absyn_Enumfield*_tmpF9=(struct Cyc_Absyn_Enumfield*)_tmpF8->hd;
Cyc_scan_exp_opt(_tmpF9->tag);}}{struct Cyc_List_List*_tmpFA=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpE2->fields))->v;for(0;_tmpFA != 0;_tmpFA=
_tmpFA->tl){;}}}goto _LL151;}_LL15A: if(*((int*)_tmpDE)!= 7)goto _LL15C;_tmpE3=((
struct Cyc_Absyn_Typedef_d_struct*)_tmpDE)->f1;_LL15B: {struct _tuple0 _tmpFC;
struct _tagged_arr*_tmpFD;struct _tuple0*_tmpFB=_tmpE3->name;_tmpFC=*_tmpFB;_tmpFD=
_tmpFC.f2;Cyc_current_source=(struct _tagged_arr*)_tmpFD;if((unsigned int)_tmpE3->defn)
Cyc_scan_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE3->defn))->v);goto
_LL151;}_LL15C: if(*((int*)_tmpDE)!= 2)goto _LL15E;_LL15D:({void*_tmpFE[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected let declaration\n",sizeof(char),35),
_tag_arr(_tmpFE,sizeof(void*),0));});exit(1);return 0;_LL15E: if(*((int*)_tmpDE)!= 
5)goto _LL160;_LL15F:({void*_tmpFF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected tunion declaration\n",
sizeof(char),38),_tag_arr(_tmpFF,sizeof(void*),0));});exit(1);return 0;_LL160: if(*((
int*)_tmpDE)!= 3)goto _LL162;_LL161:({void*_tmp100[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(char),35),_tag_arr(_tmp100,
sizeof(void*),0));});exit(1);return 0;_LL162: if(*((int*)_tmpDE)!= 8)goto _LL164;
_LL163:({void*_tmp101[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected namespace declaration\n",
sizeof(char),41),_tag_arr(_tmp101,sizeof(void*),0));});exit(1);return 0;_LL164:
if(*((int*)_tmpDE)!= 9)goto _LL166;_LL165:({void*_tmp102[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected using declaration\n",sizeof(char),37),
_tag_arr(_tmp102,sizeof(void*),0));});exit(1);return 0;_LL166: if(*((int*)_tmpDE)
!= 10)goto _LL151;_LL167:({void*_tmp103[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n",sizeof(char),42),
_tag_arr(_tmp103,sizeof(void*),0));});exit(1);return 0;_LL151:;}return({struct
_tuple10*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp104->f2=*((struct Cyc_Set_Set**)_check_null(
Cyc_current_targets));_tmp104;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp105;_push_handler(& _tmp105);{int
_tmp107=0;if(setjmp(_tmp105.handler))_tmp107=1;if(!_tmp107){{struct Cyc_Set_Set*
_tmp108=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp108;};_pop_handler();}
else{void*_tmp106=(void*)_exn_thrown;void*_tmp10A=_tmp106;_LL169: if(_tmp10A != 
Cyc_Core_Not_found)goto _LL16B;_LL16A: return((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL16B:;
_LL16C:(void)_throw(_tmp10A);_LL168:;}}}struct Cyc_Set_Set*Cyc_reachable(struct
Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp10B=curr;struct
_tagged_arr*_tmp10C=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp10B)
> 0){struct Cyc_Set_Set*_tmp10D=emptyset;struct Cyc_Iter_Iter _tmp10E=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp10B);while(((int(*)(struct Cyc_Iter_Iter,struct _tagged_arr**))Cyc_Iter_next)(
_tmp10E,& _tmp10C)){_tmp10D=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp10D,Cyc_find(t,_tmp10C));}_tmp10B=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp10D,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp10B);}return curr;}}static void*Cyc_mode=(void*)0;
static int Cyc_gathering(){return Cyc_mode == (void*)1?1: Cyc_mode == (void*)2;}static
struct Cyc_Std___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_script_file == 0){({void*_tmp110[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Internal error: script file is NULL\n",sizeof(char),37),
_tag_arr(_tmp110,sizeof(void*),0));});exit(1);}return Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file),fmt,ap);}int Cyc_force_directory(struct _tagged_arr d){
if(Cyc_mode == (void*)2)({struct Cyc_Std_String_pa_struct _tmp113;_tmp113.tag=0;
_tmp113.f1=(struct _tagged_arr)d;{struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=
0;_tmp112.f1=(struct _tagged_arr)d;{void*_tmp111[2]={& _tmp112,& _tmp113};Cyc_prscript(
_tag_arr("if ! test -e %s; then mkdir %s; fi\n",sizeof(char),36),_tag_arr(
_tmp111,sizeof(void*),2));}}});else{int _tmp114=({unsigned int _tmp117[0];Cyc_Std_open(
d,0,_tag_arr(_tmp117,sizeof(unsigned int),0));});if(_tmp114 == - 1){if(Cyc_Std_mkdir(
d,448)== - 1){({struct Cyc_Std_String_pa_struct _tmp116;_tmp116.tag=0;_tmp116.f1=(
struct _tagged_arr)d;{void*_tmp115[1]={& _tmp116};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp115,sizeof(void*),1));}});return 1;}}else{close(_tmp114);}}return 0;}int Cyc_force_directory_prefixes(
struct _tagged_arr file){struct _tagged_arr _tmp118=Cyc_Std_strdup(file);struct Cyc_List_List*
_tmp119=0;while(1){_tmp118=Cyc_Filename_dirname((struct _tagged_arr)_tmp118);if(
Cyc_Std_strlen((struct _tagged_arr)_tmp118)== 0)break;_tmp119=({struct Cyc_List_List*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->hd=({struct _tagged_arr*_tmp11B=
_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=(struct _tagged_arr)_tmp118;_tmp11B;});
_tmp11A->tl=_tmp119;_tmp11A;});}for(0;_tmp119 != 0;_tmp119=_tmp119->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)_tmp119->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT";
struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(
struct _tagged_arr filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*
omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*epilogue,struct Cyc_List_List*
cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*maybe;struct Cyc_Std___cycFILE*
in_file;struct Cyc_Std___cycFILE*out_file;int errorcode=0;({struct Cyc_Std_String_pa_struct
_tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _tagged_arr)filename;{void*_tmp11C[1]={&
_tmp11D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("********************************* %s...\n",
sizeof(char),41),_tag_arr(_tmp11C,sizeof(void*),1));}});if(!Cyc_gathering())({
struct Cyc_Std_String_pa_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct
_tagged_arr)filename;{void*_tmp11E[1]={& _tmp11F};Cyc_log(_tag_arr("\n%s:\n",
sizeof(char),6),_tag_arr(_tmp11E,sizeof(void*),1));}});{struct _tagged_arr _tmp120=
Cyc_Filename_basename(filename);struct _tagged_arr _tmp121=Cyc_Filename_dirname(
filename);struct _tagged_arr _tmp122=Cyc_Filename_chop_extension((struct
_tagged_arr)_tmp120);struct _tagged_arr _tmp123=Cyc_Std_strconcat((struct
_tagged_arr)_tmp122,_tag_arr(".iA",sizeof(char),4));struct _tagged_arr _tmp124=
_get_arr_size(_tmp121,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp1FF;
_tmp1FF.tag=0;_tmp1FF.f1=(struct _tagged_arr)_tmp122;{void*_tmp1FE[1]={& _tmp1FF};
Cyc_Std_aprintf(_tag_arr("%s.iB",sizeof(char),6),_tag_arr(_tmp1FE,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp121,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(struct
_tagged_arr)_tmp122;{void*_tmp200[1]={& _tmp201};Cyc_Std_aprintf(_tag_arr("%s.iB",
sizeof(char),6),_tag_arr(_tmp200,sizeof(void*),1));}}));struct _tagged_arr _tmp125=
_get_arr_size(_tmp121,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp1FB;
_tmp1FB.tag=0;_tmp1FB.f1=(struct _tagged_arr)_tmp122;{void*_tmp1FA[1]={& _tmp1FB};
Cyc_Std_aprintf(_tag_arr("%s.iC",sizeof(char),6),_tag_arr(_tmp1FA,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp121,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=(struct
_tagged_arr)_tmp122;{void*_tmp1FC[1]={& _tmp1FD};Cyc_Std_aprintf(_tag_arr("%s.iC",
sizeof(char),6),_tag_arr(_tmp1FC,sizeof(void*),1));}}));struct _tagged_arr _tmp126=
_get_arr_size(_tmp121,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp1F7;
_tmp1F7.tag=0;_tmp1F7.f1=(struct _tagged_arr)_tmp122;{void*_tmp1F6[1]={& _tmp1F7};
Cyc_Std_aprintf(_tag_arr("%s.iD",sizeof(char),6),_tag_arr(_tmp1F6,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp121,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1F9;_tmp1F9.tag=0;_tmp1F9.f1=(struct
_tagged_arr)_tmp122;{void*_tmp1F8[1]={& _tmp1F9};Cyc_Std_aprintf(_tag_arr("%s.iD",
sizeof(char),6),_tag_arr(_tmp1F8,sizeof(void*),1));}}));struct _tagged_arr _tmp127=
_get_arr_size(_tmp121,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp1F3;
_tmp1F3.tag=0;_tmp1F3.f1=(struct _tagged_arr)_tmp120;{void*_tmp1F2[1]={& _tmp1F3};
Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp1F2,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp121,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _tagged_arr)_tmp120;{void*_tmp1F4[1]={&
_tmp1F5};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp1F4,sizeof(
void*),1));}}));struct _handler_cons _tmp128;_push_handler(& _tmp128);{int _tmp12A=0;
if(setjmp(_tmp128.handler))_tmp12A=1;if(!_tmp12A){if(Cyc_force_directory_prefixes(
filename)){int _tmp12B=1;_npop_handler(0);return _tmp12B;}if(Cyc_mode != (void*)3){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct _tmp12D;
_tmp12D.tag=0;_tmp12D.f1=(struct _tagged_arr)_tmp123;{void*_tmp12C[1]={& _tmp12D};
Cyc_prscript(_tag_arr("cat >%s <<XXX\n",sizeof(char),15),_tag_arr(_tmp12C,
sizeof(void*),1));}});{struct Cyc_List_List*_tmp12E=Cyc_current_cpp;for(0;_tmp12E
!= 0;_tmp12E=_tmp12E->tl){({struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp12E->hd);{void*_tmp12F[1]={&
_tmp130};Cyc_prscript(_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp12F,sizeof(void*),
1));}});}}({struct Cyc_Std_String_pa_struct _tmp132;_tmp132.tag=0;_tmp132.f1=(
struct _tagged_arr)filename;{void*_tmp131[1]={& _tmp132};Cyc_prscript(_tag_arr("#include <%s>\n",
sizeof(char),15),_tag_arr(_tmp131,sizeof(void*),1));}});({void*_tmp133[0]={};Cyc_prscript(
_tag_arr("XXX\n",sizeof(char),5),_tag_arr(_tmp133,sizeof(void*),0));});({struct
Cyc_Std_String_pa_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _tagged_arr)
_tmp123;{struct Cyc_Std_String_pa_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(struct
_tagged_arr)_tmp124;{void*_tmp134[2]={& _tmp135,& _tmp136};Cyc_prscript(_tag_arr("$GCC -E -dM -o %s -x c %s && \\\n",
sizeof(char),32),_tag_arr(_tmp134,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp139;_tmp139.tag=0;_tmp139.f1=(struct _tagged_arr)_tmp123;{struct Cyc_Std_String_pa_struct
_tmp138;_tmp138.tag=0;_tmp138.f1=(struct _tagged_arr)_tmp125;{void*_tmp137[2]={&
_tmp138,& _tmp139};Cyc_prscript(_tag_arr("$GCC -E     -o %s -x c %s;\n",sizeof(
char),28),_tag_arr(_tmp137,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _tagged_arr)_tmp123;{void*_tmp13A[1]={&
_tmp13B};Cyc_prscript(_tag_arr("rm %s\n",sizeof(char),7),_tag_arr(_tmp13A,
sizeof(void*),1));}});}else{maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp123,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _tagged_arr)_tmp123;{void*_tmp13C[1]={&
_tmp13D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp13C,sizeof(void*),1));}});{int _tmp13E=1;
_npop_handler(0);return _tmp13E;}}out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);{struct Cyc_List_List*_tmp13F=Cyc_current_cpp;for(0;_tmp13F != 0;_tmp13F=
_tmp13F->tl){Cyc_Std_fputs(*((struct _tagged_arr*)_tmp13F->hd),out_file);}}({
struct Cyc_Std_String_pa_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(struct
_tagged_arr)filename;{void*_tmp140[1]={& _tmp141};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n",sizeof(char),15),_tag_arr(_tmp140,sizeof(void*),1));}});
Cyc_Std_fclose(out_file);{struct _tagged_arr _tmp142=Cstring_to_string(Ccomp);
struct _tagged_arr _tmp143=({struct Cyc_Std_String_pa_struct _tmp14B;_tmp14B.tag=0;
_tmp14B.f1=(struct _tagged_arr)_tmp123;{struct Cyc_Std_String_pa_struct _tmp14A;
_tmp14A.tag=0;_tmp14A.f1=(struct _tagged_arr)_tmp124;{struct Cyc_Std_String_pa_struct
_tmp149;_tmp149.tag=0;_tmp149.f1=(struct _tagged_arr)_tmp142;{void*_tmp148[3]={&
_tmp149,& _tmp14A,& _tmp14B};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(char),24),_tag_arr(_tmp148,sizeof(void*),3));}}}});if(!Cyc_Std_system((
struct _tagged_arr)_tmp143)){_tmp143=({struct Cyc_Std_String_pa_struct _tmp147;
_tmp147.tag=0;_tmp147.f1=(struct _tagged_arr)_tmp123;{struct Cyc_Std_String_pa_struct
_tmp146;_tmp146.tag=0;_tmp146.f1=(struct _tagged_arr)_tmp125;{struct Cyc_Std_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _tagged_arr)_tmp142;{void*_tmp144[3]={&
_tmp145,& _tmp146,& _tmp147};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",
sizeof(char),20),_tag_arr(_tmp144,sizeof(void*),3));}}}});Cyc_Std_system((struct
_tagged_arr)_tmp143);}Cyc_Std_remove((struct _tagged_arr)_tmp123);}}}if(Cyc_gathering()){
int _tmp14C=0;_npop_handler(0);return _tmp14C;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp124,_tag_arr("r",sizeof(char),2));if(
!((unsigned int)maybe))(int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);{struct Cyc_Lexing_lexbuf*_tmp14D=Cyc_Lexing_from_file(in_file);
struct _tuple10*entry;while((entry=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp14D))!= 0){struct _tuple10 _tmp14F;struct _tagged_arr*_tmp150;
struct Cyc_Set_Set*_tmp151;struct _tuple10*_tmp14E=(struct _tuple10*)_check_null(
entry);_tmp14F=*_tmp14E;_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;((void(*)(struct
Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp150,_tmp151);}Cyc_Std_fclose(in_file);maybe=Cyc_Std_fopen((struct
_tagged_arr)_tmp125,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(int)
_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp14D=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_Std_fopen((struct
_tagged_arr)_tmp126,_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_slurp_out)){
int _tmp152=1;_npop_handler(0);return _tmp152;}while(((int(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_slurp)(_tmp14D)){;}Cyc_Std_fclose(in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)Cyc_Std_remove((struct
_tagged_arr)_tmp125);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp126,_tag_arr("r",
sizeof(char),2));if(!((unsigned int)maybe)){int _tmp153=1;_npop_handler(0);return
_tmp153;}in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_Position_reset_position((
struct _tagged_arr)_tmp126);Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp154=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp126);{struct Cyc_List_List*_tmp155=_tmp154;for(0;_tmp155 != 0;
_tmp155=_tmp155->tl){struct _tuple10*_tmp156=Cyc_scan_decl((struct Cyc_Absyn_Decl*)
_tmp155->hd);if(_tmp156 == 0)continue;{struct _tuple10 _tmp158;struct _tagged_arr*
_tmp159;struct Cyc_Set_Set*_tmp15A;struct _tuple10*_tmp157=(struct _tuple10*)
_check_null(_tmp156);_tmp158=*_tmp157;_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;{
struct Cyc_Set_Set*old;{struct _handler_cons _tmp15B;_push_handler(& _tmp15B);{int
_tmp15D=0;if(setjmp(_tmp15B.handler))_tmp15D=1;if(!_tmp15D){old=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,
_tmp159);;_pop_handler();}else{void*_tmp15C=(void*)_exn_thrown;void*_tmp15F=
_tmp15C;_LL16E: if(_tmp15F != Cyc_Core_Not_found)goto _LL170;_LL16F: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL16D;_LL170:;_LL171:(void)_throw(_tmp15F);_LL16D:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp159,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp15A,old));}}}}{struct Cyc_Set_Set*_tmp160=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp161=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp162=_tmp154;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct Cyc_Absyn_Decl*_tmp163=(struct Cyc_Absyn_Decl*)_tmp162->hd;struct
_tagged_arr*name;{void*_tmp164=(void*)_tmp163->r;struct Cyc_Absyn_Vardecl*_tmp165;
struct Cyc_Absyn_Fndecl*_tmp166;struct Cyc_Absyn_Aggrdecl*_tmp167;struct Cyc_Absyn_Enumdecl*
_tmp168;struct Cyc_Absyn_Typedefdecl*_tmp169;_LL173: if(*((int*)_tmp164)!= 0)goto
_LL175;_tmp165=((struct Cyc_Absyn_Var_d_struct*)_tmp164)->f1;_LL174: {struct
_tuple0 _tmp16B;struct _tagged_arr*_tmp16C;struct _tuple0*_tmp16A=_tmp165->name;
_tmp16B=*_tmp16A;_tmp16C=_tmp16B.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp16C);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp16C))name=0;else{name=(struct _tagged_arr*)_tmp16C;}goto _LL172;}
_LL175: if(*((int*)_tmp164)!= 1)goto _LL177;_tmp166=((struct Cyc_Absyn_Fn_d_struct*)
_tmp164)->f1;_LL176: {struct _tuple0 _tmp16E;struct _tagged_arr*_tmp16F;struct
_tuple0*_tmp16D=_tmp166->name;_tmp16E=*_tmp16D;_tmp16F=_tmp16E.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp16F);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp16F))name=0;else{name=(struct _tagged_arr*)
_tmp16F;}goto _LL172;}_LL177: if(*((int*)_tmp164)!= 4)goto _LL179;_tmp167=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp164)->f1;_LL178: {struct _tuple0 _tmp171;struct
_tagged_arr*_tmp172;struct _tuple0*_tmp170=_tmp167->name;_tmp171=*_tmp170;_tmp172=
_tmp171.f2;name=(struct _tagged_arr*)_tmp172;goto _LL172;}_LL179: if(*((int*)
_tmp164)!= 6)goto _LL17B;_tmp168=((struct Cyc_Absyn_Enum_d_struct*)_tmp164)->f1;
_LL17A: {struct _tuple0 _tmp174;struct _tagged_arr*_tmp175;struct _tuple0*_tmp173=
_tmp168->name;_tmp174=*_tmp173;_tmp175=_tmp174.f2;name=(struct _tagged_arr*)
_tmp175;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp160,(struct _tagged_arr*)_check_null(name)): 0)_tmp161=({struct Cyc_List_List*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=_tmp163;_tmp176->tl=_tmp161;
_tmp176;});else{if((unsigned int)_tmp168->fields){struct Cyc_List_List*_tmp177=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp168->fields))->v;for(
0;_tmp177 != 0;_tmp177=_tmp177->tl){struct Cyc_Absyn_Enumfield*_tmp178=(struct Cyc_Absyn_Enumfield*)
_tmp177->hd;struct _tuple0 _tmp17A;struct _tagged_arr*_tmp17B;struct _tuple0*_tmp179=
_tmp178->name;_tmp17A=*_tmp179;_tmp17B=_tmp17A.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp160,_tmp17B)){_tmp161=({struct Cyc_List_List*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=_tmp163;_tmp17C->tl=_tmp161;
_tmp17C;});break;}}}}name=0;goto _LL172;}_LL17B: if(*((int*)_tmp164)!= 7)goto
_LL17D;_tmp169=((struct Cyc_Absyn_Typedef_d_struct*)_tmp164)->f1;_LL17C: {struct
_tuple0 _tmp17E;struct _tagged_arr*_tmp17F;struct _tuple0*_tmp17D=_tmp169->name;
_tmp17E=*_tmp17D;_tmp17F=_tmp17E.f2;name=(struct _tagged_arr*)_tmp17F;goto _LL172;}
_LL17D: if(*((int*)_tmp164)!= 2)goto _LL17F;_LL17E: goto _LL180;_LL17F: if(*((int*)
_tmp164)!= 5)goto _LL181;_LL180: goto _LL182;_LL181: if(*((int*)_tmp164)!= 3)goto
_LL183;_LL182: goto _LL184;_LL183: if(*((int*)_tmp164)!= 8)goto _LL185;_LL184: goto
_LL186;_LL185: if(*((int*)_tmp164)!= 9)goto _LL187;_LL186: goto _LL188;_LL187: if(*((
int*)_tmp164)!= 10)goto _LL172;_LL188: name=0;goto _LL172;_LL172:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp160,(
struct _tagged_arr*)_check_null(name)): 0)_tmp161=({struct Cyc_List_List*_tmp180=
_cycalloc(sizeof(*_tmp180));_tmp180->hd=_tmp163;_tmp180->tl=_tmp161;_tmp180;});}}
maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(char),2));if(!((unsigned int)
maybe)){int _tmp181=1;_npop_handler(0);return _tmp181;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(
struct _tagged_arr)_tmp127;{void*_tmp182[1]={& _tmp183};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\nusing Std;\n",sizeof(char),26),_tag_arr(_tmp182,sizeof(
void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp127,_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){int _tmp184=1;
_npop_handler(0);return _tmp184;}out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);{struct _tagged_arr ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1E4;
_tmp1E4.tag=0;_tmp1E4.f1=(struct _tagged_arr)filename;{void*_tmp1E3[1]={& _tmp1E4};
Cyc_Std_aprintf(_tag_arr("_%s_",sizeof(char),5),_tag_arr(_tmp1E3,sizeof(void*),1));}});{
int _tmp185=0;for(0;_tmp185 < _get_arr_size(ifdefmacro,sizeof(char));_tmp185 ++){
if(((char*)ifdefmacro.curr)[_tmp185]== '.'?1:((char*)ifdefmacro.curr)[_tmp185]== '/')((
char*)ifdefmacro.curr)[_tmp185]='_';else{if(((char*)ifdefmacro.curr)[_tmp185]!= '_'?((
char*)ifdefmacro.curr)[_tmp185]!= '/': 0)((char*)ifdefmacro.curr)[_tmp185]=(char)
toupper((int)((char*)ifdefmacro.curr)[_tmp185]);}}}({struct Cyc_Std_String_pa_struct
_tmp188;_tmp188.tag=0;_tmp188.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct
_tmp187;_tmp187.tag=0;_tmp187.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp186[2]={&
_tmp187,& _tmp188};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",
sizeof(char),23),_tag_arr(_tmp186,sizeof(void*),2));}}});if(prologue != 0){struct
Cyc_List_List*_tmp189=prologue;for(0;_tmp189 != 0;_tmp189=_tmp189->tl){struct
_tuple12 _tmp18B;struct _tagged_arr _tmp18C;struct _tagged_arr _tmp18D;struct _tuple12*
_tmp18A=(struct _tuple12*)_tmp189->hd;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f1;_tmp18D=
_tmp18B.f2;if(_tmp18D.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp18C.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=_tmp18C;_tmp18E;})): 0)Cyc_Std_fputs(
_tmp18D,out_file);else{({struct Cyc_Std_String_pa_struct _tmp190;_tmp190.tag=0;
_tmp190.f1=(struct _tagged_arr)_tmp18C;{void*_tmp18F[1]={& _tmp190};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp18F,sizeof(void*),1));}});}}}{struct Cyc_List_List*_tmp191=0;struct Cyc_List_List*
_tmp192=0;{struct Cyc_List_List*_tmp193=_tmp161;for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
struct Cyc_Absyn_Decl*_tmp194=(struct Cyc_Absyn_Decl*)_tmp193->hd;int _tmp195=0;
struct _tagged_arr*name;{void*_tmp196=(void*)_tmp194->r;struct Cyc_Absyn_Vardecl*
_tmp197;struct Cyc_Absyn_Fndecl*_tmp198;struct Cyc_Absyn_Aggrdecl*_tmp199;struct
Cyc_Absyn_Enumdecl*_tmp19A;struct Cyc_Absyn_Typedefdecl*_tmp19B;_LL18A: if(*((int*)
_tmp196)!= 0)goto _LL18C;_tmp197=((struct Cyc_Absyn_Var_d_struct*)_tmp196)->f1;
_LL18B: {struct _tuple0 _tmp19D;struct _tagged_arr*_tmp19E;struct _tuple0*_tmp19C=
_tmp197->name;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f2;name=(struct _tagged_arr*)
_tmp19E;goto _LL189;}_LL18C: if(*((int*)_tmp196)!= 1)goto _LL18E;_tmp198=((struct
Cyc_Absyn_Fn_d_struct*)_tmp196)->f1;_LL18D: {struct _tuple0 _tmp1A0;struct
_tagged_arr*_tmp1A1;struct _tuple0*_tmp19F=_tmp198->name;_tmp1A0=*_tmp19F;_tmp1A1=
_tmp1A0.f2;name=(struct _tagged_arr*)_tmp1A1;goto _LL189;}_LL18E: if(*((int*)
_tmp196)!= 4)goto _LL190;_tmp199=((struct Cyc_Absyn_Aggr_d_struct*)_tmp196)->f1;
_LL18F: {struct _tuple0 _tmp1A3;struct _tagged_arr*_tmp1A4;struct _tuple0*_tmp1A2=
_tmp199->name;_tmp1A3=*_tmp1A2;_tmp1A4=_tmp1A3.f2;name=(struct _tagged_arr*)
_tmp1A4;goto _LL189;}_LL190: if(*((int*)_tmp196)!= 6)goto _LL192;_tmp19A=((struct
Cyc_Absyn_Enum_d_struct*)_tmp196)->f1;_LL191: {struct _tuple0 _tmp1A6;struct
_tagged_arr*_tmp1A7;struct _tuple0*_tmp1A5=_tmp19A->name;_tmp1A6=*_tmp1A5;_tmp1A7=
_tmp1A6.f2;name=(struct _tagged_arr*)_tmp1A7;goto _LL189;}_LL192: if(*((int*)
_tmp196)!= 7)goto _LL194;_tmp19B=((struct Cyc_Absyn_Typedef_d_struct*)_tmp196)->f1;
_LL193: {struct _tuple0 _tmp1A9;struct _tagged_arr*_tmp1AA;struct _tuple0*_tmp1A8=
_tmp19B->name;_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.f2;name=(struct _tagged_arr*)
_tmp1AA;goto _LL189;}_LL194: if(*((int*)_tmp196)!= 2)goto _LL196;_LL195: goto _LL197;
_LL196: if(*((int*)_tmp196)!= 5)goto _LL198;_LL197: goto _LL199;_LL198: if(*((int*)
_tmp196)!= 3)goto _LL19A;_LL199: goto _LL19B;_LL19A: if(*((int*)_tmp196)!= 8)goto
_LL19C;_LL19B: goto _LL19D;_LL19C: if(*((int*)_tmp196)!= 9)goto _LL19E;_LL19D: goto
_LL19F;_LL19E: if(*((int*)_tmp196)!= 10)goto _LL189;_LL19F: name=0;goto _LL189;
_LL189:;}if(!((unsigned int)name)?!_tmp195: 0)continue;_tmp191=({struct Cyc_List_List*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=_tmp194;_tmp1AB->tl=_tmp191;
_tmp1AB;});_tmp192=({struct Cyc_List_List*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));
_tmp1AC->hd=name;_tmp1AC->tl=_tmp192;_tmp1AC;});}}{struct Cyc_Tcenv_Tenv*_tmp1AD=
Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp1AD,1,_tmp191);({void*_tmp1AE[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("\nnamespace Std {\n",sizeof(char),18),_tag_arr(_tmp1AE,
sizeof(void*),0));});{struct Cyc_List_List*_tmp1B1;struct Cyc_List_List*_tmp1B2;
struct _tuple16 _tmp1B0=({struct _tuple16 _tmp1AF;_tmp1AF.f1=_tmp191;_tmp1AF.f2=
_tmp192;_tmp1AF;});_tmp1B1=_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;for(0;_tmp1B1 != 0?
_tmp1B2 != 0: 0;(_tmp1B1=_tmp1B1->tl,_tmp1B2=_tmp1B2->tl)){struct Cyc_Absyn_Decl*
_tmp1B3=(struct Cyc_Absyn_Decl*)_tmp1B1->hd;struct _tagged_arr*_tmp1B4=(struct
_tagged_arr*)_tmp1B2->hd;int _tmp1B5=0;if(!((unsigned int)_tmp1B4))_tmp1B5=1;if((
unsigned int)_tmp1B4){ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1B7;_tmp1B7.tag=
0;_tmp1B7.f1=(struct _tagged_arr)*_tmp1B4;{void*_tmp1B6[1]={& _tmp1B7};Cyc_Std_aprintf(
_tag_arr("_%s_def_",sizeof(char),9),_tag_arr(_tmp1B6,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1B8[1]={& _tmp1B9};Cyc_Std_fprintf(out_file,
_tag_arr("#ifndef %s\n",sizeof(char),12),_tag_arr(_tmp1B8,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1BA[1]={& _tmp1BB};Cyc_Std_fprintf(out_file,
_tag_arr("#define %s\n",sizeof(char),12),_tag_arr(_tmp1BA,sizeof(void*),1));}});
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BC[1];_tmp1BC[0]=_tmp1B3;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1BC,
sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1BD[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("#endif\n",sizeof(char),8),_tag_arr(_tmp1BD,sizeof(void*),0));});}
else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BE[1];_tmp1BE[0]=
_tmp1B3;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1BE,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp124,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(
int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp14D=Cyc_Lexing_from_file(in_file);{struct _tuple11*entry2;while((
entry2=((struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(
_tmp14D))!= 0){struct _tuple11 _tmp1C0;struct _tagged_arr _tmp1C1;struct _tagged_arr*
_tmp1C2;struct _tuple11*_tmp1BF=(struct _tuple11*)_check_null(entry2);_tmp1C0=*
_tmp1BF;_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;if(((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(_tmp160,_tmp1C2)){({struct Cyc_Std_String_pa_struct
_tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(struct _tagged_arr)*_tmp1C2;{void*_tmp1C3[1]={&
_tmp1C4};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(char),12),
_tag_arr(_tmp1C3,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1C6;
_tmp1C6.tag=0;_tmp1C6.f1=(struct _tagged_arr)_tmp1C1;{void*_tmp1C5[1]={& _tmp1C6};
Cyc_Std_fprintf(out_file,_tag_arr("%s\n",sizeof(char),4),_tag_arr(_tmp1C5,
sizeof(void*),1));}});({void*_tmp1C7[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",
sizeof(char),8),_tag_arr(_tmp1C7,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);
if(Cyc_mode != (void*)3)Cyc_Std_remove((struct _tagged_arr)_tmp124);if(epilogue != 
0){struct Cyc_List_List*_tmp1C8=epilogue;for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct _tuple12 _tmp1CA;struct _tagged_arr _tmp1CB;struct _tagged_arr _tmp1CC;struct
_tuple12*_tmp1C9=(struct _tuple12*)_tmp1C8->hd;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f1;
_tmp1CC=_tmp1CA.f2;if(_tmp1CC.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp1CB.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=_tmp1CB;_tmp1CD;})): 0)Cyc_Std_fputs(
_tmp1CC,out_file);else{({struct Cyc_Std_String_pa_struct _tmp1CF;_tmp1CF.tag=0;
_tmp1CF.f1=(struct _tagged_arr)_tmp1CB;{void*_tmp1CE[1]={& _tmp1CF};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp1CE,sizeof(void*),1));}});}}}({void*_tmp1D0[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("}\n",sizeof(char),3),_tag_arr(_tmp1D0,sizeof(void*),0));});({void*
_tmp1D1[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(char),8),
_tag_arr(_tmp1D1,sizeof(void*),0));});Cyc_Std_fclose(out_file);if(cstubs != 0){
out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file);{struct Cyc_List_List*
_tmp1D2=cstubs;for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){struct _tuple12 _tmp1D4;
struct _tagged_arr _tmp1D5;struct _tagged_arr _tmp1D6;struct _tuple12*_tmp1D3=(struct
_tuple12*)_tmp1D2->hd;_tmp1D4=*_tmp1D3;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;if(
_tmp1D6.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1D5.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1D7=
_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=_tmp1D5;_tmp1D7;})): 0)Cyc_Std_fputs(
_tmp1D6,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _tmp1D9;_tmp1D9.tag=0;_tmp1D9.f1=(struct
_tagged_arr)_tmp127;{void*_tmp1D8[1]={& _tmp1D9};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n\n",sizeof(char),16),_tag_arr(_tmp1D8,sizeof(void*),1));}});
if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
void*_tmp1DA[0]={};Cyc_Std_fprintf(out_file,_tag_arr("namespace Std {\n",sizeof(
char),17),_tag_arr(_tmp1DA,sizeof(void*),0));});{struct Cyc_List_List*_tmp1DB=
cycstubs;for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){struct _tuple12 _tmp1DD;struct
_tagged_arr _tmp1DE;struct _tagged_arr _tmp1DF;struct _tuple12*_tmp1DC=(struct
_tuple12*)_tmp1DB->hd;_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;if(
_tmp1DF.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1DE.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1E0=
_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=_tmp1DE;_tmp1E0;})): 0)Cyc_Std_fputs(
_tmp1DF,out_file);}}({void*_tmp1E1[0]={};Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",
sizeof(char),4),_tag_arr(_tmp1E1,sizeof(void*),0));});}{int _tmp1E2=0;
_npop_handler(0);return _tmp1E2;}}}}}}}}};_pop_handler();}else{void*_tmp129=(void*)
_exn_thrown;void*_tmp1E6=_tmp129;_LL1A1:;_LL1A2: maybe=Cyc_Std_fopen(filename,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct _tagged_arr)filename;{void*_tmp1E7[1]={&
_tmp1E8};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp1E7,sizeof(void*),1));}});return 1;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _tagged_arr)filename;{void*_tmp1E9[1]={&
_tmp1EA};Cyc_Std_fprintf(out_file,_tag_arr("#error -- %s is not supported on this platform\n",
sizeof(char),48),_tag_arr(_tmp1E9,sizeof(void*),1));}});Cyc_Std_fclose(out_file);
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp127,_tag_arr("w",sizeof(char),2));if(
!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp1EC;_tmp1EC.tag=0;
_tmp1EC.f1=(struct _tagged_arr)_tmp127;{void*_tmp1EB[1]={& _tmp1EC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(char),33),
_tag_arr(_tmp1EB,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(
struct _tagged_arr)_tmp127;{void*_tmp1ED[1]={& _tmp1EE};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(char),48),
_tag_arr(_tmp1ED,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=(struct _tagged_arr)filename;{void*_tmp1EF[1]={&
_tmp1F0};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(char),52),_tag_arr(_tmp1EF,sizeof(void*),1));}});({void*_tmp1F1[0]={};Cyc_log(
_tag_arr("Not supported on this platform\n",sizeof(char),32),_tag_arr(_tmp1F1,
sizeof(void*),0));});Cyc_Std_remove((struct _tagged_arr)_tmp124);Cyc_Std_remove((
struct _tagged_arr)_tmp125);Cyc_Std_remove((struct _tagged_arr)_tmp126);return 0;
_LL1A3:;_LL1A4:(void)_throw(_tmp1E6);_LL1A0:;}}}}int Cyc_process_specfile(struct
_tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp202=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(char),2));if(!((unsigned int)_tmp202)){({struct Cyc_Std_String_pa_struct
_tmp204;_tmp204.tag=0;_tmp204.f1=(struct _tagged_arr)file;{void*_tmp203[1]={&
_tmp204};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not open %s\n",
sizeof(char),26),_tag_arr(_tmp203,sizeof(void*),1));}});return 1;}{struct Cyc_Std___cycFILE*
_tmp205=(struct Cyc_Std___cycFILE*)_check_null(_tmp202);struct _tagged_arr buf=
_tag_arr(({unsigned int _tmp218=(unsigned int)1024;char*_tmp219=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp218));{unsigned int _tmp21A=
_tmp218;unsigned int i;for(i=0;i < _tmp21A;i ++){_tmp219[i]='\000';}}_tmp219;}),
sizeof(char),(unsigned int)1024);struct _tagged_arr _tmp206=Cyc_Std_getcwd(buf,
_get_arr_size(buf,sizeof(char)));if(Cyc_mode != (void*)2){if(Cyc_Std_chdir(dir)){({
struct Cyc_Std_String_pa_struct _tmp208;_tmp208.tag=0;_tmp208.f1=(struct
_tagged_arr)dir;{void*_tmp207[1]={& _tmp208};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n",sizeof(char),37),_tag_arr(
_tmp207,sizeof(void*),1));}});return 1;}}if(Cyc_mode == (void*)1){struct
_tagged_arr _tmp209=Cstring_to_string(Ccomp);Cyc_Std_system((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct
_tagged_arr)_tmp209;{void*_tmp20A[1]={& _tmp20B};Cyc_Std_aprintf(_tag_arr("echo | %s -E -dM - -o INITMACROS.h\n",
sizeof(char),36),_tag_arr(_tmp20A,sizeof(void*),1));}}));}{struct Cyc_Lexing_lexbuf*
_tmp20C=Cyc_Lexing_from_file(_tmp205);struct _tuple13*entry;while((entry=((struct
_tuple13*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp20C))!= 0){struct
_tuple13 _tmp20E;struct _tagged_arr _tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*
_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_List_List*
_tmp214;struct Cyc_List_List*_tmp215;struct _tuple13*_tmp20D=(struct _tuple13*)
_check_null(entry);_tmp20E=*_tmp20D;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;
_tmp211=_tmp20E.f3;_tmp212=_tmp20E.f4;_tmp213=_tmp20E.f5;_tmp214=_tmp20E.f6;
_tmp215=_tmp20E.f7;if(Cyc_process_file(_tmp20F,_tmp210,_tmp211,_tmp212,_tmp213,
_tmp214,_tmp215))return 1;}Cyc_Std_fclose(_tmp205);if(Cyc_mode != (void*)2){if(Cyc_Std_chdir((
struct _tagged_arr)_tmp206)){({struct Cyc_Std_String_pa_struct _tmp217;_tmp217.tag=
0;_tmp217.f1=(struct _tagged_arr)_tmp206;{void*_tmp216[1]={& _tmp217};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not change directory to %s\n",sizeof(char),
41),_tag_arr(_tmp216,sizeof(void*),1));}});return 1;}}return 0;}}}static char
_tmp21B[13]="BUILDLIB.OUT";static struct _tagged_arr Cyc_output_dir={_tmp21B,
_tmp21B,_tmp21B + 13};static void Cyc_set_output_dir(struct _tagged_arr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_tagged_arr s){Cyc_spec_files=({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*
_tmp21C));_tmp21C->hd=({struct _tagged_arr*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D[0]=s;_tmp21D;});_tmp21C->tl=Cyc_spec_files;_tmp21C;});}static void Cyc_set_GATHER(){
Cyc_mode=(void*)1;}static void Cyc_set_GATHERSCRIPT(){Cyc_mode=(void*)2;}static
void Cyc_set_FINISH(){Cyc_mode=(void*)3;}static int Cyc_badparse=0;static void Cyc_unsupported_option(
struct _tagged_arr s){({struct Cyc_Std_String_pa_struct _tmp21F;_tmp21F.tag=0;
_tmp21F.f1=(struct _tagged_arr)s;{void*_tmp21E[1]={& _tmp21F};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Unsupported option %s\n",sizeof(char),23),_tag_arr(_tmp21E,sizeof(void*),
1));}});Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple17{struct
_tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(
int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
options=({struct _tuple17*_tmp230[5];_tmp230[4]=({struct _tuple17*_tmp23D=
_cycalloc(sizeof(*_tmp23D));_tmp23D->f1=_tag_arr("-",sizeof(char),2);_tmp23D->f2=
1;_tmp23D->f3=_tag_arr("",sizeof(char),1);_tmp23D->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp23F;_tmp23F.tag=1;_tmp23F.f1=Cyc_unsupported_option;_tmp23F;});_tmp23E;});
_tmp23D->f5=_tag_arr("",sizeof(char),1);_tmp23D;});_tmp230[3]=({struct _tuple17*
_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=_tag_arr("-finish",sizeof(char),
8);_tmp23A->f2=0;_tmp23A->f3=_tag_arr("",sizeof(char),1);_tmp23A->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp23C;_tmp23C.tag=0;_tmp23C.f1=Cyc_set_FINISH;
_tmp23C;});_tmp23B;});_tmp23A->f5=_tag_arr("Produce Cyclone headers from pre-gathered C library info",
sizeof(char),57);_tmp23A;});_tmp230[2]=({struct _tuple17*_tmp237=_cycalloc(
sizeof(*_tmp237));_tmp237->f1=_tag_arr("-gatherscript",sizeof(char),14);_tmp237->f2=
0;_tmp237->f3=_tag_arr("",sizeof(char),1);_tmp237->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp239;_tmp239.tag=0;_tmp239.f1=Cyc_set_GATHERSCRIPT;_tmp239;});_tmp238;});
_tmp237->f5=_tag_arr("Produce a script to gather C library info",sizeof(char),42);
_tmp237;});_tmp230[1]=({struct _tuple17*_tmp234=_cycalloc(sizeof(*_tmp234));
_tmp234->f1=_tag_arr("-gather",sizeof(char),8);_tmp234->f2=0;_tmp234->f3=
_tag_arr("",sizeof(char),1);_tmp234->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp236;_tmp236.tag=0;_tmp236.f1=Cyc_set_GATHER;_tmp236;});_tmp235;});_tmp234->f5=
_tag_arr("Gather C library info but don't produce Cyclone headers",sizeof(char),
56);_tmp234;});_tmp230[0]=({struct _tuple17*_tmp231=_cycalloc(sizeof(*_tmp231));
_tmp231->f1=_tag_arr("-d",sizeof(char),3);_tmp231->f2=0;_tmp231->f3=_tag_arr(" <file>",
sizeof(char),8);_tmp231->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp232=
_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Arg_String_spec_struct _tmp233;
_tmp233.tag=5;_tmp233.f1=Cyc_set_output_dir;_tmp233;});_tmp232;});_tmp231->f5=
_tag_arr("Set the output directory to <file>",sizeof(char),35);_tmp231;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp230,
sizeof(struct _tuple17*),5));});Cyc_Arg_parse(options,Cyc_add_spec_file,_tag_arr("Options:",
sizeof(char),9),argv);if(Cyc_badparse?1: Cyc_spec_files == 0){Cyc_Arg_usage(
options,_tag_arr("Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
sizeof(char),59));return 1;}if(Cyc_mode == (void*)2){Cyc_script_file=Cyc_Std_fopen(
_tag_arr("BUILDLIB.sh",sizeof(char),12),_tag_arr("w",sizeof(char),2));if(!((
unsigned int)Cyc_script_file)){({void*_tmp220[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Could not create file BUILDLIB.sh\n",sizeof(char),35),_tag_arr(_tmp220,
sizeof(void*),0));});exit(1);}({void*_tmp221[0]={};Cyc_prscript(_tag_arr("#!/bin/sh\n",
sizeof(char),11),_tag_arr(_tmp221,sizeof(void*),0));});({void*_tmp222[0]={};Cyc_prscript(
_tag_arr("GCC=\"gcc\"\n",sizeof(char),11),_tag_arr(_tmp222,sizeof(void*),0));});}
if(Cyc_force_directory_prefixes(Cyc_output_dir)?1: Cyc_force_directory(Cyc_output_dir)){({
struct Cyc_Std_String_pa_struct _tmp224;_tmp224.tag=0;_tmp224.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp223[1]={& _tmp224};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp223,sizeof(void*),1));}});return 1;}if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct
_tmp226;_tmp226.tag=0;_tmp226.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp225[
1]={& _tmp226};Cyc_prscript(_tag_arr("cd %s\n",sizeof(char),7),_tag_arr(_tmp225,
sizeof(void*),1));}});({void*_tmp227[0]={};Cyc_prscript(_tag_arr("echo | $GCC -E -dM - -o INITMACROS.h\n",
sizeof(char),38),_tag_arr(_tmp227,sizeof(void*),0));});}if(!Cyc_gathering()){Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("BUILDLIB.LOG",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _tmp229;_tmp229.tag=0;_tmp229.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp228[1]={& _tmp229};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create log file in directory %s\n",sizeof(char),50),
_tag_arr(_tmp228,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cstubs.c",sizeof(
char),9)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp22B;_tmp22B.tag=0;_tmp22B.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp22A[1]={& _tmp22B};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cstubs.c in directory %s\n",sizeof(char),50),
_tag_arr(_tmp22A,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cycstubs.cyc",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp22D;_tmp22D.tag=0;_tmp22D.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp22C[1]={& _tmp22D};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cycstubs.c in directory %s\n",sizeof(char),52),
_tag_arr(_tmp22C,sizeof(void*),1));}});return 1;}({void*_tmp22E[0]={};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),_tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof(char),32),_tag_arr(_tmp22E,sizeof(void*),0));});}for(0;Cyc_spec_files != 0;
Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(Cyc_process_specfile(*((
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd),Cyc_output_dir)){({
void*_tmp22F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("FATAL ERROR -- QUIT!\n",
sizeof(char),22),_tag_arr(_tmp22F,sizeof(void*),0));});exit(1);}}if(Cyc_mode == (
void*)2)Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_script_file));
else{if(!Cyc_gathering()){Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(
Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file));}}
return 0;}}
