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
0;int Cyc_numdef=0;const int Cyc_lex_base[517]=(const int[517]){0,0,75,192,305,310,
311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,
166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 16,223,12,- 2,216,23,26,28,
24,28,58,56,65,52,70,370,1024,1139,386,93,83,90,112,96,106,115,124,115,126,113,
375,414,152,180,209,227,228,377,525,227,226,229,238,226,248,498,1253,1368,654,- 8,
238,235,254,239,239,239,259,508,1482,1597,- 11,684,- 12,241,270,510,1711,1826,- 9,
720,- 10,746,- 13,432,515,550,1903,1980,2057,2138,434,465,667,2213,269,269,279,277,
279,290,0,13,4,2294,5,660,2302,2367,822,49,467,6,2328,7,990,2390,2428,998,- 32,
1051,1056,297,288,272,297,289,299,722,1169,315,311,302,310,303,300,311,316,311,
327,329,669,1167,319,327,365,396,396,709,1279,403,398,1259,405,413,829,1284,412,
410,1370,416,415,414,442,427,846,1394,434,434,447,441,452,450,451,438,1484,458,
457,456,458,448,1599,450,449,1713,2490,473,468,1828,571,473,2352,475,473,498,520,
500,2492,522,510,552,536,545,2500,566,565,576,575,573,10,14,596,576,2502,600,589,
603,602,613,2507,1182,611,617,626,619,630,642,629,629,632,645,652,637,- 30,649,
653,650,660,678,679,- 25,659,675,696,693,704,700,731,749,762,- 19,748,745,752,784,
785,- 27,776,774,810,822,818,834,813,871,874,862,867,858,858,- 29,892,886,889,899,
908,918,920,953,960,962,1,4,6,963,964,953,952,967,967,975,976,2,52,987,989,2292,
20,21,1012,1013,976,1001,1016,1022,1023,1079,1110,1112,- 24,1059,1061,1116,1117,
1118,- 21,1066,1067,1122,1123,1125,- 22,1072,1073,1128,1131,1132,- 20,1079,1080,
1140,1142,1143,- 23,1090,1091,1604,- 31,1399,1100,1101,1099,1099,1098,1119,1121,-
18,1124,1125,1124,1166,1718,- 14,1159,1162,1162,1177,1833,2441,1180,1181,1182,
1187,1187,1200,2536,- 15,1204,1205,1203,1216,2542,- 17,- 7,- 8,8,1381,2508,9,1452,
2564,2587,1469,1609,- 49,1623,- 2,1256,- 4,1283,1344,2110,1284,1345,1451,1684,1285,
2621,2664,1289,1325,1288,1331,2734,1334,1335,- 36,- 42,- 37,2809,- 28,1336,- 40,1337,
- 45,- 39,- 48,2884,2913,2535,1343,1353,1799,2923,2953,2702,2894,2986,3017,3055,
1345,1356,3125,3163,1425,1435,1456,1468,1461,1548,- 6,- 34,1353,3095,- 47,- 46,- 33,
1362,3203,1371,1375,2725,1388,1476,1487,1488,1489,1490,1501,1506,1535,3276,3360,
3233,1536,- 41,- 38,- 35,- 26,1737,3442,4,3525,1570,15,1515,1515,1518,1516,1513,1524,
1594};const int Cyc_lex_backtrk[517]=(const int[517]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,
5,1,0,- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,14,2,
4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,31,
31,31,31,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,27,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,
48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,42,- 1,- 1,- 1,9,7,- 1,7,7,-
1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[517]=(const int[517]){- 1,- 1,- 1,419,408,154,23,36,23,19,- 1,
- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,
- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,
126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,-
1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,
- 1,482,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,- 1,0,0,0,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3782]=(const int[3782]){0,0,0,0,0,0,0,
0,0,0,22,19,28,509,19,28,19,28,95,19,45,45,45,45,45,22,45,0,0,0,0,0,21,317,328,
510,21,22,- 1,- 1,22,- 1,- 1,45,318,45,319,22,507,507,507,507,507,507,507,507,507,
507,31,119,22,245,130,40,246,507,507,507,507,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,507,507,507,507,507,507,507,507,31,329,363,357,507,146,507,
507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,505,505,505,505,505,505,505,505,505,505,137,20,85,78,65,55,
56,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,57,58,59,60,505,61,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,66,67,37,
420,421,420,420,421,39,22,68,69,70,71,72,145,34,34,34,34,34,34,34,34,73,74,420,
422,423,75,76,424,425,426,120,120,427,428,120,429,430,431,432,433,433,433,433,
433,433,433,433,433,434,79,435,436,437,120,19,438,438,438,438,438,438,438,438,
438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,19,- 1,- 1,
439,438,80,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,
438,438,438,438,438,438,438,438,438,409,440,37,410,155,155,24,24,155,147,138,81,
82,96,83,38,97,86,28,87,24,29,98,25,411,88,89,155,90,22,26,26,21,21,91,117,109,
156,99,100,101,102,103,110,26,35,35,35,35,35,35,35,35,111,131,132,30,30,30,30,30,
30,30,30,62,62,133,134,62,77,77,84,84,77,135,84,136,255,159,160,64,64,412,161,64,
162,163,62,148,139,157,39,77,22,84,239,233,224,170,195,171,172,27,64,173,31,174,
21,77,77,158,196,77,175,176,188,180,129,129,129,129,129,129,129,129,129,129,- 1,
32,- 1,- 1,77,- 1,22,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,- 1,181,- 1,- 1,129,- 1,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,92,92,35,182,92,183,186,187,189,190,104,104,112,112,104,193,112,120,120,
194,217,120,197,92,198,121,121,84,84,121,28,84,21,104,199,112,200,211,204,205,
120,206,207,208,209,210,212,213,121,214,84,215,128,128,114,216,128,218,219,21,21,
21,122,123,122,122,122,122,122,122,122,122,122,122,21,128,222,223,227,228,229,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,230,231,232,234,122,235,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,47,47,19,
236,47,237,238,33,33,33,33,33,33,33,33,33,33,94,94,249,225,94,240,47,33,33,33,33,
33,33,128,128,177,177,128,242,177,241,48,243,94,226,- 1,244,- 1,247,248,107,107,
250,49,107,251,128,252,177,33,33,33,33,33,33,144,144,144,144,144,144,144,144,107,
253,184,184,254,- 1,184,- 1,41,41,306,307,41,115,115,163,163,115,292,163,50,286,51,
276,269,184,52,262,263,264,265,41,53,54,266,267,115,268,163,117,117,270,271,117,
272,273,164,42,42,42,42,42,42,42,42,42,42,274,275,277,278,95,117,28,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,279,108,280,
281,42,282,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,191,191,43,283,191,116,284,33,33,33,33,33,33,33,33,33,33,201,201,285,
287,201,288,191,33,33,33,33,33,33,289,118,35,35,35,35,35,35,35,35,201,290,291,
293,294,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,295,296,297,298,44,299,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,300,301,302,28,303,304,305,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,369,308,309,310,44,311,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,
312,313,62,153,153,153,153,153,153,153,153,35,35,35,35,35,35,35,35,314,315,62,
316,320,321,393,393,322,323,393,373,373,324,325,373,326,327,63,63,63,63,63,63,63,
63,63,63,330,393,331,351,345,337,373,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,338,339,340,341,63,342,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,31,64,64,394,343,64,344,
346,374,347,348,349,350,395,352,353,354,355,375,356,358,359,360,376,64,361,362,
364,365,177,177,164,164,177,366,164,367,368,370,371,63,63,63,63,63,63,63,63,63,
63,388,382,177,377,164,378,379,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,380,381,383,384,63,385,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,92,92,165,386,92,389,187,187,
390,166,187,391,167,178,392,402,396,256,257,258,179,259,168,92,169,260,184,184,
397,187,184,191,191,398,261,191,399,400,116,93,93,93,93,93,93,93,93,93,93,184,
403,404,405,406,191,503,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,502,481,476,451,93,275,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,36,94,94,194,194,94,500,194,185,448,
291,449,305,192,418,418,445,446,418,443,441,447,450,479,94,344,194,201,201,501,
372,201,373,373,362,114,373,418,285,21,93,93,93,93,93,93,93,93,93,93,201,350,95,
95,35,373,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,442,95,95,35,93,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,104,104,210,210,104,268,210,374,202,417,417,417,
417,417,417,417,417,375,203,31,31,154,480,104,381,210,31,31,31,31,31,31,31,31,
108,106,108,118,387,105,105,105,105,105,105,105,105,105,105,401,36,31,31,36,46,
475,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,36,407,356,36,105,475,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,106,
107,107,216,216,107,21,216,371,371,511,512,371,418,418,513,514,418,515,475,516,
21,0,0,107,0,216,504,0,504,504,371,0,0,0,118,418,0,21,372,105,105,105,105,105,
105,105,105,105,105,504,475,0,0,0,0,0,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,105,0,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,112,112,220,220,112,0,220,386,386,0,477,386,478,478,478,
478,478,478,478,478,478,478,0,112,0,220,504,0,504,504,386,0,0,0,95,0,0,0,387,113,
113,113,113,113,113,113,113,113,113,504,0,0,0,0,0,0,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,
0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,114,115,115,223,223,115,0,223,392,392,0,0,
392,458,458,458,458,458,458,458,458,458,458,0,115,0,223,0,0,0,0,392,0,0,0,106,0,
0,0,46,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,
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
124,124,124,124,124,124,124,22,0,0,0,0,- 1,0,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,- 1,- 1,0,- 1,
124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,125,125,125,125,125,125,125,
125,125,127,0,0,0,0,0,483,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,125,0,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,129,129,129,129,129,129,129,129,129,129,0,0,0,0,0,0,0,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,0,0,28,0,0,149,141,141,141,
141,141,141,141,141,143,143,143,143,143,143,143,143,143,143,332,226,226,0,333,
226,- 1,143,143,143,143,143,143,334,0,335,150,150,150,150,150,150,150,150,226,0,0,
0,0,0,0,31,0,36,- 1,0,0,0,0,143,143,143,143,143,143,0,0,0,0,0,0,336,0,0,142,143,
143,143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,143,143,143,143,152,
152,152,152,152,152,152,152,152,152,151,0,393,393,0,0,393,152,152,152,152,152,
152,0,0,0,143,143,143,143,143,143,0,0,0,393,0,0,152,152,152,152,152,152,152,152,
152,152,0,152,152,152,152,152,152,152,152,152,152,152,152,220,220,232,232,220,0,
232,0,0,0,238,238,248,248,238,0,248,254,254,19,0,254,413,220,0,232,152,152,152,
152,152,152,95,238,35,248,0,0,0,0,254,394,19,0,31,0,400,400,0,28,400,395,406,406,
0,0,406,414,414,414,414,414,414,414,414,0,0,0,0,400,0,0,0,0,0,406,0,401,0,457,0,
457,0,407,458,458,458,458,458,458,458,458,458,458,0,0,221,0,0,0,0,0,0,0,0,0,28,0,
0,0,0,0,0,416,416,416,416,416,416,416,416,416,416,0,0,0,0,0,0,415,416,416,416,
416,416,416,416,416,416,416,416,416,416,416,416,416,0,0,0,0,0,0,0,416,416,416,
416,416,416,0,0,0,416,416,416,416,416,416,452,0,463,463,463,463,463,463,463,463,
464,464,0,0,0,0,0,416,416,416,416,416,416,454,0,0,0,0,0,0,465,0,0,0,0,0,0,0,0,
466,0,0,467,452,0,453,453,453,453,453,453,453,453,453,453,454,0,0,0,0,0,0,465,0,
0,0,454,0,0,0,0,466,0,455,467,0,0,0,461,0,461,0,456,462,462,462,462,462,462,462,
462,462,462,0,0,0,0,356,454,0,0,0,0,0,0,455,498,498,498,498,498,498,498,498,456,
444,444,444,444,444,444,444,444,444,444,0,0,0,0,0,0,0,444,444,444,444,444,444,
444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
0,0,0,0,444,0,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
0,0,0,0,0,0,0,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
444,444,444,444,444,444,444,444,444,444,0,0,0,0,444,0,444,444,444,444,444,444,
444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
459,459,459,459,459,459,459,459,459,459,462,462,462,462,462,462,462,462,462,462,
0,460,116,0,0,0,0,452,116,453,453,453,453,453,453,453,453,453,453,458,458,458,
458,458,458,458,458,458,458,0,454,0,0,460,116,0,0,455,0,0,116,114,0,0,0,0,456,
114,0,459,459,459,459,459,459,459,459,459,459,0,0,0,454,0,0,0,0,0,0,455,460,116,
0,114,0,0,0,116,456,114,0,0,462,462,462,462,462,462,462,462,462,462,0,0,0,0,0,0,
0,0,0,0,460,116,116,0,0,0,0,116,116,452,0,463,463,463,463,463,463,463,463,464,
464,0,0,0,0,0,0,0,0,0,0,0,454,0,116,0,0,0,0,473,116,0,0,0,0,0,0,452,474,464,464,
464,464,464,464,464,464,464,464,0,0,0,0,0,454,0,0,0,0,0,454,473,0,0,0,0,0,471,0,
0,474,0,0,0,0,0,472,0,0,478,478,478,478,478,478,478,478,478,478,0,0,0,454,0,0,0,
0,0,0,471,460,116,0,0,0,0,0,116,472,468,468,468,468,468,468,468,468,468,468,0,0,
0,0,0,0,0,468,468,468,468,468,468,460,116,0,0,0,0,0,116,0,0,0,0,0,0,0,468,468,
468,468,468,468,468,468,468,468,0,468,468,468,468,468,468,468,468,468,468,468,
468,0,0,0,484,0,469,0,0,485,0,0,0,0,0,470,0,0,486,486,486,486,486,486,486,486,0,
468,468,468,468,468,468,487,0,0,0,0,469,356,0,0,0,0,0,0,0,470,499,499,499,499,
499,499,499,499,0,0,0,0,0,0,488,0,0,0,0,489,490,0,0,0,491,0,0,0,0,0,0,0,492,0,0,
0,493,0,494,0,495,0,496,497,497,497,497,497,497,497,497,497,497,0,0,0,0,0,0,0,
497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,
497,497,497,497,497,497,0,0,0,0,0,0,497,497,497,497,497,497,497,497,497,497,497,
497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,368,0,0,0,0,0,0,0,0,
497,497,497,497,497,497,497,497,497,497,0,0,0,0,0,0,0,497,497,497,497,497,497,
497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,
0,0,0,0,0,0,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,
497,497,497,497,497,497,497,497,497,22,0,0,506,0,0,0,505,505,505,505,505,505,505,
505,505,505,0,0,0,0,0,0,0,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,0,0,0,0,505,0,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,508,0,0,0,0,0,0,0,507,507,507,507,507,507,507,507,507,507,0,0,0,0,0,
0,0,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,507,507,0,0,0,0,507,0,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0};const int Cyc_lex_check[3782]=(const int[3782]){- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,25,29,0,138,140,147,149,410,413,41,41,45,45,41,509,45,- 1,- 1,- 1,- 1,
- 1,136,316,327,0,506,10,12,40,10,12,40,41,317,45,318,20,1,1,1,1,1,1,1,1,1,1,38,
48,137,244,10,38,245,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,145,328,
332,333,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,
2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,
57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,7,3,3,3,
3,3,7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,47,47,3,
3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,4,3,16,4,5,5,6,8,5,6,8,80,81,50,82,16,50,85,27,86,17,27,50,17,4,87,88,5,89,
5,6,8,9,18,90,96,97,5,98,99,100,101,102,109,17,34,34,34,34,34,34,34,34,110,130,
131,27,27,27,27,27,27,27,27,61,61,132,133,61,76,76,83,83,76,134,83,135,157,158,
159,64,64,4,160,64,161,162,61,6,8,5,16,76,16,83,165,166,167,169,168,170,171,17,
64,172,27,173,7,77,77,5,168,77,174,175,178,179,11,11,11,11,11,11,11,11,11,11,119,
27,126,119,77,126,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,127,180,146,127,11,146,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,64,181,91,182,185,186,188,189,103,
103,111,111,103,192,111,120,120,193,195,120,196,91,197,13,13,84,84,13,77,84,16,
103,198,111,199,202,203,204,120,205,206,207,208,209,211,212,13,213,84,214,121,
121,4,215,121,217,218,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,121,221,222,
225,227,228,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,229,230,231,233,13,234,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,14,14,84,235,14,236,237,32,32,32,32,32,32,32,32,32,
32,94,94,240,224,94,239,14,32,32,32,32,32,32,128,128,176,176,128,241,176,239,14,
242,94,224,119,243,126,246,247,107,107,249,14,107,250,128,251,176,32,32,32,32,32,
32,141,141,141,141,141,141,141,141,107,252,183,183,253,127,183,146,15,15,256,256,
15,115,115,163,163,115,257,163,14,258,14,259,260,183,14,261,262,263,264,15,14,14,
265,266,115,267,163,117,117,269,270,117,271,272,163,15,15,15,15,15,15,15,15,15,
15,273,274,276,277,94,117,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,278,107,279,280,15,281,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,190,190,15,282,190,115,283,33,33,
33,33,33,33,33,33,33,33,200,200,284,286,200,287,190,33,33,33,33,33,33,288,117,
144,144,144,144,144,144,144,144,200,289,290,292,293,42,42,42,42,42,42,42,42,42,
42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,294,295,296,297,42,298,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,299,300,
301,15,302,303,304,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,306,307,308,309,44,310,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,311,312,62,150,150,150,150,150,150,
150,150,153,153,153,153,153,153,153,153,313,314,62,315,319,320,155,155,321,322,
155,156,156,323,324,156,325,326,62,62,62,62,62,62,62,62,62,62,329,155,330,334,
335,336,156,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,337,338,339,340,62,341,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,63,63,155,342,63,343,345,156,346,347,348,349,
155,351,352,353,354,156,355,357,358,359,156,63,360,361,363,364,177,177,164,164,
177,365,164,366,367,369,370,63,63,63,63,63,63,63,63,63,63,374,375,177,376,164,
377,378,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,379,380,382,383,63,384,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,92,92,164,385,92,388,187,187,389,164,187,390,164,177,
391,394,395,255,255,255,177,255,164,92,164,255,184,184,396,187,184,191,191,397,
255,191,398,399,187,92,92,92,92,92,92,92,92,92,92,184,402,403,404,405,191,422,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,424,
427,431,434,92,436,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,93,93,194,194,93,425,194,184,435,435,435,428,191,411,411,
437,437,411,439,440,446,448,477,93,482,194,201,201,425,428,201,373,373,484,194,
373,411,485,411,93,93,93,93,93,93,93,93,93,93,201,487,455,456,465,373,466,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,440,455,
456,465,93,466,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,104,104,210,210,104,429,210,373,201,414,414,414,414,414,414,414,414,
373,201,469,470,429,429,104,488,210,417,417,417,417,417,417,417,417,210,489,490,
491,492,104,104,104,104,104,104,104,104,104,104,493,471,469,470,472,494,473,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,471,495,499,472,104,473,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,
216,216,105,508,216,371,371,510,511,371,418,418,512,513,418,514,474,515,516,- 1,-
1,105,- 1,216,420,- 1,420,420,371,- 1,- 1,- 1,216,418,- 1,418,371,105,105,105,105,105,
105,105,105,105,105,420,474,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,
105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,112,112,219,219,112,- 1,219,386,386,- 1,430,386,
430,430,430,430,430,430,430,430,430,430,- 1,112,- 1,219,504,- 1,504,504,386,- 1,- 1,-
1,219,- 1,- 1,- 1,386,112,112,112,112,112,112,112,112,112,112,504,- 1,- 1,- 1,- 1,- 1,- 1,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,113,113,
223,223,113,- 1,223,392,392,- 1,- 1,392,457,457,457,457,457,457,457,457,457,457,- 1,
113,- 1,223,- 1,- 1,- 1,- 1,392,- 1,- 1,- 1,223,- 1,- 1,- 1,392,113,113,113,113,113,113,113,
113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,
- 1,- 1,- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,
123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,
124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,426,- 1,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,125,426,
- 1,125,124,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,
125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,426,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,
125,- 1,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,139,- 1,- 1,139,129,- 1,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
- 1,- 1,148,- 1,- 1,148,139,139,139,139,139,139,139,139,142,142,142,142,142,142,142,
142,142,142,331,226,226,- 1,331,226,426,142,142,142,142,142,142,331,- 1,331,148,
148,148,148,148,148,148,148,226,- 1,- 1,- 1,- 1,- 1,- 1,139,- 1,226,125,- 1,- 1,- 1,- 1,142,
142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,139,143,143,143,143,143,143,143,
143,143,143,148,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,151,151,151,151,151,
151,151,151,151,151,148,- 1,393,393,- 1,- 1,393,151,151,151,151,151,151,- 1,- 1,- 1,
143,143,143,143,143,143,- 1,- 1,- 1,393,- 1,- 1,152,152,152,152,152,152,152,152,152,
152,- 1,151,151,151,151,151,151,152,152,152,152,152,152,220,220,232,232,220,- 1,
232,- 1,- 1,- 1,238,238,248,248,238,- 1,248,254,254,412,- 1,254,412,220,- 1,232,152,
152,152,152,152,152,220,238,232,248,- 1,- 1,- 1,- 1,254,393,238,- 1,248,- 1,400,400,- 1,
254,400,393,406,406,- 1,- 1,406,412,412,412,412,412,412,412,412,- 1,- 1,- 1,- 1,400,- 1,
- 1,- 1,- 1,- 1,406,- 1,400,- 1,454,- 1,454,- 1,406,454,454,454,454,454,454,454,454,454,
454,- 1,- 1,220,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,412,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,
415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,412,415,415,415,415,415,415,416,416,
416,416,416,416,416,416,416,416,- 1,- 1,- 1,- 1,- 1,- 1,- 1,416,416,416,416,416,416,- 1,
- 1,- 1,415,415,415,415,415,415,432,- 1,432,432,432,432,432,432,432,432,432,432,- 1,
- 1,- 1,- 1,- 1,416,416,416,416,416,416,432,- 1,- 1,- 1,- 1,- 1,- 1,432,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,432,- 1,- 1,432,433,- 1,433,433,433,433,433,433,433,433,433,433,432,- 1,- 1,- 1,
- 1,- 1,- 1,432,- 1,- 1,- 1,433,- 1,- 1,- 1,- 1,432,- 1,433,432,- 1,- 1,- 1,460,- 1,460,- 1,433,
460,460,460,460,460,460,460,460,460,460,- 1,- 1,- 1,- 1,486,433,- 1,- 1,- 1,- 1,- 1,- 1,
433,486,486,486,486,486,486,486,486,433,438,438,438,438,438,438,438,438,438,438,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,
438,438,438,438,438,438,438,438,438,438,438,- 1,- 1,- 1,- 1,438,- 1,438,438,438,438,
438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,438,
438,438,444,444,444,444,444,444,444,444,444,444,- 1,- 1,- 1,- 1,- 1,- 1,- 1,444,444,444,
444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,444,
444,444,444,- 1,- 1,- 1,- 1,444,- 1,444,444,444,444,444,444,444,444,444,444,444,444,
444,444,444,444,444,444,444,444,444,444,444,444,444,444,452,452,452,452,452,452,
452,452,452,452,461,461,461,461,461,461,461,461,461,461,- 1,452,452,- 1,- 1,- 1,- 1,
453,452,453,453,453,453,453,453,453,453,453,453,458,458,458,458,458,458,458,458,
458,458,- 1,453,- 1,- 1,452,452,- 1,- 1,453,- 1,- 1,452,458,- 1,- 1,- 1,- 1,453,458,- 1,459,
459,459,459,459,459,459,459,459,459,- 1,- 1,- 1,453,- 1,- 1,- 1,- 1,- 1,- 1,453,459,459,-
1,458,- 1,- 1,- 1,459,453,458,- 1,- 1,462,462,462,462,462,462,462,462,462,462,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,459,459,462,- 1,- 1,- 1,- 1,459,462,463,- 1,463,463,463,463,
463,463,463,463,463,463,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,463,- 1,462,- 1,- 1,- 1,- 1,
463,462,- 1,- 1,- 1,- 1,- 1,- 1,464,463,464,464,464,464,464,464,464,464,464,464,- 1,- 1,
- 1,- 1,- 1,463,- 1,- 1,- 1,- 1,- 1,464,463,- 1,- 1,- 1,- 1,- 1,464,- 1,- 1,463,- 1,- 1,- 1,- 1,- 1,
464,- 1,- 1,478,478,478,478,478,478,478,478,478,478,- 1,- 1,- 1,464,- 1,- 1,- 1,- 1,- 1,- 1,
464,478,478,- 1,- 1,- 1,- 1,- 1,478,464,467,467,467,467,467,467,467,467,467,467,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,467,467,467,467,467,467,478,478,- 1,- 1,- 1,- 1,- 1,478,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,468,468,468,468,468,468,468,468,468,468,- 1,467,467,467,467,467,467,468,468,
468,468,468,468,- 1,- 1,- 1,483,- 1,468,- 1,- 1,483,- 1,- 1,- 1,- 1,- 1,468,- 1,- 1,483,483,
483,483,483,483,483,483,- 1,468,468,468,468,468,468,483,- 1,- 1,- 1,- 1,468,498,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,468,498,498,498,498,498,498,498,498,- 1,- 1,- 1,- 1,- 1,- 1,483,- 1,- 1,-
1,- 1,483,483,- 1,- 1,- 1,483,- 1,- 1,- 1,- 1,- 1,- 1,- 1,483,- 1,- 1,- 1,483,- 1,483,- 1,483,- 1,
483,496,496,496,496,496,496,496,496,496,496,- 1,- 1,- 1,- 1,- 1,- 1,- 1,496,496,496,496,
496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,496,
496,496,- 1,- 1,- 1,- 1,- 1,- 1,496,496,496,496,496,496,496,496,496,496,496,496,496,
496,496,496,496,496,496,496,496,496,496,496,496,496,497,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
497,497,497,497,497,497,497,497,497,497,- 1,- 1,- 1,- 1,- 1,- 1,- 1,497,497,497,497,497,
497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,497,
497,- 1,- 1,- 1,- 1,- 1,- 1,497,497,497,497,497,497,497,497,497,497,497,497,497,497,
497,497,497,497,497,497,497,497,497,497,497,497,505,- 1,- 1,505,- 1,- 1,- 1,505,505,
505,505,505,505,505,505,505,505,- 1,- 1,- 1,- 1,- 1,- 1,- 1,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,- 1,-
1,- 1,- 1,505,- 1,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,507,- 1,- 1,- 1,- 1,- 1,- 1,- 1,507,507,507,507,
507,507,507,507,507,507,- 1,- 1,- 1,- 1,- 1,- 1,- 1,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,- 1,- 1,- 1,- 1,
507,- 1,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,
507,507,507,507,507,507,507,507,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf){int state;int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(
lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(-
state)- 1;}while(1){base=Cyc_lex_base[state];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[state];if(backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(
!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3782,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3782,base + c)];else{
state=Cyc_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
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
_tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp40[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp40,sizeof(void*),0));});return 1;case 15: _LL5C: Cyc_Std_fputs(
_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp41[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp41,sizeof(void*),0));});return 1;case 16: _LL5D: Cyc_Std_fputs(
_tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC(",sizeof(char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp42[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(char),44),_tag_arr(_tmp42,sizeof(void*),0));});return 1;case 17: _LL5E: Cyc_Std_fputs(
_tag_arr("__region",sizeof(char),9),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp43[0]={};Cyc_log(_tag_arr("Warning: use of region sidestepped\n",
sizeof(char),36),_tag_arr(_tmp43,sizeof(void*),0));});return 1;case 18: _LL5F:
return 1;case 19: _LL60: return 1;case 20: _LL61: return 1;case 21: _LL62: return 1;case 22:
_LL63: return 1;case 23: _LL64: return 1;case 24: _LL65: Cyc_Std_fputs(_tag_arr("inline",
sizeof(char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 25: _LL66: Cyc_Std_fputs(_tag_arr("inline",sizeof(char),7),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 26: _LL67: Cyc_Std_fputs(_tag_arr("const",
sizeof(char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 27: _LL68: Cyc_Std_fputs(_tag_arr("const",sizeof(char),6),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 28: _LL69: Cyc_Std_fputs(_tag_arr("int",
sizeof(char),4),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 29: _LL6A: return 1;case 30: _LL6B: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}
Cyc_Std_fputs(_tag_arr("0",sizeof(char),2),(struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out));({void*_tmp44[0]={};Cyc_log(_tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof(char),42),_tag_arr(_tmp44,sizeof(void*),0));});return 1;case 31: _LL6C: Cyc_Std_fputc((
int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;default: _LL6D:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp45=_cycalloc(
sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Lexing_Error_struct _tmp46;_tmp46.tag=Cyc_Lexing_Error;
_tmp46.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp46;});
_tmp45;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: return 0;
case 1: _LL70: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 0;case 2: _LL71:({void*_tmp47[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(char),26),_tag_arr(_tmp47,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp49;_tmp49.tag=0;_tmp49.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp48[1]={& _tmp49};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp48,sizeof(void*),1));}});return 1;case
3: _LL72:({struct Cyc_Std_String_pa_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp4A[1]={& _tmp4B};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp4A,sizeof(void*),1));}});return 1;case 4: _LL73:({struct Cyc_Std_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4C[1]={& _tmp4D};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4C,sizeof(void*),1));}});return 1;case
5: _LL74:({struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp4E[1]={& _tmp4F};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp4E,sizeof(void*),1));}});return 1;case 6: _LL75:({struct Cyc_Std_String_pa_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp50[1]={& _tmp51};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp50,sizeof(void*),1));}});return 1;case
7: _LL76:({struct Cyc_Std_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp52[1]={& _tmp53};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmp52,sizeof(void*),1));}});return 1;case 8: _LL77:({struct Cyc_Std_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp54[1]={& _tmp55};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp54,sizeof(void*),1));}});return 1;
default: _LL78:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp56=_cycalloc(
sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Lexing_Error_struct _tmp57;_tmp57.tag=Cyc_Lexing_Error;
_tmp57.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp57;});
_tmp56;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL7A: return 0;case 1: _LL7B:
if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match --;return 1;case 2: _LL7C: Cyc_parens_to_match
++;return 1;case 3: _LL7D: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL7E:
while(Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL7F: return 1;case 6: _LL80: return 1;
default: _LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58[0]=({struct Cyc_Lexing_Error_struct _tmp59;_tmp59.tag=Cyc_Lexing_Error;
_tmp59.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp59;});
_tmp58;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,
7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL83:({void*_tmp5A[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp5A,
sizeof(void*),0));});return 0;case 1: _LL84: return 0;case 2: _LL85:({void*_tmp5B[0]={};
Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),_tag_arr(_tmp5B,
sizeof(void*),0));});return 1;case 3: _LL86: return 1;case 4: _LL87: return 1;case 5: _LL88:
return 1;case 6: _LL89: return 1;case 7: _LL8A: return 1;case 8: _LL8B: return 1;default:
_LL8C:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C[0]=({struct Cyc_Lexing_Error_struct _tmp5D;_tmp5D.tag=Cyc_Lexing_Error;
_tmp5D.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp5D;});
_tmp5C;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8E:({void*
_tmp5E[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp5E,sizeof(void*),0));});return 0;case 1: _LL8F: return 0;case 2: _LL90:
return 1;default: _LL91:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5F=
_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Lexing_Error_struct _tmp60;
_tmp60.tag=Cyc_Lexing_Error;_tmp60.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp60;});_tmp5F;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple10*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL93: Cyc_current_line=_tag_arr("#define ",
sizeof(char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple10*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61->f1=Cyc_current_line;_tmp61->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp61;});case 1: _LL94: return Cyc_suck_line(
lexbuf);case 2: _LL95: return 0;default: _LL96:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Lexing_Error_struct
_tmp63;_tmp63.tag=Cyc_Lexing_Error;_tmp63.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmp63;});_tmp62;}));}struct _tuple10*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL98: Cyc_current_source=({struct _tagged_arr*_tmp64=_cycalloc(sizeof(*
_tmp64));_tmp64[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp64;});Cyc_current_line=(
struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((struct _tagged_arr*)
_check_null(Cyc_current_source)));return Cyc_suck_restofline(lexbuf);default:
_LL99:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp65=_cycalloc(sizeof(*
_tmp65));_tmp65[0]=({struct Cyc_Lexing_Error_struct _tmp66;_tmp66.tag=Cyc_Lexing_Error;
_tmp66.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp66;});
_tmp65;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9B:
Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,(struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL9C:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({
struct Cyc_Lexing_Error_struct _tmp68;_tmp68.tag=Cyc_Lexing_Error;_tmp68.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp68;});_tmp67;}));}int
Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple12*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9E:
return Cyc_spec(lexbuf);case 1: _LL9F: Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=
0;while(Cyc_commands(lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);return({struct _tuple12*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->f1=Cyc_current_header;_tmp69->f2=Cyc_current_symbols;
_tmp69->f3=Cyc_current_omit_symbols;_tmp69->f4=Cyc_current_prologue;_tmp69->f5=
Cyc_current_epilogue;_tmp69->f6=Cyc_current_cstubs;_tmp69->f7=Cyc_current_cycstubs;
_tmp69;});case 2: _LLA0: return Cyc_spec(lexbuf);case 3: _LLA1: return 0;case 4: _LLA2:({
struct Cyc_Std_Int_pa_struct _tmp6B;_tmp6B.tag=1;_tmp6B.f1=(int)((unsigned int)((
int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp6A[1]={& _tmp6B};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(char),67),_tag_arr(_tmp6A,sizeof(void*),1));}});return 0;default: _LLA3:(
lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({
struct Cyc_Lexing_Error_struct _tmp6D;_tmp6D.tag=Cyc_Lexing_Error;_tmp6D.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp6D;});_tmp6C;}));}
struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA5: return 0;
case 1: _LLA6: return 0;case 2: _LLA7: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(
lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LLA8: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA9: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp6F->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp6F;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=x;_tmp6E->tl=
Cyc_current_prologue;_tmp6E;});return 1;}case 5: _LLAA: {struct _tagged_arr _tmp70=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp70,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("prologue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp70,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp70,sizeof(char),1);}{struct _tagged_arr t=_tmp70;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp71=Cyc_Std_substring((struct _tagged_arr)
_tmp70,0,(unsigned int)((t.curr - _tmp70.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=(
struct _tagged_arr)_tmp71;_tmp73->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp73;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=x;_tmp72->tl=
Cyc_current_prologue;_tmp72;});return 1;}}}}case 6: _LLAB: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp75->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp75;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->hd=x;_tmp74->tl=
Cyc_current_epilogue;_tmp74;});return 1;}case 7: _LLAC: {struct _tagged_arr _tmp76=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp76,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("epilogue",sizeof(char),9)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp76,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp76,sizeof(char),1);}{struct _tagged_arr t=_tmp76;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp77=Cyc_Std_substring((struct _tagged_arr)
_tmp76,0,(unsigned int)((t.curr - _tmp76.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=(
struct _tagged_arr)_tmp77;_tmp79->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp79;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=x;_tmp78->tl=
Cyc_current_epilogue;_tmp78;});return 1;}}}}case 8: _LLAD: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp7B->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7B;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->hd=x;_tmp7A->tl=
Cyc_current_cstubs;_tmp7A;});return 1;}case 9: _LLAE: {struct _tagged_arr _tmp7C=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp7C,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cstub",sizeof(char),6)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp7C,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp7C,sizeof(char),1);}{struct _tagged_arr t=_tmp7C;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp7D=Cyc_Std_substring((struct _tagged_arr)
_tmp7C,0,(unsigned int)((t.curr - _tmp7C.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->f1=(
struct _tagged_arr)_tmp7D;_tmp7F->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7F;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=x;_tmp7E->tl=
Cyc_current_cstubs;_tmp7E;});return 1;}}}}case 10: _LLAF: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple11*x=({struct _tuple11*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp81->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp81;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->hd=x;_tmp80->tl=
Cyc_current_cycstubs;_tmp80;});return 1;}case 11: _LLB0: {struct _tagged_arr _tmp82=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp82,sizeof(char),(int)Cyc_Std_strlen(
_tag_arr("cycstub",sizeof(char),8)));while(isspace((int)*((char*)
_check_unknown_subscript(_tmp82,sizeof(char),0)))){_tagged_arr_inplace_plus_post(&
_tmp82,sizeof(char),1);}{struct _tagged_arr t=_tmp82;while(!isspace((int)*((char*)
_check_unknown_subscript(t,sizeof(char),0)))){_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);}{struct _tagged_arr _tmp83=Cyc_Std_substring((struct _tagged_arr)
_tmp82,0,(unsigned int)((t.curr - _tmp82.curr)/ sizeof(char)));Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=(
struct _tagged_arr)_tmp83;_tmp85->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp85;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=x;_tmp84->tl=
Cyc_current_cycstubs;_tmp84;});return 1;}}}}case 12: _LLB1: Cyc_braces_to_match=1;
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tagged_arr*x=({struct _tagged_arr*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[
0]=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp87;});Cyc_current_cpp=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86->hd=x;_tmp86->tl=Cyc_current_cpp;_tmp86;});return 1;}case 13: _LLB2: return 1;
case 14: _LLB3: return 1;case 15: _LLB4:({struct Cyc_Std_Int_pa_struct _tmp89;_tmp89.tag=
1;_tmp89.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*
_tmp88[1]={& _tmp89};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(char),58),_tag_arr(_tmp88,sizeof(void*),1));}});return 0;default: _LLB5:(
lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A[0]=({struct Cyc_Lexing_Error_struct _tmp8B;_tmp8B.tag=Cyc_Lexing_Error;
_tmp8B.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp8B;});
_tmp8A;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB7: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=({struct
_tagged_arr*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp8D;});_tmp8C->tl=Cyc_snarfed_symbols;_tmp8C;});return 1;case 1: _LLB8:
return 1;case 2: _LLB9: return 0;case 3: _LLBA:({void*_tmp8E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n",sizeof(char),44),
_tag_arr(_tmp8E,sizeof(void*),0));});return 0;case 4: _LLBB:({struct Cyc_Std_Int_pa_struct
_tmp90;_tmp90.tag=1;_tmp90.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp8F[1]={& _tmp90};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(char),57),_tag_arr(_tmp8F,sizeof(void*),1));}});return 0;default: _LLBC:(
lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91[0]=({struct Cyc_Lexing_Error_struct _tmp92;_tmp92.tag=Cyc_Lexing_Error;
_tmp92.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp92;});
_tmp91;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBE:({void*
_tmp93[0]={};Cyc_log(_tag_arr("Warning: unclosed brace\n",sizeof(char),25),
_tag_arr(_tmp93,sizeof(void*),0));});return 0;case 1: _LLBF: if(Cyc_braces_to_match
== 1)return 0;Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLC0: Cyc_braces_to_match ++;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLC1: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(char),3));while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC4: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLC5:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp94=_cycalloc(
sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Lexing_Error_struct _tmp95;_tmp95.tag=Cyc_Lexing_Error;
_tmp95.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp95;});
_tmp94;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(
lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:({void*
_tmp96[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp96,sizeof(void*),0));});return 0;case 1: _LLC8: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLC9:({void*
_tmp97[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(char),26),
_tag_arr(_tmp97,sizeof(void*),0));});Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 3: _LLCA: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLCB: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLCC: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 6: _LLCD: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLCE: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLCF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
default: _LLD0:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Lexing_Error_struct _tmp99;_tmp99.tag=Cyc_Lexing_Error;
_tmp99.f1=_tag_arr("some action didn't return!",sizeof(char),27);_tmp99;});
_tmp98;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLD2:({void*
_tmp9A[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(char),27),
_tag_arr(_tmp9A,sizeof(void*),0));});return 0;case 1: _LLD3: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(char),3));
return 0;case 2: _LLD4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLD5:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({
struct Cyc_Lexing_Error_struct _tmp9C;_tmp9C.tag=Cyc_Lexing_Error;_tmp9C.f1=
_tag_arr("some action didn't return!",sizeof(char),27);_tmp9C;});_tmp9B;}));}int
Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp9D=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmp9E;struct
_tuple0 _tmp9F;struct _tagged_arr*_tmpA0;struct _tuple0*_tmpA1;struct _tuple0 _tmpA2;
struct _tagged_arr*_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_Absyn_Exp*_tmpA5;
struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;
struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;
struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;
struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;
struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_List_List*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;
struct Cyc_List_List*_tmpB5;void*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_MallocInfo
_tmpB8;int _tmpB9;struct Cyc_Absyn_Exp*_tmpBA;void**_tmpBB;struct Cyc_Absyn_Exp*
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct _tagged_arr*_tmpBF;struct Cyc_Absyn_Exp*
_tmpC0;struct _tagged_arr*_tmpC1;void*_tmpC2;void*_tmpC3;struct Cyc_List_List*
_tmpC4;_LLD8: if(*((int*)_tmp9D)!= 1)goto _LLDA;_tmp9E=((struct Cyc_Absyn_Var_e_struct*)
_tmp9D)->f1;_tmp9F=*_tmp9E;_tmpA0=_tmp9F.f2;_LLD9: _tmpA3=_tmpA0;goto _LLDB;_LLDA:
if(*((int*)_tmp9D)!= 2)goto _LLDC;_tmpA1=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp9D)->f1;_tmpA2=*_tmpA1;_tmpA3=_tmpA2.f2;_LLDB: Cyc_add_target(_tmpA3);return;
_LLDC: if(*((int*)_tmp9D)!= 3)goto _LLDE;_tmpA4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9D)->f2;_LLDD: for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpA4->hd));}return;_LLDE: if(*((int*)_tmp9D)!= 23)goto
_LLE0;_tmpA5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9D)->f1;_tmpA6=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp9D)->f2;_LLDF: _tmpA7=_tmpA5;_tmpA8=_tmpA6;goto
_LLE1;_LLE0: if(*((int*)_tmp9D)!= 7)goto _LLE2;_tmpA7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9D)->f1;_tmpA8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9D)->f2;_LLE1: _tmpA9=
_tmpA7;_tmpAA=_tmpA8;goto _LLE3;_LLE2: if(*((int*)_tmp9D)!= 4)goto _LLE4;_tmpA9=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9D)->f1;_tmpAA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9D)->f3;_LLE3: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA9);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpAA);return;_LLE4: if(*((int*)_tmp9D)!= 20)goto _LLE6;
_tmpAB=((struct Cyc_Absyn_Deref_e_struct*)_tmp9D)->f1;_LLE5: _tmpAC=_tmpAB;goto
_LLE7;_LLE6: if(*((int*)_tmp9D)!= 17)goto _LLE8;_tmpAC=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9D)->f1;_LLE7: _tmpAD=_tmpAC;goto _LLE9;_LLE8: if(*((int*)_tmp9D)!= 14)goto
_LLEA;_tmpAD=((struct Cyc_Absyn_Address_e_struct*)_tmp9D)->f1;_LLE9: _tmpAE=_tmpAD;
goto _LLEB;_LLEA: if(*((int*)_tmp9D)!= 5)goto _LLEC;_tmpAE=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9D)->f1;_LLEB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAE);return;_LLEC: if(*((
int*)_tmp9D)!= 6)goto _LLEE;_tmpAF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9D)->f1;
_tmpB0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9D)->f2;_tmpB1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9D)->f3;_LLED: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAF);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpB0);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB1);return;
_LLEE: if(*((int*)_tmp9D)!= 9)goto _LLF0;_tmpB2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp9D)->f1;_tmpB3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9D)->f2;_LLEF: _tmpB4=
_tmpB2;_tmpB5=_tmpB3;goto _LLF1;_LLF0: if(*((int*)_tmp9D)!= 8)goto _LLF2;_tmpB4=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9D)->f1;_tmpB5=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp9D)->f2;_LLF1: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB4);for(0;_tmpB5 != 0;
_tmpB5=_tmpB5->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpB5->hd));}return;_LLF2: if(*((int*)_tmp9D)!= 13)goto _LLF4;_tmpB6=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9D)->f1;_tmpB7=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9D)->f2;_LLF3: Cyc_scan_type(_tmpB6);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpB7);return;_LLF4: if(*((int*)_tmp9D)!= 33)goto _LLF6;_tmpB8=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9D)->f1;_tmpB9=_tmpB8.is_calloc;_tmpBA=_tmpB8.rgn;_tmpBB=_tmpB8.elt_type;
_tmpBC=_tmpB8.num_elts;_LLF5: if(_tmpBA != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpBA)));if(_tmpBB != 0)Cyc_scan_type(*_tmpBB);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBC);return;_LLF6: if(*((int*)_tmp9D)!= 16)
goto _LLF8;_tmpBD=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9D)->f1;_LLF7:
Cyc_scan_type(_tmpBD);return;_LLF8: if(*((int*)_tmp9D)!= 21)goto _LLFA;_tmpBE=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9D)->f1;_tmpBF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9D)->f2;_LLF9: _tmpC0=_tmpBE;_tmpC1=_tmpBF;goto _LLFB;_LLFA: if(*((int*)_tmp9D)
!= 22)goto _LLFC;_tmpC0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9D)->f1;_tmpC1=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9D)->f2;_LLFB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpC0);Cyc_add_target(_tmpC1);return;_LLFC: if(*((int*)_tmp9D)!= 18)goto _LLFE;
_tmpC2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp9D)->f1;_tmpC3=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp9D)->f2;_LLFD: Cyc_scan_type(_tmpC2);{void*
_tmpC5=_tmpC3;struct _tagged_arr*_tmpC6;_LL125: if(*((int*)_tmpC5)!= 0)goto _LL127;
_tmpC6=((struct Cyc_Absyn_StructField_struct*)_tmpC5)->f1;_LL126: Cyc_add_target(
_tmpC6);goto _LL124;_LL127: if(*((int*)_tmpC5)!= 1)goto _LL124;_LL128: goto _LL124;
_LL124:;}return;_LLFE: if(*((int*)_tmp9D)!= 0)goto _LL100;_LLFF: return;_LL100: if(*((
int*)_tmp9D)!= 34)goto _LL102;_tmpC4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp9D)->f2;_LL101: for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){struct _tuple13 _tmpC8;
struct Cyc_Absyn_Exp*_tmpC9;struct _tuple13*_tmpC7=(struct _tuple13*)_tmpC4->hd;
_tmpC8=*_tmpC7;_tmpC9=_tmpC8.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC9);}
return;_LL102: if(*((int*)_tmp9D)!= 35)goto _LL104;_LL103:({void*_tmpCA[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",sizeof(char),26),_tag_arr(
_tmpCA,sizeof(void*),0));});exit(1);return;_LL104: if(*((int*)_tmp9D)!= 10)goto
_LL106;_LL105:({void*_tmpCB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(char),27),_tag_arr(_tmpCB,sizeof(void*),0));});exit(1);return;_LL106: if(*((
int*)_tmp9D)!= 11)goto _LL108;_LL107:({void*_tmpCC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected NoInstantiate_e\n",sizeof(char),35),_tag_arr(_tmpCC,
sizeof(void*),0));});exit(1);return;_LL108: if(*((int*)_tmp9D)!= 12)goto _LL10A;
_LL109:({void*_tmpCD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",
sizeof(char),33),_tag_arr(_tmpCD,sizeof(void*),0));});exit(1);return;_LL10A: if(*((
int*)_tmp9D)!= 15)goto _LL10C;_LL10B:({void*_tmpCE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected New_e\n",sizeof(char),25),_tag_arr(_tmpCE,sizeof(
void*),0));});exit(1);return;_LL10C: if(*((int*)_tmp9D)!= 19)goto _LL10E;_LL10D:({
void*_tmpCF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(char),28),_tag_arr(_tmpCF,sizeof(void*),0));});exit(1);return;_LL10E: if(*((
int*)_tmp9D)!= 24)goto _LL110;_LL10F:({void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tuple_e\n",sizeof(char),27),_tag_arr(_tmpD0,sizeof(
void*),0));});exit(1);return;_LL110: if(*((int*)_tmp9D)!= 25)goto _LL112;_LL111:({
void*_tmpD1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(char),33),_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;_LL112: if(*((
int*)_tmp9D)!= 26)goto _LL114;_LL113:({void*_tmpD2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Array_e\n",sizeof(char),27),_tag_arr(_tmpD2,sizeof(
void*),0));});exit(1);return;_LL114: if(*((int*)_tmp9D)!= 27)goto _LL116;_LL115:({
void*_tmpD3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(char),35),_tag_arr(_tmpD3,sizeof(void*),0));});exit(1);return;_LL116: if(*((
int*)_tmp9D)!= 28)goto _LL118;_LL117:({void*_tmpD4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Struct_e\n",sizeof(char),28),_tag_arr(_tmpD4,sizeof(
void*),0));});exit(1);return;_LL118: if(*((int*)_tmp9D)!= 29)goto _LL11A;_LL119:({
void*_tmpD5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(char),32),_tag_arr(_tmpD5,sizeof(void*),0));});exit(1);return;_LL11A: if(*((
int*)_tmp9D)!= 30)goto _LL11C;_LL11B:({void*_tmpD6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Tunion_e\n",sizeof(char),28),_tag_arr(_tmpD6,sizeof(
void*),0));});exit(1);return;_LL11C: if(*((int*)_tmp9D)!= 31)goto _LL11E;_LL11D:({
void*_tmpD7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(char),26),_tag_arr(_tmpD7,sizeof(void*),0));});exit(1);return;_LL11E: if(*((
int*)_tmp9D)!= 32)goto _LL120;_LL11F:({void*_tmpD8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonEnum_e\n",sizeof(char),30),_tag_arr(_tmpD8,
sizeof(void*),0));});exit(1);return;_LL120: if(*((int*)_tmp9D)!= 36)goto _LL122;
_LL121:({void*_tmpD9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Codegen_e\n",
sizeof(char),29),_tag_arr(_tmpD9,sizeof(void*),0));});exit(1);return;_LL122: if(*((
int*)_tmp9D)!= 37)goto _LLD7;_LL123:({void*_tmpDA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Fill_e\n",sizeof(char),26),_tag_arr(_tmpDA,sizeof(
void*),0));});exit(1);return;_LLD7:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){
if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*
_tmpDB=t;struct Cyc_Absyn_PtrInfo _tmpDC;void*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;
struct Cyc_Absyn_FnInfo _tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_AggrInfo
_tmpE1;void*_tmpE2;struct _tuple0*_tmpE3;struct _tuple0 _tmpE4;struct _tagged_arr*
_tmpE5;struct _tuple0*_tmpE6;struct _tuple0 _tmpE7;struct _tagged_arr*_tmpE8;_LL12A:
if((int)_tmpDB != 0)goto _LL12C;_LL12B: goto _LL12D;_LL12C: if(_tmpDB <= (void*)3?1:*((
int*)_tmpDB)!= 5)goto _LL12E;_LL12D: goto _LL12F;_LL12E: if((int)_tmpDB != 1)goto
_LL130;_LL12F: goto _LL131;_LL130: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 6)goto
_LL132;_LL131: return;_LL132: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 4)goto
_LL134;_tmpDC=((struct Cyc_Absyn_PointerType_struct*)_tmpDB)->f1;_LL133: Cyc_scan_type((
void*)_tmpDC.elt_typ);return;_LL134: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 7)
goto _LL136;_tmpDD=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpDB)->f1;_tmpDE=((
struct Cyc_Absyn_ArrayType_struct*)_tmpDB)->f3;_LL135: Cyc_scan_type(_tmpDD);Cyc_scan_exp_opt(
_tmpDE);return;_LL136: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 8)goto _LL138;
_tmpDF=((struct Cyc_Absyn_FnType_struct*)_tmpDB)->f1;_LL137: Cyc_scan_type((void*)
_tmpDF.ret_typ);{struct Cyc_List_List*_tmpE9=_tmpDF.args;for(0;_tmpE9 != 0;_tmpE9=
_tmpE9->tl){struct _tuple1 _tmpEB;void*_tmpEC;struct _tuple1*_tmpEA=(struct _tuple1*)
_tmpE9->hd;_tmpEB=*_tmpEA;_tmpEC=_tmpEB.f3;Cyc_scan_type(_tmpEC);}}if(_tmpDF.cyc_varargs
!= 0)Cyc_scan_type((void*)(_tmpDF.cyc_varargs)->type);return;_LL138: if(_tmpDB <= (
void*)3?1:*((int*)_tmpDB)!= 11)goto _LL13A;_tmpE0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDB)->f2;_LL139: for(0;_tmpE0 != 0;_tmpE0=_tmpE0->tl){Cyc_scan_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmpE0->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)
_tmpE0->hd)->width);}return;_LL13A: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 13)
goto _LL13C;_LL13B: return;_LL13C: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 10)goto
_LL13E;_tmpE1=((struct Cyc_Absyn_AggrType_struct*)_tmpDB)->f1;_tmpE2=(void*)
_tmpE1.aggr_info;_LL13D: {struct _tuple0*_tmpEE;struct _tuple0 _tmpEF;struct
_tagged_arr*_tmpF0;struct _tuple3 _tmpED=Cyc_Absyn_aggr_kinded_name(_tmpE2);_tmpEE=
_tmpED.f2;_tmpEF=*_tmpEE;_tmpF0=_tmpEF.f2;_tmpE5=_tmpF0;goto _LL13F;}_LL13E: if(
_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 12)goto _LL140;_tmpE3=((struct Cyc_Absyn_EnumType_struct*)
_tmpDB)->f1;_tmpE4=*_tmpE3;_tmpE5=_tmpE4.f2;_LL13F: _tmpE8=_tmpE5;goto _LL141;
_LL140: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 16)goto _LL142;_tmpE6=((struct Cyc_Absyn_TypedefType_struct*)
_tmpDB)->f1;_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f2;_LL141: Cyc_add_target(_tmpE8);
return;_LL142: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 0)goto _LL144;_LL143:({
void*_tmpF1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Evar\n",
sizeof(char),24),_tag_arr(_tmpF1,sizeof(void*),0));});exit(1);return;_LL144: if(
_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 1)goto _LL146;_LL145:({void*_tmpF2[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected VarType\n",sizeof(
char),27),_tag_arr(_tmpF2,sizeof(void*),0));});exit(1);return;_LL146: if(_tmpDB <= (
void*)3?1:*((int*)_tmpDB)!= 2)goto _LL148;_LL147:({void*_tmpF3[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected TunionType\n",sizeof(char),30),
_tag_arr(_tmpF3,sizeof(void*),0));});exit(1);return;_LL148: if(_tmpDB <= (void*)3?
1:*((int*)_tmpDB)!= 3)goto _LL14A;_LL149:({void*_tmpF4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionFieldType\n",sizeof(char),35),_tag_arr(_tmpF4,
sizeof(void*),0));});exit(1);return;_LL14A: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)
!= 9)goto _LL14C;_LL14B:({void*_tmpF5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n",sizeof(char),29),_tag_arr(_tmpF5,
sizeof(void*),0));});exit(1);return;_LL14C: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)
!= 15)goto _LL14E;_LL14D:({void*_tmpF6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnHandleType\n",sizeof(char),33),_tag_arr(_tmpF6,
sizeof(void*),0));});exit(1);return;_LL14E: if((int)_tmpDB != 2)goto _LL150;_LL14F:({
void*_tmpF7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(char),27),_tag_arr(_tmpF7,sizeof(void*),0));});exit(1);return;_LL150: if(
_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 17)goto _LL152;_LL151:({void*_tmpF8[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",sizeof(
char),29),_tag_arr(_tmpF8,sizeof(void*),0));});exit(1);return;_LL152: if(_tmpDB <= (
void*)3?1:*((int*)_tmpDB)!= 18)goto _LL154;_LL153:({void*_tmpF9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",sizeof(char),27),_tag_arr(
_tmpF9,sizeof(void*),0));});exit(1);return;_LL154: if(_tmpDB <= (void*)3?1:*((int*)
_tmpDB)!= 19)goto _LL156;_LL155:({void*_tmpFA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnsEff\n",sizeof(char),27),_tag_arr(_tmpFA,sizeof(
void*),0));});exit(1);return;_LL156: if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 14)
goto _LL129;_LL157:({void*_tmpFB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(char),28),_tag_arr(_tmpFB,sizeof(void*),0));});exit(1);return;_LL129:;}
struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple9*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpFC;});{
void*_tmpFD=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpFE;struct Cyc_Absyn_Fndecl*
_tmpFF;struct Cyc_Absyn_Aggrdecl*_tmp100;struct Cyc_Absyn_Enumdecl*_tmp101;struct
Cyc_Absyn_Typedefdecl*_tmp102;_LL159: if(*((int*)_tmpFD)!= 0)goto _LL15B;_tmpFE=((
struct Cyc_Absyn_Var_d_struct*)_tmpFD)->f1;_LL15A: {struct _tuple0 _tmp104;struct
_tagged_arr*_tmp105;struct _tuple0*_tmp103=_tmpFE->name;_tmp104=*_tmp103;_tmp105=
_tmp104.f2;Cyc_current_source=(struct _tagged_arr*)_tmp105;Cyc_scan_type((void*)
_tmpFE->type);Cyc_scan_exp_opt(_tmpFE->initializer);goto _LL158;}_LL15B: if(*((int*)
_tmpFD)!= 1)goto _LL15D;_tmpFF=((struct Cyc_Absyn_Fn_d_struct*)_tmpFD)->f1;_LL15C: {
struct _tuple0 _tmp107;struct _tagged_arr*_tmp108;struct _tuple0*_tmp106=_tmpFF->name;
_tmp107=*_tmp106;_tmp108=_tmp107.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp108;Cyc_scan_type((void*)_tmpFF->ret_type);{struct Cyc_List_List*_tmp109=
_tmpFF->args;for(0;_tmp109 != 0;_tmp109=_tmp109->tl){struct _tuple14 _tmp10B;void*
_tmp10C;struct _tuple14*_tmp10A=(struct _tuple14*)_tmp109->hd;_tmp10B=*_tmp10A;
_tmp10C=_tmp10B.f3;Cyc_scan_type(_tmp10C);}}if(_tmpFF->cyc_varargs != 0)Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpFF->cyc_varargs))->type);if(
_tmpFF->is_inline)({void*_tmp10D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: ignoring inline function\n",
sizeof(char),35),_tag_arr(_tmp10D,sizeof(void*),0));});goto _LL158;}_LL15D: if(*((
int*)_tmpFD)!= 4)goto _LL15F;_tmp100=((struct Cyc_Absyn_Aggr_d_struct*)_tmpFD)->f1;
_LL15E: {struct _tuple0 _tmp10F;struct _tagged_arr*_tmp110;struct _tuple0*_tmp10E=
_tmp100->name;_tmp10F=*_tmp10E;_tmp110=_tmp10F.f2;Cyc_current_source=(struct
_tagged_arr*)_tmp110;if((unsigned int)_tmp100->fields){{struct Cyc_List_List*
_tmp111=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp100->fields))->v;
for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct Cyc_Absyn_Aggrfield*_tmp112=(struct
Cyc_Absyn_Aggrfield*)_tmp111->hd;Cyc_scan_type((void*)_tmp112->type);Cyc_scan_exp_opt(
_tmp112->width);}}{struct Cyc_List_List*_tmp113=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp100->fields))->v;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){;}}}goto
_LL158;}_LL15F: if(*((int*)_tmpFD)!= 6)goto _LL161;_tmp101=((struct Cyc_Absyn_Enum_d_struct*)
_tmpFD)->f1;_LL160: {struct _tuple0 _tmp115;struct _tagged_arr*_tmp116;struct
_tuple0*_tmp114=_tmp101->name;_tmp115=*_tmp114;_tmp116=_tmp115.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp116;if((unsigned int)_tmp101->fields){{struct Cyc_List_List*
_tmp117=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp101->fields))->v;
for(0;_tmp117 != 0;_tmp117=_tmp117->tl){struct Cyc_Absyn_Enumfield*_tmp118=(struct
Cyc_Absyn_Enumfield*)_tmp117->hd;Cyc_scan_exp_opt(_tmp118->tag);}}{struct Cyc_List_List*
_tmp119=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp101->fields))->v;
for(0;_tmp119 != 0;_tmp119=_tmp119->tl){;}}}goto _LL158;}_LL161: if(*((int*)_tmpFD)
!= 7)goto _LL163;_tmp102=((struct Cyc_Absyn_Typedef_d_struct*)_tmpFD)->f1;_LL162: {
struct _tuple0 _tmp11B;struct _tagged_arr*_tmp11C;struct _tuple0*_tmp11A=_tmp102->name;
_tmp11B=*_tmp11A;_tmp11C=_tmp11B.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp11C;if((unsigned int)_tmp102->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp102->defn))->v);goto _LL158;}_LL163: if(*((int*)_tmpFD)!= 2)goto
_LL165;_LL164:({void*_tmp11D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected let declaration\n",
sizeof(char),35),_tag_arr(_tmp11D,sizeof(void*),0));});exit(1);return 0;_LL165:
if(*((int*)_tmpFD)!= 5)goto _LL167;_LL166:({void*_tmp11E[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected tunion declaration\n",sizeof(char),38),
_tag_arr(_tmp11E,sizeof(void*),0));});exit(1);return 0;_LL167: if(*((int*)_tmpFD)
!= 3)goto _LL169;_LL168:({void*_tmp11F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(char),35),_tag_arr(_tmp11F,
sizeof(void*),0));});exit(1);return 0;_LL169: if(*((int*)_tmpFD)!= 8)goto _LL16B;
_LL16A:({void*_tmp120[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected namespace declaration\n",
sizeof(char),41),_tag_arr(_tmp120,sizeof(void*),0));});exit(1);return 0;_LL16B:
if(*((int*)_tmpFD)!= 9)goto _LL16D;_LL16C:({void*_tmp121[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected using declaration\n",sizeof(char),37),
_tag_arr(_tmp121,sizeof(void*),0));});exit(1);return 0;_LL16D: if(*((int*)_tmpFD)
!= 10)goto _LL158;_LL16E:({void*_tmp122[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n",sizeof(char),42),
_tag_arr(_tmp122,sizeof(void*),0));});exit(1);return 0;_LL158:;}return({struct
_tuple9*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp123->f2=*((struct Cyc_Set_Set**)_check_null(
Cyc_current_targets));_tmp123;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp124;_push_handler(& _tmp124);{int
_tmp126=0;if(setjmp(_tmp124.handler))_tmp126=1;if(!_tmp126){{struct Cyc_Set_Set*
_tmp127=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp127;};_pop_handler();}
else{void*_tmp125=(void*)_exn_thrown;void*_tmp129=_tmp125;_LL170: if(_tmp129 != 
Cyc_Core_Not_found)goto _LL172;_LL171: return((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL172:;
_LL173:(void)_throw(_tmp129);_LL16F:;}}}struct Cyc_Set_Set*Cyc_reachable(struct
Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp12A=curr;struct
_tagged_arr*_tmp12B=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp12A)
> 0){struct Cyc_Set_Set*_tmp12C=emptyset;struct Cyc_Iter_Iter _tmp12D=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp12A);while(((int(*)(struct Cyc_Iter_Iter,struct _tagged_arr**))Cyc_Iter_next)(
_tmp12D,& _tmp12B)){_tmp12C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp12C,Cyc_find(t,_tmp12B));}_tmp12A=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp12C,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp12A);}return curr;}}static void*Cyc_mode=(void*)0;
static int Cyc_gathering(){return Cyc_mode == (void*)1?1: Cyc_mode == (void*)2;}static
struct Cyc_Std___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _tagged_arr fmt,
struct _tagged_arr ap){if(Cyc_script_file == 0){({void*_tmp12F[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Internal error: script file is NULL\n",sizeof(char),37),
_tag_arr(_tmp12F,sizeof(void*),0));});exit(1);}return Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file),fmt,ap);}int Cyc_force_directory(struct _tagged_arr d){
if(Cyc_mode == (void*)2)({struct Cyc_Std_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _tagged_arr)d;{struct Cyc_Std_String_pa_struct _tmp131;_tmp131.tag=
0;_tmp131.f1=(struct _tagged_arr)d;{void*_tmp130[2]={& _tmp131,& _tmp132};Cyc_prscript(
_tag_arr("if ! test -e %s; then mkdir %s; fi\n",sizeof(char),36),_tag_arr(
_tmp130,sizeof(void*),2));}}});else{int _tmp133=({unsigned int _tmp136[0];Cyc_Std_open(
d,0,_tag_arr(_tmp136,sizeof(unsigned int),0));});if(_tmp133 == - 1){if(Cyc_Std_mkdir(
d,448)== - 1){({struct Cyc_Std_String_pa_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(
struct _tagged_arr)d;{void*_tmp134[1]={& _tmp135};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp134,sizeof(void*),1));}});return 1;}}else{close(_tmp133);}}return 0;}int Cyc_force_directory_prefixes(
struct _tagged_arr file){struct _tagged_arr _tmp137=Cyc_Std_strdup(file);struct Cyc_List_List*
_tmp138=0;while(1){_tmp137=Cyc_Filename_dirname((struct _tagged_arr)_tmp137);if(
_get_arr_size(_tmp137,sizeof(char))== 0)break;_tmp138=({struct Cyc_List_List*
_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=({struct _tagged_arr*_tmp13A=
_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=(struct _tagged_arr)_tmp137;_tmp13A;});
_tmp139->tl=_tmp138;_tmp139;});}for(0;_tmp138 != 0;_tmp138=_tmp138->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)_tmp138->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT";
int Cyc_process_file(struct _tagged_arr filename,struct Cyc_List_List*start_symbols,
struct Cyc_List_List*omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*
epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*
maybe;struct Cyc_Std___cycFILE*in_file;struct Cyc_Std___cycFILE*out_file;int
errorcode=0;({struct Cyc_Std_String_pa_struct _tmp13C;_tmp13C.tag=0;_tmp13C.f1=(
struct _tagged_arr)filename;{void*_tmp13B[1]={& _tmp13C};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("********************************* %s...\n",sizeof(char),41),_tag_arr(
_tmp13B,sizeof(void*),1));}});if(!Cyc_gathering())({struct Cyc_Std_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _tagged_arr)filename;{void*_tmp13D[1]={&
_tmp13E};Cyc_log(_tag_arr("\n%s:\n",sizeof(char),6),_tag_arr(_tmp13D,sizeof(void*),
1));}});{struct _tagged_arr _tmp13F=Cyc_Filename_basename(filename);struct
_tagged_arr _tmp140=Cyc_Filename_dirname(filename);struct _tagged_arr _tmp141=Cyc_Filename_chop_extension((
struct _tagged_arr)_tmp13F);struct _tagged_arr _tmp142=Cyc_Std_strconcat((struct
_tagged_arr)_tmp141,_tag_arr(".iA",sizeof(char),4));struct _tagged_arr _tmp143=
_get_arr_size(_tmp140,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp212;
_tmp212.tag=0;_tmp212.f1=(struct _tagged_arr)_tmp141;{void*_tmp211[1]={& _tmp212};
Cyc_Std_aprintf(_tag_arr("%s.iB",sizeof(char),6),_tag_arr(_tmp211,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp140,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct
_tagged_arr)_tmp141;{void*_tmp213[1]={& _tmp214};Cyc_Std_aprintf(_tag_arr("%s.iB",
sizeof(char),6),_tag_arr(_tmp213,sizeof(void*),1));}}));struct _tagged_arr _tmp144=
_get_arr_size(_tmp140,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp20E;
_tmp20E.tag=0;_tmp20E.f1=(struct _tagged_arr)_tmp141;{void*_tmp20D[1]={& _tmp20E};
Cyc_Std_aprintf(_tag_arr("%s.iC",sizeof(char),6),_tag_arr(_tmp20D,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp140,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp210;_tmp210.tag=0;_tmp210.f1=(struct
_tagged_arr)_tmp141;{void*_tmp20F[1]={& _tmp210};Cyc_Std_aprintf(_tag_arr("%s.iC",
sizeof(char),6),_tag_arr(_tmp20F,sizeof(void*),1));}}));struct _tagged_arr _tmp145=
_get_arr_size(_tmp140,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp20A;
_tmp20A.tag=0;_tmp20A.f1=(struct _tagged_arr)_tmp141;{void*_tmp209[1]={& _tmp20A};
Cyc_Std_aprintf(_tag_arr("%s.iD",sizeof(char),6),_tag_arr(_tmp209,sizeof(void*),
1));}}): Cyc_Filename_concat((struct _tagged_arr)_tmp140,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp20C;_tmp20C.tag=0;_tmp20C.f1=(struct
_tagged_arr)_tmp141;{void*_tmp20B[1]={& _tmp20C};Cyc_Std_aprintf(_tag_arr("%s.iD",
sizeof(char),6),_tag_arr(_tmp20B,sizeof(void*),1));}}));struct _tagged_arr _tmp146=
_get_arr_size(_tmp140,sizeof(char))== 0?({struct Cyc_Std_String_pa_struct _tmp206;
_tmp206.tag=0;_tmp206.f1=(struct _tagged_arr)_tmp13F;{void*_tmp205[1]={& _tmp206};
Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp205,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp140,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp208;_tmp208.tag=0;_tmp208.f1=(struct _tagged_arr)_tmp13F;{void*_tmp207[1]={&
_tmp208};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(char),4),_tag_arr(_tmp207,sizeof(
void*),1));}}));struct _handler_cons _tmp147;_push_handler(& _tmp147);{int _tmp149=0;
if(setjmp(_tmp147.handler))_tmp149=1;if(!_tmp149){if(Cyc_force_directory_prefixes(
filename)){int _tmp14A=1;_npop_handler(0);return _tmp14A;}if(Cyc_mode != (void*)3){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct _tmp14C;
_tmp14C.tag=0;_tmp14C.f1=(struct _tagged_arr)_tmp142;{void*_tmp14B[1]={& _tmp14C};
Cyc_prscript(_tag_arr("cat >%s <<XXX\n",sizeof(char),15),_tag_arr(_tmp14B,
sizeof(void*),1));}});{struct Cyc_List_List*_tmp14D=Cyc_current_cpp;for(0;_tmp14D
!= 0;_tmp14D=_tmp14D->tl){({struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;
_tmp14F.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp14D->hd);{void*_tmp14E[1]={&
_tmp14F};Cyc_prscript(_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp14E,sizeof(void*),
1));}});}}({struct Cyc_Std_String_pa_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(
struct _tagged_arr)filename;{void*_tmp150[1]={& _tmp151};Cyc_prscript(_tag_arr("#include <%s>\n",
sizeof(char),15),_tag_arr(_tmp150,sizeof(void*),1));}});({void*_tmp152[0]={};Cyc_prscript(
_tag_arr("XXX\n",sizeof(char),5),_tag_arr(_tmp152,sizeof(void*),0));});({struct
Cyc_Std_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(struct _tagged_arr)
_tmp142;{struct Cyc_Std_String_pa_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct
_tagged_arr)_tmp143;{void*_tmp153[2]={& _tmp154,& _tmp155};Cyc_prscript(_tag_arr("$GCC -E -dM -o %s -x c %s && \\\n",
sizeof(char),32),_tag_arr(_tmp153,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct
_tmp157;_tmp157.tag=0;_tmp157.f1=(struct _tagged_arr)_tmp144;{void*_tmp156[2]={&
_tmp157,& _tmp158};Cyc_prscript(_tag_arr("$GCC -E     -o %s -x c %s;\n",sizeof(
char),28),_tag_arr(_tmp156,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)_tmp142;{void*_tmp159[1]={&
_tmp15A};Cyc_prscript(_tag_arr("rm %s\n",sizeof(char),7),_tag_arr(_tmp159,
sizeof(void*),1));}});}else{maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp142,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _tagged_arr)_tmp142;{void*_tmp15B[1]={&
_tmp15C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp15B,sizeof(void*),1));}});{int _tmp15D=1;
_npop_handler(0);return _tmp15D;}}out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);{struct Cyc_List_List*_tmp15E=Cyc_current_cpp;for(0;_tmp15E != 0;_tmp15E=
_tmp15E->tl){Cyc_Std_fputs(*((struct _tagged_arr*)_tmp15E->hd),out_file);}}({
struct Cyc_Std_String_pa_struct _tmp160;_tmp160.tag=0;_tmp160.f1=(struct
_tagged_arr)filename;{void*_tmp15F[1]={& _tmp160};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n",sizeof(char),15),_tag_arr(_tmp15F,sizeof(void*),1));}});
Cyc_Std_fclose(out_file);{struct _tagged_arr _tmp161=Cstring_to_string(Ccomp);
struct _tagged_arr _tmp162=({struct Cyc_Std_String_pa_struct _tmp16A;_tmp16A.tag=0;
_tmp16A.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct _tmp169;
_tmp169.tag=0;_tmp169.f1=(struct _tagged_arr)_tmp143;{struct Cyc_Std_String_pa_struct
_tmp168;_tmp168.tag=0;_tmp168.f1=(struct _tagged_arr)_tmp161;{void*_tmp167[3]={&
_tmp168,& _tmp169,& _tmp16A};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(char),24),_tag_arr(_tmp167,sizeof(void*),3));}}}});if(!Cyc_Std_system((
struct _tagged_arr)_tmp162)){_tmp162=({struct Cyc_Std_String_pa_struct _tmp166;
_tmp166.tag=0;_tmp166.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct
_tmp165;_tmp165.tag=0;_tmp165.f1=(struct _tagged_arr)_tmp144;{struct Cyc_Std_String_pa_struct
_tmp164;_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)_tmp161;{void*_tmp163[3]={&
_tmp164,& _tmp165,& _tmp166};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",
sizeof(char),20),_tag_arr(_tmp163,sizeof(void*),3));}}}});Cyc_Std_system((struct
_tagged_arr)_tmp162);}Cyc_Std_remove((struct _tagged_arr)_tmp142);}}}if(Cyc_gathering()){
int _tmp16B=0;_npop_handler(0);return _tmp16B;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp143,_tag_arr("r",sizeof(char),2));if(
!((unsigned int)maybe))(int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);{struct Cyc_Lexing_lexbuf*_tmp16C=Cyc_Lexing_from_file(in_file);
struct _tuple9*entry;while((entry=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp16C))!= 0){struct _tuple9 _tmp16E;struct _tagged_arr*_tmp16F;
struct Cyc_Set_Set*_tmp170;struct _tuple9*_tmp16D=(struct _tuple9*)_check_null(
entry);_tmp16E=*_tmp16D;_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;((void(*)(struct
Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp16F,_tmp170);}Cyc_Std_fclose(in_file);maybe=Cyc_Std_fopen((struct
_tagged_arr)_tmp144,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(int)
_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp16C=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_Std_fopen((struct
_tagged_arr)_tmp145,_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_slurp_out)){
int _tmp171=1;_npop_handler(0);return _tmp171;}while(((int(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_slurp)(_tmp16C)){;}Cyc_Std_fclose(in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)Cyc_Std_remove((struct
_tagged_arr)_tmp144);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp145,_tag_arr("r",
sizeof(char),2));if(!((unsigned int)maybe)){int _tmp172=1;_npop_handler(0);return
_tmp172;}in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_Position_reset_position((
struct _tagged_arr)_tmp145);Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp173=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp145);{struct Cyc_List_List*_tmp174=_tmp173;for(0;_tmp174 != 0;
_tmp174=_tmp174->tl){struct _tuple9*_tmp175=Cyc_scan_decl((struct Cyc_Absyn_Decl*)
_tmp174->hd);if(_tmp175 == 0)continue;{struct _tuple9 _tmp177;struct _tagged_arr*
_tmp178;struct Cyc_Set_Set*_tmp179;struct _tuple9*_tmp176=(struct _tuple9*)
_check_null(_tmp175);_tmp177=*_tmp176;_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;{
struct Cyc_Set_Set*old;{struct _handler_cons _tmp17A;_push_handler(& _tmp17A);{int
_tmp17C=0;if(setjmp(_tmp17A.handler))_tmp17C=1;if(!_tmp17C){old=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,
_tmp178);;_pop_handler();}else{void*_tmp17B=(void*)_exn_thrown;void*_tmp17E=
_tmp17B;_LL175: if(_tmp17E != Cyc_Core_Not_found)goto _LL177;_LL176: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL174;_LL177:;_LL178:(void)_throw(_tmp17E);_LL174:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp178,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp179,old));}}}}{struct Cyc_Set_Set*_tmp17F=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp180=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp181=_tmp173;for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
struct Cyc_Absyn_Decl*_tmp182=(struct Cyc_Absyn_Decl*)_tmp181->hd;struct
_tagged_arr*name;{void*_tmp183=(void*)_tmp182->r;struct Cyc_Absyn_Vardecl*_tmp184;
struct Cyc_Absyn_Fndecl*_tmp185;struct Cyc_Absyn_Aggrdecl*_tmp186;struct Cyc_Absyn_Enumdecl*
_tmp187;struct Cyc_Absyn_Typedefdecl*_tmp188;_LL17A: if(*((int*)_tmp183)!= 0)goto
_LL17C;_tmp184=((struct Cyc_Absyn_Var_d_struct*)_tmp183)->f1;_LL17B: {struct
_tuple0 _tmp18A;struct _tagged_arr*_tmp18B;struct _tuple0*_tmp189=_tmp184->name;
_tmp18A=*_tmp189;_tmp18B=_tmp18A.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp18B);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp18B))name=0;else{name=(struct _tagged_arr*)_tmp18B;}goto _LL179;}
_LL17C: if(*((int*)_tmp183)!= 1)goto _LL17E;_tmp185=((struct Cyc_Absyn_Fn_d_struct*)
_tmp183)->f1;_LL17D: {struct _tuple0 _tmp18D;struct _tagged_arr*_tmp18E;struct
_tuple0*_tmp18C=_tmp185->name;_tmp18D=*_tmp18C;_tmp18E=_tmp18D.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp18E);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp18E))name=0;else{name=(struct _tagged_arr*)
_tmp18E;}goto _LL179;}_LL17E: if(*((int*)_tmp183)!= 4)goto _LL180;_tmp186=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp183)->f1;_LL17F: {struct _tuple0 _tmp190;struct
_tagged_arr*_tmp191;struct _tuple0*_tmp18F=_tmp186->name;_tmp190=*_tmp18F;_tmp191=
_tmp190.f2;name=(struct _tagged_arr*)_tmp191;goto _LL179;}_LL180: if(*((int*)
_tmp183)!= 6)goto _LL182;_tmp187=((struct Cyc_Absyn_Enum_d_struct*)_tmp183)->f1;
_LL181: {struct _tuple0 _tmp193;struct _tagged_arr*_tmp194;struct _tuple0*_tmp192=
_tmp187->name;_tmp193=*_tmp192;_tmp194=_tmp193.f2;name=(struct _tagged_arr*)
_tmp194;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp17F,(struct _tagged_arr*)_check_null(name)): 0)_tmp180=({struct Cyc_List_List*
_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->hd=_tmp182;_tmp195->tl=_tmp180;
_tmp195;});else{if((unsigned int)_tmp187->fields){struct Cyc_List_List*_tmp196=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp187->fields))->v;for(
0;_tmp196 != 0;_tmp196=_tmp196->tl){struct Cyc_Absyn_Enumfield*_tmp197=(struct Cyc_Absyn_Enumfield*)
_tmp196->hd;struct _tuple0 _tmp199;struct _tagged_arr*_tmp19A;struct _tuple0*_tmp198=
_tmp197->name;_tmp199=*_tmp198;_tmp19A=_tmp199.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp17F,_tmp19A)){_tmp180=({struct Cyc_List_List*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=_tmp182;_tmp19B->tl=_tmp180;
_tmp19B;});break;}}}}name=0;goto _LL179;}_LL182: if(*((int*)_tmp183)!= 7)goto
_LL184;_tmp188=((struct Cyc_Absyn_Typedef_d_struct*)_tmp183)->f1;_LL183: {struct
_tuple0 _tmp19D;struct _tagged_arr*_tmp19E;struct _tuple0*_tmp19C=_tmp188->name;
_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f2;name=(struct _tagged_arr*)_tmp19E;goto _LL179;}
_LL184: if(*((int*)_tmp183)!= 2)goto _LL186;_LL185: goto _LL187;_LL186: if(*((int*)
_tmp183)!= 5)goto _LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp183)!= 3)goto
_LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp183)!= 8)goto _LL18C;_LL18B: goto
_LL18D;_LL18C: if(*((int*)_tmp183)!= 9)goto _LL18E;_LL18D: goto _LL18F;_LL18E: if(*((
int*)_tmp183)!= 10)goto _LL179;_LL18F: name=0;goto _LL179;_LL179:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp17F,(
struct _tagged_arr*)_check_null(name)): 0)_tmp180=({struct Cyc_List_List*_tmp19F=
_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=_tmp182;_tmp19F->tl=_tmp180;_tmp19F;});}}
_tmp180=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp180);maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(char),2));if(!((
unsigned int)maybe)){int _tmp1A0=1;_npop_handler(0);return _tmp1A0;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _tagged_arr)_tmp146;{void*_tmp1A1[1]={&
_tmp1A2};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\nusing Std;\n",sizeof(
char),26),_tag_arr(_tmp1A1,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=
Cyc_Std_fopen((struct _tagged_arr)_tmp146,_tag_arr("w",sizeof(char),2));if(!((
unsigned int)maybe)){int _tmp1A3=1;_npop_handler(0);return _tmp1A3;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);{struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=(struct
_tagged_arr)filename;{void*_tmp1F6[1]={& _tmp1F7};Cyc_Std_aprintf(_tag_arr("_%s_",
sizeof(char),5),_tag_arr(_tmp1F6,sizeof(void*),1));}});{int _tmp1A4=0;for(0;
_tmp1A4 < _get_arr_size(ifdefmacro,sizeof(char));_tmp1A4 ++){if(((char*)ifdefmacro.curr)[
_tmp1A4]== '.'?1:((char*)ifdefmacro.curr)[_tmp1A4]== '/')((char*)ifdefmacro.curr)[
_tmp1A4]='_';else{if(((char*)ifdefmacro.curr)[_tmp1A4]!= '_'?((char*)ifdefmacro.curr)[
_tmp1A4]!= '/': 0)((char*)ifdefmacro.curr)[_tmp1A4]=(char)toupper((int)((char*)
ifdefmacro.curr)[_tmp1A4]);}}}({struct Cyc_Std_String_pa_struct _tmp1A7;_tmp1A7.tag=
0;_tmp1A7.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct
_tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1A5[2]={&
_tmp1A6,& _tmp1A7};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",
sizeof(char),23),_tag_arr(_tmp1A5,sizeof(void*),2));}}});if(prologue != 0){struct
Cyc_List_List*_tmp1A8=prologue;for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){struct
_tuple11 _tmp1AA;struct _tagged_arr _tmp1AB;struct _tagged_arr _tmp1AC;struct _tuple11*
_tmp1A9=(struct _tuple11*)_tmp1A8->hd;_tmp1AA=*_tmp1A9;_tmp1AB=_tmp1AA.f1;_tmp1AC=
_tmp1AA.f2;if(_tmp1AC.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1AB.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=_tmp1AB;_tmp1AD;})): 0)Cyc_Std_fputs(
_tmp1AC,out_file);else{({struct Cyc_Std_String_pa_struct _tmp1AF;_tmp1AF.tag=0;
_tmp1AF.f1=(struct _tagged_arr)_tmp1AB;{void*_tmp1AE[1]={& _tmp1AF};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp1AE,sizeof(void*),1));}});}}}({void*_tmp1B0[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("\nnamespace Std {\n",sizeof(char),18),_tag_arr(_tmp1B0,sizeof(void*),0));});{
struct Cyc_List_List*_tmp1B1=_tmp180;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
struct Cyc_Absyn_Decl*_tmp1B2=(struct Cyc_Absyn_Decl*)_tmp1B1->hd;int _tmp1B3=0;
struct _tagged_arr*name;{void*_tmp1B4=(void*)_tmp1B2->r;struct Cyc_Absyn_Vardecl*
_tmp1B5;struct Cyc_Absyn_Fndecl*_tmp1B6;struct Cyc_Absyn_Aggrdecl*_tmp1B7;struct
Cyc_Absyn_Enumdecl*_tmp1B8;struct Cyc_Absyn_Typedefdecl*_tmp1B9;_LL191: if(*((int*)
_tmp1B4)!= 0)goto _LL193;_tmp1B5=((struct Cyc_Absyn_Var_d_struct*)_tmp1B4)->f1;
_LL192: {struct _tuple0 _tmp1BB;struct _tagged_arr*_tmp1BC;struct _tuple0*_tmp1BA=
_tmp1B5->name;_tmp1BB=*_tmp1BA;_tmp1BC=_tmp1BB.f2;name=(struct _tagged_arr*)
_tmp1BC;goto _LL190;}_LL193: if(*((int*)_tmp1B4)!= 1)goto _LL195;_tmp1B6=((struct
Cyc_Absyn_Fn_d_struct*)_tmp1B4)->f1;_LL194: {struct _tuple0 _tmp1BE;struct
_tagged_arr*_tmp1BF;struct _tuple0*_tmp1BD=_tmp1B6->name;_tmp1BE=*_tmp1BD;_tmp1BF=
_tmp1BE.f2;name=(struct _tagged_arr*)_tmp1BF;goto _LL190;}_LL195: if(*((int*)
_tmp1B4)!= 4)goto _LL197;_tmp1B7=((struct Cyc_Absyn_Aggr_d_struct*)_tmp1B4)->f1;
_LL196: {struct _tuple0 _tmp1C1;struct _tagged_arr*_tmp1C2;struct _tuple0*_tmp1C0=
_tmp1B7->name;_tmp1C1=*_tmp1C0;_tmp1C2=_tmp1C1.f2;name=(struct _tagged_arr*)
_tmp1C2;goto _LL190;}_LL197: if(*((int*)_tmp1B4)!= 6)goto _LL199;_tmp1B8=((struct
Cyc_Absyn_Enum_d_struct*)_tmp1B4)->f1;_LL198: {struct _tuple0 _tmp1C4;struct
_tagged_arr*_tmp1C5;struct _tuple0*_tmp1C3=_tmp1B8->name;_tmp1C4=*_tmp1C3;_tmp1C5=
_tmp1C4.f2;name=(struct _tagged_arr*)_tmp1C5;goto _LL190;}_LL199: if(*((int*)
_tmp1B4)!= 7)goto _LL19B;_tmp1B9=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1B4)->f1;
_LL19A: {struct _tuple0 _tmp1C7;struct _tagged_arr*_tmp1C8;struct _tuple0*_tmp1C6=
_tmp1B9->name;_tmp1C7=*_tmp1C6;_tmp1C8=_tmp1C7.f2;name=(struct _tagged_arr*)
_tmp1C8;goto _LL190;}_LL19B: if(*((int*)_tmp1B4)!= 2)goto _LL19D;_LL19C: goto _LL19E;
_LL19D: if(*((int*)_tmp1B4)!= 5)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)
_tmp1B4)!= 3)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)_tmp1B4)!= 8)goto
_LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if(*((int*)_tmp1B4)!= 9)goto _LL1A5;_LL1A4: goto
_LL1A6;_LL1A5: if(*((int*)_tmp1B4)!= 10)goto _LL190;_LL1A6: name=0;goto _LL190;
_LL190:;}if(!((unsigned int)name)?!_tmp1B3: 0)continue;if((unsigned int)name){
ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(
struct _tagged_arr)*name;{void*_tmp1C9[1]={& _tmp1CA};Cyc_Std_aprintf(_tag_arr("_%s_def_",
sizeof(char),9),_tag_arr(_tmp1C9,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp1CC;_tmp1CC.tag=0;_tmp1CC.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1CB[1]={&
_tmp1CC};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(char),12),
_tag_arr(_tmp1CB,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1CE;
_tmp1CE.tag=0;_tmp1CE.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1CD[1]={&
_tmp1CE};Cyc_Std_fprintf(out_file,_tag_arr("#define %s\n",sizeof(char),12),
_tag_arr(_tmp1CD,sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp1CF[1];_tmp1CF[0]=_tmp1B2;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1CF,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1D0[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(char),8),_tag_arr(_tmp1D0,
sizeof(void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp1D1[1];_tmp1D1[0]=_tmp1B2;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1D1,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp143,_tag_arr("r",sizeof(char),2));if(!((unsigned int)maybe))(
int)_throw((void*)Cyc_NO_SUPPORT);in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);_tmp16C=Cyc_Lexing_from_file(in_file);{struct _tuple10*entry2;while((
entry2=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(
_tmp16C))!= 0){struct _tuple10 _tmp1D3;struct _tagged_arr _tmp1D4;struct _tagged_arr*
_tmp1D5;struct _tuple10*_tmp1D2=(struct _tuple10*)_check_null(entry2);_tmp1D3=*
_tmp1D2;_tmp1D4=_tmp1D3.f1;_tmp1D5=_tmp1D3.f2;if(((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(_tmp17F,_tmp1D5)){({struct Cyc_Std_String_pa_struct
_tmp1D7;_tmp1D7.tag=0;_tmp1D7.f1=(struct _tagged_arr)*_tmp1D5;{void*_tmp1D6[1]={&
_tmp1D7};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(char),12),
_tag_arr(_tmp1D6,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1D9;
_tmp1D9.tag=0;_tmp1D9.f1=(struct _tagged_arr)_tmp1D4;{void*_tmp1D8[1]={& _tmp1D9};
Cyc_Std_fprintf(out_file,_tag_arr("%s\n",sizeof(char),4),_tag_arr(_tmp1D8,
sizeof(void*),1));}});({void*_tmp1DA[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",
sizeof(char),8),_tag_arr(_tmp1DA,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);
if(Cyc_mode != (void*)3)Cyc_Std_remove((struct _tagged_arr)_tmp143);if(epilogue != 
0){struct Cyc_List_List*_tmp1DB=epilogue;for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){
struct _tuple11 _tmp1DD;struct _tagged_arr _tmp1DE;struct _tagged_arr _tmp1DF;struct
_tuple11*_tmp1DC=(struct _tuple11*)_tmp1DB->hd;_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DD.f1;
_tmp1DF=_tmp1DD.f2;if(_tmp1DF.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp1DE.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=_tmp1DE;_tmp1E0;})): 0)Cyc_Std_fputs(
_tmp1DF,out_file);else{({struct Cyc_Std_String_pa_struct _tmp1E2;_tmp1E2.tag=0;
_tmp1E2.f1=(struct _tagged_arr)_tmp1DE;{void*_tmp1E1[1]={& _tmp1E2};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(char),38),_tag_arr(
_tmp1E1,sizeof(void*),1));}});}}}({void*_tmp1E3[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("}\n",sizeof(char),3),_tag_arr(_tmp1E3,sizeof(void*),0));});({void*
_tmp1E4[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(char),8),
_tag_arr(_tmp1E4,sizeof(void*),0));});Cyc_Std_fclose(out_file);if(cstubs != 0){
out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cstubs_file);{struct Cyc_List_List*
_tmp1E5=cstubs;for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){struct _tuple11 _tmp1E7;
struct _tagged_arr _tmp1E8;struct _tagged_arr _tmp1E9;struct _tuple11*_tmp1E6=(struct
_tuple11*)_tmp1E5->hd;_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;if(
_tmp1E9.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1E8.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1EA=
_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=_tmp1E8;_tmp1EA;})): 0)Cyc_Std_fputs(
_tmp1E9,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=(struct
_tagged_arr)_tmp146;{void*_tmp1EB[1]={& _tmp1EC};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n\n",sizeof(char),16),_tag_arr(_tmp1EB,sizeof(void*),1));}});
if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
void*_tmp1ED[0]={};Cyc_Std_fprintf(out_file,_tag_arr("namespace Std {\n",sizeof(
char),17),_tag_arr(_tmp1ED,sizeof(void*),0));});{struct Cyc_List_List*_tmp1EE=
cycstubs;for(0;_tmp1EE != 0;_tmp1EE=_tmp1EE->tl){struct _tuple11 _tmp1F0;struct
_tagged_arr _tmp1F1;struct _tagged_arr _tmp1F2;struct _tuple11*_tmp1EF=(struct
_tuple11*)_tmp1EE->hd;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;if(
_tmp1F2.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?_tmp1F1.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*_tmp1F3=
_cycalloc(sizeof(*_tmp1F3));_tmp1F3[0]=_tmp1F1;_tmp1F3;})): 0)Cyc_Std_fputs(
_tmp1F2,out_file);}}({void*_tmp1F4[0]={};Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",
sizeof(char),4),_tag_arr(_tmp1F4,sizeof(void*),0));});}{int _tmp1F5=0;
_npop_handler(0);return _tmp1F5;}}}}}}};_pop_handler();}else{void*_tmp148=(void*)
_exn_thrown;void*_tmp1F9=_tmp148;_LL1A8:;_LL1A9: maybe=Cyc_Std_fopen(filename,
_tag_arr("w",sizeof(char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _tagged_arr)filename;{void*_tmp1FA[1]={&
_tmp1FB};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(char),33),_tag_arr(_tmp1FA,sizeof(void*),1));}});return 1;}out_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=(struct _tagged_arr)filename;{void*_tmp1FC[1]={&
_tmp1FD};Cyc_Std_fprintf(out_file,_tag_arr("#error -- %s is not supported on this platform\n",
sizeof(char),48),_tag_arr(_tmp1FC,sizeof(void*),1));}});Cyc_Std_fclose(out_file);
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp146,_tag_arr("w",sizeof(char),2));if(
!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp1FF;_tmp1FF.tag=0;
_tmp1FF.f1=(struct _tagged_arr)_tmp146;{void*_tmp1FE[1]={& _tmp1FF};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(char),33),
_tag_arr(_tmp1FE,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(
struct _tagged_arr)_tmp146;{void*_tmp200[1]={& _tmp201};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(char),48),
_tag_arr(_tmp200,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp203;_tmp203.tag=0;_tmp203.f1=(struct _tagged_arr)filename;{void*_tmp202[1]={&
_tmp203};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(char),52),_tag_arr(_tmp202,sizeof(void*),1));}});({void*_tmp204[0]={};Cyc_log(
_tag_arr("Not supported on this platform\n",sizeof(char),32),_tag_arr(_tmp204,
sizeof(void*),0));});Cyc_Std_remove((struct _tagged_arr)_tmp143);Cyc_Std_remove((
struct _tagged_arr)_tmp144);Cyc_Std_remove((struct _tagged_arr)_tmp145);return 0;
_LL1AA:;_LL1AB:(void)_throw(_tmp1F9);_LL1A7:;}}}}int Cyc_process_specfile(struct
_tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp215=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(char),2));if(!((unsigned int)_tmp215)){({struct Cyc_Std_String_pa_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=(struct _tagged_arr)file;{void*_tmp216[1]={&
_tmp217};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not open %s\n",
sizeof(char),26),_tag_arr(_tmp216,sizeof(void*),1));}});return 1;}{struct Cyc_Std___cycFILE*
_tmp218=(struct Cyc_Std___cycFILE*)_check_null(_tmp215);struct _tagged_arr buf=
_tag_arr(({unsigned int _tmp22B=(unsigned int)1024;char*_tmp22C=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp22B));{unsigned int _tmp22D=
_tmp22B;unsigned int i;for(i=0;i < _tmp22D;i ++){_tmp22C[i]='\000';}}_tmp22C;}),
sizeof(char),(unsigned int)1024);struct _tagged_arr _tmp219=Cyc_Std_getcwd(buf,
_get_arr_size(buf,sizeof(char)));if(Cyc_mode != (void*)2){if(Cyc_Std_chdir(dir)){({
struct Cyc_Std_String_pa_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=(struct
_tagged_arr)dir;{void*_tmp21A[1]={& _tmp21B};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n",sizeof(char),37),_tag_arr(
_tmp21A,sizeof(void*),1));}});return 1;}}if(Cyc_mode == (void*)1){struct
_tagged_arr _tmp21C=Cstring_to_string(Ccomp);Cyc_Std_system((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp21E;_tmp21E.tag=0;_tmp21E.f1=(struct
_tagged_arr)_tmp21C;{void*_tmp21D[1]={& _tmp21E};Cyc_Std_aprintf(_tag_arr("echo | %s -E -dM - -o INITMACROS.h\n",
sizeof(char),36),_tag_arr(_tmp21D,sizeof(void*),1));}}));}{struct Cyc_Lexing_lexbuf*
_tmp21F=Cyc_Lexing_from_file(_tmp218);struct _tuple12*entry;while((entry=((struct
_tuple12*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp21F))!= 0){struct
_tuple12 _tmp221;struct _tagged_arr _tmp222;struct Cyc_List_List*_tmp223;struct Cyc_List_List*
_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_List_List*
_tmp227;struct Cyc_List_List*_tmp228;struct _tuple12*_tmp220=(struct _tuple12*)
_check_null(entry);_tmp221=*_tmp220;_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;
_tmp224=_tmp221.f3;_tmp225=_tmp221.f4;_tmp226=_tmp221.f5;_tmp227=_tmp221.f6;
_tmp228=_tmp221.f7;if(Cyc_process_file(_tmp222,_tmp223,_tmp224,_tmp225,_tmp226,
_tmp227,_tmp228))return 1;}Cyc_Std_fclose(_tmp218);if(Cyc_mode != (void*)2){if(Cyc_Std_chdir((
struct _tagged_arr)_tmp219)){({struct Cyc_Std_String_pa_struct _tmp22A;_tmp22A.tag=
0;_tmp22A.f1=(struct _tagged_arr)_tmp219;{void*_tmp229[1]={& _tmp22A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not change directory to %s\n",sizeof(char),
41),_tag_arr(_tmp229,sizeof(void*),1));}});return 1;}}return 0;}}}int Cyc_getsize(
struct _tagged_arr dir,struct _tagged_arr includes,struct _tagged_arr type){struct
_tagged_arr _tmp22E=Cyc_Filename_concat(dir,_tag_arr("getsize.c",sizeof(char),10));
struct _tagged_arr _tmp22F=Cyc_Filename_concat(dir,_tag_arr("getsize",sizeof(char),
8));struct _tagged_arr _tmp230=Cyc_Filename_concat(dir,_tag_arr("getsize.out",
sizeof(char),12));struct Cyc_Std___cycFILE*_tmp231=Cyc_Std_fopen((struct
_tagged_arr)_tmp22E,_tag_arr("w",sizeof(char),2));if(!((unsigned int)_tmp231)){({
struct Cyc_Std_String_pa_struct _tmp233;_tmp233.tag=0;_tmp233.f1=(struct
_tagged_arr)_tmp22E;{void*_tmp232[1]={& _tmp233};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create %s\n",sizeof(char),28),_tag_arr(_tmp232,
sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp235;_tmp235.tag=0;
_tmp235.f1=(struct _tagged_arr)type;{void*_tmp234[1]={& _tmp235};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(char),40),_tag_arr(_tmp234,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_String_pa_struct
_tmp23A;_tmp23A.tag=0;_tmp23A.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp239;_tmp239.tag=0;_tmp239.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp238;_tmp238.tag=0;_tmp238.f1=(struct _tagged_arr)_tmp230;{struct Cyc_Std_String_pa_struct
_tmp237;_tmp237.tag=0;_tmp237.f1=(struct _tagged_arr)includes;{void*_tmp236[4]={&
_tmp237,& _tmp238,& _tmp239,& _tmp23A};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(_tmp231),_tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof(char),139),_tag_arr(_tmp236,sizeof(void*),4));}}}}});Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(_tmp231));{struct _tagged_arr _tmp23B=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp23C=({struct Cyc_Std_String_pa_struct
_tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _tagged_arr)_tmp22E;{struct Cyc_Std_String_pa_struct
_tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _tagged_arr)_tmp22F;{struct Cyc_Std_String_pa_struct
_tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _tagged_arr)_tmp23B;{void*_tmp24B[3]={&
_tmp24C,& _tmp24D,& _tmp24E};Cyc_Std_aprintf(_tag_arr("%s -o %s -x c %s",sizeof(
char),17),_tag_arr(_tmp24B,sizeof(void*),3));}}}});if(Cyc_Std_system((struct
_tagged_arr)_tmp23C)){({struct Cyc_Std_String_pa_struct _tmp23E;_tmp23E.tag=0;
_tmp23E.f1=(struct _tagged_arr)type;{void*_tmp23D[1]={& _tmp23E};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof(char),59),_tag_arr(_tmp23D,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp240;_tmp240.tag=0;_tmp240.f1=(struct _tagged_arr)type;{void*_tmp23F[1]={&
_tmp240};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(char),
40),_tag_arr(_tmp23F,sizeof(void*),1));}});return - 1;}{int _tmp241=Cyc_Std_system((
struct _tagged_arr)_tmp22F);struct Cyc_Std___cycFILE*_tmp242=Cyc_Std_fopen((struct
_tagged_arr)_tmp230,_tag_arr("r",sizeof(char),2));int w=- 1;if(({struct Cyc_Std_IntPtr_sa_struct
_tmp244;_tmp244.tag=2;_tmp244.f1=& w;{void*_tmp243[1]={& _tmp244};Cyc_Std_fscanf((
struct Cyc_Std___cycFILE*)_check_null(_tmp242),_tag_arr("%d",sizeof(char),3),
_tag_arr(_tmp243,sizeof(void*),1));}})!= 1){({struct Cyc_Std_String_pa_struct
_tmp246;_tmp246.tag=0;_tmp246.f1=(struct _tagged_arr)type;{void*_tmp245[1]={&
_tmp246};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(char),
40),_tag_arr(_tmp245,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_Int_pa_struct
_tmp24A;_tmp24A.tag=1;_tmp24A.f1=(int)((unsigned int)w);{struct Cyc_Std_String_pa_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=(struct _tagged_arr)_tmp230;{struct Cyc_Std_String_pa_struct
_tmp248;_tmp248.tag=0;_tmp248.f1=(struct _tagged_arr)type;{void*_tmp247[3]={&
_tmp248,& _tmp249,& _tmp24A};Cyc_log(_tag_arr("size of %s read from file %s is %d\n",
sizeof(char),36),_tag_arr(_tmp247,sizeof(void*),3));}}}});Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(_tmp242));Cyc_Std_remove((struct _tagged_arr)
_tmp230);Cyc_Std_remove((struct _tagged_arr)_tmp22E);Cyc_Std_remove((struct
_tagged_arr)_tmp22F);return w;}}}static char _tmp24F[13]="BUILDLIB.OUT";static
struct _tagged_arr Cyc_output_dir={_tmp24F,_tmp24F,_tmp24F + 13};static void Cyc_set_output_dir(
struct _tagged_arr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _tagged_arr s){Cyc_spec_files=({struct Cyc_List_List*
_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=({struct _tagged_arr*_tmp251=
_cycalloc(sizeof(*_tmp251));_tmp251[0]=s;_tmp251;});_tmp250->tl=Cyc_spec_files;
_tmp250;});}static void Cyc_set_GATHER(){Cyc_mode=(void*)1;}static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=(void*)2;}static void Cyc_set_FINISH(){Cyc_mode=(void*)3;}static int Cyc_badparse=
0;static void Cyc_unsupported_option(struct _tagged_arr s){({struct Cyc_Std_String_pa_struct
_tmp253;_tmp253.tag=0;_tmp253.f1=(struct _tagged_arr)s;{void*_tmp252[1]={& _tmp253};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Unsupported option %s\n",sizeof(char),
23),_tag_arr(_tmp252,sizeof(void*),1));}});Cyc_badparse=1;}void
GC_blacklist_warn_clear();struct _tuple15{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*options=({struct
_tuple15*_tmp264[5];_tmp264[4]=({struct _tuple15*_tmp271=_cycalloc(sizeof(*
_tmp271));_tmp271->f1=_tag_arr("-",sizeof(char),2);_tmp271->f2=1;_tmp271->f3=
_tag_arr("",sizeof(char),1);_tmp271->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp273;_tmp273.tag=1;_tmp273.f1=Cyc_unsupported_option;_tmp273;});_tmp272;});
_tmp271->f5=_tag_arr("",sizeof(char),1);_tmp271;});_tmp264[3]=({struct _tuple15*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->f1=_tag_arr("-finish",sizeof(char),
8);_tmp26E->f2=0;_tmp26E->f3=_tag_arr("",sizeof(char),1);_tmp26E->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp270;_tmp270.tag=0;_tmp270.f1=Cyc_set_FINISH;
_tmp270;});_tmp26F;});_tmp26E->f5=_tag_arr("Produce Cyclone headers from pre-gathered C library info",
sizeof(char),57);_tmp26E;});_tmp264[2]=({struct _tuple15*_tmp26B=_cycalloc(
sizeof(*_tmp26B));_tmp26B->f1=_tag_arr("-gatherscript",sizeof(char),14);_tmp26B->f2=
0;_tmp26B->f3=_tag_arr("",sizeof(char),1);_tmp26B->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26D;_tmp26D.tag=0;_tmp26D.f1=Cyc_set_GATHERSCRIPT;_tmp26D;});_tmp26C;});
_tmp26B->f5=_tag_arr("Produce a script to gather C library info",sizeof(char),42);
_tmp26B;});_tmp264[1]=({struct _tuple15*_tmp268=_cycalloc(sizeof(*_tmp268));
_tmp268->f1=_tag_arr("-gather",sizeof(char),8);_tmp268->f2=0;_tmp268->f3=
_tag_arr("",sizeof(char),1);_tmp268->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26A;_tmp26A.tag=0;_tmp26A.f1=Cyc_set_GATHER;_tmp26A;});_tmp269;});_tmp268->f5=
_tag_arr("Gather C library info but don't produce Cyclone headers",sizeof(char),
56);_tmp268;});_tmp264[0]=({struct _tuple15*_tmp265=_cycalloc(sizeof(*_tmp265));
_tmp265->f1=_tag_arr("-d",sizeof(char),3);_tmp265->f2=0;_tmp265->f3=_tag_arr(" <file>",
sizeof(char),8);_tmp265->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp266=
_cycalloc(sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Arg_String_spec_struct _tmp267;
_tmp267.tag=5;_tmp267.f1=Cyc_set_output_dir;_tmp267;});_tmp266;});_tmp265->f5=
_tag_arr("Set the output directory to <file>",sizeof(char),35);_tmp265;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp264,
sizeof(struct _tuple15*),5));});Cyc_Arg_parse(options,Cyc_add_spec_file,_tag_arr("Options:",
sizeof(char),9),argv);if(Cyc_badparse){Cyc_Arg_usage(options,_tag_arr("Options:",
sizeof(char),9));return 1;}if(Cyc_mode == (void*)2){Cyc_script_file=Cyc_Std_fopen(
_tag_arr("BUILDLIB.sh",sizeof(char),12),_tag_arr("w",sizeof(char),2));if(!((
unsigned int)Cyc_script_file)){({void*_tmp254[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Could not create file BUILDLIB.sh\n",sizeof(char),35),_tag_arr(_tmp254,
sizeof(void*),0));});exit(1);}({void*_tmp255[0]={};Cyc_prscript(_tag_arr("#!/bin/sh\n",
sizeof(char),11),_tag_arr(_tmp255,sizeof(void*),0));});({void*_tmp256[0]={};Cyc_prscript(
_tag_arr("GCC=\"gcc\"\n",sizeof(char),11),_tag_arr(_tmp256,sizeof(void*),0));});}
if(Cyc_force_directory_prefixes(Cyc_output_dir)?1: Cyc_force_directory(Cyc_output_dir)){({
struct Cyc_Std_String_pa_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp257[1]={& _tmp258};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(char),38),_tag_arr(
_tmp257,sizeof(void*),1));}});return 1;}if(Cyc_mode == (void*)2){({struct Cyc_Std_String_pa_struct
_tmp25A;_tmp25A.tag=0;_tmp25A.f1=(struct _tagged_arr)Cyc_output_dir;{void*_tmp259[
1]={& _tmp25A};Cyc_prscript(_tag_arr("cd %s\n",sizeof(char),7),_tag_arr(_tmp259,
sizeof(void*),1));}});({void*_tmp25B[0]={};Cyc_prscript(_tag_arr("echo | $GCC -E -dM - -o INITMACROS.h\n",
sizeof(char),38),_tag_arr(_tmp25B,sizeof(void*),0));});}if(!Cyc_gathering()){Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("BUILDLIB.LOG",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp25C[1]={& _tmp25D};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create log file in directory %s\n",sizeof(char),50),
_tag_arr(_tmp25C,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cstubs.c",sizeof(
char),9)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp25F;_tmp25F.tag=0;_tmp25F.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp25E[1]={& _tmp25F};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cstubs.c in directory %s\n",sizeof(char),50),
_tag_arr(_tmp25E,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=Cyc_Std_fopen((
struct _tagged_arr)Cyc_Filename_concat(Cyc_output_dir,_tag_arr("cycstubs.cyc",
sizeof(char),13)),_tag_arr("w",sizeof(char),2));if(!((unsigned int)Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _tmp261;_tmp261.tag=0;_tmp261.f1=(struct
_tagged_arr)Cyc_output_dir;{void*_tmp260[1]={& _tmp261};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create cycstubs.c in directory %s\n",sizeof(char),52),
_tag_arr(_tmp260,sizeof(void*),1));}});return 1;}({void*_tmp262[0]={};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),_tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof(char),32),_tag_arr(_tmp262,sizeof(void*),0));});Cyc_sizeof_unsignedlongint=
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
Cyc_spec_files))->hd),Cyc_output_dir)){({void*_tmp263[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("FATAL ERROR -- QUIT!\n",sizeof(char),22),_tag_arr(_tmp263,sizeof(void*),
0));});exit(1);}}if(Cyc_mode == (void*)2)Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_script_file));else{if(!Cyc_gathering()){Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file));}}return 0;}}
