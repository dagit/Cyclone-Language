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
struct _tagged_arr errmsg,struct _tagged_arr args);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int
__pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern
struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_feof(struct
Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*
Cyc_Std_fopen(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);unsigned int Cyc_Std_fread(struct _tagged_arr,
unsigned int,unsigned int,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};unsigned int Cyc_Std_fwrite(struct _tagged_arr,unsigned int,
unsigned int,struct Cyc_Std___cycFILE*);int Cyc_Std_printf(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_remove(struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE*Cyc_Std_file_open(struct
_tagged_arr,struct _tagged_arr);void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
typedef struct{int quot;int rem;}Cyc_Std_div_t;typedef struct{int quot;int rem;}Cyc_Std_ldiv_t;
struct _tagged_arr Cyc_Std_getenv(struct _tagged_arr);int Cyc_Std_system(struct
_tagged_arr);unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_strconcat_l(struct
Cyc_List_List*);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,
unsigned int n);struct _tagged_arr Cyc_Std_strchr(struct _tagged_arr s,char c);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
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
static int Cyc_is_cyclone_sourcefile(struct _tagged_arr s){unsigned int _tmp11=Cyc_Std_strlen(
s);if(_tmp11 <= 4)return 0;else{return Cyc_Std_strcmp(_tagged_arr_plus(s,sizeof(
char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";_tag_arr(_tmp12,sizeof(char),
_get_zero_arr_size(_tmp12,5));}))== 0;}}static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _tagged_arr s){Cyc_ccargs=({struct Cyc_List_List*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=({struct _tagged_arr*_tmp14=
_cycalloc(sizeof(*_tmp14));_tmp14[0]=s;_tmp14;});_tmp13->tl=Cyc_ccargs;_tmp13;});}
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(
struct _tagged_arr s){unsigned int _tmp15=Cyc_Std_strlen(s);if(_tmp15 <= 2)return;{
struct _tagged_arr _tmp16=(struct _tagged_arr)Cyc_Std_substring(s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct
_tagged_arr*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=
Cyc_cyclone_exec_path;_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static
void Cyc_add_libarg(struct _tagged_arr s){if(Cyc_Std_strcmp(s,({const char*_tmp19="-lxml";
_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,6));}))== 0)Cyc_add_ccarg(
s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1A=_cycalloc(sizeof(*_tmp1A));
_tmp1A->hd=({struct _tagged_arr*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=s;
_tmp1B;});_tmp1A->tl=Cyc_libargs;_tmp1A;});}}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;Cyc_add_ccarg(({const char*_tmp1C="-save-temps";_tag_arr(_tmp1C,sizeof(char),
_get_zero_arr_size(_tmp1C,12));}));}static int Cyc_produce_dependencies=0;static
void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
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
if(Cyc_Std_strcmp(s,({const char*_tmp28="cyc";_tag_arr(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,4));}))== 0)Cyc_intype=(void*)1;else{if(Cyc_Std_strcmp(
s,({const char*_tmp29="none";_tag_arr(_tmp29,sizeof(char),_get_zero_arr_size(
_tmp29,5));}))== 0)Cyc_intype=(void*)0;else{({struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)s;{void*_tmp2A[1]={& _tmp2C};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp2B="Input type '%s' not supported\n";_tag_arr(
_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,31));}),_tag_arr(_tmp2A,sizeof(
void*),1));}});}}}struct _tagged_arr Cyc_make_base_filename(struct _tagged_arr s,
struct _tagged_arr*output_file){struct _tagged_arr _tmp2D=Cyc_Filename_dirname(
output_file == 0?s:*((struct _tagged_arr*)output_file));struct _tagged_arr _tmp2E=
Cyc_Filename_chop_extension((struct _tagged_arr)Cyc_Filename_basename(s));struct
_tagged_arr _tmp2F=Cyc_Std_strlen((struct _tagged_arr)_tmp2D)> 0?Cyc_Filename_concat((
struct _tagged_arr)_tmp2D,(struct _tagged_arr)_tmp2E): _tmp2E;return(struct
_tagged_arr)_tmp2F;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(
struct _tagged_arr s){if(Cyc_intype == (void*)1?1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({struct
_tagged_arr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=s;_tmp31;});_tmp30->tl=
Cyc_cyclone_files;_tmp30;});{struct _tagged_arr _tmp32=Cyc_make_base_filename(s,
Cyc_output_file);struct _tagged_arr _tmp33=Cyc_Std_strconcat(_tmp32,({const char*
_tmp34=".c";_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,3));}));Cyc_add_ccarg((
struct _tagged_arr)_tmp33);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _tagged_arr s){if(Cyc_save_temps_r)return;else{Cyc_Std_remove(s);}}int Cyc_compile_failure=
0;struct Cyc_Std___cycFILE*Cyc_try_file_open(struct _tagged_arr filename,struct
_tagged_arr mode,struct _tagged_arr msg_part){struct _handler_cons _tmp35;
_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!
_tmp37){{struct Cyc_Std___cycFILE*_tmp38=(struct Cyc_Std___cycFILE*)Cyc_Std_file_open(
filename,mode);_npop_handler(0);return _tmp38;};_pop_handler();}else{void*_tmp36=(
void*)_exn_thrown;void*_tmp3A=_tmp36;_LL1:;_LL2: Cyc_compile_failure=1;({struct
Cyc_Std_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)
filename;{struct Cyc_Std_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct
_tagged_arr)msg_part;{void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_Std_fprintf(Cyc_Std_stderr,({
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
1;if(Cyc_Std_strcmp(exn_string,({const char*_tmp4D="";_tag_arr(_tmp4D,sizeof(char),
_get_zero_arr_size(_tmp4D,1));}))!= 0){Cyc_compile_failure=1;({struct Cyc_Std_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)explain_string;{struct Cyc_Std_String_pa_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=(struct _tagged_arr)stage_name;{struct Cyc_Std_String_pa_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=(struct _tagged_arr)exn_string;{void*_tmp4E[3]={&
_tmp50,& _tmp51,& _tmp52};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp4F="\n%s thrown during %s: %s";
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
Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp71="r";_tag_arr(_tmp71,sizeof(
char),_get_zero_arr_size(_tmp71,2));}),({const char*_tmp72="internal compiler file";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,23));}));if(_tmp69 == 0)
return tds;{struct Cyc_Std___cycFILE*_tmp6A=(struct Cyc_Std___cycFILE*)_check_null(
_tmp69);unsigned int n_read=1024;unsigned int n_written;char buf[1024]=(char[1024]){};
while(n_read == 1024){n_read=Cyc_Std_fread(({char*_tmp6B=buf;_tag_arr(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,1024));}),1,1024,_tmp6A);if(n_read != 1024?
!Cyc_Std_feof(_tmp6A): 0){Cyc_compile_failure=1;({void*_tmp6C[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp6D="\nError: problem copying cyc_include.h\n";
_tag_arr(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,39));}),_tag_arr(_tmp6C,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);
return tds;}n_written=Cyc_Std_fwrite(({char*_tmp6E=buf;_tag_arr(_tmp6E,sizeof(
char),_get_zero_arr_size(_tmp6E,1024));}),1,n_read,out_file);if(n_read != 
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
return 0;{struct Cyc_List_List*_tmp73=0;unsigned int _tmp74=Cyc_Std_strlen(s);
while(_tmp74 > 0){struct _tagged_arr _tmp75=Cyc_Std_strchr(s,c);if(_tmp75.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr){_tmp73=({struct Cyc_List_List*_tmp76=
_cycalloc(sizeof(*_tmp76));_tmp76->hd=({struct _tagged_arr*_tmp77=_cycalloc(
sizeof(*_tmp77));_tmp77[0]=s;_tmp77;});_tmp76->tl=_tmp73;_tmp76;});break;}else{
_tmp73=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=({
struct _tagged_arr*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=(struct _tagged_arr)
Cyc_Std_substring(s,0,(unsigned int)((((struct _tagged_arr)_tmp75).curr - s.curr)/ 
sizeof(char)));_tmp79;});_tmp78->tl=_tmp73;_tmp78;});_tmp74 -=(((struct
_tagged_arr)_tmp75).curr - s.curr)/ sizeof(char);s=_tagged_arr_plus(_tmp75,
sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp73);}}static int Cyc_file_exists(struct _tagged_arr file){struct Cyc_Std___cycFILE*
f=0;{struct _handler_cons _tmp7A;_push_handler(& _tmp7A);{int _tmp7C=0;if(setjmp(
_tmp7A.handler))_tmp7C=1;if(!_tmp7C){f=(struct Cyc_Std___cycFILE*)Cyc_Std_file_open(
file,({const char*_tmp7D="r";_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(
_tmp7D,2));}));;_pop_handler();}else{void*_tmp7B=(void*)_exn_thrown;void*_tmp7F=
_tmp7B;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmp7F);_LL12:;}}}if(f == 
0)return 0;else{Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(f));return 1;}}
static struct _tagged_arr Cyc_sprint_list(struct Cyc_List_List*dirs){struct
_tagged_arr tmp=({const char*_tmp82="";_tag_arr(_tmp82,sizeof(char),
_get_zero_arr_size(_tmp82,1));});for(0;dirs != 0;dirs=dirs->tl){struct _tagged_arr
_tmp80=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));if(_tmp80.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen(_tmp80)== 0)continue;
_tmp80=(struct _tagged_arr)Cyc_Std_strconcat(_tmp80,({const char*_tmp81=":";
_tag_arr(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,2));}));tmp=(struct
_tagged_arr)Cyc_Std_strconcat(_tmp80,tmp);}return tmp;}static struct _tagged_arr*
Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr file){if(file.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct
_tagged_arr _tmp83=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));if(
_tmp83.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen(_tmp83)
== 0)continue;{struct _tagged_arr s=(struct _tagged_arr)Cyc_Filename_concat(_tmp83,
file);if(Cyc_file_exists(s))return({struct _tagged_arr*_tmp84=_cycalloc(sizeof(*
_tmp84));_tmp84[0]=s;_tmp84;});}}return 0;}static struct _tagged_arr Cyc_do_find(
struct Cyc_List_List*dirs,struct _tagged_arr file){struct _tagged_arr*_tmp85=Cyc_find(
dirs,file);if(_tmp85 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({
struct Cyc_Core_Failure_struct _tmp87;_tmp87.tag=Cyc_Core_Failure;_tmp87.f1=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(
struct _tagged_arr)Cyc_sprint_list(dirs);{struct Cyc_Std_String_pa_struct _tmp8A;
_tmp8A.tag=0;_tmp8A.f1=(struct _tagged_arr)file;{void*_tmp88[2]={& _tmp8A,& _tmp8B};
Cyc_Std_aprintf(({const char*_tmp89="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp89,sizeof(char),_get_zero_arr_size(_tmp89,56));}),_tag_arr(_tmp88,
sizeof(void*),2));}}});_tmp87;});_tmp86;}));}return*((struct _tagged_arr*)_tmp85);}
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_tagged_arr subdir){struct Cyc_List_List*_tmp8C=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp8C=({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->hd=({
struct _tagged_arr*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=(struct _tagged_arr)
Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd)),subdir);
_tmp8E;});_tmp8D->tl=_tmp8C;_tmp8D;});}_tmp8C=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8C);return _tmp8C;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _tagged_arr subdir){struct Cyc_List_List*
_tmp8F=0;for(0;dirs != 0;dirs=dirs->tl){_tmp8F=({struct Cyc_List_List*_tmp90=
_cycalloc(sizeof(*_tmp90));_tmp90->hd=(struct _tagged_arr*)dirs->hd;_tmp90->tl=
_tmp8F;_tmp90;});_tmp8F=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91->hd=({struct _tagged_arr*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=(
struct _tagged_arr)Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)
dirs->hd)),subdir);_tmp92;});_tmp91->tl=_tmp8F;_tmp91;});}_tmp8F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8F);return _tmp8F;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp93=Cyc_Std_strlen(s);int _tmp94=0;int _tmp95=0;{
int i=0;for(0;i < _tmp93;i ++){char _tmp96=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(_tmp96 == '\'')_tmp94 ++;else{if(Cyc_is_other_special(_tmp96))
_tmp95 ++;}}}if(_tmp94 == 0?_tmp95 == 0: 0)return s;if(_tmp94 == 0)return(struct
_tagged_arr)Cyc_Std_strconcat_l(({struct _tagged_arr*_tmp97[3];_tmp97[2]=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(char),2);_tmp97[1]=({
struct _tagged_arr*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=(struct _tagged_arr)
s;_tmp99;});_tmp97[0]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp97,sizeof(struct _tagged_arr*),3));}));{unsigned int _tmp9B=(_tmp93 + 
_tmp94)+ _tmp95;struct _tagged_arr s2=({unsigned int _tmpA5=_tmp9B + 1;char*_tmpA6=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpA5 + 1));struct _tagged_arr
_tmpA8=_tag_arr(_tmpA6,sizeof(char),_tmpA5 + 1);{unsigned int _tmpA7=_tmpA5;
unsigned int i;for(i=0;i < _tmpA7;i ++){_tmpA6[i]='\000';}_tmpA6[_tmpA7]=(char)0;}
_tmpA8;});int _tmp9C=0;int _tmp9D=0;for(0;_tmp9C < _tmp93;_tmp9C ++){char _tmp9E=*((
const char*)_check_unknown_subscript(s,sizeof(char),_tmp9C));if(_tmp9E == '\''?1:
Cyc_is_other_special(_tmp9E))({struct _tagged_arr _tmp9F=_tagged_arr_plus(s2,
sizeof(char),_tmp9D ++);char _tmpA0=*((char*)_check_unknown_subscript(_tmp9F,
sizeof(char),0));char _tmpA1='\\';if(_get_arr_size(_tmp9F,sizeof(char))== 1?
_tmpA0 == '\000'?_tmpA1 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp9F.curr)=
_tmpA1;});({struct _tagged_arr _tmpA2=_tagged_arr_plus(s2,sizeof(char),_tmp9D ++);
char _tmpA3=*((char*)_check_unknown_subscript(_tmpA2,sizeof(char),0));char _tmpA4=
_tmp9E;if(_get_arr_size(_tmpA2,sizeof(char))== 1?_tmpA3 == '\000'?_tmpA4 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmpA2.curr)=_tmpA4;});}return(struct
_tagged_arr)s2;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct
_tagged_arr*sp){return({struct _tagged_arr*_tmpA9=_cycalloc(sizeof(*_tmpA9));
_tmpA9[0]=Cyc_sh_escape_string(*((struct _tagged_arr*)sp));_tmpA9;});}static void
Cyc_process_file(struct _tagged_arr filename){struct _tagged_arr _tmpAA=Cyc_make_base_filename(
filename,Cyc_output_file);struct _tagged_arr _tmpAB=Cyc_Std_strconcat(_tmpAA,({
const char*_tmp100=".cyp";_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(
_tmp100,5));}));struct _tagged_arr _tmpAC=Cyc_Std_strconcat(_tmpAA,({const char*
_tmpFF=".cyci";_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,6));}));
struct _tagged_arr _tmpAD=Cyc_Std_strconcat(_tmpAA,({const char*_tmpFE=".cycio";
_tag_arr(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,7));}));struct _tagged_arr
_tmpAE=Cyc_Std_strconcat(_tmpAA,({const char*_tmpFD=".c";_tag_arr(_tmpFD,sizeof(
char),_get_zero_arr_size(_tmpFD,3));}));if(Cyc_v_r)({struct Cyc_Std_String_pa_struct
_tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct _tagged_arr)filename;{void*_tmpAF[1]={&
_tmpB1};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpB0="Compiling %s\n";
_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,14));}),_tag_arr(_tmpAF,
sizeof(void*),1));}});{struct Cyc_Std___cycFILE*f0=Cyc_try_file_open(filename,({
const char*_tmpFB="r";_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,2));}),({
const char*_tmpFC="input file";_tag_arr(_tmpFC,sizeof(char),_get_zero_arr_size(
_tmpFC,11));}));if(Cyc_compile_failure)return;Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(f0));{struct _tagged_arr _tmpB2=Cyc_Std_str_sepstr(({struct Cyc_List_List*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmpF8->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmpF8;}),({const char*_tmpFA=" ";_tag_arr(_tmpFA,sizeof(char),
_get_zero_arr_size(_tmpFA,2));}));struct _tagged_arr def_inc_path=(struct
_tagged_arr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpB3=Cyc_add_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);_tmpB3=Cyc_add_subdir(_tmpB3,({const char*
_tmpB4="include";_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,8));}));
if(Cyc_Std_strlen(def_inc_path)> 0)_tmpB3=({struct Cyc_List_List*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5->hd=({struct _tagged_arr*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6[0]=def_inc_path;_tmpB6;});_tmpB5->tl=_tmpB3;_tmpB5;});{struct _tagged_arr
_tmpB7=Cyc_Std_getenv(({const char*_tmpF7="CYCLONE_INCLUDE_PATH";_tag_arr(_tmpF7,
sizeof(char),_get_zero_arr_size(_tmpF7,21));}));if(_tmpB7.curr != (_tag_arr(0,0,0)).curr)
_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_split_by_char((struct _tagged_arr)_tmpB7,':'),_tmpB3);{
struct _tagged_arr stdinc_string=(struct _tagged_arr)Cyc_Std_str_sepstr(({struct Cyc_List_List*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmpF4->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpB3);_tmpF4;}),({const char*_tmpF6=" -I";_tag_arr(
_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,4));}));struct _tagged_arr
ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)ofile_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_output_file));{void*
_tmpB8[1]={& _tmpBA};Cyc_Std_aprintf(({const char*_tmpB9=" > %s";_tag_arr(_tmpB9,
sizeof(char),_get_zero_arr_size(_tmpB9,6));}),_tag_arr(_tmpB8,sizeof(void*),1));}});
else{ofile_string=({const char*_tmpBB="";_tag_arr(_tmpBB,sizeof(char),
_get_zero_arr_size(_tmpBB,1));});}}else{ofile_string=(struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _tagged_arr)Cyc_sh_escape_string((
struct _tagged_arr)_tmpAB);{void*_tmpBC[1]={& _tmpBE};Cyc_Std_aprintf(({const char*
_tmpBD=" > %s";_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,6));}),
_tag_arr(_tmpBC,sizeof(void*),1));}});}{struct _tagged_arr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0)fixup_string=({const char*_tmpBF=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_arr(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,35));});else{fixup_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_dependencies_target));{
void*_tmpC0[1]={& _tmpC2};Cyc_Std_aprintf(({const char*_tmpC1=" | sed 's/^.*\\.cyc\\.o:/%s:/'";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,29));}),_tag_arr(_tmpC0,
sizeof(void*),1));}});}}else{fixup_string=({const char*_tmpC3="";_tag_arr(_tmpC3,
sizeof(char),_get_zero_arr_size(_tmpC3,1));});}{struct _tagged_arr _tmpC4=({struct
Cyc_Std_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)
ofile_string;{struct Cyc_Std_String_pa_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(
struct _tagged_arr)fixup_string;{struct Cyc_Std_String_pa_struct _tmpF1;_tmpF1.tag=
0;_tmpF1.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{struct Cyc_Std_String_pa_struct
_tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct _tagged_arr)stdinc_string;{struct Cyc_Std_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)_tmpB2;{struct Cyc_Std_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)Cyc_cpp;{void*_tmpEC[6]={&
_tmpEE,& _tmpEF,& _tmpF0,& _tmpF1,& _tmpF2,& _tmpF3};Cyc_Std_aprintf(({const char*
_tmpED="%s %s%s %s%s%s";_tag_arr(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,
15));}),_tag_arr(_tmpEC,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_Std_String_pa_struct
_tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _tagged_arr)_tmpC4;{void*_tmpC5[1]={& _tmpC7};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpC6="%s\n";_tag_arr(_tmpC6,sizeof(
char),_get_zero_arr_size(_tmpC6,4));}),_tag_arr(_tmpC5,sizeof(void*),1));}});if(
Cyc_Std_system((struct _tagged_arr)_tmpC4)!= 0){Cyc_compile_failure=1;({void*
_tmpC8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpC9="\nError: preprocessing\n";
_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,23));}),_tag_arr(_tmpC8,
sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((
struct _tagged_arr)_tmpAB);{struct Cyc_Std___cycFILE*in_file=Cyc_try_file_open((
struct _tagged_arr)_tmpAB,({const char*_tmpEA="r";_tag_arr(_tmpEA,sizeof(char),
_get_zero_arr_size(_tmpEA,2));}),({const char*_tmpEB="file";_tag_arr(_tmpEB,
sizeof(char),_get_zero_arr_size(_tmpEB,5));}));if(Cyc_compile_failure)return;{
struct Cyc_List_List*tds=0;{struct _handler_cons _tmpCA;_push_handler(& _tmpCA);{int
_tmpCC=0;if(setjmp(_tmpCA.handler))_tmpCC=1;if(!_tmpCC){tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Std___cycFILE*,
struct Cyc_List_List*),struct Cyc_Std___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpCD="parsing";_tag_arr(_tmpCD,sizeof(char),_get_zero_arr_size(
_tmpCD,8));}),Cyc_do_parse,(struct Cyc_Std___cycFILE*)_check_null(in_file),tds);;
_pop_handler();}else{void*_tmpCB=(void*)_exn_thrown;void*_tmpCF=_tmpCB;_LL27:;
_LL28: Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(in_file));Cyc_remove_file((
struct _tagged_arr)_tmpAB);(int)_throw(_tmpCF);_LL29:;_LL2A:(void)_throw(_tmpCF);
_LL26:;}}}Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(in_file));if(
Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAB);return;}{struct
Cyc_Tcenv_Tenv*_tmpD0=Cyc_Tcenv_tc_init();if(Cyc_parseonly_r)goto PRINTC;tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpD1="type checking";_tag_arr(_tmpD1,sizeof(
char),_get_zero_arr_size(_tmpD1,14));}),Cyc_do_typecheck,_tmpD0,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAB);return;}tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD2="control-flow checking";
_tag_arr(_tmpD2,sizeof(char),_get_zero_arr_size(_tmpD2,22));}),Cyc_do_cfcheck,1,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAB);return;}
if(Cyc_ic_r){struct Cyc_Std___cycFILE*inter_file=Cyc_Std_fopen((struct _tagged_arr)
_tmpAC,({const char*_tmpD7="r";_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(
_tmpD7,2));}));struct Cyc_Std___cycFILE*inter_objfile=Cyc_try_file_open((struct
_tagged_arr)_tmpAD,({const char*_tmpD5="w";_tag_arr(_tmpD5,sizeof(char),
_get_zero_arr_size(_tmpD5,2));}),({const char*_tmpD6="interface object file";
_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,22));}));if(inter_objfile
== 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((struct
_tagged_arr)_tmpAC);tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct _tuple10*,struct Cyc_List_List*),struct _tuple10*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD3="interface checking";
_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,19));}),Cyc_do_interface,({
struct _tuple10*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=_tmpD0;_tmpD4->f2=
inter_file;_tmpD4->f3=(struct Cyc_Std___cycFILE*)_check_null(inter_objfile);
_tmpD4;}),tds);if(inter_file != 0)Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_file));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_objfile));}if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD8="translation to C";
_tag_arr(_tmpD8,sizeof(char),_get_zero_arr_size(_tmpD8,17));}),Cyc_do_translate,
1,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAB);
return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();Cyc_remove_file((
struct _tagged_arr)_tmpAB);if(Cyc_compile_failure)return;if(Cyc_tovc_r)tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,
struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpD9="post-pass to VC";_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,
16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)return;PRINTC: {struct Cyc_Std___cycFILE*
out_file;if(Cyc_parseonly_r?1: Cyc_tc_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmpDA="w";_tag_arr(
_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,2));}),({const char*_tmpDB="output file";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,12));}));else{out_file=(
struct Cyc_Std___cycFILE*)Cyc_Std_stdout;}}else{if(Cyc_toc_r?Cyc_output_file != 0:
0)out_file=Cyc_try_file_open(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmpDC="w";_tag_arr(_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,2));}),({
const char*_tmpDD="output file";_tag_arr(_tmpDD,sizeof(char),_get_zero_arr_size(
_tmpDD,12));}));else{out_file=Cyc_try_file_open((struct _tagged_arr)_tmpAE,({
const char*_tmpDE="w";_tag_arr(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,2));}),({
const char*_tmpDF="output file";_tag_arr(_tmpDF,sizeof(char),_get_zero_arr_size(
_tmpDF,12));}));}}if(Cyc_compile_failure)return;{struct _handler_cons _tmpE0;
_push_handler(& _tmpE0);{int _tmpE2=0;if(setjmp(_tmpE0.handler))_tmpE2=1;if(!
_tmpE2){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(struct Cyc_Std___cycFILE*,struct Cyc_List_List*),struct Cyc_Std___cycFILE*env,
struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpE3="printing";_tag_arr(
_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,9));}),Cyc_do_print,(struct Cyc_Std___cycFILE*)
_check_null(out_file),tds);;_pop_handler();}else{void*_tmpE1=(void*)_exn_thrown;
void*_tmpE5=_tmpE1;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_Std_file_close((struct
Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=({struct _tagged_arr*_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=(struct _tagged_arr)_tmpAE;_tmpE7;});_tmpE6->tl=
Cyc_cfiles;_tmpE6;});(int)_throw(_tmpE5);_LL2E:;_LL2F:(void)_throw(_tmpE5);_LL2B:;}}}
Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({
struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=({struct
_tagged_arr*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=(struct _tagged_arr)
_tmpAE;_tmpE9;});_tmpE8->tl=Cyc_cfiles;_tmpE8;});}}}}}}}}}}}static char _tmp101[8]="<final>";
static struct _tagged_arr Cyc_final_str={_tmp101,_tmp101,_tmp101 + 8};static struct
_tagged_arr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _tagged_arr*n){if(n == (struct _tagged_arr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _tagged_arr basename;{struct _handler_cons _tmp102;_push_handler(& _tmp102);{
int _tmp104=0;if(setjmp(_tmp102.handler))_tmp104=1;if(!_tmp104){basename=(struct
_tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)n));;_pop_handler();}
else{void*_tmp103=(void*)_exn_thrown;void*_tmp106=_tmp103;_LL31: if(*((void**)
_tmp106)!= Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*((struct
_tagged_arr*)n);goto _LL30;_LL33:;_LL34:(void)_throw(_tmp106);_LL30:;}}}{struct
_tagged_arr _tmp107=Cyc_Std_strconcat(basename,({const char*_tmp10C=".cycio";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,7));}));struct Cyc_Std___cycFILE*
_tmp108=Cyc_try_file_open((struct _tagged_arr)_tmp107,({const char*_tmp10A="rb";
_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,3));}),({const char*
_tmp10B="interface object file";_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(
_tmp10B,22));}));if(_tmp108 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(
1);}Cyc_Position_reset_position((struct _tagged_arr)_tmp107);{struct Cyc_Interface_I*
_tmp109=Cyc_Interface_load((struct Cyc_Std___cycFILE*)_check_null(_tmp108));Cyc_Std_file_close((
struct Cyc_Std___cycFILE*)_check_null(_tmp108));return _tmp109;}}}}static int Cyc_is_cfile(
struct _tagged_arr*n){return*((const char*)_check_unknown_subscript(*((struct
_tagged_arr*)n),sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct
_tuple11{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();{
struct _tagged_arr comp=(struct _tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((
struct _tagged_arr)Cyc_Std_strconcat(comp,({const char*_tmp10D=" -x c -E -U__GNUC__ -nostdinc";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,30));})));{struct Cyc_List_List*
options=({struct _tuple11*_tmp173[51];_tmp173[50]=({struct _tuple11*_tmp2A0=
_cycalloc(sizeof(*_tmp2A0));_tmp2A0->f1=({const char*_tmp2A5="-noregister";
_tag_arr(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,12));});_tmp2A0->f2=0;
_tmp2A0->f3=({const char*_tmp2A4="";_tag_arr(_tmp2A4,sizeof(char),
_get_zero_arr_size(_tmp2A4,1));});_tmp2A0->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2A3;_tmp2A3.tag=3;_tmp2A3.f1=& Cyc_Parse_no_register;_tmp2A3;});_tmp2A2;});
_tmp2A0->f5=({const char*_tmp2A1="Treat register storage class as public";
_tag_arr(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,39));});_tmp2A0;});
_tmp173[49]=({struct _tuple11*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->f1=({
const char*_tmp29F="-warnboundschecks";_tag_arr(_tmp29F,sizeof(char),
_get_zero_arr_size(_tmp29F,18));});_tmp29A->f2=0;_tmp29A->f3=({const char*_tmp29E="";
_tag_arr(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,1));});_tmp29A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));
_tmp29C[0]=({struct Cyc_Arg_Set_spec_struct _tmp29D;_tmp29D.tag=3;_tmp29D.f1=& Cyc_Toc_warn_bounds_checks;
_tmp29D;});_tmp29C;});_tmp29A->f5=({const char*_tmp29B="Warn when bounds checks can't be eliminated";
_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,44));});_tmp29A;});
_tmp173[48]=({struct _tuple11*_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->f1=({
const char*_tmp299="-CI";_tag_arr(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,
4));});_tmp294->f2=0;_tmp294->f3=({const char*_tmp298=" <file>";_tag_arr(_tmp298,
sizeof(char),_get_zero_arr_size(_tmp298,8));});_tmp294->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Arg_String_spec_struct
_tmp297;_tmp297.tag=5;_tmp297.f1=Cyc_set_cyc_include;_tmp297;});_tmp296;});
_tmp294->f5=({const char*_tmp295="Set cyc_include.h to be <file>";_tag_arr(
_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,31));});_tmp294;});_tmp173[47]=({
struct _tuple11*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->f1=({const char*
_tmp293="-printfullevars";_tag_arr(_tmp293,sizeof(char),_get_zero_arr_size(
_tmp293,16));});_tmp28E->f2=0;_tmp28E->f3=({const char*_tmp292="";_tag_arr(
_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,1));});_tmp28E->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290[0]=({
struct Cyc_Arg_Set_spec_struct _tmp291;_tmp291.tag=3;_tmp291.f1=& Cyc_print_full_evars_r;
_tmp291;});_tmp290;});_tmp28E->f5=({const char*_tmp28F="Print full information for evars (type debugging)";
_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,50));});_tmp28E;});
_tmp173[46]=({struct _tuple11*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->f1=({
const char*_tmp28D="-printallkinds";_tag_arr(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,15));});_tmp288->f2=0;_tmp288->f3=({const char*_tmp28C="";
_tag_arr(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,1));});_tmp288->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28A=_cycalloc(sizeof(*_tmp28A));
_tmp28A[0]=({struct Cyc_Arg_Set_spec_struct _tmp28B;_tmp28B.tag=3;_tmp28B.f1=& Cyc_print_all_kinds_r;
_tmp28B;});_tmp28A;});_tmp288->f5=({const char*_tmp289="Always print kinds of type variables";
_tag_arr(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,37));});_tmp288;});
_tmp173[45]=({struct _tuple11*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->f1=({
const char*_tmp287="-printalltvars";_tag_arr(_tmp287,sizeof(char),
_get_zero_arr_size(_tmp287,15));});_tmp282->f2=0;_tmp282->f3=({const char*_tmp286="";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,1));});_tmp282->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp284=_cycalloc(sizeof(*_tmp284));
_tmp284[0]=({struct Cyc_Arg_Set_spec_struct _tmp285;_tmp285.tag=3;_tmp285.f1=& Cyc_print_all_tvars_r;
_tmp285;});_tmp284;});_tmp282->f5=({const char*_tmp283="Print all type variables (even implicit default effects)";
_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,57));});_tmp282;});
_tmp173[44]=({struct _tuple11*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->f1=({
const char*_tmp281="-noexpandtypedefs";_tag_arr(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,18));});_tmp27C->f2=0;_tmp27C->f3=({const char*_tmp280="";
_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,1));});_tmp27C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp27E=_cycalloc(sizeof(*_tmp27E));
_tmp27E[0]=({struct Cyc_Arg_Set_spec_struct _tmp27F;_tmp27F.tag=3;_tmp27F.f1=& Cyc_noexpand_r;
_tmp27F;});_tmp27E;});_tmp27C->f5=({const char*_tmp27D="Don't expand typedefs in pretty printing";
_tag_arr(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,41));});_tmp27C;});
_tmp173[43]=({struct _tuple11*_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->f1=({
const char*_tmp27B="-noremoveunused";_tag_arr(_tmp27B,sizeof(char),
_get_zero_arr_size(_tmp27B,16));});_tmp276->f2=0;_tmp276->f3=({const char*_tmp27A="";
_tag_arr(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,1));});_tmp276->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp278=_cycalloc(sizeof(*_tmp278));
_tmp278[0]=({struct Cyc_Arg_Set_spec_struct _tmp279;_tmp279.tag=3;_tmp279.f1=& Cyc_noshake_r;
_tmp279;});_tmp278;});_tmp276->f5=({const char*_tmp277="Don't remove externed variables that aren't used";
_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,49));});_tmp276;});
_tmp173[42]=({struct _tuple11*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->f1=({
const char*_tmp275="-nogc";_tag_arr(_tmp275,sizeof(char),_get_zero_arr_size(
_tmp275,6));});_tmp270->f2=0;_tmp270->f3=({const char*_tmp274="";_tag_arr(_tmp274,
sizeof(char),_get_zero_arr_size(_tmp274,1));});_tmp270->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Arg_Set_spec_struct
_tmp273;_tmp273.tag=3;_tmp273.f1=& Cyc_nogc_r;_tmp273;});_tmp272;});_tmp270->f5=({
const char*_tmp271="Don't link in the garbage collector";_tag_arr(_tmp271,sizeof(
char),_get_zero_arr_size(_tmp271,36));});_tmp270;});_tmp173[41]=({struct _tuple11*
_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->f1=({const char*_tmp26F="-nocyc";
_tag_arr(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,7));});_tmp26A->f2=0;
_tmp26A->f3=({const char*_tmp26E="";_tag_arr(_tmp26E,sizeof(char),
_get_zero_arr_size(_tmp26E,1));});_tmp26A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26D;_tmp26D.tag=0;_tmp26D.f1=Cyc_set_nocyc;_tmp26D;});_tmp26C;});_tmp26A->f5=({
const char*_tmp26B="Don't add implicit namespace Cyc";_tag_arr(_tmp26B,sizeof(
char),_get_zero_arr_size(_tmp26B,33));});_tmp26A;});_tmp173[40]=({struct _tuple11*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->f1=({const char*_tmp269="-no-cpp-precomp";
_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,16));});_tmp264->f2=0;
_tmp264->f3=({const char*_tmp268="";_tag_arr(_tmp268,sizeof(char),
_get_zero_arr_size(_tmp268,1));});_tmp264->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp267;_tmp267.tag=0;_tmp267.f1=Cyc_set_nocppprecomp;_tmp267;});_tmp266;});
_tmp264->f5=({const char*_tmp265="Don't do smart preprocessing (mac only)";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,40));});_tmp264;});
_tmp173[39]=({struct _tuple11*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->f1=({
const char*_tmp263="-use-cpp";_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(
_tmp263,9));});_tmp25E->f2=0;_tmp25E->f3=({const char*_tmp262="<path>";_tag_arr(
_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,7));});_tmp25E->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({
struct Cyc_Arg_String_spec_struct _tmp261;_tmp261.tag=5;_tmp261.f1=Cyc_set_cpp;
_tmp261;});_tmp260;});_tmp25E->f5=({const char*_tmp25F="Indicate which preprocessor to use";
_tag_arr(_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,35));});_tmp25E;});
_tmp173[38]=({struct _tuple11*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->f1=({
const char*_tmp25D="--noboundschecks";_tag_arr(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,17));});_tmp258->f2=0;_tmp258->f3=({const char*_tmp25C="";
_tag_arr(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,1));});_tmp258->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));
_tmp25A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25B;_tmp25B.tag=0;_tmp25B.f1=Cyc_set_noboundschecks;
_tmp25B;});_tmp25A;});_tmp258->f5=({const char*_tmp259="Disable bounds checks";
_tag_arr(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,22));});_tmp258;});
_tmp173[37]=({struct _tuple11*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->f1=({
const char*_tmp257="--nonullchecks";_tag_arr(_tmp257,sizeof(char),
_get_zero_arr_size(_tmp257,15));});_tmp252->f2=0;_tmp252->f3=({const char*_tmp256="";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,1));});_tmp252->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp254=_cycalloc(sizeof(*_tmp254));
_tmp254[0]=({struct Cyc_Arg_Unit_spec_struct _tmp255;_tmp255.tag=0;_tmp255.f1=Cyc_set_nonullchecks;
_tmp255;});_tmp254;});_tmp252->f5=({const char*_tmp253="Disable null checks";
_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,20));});_tmp252;});
_tmp173[36]=({struct _tuple11*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->f1=({
const char*_tmp251="--nochecks";_tag_arr(_tmp251,sizeof(char),_get_zero_arr_size(
_tmp251,11));});_tmp24C->f2=0;_tmp24C->f3=({const char*_tmp250="";_tag_arr(
_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,1));});_tmp24C->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp24F;_tmp24F.tag=0;_tmp24F.f1=Cyc_set_nochecks;
_tmp24F;});_tmp24E;});_tmp24C->f5=({const char*_tmp24D="Disable bounds/null checks";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,27));});_tmp24C;});
_tmp173[35]=({struct _tuple11*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->f1=({
const char*_tmp24B="--lineno";_tag_arr(_tmp24B,sizeof(char),_get_zero_arr_size(
_tmp24B,9));});_tmp246->f2=0;_tmp246->f3=({const char*_tmp24A="";_tag_arr(_tmp24A,
sizeof(char),_get_zero_arr_size(_tmp24A,1));});_tmp246->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=Cyc_set_lineno;_tmp249;});_tmp248;});_tmp246->f5=({
const char*_tmp247="Generate line numbers for debugging";_tag_arr(_tmp247,sizeof(
char),_get_zero_arr_size(_tmp247,36));});_tmp246;});_tmp173[34]=({struct _tuple11*
_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240->f1=({const char*_tmp245="-save-c";
_tag_arr(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,8));});_tmp240->f2=0;
_tmp240->f3=({const char*_tmp244="";_tag_arr(_tmp244,sizeof(char),
_get_zero_arr_size(_tmp244,1));});_tmp240->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242[0]=({struct Cyc_Arg_Set_spec_struct
_tmp243;_tmp243.tag=3;_tmp243.f1=& Cyc_save_c_r;_tmp243;});_tmp242;});_tmp240->f5=({
const char*_tmp241="Don't delete temporary C files";_tag_arr(_tmp241,sizeof(char),
_get_zero_arr_size(_tmp241,31));});_tmp240;});_tmp173[33]=({struct _tuple11*
_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=({const char*_tmp23F="-save-temps";
_tag_arr(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,12));});_tmp23A->f2=0;
_tmp23A->f3=({const char*_tmp23E="";_tag_arr(_tmp23E,sizeof(char),
_get_zero_arr_size(_tmp23E,1));});_tmp23A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp23D;_tmp23D.tag=0;_tmp23D.f1=Cyc_set_save_temps;_tmp23D;});_tmp23C;});
_tmp23A->f5=({const char*_tmp23B="Don't delete temporary files";_tag_arr(_tmp23B,
sizeof(char),_get_zero_arr_size(_tmp23B,29));});_tmp23A;});_tmp173[32]=({struct
_tuple11*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->f1=({const char*_tmp239="-tovc";
_tag_arr(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,6));});_tmp234->f2=0;
_tmp234->f3=({const char*_tmp238="";_tag_arr(_tmp238,sizeof(char),
_get_zero_arr_size(_tmp238,1));});_tmp234->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Arg_Set_spec_struct
_tmp237;_tmp237.tag=3;_tmp237.f1=& Cyc_tovc_r;_tmp237;});_tmp236;});_tmp234->f5=({
const char*_tmp235="Avoid gcc extensions in C output";_tag_arr(_tmp235,sizeof(
char),_get_zero_arr_size(_tmp235,33));});_tmp234;});_tmp173[31]=({struct _tuple11*
_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->f1=({const char*_tmp233="-up";
_tag_arr(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,4));});_tmp22E->f2=0;
_tmp22E->f3=({const char*_tmp232="";_tag_arr(_tmp232,sizeof(char),
_get_zero_arr_size(_tmp232,1));});_tmp22E->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp231;_tmp231.tag=4;_tmp231.f1=& Cyc_pp_r;_tmp231;});_tmp230;});_tmp22E->f5=({
const char*_tmp22F="Ugly print";_tag_arr(_tmp22F,sizeof(char),_get_zero_arr_size(
_tmp22F,11));});_tmp22E;});_tmp173[30]=({struct _tuple11*_tmp228=_cycalloc(
sizeof(*_tmp228));_tmp228->f1=({const char*_tmp22D="-pp";_tag_arr(_tmp22D,sizeof(
char),_get_zero_arr_size(_tmp22D,4));});_tmp228->f2=0;_tmp228->f3=({const char*
_tmp22C="";_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,1));});
_tmp228->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22A=_cycalloc(sizeof(*
_tmp22A));_tmp22A[0]=({struct Cyc_Arg_Set_spec_struct _tmp22B;_tmp22B.tag=3;
_tmp22B.f1=& Cyc_pp_r;_tmp22B;});_tmp22A;});_tmp228->f5=({const char*_tmp229="Pretty print";
_tag_arr(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,13));});_tmp228;});
_tmp173[29]=({struct _tuple11*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->f1=({
const char*_tmp227="-ic";_tag_arr(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,
4));});_tmp222->f2=0;_tmp222->f3=({const char*_tmp226="";_tag_arr(_tmp226,sizeof(
char),_get_zero_arr_size(_tmp226,1));});_tmp222->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Arg_Set_spec_struct
_tmp225;_tmp225.tag=3;_tmp225.f1=& Cyc_ic_r;_tmp225;});_tmp224;});_tmp222->f5=({
const char*_tmp223="Activate the link-checker";_tag_arr(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,26));});_tmp222;});_tmp173[28]=({struct _tuple11*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->f1=({const char*_tmp221="-stopafter-toc";
_tag_arr(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,15));});_tmp21C->f2=0;
_tmp21C->f3=({const char*_tmp220="";_tag_arr(_tmp220,sizeof(char),
_get_zero_arr_size(_tmp220,1));});_tmp21C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Arg_Set_spec_struct
_tmp21F;_tmp21F.tag=3;_tmp21F.f1=& Cyc_toc_r;_tmp21F;});_tmp21E;});_tmp21C->f5=({
const char*_tmp21D="Stop after translation to C";_tag_arr(_tmp21D,sizeof(char),
_get_zero_arr_size(_tmp21D,28));});_tmp21C;});_tmp173[27]=({struct _tuple11*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->f1=({const char*_tmp21B="-stopafter-tc";
_tag_arr(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,14));});_tmp216->f2=0;
_tmp216->f3=({const char*_tmp21A="";_tag_arr(_tmp21A,sizeof(char),
_get_zero_arr_size(_tmp21A,1));});_tmp216->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Arg_Set_spec_struct
_tmp219;_tmp219.tag=3;_tmp219.f1=& Cyc_tc_r;_tmp219;});_tmp218;});_tmp216->f5=({
const char*_tmp217="Stop after type checking";_tag_arr(_tmp217,sizeof(char),
_get_zero_arr_size(_tmp217,25));});_tmp216;});_tmp173[26]=({struct _tuple11*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->f1=({const char*_tmp215="-stopafter-parse";
_tag_arr(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,17));});_tmp210->f2=0;
_tmp210->f3=({const char*_tmp214="";_tag_arr(_tmp214,sizeof(char),
_get_zero_arr_size(_tmp214,1));});_tmp210->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Arg_Set_spec_struct
_tmp213;_tmp213.tag=3;_tmp213.f1=& Cyc_parseonly_r;_tmp213;});_tmp212;});_tmp210->f5=({
const char*_tmp211="Stop after parsing";_tag_arr(_tmp211,sizeof(char),
_get_zero_arr_size(_tmp211,19));});_tmp210;});_tmp173[25]=({struct _tuple11*
_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->f1=({const char*_tmp20F="-E";
_tag_arr(_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,3));});_tmp20A->f2=0;
_tmp20A->f3=({const char*_tmp20E="";_tag_arr(_tmp20E,sizeof(char),
_get_zero_arr_size(_tmp20E,1));});_tmp20A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Arg_Set_spec_struct
_tmp20D;_tmp20D.tag=3;_tmp20D.f1=& Cyc_stop_after_cpp_r;_tmp20D;});_tmp20C;});
_tmp20A->f5=({const char*_tmp20B="Stop after preprocessing";_tag_arr(_tmp20B,
sizeof(char),_get_zero_arr_size(_tmp20B,25));});_tmp20A;});_tmp173[24]=({struct
_tuple11*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->f1=({const char*_tmp209="-b";
_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,3));});_tmp204->f2=0;
_tmp204->f3=({const char*_tmp208="<arch>";_tag_arr(_tmp208,sizeof(char),
_get_zero_arr_size(_tmp208,7));});_tmp204->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=({struct Cyc_Arg_String_spec_struct
_tmp207;_tmp207.tag=5;_tmp207.f1=Cyc_set_target_arch;_tmp207;});_tmp206;});
_tmp204->f5=({const char*_tmp205="Set target architecture; implies -stopafter-toc";
_tag_arr(_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,48));});_tmp204;});
_tmp173[23]=({struct _tuple11*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->f1=({
const char*_tmp203="-MT";_tag_arr(_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,
4));});_tmp1FE->f2=0;_tmp1FE->f3=({const char*_tmp202=" <target>";_tag_arr(
_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,10));});_tmp1FE->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({
struct Cyc_Arg_String_spec_struct _tmp201;_tmp201.tag=5;_tmp201.f1=Cyc_set_dependencies_target;
_tmp201;});_tmp200;});_tmp1FE->f5=({const char*_tmp1FF="Give target for dependencies";
_tag_arr(_tmp1FF,sizeof(char),_get_zero_arr_size(_tmp1FF,29));});_tmp1FE;});
_tmp173[22]=({struct _tuple11*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->f1=({
const char*_tmp1FD="-MG";_tag_arr(_tmp1FD,sizeof(char),_get_zero_arr_size(_tmp1FD,
4));});_tmp1F8->f2=0;_tmp1F8->f3=({const char*_tmp1FC="";_tag_arr(_tmp1FC,sizeof(
char),_get_zero_arr_size(_tmp1FC,1));});_tmp1F8->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1FB;_tmp1FB.tag=1;_tmp1FB.f1=Cyc_add_cpparg;_tmp1FB;});_tmp1FA;});_tmp1F8->f5=({
const char*_tmp1F9="When producing dependencies assume that missing files are generated";
_tag_arr(_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,68));});_tmp1F8;});
_tmp173[21]=({struct _tuple11*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->f1=({
const char*_tmp1F7="-M";_tag_arr(_tmp1F7,sizeof(char),_get_zero_arr_size(_tmp1F7,
3));});_tmp1F2->f2=0;_tmp1F2->f3=({const char*_tmp1F6="";_tag_arr(_tmp1F6,sizeof(
char),_get_zero_arr_size(_tmp1F6,1));});_tmp1F2->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=Cyc_set_produce_dependencies;_tmp1F5;});_tmp1F4;});
_tmp1F2->f5=({const char*_tmp1F3="Produce dependencies";_tag_arr(_tmp1F3,sizeof(
char),_get_zero_arr_size(_tmp1F3,21));});_tmp1F2;});_tmp173[20]=({struct _tuple11*
_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->f1=({const char*_tmp1F1="-S";
_tag_arr(_tmp1F1,sizeof(char),_get_zero_arr_size(_tmp1F1,3));});_tmp1EC->f2=0;
_tmp1EC->f3=({const char*_tmp1F0="";_tag_arr(_tmp1F0,sizeof(char),
_get_zero_arr_size(_tmp1F0,1));});_tmp1EC->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=Cyc_set_stop_after_asmfile;_tmp1EF;});_tmp1EE;});
_tmp1EC->f5=({const char*_tmp1ED="Stop after producing assembly code";_tag_arr(
_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,35));});_tmp1EC;});_tmp173[19]=({
struct _tuple11*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->f1=({const char*
_tmp1EB="-pa";_tag_arr(_tmp1EB,sizeof(char),_get_zero_arr_size(_tmp1EB,4));});
_tmp1E6->f2=0;_tmp1E6->f3=({const char*_tmp1EA="";_tag_arr(_tmp1EA,sizeof(char),
_get_zero_arr_size(_tmp1EA,1));});_tmp1E6->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=Cyc_set_pa;_tmp1E9;});_tmp1E8;});_tmp1E6->f5=({
const char*_tmp1E7="Compile for profiling with aprof";_tag_arr(_tmp1E7,sizeof(
char),_get_zero_arr_size(_tmp1E7,33));});_tmp1E6;});_tmp173[18]=({struct _tuple11*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->f1=({const char*_tmp1E5="-pg";
_tag_arr(_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,4));});_tmp1E0->f2=0;
_tmp1E0->f3=({const char*_tmp1E4="";_tag_arr(_tmp1E4,sizeof(char),
_get_zero_arr_size(_tmp1E4,1));});_tmp1E0->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E3;_tmp1E3.tag=0;_tmp1E3.f1=Cyc_set_pg;_tmp1E3;});_tmp1E2;});_tmp1E0->f5=({
const char*_tmp1E1="Compile for profiling with gprof";_tag_arr(_tmp1E1,sizeof(
char),_get_zero_arr_size(_tmp1E1,33));});_tmp1E0;});_tmp173[17]=({struct _tuple11*
_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=({const char*_tmp1DF="-p";
_tag_arr(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,3));});_tmp1DA->f2=0;
_tmp1DA->f3=({const char*_tmp1DE="";_tag_arr(_tmp1DE,sizeof(char),
_get_zero_arr_size(_tmp1DE,1));});_tmp1DA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1DD;_tmp1DD.tag=1;_tmp1DD.f1=Cyc_add_ccarg;_tmp1DD;});_tmp1DC;});_tmp1DA->f5=({
const char*_tmp1DB="Compile for profiling with prof";_tag_arr(_tmp1DB,sizeof(char),
_get_zero_arr_size(_tmp1DB,32));});_tmp1DA;});_tmp173[16]=({struct _tuple11*
_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=({const char*_tmp1D9="-g";
_tag_arr(_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,3));});_tmp1D4->f2=0;
_tmp1D4->f3=({const char*_tmp1D8="";_tag_arr(_tmp1D8,sizeof(char),
_get_zero_arr_size(_tmp1D8,1));});_tmp1D4->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=Cyc_add_ccarg;_tmp1D7;});_tmp1D6;});_tmp1D4->f5=({
const char*_tmp1D5="Compile for debugging";_tag_arr(_tmp1D5,sizeof(char),
_get_zero_arr_size(_tmp1D5,22));});_tmp1D4;});_tmp173[15]=({struct _tuple11*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->f1=({const char*_tmp1D3="-fomit-frame-pointer";
_tag_arr(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,21));});_tmp1CE->f2=0;
_tmp1CE->f3=({const char*_tmp1D2="";_tag_arr(_tmp1D2,sizeof(char),
_get_zero_arr_size(_tmp1D2,1));});_tmp1CE->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=Cyc_add_ccarg;_tmp1D1;});_tmp1D0;});_tmp1CE->f5=({
const char*_tmp1CF="Omit frame pointer";_tag_arr(_tmp1CF,sizeof(char),
_get_zero_arr_size(_tmp1CF,19));});_tmp1CE;});_tmp173[14]=({struct _tuple11*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->f1=({const char*_tmp1CD="-O3";
_tag_arr(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,4));});_tmp1C8->f2=0;
_tmp1C8->f3=({const char*_tmp1CC="";_tag_arr(_tmp1CC,sizeof(char),
_get_zero_arr_size(_tmp1CC,1));});_tmp1C8->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CB;_tmp1CB.tag=1;_tmp1CB.f1=Cyc_add_ccarg;_tmp1CB;});_tmp1CA;});_tmp1C8->f5=({
const char*_tmp1C9="Even more optimization";_tag_arr(_tmp1C9,sizeof(char),
_get_zero_arr_size(_tmp1C9,23));});_tmp1C8;});_tmp173[13]=({struct _tuple11*
_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->f1=({const char*_tmp1C7="-O2";
_tag_arr(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,4));});_tmp1C2->f2=0;
_tmp1C2->f3=({const char*_tmp1C6="";_tag_arr(_tmp1C6,sizeof(char),
_get_zero_arr_size(_tmp1C6,1));});_tmp1C2->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=Cyc_add_ccarg;_tmp1C5;});_tmp1C4;});_tmp1C2->f5=({
const char*_tmp1C3="A higher level of optimization";_tag_arr(_tmp1C3,sizeof(char),
_get_zero_arr_size(_tmp1C3,31));});_tmp1C2;});_tmp173[12]=({struct _tuple11*
_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->f1=({const char*_tmp1C1="-O";
_tag_arr(_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,3));});_tmp1BC->f2=0;
_tmp1BC->f3=({const char*_tmp1C0="";_tag_arr(_tmp1C0,sizeof(char),
_get_zero_arr_size(_tmp1C0,1));});_tmp1BC->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1BF;_tmp1BF.tag=1;_tmp1BF.f1=Cyc_add_ccarg;_tmp1BF;});_tmp1BE;});_tmp1BC->f5=({
const char*_tmp1BD="Optimize";_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(
_tmp1BD,9));});_tmp1BC;});_tmp173[11]=({struct _tuple11*_tmp1B6=_cycalloc(sizeof(*
_tmp1B6));_tmp1B6->f1=({const char*_tmp1BB="-O0";_tag_arr(_tmp1BB,sizeof(char),
_get_zero_arr_size(_tmp1BB,4));});_tmp1B6->f2=0;_tmp1B6->f3=({const char*_tmp1BA="";
_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,1));});_tmp1B6->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));
_tmp1B8[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B9;_tmp1B9.tag=1;_tmp1B9.f1=Cyc_add_ccarg;
_tmp1B9;});_tmp1B8;});_tmp1B6->f5=({const char*_tmp1B7="Don't optimize";_tag_arr(
_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,15));});_tmp1B6;});_tmp173[10]=({
struct _tuple11*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=({const char*
_tmp1B5="-s";_tag_arr(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,3));});
_tmp1B0->f2=0;_tmp1B0->f3=({const char*_tmp1B4="";_tag_arr(_tmp1B4,sizeof(char),
_get_zero_arr_size(_tmp1B4,1));});_tmp1B0->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=Cyc_add_ccarg;_tmp1B3;});_tmp1B2;});_tmp1B0->f5=({
const char*_tmp1B1="Remove all symbol table and relocation info from executable";
_tag_arr(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,60));});_tmp1B0;});
_tmp173[9]=({struct _tuple11*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->f1=({
const char*_tmp1AF="-x";_tag_arr(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,
3));});_tmp1AA->f2=0;_tmp1AA->f3=({const char*_tmp1AE=" <language>";_tag_arr(
_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,12));});_tmp1AA->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=({
struct Cyc_Arg_String_spec_struct _tmp1AD;_tmp1AD.tag=5;_tmp1AD.f1=Cyc_set_inputtype;
_tmp1AD;});_tmp1AC;});_tmp1AA->f5=({const char*_tmp1AB="Specify <language> for the following input files";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,49));});_tmp1AA;});
_tmp173[8]=({struct _tuple11*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->f1=({
const char*_tmp1A9="-c";_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,
3));});_tmp1A4->f2=0;_tmp1A4->f3=({const char*_tmp1A8="";_tag_arr(_tmp1A8,sizeof(
char),_get_zero_arr_size(_tmp1A8,1));});_tmp1A4->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=Cyc_set_stop_after_objectfile;_tmp1A7;});
_tmp1A6;});_tmp1A4->f5=({const char*_tmp1A5="Produce an object file instead of an executable; do not link";
_tag_arr(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,61));});_tmp1A4;});
_tmp173[7]=({struct _tuple11*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->f1=({
const char*_tmp1A3="-l";_tag_arr(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,
3));});_tmp19E->f2=1;_tmp19E->f3=({const char*_tmp1A2="<file>";_tag_arr(_tmp1A2,
sizeof(char),_get_zero_arr_size(_tmp1A2,7));});_tmp19E->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A1;_tmp1A1.tag=1;_tmp1A1.f1=Cyc_add_libarg;_tmp1A1;});_tmp1A0;});_tmp19E->f5=({
const char*_tmp19F="Library file";_tag_arr(_tmp19F,sizeof(char),
_get_zero_arr_size(_tmp19F,13));});_tmp19E;});_tmp173[6]=({struct _tuple11*
_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->f1=({const char*_tmp19D="-L";
_tag_arr(_tmp19D,sizeof(char),_get_zero_arr_size(_tmp19D,3));});_tmp198->f2=1;
_tmp198->f3=({const char*_tmp19C="<dir>";_tag_arr(_tmp19C,sizeof(char),
_get_zero_arr_size(_tmp19C,6));});_tmp198->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp19B;_tmp19B.tag=1;_tmp19B.f1=Cyc_add_ccarg;_tmp19B;});_tmp19A;});_tmp198->f5=({
const char*_tmp199="Add to the list of directories for -l";_tag_arr(_tmp199,
sizeof(char),_get_zero_arr_size(_tmp199,38));});_tmp198;});_tmp173[5]=({struct
_tuple11*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->f1=({const char*_tmp197="-I";
_tag_arr(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,3));});_tmp192->f2=1;
_tmp192->f3=({const char*_tmp196="<dir>";_tag_arr(_tmp196,sizeof(char),
_get_zero_arr_size(_tmp196,6));});_tmp192->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp195;_tmp195.tag=1;_tmp195.f1=Cyc_add_cpparg;_tmp195;});_tmp194;});_tmp192->f5=({
const char*_tmp193="Add to the list of directories to search for include files";
_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,59));});_tmp192;});
_tmp173[4]=({struct _tuple11*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->f1=({
const char*_tmp191="-B";_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,
3));});_tmp18C->f2=1;_tmp18C->f3=({const char*_tmp190="<file>";_tag_arr(_tmp190,
sizeof(char),_get_zero_arr_size(_tmp190,7));});_tmp18C->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp18F;_tmp18F.tag=1;_tmp18F.f1=Cyc_add_cyclone_exec_path;_tmp18F;});_tmp18E;});
_tmp18C->f5=({const char*_tmp18D="Add to the list of directories to search for compiler files";
_tag_arr(_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,60));});_tmp18C;});
_tmp173[3]=({struct _tuple11*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->f1=({
const char*_tmp18B="-D";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,
3));});_tmp186->f2=1;_tmp186->f3=({const char*_tmp18A="<name>[=<value>]";_tag_arr(
_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,17));});_tmp186->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp189;_tmp189.tag=1;_tmp189.f1=Cyc_add_cpparg;
_tmp189;});_tmp188;});_tmp186->f5=({const char*_tmp187="Pass definition to preprocessor";
_tag_arr(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,32));});_tmp186;});
_tmp173[2]=({struct _tuple11*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->f1=({
const char*_tmp185="-o";_tag_arr(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,
3));});_tmp180->f2=0;_tmp180->f3=({const char*_tmp184=" <file>";_tag_arr(_tmp184,
sizeof(char),_get_zero_arr_size(_tmp184,8));});_tmp180->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=({struct Cyc_Arg_String_spec_struct
_tmp183;_tmp183.tag=5;_tmp183.f1=Cyc_set_output_file;_tmp183;});_tmp182;});
_tmp180->f5=({const char*_tmp181="Set the output file name to <file>";_tag_arr(
_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,35));});_tmp180;});_tmp173[1]=({
struct _tuple11*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->f1=({const char*
_tmp17F="--version";_tag_arr(_tmp17F,sizeof(char),_get_zero_arr_size(_tmp17F,10));});
_tmp17A->f2=0;_tmp17A->f3=({const char*_tmp17E="";_tag_arr(_tmp17E,sizeof(char),
_get_zero_arr_size(_tmp17E,1));});_tmp17A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp17D;_tmp17D.tag=0;_tmp17D.f1=Cyc_print_version;_tmp17D;});_tmp17C;});_tmp17A->f5=({
const char*_tmp17B="Print version information and exit";_tag_arr(_tmp17B,sizeof(
char),_get_zero_arr_size(_tmp17B,35));});_tmp17A;});_tmp173[0]=({struct _tuple11*
_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->f1=({const char*_tmp179="-v";
_tag_arr(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,3));});_tmp174->f2=0;
_tmp174->f3=({const char*_tmp178="";_tag_arr(_tmp178,sizeof(char),
_get_zero_arr_size(_tmp178,1));});_tmp174->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Arg_Set_spec_struct
_tmp177;_tmp177.tag=3;_tmp177.f1=& Cyc_v_r;_tmp177;});_tmp176;});_tmp174->f5=({
const char*_tmp175="Print compilation stages verbosely";_tag_arr(_tmp175,sizeof(
char),_get_zero_arr_size(_tmp175,35));});_tmp174;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp173,sizeof(struct _tuple11*),51));});
Cyc_Arg_parse(options,Cyc_add_other,({const char*_tmp10E="Options:";_tag_arr(
_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,9));}),argv);if(Cyc_cyclone_files
== 0?Cyc_ccargs == 0: 0){({void*_tmp10F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp110="missing file\n";_tag_arr(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,14));}),_tag_arr(_tmp10F,sizeof(void*),0));});exit(1);}
if(Cyc_target_arch.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_target_arch=(
struct _tagged_arr)Cstring_to_string(Carch);{struct _tagged_arr cyclone_exec_prefix=(
struct _tagged_arr)Cyc_Std_getenv(({const char*_tmp172="CYCLONE_EXEC_PREFIX";
_tag_arr(_tmp172,sizeof(char),_get_zero_arr_size(_tmp172,20));}));if(
cyclone_exec_prefix.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->hd=({struct
_tagged_arr*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=cyclone_exec_prefix;
_tmp112;});_tmp111->tl=Cyc_cyclone_exec_path;_tmp111;});{struct _tagged_arr
def_lib_path=(struct _tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_Std_strlen(
def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp113=_cycalloc(
sizeof(*_tmp113));_tmp113->hd=({struct _tagged_arr*_tmp114=_cycalloc(sizeof(*
_tmp114));_tmp114[0]=(struct _tagged_arr)Cyc_Filename_concat(def_lib_path,({const
char*_tmp115="cyc-lib";_tag_arr(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,
8));}));_tmp114;});_tmp113->tl=Cyc_cyclone_exec_path;_tmp113;});Cyc_cyclone_exec_path=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{
struct Cyc_List_List*_tmp116=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
if(!Cyc_stop_after_cpp_r?Cyc_Std_strlen(Cyc_cyc_include)== 0: 0)Cyc_cyc_include=
Cyc_do_find(Cyc_cyclone_exec_path,({const char*_tmp117="cyc_include.h";_tag_arr(
_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,14));}));{struct Cyc_List_List*
_tmp118=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp118 != 0;_tmp118=_tmp118->tl){Cyc_process_file(*((struct _tagged_arr*)((
struct _tagged_arr*)_tmp118->hd)));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r?
1: Cyc_parseonly_r)?1: Cyc_tc_r)?1: Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _tagged_arr)Cyc_Std_strconcat(({const char*_tmp119="-L";_tag_arr(_tmp119,
sizeof(char),_get_zero_arr_size(_tmp119,3));}),def_lib_path));Cyc_ccargs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct
_tagged_arr _tmp11A=Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_ccargs),({const char*_tmp171=" ";_tag_arr(_tmp171,sizeof(char),
_get_zero_arr_size(_tmp171,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmp11B=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp16E->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp16E;}),({const char*
_tmp170=" ";_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,2));}));
struct Cyc_List_List*stdlib;struct _tagged_arr stdlib_string;int _tmp11C=((Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmp16C=".a";
_tag_arr(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,3));})): 0))?1:(Cyc_output_file
!= 0?Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmp16D=".lib";_tag_arr(_tmp16D,sizeof(char),_get_zero_arr_size(
_tmp16D,5));})): 0);if(_tmp11C){stdlib=0;stdlib_string=({const char*_tmp11D="";
_tag_arr(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,1));});}else{struct
_tagged_arr libcyc_flag;struct _tagged_arr nogc_filename;struct _tagged_arr
runtime_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){libcyc_flag=({const
char*_tmp11E="-lcyc_a";_tag_arr(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,
8));});nogc_filename=({const char*_tmp11F="nogc_a.a";_tag_arr(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,9));});runtime_filename=({const char*_tmp120="runtime_cyc_a.o";
_tag_arr(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp121="-lcyc_nocheck";_tag_arr(_tmp121,sizeof(char),
_get_zero_arr_size(_tmp121,14));});nogc_filename=({const char*_tmp122="nogc_nocheck.a";
_tag_arr(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,15));});
runtime_filename=({const char*_tmp123="runtime_cyc.o";_tag_arr(_tmp123,sizeof(
char),_get_zero_arr_size(_tmp123,14));});}else{if(Cyc_pg_r){libcyc_flag=({const
char*_tmp124="-lcyc_pg";_tag_arr(_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,
9));});runtime_filename=({const char*_tmp125="runtime_cyc_pg.a";_tag_arr(_tmp125,
sizeof(char),_get_zero_arr_size(_tmp125,17));});nogc_filename=({const char*
_tmp126="nogc_pg.a";_tag_arr(_tmp126,sizeof(char),_get_zero_arr_size(_tmp126,10));});}
else{libcyc_flag=({const char*_tmp127="-lcyc";_tag_arr(_tmp127,sizeof(char),
_get_zero_arr_size(_tmp127,6));});nogc_filename=({const char*_tmp128="nogc.a";
_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,7));});runtime_filename=({
const char*_tmp129="runtime_cyc.o";_tag_arr(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,14));});}}}gc_filename=({const char*_tmp12A="gc.a";
_tag_arr(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,5));});{struct
_tagged_arr _tmp12B=Cyc_nogc_r?Cyc_do_find(_tmp116,nogc_filename): Cyc_do_find(
_tmp116,gc_filename);struct _tagged_arr _tmp12C=Cyc_do_find(_tmp116,
runtime_filename);stdlib=0;stdlib_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)_tmp12B;{struct Cyc_Std_String_pa_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=(struct _tagged_arr)libcyc_flag;{struct Cyc_Std_String_pa_struct
_tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct _tagged_arr)_tmp12C;{void*_tmp12D[3]={&
_tmp12F,& _tmp130,& _tmp131};Cyc_Std_aprintf(({const char*_tmp12E=" %s %s %s";
_tag_arr(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,10));}),_tag_arr(
_tmp12D,sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp132;
_push_handler(& _tmp132);{int _tmp134=0;if(setjmp(_tmp132.handler))_tmp134=1;if(!
_tmp134){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct
Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp11C)_tmp135=({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));
_tmp136->hd=Cyc_final_strptr;_tmp136->tl=_tmp135;_tmp136;});{struct Cyc_Interface_I*
_tmp137=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_tagged_arr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp135,_tmp135);if(_tmp137 == 0){({void*_tmp138[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp139="Error: interfaces incompatible\n";_tag_arr(
_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,32));}),_tag_arr(_tmp138,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13A=1;
_npop_handler(0);return _tmp13A;}}if(_tmp11C){if(Cyc_output_file != 0){struct
_tagged_arr _tmp13B=({struct Cyc_Std_String_pa_struct _tmp142;_tmp142.tag=0;_tmp142.f1=(
struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)_check_null(
Cyc_output_file)));{void*_tmp140[1]={& _tmp142};Cyc_Std_aprintf(({const char*
_tmp141="%s.cycio";_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,9));}),
_tag_arr(_tmp140,sizeof(void*),1));}});struct Cyc_Std___cycFILE*_tmp13C=Cyc_try_file_open((
struct _tagged_arr)_tmp13B,({const char*_tmp13E="wb";_tag_arr(_tmp13E,sizeof(char),
_get_zero_arr_size(_tmp13E,3));}),({const char*_tmp13F="interface object file";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,22));}));if(_tmp13C == 0){
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13D=1;_npop_handler(0);return
_tmp13D;}}Cyc_Interface_save((struct Cyc_Interface_I*)_check_null(_tmp137),(
struct Cyc_Std___cycFILE*)_check_null(_tmp13C));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(_tmp13C));}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(
struct Cyc_Interface_I*)_check_null(_tmp137),({struct _tuple9*_tmp143=_cycalloc(
sizeof(*_tmp143));_tmp143->f1=({const char*_tmp145="empty interface";_tag_arr(
_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,16));});_tmp143->f2=({const char*
_tmp144="global interface";_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(
_tmp144,17));});_tmp143;}))){({void*_tmp146[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp147="Error: some objects are still undefined\n";_tag_arr(_tmp147,
sizeof(char),_get_zero_arr_size(_tmp147,41));}),_tag_arr(_tmp146,sizeof(void*),0));});
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp148=1;_npop_handler(0);return
_tmp148;}}}}};_pop_handler();}else{void*_tmp133=(void*)_exn_thrown;void*_tmp14A=
_tmp133;struct _tagged_arr _tmp14B;struct _tagged_arr _tmp14C;struct _tagged_arr
_tmp14D;_LL36: if(*((void**)_tmp14A)!= Cyc_Core_Failure)goto _LL38;_tmp14B=((
struct Cyc_Core_Failure_struct*)_tmp14A)->f1;_LL37:({struct Cyc_Std_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _tagged_arr)_tmp14B;{void*_tmp14E[1]={&
_tmp150};Cyc_Std_printf(({const char*_tmp14F="Exception Core::Failure %s\n";
_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,28));}),_tag_arr(
_tmp14E,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL38: if(*((void**)_tmp14A)!= Cyc_Core_Impossible)goto _LL3A;_tmp14C=((struct Cyc_Core_Impossible_struct*)
_tmp14A)->f1;_LL39:({struct Cyc_Std_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(
struct _tagged_arr)_tmp14C;{void*_tmp151[1]={& _tmp153};Cyc_Std_printf(({const char*
_tmp152="Exception Core::Impossible %s\n";_tag_arr(_tmp152,sizeof(char),
_get_zero_arr_size(_tmp152,31));}),_tag_arr(_tmp151,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3A: if(_tmp14A != Cyc_Dict_Absent)goto _LL3C;_LL3B:({
void*_tmp154[0]={};Cyc_Std_printf(({const char*_tmp155="Exception Dict::Absent\n";
_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,24));}),_tag_arr(
_tmp154,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3C: if(*((void**)_tmp14A)!= Cyc_Core_Invalid_argument)goto _LL3E;_tmp14D=((
struct Cyc_Core_Invalid_argument_struct*)_tmp14A)->f1;_LL3D:({struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)_tmp14D;{void*_tmp156[1]={&
_tmp158};Cyc_Std_printf(({const char*_tmp157="Exception Core::Invalid_argument %s\n";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,37));}),_tag_arr(
_tmp156,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3E:;_LL3F:({void*_tmp159[0]={};Cyc_Std_printf(({const char*_tmp15A="Uncaught exception\n";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,20));}),_tag_arr(
_tmp159,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL40:;_LL41:(void)_throw(_tmp14A);_LL35:;}}}{struct _tagged_arr _tmp15B=({struct
Cyc_Std_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(struct _tagged_arr)
_tmp11B;{struct Cyc_Std_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(struct
_tagged_arr)stdlib_string;{struct Cyc_Std_String_pa_struct _tmp165;_tmp165.tag=0;
_tmp165.f1=(struct _tagged_arr)_tmp11A;{struct Cyc_Std_String_pa_struct _tmp164;
_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)(Cyc_output_file == 0?({const char*
_tmp168="";_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,1));}):(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(
struct _tagged_arr)Cyc_sh_escape_string(*((struct _tagged_arr*)_check_null(Cyc_output_file)));{
void*_tmp169[1]={& _tmp16B};Cyc_Std_aprintf(({const char*_tmp16A=" -o %s";_tag_arr(
_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,7));}),_tag_arr(_tmp169,sizeof(
void*),1));}}));{struct Cyc_Std_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(
struct _tagged_arr)comp;{void*_tmp161[5]={& _tmp163,& _tmp164,& _tmp165,& _tmp166,&
_tmp167};Cyc_Std_aprintf(({const char*_tmp162="%s %s %s%s%s";_tag_arr(_tmp162,
sizeof(char),_get_zero_arr_size(_tmp162,13));}),_tag_arr(_tmp161,sizeof(void*),5));}}}}}});
if(Cyc_v_r)({struct Cyc_Std_String_pa_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(
struct _tagged_arr)_tmp15B;{void*_tmp15C[1]={& _tmp15E};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp15D="%s\n";_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(
_tmp15D,4));}),_tag_arr(_tmp15C,sizeof(void*),1));}});if(Cyc_Std_system((struct
_tagged_arr)_tmp15B)!= 0){({void*_tmp15F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp160="Error: C compiler failed\n";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,26));}),_tag_arr(_tmp15F,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();return Cyc_compile_failure?1: 0;}}}}}}}}}
