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
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
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
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;int Cyc_Std_remove(struct _tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);
int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(
struct _tagged_arr __filename,struct _tagged_arr __modes);int Cyc_Std_fputc(int __c,
struct Cyc_Std___cycFILE*__stream);int Cyc_Std_fputs(struct _tagged_arr __s,struct
Cyc_Std___cycFILE*__stream);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);int Cyc_Std_vfprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};int Cyc_Std_fscanf(struct Cyc_Std___cycFILE*stream,struct
_tagged_arr fmt,struct _tagged_arr);extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
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
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct
Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{
int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct
Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*
f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[
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
first_column;int last_line;int last_column;};extern char Cyc_Array_Array_mismatch[19];
struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};int Cyc_Std_system(
struct _tagged_arr);struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;
int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};extern int timezone;
struct Cyc_Std_stat_t{unsigned long long st_dev;unsigned short __pad1;unsigned int
st_ino;unsigned int st_mode;unsigned int st_nlink;unsigned int st_uid;unsigned int
st_gid;unsigned long long st_rdev;unsigned short __pad2;int st_size;int st_blksize;
int st_blocks;int st_atime;unsigned int __unused1;int st_mtime;unsigned int __unused2;
int st_ctime;unsigned int __unused3;unsigned int __unused4;unsigned int __unused5;};
int Cyc_Std_mkdir(struct _tagged_arr pathname,unsigned int mode);struct Cyc_Std_flock{
short l_type;int l_start;short l_whence;int l_len;int l_pid;};struct Cyc_Std_Flock_struct{
int tag;struct Cyc_Std_flock*f1;};struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_open(
struct _tagged_arr,int,struct _tagged_arr);struct Cyc_Std_timeval{int tv_sec;int
tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};struct Cyc_Std_itimerval{
struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval it_value;};typedef struct{
unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t Cyc_Std_sigset_t;
struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;struct Cyc_Std___fd_set{
int __fds_bits[32];};struct Cyc_Std_option{struct _tagged_arr name;int has_arg;int*
flag;int val;};int close(int);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(
struct _tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned int,unsigned int);
struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int size);int Cyc_Std_execl(
struct _tagged_arr path,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execlp(
struct _tagged_arr file,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execve(
struct _tagged_arr filename,struct _tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,
unsigned int count);int Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(
int fd,struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct
_tagged_arr pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int
Cyc_Std_chroot(struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct
_tagged_arr prompt);int isspace(int);int toupper(int);void Cyc_Lex_lex_init();extern
char*Ccomp;struct Cyc_Std___cycFILE*Cyc_log_file=0;struct Cyc_Std___cycFILE*Cyc_cstubs_file=
0;struct Cyc_Std___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_log_file == 0){({void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Internal error: log file is NULL\n",sizeof(char),34),_tag_arr(_tmp0,
sizeof(void*),0));});exit(1);}{int _tmp1=Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_log_file),fmt,ap);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)((
struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file)));return _tmp1;}}int Cyc_sizeof_unsignedlongint=
- 1;int Cyc_sizeof_unsignedshortint=- 1;int Cyc_sizeof_shortint=- 1;int Cyc_sizeof_int=
- 1;int Cyc_sizeof_short=- 1;int Cyc_sizeof_fdmask=- 1;int Cyc_sizeof___fdmask=- 1;int
Cyc_sizeof_uint32=- 1;int Cyc_sizeof_sockaddr=- 1;int Cyc_sizeof_inport=- 1;int Cyc_sizeof_inaddr=
- 1;static struct _tagged_arr*Cyc_current_source=0;static struct Cyc_List_List*Cyc_current_args=
0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(struct
_tagged_arr*sptr){Cyc_current_targets=({struct Cyc_Set_Set**_tmp2=_cycalloc(
sizeof(*_tmp2));_tmp2[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr);_tmp2;});}struct _tuple9{struct _tagged_arr*f1;struct Cyc_Set_Set*f2;};struct
_tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc_Std___cycFILE*Cyc_slurp_out=0;int
Cyc_slurp(struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm(struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple10{struct _tagged_arr f1;
struct _tagged_arr*f2;};struct _tuple10*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct _tuple11{struct _tagged_arr f1;struct _tagged_arr f2;};struct _tuple12{
struct _tagged_arr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};struct
_tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_header=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*Cyc_current_symbols=
0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*Cyc_current_cycstubs=
0;struct Cyc_List_List*Cyc_current_prologue=0;struct Cyc_List_List*Cyc_current_epilogue=
0;struct Cyc_List_List*Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=
0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=
0;int Cyc_numdef=0;const int Cyc_lex_base[503]=(const int[503]){0,0,75,192,305,283,
309,166,310,91,27,384,28,502,616,694,287,338,93,- 3,0,- 1,- 2,- 8,- 3,1,- 2,585,- 4,2,
166,- 5,776,799,278,- 6,- 7,- 4,16,- 3,29,11,854,- 3,929,13,- 16,223,12,- 2,225,22,26,28,
24,28,58,56,65,52,70,332,1043,1158,352,93,83,90,112,96,106,115,124,115,126,113,
343,357,152,180,207,235,235,345,386,233,242,246,256,243,262,372,1272,1387,396,- 8,
256,253,272,258,258,259,281,381,1501,1616,- 11,410,- 12,263,285,504,1730,1845,- 9,
528,- 10,618,- 13,390,512,514,1922,1999,2076,2157,398,404,632,2232,286,286,286,287,
283,306,0,13,4,2313,5,617,2321,2386,635,49,497,6,2347,7,643,2409,2447,681,- 30,
1070,320,311,299,323,314,324,668,1188,332,329,321,404,333,329,340,343,337,375,
377,648,1072,367,412,407,412,412,808,882,419,414,848,421,429,850,1186,425,424,
1045,431,431,430,463,447,1052,1298,454,482,497,523,535,543,544,533,1389,554,555,
553,555,542,1503,545,546,1618,2509,563,558,1732,604,564,1847,568,587,586,611,593,
2371,615,602,615,598,607,2511,620,616,613,609,607,10,14,629,609,2519,643,630,643,
637,647,2521,973,640,640,674,666,677,687,675,704,708,718,735,719,- 28,728,732,728,
738,767,775,- 23,755,778,771,768,779,774,776,792,793,- 17,780,778,778,817,818,- 25,
810,808,801,813,835,851,829,851,893,881,885,876,876,- 27,884,904,907,917,925,908,
941,958,965,968,1,4,6,969,971,958,957,969,972,981,993,2,52,994,995,1102,20,21,
1028,1030,994,992,1007,1039,1040,1095,1096,1098,- 22,1077,1078,1134,1135,1136,- 19,
1083,1086,1141,1145,1147,- 20,1094,1096,1151,1152,1153,- 18,1105,1107,1162,1163,
1164,- 21,1121,1122,1508,- 29,1416,1122,1125,1148,1147,1146,1141,1144,1176,1178,
1176,1189,1623,- 14,1181,1184,1183,1196,1737,- 15,- 7,- 8,8,1300,2523,9,1356,2531,
2556,1589,1513,- 49,1287,- 2,1236,- 4,1239,1274,2129,1240,1272,1356,1703,1241,2613,
2656,1248,1256,1253,1302,2726,1270,1304,- 36,- 42,- 37,2801,1275,- 40,1305,- 45,- 39,-
48,2876,2905,1816,1329,1339,2623,2915,2945,2694,2886,2978,3009,3047,1364,1375,
3117,3155,1442,1452,1444,1454,1446,1456,- 6,- 34,1322,3087,- 47,- 32,- 46,- 31,- 33,
1373,3195,1377,1381,2376,1383,1384,1385,1388,1392,1393,1394,1395,1406,3268,3352,
2515,1411,- 24,- 41,- 38,- 35,- 26,1642,3434,4,3517,1437,15,1379,1379,1417,1415,1414,
1433,1504};const int Cyc_lex_backtrk[503]=(const int[503]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,-
1,6,- 1,5,1,0,- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,
14,2,4,4,- 1,1,1,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,-
1,29,29,29,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,
4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,
- 1,0,43,- 1,42,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,
- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[503]=(const
int[503]){- 1,- 1,- 1,403,392,154,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,
0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,
- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,-
1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,-
1,467,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,0,0,0,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3774]=(const int[3774]){0,0,0,0,0,0,
0,0,0,0,22,19,28,495,19,28,19,28,95,19,45,45,45,45,45,22,45,0,0,0,0,0,21,316,327,
496,21,22,- 1,- 1,22,- 1,- 1,45,317,45,318,22,493,493,493,493,493,493,493,493,493,
493,31,119,22,244,130,40,245,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,31,328,362,356,493,146,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,491,491,491,491,491,491,491,491,491,491,137,85,20,78,65,55,
56,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,57,58,59,60,491,61,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,66,67,37,
404,405,404,404,405,39,22,68,69,70,71,72,145,34,34,34,34,34,34,34,34,73,74,404,
406,407,75,76,408,409,410,120,120,411,412,120,413,414,415,416,417,417,417,417,
417,417,417,417,417,418,79,419,420,421,120,19,422,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,19,- 1,- 1,
423,422,80,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,393,424,22,394,24,24,37,147,138,81,155,35,35,
35,35,35,35,35,35,38,82,83,96,86,395,97,62,62,26,26,62,98,21,24,21,87,25,77,77,
84,84,77,88,84,89,90,64,64,91,62,64,77,77,117,109,77,99,26,100,101,77,102,84,156,
103,110,92,92,111,64,92,131,132,133,77,104,104,134,135,104,84,84,396,157,84,- 1,
148,139,- 1,92,94,94,136,- 1,94,39,- 1,22,104,- 1,254,158,- 1,84,107,107,159,21,107,
160,161,162,238,94,232,27,223,129,129,129,129,129,129,129,129,129,129,107,169,
170,171,172,173,22,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,35,174,175,187,129,28,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,- 1,194,19,- 1,121,121,112,112,121,179,112,180,95,195,120,120,128,128,
120,181,128,182,185,186,188,189,108,121,192,112,115,115,21,193,115,216,21,120,
196,128,197,122,123,122,122,122,122,122,122,122,122,122,122,115,114,198,199,210,
21,21,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,203,21,28,204,122,29,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,47,
47,117,117,47,205,117,206,30,30,30,30,30,30,30,30,128,128,207,208,128,- 1,209,47,
211,117,116,212,213,- 1,214,215,176,176,217,- 1,176,218,48,128,144,144,144,144,144,
144,144,144,221,222,49,226,162,162,227,176,162,31,35,35,35,35,35,35,35,35,153,
153,153,153,153,153,153,153,224,162,228,229,41,41,32,230,41,163,231,233,234,235,
236,237,50,248,51,241,242,225,52,239,243,246,247,41,53,54,35,35,35,35,35,35,35,
35,240,249,250,251,118,42,42,42,42,42,42,42,42,42,42,252,- 1,253,305,306,291,28,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
285,275,268,261,42,262,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,183,183,43,263,183,264,265,33,33,33,33,33,33,33,33,33,33,
266,267,269,270,271,272,183,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,186,
186,190,190,186,273,190,33,33,33,33,33,33,274,276,21,33,33,33,33,33,33,277,186,
278,190,279,280,281,282,283,284,116,286,183,183,287,288,183,33,33,33,33,33,33,44,
44,44,44,44,44,44,44,44,44,289,290,183,292,293,294,295,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,296,297,298,299,44,28,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,184,300,301,302,303,304,368,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,307,308,309,310,44,311,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,
193,193,62,312,193,313,314,200,200,315,319,200,320,321,322,323,255,256,257,324,
258,62,325,193,259,372,372,176,176,372,200,176,114,260,326,329,330,63,63,63,63,
63,63,63,63,63,63,350,372,344,176,336,337,338,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,339,340,341,342,63,343,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,31,64,64,373,
331,64,345,346,332,347,348,349,351,374,177,352,353,333,375,334,354,178,355,357,
64,358,359,360,361,190,190,163,163,190,363,163,364,365,366,367,63,63,63,63,63,63,
63,63,63,63,369,370,190,386,163,335,380,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,376,377,378,379,63,46,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,92,92,164,381,92,382,
383,384,387,165,191,388,166,389,390,490,489,490,490,488,466,459,167,92,168,434,
200,200,402,402,200,486,402,290,371,431,304,432,490,93,93,93,93,93,93,93,93,93,
93,200,427,402,465,21,487,430,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,267,429,425,433,93,462,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,36,94,94,209,209,94,154,209,201,
401,401,401,401,401,401,401,401,485,202,95,95,355,463,464,94,349,209,367,361,106,
372,372,108,426,372,108,118,385,391,46,93,93,93,93,93,93,93,93,93,93,284,95,95,
372,35,343,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,21,497,498,35,93,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,104,104,215,215,104,373,215,370,370,499,500,370,
402,402,501,374,402,31,31,36,36,458,458,104,502,215,21,0,0,0,370,0,0,0,118,402,0,
21,371,105,105,105,105,105,105,105,105,105,105,31,31,36,36,458,458,0,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,106,107,107,219,219,107,0,
219,384,384,0,0,384,31,31,31,31,31,31,31,31,0,0,0,107,0,219,490,0,490,490,384,0,
0,0,95,0,0,0,385,105,105,105,105,105,105,105,105,105,105,490,0,0,0,0,0,0,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,112,112,222,222,112,0,
222,390,390,0,460,390,461,461,461,461,461,461,461,461,461,461,0,112,0,222,0,0,0,
0,390,0,0,0,106,0,0,0,391,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,114,115,115,225,
225,115,440,225,440,0,0,441,441,441,441,441,441,441,441,441,441,0,0,0,115,0,225,
0,0,0,0,0,0,0,0,36,0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,124,124,124,124,
124,124,124,124,124,124,124,124,21,0,0,0,0,0,0,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,
124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,125,124,124,124,124,124,124,124,124,124,124,
21,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,21,0,0,0,0,- 1,0,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,- 1,- 1,0,- 1,124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,125,125,125,
125,125,125,125,125,125,127,0,0,0,0,0,468,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,125,
0,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,0,0,0,0,0,0,
0,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,0,0,28,0,0,
149,141,141,141,141,141,141,141,141,143,143,143,143,143,143,143,143,143,143,0,
231,231,0,0,231,- 1,143,143,143,143,143,143,0,0,0,150,150,150,150,150,150,150,150,
231,0,0,0,0,0,0,31,0,35,- 1,0,343,0,0,143,143,143,143,143,143,483,483,483,483,483,
483,483,483,0,142,143,143,143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,
143,143,143,143,152,152,152,152,152,152,152,152,152,152,151,0,0,0,0,0,0,152,152,
152,152,152,152,0,0,0,143,143,143,143,143,143,0,0,0,0,0,0,152,152,152,152,152,
152,152,152,152,152,0,152,152,152,152,152,152,152,152,152,152,152,152,219,219,
237,237,219,0,237,0,0,0,247,247,253,253,247,19,253,0,397,0,0,0,0,219,0,237,152,
152,152,152,152,152,95,247,19,253,343,0,0,0,0,0,31,0,28,484,484,484,484,484,484,
484,484,398,398,398,398,398,398,398,398,400,400,400,400,400,400,400,400,400,400,
0,0,0,0,0,0,0,400,400,400,400,400,400,0,0,400,400,400,400,400,400,400,400,400,
400,220,0,0,0,0,0,28,400,400,400,400,400,400,0,400,400,400,400,400,400,0,0,0,0,0,
0,0,0,0,399,0,0,0,0,0,0,0,0,0,400,400,400,400,400,400,435,0,446,446,446,446,446,
446,446,446,447,447,441,441,441,441,441,441,441,441,441,441,0,437,0,0,0,0,0,0,
448,0,0,0,0,0,0,0,0,449,0,0,450,435,0,436,436,436,436,436,436,436,436,436,436,
437,0,0,0,0,0,0,448,0,0,0,437,0,0,0,0,449,0,438,450,0,0,0,444,0,444,0,439,445,
445,445,445,445,445,445,445,445,445,0,0,0,0,0,437,0,0,0,0,0,0,438,0,0,0,0,0,0,0,
0,439,428,428,428,428,428,428,428,428,428,428,0,0,0,0,0,0,0,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,0,0,0,0,428,0,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,0,0,0,0,0,0,0,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,0,0,0,0,428,0,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,442,442,442,442,442,442,442,442,442,442,445,445,445,445,445,445,445,445,445,
445,0,443,116,0,0,0,0,435,116,436,436,436,436,436,436,436,436,436,436,441,441,
441,441,441,441,441,441,441,441,0,437,0,0,443,116,0,0,438,0,0,116,114,0,0,0,0,
439,114,0,442,442,442,442,442,442,442,442,442,442,0,0,0,437,0,0,0,0,0,0,438,443,
116,0,114,0,0,0,116,439,114,0,0,445,445,445,445,445,445,445,445,445,445,0,0,0,0,
0,0,0,0,0,0,443,116,116,0,0,0,0,116,116,435,0,446,446,446,446,446,446,446,446,
447,447,0,0,0,0,0,0,0,0,0,0,0,437,0,116,0,0,0,0,456,116,0,0,0,0,0,0,435,457,447,
447,447,447,447,447,447,447,447,447,0,0,0,0,0,437,0,0,0,0,0,437,456,0,0,0,0,0,
454,0,0,457,0,0,0,0,0,455,0,0,461,461,461,461,461,461,461,461,461,461,0,0,0,437,
0,0,0,0,0,0,454,443,116,0,0,0,0,0,116,455,451,451,451,451,451,451,451,451,451,
451,0,0,0,0,0,0,0,451,451,451,451,451,451,443,116,0,0,0,0,0,116,0,0,0,0,0,0,0,
451,451,451,451,451,451,451,451,451,451,0,451,451,451,451,451,451,451,451,451,
451,451,451,0,0,0,469,0,452,0,0,470,0,0,0,0,0,453,0,0,471,471,471,471,471,471,
471,471,0,451,451,451,451,451,451,472,0,0,0,0,452,0,0,0,0,0,0,0,0,453,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,473,0,0,0,0,474,475,0,0,0,476,0,0,0,0,0,0,0,477,0,0,0,478,0,
479,0,480,0,481,482,482,482,482,482,482,482,482,482,482,0,0,0,0,0,0,0,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,0,0,0,0,0,0,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,274,0,0,0,0,0,0,0,0,482,482,
482,482,482,482,482,482,482,482,0,0,0,0,0,0,0,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,0,0,0,0,
0,0,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,22,0,0,492,0,0,0,491,491,491,491,491,491,491,491,491,
491,0,0,0,0,0,0,0,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,0,0,0,0,491,0,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,494,0,0,0,0,0,0,0,493,493,493,493,493,493,493,493,493,493,0,0,0,0,0,0,0,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,0,0,0,0,493,0,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3774]=(const int[3774]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,138,140,147,149,394,397,41,41,45,45,41,495,45,- 1,- 1,- 1,- 1,- 1,136,
315,326,0,492,10,12,40,10,12,40,41,316,45,317,20,1,1,1,1,1,1,1,1,1,1,38,48,137,
243,10,38,244,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,145,327,331,
332,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,51,18,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,7,3,3,3,3,3,
7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,5,4,6,8,16,6,8,80,5,34,34,34,34,34,34,34,34,16,81,82,50,85,4,50,61,61,6,
8,61,50,9,17,18,86,17,76,76,83,83,76,87,83,88,89,64,64,90,61,64,77,77,96,97,77,
98,17,99,100,76,101,83,5,102,109,91,91,110,64,91,130,131,132,77,103,103,133,134,
103,84,84,4,5,84,119,6,8,119,91,94,94,135,126,94,16,126,16,103,127,156,157,127,
84,107,107,158,7,107,159,160,161,164,94,165,17,166,11,11,11,11,11,11,11,11,11,11,
107,168,169,170,171,172,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,64,173,174,177,11,77,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,146,167,84,146,13,13,111,111,13,178,
111,179,94,167,120,120,121,121,120,180,121,181,184,185,187,188,107,13,191,111,
115,115,5,192,115,194,16,120,195,121,196,13,13,13,13,13,13,13,13,13,13,13,13,115,
4,197,198,201,6,8,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,202,17,27,203,13,27,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,14,14,117,117,14,204,117,205,27,27,27,27,27,27,
27,27,128,128,206,207,128,119,208,14,210,117,115,211,212,126,213,214,175,175,216,
127,175,217,14,128,141,141,141,141,141,141,141,141,220,221,14,224,162,162,226,
175,162,27,144,144,144,144,144,144,144,144,150,150,150,150,150,150,150,150,223,
162,227,228,15,15,27,229,15,162,230,232,233,234,235,236,14,239,14,240,241,223,14,
238,242,245,246,15,14,14,153,153,153,153,153,153,153,153,238,248,249,250,117,15,
15,15,15,15,15,15,15,15,15,251,146,252,255,255,256,13,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,257,258,259,260,15,261,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,182,182,
15,262,182,263,264,32,32,32,32,32,32,32,32,32,32,265,266,268,269,270,271,182,32,
32,32,32,32,32,33,33,33,33,33,33,33,33,33,33,186,186,189,189,186,272,189,33,33,
33,33,33,33,273,275,14,32,32,32,32,32,32,276,186,277,189,278,279,280,281,282,283,
186,285,183,183,286,287,183,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,288,
289,183,291,292,293,294,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,295,296,297,298,42,15,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,183,299,
300,301,302,303,305,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,306,307,308,309,44,310,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,193,193,62,311,193,312,313,199,199,
314,318,199,319,320,321,322,254,254,254,323,254,62,324,193,254,155,155,176,176,
155,199,176,193,254,325,328,329,62,62,62,62,62,62,62,62,62,62,333,155,334,176,
335,336,337,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,338,339,340,341,62,342,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,63,63,155,330,63,344,345,330,346,347,348,350,
155,176,351,352,330,155,330,353,176,354,356,63,357,358,359,360,190,190,163,163,
190,362,163,363,364,365,366,63,63,63,63,63,63,63,63,63,63,368,369,190,373,163,
330,374,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,375,376,377,378,63,379,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,92,92,163,380,92,381,382,383,386,163,190,387,163,388,
389,404,406,404,404,408,411,415,163,92,163,418,200,200,395,395,200,409,395,420,
412,419,419,419,404,92,92,92,92,92,92,92,92,92,92,200,423,395,412,395,409,429,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,421,
421,424,431,92,460,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,93,93,209,209,93,413,209,200,398,398,398,398,398,398,398,
398,467,200,438,439,469,413,413,93,470,209,472,473,474,372,372,475,424,372,209,
476,477,478,479,93,93,93,93,93,93,93,93,93,93,480,438,439,372,448,484,449,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,494,496,
497,448,93,449,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,104,104,215,215,104,372,215,370,370,498,499,370,402,402,500,372,402,
452,453,454,455,456,457,104,501,215,502,- 1,- 1,- 1,370,- 1,- 1,- 1,215,402,- 1,402,370,
104,104,104,104,104,104,104,104,104,104,452,453,454,455,456,457,- 1,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,218,218,105,
- 1,218,384,384,- 1,- 1,384,401,401,401,401,401,401,401,401,- 1,- 1,- 1,105,- 1,218,490,
- 1,490,490,384,- 1,- 1,- 1,218,- 1,- 1,- 1,384,105,105,105,105,105,105,105,105,105,105,
490,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,112,112,222,222,112,- 1,222,390,390,- 1,414,390,414,414,414,414,414,
414,414,414,414,414,- 1,112,- 1,222,- 1,- 1,- 1,- 1,390,- 1,- 1,- 1,222,- 1,- 1,- 1,390,112,
112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,113,113,225,225,113,437,225,437,
- 1,- 1,437,437,437,437,437,437,437,437,437,437,- 1,- 1,- 1,113,- 1,225,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,225,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,122,
122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,- 1,- 1,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,
123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,
- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,124,124,124,124,
124,124,- 1,- 1,- 1,- 1,410,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,125,410,- 1,125,124,- 1,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,125,125,125,
125,- 1,- 1,- 1,- 1,- 1,410,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,125,- 1,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,139,- 1,- 1,139,129,- 1,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,- 1,- 1,148,- 1,- 1,148,
139,139,139,139,139,139,139,139,142,142,142,142,142,142,142,142,142,142,- 1,231,
231,- 1,- 1,231,410,142,142,142,142,142,142,- 1,- 1,- 1,148,148,148,148,148,148,148,
148,231,- 1,- 1,- 1,- 1,- 1,- 1,139,- 1,231,125,- 1,471,- 1,- 1,142,142,142,142,142,142,
471,471,471,471,471,471,471,471,- 1,139,143,143,143,143,143,143,143,143,143,143,
148,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,151,151,151,151,151,151,151,151,
151,151,148,- 1,- 1,- 1,- 1,- 1,- 1,151,151,151,151,151,151,- 1,- 1,- 1,143,143,143,143,
143,143,- 1,- 1,- 1,- 1,- 1,- 1,152,152,152,152,152,152,152,152,152,152,- 1,151,151,151,
151,151,151,152,152,152,152,152,152,219,219,237,237,219,- 1,237,- 1,- 1,- 1,247,247,
253,253,247,396,253,- 1,396,- 1,- 1,- 1,- 1,219,- 1,237,152,152,152,152,152,152,219,
247,237,253,483,- 1,- 1,- 1,- 1,- 1,247,- 1,253,483,483,483,483,483,483,483,483,396,
396,396,396,396,396,396,396,399,399,399,399,399,399,399,399,399,399,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,399,399,399,399,399,399,- 1,- 1,400,400,400,400,400,400,400,400,400,400,
219,- 1,- 1,- 1,- 1,- 1,396,400,400,400,400,400,400,- 1,399,399,399,399,399,399,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,416,
- 1,416,416,416,416,416,416,416,416,416,416,440,440,440,440,440,440,440,440,440,
440,- 1,416,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,416,417,- 1,
417,417,417,417,417,417,417,417,417,417,416,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,- 1,417,-
1,- 1,- 1,- 1,416,- 1,417,416,- 1,- 1,- 1,443,- 1,443,- 1,417,443,443,443,443,443,443,443,
443,443,443,- 1,- 1,- 1,- 1,- 1,417,- 1,- 1,- 1,- 1,- 1,- 1,417,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,417,
422,422,422,422,422,422,422,422,422,422,- 1,- 1,- 1,- 1,- 1,- 1,- 1,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,- 1,- 1,- 1,- 1,422,- 1,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,428,428,428,428,428,428,428,428,
428,428,- 1,- 1,- 1,- 1,- 1,- 1,- 1,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,- 1,- 1,- 1,- 1,428,- 1,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,435,435,435,435,435,435,435,435,435,435,444,444,444,444,444,444,
444,444,444,444,- 1,435,435,- 1,- 1,- 1,- 1,436,435,436,436,436,436,436,436,436,436,
436,436,441,441,441,441,441,441,441,441,441,441,- 1,436,- 1,- 1,435,435,- 1,- 1,436,-
1,- 1,435,441,- 1,- 1,- 1,- 1,436,441,- 1,442,442,442,442,442,442,442,442,442,442,- 1,-
1,- 1,436,- 1,- 1,- 1,- 1,- 1,- 1,436,442,442,- 1,441,- 1,- 1,- 1,442,436,441,- 1,- 1,445,445,
445,445,445,445,445,445,445,445,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,442,442,445,- 1,- 1,
- 1,- 1,442,445,446,- 1,446,446,446,446,446,446,446,446,446,446,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,446,- 1,445,- 1,- 1,- 1,- 1,446,445,- 1,- 1,- 1,- 1,- 1,- 1,447,446,447,447,
447,447,447,447,447,447,447,447,- 1,- 1,- 1,- 1,- 1,446,- 1,- 1,- 1,- 1,- 1,447,446,- 1,- 1,
- 1,- 1,- 1,447,- 1,- 1,446,- 1,- 1,- 1,- 1,- 1,447,- 1,- 1,461,461,461,461,461,461,461,461,
461,461,- 1,- 1,- 1,447,- 1,- 1,- 1,- 1,- 1,- 1,447,461,461,- 1,- 1,- 1,- 1,- 1,461,447,450,
450,450,450,450,450,450,450,450,450,- 1,- 1,- 1,- 1,- 1,- 1,- 1,450,450,450,450,450,450,
461,461,- 1,- 1,- 1,- 1,- 1,461,- 1,- 1,- 1,- 1,- 1,- 1,- 1,451,451,451,451,451,451,451,451,
451,451,- 1,450,450,450,450,450,450,451,451,451,451,451,451,- 1,- 1,- 1,468,- 1,451,-
1,- 1,468,- 1,- 1,- 1,- 1,- 1,451,- 1,- 1,468,468,468,468,468,468,468,468,- 1,451,451,451,
451,451,451,468,- 1,- 1,- 1,- 1,451,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,451,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,468,- 1,- 1,- 1,- 1,468,468,- 1,- 1,- 1,468,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
468,- 1,- 1,- 1,468,- 1,468,- 1,468,- 1,468,481,481,481,481,481,481,481,481,481,481,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,
481,481,481,481,481,481,481,481,481,481,481,- 1,- 1,- 1,- 1,- 1,- 1,481,481,481,481,
481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,
481,481,482,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,482,482,482,482,482,- 1,-
1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,- 1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,491,- 1,- 1,491,- 1,- 1,- 1,491,491,491,491,491,491,491,491,491,491,- 1,- 1,- 1,- 1,-
1,- 1,- 1,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,- 1,- 1,- 1,- 1,491,- 1,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,493,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,493,493,493,493,493,493,493,493,493,493,- 1,- 1,- 1,- 1,- 1,- 1,-
1,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,- 1,- 1,- 1,- 1,493,- 1,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
state];if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[state];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_unknown_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos
++));if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3774,base + 
c)]== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3774,base + c)];
else{state=Cyc_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Lexing_Error_struct _tmp4;
_tmp4.tag=Cyc_Lexing_Error;_tmp4.f1=_tag_arr("empty token",sizeof(char),12);
_tmp4;});_tmp3;}));else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=
0;}}}struct _tuple9*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: Cyc_macroname(
lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)
_check_null(Cyc_current_args))->tl){Cyc_current_targets=({struct Cyc_Set_Set**
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);
_tmp5;});}return({struct _tuple9*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->f1=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp6->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp6;});case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp7=
_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Lexing_Error_struct _tmp8;_tmp8.tag=
Cyc_Lexing_Error;_tmp8.f1=_tag_arr("some action didn't return!",sizeof(char),27);
_tmp8;});_tmp7;}));}struct _tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
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
_LL4F:({struct Cyc_Std_Int_pa_struct _tmp1F;_tmp1F.tag=1;_tmp1F.f1=(int)((
unsigned int)Cyc_sizeof_fdmask);{void*_tmp1E[1]={& _tmp1F};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),
5),_tag_arr(_tmp1E,sizeof(void*),1));}});({void*_tmp20[0]={};Cyc_log(_tag_arr("Warning: sizeof(fd_mask) inlined\n",
sizeof(char),34),_tag_arr(_tmp20,sizeof(void*),0));});return 1;case 3: _LL50:({
struct Cyc_Std_Int_pa_struct _tmp22;_tmp22.tag=1;_tmp22.f1=(int)((unsigned int)Cyc_sizeof___fdmask);{
void*_tmp21[1]={& _tmp22};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp21,sizeof(void*),1));}});({
void*_tmp23[0]={};Cyc_log(_tag_arr("Warning: sizeof(__fd_mask) inlined\n",
sizeof(char),36),_tag_arr(_tmp23,sizeof(void*),0));});return 1;case 4: _LL51:({
struct Cyc_Std_Int_pa_struct _tmp25;_tmp25.tag=1;_tmp25.f1=(int)((unsigned int)Cyc_sizeof_uint32);{
void*_tmp24[1]={& _tmp25};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp24,sizeof(void*),1));}});({
void*_tmp26[0]={};Cyc_log(_tag_arr("Warning: sizeof(__uint32_t) inlined\n",
sizeof(char),37),_tag_arr(_tmp26,sizeof(void*),0));});return 1;case 5: _LL52:({
struct Cyc_Std_Int_pa_struct _tmp28;_tmp28.tag=1;_tmp28.f1=(int)((unsigned int)Cyc_sizeof_inport);{
void*_tmp27[1]={& _tmp28};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp27,sizeof(void*),1));}});({
void*_tmp29[0]={};Cyc_log(_tag_arr("Warning: sizeof(__in_port_t) inlined\n",
sizeof(char),38),_tag_arr(_tmp29,sizeof(void*),0));});return 1;case 6: _LL53:({
struct Cyc_Std_Int_pa_struct _tmp2B;_tmp2B.tag=1;_tmp2B.f1=(int)((unsigned int)Cyc_sizeof_int);{
void*_tmp2A[1]={& _tmp2B};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp2A,sizeof(void*),1));}});({
void*_tmp2C[0]={};Cyc_log(_tag_arr("Warning: sizeof(int) inlined\n",sizeof(char),
30),_tag_arr(_tmp2C,sizeof(void*),0));});return 1;case 7: _LL54:({struct Cyc_Std_Int_pa_struct
_tmp2E;_tmp2E.tag=1;_tmp2E.f1=(int)((unsigned int)Cyc_sizeof_short);{void*_tmp2D[
1]={& _tmp2E};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp2D,sizeof(void*),1));}});({void*
_tmp2F[0]={};Cyc_log(_tag_arr("Warning: sizeof(__short) inlined\n",sizeof(char),
34),_tag_arr(_tmp2F,sizeof(void*),0));});return 1;case 8: _LL55:({struct Cyc_Std_Int_pa_struct
_tmp31;_tmp31.tag=1;_tmp31.f1=(int)((unsigned int)Cyc_sizeof_unsignedlongint);{
void*_tmp30[1]={& _tmp31};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp30,sizeof(void*),1));}});({
void*_tmp32[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned long int) inlined\n",
sizeof(char),44),_tag_arr(_tmp32,sizeof(void*),0));});return 1;case 9: _LL56:({
struct Cyc_Std_Int_pa_struct _tmp34;_tmp34.tag=1;_tmp34.f1=(int)((unsigned int)Cyc_sizeof_unsignedshortint);{
void*_tmp33[1]={& _tmp34};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp33,sizeof(void*),1));}});({
void*_tmp35[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned short int) inlined\n",
sizeof(char),45),_tag_arr(_tmp35,sizeof(void*),0));});return 1;case 10: _LL57:({
struct Cyc_Std_Int_pa_struct _tmp37;_tmp37.tag=1;_tmp37.f1=(int)((unsigned int)Cyc_sizeof_shortint);{
void*_tmp36[1]={& _tmp37};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp36,sizeof(void*),1));}});({
void*_tmp38[0]={};Cyc_log(_tag_arr("Warning: sizeof(short int) inlined\n",
sizeof(char),36),_tag_arr(_tmp38,sizeof(void*),0));});return 1;case 11: _LL58:({
struct Cyc_Std_Int_pa_struct _tmp3A;_tmp3A.tag=1;_tmp3A.f1=(int)((unsigned int)Cyc_sizeof_sockaddr);{
void*_tmp39[1]={& _tmp3A};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp39,sizeof(void*),1));}});({
void*_tmp3B[0]={};Cyc_log(_tag_arr("Warning: sizeof(struct sockaddr) inlined\n",
sizeof(char),42),_tag_arr(_tmp3B,sizeof(void*),0));});return 1;case 12: _LL59:({
struct Cyc_Std_Int_pa_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=(int)((unsigned int)Cyc_sizeof_inaddr);{
void*_tmp3C[1]={& _tmp3D};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(char),5),_tag_arr(_tmp3C,sizeof(void*),1));}});({
void*_tmp3E[0]={};Cyc_log(_tag_arr("Warning: sizeof(struct inaddr) inlined\n",
sizeof(char),40),_tag_arr(_tmp3E,sizeof(void*),0));});return 1;case 13: _LL5A: Cyc_Std_fputs(
_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp3F[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp3F,sizeof(void*),0));});return 1;case 14: _LL5B: Cyc_Std_fputs(
_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp40[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp40,sizeof(void*),0));});return 1;case 15: _LL5C: Cyc_Std_fputs(
_tag_arr("__region",sizeof(char),9),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp41[0]={};Cyc_log(_tag_arr("Warning: use of region sidestepped\n",
sizeof(char),36),_tag_arr(_tmp41,sizeof(void*),0));});return 1;case 16: _LL5D:
return 1;case 17: _LL5E: return 1;case 18: _LL5F: return 1;case 19: _LL60: return 1;case 20:
_LL61: return 1;case 21: _LL62: return 1;case 22: _LL63: Cyc_Std_fputs(_tag_arr("inline",
sizeof(char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 23: _LL64: Cyc_Std_fputs(_tag_arr("inline",sizeof(char),7),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 24: _LL65: Cyc_Std_fputs(_tag_arr("const",
sizeof(char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 25: _LL66: Cyc_Std_fputs(_tag_arr("const",sizeof(char),6),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 26: _LL67: Cyc_Std_fputs(_tag_arr("int",
sizeof(char),4),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 27: _LL68: return 1;case 28: _LL69: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}
Cyc_Std_fputs(_tag_arr("0",sizeof(char),2),(struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out));({void*_tmp42[0]={};Cyc_log(_tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof(char),42),_tag_arr(_tmp42,sizeof(void*),0));});return 1;case 29: _LL6A: Cyc_Std_fputc((
int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;default: _LL6B:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp43=_cycalloc(
sizeof(*_tmp43));_tmp43[0]=({struct Cyc_Lexing_Error_struct _tmp44;_tmp44.tag=Cyc_Lexing_Error;
_tmp44.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp44;});
_tmp43;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6D: return 0;
case 1: _LL6E: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 0;case 2: _LL6F:({void*_tmp45[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(char),26),_tag_arr(_tmp45,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp46[1]={& _tmp47};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp46,sizeof(void*),1));}});return 1;case
3: _LL70:({struct Cyc_Std_String_pa_struct _tmp49;_tmp49.tag=0;_tmp49.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp48[1]={& _tmp49};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp48,sizeof(void*),1));}});return 1;case 4: _LL71:({struct Cyc_Std_String_pa_struct
_tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4A[1]={& _tmp4B};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4A,sizeof(void*),1));}});return 1;case
5: _LL72:({struct Cyc_Std_String_pa_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp4C[1]={& _tmp4D};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp4C,sizeof(void*),1));}});return 1;case 6: _LL73:({struct Cyc_Std_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4E[1]={& _tmp4F};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4E,sizeof(void*),1));}});return 1;case
7: _LL74:({struct Cyc_Std_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp50[1]={& _tmp51};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp50,sizeof(void*),1));}});return 1;case 8: _LL75:({struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp52[1]={& _tmp53};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp52,sizeof(void*),1));}});return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp54=_cycalloc(
sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Lexing_Error_struct _tmp55;_tmp55.tag=Cyc_Lexing_Error;
_tmp55.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp55;});
_tmp54;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL78: return 0;case 1: _LL79:
if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match --;return 1;case 2: _LL7A: Cyc_parens_to_match
++;return 1;case 3: _LL7B: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL7C:
while(Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL7D: return 1;case 6: _LL7E: return 1;
default: _LL7F:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp56=_cycalloc(sizeof(*
_tmp56));_tmp56[0]=({struct Cyc_Lexing_Error_struct _tmp57;_tmp57.tag=Cyc_Lexing_Error;
_tmp57.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp57;});
_tmp56;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,
7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL81:({void*_tmp58[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp58,
sizeof(void*),0));});return 0;case 1: _LL82: return 0;case 2: _LL83:({void*_tmp59[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp59,
sizeof(void*),0));});return 1;case 3: _LL84: return 1;case 4: _LL85: return 1;case 5: _LL86:
return 1;case 6: _LL87: return 1;case 7: _LL88: return 1;case 8: _LL89: return 1;default:
_LL8A:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5A=_cycalloc(sizeof(*
_tmp5A));_tmp5A[0]=({struct Cyc_Lexing_Error_struct _tmp5B;_tmp5B.tag=Cyc_Lexing_Error;
_tmp5B.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp5B;});
_tmp5A;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8C:({void*
_tmp5C[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp5C,sizeof(void*),0));});return 0;case 1: _LL8D: return 0;case 2: _LL8E:
return 1;default: _LL8F:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5D=
_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Lexing_Error_struct _tmp5E;
_tmp5E.tag=Cyc_Lexing_Error;_tmp5E.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp5E;});_tmp5D;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple10*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL91: Cyc_current_line=_tag_arr("#define ",
sizeof(char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple10*_tmp5F=
_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=Cyc_current_line;_tmp5F->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp5F;});case 1: _LL92: return Cyc_suck_line(
lexbuf);case 2: _LL93: return 0;default: _LL94:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Lexing_Error_struct
_tmp61;_tmp61.tag=Cyc_Lexing_Error;_tmp61.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp61;});_tmp60;}));}struct _tuple10*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL96: Cyc_current_source=({struct _tagged_arr*_tmp62=_cycalloc(sizeof(*
_tmp62));_tmp62[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp62;});Cyc_current_line=(
struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((struct _tagged_arr*)
_check_null(Cyc_current_source)));return Cyc_suck_restofline(lexbuf);default:
_LL97:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp63=_cycalloc(sizeof(*
_tmp63));_tmp63[0]=({struct Cyc_Lexing_Error_struct _tmp64;_tmp64.tag=Cyc_Lexing_Error;
_tmp64.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp64;});
_tmp63;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL99:
Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL9A:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({
struct Cyc_Lexing_Error_struct _tmp66;_tmp66.tag=Cyc_Lexing_Error;_tmp66.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp66;});_tmp65;}));}int
Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple12*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9C:
Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);Cyc_current_cstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);return({struct _tuple12*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->f1=Cyc_current_header;_tmp67->f2=Cyc_current_symbols;_tmp67->f3=Cyc_current_omit_symbols;
_tmp67->f4=Cyc_current_prologue;_tmp67->f5=Cyc_current_epilogue;_tmp67->f6=Cyc_current_cstubs;
_tmp67->f7=Cyc_current_cycstubs;_tmp67;});case 1: _LL9D: return Cyc_spec(lexbuf);
case 2: _LL9E: return Cyc_spec(lexbuf);case 3: _LL9F: return 0;case 4: _LLA0:({struct Cyc_Std_Int_pa_struct
_tmp69;_tmp69.tag=1;_tmp69.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp68[1]={& _tmp69};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(char),67),_tag_arr(_tmp68,sizeof(void*),1));}});return 0;default: _LLA1:(
lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({
struct Cyc_Lexing_Error_struct _tmp6B;_tmp6B.tag=Cyc_Lexing_Error;_tmp6B.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp6B;});_tmp6A;}));}
struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA3: return 0;
case 1: _LLA4: return 0;case 2: _LLA5: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(
lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LLA6: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA7: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp6D->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp6D;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=x;_tmp6C->tl=
Cyc_current_prologue;_tmp6C;});return 1;}case 5: _LLA8: {struct _tagged_arr _tmp6E=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp6E,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("prologue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp6E,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp6E,sizeof(char),1);}{struct _tagged_arr t=_tmp6E;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp6F=Cyc_Std_substring((struct _tagged_arr)
_tmp6E,0,(unsigned int)((t.curr - _tmp6E.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=(
struct _tagged_arr)_tmp6F;_tmp71->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp71;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->hd=x;_tmp70->tl=
Cyc_current_prologue;_tmp70;});return 1;}}}}case 6: _LLA9: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp73->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp73;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=x;_tmp72->tl=
Cyc_current_epilogue;_tmp72;});return 1;}case 7: _LLAA: {struct _tagged_arr _tmp74=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp74,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("epilogue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp74,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp74,sizeof(char),1);}{struct _tagged_arr t=_tmp74;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp75=Cyc_Std_substring((struct _tagged_arr)
_tmp74,0,(unsigned int)((t.curr - _tmp74.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(
struct _tagged_arr)_tmp75;_tmp77->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp77;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=x;_tmp76->tl=
Cyc_current_epilogue;_tmp76;});return 1;}}}}case 8: _LLAB: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp79->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp79;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=x;_tmp78->tl=
Cyc_current_cstubs;_tmp78;});return 1;}case 9: _LLAC: {struct _tagged_arr _tmp7A=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp7A,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cstub",sizeof(char),6)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp7A,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp7A,sizeof(char),1);}{struct _tagged_arr t=_tmp7A;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp7B=Cyc_Std_substring((struct _tagged_arr)
_tmp7A,0,(unsigned int)((t.curr - _tmp7A.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=(
struct _tagged_arr)_tmp7B;_tmp7D->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7D;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->hd=x;_tmp7C->tl=
Cyc_current_cstubs;_tmp7C;});return 1;}}}}case 10: _LLAD: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp7F->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7F;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=x;_tmp7E->tl=
Cyc_current_cycstubs;_tmp7E;});return 1;}case 11: _LLAE: {struct _tagged_arr _tmp80=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp80,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cycstub",sizeof(char),8)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp80,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp80,sizeof(char),1);}{struct _tagged_arr t=_tmp80;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp81=Cyc_Std_substring((struct _tagged_arr)
_tmp80,0,(unsigned int)((t.curr - _tmp80.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->f1=(
struct _tagged_arr)_tmp81;_tmp83->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp83;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=x;_tmp82->tl=
Cyc_current_cycstubs;_tmp82;});return 1;}}}}case 12: _LLAF: Cyc_braces_to_match=1;
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tagged_arr*x=({struct _tagged_arr*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[
0]=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp85;});Cyc_current_cpp=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84->hd=x;_tmp84->tl=Cyc_current_cpp;_tmp84;});return 1;}case 13: _LLB0: return 1;
case 14: _LLB1: return 1;case 15: _LLB2:({struct Cyc_Std_Int_pa_struct _tmp87;_tmp87.tag=
1;_tmp87.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*
_tmp86[1]={& _tmp87};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(char),58),_tag_arr(_tmp86,sizeof(void*),1));}});return 0;default: _LLB3:(
lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88[0]=({struct Cyc_Lexing_Error_struct _tmp89;_tmp89.tag=Cyc_Lexing_Error;
_tmp89.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp89;});
_tmp88;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB5: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->hd=({struct
_tagged_arr*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp8B;});_tmp8A->tl=Cyc_snarfed_symbols;_tmp8A;});return 1;case 1: _LLB6:
return 1;case 2: _LLB7: return 0;case 3: _LLB8:({void*_tmp8C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n",sizeof(char),44),
_tag_arr(_tmp8C,sizeof(void*),0));});return 0;case 4: _LLB9:({struct Cyc_Std_Int_pa_struct
_tmp8E;_tmp8E.tag=1;_tmp8E.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp8D[1]={& _tmp8E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(char),57),_tag_arr(_tmp8D,sizeof(void*),1));}});return 0;default: _LLBA:(
lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));
_tmp8F[0]=({struct Cyc_Lexing_Error_struct _tmp90;_tmp90.tag=Cyc_Lexing_Error;
_tmp90.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp90;});
_tmp8F;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBC:({void*
_tmp91[0]={};Cyc_log(_tag_arr("Warning: unclosed brace\n",sizeof(char),25),
_tag_arr(_tmp91,sizeof(void*),0));});return 0;case 1: _LLBD: if(Cyc_braces_to_match
== 1)return 0;Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLBE: Cyc_braces_to_match ++;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLBF: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(char),3));while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLC3:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp92=_cycalloc(
sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Lexing_Error_struct _tmp93;_tmp93.tag=Cyc_Lexing_Error;
_tmp93.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp93;});
_tmp92;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(
lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC5:({void*
_tmp94[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp94,sizeof(void*),0));});return 0;case 1: _LLC6: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLC7:({void*
_tmp95[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp95,sizeof(void*),0));});Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 3: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLCA: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 6: _LLCB: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLCC: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLCD: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
default: _LLCE:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Lexing_Error_struct _tmp97;_tmp97.tag=Cyc_Lexing_Error;
_tmp97.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp97;});
_tmp96;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLD0:({void*
_tmp98[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp98,sizeof(void*),0));});return 0;case 1: _LLD1: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(char),3));
return 0;case 2: _LLD2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLD3:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({
struct Cyc_Lexing_Error_struct _tmp9A;_tmp9A.tag=Cyc_Lexing_Error;_tmp9A.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp9A;});_tmp99;}));}int
Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp9B=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmp9C;struct
_tuple0 _tmp9D;struct _tagged_arr*_tmp9E;struct _tuple0*_tmp9F;struct _tuple0 _tmpA0;
struct _tagged_arr*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;
struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;
struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;
struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_List_List*_tmpB3;void*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_MallocInfo
_tmpB6;int _tmpB7;struct Cyc_Absyn_Exp*_tmpB8;void**_tmpB9;struct Cyc_Absyn_Exp*
_tmpBA;void*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct _tagged_arr*_tmpBD;struct Cyc_Absyn_Exp*
_tmpBE;struct _tagged_arr*_tmpBF;void*_tmpC0;void*_tmpC1;struct Cyc_List_List*
_tmpC2;_LLD6: if(*((int*)_tmp9B)!= 1)goto _LLD8;_tmp9C=((struct Cyc_Absyn_Var_e_struct*)
_tmp9B)->f1;_tmp9D=*_tmp9C;_tmp9E=_tmp9D.f2;_LLD7: _tmpA1=_tmp9E;goto _LLD9;_LLD8:
if(*((int*)_tmp9B)!= 2)goto _LLDA;_tmp9F=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp9B)->f1;_tmpA0=*_tmp9F;_tmpA1=_tmpA0.f2;_LLD9: Cyc_add_target(_tmpA1);return;
_LLDA: if(*((int*)_tmp9B)!= 3)goto _LLDC;_tmpA2=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9B)->f2;_LLDB: for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpA2->hd));}return;_LLDC: if(*((int*)_tmp9B)!= 23)goto
_LLDE;_tmpA3=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9B)->f1;_tmpA4=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp9B)->f2;_LLDD: _tmpA5=_tmpA3;_tmpA6=_tmpA4;goto
_LLDF;_LLDE: if(*((int*)_tmp9B)!= 7)goto _LLE0;_tmpA5=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9B)->f1;_tmpA6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9B)->f2;_LLDF: _tmpA7=
_tmpA5;_tmpA8=_tmpA6;goto _LLE1;_LLE0: if(*((int*)_tmp9B)!= 4)goto _LLE2;_tmpA7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9B)->f1;_tmpA8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9B)->f3;_LLE1: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA7);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpA8);return;_LLE2: if(*((int*)_tmp9B)!= 20)goto _LLE4;
_tmpA9=((struct Cyc_Absyn_Deref_e_struct*)_tmp9B)->f1;_LLE3: _tmpAA=_tmpA9;goto
_LLE5;_LLE4: if(*((int*)_tmp9B)!= 17)goto _LLE6;_tmpAA=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9B)->f1;_LLE5: _tmpAB=_tmpAA;goto _LLE7;_LLE6: if(*((int*)_tmp9B)!= 14)goto
_LLE8;_tmpAB=((struct Cyc_Absyn_Address_e_struct*)_tmp9B)->f1;_LLE7: _tmpAC=_tmpAB;
goto _LLE9;_LLE8: if(*((int*)_tmp9B)!= 5)goto _LLEA;_tmpAC=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9B)->f1;_LLE9: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAC);return;_LLEA: if(*((
int*)_tmp9B)!= 6)goto _LLEC;_tmpAD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9B)->f1;
_tmpAE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9B)->f2;_tmpAF=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9B)->f3;_LLEB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAD);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpAE);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAF);return;
_LLEC: if(*((int*)_tmp9B)!= 9)goto _LLEE;_tmpB0=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp9B)->f1;_tmpB1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9B)->f2;_LLED: _tmpB2=
_tmpB0;_tmpB3=_tmpB1;goto _LLEF;_LLEE: if(*((int*)_tmp9B)!= 8)goto _LLF0;_tmpB2=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9B)->f1;_tmpB3=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp9B)->f2;_LLEF: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB2);for(0;_tmpB3 != 0;
_tmpB3=_tmpB3->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpB3->hd));}return;_LLF0: if(*((int*)_tmp9B)!= 13)goto _LLF2;_tmpB4=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9B)->f1;_tmpB5=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9B)->f2;_LLF1: Cyc_scan_type(_tmpB4);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpB5);return;_LLF2: if(*((int*)_tmp9B)!= 33)goto _LLF4;_tmpB6=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9B)->f1;_tmpB7=_tmpB6.is_calloc;_tmpB8=_tmpB6.rgn;_tmpB9=_tmpB6.elt_type;
_tmpBA=_tmpB6.num_elts;_LLF3: if(_tmpB8 != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpB8)));if(_tmpB9 != 0)Cyc_scan_type(*_tmpB9);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBA);return;_LLF4: if(*((int*)_tmp9B)!= 16)
goto _LLF6;_tmpBB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9B)->f1;_LLF5:
Cyc_scan_type(_tmpBB);return;_LLF6: if(*((int*)_tmp9B)!= 21)goto _LLF8;_tmpBC=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9B)->f1;_tmpBD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9B)->f2;_LLF7: _tmpBE=_tmpBC;_tmpBF=_tmpBD;goto _LLF9;_LLF8: if(*((int*)_tmp9B)
!= 22)goto _LLFA;_tmpBE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9B)->f1;_tmpBF=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9B)->f2;_LLF9: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpBE);Cyc_add_target(_tmpBF);return;_LLFA: if(*((int*)_tmp9B)!= 18)goto _LLFC;
_tmpC0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp9B)->f1;_tmpC1=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp9B)->f2;_LLFB: Cyc_scan_type(_tmpC0);{void*
_tmpC3=_tmpC1;struct _tagged_arr*_tmpC4;_LL123: if(*((int*)_tmpC3)!= 0)goto _LL125;
_tmpC4=((struct Cyc_Absyn_StructField_struct*)_tmpC3)->f1;_LL124: Cyc_add_target(
_tmpC4);goto _LL122;_LL125: if(*((int*)_tmpC3)!= 1)goto _LL122;_LL126: goto _LL122;
_LL122:;}return;_LLFC: if(*((int*)_tmp9B)!= 0)goto _LLFE;_LLFD: return;_LLFE: if(*((
int*)_tmp9B)!= 34)goto _LL100;_tmpC2=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp9B)->f2;_LLFF: for(0;_tmpC2 != 0;_tmpC2=_tmpC2->tl){struct _tuple13 _tmpC6;
struct Cyc_Absyn_Exp*_tmpC7;struct _tuple13*_tmpC5=(struct _tuple13*)_tmpC2->hd;
_tmpC6=*_tmpC5;_tmpC7=_tmpC6.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC7);}
return;_LL100: if(*((int*)_tmp9B)!= 35)goto _LL102;_LL101:({void*_tmpC8[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",sizeof(char),26),_tag_arr(
_tmpC8,sizeof(void*),0));});exit(1);return;_LL102: if(*((int*)_tmp9B)!= 10)goto
_LL104;_LL103:({void*_tmpC9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(char),27),_tag_arr(_tmpC9,sizeof(void*),0));});exit(1);return;_LL104: if(*((
int*)_tmp9B)!= 11)goto _LL106;_LL105:({void*_tmpCA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected NoInstantiate_e\n",sizeof(char),35),_tag_arr(_tmpCA,
sizeof(void*),0));});exit(1);return;_LL106: if(*((int*)_tmp9B)!= 12)goto _LL108;
_LL107:({void*_tmpCB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",
sizeof(char),33),_tag_arr(_tmpCB,sizeof(void*),0));});exit(1);return;_LL108: if(*((
int*)_tmp9B)!= 15)goto _LL10A;_LL109:({void*_tmpCC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected New_e\n",sizeof(char),25),_tag_arr(_tmpCC,sizeof(
void*),0));});exit(1);return;_LL10A: if(*((int*)_tmp9B)!= 19)goto _LL10C;_LL10B:({
void*_tmpCD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(char),28),_tag_arr(_tmpCD,sizeof(void*),0));});exit(1);return;_LL10C: if(*((
int*)_tmp9B)!= 24)goto _LL10E;_LL10D:({void*_tmpCE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tuple_e\n",sizeof(char),27),_tag_arr(_tmpCE,sizeof(
void*),0));});exit(1);return;_LL10E: if(*((int*)_tmp9B)!= 25)goto _LL110;_LL10F:({
void*_tmpCF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(char),33),_tag_arr(_tmpCF,sizeof(void*),0));});exit(1);return;_LL110: if(*((
int*)_tmp9B)!= 26)goto _LL112;_LL111:({void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Array_e\n",sizeof(char),27),_tag_arr(_tmpD0,sizeof(
void*),0));});exit(1);return;_LL112: if(*((int*)_tmp9B)!= 27)goto _LL114;_LL113:({
void*_tmpD1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(char),35),_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;_LL114: if(*((
int*)_tmp9B)!= 28)goto _LL116;_LL115:({void*_tmpD2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Struct_e\n",sizeof(char),28),_tag_arr(_tmpD2,sizeof(
void*),0));});exit(1);return;_LL116: if(*((int*)_tmp9B)!= 29)goto _LL118;_LL117:({
void*_tmpD3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(char),32),_tag_arr(_tmpD3,sizeof(void*),0));});exit(1);return;_LL118: if(*((
int*)_tmp9B)!= 30)goto _LL11A;_LL119:({void*_tmpD4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tunion_e\n",sizeof(char),28),_tag_arr(_tmpD4,sizeof(
void*),0));});exit(1);return;_LL11A: if(*((int*)_tmp9B)!= 31)goto _LL11C;_LL11B:({
void*_tmpD5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(char),26),_tag_arr(_tmpD5,sizeof(void*),0));});exit(1);return;_LL11C: if(*((
int*)_tmp9B)!= 32)goto _LL11E;_LL11D:({void*_tmpD6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonEnum_e\n",sizeof(char),30),_tag_arr(_tmpD6,
sizeof(void*),0));});exit(1);return;_LL11E: if(*((int*)_tmp9B)!= 36)goto _LL120;
_LL11F:({void*_tmpD7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Codegen_e\n",
sizeof(char),29),_tag_arr(_tmpD7,sizeof(void*),0));});exit(1);return;_LL120: if(*((
int*)_tmp9B)!= 37)goto _LLD5;_LL121:({void*_tmpD8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Fill_e\n",sizeof(char),26),_tag_arr(_tmpD8,sizeof(
void*),0));});exit(1);return;_LLD5:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){
if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*
_tmpD9=t;struct Cyc_Absyn_PtrInfo _tmpDA;void*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;
struct Cyc_Absyn_FnInfo _tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Absyn_AggrInfo
_tmpDF;void*_tmpE0;struct _tuple0*_tmpE1;struct _tuple0 _tmpE2;struct _tagged_arr*
_tmpE3;struct _tuple0*_tmpE4;struct _tuple0 _tmpE5;struct _tagged_arr*_tmpE6;_LL128:
if((int)_tmpD9 != 0)goto _LL12A;_LL129: goto _LL12B;_LL12A: if(_tmpD9 <= (void*)3?1:*((
int*)_tmpD9)!= 5)goto _LL12C;_LL12B: goto _LL12D;_LL12C: if((int)_tmpD9 != 1)goto
_LL12E;_LL12D: goto _LL12F;_LL12E: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 6)goto
_LL130;_LL12F: return;_LL130: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 4)goto
_LL132;_tmpDA=((struct Cyc_Absyn_PointerType_struct*)_tmpD9)->f1;_LL131: Cyc_scan_type((
void*)_tmpDA.elt_typ);return;_LL132: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 7)
goto _LL134;_tmpDB=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD9)->f1;_tmpDC=((
struct Cyc_Absyn_ArrayType_struct*)_tmpD9)->f3;_LL133: Cyc_scan_type(_tmpDB);Cyc_scan_exp_opt(
_tmpDC);return;_LL134: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 8)goto _LL136;
_tmpDD=((struct Cyc_Absyn_FnType_struct*)_tmpD9)->f1;_LL135: Cyc_scan_type((void*)
_tmpDD.ret_typ);{struct Cyc_List_List*_tmpE7=_tmpDD.args;for(0;_tmpE7 != 0;_tmpE7=
_tmpE7->tl){struct _tuple1 _tmpE9;void*_tmpEA;struct _tuple1*_tmpE8=(struct _tuple1*)
_tmpE7->hd;_tmpE9=*_tmpE8;_tmpEA=_tmpE9.f3;Cyc_scan_type(_tmpEA);}}if(_tmpDD.cyc_varargs
!= 0)Cyc_scan_type((void*)(_tmpDD.cyc_varargs)->type);return;_LL136: if(_tmpD9 <= (
void*)3?1:*((int*)_tmpD9)!= 11)goto _LL138;_tmpDE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpD9)->f2;_LL137: for(0;_tmpDE != 0;_tmpDE=_tmpDE->tl){Cyc_scan_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmpDE->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)
_tmpDE->hd)->width);}return;_LL138: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 13)
goto _LL13A;_LL139: return;_LL13A: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 10)goto
_LL13C;_tmpDF=((struct Cyc_Absyn_AggrType_struct*)_tmpD9)->f1;_tmpE0=(void*)
_tmpDF.aggr_info;_LL13B: {struct _tuple0*_tmpEC;struct _tuple0 _tmpED;struct
_tagged_arr*_tmpEE;struct _tuple3 _tmpEB=Cyc_Absyn_aggr_kinded_name(_tmpE0);_tmpEC=
_tmpEB.f2;_tmpED=*_tmpEC;_tmpEE=_tmpED.f2;_tmpE3=_tmpEE;goto _LL13D;}_LL13C: if(
_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 12)goto _LL13E;_tmpE1=((struct Cyc_Absyn_EnumType_struct*)
_tmpD9)->f1;_tmpE2=*_tmpE1;_tmpE3=_tmpE2.f2;_LL13D: _tmpE6=_tmpE3;goto _LL13F;
_LL13E: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 16)goto _LL140;_tmpE4=((struct Cyc_Absyn_TypedefType_struct*)
_tmpD9)->f1;_tmpE5=*_tmpE4;_tmpE6=_tmpE5.f2;_LL13F: Cyc_add_target(_tmpE6);
return;_LL140: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 0)goto _LL142;_LL141:({
void*_tmpEF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Evar\n",
sizeof(char),24),_tag_arr(_tmpEF,sizeof(void*),0));});exit(1);return;_LL142: if(
_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 1)goto _LL144;_LL143:({void*_tmpF0[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected VarType\n",sizeof(
char),27),_tag_arr(_tmpF0,sizeof(void*),0));});exit(1);return;_LL144: if(_tmpD9 <= (
void*)3?1:*((int*)_tmpD9)!= 2)goto _LL146;_LL145:({void*_tmpF1[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected TunionType\n",sizeof(char),30),
_tag_arr(_tmpF1,sizeof(void*),0));});exit(1);return;_LL146: if(_tmpD9 <= (void*)3?
1:*((int*)_tmpD9)!= 3)goto _LL148;_LL147:({void*_tmpF2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionFieldType\n",sizeof(char),35),_tag_arr(_tmpF2,
sizeof(void*),0));});exit(1);return;_LL148: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)
!= 9)goto _LL14A;_LL149:({void*_tmpF3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n",sizeof(char),29),_tag_arr(_tmpF3,
sizeof(void*),0));});exit(1);return;_LL14A: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)
!= 15)goto _LL14C;_LL14B:({void*_tmpF4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnHandleType\n",sizeof(char),33),_tag_arr(_tmpF4,
sizeof(void*),0));});exit(1);return;_LL14C: if((int)_tmpD9 != 2)goto _LL14E;_LL14D:({
void*_tmpF5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(char),27),_tag_arr(_tmpF5,sizeof(void*),0));});exit(1);return;_LL14E: if(
_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 17)goto _LL150;_LL14F:({void*_tmpF6[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",sizeof(
char),29),_tag_arr(_tmpF6,sizeof(void*),0));});exit(1);return;_LL150: if(_tmpD9 <= (
void*)3?1:*((int*)_tmpD9)!= 18)goto _LL152;_LL151:({void*_tmpF7[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",sizeof(char),27),_tag_arr(
_tmpF7,sizeof(void*),0));});exit(1);return;_LL152: if(_tmpD9 <= (void*)3?1:*((int*)
_tmpD9)!= 19)goto _LL154;_LL153:({void*_tmpF8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnsEff\n",sizeof(char),27),_tag_arr(_tmpF8,sizeof(
void*),0));});exit(1);return;_LL154: if(_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 14)
goto _LL127;_LL155:({void*_tmpF9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(char),28),_tag_arr(_tmpF9,sizeof(void*),0));});exit(1);return;_LL127:;}
struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple9*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpFA;});{
void*_tmpFB=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpFC;struct Cyc_Absyn_Fndecl*
_tmpFD;struct Cyc_Absyn_Aggrdecl*_tmpFE;struct Cyc_Absyn_Enumdecl*_tmpFF;struct Cyc_Absyn_Typedefdecl*
_tmp100;_LL157: if(*((int*)_tmpFB)!= 0)goto _LL159;_tmpFC=((struct Cyc_Absyn_Var_d_struct*)
_tmpFB)->f1;_LL158: {struct _tuple0 _tmp102;struct _tagged_arr*_tmp103;struct
_tuple0*_tmp101=_tmpFC->name;_tmp102=*_tmp101;_tmp103=_tmp102.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp103;Cyc_scan_type((void*)_tmpFC->type);Cyc_scan_exp_opt(
_tmpFC->initializer);goto _LL156;}_LL159: if(*((int*)_tmpFB)!= 1)goto _LL15B;_tmpFD=((
struct Cyc_Absyn_Fn_d_struct*)_tmpFB)->f1;_LL15A: {struct _tuple0 _tmp105;struct
_tagged_arr*_tmp106;struct _tuple0*_tmp104=_tmpFD->name;_tmp105=*_tmp104;_tmp106=
_tmp105.f2;Cyc_current_source=(struct _tagged_arr*)_tmp106;Cyc_scan_type((void*)
_tmpFD->ret_type);{struct Cyc_List_List*_tmp107=_tmpFD->args;for(0;_tmp107 != 0;
_tmp107=_tmp107->tl){struct _tuple14 _tmp109;void*_tmp10A;struct _tuple14*_tmp108=(
struct _tuple14*)_tmp107->hd;_tmp109=*_tmp108;_tmp10A=_tmp109.f3;Cyc_scan_type(
_tmp10A);}}if(_tmpFD->cyc_varargs != 0)Cyc_scan_type((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmpFD->cyc_varargs))->type);if(_tmpFD->is_inline)({void*_tmp10B[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: ignoring inline function\n",
sizeof(char),35),_tag_arr(_tmp10B,sizeof(void*),0));});goto _LL156;}_LL15B: if(*((
int*)_tmpFB)!= 4)goto _LL15D;_tmpFE=((struct Cyc_Absyn_Aggr_d_struct*)_tmpFB)->f1;
_LL15C: {struct _tuple0 _tmp10D;struct _tagged_arr*_tmp10E;struct _tuple0*_tmp10C=
_tmpFE->name;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.f2;Cyc_current_source=(struct
_tagged_arr*)_tmp10E;if((unsigned int)_tmpFE->fields){{struct Cyc_List_List*
_tmp10F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpFE->fields))->v;
for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){struct Cyc_Absyn_Aggrfield*_tmp110=(struct
Cyc_Absyn_Aggrfield*)_tmp10F->hd;Cyc_scan_type((void*)_tmp110->type);Cyc_scan_exp_opt(
_tmp110->width);}}{struct Cyc_List_List*_tmp111=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpFE->fields))->v;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){;}}}goto
_LL156;}_LL15D: if(*((int*)_tmpFB)!= 6)goto _LL15F;_tmpFF=((struct Cyc_Absyn_Enum_d_struct*)
_tmpFB)->f1;_LL15E: {struct _tuple0 _tmp113;struct _tagged_arr*_tmp114;struct
_tuple0*_tmp112=_tmpFF->name;_tmp113=*_tmp112;_tmp114=_tmp113.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp114;if((unsigned int)_tmpFF->fields){{struct Cyc_List_List*
_tmp115=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpFF->fields))->v;
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){struct Cyc_Absyn_Enumfield*_tmp116=(struct
Cyc_Absyn_Enumfield*)_tmp115->hd;Cyc_scan_exp_opt(_tmp116->tag);}}{struct Cyc_List_List*
_tmp117=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpFF->fields))->v;
for(0;_tmp117 != 0;_tmp117=_tmp117->tl){;}}}goto _LL156;}_LL15F: if(*((int*)_tmpFB)
!= 7)goto _LL161;_tmp100=((struct Cyc_Absyn_Typedef_d_struct*)_tmpFB)->f1;_LL160: {
struct _tuple0 _tmp119;struct _tagged_arr*_tmp11A;struct _tuple0*_tmp118=_tmp100->name;
_tmp119=*_tmp118;_tmp11A=_tmp119.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp11A;if((unsigned int)_tmp100->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp100->defn))->v);goto _LL156;}_LL161: if(*((int*)_tmpFB)!= 2)goto
_LL163;_LL162:({void*_tmp11B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected let declaration\n",
sizeof(char),35),_tag_arr(_tmp11B,sizeof(void*),0));});exit(1);return 0;_LL163:
if(*((int*)_tmpFB)!= 5)goto _LL165;_LL164:({void*_tmp11C[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected tunion declaration\n",sizeof(char),38),
_tag_arr(_tmp11C,sizeof(void*),0));});exit(1);return 0;_LL165: if(*((int*)_tmpFB)
!= 3)goto _LL167;_LL166:({void*_tmp11D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(char),35),_tag_arr(_tmp11D,
sizeof(void*),0));});exit(1);return 0;_LL167: if(*((int*)_tmpFB)!= 8)goto _LL169;
_LL168:({void*_tmp11E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected namespace declaration\n",
sizeof(char),41),_tag_arr(_tmp11E,sizeof(void*),0));});exit(1);return 0;_LL169:
if(*((int*)_tmpFB)!= 9)goto _LL16B;_LL16A:({void*_tmp11F[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected using declaration\n",sizeof(char),37),
_tag_arr(_tmp11F,sizeof(void*),0));});exit(1);return 0;_LL16B: if(*((int*)_tmpFB)
!= 10)goto _LL156;_LL16C:({void*_tmp120[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n",sizeof(char),42),
_tag_arr(_tmp120,sizeof(void*),0));});exit(1);return 0;_LL156:;}return({struct
_tuple9*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp121->f2=*((struct Cyc_Set_Set**)_check_null(
Cyc_current_targets));_tmp121;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp122;_push_handler(& _tmp122);{int
_tmp124=0;if(setjmp(_tmp122.handler))_tmp124=1;if(!_tmp124){{struct Cyc_Set_Set*
_tmp125=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp125;};_pop_handler();}
else{void*_tmp123=(void*)_exn_thrown;void*_tmp127=_tmp123;_LL16E: if(_tmp127 != 
Cyc_Core_Not_found)goto _LL170;_LL16F: return((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL170:;
_LL171:(void)_throw(_tmp127);_LL16D:;}}}struct Cyc_Set_Set*Cyc_reachable(struct
Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp128=curr;struct
_tagged_arr*_tmp129=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp128)
> 0){struct Cyc_Set_Set*_tmp12A=emptyset;struct Cyc_Iter_Iter _tmp12B=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp128);while(((int(*)(struct Cyc_Iter_Iter,struct _tagged_arr**))Cyc_Iter_next)(
_tmp12B,& _tmp129)){_tmp12A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp12A,Cyc_find(t,_tmp129));}_tmp128=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp12A,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp128);}return curr;}}static void*Cyc_mode=(void*)0;
static int Cyc_gathering(){return Cyc_mode == (void*)1?1: Cyc_mode == (void*)2;}static
struct Cyc_Std___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_script_file == 0){({void*_tmp12D[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Internal error: script file is NULL\n",sizeof(char),37),
_tag_arr(_tmp12D,sizeof(void*),0));});exit(1);}return Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file),fmt,ap);}int Cyc_force_directory(struct _tagged_arr d){
if(Cyc_mode == (void*)2)({struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _tagged_arr)d;{struct Cyc_Std_String_pa_struct _tmp12F;_tmp12F.tag=
0;_tmp12F.f1=(struct _tagged_arr)d;{void*_tmp12E[2]={& _tmp12F,& _tmp130};Cyc_prscript(
_tag_arr("if ! test -e %s; then mkdir %s; fi\n",sizeof(char),36),_tag_arr(
_tmp12E,sizeof(void*),2));}}});else{int _tmp131=({unsigned int _tmp134[0];Cyc_Std_open(
d,0,_tag_arr(_tmp134,sizeof(unsigned int),0));});if(_tmp131 == - 1){if(Cyc_Std_mkdir(
d,448)== - 1){({struct Cyc_Std_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(
struct _tagged_arr)d;{void*_tmp132[1]={& _tmp133};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp132,sizeof(void*),1));}});return 1;}}else{close(_tmp131);}}return 0;}int Cyc_force_directory_prefixes(
struct _tagged_arr file){struct _tagged_arr _tmp135=Cyc_Std_strdup(file);struct Cyc_List_List*
_tmp136=0;while(1){_tmp135=Cyc_Filename_dirname((struct _tagged_arr)_tmp135);if(
_get_arr_size(_tmp135,sizeof(char))== 0)break;_tmp136=({struct Cyc_List_List*
_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=({struct _tagged_arr*_tmp138=
_cycalloc(sizeof(*_tmp138));_tmp138[0]=(struct _tagged_arr)_tmp135;_tmp138;});
_tmp137->tl=_tmp136;_tmp137;});}for(0;_tmp136 != 0;_tmp136=_tmp136->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)_tmp136->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT";
int Cyc_process_file(struct _tagged_arr filename,struct Cyc_List_List*start_symbols,
struct Cyc_List_List*omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*
epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*
maybe;struct Cyc_Std___cycFILE*in_file;struct Cyc_Std___cycFILE*out_file;int
errorcode=0;({struct Cyc_Std_String_pa_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(
struct _tagged_arr)filename;{void*_tmp139[1]={& _tmp13A};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("********************************* %s...\n",sizeof(char),41),_tag_arr(
_tmp139,sizeof(void*),1));}});if(!Cyc_gathering())({struct Cyc_Std_String_pa_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _tagged_arr)filename;{void*_tmp13B[1]={&
_tmp13C};Cyc_log(_tag_arr("\n%s:\n",sizeof(char),6),_tag_arr(_tmp13B,sizeof(void*),
1));}});{struct _tagged_arr _tmp13D=Cyc_Filename_basename(filename);struct
_tagged_arr _tmp13E=Cyc_Filename_dirname(filename);struct _tagged_arr _tmp13F=Cyc_Filename_chop_extension((
struct _tagged_arr)_tmp13D);struct _tagged_arr _tmp140=Cyc_Std_strconcat((struct
_tagged_arr)_tmp13F,_tag_arr(".iA",sizeof(char),4));struct _tagged_arr _tmp141=
_get_arr_size(_tmp13E,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp210;
_tmp210.tag=0;_tmp210.f1=(struct _tagged_arr)_tmp13F;{void*_tmp20F[1]={& _tmp210};
Cyc_Std_aprintf(_tag_arr("%s.iB",sizeof(char),6),_tag_arr(_tmp20F,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp13E,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp212;_tmp212.tag=0;_tmp212.f1=(struct
_tagged_arr)_tmp13F;{void*_tmp211[1]={& _tmp212};Cyc_Std_aprintf(_tag_arr("%s.iB",
sizeof(char),6),_tag_arr(_tmp211,sizeof(void*),1));}}));struct _tagged_arr _tmp142=
_get_arr_size(_tmp13E,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp20C;
_tmp20C.tag=0;_tmp20C.f1=(struct _tagged_arr)_tmp13F;{void*_tmp20B[1]={& _tmp20C};
Cyc_Std_aprintf(_tag_arr("%s.iC",sizeof(char),6),_tag_arr(_tmp20B,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp13E,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=(struct
_tagged_arr)_tmp13F;{void*_tmp20D[1]={& _tmp20E};Cyc_Std_aprintf(_tag_arr("%s.iC",
sizeof(char),6),_tag_arr(_tmp20D,sizeof(void*),1));}}));struct _tagged_arr _tmp143=
_get_arr_size(_tmp13E,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp208;
_tmp208.tag=0;_tmp208.f1=(struct _tagged_arr)_tmp13F;{void*_tmp207[1]={& _tmp208};
Cyc_Std_aprintf(_tag_arr("%s.iD",sizeof(char),6),_tag_arr(_tmp207,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp13E,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp20A;_tmp20A.tag=0;_tmp20A.f1=(struct
_tagged_arr)_tmp13F;{void*_tmp209[1]={& _tmp20A};Cyc_Std_aprintf(_tag_arr("%s.iD",
sizeof(char),6),_tag_arr(_tmp209,sizeof(void*),1));}}));struct _tagged_arr _tmp144=
_get_arr_size(_tmp13E,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp204;
_tmp204.tag=0;_tmp204.f1=(struct _tagged_arr)_tmp13D;{void*_tmp203[1]={& _tmp204};
Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp203,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp13E,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp206;_tmp206.tag=0;_tmp206.f1=(struct _tagged_arr)_tmp13D;{void*_tmp205[1]={&
_tmp206};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp205,sizeof(
void*),1));}}));struct _handler_cons _tmp145;_push_handler(& _tmp145);{int _tmp147=0;
if(setjmp(_tmp145.handler))_tmp147=1;if(!_tmp147){if(Cyc_force_directory_prefixes(
filename)){int _tmp148=1;_npop_handler(0);return _tmp148;}if(Cyc_mode != (void*)3){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct _tmp14A;
_tmp14A.tag=0;_tmp14A.f1=(struct _tagged_arr)_tmp140;{void*_tmp149[1]={& _tmp14A};
Cyc_prscript(_tag_arr("cat >%s <<XXX\n",sizeof(char),15),_tag_arr(_tmp149,
sizeof(void*),1));}});{struct Cyc_List_List*_tmp14B=Cyc_current_cpp;for(0;_tmp14B
!= 0;_tmp14B=_tmp14B->tl){({struct Cyc_Std_String_pa_struct _tmp14D;_tmp14D.tag=0;
_tmp14D.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp14B->hd);{void*_tmp14C[1]={&
_tmp14D};Cyc_prscript(_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp14C,sizeof(void*),
1));}});}}({struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(
struct _tagged_arr)filename;{void*_tmp14E[1]={& _tmp14F};Cyc_prscript(_tag_arr("#include <%s>\n",
sizeof(char),15),_tag_arr(_tmp14E,sizeof(void*),1));}});({void*_tmp150[0]={};Cyc_prscript(
_tag_arr("XXX\n",sizeof(char),5),_tag_arr(_tmp150,sizeof(void*),0));});({struct
Cyc_Std_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)
_tmp140;{struct Cyc_Std_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct
_tagged_arr)_tmp141;{void*_tmp151[2]={& _tmp152,& _tmp153};Cyc_prscript(_tag_arr("$GCC -E -dM -o %s -x c %s && \\\n",
sizeof(char),32),_tag_arr(_tmp151,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp156;_tmp156.tag=0;_tmp156.f1=(struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct
_tmp155;_tmp155.tag=0;_tmp155.f1=(struct _tagged_arr)_tmp142;{void*_tmp154[2]={&
_tmp155,& _tmp156};Cyc_prscript(_tag_arr("$GCC -E     -o %s -x c %s;\n",sizeof(
char),28),_tag_arr(_tmp154,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)_tmp140;{void*_tmp157[1]={&
_tmp158};Cyc_prscript(_tag_arr("rm %s\n",sizeof(char),7),_tag_arr(_tmp157,
sizeof(void*),1));}});}else{maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp140,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)_tmp140;{void*_tmp159[1]={&
_tmp15A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp159,sizeof(void*),1));}});{int _tmp15B=1;
_npop_handler(0);return _tmp15B;}}out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);{struct Cyc_List_List*_tmp15C=Cyc_current_cpp;for(0;_tmp15C != 0;_tmp15C=
_tmp15C->tl){Cyc_Std_fputs(*((struct _tagged_arr*)_tmp15C->hd),out_file);}}({
struct Cyc_Std_String_pa_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct
_tagged_arr)filename;{void*_tmp15D[1]={& _tmp15E};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n",sizeof(char),15),_tag_arr(_tmp15D,sizeof(void*),1));}});
Cyc_Std_fclose(out_file);{struct _tagged_arr _tmp15F=Cstring_to_string(Ccomp);
struct _tagged_arr _tmp160=({struct Cyc_Std_String_pa_struct _tmp168;_tmp168.tag=0;
_tmp168.f1=(struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct _tmp167;
_tmp167.tag=0;_tmp167.f1=(struct _tagged_arr)_tmp141;{struct Cyc_Std_String_pa_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=(struct _tagged_arr)_tmp15F;{void*_tmp165[3]={&
_tmp166,& _tmp167,& _tmp168};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(char),24),_tag_arr(_tmp165,sizeof(void*),3));}}}});if(!Cyc_Std_system((
struct _tagged_arr)_tmp160)){_tmp160=({struct Cyc_Std_String_pa_struct _tmp164;
_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct
_tmp163;_tmp163.tag=0;_tmp163.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct
_tmp162;_tmp162.tag=0;_tmp162.f1=(struct _tagged_arr)_tmp15F;{void*_tmp161[3]={&
_tmp162,& _tmp163,& _tmp164};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",
sizeof(char),20),_tag_arr(_tmp161,sizeof(void*),3));}}}});Cyc_Std_system((struct
_tagged_arr)_tmp160);}Cyc_Std_remove((struct _tagged_arr)_tmp140);}}}if(Cyc_gathering()){
int _tmp169=0;_npop_handler(0);return _tmp169;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp141,_tag_arr("r",sizeof(char),2));if(
!((unsigned int)maybe))(int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);{struct Cyc_Lexing_lexbuf*_tmp16A=Cyc_Lexing_from_file(in_file);
struct _tuple9*entry;while((entry=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp16A))!= 0){struct _tuple9 _tmp16C;struct _tagged_arr*_tmp16D;
struct Cyc_Set_Set*_tmp16E;struct _tuple9*_tmp16B=(struct _tuple9*)_check_null(
entry);_tmp16C=*_tmp16B;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;((void(*)(struct
Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp16D,_tmp16E);}Cyc_Std_fclose(in_file);maybe=Cyc_Std_fopen((struct
_tagged_arr)_tmp142,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(int)
_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp16A=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_Std_fopen((struct
_tagged_arr)_tmp143,_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_slurp_out)){
int _tmp16F=1;_npop_handler(0);return _tmp16F;}while(((int(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_slurp)(_tmp16A)){;}Cyc_Std_fclose(in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)Cyc_Std_remove((struct
_tagged_arr)_tmp142);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp143,_tag_arr("r",
sizeof(char),2));if(!((unsigned int)maybe)){int _tmp170=1;_npop_handler(0);return
_tmp170;}in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_Position_reset_position((
struct _tagged_arr)_tmp143);Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp171=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp143);{struct Cyc_List_List*_tmp172=_tmp171;for(0;_tmp172 != 0;
_tmp172=_tmp172->tl){struct _tuple9*_tmp173=Cyc_scan_decl((struct Cyc_Absyn_Decl*)
_tmp172->hd);if(_tmp173 == 0)continue;{struct _tuple9 _tmp175;struct _tagged_arr*
_tmp176;struct Cyc_Set_Set*_tmp177;struct _tuple9*_tmp174=(struct _tuple9*)
_check_null(_tmp173);_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;{
struct Cyc_Set_Set*old;{struct _handler_cons _tmp178;_push_handler(& _tmp178);{int
_tmp17A=0;if(setjmp(_tmp178.handler))_tmp17A=1;if(!_tmp17A){old=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,
_tmp176);;_pop_handler();}else{void*_tmp179=(void*)_exn_thrown;void*_tmp17C=
_tmp179;_LL173: if(_tmp17C != Cyc_Core_Not_found)goto _LL175;_LL174: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL172;_LL175:;_LL176:(void)_throw(_tmp17C);_LL172:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp176,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp177,old));}}}}{struct Cyc_Set_Set*_tmp17D=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp17E=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp17F=_tmp171;for(0;_tmp17F != 0;_tmp17F=_tmp17F->tl){
struct Cyc_Absyn_Decl*_tmp180=(struct Cyc_Absyn_Decl*)_tmp17F->hd;struct
_tagged_arr*name;{void*_tmp181=(void*)_tmp180->r;struct Cyc_Absyn_Vardecl*_tmp182;
struct Cyc_Absyn_Fndecl*_tmp183;struct Cyc_Absyn_Aggrdecl*_tmp184;struct Cyc_Absyn_Enumdecl*
_tmp185;struct Cyc_Absyn_Typedefdecl*_tmp186;_LL178: if(*((int*)_tmp181)!= 0)goto
_LL17A;_tmp182=((struct Cyc_Absyn_Var_d_struct*)_tmp181)->f1;_LL179: {struct
_tuple0 _tmp188;struct _tagged_arr*_tmp189;struct _tuple0*_tmp187=_tmp182->name;
_tmp188=*_tmp187;_tmp189=_tmp188.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp189);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp189))name=0;else{name=(struct _tagged_arr*)_tmp189;}goto _LL177;}
_LL17A: if(*((int*)_tmp181)!= 1)goto _LL17C;_tmp183=((struct Cyc_Absyn_Fn_d_struct*)
_tmp181)->f1;_LL17B: {struct _tuple0 _tmp18B;struct _tagged_arr*_tmp18C;struct
_tuple0*_tmp18A=_tmp183->name;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp18C);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp18C))name=0;else{name=(struct _tagged_arr*)
_tmp18C;}goto _LL177;}_LL17C: if(*((int*)_tmp181)!= 4)goto _LL17E;_tmp184=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp181)->f1;_LL17D: {struct _tuple0 _tmp18E;struct
_tagged_arr*_tmp18F;struct _tuple0*_tmp18D=_tmp184->name;_tmp18E=*_tmp18D;_tmp18F=
_tmp18E.f2;name=(struct _tagged_arr*)_tmp18F;goto _LL177;}_LL17E: if(*((int*)
_tmp181)!= 6)goto _LL180;_tmp185=((struct Cyc_Absyn_Enum_d_struct*)_tmp181)->f1;
_LL17F: {struct _tuple0 _tmp191;struct _tagged_arr*_tmp192;struct _tuple0*_tmp190=
_tmp185->name;_tmp191=*_tmp190;_tmp192=_tmp191.f2;name=(struct _tagged_arr*)
_tmp192;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp17D,(struct _tagged_arr*)_check_null(name)): 0)_tmp17E=({struct Cyc_List_List*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=_tmp180;_tmp193->tl=_tmp17E;
_tmp193;});else{if((unsigned int)_tmp185->fields){struct Cyc_List_List*_tmp194=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp185->fields))->v;for(
0;_tmp194 != 0;_tmp194=_tmp194->tl){struct Cyc_Absyn_Enumfield*_tmp195=(struct Cyc_Absyn_Enumfield*)
_tmp194->hd;struct _tuple0 _tmp197;struct _tagged_arr*_tmp198;struct _tuple0*_tmp196=
_tmp195->name;_tmp197=*_tmp196;_tmp198=_tmp197.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp17D,_tmp198)){_tmp17E=({struct Cyc_List_List*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=_tmp180;_tmp199->tl=_tmp17E;
_tmp199;});break;}}}}name=0;goto _LL177;}_LL180: if(*((int*)_tmp181)!= 7)goto
_LL182;_tmp186=((struct Cyc_Absyn_Typedef_d_struct*)_tmp181)->f1;_LL181: {struct
_tuple0 _tmp19B;struct _tagged_arr*_tmp19C;struct _tuple0*_tmp19A=_tmp186->name;
_tmp19B=*_tmp19A;_tmp19C=_tmp19B.f2;name=(struct _tagged_arr*)_tmp19C;goto _LL177;}
_LL182: if(*((int*)_tmp181)!= 2)goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)
_tmp181)!= 5)goto _LL186;_LL185: goto _LL187;_LL186: if(*((int*)_tmp181)!= 3)goto
_LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp181)!= 8)goto _LL18A;_LL189: goto
_LL18B;_LL18A: if(*((int*)_tmp181)!= 9)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((
int*)_tmp181)!= 10)goto _LL177;_LL18D: name=0;goto _LL177;_LL177:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp17D,(
struct _tagged_arr*)_check_null(name)): 0)_tmp17E=({struct Cyc_List_List*_tmp19D=
_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=_tmp180;_tmp19D->tl=_tmp17E;_tmp19D;});}}
_tmp17E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp17E);maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(char),2));if(!((
unsigned int)maybe)){int _tmp19E=1;_npop_handler(0);return _tmp19E;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct _tagged_arr)_tmp144;{void*_tmp19F[1]={&
_tmp1A0};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\nusing Std;\n",sizeof(
char),26),_tag_arr(_tmp19F,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=
Cyc_Std_fopen((struct _tagged_arr)_tmp144,_tag_arr("w",sizeof(char),2));if(!((
unsigned int)maybe)){int _tmp1A1=1;_npop_handler(0);return _tmp1A1;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);{struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct
_tagged_arr)filename;{void*_tmp1F4[1]={& _tmp1F5};Cyc_Std_aprintf(_tag_arr("_%s_",
sizeof(char),5),_tag_arr(_tmp1F4,sizeof(void*),1));}});{int _tmp1A2=0;for(0;
_tmp1A2 < _get_arr_size(ifdefmacro,sizeof(char));_tmp1A2 ++){if(((char*)ifdefmacro.curr)[
_tmp1A2]== '.'?1:((char*)ifdefmacro.curr)[_tmp1A2]== '/')((char*)ifdefmacro.curr)[
_tmp1A2]='_';else{if(((char*)ifdefmacro.curr)[_tmp1A2]!= '_'?((char*)ifdefmacro.curr)[
_tmp1A2]!= '/': 0)((char*)ifdefmacro.curr)[_tmp1A2]=(char)toupper((int)((char*)
ifdefmacro.curr)[_tmp1A2]);}}}({struct Cyc_Std_String_pa_struct _tmp1A5;_tmp1A5.tag=
0;_tmp1A5.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct
_tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1A3[2]={&
_tmp1A4,& _tmp1A5};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",
sizeof(char),23),_tag_arr(_tmp1A3,sizeof(void*),2));}}});if(prologue != 0){struct
Cyc_List_List*_tmp1A6=prologue;for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){struct
_tuple11 _tmp1A8;struct _tagged_arr _tmp1A9;struct _tagged_arr _tmp1AA;struct _tuple11*
_tmp1A7=(struct _tuple11*)_tmp1A6->hd;_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.f1;_tmp1AA=
_tmp1A8.f2;if(_tmp1AA.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1A9.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=_tmp1A9;_tmp1AB;})): 0)Cyc_Std_fputs(
_tmp1AA,out_file);else{({struct Cyc_Std_String_pa_struct _tmp1AD;_tmp1AD.tag=0;
_tmp1AD.f1=(struct _tagged_arr)_tmp1A9;{void*_tmp1AC[1]={& _tmp1AD};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp1AC,sizeof(void*),1));}});}}}({void*_tmp1AE[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("\nnamespace Std {\n",sizeof(char),18),_tag_arr(_tmp1AE,sizeof(void*),0));});{
struct Cyc_List_List*_tmp1AF=_tmp17E;for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){
struct Cyc_Absyn_Decl*_tmp1B0=(struct Cyc_Absyn_Decl*)_tmp1AF->hd;int _tmp1B1=0;
struct _tagged_arr*name;{void*_tmp1B2=(void*)_tmp1B0->r;struct Cyc_Absyn_Vardecl*
_tmp1B3;struct Cyc_Absyn_Fndecl*_tmp1B4;struct Cyc_Absyn_Aggrdecl*_tmp1B5;struct
Cyc_Absyn_Enumdecl*_tmp1B6;struct Cyc_Absyn_Typedefdecl*_tmp1B7;_LL18F: if(*((int*)
_tmp1B2)!= 0)goto _LL191;_tmp1B3=((struct Cyc_Absyn_Var_d_struct*)_tmp1B2)->f1;
_LL190: {struct _tuple0 _tmp1B9;struct _tagged_arr*_tmp1BA;struct _tuple0*_tmp1B8=
_tmp1B3->name;_tmp1B9=*_tmp1B8;_tmp1BA=_tmp1B9.f2;name=(struct _tagged_arr*)
_tmp1BA;goto _LL18E;}_LL191: if(*((int*)_tmp1B2)!= 1)goto _LL193;_tmp1B4=((struct
Cyc_Absyn_Fn_d_struct*)_tmp1B2)->f1;_LL192: {struct _tuple0 _tmp1BC;struct
_tagged_arr*_tmp1BD;struct _tuple0*_tmp1BB=_tmp1B4->name;_tmp1BC=*_tmp1BB;_tmp1BD=
_tmp1BC.f2;name=(struct _tagged_arr*)_tmp1BD;goto _LL18E;}_LL193: if(*((int*)
_tmp1B2)!= 4)goto _LL195;_tmp1B5=((struct Cyc_Absyn_Aggr_d_struct*)_tmp1B2)->f1;
_LL194: {struct _tuple0 _tmp1BF;struct _tagged_arr*_tmp1C0;struct _tuple0*_tmp1BE=
_tmp1B5->name;_tmp1BF=*_tmp1BE;_tmp1C0=_tmp1BF.f2;name=(struct _tagged_arr*)
_tmp1C0;goto _LL18E;}_LL195: if(*((int*)_tmp1B2)!= 6)goto _LL197;_tmp1B6=((struct
Cyc_Absyn_Enum_d_struct*)_tmp1B2)->f1;_LL196: {struct _tuple0 _tmp1C2;struct
_tagged_arr*_tmp1C3;struct _tuple0*_tmp1C1=_tmp1B6->name;_tmp1C2=*_tmp1C1;_tmp1C3=
_tmp1C2.f2;name=(struct _tagged_arr*)_tmp1C3;goto _LL18E;}_LL197: if(*((int*)
_tmp1B2)!= 7)goto _LL199;_tmp1B7=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1B2)->f1;
_LL198: {struct _tuple0 _tmp1C5;struct _tagged_arr*_tmp1C6;struct _tuple0*_tmp1C4=
_tmp1B7->name;_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.f2;name=(struct _tagged_arr*)
_tmp1C6;goto _LL18E;}_LL199: if(*((int*)_tmp1B2)!= 2)goto _LL19B;_LL19A: goto _LL19C;
_LL19B: if(*((int*)_tmp1B2)!= 5)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if(*((int*)
_tmp1B2)!= 3)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)_tmp1B2)!= 8)goto
_LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)_tmp1B2)!= 9)goto _LL1A3;_LL1A2: goto
_LL1A4;_LL1A3: if(*((int*)_tmp1B2)!= 10)goto _LL18E;_LL1A4: name=0;goto _LL18E;
_LL18E:;}if(!((unsigned int)name)?!_tmp1B1: 0)continue;if((unsigned int)name){
ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1C8;_tmp1C8.tag=0;_tmp1C8.f1=(
struct _tagged_arr)*name;{void*_tmp1C7[1]={& _tmp1C8};Cyc_Std_aprintf(_tag_arr("_%s_def_",
sizeof(char),9),_tag_arr(_tmp1C7,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1C9[1]={&
_tmp1CA};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(char),12),
_tag_arr(_tmp1C9,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1CC;
_tmp1CC.tag=0;_tmp1CC.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1CB[1]={&
_tmp1CC};Cyc_Std_fprintf(out_file,_tag_arr("#define %s\n",sizeof(char),12),
_tag_arr(_tmp1CB,sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp1CD[1];_tmp1CD[0]=_tmp1B0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1CD,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1CE[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(char),8),_tag_arr(_tmp1CE,
sizeof(void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp1CF[1];_tmp1CF[0]=_tmp1B0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1CF,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp141,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(
int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp16A=Cyc_Lexing_from_file(in_file);{struct _tuple10*entry2;while((
entry2=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(
_tmp16A))!= 0){struct _tuple10 _tmp1D1;struct _tagged_arr _tmp1D2;struct _tagged_arr*
_tmp1D3;struct _tuple10*_tmp1D0=(struct _tuple10*)_check_null(entry2);_tmp1D1=*
_tmp1D0;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;if(((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(_tmp17D,_tmp1D3)){({struct Cyc_Std_String_pa_struct
_tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _tagged_arr)*_tmp1D3;{void*_tmp1D4[1]={&
_tmp1D5};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(char),12),
_tag_arr(_tmp1D4,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1D7;
_tmp1D7.tag=0;_tmp1D7.f1=(struct _tagged_arr)_tmp1D2;{void*_tmp1D6[1]={& _tmp1D7};
Cyc_Std_fprintf(out_file,_tag_arr("%s\n",sizeof(char),4),_tag_arr(_tmp1D6,
sizeof(void*),1));}});({void*_tmp1D8[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",
sizeof(char),8),_tag_arr(_tmp1D8,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);
if(Cyc_mode != (void*)3)Cyc_Std_remove((struct _tagged_arr)_tmp141);if(epilogue != 
0){struct Cyc_List_List*_tmp1D9=epilogue;for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
struct _tuple11 _tmp1DB;struct _tagged_arr _tmp1DC;struct _tagged_arr _tmp1DD;struct
_tuple11*_tmp1DA=(struct _tuple11*)_tmp1D9->hd;_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1DB.f1;
_tmp1DD=_tmp1DB.f2;if(_tmp1DD.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp1DC.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=_tmp1DC;_tmp1DE;})): 0)Cyc_Std_fputs(
_tmp1DD,out_file);else{({struct Cyc_Std_String_pa_struct _tmp1E0;_tmp1E0.tag=0;
_tmp1E0.f1=(struct _tagged_arr)_tmp1DC;{void*_tmp1DF[1]={& _tmp1E0};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp1DF,sizeof(void*),1));}});}}}({void*_tmp1E1[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("}\n",sizeof(char),3),_tag_arr(_tmp1E1,sizeof(void*),0));});({void*
_tmp1E2[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(char),8),
_tag_arr(_tmp1E2,sizeof(void*),0));});Cyc_Std_fclose(out_file);if(cstubs != 0){
out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file);{struct Cyc_List_List*
_tmp1E3=cstubs;for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){struct _tuple11 _tmp1E5;
struct _tagged_arr _tmp1E6;struct _tagged_arr _tmp1E7;struct _tuple11*_tmp1E4=(struct
_tuple11*)_tmp1E3->hd;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;if(
_tmp1E7.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1E6.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1E8=
_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=_tmp1E6;_tmp1E8;})): 0)Cyc_Std_fputs(
_tmp1E7,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct
_tagged_arr)_tmp144;{void*_tmp1E9[1]={& _tmp1EA};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n\n",sizeof(char),16),_tag_arr(_tmp1E9,sizeof(void*),1));}});
if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
void*_tmp1EB[0]={};Cyc_Std_fprintf(out_file,_tag_arr("namespace Std {\n",sizeof(
char),17),_tag_arr(_tmp1EB,sizeof(void*),0));});{struct Cyc_List_List*_tmp1EC=
cycstubs;for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){struct _tuple11 _tmp1EE;struct
_tagged_arr _tmp1EF;struct _tagged_arr _tmp1F0;struct _tuple11*_tmp1ED=(struct
_tuple11*)_tmp1EC->hd;_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.f1;_tmp1F0=_tmp1EE.f2;if(
_tmp1F0.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1EF.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1F1=
_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=_tmp1EF;_tmp1F1;})): 0)Cyc_Std_fputs(
_tmp1F0,out_file);}}({void*_tmp1F2[0]={};Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",
sizeof(char),4),_tag_arr(_tmp1F2,sizeof(void*),0));});}{int _tmp1F3=0;
_npop_handler(0);return _tmp1F3;}}}}}}};_pop_handler();}else{void*_tmp146=(void*)
_exn_thrown;void*_tmp1F7=_tmp146;_LL1A6:;_LL1A7: maybe=Cyc_Std_fopen(filename,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp1F9;_tmp1F9.tag=0;_tmp1F9.f1=(struct _tagged_arr)filename;{void*_tmp1F8[1]={&
_tmp1F9};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp1F8,sizeof(void*),1));}});return 1;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _tagged_arr)filename;{void*_tmp1FA[1]={&
_tmp1FB};Cyc_Std_fprintf(out_file,_tag_arr("#error -- %s is not supported on this platform\n",
sizeof(char),48),_tag_arr(_tmp1FA,sizeof(void*),1));}});Cyc_Std_fclose(out_file);
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp144,_tag_arr("w",sizeof(char),2));if(
!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp1FD;_tmp1FD.tag=0;
_tmp1FD.f1=(struct _tagged_arr)_tmp144;{void*_tmp1FC[1]={& _tmp1FD};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(char),33),
_tag_arr(_tmp1FC,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp1FF;_tmp1FF.tag=0;_tmp1FF.f1=(
struct _tagged_arr)_tmp144;{void*_tmp1FE[1]={& _tmp1FF};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(char),48),
_tag_arr(_tmp1FE,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp201;_tmp201.tag=0;_tmp201.f1=(struct _tagged_arr)filename;{void*_tmp200[1]={&
_tmp201};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(char),52),_tag_arr(_tmp200,sizeof(void*),1));}});({void*_tmp202[0]={};Cyc_log(
_tag_arr("Not supported on this platform\n",sizeof(char),32),_tag_arr(_tmp202,
sizeof(void*),0));});Cyc_Std_remove((struct _tagged_arr)_tmp141);Cyc_Std_remove((
struct _tagged_arr)_tmp142);Cyc_Std_remove((struct _tagged_arr)_tmp143);return 0;
_LL1A8:;_LL1A9:(void)_throw(_tmp1F7);_LL1A5:;}}}}int Cyc_process_specfile(struct
_tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp213=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(char),2));if(!((unsigned int)_tmp213)){({struct Cyc_Std_String_pa_struct
_tmp215;_tmp215.tag=0;_tmp215.f1=(struct _tagged_arr)file;{void*_tmp214[1]={&
_tmp215};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not open %s\n",
sizeof(char),26),_tag_arr(_tmp214,sizeof(void*),1));}});return 1;}{struct Cyc_Std___cycFILE*
_tmp216=(struct Cyc_Std___cycFILE*)_check_null(_tmp213);struct _tagged_arr buf=
_tag_arr(({unsigned int _tmp229=(unsigned int)1024;char*_tmp22A=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp229));{unsigned int _tmp22B=
_tmp229;unsigned int i;for(i=0;i < _tmp22B;i ++){_tmp22A[i]='\000';}}_tmp22A;}),
sizeof(char),(unsigned int)1024);struct _tagged_arr _tmp217=Cyc_Std_getcwd(buf,
_get_arr_size(buf,sizeof(char)));if(Cyc_mode != (void*)2){if(Cyc_Std_chdir(dir)){({
struct Cyc_Std_String_pa_struct _tmp219;_tmp219.tag=0;_tmp219.f1=(struct
_tagged_arr)dir;{void*_tmp218[1]={& _tmp219};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n",sizeof(char),37),_tag_arr(
_tmp218,sizeof(void*),1));}});return 1;}}if(Cyc_mode == (void*)1){struct
_tagged_arr _tmp21A=Cstring_to_string(Ccomp);Cyc_Std_system((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct
_tagged_arr)_tmp21A;{void*_tmp21B[1]={& _tmp21C};Cyc_Std_aprintf(_tag_arr("echo | %s -E -dM - -o INITMACROS.h\n",
sizeof(char),36),_tag_arr(_tmp21B,sizeof(void*),1));}}));}{struct Cyc_Lexing_lexbuf*
_tmp21D=Cyc_Lexing_from_file(_tmp216);struct _tuple12*entry;while((entry=((struct
_tuple12*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp21D))!= 0){struct
_tuple12 _tmp21F;struct _tagged_arr _tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*
_tmp222;struct Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_List_List*
_tmp225;struct Cyc_List_List*_tmp226;struct _tuple12*_tmp21E=(struct _tuple12*)
_check_null(entry);_tmp21F=*_tmp21E;_tmp220=_tmp21F.f1;_tmp221=_tmp21F.f2;
_tmp222=_tmp21F.f3;_tmp223=_tmp21F.f4;_tmp224=_tmp21F.f5;_tmp225=_tmp21F.f6;
_tmp226=_tmp21F.f7;if(Cyc_process_file(_tmp220,_tmp221,_tmp222,_tmp223,_tmp224,
_tmp225,_tmp226))return 1;}Cyc_Std_fclose(_tmp216);if(Cyc_mode != (void*)2){if(Cyc_Std_chdir((
struct _tagged_arr)_tmp217)){({struct Cyc_Std_String_pa_struct _tmp228;_tmp228.tag=
0;_tmp228.f1=(struct _tagged_arr)_tmp217;{void*_tmp227[1]={& _tmp228};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not change directory to %s\n",sizeof(char),
41),_tag_arr(_tmp227,sizeof(void*),1));}});return 1;}}return 0;}}}int Cyc_getsize(
struct _tagged_arr dir,struct _tagged_arr includes,struct _tagged_arr type){struct
_tagged_arr _tmp22C=Cyc_Filename_concat(dir,_tag_arr("getsize.c",sizeof(char),10));
struct _tagged_arr _tmp22D=Cyc_Filename_concat(dir,_tag_arr("getsize",sizeof(char),
8));struct _tagged_arr _tmp22E=Cyc_Filename_concat(dir,_tag_arr("getsize.out",
sizeof(char),12));struct Cyc_Std___cycFILE*_tmp22F=Cyc_Std_fopen((struct
_tagged_arr)_tmp22C,_tag_arr("w",sizeof(char),2));if(!((unsigned int)_tmp22F)){({
struct Cyc_Std_String_pa_struct _tmp231;_tmp231.tag=0;_tmp231.f1=(struct
_tagged_arr)_tmp22C;{void*_tmp230[1]={& _tmp231};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create %s\n",sizeof(char),28),_tag_arr(_tmp230,
sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp233;_tmp233.tag=0;
_tmp233.f1=(struct _tagged_arr)type;{void*_tmp232[1]={& _tmp233};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(char),40),_tag_arr(_tmp232,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_String_pa_struct
_tmp238;_tmp238.tag=0;_tmp238.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp237;_tmp237.tag=0;_tmp237.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp236;_tmp236.tag=0;_tmp236.f1=(struct _tagged_arr)_tmp22E;{struct Cyc_Std_String_pa_struct
_tmp235;_tmp235.tag=0;_tmp235.f1=(struct _tagged_arr)includes;{void*_tmp234[4]={&
_tmp235,& _tmp236,& _tmp237,& _tmp238};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(_tmp22F),_tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof(char),139),_tag_arr(_tmp234,sizeof(void*),4));}}}}});Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(_tmp22F));{struct _tagged_arr _tmp239=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp23A=({struct Cyc_Std_String_pa_struct
_tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _tagged_arr)_tmp22C;{struct Cyc_Std_String_pa_struct
_tmp24B;_tmp24B.tag=0;_tmp24B.f1=(struct _tagged_arr)_tmp22D;{struct Cyc_Std_String_pa_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _tagged_arr)_tmp239;{void*_tmp249[3]={&
_tmp24A,& _tmp24B,& _tmp24C};Cyc_Std_aprintf(_tag_arr("%s -o %s -x c %s",sizeof(
char),17),_tag_arr(_tmp249,sizeof(void*),3));}}}});if(Cyc_Std_system((struct
_tagged_arr)_tmp23A)){({struct Cyc_Std_String_pa_struct _tmp23C;_tmp23C.tag=0;
_tmp23C.f1=(struct _tagged_arr)type;{void*_tmp23B[1]={& _tmp23C};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof(char),59),_tag_arr(_tmp23B,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp23E;_tmp23E.tag=0;_tmp23E.f1=(struct _tagged_arr)type;{void*_tmp23D[1]={&
_tmp23E};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(char),
40),_tag_arr(_tmp23D,sizeof(void*),1));}});return - 1;}{int _tmp23F=Cyc_Std_system((
struct _tagged_arr)_tmp22D);struct Cyc_Std___cycFILE*_tmp240=Cyc_Std_fopen((struct
_tagged_arr)_tmp22E,_tag_arr("r",sizeof(char),2));int w=- 1;if(({struct Cyc_Std_IntPtr_sa_struct
_tmp242;_tmp242.tag=2;_tmp242.f1=& w;{void*_tmp241[1]={& _tmp242};Cyc_Std_fscanf((
struct Cyc_Std___cycFILE*)_check_null(_tmp240),_tag_arr("%d",sizeof(char),3),
_tag_arr(_tmp241,sizeof(void*),1));}})!= 1){({struct Cyc_Std_String_pa_struct
_tmp244;_tmp244.tag=0;_tmp244.f1=(struct _tagged_arr)type;{void*_tmp243[1]={&
_tmp244};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(char),
40),_tag_arr(_tmp243,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_Int_pa_struct
_tmp248;_tmp248.tag=1;_tmp248.f1=(int)((unsigned int)w);{struct Cyc_Std_String_pa_struct
_tmp247;_tmp247.tag=0;_tmp247.f1=(struct _tagged_arr)_tmp22E;{struct Cyc_Std_String_pa_struct
_tmp246;_tmp246.tag=0;_tmp246.f1=(struct _tagged_arr)type;{void*_tmp245[3]={&
_tmp246,& _tmp247,& _tmp248};Cyc_log(_tag_arr("size of %s read from file %s is %d\n",
sizeof(char),36),_tag_arr(_tmp245,sizeof(void*),3));}}}});Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(_tmp240));Cyc_Std_remove((struct _tagged_arr)
_tmp22E);Cyc_Std_remove((struct _tagged_arr)_tmp22C);Cyc_Std_remove((struct
_tagged_arr)_tmp22D);return w;}}}static char _tmp24D[13]="BUILDLIB.OUT";static
struct _tagged_arr Cyc_output_dir={_tmp24D,_tmp24D,_tmp24D + 13};static void Cyc_set_output_dir(
struct _tagged_arr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _tagged_arr s){Cyc_spec_files=({struct Cyc_List_List*
_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->hd=({struct _tagged_arr*_tmp24F=
_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=s;_tmp24F;});_tmp24E->tl=Cyc_spec_files;
_tmp24E;});}static void Cyc_set_GATHER(){Cyc_mode=(void*)1;}static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=(void*)2;}static void Cyc_set_FINISH(){Cyc_mode=(void*)3;}static int Cyc_badparse=
0;static void Cyc_unsupported_option(struct _tagged_arr s){({struct Cyc_Std_String_pa_struct
_tmp251;_tmp251.tag=0;_tmp251.f1=(struct _tagged_arr)s;{void*_tmp250[1]={& _tmp251};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Unsupported option %s\n",sizeof(char),
23),_tag_arr(_tmp250,sizeof(void*),1));}});Cyc_badparse=1;}void
GC_blacklist_warn_clear();struct _tuple15{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*options=({struct
_tuple15*_tmp262[5];_tmp262[4]=({struct _tuple15*_tmp26F=_cycalloc(sizeof(*
_tmp26F));_tmp26F->f1=_tag_arr("-",sizeof(char),2);_tmp26F->f2=1;_tmp26F->f3=
_tag_arr("",sizeof(char),1);_tmp26F->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp271;_tmp271.tag=1;_tmp271.f1=Cyc_unsupported_option;_tmp271;});_tmp270;});
_tmp26F->f5=_tag_arr("",sizeof(char),1);_tmp26F;});_tmp262[3]=({struct _tuple15*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->f1=_tag_arr("-finish",sizeof(char),
8);_tmp26C->f2=0;_tmp26C->f3=_tag_arr("",sizeof(char),1);_tmp26C->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=Cyc_set_FINISH;
_tmp26E;});_tmp26D;});_tmp26C->f5=_tag_arr("Produce Cyclone headers from pre-gathered C library info",
sizeof(char),57);_tmp26C;});_tmp262[2]=({struct _tuple15*_tmp269=_cycalloc(
sizeof(*_tmp269));_tmp269->f1=_tag_arr("-gatherscript",sizeof(char),14);_tmp269->f2=
0;_tmp269->f3=_tag_arr("",sizeof(char),1);_tmp269->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26B;_tmp26B.tag=0;_tmp26B.f1=Cyc_set_GATHERSCRIPT;_tmp26B;});_tmp26A;});
_tmp269->f5=_tag_arr("Produce a script to gather C library info",sizeof(char),42);
_tmp269;});_tmp262[1]=({struct _tuple15*_tmp266=_cycalloc(sizeof(*_tmp266));
_tmp266->f1=_tag_arr("-gather",sizeof(char),8);_tmp266->f2=0;_tmp266->f3=
_tag_arr("",sizeof(char),1);_tmp266->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp268;_tmp268.tag=0;_tmp268.f1=Cyc_set_GATHER;_tmp268;});_tmp267;});_tmp266->f5=
_tag_arr("Gather C library info but don't produce Cyclone headers",sizeof(char),
56);_tmp266;});_tmp262[0]=({struct _tuple15*_tmp263=_cycalloc(sizeof(*_tmp263));
_tmp263->f1=_tag_arr("-d",sizeof(char),3);_tmp263->f2=0;_tmp263->f3=_tag_arr(" <file>",
sizeof(char),8);_tmp263->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp264=
_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Arg_String_spec_struct _tmp265;
_tmp265.tag=5;_tmp265.f1=Cyc_set_output_dir;_tmp265;});_tmp264;});_tmp263->f5=
_tag_arr("Set the output directory to <file>",sizeof(char),35);_tmp263;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp262,
sizeof(struct _tuple15*),5));});Cyc_Arg_parse(options,Cyc_add_spec_file,_tag_arr("Options:",
sizeof(char),9),argv);if(Cyc_badparse){Cyc_Arg_usage(options,_tag_arr("Options:",
sizeof(char),9));return 1;}if(Cyc_mode == (void*)2){Cyc_script_file=Cyc_Std_fopen(
_tag_arr("BUILDLIB.sh",sizeof(char),12),_tag_arr("w",sizeof(char),2));if(!((
unsigned int)Cyc_script_file)){({void*_tmp252[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Could not create file BUILDLIB.sh\n",sizeof(char),35),_tag_arr(_tmp252,
sizeof(void*),0));});exit(1);}({void*_tmp253[0]={};Cyc_prscript(_tag_arr("#!/bin/sh\n",
sizeof(char),11),_tag_arr(_tmp253,sizeof(void*),0));});({void*_tmp254[0]={};Cyc_prscript(
_tag_arr("GCC=\"gcc\"\n",sizeof(char),11),_tag_arr(_tmp254,sizeof(void*),0));});}
if(Cyc_force_directory_prefixes(Cyc_output_dir)?1: Cyc_force_directory(Cyc_output_dir)){({
struct Cyc_Std_String_pa_struct _tmp256;_tmp256.tag=0;_tmp256.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp255[1]={& _tmp256};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp255,sizeof(void*),1));}});return 1;}if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct
_tmp258;_tmp258.tag=0;_tmp258.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp257[
1]={& _tmp258};Cyc_prscript(_tag_arr("cd %s\n",sizeof(char),7),_tag_arr(_tmp257,
sizeof(void*),1));}});({void*_tmp259[0]={};Cyc_prscript(_tag_arr("echo | $GCC -E -dM - -o INITMACROS.h\n",
sizeof(char),38),_tag_arr(_tmp259,sizeof(void*),0));});}if(!Cyc_gathering()){Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("BUILDLIB.LOG",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _tmp25B;_tmp25B.tag=0;_tmp25B.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp25A[1]={& _tmp25B};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create log file in directory %s\n",sizeof(char),50),
_tag_arr(_tmp25A,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cstubs.c",sizeof(
char),9)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp25C[1]={& _tmp25D};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cstubs.c in directory %s\n",sizeof(char),50),
_tag_arr(_tmp25C,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cycstubs.cyc",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp25F;_tmp25F.tag=0;_tmp25F.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp25E[1]={& _tmp25F};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cycstubs.c in directory %s\n",sizeof(char),52),
_tag_arr(_tmp25E,sizeof(void*),1));}});return 1;}({void*_tmp260[0]={};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),_tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof(char),32),_tag_arr(_tmp260,sizeof(void*),0));});Cyc_sizeof_unsignedlongint=
Cyc_getsize(Cyc_output_dir,_tag_arr("",sizeof(char),1),_tag_arr("unsigned long int",
sizeof(char),18));Cyc_sizeof_unsignedshortint=Cyc_getsize(Cyc_output_dir,
_tag_arr("",sizeof(char),1),_tag_arr("unsigned short int",sizeof(char),19));Cyc_sizeof_shortint=
Cyc_getsize(Cyc_output_dir,_tag_arr("",sizeof(char),1),_tag_arr("short int",
sizeof(char),10));Cyc_sizeof_int=Cyc_getsize(Cyc_output_dir,_tag_arr("",sizeof(
char),1),_tag_arr("int",sizeof(char),4));Cyc_sizeof_short=Cyc_getsize(Cyc_output_dir,
_tag_arr("",sizeof(char),1),_tag_arr("short",sizeof(char),6));Cyc_sizeof_fdmask=
Cyc_getsize(Cyc_output_dir,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(char),47),_tag_arr("fd_mask",sizeof(char),8));Cyc_sizeof___fdmask=Cyc_getsize(
Cyc_output_dir,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(char),47),_tag_arr("__fd_mask",sizeof(char),10));Cyc_sizeof_uint32=Cyc_getsize(
Cyc_output_dir,_tag_arr("#include <sys/types.h>",sizeof(char),23),_tag_arr("__uint32_t",
sizeof(char),11));Cyc_sizeof_sockaddr=Cyc_getsize(Cyc_output_dir,_tag_arr("#include <sys/types.h>\n#include <sys/socket.h>",
sizeof(char),47),_tag_arr("struct sockaddr",sizeof(char),16));Cyc_sizeof_inport=
Cyc_getsize(Cyc_output_dir,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(char),47),_tag_arr("in_port_t",sizeof(char),10));Cyc_sizeof_inaddr=Cyc_getsize(
Cyc_output_dir,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(char),47),_tag_arr("struct in_addr",sizeof(char),15));}for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile(*((struct _tagged_arr*)((struct Cyc_List_List*)_check_null(
Cyc_spec_files))->hd),Cyc_output_dir)){({void*_tmp261[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("FATAL ERROR -- QUIT!\n",sizeof(char),22),_tag_arr(_tmp261,sizeof(void*),
0));});exit(1);}}if(Cyc_mode == (void*)2)Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file));else{if(!Cyc_gathering()){Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file));}}return 0;}}
