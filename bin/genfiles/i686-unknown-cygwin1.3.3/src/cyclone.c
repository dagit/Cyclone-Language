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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct
_tagged_arr Cstring_to_string(char*);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),
struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc_Std___cycFILE;extern
struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*
f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct _tagged_arr);int
Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_feof(struct Cyc_Std___cycFILE*);
int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);unsigned int Cyc_Std_fread(struct _tagged_arr,
unsigned int,unsigned int,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Std_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
unsigned int Cyc_Std_fwrite(struct _tagged_arr,unsigned int,unsigned int,struct Cyc_Std___cycFILE*);
int Cyc_Std_printf(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_remove(struct
_tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE*
Cyc_Std_file_open(struct _tagged_arr,struct _tagged_arr);void Cyc_Std_file_close(
struct Cyc_Std___cycFILE*);typedef struct{int quot;int rem;}Cyc_Std_div_t;typedef
struct{int quot;int rem;}Cyc_Std_ldiv_t;struct _tagged_arr Cyc_Std_getenv(struct
_tagged_arr);int Cyc_Std_system(struct _tagged_arr);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);struct _tagged_arr Cyc_Std_str_sepstr(
struct Cyc_List_List*,struct _tagged_arr);struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct _tagged_arr Cyc_Std_strchr(struct
_tagged_arr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);
};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);
struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr);int Cyc_Filename_check_suffix(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];void Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);extern int Cyc_Parse_no_register;
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int
Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
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
first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc_Std___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct
Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();
struct Cyc_Interface_I*Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae);struct _tuple9{struct _tagged_arr f1;struct _tagged_arr f2;};
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple9*info);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___cycFILE*);void
Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc_Std___cycFILE*);struct Cyc_Interface_I*
Cyc_Interface_load(struct Cyc_Std___cycFILE*);void Cyc_Lex_lex_init();int Cyc_Toc_warn_bounds_checks;
static int Cyc_pp_r=0;static int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int
Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int
Cyc_ic_r=0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int
Cyc_stop_after_asmfile_r=0;static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=
0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=
0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=
0;static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int
Cyc_print_all_kinds_r=0;static struct _tagged_arr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _tagged_arr s){Cyc_output_file=({struct _tagged_arr*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=s;_tmp0;});}extern char*Cdef_inc_path;extern char*Cdef_lib_path;
extern char*Carch;extern char*Ccomp;extern char*Cversion;static char _tmp1[1]="";
static struct _tagged_arr Cyc_cpp={_tmp1,_tmp1,_tmp1 + 1};static void Cyc_set_cpp(
struct _tagged_arr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static
void Cyc_add_cpparg(struct _tagged_arr s){Cyc_cppargs=({struct Cyc_List_List*_tmp2=
_cycalloc(sizeof(*_tmp2));_tmp2->hd=({struct _tagged_arr*_tmp3=_cycalloc(sizeof(*
_tmp3));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}static struct
_tagged_arr Cyc_target_arch=(struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _tagged_arr s){Cyc_toc_r=1;Cyc_target_arch=s;}
static void Cyc_print_version(){({struct Cyc_Std_String_pa_struct _tmp6;_tmp6.tag=0;
_tmp6.f1=(struct _tagged_arr)Cstring_to_string(Cversion);{void*_tmp4[1]={& _tmp6};
Cyc_Std_printf(({const char*_tmp5="The Cyclone compiler, version %s\n";_tag_arr(
_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,34));}),_tag_arr(_tmp4,sizeof(void*),
1));}});({struct Cyc_Std_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct
_tagged_arr)Cstring_to_string(Ccomp);{struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=
0;_tmp9.f1=(struct _tagged_arr)Cstring_to_string(Carch);{void*_tmp7[2]={& _tmp9,&
_tmpA};Cyc_Std_printf(({const char*_tmp8="Compiled for architecture: %s CC=\"%s\"\n";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,39));}),_tag_arr(_tmp7,
sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(
struct _tagged_arr)Cstring_to_string(Cdef_lib_path);{void*_tmpB[1]={& _tmpD};Cyc_Std_printf(({
const char*_tmpC="Standard library directory: %s\n";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,32));}),_tag_arr(_tmpB,sizeof(void*),1));}});({struct
Cyc_Std_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _tagged_arr)
Cstring_to_string(Cdef_inc_path);{void*_tmpE[1]={& _tmp10};Cyc_Std_printf(({const
char*_tmpF="Standard include directory: %s\n";_tag_arr(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,32));}),_tag_arr(_tmpE,sizeof(void*),1));}});exit(0);}
static int Cyc_is_cyclone_sourcefile(struct _tagged_arr s){unsigned int _tmp11=Cyc_Std_strlen((
struct _tagged_arr)s);if(_tmp11 <= 4)return 0;else{return Cyc_Std_strcmp((struct
_tagged_arr)_tagged_arr_plus(s,sizeof(char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";
_tag_arr(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,5));}))== 0;}}static
struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _tagged_arr s){Cyc_ccargs=({
struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=({struct
_tagged_arr*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=s;_tmp14;});_tmp13->tl=
Cyc_ccargs;_tmp13;});}static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static
void Cyc_add_cyclone_exec_path(struct _tagged_arr s){unsigned int _tmp15=Cyc_Std_strlen((
struct _tagged_arr)s);if(_tmp15 <= 2)return;{struct _tagged_arr _tmp16=(struct
_tagged_arr)Cyc_Std_substring((struct _tagged_arr)s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct
_tagged_arr*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=
Cyc_cyclone_exec_path;_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static
void Cyc_add_libarg(struct _tagged_arr s){if(Cyc_Std_strcmp((struct _tagged_arr)s,({
const char*_tmp19="-lxml";_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,
6));}))== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1A=
_cycalloc(sizeof(*_tmp1A));_tmp1A->hd=({struct _tagged_arr*_tmp1B=_cycalloc(
sizeof(*_tmp1B));_tmp1B[0]=s;_tmp1B;});_tmp1A->tl=Cyc_libargs;_tmp1A;});}}static
void Cyc_set_save_temps(){Cyc_save_temps_r=1;Cyc_add_ccarg(({const char*_tmp1C="-save-temps";
_tag_arr(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,12));}));}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(({const char*_tmp1D="-M";_tag_arr(_tmp1D,sizeof(char),
_get_zero_arr_size(_tmp1D,3));}));}static struct _tagged_arr*Cyc_dependencies_target=
0;static void Cyc_set_dependencies_target(struct _tagged_arr s){Cyc_dependencies_target=({
struct _tagged_arr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=s;_tmp1E;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({
const char*_tmp1F="-c";_tag_arr(_tmp1F,sizeof(char),_get_zero_arr_size(_tmp1F,3));}));}
static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({const char*_tmp20="-no-cpp-precomp";
_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,16));}));Cyc_add_ccarg(({
const char*_tmp21="-no-cpp-precomp";_tag_arr(_tmp21,sizeof(char),
_get_zero_arr_size(_tmp21,16));}));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=
1;Cyc_set_save_temps();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({const
char*_tmp22="-DNO_CYC_BOUNDS_CHECKS";_tag_arr(_tmp22,sizeof(char),
_get_zero_arr_size(_tmp22,23));}));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({
const char*_tmp23="-DNO_CYC_NULL_CHECKS";_tag_arr(_tmp23,sizeof(char),
_get_zero_arr_size(_tmp23,21));}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp24="-DNO_CYC_PREFIX";_tag_arr(_tmp24,sizeof(char),
_get_zero_arr_size(_tmp24,16));}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({
const char*_tmp25="-DCYC_REGION_PROFILE";_tag_arr(_tmp25,sizeof(char),
_get_zero_arr_size(_tmp25,21));}));}static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({
const char*_tmp26="-pg";_tag_arr(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,4));}));}
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({
const char*_tmp27="-S";_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,3));}));}
static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(struct _tagged_arr s){
if(Cyc_Std_strcmp((struct _tagged_arr)s,({const char*_tmp28="cyc";_tag_arr(_tmp28,
sizeof(char),_get_zero_arr_size(_tmp28,4));}))== 0)Cyc_intype=(void*)1;else{if(
Cyc_Std_strcmp((struct _tagged_arr)s,({const char*_tmp29="none";_tag_arr(_tmp29,
sizeof(char),_get_zero_arr_size(_tmp29,5));}))== 0)Cyc_intype=(void*)0;else{({
struct Cyc_Std_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)s;{
void*_tmp2A[1]={& _tmp2C};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp2B="Input type '%s' not supported\n";
_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,31));}),_tag_arr(_tmp2A,
sizeof(void*),1));}});}}}struct _tagged_arr Cyc_make_base_filename(struct
_tagged_arr s,struct _tagged_arr*output_file){struct _tagged_arr _tmp2D=Cyc_Filename_dirname(
output_file == 0?s:*((struct _tagged_arr*)output_file));struct _tagged_arr _tmp2E=
Cyc_Filename_chop_extension((struct _tagged_arr)Cyc_Filename_basename(s));struct
_tagged_arr _tmp2F=Cyc_Std_strlen((struct _tagged_arr)_tmp2D)> 0?Cyc_Filename_concat((
struct _tagged_arr)_tmp2D,(struct _tagged_arr)_tmp2E): _tmp2E;return(struct
_tagged_arr)_tmp2F;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(
struct _tagged_arr s){if(Cyc_intype == (void*)1?1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({struct
_tagged_arr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=s;_tmp31;});_tmp30->tl=
Cyc_cyclone_files;_tmp30;});{struct _tagged_arr _tmp32=Cyc_make_base_filename(s,
Cyc_output_file);struct _tagged_arr _tmp33=Cyc_Std_strconcat((struct _tagged_arr)
_tmp32,({const char*_tmp34=".c";_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(
_tmp34,3));}));Cyc_add_ccarg((struct _tagged_arr)_tmp33);}}else{Cyc_add_ccarg(s);}}
static void Cyc_remove_file(struct _tagged_arr s){if(Cyc_save_temps_r)return;else{
Cyc_Std_remove(s);}}int Cyc_compile_failure=0;struct Cyc_Std___cycFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))
_tmp37=1;if(!_tmp37){{struct Cyc_Std___cycFILE*_tmp38=(struct Cyc_Std___cycFILE*)
Cyc_Std_file_open(filename,mode);_npop_handler(0);return _tmp38;};_pop_handler();}
else{void*_tmp36=(void*)_exn_thrown;void*_tmp3A=_tmp36;_LL1:;_LL2: Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct
_tagged_arr)filename;{struct Cyc_Std_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _tagged_arr)msg_part;{void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp3C="\nError: couldn't open %s %s\n";_tag_arr(_tmp3C,sizeof(char),
_get_zero_arr_size(_tmp3C,29));}),_tag_arr(_tmp3B,sizeof(void*),2));}}});Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp3A);
_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){struct
_tagged_arr exn_string=({const char*_tmp5C="";_tag_arr(_tmp5C,sizeof(char),
_get_zero_arr_size(_tmp5C,1));});struct _tagged_arr explain_string=({const char*
_tmp5B="";_tag_arr(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,1));});int
other_exn=0;void*ex=(void*)({struct Cyc_Core_Impossible_struct*_tmp58=_cycalloc(
sizeof(*_tmp58));_tmp58[0]=({struct Cyc_Core_Impossible_struct _tmp59;_tmp59.tag=
Cyc_Core_Impossible;_tmp59.f1=({const char*_tmp5A="";_tag_arr(_tmp5A,sizeof(char),
_get_zero_arr_size(_tmp5A,1));});_tmp59;});_tmp58;});struct Cyc_List_List*_tmp3F=
0;{struct _handler_cons _tmp40;_push_handler(& _tmp40);{int _tmp42=0;if(setjmp(
_tmp40.handler))_tmp42=1;if(!_tmp42){_tmp3F=f(env,tds);;_pop_handler();}else{
void*_tmp41=(void*)_exn_thrown;void*_tmp44=_tmp41;struct _tagged_arr _tmp45;struct
_tagged_arr _tmp46;struct _tagged_arr _tmp47;_LL6: if(*((void**)_tmp44)!= Cyc_Core_Impossible)
goto _LL8;_tmp45=((struct Cyc_Core_Impossible_struct*)_tmp44)->f1;_LL7: exn_string=({
const char*_tmp48="Exception Core::Impossible";_tag_arr(_tmp48,sizeof(char),
_get_zero_arr_size(_tmp48,27));});explain_string=_tmp45;goto _LL5;_LL8: if(_tmp44
!= Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp49="Exception Dict::Absent";
_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,23));});goto _LL5;_LLA: if(*((
void**)_tmp44)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp46=((struct Cyc_Core_Invalid_argument_struct*)
_tmp44)->f1;_LLB: exn_string=({const char*_tmp4A="Exception Core::Invalid_argument";
_tag_arr(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,33));});explain_string=
_tmp46;goto _LL5;_LLC: if(*((void**)_tmp44)!= Cyc_Core_Failure)goto _LLE;_tmp47=((
struct Cyc_Core_Failure_struct*)_tmp44)->f1;_LLD: exn_string=({const char*_tmp4B="Exception Core::Failure";
_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,24));});explain_string=
_tmp47;goto _LL5;_LLE:;_LLF: ex=_tmp44;other_exn=1;exn_string=({const char*_tmp4C="Uncaught exception";
_tag_arr(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,19));});goto _LL5;_LL10:;
_LL11:(void)_throw(_tmp44);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_Std_strcmp((struct _tagged_arr)exn_string,({const char*_tmp4D="";_tag_arr(
_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,1));}))!= 0){Cyc_compile_failure=1;({
struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)
explain_string;{struct Cyc_Std_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(
struct _tagged_arr)stage_name;{struct Cyc_Std_String_pa_struct _tmp50;_tmp50.tag=0;
_tmp50.f1=(struct _tagged_arr)exn_string;{void*_tmp4E[3]={& _tmp50,& _tmp51,& _tmp52};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp4F="\n%s thrown during %s: %s";
_tag_arr(_tmp4F,sizeof(char),_get_zero_arr_size(_tmp4F,25));}),_tag_arr(_tmp4E,
sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*_tmp53[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp54="\nCOMPILATION FAILED!\n";_tag_arr(_tmp54,
sizeof(char),_get_zero_arr_size(_tmp54,22));}),_tag_arr(_tmp53,sizeof(void*),0));});
Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);if(other_exn)(int)
_throw(ex);return _tmp3F;}else{if(Cyc_v_r){({struct Cyc_Std_String_pa_struct _tmp57;
_tmp57.tag=0;_tmp57.f1=(struct _tagged_arr)stage_name;{void*_tmp55[1]={& _tmp57};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp56="%s completed.\n";_tag_arr(
_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,15));}),_tag_arr(_tmp55,sizeof(
void*),1));}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);return
_tmp3F;}}return _tmp3F;}struct Cyc_List_List*Cyc_do_parse(struct Cyc_Std___cycFILE*
f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp5D=Cyc_Parse_parse_file(
f);Cyc_Lex_lex_init();return _tmp5D;}}struct Cyc_List_List*Cyc_do_typecheck(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){Cyc_Tc_tc(te,1,tds);if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int
ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple10{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Std___cycFILE*f2;struct Cyc_Std___cycFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple10*params,struct Cyc_List_List*
tds){struct _tuple10 _tmp5F;struct Cyc_Tcenv_Tenv*_tmp60;struct Cyc_Std___cycFILE*
_tmp61;struct Cyc_Std___cycFILE*_tmp62;struct _tuple10*_tmp5E=params;_tmp5F=*
_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;_tmp62=_tmp5F.f3;{struct Cyc_Interface_I*
_tmp63=Cyc_Interface_extract(_tmp60->ae);if(_tmp61 == 0)Cyc_Interface_save(_tmp63,
_tmp62);else{struct Cyc_Interface_I*_tmp64=Cyc_Interface_parse((struct Cyc_Std___cycFILE*)
_check_null(_tmp61));if(!Cyc_Interface_is_subinterface(_tmp64,_tmp63,({struct
_tuple9*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=({const char*_tmp67="written interface";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,18));});_tmp65->f2=({
const char*_tmp66="maximal interface";_tag_arr(_tmp66,sizeof(char),
_get_zero_arr_size(_tmp66,18));});_tmp65;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp64,_tmp62);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){return Cyc_Tovc_tovc(tds);}static char _tmp68[1]="";
static struct _tagged_arr Cyc_cyc_include={_tmp68,_tmp68,_tmp68 + 1};static void Cyc_set_cyc_include(
struct _tagged_arr f){Cyc_cyc_include=f;}struct Cyc_List_List*Cyc_do_print(struct
Cyc_Std___cycFILE*out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params
params_r;if(Cyc_tc_r)params_r=Cyc_Absynpp_cyc_params_r;else{params_r=Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs=!Cyc_noexpand_r;params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r;params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;{struct Cyc_Std___cycFILE*_tmp69=
Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp72="r";_tag_arr(_tmp72,sizeof(
char),_get_zero_arr_size(_tmp72,2));}),({const char*_tmp73="internal compiler file";
_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,23));}));if(_tmp69 == 0)
return tds;{struct Cyc_Std___cycFILE*_tmp6A=(struct Cyc_Std___cycFILE*)_check_null(
_tmp69);unsigned int n_read=1024;unsigned int n_written;char buf[1025];{
unsigned int _tmp71=1024;unsigned int i;for(i=0;i < _tmp71;i ++){buf[i]='\000';}buf[
_tmp71]=(char)0;}while(n_read == 1024){n_read=Cyc_Std_fread(({char*_tmp6B=buf;
_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,1025));}),1,1024,_tmp6A);
if(n_read != 1024?!Cyc_Std_feof(_tmp6A): 0){Cyc_compile_failure=1;({void*_tmp6C[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp6D="\nError: problem copying cyc_include.h\n";
_tag_arr(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,39));}),_tag_arr(_tmp6C,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);
return tds;}n_written=Cyc_Std_fwrite(({char*_tmp6E=buf;_tag_arr(_tmp6E,sizeof(
char),_get_zero_arr_size(_tmp6E,1025));}),1,n_read,out_file);if(n_read != 
n_written){Cyc_compile_failure=1;({void*_tmp6F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp70="\nError: problem copying cyc_include.h\n";_tag_arr(_tmp70,
sizeof(char),_get_zero_arr_size(_tmp70,39));}),_tag_arr(_tmp6F,sizeof(void*),0));});
Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);return tds;}}Cyc_Std_fclose(
_tmp6A);if(Cyc_pp_r){Cyc_Absynpp_set_params(& params_r);Cyc_Absynpp_decllist2file(
tds,out_file);}else{Cyc_Absyndump_set_params(& params_r);Cyc_Absyndump_dumpdecllist2file(
tds,out_file);}Cyc_Std_fflush((struct Cyc_Std___cycFILE*)out_file);return tds;}}}
static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _tagged_arr*)((struct _tagged_arr*)((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->hd)));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _tagged_arr s,char c){if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)
return 0;{struct Cyc_List_List*_tmp74=0;unsigned int _tmp75=Cyc_Std_strlen((struct
_tagged_arr)s);while(_tmp75 > 0){struct _tagged_arr _tmp76=Cyc_Std_strchr(s,c);if(
_tmp76.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){_tmp74=({struct Cyc_List_List*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=({struct _tagged_arr*_tmp78=
_cycalloc(sizeof(*_tmp78));_tmp78[0]=s;_tmp78;});_tmp77->tl=_tmp74;_tmp77;});
break;}else{_tmp74=({struct Cyc_List_List*_tmp79=_cycalloc(sizeof(*_tmp79));
_tmp79->hd=({struct _tagged_arr*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=(
struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)s,0,(unsigned int)((((
struct _tagged_arr)_tmp76).curr - s.curr)/ sizeof(char)));_tmp7A;});_tmp79->tl=
_tmp74;_tmp79;});_tmp75 -=(((struct _tagged_arr)_tmp76).curr - s.curr)/ sizeof(char);
s=_tagged_arr_plus(_tmp76,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp74);}}static int Cyc_file_exists(
struct _tagged_arr file){struct Cyc_Std___cycFILE*f=0;{struct _handler_cons _tmp7B;
_push_handler(& _tmp7B);{int _tmp7D=0;if(setjmp(_tmp7B.handler))_tmp7D=1;if(!
_tmp7D){f=(struct Cyc_Std___cycFILE*)Cyc_Std_file_open(file,({const char*_tmp7E="r";
_tag_arr(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,2));}));;_pop_handler();}
else{void*_tmp7C=(void*)_exn_thrown;void*_tmp80=_tmp7C;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmp80);_LL12:;}}}if(f == 0)return 0;else{Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(f));return 1;}}static struct _tagged_arr Cyc_sprint_list(
struct Cyc_List_List*dirs){struct _tagged_arr tmp=({const char*_tmp83="";_tag_arr(
_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,1));});for(0;dirs != 0;dirs=dirs->tl){
struct _tagged_arr _tmp81=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));
if(_tmp81.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen((
struct _tagged_arr)_tmp81)== 0)continue;_tmp81=(struct _tagged_arr)Cyc_Std_strconcat((
struct _tagged_arr)_tmp81,({const char*_tmp82=":";_tag_arr(_tmp82,sizeof(char),
_get_zero_arr_size(_tmp82,2));}));tmp=(struct _tagged_arr)Cyc_Std_strconcat((
struct _tagged_arr)_tmp81,(struct _tagged_arr)tmp);}return tmp;}static struct
_tagged_arr*Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr file){if(file.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){
struct _tagged_arr _tmp84=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));
if(_tmp84.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen((
struct _tagged_arr)_tmp84)== 0)continue;{struct _tagged_arr s=(struct _tagged_arr)
Cyc_Filename_concat(_tmp84,file);if(Cyc_file_exists(s))return({struct _tagged_arr*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=s;_tmp85;});}}return 0;}static struct
_tagged_arr Cyc_do_find(struct Cyc_List_List*dirs,struct _tagged_arr file){struct
_tagged_arr*_tmp86=Cyc_find(dirs,file);if(_tmp86 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87[0]=({struct Cyc_Core_Failure_struct _tmp88;_tmp88.tag=Cyc_Core_Failure;
_tmp88.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8C;_tmp8C.tag=0;
_tmp8C.f1=(struct _tagged_arr)Cyc_sprint_list(dirs);{struct Cyc_Std_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _tagged_arr)file;{void*_tmp89[2]={& _tmp8B,&
_tmp8C};Cyc_Std_aprintf(({const char*_tmp8A="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,56));}),_tag_arr(_tmp89,
sizeof(void*),2));}}});_tmp88;});_tmp87;}));}return*((struct _tagged_arr*)_tmp86);}
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_tagged_arr subdir){struct Cyc_List_List*_tmp8D=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp8D=({struct Cyc_List_List*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->hd=({
struct _tagged_arr*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=(struct _tagged_arr)
Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd)),subdir);
_tmp8F;});_tmp8E->tl=_tmp8D;_tmp8E;});}_tmp8D=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8D);return _tmp8D;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _tagged_arr subdir){struct Cyc_List_List*
_tmp90=0;for(0;dirs != 0;dirs=dirs->tl){_tmp90=({struct Cyc_List_List*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91->hd=(struct _tagged_arr*)dirs->hd;_tmp91->tl=
_tmp90;_tmp91;});_tmp90=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));
_tmp92->hd=({struct _tagged_arr*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=(
struct _tagged_arr)Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)
dirs->hd)),subdir);_tmp93;});_tmp92->tl=_tmp90;_tmp92;});}_tmp90=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp90);return _tmp90;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp94=Cyc_Std_strlen((struct _tagged_arr)s);int
_tmp95=0;int _tmp96=0;{int i=0;for(0;i < _tmp94;i ++){char _tmp97=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(_tmp97 == '\'')_tmp95 ++;else{if(
Cyc_is_other_special(_tmp97))_tmp96 ++;}}}if(_tmp95 == 0?_tmp96 == 0: 0)return s;if(
_tmp95 == 0)return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct _tagged_arr*
_tmp98[3];_tmp98[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);_tmp98[1]=({struct _tagged_arr*_tmp9A=_cycalloc(sizeof(*_tmp9A));
_tmp9A[0]=(struct _tagged_arr)s;_tmp9A;});_tmp98[0]=_init_tag_arr(_cycalloc(
sizeof(struct _tagged_arr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp98,sizeof(struct _tagged_arr*),3));}));{
unsigned int _tmp9C=(_tmp94 + _tmp95)+ _tmp96;struct _tagged_arr s2=({unsigned int
_tmpA6=_tmp9C + 1;char*_tmpA7=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmpA6 + 1));struct _tagged_arr _tmpA9=_tag_arr(_tmpA7,sizeof(char),_tmpA6 + 1);{
unsigned int _tmpA8=_tmpA6;unsigned int i;for(i=0;i < _tmpA8;i ++){_tmpA7[i]='\000';}
_tmpA7[_tmpA8]=(char)0;}_tmpA9;});int _tmp9D=0;int _tmp9E=0;for(0;_tmp9D < _tmp94;
_tmp9D ++){char _tmp9F=*((const char*)_check_unknown_subscript(s,sizeof(char),
_tmp9D));if(_tmp9F == '\''?1: Cyc_is_other_special(_tmp9F))({struct _tagged_arr
_tmpA0=_tagged_arr_plus(s2,sizeof(char),_tmp9E ++);char _tmpA1=*((char*)
_check_unknown_subscript(_tmpA0,sizeof(char),0));char _tmpA2='\\';if(
_get_arr_size(_tmpA0,sizeof(char))== 1?_tmpA1 == '\000'?_tmpA2 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA0.curr)=_tmpA2;});({struct _tagged_arr _tmpA3=
_tagged_arr_plus(s2,sizeof(char),_tmp9E ++);char _tmpA4=*((char*)
_check_unknown_subscript(_tmpA3,sizeof(char),0));char _tmpA5=_tmp9F;if(
_get_arr_size(_tmpA3,sizeof(char))== 1?_tmpA4 == '\000'?_tmpA5 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA3.curr)=_tmpA5;});}return(struct _tagged_arr)
s2;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){
return({struct _tagged_arr*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=Cyc_sh_escape_string(*((
struct _tagged_arr*)sp));_tmpAA;});}static void Cyc_process_file(struct _tagged_arr
filename){struct _tagged_arr _tmpAB=Cyc_make_base_filename(filename,Cyc_output_file);
struct _tagged_arr _tmpAC=Cyc_Std_strconcat((struct _tagged_arr)_tmpAB,({const char*
_tmp101=".cyp";_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,5));}));
struct _tagged_arr _tmpAD=Cyc_Std_strconcat((struct _tagged_arr)_tmpAB,({const char*
_tmp100=".cyci";_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,6));}));
struct _tagged_arr _tmpAE=Cyc_Std_strconcat((struct _tagged_arr)_tmpAB,({const char*
_tmpFF=".cycio";_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,7));}));
struct _tagged_arr _tmpAF=Cyc_Std_strconcat((struct _tagged_arr)_tmpAB,({const char*
_tmpFE=".c";_tag_arr(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,3));}));if(
Cyc_v_r)({struct Cyc_Std_String_pa_struct _tmpB2;_tmpB2.tag=0;_tmpB2.f1=(struct
_tagged_arr)filename;{void*_tmpB0[1]={& _tmpB2};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmpB1="Compiling %s\n";_tag_arr(_tmpB1,sizeof(char),
_get_zero_arr_size(_tmpB1,14));}),_tag_arr(_tmpB0,sizeof(void*),1));}});{struct
Cyc_Std___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmpFC="r";_tag_arr(
_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,2));}),({const char*_tmpFD="input file";
_tag_arr(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,11));}));if(Cyc_compile_failure)
return;Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(f0));{struct
_tagged_arr _tmpB3=Cyc_Std_str_sepstr(({struct Cyc_List_List*_tmpF9=_cycalloc(
sizeof(*_tmpF9));_tmpF9->hd=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);_tmpF9->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));
_tmpF9;}),({const char*_tmpFB=" ";_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(
_tmpFB,2));}));struct _tagged_arr def_inc_path=(struct _tagged_arr)
Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpB4=Cyc_add_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);_tmpB4=Cyc_add_subdir(_tmpB4,({const char*_tmpB5="include";
_tag_arr(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,8));}));if(Cyc_Std_strlen((
struct _tagged_arr)def_inc_path)> 0)_tmpB4=({struct Cyc_List_List*_tmpB6=_cycalloc(
sizeof(*_tmpB6));_tmpB6->hd=({struct _tagged_arr*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=def_inc_path;_tmpB7;});_tmpB6->tl=_tmpB4;_tmpB6;});{struct _tagged_arr
_tmpB8=Cyc_Std_getenv(({const char*_tmpF8="CYCLONE_INCLUDE_PATH";_tag_arr(_tmpF8,
sizeof(char),_get_zero_arr_size(_tmpF8,21));}));if(_tmpB8.curr != (_tag_arr(0,0,0)).curr)
_tmpB4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_split_by_char((struct _tagged_arr)_tmpB8,':'),_tmpB4);{
struct _tagged_arr stdinc_string=(struct _tagged_arr)Cyc_Std_str_sepstr(({struct Cyc_List_List*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmpF5->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpB4);_tmpF5;}),({const char*_tmpF7=" -I";_tag_arr(
_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,4));}));struct _tagged_arr
ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)ofile_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_output_file));{void*
_tmpB9[1]={& _tmpBB};Cyc_Std_aprintf(({const char*_tmpBA=" > %s";_tag_arr(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,6));}),_tag_arr(_tmpB9,sizeof(void*),1));}});
else{ofile_string=({const char*_tmpBC="";_tag_arr(_tmpBC,sizeof(char),
_get_zero_arr_size(_tmpBC,1));});}}else{ofile_string=(struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _tagged_arr)Cyc_sh_escape_string((
struct _tagged_arr)_tmpAC);{void*_tmpBD[1]={& _tmpBF};Cyc_Std_aprintf(({const char*
_tmpBE=" > %s";_tag_arr(_tmpBE,sizeof(char),_get_zero_arr_size(_tmpBE,6));}),
_tag_arr(_tmpBD,sizeof(void*),1));}});}{struct _tagged_arr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0)fixup_string=({const char*_tmpC0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_arr(_tmpC0,sizeof(char),_get_zero_arr_size(_tmpC0,35));});else{fixup_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpC3;_tmpC3.tag=0;_tmpC3.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_dependencies_target));{
void*_tmpC1[1]={& _tmpC3};Cyc_Std_aprintf(({const char*_tmpC2=" | sed 's/^.*\\.cyc\\.o:/%s:/'";
_tag_arr(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,29));}),_tag_arr(_tmpC1,
sizeof(void*),1));}});}}else{fixup_string=({const char*_tmpC4="";_tag_arr(_tmpC4,
sizeof(char),_get_zero_arr_size(_tmpC4,1));});}{struct _tagged_arr _tmpC5=({struct
Cyc_Std_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _tagged_arr)
ofile_string;{struct Cyc_Std_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(
struct _tagged_arr)fixup_string;{struct Cyc_Std_String_pa_struct _tmpF2;_tmpF2.tag=
0;_tmpF2.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{struct Cyc_Std_String_pa_struct
_tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _tagged_arr)stdinc_string;{struct Cyc_Std_String_pa_struct
_tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct _tagged_arr)_tmpB3;{struct Cyc_Std_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)Cyc_cpp;{void*_tmpED[6]={&
_tmpEF,& _tmpF0,& _tmpF1,& _tmpF2,& _tmpF3,& _tmpF4};Cyc_Std_aprintf(({const char*
_tmpEE="%s %s%s %s%s%s";_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,
15));}),_tag_arr(_tmpED,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_Std_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _tagged_arr)_tmpC5;{void*_tmpC6[1]={& _tmpC8};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpC7="%s\n";_tag_arr(_tmpC7,sizeof(
char),_get_zero_arr_size(_tmpC7,4));}),_tag_arr(_tmpC6,sizeof(void*),1));}});if(
Cyc_Std_system((struct _tagged_arr)_tmpC5)!= 0){Cyc_compile_failure=1;({void*
_tmpC9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpCA="\nError: preprocessing\n";
_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,23));}),_tag_arr(_tmpC9,
sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((
struct _tagged_arr)_tmpAC);{struct Cyc_Std___cycFILE*in_file=Cyc_try_file_open((
struct _tagged_arr)_tmpAC,({const char*_tmpEB="r";_tag_arr(_tmpEB,sizeof(char),
_get_zero_arr_size(_tmpEB,2));}),({const char*_tmpEC="file";_tag_arr(_tmpEC,
sizeof(char),_get_zero_arr_size(_tmpEC,5));}));if(Cyc_compile_failure)return;{
struct Cyc_List_List*tds=0;{struct _handler_cons _tmpCB;_push_handler(& _tmpCB);{int
_tmpCD=0;if(setjmp(_tmpCB.handler))_tmpCD=1;if(!_tmpCD){tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Std___cycFILE*,
struct Cyc_List_List*),struct Cyc_Std___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpCE="parsing";_tag_arr(_tmpCE,sizeof(char),_get_zero_arr_size(
_tmpCE,8));}),Cyc_do_parse,(struct Cyc_Std___cycFILE*)_check_null(in_file),tds);;
_pop_handler();}else{void*_tmpCC=(void*)_exn_thrown;void*_tmpD0=_tmpCC;_LL27:;
_LL28: Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(in_file));Cyc_remove_file((
struct _tagged_arr)_tmpAC);(int)_throw(_tmpD0);_LL29:;_LL2A:(void)_throw(_tmpD0);
_LL26:;}}}Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(in_file));if(
Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAC);return;}{struct
Cyc_Tcenv_Tenv*_tmpD1=Cyc_Tcenv_tc_init();if(Cyc_parseonly_r)goto PRINTC;tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpD2="type checking";_tag_arr(_tmpD2,sizeof(
char),_get_zero_arr_size(_tmpD2,14));}),Cyc_do_typecheck,_tmpD1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAC);return;}tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD3="control-flow checking";
_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,22));}),Cyc_do_cfcheck,1,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAC);return;}
if(Cyc_ic_r){struct Cyc_Std___cycFILE*inter_file=Cyc_Std_fopen((struct _tagged_arr)
_tmpAD,({const char*_tmpD8="r";_tag_arr(_tmpD8,sizeof(char),_get_zero_arr_size(
_tmpD8,2));}));struct Cyc_Std___cycFILE*inter_objfile=Cyc_try_file_open((struct
_tagged_arr)_tmpAE,({const char*_tmpD6="w";_tag_arr(_tmpD6,sizeof(char),
_get_zero_arr_size(_tmpD6,2));}),({const char*_tmpD7="interface object file";
_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,22));}));if(inter_objfile
== 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((struct
_tagged_arr)_tmpAD);tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct _tuple10*,struct Cyc_List_List*),struct _tuple10*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD4="interface checking";
_tag_arr(_tmpD4,sizeof(char),_get_zero_arr_size(_tmpD4,19));}),Cyc_do_interface,({
struct _tuple10*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->f1=_tmpD1;_tmpD5->f2=
inter_file;_tmpD5->f3=(struct Cyc_Std___cycFILE*)_check_null(inter_objfile);
_tmpD5;}),tds);if(inter_file != 0)Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_file));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_objfile));}if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD9="translation to C";
_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,17));}),Cyc_do_translate,
1,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAC);
return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();Cyc_remove_file((
struct _tagged_arr)_tmpAC);if(Cyc_compile_failure)return;if(Cyc_tovc_r)tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,
struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpDA="post-pass to VC";_tag_arr(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,
16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)return;PRINTC: {struct Cyc_Std___cycFILE*
out_file;if(Cyc_parseonly_r?1: Cyc_tc_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmpDB="w";_tag_arr(
_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,2));}),({const char*_tmpDC="output file";
_tag_arr(_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,12));}));else{out_file=(
struct Cyc_Std___cycFILE*)Cyc_Std_stdout;}}else{if(Cyc_toc_r?Cyc_output_file != 0:
0)out_file=Cyc_try_file_open(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmpDD="w";_tag_arr(_tmpDD,sizeof(char),_get_zero_arr_size(_tmpDD,2));}),({
const char*_tmpDE="output file";_tag_arr(_tmpDE,sizeof(char),_get_zero_arr_size(
_tmpDE,12));}));else{out_file=Cyc_try_file_open((struct _tagged_arr)_tmpAF,({
const char*_tmpDF="w";_tag_arr(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,2));}),({
const char*_tmpE0="output file";_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(
_tmpE0,12));}));}}if(Cyc_compile_failure)return;{struct _handler_cons _tmpE1;
_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!
_tmpE3){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(struct Cyc_Std___cycFILE*,struct Cyc_List_List*),struct Cyc_Std___cycFILE*env,
struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpE4="printing";_tag_arr(
_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,9));}),Cyc_do_print,(struct Cyc_Std___cycFILE*)
_check_null(out_file),tds);;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;
void*_tmpE6=_tmpE2;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_Std_file_close((struct
Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*
_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=({struct _tagged_arr*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=(struct _tagged_arr)_tmpAF;_tmpE8;});_tmpE7->tl=
Cyc_cfiles;_tmpE7;});(int)_throw(_tmpE6);_LL2E:;_LL2F:(void)_throw(_tmpE6);_LL2B:;}}}
Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({
struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=({struct
_tagged_arr*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=(struct _tagged_arr)
_tmpAF;_tmpEA;});_tmpE9->tl=Cyc_cfiles;_tmpE9;});}}}}}}}}}}}static char _tmp102[8]="<final>";
static struct _tagged_arr Cyc_final_str={_tmp102,_tmp102,_tmp102 + 8};static struct
_tagged_arr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _tagged_arr*n){if(n == (struct _tagged_arr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _tagged_arr basename;{struct _handler_cons _tmp103;_push_handler(& _tmp103);{
int _tmp105=0;if(setjmp(_tmp103.handler))_tmp105=1;if(!_tmp105){basename=(struct
_tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)n));;_pop_handler();}
else{void*_tmp104=(void*)_exn_thrown;void*_tmp107=_tmp104;_LL31: if(*((void**)
_tmp107)!= Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*((struct
_tagged_arr*)n);goto _LL30;_LL33:;_LL34:(void)_throw(_tmp107);_LL30:;}}}{struct
_tagged_arr _tmp108=Cyc_Std_strconcat((struct _tagged_arr)basename,({const char*
_tmp10D=".cycio";_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,7));}));
struct Cyc_Std___cycFILE*_tmp109=Cyc_try_file_open((struct _tagged_arr)_tmp108,({
const char*_tmp10B="rb";_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,
3));}),({const char*_tmp10C="interface object file";_tag_arr(_tmp10C,sizeof(char),
_get_zero_arr_size(_tmp10C,22));}));if(_tmp109 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _tagged_arr)_tmp108);{struct Cyc_Interface_I*
_tmp10A=Cyc_Interface_load((struct Cyc_Std___cycFILE*)_check_null(_tmp109));Cyc_Std_file_close((
struct Cyc_Std___cycFILE*)_check_null(_tmp109));return _tmp10A;}}}}static int Cyc_is_cfile(
struct _tagged_arr*n){return*((const char*)_check_unknown_subscript(*((struct
_tagged_arr*)n),sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct
_tuple11{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();{
struct _tagged_arr comp=(struct _tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((
struct _tagged_arr)Cyc_Std_strconcat((struct _tagged_arr)comp,({const char*_tmp10E=" -x c -E -U__GNUC__ -nostdinc";
_tag_arr(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,30));})));{struct Cyc_List_List*
options=({struct _tuple11*_tmp174[51];_tmp174[50]=({struct _tuple11*_tmp2A1=
_cycalloc(sizeof(*_tmp2A1));_tmp2A1->f1=({const char*_tmp2A6="-noregister";
_tag_arr(_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,12));});_tmp2A1->f2=0;
_tmp2A1->f3=({const char*_tmp2A5="";_tag_arr(_tmp2A5,sizeof(char),
_get_zero_arr_size(_tmp2A5,1));});_tmp2A1->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2A4;_tmp2A4.tag=3;_tmp2A4.f1=& Cyc_Parse_no_register;_tmp2A4;});_tmp2A3;});
_tmp2A1->f5=({const char*_tmp2A2="Treat register storage class as public";
_tag_arr(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,39));});_tmp2A1;});
_tmp174[49]=({struct _tuple11*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->f1=({
const char*_tmp2A0="-warnboundschecks";_tag_arr(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,18));});_tmp29B->f2=0;_tmp29B->f3=({const char*_tmp29F="";
_tag_arr(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,1));});_tmp29B->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));
_tmp29D[0]=({struct Cyc_Arg_Set_spec_struct _tmp29E;_tmp29E.tag=3;_tmp29E.f1=& Cyc_Toc_warn_bounds_checks;
_tmp29E;});_tmp29D;});_tmp29B->f5=({const char*_tmp29C="Warn when bounds checks can't be eliminated";
_tag_arr(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,44));});_tmp29B;});
_tmp174[48]=({struct _tuple11*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->f1=({
const char*_tmp29A="-CI";_tag_arr(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,
4));});_tmp295->f2=0;_tmp295->f3=({const char*_tmp299=" <file>";_tag_arr(_tmp299,
sizeof(char),_get_zero_arr_size(_tmp299,8));});_tmp295->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297[0]=({struct Cyc_Arg_String_spec_struct
_tmp298;_tmp298.tag=5;_tmp298.f1=Cyc_set_cyc_include;_tmp298;});_tmp297;});
_tmp295->f5=({const char*_tmp296="Set cyc_include.h to be <file>";_tag_arr(
_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,31));});_tmp295;});_tmp174[47]=({
struct _tuple11*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->f1=({const char*
_tmp294="-printfullevars";_tag_arr(_tmp294,sizeof(char),_get_zero_arr_size(
_tmp294,16));});_tmp28F->f2=0;_tmp28F->f3=({const char*_tmp293="";_tag_arr(
_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,1));});_tmp28F->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291[0]=({
struct Cyc_Arg_Set_spec_struct _tmp292;_tmp292.tag=3;_tmp292.f1=& Cyc_print_full_evars_r;
_tmp292;});_tmp291;});_tmp28F->f5=({const char*_tmp290="Print full information for evars (type debugging)";
_tag_arr(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,50));});_tmp28F;});
_tmp174[46]=({struct _tuple11*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289->f1=({
const char*_tmp28E="-printallkinds";_tag_arr(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,15));});_tmp289->f2=0;_tmp289->f3=({const char*_tmp28D="";
_tag_arr(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,1));});_tmp289->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));
_tmp28B[0]=({struct Cyc_Arg_Set_spec_struct _tmp28C;_tmp28C.tag=3;_tmp28C.f1=& Cyc_print_all_kinds_r;
_tmp28C;});_tmp28B;});_tmp289->f5=({const char*_tmp28A="Always print kinds of type variables";
_tag_arr(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,37));});_tmp289;});
_tmp174[45]=({struct _tuple11*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->f1=({
const char*_tmp288="-printalltvars";_tag_arr(_tmp288,sizeof(char),
_get_zero_arr_size(_tmp288,15));});_tmp283->f2=0;_tmp283->f3=({const char*_tmp287="";
_tag_arr(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,1));});_tmp283->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp285=_cycalloc(sizeof(*_tmp285));
_tmp285[0]=({struct Cyc_Arg_Set_spec_struct _tmp286;_tmp286.tag=3;_tmp286.f1=& Cyc_print_all_tvars_r;
_tmp286;});_tmp285;});_tmp283->f5=({const char*_tmp284="Print all type variables (even implicit default effects)";
_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,57));});_tmp283;});
_tmp174[44]=({struct _tuple11*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->f1=({
const char*_tmp282="-noexpandtypedefs";_tag_arr(_tmp282,sizeof(char),
_get_zero_arr_size(_tmp282,18));});_tmp27D->f2=0;_tmp27D->f3=({const char*_tmp281="";
_tag_arr(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,1));});_tmp27D->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp27F=_cycalloc(sizeof(*_tmp27F));
_tmp27F[0]=({struct Cyc_Arg_Set_spec_struct _tmp280;_tmp280.tag=3;_tmp280.f1=& Cyc_noexpand_r;
_tmp280;});_tmp27F;});_tmp27D->f5=({const char*_tmp27E="Don't expand typedefs in pretty printing";
_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,41));});_tmp27D;});
_tmp174[43]=({struct _tuple11*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->f1=({
const char*_tmp27C="-noremoveunused";_tag_arr(_tmp27C,sizeof(char),
_get_zero_arr_size(_tmp27C,16));});_tmp277->f2=0;_tmp277->f3=({const char*_tmp27B="";
_tag_arr(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,1));});_tmp277->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp279=_cycalloc(sizeof(*_tmp279));
_tmp279[0]=({struct Cyc_Arg_Set_spec_struct _tmp27A;_tmp27A.tag=3;_tmp27A.f1=& Cyc_noshake_r;
_tmp27A;});_tmp279;});_tmp277->f5=({const char*_tmp278="Don't remove externed variables that aren't used";
_tag_arr(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,49));});_tmp277;});
_tmp174[42]=({struct _tuple11*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->f1=({
const char*_tmp276="-nogc";_tag_arr(_tmp276,sizeof(char),_get_zero_arr_size(
_tmp276,6));});_tmp271->f2=0;_tmp271->f3=({const char*_tmp275="";_tag_arr(_tmp275,
sizeof(char),_get_zero_arr_size(_tmp275,1));});_tmp271->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273[0]=({struct Cyc_Arg_Set_spec_struct
_tmp274;_tmp274.tag=3;_tmp274.f1=& Cyc_nogc_r;_tmp274;});_tmp273;});_tmp271->f5=({
const char*_tmp272="Don't link in the garbage collector";_tag_arr(_tmp272,sizeof(
char),_get_zero_arr_size(_tmp272,36));});_tmp271;});_tmp174[41]=({struct _tuple11*
_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->f1=({const char*_tmp270="-nocyc";
_tag_arr(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,7));});_tmp26B->f2=0;
_tmp26B->f3=({const char*_tmp26F="";_tag_arr(_tmp26F,sizeof(char),
_get_zero_arr_size(_tmp26F,1));});_tmp26B->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26E;_tmp26E.tag=0;_tmp26E.f1=Cyc_set_nocyc;_tmp26E;});_tmp26D;});_tmp26B->f5=({
const char*_tmp26C="Don't add implicit namespace Cyc";_tag_arr(_tmp26C,sizeof(
char),_get_zero_arr_size(_tmp26C,33));});_tmp26B;});_tmp174[40]=({struct _tuple11*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->f1=({const char*_tmp26A="-no-cpp-precomp";
_tag_arr(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,16));});_tmp265->f2=0;
_tmp265->f3=({const char*_tmp269="";_tag_arr(_tmp269,sizeof(char),
_get_zero_arr_size(_tmp269,1));});_tmp265->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp268;_tmp268.tag=0;_tmp268.f1=Cyc_set_nocppprecomp;_tmp268;});_tmp267;});
_tmp265->f5=({const char*_tmp266="Don't do smart preprocessing (mac only)";
_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,40));});_tmp265;});
_tmp174[39]=({struct _tuple11*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->f1=({
const char*_tmp264="-use-cpp";_tag_arr(_tmp264,sizeof(char),_get_zero_arr_size(
_tmp264,9));});_tmp25F->f2=0;_tmp25F->f3=({const char*_tmp263="<path>";_tag_arr(
_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,7));});_tmp25F->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({
struct Cyc_Arg_String_spec_struct _tmp262;_tmp262.tag=5;_tmp262.f1=Cyc_set_cpp;
_tmp262;});_tmp261;});_tmp25F->f5=({const char*_tmp260="Indicate which preprocessor to use";
_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,35));});_tmp25F;});
_tmp174[38]=({struct _tuple11*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->f1=({
const char*_tmp25E="--noboundschecks";_tag_arr(_tmp25E,sizeof(char),
_get_zero_arr_size(_tmp25E,17));});_tmp259->f2=0;_tmp259->f3=({const char*_tmp25D="";
_tag_arr(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,1));});_tmp259->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));
_tmp25B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=Cyc_set_noboundschecks;
_tmp25C;});_tmp25B;});_tmp259->f5=({const char*_tmp25A="Disable bounds checks";
_tag_arr(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,22));});_tmp259;});
_tmp174[37]=({struct _tuple11*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->f1=({
const char*_tmp258="--nonullchecks";_tag_arr(_tmp258,sizeof(char),
_get_zero_arr_size(_tmp258,15));});_tmp253->f2=0;_tmp253->f3=({const char*_tmp257="";
_tag_arr(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,1));});_tmp253->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp255=_cycalloc(sizeof(*_tmp255));
_tmp255[0]=({struct Cyc_Arg_Unit_spec_struct _tmp256;_tmp256.tag=0;_tmp256.f1=Cyc_set_nonullchecks;
_tmp256;});_tmp255;});_tmp253->f5=({const char*_tmp254="Disable null checks";
_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,20));});_tmp253;});
_tmp174[36]=({struct _tuple11*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->f1=({
const char*_tmp252="--nochecks";_tag_arr(_tmp252,sizeof(char),_get_zero_arr_size(
_tmp252,11));});_tmp24D->f2=0;_tmp24D->f3=({const char*_tmp251="";_tag_arr(
_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,1));});_tmp24D->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp250;_tmp250.tag=0;_tmp250.f1=Cyc_set_nochecks;
_tmp250;});_tmp24F;});_tmp24D->f5=({const char*_tmp24E="Disable bounds/null checks";
_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,27));});_tmp24D;});
_tmp174[35]=({struct _tuple11*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->f1=({
const char*_tmp24C="--lineno";_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(
_tmp24C,9));});_tmp247->f2=0;_tmp247->f3=({const char*_tmp24B="";_tag_arr(_tmp24B,
sizeof(char),_get_zero_arr_size(_tmp24B,1));});_tmp247->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=Cyc_set_lineno;_tmp24A;});_tmp249;});_tmp247->f5=({
const char*_tmp248="Generate line numbers for debugging";_tag_arr(_tmp248,sizeof(
char),_get_zero_arr_size(_tmp248,36));});_tmp247;});_tmp174[34]=({struct _tuple11*
_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->f1=({const char*_tmp246="-save-c";
_tag_arr(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,8));});_tmp241->f2=0;
_tmp241->f3=({const char*_tmp245="";_tag_arr(_tmp245,sizeof(char),
_get_zero_arr_size(_tmp245,1));});_tmp241->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Arg_Set_spec_struct
_tmp244;_tmp244.tag=3;_tmp244.f1=& Cyc_save_c_r;_tmp244;});_tmp243;});_tmp241->f5=({
const char*_tmp242="Don't delete temporary C files";_tag_arr(_tmp242,sizeof(char),
_get_zero_arr_size(_tmp242,31));});_tmp241;});_tmp174[33]=({struct _tuple11*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->f1=({const char*_tmp240="-save-temps";
_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,12));});_tmp23B->f2=0;
_tmp23B->f3=({const char*_tmp23F="";_tag_arr(_tmp23F,sizeof(char),
_get_zero_arr_size(_tmp23F,1));});_tmp23B->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp23E;_tmp23E.tag=0;_tmp23E.f1=Cyc_set_save_temps;_tmp23E;});_tmp23D;});
_tmp23B->f5=({const char*_tmp23C="Don't delete temporary files";_tag_arr(_tmp23C,
sizeof(char),_get_zero_arr_size(_tmp23C,29));});_tmp23B;});_tmp174[32]=({struct
_tuple11*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235->f1=({const char*_tmp23A="-tovc";
_tag_arr(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,6));});_tmp235->f2=0;
_tmp235->f3=({const char*_tmp239="";_tag_arr(_tmp239,sizeof(char),
_get_zero_arr_size(_tmp239,1));});_tmp235->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Arg_Set_spec_struct
_tmp238;_tmp238.tag=3;_tmp238.f1=& Cyc_tovc_r;_tmp238;});_tmp237;});_tmp235->f5=({
const char*_tmp236="Avoid gcc extensions in C output";_tag_arr(_tmp236,sizeof(
char),_get_zero_arr_size(_tmp236,33));});_tmp235;});_tmp174[31]=({struct _tuple11*
_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->f1=({const char*_tmp234="-up";
_tag_arr(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,4));});_tmp22F->f2=0;
_tmp22F->f3=({const char*_tmp233="";_tag_arr(_tmp233,sizeof(char),
_get_zero_arr_size(_tmp233,1));});_tmp22F->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp232;_tmp232.tag=4;_tmp232.f1=& Cyc_pp_r;_tmp232;});_tmp231;});_tmp22F->f5=({
const char*_tmp230="Ugly print";_tag_arr(_tmp230,sizeof(char),_get_zero_arr_size(
_tmp230,11));});_tmp22F;});_tmp174[30]=({struct _tuple11*_tmp229=_cycalloc(
sizeof(*_tmp229));_tmp229->f1=({const char*_tmp22E="-pp";_tag_arr(_tmp22E,sizeof(
char),_get_zero_arr_size(_tmp22E,4));});_tmp229->f2=0;_tmp229->f3=({const char*
_tmp22D="";_tag_arr(_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,1));});
_tmp229->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22B=_cycalloc(sizeof(*
_tmp22B));_tmp22B[0]=({struct Cyc_Arg_Set_spec_struct _tmp22C;_tmp22C.tag=3;
_tmp22C.f1=& Cyc_pp_r;_tmp22C;});_tmp22B;});_tmp229->f5=({const char*_tmp22A="Pretty print";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,13));});_tmp229;});
_tmp174[29]=({struct _tuple11*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->f1=({
const char*_tmp228="-ic";_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,
4));});_tmp223->f2=0;_tmp223->f3=({const char*_tmp227="";_tag_arr(_tmp227,sizeof(
char),_get_zero_arr_size(_tmp227,1));});_tmp223->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Arg_Set_spec_struct
_tmp226;_tmp226.tag=3;_tmp226.f1=& Cyc_ic_r;_tmp226;});_tmp225;});_tmp223->f5=({
const char*_tmp224="Activate the link-checker";_tag_arr(_tmp224,sizeof(char),
_get_zero_arr_size(_tmp224,26));});_tmp223;});_tmp174[28]=({struct _tuple11*
_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->f1=({const char*_tmp222="-stopafter-toc";
_tag_arr(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,15));});_tmp21D->f2=0;
_tmp21D->f3=({const char*_tmp221="";_tag_arr(_tmp221,sizeof(char),
_get_zero_arr_size(_tmp221,1));});_tmp21D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F[0]=({struct Cyc_Arg_Set_spec_struct
_tmp220;_tmp220.tag=3;_tmp220.f1=& Cyc_toc_r;_tmp220;});_tmp21F;});_tmp21D->f5=({
const char*_tmp21E="Stop after translation to C";_tag_arr(_tmp21E,sizeof(char),
_get_zero_arr_size(_tmp21E,28));});_tmp21D;});_tmp174[27]=({struct _tuple11*
_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->f1=({const char*_tmp21C="-stopafter-tc";
_tag_arr(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,14));});_tmp217->f2=0;
_tmp217->f3=({const char*_tmp21B="";_tag_arr(_tmp21B,sizeof(char),
_get_zero_arr_size(_tmp21B,1));});_tmp217->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Arg_Set_spec_struct
_tmp21A;_tmp21A.tag=3;_tmp21A.f1=& Cyc_tc_r;_tmp21A;});_tmp219;});_tmp217->f5=({
const char*_tmp218="Stop after type checking";_tag_arr(_tmp218,sizeof(char),
_get_zero_arr_size(_tmp218,25));});_tmp217;});_tmp174[26]=({struct _tuple11*
_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=({const char*_tmp216="-stopafter-parse";
_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,17));});_tmp211->f2=0;
_tmp211->f3=({const char*_tmp215="";_tag_arr(_tmp215,sizeof(char),
_get_zero_arr_size(_tmp215,1));});_tmp211->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Arg_Set_spec_struct
_tmp214;_tmp214.tag=3;_tmp214.f1=& Cyc_parseonly_r;_tmp214;});_tmp213;});_tmp211->f5=({
const char*_tmp212="Stop after parsing";_tag_arr(_tmp212,sizeof(char),
_get_zero_arr_size(_tmp212,19));});_tmp211;});_tmp174[25]=({struct _tuple11*
_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->f1=({const char*_tmp210="-E";
_tag_arr(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,3));});_tmp20B->f2=0;
_tmp20B->f3=({const char*_tmp20F="";_tag_arr(_tmp20F,sizeof(char),
_get_zero_arr_size(_tmp20F,1));});_tmp20B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp20E;_tmp20E.tag=3;_tmp20E.f1=& Cyc_stop_after_cpp_r;_tmp20E;});_tmp20D;});
_tmp20B->f5=({const char*_tmp20C="Stop after preprocessing";_tag_arr(_tmp20C,
sizeof(char),_get_zero_arr_size(_tmp20C,25));});_tmp20B;});_tmp174[24]=({struct
_tuple11*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->f1=({const char*_tmp20A="-b";
_tag_arr(_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,3));});_tmp205->f2=0;
_tmp205->f3=({const char*_tmp209="<arch>";_tag_arr(_tmp209,sizeof(char),
_get_zero_arr_size(_tmp209,7));});_tmp205->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Arg_String_spec_struct
_tmp208;_tmp208.tag=5;_tmp208.f1=Cyc_set_target_arch;_tmp208;});_tmp207;});
_tmp205->f5=({const char*_tmp206="Set target architecture; implies -stopafter-toc";
_tag_arr(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,48));});_tmp205;});
_tmp174[23]=({struct _tuple11*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->f1=({
const char*_tmp204="-MT";_tag_arr(_tmp204,sizeof(char),_get_zero_arr_size(_tmp204,
4));});_tmp1FF->f2=0;_tmp1FF->f3=({const char*_tmp203=" <target>";_tag_arr(
_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,10));});_tmp1FF->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201[0]=({
struct Cyc_Arg_String_spec_struct _tmp202;_tmp202.tag=5;_tmp202.f1=Cyc_set_dependencies_target;
_tmp202;});_tmp201;});_tmp1FF->f5=({const char*_tmp200="Give target for dependencies";
_tag_arr(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,29));});_tmp1FF;});
_tmp174[22]=({struct _tuple11*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->f1=({
const char*_tmp1FE="-MG";_tag_arr(_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,
4));});_tmp1F9->f2=0;_tmp1F9->f3=({const char*_tmp1FD="";_tag_arr(_tmp1FD,sizeof(
char),_get_zero_arr_size(_tmp1FD,1));});_tmp1F9->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1FC;_tmp1FC.tag=1;_tmp1FC.f1=Cyc_add_cpparg;_tmp1FC;});_tmp1FB;});_tmp1F9->f5=({
const char*_tmp1FA="When producing dependencies assume that missing files are generated";
_tag_arr(_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,68));});_tmp1F9;});
_tmp174[21]=({struct _tuple11*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->f1=({
const char*_tmp1F8="-M";_tag_arr(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,
3));});_tmp1F3->f2=0;_tmp1F3->f3=({const char*_tmp1F7="";_tag_arr(_tmp1F7,sizeof(
char),_get_zero_arr_size(_tmp1F7,1));});_tmp1F3->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=Cyc_set_produce_dependencies;_tmp1F6;});_tmp1F5;});
_tmp1F3->f5=({const char*_tmp1F4="Produce dependencies";_tag_arr(_tmp1F4,sizeof(
char),_get_zero_arr_size(_tmp1F4,21));});_tmp1F3;});_tmp174[20]=({struct _tuple11*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->f1=({const char*_tmp1F2="-S";
_tag_arr(_tmp1F2,sizeof(char),_get_zero_arr_size(_tmp1F2,3));});_tmp1ED->f2=0;
_tmp1ED->f3=({const char*_tmp1F1="";_tag_arr(_tmp1F1,sizeof(char),
_get_zero_arr_size(_tmp1F1,1));});_tmp1ED->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=Cyc_set_stop_after_asmfile;_tmp1F0;});_tmp1EF;});
_tmp1ED->f5=({const char*_tmp1EE="Stop after producing assembly code";_tag_arr(
_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,35));});_tmp1ED;});_tmp174[19]=({
struct _tuple11*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->f1=({const char*
_tmp1EC="-pa";_tag_arr(_tmp1EC,sizeof(char),_get_zero_arr_size(_tmp1EC,4));});
_tmp1E7->f2=0;_tmp1E7->f3=({const char*_tmp1EB="";_tag_arr(_tmp1EB,sizeof(char),
_get_zero_arr_size(_tmp1EB,1));});_tmp1E7->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=Cyc_set_pa;_tmp1EA;});_tmp1E9;});_tmp1E7->f5=({
const char*_tmp1E8="Compile for profiling with aprof";_tag_arr(_tmp1E8,sizeof(
char),_get_zero_arr_size(_tmp1E8,33));});_tmp1E7;});_tmp174[18]=({struct _tuple11*
_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->f1=({const char*_tmp1E6="-pg";
_tag_arr(_tmp1E6,sizeof(char),_get_zero_arr_size(_tmp1E6,4));});_tmp1E1->f2=0;
_tmp1E1->f3=({const char*_tmp1E5="";_tag_arr(_tmp1E5,sizeof(char),
_get_zero_arr_size(_tmp1E5,1));});_tmp1E1->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E4;_tmp1E4.tag=0;_tmp1E4.f1=Cyc_set_pg;_tmp1E4;});_tmp1E3;});_tmp1E1->f5=({
const char*_tmp1E2="Compile for profiling with gprof";_tag_arr(_tmp1E2,sizeof(
char),_get_zero_arr_size(_tmp1E2,33));});_tmp1E1;});_tmp174[17]=({struct _tuple11*
_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->f1=({const char*_tmp1E0="-p";
_tag_arr(_tmp1E0,sizeof(char),_get_zero_arr_size(_tmp1E0,3));});_tmp1DB->f2=0;
_tmp1DB->f3=({const char*_tmp1DF="";_tag_arr(_tmp1DF,sizeof(char),
_get_zero_arr_size(_tmp1DF,1));});_tmp1DB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1DE;_tmp1DE.tag=1;_tmp1DE.f1=Cyc_add_ccarg;_tmp1DE;});_tmp1DD;});_tmp1DB->f5=({
const char*_tmp1DC="Compile for profiling with prof";_tag_arr(_tmp1DC,sizeof(char),
_get_zero_arr_size(_tmp1DC,32));});_tmp1DB;});_tmp174[16]=({struct _tuple11*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=({const char*_tmp1DA="-g";
_tag_arr(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,3));});_tmp1D5->f2=0;
_tmp1D5->f3=({const char*_tmp1D9="";_tag_arr(_tmp1D9,sizeof(char),
_get_zero_arr_size(_tmp1D9,1));});_tmp1D5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D8;_tmp1D8.tag=1;_tmp1D8.f1=Cyc_add_ccarg;_tmp1D8;});_tmp1D7;});_tmp1D5->f5=({
const char*_tmp1D6="Compile for debugging";_tag_arr(_tmp1D6,sizeof(char),
_get_zero_arr_size(_tmp1D6,22));});_tmp1D5;});_tmp174[15]=({struct _tuple11*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->f1=({const char*_tmp1D4="-fomit-frame-pointer";
_tag_arr(_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,21));});_tmp1CF->f2=0;
_tmp1CF->f3=({const char*_tmp1D3="";_tag_arr(_tmp1D3,sizeof(char),
_get_zero_arr_size(_tmp1D3,1));});_tmp1CF->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=Cyc_add_ccarg;_tmp1D2;});_tmp1D1;});_tmp1CF->f5=({
const char*_tmp1D0="Omit frame pointer";_tag_arr(_tmp1D0,sizeof(char),
_get_zero_arr_size(_tmp1D0,19));});_tmp1CF;});_tmp174[14]=({struct _tuple11*
_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->f1=({const char*_tmp1CE="-O3";
_tag_arr(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,4));});_tmp1C9->f2=0;
_tmp1C9->f3=({const char*_tmp1CD="";_tag_arr(_tmp1CD,sizeof(char),
_get_zero_arr_size(_tmp1CD,1));});_tmp1C9->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=Cyc_add_ccarg;_tmp1CC;});_tmp1CB;});_tmp1C9->f5=({
const char*_tmp1CA="Even more optimization";_tag_arr(_tmp1CA,sizeof(char),
_get_zero_arr_size(_tmp1CA,23));});_tmp1C9;});_tmp174[13]=({struct _tuple11*
_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->f1=({const char*_tmp1C8="-O2";
_tag_arr(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,4));});_tmp1C3->f2=0;
_tmp1C3->f3=({const char*_tmp1C7="";_tag_arr(_tmp1C7,sizeof(char),
_get_zero_arr_size(_tmp1C7,1));});_tmp1C3->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C6;_tmp1C6.tag=1;_tmp1C6.f1=Cyc_add_ccarg;_tmp1C6;});_tmp1C5;});_tmp1C3->f5=({
const char*_tmp1C4="A higher level of optimization";_tag_arr(_tmp1C4,sizeof(char),
_get_zero_arr_size(_tmp1C4,31));});_tmp1C3;});_tmp174[12]=({struct _tuple11*
_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->f1=({const char*_tmp1C2="-O";
_tag_arr(_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,3));});_tmp1BD->f2=0;
_tmp1BD->f3=({const char*_tmp1C1="";_tag_arr(_tmp1C1,sizeof(char),
_get_zero_arr_size(_tmp1C1,1));});_tmp1BD->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C0;_tmp1C0.tag=1;_tmp1C0.f1=Cyc_add_ccarg;_tmp1C0;});_tmp1BF;});_tmp1BD->f5=({
const char*_tmp1BE="Optimize";_tag_arr(_tmp1BE,sizeof(char),_get_zero_arr_size(
_tmp1BE,9));});_tmp1BD;});_tmp174[11]=({struct _tuple11*_tmp1B7=_cycalloc(sizeof(*
_tmp1B7));_tmp1B7->f1=({const char*_tmp1BC="-O0";_tag_arr(_tmp1BC,sizeof(char),
_get_zero_arr_size(_tmp1BC,4));});_tmp1B7->f2=0;_tmp1B7->f3=({const char*_tmp1BB="";
_tag_arr(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,1));});_tmp1B7->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));
_tmp1B9[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BA;_tmp1BA.tag=1;_tmp1BA.f1=Cyc_add_ccarg;
_tmp1BA;});_tmp1B9;});_tmp1B7->f5=({const char*_tmp1B8="Don't optimize";_tag_arr(
_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,15));});_tmp1B7;});_tmp174[10]=({
struct _tuple11*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->f1=({const char*
_tmp1B6="-s";_tag_arr(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,3));});
_tmp1B1->f2=0;_tmp1B1->f3=({const char*_tmp1B5="";_tag_arr(_tmp1B5,sizeof(char),
_get_zero_arr_size(_tmp1B5,1));});_tmp1B1->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1B4;_tmp1B4.tag=1;_tmp1B4.f1=Cyc_add_ccarg;_tmp1B4;});_tmp1B3;});_tmp1B1->f5=({
const char*_tmp1B2="Remove all symbol table and relocation info from executable";
_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,60));});_tmp1B1;});
_tmp174[9]=({struct _tuple11*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->f1=({
const char*_tmp1B0="-x";_tag_arr(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,
3));});_tmp1AB->f2=0;_tmp1AB->f3=({const char*_tmp1AF=" <language>";_tag_arr(
_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,12));});_tmp1AB->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({
struct Cyc_Arg_String_spec_struct _tmp1AE;_tmp1AE.tag=5;_tmp1AE.f1=Cyc_set_inputtype;
_tmp1AE;});_tmp1AD;});_tmp1AB->f5=({const char*_tmp1AC="Specify <language> for the following input files";
_tag_arr(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,49));});_tmp1AB;});
_tmp174[8]=({struct _tuple11*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=({
const char*_tmp1AA="-c";_tag_arr(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,
3));});_tmp1A5->f2=0;_tmp1A5->f3=({const char*_tmp1A9="";_tag_arr(_tmp1A9,sizeof(
char),_get_zero_arr_size(_tmp1A9,1));});_tmp1A5->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=Cyc_set_stop_after_objectfile;_tmp1A8;});
_tmp1A7;});_tmp1A5->f5=({const char*_tmp1A6="Produce an object file instead of an executable; do not link";
_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,61));});_tmp1A5;});
_tmp174[7]=({struct _tuple11*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->f1=({
const char*_tmp1A4="-l";_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,
3));});_tmp19F->f2=1;_tmp19F->f3=({const char*_tmp1A3="<file>";_tag_arr(_tmp1A3,
sizeof(char),_get_zero_arr_size(_tmp1A3,7));});_tmp19F->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A2;_tmp1A2.tag=1;_tmp1A2.f1=Cyc_add_libarg;_tmp1A2;});_tmp1A1;});_tmp19F->f5=({
const char*_tmp1A0="Library file";_tag_arr(_tmp1A0,sizeof(char),
_get_zero_arr_size(_tmp1A0,13));});_tmp19F;});_tmp174[6]=({struct _tuple11*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->f1=({const char*_tmp19E="-L";
_tag_arr(_tmp19E,sizeof(char),_get_zero_arr_size(_tmp19E,3));});_tmp199->f2=1;
_tmp199->f3=({const char*_tmp19D="<dir>";_tag_arr(_tmp19D,sizeof(char),
_get_zero_arr_size(_tmp19D,6));});_tmp199->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp19C;_tmp19C.tag=1;_tmp19C.f1=Cyc_add_ccarg;_tmp19C;});_tmp19B;});_tmp199->f5=({
const char*_tmp19A="Add to the list of directories for -l";_tag_arr(_tmp19A,
sizeof(char),_get_zero_arr_size(_tmp19A,38));});_tmp199;});_tmp174[5]=({struct
_tuple11*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=({const char*_tmp198="-I";
_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,3));});_tmp193->f2=1;
_tmp193->f3=({const char*_tmp197="<dir>";_tag_arr(_tmp197,sizeof(char),
_get_zero_arr_size(_tmp197,6));});_tmp193->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp196;_tmp196.tag=1;_tmp196.f1=Cyc_add_cpparg;_tmp196;});_tmp195;});_tmp193->f5=({
const char*_tmp194="Add to the list of directories to search for include files";
_tag_arr(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,59));});_tmp193;});
_tmp174[4]=({struct _tuple11*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=({
const char*_tmp192="-B";_tag_arr(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,
3));});_tmp18D->f2=1;_tmp18D->f3=({const char*_tmp191="<file>";_tag_arr(_tmp191,
sizeof(char),_get_zero_arr_size(_tmp191,7));});_tmp18D->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp190;_tmp190.tag=1;_tmp190.f1=Cyc_add_cyclone_exec_path;_tmp190;});_tmp18F;});
_tmp18D->f5=({const char*_tmp18E="Add to the list of directories to search for compiler files";
_tag_arr(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,60));});_tmp18D;});
_tmp174[3]=({struct _tuple11*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->f1=({
const char*_tmp18C="-D";_tag_arr(_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,
3));});_tmp187->f2=1;_tmp187->f3=({const char*_tmp18B="<name>[=<value>]";_tag_arr(
_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,17));});_tmp187->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp18A;_tmp18A.tag=1;_tmp18A.f1=Cyc_add_cpparg;
_tmp18A;});_tmp189;});_tmp187->f5=({const char*_tmp188="Pass definition to preprocessor";
_tag_arr(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,32));});_tmp187;});
_tmp174[2]=({struct _tuple11*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->f1=({
const char*_tmp186="-o";_tag_arr(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,
3));});_tmp181->f2=0;_tmp181->f3=({const char*_tmp185=" <file>";_tag_arr(_tmp185,
sizeof(char),_get_zero_arr_size(_tmp185,8));});_tmp181->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Arg_String_spec_struct
_tmp184;_tmp184.tag=5;_tmp184.f1=Cyc_set_output_file;_tmp184;});_tmp183;});
_tmp181->f5=({const char*_tmp182="Set the output file name to <file>";_tag_arr(
_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,35));});_tmp181;});_tmp174[1]=({
struct _tuple11*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=({const char*
_tmp180="--version";_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,10));});
_tmp17B->f2=0;_tmp17B->f3=({const char*_tmp17F="";_tag_arr(_tmp17F,sizeof(char),
_get_zero_arr_size(_tmp17F,1));});_tmp17B->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=Cyc_print_version;_tmp17E;});_tmp17D;});_tmp17B->f5=({
const char*_tmp17C="Print version information and exit";_tag_arr(_tmp17C,sizeof(
char),_get_zero_arr_size(_tmp17C,35));});_tmp17B;});_tmp174[0]=({struct _tuple11*
_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->f1=({const char*_tmp17A="-v";
_tag_arr(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,3));});_tmp175->f2=0;
_tmp175->f3=({const char*_tmp179="";_tag_arr(_tmp179,sizeof(char),
_get_zero_arr_size(_tmp179,1));});_tmp175->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177[0]=({struct Cyc_Arg_Set_spec_struct
_tmp178;_tmp178.tag=3;_tmp178.f1=& Cyc_v_r;_tmp178;});_tmp177;});_tmp175->f5=({
const char*_tmp176="Print compilation stages verbosely";_tag_arr(_tmp176,sizeof(
char),_get_zero_arr_size(_tmp176,35));});_tmp175;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp174,sizeof(struct _tuple11*),51));});
Cyc_Arg_parse(options,Cyc_add_other,({const char*_tmp10F="Options:";_tag_arr(
_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,9));}),argv);if(Cyc_cyclone_files
== 0?Cyc_ccargs == 0: 0){({void*_tmp110[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp111="missing file\n";_tag_arr(_tmp111,sizeof(char),
_get_zero_arr_size(_tmp111,14));}),_tag_arr(_tmp110,sizeof(void*),0));});exit(1);}
if(Cyc_target_arch.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_target_arch=(
struct _tagged_arr)Cstring_to_string(Carch);{struct _tagged_arr cyclone_exec_prefix=(
struct _tagged_arr)Cyc_Std_getenv(({const char*_tmp173="CYCLONE_EXEC_PREFIX";
_tag_arr(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,20));}));if(
cyclone_exec_prefix.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->hd=({struct
_tagged_arr*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=cyclone_exec_prefix;
_tmp113;});_tmp112->tl=Cyc_cyclone_exec_path;_tmp112;});{struct _tagged_arr
def_lib_path=(struct _tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_Std_strlen((
struct _tagged_arr)def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=({struct _tagged_arr*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=(struct _tagged_arr)Cyc_Filename_concat(
def_lib_path,({const char*_tmp116="cyc-lib";_tag_arr(_tmp116,sizeof(char),
_get_zero_arr_size(_tmp116,8));}));_tmp115;});_tmp114->tl=Cyc_cyclone_exec_path;
_tmp114;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{struct Cyc_List_List*_tmp117=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);if(!Cyc_stop_after_cpp_r?Cyc_Std_strlen((
struct _tagged_arr)Cyc_cyc_include)== 0: 0)Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({
const char*_tmp118="cyc_include.h";_tag_arr(_tmp118,sizeof(char),
_get_zero_arr_size(_tmp118,14));}));{struct Cyc_List_List*_tmp119=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp119 != 0;
_tmp119=_tmp119->tl){Cyc_process_file(*((struct _tagged_arr*)((struct _tagged_arr*)
_tmp119->hd)));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r?1: Cyc_parseonly_r)?
1: Cyc_tc_r)?1: Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((struct
_tagged_arr)Cyc_Std_strconcat(({const char*_tmp11A="-L";_tag_arr(_tmp11A,sizeof(
char),_get_zero_arr_size(_tmp11A,3));}),(struct _tagged_arr)def_lib_path));Cyc_ccargs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct
_tagged_arr _tmp11B=Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_ccargs),({const char*_tmp172=" ";_tag_arr(_tmp172,sizeof(char),
_get_zero_arr_size(_tmp172,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmp11C=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp16F->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp16F;}),({const char*
_tmp171=" ";_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,2));}));
struct Cyc_List_List*stdlib;struct _tagged_arr stdlib_string;int _tmp11D=((Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmp16D=".a";
_tag_arr(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,3));})): 0))?1:(Cyc_output_file
!= 0?Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmp16E=".lib";_tag_arr(_tmp16E,sizeof(char),_get_zero_arr_size(
_tmp16E,5));})): 0);if(_tmp11D){stdlib=0;stdlib_string=({const char*_tmp11E="";
_tag_arr(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,1));});}else{struct
_tagged_arr libcyc_flag;struct _tagged_arr nogc_filename;struct _tagged_arr
runtime_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){libcyc_flag=({const
char*_tmp11F="-lcyc_a";_tag_arr(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,
8));});nogc_filename=({const char*_tmp120="nogc_a.a";_tag_arr(_tmp120,sizeof(char),
_get_zero_arr_size(_tmp120,9));});runtime_filename=({const char*_tmp121="runtime_cyc_a.o";
_tag_arr(_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp122="-lcyc_nocheck";_tag_arr(_tmp122,sizeof(char),
_get_zero_arr_size(_tmp122,14));});nogc_filename=({const char*_tmp123="nogc_nocheck.a";
_tag_arr(_tmp123,sizeof(char),_get_zero_arr_size(_tmp123,15));});
runtime_filename=({const char*_tmp124="runtime_cyc.o";_tag_arr(_tmp124,sizeof(
char),_get_zero_arr_size(_tmp124,14));});}else{if(Cyc_pg_r){libcyc_flag=({const
char*_tmp125="-lcyc_pg";_tag_arr(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,
9));});runtime_filename=({const char*_tmp126="runtime_cyc_pg.a";_tag_arr(_tmp126,
sizeof(char),_get_zero_arr_size(_tmp126,17));});nogc_filename=({const char*
_tmp127="nogc_pg.a";_tag_arr(_tmp127,sizeof(char),_get_zero_arr_size(_tmp127,10));});}
else{libcyc_flag=({const char*_tmp128="-lcyc";_tag_arr(_tmp128,sizeof(char),
_get_zero_arr_size(_tmp128,6));});nogc_filename=({const char*_tmp129="nogc.a";
_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,7));});runtime_filename=({
const char*_tmp12A="runtime_cyc.o";_tag_arr(_tmp12A,sizeof(char),
_get_zero_arr_size(_tmp12A,14));});}}}gc_filename=({const char*_tmp12B="gc.a";
_tag_arr(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,5));});{struct
_tagged_arr _tmp12C=Cyc_nogc_r?Cyc_do_find(_tmp117,nogc_filename): Cyc_do_find(
_tmp117,gc_filename);struct _tagged_arr _tmp12D=Cyc_do_find(_tmp117,
runtime_filename);stdlib=0;stdlib_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)_tmp12C;{struct Cyc_Std_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)libcyc_flag;{struct Cyc_Std_String_pa_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=(struct _tagged_arr)_tmp12D;{void*_tmp12E[3]={&
_tmp130,& _tmp131,& _tmp132};Cyc_Std_aprintf(({const char*_tmp12F=" %s %s %s";
_tag_arr(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,10));}),_tag_arr(
_tmp12E,sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp133;
_push_handler(& _tmp133);{int _tmp135=0;if(setjmp(_tmp133.handler))_tmp135=1;if(!
_tmp135){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct
Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp11D)_tmp136=({struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137->hd=Cyc_final_strptr;_tmp137->tl=_tmp136;_tmp137;});{struct Cyc_Interface_I*
_tmp138=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_tagged_arr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp136,_tmp136);if(_tmp138 == 0){({void*_tmp139[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp13A="Error: interfaces incompatible\n";_tag_arr(
_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,32));}),_tag_arr(_tmp139,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13B=1;
_npop_handler(0);return _tmp13B;}}if(_tmp11D){if(Cyc_output_file != 0){struct
_tagged_arr _tmp13C=({struct Cyc_Std_String_pa_struct _tmp143;_tmp143.tag=0;_tmp143.f1=(
struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)_check_null(
Cyc_output_file)));{void*_tmp141[1]={& _tmp143};Cyc_Std_aprintf(({const char*
_tmp142="%s.cycio";_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,9));}),
_tag_arr(_tmp141,sizeof(void*),1));}});struct Cyc_Std___cycFILE*_tmp13D=Cyc_try_file_open((
struct _tagged_arr)_tmp13C,({const char*_tmp13F="wb";_tag_arr(_tmp13F,sizeof(char),
_get_zero_arr_size(_tmp13F,3));}),({const char*_tmp140="interface object file";
_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,22));}));if(_tmp13D == 0){
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13E=1;_npop_handler(0);return
_tmp13E;}}Cyc_Interface_save((struct Cyc_Interface_I*)_check_null(_tmp138),(
struct Cyc_Std___cycFILE*)_check_null(_tmp13D));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(_tmp13D));}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(
struct Cyc_Interface_I*)_check_null(_tmp138),({struct _tuple9*_tmp144=_cycalloc(
sizeof(*_tmp144));_tmp144->f1=({const char*_tmp146="empty interface";_tag_arr(
_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,16));});_tmp144->f2=({const char*
_tmp145="global interface";_tag_arr(_tmp145,sizeof(char),_get_zero_arr_size(
_tmp145,17));});_tmp144;}))){({void*_tmp147[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp148="Error: some objects are still undefined\n";_tag_arr(_tmp148,
sizeof(char),_get_zero_arr_size(_tmp148,41));}),_tag_arr(_tmp147,sizeof(void*),0));});
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp149=1;_npop_handler(0);return
_tmp149;}}}}};_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp14B=
_tmp134;struct _tagged_arr _tmp14C;struct _tagged_arr _tmp14D;struct _tagged_arr
_tmp14E;_LL36: if(*((void**)_tmp14B)!= Cyc_Core_Failure)goto _LL38;_tmp14C=((
struct Cyc_Core_Failure_struct*)_tmp14B)->f1;_LL37:({struct Cyc_Std_String_pa_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)_tmp14C;{void*_tmp14F[1]={&
_tmp151};Cyc_Std_printf(({const char*_tmp150="Exception Core::Failure %s\n";
_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,28));}),_tag_arr(
_tmp14F,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL38: if(*((void**)_tmp14B)!= Cyc_Core_Impossible)goto _LL3A;_tmp14D=((struct Cyc_Core_Impossible_struct*)
_tmp14B)->f1;_LL39:({struct Cyc_Std_String_pa_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(
struct _tagged_arr)_tmp14D;{void*_tmp152[1]={& _tmp154};Cyc_Std_printf(({const char*
_tmp153="Exception Core::Impossible %s\n";_tag_arr(_tmp153,sizeof(char),
_get_zero_arr_size(_tmp153,31));}),_tag_arr(_tmp152,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3A: if(_tmp14B != Cyc_Dict_Absent)goto _LL3C;_LL3B:({
void*_tmp155[0]={};Cyc_Std_printf(({const char*_tmp156="Exception Dict::Absent\n";
_tag_arr(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,24));}),_tag_arr(
_tmp155,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3C: if(*((void**)_tmp14B)!= Cyc_Core_Invalid_argument)goto _LL3E;_tmp14E=((
struct Cyc_Core_Invalid_argument_struct*)_tmp14B)->f1;_LL3D:({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)_tmp14E;{void*_tmp157[1]={&
_tmp159};Cyc_Std_printf(({const char*_tmp158="Exception Core::Invalid_argument %s\n";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,37));}),_tag_arr(
_tmp157,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3E:;_LL3F:({void*_tmp15A[0]={};Cyc_Std_printf(({const char*_tmp15B="Uncaught exception\n";
_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,20));}),_tag_arr(
_tmp15A,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL40:;_LL41:(void)_throw(_tmp14B);_LL35:;}}}{struct _tagged_arr _tmp15C=({struct
Cyc_Std_String_pa_struct _tmp168;_tmp168.tag=0;_tmp168.f1=(struct _tagged_arr)
_tmp11C;{struct Cyc_Std_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(struct
_tagged_arr)stdlib_string;{struct Cyc_Std_String_pa_struct _tmp166;_tmp166.tag=0;
_tmp166.f1=(struct _tagged_arr)_tmp11B;{struct Cyc_Std_String_pa_struct _tmp165;
_tmp165.tag=0;_tmp165.f1=(struct _tagged_arr)(Cyc_output_file == 0?({const char*
_tmp169="";_tag_arr(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,1));}):(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(
struct _tagged_arr)Cyc_sh_escape_string(*((struct _tagged_arr*)_check_null(Cyc_output_file)));{
void*_tmp16A[1]={& _tmp16C};Cyc_Std_aprintf(({const char*_tmp16B=" -o %s";_tag_arr(
_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,7));}),_tag_arr(_tmp16A,sizeof(
void*),1));}}));{struct Cyc_Std_String_pa_struct _tmp164;_tmp164.tag=0;_tmp164.f1=(
struct _tagged_arr)comp;{void*_tmp162[5]={& _tmp164,& _tmp165,& _tmp166,& _tmp167,&
_tmp168};Cyc_Std_aprintf(({const char*_tmp163="%s %s %s%s%s";_tag_arr(_tmp163,
sizeof(char),_get_zero_arr_size(_tmp163,13));}),_tag_arr(_tmp162,sizeof(void*),5));}}}}}});
if(Cyc_v_r)({struct Cyc_Std_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(
struct _tagged_arr)_tmp15C;{void*_tmp15D[1]={& _tmp15F};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp15E="%s\n";_tag_arr(_tmp15E,sizeof(char),_get_zero_arr_size(
_tmp15E,4));}),_tag_arr(_tmp15D,sizeof(void*),1));}});if(Cyc_Std_system((struct
_tagged_arr)_tmp15C)!= 0){({void*_tmp160[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp161="Error: C compiler failed\n";_tag_arr(_tmp161,sizeof(char),
_get_zero_arr_size(_tmp161,26));}),_tag_arr(_tmp160,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();return Cyc_compile_failure?1: 0;}}}}}}}}}
