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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static inline void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static inline char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
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
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
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
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tag_arr(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr *
_init_tag_arr(struct _tagged_arr *arr_ptr,
              void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _tagged_arr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_arr(struct _tagged_arr arr, unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_arr_size(struct _tagged_arr arr,unsigned elt_sz) {
  struct _tagged_arr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_plus(struct _tagged_arr arr,unsigned elt_sz,int change) {
  struct _tagged_arr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus_post(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  struct _tagged_arr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

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
 void exit(int);void*abort();struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
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
struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fclose(struct
Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);int Cyc_fflush(struct Cyc___cycFILE*);
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
struct{int quot;int rem;}Cyc_div_t;typedef struct{int quot;int rem;}Cyc_ldiv_t;void*
abort();void exit(int);char*getenv(const char*);int system(const char*);unsigned int
Cyc_strlen(struct _tagged_arr s);int Cyc_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);struct _tagged_arr Cyc_strconcat(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_strconcat_l(struct Cyc_List_List*);struct
_tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);struct
_tagged_arr Cyc_substring(struct _tagged_arr,int ofs,unsigned int n);struct
_tagged_arr Cyc_strchr(struct _tagged_arr s,char c);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);
struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr);int Cyc_Filename_check_suffix(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];void Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];extern int
Cyc_Position_use_gcc_style_location;int Cyc_Position_error_p();struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;
struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
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
extern char Cyc_YY1[8];struct _tuple9{struct Cyc_Absyn_Conref*f1;struct Cyc_Absyn_Conref*
f2;};struct Cyc_YY1_struct{char*tag;struct _tuple9*f1;};extern char Cyc_YY2[8];
struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YY3[8];
struct Cyc_YY3_struct{char*tag;struct _tuple7*f1;};extern char Cyc_YY4[8];struct Cyc_YY4_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
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
vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();
struct Cyc_Interface_I*Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae);struct _tuple10{struct _tagged_arr f1;struct _tagged_arr f2;}
;int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple10*info);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
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
_tmp6.f1=(struct _tagged_arr)((struct _tagged_arr)Cstring_to_string(Cversion));{
void*_tmp4[1]={& _tmp6};Cyc_printf(({const char*_tmp5="The Cyclone compiler, version %s\n";
_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,34));}),_tag_arr(_tmp4,
sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(
struct _tagged_arr)((struct _tagged_arr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct
_tmp9;_tmp9.tag=0;_tmp9.f1=(struct _tagged_arr)((struct _tagged_arr)
Cstring_to_string(Carch));{void*_tmp7[2]={& _tmp9,& _tmpA};Cyc_printf(({const char*
_tmp8="Compiled for architecture: %s CC=\"%s\"\n";_tag_arr(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,39));}),_tag_arr(_tmp7,sizeof(void*),2));}}});({struct
Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _tagged_arr)((struct
_tagged_arr)Cstring_to_string(Cdef_lib_path));{void*_tmpB[1]={& _tmpD};Cyc_printf(({
const char*_tmpC="Standard library directory: %s\n";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,32));}),_tag_arr(_tmpB,sizeof(void*),1));}});({struct
Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _tagged_arr)((struct
_tagged_arr)Cstring_to_string(Cdef_inc_path));{void*_tmpE[1]={& _tmp10};Cyc_printf(({
const char*_tmpF="Standard include directory: %s\n";_tag_arr(_tmpF,sizeof(char),
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
1;Cyc_set_save_temps();}static void Cyc_set_inline_functions(){Cyc_add_ccarg(({
const char*_tmp22="-D_INLINE_FUNCTIONS";_tag_arr(_tmp22,sizeof(char),
_get_zero_arr_size(_tmp22,20));}));}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({
const char*_tmp23="-DNO_CYC_BOUNDS_CHECKS";_tag_arr(_tmp23,sizeof(char),
_get_zero_arr_size(_tmp23,23));}));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({
const char*_tmp24="-DNO_CYC_NULL_CHECKS";_tag_arr(_tmp24,sizeof(char),
_get_zero_arr_size(_tmp24,21));}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp25="-DNO_CYC_PREFIX";_tag_arr(_tmp25,sizeof(char),
_get_zero_arr_size(_tmp25,16));}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({
const char*_tmp26="-DCYC_REGION_PROFILE";_tag_arr(_tmp26,sizeof(char),
_get_zero_arr_size(_tmp26,21));}));}static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({
const char*_tmp27="-pg";_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,4));}));}
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({
const char*_tmp28="-S";_tag_arr(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,3));}));}
static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(struct _tagged_arr s){
if(Cyc_strcmp((struct _tagged_arr)s,({const char*_tmp29="cyc";_tag_arr(_tmp29,
sizeof(char),_get_zero_arr_size(_tmp29,4));}))== 0)Cyc_intype=(void*)1;else{if(
Cyc_strcmp((struct _tagged_arr)s,({const char*_tmp2A="none";_tag_arr(_tmp2A,
sizeof(char),_get_zero_arr_size(_tmp2A,5));}))== 0)Cyc_intype=(void*)0;else{({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp2B[1]={& _tmp2D};Cyc_fprintf(Cyc_stderr,({const
char*_tmp2C="Input type '%s' not supported\n";_tag_arr(_tmp2C,sizeof(char),
_get_zero_arr_size(_tmp2C,31));}),_tag_arr(_tmp2B,sizeof(void*),1));}});}}}
struct _tagged_arr Cyc_make_base_filename(struct _tagged_arr s,struct _tagged_arr*
output_file){struct _tagged_arr _tmp2E=Cyc_Filename_dirname(output_file == 0?s:*
output_file);struct _tagged_arr _tmp2F=Cyc_Filename_chop_extension((struct
_tagged_arr)Cyc_Filename_basename(s));struct _tagged_arr _tmp30=Cyc_strlen((struct
_tagged_arr)_tmp2E)> 0?Cyc_Filename_concat((struct _tagged_arr)_tmp2E,(struct
_tagged_arr)_tmp2F): _tmp2F;return(struct _tagged_arr)_tmp30;}static struct Cyc_List_List*
Cyc_cyclone_files=0;static void Cyc_add_other(struct _tagged_arr s){if(Cyc_intype == (
void*)1?1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({struct Cyc_List_List*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct _tagged_arr*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=s;_tmp32;});_tmp31->tl=Cyc_cyclone_files;
_tmp31;});{struct _tagged_arr _tmp33=Cyc_make_base_filename(s,Cyc_output_file);
struct _tagged_arr _tmp34=Cyc_strconcat((struct _tagged_arr)_tmp33,({const char*
_tmp35=".c";_tag_arr(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,3));}));Cyc_add_ccarg((
struct _tagged_arr)_tmp34);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _tagged_arr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_arr(s,sizeof(char),1));}}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp36;_push_handler(& _tmp36);{int _tmp38=0;if(setjmp(_tmp36.handler))
_tmp38=1;if(!_tmp38){{struct Cyc___cycFILE*_tmp39=(struct Cyc___cycFILE*)Cyc_file_open(
filename,mode);_npop_handler(0);return _tmp39;};_pop_handler();}else{void*_tmp37=(
void*)_exn_thrown;void*_tmp3B=_tmp37;_LL1:;_LL2: Cyc_compile_failure=1;({struct
Cyc_String_pa_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _tagged_arr)((struct
_tagged_arr)filename);{struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(
struct _tagged_arr)((struct _tagged_arr)msg_part);{void*_tmp3C[2]={& _tmp3E,& _tmp3F};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3D="\nError: couldn't open %s %s\n";
_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,29));}),_tag_arr(_tmp3C,
sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;
_LL4:(void)_throw(_tmp3B);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*
env,struct Cyc_List_List*tds){struct _tagged_arr exn_string=({const char*_tmp5D="";
_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,1));});struct _tagged_arr
explain_string=({const char*_tmp5C="";_tag_arr(_tmp5C,sizeof(char),
_get_zero_arr_size(_tmp5C,1));});int other_exn=0;void*ex=(void*)({struct Cyc_Core_Impossible_struct*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Core_Impossible_struct
_tmp5A;_tmp5A.tag=Cyc_Core_Impossible;_tmp5A.f1=({const char*_tmp5B="";_tag_arr(
_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,1));});_tmp5A;});_tmp59;});struct
Cyc_List_List*_tmp40=0;{struct _handler_cons _tmp41;_push_handler(& _tmp41);{int
_tmp43=0;if(setjmp(_tmp41.handler))_tmp43=1;if(!_tmp43){_tmp40=f(env,tds);;
_pop_handler();}else{void*_tmp42=(void*)_exn_thrown;void*_tmp45=_tmp42;struct
_tagged_arr _tmp46;struct _tagged_arr _tmp47;struct _tagged_arr _tmp48;_LL6: if(*((
void**)_tmp45)!= Cyc_Core_Impossible)goto _LL8;_tmp46=((struct Cyc_Core_Impossible_struct*)
_tmp45)->f1;_LL7: exn_string=({const char*_tmp49="Exception Core::Impossible";
_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,27));});explain_string=
_tmp46;goto _LL5;_LL8: if(_tmp45 != Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({
const char*_tmp4A="Exception Dict::Absent";_tag_arr(_tmp4A,sizeof(char),
_get_zero_arr_size(_tmp4A,23));});goto _LL5;_LLA: if(*((void**)_tmp45)!= Cyc_Core_Invalid_argument)
goto _LLC;_tmp47=((struct Cyc_Core_Invalid_argument_struct*)_tmp45)->f1;_LLB:
exn_string=({const char*_tmp4B="Exception Core::Invalid_argument";_tag_arr(_tmp4B,
sizeof(char),_get_zero_arr_size(_tmp4B,33));});explain_string=_tmp47;goto _LL5;
_LLC: if(*((void**)_tmp45)!= Cyc_Core_Failure)goto _LLE;_tmp48=((struct Cyc_Core_Failure_struct*)
_tmp45)->f1;_LLD: exn_string=({const char*_tmp4C="Exception Core::Failure";
_tag_arr(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,24));});explain_string=
_tmp48;goto _LL5;_LLE:;_LLF: ex=_tmp45;other_exn=1;exn_string=({const char*_tmp4D="Uncaught exception";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,19));});goto _LL5;_LL10:;
_LL11:(void)_throw(_tmp45);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_strcmp((struct _tagged_arr)exn_string,({const char*_tmp4E="";_tag_arr(
_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,1));}))!= 0){Cyc_compile_failure=1;({
struct Cyc_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)((
struct _tagged_arr)explain_string);{struct Cyc_String_pa_struct _tmp52;_tmp52.tag=0;
_tmp52.f1=(struct _tagged_arr)((struct _tagged_arr)stage_name);{struct Cyc_String_pa_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=(struct _tagged_arr)((struct _tagged_arr)exn_string);{
void*_tmp4F[3]={& _tmp51,& _tmp52,& _tmp53};Cyc_fprintf(Cyc_stderr,({const char*
_tmp50="\n%s thrown during %s: %s";_tag_arr(_tmp50,sizeof(char),
_get_zero_arr_size(_tmp50,25));}),_tag_arr(_tmp4F,sizeof(void*),3));}}}});}if(
Cyc_compile_failure){({void*_tmp54[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp55="\nCOMPILATION FAILED!\n";_tag_arr(_tmp55,sizeof(char),_get_zero_arr_size(
_tmp55,22));}),_tag_arr(_tmp54,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);if(other_exn)(int)_throw(ex);return _tmp40;}else{if(Cyc_v_r){({struct
Cyc_String_pa_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _tagged_arr)((struct
_tagged_arr)stage_name);{void*_tmp56[1]={& _tmp58};Cyc_fprintf(Cyc_stderr,({const
char*_tmp57="%s completed.\n";_tag_arr(_tmp57,sizeof(char),_get_zero_arr_size(
_tmp57,15));}),_tag_arr(_tmp56,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp40;}}return _tmp40;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init();{struct Cyc_List_List*
_tmp5E=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init();return _tmp5E;}}struct Cyc_List_List*
Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){Cyc_Tc_tc(te,1,
tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(te,tds);return tds;}struct Cyc_List_List*
Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple11{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;
struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple11*
params,struct Cyc_List_List*tds){struct _tuple11 _tmp60;struct Cyc_Tcenv_Tenv*_tmp61;
struct Cyc___cycFILE*_tmp62;struct Cyc___cycFILE*_tmp63;struct _tuple11*_tmp5F=
params;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;_tmp63=_tmp60.f3;{struct
Cyc_Interface_I*_tmp64=Cyc_Interface_extract(_tmp61->ae);if(_tmp62 == 0)Cyc_Interface_save(
_tmp64,_tmp63);else{struct Cyc_Interface_I*_tmp65=Cyc_Interface_parse((struct Cyc___cycFILE*)
_check_null(_tmp62));if(!Cyc_Interface_is_subinterface(_tmp65,_tmp64,({struct
_tuple10*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=({const char*_tmp68="written interface";
_tag_arr(_tmp68,sizeof(char),_get_zero_arr_size(_tmp68,18));});_tmp66->f2=({
const char*_tmp67="maximal interface";_tag_arr(_tmp67,sizeof(char),
_get_zero_arr_size(_tmp67,18));});_tmp66;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp65,_tmp63);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){return Cyc_Tovc_tovc(tds);}static char _tmp69[1]="";
static struct _tagged_arr Cyc_cyc_include={_tmp69,_tmp69,_tmp69 + 1};static void Cyc_set_cyc_include(
struct _tagged_arr f){Cyc_cyc_include=f;}struct Cyc_List_List*Cyc_do_print(struct
Cyc___cycFILE*out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params params_r;
if(Cyc_tc_r)params_r=Cyc_Absynpp_cyc_params_r;else{params_r=Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs=!Cyc_noexpand_r;params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r;params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;{struct Cyc___cycFILE*_tmp6A=Cyc_try_file_open(
Cyc_cyc_include,({const char*_tmp71="r";_tag_arr(_tmp71,sizeof(char),
_get_zero_arr_size(_tmp71,2));}),({const char*_tmp72="internal compiler file";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,23));}));if(_tmp6A == 0)
return tds;{struct Cyc___cycFILE*_tmp6B=(struct Cyc___cycFILE*)_check_null(_tmp6A);
unsigned int n_read=1024;unsigned int n_written;char buf[1024];{unsigned int _tmp70=
1024;unsigned int i;for(i=0;i < _tmp70;i ++){buf[i]='\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_arr(buf,sizeof(char),1024),1,1024,_tmp6B);if(n_read != 1024?
!Cyc_feof(_tmp6B): 0){Cyc_compile_failure=1;({void*_tmp6C[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp6D="\nError: problem copying cyc_include.h\n";_tag_arr(_tmp6D,
sizeof(char),_get_zero_arr_size(_tmp6D,39));}),_tag_arr(_tmp6C,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return tds;}n_written=Cyc_fwrite(
_tag_arr(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=
1;({void*_tmp6E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6F="\nError: problem copying cyc_include.h\n";
_tag_arr(_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,39));}),_tag_arr(_tmp6E,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return tds;}}
Cyc_fclose(_tmp6B);if(Cyc_pp_r){Cyc_Absynpp_set_params(& params_r);Cyc_Absynpp_decllist2file(
tds,out_file);}else{Cyc_Absyndump_set_params(& params_r);Cyc_Absyndump_dumpdecllist2file(
tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)out_file);return tds;}}}static
struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
static struct Cyc_List_List*Cyc_split_by_char(struct _tagged_arr s,char c){if(s.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp73=
0;unsigned int _tmp74=Cyc_strlen((struct _tagged_arr)s);while(_tmp74 > 0){struct
_tagged_arr _tmp75=Cyc_strchr(s,c);if(_tmp75.curr == ((struct _tagged_arr)_tag_arr(
0,0,0)).curr){_tmp73=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));
_tmp76->hd=({struct _tagged_arr*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=s;
_tmp77;});_tmp76->tl=_tmp73;_tmp76;});break;}else{_tmp73=({struct Cyc_List_List*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=({struct _tagged_arr*_tmp79=
_cycalloc(sizeof(*_tmp79));_tmp79[0]=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)s,0,(unsigned int)((((struct _tagged_arr)_tmp75).curr - s.curr)/ 
sizeof(char)));_tmp79;});_tmp78->tl=_tmp73;_tmp78;});_tmp74 -=(((struct
_tagged_arr)_tmp75).curr - s.curr)/ sizeof(char);s=_tagged_arr_plus(_tmp75,
sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp73);}}static int Cyc_file_exists(struct _tagged_arr file){struct Cyc___cycFILE*f=
0;{struct _handler_cons _tmp7A;_push_handler(& _tmp7A);{int _tmp7C=0;if(setjmp(
_tmp7A.handler))_tmp7C=1;if(!_tmp7C){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({
const char*_tmp7D="r";_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,2));}));;
_pop_handler();}else{void*_tmp7B=(void*)_exn_thrown;void*_tmp7F=_tmp7B;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmp7F);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)_check_null(f));return 1;}}static struct
_tagged_arr Cyc_sprint_list(struct Cyc_List_List*dirs){struct _tagged_arr tmp=({
const char*_tmp82="";_tag_arr(_tmp82,sizeof(char),_get_zero_arr_size(_tmp82,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _tagged_arr _tmp80=*((struct _tagged_arr*)dirs->hd);
if(_tmp80.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_strlen((struct
_tagged_arr)_tmp80)== 0)continue;_tmp80=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)_tmp80,({const char*_tmp81=":";_tag_arr(_tmp81,sizeof(char),
_get_zero_arr_size(_tmp81,2));}));tmp=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)_tmp80,(struct _tagged_arr)tmp);}return tmp;}static struct _tagged_arr*
Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr file){if(file.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct
_tagged_arr _tmp83=*((struct _tagged_arr*)dirs->hd);if(_tmp83.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_strlen((struct _tagged_arr)_tmp83)== 0)
continue;{struct _tagged_arr s=(struct _tagged_arr)Cyc_Filename_concat(_tmp83,file);
if(Cyc_file_exists(s))return({struct _tagged_arr*_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84[0]=s;_tmp84;});}}return 0;}static struct _tagged_arr Cyc_do_find(struct Cyc_List_List*
dirs,struct _tagged_arr file){struct _tagged_arr*_tmp85=Cyc_find(dirs,file);if(
_tmp85 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct
_tmp89;_tmp89.tag=0;_tmp89.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_sprint_list(
dirs));{struct Cyc_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(struct
_tagged_arr)((struct _tagged_arr)file);{void*_tmp86[2]={& _tmp88,& _tmp89};Cyc_fprintf(
Cyc_stderr,({const char*_tmp87="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,56));}),_tag_arr(_tmp86,
sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({
struct Cyc_Core_Failure_struct _tmp8B;_tmp8B.tag=Cyc_Core_Failure;_tmp8B.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _tagged_arr)((struct _tagged_arr)file);{void*
_tmp8C[2]={& _tmp8E,& _tmp8F};Cyc_aprintf(({const char*_tmp8D="Error: can't find internal compiler file %s in path %s\n";
_tag_arr(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,56));}),_tag_arr(_tmp8C,
sizeof(void*),2));}}});_tmp8B;});_tmp8A;}));}return*_tmp85;}static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _tagged_arr subdir){struct Cyc_List_List*
_tmp90=0;for(0;dirs != 0;dirs=dirs->tl){_tmp90=({struct Cyc_List_List*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91->hd=({struct _tagged_arr*_tmp92=_cycalloc(
sizeof(*_tmp92));_tmp92[0]=(struct _tagged_arr)Cyc_Filename_concat(*((struct
_tagged_arr*)dirs->hd),subdir);_tmp92;});_tmp91->tl=_tmp90;_tmp91;});}_tmp90=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp90);return
_tmp90;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _tagged_arr subdir){struct Cyc_List_List*_tmp93=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp93=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->hd=(
struct _tagged_arr*)dirs->hd;_tmp94->tl=_tmp93;_tmp94;});_tmp93=({struct Cyc_List_List*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=({struct _tagged_arr*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96[0]=(struct _tagged_arr)Cyc_Filename_concat(*((
struct _tagged_arr*)dirs->hd),subdir);_tmp96;});_tmp95->tl=_tmp93;_tmp95;});}
_tmp93=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93);
return _tmp93;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case ' ': _LL21:
goto _LL22;case '\n': _LL22: goto _LL23;case '\t': _LL23: return 1;default: _LL24: return 0;}}
static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int
_tmp97=Cyc_strlen((struct _tagged_arr)s);int _tmp98=0;int _tmp99=0;{int i=0;for(0;i < 
_tmp97;i ++){char _tmp9A=*((const char*)_check_unknown_subscript(s,sizeof(char),i));
if(_tmp9A == '\'')_tmp98 ++;else{if(Cyc_is_other_special(_tmp9A))_tmp99 ++;}}}if(
_tmp98 == 0?_tmp99 == 0: 0)return s;if(_tmp98 == 0)return(struct _tagged_arr)Cyc_strconcat_l(({
struct _tagged_arr*_tmp9B[3];_tmp9B[2]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(char),2);_tmp9B[1]=({struct _tagged_arr*_tmp9D=_cycalloc(
sizeof(*_tmp9D));_tmp9D[0]=(struct _tagged_arr)s;_tmp9D;});_tmp9B[0]=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(char),2);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp9B,sizeof(
struct _tagged_arr*),3));}));{unsigned int _tmp9F=(_tmp97 + _tmp98)+ _tmp99;struct
_tagged_arr s2=({unsigned int _tmpA9=_tmp9F + 1;char*_tmpAA=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmpA9 + 1));struct _tagged_arr _tmpAC=_tag_arr(_tmpAA,
sizeof(char),_tmpA9 + 1);{unsigned int _tmpAB=_tmpA9;unsigned int i;for(i=0;i < 
_tmpAB;i ++){_tmpAA[i]='\000';}_tmpAA[_tmpAB]=(char)0;}_tmpAC;});int _tmpA0=0;int
_tmpA1=0;for(0;_tmpA0 < _tmp97;_tmpA0 ++){char _tmpA2=*((const char*)
_check_unknown_subscript(s,sizeof(char),_tmpA0));if(_tmpA2 == '\''?1: Cyc_is_other_special(
_tmpA2))({struct _tagged_arr _tmpA3=_tagged_arr_plus(s2,sizeof(char),_tmpA1 ++);
char _tmpA4=*((char*)_check_unknown_subscript(_tmpA3,sizeof(char),0));char _tmpA5='\\';
if(_get_arr_size(_tmpA3,sizeof(char))== 1?_tmpA4 == '\000'?_tmpA5 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA3.curr)=_tmpA5;});({struct _tagged_arr _tmpA6=
_tagged_arr_plus(s2,sizeof(char),_tmpA1 ++);char _tmpA7=*((char*)
_check_unknown_subscript(_tmpA6,sizeof(char),0));char _tmpA8=_tmpA2;if(
_get_arr_size(_tmpA6,sizeof(char))== 1?_tmpA7 == '\000'?_tmpA8 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA6.curr)=_tmpA8;});}return(struct _tagged_arr)
s2;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){
return({struct _tagged_arr*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=Cyc_sh_escape_string(*
sp);_tmpAD;});}static void Cyc_process_file(struct _tagged_arr filename){struct
_tagged_arr _tmpAE=Cyc_make_base_filename(filename,Cyc_output_file);struct
_tagged_arr _tmpAF=Cyc_strconcat((struct _tagged_arr)_tmpAE,({const char*_tmp103=".cyp";
_tag_arr(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,5));}));struct
_tagged_arr _tmpB0=Cyc_strconcat((struct _tagged_arr)_tmpAE,({const char*_tmp102=".cyci";
_tag_arr(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,6));}));struct
_tagged_arr _tmpB1=Cyc_strconcat((struct _tagged_arr)_tmpAE,({const char*_tmp101=".cycio";
_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,7));}));struct
_tagged_arr _tmpB2=Cyc_strconcat((struct _tagged_arr)_tmpAE,({const char*_tmp100=".c";
_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,3));}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _tagged_arr)((
struct _tagged_arr)filename);{void*_tmpB3[1]={& _tmpB5};Cyc_fprintf(Cyc_stderr,({
const char*_tmpB4="Compiling %s\n";_tag_arr(_tmpB4,sizeof(char),
_get_zero_arr_size(_tmpB4,14));}),_tag_arr(_tmpB3,sizeof(void*),1));}});{struct
Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmpFE="r";_tag_arr(
_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,2));}),({const char*_tmpFF="input file";
_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,11));}));if(Cyc_compile_failure)
return;Cyc_fclose((struct Cyc___cycFILE*)_check_null(f0));{struct _tagged_arr
_tmpB6=Cyc_str_sepstr(({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(*_tmpFB));
_tmpFB->hd=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);
_tmpFB->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmpFB;}),({const char*_tmpFD=" ";
_tag_arr(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,2));}));struct _tagged_arr
def_inc_path=(struct _tagged_arr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpB7=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpB7=Cyc_add_subdir(
_tmpB7,({const char*_tmpB8="include";_tag_arr(_tmpB8,sizeof(char),
_get_zero_arr_size(_tmpB8,8));}));if(Cyc_strlen((struct _tagged_arr)def_inc_path)
> 0)_tmpB7=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=({
struct _tagged_arr*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=def_inc_path;
_tmpBA;});_tmpB9->tl=_tmpB7;_tmpB9;});{char*_tmpBB=getenv((const char*)"CYCLONE_INCLUDE_PATH");
if((char*)_tmpBB != 0)_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({char*_tmpBC=_tmpBB;
_tag_arr(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,1));}),':'),_tmpB7);{
struct _tagged_arr stdinc_string=(struct _tagged_arr)Cyc_str_sepstr(({struct Cyc_List_List*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmpF8->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpB7);_tmpF8;}),({const char*_tmpFA=" -I";_tag_arr(
_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,4));}));struct _tagged_arr
ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)ofile_string=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_output_file)));{
void*_tmpBD[1]={& _tmpBF};Cyc_aprintf(({const char*_tmpBE=" > %s";_tag_arr(_tmpBE,
sizeof(char),_get_zero_arr_size(_tmpBE,6));}),_tag_arr(_tmpBD,sizeof(void*),1));}});
else{ofile_string=({const char*_tmpC0="";_tag_arr(_tmpC0,sizeof(char),
_get_zero_arr_size(_tmpC0,1));});}}else{ofile_string=(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmpC3;_tmpC3.tag=0;_tmpC3.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmpAF));{void*_tmpC1[1]={&
_tmpC3};Cyc_aprintf(({const char*_tmpC2=" > %s";_tag_arr(_tmpC2,sizeof(char),
_get_zero_arr_size(_tmpC2,6));}),_tag_arr(_tmpC1,sizeof(void*),1));}});}{struct
_tagged_arr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0)fixup_string=({const char*_tmpC4=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_arr(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,35));});else{fixup_string=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_dependencies_target)));{
void*_tmpC5[1]={& _tmpC7};Cyc_aprintf(({const char*_tmpC6=" | sed 's/^.*\\.cyc\\.o:/%s:/'";
_tag_arr(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,29));}),_tag_arr(_tmpC5,
sizeof(void*),1));}});}}else{fixup_string=({const char*_tmpC8="";_tag_arr(_tmpC8,
sizeof(char),_get_zero_arr_size(_tmpC8,1));});}{struct _tagged_arr _tmpC9=({struct
Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _tagged_arr)((struct
_tagged_arr)ofile_string);{struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(
struct _tagged_arr)((struct _tagged_arr)fixup_string);{struct Cyc_String_pa_struct
_tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_sh_escape_string(
filename));{struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct
_tagged_arr)((struct _tagged_arr)stdinc_string);{struct Cyc_String_pa_struct _tmpF3;
_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpB6);{struct Cyc_String_pa_struct
_tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_cpp);{
void*_tmpF0[6]={& _tmpF2,& _tmpF3,& _tmpF4,& _tmpF5,& _tmpF6,& _tmpF7};Cyc_aprintf(({
const char*_tmpF1="%s %s%s %s%s%s";_tag_arr(_tmpF1,sizeof(char),
_get_zero_arr_size(_tmpF1,15));}),_tag_arr(_tmpF0,sizeof(void*),6));}}}}}}});if(
Cyc_v_r)({struct Cyc_String_pa_struct _tmpCC;_tmpCC.tag=0;_tmpCC.f1=(struct
_tagged_arr)((struct _tagged_arr)_tmpC9);{void*_tmpCA[1]={& _tmpCC};Cyc_fprintf(
Cyc_stderr,({const char*_tmpCB="%s\n";_tag_arr(_tmpCB,sizeof(char),
_get_zero_arr_size(_tmpCB,4));}),_tag_arr(_tmpCA,sizeof(void*),1));}});if(system((
const char*)_untag_arr(_tmpC9,sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*
_tmpCD[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpCE="\nError: preprocessing\n";
_tag_arr(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,23));}),_tag_arr(_tmpCD,
sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((
struct _tagged_arr)_tmpAF);{struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct
_tagged_arr)_tmpAF,({const char*_tmpEE="r";_tag_arr(_tmpEE,sizeof(char),
_get_zero_arr_size(_tmpEE,2));}),({const char*_tmpEF="file";_tag_arr(_tmpEF,
sizeof(char),_get_zero_arr_size(_tmpEF,5));}));if(Cyc_compile_failure)return;{
struct Cyc_List_List*tds=0;{struct _handler_cons _tmpCF;_push_handler(& _tmpCF);{int
_tmpD1=0;if(setjmp(_tmpCF.handler))_tmpD1=1;if(!_tmpD1){tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpD2="parsing";_tag_arr(_tmpD2,sizeof(char),_get_zero_arr_size(
_tmpD2,8));}),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);;
_pop_handler();}else{void*_tmpD0=(void*)_exn_thrown;void*_tmpD4=_tmpD0;_LL27:;
_LL28: Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));Cyc_remove_file((
struct _tagged_arr)_tmpAF);(int)_throw(_tmpD4);_LL29:;_LL2A:(void)_throw(_tmpD4);
_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAF);return;}{struct Cyc_Tcenv_Tenv*_tmpD5=
Cyc_Tcenv_tc_init();if(Cyc_parseonly_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpD6="type checking";_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(
_tmpD6,14));}),Cyc_do_typecheck,_tmpD5,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmpAF);return;}tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,
struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD7="control-flow checking";
_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,22));}),Cyc_do_cfcheck,1,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmpAF);return;}
if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)
_untag_arr(_tmpB0,sizeof(char),1)),(const char*)"r");struct Cyc___cycFILE*
inter_objfile=Cyc_try_file_open((struct _tagged_arr)_tmpB1,({const char*_tmpDA="w";
_tag_arr(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,2));}),({const char*_tmpDB="interface object file";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,22));}));if(inter_objfile
== 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((struct
_tagged_arr)_tmpB0);tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct _tuple11*,struct Cyc_List_List*),struct _tuple11*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD8="interface checking";
_tag_arr(_tmpD8,sizeof(char),_get_zero_arr_size(_tmpD8,19));}),Cyc_do_interface,({
struct _tuple11*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->f1=_tmpD5;_tmpD9->f2=
inter_file;_tmpD9->f3=(struct Cyc___cycFILE*)_check_null(inter_objfile);_tmpD9;}),
tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)_check_null(
inter_file));Cyc_file_close((struct Cyc___cycFILE*)_check_null(inter_objfile));}
if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpDC="translation to C";_tag_arr(_tmpDC,sizeof(
char),_get_zero_arr_size(_tmpDC,17));}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmpAF);return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _tagged_arr)_tmpAF);if(Cyc_compile_failure)return;if(Cyc_tovc_r)
tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpDD="post-pass to VC";_tag_arr(_tmpDD,sizeof(char),
_get_zero_arr_size(_tmpDD,16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if(Cyc_parseonly_r?1: Cyc_tc_r){if(
Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct _tagged_arr*)_check_null(
Cyc_output_file)),({const char*_tmpDE="w";_tag_arr(_tmpDE,sizeof(char),
_get_zero_arr_size(_tmpDE,2));}),({const char*_tmpDF="output file";_tag_arr(
_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,12));}));else{out_file=(struct Cyc___cycFILE*)
Cyc_stdout;}}else{if(Cyc_toc_r?Cyc_output_file != 0: 0)out_file=Cyc_try_file_open(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmpE0="w";_tag_arr(
_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,2));}),({const char*_tmpE1="output file";
_tag_arr(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,12));}));else{out_file=
Cyc_try_file_open((struct _tagged_arr)_tmpB2,({const char*_tmpE2="w";_tag_arr(
_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,2));}),({const char*_tmpE3="output file";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,12));}));}}if(Cyc_compile_failure)
return;{struct _handler_cons _tmpE4;_push_handler(& _tmpE4);{int _tmpE6=0;if(setjmp(
_tmpE4.handler))_tmpE6=1;if(!_tmpE6){tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),
struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpE7="printing";_tag_arr(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,9));}),
Cyc_do_print,(struct Cyc___cycFILE*)_check_null(out_file),tds);;_pop_handler();}
else{void*_tmpE5=(void*)_exn_thrown;void*_tmpE9=_tmpE5;_LL2C:;_LL2D: Cyc_compile_failure=
1;Cyc_file_close((struct Cyc___cycFILE*)_check_null(out_file));Cyc_cfiles=({
struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=({struct
_tagged_arr*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=(struct _tagged_arr)
_tmpB2;_tmpEB;});_tmpEA->tl=Cyc_cfiles;_tmpEA;});(int)_throw(_tmpE9);_LL2E:;
_LL2F:(void)_throw(_tmpE9);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*_tmpEC=_cycalloc(sizeof(*
_tmpEC));_tmpEC->hd=({struct _tagged_arr*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[
0]=(struct _tagged_arr)_tmpB2;_tmpED;});_tmpEC->tl=Cyc_cfiles;_tmpEC;});}}}}}}}}}}}
static char _tmp104[8]="<final>";static struct _tagged_arr Cyc_final_str={_tmp104,
_tmp104,_tmp104 + 8};static struct _tagged_arr*Cyc_final_strptr=& Cyc_final_str;
static struct Cyc_Interface_I*Cyc_read_cycio(struct _tagged_arr*n){if(n == (struct
_tagged_arr*)Cyc_final_strptr)return Cyc_Interface_final();{struct _tagged_arr
basename;{struct _handler_cons _tmp105;_push_handler(& _tmp105);{int _tmp107=0;if(
setjmp(_tmp105.handler))_tmp107=1;if(!_tmp107){basename=(struct _tagged_arr)Cyc_Filename_chop_extension(*
n);;_pop_handler();}else{void*_tmp106=(void*)_exn_thrown;void*_tmp109=_tmp106;
_LL31: if(*((void**)_tmp109)!= Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*
n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmp109);_LL30:;}}}{struct _tagged_arr
_tmp10A=Cyc_strconcat((struct _tagged_arr)basename,({const char*_tmp10F=".cycio";
_tag_arr(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,7));}));struct Cyc___cycFILE*
_tmp10B=Cyc_try_file_open((struct _tagged_arr)_tmp10A,({const char*_tmp10D="rb";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,3));}),({const char*
_tmp10E="interface object file";_tag_arr(_tmp10E,sizeof(char),_get_zero_arr_size(
_tmp10E,22));}));if(_tmp10B == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(
1);}Cyc_Position_reset_position((struct _tagged_arr)_tmp10A);{struct Cyc_Interface_I*
_tmp10C=Cyc_Interface_load((struct Cyc___cycFILE*)_check_null(_tmp10B));Cyc_file_close((
struct Cyc___cycFILE*)_check_null(_tmp10B));return _tmp10C;}}}}static int Cyc_is_cfile(
struct _tagged_arr*n){return*((const char*)_check_unknown_subscript(*n,sizeof(char),
0))!= '-';}void GC_blacklist_warn_clear();struct _tuple12{struct _tagged_arr f1;int
f2;struct _tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct _tagged_arr comp=(struct
_tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)comp,({const char*_tmp110=" -x c -E -U__GNUC__ -nostdinc";
_tag_arr(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,30));})));{struct Cyc_List_List*
options=({struct _tuple12*_tmp176[53];_tmp176[52]=({struct _tuple12*_tmp2AF=
_cycalloc(sizeof(*_tmp2AF));_tmp2AF->f1=({const char*_tmp2B4="-detailedlocation";
_tag_arr(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,18));});_tmp2AF->f2=0;
_tmp2AF->f3=({const char*_tmp2B3="";_tag_arr(_tmp2B3,sizeof(char),
_get_zero_arr_size(_tmp2B3,1));});_tmp2AF->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp2B2;_tmp2B2.tag=4;_tmp2B2.f1=& Cyc_Position_use_gcc_style_location;_tmp2B2;});
_tmp2B1;});_tmp2AF->f5=({const char*_tmp2B0="Try to give more detailed location information for errors";
_tag_arr(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,58));});_tmp2AF;});
_tmp176[51]=({struct _tuple12*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->f1=({
const char*_tmp2AE="-noregister";_tag_arr(_tmp2AE,sizeof(char),_get_zero_arr_size(
_tmp2AE,12));});_tmp2A9->f2=0;_tmp2A9->f3=({const char*_tmp2AD="";_tag_arr(
_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,1));});_tmp2A9->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2AC;_tmp2AC.tag=3;_tmp2AC.f1=& Cyc_Parse_no_register;
_tmp2AC;});_tmp2AB;});_tmp2A9->f5=({const char*_tmp2AA="Treat register storage class as public";
_tag_arr(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,39));});_tmp2A9;});
_tmp176[50]=({struct _tuple12*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->f1=({
const char*_tmp2A8="-warnboundschecks";_tag_arr(_tmp2A8,sizeof(char),
_get_zero_arr_size(_tmp2A8,18));});_tmp2A3->f2=0;_tmp2A3->f3=({const char*_tmp2A7="";
_tag_arr(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,1));});_tmp2A3->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));
_tmp2A5[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A6;_tmp2A6.tag=3;_tmp2A6.f1=& Cyc_Toc_warn_bounds_checks;
_tmp2A6;});_tmp2A5;});_tmp2A3->f5=({const char*_tmp2A4="Warn when bounds checks can't be eliminated";
_tag_arr(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,44));});_tmp2A3;});
_tmp176[49]=({struct _tuple12*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->f1=({
const char*_tmp2A2="-CI";_tag_arr(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,
4));});_tmp29D->f2=0;_tmp29D->f3=({const char*_tmp2A1=" <file>";_tag_arr(_tmp2A1,
sizeof(char),_get_zero_arr_size(_tmp2A1,8));});_tmp29D->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F[0]=({struct Cyc_Arg_String_spec_struct
_tmp2A0;_tmp2A0.tag=5;_tmp2A0.f1=Cyc_set_cyc_include;_tmp2A0;});_tmp29F;});
_tmp29D->f5=({const char*_tmp29E="Set cyc_include.h to be <file>";_tag_arr(
_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,31));});_tmp29D;});_tmp176[48]=({
struct _tuple12*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->f1=({const char*
_tmp29C="-printfullevars";_tag_arr(_tmp29C,sizeof(char),_get_zero_arr_size(
_tmp29C,16));});_tmp297->f2=0;_tmp297->f3=({const char*_tmp29B="";_tag_arr(
_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,1));});_tmp297->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=({
struct Cyc_Arg_Set_spec_struct _tmp29A;_tmp29A.tag=3;_tmp29A.f1=& Cyc_print_full_evars_r;
_tmp29A;});_tmp299;});_tmp297->f5=({const char*_tmp298="Print full information for evars (type debugging)";
_tag_arr(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,50));});_tmp297;});
_tmp176[47]=({struct _tuple12*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->f1=({
const char*_tmp296="-printallkinds";_tag_arr(_tmp296,sizeof(char),
_get_zero_arr_size(_tmp296,15));});_tmp291->f2=0;_tmp291->f3=({const char*_tmp295="";
_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,1));});_tmp291->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp293=_cycalloc(sizeof(*_tmp293));
_tmp293[0]=({struct Cyc_Arg_Set_spec_struct _tmp294;_tmp294.tag=3;_tmp294.f1=& Cyc_print_all_kinds_r;
_tmp294;});_tmp293;});_tmp291->f5=({const char*_tmp292="Always print kinds of type variables";
_tag_arr(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,37));});_tmp291;});
_tmp176[46]=({struct _tuple12*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->f1=({
const char*_tmp290="-printalltvars";_tag_arr(_tmp290,sizeof(char),
_get_zero_arr_size(_tmp290,15));});_tmp28B->f2=0;_tmp28B->f3=({const char*_tmp28F="";
_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,1));});_tmp28B->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));
_tmp28D[0]=({struct Cyc_Arg_Set_spec_struct _tmp28E;_tmp28E.tag=3;_tmp28E.f1=& Cyc_print_all_tvars_r;
_tmp28E;});_tmp28D;});_tmp28B->f5=({const char*_tmp28C="Print all type variables (even implicit default effects)";
_tag_arr(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,57));});_tmp28B;});
_tmp176[45]=({struct _tuple12*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->f1=({
const char*_tmp28A="-noexpandtypedefs";_tag_arr(_tmp28A,sizeof(char),
_get_zero_arr_size(_tmp28A,18));});_tmp285->f2=0;_tmp285->f3=({const char*_tmp289="";
_tag_arr(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,1));});_tmp285->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp287=_cycalloc(sizeof(*_tmp287));
_tmp287[0]=({struct Cyc_Arg_Set_spec_struct _tmp288;_tmp288.tag=3;_tmp288.f1=& Cyc_noexpand_r;
_tmp288;});_tmp287;});_tmp285->f5=({const char*_tmp286="Don't expand typedefs in pretty printing";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,41));});_tmp285;});
_tmp176[44]=({struct _tuple12*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->f1=({
const char*_tmp284="-noremoveunused";_tag_arr(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,16));});_tmp27F->f2=0;_tmp27F->f3=({const char*_tmp283="";
_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,1));});_tmp27F->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp281=_cycalloc(sizeof(*_tmp281));
_tmp281[0]=({struct Cyc_Arg_Set_spec_struct _tmp282;_tmp282.tag=3;_tmp282.f1=& Cyc_noshake_r;
_tmp282;});_tmp281;});_tmp27F->f5=({const char*_tmp280="Don't remove externed variables that aren't used";
_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,49));});_tmp27F;});
_tmp176[43]=({struct _tuple12*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->f1=({
const char*_tmp27E="-nogc";_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(
_tmp27E,6));});_tmp279->f2=0;_tmp279->f3=({const char*_tmp27D="";_tag_arr(_tmp27D,
sizeof(char),_get_zero_arr_size(_tmp27D,1));});_tmp279->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Arg_Set_spec_struct
_tmp27C;_tmp27C.tag=3;_tmp27C.f1=& Cyc_nogc_r;_tmp27C;});_tmp27B;});_tmp279->f5=({
const char*_tmp27A="Don't link in the garbage collector";_tag_arr(_tmp27A,sizeof(
char),_get_zero_arr_size(_tmp27A,36));});_tmp279;});_tmp176[42]=({struct _tuple12*
_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->f1=({const char*_tmp278="-nocyc";
_tag_arr(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,7));});_tmp273->f2=0;
_tmp273->f3=({const char*_tmp277="";_tag_arr(_tmp277,sizeof(char),
_get_zero_arr_size(_tmp277,1));});_tmp273->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp276;_tmp276.tag=0;_tmp276.f1=Cyc_set_nocyc;_tmp276;});_tmp275;});_tmp273->f5=({
const char*_tmp274="Don't add implicit namespace Cyc";_tag_arr(_tmp274,sizeof(
char),_get_zero_arr_size(_tmp274,33));});_tmp273;});_tmp176[41]=({struct _tuple12*
_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->f1=({const char*_tmp272="-no-cpp-precomp";
_tag_arr(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,16));});_tmp26D->f2=0;
_tmp26D->f3=({const char*_tmp271="";_tag_arr(_tmp271,sizeof(char),
_get_zero_arr_size(_tmp271,1));});_tmp26D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=Cyc_set_nocppprecomp;_tmp270;});_tmp26F;});
_tmp26D->f5=({const char*_tmp26E="Don't do smart preprocessing (mac only)";
_tag_arr(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,40));});_tmp26D;});
_tmp176[40]=({struct _tuple12*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->f1=({
const char*_tmp26C="-use-cpp";_tag_arr(_tmp26C,sizeof(char),_get_zero_arr_size(
_tmp26C,9));});_tmp267->f2=0;_tmp267->f3=({const char*_tmp26B="<path>";_tag_arr(
_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,7));});_tmp267->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({
struct Cyc_Arg_String_spec_struct _tmp26A;_tmp26A.tag=5;_tmp26A.f1=Cyc_set_cpp;
_tmp26A;});_tmp269;});_tmp267->f5=({const char*_tmp268="Indicate which preprocessor to use";
_tag_arr(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,35));});_tmp267;});
_tmp176[39]=({struct _tuple12*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->f1=({
const char*_tmp266="--inline-checks";_tag_arr(_tmp266,sizeof(char),
_get_zero_arr_size(_tmp266,16));});_tmp261->f2=0;_tmp261->f3=({const char*_tmp265="";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,1));});_tmp261->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp263=_cycalloc(sizeof(*_tmp263));
_tmp263[0]=({struct Cyc_Arg_Unit_spec_struct _tmp264;_tmp264.tag=0;_tmp264.f1=Cyc_set_inline_functions;
_tmp264;});_tmp263;});_tmp261->f5=({const char*_tmp262="Inline bounds checks instead of #define";
_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,40));});_tmp261;});
_tmp176[38]=({struct _tuple12*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->f1=({
const char*_tmp260="--noboundschecks";_tag_arr(_tmp260,sizeof(char),
_get_zero_arr_size(_tmp260,17));});_tmp25B->f2=0;_tmp25B->f3=({const char*_tmp25F="";
_tag_arr(_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,1));});_tmp25B->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));
_tmp25D[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=Cyc_set_noboundschecks;
_tmp25E;});_tmp25D;});_tmp25B->f5=({const char*_tmp25C="Disable bounds checks";
_tag_arr(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,22));});_tmp25B;});
_tmp176[37]=({struct _tuple12*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255->f1=({
const char*_tmp25A="--nonullchecks";_tag_arr(_tmp25A,sizeof(char),
_get_zero_arr_size(_tmp25A,15));});_tmp255->f2=0;_tmp255->f3=({const char*_tmp259="";
_tag_arr(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,1));});_tmp255->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp257=_cycalloc(sizeof(*_tmp257));
_tmp257[0]=({struct Cyc_Arg_Unit_spec_struct _tmp258;_tmp258.tag=0;_tmp258.f1=Cyc_set_nonullchecks;
_tmp258;});_tmp257;});_tmp255->f5=({const char*_tmp256="Disable null checks";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,20));});_tmp255;});
_tmp176[36]=({struct _tuple12*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->f1=({
const char*_tmp254="--nochecks";_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(
_tmp254,11));});_tmp24F->f2=0;_tmp24F->f3=({const char*_tmp253="";_tag_arr(
_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,1));});_tmp24F->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp252;_tmp252.tag=0;_tmp252.f1=Cyc_set_nochecks;
_tmp252;});_tmp251;});_tmp24F->f5=({const char*_tmp250="Disable bounds/null checks";
_tag_arr(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,27));});_tmp24F;});
_tmp176[35]=({struct _tuple12*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->f1=({
const char*_tmp24E="--lineno";_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(
_tmp24E,9));});_tmp249->f2=0;_tmp249->f3=({const char*_tmp24D="";_tag_arr(_tmp24D,
sizeof(char),_get_zero_arr_size(_tmp24D,1));});_tmp249->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp24C;_tmp24C.tag=0;_tmp24C.f1=Cyc_set_lineno;_tmp24C;});_tmp24B;});_tmp249->f5=({
const char*_tmp24A="Generate line numbers for debugging";_tag_arr(_tmp24A,sizeof(
char),_get_zero_arr_size(_tmp24A,36));});_tmp249;});_tmp176[34]=({struct _tuple12*
_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->f1=({const char*_tmp248="-save-c";
_tag_arr(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,8));});_tmp243->f2=0;
_tmp243->f3=({const char*_tmp247="";_tag_arr(_tmp247,sizeof(char),
_get_zero_arr_size(_tmp247,1));});_tmp243->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Arg_Set_spec_struct
_tmp246;_tmp246.tag=3;_tmp246.f1=& Cyc_save_c_r;_tmp246;});_tmp245;});_tmp243->f5=({
const char*_tmp244="Don't delete temporary C files";_tag_arr(_tmp244,sizeof(char),
_get_zero_arr_size(_tmp244,31));});_tmp243;});_tmp176[33]=({struct _tuple12*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->f1=({const char*_tmp242="-save-temps";
_tag_arr(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,12));});_tmp23D->f2=0;
_tmp23D->f3=({const char*_tmp241="";_tag_arr(_tmp241,sizeof(char),
_get_zero_arr_size(_tmp241,1));});_tmp23D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp240;_tmp240.tag=0;_tmp240.f1=Cyc_set_save_temps;_tmp240;});_tmp23F;});
_tmp23D->f5=({const char*_tmp23E="Don't delete temporary files";_tag_arr(_tmp23E,
sizeof(char),_get_zero_arr_size(_tmp23E,29));});_tmp23D;});_tmp176[32]=({struct
_tuple12*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->f1=({const char*_tmp23C="-tovc";
_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,6));});_tmp237->f2=0;
_tmp237->f3=({const char*_tmp23B="";_tag_arr(_tmp23B,sizeof(char),
_get_zero_arr_size(_tmp23B,1));});_tmp237->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Arg_Set_spec_struct
_tmp23A;_tmp23A.tag=3;_tmp23A.f1=& Cyc_tovc_r;_tmp23A;});_tmp239;});_tmp237->f5=({
const char*_tmp238="Avoid gcc extensions in C output";_tag_arr(_tmp238,sizeof(
char),_get_zero_arr_size(_tmp238,33));});_tmp237;});_tmp176[31]=({struct _tuple12*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->f1=({const char*_tmp236="-up";
_tag_arr(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,4));});_tmp231->f2=0;
_tmp231->f3=({const char*_tmp235="";_tag_arr(_tmp235,sizeof(char),
_get_zero_arr_size(_tmp235,1));});_tmp231->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp234;_tmp234.tag=4;_tmp234.f1=& Cyc_pp_r;_tmp234;});_tmp233;});_tmp231->f5=({
const char*_tmp232="Ugly print";_tag_arr(_tmp232,sizeof(char),_get_zero_arr_size(
_tmp232,11));});_tmp231;});_tmp176[30]=({struct _tuple12*_tmp22B=_cycalloc(
sizeof(*_tmp22B));_tmp22B->f1=({const char*_tmp230="-pp";_tag_arr(_tmp230,sizeof(
char),_get_zero_arr_size(_tmp230,4));});_tmp22B->f2=0;_tmp22B->f3=({const char*
_tmp22F="";_tag_arr(_tmp22F,sizeof(char),_get_zero_arr_size(_tmp22F,1));});
_tmp22B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22D=_cycalloc(sizeof(*
_tmp22D));_tmp22D[0]=({struct Cyc_Arg_Set_spec_struct _tmp22E;_tmp22E.tag=3;
_tmp22E.f1=& Cyc_pp_r;_tmp22E;});_tmp22D;});_tmp22B->f5=({const char*_tmp22C="Pretty print";
_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,13));});_tmp22B;});
_tmp176[29]=({struct _tuple12*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->f1=({
const char*_tmp22A="-ic";_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,
4));});_tmp225->f2=0;_tmp225->f3=({const char*_tmp229="";_tag_arr(_tmp229,sizeof(
char),_get_zero_arr_size(_tmp229,1));});_tmp225->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=({struct Cyc_Arg_Set_spec_struct
_tmp228;_tmp228.tag=3;_tmp228.f1=& Cyc_ic_r;_tmp228;});_tmp227;});_tmp225->f5=({
const char*_tmp226="Activate the link-checker";_tag_arr(_tmp226,sizeof(char),
_get_zero_arr_size(_tmp226,26));});_tmp225;});_tmp176[28]=({struct _tuple12*
_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->f1=({const char*_tmp224="-stopafter-toc";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,15));});_tmp21F->f2=0;
_tmp21F->f3=({const char*_tmp223="";_tag_arr(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,1));});_tmp21F->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Arg_Set_spec_struct
_tmp222;_tmp222.tag=3;_tmp222.f1=& Cyc_toc_r;_tmp222;});_tmp221;});_tmp21F->f5=({
const char*_tmp220="Stop after translation to C";_tag_arr(_tmp220,sizeof(char),
_get_zero_arr_size(_tmp220,28));});_tmp21F;});_tmp176[27]=({struct _tuple12*
_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219->f1=({const char*_tmp21E="-stopafter-tc";
_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,14));});_tmp219->f2=0;
_tmp219->f3=({const char*_tmp21D="";_tag_arr(_tmp21D,sizeof(char),
_get_zero_arr_size(_tmp21D,1));});_tmp219->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Arg_Set_spec_struct
_tmp21C;_tmp21C.tag=3;_tmp21C.f1=& Cyc_tc_r;_tmp21C;});_tmp21B;});_tmp219->f5=({
const char*_tmp21A="Stop after type checking";_tag_arr(_tmp21A,sizeof(char),
_get_zero_arr_size(_tmp21A,25));});_tmp219;});_tmp176[26]=({struct _tuple12*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->f1=({const char*_tmp218="-stopafter-parse";
_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,17));});_tmp213->f2=0;
_tmp213->f3=({const char*_tmp217="";_tag_arr(_tmp217,sizeof(char),
_get_zero_arr_size(_tmp217,1));});_tmp213->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Arg_Set_spec_struct
_tmp216;_tmp216.tag=3;_tmp216.f1=& Cyc_parseonly_r;_tmp216;});_tmp215;});_tmp213->f5=({
const char*_tmp214="Stop after parsing";_tag_arr(_tmp214,sizeof(char),
_get_zero_arr_size(_tmp214,19));});_tmp213;});_tmp176[25]=({struct _tuple12*
_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->f1=({const char*_tmp212="-E";
_tag_arr(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,3));});_tmp20D->f2=0;
_tmp20D->f3=({const char*_tmp211="";_tag_arr(_tmp211,sizeof(char),
_get_zero_arr_size(_tmp211,1));});_tmp20D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=({struct Cyc_Arg_Set_spec_struct
_tmp210;_tmp210.tag=3;_tmp210.f1=& Cyc_stop_after_cpp_r;_tmp210;});_tmp20F;});
_tmp20D->f5=({const char*_tmp20E="Stop after preprocessing";_tag_arr(_tmp20E,
sizeof(char),_get_zero_arr_size(_tmp20E,25));});_tmp20D;});_tmp176[24]=({struct
_tuple12*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->f1=({const char*_tmp20C="-b";
_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,3));});_tmp207->f2=0;
_tmp207->f3=({const char*_tmp20B="<arch>";_tag_arr(_tmp20B,sizeof(char),
_get_zero_arr_size(_tmp20B,7));});_tmp207->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Arg_String_spec_struct
_tmp20A;_tmp20A.tag=5;_tmp20A.f1=Cyc_set_target_arch;_tmp20A;});_tmp209;});
_tmp207->f5=({const char*_tmp208="Set target architecture; implies -stopafter-toc";
_tag_arr(_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,48));});_tmp207;});
_tmp176[23]=({struct _tuple12*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->f1=({
const char*_tmp206="-MT";_tag_arr(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,
4));});_tmp201->f2=0;_tmp201->f3=({const char*_tmp205=" <target>";_tag_arr(
_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,10));});_tmp201->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({
struct Cyc_Arg_String_spec_struct _tmp204;_tmp204.tag=5;_tmp204.f1=Cyc_set_dependencies_target;
_tmp204;});_tmp203;});_tmp201->f5=({const char*_tmp202="Give target for dependencies";
_tag_arr(_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,29));});_tmp201;});
_tmp176[22]=({struct _tuple12*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->f1=({
const char*_tmp200="-MG";_tag_arr(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,
4));});_tmp1FB->f2=0;_tmp1FB->f3=({const char*_tmp1FF="";_tag_arr(_tmp1FF,sizeof(
char),_get_zero_arr_size(_tmp1FF,1));});_tmp1FB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1FE;_tmp1FE.tag=1;_tmp1FE.f1=Cyc_add_cpparg;_tmp1FE;});_tmp1FD;});_tmp1FB->f5=({
const char*_tmp1FC="When producing dependencies assume that missing files are generated";
_tag_arr(_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,68));});_tmp1FB;});
_tmp176[21]=({struct _tuple12*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->f1=({
const char*_tmp1FA="-M";_tag_arr(_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,
3));});_tmp1F5->f2=0;_tmp1F5->f3=({const char*_tmp1F9="";_tag_arr(_tmp1F9,sizeof(
char),_get_zero_arr_size(_tmp1F9,1));});_tmp1F5->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F8;_tmp1F8.tag=0;_tmp1F8.f1=Cyc_set_produce_dependencies;_tmp1F8;});_tmp1F7;});
_tmp1F5->f5=({const char*_tmp1F6="Produce dependencies";_tag_arr(_tmp1F6,sizeof(
char),_get_zero_arr_size(_tmp1F6,21));});_tmp1F5;});_tmp176[20]=({struct _tuple12*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->f1=({const char*_tmp1F4="-S";
_tag_arr(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,3));});_tmp1EF->f2=0;
_tmp1EF->f3=({const char*_tmp1F3="";_tag_arr(_tmp1F3,sizeof(char),
_get_zero_arr_size(_tmp1F3,1));});_tmp1EF->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=Cyc_set_stop_after_asmfile;_tmp1F2;});_tmp1F1;});
_tmp1EF->f5=({const char*_tmp1F0="Stop after producing assembly code";_tag_arr(
_tmp1F0,sizeof(char),_get_zero_arr_size(_tmp1F0,35));});_tmp1EF;});_tmp176[19]=({
struct _tuple12*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=({const char*
_tmp1EE="-pa";_tag_arr(_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,4));});
_tmp1E9->f2=0;_tmp1E9->f3=({const char*_tmp1ED="";_tag_arr(_tmp1ED,sizeof(char),
_get_zero_arr_size(_tmp1ED,1));});_tmp1E9->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=Cyc_set_pa;_tmp1EC;});_tmp1EB;});_tmp1E9->f5=({
const char*_tmp1EA="Compile for profiling with aprof";_tag_arr(_tmp1EA,sizeof(
char),_get_zero_arr_size(_tmp1EA,33));});_tmp1E9;});_tmp176[18]=({struct _tuple12*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->f1=({const char*_tmp1E8="-pg";
_tag_arr(_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,4));});_tmp1E3->f2=0;
_tmp1E3->f3=({const char*_tmp1E7="";_tag_arr(_tmp1E7,sizeof(char),
_get_zero_arr_size(_tmp1E7,1));});_tmp1E3->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=Cyc_set_pg;_tmp1E6;});_tmp1E5;});_tmp1E3->f5=({
const char*_tmp1E4="Compile for profiling with gprof";_tag_arr(_tmp1E4,sizeof(
char),_get_zero_arr_size(_tmp1E4,33));});_tmp1E3;});_tmp176[17]=({struct _tuple12*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->f1=({const char*_tmp1E2="-p";
_tag_arr(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,3));});_tmp1DD->f2=0;
_tmp1DD->f3=({const char*_tmp1E1="";_tag_arr(_tmp1E1,sizeof(char),
_get_zero_arr_size(_tmp1E1,1));});_tmp1DD->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=Cyc_add_ccarg;_tmp1E0;});_tmp1DF;});_tmp1DD->f5=({
const char*_tmp1DE="Compile for profiling with prof";_tag_arr(_tmp1DE,sizeof(char),
_get_zero_arr_size(_tmp1DE,32));});_tmp1DD;});_tmp176[16]=({struct _tuple12*
_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->f1=({const char*_tmp1DC="-g";
_tag_arr(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,3));});_tmp1D7->f2=0;
_tmp1D7->f3=({const char*_tmp1DB="";_tag_arr(_tmp1DB,sizeof(char),
_get_zero_arr_size(_tmp1DB,1));});_tmp1D7->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=Cyc_add_ccarg;_tmp1DA;});_tmp1D9;});_tmp1D7->f5=({
const char*_tmp1D8="Compile for debugging";_tag_arr(_tmp1D8,sizeof(char),
_get_zero_arr_size(_tmp1D8,22));});_tmp1D7;});_tmp176[15]=({struct _tuple12*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=({const char*_tmp1D6="-fomit-frame-pointer";
_tag_arr(_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,21));});_tmp1D1->f2=0;
_tmp1D1->f3=({const char*_tmp1D5="";_tag_arr(_tmp1D5,sizeof(char),
_get_zero_arr_size(_tmp1D5,1));});_tmp1D1->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=Cyc_add_ccarg;_tmp1D4;});_tmp1D3;});_tmp1D1->f5=({
const char*_tmp1D2="Omit frame pointer";_tag_arr(_tmp1D2,sizeof(char),
_get_zero_arr_size(_tmp1D2,19));});_tmp1D1;});_tmp176[14]=({struct _tuple12*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->f1=({const char*_tmp1D0="-O3";
_tag_arr(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,4));});_tmp1CB->f2=0;
_tmp1CB->f3=({const char*_tmp1CF="";_tag_arr(_tmp1CF,sizeof(char),
_get_zero_arr_size(_tmp1CF,1));});_tmp1CB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=Cyc_add_ccarg;_tmp1CE;});_tmp1CD;});_tmp1CB->f5=({
const char*_tmp1CC="Even more optimization";_tag_arr(_tmp1CC,sizeof(char),
_get_zero_arr_size(_tmp1CC,23));});_tmp1CB;});_tmp176[13]=({struct _tuple12*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->f1=({const char*_tmp1CA="-O2";
_tag_arr(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,4));});_tmp1C5->f2=0;
_tmp1C5->f3=({const char*_tmp1C9="";_tag_arr(_tmp1C9,sizeof(char),
_get_zero_arr_size(_tmp1C9,1));});_tmp1C5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=Cyc_add_ccarg;_tmp1C8;});_tmp1C7;});_tmp1C5->f5=({
const char*_tmp1C6="A higher level of optimization";_tag_arr(_tmp1C6,sizeof(char),
_get_zero_arr_size(_tmp1C6,31));});_tmp1C5;});_tmp176[12]=({struct _tuple12*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->f1=({const char*_tmp1C4="-O";
_tag_arr(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,3));});_tmp1BF->f2=0;
_tmp1BF->f3=({const char*_tmp1C3="";_tag_arr(_tmp1C3,sizeof(char),
_get_zero_arr_size(_tmp1C3,1));});_tmp1BF->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=Cyc_add_ccarg;_tmp1C2;});_tmp1C1;});_tmp1BF->f5=({
const char*_tmp1C0="Optimize";_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(
_tmp1C0,9));});_tmp1BF;});_tmp176[11]=({struct _tuple12*_tmp1B9=_cycalloc(sizeof(*
_tmp1B9));_tmp1B9->f1=({const char*_tmp1BE="-O0";_tag_arr(_tmp1BE,sizeof(char),
_get_zero_arr_size(_tmp1BE,4));});_tmp1B9->f2=0;_tmp1B9->f3=({const char*_tmp1BD="";
_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,1));});_tmp1B9->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BC;_tmp1BC.tag=1;_tmp1BC.f1=Cyc_add_ccarg;
_tmp1BC;});_tmp1BB;});_tmp1B9->f5=({const char*_tmp1BA="Don't optimize";_tag_arr(
_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,15));});_tmp1B9;});_tmp176[10]=({
struct _tuple12*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->f1=({const char*
_tmp1B8="-s";_tag_arr(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,3));});
_tmp1B3->f2=0;_tmp1B3->f3=({const char*_tmp1B7="";_tag_arr(_tmp1B7,sizeof(char),
_get_zero_arr_size(_tmp1B7,1));});_tmp1B3->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1B6;_tmp1B6.tag=1;_tmp1B6.f1=Cyc_add_ccarg;_tmp1B6;});_tmp1B5;});_tmp1B3->f5=({
const char*_tmp1B4="Remove all symbol table and relocation info from executable";
_tag_arr(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,60));});_tmp1B3;});
_tmp176[9]=({struct _tuple12*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->f1=({
const char*_tmp1B2="-x";_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,
3));});_tmp1AD->f2=0;_tmp1AD->f3=({const char*_tmp1B1=" <language>";_tag_arr(
_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,12));});_tmp1AD->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({
struct Cyc_Arg_String_spec_struct _tmp1B0;_tmp1B0.tag=5;_tmp1B0.f1=Cyc_set_inputtype;
_tmp1B0;});_tmp1AF;});_tmp1AD->f5=({const char*_tmp1AE="Specify <language> for the following input files";
_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,49));});_tmp1AD;});
_tmp176[8]=({struct _tuple12*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->f1=({
const char*_tmp1AC="-c";_tag_arr(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,
3));});_tmp1A7->f2=0;_tmp1A7->f3=({const char*_tmp1AB="";_tag_arr(_tmp1AB,sizeof(
char),_get_zero_arr_size(_tmp1AB,1));});_tmp1A7->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1AA;_tmp1AA.tag=0;_tmp1AA.f1=Cyc_set_stop_after_objectfile;_tmp1AA;});
_tmp1A9;});_tmp1A7->f5=({const char*_tmp1A8="Produce an object file instead of an executable; do not link";
_tag_arr(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,61));});_tmp1A7;});
_tmp176[7]=({struct _tuple12*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->f1=({
const char*_tmp1A6="-l";_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,
3));});_tmp1A1->f2=1;_tmp1A1->f3=({const char*_tmp1A5="<file>";_tag_arr(_tmp1A5,
sizeof(char),_get_zero_arr_size(_tmp1A5,7));});_tmp1A1->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A4;_tmp1A4.tag=1;_tmp1A4.f1=Cyc_add_libarg;_tmp1A4;});_tmp1A3;});_tmp1A1->f5=({
const char*_tmp1A2="Library file";_tag_arr(_tmp1A2,sizeof(char),
_get_zero_arr_size(_tmp1A2,13));});_tmp1A1;});_tmp176[6]=({struct _tuple12*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->f1=({const char*_tmp1A0="-L";
_tag_arr(_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,3));});_tmp19B->f2=1;
_tmp19B->f3=({const char*_tmp19F="<dir>";_tag_arr(_tmp19F,sizeof(char),
_get_zero_arr_size(_tmp19F,6));});_tmp19B->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp19E;_tmp19E.tag=1;_tmp19E.f1=Cyc_add_ccarg;_tmp19E;});_tmp19D;});_tmp19B->f5=({
const char*_tmp19C="Add to the list of directories for -l";_tag_arr(_tmp19C,
sizeof(char),_get_zero_arr_size(_tmp19C,38));});_tmp19B;});_tmp176[5]=({struct
_tuple12*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->f1=({const char*_tmp19A="-I";
_tag_arr(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,3));});_tmp195->f2=1;
_tmp195->f3=({const char*_tmp199="<dir>";_tag_arr(_tmp199,sizeof(char),
_get_zero_arr_size(_tmp199,6));});_tmp195->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp198;_tmp198.tag=1;_tmp198.f1=Cyc_add_cpparg;_tmp198;});_tmp197;});_tmp195->f5=({
const char*_tmp196="Add to the list of directories to search for include files";
_tag_arr(_tmp196,sizeof(char),_get_zero_arr_size(_tmp196,59));});_tmp195;});
_tmp176[4]=({struct _tuple12*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=({
const char*_tmp194="-B";_tag_arr(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,
3));});_tmp18F->f2=1;_tmp18F->f3=({const char*_tmp193="<file>";_tag_arr(_tmp193,
sizeof(char),_get_zero_arr_size(_tmp193,7));});_tmp18F->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp192;_tmp192.tag=1;_tmp192.f1=Cyc_add_cyclone_exec_path;_tmp192;});_tmp191;});
_tmp18F->f5=({const char*_tmp190="Add to the list of directories to search for compiler files";
_tag_arr(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,60));});_tmp18F;});
_tmp176[3]=({struct _tuple12*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->f1=({
const char*_tmp18E="-D";_tag_arr(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,
3));});_tmp189->f2=1;_tmp189->f3=({const char*_tmp18D="<name>[=<value>]";_tag_arr(
_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,17));});_tmp189->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp18C;_tmp18C.tag=1;_tmp18C.f1=Cyc_add_cpparg;
_tmp18C;});_tmp18B;});_tmp189->f5=({const char*_tmp18A="Pass definition to preprocessor";
_tag_arr(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,32));});_tmp189;});
_tmp176[2]=({struct _tuple12*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->f1=({
const char*_tmp188="-o";_tag_arr(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,
3));});_tmp183->f2=0;_tmp183->f3=({const char*_tmp187=" <file>";_tag_arr(_tmp187,
sizeof(char),_get_zero_arr_size(_tmp187,8));});_tmp183->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Arg_String_spec_struct
_tmp186;_tmp186.tag=5;_tmp186.f1=Cyc_set_output_file;_tmp186;});_tmp185;});
_tmp183->f5=({const char*_tmp184="Set the output file name to <file>";_tag_arr(
_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,35));});_tmp183;});_tmp176[1]=({
struct _tuple12*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->f1=({const char*
_tmp182="--version";_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,10));});
_tmp17D->f2=0;_tmp17D->f3=({const char*_tmp181="";_tag_arr(_tmp181,sizeof(char),
_get_zero_arr_size(_tmp181,1));});_tmp17D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp180;_tmp180.tag=0;_tmp180.f1=Cyc_print_version;_tmp180;});_tmp17F;});_tmp17D->f5=({
const char*_tmp17E="Print version information and exit";_tag_arr(_tmp17E,sizeof(
char),_get_zero_arr_size(_tmp17E,35));});_tmp17D;});_tmp176[0]=({struct _tuple12*
_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=({const char*_tmp17C="-v";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,3));});_tmp177->f2=0;
_tmp177->f3=({const char*_tmp17B="";_tag_arr(_tmp17B,sizeof(char),
_get_zero_arr_size(_tmp17B,1));});_tmp177->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Arg_Set_spec_struct
_tmp17A;_tmp17A.tag=3;_tmp17A.f1=& Cyc_v_r;_tmp17A;});_tmp179;});_tmp177->f5=({
const char*_tmp178="Print compilation stages verbosely";_tag_arr(_tmp178,sizeof(
char),_get_zero_arr_size(_tmp178,35));});_tmp177;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp176,sizeof(struct _tuple12*),53));});
Cyc_Arg_parse(options,Cyc_add_other,({const char*_tmp111="Options:";_tag_arr(
_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,9));}),argv);if(Cyc_cyclone_files
== 0?Cyc_ccargs == 0: 0){({void*_tmp112[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp113="missing file\n";_tag_arr(_tmp113,sizeof(char),_get_zero_arr_size(
_tmp113,14));}),_tag_arr(_tmp112,sizeof(void*),0));});exit(1);}if(Cyc_target_arch.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_target_arch=(struct _tagged_arr)
Cstring_to_string(Carch);{struct _tagged_arr cyclone_exec_prefix=({char*_tmp175=
getenv((const char*)"CYCLONE_EXEC_PREFIX");_tag_arr(_tmp175,sizeof(char),
_get_zero_arr_size(_tmp175,1));});if(cyclone_exec_prefix.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=({struct _tagged_arr*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=cyclone_exec_prefix;_tmp115;});_tmp114->tl=
Cyc_cyclone_exec_path;_tmp114;});{struct _tagged_arr def_lib_path=(struct
_tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct _tagged_arr)
def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp116=_cycalloc(
sizeof(*_tmp116));_tmp116->hd=({struct _tagged_arr*_tmp117=_cycalloc(sizeof(*
_tmp117));_tmp117[0]=(struct _tagged_arr)Cyc_Filename_concat(def_lib_path,({const
char*_tmp118="cyc-lib";_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,
8));}));_tmp117;});_tmp116->tl=Cyc_cyclone_exec_path;_tmp116;});Cyc_cyclone_exec_path=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{
struct Cyc_List_List*_tmp119=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
if(!Cyc_stop_after_cpp_r?Cyc_strlen((struct _tagged_arr)Cyc_cyc_include)== 0: 0)
Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({const char*_tmp11A="cyc_include.h";
_tag_arr(_tmp11A,sizeof(char),_get_zero_arr_size(_tmp11A,14));}));{struct Cyc_List_List*
_tmp11B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp11B != 0;_tmp11B=_tmp11B->tl){Cyc_process_file(*((struct _tagged_arr*)
_tmp11B->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r?1: Cyc_parseonly_r)?
1: Cyc_tc_r)?1: Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((struct
_tagged_arr)Cyc_strconcat(({const char*_tmp11C="-L";_tag_arr(_tmp11C,sizeof(char),
_get_zero_arr_size(_tmp11C,3));}),(struct _tagged_arr)def_lib_path));Cyc_ccargs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct
_tagged_arr _tmp11D=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _tagged_arr*(*
f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_ccargs),({const char*_tmp174=" ";_tag_arr(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmp11E=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp171->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp171;}),({const char*
_tmp173=" ";_tag_arr(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,2));}));
struct Cyc_List_List*stdlib;struct _tagged_arr stdlib_string;int _tmp11F=((Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),({const char*_tmp16F=".a";
_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,3));})): 0))?1:(Cyc_output_file
!= 0?Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),({
const char*_tmp170=".lib";_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,5));})): 0);if(_tmp11F){stdlib=0;stdlib_string=({const char*_tmp120="";
_tag_arr(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,1));});}else{struct
_tagged_arr libcyc_flag;struct _tagged_arr nogc_filename;struct _tagged_arr
runtime_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){libcyc_flag=({const
char*_tmp121="-lcyc_a";_tag_arr(_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,
8));});nogc_filename=({const char*_tmp122="nogc_a.a";_tag_arr(_tmp122,sizeof(char),
_get_zero_arr_size(_tmp122,9));});runtime_filename=({const char*_tmp123="runtime_cyc_a.o";
_tag_arr(_tmp123,sizeof(char),_get_zero_arr_size(_tmp123,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp124="-lcyc_nocheck";_tag_arr(_tmp124,sizeof(char),
_get_zero_arr_size(_tmp124,14));});nogc_filename=({const char*_tmp125="nogc_nocheck.a";
_tag_arr(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,15));});
runtime_filename=({const char*_tmp126="runtime_cyc.o";_tag_arr(_tmp126,sizeof(
char),_get_zero_arr_size(_tmp126,14));});}else{if(Cyc_pg_r){libcyc_flag=({const
char*_tmp127="-lcyc_pg";_tag_arr(_tmp127,sizeof(char),_get_zero_arr_size(_tmp127,
9));});runtime_filename=({const char*_tmp128="runtime_cyc_pg.o";_tag_arr(_tmp128,
sizeof(char),_get_zero_arr_size(_tmp128,17));});nogc_filename=({const char*
_tmp129="nogc_pg.a";_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,10));});}
else{libcyc_flag=({const char*_tmp12A="-lcyc";_tag_arr(_tmp12A,sizeof(char),
_get_zero_arr_size(_tmp12A,6));});nogc_filename=({const char*_tmp12B="nogc.a";
_tag_arr(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,7));});runtime_filename=({
const char*_tmp12C="runtime_cyc.o";_tag_arr(_tmp12C,sizeof(char),
_get_zero_arr_size(_tmp12C,14));});}}}gc_filename=({const char*_tmp12D="gc.a";
_tag_arr(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,5));});{struct
_tagged_arr _tmp12E=Cyc_nogc_r?Cyc_do_find(_tmp119,nogc_filename): Cyc_do_find(
_tmp119,gc_filename);struct _tagged_arr _tmp12F=Cyc_do_find(_tmp119,
runtime_filename);stdlib=0;stdlib_string=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp134;_tmp134.tag=0;_tmp134.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp12E);{
struct Cyc_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct _tagged_arr)((
struct _tagged_arr)libcyc_flag);{struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp12F);{void*_tmp130[3]={&
_tmp132,& _tmp133,& _tmp134};Cyc_aprintf(({const char*_tmp131=" %s %s %s";_tag_arr(
_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,10));}),_tag_arr(_tmp130,sizeof(
void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp135;_push_handler(&
_tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){Cyc_ccargs=((
struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct
_tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp11F)_tmp138=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));
_tmp139->hd=Cyc_final_strptr;_tmp139->tl=_tmp138;_tmp139;});{struct Cyc_Interface_I*
_tmp13A=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_tagged_arr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp138,_tmp138);if(_tmp13A == 0){({void*_tmp13B[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13C="Error: interfaces incompatible\n";_tag_arr(
_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,32));}),_tag_arr(_tmp13B,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp13D=1;
_npop_handler(0);return _tmp13D;}}if(_tmp11F){if(Cyc_output_file != 0){struct
_tagged_arr _tmp13E=({struct Cyc_String_pa_struct _tmp145;_tmp145.tag=0;_tmp145.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Filename_chop_extension(*((struct
_tagged_arr*)_check_null(Cyc_output_file))));{void*_tmp143[1]={& _tmp145};Cyc_aprintf(({
const char*_tmp144="%s.cycio";_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(
_tmp144,9));}),_tag_arr(_tmp143,sizeof(void*),1));}});struct Cyc___cycFILE*
_tmp13F=Cyc_try_file_open((struct _tagged_arr)_tmp13E,({const char*_tmp141="wb";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,3));}),({const char*
_tmp142="interface object file";_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(
_tmp142,22));}));if(_tmp13F == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int
_tmp140=1;_npop_handler(0);return _tmp140;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_check_null(_tmp13A),(struct Cyc___cycFILE*)_check_null(_tmp13F));Cyc_file_close((
struct Cyc___cycFILE*)_check_null(_tmp13F));}}else{if(!Cyc_Interface_is_subinterface(
Cyc_Interface_empty(),(struct Cyc_Interface_I*)_check_null(_tmp13A),({struct
_tuple10*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->f1=({const char*_tmp148="empty interface";
_tag_arr(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,16));});_tmp146->f2=({
const char*_tmp147="global interface";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,17));});_tmp146;}))){({void*_tmp149[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp14A="Error: some objects are still undefined\n";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,41));}),_tag_arr(
_tmp149,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int
_tmp14B=1;_npop_handler(0);return _tmp14B;}}}}};_pop_handler();}else{void*_tmp136=(
void*)_exn_thrown;void*_tmp14D=_tmp136;struct _tagged_arr _tmp14E;struct
_tagged_arr _tmp14F;struct _tagged_arr _tmp150;_LL36: if(*((void**)_tmp14D)!= Cyc_Core_Failure)
goto _LL38;_tmp14E=((struct Cyc_Core_Failure_struct*)_tmp14D)->f1;_LL37:({struct
Cyc_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)((struct
_tagged_arr)_tmp14E);{void*_tmp151[1]={& _tmp153};Cyc_printf(({const char*_tmp152="Exception Core::Failure %s\n";
_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,28));}),_tag_arr(
_tmp151,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL38: if(*((void**)_tmp14D)!= Cyc_Core_Impossible)goto _LL3A;_tmp14F=((struct Cyc_Core_Impossible_struct*)
_tmp14D)->f1;_LL39:({struct Cyc_String_pa_struct _tmp156;_tmp156.tag=0;_tmp156.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp14F);{void*_tmp154[1]={& _tmp156};Cyc_printf(({
const char*_tmp155="Exception Core::Impossible %s\n";_tag_arr(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,31));}),_tag_arr(_tmp154,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3A: if(_tmp14D != Cyc_Dict_Absent)goto _LL3C;_LL3B:({
void*_tmp157[0]={};Cyc_printf(({const char*_tmp158="Exception Dict::Absent\n";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,24));}),_tag_arr(
_tmp157,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3C: if(*((void**)_tmp14D)!= Cyc_Core_Invalid_argument)goto _LL3E;_tmp150=((
struct Cyc_Core_Invalid_argument_struct*)_tmp14D)->f1;_LL3D:({struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp150);{
void*_tmp159[1]={& _tmp15B};Cyc_printf(({const char*_tmp15A="Exception Core::Invalid_argument %s\n";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,37));}),_tag_arr(
_tmp159,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3E:;_LL3F:({void*_tmp15C[0]={};Cyc_printf(({const char*_tmp15D="Uncaught exception\n";
_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,20));}),_tag_arr(
_tmp15C,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL40:;_LL41:(void)_throw(_tmp14D);_LL35:;}}}{struct _tagged_arr outfile_str=({
const char*_tmp16E="";_tag_arr(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,1));});
if(Cyc_output_file != 0)outfile_str=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp160;_tmp160.tag=0;_tmp160.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_sh_escape_string(*((
struct _tagged_arr*)_check_null(Cyc_output_file))));{void*_tmp15E[1]={& _tmp160};
Cyc_aprintf(({const char*_tmp15F=" -o %s";_tag_arr(_tmp15F,sizeof(char),
_get_zero_arr_size(_tmp15F,7));}),_tag_arr(_tmp15E,sizeof(void*),1));}});{struct
_tagged_arr _tmp161=({struct Cyc_String_pa_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp11E);{struct Cyc_String_pa_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct _tagged_arr)((struct _tagged_arr)
stdlib_string);{struct Cyc_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp11D);{struct Cyc_String_pa_struct
_tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct _tagged_arr)((struct _tagged_arr)
outfile_str);{struct Cyc_String_pa_struct _tmp169;_tmp169.tag=0;_tmp169.f1=(struct
_tagged_arr)((struct _tagged_arr)comp);{void*_tmp167[5]={& _tmp169,& _tmp16A,&
_tmp16B,& _tmp16C,& _tmp16D};Cyc_aprintf(({const char*_tmp168="%s %s %s%s%s";
_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,13));}),_tag_arr(
_tmp167,sizeof(void*),5));}}}}}});if(Cyc_v_r){({struct Cyc_String_pa_struct
_tmp164;_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp161);{
void*_tmp162[1]={& _tmp164};Cyc_fprintf(Cyc_stderr,({const char*_tmp163="%s\n";
_tag_arr(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,4));}),_tag_arr(_tmp162,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((
const char*)_untag_arr(_tmp161,sizeof(char),1))!= 0){({void*_tmp165[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp166="Error: C compiler failed\n";_tag_arr(_tmp166,
sizeof(char),_get_zero_arr_size(_tmp166,26));}),_tag_arr(_tmp165,sizeof(void*),0));});
Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();return Cyc_compile_failure?
1: 0;}}}}}}}}}}
