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
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;
};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int
tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fclose(
struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);int Cyc_fflush(struct Cyc___cycFILE*);
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);unsigned int Cyc_fread(struct _tagged_arr,
unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int
tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
unsigned int Cyc_fwrite(struct _tagged_arr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_printf(struct _tagged_arr,struct _tagged_arr);int remove(const char*);extern
char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_tagged_arr,struct _tagged_arr);void Cyc_file_close(struct Cyc___cycFILE*);typedef
struct{int quot;int rem;}Cyc_div_t;typedef struct{int quot;int rem;}Cyc_ldiv_t;char*
getenv(const char*);int system(const char*);unsigned int Cyc_strlen(struct
_tagged_arr s);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);struct
_tagged_arr Cyc_strconcat(struct _tagged_arr,struct _tagged_arr);struct _tagged_arr
Cyc_strconcat_l(struct Cyc_List_List*);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct _tagged_arr Cyc_substring(struct _tagged_arr,int ofs,
unsigned int n);struct _tagged_arr Cyc_strchr(struct _tagged_arr s,char c);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct _tagged_arr Cyc_Filename_concat(
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
Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern int Cyc_Parse_no_register;
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
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*
tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);void Cyc_Lex_lex_init();int Cyc_Toc_warn_bounds_checks;
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
static void Cyc_print_version(){({struct Cyc_String_pa_struct _tmp6;_tmp6.tag=0;
_tmp6.f1=(struct _tagged_arr)Cstring_to_string(Cversion);{void*_tmp4[1]={& _tmp6};
Cyc_printf(({const char*_tmp5="The Cyclone compiler, version %s\n";_tag_arr(_tmp5,
sizeof(char),_get_zero_arr_size(_tmp5,34));}),_tag_arr(_tmp4,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)
Cstring_to_string(Ccomp);{struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(
struct _tagged_arr)Cstring_to_string(Carch);{void*_tmp7[2]={& _tmp9,& _tmpA};Cyc_printf(({
const char*_tmp8="Compiled for architecture: %s CC=\"%s\"\n";_tag_arr(_tmp8,
sizeof(char),_get_zero_arr_size(_tmp8,39));}),_tag_arr(_tmp7,sizeof(void*),2));}}});({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _tagged_arr)
Cstring_to_string(Cdef_lib_path);{void*_tmpB[1]={& _tmpD};Cyc_printf(({const char*
_tmpC="Standard library directory: %s\n";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,32));}),_tag_arr(_tmpB,sizeof(void*),1));}});({struct
Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _tagged_arr)
Cstring_to_string(Cdef_inc_path);{void*_tmpE[1]={& _tmp10};Cyc_printf(({const char*
_tmpF="Standard include directory: %s\n";_tag_arr(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,32));}),_tag_arr(_tmpE,sizeof(void*),1));}});exit(0);}
static int Cyc_is_cyclone_sourcefile(struct _tagged_arr s){unsigned int _tmp11=Cyc_strlen((
struct _tagged_arr)s);if(_tmp11 <= 4)return 0;else{return Cyc_strcmp((struct
_tagged_arr)_tagged_arr_plus(s,sizeof(char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";
_tag_arr(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,5));}))== 0;}}static
struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _tagged_arr s){Cyc_ccargs=({
struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=({struct
_tagged_arr*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=s;_tmp14;});_tmp13->tl=
Cyc_ccargs;_tmp13;});}static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static
void Cyc_add_cyclone_exec_path(struct _tagged_arr s){unsigned int _tmp15=Cyc_strlen((
struct _tagged_arr)s);if(_tmp15 <= 2)return;{struct _tagged_arr _tmp16=(struct
_tagged_arr)Cyc_substring((struct _tagged_arr)s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct
_tagged_arr*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=
Cyc_cyclone_exec_path;_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static
void Cyc_add_libarg(struct _tagged_arr s){if(Cyc_strcmp((struct _tagged_arr)s,({
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
if(Cyc_strcmp((struct _tagged_arr)s,({const char*_tmp28="cyc";_tag_arr(_tmp28,
sizeof(char),_get_zero_arr_size(_tmp28,4));}))== 0)Cyc_intype=(void*)1;else{if(
Cyc_strcmp((struct _tagged_arr)s,({const char*_tmp29="none";_tag_arr(_tmp29,
sizeof(char),_get_zero_arr_size(_tmp29,5));}))== 0)Cyc_intype=(void*)0;else{({
struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)s;{
void*_tmp2A[1]={& _tmp2C};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B="Input type '%s' not supported\n";
_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,31));}),_tag_arr(_tmp2A,
sizeof(void*),1));}});}}}struct _tagged_arr Cyc_make_base_filename(struct
_tagged_arr s,struct _tagged_arr*output_file){struct _tagged_arr _tmp2D=Cyc_Filename_dirname(
output_file == 0?s:*((struct _tagged_arr*)output_file));struct _tagged_arr _tmp2E=
Cyc_Filename_chop_extension((struct _tagged_arr)Cyc_Filename_basename(s));struct
_tagged_arr _tmp2F=Cyc_strlen((struct _tagged_arr)_tmp2D)> 0?Cyc_Filename_concat((
struct _tagged_arr)_tmp2D,(struct _tagged_arr)_tmp2E): _tmp2E;return(struct
_tagged_arr)_tmp2F;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(
struct _tagged_arr s){if(Cyc_intype == (void*)1?1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({struct
_tagged_arr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=s;_tmp31;});_tmp30->tl=
Cyc_cyclone_files;_tmp30;});{struct _tagged_arr _tmp32=Cyc_make_base_filename(s,
Cyc_output_file);struct _tagged_arr _tmp33=Cyc_strconcat((struct _tagged_arr)_tmp32,({
const char*_tmp34=".c";_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,3));}));
Cyc_add_ccarg((struct _tagged_arr)_tmp33);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _tagged_arr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_arr(s,sizeof(char),1));}}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))
_tmp37=1;if(!_tmp37){{struct Cyc___cycFILE*_tmp38=(struct Cyc___cycFILE*)Cyc_file_open(
filename,mode);_npop_handler(0);return _tmp38;};_pop_handler();}else{void*_tmp36=(
void*)_exn_thrown;void*_tmp3A=_tmp36;_LL1:;_LL2: Cyc_compile_failure=1;({struct
Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)filename;{
struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _tagged_arr)
msg_part;{void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_fprintf(Cyc_stderr,({const char*
_tmp3C="\nError: couldn't open %s %s\n";_tag_arr(_tmp3C,sizeof(char),
_get_zero_arr_size(_tmp3C,29));}),_tag_arr(_tmp3B,sizeof(void*),2));}}});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp3A);_LL0:;}}}
struct Cyc_List_List*Cyc_do_stage(struct _tagged_arr stage_name,struct Cyc_List_List*(*
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
1;if(Cyc_strcmp((struct _tagged_arr)exn_string,({const char*_tmp4D="";_tag_arr(
_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,1));}))!= 0){Cyc_compile_failure=1;({
struct Cyc_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)
explain_string;{struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_tagged_arr)stage_name;{struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(
struct _tagged_arr)exn_string;{void*_tmp4E[3]={& _tmp50,& _tmp51,& _tmp52};Cyc_fprintf(
Cyc_stderr,({const char*_tmp4F="\n%s thrown during %s: %s";_tag_arr(_tmp4F,
sizeof(char),_get_zero_arr_size(_tmp4F,25));}),_tag_arr(_tmp4E,sizeof(void*),3));}}}});}
if(Cyc_compile_failure){({void*_tmp53[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp54="\nCOMPILATION FAILED!\n";_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(
_tmp54,22));}),_tag_arr(_tmp53,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);if(other_exn)(int)_throw(ex);return _tmp3F;}else{if(Cyc_v_r){({struct
Cyc_String_pa_struct _tmp57;_tmp57.tag=0;_tmp57.f1=(struct _tagged_arr)stage_name;{
void*_tmp55[1]={& _tmp57};Cyc_fprintf(Cyc_stderr,({const char*_tmp56="%s completed.\n";
_tag_arr(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,15));}),_tag_arr(_tmp55,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp3F;}}
return _tmp3F;}struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*
ignore){Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp5D=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init();return _tmp5D;}}struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*tds){Cyc_Tc_tc(te,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(
te,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*
tds){Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple10{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple10*params,struct Cyc_List_List*tds){struct _tuple10 _tmp5F;struct Cyc_Tcenv_Tenv*
_tmp60;struct Cyc___cycFILE*_tmp61;struct Cyc___cycFILE*_tmp62;struct _tuple10*
_tmp5E=params;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;_tmp62=_tmp5F.f3;{
struct Cyc_Interface_I*_tmp63=Cyc_Interface_extract(_tmp60->ae);if(_tmp61 == 0)Cyc_Interface_save(
_tmp63,_tmp62);else{struct Cyc_Interface_I*_tmp64=Cyc_Interface_parse((struct Cyc___cycFILE*)
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
Cyc___cycFILE*out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params params_r;
if(Cyc_tc_r)params_r=Cyc_Absynpp_cyc_params_r;else{params_r=Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs=!Cyc_noexpand_r;params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r;params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;{struct Cyc___cycFILE*_tmp69=Cyc_try_file_open(
Cyc_cyc_include,({const char*_tmp70="r";_tag_arr(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,2));}),({const char*_tmp71="internal compiler file";
_tag_arr(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,23));}));if(_tmp69 == 0)
return tds;{struct Cyc___cycFILE*_tmp6A=(struct Cyc___cycFILE*)_check_null(_tmp69);
unsigned int n_read=1024;unsigned int n_written;char buf[1024];{unsigned int _tmp6F=
1024;unsigned int i;for(i=0;i < _tmp6F;i ++){buf[i]='\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_arr(buf,sizeof(char),1024),1,1024,_tmp6A);if(n_read != 1024?
!Cyc_feof(_tmp6A): 0){Cyc_compile_failure=1;({void*_tmp6B[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp6C="\nError: problem copying cyc_include.h\n";_tag_arr(_tmp6C,
sizeof(char),_get_zero_arr_size(_tmp6C,39));}),_tag_arr(_tmp6B,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return tds;}n_written=Cyc_fwrite(
_tag_arr(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=
1;({void*_tmp6D[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6E="\nError: problem copying cyc_include.h\n";
_tag_arr(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,39));}),_tag_arr(_tmp6D,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return tds;}}
Cyc_fclose(_tmp6A);if(Cyc_pp_r){Cyc_Absynpp_set_params(& params_r);Cyc_Absynpp_decllist2file(
tds,out_file);}else{Cyc_Absyndump_set_params(& params_r);Cyc_Absyndump_dumpdecllist2file(
tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)out_file);return tds;}}}static
struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _tagged_arr*)((struct _tagged_arr*)((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->hd)));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _tagged_arr s,char c){if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)
return 0;{struct Cyc_List_List*_tmp72=0;unsigned int _tmp73=Cyc_strlen((struct
_tagged_arr)s);while(_tmp73 > 0){struct _tagged_arr _tmp74=Cyc_strchr(s,c);if(
_tmp74.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){_tmp72=({struct Cyc_List_List*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=({struct _tagged_arr*_tmp76=
_cycalloc(sizeof(*_tmp76));_tmp76[0]=s;_tmp76;});_tmp75->tl=_tmp72;_tmp75;});
break;}else{_tmp72=({struct Cyc_List_List*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77->hd=({struct _tagged_arr*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=(
struct _tagged_arr)Cyc_substring((struct _tagged_arr)s,0,(unsigned int)((((struct
_tagged_arr)_tmp74).curr - s.curr)/ sizeof(char)));_tmp78;});_tmp77->tl=_tmp72;
_tmp77;});_tmp73 -=(((struct _tagged_arr)_tmp74).curr - s.curr)/ sizeof(char);s=
_tagged_arr_plus(_tmp74,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);}}static int Cyc_file_exists(struct
_tagged_arr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmp79;
_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(_tmp79.handler))_tmp7B=1;if(!
_tmp7B){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmp7C="r";
_tag_arr(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,2));}));;_pop_handler();}
else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7E=_tmp7A;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmp7E);_LL12:;}}}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)_check_null(f));return 1;}}static struct _tagged_arr Cyc_sprint_list(
struct Cyc_List_List*dirs){struct _tagged_arr tmp=({const char*_tmp81="";_tag_arr(
_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,1));});for(0;dirs != 0;dirs=dirs->tl){
struct _tagged_arr _tmp7F=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));
if(_tmp7F.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_strlen((struct
_tagged_arr)_tmp7F)== 0)continue;_tmp7F=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)_tmp7F,({const char*_tmp80=":";_tag_arr(_tmp80,sizeof(char),
_get_zero_arr_size(_tmp80,2));}));tmp=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)_tmp7F,(struct _tagged_arr)tmp);}return tmp;}static struct _tagged_arr*
Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr file){if(file.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct
_tagged_arr _tmp82=*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd));if(
_tmp82.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_strlen((struct
_tagged_arr)_tmp82)== 0)continue;{struct _tagged_arr s=(struct _tagged_arr)Cyc_Filename_concat(
_tmp82,file);if(Cyc_file_exists(s))return({struct _tagged_arr*_tmp83=_cycalloc(
sizeof(*_tmp83));_tmp83[0]=s;_tmp83;});}}return 0;}static struct _tagged_arr Cyc_do_find(
struct Cyc_List_List*dirs,struct _tagged_arr file){struct _tagged_arr*_tmp84=Cyc_find(
dirs,file);if(_tmp84 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct
_tmp88;_tmp88.tag=0;_tmp88.f1=(struct _tagged_arr)Cyc_sprint_list(dirs);{struct
Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(struct _tagged_arr)file;{void*
_tmp85[2]={& _tmp87,& _tmp88};Cyc_fprintf(Cyc_stderr,({const char*_tmp86="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,56));}),_tag_arr(_tmp85,
sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({
struct Cyc_Core_Failure_struct _tmp8A;_tmp8A.tag=Cyc_Core_Failure;_tmp8A.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(
struct _tagged_arr)Cyc_sprint_list(dirs);{struct Cyc_String_pa_struct _tmp8D;_tmp8D.tag=
0;_tmp8D.f1=(struct _tagged_arr)file;{void*_tmp8B[2]={& _tmp8D,& _tmp8E};Cyc_aprintf(({
const char*_tmp8C="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,56));}),_tag_arr(_tmp8B,
sizeof(void*),2));}}});_tmp8A;});_tmp89;}));}return*((struct _tagged_arr*)_tmp84);}
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_tagged_arr subdir){struct Cyc_List_List*_tmp8F=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp8F=({struct Cyc_List_List*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->hd=({
struct _tagged_arr*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=(struct _tagged_arr)
Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)dirs->hd)),subdir);
_tmp91;});_tmp90->tl=_tmp8F;_tmp90;});}_tmp8F=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8F);return _tmp8F;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _tagged_arr subdir){struct Cyc_List_List*
_tmp92=0;for(0;dirs != 0;dirs=dirs->tl){_tmp92=({struct Cyc_List_List*_tmp93=
_cycalloc(sizeof(*_tmp93));_tmp93->hd=(struct _tagged_arr*)dirs->hd;_tmp93->tl=
_tmp92;_tmp93;});_tmp92=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));
_tmp94->hd=({struct _tagged_arr*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=(
struct _tagged_arr)Cyc_Filename_concat(*((struct _tagged_arr*)((struct _tagged_arr*)
dirs->hd)),subdir);_tmp95;});_tmp94->tl=_tmp92;_tmp94;});}_tmp92=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp92);return _tmp92;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp96=Cyc_strlen((struct _tagged_arr)s);int
_tmp97=0;int _tmp98=0;{int i=0;for(0;i < _tmp96;i ++){char _tmp99=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(_tmp99 == '\'')_tmp97 ++;else{if(
Cyc_is_other_special(_tmp99))_tmp98 ++;}}}if(_tmp97 == 0?_tmp98 == 0: 0)return s;if(
_tmp97 == 0)return(struct _tagged_arr)Cyc_strconcat_l(({struct _tagged_arr*_tmp9A[3];
_tmp9A[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(char),2);
_tmp9A[1]=({struct _tagged_arr*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=(
struct _tagged_arr)s;_tmp9C;});_tmp9A[0]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp9A,sizeof(struct _tagged_arr*),3));}));{unsigned int
_tmp9E=(_tmp96 + _tmp97)+ _tmp98;struct _tagged_arr s2=({unsigned int _tmpA8=_tmp9E + 
1;char*_tmpA9=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpA8 + 1));
struct _tagged_arr _tmpAB=_tag_arr(_tmpA9,sizeof(char),_tmpA8 + 1);{unsigned int
_tmpAA=_tmpA8;unsigned int i;for(i=0;i < _tmpAA;i ++){_tmpA9[i]='\000';}_tmpA9[
_tmpAA]=(char)0;}_tmpAB;});int _tmp9F=0;int _tmpA0=0;for(0;_tmp9F < _tmp96;_tmp9F ++){
char _tmpA1=*((const char*)_check_unknown_subscript(s,sizeof(char),_tmp9F));if(
_tmpA1 == '\''?1: Cyc_is_other_special(_tmpA1))({struct _tagged_arr _tmpA2=
_tagged_arr_plus(s2,sizeof(char),_tmpA0 ++);char _tmpA3=*((char*)
_check_unknown_subscript(_tmpA2,sizeof(char),0));char _tmpA4='\\';if(
_get_arr_size(_tmpA2,sizeof(char))== 1?_tmpA3 == '\000'?_tmpA4 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA2.curr)=_tmpA4;});({struct _tagged_arr _tmpA5=
_tagged_arr_plus(s2,sizeof(char),_tmpA0 ++);char _tmpA6=*((char*)
_check_unknown_subscript(_tmpA5,sizeof(char),0));char _tmpA7=_tmpA1;if(
_get_arr_size(_tmpA5,sizeof(char))== 1?_tmpA6 == '\000'?_tmpA7 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA5.curr)=_tmpA7;});}return(struct _tagged_arr)
s2;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){
return({struct _tagged_arr*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=Cyc_sh_escape_string(*((
struct _tagged_arr*)sp));_tmpAC;});}static void Cyc_process_file(struct _tagged_arr
filename){struct _tagged_arr _tmpAD=Cyc_make_base_filename(filename,Cyc_output_file);
struct _tagged_arr _tmpAE=Cyc_strconcat((struct _tagged_arr)_tmpAD,({const char*
_tmp102=".cyp";_tag_arr(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,5));}));
struct _tagged_arr _tmpAF=Cyc_strconcat((struct _tagged_arr)_tmpAD,({const char*
_tmp101=".cyci";_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,6));}));
struct _tagged_arr _tmpB0=Cyc_strconcat((struct _tagged_arr)_tmpAD,({const char*
_tmp100=".cycio";_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,7));}));
struct _tagged_arr _tmpB1=Cyc_strconcat((struct _tagged_arr)_tmpAD,({const char*
_tmpFF=".c";_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,3));}));if(
Cyc_v_r)({struct Cyc_String_pa_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=(struct
_tagged_arr)filename;{void*_tmpB2[1]={& _tmpB4};Cyc_fprintf(Cyc_stderr,({const
char*_tmpB3="Compiling %s\n";_tag_arr(_tmpB3,sizeof(char),_get_zero_arr_size(
_tmpB3,14));}),_tag_arr(_tmpB2,sizeof(void*),1));}});{struct Cyc___cycFILE*f0=Cyc_try_file_open(
filename,({const char*_tmpFD="r";_tag_arr(_tmpFD,sizeof(char),_get_zero_arr_size(
_tmpFD,2));}),({const char*_tmpFE="input file";_tag_arr(_tmpFE,sizeof(char),
_get_zero_arr_size(_tmpFE,11));}));if(Cyc_compile_failure)return;Cyc_fclose((
struct Cyc___cycFILE*)_check_null(f0));{struct _tagged_arr _tmpB5=Cyc_str_sepstr(({
struct Cyc_List_List*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmpFA->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmpFA;}),({const char*_tmpFC=" ";_tag_arr(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,2));}));struct _tagged_arr def_inc_path=(struct
_tagged_arr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpB6=Cyc_add_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);_tmpB6=Cyc_add_subdir(_tmpB6,({const char*
_tmpB7="include";_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,8));}));
if(Cyc_strlen((struct _tagged_arr)def_inc_path)> 0)_tmpB6=({struct Cyc_List_List*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->hd=({struct _tagged_arr*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=def_inc_path;_tmpB9;});_tmpB8->tl=_tmpB6;
_tmpB8;});{char*_tmpBA=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmpBA != 0)_tmpB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(Cyc_split_by_char(({char*_tmpBB=_tmpBA;_tag_arr(_tmpBB,
sizeof(char),_get_zero_arr_size(_tmpBB,1));}),':'),_tmpB6);{struct _tagged_arr
stdinc_string=(struct _tagged_arr)Cyc_str_sepstr(({struct Cyc_List_List*_tmpF7=
_cycalloc(sizeof(*_tmpF7));_tmpF7->hd=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"",sizeof(char),1);_tmpF7->tl=((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
_tmpB6);_tmpF7;}),({const char*_tmpF9=" -I";_tag_arr(_tmpF9,sizeof(char),
_get_zero_arr_size(_tmpF9,4));}));struct _tagged_arr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)ofile_string=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_null(Cyc_output_file));{void*_tmpBC[1]={& _tmpBE};Cyc_aprintf(({const char*
_tmpBD=" > %s";_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,6));}),
_tag_arr(_tmpBC,sizeof(void*),1));}});else{ofile_string=({const char*_tmpBF="";
_tag_arr(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,1));});}}else{
ofile_string=(struct _tagged_arr)({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;
_tmpC2.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmpAE);{
void*_tmpC0[1]={& _tmpC2};Cyc_aprintf(({const char*_tmpC1=" > %s";_tag_arr(_tmpC1,
sizeof(char),_get_zero_arr_size(_tmpC1,6));}),_tag_arr(_tmpC0,sizeof(void*),1));}});}{
struct _tagged_arr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0)fixup_string=({const char*_tmpC3=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_arr(_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,35));});else{fixup_string=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmpC6;_tmpC6.tag=0;_tmpC6.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_dependencies_target));{
void*_tmpC4[1]={& _tmpC6};Cyc_aprintf(({const char*_tmpC5=" | sed 's/^.*\\.cyc\\.o:/%s:/'";
_tag_arr(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,29));}),_tag_arr(_tmpC4,
sizeof(void*),1));}});}}else{fixup_string=({const char*_tmpC7="";_tag_arr(_tmpC7,
sizeof(char),_get_zero_arr_size(_tmpC7,1));});}{struct _tagged_arr _tmpC8=({struct
Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)
ofile_string;{struct Cyc_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct
_tagged_arr)fixup_string;{struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(
struct _tagged_arr)Cyc_sh_escape_string(filename);{struct Cyc_String_pa_struct
_tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)stdinc_string;{struct Cyc_String_pa_struct
_tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _tagged_arr)_tmpB5;{struct Cyc_String_pa_struct
_tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _tagged_arr)Cyc_cpp;{void*_tmpEF[6]={&
_tmpF1,& _tmpF2,& _tmpF3,& _tmpF4,& _tmpF5,& _tmpF6};Cyc_aprintf(({const char*_tmpF0="%s %s%s %s%s%s";
_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,15));}),_tag_arr(_tmpEF,
sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_String_pa_struct _tmpCB;_tmpCB.tag=
0;_tmpCB.f1=(struct _tagged_arr)_tmpC8;{void*_tmpC9[1]={& _tmpCB};Cyc_fprintf(Cyc_stderr,({
const char*_tmpCA="%s\n";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,4));}),
_tag_arr(_tmpC9,sizeof(void*),1));}});if(system((const char*)_untag_arr(_tmpC8,
sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*_tmpCC[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpCD="\nError: preprocessing\n";_tag_arr(_tmpCD,sizeof(char),
_get_zero_arr_size(_tmpCD,23));}),_tag_arr(_tmpCC,sizeof(void*),0));});return;}
if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct _tagged_arr)
_tmpAE);{struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _tagged_arr)_tmpAE,({
const char*_tmpED="r";_tag_arr(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,2));}),({
const char*_tmpEE="file";_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,5));}));
if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct _handler_cons
_tmpCE;_push_handler(& _tmpCE);{int _tmpD0=0;if(setjmp(_tmpCE.handler))_tmpD0=1;
if(!_tmpD0){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpD1="parsing";_tag_arr(_tmpD1,sizeof(char),
_get_zero_arr_size(_tmpD1,8));}),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(
in_file),tds);;_pop_handler();}else{void*_tmpCF=(void*)_exn_thrown;void*_tmpD3=
_tmpCF;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _tagged_arr)_tmpAE);(int)_throw(_tmpD3);_LL29:;_LL2A:(
void)_throw(_tmpD3);_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)_check_null(
in_file));if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAE);
return;}{struct Cyc_Tcenv_Tenv*_tmpD4=Cyc_Tcenv_tc_init();if(Cyc_parseonly_r)goto
PRINTC;tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct
Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD5="type checking";_tag_arr(
_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,14));}),Cyc_do_typecheck,_tmpD4,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAE);return;}
tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpD6="control-flow checking";_tag_arr(_tmpD6,sizeof(char),
_get_zero_arr_size(_tmpD6,22));}),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAE);return;}if(Cyc_ic_r){struct Cyc___cycFILE*
inter_file=Cyc_fopen((const char*)((char*)_untag_arr(_tmpAF,sizeof(char),1)),(
const char*)"r");struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct
_tagged_arr)_tmpB0,({const char*_tmpD9="w";_tag_arr(_tmpD9,sizeof(char),
_get_zero_arr_size(_tmpD9,2));}),({const char*_tmpDA="interface object file";
_tag_arr(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,22));}));if(inter_objfile
== 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((struct
_tagged_arr)_tmpAF);tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct _tuple10*,struct Cyc_List_List*),struct _tuple10*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD7="interface checking";
_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,19));}),Cyc_do_interface,({
struct _tuple10*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=_tmpD4;_tmpD8->f2=
inter_file;_tmpD8->f3=(struct Cyc___cycFILE*)_check_null(inter_objfile);_tmpD8;}),
tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)_check_null(
inter_file));Cyc_file_close((struct Cyc___cycFILE*)_check_null(inter_objfile));}
if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpDB="translation to C";_tag_arr(_tmpDB,sizeof(
char),_get_zero_arr_size(_tmpDB,17));}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAE);return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _tagged_arr)_tmpAE);if(Cyc_compile_failure)return;if(Cyc_tovc_r)
tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpDC="post-pass to VC";_tag_arr(_tmpDC,sizeof(char),
_get_zero_arr_size(_tmpDC,16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if(Cyc_parseonly_r?1: Cyc_tc_r){if(
Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct _tagged_arr*)_check_null(
Cyc_output_file)),({const char*_tmpDD="w";_tag_arr(_tmpDD,sizeof(char),
_get_zero_arr_size(_tmpDD,2));}),({const char*_tmpDE="output file";_tag_arr(
_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,12));}));else{out_file=(struct Cyc___cycFILE*)
Cyc_stdout;}}else{if(Cyc_toc_r?Cyc_output_file != 0: 0)out_file=Cyc_try_file_open(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmpDF="w";_tag_arr(
_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,2));}),({const char*_tmpE0="output file";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,12));}));else{out_file=
Cyc_try_file_open((struct _tagged_arr)_tmpB1,({const char*_tmpE1="w";_tag_arr(
_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,2));}),({const char*_tmpE2="output file";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,12));}));}}if(Cyc_compile_failure)
return;{struct _handler_cons _tmpE3;_push_handler(& _tmpE3);{int _tmpE5=0;if(setjmp(
_tmpE3.handler))_tmpE5=1;if(!_tmpE5){tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),
struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpE6="printing";_tag_arr(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,9));}),
Cyc_do_print,(struct Cyc___cycFILE*)_check_null(out_file),tds);;_pop_handler();}
else{void*_tmpE4=(void*)_exn_thrown;void*_tmpE8=_tmpE4;_LL2C:;_LL2D: Cyc_compile_failure=
1;Cyc_file_close((struct Cyc___cycFILE*)_check_null(out_file));Cyc_cfiles=({
struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=({struct
_tagged_arr*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=(struct _tagged_arr)
_tmpB1;_tmpEA;});_tmpE9->tl=Cyc_cfiles;_tmpE9;});(int)_throw(_tmpE8);_LL2E:;
_LL2F:(void)_throw(_tmpE8);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*
_tmpEB));_tmpEB->hd=({struct _tagged_arr*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[
0]=(struct _tagged_arr)_tmpB1;_tmpEC;});_tmpEB->tl=Cyc_cfiles;_tmpEB;});}}}}}}}}}}}
static char _tmp103[8]="<final>";static struct _tagged_arr Cyc_final_str={_tmp103,
_tmp103,_tmp103 + 8};static struct _tagged_arr*Cyc_final_strptr=& Cyc_final_str;
static struct Cyc_Interface_I*Cyc_read_cycio(struct _tagged_arr*n){if(n == (struct
_tagged_arr*)Cyc_final_strptr)return Cyc_Interface_final();{struct _tagged_arr
basename;{struct _handler_cons _tmp104;_push_handler(& _tmp104);{int _tmp106=0;if(
setjmp(_tmp104.handler))_tmp106=1;if(!_tmp106){basename=(struct _tagged_arr)Cyc_Filename_chop_extension(*((
struct _tagged_arr*)n));;_pop_handler();}else{void*_tmp105=(void*)_exn_thrown;
void*_tmp108=_tmp105;_LL31: if(*((void**)_tmp108)!= Cyc_Core_Invalid_argument)
goto _LL33;_LL32: basename=*((struct _tagged_arr*)n);goto _LL30;_LL33:;_LL34:(void)
_throw(_tmp108);_LL30:;}}}{struct _tagged_arr _tmp109=Cyc_strconcat((struct
_tagged_arr)basename,({const char*_tmp10E=".cycio";_tag_arr(_tmp10E,sizeof(char),
_get_zero_arr_size(_tmp10E,7));}));struct Cyc___cycFILE*_tmp10A=Cyc_try_file_open((
struct _tagged_arr)_tmp109,({const char*_tmp10C="rb";_tag_arr(_tmp10C,sizeof(char),
_get_zero_arr_size(_tmp10C,3));}),({const char*_tmp10D="interface object file";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,22));}));if(_tmp10A == 0){
Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _tagged_arr)_tmp109);{struct Cyc_Interface_I*_tmp10B=Cyc_Interface_load((
struct Cyc___cycFILE*)_check_null(_tmp10A));Cyc_file_close((struct Cyc___cycFILE*)
_check_null(_tmp10A));return _tmp10B;}}}}static int Cyc_is_cfile(struct _tagged_arr*
n){return*((const char*)_check_unknown_subscript(*((struct _tagged_arr*)n),sizeof(
char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple11{struct _tagged_arr
f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,
struct _tagged_arr argv){GC_blacklist_warn_clear();{struct _tagged_arr comp=(struct
_tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)comp,({const char*_tmp10F=" -x c -E -U__GNUC__ -nostdinc";
_tag_arr(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,30));})));{struct Cyc_List_List*
options=({struct _tuple11*_tmp175[51];_tmp175[50]=({struct _tuple11*_tmp2A2=
_cycalloc(sizeof(*_tmp2A2));_tmp2A2->f1=({const char*_tmp2A7="-noregister";
_tag_arr(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,12));});_tmp2A2->f2=0;
_tmp2A2->f3=({const char*_tmp2A6="";_tag_arr(_tmp2A6,sizeof(char),
_get_zero_arr_size(_tmp2A6,1));});_tmp2A2->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2A5;_tmp2A5.tag=3;_tmp2A5.f1=& Cyc_Parse_no_register;_tmp2A5;});_tmp2A4;});
_tmp2A2->f5=({const char*_tmp2A3="Treat register storage class as public";
_tag_arr(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,39));});_tmp2A2;});
_tmp175[49]=({struct _tuple11*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->f1=({
const char*_tmp2A1="-warnboundschecks";_tag_arr(_tmp2A1,sizeof(char),
_get_zero_arr_size(_tmp2A1,18));});_tmp29C->f2=0;_tmp29C->f3=({const char*_tmp2A0="";
_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,1));});_tmp29C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));
_tmp29E[0]=({struct Cyc_Arg_Set_spec_struct _tmp29F;_tmp29F.tag=3;_tmp29F.f1=& Cyc_Toc_warn_bounds_checks;
_tmp29F;});_tmp29E;});_tmp29C->f5=({const char*_tmp29D="Warn when bounds checks can't be eliminated";
_tag_arr(_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,44));});_tmp29C;});
_tmp175[48]=({struct _tuple11*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->f1=({
const char*_tmp29B="-CI";_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,
4));});_tmp296->f2=0;_tmp296->f3=({const char*_tmp29A=" <file>";_tag_arr(_tmp29A,
sizeof(char),_get_zero_arr_size(_tmp29A,8));});_tmp296->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298[0]=({struct Cyc_Arg_String_spec_struct
_tmp299;_tmp299.tag=5;_tmp299.f1=Cyc_set_cyc_include;_tmp299;});_tmp298;});
_tmp296->f5=({const char*_tmp297="Set cyc_include.h to be <file>";_tag_arr(
_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,31));});_tmp296;});_tmp175[47]=({
struct _tuple11*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->f1=({const char*
_tmp295="-printfullevars";_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(
_tmp295,16));});_tmp290->f2=0;_tmp290->f3=({const char*_tmp294="";_tag_arr(
_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,1));});_tmp290->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292[0]=({
struct Cyc_Arg_Set_spec_struct _tmp293;_tmp293.tag=3;_tmp293.f1=& Cyc_print_full_evars_r;
_tmp293;});_tmp292;});_tmp290->f5=({const char*_tmp291="Print full information for evars (type debugging)";
_tag_arr(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,50));});_tmp290;});
_tmp175[46]=({struct _tuple11*_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A->f1=({
const char*_tmp28F="-printallkinds";_tag_arr(_tmp28F,sizeof(char),
_get_zero_arr_size(_tmp28F,15));});_tmp28A->f2=0;_tmp28A->f3=({const char*_tmp28E="";
_tag_arr(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,1));});_tmp28A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28C=_cycalloc(sizeof(*_tmp28C));
_tmp28C[0]=({struct Cyc_Arg_Set_spec_struct _tmp28D;_tmp28D.tag=3;_tmp28D.f1=& Cyc_print_all_kinds_r;
_tmp28D;});_tmp28C;});_tmp28A->f5=({const char*_tmp28B="Always print kinds of type variables";
_tag_arr(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,37));});_tmp28A;});
_tmp175[45]=({struct _tuple11*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->f1=({
const char*_tmp289="-printalltvars";_tag_arr(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,15));});_tmp284->f2=0;_tmp284->f3=({const char*_tmp288="";
_tag_arr(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,1));});_tmp284->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp286=_cycalloc(sizeof(*_tmp286));
_tmp286[0]=({struct Cyc_Arg_Set_spec_struct _tmp287;_tmp287.tag=3;_tmp287.f1=& Cyc_print_all_tvars_r;
_tmp287;});_tmp286;});_tmp284->f5=({const char*_tmp285="Print all type variables (even implicit default effects)";
_tag_arr(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,57));});_tmp284;});
_tmp175[44]=({struct _tuple11*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->f1=({
const char*_tmp283="-noexpandtypedefs";_tag_arr(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,18));});_tmp27E->f2=0;_tmp27E->f3=({const char*_tmp282="";
_tag_arr(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,1));});_tmp27E->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp280=_cycalloc(sizeof(*_tmp280));
_tmp280[0]=({struct Cyc_Arg_Set_spec_struct _tmp281;_tmp281.tag=3;_tmp281.f1=& Cyc_noexpand_r;
_tmp281;});_tmp280;});_tmp27E->f5=({const char*_tmp27F="Don't expand typedefs in pretty printing";
_tag_arr(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,41));});_tmp27E;});
_tmp175[43]=({struct _tuple11*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->f1=({
const char*_tmp27D="-noremoveunused";_tag_arr(_tmp27D,sizeof(char),
_get_zero_arr_size(_tmp27D,16));});_tmp278->f2=0;_tmp278->f3=({const char*_tmp27C="";
_tag_arr(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,1));});_tmp278->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));
_tmp27A[0]=({struct Cyc_Arg_Set_spec_struct _tmp27B;_tmp27B.tag=3;_tmp27B.f1=& Cyc_noshake_r;
_tmp27B;});_tmp27A;});_tmp278->f5=({const char*_tmp279="Don't remove externed variables that aren't used";
_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,49));});_tmp278;});
_tmp175[42]=({struct _tuple11*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->f1=({
const char*_tmp277="-nogc";_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(
_tmp277,6));});_tmp272->f2=0;_tmp272->f3=({const char*_tmp276="";_tag_arr(_tmp276,
sizeof(char),_get_zero_arr_size(_tmp276,1));});_tmp272->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Arg_Set_spec_struct
_tmp275;_tmp275.tag=3;_tmp275.f1=& Cyc_nogc_r;_tmp275;});_tmp274;});_tmp272->f5=({
const char*_tmp273="Don't link in the garbage collector";_tag_arr(_tmp273,sizeof(
char),_get_zero_arr_size(_tmp273,36));});_tmp272;});_tmp175[41]=({struct _tuple11*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->f1=({const char*_tmp271="-nocyc";
_tag_arr(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,7));});_tmp26C->f2=0;
_tmp26C->f3=({const char*_tmp270="";_tag_arr(_tmp270,sizeof(char),
_get_zero_arr_size(_tmp270,1));});_tmp26C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26F;_tmp26F.tag=0;_tmp26F.f1=Cyc_set_nocyc;_tmp26F;});_tmp26E;});_tmp26C->f5=({
const char*_tmp26D="Don't add implicit namespace Cyc";_tag_arr(_tmp26D,sizeof(
char),_get_zero_arr_size(_tmp26D,33));});_tmp26C;});_tmp175[40]=({struct _tuple11*
_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->f1=({const char*_tmp26B="-no-cpp-precomp";
_tag_arr(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,16));});_tmp266->f2=0;
_tmp266->f3=({const char*_tmp26A="";_tag_arr(_tmp26A,sizeof(char),
_get_zero_arr_size(_tmp26A,1));});_tmp266->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp269;_tmp269.tag=0;_tmp269.f1=Cyc_set_nocppprecomp;_tmp269;});_tmp268;});
_tmp266->f5=({const char*_tmp267="Don't do smart preprocessing (mac only)";
_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,40));});_tmp266;});
_tmp175[39]=({struct _tuple11*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->f1=({
const char*_tmp265="-use-cpp";_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(
_tmp265,9));});_tmp260->f2=0;_tmp260->f3=({const char*_tmp264="<path>";_tag_arr(
_tmp264,sizeof(char),_get_zero_arr_size(_tmp264,7));});_tmp260->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262[0]=({
struct Cyc_Arg_String_spec_struct _tmp263;_tmp263.tag=5;_tmp263.f1=Cyc_set_cpp;
_tmp263;});_tmp262;});_tmp260->f5=({const char*_tmp261="Indicate which preprocessor to use";
_tag_arr(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,35));});_tmp260;});
_tmp175[38]=({struct _tuple11*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->f1=({
const char*_tmp25F="--noboundschecks";_tag_arr(_tmp25F,sizeof(char),
_get_zero_arr_size(_tmp25F,17));});_tmp25A->f2=0;_tmp25A->f3=({const char*_tmp25E="";
_tag_arr(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,1));});_tmp25A->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));
_tmp25C[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=Cyc_set_noboundschecks;
_tmp25D;});_tmp25C;});_tmp25A->f5=({const char*_tmp25B="Disable bounds checks";
_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,22));});_tmp25A;});
_tmp175[37]=({struct _tuple11*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->f1=({
const char*_tmp259="--nonullchecks";_tag_arr(_tmp259,sizeof(char),
_get_zero_arr_size(_tmp259,15));});_tmp254->f2=0;_tmp254->f3=({const char*_tmp258="";
_tag_arr(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,1));});_tmp254->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp256=_cycalloc(sizeof(*_tmp256));
_tmp256[0]=({struct Cyc_Arg_Unit_spec_struct _tmp257;_tmp257.tag=0;_tmp257.f1=Cyc_set_nonullchecks;
_tmp257;});_tmp256;});_tmp254->f5=({const char*_tmp255="Disable null checks";
_tag_arr(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,20));});_tmp254;});
_tmp175[36]=({struct _tuple11*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->f1=({
const char*_tmp253="--nochecks";_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(
_tmp253,11));});_tmp24E->f2=0;_tmp24E->f3=({const char*_tmp252="";_tag_arr(
_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,1));});_tmp24E->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp251;_tmp251.tag=0;_tmp251.f1=Cyc_set_nochecks;
_tmp251;});_tmp250;});_tmp24E->f5=({const char*_tmp24F="Disable bounds/null checks";
_tag_arr(_tmp24F,sizeof(char),_get_zero_arr_size(_tmp24F,27));});_tmp24E;});
_tmp175[35]=({struct _tuple11*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->f1=({
const char*_tmp24D="--lineno";_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(
_tmp24D,9));});_tmp248->f2=0;_tmp248->f3=({const char*_tmp24C="";_tag_arr(_tmp24C,
sizeof(char),_get_zero_arr_size(_tmp24C,1));});_tmp248->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp24B;_tmp24B.tag=0;_tmp24B.f1=Cyc_set_lineno;_tmp24B;});_tmp24A;});_tmp248->f5=({
const char*_tmp249="Generate line numbers for debugging";_tag_arr(_tmp249,sizeof(
char),_get_zero_arr_size(_tmp249,36));});_tmp248;});_tmp175[34]=({struct _tuple11*
_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->f1=({const char*_tmp247="-save-c";
_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,8));});_tmp242->f2=0;
_tmp242->f3=({const char*_tmp246="";_tag_arr(_tmp246,sizeof(char),
_get_zero_arr_size(_tmp246,1));});_tmp242->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Arg_Set_spec_struct
_tmp245;_tmp245.tag=3;_tmp245.f1=& Cyc_save_c_r;_tmp245;});_tmp244;});_tmp242->f5=({
const char*_tmp243="Don't delete temporary C files";_tag_arr(_tmp243,sizeof(char),
_get_zero_arr_size(_tmp243,31));});_tmp242;});_tmp175[33]=({struct _tuple11*
_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->f1=({const char*_tmp241="-save-temps";
_tag_arr(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,12));});_tmp23C->f2=0;
_tmp23C->f3=({const char*_tmp240="";_tag_arr(_tmp240,sizeof(char),
_get_zero_arr_size(_tmp240,1));});_tmp23C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp23F;_tmp23F.tag=0;_tmp23F.f1=Cyc_set_save_temps;_tmp23F;});_tmp23E;});
_tmp23C->f5=({const char*_tmp23D="Don't delete temporary files";_tag_arr(_tmp23D,
sizeof(char),_get_zero_arr_size(_tmp23D,29));});_tmp23C;});_tmp175[32]=({struct
_tuple11*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->f1=({const char*_tmp23B="-tovc";
_tag_arr(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,6));});_tmp236->f2=0;
_tmp236->f3=({const char*_tmp23A="";_tag_arr(_tmp23A,sizeof(char),
_get_zero_arr_size(_tmp23A,1));});_tmp236->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Arg_Set_spec_struct
_tmp239;_tmp239.tag=3;_tmp239.f1=& Cyc_tovc_r;_tmp239;});_tmp238;});_tmp236->f5=({
const char*_tmp237="Avoid gcc extensions in C output";_tag_arr(_tmp237,sizeof(
char),_get_zero_arr_size(_tmp237,33));});_tmp236;});_tmp175[31]=({struct _tuple11*
_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->f1=({const char*_tmp235="-up";
_tag_arr(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,4));});_tmp230->f2=0;
_tmp230->f3=({const char*_tmp234="";_tag_arr(_tmp234,sizeof(char),
_get_zero_arr_size(_tmp234,1));});_tmp230->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp233;_tmp233.tag=4;_tmp233.f1=& Cyc_pp_r;_tmp233;});_tmp232;});_tmp230->f5=({
const char*_tmp231="Ugly print";_tag_arr(_tmp231,sizeof(char),_get_zero_arr_size(
_tmp231,11));});_tmp230;});_tmp175[30]=({struct _tuple11*_tmp22A=_cycalloc(
sizeof(*_tmp22A));_tmp22A->f1=({const char*_tmp22F="-pp";_tag_arr(_tmp22F,sizeof(
char),_get_zero_arr_size(_tmp22F,4));});_tmp22A->f2=0;_tmp22A->f3=({const char*
_tmp22E="";_tag_arr(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,1));});
_tmp22A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22C=_cycalloc(sizeof(*
_tmp22C));_tmp22C[0]=({struct Cyc_Arg_Set_spec_struct _tmp22D;_tmp22D.tag=3;
_tmp22D.f1=& Cyc_pp_r;_tmp22D;});_tmp22C;});_tmp22A->f5=({const char*_tmp22B="Pretty print";
_tag_arr(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,13));});_tmp22A;});
_tmp175[29]=({struct _tuple11*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->f1=({
const char*_tmp229="-ic";_tag_arr(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,
4));});_tmp224->f2=0;_tmp224->f3=({const char*_tmp228="";_tag_arr(_tmp228,sizeof(
char),_get_zero_arr_size(_tmp228,1));});_tmp224->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226[0]=({struct Cyc_Arg_Set_spec_struct
_tmp227;_tmp227.tag=3;_tmp227.f1=& Cyc_ic_r;_tmp227;});_tmp226;});_tmp224->f5=({
const char*_tmp225="Activate the link-checker";_tag_arr(_tmp225,sizeof(char),
_get_zero_arr_size(_tmp225,26));});_tmp224;});_tmp175[28]=({struct _tuple11*
_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E->f1=({const char*_tmp223="-stopafter-toc";
_tag_arr(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,15));});_tmp21E->f2=0;
_tmp21E->f3=({const char*_tmp222="";_tag_arr(_tmp222,sizeof(char),
_get_zero_arr_size(_tmp222,1));});_tmp21E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Arg_Set_spec_struct
_tmp221;_tmp221.tag=3;_tmp221.f1=& Cyc_toc_r;_tmp221;});_tmp220;});_tmp21E->f5=({
const char*_tmp21F="Stop after translation to C";_tag_arr(_tmp21F,sizeof(char),
_get_zero_arr_size(_tmp21F,28));});_tmp21E;});_tmp175[27]=({struct _tuple11*
_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218->f1=({const char*_tmp21D="-stopafter-tc";
_tag_arr(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,14));});_tmp218->f2=0;
_tmp218->f3=({const char*_tmp21C="";_tag_arr(_tmp21C,sizeof(char),
_get_zero_arr_size(_tmp21C,1));});_tmp218->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Arg_Set_spec_struct
_tmp21B;_tmp21B.tag=3;_tmp21B.f1=& Cyc_tc_r;_tmp21B;});_tmp21A;});_tmp218->f5=({
const char*_tmp219="Stop after type checking";_tag_arr(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,25));});_tmp218;});_tmp175[26]=({struct _tuple11*
_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->f1=({const char*_tmp217="-stopafter-parse";
_tag_arr(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,17));});_tmp212->f2=0;
_tmp212->f3=({const char*_tmp216="";_tag_arr(_tmp216,sizeof(char),
_get_zero_arr_size(_tmp216,1));});_tmp212->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214[0]=({struct Cyc_Arg_Set_spec_struct
_tmp215;_tmp215.tag=3;_tmp215.f1=& Cyc_parseonly_r;_tmp215;});_tmp214;});_tmp212->f5=({
const char*_tmp213="Stop after parsing";_tag_arr(_tmp213,sizeof(char),
_get_zero_arr_size(_tmp213,19));});_tmp212;});_tmp175[25]=({struct _tuple11*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->f1=({const char*_tmp211="-E";
_tag_arr(_tmp211,sizeof(char),_get_zero_arr_size(_tmp211,3));});_tmp20C->f2=0;
_tmp20C->f3=({const char*_tmp210="";_tag_arr(_tmp210,sizeof(char),
_get_zero_arr_size(_tmp210,1));});_tmp20C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E[0]=({struct Cyc_Arg_Set_spec_struct
_tmp20F;_tmp20F.tag=3;_tmp20F.f1=& Cyc_stop_after_cpp_r;_tmp20F;});_tmp20E;});
_tmp20C->f5=({const char*_tmp20D="Stop after preprocessing";_tag_arr(_tmp20D,
sizeof(char),_get_zero_arr_size(_tmp20D,25));});_tmp20C;});_tmp175[24]=({struct
_tuple11*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->f1=({const char*_tmp20B="-b";
_tag_arr(_tmp20B,sizeof(char),_get_zero_arr_size(_tmp20B,3));});_tmp206->f2=0;
_tmp206->f3=({const char*_tmp20A="<arch>";_tag_arr(_tmp20A,sizeof(char),
_get_zero_arr_size(_tmp20A,7));});_tmp206->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Arg_String_spec_struct
_tmp209;_tmp209.tag=5;_tmp209.f1=Cyc_set_target_arch;_tmp209;});_tmp208;});
_tmp206->f5=({const char*_tmp207="Set target architecture; implies -stopafter-toc";
_tag_arr(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,48));});_tmp206;});
_tmp175[23]=({struct _tuple11*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->f1=({
const char*_tmp205="-MT";_tag_arr(_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,
4));});_tmp200->f2=0;_tmp200->f3=({const char*_tmp204=" <target>";_tag_arr(
_tmp204,sizeof(char),_get_zero_arr_size(_tmp204,10));});_tmp200->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({
struct Cyc_Arg_String_spec_struct _tmp203;_tmp203.tag=5;_tmp203.f1=Cyc_set_dependencies_target;
_tmp203;});_tmp202;});_tmp200->f5=({const char*_tmp201="Give target for dependencies";
_tag_arr(_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,29));});_tmp200;});
_tmp175[22]=({struct _tuple11*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->f1=({
const char*_tmp1FF="-MG";_tag_arr(_tmp1FF,sizeof(char),_get_zero_arr_size(_tmp1FF,
4));});_tmp1FA->f2=0;_tmp1FA->f3=({const char*_tmp1FE="";_tag_arr(_tmp1FE,sizeof(
char),_get_zero_arr_size(_tmp1FE,1));});_tmp1FA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1FD;_tmp1FD.tag=1;_tmp1FD.f1=Cyc_add_cpparg;_tmp1FD;});_tmp1FC;});_tmp1FA->f5=({
const char*_tmp1FB="When producing dependencies assume that missing files are generated";
_tag_arr(_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,68));});_tmp1FA;});
_tmp175[21]=({struct _tuple11*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->f1=({
const char*_tmp1F9="-M";_tag_arr(_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,
3));});_tmp1F4->f2=0;_tmp1F4->f3=({const char*_tmp1F8="";_tag_arr(_tmp1F8,sizeof(
char),_get_zero_arr_size(_tmp1F8,1));});_tmp1F4->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=Cyc_set_produce_dependencies;_tmp1F7;});_tmp1F6;});
_tmp1F4->f5=({const char*_tmp1F5="Produce dependencies";_tag_arr(_tmp1F5,sizeof(
char),_get_zero_arr_size(_tmp1F5,21));});_tmp1F4;});_tmp175[20]=({struct _tuple11*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->f1=({const char*_tmp1F3="-S";
_tag_arr(_tmp1F3,sizeof(char),_get_zero_arr_size(_tmp1F3,3));});_tmp1EE->f2=0;
_tmp1EE->f3=({const char*_tmp1F2="";_tag_arr(_tmp1F2,sizeof(char),
_get_zero_arr_size(_tmp1F2,1));});_tmp1EE->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=Cyc_set_stop_after_asmfile;_tmp1F1;});_tmp1F0;});
_tmp1EE->f5=({const char*_tmp1EF="Stop after producing assembly code";_tag_arr(
_tmp1EF,sizeof(char),_get_zero_arr_size(_tmp1EF,35));});_tmp1EE;});_tmp175[19]=({
struct _tuple11*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->f1=({const char*
_tmp1ED="-pa";_tag_arr(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,4));});
_tmp1E8->f2=0;_tmp1E8->f3=({const char*_tmp1EC="";_tag_arr(_tmp1EC,sizeof(char),
_get_zero_arr_size(_tmp1EC,1));});_tmp1E8->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1EB;_tmp1EB.tag=0;_tmp1EB.f1=Cyc_set_pa;_tmp1EB;});_tmp1EA;});_tmp1E8->f5=({
const char*_tmp1E9="Compile for profiling with aprof";_tag_arr(_tmp1E9,sizeof(
char),_get_zero_arr_size(_tmp1E9,33));});_tmp1E8;});_tmp175[18]=({struct _tuple11*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->f1=({const char*_tmp1E7="-pg";
_tag_arr(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,4));});_tmp1E2->f2=0;
_tmp1E2->f3=({const char*_tmp1E6="";_tag_arr(_tmp1E6,sizeof(char),
_get_zero_arr_size(_tmp1E6,1));});_tmp1E2->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=Cyc_set_pg;_tmp1E5;});_tmp1E4;});_tmp1E2->f5=({
const char*_tmp1E3="Compile for profiling with gprof";_tag_arr(_tmp1E3,sizeof(
char),_get_zero_arr_size(_tmp1E3,33));});_tmp1E2;});_tmp175[17]=({struct _tuple11*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->f1=({const char*_tmp1E1="-p";
_tag_arr(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,3));});_tmp1DC->f2=0;
_tmp1DC->f3=({const char*_tmp1E0="";_tag_arr(_tmp1E0,sizeof(char),
_get_zero_arr_size(_tmp1E0,1));});_tmp1DC->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1DF;_tmp1DF.tag=1;_tmp1DF.f1=Cyc_add_ccarg;_tmp1DF;});_tmp1DE;});_tmp1DC->f5=({
const char*_tmp1DD="Compile for profiling with prof";_tag_arr(_tmp1DD,sizeof(char),
_get_zero_arr_size(_tmp1DD,32));});_tmp1DC;});_tmp175[16]=({struct _tuple11*
_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->f1=({const char*_tmp1DB="-g";
_tag_arr(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,3));});_tmp1D6->f2=0;
_tmp1D6->f3=({const char*_tmp1DA="";_tag_arr(_tmp1DA,sizeof(char),
_get_zero_arr_size(_tmp1DA,1));});_tmp1D6->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D9;_tmp1D9.tag=1;_tmp1D9.f1=Cyc_add_ccarg;_tmp1D9;});_tmp1D8;});_tmp1D6->f5=({
const char*_tmp1D7="Compile for debugging";_tag_arr(_tmp1D7,sizeof(char),
_get_zero_arr_size(_tmp1D7,22));});_tmp1D6;});_tmp175[15]=({struct _tuple11*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->f1=({const char*_tmp1D5="-fomit-frame-pointer";
_tag_arr(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,21));});_tmp1D0->f2=0;
_tmp1D0->f3=({const char*_tmp1D4="";_tag_arr(_tmp1D4,sizeof(char),
_get_zero_arr_size(_tmp1D4,1));});_tmp1D0->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D3;_tmp1D3.tag=1;_tmp1D3.f1=Cyc_add_ccarg;_tmp1D3;});_tmp1D2;});_tmp1D0->f5=({
const char*_tmp1D1="Omit frame pointer";_tag_arr(_tmp1D1,sizeof(char),
_get_zero_arr_size(_tmp1D1,19));});_tmp1D0;});_tmp175[14]=({struct _tuple11*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->f1=({const char*_tmp1CF="-O3";
_tag_arr(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,4));});_tmp1CA->f2=0;
_tmp1CA->f3=({const char*_tmp1CE="";_tag_arr(_tmp1CE,sizeof(char),
_get_zero_arr_size(_tmp1CE,1));});_tmp1CA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=Cyc_add_ccarg;_tmp1CD;});_tmp1CC;});_tmp1CA->f5=({
const char*_tmp1CB="Even more optimization";_tag_arr(_tmp1CB,sizeof(char),
_get_zero_arr_size(_tmp1CB,23));});_tmp1CA;});_tmp175[13]=({struct _tuple11*
_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->f1=({const char*_tmp1C9="-O2";
_tag_arr(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,4));});_tmp1C4->f2=0;
_tmp1C4->f3=({const char*_tmp1C8="";_tag_arr(_tmp1C8,sizeof(char),
_get_zero_arr_size(_tmp1C8,1));});_tmp1C4->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=Cyc_add_ccarg;_tmp1C7;});_tmp1C6;});_tmp1C4->f5=({
const char*_tmp1C5="A higher level of optimization";_tag_arr(_tmp1C5,sizeof(char),
_get_zero_arr_size(_tmp1C5,31));});_tmp1C4;});_tmp175[12]=({struct _tuple11*
_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->f1=({const char*_tmp1C3="-O";
_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,3));});_tmp1BE->f2=0;
_tmp1BE->f3=({const char*_tmp1C2="";_tag_arr(_tmp1C2,sizeof(char),
_get_zero_arr_size(_tmp1C2,1));});_tmp1BE->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C1;_tmp1C1.tag=1;_tmp1C1.f1=Cyc_add_ccarg;_tmp1C1;});_tmp1C0;});_tmp1BE->f5=({
const char*_tmp1BF="Optimize";_tag_arr(_tmp1BF,sizeof(char),_get_zero_arr_size(
_tmp1BF,9));});_tmp1BE;});_tmp175[11]=({struct _tuple11*_tmp1B8=_cycalloc(sizeof(*
_tmp1B8));_tmp1B8->f1=({const char*_tmp1BD="-O0";_tag_arr(_tmp1BD,sizeof(char),
_get_zero_arr_size(_tmp1BD,4));});_tmp1B8->f2=0;_tmp1B8->f3=({const char*_tmp1BC="";
_tag_arr(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,1));});_tmp1B8->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));
_tmp1BA[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=Cyc_add_ccarg;
_tmp1BB;});_tmp1BA;});_tmp1B8->f5=({const char*_tmp1B9="Don't optimize";_tag_arr(
_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,15));});_tmp1B8;});_tmp175[10]=({
struct _tuple11*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->f1=({const char*
_tmp1B7="-s";_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,3));});
_tmp1B2->f2=0;_tmp1B2->f3=({const char*_tmp1B6="";_tag_arr(_tmp1B6,sizeof(char),
_get_zero_arr_size(_tmp1B6,1));});_tmp1B2->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1B5;_tmp1B5.tag=1;_tmp1B5.f1=Cyc_add_ccarg;_tmp1B5;});_tmp1B4;});_tmp1B2->f5=({
const char*_tmp1B3="Remove all symbol table and relocation info from executable";
_tag_arr(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,60));});_tmp1B2;});
_tmp175[9]=({struct _tuple11*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->f1=({
const char*_tmp1B1="-x";_tag_arr(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,
3));});_tmp1AC->f2=0;_tmp1AC->f3=({const char*_tmp1B0=" <language>";_tag_arr(
_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,12));});_tmp1AC->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE[0]=({
struct Cyc_Arg_String_spec_struct _tmp1AF;_tmp1AF.tag=5;_tmp1AF.f1=Cyc_set_inputtype;
_tmp1AF;});_tmp1AE;});_tmp1AC->f5=({const char*_tmp1AD="Specify <language> for the following input files";
_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,49));});_tmp1AC;});
_tmp175[8]=({struct _tuple11*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->f1=({
const char*_tmp1AB="-c";_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,
3));});_tmp1A6->f2=0;_tmp1A6->f3=({const char*_tmp1AA="";_tag_arr(_tmp1AA,sizeof(
char),_get_zero_arr_size(_tmp1AA,1));});_tmp1A6->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A9;_tmp1A9.tag=0;_tmp1A9.f1=Cyc_set_stop_after_objectfile;_tmp1A9;});
_tmp1A8;});_tmp1A6->f5=({const char*_tmp1A7="Produce an object file instead of an executable; do not link";
_tag_arr(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,61));});_tmp1A6;});
_tmp175[7]=({struct _tuple11*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->f1=({
const char*_tmp1A5="-l";_tag_arr(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,
3));});_tmp1A0->f2=1;_tmp1A0->f3=({const char*_tmp1A4="<file>";_tag_arr(_tmp1A4,
sizeof(char),_get_zero_arr_size(_tmp1A4,7));});_tmp1A0->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A3;_tmp1A3.tag=1;_tmp1A3.f1=Cyc_add_libarg;_tmp1A3;});_tmp1A2;});_tmp1A0->f5=({
const char*_tmp1A1="Library file";_tag_arr(_tmp1A1,sizeof(char),
_get_zero_arr_size(_tmp1A1,13));});_tmp1A0;});_tmp175[6]=({struct _tuple11*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->f1=({const char*_tmp19F="-L";
_tag_arr(_tmp19F,sizeof(char),_get_zero_arr_size(_tmp19F,3));});_tmp19A->f2=1;
_tmp19A->f3=({const char*_tmp19E="<dir>";_tag_arr(_tmp19E,sizeof(char),
_get_zero_arr_size(_tmp19E,6));});_tmp19A->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp19D;_tmp19D.tag=1;_tmp19D.f1=Cyc_add_ccarg;_tmp19D;});_tmp19C;});_tmp19A->f5=({
const char*_tmp19B="Add to the list of directories for -l";_tag_arr(_tmp19B,
sizeof(char),_get_zero_arr_size(_tmp19B,38));});_tmp19A;});_tmp175[5]=({struct
_tuple11*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->f1=({const char*_tmp199="-I";
_tag_arr(_tmp199,sizeof(char),_get_zero_arr_size(_tmp199,3));});_tmp194->f2=1;
_tmp194->f3=({const char*_tmp198="<dir>";_tag_arr(_tmp198,sizeof(char),
_get_zero_arr_size(_tmp198,6));});_tmp194->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp197;_tmp197.tag=1;_tmp197.f1=Cyc_add_cpparg;_tmp197;});_tmp196;});_tmp194->f5=({
const char*_tmp195="Add to the list of directories to search for include files";
_tag_arr(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,59));});_tmp194;});
_tmp175[4]=({struct _tuple11*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->f1=({
const char*_tmp193="-B";_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,
3));});_tmp18E->f2=1;_tmp18E->f3=({const char*_tmp192="<file>";_tag_arr(_tmp192,
sizeof(char),_get_zero_arr_size(_tmp192,7));});_tmp18E->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp191;_tmp191.tag=1;_tmp191.f1=Cyc_add_cyclone_exec_path;_tmp191;});_tmp190;});
_tmp18E->f5=({const char*_tmp18F="Add to the list of directories to search for compiler files";
_tag_arr(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,60));});_tmp18E;});
_tmp175[3]=({struct _tuple11*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->f1=({
const char*_tmp18D="-D";_tag_arr(_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,
3));});_tmp188->f2=1;_tmp188->f3=({const char*_tmp18C="<name>[=<value>]";_tag_arr(
_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,17));});_tmp188->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp18B;_tmp18B.tag=1;_tmp18B.f1=Cyc_add_cpparg;
_tmp18B;});_tmp18A;});_tmp188->f5=({const char*_tmp189="Pass definition to preprocessor";
_tag_arr(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,32));});_tmp188;});
_tmp175[2]=({struct _tuple11*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->f1=({
const char*_tmp187="-o";_tag_arr(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,
3));});_tmp182->f2=0;_tmp182->f3=({const char*_tmp186=" <file>";_tag_arr(_tmp186,
sizeof(char),_get_zero_arr_size(_tmp186,8));});_tmp182->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Arg_String_spec_struct
_tmp185;_tmp185.tag=5;_tmp185.f1=Cyc_set_output_file;_tmp185;});_tmp184;});
_tmp182->f5=({const char*_tmp183="Set the output file name to <file>";_tag_arr(
_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,35));});_tmp182;});_tmp175[1]=({
struct _tuple11*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=({const char*
_tmp181="--version";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,10));});
_tmp17C->f2=0;_tmp17C->f3=({const char*_tmp180="";_tag_arr(_tmp180,sizeof(char),
_get_zero_arr_size(_tmp180,1));});_tmp17C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp17F;_tmp17F.tag=0;_tmp17F.f1=Cyc_print_version;_tmp17F;});_tmp17E;});_tmp17C->f5=({
const char*_tmp17D="Print version information and exit";_tag_arr(_tmp17D,sizeof(
char),_get_zero_arr_size(_tmp17D,35));});_tmp17C;});_tmp175[0]=({struct _tuple11*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->f1=({const char*_tmp17B="-v";
_tag_arr(_tmp17B,sizeof(char),_get_zero_arr_size(_tmp17B,3));});_tmp176->f2=0;
_tmp176->f3=({const char*_tmp17A="";_tag_arr(_tmp17A,sizeof(char),
_get_zero_arr_size(_tmp17A,1));});_tmp176->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Arg_Set_spec_struct
_tmp179;_tmp179.tag=3;_tmp179.f1=& Cyc_v_r;_tmp179;});_tmp178;});_tmp176->f5=({
const char*_tmp177="Print compilation stages verbosely";_tag_arr(_tmp177,sizeof(
char),_get_zero_arr_size(_tmp177,35));});_tmp176;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp175,sizeof(struct _tuple11*),51));});
Cyc_Arg_parse(options,Cyc_add_other,({const char*_tmp110="Options:";_tag_arr(
_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,9));}),argv);if(Cyc_cyclone_files
== 0?Cyc_ccargs == 0: 0){({void*_tmp111[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp112="missing file\n";_tag_arr(_tmp112,sizeof(char),_get_zero_arr_size(
_tmp112,14));}),_tag_arr(_tmp111,sizeof(void*),0));});exit(1);}if(Cyc_target_arch.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_target_arch=(struct _tagged_arr)
Cstring_to_string(Carch);{struct _tagged_arr cyclone_exec_prefix=({char*_tmp174=
getenv((const char*)"CYCLONE_EXEC_PREFIX");_tag_arr(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,1));});if(cyclone_exec_prefix.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=({struct _tagged_arr*_tmp114=
_cycalloc(sizeof(*_tmp114));_tmp114[0]=cyclone_exec_prefix;_tmp114;});_tmp113->tl=
Cyc_cyclone_exec_path;_tmp113;});{struct _tagged_arr def_lib_path=(struct
_tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct _tagged_arr)
def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp115=_cycalloc(
sizeof(*_tmp115));_tmp115->hd=({struct _tagged_arr*_tmp116=_cycalloc(sizeof(*
_tmp116));_tmp116[0]=(struct _tagged_arr)Cyc_Filename_concat(def_lib_path,({const
char*_tmp117="cyc-lib";_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,
8));}));_tmp116;});_tmp115->tl=Cyc_cyclone_exec_path;_tmp115;});Cyc_cyclone_exec_path=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{
struct Cyc_List_List*_tmp118=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
if(!Cyc_stop_after_cpp_r?Cyc_strlen((struct _tagged_arr)Cyc_cyc_include)== 0: 0)
Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({const char*_tmp119="cyc_include.h";
_tag_arr(_tmp119,sizeof(char),_get_zero_arr_size(_tmp119,14));}));{struct Cyc_List_List*
_tmp11A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp11A != 0;_tmp11A=_tmp11A->tl){Cyc_process_file(*((struct _tagged_arr*)((
struct _tagged_arr*)_tmp11A->hd)));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r?
1: Cyc_parseonly_r)?1: Cyc_tc_r)?1: Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _tagged_arr)Cyc_strconcat(({const char*_tmp11B="-L";_tag_arr(_tmp11B,
sizeof(char),_get_zero_arr_size(_tmp11B,3));}),(struct _tagged_arr)def_lib_path));
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _tagged_arr _tmp11C=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_ccargs),({const char*_tmp173=" ";_tag_arr(_tmp173,sizeof(char),
_get_zero_arr_size(_tmp173,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmp11D=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp170->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp170;}),({const char*
_tmp172=" ";_tag_arr(_tmp172,sizeof(char),_get_zero_arr_size(_tmp172,2));}));
struct Cyc_List_List*stdlib;struct _tagged_arr stdlib_string;int _tmp11E=((Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmp16E=".a";
_tag_arr(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,3));})): 0))?1:(Cyc_output_file
!= 0?Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmp16F=".lib";_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(
_tmp16F,5));})): 0);if(_tmp11E){stdlib=0;stdlib_string=({const char*_tmp11F="";
_tag_arr(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,1));});}else{struct
_tagged_arr libcyc_flag;struct _tagged_arr nogc_filename;struct _tagged_arr
runtime_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){libcyc_flag=({const
char*_tmp120="-lcyc_a";_tag_arr(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,
8));});nogc_filename=({const char*_tmp121="nogc_a.a";_tag_arr(_tmp121,sizeof(char),
_get_zero_arr_size(_tmp121,9));});runtime_filename=({const char*_tmp122="runtime_cyc_a.o";
_tag_arr(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp123="-lcyc_nocheck";_tag_arr(_tmp123,sizeof(char),
_get_zero_arr_size(_tmp123,14));});nogc_filename=({const char*_tmp124="nogc_nocheck.a";
_tag_arr(_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,15));});
runtime_filename=({const char*_tmp125="runtime_cyc.o";_tag_arr(_tmp125,sizeof(
char),_get_zero_arr_size(_tmp125,14));});}else{if(Cyc_pg_r){libcyc_flag=({const
char*_tmp126="-lcyc_pg";_tag_arr(_tmp126,sizeof(char),_get_zero_arr_size(_tmp126,
9));});runtime_filename=({const char*_tmp127="runtime_cyc_pg.o";_tag_arr(_tmp127,
sizeof(char),_get_zero_arr_size(_tmp127,17));});nogc_filename=({const char*
_tmp128="nogc_pg.a";_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,10));});}
else{libcyc_flag=({const char*_tmp129="-lcyc";_tag_arr(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,6));});nogc_filename=({const char*_tmp12A="nogc.a";
_tag_arr(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,7));});runtime_filename=({
const char*_tmp12B="runtime_cyc.o";_tag_arr(_tmp12B,sizeof(char),
_get_zero_arr_size(_tmp12B,14));});}}}gc_filename=({const char*_tmp12C="gc.a";
_tag_arr(_tmp12C,sizeof(char),_get_zero_arr_size(_tmp12C,5));});{struct
_tagged_arr _tmp12D=Cyc_nogc_r?Cyc_do_find(_tmp118,nogc_filename): Cyc_do_find(
_tmp118,gc_filename);struct _tagged_arr _tmp12E=Cyc_do_find(_tmp118,
runtime_filename);stdlib=0;stdlib_string=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp133;_tmp133.tag=0;_tmp133.f1=(struct _tagged_arr)_tmp12D;{struct Cyc_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)libcyc_flag;{struct Cyc_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)_tmp12E;{void*_tmp12F[3]={&
_tmp131,& _tmp132,& _tmp133};Cyc_aprintf(({const char*_tmp130=" %s %s %s";_tag_arr(
_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,10));}),_tag_arr(_tmp12F,sizeof(
void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp134;_push_handler(&
_tmp134);{int _tmp136=0;if(setjmp(_tmp134.handler))_tmp136=1;if(!_tmp136){Cyc_ccargs=((
struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct
_tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp11E)_tmp137=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));
_tmp138->hd=Cyc_final_strptr;_tmp138->tl=_tmp137;_tmp138;});{struct Cyc_Interface_I*
_tmp139=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_tagged_arr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp137,_tmp137);if(_tmp139 == 0){({void*_tmp13A[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13B="Error: interfaces incompatible\n";_tag_arr(
_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,32));}),_tag_arr(_tmp13A,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13C=1;
_npop_handler(0);return _tmp13C;}}if(_tmp11E){if(Cyc_output_file != 0){struct
_tagged_arr _tmp13D=({struct Cyc_String_pa_struct _tmp144;_tmp144.tag=0;_tmp144.f1=(
struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)_check_null(
Cyc_output_file)));{void*_tmp142[1]={& _tmp144};Cyc_aprintf(({const char*_tmp143="%s.cycio";
_tag_arr(_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,9));}),_tag_arr(_tmp142,
sizeof(void*),1));}});struct Cyc___cycFILE*_tmp13E=Cyc_try_file_open((struct
_tagged_arr)_tmp13D,({const char*_tmp140="wb";_tag_arr(_tmp140,sizeof(char),
_get_zero_arr_size(_tmp140,3));}),({const char*_tmp141="interface object file";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,22));}));if(_tmp13E == 0){
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13F=1;_npop_handler(0);return
_tmp13F;}}Cyc_Interface_save((struct Cyc_Interface_I*)_check_null(_tmp139),(
struct Cyc___cycFILE*)_check_null(_tmp13E));Cyc_file_close((struct Cyc___cycFILE*)
_check_null(_tmp13E));}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(
struct Cyc_Interface_I*)_check_null(_tmp139),({struct _tuple9*_tmp145=_cycalloc(
sizeof(*_tmp145));_tmp145->f1=({const char*_tmp147="empty interface";_tag_arr(
_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,16));});_tmp145->f2=({const char*
_tmp146="global interface";_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(
_tmp146,17));});_tmp145;}))){({void*_tmp148[0]={};Cyc_fprintf(Cyc_stderr,({const
char*_tmp149="Error: some objects are still undefined\n";_tag_arr(_tmp149,
sizeof(char),_get_zero_arr_size(_tmp149,41));}),_tag_arr(_tmp148,sizeof(void*),0));});
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp14A=1;_npop_handler(0);return
_tmp14A;}}}}};_pop_handler();}else{void*_tmp135=(void*)_exn_thrown;void*_tmp14C=
_tmp135;struct _tagged_arr _tmp14D;struct _tagged_arr _tmp14E;struct _tagged_arr
_tmp14F;_LL36: if(*((void**)_tmp14C)!= Cyc_Core_Failure)goto _LL38;_tmp14D=((
struct Cyc_Core_Failure_struct*)_tmp14C)->f1;_LL37:({struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)_tmp14D;{void*_tmp150[1]={&
_tmp152};Cyc_printf(({const char*_tmp151="Exception Core::Failure %s\n";_tag_arr(
_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,28));}),_tag_arr(_tmp150,sizeof(
void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;_LL38: if(*((
void**)_tmp14C)!= Cyc_Core_Impossible)goto _LL3A;_tmp14E=((struct Cyc_Core_Impossible_struct*)
_tmp14C)->f1;_LL39:({struct Cyc_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(
struct _tagged_arr)_tmp14E;{void*_tmp153[1]={& _tmp155};Cyc_printf(({const char*
_tmp154="Exception Core::Impossible %s\n";_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,31));}),_tag_arr(_tmp153,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3A: if(_tmp14C != Cyc_Dict_Absent)goto _LL3C;_LL3B:({
void*_tmp156[0]={};Cyc_printf(({const char*_tmp157="Exception Dict::Absent\n";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,24));}),_tag_arr(
_tmp156,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3C: if(*((void**)_tmp14C)!= Cyc_Core_Invalid_argument)goto _LL3E;_tmp14F=((
struct Cyc_Core_Invalid_argument_struct*)_tmp14C)->f1;_LL3D:({struct Cyc_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)_tmp14F;{void*_tmp158[1]={&
_tmp15A};Cyc_printf(({const char*_tmp159="Exception Core::Invalid_argument %s\n";
_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,37));}),_tag_arr(
_tmp158,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3E:;_LL3F:({void*_tmp15B[0]={};Cyc_printf(({const char*_tmp15C="Uncaught exception\n";
_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,20));}),_tag_arr(
_tmp15B,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL40:;_LL41:(void)_throw(_tmp14C);_LL35:;}}}{struct _tagged_arr _tmp15D=({struct
Cyc_String_pa_struct _tmp169;_tmp169.tag=0;_tmp169.f1=(struct _tagged_arr)_tmp11D;{
struct Cyc_String_pa_struct _tmp168;_tmp168.tag=0;_tmp168.f1=(struct _tagged_arr)
stdlib_string;{struct Cyc_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(
struct _tagged_arr)_tmp11C;{struct Cyc_String_pa_struct _tmp166;_tmp166.tag=0;
_tmp166.f1=(struct _tagged_arr)(Cyc_output_file == 0?({const char*_tmp16A="";
_tag_arr(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,1));}):(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=(struct
_tagged_arr)Cyc_sh_escape_string(*((struct _tagged_arr*)_check_null(Cyc_output_file)));{
void*_tmp16B[1]={& _tmp16D};Cyc_aprintf(({const char*_tmp16C=" -o %s";_tag_arr(
_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,7));}),_tag_arr(_tmp16B,sizeof(
void*),1));}}));{struct Cyc_String_pa_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(
struct _tagged_arr)comp;{void*_tmp163[5]={& _tmp165,& _tmp166,& _tmp167,& _tmp168,&
_tmp169};Cyc_aprintf(({const char*_tmp164="%s %s %s%s%s";_tag_arr(_tmp164,sizeof(
char),_get_zero_arr_size(_tmp164,13));}),_tag_arr(_tmp163,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp160;_tmp160.tag=0;_tmp160.f1=(struct
_tagged_arr)_tmp15D;{void*_tmp15E[1]={& _tmp160};Cyc_fprintf(Cyc_stderr,({const
char*_tmp15F="%s\n";_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,4));}),
_tag_arr(_tmp15E,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
if(system((const char*)_untag_arr(_tmp15D,sizeof(char),1))!= 0){({void*_tmp161[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp162="Error: C compiler failed\n";_tag_arr(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,26));}),_tag_arr(_tmp161,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure?1: 0;}}}}}}}}}
