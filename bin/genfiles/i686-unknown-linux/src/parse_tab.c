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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _tagged_arr s);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int
Cyc_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_strcat(struct _tagged_arr
dest,struct _tagged_arr src);struct _tagged_arr Cyc_strcpy(struct _tagged_arr dest,
struct _tagged_arr src);struct _tagged_arr Cyc_substring(struct _tagged_arr,int ofs,
unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;
};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*
f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct
Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
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
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Reference_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _tagged_arr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
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
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct
_tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,
struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*
Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct
Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct
Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;
void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(struct
_tagged_arr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(struct
_tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*attributes;};
struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List*tms;};struct _tuple5{void*f1;struct Cyc_Core_Opt*f2;};static
struct _tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct
Cyc_List_List*f4;};static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
void*,struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Parse_lbuf=
0;static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((struct _tagged_arr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)Cyc_Position_Exit);}static
void Cyc_Parse_unimp(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){({struct
Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct _tagged_arr)((struct
_tagged_arr)msg);{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(sg));{void*_tmp0[
2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,46));}),_tag_arr(_tmp0,
sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=0;struct _tuple1*
Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9[0]=({struct Cyc_Absyn_Rel_n_struct _tmpA;_tmpA.tag=0;_tmpA.f1=0;_tmpA;});
_tmp9;});_tmp4->f2=({struct _tagged_arr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(
unsigned int)Cyc_Parse_enum_counter ++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({
const char*_tmp7="__anonymous_enum_%d__";_tag_arr(_tmp7,sizeof(char),
_get_zero_arr_size(_tmp7,22));}),_tag_arr(_tmp6,sizeof(void*),1));}});_tmp5;});
_tmp4;});}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct
Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info){struct _tuple8 _tmpC;
struct _tuple7*_tmpD;struct _tuple7 _tmpE;struct _tuple1*_tmpF;struct Cyc_Absyn_Tqual
_tmp10;void*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_Absyn_Exp*_tmp14;struct _tuple8*_tmpB=field_info;_tmpC=*_tmpB;_tmpD=_tmpC.f1;
_tmpE=*_tmpD;_tmpF=_tmpE.f1;_tmp10=_tmpE.f2;_tmp11=_tmpE.f3;_tmp12=_tmpE.f4;
_tmp13=_tmpE.f5;_tmp14=_tmpC.f2;if(_tmp12 != 0)Cyc_Parse_err(({const char*_tmp15="bad type params in struct field";
_tag_arr(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,32));}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmpF))Cyc_Parse_err(({const char*_tmp16="struct or union field cannot be qualified with a namespace";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,59));}),loc);return({
struct Cyc_Absyn_Aggrfield*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->name=(*_tmpF).f2;
_tmp17->tq=_tmp10;_tmp17->type=(void*)_tmp11;_tmp17->width=_tmp14;_tmp17->attributes=
_tmp13;_tmp17;});}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){return(void*)({struct Cyc_Parse_Type_spec_struct*_tmp18=_cycalloc(sizeof(*
_tmp18));_tmp18[0]=({struct Cyc_Parse_Type_spec_struct _tmp19;_tmp19.tag=4;_tmp19.f1=(
void*)t;_tmp19.f2=loc;_tmp19;});_tmp18;});}static void*Cyc_Parse_array2ptr(void*t,
int argposn){void*_tmp1A=t;struct Cyc_Absyn_ArrayInfo _tmp1B;void*_tmp1C;struct Cyc_Absyn_Tqual
_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Conref*_tmp1F;_LL1: if(_tmp1A <= (
void*)3  || *((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A)->f1;_tmp1C=(void*)_tmp1B.elt_type;_tmp1D=_tmp1B.tq;_tmp1E=_tmp1B.num_elts;
_tmp1F=_tmp1B.zero_term;_LL2: return Cyc_Absyn_starb_typ(_tmp1C,argposn?Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(void*)((void*)3);
_tmp20;}),0):(void*)2,_tmp1D,_tmp1E == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct Cyc_Absyn_Exp*)_tmp1E;_tmp22;});_tmp21;}),
_tmp1F);_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp23=_cycalloc(sizeof(*_tmp23));
_tmp23->f1=(*t).f2;_tmp23->f2=(*t).f3;_tmp23;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp24=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp25;_LL6: if(*((int*)_tmp24)!= 0)goto _LL8;_tmp25=((struct Cyc_Absyn_Var_d_struct*)
_tmp24)->f1;_LL7: return _tmp25;_LL8:;_LL9:({void*_tmp26[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,({const char*
_tmp27="bad declaration in `forarray' statement";_tag_arr(_tmp27,sizeof(char),
_get_zero_arr_size(_tmp27,40));}),_tag_arr(_tmp26,sizeof(void*),0));});_LL5:;}
static int Cyc_Parse_is_typeparam(void*tm){void*_tmp28=tm;_LLB: if(*((int*)_tmp28)
!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _tagged_arr s,void*k){if(Cyc_zstrcmp((struct _tagged_arr)s,({const char*
_tmp29="`H";_tag_arr(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,3));}))== 0)
return(void*)2;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_VarType_struct _tmp2B;
_tmp2B.tag=1;_tmp2B.f1=({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C->name=({struct _tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=s;
_tmp2D;});_tmp2C->identity=0;_tmp2C->kind=(void*)k;_tmp2C;});_tmp2B;});_tmp2A;});}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{
void*_tmp2E=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp2F;void*_tmp30;_LL10:
if(*((int*)_tmp2E)!= 0)goto _LL12;_tmp2F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp2E)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp31=_cycalloc(
sizeof(*_tmp31));_tmp31[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp32;_tmp32.tag=0;
_tmp32.f1=(void*)_tmp2F;_tmp32;});_tmp31;});goto _LLF;_LL12: if(*((int*)_tmp2E)!= 
1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp33=_cycalloc(
sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp34;_tmp34.tag=
1;_tmp34.f1=0;_tmp34;});_tmp33;});goto _LLF;_LL14: if(*((int*)_tmp2E)!= 2)goto _LLF;
_tmp30=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL15: k=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp36;_tmp36.tag=2;_tmp36.f1=0;_tmp36.f2=(void*)
_tmp30;_tmp36;});_tmp35;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp37=
_cycalloc(sizeof(*_tmp37));_tmp37->name=t->name;_tmp37->identity=0;_tmp37->kind=(
void*)k;_tmp37;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp38=t;struct Cyc_Absyn_Tvar*_tmp39;_LL17: if(_tmp38 <= (void*)3
 || *((int*)_tmp38)!= 1)goto _LL19;_tmp39=((struct Cyc_Absyn_VarType_struct*)
_tmp38)->f1;_LL18: return _tmp39;_LL19:;_LL1A:({void*_tmp3A[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,({const char*_tmp3B="expecting a list of type variables, not types";_tag_arr(
_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,46));}),_tag_arr(_tmp3A,sizeof(
void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
return(void*)({struct Cyc_Absyn_VarType_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));
_tmp3C[0]=({struct Cyc_Absyn_VarType_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=pr;
_tmp3D;});_tmp3C;});}static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*
_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3F;struct Cyc_Absyn_Tvar
_tmp40;void*_tmp41;void**_tmp42;_LL1C: if(_tmp3E <= (void*)3  || *((int*)_tmp3E)!= 
1)goto _LL1E;_tmp3F=((struct Cyc_Absyn_VarType_struct*)_tmp3E)->f1;_tmp40=*_tmp3F;
_tmp41=(void*)_tmp40.kind;_tmp42=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3E)->f1).kind;_LL1D: {void*_tmp43=Cyc_Absyn_compress_kb(*_tmp42);_LL21: if(*((
int*)_tmp43)!= 1)goto _LL23;_LL22:*_tmp42=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp45;
_tmp45.tag=0;_tmp45.f1=(void*)k;_tmp45;});_tmp44;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp46=(void*)tms->hd;void*_tmp47;
_LL26: if(*((int*)_tmp46)!= 3)goto _LL28;_tmp47=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp46)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp48=_tmp47;struct Cyc_List_List*_tmp49;_LL2B: if(*((int*)_tmp48)!= 1)
goto _LL2D;_LL2C:({void*_tmp4A[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4B="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,93));}),_tag_arr(_tmp4A,
sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp48)!= 0)goto _LL2A;_tmp49=((
struct Cyc_Absyn_NoTypes_struct*)_tmp48)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp49)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tds))({void*_tmp4C[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp4D="wrong number of parameter declarations in old-style function declaration";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,73));}),_tag_arr(_tmp4C,
sizeof(void*),0));});{struct Cyc_List_List*_tmp4E=0;for(0;_tmp49 != 0;_tmp49=
_tmp49->tl){struct Cyc_List_List*_tmp4F=tds;for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){
struct Cyc_Absyn_Decl*_tmp50=(struct Cyc_Absyn_Decl*)_tmp4F->hd;void*_tmp51=(void*)
_tmp50->r;struct Cyc_Absyn_Vardecl*_tmp52;_LL30: if(*((int*)_tmp51)!= 0)goto _LL32;
_tmp52=((struct Cyc_Absyn_Var_d_struct*)_tmp51)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp52->name).f2,(struct _tagged_arr*)_tmp49->hd)!= 0)continue;if(_tmp52->initializer
!= 0)({void*_tmp53[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp50->loc,({const char*
_tmp54="initializer found in parameter declaration";_tag_arr(_tmp54,sizeof(char),
_get_zero_arr_size(_tmp54,43));}),_tag_arr(_tmp53,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp52->name))({void*_tmp55[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp50->loc,({const char*
_tmp56="namespaces forbidden in parameter declarations";_tag_arr(_tmp56,sizeof(
char),_get_zero_arr_size(_tmp56,47));}),_tag_arr(_tmp55,sizeof(void*),0));});
_tmp4E=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=({
struct _tuple2*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=({struct Cyc_Core_Opt*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->v=(*_tmp52->name).f2;_tmp59;});_tmp58->f2=
_tmp52->tq;_tmp58->f3=(void*)_tmp52->type;_tmp58;});_tmp57->tl=_tmp4E;_tmp57;});
goto L;_LL32:;_LL33:({void*_tmp5A[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp50->loc,({const
char*_tmp5B="nonvariable declaration in parameter type";_tag_arr(_tmp5B,sizeof(
char),_get_zero_arr_size(_tmp5B,42));}),_tag_arr(_tmp5A,sizeof(void*),0));});
_LL2F:;}L: if(_tmp4F == 0)({struct Cyc_String_pa_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp49->hd));{void*
_tmp5C[1]={& _tmp5E};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp5D="%s is not given a type";
_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,23));}),_tag_arr(_tmp5C,
sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*
_tmp5F));_tmp5F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp60=
_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_Function_mod_struct _tmp61;
_tmp61.tag=3;_tmp61.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp62=
_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_WithTypes_struct _tmp63;
_tmp63.tag=1;_tmp63.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4E);_tmp63.f2=0;_tmp63.f3=0;_tmp63.f4=0;_tmp63.f5=0;_tmp63;});_tmp62;}));
_tmp61;});_tmp60;}));_tmp5F->tl=0;_tmp5F;});}_LL2A:;}goto _LL29;_LL28:;_LL29:
return({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=(void*)((
void*)tms->hd);_tmp64->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp64;});
_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp65=
_cycalloc(sizeof(*_tmp65));_tmp65->id=d->id;_tmp65->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp65;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp66=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp66 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp66 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp67="bad storage class on function";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,30));}),loc);goto _LL34;
_LL34:;}}{void*_tmp69;struct Cyc_Core_Opt*_tmp6A;struct _tuple5 _tmp68=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;{struct Cyc_Absyn_Tqual _tmp6C;void*
_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6F;struct _tuple6 _tmp6B=
Cyc_Parse_apply_tms(tq,_tmp69,atts,d->tms);_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;
_tmp6E=_tmp6B.f3;_tmp6F=_tmp6B.f4;if(_tmp6A != 0)({void*_tmp70[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp71="nested type declaration within function prototype";
_tag_arr(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,50));}),_tag_arr(_tmp70,
sizeof(void*),0));});if(_tmp6E != 0)({void*_tmp72[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp73="bad type params, ignoring";_tag_arr(_tmp73,sizeof(char),
_get_zero_arr_size(_tmp73,26));}),_tag_arr(_tmp72,sizeof(void*),0));});{void*
_tmp74=_tmp6D;struct Cyc_Absyn_FnInfo _tmp75;struct Cyc_List_List*_tmp76;struct Cyc_Core_Opt*
_tmp77;void*_tmp78;struct Cyc_List_List*_tmp79;int _tmp7A;struct Cyc_Absyn_VarargInfo*
_tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7D;_LL3C: if(_tmp74 <= (
void*)3  || *((int*)_tmp74)!= 8)goto _LL3E;_tmp75=((struct Cyc_Absyn_FnType_struct*)
_tmp74)->f1;_tmp76=_tmp75.tvars;_tmp77=_tmp75.effect;_tmp78=(void*)_tmp75.ret_typ;
_tmp79=_tmp75.args;_tmp7A=_tmp75.c_varargs;_tmp7B=_tmp75.cyc_varargs;_tmp7C=
_tmp75.rgn_po;_tmp7D=_tmp75.attributes;_LL3D: {struct Cyc_List_List*_tmp7E=0;{
struct Cyc_List_List*_tmp7F=_tmp79;for(0;_tmp7F != 0;_tmp7F=_tmp7F->tl){struct
_tuple2 _tmp81;struct Cyc_Core_Opt*_tmp82;struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;
struct _tuple2*_tmp80=(struct _tuple2*)_tmp7F->hd;_tmp81=*_tmp80;_tmp82=_tmp81.f1;
_tmp83=_tmp81.f2;_tmp84=_tmp81.f3;if(_tmp82 == 0){Cyc_Parse_err(({const char*
_tmp85="missing argument variable in function prototype";_tag_arr(_tmp85,sizeof(
char),_get_zero_arr_size(_tmp85,48));}),loc);_tmp7E=({struct Cyc_List_List*_tmp86=
_cycalloc(sizeof(*_tmp86));_tmp86->hd=({struct _tuple10*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",
sizeof(char),2);_tmp87->f2=_tmp83;_tmp87->f3=_tmp84;_tmp87;});_tmp86->tl=_tmp7E;
_tmp86;});}else{_tmp7E=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));
_tmp89->hd=({struct _tuple10*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=(struct
_tagged_arr*)_tmp82->v;_tmp8A->f2=_tmp83;_tmp8A->f3=_tmp84;_tmp8A;});_tmp89->tl=
_tmp7E;_tmp89;});}}}return({struct Cyc_Absyn_Fndecl*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->sc=(void*)sc;_tmp8B->name=d->id;_tmp8B->tvs=_tmp76;_tmp8B->is_inline=
is_inline;_tmp8B->effect=_tmp77;_tmp8B->ret_type=(void*)_tmp78;_tmp8B->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7E);_tmp8B->c_varargs=
_tmp7A;_tmp8B->cyc_varargs=_tmp7B;_tmp8B->rgn_po=_tmp7C;_tmp8B->body=body;_tmp8B->cached_typ=
0;_tmp8B->param_vardecls=0;_tmp8B->attributes=Cyc_List_append(_tmp7D,_tmp6F);
_tmp8B;});}_LL3E:;_LL3F:({void*_tmp8C[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp8D="declarator is not a function prototype";_tag_arr(_tmp8D,sizeof(char),
_get_zero_arr_size(_tmp8D,39));}),_tag_arr(_tmp8C,sizeof(void*),0));});_LL3B:;}}}}}
static char _tmp8E[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp8E,_tmp8E,_tmp8E + 52};static char
_tmp8F[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp8F,_tmp8F,_tmp8F + 63};static char
_tmp90[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp90,_tmp90,_tmp90 + 50};static char _tmp91[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp91,_tmp91,_tmp91 + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp92=(void*)ts->hd;void*_tmp93;
struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;struct Cyc_Position_Segment*
_tmp96;struct Cyc_Position_Segment*_tmp97;struct Cyc_Position_Segment*_tmp98;
struct Cyc_Absyn_Decl*_tmp99;_LL41: if(*((int*)_tmp92)!= 4)goto _LL43;_tmp93=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp92)->f1;_tmp94=((struct Cyc_Parse_Type_spec_struct*)
_tmp92)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp94);last_loc=
_tmp94;seen_type=1;t=_tmp93;goto _LL40;_LL43: if(*((int*)_tmp92)!= 0)goto _LL45;
_tmp95=((struct Cyc_Parse_Signed_spec_struct*)_tmp92)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp95);if(seen_type)Cyc_Parse_err(({const char*_tmp9A="signed qualifier must come before type";
_tag_arr(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,39));}),_tmp95);last_loc=
_tmp95;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp92)!= 1)goto _LL47;
_tmp96=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp92)->f1;_LL46: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmp96);if(seen_type)Cyc_Parse_err(({const char*
_tmp9B="signed qualifier must come before type";_tag_arr(_tmp9B,sizeof(char),
_get_zero_arr_size(_tmp9B,39));}),_tmp96);last_loc=_tmp96;seen_sign=1;sgn=(void*)
1;goto _LL40;_LL47: if(*((int*)_tmp92)!= 2)goto _LL49;_tmp97=((struct Cyc_Parse_Short_spec_struct*)
_tmp92)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp9C="integral size may appear only once within a type specifier";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,59));}),_tmp97);if(
seen_type)Cyc_Parse_err(({const char*_tmp9D="short modifier must come before base type";
_tag_arr(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,42));}),_tmp97);last_loc=
_tmp97;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp92)!= 3)goto _LL4B;
_tmp98=((struct Cyc_Parse_Long_spec_struct*)_tmp92)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({
const char*_tmp9E="long modifier must come before base type";_tag_arr(_tmp9E,
sizeof(char),_get_zero_arr_size(_tmp9E,41));}),_tmp98);if(seen_size){void*_tmp9F=
sz;_LL4E: if((int)_tmp9F != 2)goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51:
Cyc_Parse_err(({const char*_tmpA0="extra long in type specifier";_tag_arr(_tmpA0,
sizeof(char),_get_zero_arr_size(_tmpA0,29));}),_tmp98);goto _LL4D;_LL4D:;}
last_loc=_tmp98;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp92)!= 5)goto _LL40;
_tmp99=((struct Cyc_Parse_Decl_spec_struct*)_tmp92)->f1;_LL4C: last_loc=_tmp99->loc;
if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpA1=(void*)_tmp99->r;struct
Cyc_Absyn_Aggrdecl*_tmpA2;struct Cyc_Absyn_Tuniondecl*_tmpA3;struct Cyc_Absyn_Enumdecl*
_tmpA4;_LL53: if(*((int*)_tmpA1)!= 4)goto _LL55;_tmpA2=((struct Cyc_Absyn_Aggr_d_struct*)
_tmpA1)->f1;_LL54: {struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA2->tvs));t=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_Absyn_AggrType_struct _tmpA7;_tmpA7.tag=10;_tmpA7.f1=({struct Cyc_Absyn_AggrInfo
_tmpA8;_tmpA8.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(void*)((void*)_tmpA2->kind);_tmpAA.f2=_tmpA2->name;
_tmpAA;});_tmpA9;}));_tmpA8.targs=_tmpA5;_tmpA8;});_tmpA7;});_tmpA6;});if(_tmpA2->impl
!= 0)declopt=({struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->v=
_tmp99;_tmpAB;});goto _LL52;}_LL55: if(*((int*)_tmpA1)!= 5)goto _LL57;_tmpA3=((
struct Cyc_Absyn_Tunion_d_struct*)_tmpA1)->f1;_LL56: {struct Cyc_List_List*_tmpAC=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA3->tvs));t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmpAD=_cycalloc(
sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_TunionType_struct _tmpAE;_tmpAE.tag=
2;_tmpAE.f1=({struct Cyc_Absyn_TunionInfo _tmpAF;_tmpAF.tunion_info=(void*)((void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmpB1;_tmpB1.tag=1;_tmpB1.f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=_tmpA3;_tmpB2;});_tmpB1;});_tmpB0;}));
_tmpAF.targs=_tmpAC;_tmpAF.rgn=(void*)((void*)2);_tmpAF;});_tmpAE;});_tmpAD;});
if(_tmpA3->fields != 0)declopt=({struct Cyc_Core_Opt*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->v=_tmp99;_tmpB3;});goto _LL52;}_LL57: if(*((int*)_tmpA1)!= 6)goto
_LL59;_tmpA4=((struct Cyc_Absyn_Enum_d_struct*)_tmpA1)->f1;_LL58: t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_EnumType_struct _tmpB5;_tmpB5.tag=12;_tmpB5.f1=_tmpA4->name;
_tmpB5.f2=0;_tmpB5;});_tmpB4;});declopt=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(
sizeof(*_tmpB6));_tmpB6->v=_tmp99;_tmpB6;});goto _LL52;_LL59:;_LL5A:({void*_tmpB7[
0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp99->loc,({const char*_tmpB8="bad declaration within type specifier";
_tag_arr(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,38));}),_tag_arr(_tmpB7,
sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp99->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpB9[0]={};
Cyc_Tcutil_warn(last_loc,({const char*_tmpBA="missing type within specifier";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,30));}),_tag_arr(_tmpB9,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_IntType_struct _tmpBC;_tmpBC.tag=5;
_tmpBC.f1=(void*)sgn;_tmpBC.f2=(void*)sz;_tmpBC;});_tmpBB;});}else{if(seen_sign){
void*_tmpBD=t;void*_tmpBE;_LL5C: if(_tmpBD <= (void*)3  || *((int*)_tmpBD)!= 5)goto
_LL5E;_tmpBE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f2;_LL5D: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({
struct Cyc_Absyn_IntType_struct _tmpC0;_tmpC0.tag=5;_tmpC0.f1=(void*)sgn;_tmpC0.f2=(
void*)_tmpBE;_tmpC0;});_tmpBF;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const
char*_tmpC1="sign specification on non-integral type";_tag_arr(_tmpC1,sizeof(
char),_get_zero_arr_size(_tmpC1,40));}),last_loc);goto _LL5B;_LL5B:;}if(seen_size){
void*_tmpC2=t;void*_tmpC3;_LL61: if(_tmpC2 <= (void*)3  || *((int*)_tmpC2)!= 5)goto
_LL63;_tmpC3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC2)->f1;_LL62: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({
struct Cyc_Absyn_IntType_struct _tmpC5;_tmpC5.tag=5;_tmpC5.f1=(void*)_tmpC3;_tmpC5.f2=(
void*)sz;_tmpC5;});_tmpC4;});goto _LL60;_LL63: if(_tmpC2 <= (void*)3  || *((int*)
_tmpC2)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpC6=
_cycalloc_atomic(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC7;_tmpC7.tag=6;_tmpC7.f1=1;_tmpC7;});_tmpC6;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpC8="size qualifier on non-integral type";_tag_arr(_tmpC8,sizeof(
char),_get_zero_arr_size(_tmpC8,36));}),last_loc);goto _LL60;_LL60:;}}return({
struct _tuple5 _tmpC9;_tmpC9.f1=t;_tmpC9.f2=declopt;_tmpC9;});}static struct Cyc_List_List*
Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCA=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCB=_tmpCA->id;
struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*
_tmpD0;struct _tuple6 _tmpCC=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCA->tms);
_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;return({
struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=({struct _tuple7*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->f1=_tmpCB;_tmpD2->f2=_tmpCD;_tmpD2->f3=
_tmpCE;_tmpD2->f4=_tmpCF;_tmpD2->f5=_tmpD0;_tmpD2;});_tmpD1->tl=Cyc_Parse_apply_tmss(
_tmpCD,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD1;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD3;_tmpD3.f1=
tq;_tmpD3.f2=t;_tmpD3.f3=0;_tmpD3.f4=atts;_tmpD3;});{void*_tmpD4=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;void*_tmpD8;struct Cyc_List_List*_tmpD9;struct Cyc_Position_Segment*_tmpDA;
struct Cyc_Absyn_PtrAtts _tmpDB;struct Cyc_Absyn_Tqual _tmpDC;struct Cyc_Position_Segment*
_tmpDD;struct Cyc_List_List*_tmpDE;_LL68: if(*((int*)_tmpD4)!= 0)goto _LL6A;_tmpD5=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpD4)->f1;_LL69: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),Cyc_Absyn_array_typ(t,tq,0,_tmpD5),atts,tms->tl);_LL6A:
if(*((int*)_tmpD4)!= 1)goto _LL6C;_tmpD6=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD4)->f1;_tmpD7=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD4)->f2;_LL6B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmpD6,_tmpD7),atts,tms->tl);_LL6C: if(*((int*)_tmpD4)!= 3)
goto _LL6E;_tmpD8=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpD4)->f1;_LL6D: {
void*_tmpDF=_tmpD8;struct Cyc_List_List*_tmpE0;int _tmpE1;struct Cyc_Absyn_VarargInfo*
_tmpE2;struct Cyc_Core_Opt*_tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_Position_Segment*
_tmpE5;_LL75: if(*((int*)_tmpDF)!= 1)goto _LL77;_tmpE0=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDF)->f1;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpDF)->f2;_tmpE2=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDF)->f3;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDF)->f4;_tmpE4=((struct Cyc_Absyn_WithTypes_struct*)_tmpDF)->f5;_LL76: {
struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*
new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((
void*)as->hd))fn_atts=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));
_tmpE6->hd=(void*)((void*)as->hd);_tmpE6->tl=fn_atts;_tmpE6;});else{new_atts=({
struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=(void*)((void*)
as->hd);_tmpE7->tl=new_atts;_tmpE7;});}}}if(tms->tl != 0){void*_tmpE8=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE9;_LL7A:
if(*((int*)_tmpE8)!= 4)goto _LL7C;_tmpE9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE8)->f1;_LL7B: typvars=_tmpE9;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;
_LL79:;}if(((((!_tmpE1  && _tmpE2 == 0) && _tmpE0 != 0) && _tmpE0->tl == 0) && (*((
struct _tuple2*)_tmpE0->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE0->hd)).f3 == (
void*)0)_tmpE0=0;t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE0);return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(typvars,_tmpE3,t,_tmpE0,_tmpE1,
_tmpE2,_tmpE4,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}
_LL77: if(*((int*)_tmpDF)!= 0)goto _LL74;_tmpE5=((struct Cyc_Absyn_NoTypes_struct*)
_tmpDF)->f2;_LL78:({void*_tmpEA[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpE5,({const char*
_tmpEB="function declaration without parameter types";_tag_arr(_tmpEB,sizeof(
char),_get_zero_arr_size(_tmpEB,45));}),_tag_arr(_tmpEA,sizeof(void*),0));});
_LL74:;}_LL6E: if(*((int*)_tmpD4)!= 4)goto _LL70;_tmpD9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD4)->f1;_tmpDA=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD4)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple6 _tmpEC;_tmpEC.f1=tq;_tmpEC.f2=t;_tmpEC.f3=
_tmpD9;_tmpEC.f4=atts;_tmpEC;});({void*_tmpED[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpDA,({const char*
_tmpEE="type parameters must appear before function arguments in declarator";
_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,68));}),_tag_arr(_tmpED,
sizeof(void*),0));});_LL70: if(*((int*)_tmpD4)!= 2)goto _LL72;_tmpDB=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD4)->f1;_tmpDC=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD4)->f2;_LL71:
return Cyc_Parse_apply_tms(_tmpDC,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_PointerType_struct
_tmpF0;_tmpF0.tag=4;_tmpF0.f1=({struct Cyc_Absyn_PtrInfo _tmpF1;_tmpF1.elt_typ=(
void*)t;_tmpF1.elt_tq=tq;_tmpF1.ptr_atts=_tmpDB;_tmpF1;});_tmpF0;});_tmpEF;}),
atts,tms->tl);_LL72: if(*((int*)_tmpD4)!= 5)goto _LL67;_tmpDD=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD4)->f1;_tmpDE=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD4)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpDE),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct _tuple5 _tmpF2=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;if(_tmpF4 != 0)({void*_tmpF5[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmpF6="ignoring nested type declaration(s) in specifier list";
_tag_arr(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,54));}),_tag_arr(_tmpF5,
sizeof(void*),0));});return _tmpF3;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t){struct _tuple7 _tmpF8;struct
_tuple1*_tmpF9;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpFB;struct Cyc_List_List*
_tmpFC;struct Cyc_List_List*_tmpFD;struct _tuple7*_tmpF7=t;_tmpF8=*_tmpF7;_tmpF9=
_tmpF8.f1;_tmpFA=_tmpF8.f2;_tmpFB=_tmpF8.f3;_tmpFC=_tmpF8.f4;_tmpFD=_tmpF8.f5;
Cyc_Lex_register_typedef(_tmpF9);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmpFE=_tmpFB;struct Cyc_Core_Opt*_tmpFF;_LL7F: if(_tmpFE <= (void*)3
 || *((int*)_tmpFE)!= 0)goto _LL81;_tmpFF=((struct Cyc_Absyn_Evar_struct*)_tmpFE)->f1;
_LL80: type=0;if(_tmpFF == 0)kind=({struct Cyc_Core_Opt*_tmp100=_cycalloc(sizeof(*
_tmp100));_tmp100->v=(void*)((void*)2);_tmp100;});else{kind=_tmpFF;}goto _LL7E;
_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmp101=_cycalloc(sizeof(*_tmp101));
_tmp101->v=(void*)_tmpFB;_tmp101;});goto _LL7E;_LL7E:;}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp102=_cycalloc(sizeof(*_tmp102));
_tmp102[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp103;_tmp103.tag=7;_tmp103.f1=({
struct Cyc_Absyn_Typedefdecl*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->name=
_tmpF9;_tmp104->tvs=_tmpFC;_tmp104->kind=kind;_tmp104->defn=type;_tmp104->atts=
_tmpFD;_tmp104->tq=_tmpFA;_tmp104;});_tmp103;});_tmp102;}),loc);}}static struct
Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Decl_s_struct _tmp106;
_tmp106.tag=12;_tmp106.f1=d;_tmp106.f2=s;_tmp106;});_tmp105;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Declaration_spec
_tmp108;struct Cyc_Absyn_Tqual _tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_List_List*
_tmp10B;struct Cyc_Parse_Declaration_spec*_tmp107=ds;_tmp108=*_tmp107;_tmp109=
_tmp108.tq;_tmp10A=_tmp108.type_specs;_tmp10B=_tmp108.attributes;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp10C="inline is allowed only on function definitions";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,47));}),loc);if(_tmp10A
== 0){Cyc_Parse_err(({const char*_tmp10D="missing type specifiers in declaration";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,39));}),loc);return 0;}{
void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp10E=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp10E != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp10E != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp10E != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp10E != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp10E != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp10E != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp10E != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp110;struct Cyc_List_List*
_tmp111;struct _tuple0 _tmp10F=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmp110=_tmp10F.f1;_tmp111=_tmp10F.f2;{int exps_empty=1;{struct Cyc_List_List*
es=_tmp111;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple5 _tmp112=Cyc_Parse_collapse_type_specifiers(
_tmp10A,loc);if(_tmp110 == 0){void*_tmp114;struct Cyc_Core_Opt*_tmp115;struct
_tuple5 _tmp113=_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;if(_tmp115 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp115->v;{void*_tmp116=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp117;struct Cyc_Absyn_Aggrdecl*_tmp118;struct Cyc_Absyn_Tuniondecl*
_tmp119;_LL93: if(*((int*)_tmp116)!= 6)goto _LL95;_tmp117=((struct Cyc_Absyn_Enum_d_struct*)
_tmp116)->f1;_LL94:(void*)(_tmp117->sc=(void*)s);if(_tmp10B != 0)Cyc_Parse_err(({
const char*_tmp11A="bad attributes on enum";_tag_arr(_tmp11A,sizeof(char),
_get_zero_arr_size(_tmp11A,23));}),loc);goto _LL92;_LL95: if(*((int*)_tmp116)!= 4)
goto _LL97;_tmp118=((struct Cyc_Absyn_Aggr_d_struct*)_tmp116)->f1;_LL96:(void*)(
_tmp118->sc=(void*)s);_tmp118->attributes=_tmp10B;goto _LL92;_LL97: if(*((int*)
_tmp116)!= 5)goto _LL99;_tmp119=((struct Cyc_Absyn_Tunion_d_struct*)_tmp116)->f1;
_LL98:(void*)(_tmp119->sc=(void*)s);if(_tmp10B != 0)Cyc_Parse_err(({const char*
_tmp11B="bad attributes on tunion";_tag_arr(_tmp11B,sizeof(char),
_get_zero_arr_size(_tmp11B,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp11C="bad declaration";_tag_arr(_tmp11C,sizeof(char),
_get_zero_arr_size(_tmp11C,16));}),loc);return 0;_LL92:;}return({struct Cyc_List_List*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=d;_tmp11D->tl=0;_tmp11D;});}
else{void*_tmp11E=_tmp114;struct Cyc_Absyn_AggrInfo _tmp11F;void*_tmp120;void*
_tmp121;struct _tuple1*_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_TunionInfo
_tmp124;void*_tmp125;struct Cyc_Absyn_Tuniondecl**_tmp126;struct Cyc_Absyn_TunionInfo
_tmp127;void*_tmp128;struct Cyc_Absyn_UnknownTunionInfo _tmp129;struct _tuple1*
_tmp12A;int _tmp12B;struct Cyc_List_List*_tmp12C;struct _tuple1*_tmp12D;struct Cyc_List_List*
_tmp12E;_LL9C: if(_tmp11E <= (void*)3  || *((int*)_tmp11E)!= 10)goto _LL9E;_tmp11F=((
struct Cyc_Absyn_AggrType_struct*)_tmp11E)->f1;_tmp120=(void*)_tmp11F.aggr_info;
if(*((int*)_tmp120)!= 0)goto _LL9E;_tmp121=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp120)->f1;_tmp122=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp120)->f2;_tmp123=
_tmp11F.targs;_LL9D: {struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp123);
struct Cyc_Absyn_Aggrdecl*_tmp130=({struct Cyc_Absyn_Aggrdecl*_tmp135=_cycalloc(
sizeof(*_tmp135));_tmp135->kind=(void*)_tmp121;_tmp135->sc=(void*)s;_tmp135->name=
_tmp122;_tmp135->tvs=_tmp12F;_tmp135->impl=0;_tmp135->attributes=0;_tmp135;});
if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp131="bad attributes on type declaration";
_tag_arr(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,35));}),loc);return({
struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp133=_cycalloc(sizeof(*_tmp133));
_tmp133[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp134;_tmp134.tag=4;_tmp134.f1=
_tmp130;_tmp134;});_tmp133;}),loc);_tmp132->tl=0;_tmp132;});}_LL9E: if(_tmp11E <= (
void*)3  || *((int*)_tmp11E)!= 2)goto _LLA0;_tmp124=((struct Cyc_Absyn_TunionType_struct*)
_tmp11E)->f1;_tmp125=(void*)_tmp124.tunion_info;if(*((int*)_tmp125)!= 1)goto
_LLA0;_tmp126=((struct Cyc_Absyn_KnownTunion_struct*)_tmp125)->f1;_LL9F: if(
_tmp10B != 0)Cyc_Parse_err(({const char*_tmp136="bad attributes on tunion";
_tag_arr(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,25));}),loc);return({
struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp138=_cycalloc(sizeof(*_tmp138));
_tmp138[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp139;_tmp139.tag=5;_tmp139.f1=*
_tmp126;_tmp139;});_tmp138;}),loc);_tmp137->tl=0;_tmp137;});_LLA0: if(_tmp11E <= (
void*)3  || *((int*)_tmp11E)!= 2)goto _LLA2;_tmp127=((struct Cyc_Absyn_TunionType_struct*)
_tmp11E)->f1;_tmp128=(void*)_tmp127.tunion_info;if(*((int*)_tmp128)!= 0)goto
_LLA2;_tmp129=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp128)->f1;_tmp12A=
_tmp129.name;_tmp12B=_tmp129.is_xtunion;_tmp12C=_tmp127.targs;_LLA1: {struct Cyc_List_List*
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12C);struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_tunion_decl(s,_tmp12A,
_tmp13A,0,_tmp12B,loc);if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on tunion";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,25));}),loc);return({
struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=_tmp13B;
_tmp13D->tl=0;_tmp13D;});}_LLA2: if(_tmp11E <= (void*)3  || *((int*)_tmp11E)!= 12)
goto _LLA4;_tmp12D=((struct Cyc_Absyn_EnumType_struct*)_tmp11E)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp13E=({struct Cyc_Absyn_Enumdecl*_tmp144=_cycalloc(sizeof(*
_tmp144));_tmp144->sc=(void*)s;_tmp144->name=_tmp12D;_tmp144->fields=0;_tmp144;});
if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp13F="bad attributes on enum";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,23));}),loc);return({
struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=({struct Cyc_Absyn_Decl*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp143;_tmp143.tag=6;_tmp143.f1=_tmp13E;_tmp143;});_tmp142;}));_tmp141->loc=loc;
_tmp141;});_tmp140->tl=0;_tmp140;});}_LLA4: if(_tmp11E <= (void*)3  || *((int*)
_tmp11E)!= 13)goto _LLA6;_tmp12E=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11E)->f1;
_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp145=({struct Cyc_Absyn_Enumdecl*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B->sc=(void*)s;_tmp14B->name=Cyc_Parse_gensym_enum();
_tmp14B->fields=({struct Cyc_Core_Opt*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->v=
_tmp12E;_tmp14C;});_tmp14B;});if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp146="bad attributes on enum";
_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,23));}),loc);return({
struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=({struct Cyc_Absyn_Decl*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14A;_tmp14A.tag=6;_tmp14A.f1=_tmp145;_tmp14A;});_tmp149;}));_tmp148->loc=loc;
_tmp148;});_tmp147->tl=0;_tmp147;});}_LLA6:;_LLA7: Cyc_Parse_err(({const char*
_tmp14D="missing declarator";_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(
_tmp14D,19));}),loc);return 0;_LL9B:;}}else{void*t=_tmp112.f1;struct Cyc_List_List*
_tmp14E=Cyc_Parse_apply_tmss(_tmp109,t,_tmp110,_tmp10B);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp14F="initializer in typedef declaration";
_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,35));}),loc);{struct Cyc_List_List*
decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp14E);if(_tmp112.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp112.f2)->v;{void*_tmp150=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp151;struct Cyc_Absyn_Tuniondecl*_tmp152;struct Cyc_Absyn_Enumdecl*_tmp153;
_LLA9: if(*((int*)_tmp150)!= 4)goto _LLAB;_tmp151=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp150)->f1;_LLAA:(void*)(_tmp151->sc=(void*)s);_tmp151->attributes=_tmp10B;
_tmp10B=0;goto _LLA8;_LLAB: if(*((int*)_tmp150)!= 5)goto _LLAD;_tmp152=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp150)->f1;_LLAC:(void*)(_tmp152->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp150)!= 6)goto _LLAF;_tmp153=((struct Cyc_Absyn_Enum_d_struct*)_tmp150)->f1;
_LLAE:(void*)(_tmp153->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp154="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,70));}),loc);goto _LLA8;
_LLA8:;}decls=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=
d;_tmp155->tl=decls;_tmp155;});}return decls;}}else{if(_tmp112.f2 != 0)Cyc_Parse_unimp(({
const char*_tmp156="nested type declaration within declarator";_tag_arr(_tmp156,
sizeof(char),_get_zero_arr_size(_tmp156,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp157=_tmp14E;for(0;_tmp157 != 0;(_tmp157=_tmp157->tl,
_tmp111=_tmp111->tl)){struct _tuple7 _tmp159;struct _tuple1*_tmp15A;struct Cyc_Absyn_Tqual
_tmp15B;void*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15E;
struct _tuple7*_tmp158=(struct _tuple7*)_tmp157->hd;_tmp159=*_tmp158;_tmp15A=
_tmp159.f1;_tmp15B=_tmp159.f2;_tmp15C=_tmp159.f3;_tmp15D=_tmp159.f4;_tmp15E=
_tmp159.f5;if(_tmp15D != 0)({void*_tmp15F[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp160="bad type params, ignoring";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,26));}),_tag_arr(_tmp15F,sizeof(void*),0));});if(
_tmp111 == 0)({void*_tmp161[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp162="unexpected NULL in parse!";
_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,26));}),_tag_arr(
_tmp161,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp163=(struct Cyc_Absyn_Exp*)
_tmp111->hd;struct Cyc_Absyn_Vardecl*_tmp164=Cyc_Absyn_new_vardecl(_tmp15A,
_tmp15C,_tmp163);_tmp164->tq=_tmp15B;(void*)(_tmp164->sc=(void*)s);_tmp164->attributes=
_tmp15E;{struct Cyc_Absyn_Decl*_tmp165=({struct Cyc_Absyn_Decl*_tmp167=_cycalloc(
sizeof(*_tmp167));_tmp167->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=({struct Cyc_Absyn_Var_d_struct
_tmp169;_tmp169.tag=0;_tmp169.f1=_tmp164;_tmp169;});_tmp168;}));_tmp167->loc=loc;
_tmp167;});decls=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));
_tmp166->hd=_tmp165;_tmp166->tl=decls;_tmp166;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3:
return(void*)2;case 'R': _LLB4: return(void*)3;case 'E': _LLB5: return(void*)4;case 'I':
_LLB6: return(void*)5;default: _LLB7: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp16A[1]={& _tmp16C};Cyc_aprintf(({const char*_tmp16B="bad kind: %s";
_tag_arr(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,13));}),_tag_arr(
_tmp16A,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp16D=(void*)p->r;struct _tuple1*_tmp16E;struct Cyc_Absyn_Vardecl*
_tmp16F;struct Cyc_Absyn_Pat*_tmp170;void*_tmp171;int _tmp172;char _tmp173;struct
_tagged_arr _tmp174;struct _tuple1*_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;_LLBA: if(_tmp16D <= (void*)2  || *((int*)_tmp16D)!= 12)goto _LLBC;_tmp16E=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp16D)->f1;_LLBB: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp178=_cycalloc(sizeof(*_tmp178));
_tmp178[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp179;_tmp179.tag=2;_tmp179.f1=
_tmp16E;_tmp179;});_tmp178;}),p->loc);_LLBC: if(_tmp16D <= (void*)2  || *((int*)
_tmp16D)!= 1)goto _LLBE;_tmp16F=((struct Cyc_Absyn_Reference_p_struct*)_tmp16D)->f1;
_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp17B;_tmp17B.tag=2;_tmp17B.f1=_tmp16F->name;_tmp17B;});_tmp17A;}),p->loc),p->loc);
_LLBE: if(_tmp16D <= (void*)2  || *((int*)_tmp16D)!= 4)goto _LLC0;_tmp170=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp16D)->f1;_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp170),p->loc);_LLC0: if((int)_tmp16D != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(
p->loc);_LLC2: if(_tmp16D <= (void*)2  || *((int*)_tmp16D)!= 7)goto _LLC4;_tmp171=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp16D)->f1;_tmp172=((struct Cyc_Absyn_Int_p_struct*)
_tmp16D)->f2;_LLC3: return Cyc_Absyn_int_exp(_tmp171,_tmp172,p->loc);_LLC4: if(
_tmp16D <= (void*)2  || *((int*)_tmp16D)!= 8)goto _LLC6;_tmp173=((struct Cyc_Absyn_Char_p_struct*)
_tmp16D)->f1;_LLC5: return Cyc_Absyn_char_exp(_tmp173,p->loc);_LLC6: if(_tmp16D <= (
void*)2  || *((int*)_tmp16D)!= 9)goto _LLC8;_tmp174=((struct Cyc_Absyn_Float_p_struct*)
_tmp16D)->f1;_LLC7: return Cyc_Absyn_float_exp(_tmp174,p->loc);_LLC8: if(_tmp16D <= (
void*)2  || *((int*)_tmp16D)!= 13)goto _LLCA;_tmp175=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp16D)->f1;_tmp176=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp16D)->f2;_LLC9: {
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp17D;_tmp17D.tag=2;_tmp17D.f1=_tmp175;_tmp17D;});_tmp17C;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp176);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(_tmp16D <= (void*)2  || *((int*)_tmp16D)!= 14)goto _LLCC;
_tmp177=((struct Cyc_Absyn_Exp_p_struct*)_tmp16D)->f1;_LLCB: return _tmp177;_LLCC:;
_LLCD: Cyc_Parse_err(({const char*_tmp17E="cannot mix patterns and expressions in case";
_tag_arr(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,44));}),p->loc);return
Cyc_Absyn_null_exp(p->loc);_LLB9:;}char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok\000";
struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok\000";
struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;};
char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok\000";
struct Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_AttributeList_tok[
22]="\000\000\000\000AttributeList_tok\000";struct Cyc_AttributeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok\000";
struct Cyc_Attribute_tok_struct{char*tag;void*f1;};char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok\000";
struct Cyc_Bool_tok_struct{char*tag;int f1;};char Cyc_Char_tok[13]="\000\000\000\000Char_tok\000";
struct Cyc_Char_tok_struct{char*tag;char f1;};char Cyc_DeclList_tok[17]="\000\000\000\000DeclList_tok\000";
struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_DeclSpec_tok[
17]="\000\000\000\000DeclSpec_tok\000";struct Cyc_DeclSpec_tok_struct{char*tag;
struct Cyc_Parse_Declaration_spec*f1;};char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok\000";
struct Cyc_Declarator_tok_struct{char*tag;struct Cyc_Parse_Declarator*f1;};char Cyc_DesignatorList_tok[
23]="\000\000\000\000DesignatorList_tok\000";struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok\000";
struct Cyc_Designator_tok_struct{char*tag;void*f1;};char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok\000";
struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Enumfield_tok[
18]="\000\000\000\000Enumfield_tok\000";struct Cyc_Enumfield_tok_struct{char*tag;
struct Cyc_Absyn_Enumfield*f1;};char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok\000";
struct Cyc_ExpList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Exp_tok[
12]="\000\000\000\000Exp_tok\000";struct Cyc_Exp_tok_struct{char*tag;struct Cyc_Absyn_Exp*
f1;};char Cyc_FieldPatternList_tok[25]="\000\000\000\000FieldPatternList_tok\000";
struct Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_FieldPattern_tok[
21]="\000\000\000\000FieldPattern_tok\000";struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{char*tag;struct
_tuple11*f1;};char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok\000";struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok\000";
struct Cyc_IdList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_InitDeclList_tok[
21]="\000\000\000\000InitDeclList_tok\000";struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok\000";
struct _tuple12{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok\000";
struct Cyc_InitializerList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Int_tok[
12]="\000\000\000\000Int_tok\000";struct _tuple13{void*f1;int f2;};struct Cyc_Int_tok_struct{
char*tag;struct _tuple13*f1;};char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok\000";
struct Cyc_Kind_tok_struct{char*tag;void*f1;};char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok\000";
struct _tuple14{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
char*tag;struct _tuple14*f1;};char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok\000";
struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_ParamDecl_tok[
18]="\000\000\000\000ParamDecl_tok\000";struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple2*f1;};char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok\000";
struct Cyc_PatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Pattern_tok[
16]="\000\000\000\000Pattern_tok\000";struct Cyc_Pattern_tok_struct{char*tag;
struct Cyc_Absyn_Pat*f1;};char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok\000";
struct Cyc_Pointer_Sort_tok_struct{char*tag;void*f1;};char Cyc_Primop_tok[15]="\000\000\000\000Primop_tok\000";
struct Cyc_Primop_tok_struct{char*tag;void*f1;};char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok\000";
struct Cyc_Primopopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_QualId_tok[
15]="\000\000\000\000QualId_tok\000";struct Cyc_QualId_tok_struct{char*tag;struct
_tuple1*f1;};char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok\000";
struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};char Cyc_Rgnorder_tok[
17]="\000\000\000\000Rgnorder_tok\000";struct Cyc_Rgnorder_tok_struct{char*tag;
struct Cyc_List_List*f1;};char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok\000";
struct Cyc_Scope_tok_struct{char*tag;void*f1;};char Cyc_Short_tok[14]="\000\000\000\000Short_tok\000";
struct Cyc_Short_tok_struct{char*tag;short f1;};char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok\000";
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};char Cyc_StorageClass_tok[
21]="\000\000\000\000StorageClass_tok\000";struct Cyc_StorageClass_tok_struct{
char*tag;void*f1;};char Cyc_String_tok[15]="\000\000\000\000String_tok\000";
struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};char Cyc_Stringopt_tok[
18]="\000\000\000\000Stringopt_tok\000";struct Cyc_Stringopt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok\000";
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchCClauseList_tok[
26]="\000\000\000\000SwitchCClauseList_tok\000";struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_SwitchClauseList_tok[25]="\000\000\000\000SwitchClauseList_tok\000";
struct Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[
24]="\000\000\000\000TunionFieldList_tok\000";struct Cyc_TunionFieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok\000";
struct Cyc_TunionField_tok_struct{char*tag;struct Cyc_Absyn_Tunionfield*f1;};char
Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok\000";struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[25]="\000\000\000\000TypeModifierList_tok\000";
struct Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[
16]="\000\000\000\000TypeOpt_tok\000";struct Cyc_TypeOpt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok\000";
struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[
22]="\000\000\000\000TypeSpecifier_tok\000";struct Cyc_TypeSpecifier_tok_struct{
char*tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok\000";struct Cyc_Type_tok_struct{
char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1\000";struct _tuple16{
struct Cyc_Absyn_Conref*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_YY1_struct{char*
tag;struct _tuple16*f1;};char Cyc_YY2[8]="\000\000\000\000YY2\000";struct Cyc_YY2_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY3[8]="\000\000\000\000YY3\000";
struct Cyc_YY3_struct{char*tag;struct _tuple14*f1;};char Cyc_YY4[8]="\000\000\000\000YY4\000";
struct Cyc_YY4_struct{char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YYINITIALSVAL[
18]="\000\000\000\000YYINITIALSVAL\000";char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";
static char _tmp180[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp180,_tmp180,_tmp180 + 15}};struct _tuple13*Cyc_yyget_Int_tok(
void*yy1){struct _tuple13*yyzzz;{void*_tmp181=yy1;struct _tuple13*_tmp182;_LLCF:
if(*((void**)_tmp181)!= Cyc_Int_tok)goto _LLD1;_tmp182=((struct Cyc_Int_tok_struct*)
_tmp181)->f1;_LLD0: yyzzz=_tmp182;goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLCE:;}return yyzzz;}static char _tmp184[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp184,_tmp184,_tmp184 + 5}};char Cyc_yyget_Char_tok(
void*yy1){char yyzzz;{void*_tmp185=yy1;char _tmp186;_LLD4: if(*((void**)_tmp185)!= 
Cyc_Char_tok)goto _LLD6;_tmp186=((struct Cyc_Char_tok_struct*)_tmp185)->f1;_LLD5:
yyzzz=_tmp186;goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLD3:;}return yyzzz;}static char _tmp188[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp188,_tmp188,_tmp188 + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp189=
yy1;struct _tagged_arr _tmp18A;_LLD9: if(*((void**)_tmp189)!= Cyc_String_tok)goto
_LLDB;_tmp18A=((struct Cyc_String_tok_struct*)_tmp189)->f1;_LLDA: yyzzz=_tmp18A;
goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLD8:;}return
yyzzz;}static char _tmp18C[38]="$(conref_t<bool>,conref_t<bounds_t>)@";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp18C,_tmp18C,
_tmp18C + 38}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*yyzzz;{void*
_tmp18D=yy1;struct _tuple16*_tmp18E;_LLDE: if(*((void**)_tmp18D)!= Cyc_YY1)goto
_LLE0;_tmp18E=((struct Cyc_YY1_struct*)_tmp18D)->f1;_LLDF: yyzzz=_tmp18E;goto _LLDD;
_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}static char
_tmp190[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp190,_tmp190,_tmp190 + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(
void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp191=yy1;struct Cyc_Absyn_Conref*
_tmp192;_LLE3: if(*((void**)_tmp191)!= Cyc_YY2)goto _LLE5;_tmp192=((struct Cyc_YY2_struct*)
_tmp191)->f1;_LLE4: yyzzz=_tmp192;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLE2:;}return yyzzz;}static char _tmp194[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp194,_tmp194,_tmp194 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp195=yy1;struct
Cyc_Absyn_Exp*_tmp196;_LLE8: if(*((void**)_tmp195)!= Cyc_Exp_tok)goto _LLEA;
_tmp196=((struct Cyc_Exp_tok_struct*)_tmp195)->f1;_LLE9: yyzzz=_tmp196;goto _LLE7;
_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLE7:;}return yyzzz;}static
char _tmp198[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp199=yy1;struct Cyc_List_List*
_tmp19A;_LLED: if(*((void**)_tmp199)!= Cyc_ExpList_tok)goto _LLEF;_tmp19A=((struct
Cyc_ExpList_tok_struct*)_tmp199)->f1;_LLEE: yyzzz=_tmp19A;goto _LLEC;_LLEF:;_LLF0:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLEC:;}return yyzzz;}static char
_tmp19C[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp19C,_tmp19C,_tmp19C + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp19D=yy1;struct Cyc_List_List*_tmp19E;_LLF2: if(*((void**)_tmp19D)
!= Cyc_InitializerList_tok)goto _LLF4;_tmp19E=((struct Cyc_InitializerList_tok_struct*)
_tmp19D)->f1;_LLF3: yyzzz=_tmp19E;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLF1:;}return yyzzz;}static char _tmp1A0[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp1A0,_tmp1A0,_tmp1A0 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp1A1=yy1;void*_tmp1A2;_LLF7: if(*((void**)_tmp1A1)
!= Cyc_Primop_tok)goto _LLF9;_tmp1A2=(void*)((struct Cyc_Primop_tok_struct*)
_tmp1A1)->f1;_LLF8: yyzzz=_tmp1A2;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLF6:;}return yyzzz;}static char _tmp1A4[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp1A4,_tmp1A4,_tmp1A4 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp1A5=yy1;struct Cyc_Core_Opt*_tmp1A6;_LLFC: if(*((void**)_tmp1A5)!= Cyc_Primopopt_tok)
goto _LLFE;_tmp1A6=((struct Cyc_Primopopt_tok_struct*)_tmp1A5)->f1;_LLFD: yyzzz=
_tmp1A6;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLFB:;}return yyzzz;}static char _tmp1A8[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8,_tmp1A8 + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp1A9=yy1;
struct _tuple1*_tmp1AA;_LL101: if(*((void**)_tmp1A9)!= Cyc_QualId_tok)goto _LL103;
_tmp1AA=((struct Cyc_QualId_tok_struct*)_tmp1A9)->f1;_LL102: yyzzz=_tmp1AA;goto
_LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL100:;}return
yyzzz;}static char _tmp1AC[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1AC,_tmp1AC,_tmp1AC + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1AD=yy1;struct Cyc_Absyn_Stmt*
_tmp1AE;_LL106: if(*((void**)_tmp1AD)!= Cyc_Stmt_tok)goto _LL108;_tmp1AE=((struct
Cyc_Stmt_tok_struct*)_tmp1AD)->f1;_LL107: yyzzz=_tmp1AE;goto _LL105;_LL108:;_LL109:(
int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL105:;}return yyzzz;}static char _tmp1B0[
24]="list_t<switch_clause_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B1=yy1;struct Cyc_List_List*
_tmp1B2;_LL10B: if(*((void**)_tmp1B1)!= Cyc_SwitchClauseList_tok)goto _LL10D;
_tmp1B2=((struct Cyc_SwitchClauseList_tok_struct*)_tmp1B1)->f1;_LL10C: yyzzz=
_tmp1B2;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL10A:;}return yyzzz;}static char _tmp1B4[25]="list_t<switchC_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp1B4,_tmp1B4,_tmp1B4 + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B5=yy1;struct Cyc_List_List*
_tmp1B6;_LL110: if(*((void**)_tmp1B5)!= Cyc_SwitchCClauseList_tok)goto _LL112;
_tmp1B6=((struct Cyc_SwitchCClauseList_tok_struct*)_tmp1B5)->f1;_LL111: yyzzz=
_tmp1B6;goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL10F:;}return yyzzz;}static char _tmp1B8[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1B8,_tmp1B8,_tmp1B8 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1B9=yy1;
struct Cyc_Absyn_Pat*_tmp1BA;_LL115: if(*((void**)_tmp1B9)!= Cyc_Pattern_tok)goto
_LL117;_tmp1BA=((struct Cyc_Pattern_tok_struct*)_tmp1B9)->f1;_LL116: yyzzz=_tmp1BA;
goto _LL114;_LL117:;_LL118:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL114:;}
return yyzzz;}static char _tmp1BC[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;_LL11A: if(*((void**)_tmp1BD)
!= Cyc_PatternList_tok)goto _LL11C;_tmp1BE=((struct Cyc_PatternList_tok_struct*)
_tmp1BD)->f1;_LL11B: yyzzz=_tmp1BE;goto _LL119;_LL11C:;_LL11D:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL119:;}return yyzzz;}static char _tmp1C0[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1C0,_tmp1C0,_tmp1C0 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1C1=yy1;struct _tuple11*_tmp1C2;_LL11F: if(*((void**)
_tmp1C1)!= Cyc_FieldPattern_tok)goto _LL121;_tmp1C2=((struct Cyc_FieldPattern_tok_struct*)
_tmp1C1)->f1;_LL120: yyzzz=_tmp1C2;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL11E:;}return yyzzz;}static char _tmp1C4[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1C4,_tmp1C4,_tmp1C4 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C5=yy1;struct Cyc_List_List*
_tmp1C6;_LL124: if(*((void**)_tmp1C5)!= Cyc_FieldPatternList_tok)goto _LL126;
_tmp1C6=((struct Cyc_FieldPatternList_tok_struct*)_tmp1C5)->f1;_LL125: yyzzz=
_tmp1C6;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL123:;}return yyzzz;}static char _tmp1C8[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1C8,_tmp1C8,_tmp1C8 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1C9=yy1;
struct Cyc_Absyn_Fndecl*_tmp1CA;_LL129: if(*((void**)_tmp1C9)!= Cyc_FnDecl_tok)
goto _LL12B;_tmp1CA=((struct Cyc_FnDecl_tok_struct*)_tmp1C9)->f1;_LL12A: yyzzz=
_tmp1CA;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL128:;}return yyzzz;}static char _tmp1CC[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1CC,_tmp1CC,_tmp1CC + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1CD=yy1;struct Cyc_List_List*_tmp1CE;_LL12E: if(*((void**)_tmp1CD)!= Cyc_DeclList_tok)
goto _LL130;_tmp1CE=((struct Cyc_DeclList_tok_struct*)_tmp1CD)->f1;_LL12F: yyzzz=
_tmp1CE;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL12D:;}return yyzzz;}static char _tmp1D0[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1D0,_tmp1D0,_tmp1D0 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1D1=yy1;struct Cyc_Parse_Declaration_spec*_tmp1D2;_LL133: if(*((
void**)_tmp1D1)!= Cyc_DeclSpec_tok)goto _LL135;_tmp1D2=((struct Cyc_DeclSpec_tok_struct*)
_tmp1D1)->f1;_LL134: yyzzz=_tmp1D2;goto _LL132;_LL135:;_LL136:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL132:;}return yyzzz;}static char _tmp1D4[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1D4,_tmp1D4,_tmp1D4 + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1D5=yy1;struct _tuple12*_tmp1D6;_LL138: if(*((void**)
_tmp1D5)!= Cyc_InitDecl_tok)goto _LL13A;_tmp1D6=((struct Cyc_InitDecl_tok_struct*)
_tmp1D5)->f1;_LL139: yyzzz=_tmp1D6;goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL137:;}return yyzzz;}static char _tmp1D8[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1D8,_tmp1D8,_tmp1D8 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D9=yy1;struct Cyc_List_List*
_tmp1DA;_LL13D: if(*((void**)_tmp1D9)!= Cyc_InitDeclList_tok)goto _LL13F;_tmp1DA=((
struct Cyc_InitDeclList_tok_struct*)_tmp1D9)->f1;_LL13E: yyzzz=_tmp1DA;goto _LL13C;
_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL13C:;}return
yyzzz;}static char _tmp1DC[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1DC,_tmp1DC,_tmp1DC + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1DD=yy1;void*
_tmp1DE;_LL142: if(*((void**)_tmp1DD)!= Cyc_StorageClass_tok)goto _LL144;_tmp1DE=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1DD)->f1;_LL143: yyzzz=_tmp1DE;
goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL141:;}return yyzzz;}static char _tmp1E0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1E1=yy1;void*
_tmp1E2;_LL147: if(*((void**)_tmp1E1)!= Cyc_TypeSpecifier_tok)goto _LL149;_tmp1E2=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1E1)->f1;_LL148: yyzzz=_tmp1E2;
goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL146:;}return yyzzz;}static char _tmp1E4[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1E5=yy1;void*
_tmp1E6;_LL14C: if(*((void**)_tmp1E5)!= Cyc_StructOrUnion_tok)goto _LL14E;_tmp1E6=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1E5)->f1;_LL14D: yyzzz=_tmp1E6;
goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL14B:;}return yyzzz;}static char _tmp1E8[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1E9=yy1;struct Cyc_Absyn_Tqual _tmp1EA;_LL151: if(*((void**)_tmp1E9)!= Cyc_TypeQual_tok)
goto _LL153;_tmp1EA=((struct Cyc_TypeQual_tok_struct*)_tmp1E9)->f1;_LL152: yyzzz=
_tmp1EA;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL150:;}return yyzzz;}static char _tmp1EC[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1EC,_tmp1EC,_tmp1EC + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1ED=yy1;struct Cyc_List_List*
_tmp1EE;_LL156: if(*((void**)_tmp1ED)!= Cyc_AggrFieldDeclList_tok)goto _LL158;
_tmp1EE=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1ED)->f1;_LL157: yyzzz=
_tmp1EE;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL155:;}return yyzzz;}static char _tmp1F0[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1F0,_tmp1F0,_tmp1F0 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F1=yy1;struct Cyc_List_List*
_tmp1F2;_LL15B: if(*((void**)_tmp1F1)!= Cyc_AggrFieldDeclListList_tok)goto _LL15D;
_tmp1F2=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1F1)->f1;_LL15C: yyzzz=
_tmp1F2;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL15A:;}return yyzzz;}static char _tmp1F4[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1F4,_tmp1F4,_tmp1F4 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F5=yy1;struct Cyc_List_List*
_tmp1F6;_LL160: if(*((void**)_tmp1F5)!= Cyc_TypeModifierList_tok)goto _LL162;
_tmp1F6=((struct Cyc_TypeModifierList_tok_struct*)_tmp1F5)->f1;_LL161: yyzzz=
_tmp1F6;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL15F:;}return yyzzz;}static char _tmp1F8[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1F9=yy1;struct Cyc_Parse_Declarator*_tmp1FA;_LL165: if(*((void**)
_tmp1F9)!= Cyc_Declarator_tok)goto _LL167;_tmp1FA=((struct Cyc_Declarator_tok_struct*)
_tmp1F9)->f1;_LL166: yyzzz=_tmp1FA;goto _LL164;_LL167:;_LL168:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL164:;}return yyzzz;}static char _tmp1FC[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1FC,_tmp1FC,_tmp1FC + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1FD=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1FE;_LL16A: if(*((void**)_tmp1FD)!= Cyc_AbstractDeclarator_tok)goto _LL16C;
_tmp1FE=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1FD)->f1;_LL16B: yyzzz=
_tmp1FE;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL169:;}return yyzzz;}static char _tmp200[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp200,_tmp200,_tmp200 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp201=yy1;int _tmp202;_LL16F: if(*((void**)_tmp201)!= 
Cyc_Bool_tok)goto _LL171;_tmp202=((struct Cyc_Bool_tok_struct*)_tmp201)->f1;_LL170:
yyzzz=_tmp202;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL16E:;}return yyzzz;}static char _tmp204[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp205=yy1;void*_tmp206;_LL174: if(*((void**)_tmp205)
!= Cyc_Scope_tok)goto _LL176;_tmp206=(void*)((struct Cyc_Scope_tok_struct*)_tmp205)->f1;
_LL175: yyzzz=_tmp206;goto _LL173;_LL176:;_LL177:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL173:;}return yyzzz;}static char _tmp208[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp209=yy1;struct Cyc_Absyn_Tunionfield*_tmp20A;_LL179: if(*((void**)
_tmp209)!= Cyc_TunionField_tok)goto _LL17B;_tmp20A=((struct Cyc_TunionField_tok_struct*)
_tmp209)->f1;_LL17A: yyzzz=_tmp20A;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL178:;}return yyzzz;}static char _tmp20C[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp20C,_tmp20C,_tmp20C + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20D=yy1;struct Cyc_List_List*
_tmp20E;_LL17E: if(*((void**)_tmp20D)!= Cyc_TunionFieldList_tok)goto _LL180;
_tmp20E=((struct Cyc_TunionFieldList_tok_struct*)_tmp20D)->f1;_LL17F: yyzzz=
_tmp20E;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL17D:;}return yyzzz;}static char _tmp210[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp210,_tmp210,_tmp210 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp211=yy1;struct _tuple15*_tmp212;_LL183: if(*((void**)
_tmp211)!= Cyc_QualSpecList_tok)goto _LL185;_tmp212=((struct Cyc_QualSpecList_tok_struct*)
_tmp211)->f1;_LL184: yyzzz=_tmp212;goto _LL182;_LL185:;_LL186:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL182:;}return yyzzz;}static char _tmp214[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp214,_tmp214,_tmp214 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp215=yy1;struct Cyc_List_List*_tmp216;_LL188:
if(*((void**)_tmp215)!= Cyc_IdList_tok)goto _LL18A;_tmp216=((struct Cyc_IdList_tok_struct*)
_tmp215)->f1;_LL189: yyzzz=_tmp216;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL187:;}return yyzzz;}static char _tmp218[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp218,_tmp218,_tmp218 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp219=yy1;struct _tuple2*_tmp21A;_LL18D: if(*((void**)
_tmp219)!= Cyc_ParamDecl_tok)goto _LL18F;_tmp21A=((struct Cyc_ParamDecl_tok_struct*)
_tmp219)->f1;_LL18E: yyzzz=_tmp21A;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL18C:;}return yyzzz;}static char _tmp21C[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp21C,_tmp21C,_tmp21C + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp21D=yy1;struct Cyc_List_List*
_tmp21E;_LL192: if(*((void**)_tmp21D)!= Cyc_ParamDeclList_tok)goto _LL194;_tmp21E=((
struct Cyc_ParamDeclList_tok_struct*)_tmp21D)->f1;_LL193: yyzzz=_tmp21E;goto _LL191;
_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL191:;}return
yyzzz;}static char _tmp220[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp220,
_tmp220,_tmp220 + 107}};struct _tuple14*Cyc_yyget_YY3(void*yy1){struct _tuple14*
yyzzz;{void*_tmp221=yy1;struct _tuple14*_tmp222;_LL197: if(*((void**)_tmp221)!= 
Cyc_YY3)goto _LL199;_tmp222=((struct Cyc_YY3_struct*)_tmp221)->f1;_LL198: yyzzz=
_tmp222;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_YY3);_LL196:;}
return yyzzz;}static char _tmp224[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp225=yy1;struct Cyc_List_List*_tmp226;_LL19C: if(*((void**)_tmp225)!= Cyc_TypeList_tok)
goto _LL19E;_tmp226=((struct Cyc_TypeList_tok_struct*)_tmp225)->f1;_LL19D: yyzzz=
_tmp226;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL19B:;}return yyzzz;}static char _tmp228[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp228,
_tmp228,_tmp228 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp229=yy1;struct Cyc_List_List*_tmp22A;_LL1A1:
if(*((void**)_tmp229)!= Cyc_DesignatorList_tok)goto _LL1A3;_tmp22A=((struct Cyc_DesignatorList_tok_struct*)
_tmp229)->f1;_LL1A2: yyzzz=_tmp22A;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL1A0:;}return yyzzz;}static char _tmp22C[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp22C,_tmp22C,_tmp22C + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp22D=yy1;void*_tmp22E;_LL1A6: if(*((void**)_tmp22D)!= Cyc_Designator_tok)
goto _LL1A8;_tmp22E=(void*)((struct Cyc_Designator_tok_struct*)_tmp22D)->f1;_LL1A7:
yyzzz=_tmp22E;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL1A5:;}return yyzzz;}static char _tmp230[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp231=yy1;void*_tmp232;_LL1AB: if(*((void**)_tmp231)
!= Cyc_Kind_tok)goto _LL1AD;_tmp232=(void*)((struct Cyc_Kind_tok_struct*)_tmp231)->f1;
_LL1AC: yyzzz=_tmp232;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL1AA:;}return yyzzz;}static char _tmp234[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp235=yy1;void*_tmp236;_LL1B0: if(*((void**)_tmp235)
!= Cyc_Type_tok)goto _LL1B2;_tmp236=(void*)((struct Cyc_Type_tok_struct*)_tmp235)->f1;
_LL1B1: yyzzz=_tmp236;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL1AF:;}return yyzzz;}static char _tmp238[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp238,
_tmp238,_tmp238 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp239=yy1;struct Cyc_List_List*_tmp23A;_LL1B5:
if(*((void**)_tmp239)!= Cyc_AttributeList_tok)goto _LL1B7;_tmp23A=((struct Cyc_AttributeList_tok_struct*)
_tmp239)->f1;_LL1B6: yyzzz=_tmp23A;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1B4:;}return yyzzz;}static char _tmp23C[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp23C,_tmp23C,_tmp23C + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp23D=yy1;void*_tmp23E;_LL1BA: if(*((void**)_tmp23D)!= Cyc_Attribute_tok)
goto _LL1BC;_tmp23E=(void*)((struct Cyc_Attribute_tok_struct*)_tmp23D)->f1;_LL1BB:
yyzzz=_tmp23E;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1B9:;}return yyzzz;}static char _tmp240[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp241=yy1;struct Cyc_Absyn_Enumfield*_tmp242;_LL1BF: if(*((void**)
_tmp241)!= Cyc_Enumfield_tok)goto _LL1C1;_tmp242=((struct Cyc_Enumfield_tok_struct*)
_tmp241)->f1;_LL1C0: yyzzz=_tmp242;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1BE:;}return yyzzz;}static char _tmp244[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp244,_tmp244,_tmp244 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp245=yy1;struct Cyc_List_List*
_tmp246;_LL1C4: if(*((void**)_tmp245)!= Cyc_EnumfieldList_tok)goto _LL1C6;_tmp246=((
struct Cyc_EnumfieldList_tok_struct*)_tmp245)->f1;_LL1C5: yyzzz=_tmp246;goto _LL1C3;
_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1C3:;}return
yyzzz;}static char _tmp248[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp249=yy1;struct Cyc_Core_Opt*_tmp24A;_LL1C9: if(*((void**)_tmp249)!= Cyc_TypeOpt_tok)
goto _LL1CB;_tmp24A=((struct Cyc_TypeOpt_tok_struct*)_tmp249)->f1;_LL1CA: yyzzz=
_tmp24A;goto _LL1C8;_LL1CB:;_LL1CC:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1C8:;}return yyzzz;}static char _tmp24C[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp24C,
_tmp24C,_tmp24C + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp24D=yy1;struct Cyc_List_List*_tmp24E;_LL1CE:
if(*((void**)_tmp24D)!= Cyc_Rgnorder_tok)goto _LL1D0;_tmp24E=((struct Cyc_Rgnorder_tok_struct*)
_tmp24D)->f1;_LL1CF: yyzzz=_tmp24E;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1CD:;}return yyzzz;}static char _tmp250[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp250,
_tmp250,_tmp250 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY4(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp251=yy1;struct Cyc_Absyn_Conref*_tmp252;_LL1D3: if(*((void**)
_tmp251)!= Cyc_YY4)goto _LL1D5;_tmp252=((struct Cyc_YY4_struct*)_tmp251)->f1;
_LL1D4: yyzzz=_tmp252;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL1D2:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp253;_tmp253.timestamp=0;_tmp253.first_line=0;
_tmp253.first_column=0;_tmp253.last_line=0;_tmp253.last_column=0;_tmp253;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[359]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,130,2,2,112,128,125,2,
108,109,118,122,110,126,115,127,2,2,2,2,2,2,2,2,2,2,114,104,113,107,121,120,119,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,116,2,117,124,111,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,105,123,106,129,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp254[
2]="$";static char _tmp255[6]="error";static char _tmp256[12]="$undefined.";static
char _tmp257[5]="AUTO";static char _tmp258[9]="REGISTER";static char _tmp259[7]="STATIC";
static char _tmp25A[7]="EXTERN";static char _tmp25B[8]="TYPEDEF";static char _tmp25C[5]="VOID";
static char _tmp25D[5]="CHAR";static char _tmp25E[6]="SHORT";static char _tmp25F[4]="INT";
static char _tmp260[5]="LONG";static char _tmp261[6]="FLOAT";static char _tmp262[7]="DOUBLE";
static char _tmp263[7]="SIGNED";static char _tmp264[9]="UNSIGNED";static char _tmp265[
6]="CONST";static char _tmp266[9]="VOLATILE";static char _tmp267[9]="RESTRICT";
static char _tmp268[7]="STRUCT";static char _tmp269[6]="UNION";static char _tmp26A[5]="CASE";
static char _tmp26B[8]="DEFAULT";static char _tmp26C[7]="INLINE";static char _tmp26D[7]="SIZEOF";
static char _tmp26E[9]="OFFSETOF";static char _tmp26F[3]="IF";static char _tmp270[5]="ELSE";
static char _tmp271[7]="SWITCH";static char _tmp272[6]="WHILE";static char _tmp273[3]="DO";
static char _tmp274[4]="FOR";static char _tmp275[5]="GOTO";static char _tmp276[9]="CONTINUE";
static char _tmp277[6]="BREAK";static char _tmp278[7]="RETURN";static char _tmp279[5]="ENUM";
static char _tmp27A[8]="NULL_kw";static char _tmp27B[4]="LET";static char _tmp27C[6]="THROW";
static char _tmp27D[4]="TRY";static char _tmp27E[6]="CATCH";static char _tmp27F[4]="NEW";
static char _tmp280[9]="ABSTRACT";static char _tmp281[9]="FALLTHRU";static char
_tmp282[6]="USING";static char _tmp283[10]="NAMESPACE";static char _tmp284[7]="TUNION";
static char _tmp285[8]="XTUNION";static char _tmp286[9]="FORARRAY";static char _tmp287[
5]="FILL";static char _tmp288[8]="CODEGEN";static char _tmp289[4]="CUT";static char
_tmp28A[7]="SPLICE";static char _tmp28B[7]="MALLOC";static char _tmp28C[8]="RMALLOC";
static char _tmp28D[7]="CALLOC";static char _tmp28E[8]="RCALLOC";static char _tmp28F[9]="REGION_T";
static char _tmp290[9]="SIZEOF_T";static char _tmp291[6]="TAG_T";static char _tmp292[7]="REGION";
static char _tmp293[5]="RNEW";static char _tmp294[8]="REGIONS";static char _tmp295[13]="RESET_REGION";
static char _tmp296[4]="GEN";static char _tmp297[14]="NOZEROTERM_kw";static char
_tmp298[12]="ZEROTERM_kw";static char _tmp299[7]="PTR_OP";static char _tmp29A[7]="INC_OP";
static char _tmp29B[7]="DEC_OP";static char _tmp29C[8]="LEFT_OP";static char _tmp29D[9]="RIGHT_OP";
static char _tmp29E[6]="LE_OP";static char _tmp29F[6]="GE_OP";static char _tmp2A0[6]="EQ_OP";
static char _tmp2A1[6]="NE_OP";static char _tmp2A2[7]="AND_OP";static char _tmp2A3[6]="OR_OP";
static char _tmp2A4[11]="MUL_ASSIGN";static char _tmp2A5[11]="DIV_ASSIGN";static char
_tmp2A6[11]="MOD_ASSIGN";static char _tmp2A7[11]="ADD_ASSIGN";static char _tmp2A8[11]="SUB_ASSIGN";
static char _tmp2A9[12]="LEFT_ASSIGN";static char _tmp2AA[13]="RIGHT_ASSIGN";static
char _tmp2AB[11]="AND_ASSIGN";static char _tmp2AC[11]="XOR_ASSIGN";static char
_tmp2AD[10]="OR_ASSIGN";static char _tmp2AE[9]="ELLIPSIS";static char _tmp2AF[11]="LEFT_RIGHT";
static char _tmp2B0[12]="COLON_COLON";static char _tmp2B1[11]="IDENTIFIER";static
char _tmp2B2[17]="INTEGER_CONSTANT";static char _tmp2B3[7]="STRING";static char
_tmp2B4[19]="CHARACTER_CONSTANT";static char _tmp2B5[18]="FLOATING_CONSTANT";
static char _tmp2B6[9]="TYPE_VAR";static char _tmp2B7[13]="TYPEDEF_NAME";static char
_tmp2B8[16]="QUAL_IDENTIFIER";static char _tmp2B9[18]="QUAL_TYPEDEF_NAME";static
char _tmp2BA[13]="TYPE_INTEGER";static char _tmp2BB[10]="ATTRIBUTE";static char
_tmp2BC[4]="';'";static char _tmp2BD[4]="'{'";static char _tmp2BE[4]="'}'";static
char _tmp2BF[4]="'='";static char _tmp2C0[4]="'('";static char _tmp2C1[4]="')'";
static char _tmp2C2[4]="','";static char _tmp2C3[4]="'_'";static char _tmp2C4[4]="'$'";
static char _tmp2C5[4]="'<'";static char _tmp2C6[4]="':'";static char _tmp2C7[4]="'.'";
static char _tmp2C8[4]="'['";static char _tmp2C9[4]="']'";static char _tmp2CA[4]="'*'";
static char _tmp2CB[4]="'@'";static char _tmp2CC[4]="'?'";static char _tmp2CD[4]="'>'";
static char _tmp2CE[4]="'+'";static char _tmp2CF[4]="'|'";static char _tmp2D0[4]="'^'";
static char _tmp2D1[4]="'&'";static char _tmp2D2[4]="'-'";static char _tmp2D3[4]="'/'";
static char _tmp2D4[4]="'%'";static char _tmp2D5[4]="'~'";static char _tmp2D6[4]="'!'";
static char _tmp2D7[5]="prog";static char _tmp2D8[17]="translation_unit";static char
_tmp2D9[21]="external_declaration";static char _tmp2DA[20]="function_definition";
static char _tmp2DB[21]="function_definition2";static char _tmp2DC[13]="using_action";
static char _tmp2DD[15]="unusing_action";static char _tmp2DE[17]="namespace_action";
static char _tmp2DF[19]="unnamespace_action";static char _tmp2E0[12]="declaration";
static char _tmp2E1[17]="declaration_list";static char _tmp2E2[23]="declaration_specifiers";
static char _tmp2E3[24]="storage_class_specifier";static char _tmp2E4[15]="attributes_opt";
static char _tmp2E5[11]="attributes";static char _tmp2E6[15]="attribute_list";static
char _tmp2E7[10]="attribute";static char _tmp2E8[15]="type_specifier";static char
_tmp2E9[5]="kind";static char _tmp2EA[15]="type_qualifier";static char _tmp2EB[15]="enum_specifier";
static char _tmp2EC[11]="enum_field";static char _tmp2ED[22]="enum_declaration_list";
static char _tmp2EE[26]="struct_or_union_specifier";static char _tmp2EF[16]="type_params_opt";
static char _tmp2F0[16]="struct_or_union";static char _tmp2F1[24]="struct_declaration_list";
static char _tmp2F2[25]="struct_declaration_list0";static char _tmp2F3[21]="init_declarator_list";
static char _tmp2F4[22]="init_declarator_list0";static char _tmp2F5[16]="init_declarator";
static char _tmp2F6[19]="struct_declaration";static char _tmp2F7[25]="specifier_qualifier_list";
static char _tmp2F8[23]="struct_declarator_list";static char _tmp2F9[24]="struct_declarator_list0";
static char _tmp2FA[18]="struct_declarator";static char _tmp2FB[17]="tunion_specifier";
static char _tmp2FC[18]="tunion_or_xtunion";static char _tmp2FD[17]="tunionfield_list";
static char _tmp2FE[18]="tunionfield_scope";static char _tmp2FF[12]="tunionfield";
static char _tmp300[11]="declarator";static char _tmp301[18]="direct_declarator";
static char _tmp302[8]="pointer";static char _tmp303[12]="one_pointer";static char
_tmp304[23]="pointer_null_and_bound";static char _tmp305[14]="pointer_bound";
static char _tmp306[18]="zeroterm_qual_opt";static char _tmp307[8]="rgn_opt";static
char _tmp308[11]="tqual_list";static char _tmp309[20]="parameter_type_list";static
char _tmp30A[9]="type_var";static char _tmp30B[16]="optional_effect";static char
_tmp30C[19]="optional_rgn_order";static char _tmp30D[10]="rgn_order";static char
_tmp30E[16]="optional_inject";static char _tmp30F[11]="effect_set";static char
_tmp310[14]="atomic_effect";static char _tmp311[11]="region_set";static char _tmp312[
15]="parameter_list";static char _tmp313[22]="parameter_declaration";static char
_tmp314[16]="identifier_list";static char _tmp315[17]="identifier_list0";static
char _tmp316[12]="initializer";static char _tmp317[18]="array_initializer";static
char _tmp318[17]="initializer_list";static char _tmp319[12]="designation";static
char _tmp31A[16]="designator_list";static char _tmp31B[11]="designator";static char
_tmp31C[10]="type_name";static char _tmp31D[14]="any_type_name";static char _tmp31E[
15]="type_name_list";static char _tmp31F[20]="abstract_declarator";static char
_tmp320[27]="direct_abstract_declarator";static char _tmp321[10]="statement";
static char _tmp322[18]="labeled_statement";static char _tmp323[21]="expression_statement";
static char _tmp324[19]="compound_statement";static char _tmp325[16]="block_item_list";
static char _tmp326[20]="selection_statement";static char _tmp327[15]="switch_clauses";
static char _tmp328[16]="switchC_clauses";static char _tmp329[20]="iteration_statement";
static char _tmp32A[15]="jump_statement";static char _tmp32B[12]="exp_pattern";
static char _tmp32C[20]="conditional_pattern";static char _tmp32D[19]="logical_or_pattern";
static char _tmp32E[20]="logical_and_pattern";static char _tmp32F[21]="inclusive_or_pattern";
static char _tmp330[21]="exclusive_or_pattern";static char _tmp331[12]="and_pattern";
static char _tmp332[17]="equality_pattern";static char _tmp333[19]="relational_pattern";
static char _tmp334[14]="shift_pattern";static char _tmp335[17]="additive_pattern";
static char _tmp336[23]="multiplicative_pattern";static char _tmp337[13]="cast_pattern";
static char _tmp338[14]="unary_pattern";static char _tmp339[16]="postfix_pattern";
static char _tmp33A[16]="primary_pattern";static char _tmp33B[8]="pattern";static
char _tmp33C[19]="tuple_pattern_list";static char _tmp33D[14]="field_pattern";
static char _tmp33E[19]="field_pattern_list";static char _tmp33F[11]="expression";
static char _tmp340[22]="assignment_expression";static char _tmp341[20]="assignment_operator";
static char _tmp342[23]="conditional_expression";static char _tmp343[20]="constant_expression";
static char _tmp344[22]="logical_or_expression";static char _tmp345[23]="logical_and_expression";
static char _tmp346[24]="inclusive_or_expression";static char _tmp347[24]="exclusive_or_expression";
static char _tmp348[15]="and_expression";static char _tmp349[20]="equality_expression";
static char _tmp34A[22]="relational_expression";static char _tmp34B[17]="shift_expression";
static char _tmp34C[20]="additive_expression";static char _tmp34D[26]="multiplicative_expression";
static char _tmp34E[16]="cast_expression";static char _tmp34F[17]="unary_expression";
static char _tmp350[15]="unary_operator";static char _tmp351[19]="postfix_expression";
static char _tmp352[19]="primary_expression";static char _tmp353[25]="argument_expression_list";
static char _tmp354[26]="argument_expression_list0";static char _tmp355[9]="constant";
static char _tmp356[20]="qual_opt_identifier";static char _tmp357[17]="qual_opt_typedef";
static char _tmp358[18]="struct_union_name";static char _tmp359[11]="field_name";
static char _tmp35A[12]="right_angle";static struct _tagged_arr Cyc_yytname[263]={{
_tmp254,_tmp254,_tmp254 + 2},{_tmp255,_tmp255,_tmp255 + 6},{_tmp256,_tmp256,
_tmp256 + 12},{_tmp257,_tmp257,_tmp257 + 5},{_tmp258,_tmp258,_tmp258 + 9},{_tmp259,
_tmp259,_tmp259 + 7},{_tmp25A,_tmp25A,_tmp25A + 7},{_tmp25B,_tmp25B,_tmp25B + 8},{
_tmp25C,_tmp25C,_tmp25C + 5},{_tmp25D,_tmp25D,_tmp25D + 5},{_tmp25E,_tmp25E,
_tmp25E + 6},{_tmp25F,_tmp25F,_tmp25F + 4},{_tmp260,_tmp260,_tmp260 + 5},{_tmp261,
_tmp261,_tmp261 + 6},{_tmp262,_tmp262,_tmp262 + 7},{_tmp263,_tmp263,_tmp263 + 7},{
_tmp264,_tmp264,_tmp264 + 9},{_tmp265,_tmp265,_tmp265 + 6},{_tmp266,_tmp266,
_tmp266 + 9},{_tmp267,_tmp267,_tmp267 + 9},{_tmp268,_tmp268,_tmp268 + 7},{_tmp269,
_tmp269,_tmp269 + 6},{_tmp26A,_tmp26A,_tmp26A + 5},{_tmp26B,_tmp26B,_tmp26B + 8},{
_tmp26C,_tmp26C,_tmp26C + 7},{_tmp26D,_tmp26D,_tmp26D + 7},{_tmp26E,_tmp26E,
_tmp26E + 9},{_tmp26F,_tmp26F,_tmp26F + 3},{_tmp270,_tmp270,_tmp270 + 5},{_tmp271,
_tmp271,_tmp271 + 7},{_tmp272,_tmp272,_tmp272 + 6},{_tmp273,_tmp273,_tmp273 + 3},{
_tmp274,_tmp274,_tmp274 + 4},{_tmp275,_tmp275,_tmp275 + 5},{_tmp276,_tmp276,
_tmp276 + 9},{_tmp277,_tmp277,_tmp277 + 6},{_tmp278,_tmp278,_tmp278 + 7},{_tmp279,
_tmp279,_tmp279 + 5},{_tmp27A,_tmp27A,_tmp27A + 8},{_tmp27B,_tmp27B,_tmp27B + 4},{
_tmp27C,_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,
_tmp27E + 6},{_tmp27F,_tmp27F,_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 9},{_tmp281,
_tmp281,_tmp281 + 9},{_tmp282,_tmp282,_tmp282 + 6},{_tmp283,_tmp283,_tmp283 + 10},{
_tmp284,_tmp284,_tmp284 + 7},{_tmp285,_tmp285,_tmp285 + 8},{_tmp286,_tmp286,
_tmp286 + 9},{_tmp287,_tmp287,_tmp287 + 5},{_tmp288,_tmp288,_tmp288 + 8},{_tmp289,
_tmp289,_tmp289 + 4},{_tmp28A,_tmp28A,_tmp28A + 7},{_tmp28B,_tmp28B,_tmp28B + 7},{
_tmp28C,_tmp28C,_tmp28C + 8},{_tmp28D,_tmp28D,_tmp28D + 7},{_tmp28E,_tmp28E,
_tmp28E + 8},{_tmp28F,_tmp28F,_tmp28F + 9},{_tmp290,_tmp290,_tmp290 + 9},{_tmp291,
_tmp291,_tmp291 + 6},{_tmp292,_tmp292,_tmp292 + 7},{_tmp293,_tmp293,_tmp293 + 5},{
_tmp294,_tmp294,_tmp294 + 8},{_tmp295,_tmp295,_tmp295 + 13},{_tmp296,_tmp296,
_tmp296 + 4},{_tmp297,_tmp297,_tmp297 + 14},{_tmp298,_tmp298,_tmp298 + 12},{_tmp299,
_tmp299,_tmp299 + 7},{_tmp29A,_tmp29A,_tmp29A + 7},{_tmp29B,_tmp29B,_tmp29B + 7},{
_tmp29C,_tmp29C,_tmp29C + 8},{_tmp29D,_tmp29D,_tmp29D + 9},{_tmp29E,_tmp29E,
_tmp29E + 6},{_tmp29F,_tmp29F,_tmp29F + 6},{_tmp2A0,_tmp2A0,_tmp2A0 + 6},{_tmp2A1,
_tmp2A1,_tmp2A1 + 6},{_tmp2A2,_tmp2A2,_tmp2A2 + 7},{_tmp2A3,_tmp2A3,_tmp2A3 + 6},{
_tmp2A4,_tmp2A4,_tmp2A4 + 11},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{_tmp2A6,_tmp2A6,
_tmp2A6 + 11},{_tmp2A7,_tmp2A7,_tmp2A7 + 11},{_tmp2A8,_tmp2A8,_tmp2A8 + 11},{
_tmp2A9,_tmp2A9,_tmp2A9 + 12},{_tmp2AA,_tmp2AA,_tmp2AA + 13},{_tmp2AB,_tmp2AB,
_tmp2AB + 11},{_tmp2AC,_tmp2AC,_tmp2AC + 11},{_tmp2AD,_tmp2AD,_tmp2AD + 10},{
_tmp2AE,_tmp2AE,_tmp2AE + 9},{_tmp2AF,_tmp2AF,_tmp2AF + 11},{_tmp2B0,_tmp2B0,
_tmp2B0 + 12},{_tmp2B1,_tmp2B1,_tmp2B1 + 11},{_tmp2B2,_tmp2B2,_tmp2B2 + 17},{
_tmp2B3,_tmp2B3,_tmp2B3 + 7},{_tmp2B4,_tmp2B4,_tmp2B4 + 19},{_tmp2B5,_tmp2B5,
_tmp2B5 + 18},{_tmp2B6,_tmp2B6,_tmp2B6 + 9},{_tmp2B7,_tmp2B7,_tmp2B7 + 13},{_tmp2B8,
_tmp2B8,_tmp2B8 + 16},{_tmp2B9,_tmp2B9,_tmp2B9 + 18},{_tmp2BA,_tmp2BA,_tmp2BA + 13},{
_tmp2BB,_tmp2BB,_tmp2BB + 10},{_tmp2BC,_tmp2BC,_tmp2BC + 4},{_tmp2BD,_tmp2BD,
_tmp2BD + 4},{_tmp2BE,_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,
_tmp2C0,_tmp2C0 + 4},{_tmp2C1,_tmp2C1,_tmp2C1 + 4},{_tmp2C2,_tmp2C2,_tmp2C2 + 4},{
_tmp2C3,_tmp2C3,_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 4},{_tmp2C5,_tmp2C5,
_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 4},{_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,
_tmp2C8,_tmp2C8 + 4},{_tmp2C9,_tmp2C9,_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{
_tmp2CB,_tmp2CB,_tmp2CB + 4},{_tmp2CC,_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD,
_tmp2CD + 4},{_tmp2CE,_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF,_tmp2CF + 4},{_tmp2D0,
_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 4},{_tmp2D2,_tmp2D2,_tmp2D2 + 4},{
_tmp2D3,_tmp2D3,_tmp2D3 + 4},{_tmp2D4,_tmp2D4,_tmp2D4 + 4},{_tmp2D5,_tmp2D5,
_tmp2D5 + 4},{_tmp2D6,_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7,_tmp2D7 + 5},{_tmp2D8,
_tmp2D8,_tmp2D8 + 17},{_tmp2D9,_tmp2D9,_tmp2D9 + 21},{_tmp2DA,_tmp2DA,_tmp2DA + 20},{
_tmp2DB,_tmp2DB,_tmp2DB + 21},{_tmp2DC,_tmp2DC,_tmp2DC + 13},{_tmp2DD,_tmp2DD,
_tmp2DD + 15},{_tmp2DE,_tmp2DE,_tmp2DE + 17},{_tmp2DF,_tmp2DF,_tmp2DF + 19},{
_tmp2E0,_tmp2E0,_tmp2E0 + 12},{_tmp2E1,_tmp2E1,_tmp2E1 + 17},{_tmp2E2,_tmp2E2,
_tmp2E2 + 23},{_tmp2E3,_tmp2E3,_tmp2E3 + 24},{_tmp2E4,_tmp2E4,_tmp2E4 + 15},{
_tmp2E5,_tmp2E5,_tmp2E5 + 11},{_tmp2E6,_tmp2E6,_tmp2E6 + 15},{_tmp2E7,_tmp2E7,
_tmp2E7 + 10},{_tmp2E8,_tmp2E8,_tmp2E8 + 15},{_tmp2E9,_tmp2E9,_tmp2E9 + 5},{_tmp2EA,
_tmp2EA,_tmp2EA + 15},{_tmp2EB,_tmp2EB,_tmp2EB + 15},{_tmp2EC,_tmp2EC,_tmp2EC + 11},{
_tmp2ED,_tmp2ED,_tmp2ED + 22},{_tmp2EE,_tmp2EE,_tmp2EE + 26},{_tmp2EF,_tmp2EF,
_tmp2EF + 16},{_tmp2F0,_tmp2F0,_tmp2F0 + 16},{_tmp2F1,_tmp2F1,_tmp2F1 + 24},{
_tmp2F2,_tmp2F2,_tmp2F2 + 25},{_tmp2F3,_tmp2F3,_tmp2F3 + 21},{_tmp2F4,_tmp2F4,
_tmp2F4 + 22},{_tmp2F5,_tmp2F5,_tmp2F5 + 16},{_tmp2F6,_tmp2F6,_tmp2F6 + 19},{
_tmp2F7,_tmp2F7,_tmp2F7 + 25},{_tmp2F8,_tmp2F8,_tmp2F8 + 23},{_tmp2F9,_tmp2F9,
_tmp2F9 + 24},{_tmp2FA,_tmp2FA,_tmp2FA + 18},{_tmp2FB,_tmp2FB,_tmp2FB + 17},{
_tmp2FC,_tmp2FC,_tmp2FC + 18},{_tmp2FD,_tmp2FD,_tmp2FD + 17},{_tmp2FE,_tmp2FE,
_tmp2FE + 18},{_tmp2FF,_tmp2FF,_tmp2FF + 12},{_tmp300,_tmp300,_tmp300 + 11},{
_tmp301,_tmp301,_tmp301 + 18},{_tmp302,_tmp302,_tmp302 + 8},{_tmp303,_tmp303,
_tmp303 + 12},{_tmp304,_tmp304,_tmp304 + 23},{_tmp305,_tmp305,_tmp305 + 14},{
_tmp306,_tmp306,_tmp306 + 18},{_tmp307,_tmp307,_tmp307 + 8},{_tmp308,_tmp308,
_tmp308 + 11},{_tmp309,_tmp309,_tmp309 + 20},{_tmp30A,_tmp30A,_tmp30A + 9},{_tmp30B,
_tmp30B,_tmp30B + 16},{_tmp30C,_tmp30C,_tmp30C + 19},{_tmp30D,_tmp30D,_tmp30D + 10},{
_tmp30E,_tmp30E,_tmp30E + 16},{_tmp30F,_tmp30F,_tmp30F + 11},{_tmp310,_tmp310,
_tmp310 + 14},{_tmp311,_tmp311,_tmp311 + 11},{_tmp312,_tmp312,_tmp312 + 15},{
_tmp313,_tmp313,_tmp313 + 22},{_tmp314,_tmp314,_tmp314 + 16},{_tmp315,_tmp315,
_tmp315 + 17},{_tmp316,_tmp316,_tmp316 + 12},{_tmp317,_tmp317,_tmp317 + 18},{
_tmp318,_tmp318,_tmp318 + 17},{_tmp319,_tmp319,_tmp319 + 12},{_tmp31A,_tmp31A,
_tmp31A + 16},{_tmp31B,_tmp31B,_tmp31B + 11},{_tmp31C,_tmp31C,_tmp31C + 10},{
_tmp31D,_tmp31D,_tmp31D + 14},{_tmp31E,_tmp31E,_tmp31E + 15},{_tmp31F,_tmp31F,
_tmp31F + 20},{_tmp320,_tmp320,_tmp320 + 27},{_tmp321,_tmp321,_tmp321 + 10},{
_tmp322,_tmp322,_tmp322 + 18},{_tmp323,_tmp323,_tmp323 + 21},{_tmp324,_tmp324,
_tmp324 + 19},{_tmp325,_tmp325,_tmp325 + 16},{_tmp326,_tmp326,_tmp326 + 20},{
_tmp327,_tmp327,_tmp327 + 15},{_tmp328,_tmp328,_tmp328 + 16},{_tmp329,_tmp329,
_tmp329 + 20},{_tmp32A,_tmp32A,_tmp32A + 15},{_tmp32B,_tmp32B,_tmp32B + 12},{
_tmp32C,_tmp32C,_tmp32C + 20},{_tmp32D,_tmp32D,_tmp32D + 19},{_tmp32E,_tmp32E,
_tmp32E + 20},{_tmp32F,_tmp32F,_tmp32F + 21},{_tmp330,_tmp330,_tmp330 + 21},{
_tmp331,_tmp331,_tmp331 + 12},{_tmp332,_tmp332,_tmp332 + 17},{_tmp333,_tmp333,
_tmp333 + 19},{_tmp334,_tmp334,_tmp334 + 14},{_tmp335,_tmp335,_tmp335 + 17},{
_tmp336,_tmp336,_tmp336 + 23},{_tmp337,_tmp337,_tmp337 + 13},{_tmp338,_tmp338,
_tmp338 + 14},{_tmp339,_tmp339,_tmp339 + 16},{_tmp33A,_tmp33A,_tmp33A + 16},{
_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 19},{_tmp33D,_tmp33D,
_tmp33D + 14},{_tmp33E,_tmp33E,_tmp33E + 19},{_tmp33F,_tmp33F,_tmp33F + 11},{
_tmp340,_tmp340,_tmp340 + 22},{_tmp341,_tmp341,_tmp341 + 20},{_tmp342,_tmp342,
_tmp342 + 23},{_tmp343,_tmp343,_tmp343 + 20},{_tmp344,_tmp344,_tmp344 + 22},{
_tmp345,_tmp345,_tmp345 + 23},{_tmp346,_tmp346,_tmp346 + 24},{_tmp347,_tmp347,
_tmp347 + 24},{_tmp348,_tmp348,_tmp348 + 15},{_tmp349,_tmp349,_tmp349 + 20},{
_tmp34A,_tmp34A,_tmp34A + 22},{_tmp34B,_tmp34B,_tmp34B + 17},{_tmp34C,_tmp34C,
_tmp34C + 20},{_tmp34D,_tmp34D,_tmp34D + 26},{_tmp34E,_tmp34E,_tmp34E + 16},{
_tmp34F,_tmp34F,_tmp34F + 17},{_tmp350,_tmp350,_tmp350 + 15},{_tmp351,_tmp351,
_tmp351 + 19},{_tmp352,_tmp352,_tmp352 + 19},{_tmp353,_tmp353,_tmp353 + 25},{
_tmp354,_tmp354,_tmp354 + 26},{_tmp355,_tmp355,_tmp355 + 9},{_tmp356,_tmp356,
_tmp356 + 20},{_tmp357,_tmp357,_tmp357 + 17},{_tmp358,_tmp358,_tmp358 + 18},{
_tmp359,_tmp359,_tmp359 + 11},{_tmp35A,_tmp35A,_tmp35A + 12}};static short Cyc_yyr1[
474]={0,131,132,132,132,132,132,132,132,133,133,133,134,134,134,134,135,135,136,
137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,142,142,142,142,143,
143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,147,147,148,148,148,
148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,149,
150,150,150,151,151,151,152,152,153,153,153,154,154,154,155,155,156,156,157,157,
158,158,159,160,160,161,161,162,163,163,163,163,163,163,164,165,165,166,166,166,
167,167,167,168,168,169,169,169,169,170,170,170,171,171,172,172,173,173,173,173,
173,173,173,173,173,173,173,174,174,175,176,176,176,177,177,177,178,178,178,179,
179,179,180,180,181,181,181,181,182,182,183,183,184,184,185,185,186,186,187,187,
188,188,188,188,189,189,190,190,191,191,191,192,193,193,194,194,195,195,195,195,
196,196,196,196,197,198,198,199,199,200,200,201,201,201,201,201,202,202,203,203,
203,204,204,204,204,204,204,204,204,204,204,204,205,205,205,205,205,205,205,205,
205,205,205,205,205,206,207,207,207,208,208,208,209,209,209,209,209,209,210,210,
210,210,210,210,210,210,211,211,211,211,211,211,211,212,212,212,212,212,213,213,
213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,214,214,214,214,
214,214,214,214,215,216,216,217,217,218,218,219,219,220,220,221,221,222,222,222,
223,223,223,223,223,224,224,224,225,225,225,226,226,226,226,227,227,228,228,228,
228,228,228,228,229,230,231,231,231,231,231,231,231,231,231,231,231,231,232,232,
233,233,234,234,235,235,236,236,237,237,237,237,237,237,237,237,237,237,237,238,
238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,245,
246,246,246,246,246,247,247,247,248,248,248,249,249,249,249,250,250,251,251,251,
251,251,251,251,251,251,251,251,251,251,251,251,251,252,252,252,253,253,253,253,
253,253,253,253,253,253,253,253,254,254,254,254,254,254,254,254,254,254,255,256,
256,257,257,257,257,258,258,259,259,260,260,261,261,262,262};static short Cyc_yyr2[
474]={0,1,2,3,5,3,5,6,0,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,
2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,4,1,
1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,1,3,1,
2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,4,5,4,5,4,4,2,2,1,2,5,2,2,1,0,3,3,0,1,
1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,
4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,
6,3,9,6,5,2,2,3,1,2,2,2,3,3,1,2,1,2,1,2,5,7,2,7,2,8,6,2,0,3,4,5,6,7,4,0,3,4,5,4,
5,2,7,2,6,7,7,8,7,8,8,9,6,7,7,8,2,9,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,
3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,4,4,5,2,2,2,4,4,
1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,
3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,
3,4,3,3,2,2,6,7,4,4,1,1,1,3,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static short Cyc_yydefact[957]={0,11,38,39,40,41,43,56,59,60,61,62,63,64,65,66,79,
80,81,95,96,34,0,0,44,0,0,122,123,0,0,76,464,167,466,465,467,77,0,0,57,0,152,152,
151,1,0,9,0,0,10,0,28,36,30,32,67,68,0,69,158,0,133,0,146,155,70,135,93,42,41,35,
0,83,463,0,464,460,461,462,0,345,0,0,0,0,190,0,347,348,18,20,0,0,0,0,0,0,0,0,0,0,
149,150,2,0,0,0,0,22,0,101,102,104,29,37,31,33,97,468,469,93,160,0,159,0,26,0,0,
12,169,0,0,144,134,147,157,156,158,145,0,71,0,42,87,0,85,0,464,353,0,0,0,0,0,0,0,
0,0,0,0,0,93,0,0,449,0,0,0,0,0,434,432,433,0,363,365,378,386,388,390,392,394,396,
399,404,407,410,414,0,416,435,448,447,0,354,352,25,0,0,93,0,0,0,111,107,109,208,
210,0,0,0,470,471,168,78,0,0,136,58,188,0,185,0,0,0,3,0,5,0,23,0,0,0,13,0,98,99,
0,92,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,464,245,248,0,0,0,0,231,232,233,0,234,
235,236,0,27,14,104,175,191,0,0,171,169,0,215,0,155,0,45,0,0,88,84,0,0,0,0,0,424,
0,380,414,0,381,382,0,0,0,0,0,0,0,0,0,417,418,0,0,0,0,0,420,421,419,346,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,368,369,370,371,372,373,374,375,376,377,367,0,
422,0,441,442,0,0,0,452,0,93,357,0,192,0,0,0,0,211,183,0,112,108,110,169,0,217,
209,218,473,472,0,73,74,75,51,50,0,48,137,169,187,217,189,72,0,0,154,153,19,0,21,
0,103,105,194,193,15,90,100,0,0,113,114,116,93,0,120,247,250,259,0,261,0,0,278,0,
280,0,293,0,0,296,297,298,0,264,0,300,0,0,0,242,243,0,0,0,0,0,256,252,104,254,
249,246,176,0,0,0,182,170,177,140,0,0,0,171,142,0,143,138,155,161,46,94,0,89,385,
86,82,355,356,0,0,0,195,0,0,199,0,0,0,204,0,0,0,0,0,0,0,0,0,0,451,0,0,450,458,0,
457,364,387,0,389,391,393,395,397,398,402,403,400,401,405,406,408,409,411,412,
413,366,440,437,0,439,0,0,0,349,0,24,0,359,361,0,350,0,0,212,0,171,0,155,0,219,
169,0,0,230,214,0,0,0,111,186,4,6,117,106,0,0,171,93,128,0,0,0,0,0,0,0,0,295,299,
0,0,0,0,238,0,0,0,244,0,16,169,0,179,0,0,172,0,141,175,163,216,139,161,148,7,423,
0,0,207,0,196,0,200,203,205,445,446,0,428,0,0,0,0,0,0,0,0,415,454,0,0,438,436,0,
0,358,360,351,0,213,184,226,0,220,221,155,0,171,0,155,0,0,0,0,47,49,115,118,97,
121,130,129,0,0,124,0,0,0,0,0,0,0,0,0,0,265,301,0,0,0,0,0,17,171,0,180,178,0,171,
0,162,0,0,0,206,197,201,0,0,0,0,383,384,427,456,0,459,379,453,455,0,362,225,223,
228,0,229,222,155,54,0,52,53,0,119,131,128,128,257,0,265,277,0,0,0,0,0,0,0,0,0,0,
0,0,302,0,0,0,0,241,165,181,173,164,169,426,425,0,202,429,0,0,443,0,227,224,0,91,
0,127,126,0,272,0,0,281,0,0,0,289,0,0,0,0,0,0,0,0,0,0,0,0,303,304,306,308,310,
312,314,316,319,324,327,330,334,336,343,344,0,0,263,0,237,240,0,0,171,0,0,0,444,
0,0,258,0,0,0,260,279,282,283,0,291,290,0,285,0,0,0,0,0,340,0,0,337,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,338,266,0,0,174,166,0,0,0,0,132,0,0,0,262,284,292,
286,287,0,271,0,0,0,265,267,307,0,309,311,313,315,317,318,322,323,320,321,325,
326,328,329,331,332,333,0,0,0,198,430,0,55,0,0,273,288,339,0,335,268,0,0,294,239,
0,276,272,274,0,0,305,265,269,431,275,342,341,270,0,0,0};static short Cyc_yydefgoto[
132]={954,45,46,47,261,48,405,49,407,262,233,263,52,477,204,392,393,205,214,206,
56,144,145,57,141,58,235,236,110,111,112,237,207,416,417,418,59,60,672,673,674,
61,62,63,64,65,102,138,123,613,554,66,555,469,605,461,465,466,375,281,222,85,86,
493,410,494,495,496,497,208,283,284,556,383,264,265,266,267,268,269,753,836,270,
271,801,802,803,804,805,806,807,808,809,810,811,812,813,814,815,816,366,367,548,
549,272,175,355,176,483,177,178,179,180,181,182,183,184,185,186,187,188,189,190,
191,513,514,192,193,68,121,215,387};static short Cyc_yypact[957]={3321,- -32768,-
-32768,- -32768,- -32768,- 35,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4168,256,
575,- -32768,215,70,- -32768,- -32768,- 11,55,66,- -32768,143,- -32768,- -32768,- -32768,
- -32768,161,350,194,168,193,193,- -32768,- -32768,3200,- -32768,115,403,- -32768,224,
4168,4168,4168,4168,- -32768,- -32768,341,- -32768,5,4039,99,206,134,131,- -32768,
215,208,214,264,- -32768,215,268,- -32768,877,241,- -32768,- -32768,- -32768,5828,-
-32768,282,317,877,348,302,357,- -32768,40,- -32768,- -32768,6407,6407,6407,156,349,
379,366,156,6585,2946,- -32768,- -32768,- -32768,3200,3441,3200,3441,- -32768,374,
395,- -32768,3970,- -32768,- -32768,- -32768,- -32768,6585,- -32768,- -32768,208,-
-32768,215,- -32768,2247,- -32768,4039,224,- -32768,4224,6407,4612,- -32768,99,-
-32768,- -32768,- -32768,5,- -32768,6407,- -32768,3441,- -32768,411,417,441,215,449,-
-32768,74,6056,461,5828,5904,479,484,495,507,509,515,520,208,6132,6132,- -32768,
1447,522,6208,6208,6208,- -32768,- -32768,- -32768,345,- -32768,- -32768,8,572,529,
521,536,502,126,512,277,- 47,- -32768,1008,6208,135,50,- -32768,549,877,- -32768,-
-32768,- -32768,570,5828,208,877,565,- 2,6585,6585,6585,343,- -32768,- 3,- 3,- 3,-
-32768,- -32768,- -32768,- -32768,51,569,- -32768,- -32768,393,491,- -32768,573,- 27,
593,- -32768,597,- -32768,599,- -32768,379,4688,4039,- -32768,611,6585,- -32768,308,
614,197,234,47,62,63,2483,65,630,637,641,4764,2589,233,651,2695,2695,- 16,653,665,
- -32768,- -32768,1597,1727,224,1857,- -32768,- -32768,- -32768,661,- -32768,- -32768,-
-32768,170,- -32768,- -32768,674,690,- -32768,56,684,681,260,691,682,87,131,686,702,
87,700,215,- -32768,5828,703,687,689,1447,- -32768,6585,- -32768,- -32768,4249,-
-32768,733,5828,3559,5828,5828,5828,5828,5828,705,1447,- -32768,- -32768,706,2377,
707,499,5828,- -32768,- -32768,- -32768,- -32768,5828,6208,5828,6208,6208,6208,6208,
6208,6208,6208,6208,6208,6208,6208,6208,6208,6208,6208,6208,6208,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5828,-
-32768,156,- -32768,- -32768,4840,156,5828,- -32768,701,208,- -32768,516,- -32768,267,
754,526,6407,- -32768,708,714,- -32768,- -32768,- -32768,3739,4916,110,- -32768,128,-
-32768,- -32768,56,- -32768,- -32768,- -32768,- -32768,717,713,718,- -32768,3626,-
-32768,132,- -32768,- -32768,6585,3069,- -32768,- -32768,- -32768,3200,- -32768,3200,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5828,723,719,- -32768,720,
208,215,725,- -32768,- -32768,- -32768,5828,- -32768,727,5828,- -32768,5828,729,806,-
-32768,2823,734,- -32768,- -32768,- -32768,276,729,795,- -32768,4992,4108,729,-
-32768,- -32768,2695,742,748,5828,2695,- -32768,- -32768,3970,- -32768,- -32768,-
-32768,- -32768,6585,735,259,- -32768,- -32768,722,- -32768,56,736,6480,681,- -32768,
6407,- -32768,- -32768,131,603,- -32768,- -32768,3200,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,737,746,759,- -32768,156,5828,- -32768,272,4688,49,- -32768,
528,749,379,533,747,750,751,550,6585,762,- -32768,769,5980,- -32768,- -32768,767,
758,- -32768,572,382,529,521,536,502,126,126,512,512,512,512,277,277,- 47,- 47,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,773,- -32768,104,6407,4531,- -32768,
877,- -32768,877,- -32768,- -32768,400,- -32768,150,785,- -32768,775,681,776,131,770,
128,6465,6407,5068,- -32768,- -32768,539,777,51,3852,- -32768,- -32768,- -32768,-
-32768,- -32768,308,5828,681,208,606,567,5828,579,598,782,5144,5220,296,- -32768,-
-32768,786,788,787,790,- -32768,774,780,601,- -32768,4039,- -32768,794,6407,- -32768,
798,56,- -32768,779,- -32768,120,- -32768,682,- -32768,603,- -32768,- -32768,796,298,
792,- -32768,799,- -32768,4343,- -32768,- -32768,- -32768,- -32768,- -32768,4039,-
-32768,5828,884,5828,5904,804,796,810,4531,- -32768,- -32768,5828,5828,- -32768,-
-32768,87,409,- -32768,- -32768,- -32768,754,- -32768,- -32768,- -32768,811,- -32768,-
-32768,131,812,681,87,131,805,604,816,817,- -32768,- -32768,- -32768,- -32768,6585,-
-32768,- -32768,- -32768,821,215,310,2695,617,825,2695,5828,5296,319,5372,328,5448,
662,- -32768,827,5828,842,1,832,- -32768,681,179,- -32768,- -32768,840,681,6585,-
-32768,831,834,5828,- -32768,- -32768,- -32768,4688,619,838,837,- -32768,733,- -32768,
- -32768,410,- -32768,- -32768,- -32768,- -32768,4531,- -32768,- -32768,- -32768,- -32768,
839,- -32768,- -32768,131,- -32768,855,- -32768,- -32768,844,- -32768,843,77,606,924,
848,662,- -32768,622,2695,624,5524,2695,626,5600,5676,361,1207,845,850,- -32768,
363,2695,2695,856,- -32768,- -32768,- -32768,847,- -32768,794,- -32768,- -32768,408,-
-32768,- -32768,6585,935,- -32768,4437,- -32768,- -32768,851,- -32768,6585,- -32768,-
-32768,2695,716,857,860,- -32768,2695,2695,634,- -32768,2695,2695,646,2695,648,
5752,852,6284,867,3069,6208,862,- -32768,20,- -32768,900,858,628,688,183,697,442,
207,- -32768,- -32768,- -32768,- -32768,901,6208,2377,- -32768,5828,- -32768,- -32768,
859,56,681,5828,873,875,- -32768,892,663,- -32768,1164,876,881,- -32768,- -32768,-
-32768,- -32768,2695,- -32768,- -32768,2695,- -32768,2695,2695,668,2377,1447,- -32768,
6585,882,- -32768,1987,6208,5828,6208,6208,6208,6208,6208,6208,6208,6208,6208,
6208,6208,6208,6208,6208,6208,6208,6208,5828,- -32768,- -32768,680,899,- -32768,-
-32768,465,885,6585,891,- -32768,887,466,2377,- -32768,- -32768,- -32768,- -32768,-
-32768,2695,- -32768,894,897,6208,662,- -32768,572,480,529,521,521,502,126,126,512,
512,512,512,277,277,- 47,- 47,- -32768,- -32768,- -32768,485,2695,2695,- -32768,-
-32768,895,- -32768,2377,2117,- -32768,- -32768,796,389,- -32768,- -32768,5828,1987,-
-32768,- -32768,902,- -32768,716,- -32768,903,904,- -32768,662,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,1009,1010,- -32768};static short Cyc_yypgoto[132]={-
-32768,176,- -32768,710,- -32768,- -32768,- -32768,- -32768,- -32768,184,- 25,972,-
-32768,- -32768,0,452,- -32768,69,909,4,- -32768,- -32768,- 109,- -32768,16,- -32768,
352,- -32768,- -32768,- -32768,791,793,544,- -32768,- -32768,447,- -32768,- -32768,61,-
-32768,- -32768,643,- 37,- 40,- -32768,- -32768,980,- 234,890,418,- 127,- 50,- 123,- 309,
210,423,428,- 377,- 420,- 98,- 372,906,- -32768,- 217,- 143,- 497,- 326,- -32768,541,- 160,
94,- 117,93,- 238,121,- -32768,- -32768,- 41,- 231,- -32768,- 662,- 635,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,17,841,391,- -32768,- 33,802,-
-32768,- 145,- 366,- 132,- 306,- 300,- 314,711,- 301,- 319,- 260,- 303,- 298,- 152,199,290,-
-32768,- -32768,- 293,- -32768,9,679,987,- -32768,- 356,- 61};static short Cyc_yytable[
6698]={53,536,221,279,55,539,317,280,299,564,124,302,522,523,519,409,320,321,322,
516,129,53,303,288,135,55,134,518,569,521,454,455,88,457,528,529,127,356,293,97,
87,530,531,603,546,644,53,174,425,572,55,475,53,53,53,53,55,55,55,55,69,53,133,
427,430,55,434,538,390,54,384,341,234,524,525,526,527,449,783,402,342,343,670,
671,88,509,274,325,124,600,54,606,149,88,757,386,33,450,569,856,451,196,92,33,
373,53,53,53,53,55,55,55,55,53,758,54,122,55,385,386,462,54,54,54,54,53,619,53,
326,55,54,55,650,318,133,618,487,239,488,715,857,363,53,559,391,200,55,482,201,
388,389,591,507,374,33,426,623,428,471,559,384,463,609,91,491,492,369,381,93,364,
429,431,294,435,54,54,54,54,311,94,397,481,54,- 125,50,295,209,210,211,532,533,
534,412,901,54,224,54,473,136,137,333,334,38,88,357,358,359,130,385,667,88,54,
131,460,324,132,370,440,379,105,106,642,104,474,278,32,380,479,464,- 169,50,38,35,
53,- 169,95,560,55,935,335,395,561,611,360,562,126,652,336,380,212,361,362,42,43,
44,213,422,864,865,649,702,53,53,318,53,55,55,668,55,96,482,498,386,501,459,948,
100,505,622,318,324,226,227,228,229,953,99,478,761,50,50,50,50,517,943,707,866,
126,101,32,382,386,54,221,867,53,35,950,32,55,140,273,421,398,39,35,53,32,289,
142,55,140,546,654,35,872,50,764,109,540,54,54,39,54,873,874,464,443,423,381,424,
444,42,43,44,- 191,633,482,725,32,297,- 191,300,300,150,397,35,33,637,143,134,72,
313,314,278,602,433,300,300,300,470,545,442,147,54,447,448,324,620,88,588,542,
621,563,760,54,324,547,300,763,194,212,701,579,707,568,581,213,582,339,684,32,
586,340,706,53,324,53,35,55,195,55,198,374,736,599,39,273,464,596,737,723,415,
745,643,727,42,43,44,324,482,598,748,656,32,53,576,657,324,55,34,35,36,32,658,53,
118,707,606,55,35,379,197,38,323,324,53,216,39,380,55,42,43,44,199,795,551,821,
42,43,44,324,32,324,54,218,54,693,230,35,53,612,212,944,55,698,32,39,213,711,768,
300,324,35,775,717,640,42,43,44,712,395,374,706,54,231,647,107,108,380,648,42,43,
44,54,719,772,881,324,720,773,290,827,291,300,54,300,300,300,300,300,300,300,300,
300,300,300,300,300,300,300,300,300,908,909,905,906,676,292,54,902,681,683,88,
464,88,706,692,904,563,907,645,150,646,870,914,915,610,871,298,593,924,916,917,
597,324,324,945,331,332,929,570,718,571,337,338,234,304,877,50,324,50,305,669,
936,324,708,726,53,937,399,400,55,306,910,911,912,913,511,324,828,670,671,74,300,
307,612,308,896,585,16,17,18,309,900,543,544,53,310,592,319,55,661,662,663,550,
544,625,324,853,126,826,628,324,220,329,742,744,854,747,327,750,328,75,365,755,
614,88,930,632,324,330,238,368,50,547,876,54,76,77,767,78,79,372,220,35,675,324,
394,67,832,401,98,80,751,752,81,82,677,324,897,300,898,83,113,694,54,941,942,403,
84,73,89,404,90,406,947,678,324,300,691,324,788,729,730,792,794,413,67,420,918,
919,920,436,926,67,739,324,769,324,67,784,324,786,324,790,324,119,834,835,217,
437,67,841,324,438,139,934,376,377,378,146,860,861,89,844,324,846,324,445,88,452,
848,89,862,863,174,458,817,868,869,275,886,400,300,464,67,895,324,453,238,232,
273,460,67,67,67,67,878,922,324,946,74,467,882,468,738,779,780,741,472,888,240,
476,386,38,480,67,485,484,486,126,325,506,541,508,510,318,552,53,553,67,566,55,
903,565,146,573,567,574,578,300,75,423,575,580,583,589,587,300,594,595,921,601,
604,607,615,148,77,53,78,79,617,55,35,53,616,629,626,55,630,631,80,396,785,81,82,
789,639,491,492,634,83,89,275,635,638,822,823,84,89,419,641,33,651,653,664,655,
54,53,679,685,686,55,688,689,687,690,278,67,697,636,833,225,695,703,456,839,840,
709,67,842,843,713,845,74,704,67,54,714,722,724,728,220,54,731,732,734,53,53,740,
754,55,55,286,756,759,53,762,220,765,55,67,766,220,220,770,771,774,776,777,778,
781,782,824,75,820,825,54,819,829,831,891,837,838,892,849,893,894,146,148,77,51,
78,79,852,855,35,858,875,879,859,883,884,67,80,885,890,81,82,889,899,923,71,925,
83,851,54,54,300,927,928,84,932,940,220,54,933,219,955,956,949,951,952,220,499,
931,300,51,665,733,666,408,103,114,115,116,117,287,414,700,699,696,128,411,880,
282,624,98,721,520,818,371,938,939,120,0,0,0,89,0,0,0,0,0,300,0,300,300,300,300,
300,300,300,300,300,300,300,300,300,300,300,300,300,67,0,67,51,51,51,51,0,0,0,67,
128,67,0,344,345,346,347,348,349,350,351,352,353,300,128,577,0,0,411,220,0,0,0,0,
502,503,504,376,0,51,354,0,0,0,0,0,512,0,0,0,0,515,0,0,0,0,0,0,0,0,300,0,0,0,0,0,
0,0,627,0,0,0,0,0,0,0,0,0,0,0,0,0,535,0,67,0,0,512,0,0,887,0,0,0,0,0,0,0,0,0,0,0,
0,0,67,0,0,558,0,0,0,0,0,0,151,152,0,0,0,0,0,0,0,0,0,0,0,74,512,153,128,0,154,
796,0,0,217,238,0,0,155,156,419,0,157,158,159,160,89,0,89,0,161,0,0,162,0,797,
798,163,164,0,0,0,0,0,0,0,220,0,74,512,67,0,0,0,0,0,67,0,0,0,32,77,165,78,79,0,0,
35,0,0,0,0,0,0,0,166,0,0,0,167,500,0,0,0,0,168,0,0,75,169,0,0,170,171,0,0,172,
173,0,0,411,0,0,148,77,0,78,79,0,0,35,0,0,0,0,0,0,0,799,0,0,81,82,0,0,220,0,0,83,
0,89,0,800,0,0,84,171,0,0,172,173,0,0,0,0,0,0,411,0,0,0,0,0,0,0,735,0,0,0,0,0,0,
0,0,0,0,0,660,0,0,0,0,0,0,0,0,0,0,0,0,51,0,51,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,128,0,0,0,0,0,0,0,0,0,128,0,0,0,0,0,411,0,0,0,0,128,0,89,0,0,
710,0,0,0,0,411,0,0,716,0,0,0,0,0,0,315,0,0,0,51,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,151,152,0,0,0,0,0,0,0,0,0,0,22,74,0,153,0,0,154,0,0,0,0,27,28,
0,155,156,0,0,157,158,159,160,29,30,31,411,161,0,0,162,0,0,0,163,164,0,0,0,411,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,33,34,35,36,37,38,0,316,0,0,166,
0,0,40,223,0,0,0,0,0,168,0,0,0,169,128,0,170,171,0,411,172,173,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,446,128,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
- 255,- 255,21,151,152,242,0,243,244,245,246,247,248,249,250,22,74,23,153,251,0,
154,24,252,0,0,27,28,253,155,156,254,255,157,158,159,160,29,30,31,256,161,0,257,
162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,
34,35,36,37,38,259,125,- 255,0,166,0,0,40,223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,
0,0,172,173,446,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 251,- 251,21,
151,152,242,0,243,244,245,246,247,248,249,250,22,74,23,153,251,0,154,24,252,0,0,
27,28,253,155,156,254,255,157,158,159,160,29,30,31,256,161,0,257,162,0,0,0,163,
164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,34,35,36,37,38,
259,125,- 251,0,166,0,0,40,223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,
446,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 253,- 253,21,151,152,242,
0,243,244,245,246,247,248,249,250,22,74,23,153,251,0,154,24,252,0,0,27,28,253,
155,156,254,255,157,158,159,160,29,30,31,256,161,0,257,162,0,0,0,163,164,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,34,35,36,37,38,259,125,-
253,0,166,0,0,40,223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,446,0,2,3,4,
70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,751,752,21,151,152,242,0,243,244,245,
246,247,248,249,250,22,74,23,153,251,0,154,24,252,0,0,27,28,253,155,156,254,255,
157,158,159,160,29,30,31,256,161,0,257,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,34,35,36,37,38,259,125,- 265,0,166,0,0,40,
223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,446,0,2,3,4,70,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,834,835,21,151,152,242,0,243,244,245,246,247,248,249,
250,22,74,23,153,251,0,154,24,252,0,0,27,28,253,155,156,254,255,157,158,159,160,
29,30,31,256,161,0,257,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,258,77,165,78,79,33,34,35,36,37,38,259,125,- 272,0,166,0,0,40,223,0,0,0,0,0,168,
0,0,0,169,0,0,170,171,0,0,172,173,241,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,151,152,242,0,243,244,245,246,247,248,249,250,22,74,23,153,251,0,
154,24,252,0,0,27,28,253,155,156,254,255,157,158,159,160,29,30,31,256,161,0,257,
162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,
34,35,36,37,38,259,125,260,0,166,0,0,40,223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,
0,0,172,173,446,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,151,
152,242,0,243,244,245,246,247,248,249,250,22,74,23,153,251,0,154,24,252,0,0,27,
28,253,155,156,254,255,157,158,159,160,29,30,31,256,161,0,257,162,0,0,0,163,164,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,33,34,35,36,37,38,259,
125,0,432,166,0,0,40,223,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,151,152,
242,0,243,244,245,246,247,248,249,250,0,74,0,153,251,0,154,0,252,0,0,0,0,253,155,
156,254,255,157,158,159,160,0,0,0,256,161,0,257,162,0,0,0,163,164,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,165,78,79,0,0,35,0,0,0,259,125,0,441,166,0,0,
0,167,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,151,152,242,0,243,244,245,
246,247,248,249,250,0,74,0,153,251,0,154,0,252,0,0,0,0,253,155,156,254,255,157,
158,159,160,0,0,0,256,161,0,257,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,258,77,165,78,79,0,0,35,0,0,0,259,125,0,446,166,0,0,0,167,0,0,0,0,0,
168,0,0,0,169,0,0,170,171,0,0,172,173,151,152,242,0,243,244,245,246,247,248,249,
250,0,74,0,153,251,0,154,0,252,0,0,0,0,253,155,156,254,255,157,158,159,160,0,0,0,
256,161,0,257,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,77,
165,78,79,0,0,35,0,0,0,259,125,0,0,166,0,0,0,167,0,0,0,0,0,168,0,0,0,169,0,0,170,
171,0,0,172,173,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,151,152,
0,0,0,0,0,0,0,0,0,0,22,74,23,153,0,0,154,24,0,0,0,27,28,0,155,156,0,0,157,158,
159,160,29,30,31,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,77,165,78,79,33,34,35,36,37,38,584,0,0,0,166,0,0,40,223,0,0,0,0,0,168,0,
0,0,169,0,0,170,171,0,0,172,173,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,151,
152,0,0,0,0,0,0,0,0,0,0,22,74,0,153,0,0,154,0,0,0,0,27,28,0,155,156,0,0,157,158,
159,160,29,30,31,0,161,202,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,32,77,165,78,79,33,34,35,36,37,38,0,203,0,0,166,0,0,40,223,0,0,0,0,0,168,
0,0,0,169,0,0,170,171,0,0,172,173,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
151,152,0,0,0,0,0,0,0,0,0,0,22,74,0,153,0,0,154,0,0,0,0,27,28,0,155,156,0,0,157,
158,159,160,29,30,31,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,32,77,165,78,79,33,34,35,36,37,38,0,0,0,0,166,0,0,40,223,0,0,0,0,0,168,
0,0,0,169,0,0,170,171,0,0,172,173,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,
0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,0,0,0,0,42,43,44,- 8,1,0,2,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,0,0,39,0,0,
40,41,0,0,0,0,0,42,43,44,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,
29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,
0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,0,0,0,0,42,43,44,2,3,4,70,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,24,0,
0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,22,0,0,0,39,0,0,40,41,
0,0,27,28,0,42,43,44,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,276,0,0,32,0,0,0,0,33,34,35,36,37,38,278,0,0,0,395,0,0,40,41,0,0,
0,380,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,0,0,33,34,0,36,37,38,278,0,0,
0,379,0,0,40,41,0,0,0,380,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,
34,35,36,37,38,0,0,0,0,39,0,0,40,41,0,0,0,0,0,42,43,44,2,3,4,70,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,0,0,
27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,125,22,232,23,0,0,40,
41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,125,22,0,23,
0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,4,70,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,33,34,0,36,37,38,24,0,
0,0,27,28,0,40,41,0,0,0,0,0,0,29,30,31,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,33,34,0,36,37,38,27,28,151,152,0,0,0,
40,41,489,0,29,30,31,0,74,0,153,0,0,154,0,0,0,0,0,0,0,155,156,0,0,157,158,159,
160,0,0,0,0,161,0,276,162,0,277,0,163,164,0,33,34,0,36,37,38,278,0,0,0,0,0,0,40,
41,0,0,0,0,0,32,77,165,78,79,0,0,35,0,0,0,0,301,490,0,166,0,0,0,167,0,0,491,492,
0,168,151,152,0,169,0,0,170,171,0,0,172,173,0,74,0,153,0,0,154,0,0,0,0,0,0,0,155,
156,0,0,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,0,0,0,0,301,705,0,166,0,0,0,167,0,0,
491,492,0,168,151,152,0,169,0,0,170,171,0,0,172,173,0,74,0,153,0,0,154,0,0,0,0,0,
0,0,155,156,0,0,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,0,0,0,0,301,830,0,166,0,0,0,
167,0,0,491,492,0,168,151,152,0,169,0,0,170,171,0,0,172,173,0,74,0,153,0,0,154,0,
0,0,0,0,0,0,155,156,0,0,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,0,0,0,0,301,151,152,
166,0,0,0,167,0,0,491,492,0,168,74,0,153,169,0,154,170,171,0,0,172,173,0,155,156,
0,0,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,0,0,0,167,0,74,0,
153,285,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,
0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,
0,0,35,151,152,0,0,301,0,0,166,0,0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,
171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,439,0,0,0,166,0,
0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,
0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,537,0,0,167,0,74,0,153,0,168,154,0,0,
169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,
164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,
0,0,0,166,0,0,0,167,0,74,0,153,557,168,154,0,0,169,0,0,170,171,155,156,172,173,
157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,590,0,0,167,0,74,0,153,
0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,
162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,
35,151,152,0,0,0,0,0,166,0,0,0,167,0,74,0,153,659,168,154,0,0,169,0,0,170,171,
155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,680,0,0,0,166,0,0,0,
167,0,74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,
0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,
165,78,79,0,0,35,151,152,0,682,0,0,0,166,0,0,0,167,0,74,0,153,0,168,154,0,0,169,
0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,
166,743,0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,
159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,746,0,0,167,0,74,0,153,0,168,
154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,
0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,
152,0,749,0,0,0,166,0,0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,
172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,787,0,0,167,0,
74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,
161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,
79,0,0,35,151,152,0,0,0,0,0,166,791,0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,
171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,793,
0,0,167,0,74,0,153,0,168,154,0,0,169,0,0,170,171,155,156,172,173,157,158,159,160,
0,0,0,0,161,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,847,0,0,167,0,74,0,153,0,168,154,0,0,
169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,161,0,0,162,0,0,0,163,
164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,
0,0,0,166,0,0,0,167,0,74,0,0,0,168,0,0,0,169,0,0,170,171,155,156,172,173,157,158,
159,160,0,0,0,0,0,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,32,77,165,78,79,0,0,35,151,152,0,0,301,0,0,166,0,0,0,167,0,74,0,0,0,168,0,0,0,
169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,0,0,0,162,0,0,0,163,164,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,636,
0,0,166,0,0,0,167,0,74,0,0,0,168,0,0,0,169,0,0,170,171,155,156,172,173,157,158,
159,160,0,0,0,0,0,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,0,296,0,0,0,167,0,74,0,0,0,168,0,0,0,
169,0,0,170,171,155,156,172,173,157,158,159,160,0,0,0,0,0,0,0,162,0,0,0,163,164,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,151,152,0,0,0,0,
0,312,0,0,0,167,0,74,0,0,0,168,0,0,0,169,0,0,170,171,155,156,172,173,157,158,159,
160,0,0,0,0,0,0,0,162,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
77,165,78,79,0,0,35,151,152,0,0,0,0,0,166,0,0,0,167,0,74,0,0,0,168,0,0,0,169,0,0,
170,171,155,156,172,173,157,158,159,160,0,0,0,0,0,0,0,162,0,0,0,163,164,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,77,165,78,79,0,0,35,0,0,0,0,0,0,0,850,0,0,0,
167,0,0,0,0,0,168,0,0,0,169,0,0,170,171,0,0,172,173,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,
0,0,0,0,29,30,31,0,0,202,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,22,0,0,33,34,0,36,37,38,0,203,27,28,0,0,22,40,41,0,0,
0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,
0,0,0,0,0,0,33,34,0,36,37,38,278,608,0,0,0,0,0,40,41,33,34,0,36,37,38,0,0,0,0,0,
0,0,40,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,38,0,0,0,0,0,0,0,
40,41};static short Cyc_yycheck[6698]={0,357,100,130,0,361,166,130,153,386,60,154,
331,332,328,232,168,169,170,325,61,21,154,140,64,21,63,327,400,330,261,262,23,
264,337,338,61,189,147,39,23,339,340,463,370,542,46,80,1,415,46,285,52,53,54,55,
52,53,54,55,95,61,62,1,1,61,1,360,17,0,73,118,113,333,334,335,336,93,740,106,127,
128,5,6,75,316,127,79,138,461,21,468,75,84,93,122,98,113,470,79,116,84,113,98,
106,105,106,107,108,105,106,107,108,113,113,46,111,113,121,122,64,52,53,54,55,
125,492,127,120,125,61,127,552,166,134,491,296,121,298,636,120,91,142,381,93,105,
142,292,108,210,211,444,312,203,98,108,107,95,281,397,73,105,471,93,115,116,199,
207,113,119,108,108,98,108,105,106,107,108,162,113,220,290,113,106,0,111,92,93,
94,341,342,343,233,855,125,101,127,110,67,68,74,75,103,194,69,70,71,108,121,575,
201,142,113,93,110,116,200,250,108,104,105,117,46,284,104,93,116,288,278,109,46,
103,100,233,114,92,108,233,900,113,108,113,476,108,116,61,555,121,116,93,115,116,
118,119,120,99,240,74,75,109,616,261,262,296,264,261,262,576,264,108,415,304,122,
306,104,937,108,310,495,312,110,105,106,107,108,947,92,287,109,105,106,107,108,
326,929,621,113,113,105,93,207,122,233,401,121,305,100,942,93,305,113,127,115,
220,108,100,316,93,142,105,316,113,648,557,100,118,142,699,104,362,261,262,108,
264,127,128,386,104,104,379,106,108,118,119,120,104,506,492,657,93,151,110,153,
154,113,395,100,98,510,95,397,105,163,164,104,106,245,168,169,170,110,104,251,
105,305,254,255,110,106,370,104,365,110,383,693,316,110,370,189,698,108,93,94,
426,720,395,429,99,431,122,104,93,435,126,621,405,110,407,100,405,93,407,110,463,
104,456,108,233,468,452,110,655,114,104,541,659,118,119,120,110,575,456,104,560,
93,435,420,560,110,435,99,100,101,93,561,445,105,773,825,445,100,108,104,103,109,
110,456,108,108,116,456,118,119,120,107,104,372,104,118,119,120,110,93,110,405,
109,407,600,104,100,480,477,93,94,480,608,93,108,99,632,707,292,110,100,728,640,
114,118,119,120,632,108,552,720,435,110,106,104,105,116,110,118,119,120,445,106,
106,826,110,110,110,110,114,106,325,456,327,328,329,330,331,332,333,334,335,336,
337,338,339,340,341,342,343,862,863,859,860,580,107,480,856,584,585,544,604,546,
773,598,858,559,861,544,113,546,122,868,869,473,126,108,449,106,870,871,453,110,
110,933,76,77,114,405,643,407,72,73,627,108,819,405,110,407,108,577,114,110,629,
658,598,114,109,110,598,108,864,865,866,867,109,110,770,5,6,38,415,108,612,108,
849,435,17,18,19,108,855,109,110,627,108,445,108,627,93,94,95,109,110,109,110,
799,456,764,109,110,100,124,679,680,800,682,78,684,123,78,105,688,480,648,889,
109,110,125,118,93,480,648,818,598,93,94,703,96,97,108,130,100,109,110,109,0,778,
108,39,108,22,23,111,112,109,110,850,492,852,118,51,601,627,928,929,106,125,22,
23,106,25,106,937,109,110,510,109,110,745,109,110,748,749,106,39,105,872,873,874,
93,884,46,109,110,109,110,51,109,110,109,110,109,110,58,22,23,97,104,63,109,110,
104,67,899,204,205,206,72,124,125,75,109,110,109,110,108,751,108,795,84,76,77,
799,106,751,72,73,128,109,110,575,825,97,109,110,114,236,107,598,93,105,106,107,
108,821,109,110,936,38,109,827,114,675,736,737,678,109,834,123,117,122,103,106,
128,121,106,121,627,79,108,113,109,109,850,110,819,106,142,109,819,857,108,147,
104,110,110,105,632,78,104,114,108,30,42,104,640,98,93,875,108,122,109,109,93,94,
849,96,97,93,849,100,855,110,110,109,855,110,110,108,220,743,111,112,746,110,115,
116,109,118,194,231,106,109,756,757,125,201,238,109,98,109,109,109,117,819,889,
108,105,104,889,104,121,109,117,104,220,121,105,781,101,106,113,263,786,787,25,
231,790,791,109,793,38,117,238,849,109,109,109,117,379,855,109,109,106,928,929,
105,104,928,929,132,93,104,937,98,395,109,937,263,109,400,401,108,110,109,94,106,
108,28,105,98,78,106,110,889,114,25,110,841,106,104,844,114,846,847,290,93,94,0,
96,97,108,114,100,78,78,121,123,109,108,305,108,94,106,111,112,114,109,93,21,109,
118,797,928,929,800,109,114,125,109,109,461,937,110,99,0,0,109,109,109,470,305,
895,818,46,567,668,574,231,43,52,53,54,55,138,236,612,608,604,61,232,825,130,496,
395,648,329,751,201,922,923,58,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,856,- 1,858,859,860,
861,862,863,864,865,866,867,868,869,870,871,872,873,874,395,- 1,397,105,106,107,
108,- 1,- 1,- 1,405,113,407,- 1,80,81,82,83,84,85,86,87,88,89,899,127,421,- 1,- 1,301,
560,- 1,- 1,- 1,- 1,307,308,309,568,- 1,142,107,- 1,- 1,- 1,- 1,- 1,319,- 1,- 1,- 1,- 1,324,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,936,- 1,- 1,- 1,- 1,- 1,- 1,- 1,500,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,355,- 1,480,- 1,- 1,360,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,500,
- 1,- 1,380,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,401,40,233,
- 1,43,1,- 1,- 1,568,668,- 1,- 1,51,52,574,- 1,55,56,57,58,544,- 1,546,- 1,63,- 1,- 1,66,-
1,25,26,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,699,- 1,38,444,568,- 1,- 1,- 1,- 1,- 1,574,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,305,- 1,- 1,- 1,- 1,
118,- 1,- 1,78,122,- 1,- 1,125,126,- 1,- 1,129,130,- 1,- 1,495,- 1,- 1,93,94,- 1,96,97,- 1,-
1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,778,- 1,- 1,118,- 1,648,- 1,122,-
1,- 1,125,126,- 1,- 1,129,130,- 1,- 1,- 1,- 1,- 1,- 1,542,- 1,- 1,- 1,- 1,- 1,- 1,- 1,673,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,562,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,- 1,407,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
435,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,445,- 1,- 1,- 1,- 1,- 1,621,- 1,- 1,- 1,- 1,456,- 1,751,- 1,
- 1,631,- 1,- 1,- 1,- 1,636,- 1,- 1,639,- 1,- 1,- 1,- 1,- 1,- 1,1,- 1,- 1,- 1,480,- 1,- 1,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,707,63,-
1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,720,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,
- 1,- 1,- 1,118,- 1,- 1,- 1,122,598,- 1,125,126,- 1,773,129,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1,627,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,
45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,
122,- 1,- 1,125,126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,
- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
- 1,- 1,125,126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,
101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,
- 1,125,126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,
102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,
125,126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,
23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,
50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,
126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,
126,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,- 1,1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,
- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,
- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,- 1,- 1,- 1,104,105,- 1,1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,
125,126,- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,-
1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,
- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
- 1,- 1,125,126,- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,-
1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,
- 1,122,- 1,- 1,125,126,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,43,44,- 1,- 1,
- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,-
1,- 1,125,126,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,
- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,
105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,
129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,
60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,129,130,0,1,- 1,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,111,112,
- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,
48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,
101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,- 1,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,
102,103,37,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,48,49,- 1,118,119,120,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,116,- 1,118,119,120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,
103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,116,- 1,118,119,120,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,
- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,
119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,107,39,- 1,
- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,- 1,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,98,99,- 1,101,102,103,44,- 1,- 1,
- 1,48,49,- 1,111,112,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,98,99,- 1,
101,102,103,48,49,25,26,- 1,- 1,- 1,111,112,32,- 1,59,60,61,- 1,38,- 1,40,- 1,- 1,43,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,90,66,- 1,93,- 1,70,71,
- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,- 1,- 1,115,116,- 1,118,25,26,
- 1,122,- 1,- 1,125,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,
105,106,- 1,108,- 1,- 1,- 1,112,- 1,- 1,115,116,- 1,118,25,26,- 1,122,- 1,- 1,125,126,- 1,-
1,129,130,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,-
1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,
112,- 1,- 1,115,116,- 1,118,25,26,- 1,122,- 1,- 1,125,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,
- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,25,26,108,- 1,- 1,- 1,112,- 1,- 1,115,116,- 1,118,38,-
1,40,122,- 1,43,125,126,- 1,- 1,129,130,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,-
1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,
117,118,43,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,-
1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,-
1,118,43,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,
118,43,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,
- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,
94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,
43,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,
- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,
- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,
- 1,122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,
70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,
122,- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,
- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,
- 1,- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,
- 1,125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,
125,126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,
25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,- 1,- 1,125,
126,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,
26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,
51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,-
1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,51,
52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,-
1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,51,52,
129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,-
1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,51,52,129,
130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,
- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,
55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,
108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,51,52,129,130,55,
56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,
- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,129,130,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,37,- 1,- 1,98,99,- 1,101,102,103,- 1,105,48,49,- 1,- 1,37,111,112,- 1,- 1,- 1,- 1,59,
60,61,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,104,90,- 1,- 1,- 1,- 1,- 1,111,
112,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _tagged_arr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple17{struct _tagged_arr
f1;void*f2;};static char _tmp409[8]="stdcall";static char _tmp40A[6]="cdecl";static
char _tmp40B[9]="fastcall";static char _tmp40C[9]="noreturn";static char _tmp40D[6]="const";
static char _tmp40E[8]="aligned";static char _tmp40F[7]="packed";static char _tmp410[7]="shared";
static char _tmp411[7]="unused";static char _tmp412[5]="weak";static char _tmp413[10]="dllimport";
static char _tmp414[10]="dllexport";static char _tmp415[23]="no_instrument_function";
static char _tmp416[12]="constructor";static char _tmp417[11]="destructor";static
char _tmp418[22]="no_check_memory_usage";static char _tmp419[5]="pure";struct
_tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){int
yystate;int yyn=0;int yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(short*)({
unsigned int _tmp9B5=10000;short*_tmp9B6=(short*)_cycalloc_atomic(_check_times(
sizeof(short),_tmp9B5));{unsigned int _tmp9B7=_tmp9B5;unsigned int i;for(i=0;i < 
_tmp9B7;i ++){_tmp9B6[i]=(short)0;}}_tmp9B6;});int yyvsp_offset;void**yyvs=(void**)({
unsigned int _tmp9B2=10000;void**_tmp9B3=(void**)_cycalloc(_check_times(sizeof(
void*),_tmp9B2));{unsigned int _tmp9B4=_tmp9B2;unsigned int i;for(i=0;i < _tmp9B4;i
++){_tmp9B3[i]=Cyc_yylval;}}_tmp9B3;});int yylsp_offset;struct Cyc_Yyltype*yyls=(
struct Cyc_Yyltype*)({unsigned int _tmp9AF=10000;struct Cyc_Yyltype*_tmp9B0=(struct
Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),_tmp9AF));{
unsigned int _tmp9B1=_tmp9AF;unsigned int i;for(i=0;i < _tmp9B1;i ++){_tmp9B0[i]=Cyc_yynewloc();}}
_tmp9B0;});int yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;
yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;
yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp35B="parser stack overflow";_tag_arr(_tmp35B,
sizeof(char),_get_zero_arr_size(_tmp35B,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(957,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();
if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar
<= 358?(int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 263;}
yyn +=yychar1;if((yyn < 0  || yyn > 6697) || Cyc_yycheck[
_check_known_subscript_notnull(6698,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6698,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 956)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(957,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(474,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1D7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1D8: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp35C;_push_handler(& _tmp35C);{int _tmp35E=0;if(setjmp(_tmp35C.handler))
_tmp35E=1;if(!_tmp35E){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp35D=(void*)_exn_thrown;void*_tmp360=_tmp35D;_LL1DB: if(*((void**)_tmp360)
!= Cyc_Core_Failure)goto _LL1DD;_LL1DC: x=0;goto _LL1DA;_LL1DD:;_LL1DE:(void)_throw(
_tmp360);_LL1DA:;}}}{struct _handler_cons _tmp361;_push_handler(& _tmp361);{int
_tmp363=0;if(setjmp(_tmp361.handler))_tmp363=1;if(!_tmp363){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp362=(void*)_exn_thrown;void*_tmp365=_tmp362;_LL1E0: if(*((void**)_tmp365)
!= Cyc_Core_Failure)goto _LL1E2;_LL1E1: y=0;goto _LL1DF;_LL1E2:;_LL1E3:(void)_throw(
_tmp365);_LL1DF:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp366=
_cycalloc(sizeof(*_tmp366));_tmp366[0]=({struct Cyc_DeclList_tok_struct _tmp367;
_tmp367.tag=Cyc_DeclList_tok;_tmp367.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp367;});_tmp366;});break;}
case 3: _LL1D9: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp368=_cycalloc(
sizeof(*_tmp368));_tmp368[0]=({struct Cyc_DeclList_tok_struct _tmp369;_tmp369.tag=
Cyc_DeclList_tok;_tmp369.f1=({struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*
_tmp36A));_tmp36A->hd=({struct Cyc_Absyn_Decl*_tmp36B=_cycalloc(sizeof(*_tmp36B));
_tmp36B->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp36C=_cycalloc(
sizeof(*_tmp36C));_tmp36C[0]=({struct Cyc_Absyn_Using_d_struct _tmp36D;_tmp36D.tag=
9;_tmp36D.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp36D.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp36D;});_tmp36C;}));
_tmp36B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp36B;});_tmp36A->tl=0;_tmp36A;});_tmp369;});
_tmp368;});Cyc_Lex_leave_using();break;case 4: _LL1E4: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_DeclList_tok_struct
_tmp36F;_tmp36F.tag=Cyc_DeclList_tok;_tmp36F.f1=({struct Cyc_List_List*_tmp370=
_cycalloc(sizeof(*_tmp370));_tmp370->hd=({struct Cyc_Absyn_Decl*_tmp371=_cycalloc(
sizeof(*_tmp371));_tmp371->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=({struct Cyc_Absyn_Using_d_struct
_tmp373;_tmp373.tag=9;_tmp373.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp373.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp373;});_tmp372;}));
_tmp371->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp371;});_tmp370->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp370;});_tmp36F;});
_tmp36E;});break;case 5: _LL1E5: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_DeclList_tok_struct
_tmp375;_tmp375.tag=Cyc_DeclList_tok;_tmp375.f1=({struct Cyc_List_List*_tmp376=
_cycalloc(sizeof(*_tmp376));_tmp376->hd=({struct Cyc_Absyn_Decl*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp379;_tmp379.tag=8;_tmp379.f1=({struct _tagged_arr*_tmp37A=_cycalloc(sizeof(*
_tmp37A));_tmp37A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp37A;});_tmp379.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp379;});_tmp378;}));
_tmp377->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp377;});_tmp376->tl=0;_tmp376;});_tmp375;});
_tmp374;});Cyc_Lex_leave_namespace();break;case 6: _LL1E6: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp37B;
_push_handler(& _tmp37B);{int _tmp37D=0;if(setjmp(_tmp37B.handler))_tmp37D=1;if(!
_tmp37D){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp37C=(void*)_exn_thrown;void*
_tmp37F=_tmp37C;_LL1E9: if(*((void**)_tmp37F)!= Cyc_Core_Failure)goto _LL1EB;
_LL1EA: nspace=({const char*_tmp380="";_tag_arr(_tmp380,sizeof(char),
_get_zero_arr_size(_tmp380,1));});goto _LL1E8;_LL1EB:;_LL1EC:(void)_throw(_tmp37F);
_LL1E8:;}}}{struct _handler_cons _tmp381;_push_handler(& _tmp381);{int _tmp383=0;if(
setjmp(_tmp381.handler))_tmp383=1;if(!_tmp383){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp382=(void*)_exn_thrown;void*_tmp385=_tmp382;_LL1EE: if(*((void**)_tmp385)
!= Cyc_Core_Failure)goto _LL1F0;_LL1EF: x=0;goto _LL1ED;_LL1F0:;_LL1F1:(void)_throw(
_tmp385);_LL1ED:;}}}{struct _handler_cons _tmp386;_push_handler(& _tmp386);{int
_tmp388=0;if(setjmp(_tmp386.handler))_tmp388=1;if(!_tmp388){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp387=(void*)_exn_thrown;void*_tmp38A=_tmp387;_LL1F3: if(*((void**)_tmp38A)
!= Cyc_Core_Failure)goto _LL1F5;_LL1F4: y=0;goto _LL1F2;_LL1F5:;_LL1F6:(void)_throw(
_tmp38A);_LL1F2:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp38B=
_cycalloc(sizeof(*_tmp38B));_tmp38B[0]=({struct Cyc_DeclList_tok_struct _tmp38C;
_tmp38C.tag=Cyc_DeclList_tok;_tmp38C.f1=({struct Cyc_List_List*_tmp38D=_cycalloc(
sizeof(*_tmp38D));_tmp38D->hd=({struct Cyc_Absyn_Decl*_tmp38E=_cycalloc(sizeof(*
_tmp38E));_tmp38E->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp38F=
_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp390;_tmp390.tag=8;_tmp390.f1=({struct _tagged_arr*_tmp391=_cycalloc(sizeof(*
_tmp391));_tmp391[0]=nspace;_tmp391;});_tmp390.f2=x;_tmp390;});_tmp38F;}));
_tmp38E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp38E;});_tmp38D->tl=y;_tmp38D;});_tmp38C;});
_tmp38B;});break;}case 7: _LL1E7: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp392="C";
_tag_arr(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp393="only extern \"C\" { ... } is allowed";_tag_arr(_tmp393,
sizeof(char),_get_zero_arr_size(_tmp393,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=({
struct Cyc_DeclList_tok_struct _tmp395;_tmp395.tag=Cyc_DeclList_tok;_tmp395.f1=({
struct Cyc_List_List*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->hd=({struct Cyc_Absyn_Decl*
_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp399;_tmp399.tag=10;_tmp399.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp399;});_tmp398;}));
_tmp397->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp397;});_tmp396->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp396;});_tmp395;});
_tmp394;});break;case 8: _LL1F7: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_DeclList_tok_struct
_tmp39B;_tmp39B.tag=Cyc_DeclList_tok;_tmp39B.f1=0;_tmp39B;});_tmp39A;});break;
case 9: _LL1F8: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp39C=_cycalloc(
sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_DeclList_tok_struct _tmp39D;_tmp39D.tag=
Cyc_DeclList_tok;_tmp39D.f1=({struct Cyc_List_List*_tmp39E=_cycalloc(sizeof(*
_tmp39E));_tmp39E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3A0;_tmp3A0.tag=1;_tmp3A0.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A0;});_tmp39F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp39E->tl=0;
_tmp39E;});_tmp39D;});_tmp39C;});break;case 10: _LL1F9: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1FA: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));
_tmp3A1[0]=({struct Cyc_DeclList_tok_struct _tmp3A2;_tmp3A2.tag=Cyc_DeclList_tok;
_tmp3A2.f1=0;_tmp3A2;});_tmp3A1;});break;case 12: _LL1FB: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_FnDecl_tok_struct
_tmp3A4;_tmp3A4.tag=Cyc_FnDecl_tok;_tmp3A4.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A4;});
_tmp3A3;});break;case 13: _LL1FC: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5[0]=({struct Cyc_FnDecl_tok_struct
_tmp3A6;_tmp3A6.tag=Cyc_FnDecl_tok;_tmp3A6.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A7;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A6;});
_tmp3A5;});break;case 14: _LL1FD: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({struct Cyc_FnDecl_tok_struct
_tmp3A9;_tmp3A9.tag=Cyc_FnDecl_tok;_tmp3A9.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A9;});
_tmp3A8;});break;case 15: _LL1FE: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA[0]=({struct Cyc_FnDecl_tok_struct
_tmp3AB;_tmp3AB.tag=Cyc_FnDecl_tok;_tmp3AB.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3AC;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3AB;});
_tmp3AA;});break;case 16: _LL1FF: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_FnDecl_tok_struct
_tmp3AE;_tmp3AE.tag=Cyc_FnDecl_tok;_tmp3AE.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3AF;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3AE;});
_tmp3AD;});break;case 17: _LL200: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({struct Cyc_FnDecl_tok_struct
_tmp3B1;_tmp3B1.tag=Cyc_FnDecl_tok;_tmp3B1.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3B2;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3B1;});
_tmp3B0;});break;case 18: _LL201: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 19: _LL202: Cyc_Lex_leave_using();
break;case 20: _LL203: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp3B3=
_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B3;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 21: _LL204: Cyc_Lex_leave_namespace();
break;case 22: _LL205: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3B4=
_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_DeclList_tok_struct _tmp3B5;
_tmp3B5.tag=Cyc_DeclList_tok;_tmp3B5.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3B5;});
_tmp3B4;});break;case 23: _LL206: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6[0]=({struct Cyc_DeclList_tok_struct
_tmp3B7;_tmp3B7.tag=Cyc_DeclList_tok;_tmp3B7.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3B7;});
_tmp3B6;});break;case 24: _LL207: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_DeclList_tok_struct
_tmp3B9;_tmp3B9.tag=Cyc_DeclList_tok;_tmp3B9.f1=({struct Cyc_List_List*_tmp3BA=
_cycalloc(sizeof(*_tmp3BA));_tmp3BA->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3BA->tl=0;
_tmp3BA;});_tmp3B9;});_tmp3B8;});break;case 25: _LL208: {struct Cyc_List_List*
_tmp3BB=0;{struct Cyc_List_List*_tmp3BC=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3BC != 0;
_tmp3BC=_tmp3BC->tl){struct _tagged_arr*_tmp3BD=(struct _tagged_arr*)_tmp3BC->hd;
struct _tuple1*_tmp3BE=({struct _tuple1*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));
_tmp3C1->f1=Cyc_Absyn_rel_ns_null;_tmp3C1->f2=_tmp3BD;_tmp3C1;});struct Cyc_Absyn_Vardecl*
_tmp3BF=Cyc_Absyn_new_vardecl(_tmp3BE,Cyc_Absyn_wildtyp(0),0);_tmp3BB=({struct
Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->hd=_tmp3BF;_tmp3C0->tl=
_tmp3BB;_tmp3C0;});}}_tmp3BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3BB);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3C2=
_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_DeclList_tok_struct _tmp3C3;
_tmp3C3.tag=Cyc_DeclList_tok;_tmp3C3.f1=({struct Cyc_List_List*_tmp3C4=_cycalloc(
sizeof(*_tmp3C4));_tmp3C4->hd=Cyc_Absyn_letv_decl(_tmp3BB,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3C4->tl=0;
_tmp3C4;});_tmp3C3;});_tmp3C2;});break;}case 26: _LL209: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL20A: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));
_tmp3C5[0]=({struct Cyc_DeclList_tok_struct _tmp3C6;_tmp3C6.tag=Cyc_DeclList_tok;
_tmp3C6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3C6;});_tmp3C5;});
break;case 28: _LL20B: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3C7=
_cycalloc(sizeof(*_tmp3C7));_tmp3C7[0]=({struct Cyc_DeclSpec_tok_struct _tmp3C8;
_tmp3C8.tag=Cyc_DeclSpec_tok;_tmp3C8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->sc=({struct Cyc_Core_Opt*_tmp3CA=
_cycalloc(sizeof(*_tmp3CA));_tmp3CA->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CA;});_tmp3C9->tq=Cyc_Absyn_empty_tqual();
_tmp3C9->type_specs=0;_tmp3C9->is_inline=0;_tmp3C9->attributes=0;_tmp3C9;});
_tmp3C8;});_tmp3C7;});break;case 29: _LL20C: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp3CB[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp3CC="Only one storage class is allowed in a declaration";
_tag_arr(_tmp3CC,sizeof(char),_get_zero_arr_size(_tmp3CC,51));}),_tag_arr(
_tmp3CB,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CE;_tmp3CE.tag=Cyc_DeclSpec_tok;_tmp3CE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->sc=({struct Cyc_Core_Opt*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3D0;});_tmp3CF->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3CF->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3CF->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3CF->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3CF;});_tmp3CE;});_tmp3CD;});break;case 30: _LL20D: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_DeclSpec_tok;_tmp3D2.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->sc=0;_tmp3D3->tq=Cyc_Absyn_empty_tqual();
_tmp3D3->type_specs=({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));
_tmp3D4->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D4->tl=0;_tmp3D4;});
_tmp3D3->is_inline=0;_tmp3D3->attributes=0;_tmp3D3;});_tmp3D2;});_tmp3D1;});
break;case 31: _LL20E: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3D5=
_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_DeclSpec_tok_struct _tmp3D6;
_tmp3D6.tag=Cyc_DeclSpec_tok;_tmp3D6.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3D7->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3D7->type_specs=({
struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3D8->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3D8;});
_tmp3D7->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3D7->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3D7;});
_tmp3D6;});_tmp3D5;});break;case 32: _LL20F: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3DA;_tmp3DA.tag=Cyc_DeclSpec_tok;_tmp3DA.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->sc=0;_tmp3DB->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3DB->type_specs=0;
_tmp3DB->is_inline=0;_tmp3DB->attributes=0;_tmp3DB;});_tmp3DA;});_tmp3D9;});
break;case 33: _LL210: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3DC=
_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_DeclSpec_tok_struct _tmp3DD;
_tmp3DD.tag=Cyc_DeclSpec_tok;_tmp3DD.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3DE->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3DE->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3DE->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3DE->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3DE;});_tmp3DD;});_tmp3DC;});break;case 34: _LL211: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3E0;_tmp3E0.tag=Cyc_DeclSpec_tok;_tmp3E0.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->sc=0;_tmp3E1->tq=Cyc_Absyn_empty_tqual();
_tmp3E1->type_specs=0;_tmp3E1->is_inline=1;_tmp3E1->attributes=0;_tmp3E1;});
_tmp3E0;});_tmp3DF;});break;case 35: _LL212: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3E3;_tmp3E3.tag=Cyc_DeclSpec_tok;_tmp3E3.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3E4->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3E4->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3E4->is_inline=1;_tmp3E4->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3E4;});
_tmp3E3;});_tmp3E2;});break;case 36: _LL213: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3E6;_tmp3E6.tag=Cyc_DeclSpec_tok;_tmp3E6.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->sc=0;_tmp3E7->tq=Cyc_Absyn_empty_tqual();
_tmp3E7->type_specs=0;_tmp3E7->is_inline=0;_tmp3E7->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E7;});_tmp3E6;});
_tmp3E5;});break;case 37: _LL214: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3E9;_tmp3E9.tag=Cyc_DeclSpec_tok;_tmp3E9.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3EA->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3EA->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3EA->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3EA->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3EA;});
_tmp3E9;});_tmp3E8;});break;case 38: _LL215: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({struct Cyc_StorageClass_tok_struct
_tmp3EC;_tmp3EC.tag=Cyc_StorageClass_tok;_tmp3EC.f1=(void*)((void*)4);_tmp3EC;});
_tmp3EB;});break;case 39: _LL216: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({struct Cyc_StorageClass_tok_struct
_tmp3EE;_tmp3EE.tag=Cyc_StorageClass_tok;_tmp3EE.f1=(void*)((void*)5);_tmp3EE;});
_tmp3ED;});break;case 40: _LL217: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF[0]=({struct Cyc_StorageClass_tok_struct
_tmp3F0;_tmp3F0.tag=Cyc_StorageClass_tok;_tmp3F0.f1=(void*)((void*)3);_tmp3F0;});
_tmp3EF;});break;case 41: _LL218: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1[0]=({struct Cyc_StorageClass_tok_struct
_tmp3F2;_tmp3F2.tag=Cyc_StorageClass_tok;_tmp3F2.f1=(void*)((void*)1);_tmp3F2;});
_tmp3F1;});break;case 42: _LL219: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3F3="C";
_tag_arr(_tmp3F3,sizeof(char),_get_zero_arr_size(_tmp3F3,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3F4="only extern or extern \"C\" is allowed";_tag_arr(_tmp3F4,
sizeof(char),_get_zero_arr_size(_tmp3F4,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5[0]=({
struct Cyc_StorageClass_tok_struct _tmp3F6;_tmp3F6.tag=Cyc_StorageClass_tok;
_tmp3F6.f1=(void*)((void*)2);_tmp3F6;});_tmp3F5;});break;case 43: _LL21A: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));
_tmp3F7[0]=({struct Cyc_StorageClass_tok_struct _tmp3F8;_tmp3F8.tag=Cyc_StorageClass_tok;
_tmp3F8.f1=(void*)((void*)0);_tmp3F8;});_tmp3F7;});break;case 44: _LL21B: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));
_tmp3F9[0]=({struct Cyc_StorageClass_tok_struct _tmp3FA;_tmp3FA.tag=Cyc_StorageClass_tok;
_tmp3FA.f1=(void*)((void*)6);_tmp3FA;});_tmp3F9;});break;case 45: _LL21C: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));
_tmp3FB[0]=({struct Cyc_AttributeList_tok_struct _tmp3FC;_tmp3FC.tag=Cyc_AttributeList_tok;
_tmp3FC.f1=0;_tmp3FC;});_tmp3FB;});break;case 46: _LL21D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 47: _LL21E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 48: _LL21F:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3FD=_cycalloc(sizeof(*
_tmp3FD));_tmp3FD[0]=({struct Cyc_AttributeList_tok_struct _tmp3FE;_tmp3FE.tag=Cyc_AttributeList_tok;
_tmp3FE.f1=({struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3FF->tl=0;_tmp3FF;});_tmp3FE;});_tmp3FD;});break;case 49:
_LL220: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp400=_cycalloc(
sizeof(*_tmp400));_tmp400[0]=({struct Cyc_AttributeList_tok_struct _tmp401;_tmp401.tag=
Cyc_AttributeList_tok;_tmp401.f1=({struct Cyc_List_List*_tmp402=_cycalloc(sizeof(*
_tmp402));_tmp402->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp402->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp402;});_tmp401;});
_tmp400;});break;case 50: _LL221: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple17 att_map[17]={{{_tmp409,_tmp409,_tmp409 + 8},(
void*)0},{{_tmp40A,_tmp40A,_tmp40A + 6},(void*)1},{{_tmp40B,_tmp40B,_tmp40B + 9},(
void*)2},{{_tmp40C,_tmp40C,_tmp40C + 9},(void*)3},{{_tmp40D,_tmp40D,_tmp40D + 6},(
void*)4},{{_tmp40E,_tmp40E,_tmp40E + 8},(void*)& att_aligned},{{_tmp40F,_tmp40F,
_tmp40F + 7},(void*)5},{{_tmp410,_tmp410,_tmp410 + 7},(void*)7},{{_tmp411,_tmp411,
_tmp411 + 7},(void*)8},{{_tmp412,_tmp412,_tmp412 + 5},(void*)9},{{_tmp413,_tmp413,
_tmp413 + 10},(void*)10},{{_tmp414,_tmp414,_tmp414 + 10},(void*)11},{{_tmp415,
_tmp415,_tmp415 + 23},(void*)12},{{_tmp416,_tmp416,_tmp416 + 12},(void*)13},{{
_tmp417,_tmp417,_tmp417 + 11},(void*)14},{{_tmp418,_tmp418,_tmp418 + 22},(void*)15},{{
_tmp419,_tmp419,_tmp419 + 5},(void*)16}};struct _tagged_arr _tmp403=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp403,sizeof(char))> 4  && *((const char*)_check_unknown_subscript(_tmp403,
sizeof(char),0))== '_') && *((const char*)_check_unknown_subscript(_tmp403,
sizeof(char),1))== '_') && *((const char*)_check_unknown_subscript(_tmp403,
sizeof(char),(int)(_get_arr_size(_tmp403,sizeof(char))- 2)))== '_') && *((const
char*)_check_unknown_subscript(_tmp403,sizeof(char),(int)(_get_arr_size(_tmp403,
sizeof(char))- 3)))== '_')_tmp403=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp403,2,_get_arr_size(_tmp403,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp403,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp404=_cycalloc(sizeof(*
_tmp404));_tmp404[0]=({struct Cyc_Attribute_tok_struct _tmp405;_tmp405.tag=Cyc_Attribute_tok;
_tmp405.f1=(void*)(att_map[i]).f2;_tmp405;});_tmp404;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp406="unrecognized attribute";_tag_arr(_tmp406,sizeof(char),
_get_zero_arr_size(_tmp406,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({
struct Cyc_Attribute_tok_struct _tmp408;_tmp408.tag=Cyc_Attribute_tok;_tmp408.f1=(
void*)((void*)1);_tmp408;});_tmp407;});}break;}}case 51: _LL222: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({
struct Cyc_Attribute_tok_struct _tmp41C;_tmp41C.tag=Cyc_Attribute_tok;_tmp41C.f1=(
void*)((void*)4);_tmp41C;});_tmp41B;});break;case 52: _LL223: {struct _tagged_arr
_tmp41D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp41F;int _tmp420;struct _tuple13*_tmp41E=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41F=*_tmp41E;
_tmp420=_tmp41F.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({const char*
_tmp421="regparm";_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,8));}))
== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({const char*_tmp422="__regparm__";
_tag_arr(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,12));}))== 0){if(
_tmp420 < 0  || _tmp420 > 3)Cyc_Parse_err(({const char*_tmp423="regparm requires value between 0 and 3";
_tag_arr(_tmp423,sizeof(char),_get_zero_arr_size(_tmp423,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp424=_cycalloc_atomic(sizeof(*_tmp424));
_tmp424[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp425;_tmp425.tag=0;_tmp425.f1=
_tmp420;_tmp425;});_tmp424;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({
const char*_tmp426="aligned";_tag_arr(_tmp426,sizeof(char),_get_zero_arr_size(
_tmp426,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({const char*
_tmp427="__aligned__";_tag_arr(_tmp427,sizeof(char),_get_zero_arr_size(_tmp427,
12));}))== 0){if(_tmp420 < 0)Cyc_Parse_err(({const char*_tmp428="aligned requires positive power of two";
_tag_arr(_tmp428,sizeof(char),_get_zero_arr_size(_tmp428,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp420;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp429="aligned requires positive power of two";
_tag_arr(_tmp429,sizeof(char),_get_zero_arr_size(_tmp429,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp42A=_cycalloc_atomic(sizeof(*_tmp42A));
_tmp42A[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp42B;_tmp42B.tag=1;_tmp42B.f1=
_tmp420;_tmp42B;});_tmp42A;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({
const char*_tmp42C="initializes";_tag_arr(_tmp42C,sizeof(char),_get_zero_arr_size(
_tmp42C,12));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp41D,({const char*
_tmp42D="__initializes__";_tag_arr(_tmp42D,sizeof(char),_get_zero_arr_size(
_tmp42D,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp42E=
_cycalloc_atomic(sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp42F;_tmp42F.tag=4;_tmp42F.f1=_tmp420;_tmp42F;});_tmp42E;});else{Cyc_Parse_err(({
const char*_tmp430="unrecognized attribute";_tag_arr(_tmp430,sizeof(char),
_get_zero_arr_size(_tmp430,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp431=_cycalloc(sizeof(*_tmp431));
_tmp431[0]=({struct Cyc_Attribute_tok_struct _tmp432;_tmp432.tag=Cyc_Attribute_tok;
_tmp432.f1=(void*)a;_tmp432;});_tmp431;});break;}}case 53: _LL224: {struct
_tagged_arr _tmp433=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp434=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp433,({const char*_tmp435="section";_tag_arr(_tmp435,sizeof(
char),_get_zero_arr_size(_tmp435,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)
_tmp433,({const char*_tmp436="__section__";_tag_arr(_tmp436,sizeof(char),
_get_zero_arr_size(_tmp436,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct Cyc_Absyn_Section_att_struct
_tmp438;_tmp438.tag=2;_tmp438.f1=_tmp434;_tmp438;});_tmp437;});else{Cyc_Parse_err(({
const char*_tmp439="unrecognized attribute";_tag_arr(_tmp439,sizeof(char),
_get_zero_arr_size(_tmp439,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));
_tmp43A[0]=({struct Cyc_Attribute_tok_struct _tmp43B;_tmp43B.tag=Cyc_Attribute_tok;
_tmp43B.f1=(void*)a;_tmp43B;});_tmp43A;});break;}case 54: _LL225: {struct
_tagged_arr _tmp43C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp43D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp43C,({const char*_tmp43E="__mode__";_tag_arr(_tmp43E,
sizeof(char),_get_zero_arr_size(_tmp43E,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp440;_tmp440.tag=5;_tmp440.f1=_tmp43D;_tmp440;});_tmp43F;});else{Cyc_Parse_err(({
const char*_tmp441="unrecognized attribute";_tag_arr(_tmp441,sizeof(char),
_get_zero_arr_size(_tmp441,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp442=_cycalloc(sizeof(*_tmp442));
_tmp442[0]=({struct Cyc_Attribute_tok_struct _tmp443;_tmp443.tag=Cyc_Attribute_tok;
_tmp443.f1=(void*)a;_tmp443;});_tmp442;});break;}case 55: _LL226: {struct
_tagged_arr _tmp444=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp445=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp447;struct
_tuple13 _tmp446=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp447=_tmp446.f2;{int _tmp449;struct _tuple13 _tmp448=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp449=_tmp448.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp444,({const char*_tmp44A="format";
_tag_arr(_tmp44A,sizeof(char),_get_zero_arr_size(_tmp44A,7));}))== 0  || Cyc_zstrcmp((
struct _tagged_arr)_tmp444,({const char*_tmp44B="__format__";_tag_arr(_tmp44B,
sizeof(char),_get_zero_arr_size(_tmp44B,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp445,({const char*_tmp44C="printf";_tag_arr(_tmp44C,sizeof(char),
_get_zero_arr_size(_tmp44C,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_Absyn_Format_att_struct
_tmp44E;_tmp44E.tag=3;_tmp44E.f1=(void*)((void*)0);_tmp44E.f2=_tmp447;_tmp44E.f3=
_tmp449;_tmp44E;});_tmp44D;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp445,({
const char*_tmp44F="scanf";_tag_arr(_tmp44F,sizeof(char),_get_zero_arr_size(
_tmp44F,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp450=
_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_Absyn_Format_att_struct
_tmp451;_tmp451.tag=3;_tmp451.f1=(void*)((void*)1);_tmp451.f2=_tmp447;_tmp451.f3=
_tmp449;_tmp451;});_tmp450;});else{Cyc_Parse_err(({const char*_tmp452="unrecognized format type";
_tag_arr(_tmp452,sizeof(char),_get_zero_arr_size(_tmp452,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp453="unrecognized attribute";_tag_arr(_tmp453,sizeof(char),
_get_zero_arr_size(_tmp453,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454[0]=({
struct Cyc_Attribute_tok_struct _tmp455;_tmp455.tag=Cyc_Attribute_tok;_tmp455.f1=(
void*)a;_tmp455;});_tmp454;});break;}}}case 56: _LL227: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp457;_tmp457.tag=Cyc_TypeSpecifier_tok;_tmp457.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp457;});_tmp456;});break;case 57: _LL228: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp458=_cycalloc(sizeof(*_tmp458));
_tmp458[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp459;_tmp459.tag=Cyc_TypeSpecifier_tok;
_tmp459.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp459;});
_tmp458;});break;case 58: _LL229: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45B;_tmp45B.tag=Cyc_TypeSpecifier_tok;_tmp45B.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp45C=_cycalloc(sizeof(*_tmp45C));
_tmp45C->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp45C;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45B;});
_tmp45A;});break;case 59: _LL22A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45E;_tmp45E.tag=Cyc_TypeSpecifier_tok;_tmp45E.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45E;});
_tmp45D;});break;case 60: _LL22B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp460;_tmp460.tag=Cyc_TypeSpecifier_tok;_tmp460.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_Parse_Short_spec_struct
_tmp462;_tmp462.tag=2;_tmp462.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp462;});
_tmp461;}));_tmp460;});_tmp45F;});break;case 61: _LL22C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp464;_tmp464.tag=Cyc_TypeSpecifier_tok;_tmp464.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp464;});
_tmp463;});break;case 62: _LL22D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp466;_tmp466.tag=Cyc_TypeSpecifier_tok;_tmp466.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Parse_Long_spec_struct
_tmp468;_tmp468.tag=3;_tmp468.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp468;});
_tmp467;}));_tmp466;});_tmp465;});break;case 63: _LL22E: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp46A;_tmp46A.tag=Cyc_TypeSpecifier_tok;_tmp46A.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46A;});
_tmp469;});break;case 64: _LL22F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp46C;_tmp46C.tag=Cyc_TypeSpecifier_tok;_tmp46C.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46C;});
_tmp46B;});break;case 65: _LL230: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp46E;_tmp46E.tag=Cyc_TypeSpecifier_tok;_tmp46E.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp470;_tmp470.tag=0;_tmp470.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp470;});
_tmp46F;}));_tmp46E;});_tmp46D;});break;case 66: _LL231: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp472;_tmp472.tag=Cyc_TypeSpecifier_tok;_tmp472.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp474;_tmp474.tag=1;_tmp474.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp474;});
_tmp473;}));_tmp472;});_tmp471;});break;case 67: _LL232: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68: _LL233: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL234:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 70:
_LL235: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp475=_cycalloc(
sizeof(*_tmp475));_tmp475[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp476;_tmp476.tag=
Cyc_TypeSpecifier_tok;_tmp476.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp476;});
_tmp475;});break;case 71: _LL236: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp478;_tmp478.tag=Cyc_TypeSpecifier_tok;_tmp478.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp479=_cycalloc(sizeof(*_tmp479));
_tmp479[0]=({struct Cyc_Absyn_TypedefType_struct _tmp47A;_tmp47A.tag=16;_tmp47A.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp47A.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp47A.f3=0;_tmp47A.f4=0;_tmp47A;});_tmp479;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp478;});
_tmp477;});break;case 72: _LL237: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47C;_tmp47C.tag=Cyc_TypeSpecifier_tok;_tmp47C.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));
_tmp47D[0]=({struct Cyc_Absyn_TupleType_struct _tmp47E;_tmp47E.tag=9;_tmp47E.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp47E;});_tmp47D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47C;});_tmp47B;});break;case 73: _LL238: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));
_tmp47F[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp480;_tmp480.tag=Cyc_TypeSpecifier_tok;
_tmp480.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp482;_tmp482.tag=15;_tmp482.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp482;});_tmp481;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp480;});_tmp47F;});break;case 74: _LL239: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp484;_tmp484.tag=Cyc_TypeSpecifier_tok;
_tmp484.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp486;_tmp486.tag=14;_tmp486.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp486;});_tmp485;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp484;});_tmp483;});break;case 75: _LL23A: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp487=_cycalloc(sizeof(*_tmp487));
_tmp487[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp488;_tmp488.tag=Cyc_TypeSpecifier_tok;
_tmp488.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_TagType_struct
_tmp48A;_tmp48A.tag=17;_tmp48A.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48A;});_tmp489;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp488;});_tmp487;});break;case 76: _LL23B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));
_tmp48B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp48C;_tmp48C.tag=Cyc_TypeSpecifier_tok;
_tmp48C.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_Absyn_TagType_struct
_tmp48E;_tmp48E.tag=17;_tmp48E.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->v=(void*)((void*)5);_tmp48F;}),0);
_tmp48E;});_tmp48D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48C;});
_tmp48B;});break;case 77: _LL23C: {struct _tuple13 _tmp491;int _tmp492;struct _tuple13*
_tmp490=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp491=*_tmp490;_tmp492=_tmp491.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp494;_tmp494.tag=Cyc_TypeSpecifier_tok;_tmp494.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp495=_cycalloc_atomic(sizeof(*_tmp495));
_tmp495[0]=({struct Cyc_Absyn_TypeInt_struct _tmp496;_tmp496.tag=18;_tmp496.f1=
_tmp492;_tmp496;});_tmp495;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp494;});
_tmp493;});break;}case 78: _LL23D: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp497=
_cycalloc(sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Kind_tok_struct _tmp498;
_tmp498.tag=Cyc_Kind_tok;_tmp498.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp498;});
_tmp497;});break;case 79: _LL23E: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp499=_cycalloc_atomic(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_TypeQual_tok_struct
_tmp49A;_tmp49A.tag=Cyc_TypeQual_tok;_tmp49A.f1=({struct Cyc_Absyn_Tqual _tmp49B;
_tmp49B.print_const=1;_tmp49B.q_volatile=0;_tmp49B.q_restrict=0;_tmp49B.real_const=
1;_tmp49B;});_tmp49A;});_tmp499;});break;case 80: _LL23F: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp49C=_cycalloc_atomic(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_TypeQual_tok_struct
_tmp49D;_tmp49D.tag=Cyc_TypeQual_tok;_tmp49D.f1=({struct Cyc_Absyn_Tqual _tmp49E;
_tmp49E.print_const=0;_tmp49E.q_volatile=1;_tmp49E.q_restrict=0;_tmp49E.real_const=
0;_tmp49E;});_tmp49D;});_tmp49C;});break;case 81: _LL240: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp49F=_cycalloc_atomic(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_TypeQual_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_TypeQual_tok;_tmp4A0.f1=({struct Cyc_Absyn_Tqual _tmp4A1;
_tmp4A1.print_const=0;_tmp4A1.q_volatile=0;_tmp4A1.q_restrict=1;_tmp4A1.real_const=
0;_tmp4A1;});_tmp4A0;});_tmp49F;});break;case 82: _LL241: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_TypeSpecifier_tok;_tmp4A3.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4A5;_tmp4A5.tag=5;_tmp4A5.f1=({struct Cyc_Absyn_Decl*_tmp4A6=_cycalloc(
sizeof(*_tmp4A6));_tmp4A6->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4A8;_tmp4A8.tag=6;_tmp4A8.f1=({struct Cyc_Absyn_Enumdecl*_tmp4A9=_cycalloc(
sizeof(*_tmp4A9));_tmp4A9->sc=(void*)((void*)2);_tmp4A9->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4A9->fields=({
struct Cyc_Core_Opt*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AA;});_tmp4A9;});
_tmp4A8;});_tmp4A7;}));_tmp4A6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A6;});
_tmp4A5;});_tmp4A4;}));_tmp4A3;});_tmp4A2;});break;case 83: _LL242: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4AC;_tmp4AC.tag=Cyc_TypeSpecifier_tok;
_tmp4AC.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_EnumType_struct
_tmp4AE;_tmp4AE.tag=12;_tmp4AE.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4AE.f2=0;_tmp4AE;});
_tmp4AD;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AC;});_tmp4AB;});break;case 84: _LL243: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));
_tmp4AF[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B0;_tmp4B0.tag=Cyc_TypeSpecifier_tok;
_tmp4B0.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4B1=_cycalloc(
sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_Parse_Type_spec_struct _tmp4B2;_tmp4B2.tag=
4;_tmp4B2.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp4B3=
_cycalloc(sizeof(*_tmp4B3));_tmp4B3[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4B4;_tmp4B4.tag=13;_tmp4B4.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B4;});_tmp4B3;}));
_tmp4B2.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4B2;});_tmp4B1;}));_tmp4B0;});_tmp4AF;});break;
case 85: _LL244: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4B5=_cycalloc(
sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Enumfield_tok_struct _tmp4B6;_tmp4B6.tag=
Cyc_Enumfield_tok;_tmp4B6.f1=({struct Cyc_Absyn_Enumfield*_tmp4B7=_cycalloc(
sizeof(*_tmp4B7));_tmp4B7->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B7->tag=0;_tmp4B7->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4B7;});_tmp4B6;});_tmp4B5;});break;case 86: _LL245:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));
_tmp4B8[0]=({struct Cyc_Enumfield_tok_struct _tmp4B9;_tmp4B9.tag=Cyc_Enumfield_tok;
_tmp4B9.f1=({struct Cyc_Absyn_Enumfield*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4BA->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4BA->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4BA;});
_tmp4B9;});_tmp4B8;});break;case 87: _LL246: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4BC;_tmp4BC.tag=Cyc_EnumfieldList_tok;_tmp4BC.f1=({struct Cyc_List_List*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4BD->tl=0;_tmp4BD;});
_tmp4BC;});_tmp4BB;});break;case 88: _LL247: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4BF;_tmp4BF.tag=Cyc_EnumfieldList_tok;_tmp4BF.f1=({struct Cyc_List_List*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C0->tl=0;_tmp4C0;});
_tmp4BF;});_tmp4BE;});break;case 89: _LL248: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4C2;_tmp4C2.tag=Cyc_EnumfieldList_tok;_tmp4C2.f1=({struct Cyc_List_List*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C3->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C3;});_tmp4C2;});
_tmp4C1;});break;case 90: _LL249: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4C5;_tmp4C5.tag=Cyc_TypeSpecifier_tok;_tmp4C5.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));
_tmp4C6[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4C7;_tmp4C7.tag=11;_tmp4C7.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4C7.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C7;});_tmp4C6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C5;});_tmp4C4;});break;case 91: _LL24A: {struct
Cyc_List_List*_tmp4C8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4CB;_tmp4CB.tag=Cyc_TypeSpecifier_tok;
_tmp4CB.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4CC=_cycalloc(
sizeof(*_tmp4CC));_tmp4CC[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4CD;_tmp4CD.tag=
5;_tmp4CD.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4C8,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4C9,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CD;});
_tmp4CC;}));_tmp4CB;});_tmp4CA;});break;}case 92: _LL24B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4CF;_tmp4CF.tag=Cyc_TypeSpecifier_tok;_tmp4CF.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));
_tmp4D0[0]=({struct Cyc_Absyn_AggrType_struct _tmp4D1;_tmp4D1.tag=10;_tmp4D1.f1=({
struct Cyc_Absyn_AggrInfo _tmp4D2;_tmp4D2.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4D4;_tmp4D4.tag=0;_tmp4D4.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D4.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D4;});_tmp4D3;}));
_tmp4D2.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4D2;});_tmp4D1;});_tmp4D0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CF;});
_tmp4CE;});break;case 93: _LL24C: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_TypeList_tok_struct
_tmp4D6;_tmp4D6.tag=Cyc_TypeList_tok;_tmp4D6.f1=0;_tmp4D6;});_tmp4D5;});break;
case 94: _LL24D: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp4D7=_cycalloc(
sizeof(*_tmp4D7));_tmp4D7[0]=({struct Cyc_TypeList_tok_struct _tmp4D8;_tmp4D8.tag=
Cyc_TypeList_tok;_tmp4D8.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4D8;});_tmp4D7;});
break;case 95: _LL24E: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp4D9=
_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4DA;_tmp4DA.tag=Cyc_StructOrUnion_tok;_tmp4DA.f1=(void*)((void*)0);_tmp4DA;});
_tmp4D9;});break;case 96: _LL24F: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4DC;_tmp4DC.tag=Cyc_StructOrUnion_tok;_tmp4DC.f1=(void*)((void*)1);_tmp4DC;});
_tmp4DB;});break;case 97: _LL250: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4DE;_tmp4DE.tag=Cyc_AggrFieldDeclList_tok;_tmp4DE.f1=0;_tmp4DE;});_tmp4DD;});
break;case 98: _LL251: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4E0;_tmp4E0.tag=Cyc_AggrFieldDeclList_tok;_tmp4E0.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4E0;});_tmp4DF;});
break;case 99: _LL252: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4E2;_tmp4E2.tag=Cyc_AggrFieldDeclListList_tok;_tmp4E2.f1=({struct Cyc_List_List*
_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E3->tl=0;_tmp4E3;});
_tmp4E2;});_tmp4E1;});break;case 100: _LL253: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4E5;_tmp4E5.tag=Cyc_AggrFieldDeclListList_tok;_tmp4E5.f1=({struct Cyc_List_List*
_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E6->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E6;});_tmp4E5;});
_tmp4E4;});break;case 101: _LL254: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4E8;_tmp4E8.tag=Cyc_InitDeclList_tok;_tmp4E8.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4E8;});_tmp4E7;});
break;case 102: _LL255: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4E9=
_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4EA;_tmp4EA.tag=Cyc_InitDeclList_tok;_tmp4EA.f1=({struct Cyc_List_List*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EB->tl=0;_tmp4EB;});
_tmp4EA;});_tmp4E9;});break;case 103: _LL256: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4ED;_tmp4ED.tag=Cyc_InitDeclList_tok;_tmp4ED.f1=({struct Cyc_List_List*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EE->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EE;});_tmp4ED;});
_tmp4EC;});break;case 104: _LL257: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF[0]=({struct Cyc_InitDecl_tok_struct
_tmp4F0;_tmp4F0.tag=Cyc_InitDecl_tok;_tmp4F0.f1=({struct _tuple12*_tmp4F1=
_cycalloc(sizeof(*_tmp4F1));_tmp4F1->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F1->f2=0;_tmp4F1;});
_tmp4F0;});_tmp4EF;});break;case 105: _LL258: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_InitDecl_tok_struct
_tmp4F3;_tmp4F3.tag=Cyc_InitDecl_tok;_tmp4F3.f1=({struct _tuple12*_tmp4F4=
_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F4->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4F4;});_tmp4F3;});_tmp4F2;});break;case 106: _LL259: {struct _tuple15 _tmp4F6;
struct Cyc_Absyn_Tqual _tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_List_List*
_tmp4F9;struct _tuple15*_tmp4F5=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F6=*_tmp4F5;_tmp4F7=
_tmp4F6.f1;_tmp4F8=_tmp4F6.f2;_tmp4F9=_tmp4F6.f3;{struct Cyc_List_List*_tmp4FB;
struct Cyc_List_List*_tmp4FC;struct _tuple0 _tmp4FA=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4FB=_tmp4FA.f1;
_tmp4FC=_tmp4FA.f2;{void*_tmp4FD=Cyc_Parse_speclist2typ(_tmp4F8,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4F7,_tmp4FD,_tmp4FB,_tmp4F9),_tmp4FC);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4FF=_cycalloc(sizeof(*
_tmp4FF));_tmp4FF[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp500;_tmp500.tag=
Cyc_AggrFieldDeclList_tok;_tmp500.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4FE);
_tmp500;});_tmp4FF;});break;}}}case 107: _LL25A: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501[0]=({struct Cyc_QualSpecList_tok_struct
_tmp502;_tmp502.tag=Cyc_QualSpecList_tok;_tmp502.f1=({struct _tuple15*_tmp503=
_cycalloc(sizeof(*_tmp503));_tmp503->f1=Cyc_Absyn_empty_tqual();_tmp503->f2=({
struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp504->tl=0;_tmp504;});
_tmp503->f3=0;_tmp503;});_tmp502;});_tmp501;});break;case 108: _LL25B: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({
struct Cyc_QualSpecList_tok_struct _tmp506;_tmp506.tag=Cyc_QualSpecList_tok;
_tmp506.f1=({struct _tuple15*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp507->f2=({struct Cyc_List_List*_tmp508=_cycalloc(sizeof(*
_tmp508));_tmp508->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp508->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp508;});_tmp507->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp507;});_tmp506;});_tmp505;});break;case 109: _LL25C: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp509=_cycalloc(sizeof(*_tmp509));
_tmp509[0]=({struct Cyc_QualSpecList_tok_struct _tmp50A;_tmp50A.tag=Cyc_QualSpecList_tok;
_tmp50A.f1=({struct _tuple15*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50B->f2=0;_tmp50B->f3=
0;_tmp50B;});_tmp50A;});_tmp509;});break;case 110: _LL25D: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_QualSpecList_tok_struct
_tmp50D;_tmp50D.tag=Cyc_QualSpecList_tok;_tmp50D.f1=({struct _tuple15*_tmp50E=
_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp50E->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp50E->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp50E;});_tmp50D;});
_tmp50C;});break;case 111: _LL25E: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_QualSpecList_tok_struct
_tmp510;_tmp510.tag=Cyc_QualSpecList_tok;_tmp510.f1=({struct _tuple15*_tmp511=
_cycalloc(sizeof(*_tmp511));_tmp511->f1=Cyc_Absyn_empty_tqual();_tmp511->f2=0;
_tmp511->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp511;});_tmp510;});_tmp50F;});break;case 112: _LL25F:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp512=_cycalloc(sizeof(*
_tmp512));_tmp512[0]=({struct Cyc_QualSpecList_tok_struct _tmp513;_tmp513.tag=Cyc_QualSpecList_tok;
_tmp513.f1=({struct _tuple15*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp514->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp514->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp514;});_tmp513;});
_tmp512;});break;case 113: _LL260: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_InitDeclList_tok_struct
_tmp516;_tmp516.tag=Cyc_InitDeclList_tok;_tmp516.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp516;});_tmp515;});
break;case 114: _LL261: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp517=
_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_InitDeclList_tok_struct
_tmp518;_tmp518.tag=Cyc_InitDeclList_tok;_tmp518.f1=({struct Cyc_List_List*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519->tl=0;_tmp519;});
_tmp518;});_tmp517;});break;case 115: _LL262: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51B;_tmp51B.tag=Cyc_InitDeclList_tok;_tmp51B.f1=({struct Cyc_List_List*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51C->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51C;});_tmp51B;});
_tmp51A;});break;case 116: _LL263: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_InitDecl_tok_struct
_tmp51E;_tmp51E.tag=Cyc_InitDecl_tok;_tmp51E.f1=({struct _tuple12*_tmp51F=
_cycalloc(sizeof(*_tmp51F));_tmp51F->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F->f2=0;_tmp51F;});
_tmp51E;});_tmp51D;});break;case 117: _LL264: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520[0]=({struct Cyc_InitDecl_tok_struct
_tmp521;_tmp521.tag=Cyc_InitDecl_tok;_tmp521.f1=({struct _tuple12*_tmp522=
_cycalloc(sizeof(*_tmp522));_tmp522->f1=({struct Cyc_Parse_Declarator*_tmp523=
_cycalloc(sizeof(*_tmp523));_tmp523->id=({struct _tuple1*_tmp524=_cycalloc(
sizeof(*_tmp524));_tmp524->f1=Cyc_Absyn_rel_ns_null;_tmp524->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp524;});_tmp523->tms=
0;_tmp523;});_tmp522->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp522;});_tmp521;});
_tmp520;});break;case 118: _LL265: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({struct Cyc_InitDecl_tok_struct
_tmp527;_tmp527.tag=Cyc_InitDecl_tok;_tmp527.f1=({struct _tuple12*_tmp528=
_cycalloc(sizeof(*_tmp528));_tmp528->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp528->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp528;});_tmp527;});_tmp526;});break;case 119: _LL266: {struct Cyc_List_List*
_tmp529=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp52B;_tmp52B.tag=Cyc_TypeSpecifier_tok;
_tmp52B.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp52C=_cycalloc(
sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_Parse_Decl_spec_struct _tmp52D;_tmp52D.tag=
5;_tmp52D.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp529,({struct Cyc_Core_Opt*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52E;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52D;});
_tmp52C;}));_tmp52B;});_tmp52A;});break;}case 120: _LL267: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp530;_tmp530.tag=Cyc_TypeSpecifier_tok;_tmp530.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp531=_cycalloc(sizeof(*_tmp531));
_tmp531[0]=({struct Cyc_Absyn_TunionType_struct _tmp532;_tmp532.tag=2;_tmp532.f1=({
struct Cyc_Absyn_TunionInfo _tmp533;_tmp533.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp535;_tmp535.tag=0;_tmp535.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp536;
_tmp536.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp536.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp536;});_tmp535;});
_tmp534;}));_tmp533.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp533.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp533;});_tmp532;});
_tmp531;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp530;});_tmp52F;});break;case 121: _LL268: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp537=_cycalloc(sizeof(*_tmp537));
_tmp537[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp538;_tmp538.tag=Cyc_TypeSpecifier_tok;
_tmp538.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp53A;_tmp53A.tag=3;_tmp53A.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp53B;
_tmp53B.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp53D;_tmp53D.tag=0;_tmp53D.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp53E;
_tmp53E.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp53E.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53E.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp53E;});_tmp53D;});
_tmp53C;}));_tmp53B.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53B;});_tmp53A;});
_tmp539;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp538;});_tmp537;});break;case 122: _LL269: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp53F=_cycalloc_atomic(sizeof(*_tmp53F));
_tmp53F[0]=({struct Cyc_Bool_tok_struct _tmp540;_tmp540.tag=Cyc_Bool_tok;_tmp540.f1=
0;_tmp540;});_tmp53F;});break;case 123: _LL26A: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp541=_cycalloc_atomic(sizeof(*_tmp541));_tmp541[0]=({struct Cyc_Bool_tok_struct
_tmp542;_tmp542.tag=Cyc_Bool_tok;_tmp542.f1=1;_tmp542;});_tmp541;});break;case
124: _LL26B: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp543=_cycalloc(
sizeof(*_tmp543));_tmp543[0]=({struct Cyc_TunionFieldList_tok_struct _tmp544;
_tmp544.tag=Cyc_TunionFieldList_tok;_tmp544.f1=({struct Cyc_List_List*_tmp545=
_cycalloc(sizeof(*_tmp545));_tmp545->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp545->tl=0;_tmp545;});
_tmp544;});_tmp543;});break;case 125: _LL26C: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp547;_tmp547.tag=Cyc_TunionFieldList_tok;_tmp547.f1=({struct Cyc_List_List*
_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp548->tl=0;_tmp548;});
_tmp547;});_tmp546;});break;case 126: _LL26D: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp54A;_tmp54A.tag=Cyc_TunionFieldList_tok;_tmp54A.f1=({struct Cyc_List_List*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54B->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54B;});_tmp54A;});
_tmp549;});break;case 127: _LL26E: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp54D;_tmp54D.tag=Cyc_TunionFieldList_tok;_tmp54D.f1=({struct Cyc_List_List*
_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54E->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54E;});_tmp54D;});
_tmp54C;});break;case 128: _LL26F: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F[0]=({struct Cyc_Scope_tok_struct
_tmp550;_tmp550.tag=Cyc_Scope_tok;_tmp550.f1=(void*)((void*)2);_tmp550;});
_tmp54F;});break;case 129: _LL270: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[0]=({struct Cyc_Scope_tok_struct
_tmp552;_tmp552.tag=Cyc_Scope_tok;_tmp552.f1=(void*)((void*)3);_tmp552;});
_tmp551;});break;case 130: _LL271: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Scope_tok_struct
_tmp554;_tmp554.tag=Cyc_Scope_tok;_tmp554.f1=(void*)((void*)0);_tmp554;});
_tmp553;});break;case 131: _LL272: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_TunionField_tok_struct
_tmp556;_tmp556.tag=Cyc_TunionField_tok;_tmp556.f1=({struct Cyc_Absyn_Tunionfield*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp557->typs=0;_tmp557->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp557->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp557;});_tmp556;});
_tmp555;});break;case 132: _LL273: {struct Cyc_List_List*_tmp558=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({
struct Cyc_TunionField_tok_struct _tmp55A;_tmp55A.tag=Cyc_TunionField_tok;_tmp55A.f1=({
struct Cyc_Absyn_Tunionfield*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp55B->typs=_tmp558;_tmp55B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp55B->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp55B;});_tmp55A;});_tmp559;});break;}case 133: _LL274: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 134: _LL275: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));
_tmp55C[0]=({struct Cyc_Declarator_tok_struct _tmp55D;_tmp55D.tag=Cyc_Declarator_tok;
_tmp55D.f1=({struct Cyc_Parse_Declarator*_tmp55E=_cycalloc(sizeof(*_tmp55E));
_tmp55E->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp55E->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp55E;});
_tmp55D;});_tmp55C;});break;case 135: _LL276: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Declarator_tok_struct
_tmp560;_tmp560.tag=Cyc_Declarator_tok;_tmp560.f1=({struct Cyc_Parse_Declarator*
_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp561->tms=0;_tmp561;});
_tmp560;});_tmp55F;});break;case 136: _LL277: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 137: _LL278: {
struct Cyc_Parse_Declarator*_tmp562=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp562->tms=({struct Cyc_List_List*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp565;_tmp565.tag=5;_tmp565.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp565.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp565;});_tmp564;}));_tmp563->tl=_tmp562->tms;_tmp563;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 138:
_LL279: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp566=_cycalloc(sizeof(*
_tmp566));_tmp566[0]=({struct Cyc_Declarator_tok_struct _tmp567;_tmp567.tag=Cyc_Declarator_tok;
_tmp567.f1=({struct Cyc_Parse_Declarator*_tmp568=_cycalloc(sizeof(*_tmp568));
_tmp568->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp568->tms=({struct Cyc_List_List*_tmp569=_cycalloc(
sizeof(*_tmp569));_tmp569->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp56B;_tmp56B.tag=0;_tmp56B.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56B;});_tmp56A;}));
_tmp569->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp569;});_tmp568;});_tmp567;});_tmp566;});break;case
139: _LL27A: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp56C=_cycalloc(
sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_Declarator_tok_struct _tmp56D;_tmp56D.tag=
Cyc_Declarator_tok;_tmp56D.f1=({struct Cyc_Parse_Declarator*_tmp56E=_cycalloc(
sizeof(*_tmp56E));_tmp56E->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp56E->tms=({
struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp570=_cycalloc(sizeof(*_tmp570));
_tmp570[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp571;_tmp571.tag=1;_tmp571.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp571.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp571;});_tmp570;}));_tmp56F->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp56F;});_tmp56E;});
_tmp56D;});_tmp56C;});break;case 140: _LL27B: {struct _tuple14 _tmp573;struct Cyc_List_List*
_tmp574;int _tmp575;struct Cyc_Absyn_VarargInfo*_tmp576;struct Cyc_Core_Opt*_tmp577;
struct Cyc_List_List*_tmp578;struct _tuple14*_tmp572=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp573=*_tmp572;_tmp574=
_tmp573.f1;_tmp575=_tmp573.f2;_tmp576=_tmp573.f3;_tmp577=_tmp573.f4;_tmp578=
_tmp573.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp579=_cycalloc(
sizeof(*_tmp579));_tmp579[0]=({struct Cyc_Declarator_tok_struct _tmp57A;_tmp57A.tag=
Cyc_Declarator_tok;_tmp57A.f1=({struct Cyc_Parse_Declarator*_tmp57B=_cycalloc(
sizeof(*_tmp57B));_tmp57B->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp57B->tms=({
struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));
_tmp57D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp57E;_tmp57E.tag=3;_tmp57E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp57F=_cycalloc(sizeof(*
_tmp57F));_tmp57F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp580;_tmp580.tag=1;
_tmp580.f1=_tmp574;_tmp580.f2=_tmp575;_tmp580.f3=_tmp576;_tmp580.f4=_tmp577;
_tmp580.f5=_tmp578;_tmp580;});_tmp57F;}));_tmp57E;});_tmp57D;}));_tmp57C->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp57C;});_tmp57B;});_tmp57A;});_tmp579;});break;}case 141: _LL27C:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp581=_cycalloc(sizeof(*_tmp581));
_tmp581[0]=({struct Cyc_Declarator_tok_struct _tmp582;_tmp582.tag=Cyc_Declarator_tok;
_tmp582.f1=({struct Cyc_Parse_Declarator*_tmp583=_cycalloc(sizeof(*_tmp583));
_tmp583->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp583->tms=({struct Cyc_List_List*_tmp584=_cycalloc(
sizeof(*_tmp584));_tmp584->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp586;_tmp586.tag=3;_tmp586.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp588;_tmp588.tag=1;_tmp588.f1=0;_tmp588.f2=0;_tmp588.f3=0;_tmp588.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp588.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp588;});_tmp587;}));
_tmp586;});_tmp585;}));_tmp584->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp584;});_tmp583;});
_tmp582;});_tmp581;});break;case 142: _LL27D: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589[0]=({struct Cyc_Declarator_tok_struct
_tmp58A;_tmp58A.tag=Cyc_Declarator_tok;_tmp58A.f1=({struct Cyc_Parse_Declarator*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp58B->tms=({
struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));
_tmp58D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp58E;_tmp58E.tag=3;_tmp58E.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F[0]=({struct Cyc_Absyn_NoTypes_struct _tmp590;_tmp590.tag=0;_tmp590.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp590.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp590;});
_tmp58F;}));_tmp58E;});_tmp58D;}));_tmp58C->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp58C;});_tmp58B;});
_tmp58A;});_tmp589;});break;case 143: _LL27E: {struct Cyc_List_List*_tmp591=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({
struct Cyc_Declarator_tok_struct _tmp593;_tmp593.tag=Cyc_Declarator_tok;_tmp593.f1=({
struct Cyc_Parse_Declarator*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp594->tms=({
struct Cyc_List_List*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp597;_tmp597.tag=4;_tmp597.f1=
_tmp591;_tmp597.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp597.f3=0;
_tmp597;});_tmp596;}));_tmp595->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp595;});_tmp594;});
_tmp593;});_tmp592;});break;}case 144: _LL27F: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_Declarator_tok_struct
_tmp599;_tmp599.tag=Cyc_Declarator_tok;_tmp599.f1=({struct Cyc_Parse_Declarator*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp59A->tms=({
struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));
_tmp59C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp59D;_tmp59D.tag=5;_tmp59D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp59D.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59D;});_tmp59C;}));
_tmp59B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp59B;});_tmp59A;});_tmp599;});_tmp598;});break;case
145: _LL280: Cyc_Parse_err(({const char*_tmp59E="identifier has not been declared as a typedef";
_tag_arr(_tmp59E,sizeof(char),_get_zero_arr_size(_tmp59E,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({
struct Cyc_Declarator_tok_struct _tmp5A0;_tmp5A0.tag=Cyc_Declarator_tok;_tmp5A0.f1=({
struct Cyc_Parse_Declarator*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A1->tms=0;_tmp5A1;});
_tmp5A0;});_tmp59F;});break;case 146: _LL281: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 147: _LL282: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2[0]=({struct Cyc_TypeModifierList_tok_struct _tmp5A3;_tmp5A3.tag=Cyc_TypeModifierList_tok;
_tmp5A3.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5A3;});_tmp5A2;});
break;case 148: _LL283: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5A6;_tmp5A6.tag=5;_tmp5A6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5A6.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5A6;});_tmp5A5;}));_tmp5A4->tl=ans;_tmp5A4;});ans=({
struct Cyc_List_List*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->hd=(void*)((
void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));
_tmp5A8[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp5A9;_tmp5A9.tag=2;_tmp5A9.f1=({
struct Cyc_Absyn_PtrAtts _tmp5AA;_tmp5AA.rgn=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5AA.nullable=(*Cyc_yyget_YY1(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5AA.bounds=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f2;
_tmp5AA.zero_term=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp5AA;});_tmp5A9.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A9;});_tmp5A8;}));
_tmp5A7->tl=ans;_tmp5A7;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp5AC;_tmp5AC.tag=Cyc_TypeModifierList_tok;_tmp5AC.f1=ans;_tmp5AC;});_tmp5AB;});
break;}case 149: _LL284: yyval=(void*)({struct Cyc_YY1_struct*_tmp5AD=_cycalloc(
sizeof(*_tmp5AD));_tmp5AD[0]=({struct Cyc_YY1_struct _tmp5AE;_tmp5AE.tag=Cyc_YY1;
_tmp5AE.f1=({struct _tuple16*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->f1=Cyc_Absyn_true_conref;
_tmp5AF->f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5AF;});_tmp5AE;});_tmp5AD;});break;case 150: _LL285: yyval=(void*)({struct Cyc_YY1_struct*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0[0]=({struct Cyc_YY1_struct _tmp5B1;
_tmp5B1.tag=Cyc_YY1;_tmp5B1.f1=({struct _tuple16*_tmp5B2=_cycalloc(sizeof(*
_tmp5B2));_tmp5B2->f1=Cyc_Absyn_false_conref;_tmp5B2->f2=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B2;});_tmp5B1;});
_tmp5B0;});break;case 151: _LL286: yyval=(void*)({struct Cyc_YY1_struct*_tmp5B3=
_cycalloc(sizeof(*_tmp5B3));_tmp5B3[0]=({struct Cyc_YY1_struct _tmp5B4;_tmp5B4.tag=
Cyc_YY1;_tmp5B4.f1=({struct _tuple16*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->f1=
Cyc_Absyn_true_conref;_tmp5B5->f2=Cyc_Absyn_bounds_unknown_conref;_tmp5B5;});
_tmp5B4;});_tmp5B3;});break;case 152: _LL287: yyval=(void*)({struct Cyc_YY2_struct*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=({struct Cyc_YY2_struct _tmp5B7;
_tmp5B7.tag=Cyc_YY2;_tmp5B7.f1=Cyc_Absyn_bounds_one_conref;_tmp5B7;});_tmp5B6;});
break;case 153: _LL288: yyval=(void*)({struct Cyc_YY2_struct*_tmp5B8=_cycalloc(
sizeof(*_tmp5B8));_tmp5B8[0]=({struct Cyc_YY2_struct _tmp5B9;_tmp5B9.tag=Cyc_YY2;
_tmp5B9.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp5BA=
_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({struct Cyc_Absyn_Upper_b_struct _tmp5BB;
_tmp5BB.tag=0;_tmp5BB.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5BB;});_tmp5BA;}));_tmp5B9;});_tmp5B8;});break;case
154: _LL289: yyval=(void*)({struct Cyc_YY2_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));
_tmp5BC[0]=({struct Cyc_YY2_struct _tmp5BD;_tmp5BD.tag=Cyc_YY2;_tmp5BD.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));
_tmp5BE[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp5BF;_tmp5BF.tag=1;_tmp5BF.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp5BF;});_tmp5BE;}));_tmp5BD;});_tmp5BC;});break;case 155: _LL28A: yyval=(
void*)({struct Cyc_YY4_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0[0]=({
struct Cyc_YY4_struct _tmp5C1;_tmp5C1.tag=Cyc_YY4;_tmp5C1.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5C1;});_tmp5C0;});break;case 156: _LL28B: yyval=(void*)({
struct Cyc_YY4_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_YY4_struct
_tmp5C3;_tmp5C3.tag=Cyc_YY4;_tmp5C3.f1=Cyc_Absyn_true_conref;_tmp5C3;});_tmp5C2;});
break;case 157: _LL28C: yyval=(void*)({struct Cyc_YY4_struct*_tmp5C4=_cycalloc(
sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_YY4_struct _tmp5C5;_tmp5C5.tag=Cyc_YY4;
_tmp5C5.f1=Cyc_Absyn_false_conref;_tmp5C5;});_tmp5C4;});break;case 158: _LL28D:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));
_tmp5C6[0]=({struct Cyc_Type_tok_struct _tmp5C7;_tmp5C7.tag=Cyc_Type_tok;_tmp5C7.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));
_tmp5C8->v=(void*)((void*)3);_tmp5C8;}),0);_tmp5C7;});_tmp5C6;});break;case 159:
_LL28E: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 160: _LL28F: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=({
struct Cyc_Type_tok_struct _tmp5CA;_tmp5CA.tag=Cyc_Type_tok;_tmp5CA.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->v=(void*)((void*)
3);_tmp5CB;}),0);_tmp5CA;});_tmp5C9;});break;case 161: _LL290: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5CC=_cycalloc_atomic(sizeof(*_tmp5CC));_tmp5CC[
0]=({struct Cyc_TypeQual_tok_struct _tmp5CD;_tmp5CD.tag=Cyc_TypeQual_tok;_tmp5CD.f1=
Cyc_Absyn_empty_tqual();_tmp5CD;});_tmp5CC;});break;case 162: _LL291: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5CE=_cycalloc_atomic(sizeof(*_tmp5CE));_tmp5CE[
0]=({struct Cyc_TypeQual_tok_struct _tmp5CF;_tmp5CF.tag=Cyc_TypeQual_tok;_tmp5CF.f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5CF;});_tmp5CE;});
break;case 163: _LL292: yyval=(void*)({struct Cyc_YY3_struct*_tmp5D0=_cycalloc(
sizeof(*_tmp5D0));_tmp5D0[0]=({struct Cyc_YY3_struct _tmp5D1;_tmp5D1.tag=Cyc_YY3;
_tmp5D1.f1=({struct _tuple14*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5D2->f2=0;
_tmp5D2->f3=0;_tmp5D2->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D2->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D2;});_tmp5D1;});
_tmp5D0;});break;case 164: _LL293: yyval=(void*)({struct Cyc_YY3_struct*_tmp5D3=
_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_YY3_struct _tmp5D4;_tmp5D4.tag=
Cyc_YY3;_tmp5D4.f1=({struct _tuple14*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp5D5->f2=1;
_tmp5D5->f3=0;_tmp5D5->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D5->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D5;});_tmp5D4;});
_tmp5D3;});break;case 165: _LL294: {struct _tuple2 _tmp5D7;struct Cyc_Core_Opt*
_tmp5D8;struct Cyc_Absyn_Tqual _tmp5D9;void*_tmp5DA;struct _tuple2*_tmp5D6=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D7=*_tmp5D6;
_tmp5D8=_tmp5D7.f1;_tmp5D9=_tmp5D7.f2;_tmp5DA=_tmp5D7.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5DB=({struct Cyc_Absyn_VarargInfo*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->name=
_tmp5D8;_tmp5DF->tq=_tmp5D9;_tmp5DF->type=(void*)_tmp5DA;_tmp5DF->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5DF;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC[0]=({
struct Cyc_YY3_struct _tmp5DD;_tmp5DD.tag=Cyc_YY3;_tmp5DD.f1=({struct _tuple14*
_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->f1=0;_tmp5DE->f2=0;_tmp5DE->f3=
_tmp5DB;_tmp5DE->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5DE->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DE;});_tmp5DD;});
_tmp5DC;});break;}}case 166: _LL295: {struct _tuple2 _tmp5E1;struct Cyc_Core_Opt*
_tmp5E2;struct Cyc_Absyn_Tqual _tmp5E3;void*_tmp5E4;struct _tuple2*_tmp5E0=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E1=*_tmp5E0;
_tmp5E2=_tmp5E1.f1;_tmp5E3=_tmp5E1.f2;_tmp5E4=_tmp5E1.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5E5=({struct Cyc_Absyn_VarargInfo*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->name=
_tmp5E2;_tmp5E9->tq=_tmp5E3;_tmp5E9->type=(void*)_tmp5E4;_tmp5E9->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5E9;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6[0]=({
struct Cyc_YY3_struct _tmp5E7;_tmp5E7.tag=Cyc_YY3;_tmp5E7.f1=({struct _tuple14*
_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5E8->f2=0;_tmp5E8->f3=
_tmp5E5;_tmp5E8->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5E8->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E8;});_tmp5E7;});
_tmp5E6;});break;}}case 167: _LL296: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_Type_tok_struct
_tmp5EB;_tmp5EB.tag=Cyc_Type_tok;_tmp5EB.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5ED;_tmp5ED.tag=1;_tmp5ED.f1=0;_tmp5ED;});_tmp5EC;}));_tmp5EB;});_tmp5EA;});
break;case 168: _LL297: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5EE=_cycalloc(
sizeof(*_tmp5EE));_tmp5EE[0]=({struct Cyc_Type_tok_struct _tmp5EF;_tmp5EF.tag=Cyc_Type_tok;
_tmp5EF.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5F1;_tmp5F1.tag=0;_tmp5F1.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F1;});_tmp5F0;}));
_tmp5EF;});_tmp5EE;});break;case 169: _LL298: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5F3;_tmp5F3.tag=Cyc_TypeOpt_tok;_tmp5F3.f1=0;_tmp5F3;});_tmp5F2;});break;
case 170: _LL299: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5F4=_cycalloc(
sizeof(*_tmp5F4));_tmp5F4[0]=({struct Cyc_TypeOpt_tok_struct _tmp5F5;_tmp5F5.tag=
Cyc_TypeOpt_tok;_tmp5F5.f1=({struct Cyc_Core_Opt*_tmp5F6=_cycalloc(sizeof(*
_tmp5F6));_tmp5F6->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5F7=
_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5F8;
_tmp5F8.tag=20;_tmp5F8.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F8;});_tmp5F7;}));
_tmp5F6;});_tmp5F5;});_tmp5F4;});break;case 171: _LL29A: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5FA;_tmp5FA.tag=Cyc_Rgnorder_tok;_tmp5FA.f1=0;_tmp5FA;});_tmp5F9;});break;
case 172: _LL29B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 173: _LL29C: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5FB=
_cycalloc(sizeof(*_tmp5FB));_tmp5FB[0]=({struct Cyc_Rgnorder_tok_struct _tmp5FC;
_tmp5FC.tag=Cyc_Rgnorder_tok;_tmp5FC.f1=({struct Cyc_List_List*_tmp5FD=_cycalloc(
sizeof(*_tmp5FD));_tmp5FD->hd=({struct _tuple4*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));
_tmp5FE->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp601=_cycalloc(sizeof(*
_tmp601));_tmp601[0]=({struct Cyc_Absyn_JoinEff_struct _tmp602;_tmp602.tag=20;
_tmp602.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp602;});_tmp601;});_tmp5FE->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp600;_tmp600.tag=0;_tmp600.f1=(void*)((void*)3);_tmp600;});_tmp5FF;}));
_tmp5FE;});_tmp5FD->tl=0;_tmp5FD;});_tmp5FC;});_tmp5FB;});break;case 174: _LL29D:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp603=_cycalloc(sizeof(*_tmp603));
_tmp603[0]=({struct Cyc_Rgnorder_tok_struct _tmp604;_tmp604.tag=Cyc_Rgnorder_tok;
_tmp604.f1=({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->hd=({
struct _tuple4*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp60A;_tmp60A.tag=20;_tmp60A.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp60A;});_tmp609;});
_tmp606->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp608;_tmp608.tag=0;_tmp608.f1=(void*)((void*)3);_tmp608;});_tmp607;}));
_tmp606;});_tmp605->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp605;});_tmp604;});
_tmp603;});break;case 175: _LL29E: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp60B=
_cycalloc_atomic(sizeof(*_tmp60B));_tmp60B[0]=({struct Cyc_Bool_tok_struct _tmp60C;
_tmp60C.tag=Cyc_Bool_tok;_tmp60C.f1=0;_tmp60C;});_tmp60B;});break;case 176: _LL29F:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp60D="inject";
_tag_arr(_tmp60D,sizeof(char),_get_zero_arr_size(_tmp60D,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp60E="missing type in function declaration";_tag_arr(_tmp60E,
sizeof(char),_get_zero_arr_size(_tmp60E,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp60F=_cycalloc_atomic(sizeof(*_tmp60F));_tmp60F[0]=({
struct Cyc_Bool_tok_struct _tmp610;_tmp610.tag=Cyc_Bool_tok;_tmp610.f1=1;_tmp610;});
_tmp60F;});break;case 177: _LL2A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 178: _LL2A1: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_TypeList_tok_struct
_tmp612;_tmp612.tag=Cyc_TypeList_tok;_tmp612.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp612;});_tmp611;});
break;case 179: _LL2A2: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp613=
_cycalloc(sizeof(*_tmp613));_tmp613[0]=({struct Cyc_TypeList_tok_struct _tmp614;
_tmp614.tag=Cyc_TypeList_tok;_tmp614.f1=0;_tmp614;});_tmp613;});break;case 180:
_LL2A3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 181: _LL2A4: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp615=_cycalloc(
sizeof(*_tmp615));_tmp615[0]=({struct Cyc_TypeList_tok_struct _tmp616;_tmp616.tag=
Cyc_TypeList_tok;_tmp616.f1=({struct Cyc_List_List*_tmp617=_cycalloc(sizeof(*
_tmp617));_tmp617->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp618=
_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp619;
_tmp619.tag=21;_tmp619.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp619;});_tmp618;}));
_tmp617->tl=0;_tmp617;});_tmp616;});_tmp615;});break;case 182: _LL2A5: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp61A=_cycalloc(sizeof(*
_tmp61A));_tmp61A[0]=({struct Cyc_TypeList_tok_struct _tmp61B;_tmp61B.tag=Cyc_TypeList_tok;
_tmp61B.f1=({struct Cyc_List_List*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp61C->tl=0;_tmp61C;});_tmp61B;});_tmp61A;});break;case 183: _LL2A6: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp61D=_cycalloc(sizeof(*
_tmp61D));_tmp61D[0]=({struct Cyc_TypeList_tok_struct _tmp61E;_tmp61E.tag=Cyc_TypeList_tok;
_tmp61E.f1=({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp620=_cycalloc(sizeof(*
_tmp620));_tmp620[0]=({struct Cyc_Absyn_AccessEff_struct _tmp621;_tmp621.tag=19;
_tmp621.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp621;});_tmp620;}));_tmp61F->tl=0;_tmp61F;});_tmp61E;});
_tmp61D;});break;case 184: _LL2A7: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp622=_cycalloc(sizeof(*_tmp622));
_tmp622[0]=({struct Cyc_TypeList_tok_struct _tmp623;_tmp623.tag=Cyc_TypeList_tok;
_tmp623.f1=({struct Cyc_List_List*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp625=_cycalloc(sizeof(*
_tmp625));_tmp625[0]=({struct Cyc_Absyn_AccessEff_struct _tmp626;_tmp626.tag=19;
_tmp626.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp626;});_tmp625;}));_tmp624->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp624;});_tmp623;});
_tmp622;});break;case 185: _LL2A8: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp628;_tmp628.tag=Cyc_ParamDeclList_tok;_tmp628.f1=({struct Cyc_List_List*
_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp629->tl=0;_tmp629;});
_tmp628;});_tmp627;});break;case 186: _LL2A9: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp62B;_tmp62B.tag=Cyc_ParamDeclList_tok;_tmp62B.f1=({struct Cyc_List_List*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62C->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp62C;});_tmp62B;});
_tmp62A;});break;case 187: _LL2AA: {struct _tuple15 _tmp62E;struct Cyc_Absyn_Tqual
_tmp62F;struct Cyc_List_List*_tmp630;struct Cyc_List_List*_tmp631;struct _tuple15*
_tmp62D=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp62E=*_tmp62D;_tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;
_tmp631=_tmp62E.f3;{struct Cyc_Parse_Declarator _tmp633;struct _tuple1*_tmp634;
struct Cyc_List_List*_tmp635;struct Cyc_Parse_Declarator*_tmp632=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp633=*_tmp632;
_tmp634=_tmp633.id;_tmp635=_tmp633.tms;{void*_tmp636=Cyc_Parse_speclist2typ(
_tmp630,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp638;void*_tmp639;struct
Cyc_List_List*_tmp63A;struct Cyc_List_List*_tmp63B;struct _tuple6 _tmp637=Cyc_Parse_apply_tms(
_tmp62F,_tmp636,_tmp631,_tmp635);_tmp638=_tmp637.f1;_tmp639=_tmp637.f2;_tmp63A=
_tmp637.f3;_tmp63B=_tmp637.f4;if(_tmp63A != 0)Cyc_Parse_err(({const char*_tmp63C="parameter with bad type params";
_tag_arr(_tmp63C,sizeof(char),_get_zero_arr_size(_tmp63C,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp634))Cyc_Parse_err(({const char*_tmp63D="parameter cannot be qualified with a namespace";
_tag_arr(_tmp63D,sizeof(char),_get_zero_arr_size(_tmp63D,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp63E=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp644=_cycalloc(sizeof(*
_tmp644));_tmp644->v=(*_tmp634).f2;_tmp644;});if(_tmp63B != 0)({void*_tmp63F[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp640="extra attributes on parameter, ignoring";
_tag_arr(_tmp640,sizeof(char),_get_zero_arr_size(_tmp640,40));}),_tag_arr(
_tmp63F,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641[0]=({struct Cyc_ParamDecl_tok_struct
_tmp642;_tmp642.tag=Cyc_ParamDecl_tok;_tmp642.f1=({struct _tuple2*_tmp643=
_cycalloc(sizeof(*_tmp643));_tmp643->f1=_tmp63E;_tmp643->f2=_tmp638;_tmp643->f3=
_tmp639;_tmp643;});_tmp642;});_tmp641;});break;}}}}case 188: _LL2AB: {struct
_tuple15 _tmp646;struct Cyc_Absyn_Tqual _tmp647;struct Cyc_List_List*_tmp648;struct
Cyc_List_List*_tmp649;struct _tuple15*_tmp645=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp646=*_tmp645;_tmp647=
_tmp646.f1;_tmp648=_tmp646.f2;_tmp649=_tmp646.f3;{void*_tmp64A=Cyc_Parse_speclist2typ(
_tmp648,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp649 != 0)({void*_tmp64B[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp64C="bad attributes on parameter, ignoring";
_tag_arr(_tmp64C,sizeof(char),_get_zero_arr_size(_tmp64C,38));}),_tag_arr(
_tmp64B,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_ParamDecl_tok_struct
_tmp64E;_tmp64E.tag=Cyc_ParamDecl_tok;_tmp64E.f1=({struct _tuple2*_tmp64F=
_cycalloc(sizeof(*_tmp64F));_tmp64F->f1=0;_tmp64F->f2=_tmp647;_tmp64F->f3=
_tmp64A;_tmp64F;});_tmp64E;});_tmp64D;});break;}}case 189: _LL2AC: {struct _tuple15
_tmp651;struct Cyc_Absyn_Tqual _tmp652;struct Cyc_List_List*_tmp653;struct Cyc_List_List*
_tmp654;struct _tuple15*_tmp650=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp651=*_tmp650;_tmp652=
_tmp651.f1;_tmp653=_tmp651.f2;_tmp654=_tmp651.f3;{void*_tmp655=Cyc_Parse_speclist2typ(
_tmp653,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp656=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual
_tmp658;void*_tmp659;struct Cyc_List_List*_tmp65A;struct Cyc_List_List*_tmp65B;
struct _tuple6 _tmp657=Cyc_Parse_apply_tms(_tmp652,_tmp655,_tmp654,_tmp656);
_tmp658=_tmp657.f1;_tmp659=_tmp657.f2;_tmp65A=_tmp657.f3;_tmp65B=_tmp657.f4;if(
_tmp65A != 0)({void*_tmp65C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp65D="bad type parameters on formal argument, ignoring";_tag_arr(_tmp65D,
sizeof(char),_get_zero_arr_size(_tmp65D,49));}),_tag_arr(_tmp65C,sizeof(void*),0));});
if(_tmp65B != 0)({void*_tmp65E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp65F="bad attributes on parameter, ignoring";_tag_arr(_tmp65F,sizeof(char),
_get_zero_arr_size(_tmp65F,38));}),_tag_arr(_tmp65E,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp660=_cycalloc(sizeof(*_tmp660));
_tmp660[0]=({struct Cyc_ParamDecl_tok_struct _tmp661;_tmp661.tag=Cyc_ParamDecl_tok;
_tmp661.f1=({struct _tuple2*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=0;
_tmp662->f2=_tmp658;_tmp662->f3=_tmp659;_tmp662;});_tmp661;});_tmp660;});break;}}
case 190: _LL2AD: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp663=_cycalloc(
sizeof(*_tmp663));_tmp663[0]=({struct Cyc_IdList_tok_struct _tmp664;_tmp664.tag=
Cyc_IdList_tok;_tmp664.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp664;});_tmp663;});break;case 191: _LL2AE: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_IdList_tok_struct
_tmp666;_tmp666.tag=Cyc_IdList_tok;_tmp666.f1=({struct Cyc_List_List*_tmp667=
_cycalloc(sizeof(*_tmp667));_tmp667->hd=({struct _tagged_arr*_tmp668=_cycalloc(
sizeof(*_tmp668));_tmp668[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp668;});_tmp667->tl=0;
_tmp667;});_tmp666;});_tmp665;});break;case 192: _LL2AF: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_IdList_tok_struct
_tmp66A;_tmp66A.tag=Cyc_IdList_tok;_tmp66A.f1=({struct Cyc_List_List*_tmp66B=
_cycalloc(sizeof(*_tmp66B));_tmp66B->hd=({struct _tagged_arr*_tmp66C=_cycalloc(
sizeof(*_tmp66C));_tmp66C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66C;});_tmp66B->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66B;});_tmp66A;});
_tmp669;});break;case 193: _LL2B0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 194: _LL2B1: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 195: _LL2B2: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Exp_tok_struct _tmp66E;
_tmp66E.tag=Cyc_Exp_tok;_tmp66E.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp670;_tmp670.tag=36;_tmp670.f1=0;_tmp670.f2=0;_tmp670;});_tmp66F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66E;});
_tmp66D;});break;case 196: _LL2B3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671[0]=({struct Cyc_Exp_tok_struct _tmp672;_tmp672.tag=
Cyc_Exp_tok;_tmp672.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp674;_tmp674.tag=36;_tmp674.f1=0;_tmp674.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp674;});_tmp673;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp672;});_tmp671;});break;case 197: _LL2B4: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({
struct Cyc_Exp_tok_struct _tmp676;_tmp676.tag=Cyc_Exp_tok;_tmp676.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp677=_cycalloc(sizeof(*_tmp677));
_tmp677[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp678;_tmp678.tag=36;
_tmp678.f1=0;_tmp678.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp678;});_tmp677;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp676;});
_tmp675;});break;case 198: _LL2B5: {struct Cyc_Absyn_Vardecl*_tmp679=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->f1=(void*)0;_tmp67E->f2=({
struct _tagged_arr*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp67F;});_tmp67E;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp679->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));
_tmp67A[0]=({struct Cyc_Exp_tok_struct _tmp67B;_tmp67B.tag=Cyc_Exp_tok;_tmp67B.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp67C=
_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp67D;_tmp67D.tag=29;_tmp67D.f1=_tmp679;_tmp67D.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67D.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67D.f4=0;
_tmp67D;});_tmp67C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp67B;});
_tmp67A;});break;}case 199: _LL2B6: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_InitializerList_tok_struct
_tmp681;_tmp681.tag=Cyc_InitializerList_tok;_tmp681.f1=({struct Cyc_List_List*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->hd=({struct _tuple18*_tmp683=
_cycalloc(sizeof(*_tmp683));_tmp683->f1=0;_tmp683->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp683;});_tmp682->tl=0;
_tmp682;});_tmp681;});_tmp680;});break;case 200: _LL2B7: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_InitializerList_tok_struct
_tmp685;_tmp685.tag=Cyc_InitializerList_tok;_tmp685.f1=({struct Cyc_List_List*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->hd=({struct _tuple18*_tmp687=
_cycalloc(sizeof(*_tmp687));_tmp687->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp687->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp687;});_tmp686->tl=
0;_tmp686;});_tmp685;});_tmp684;});break;case 201: _LL2B8: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_InitializerList_tok_struct
_tmp689;_tmp689.tag=Cyc_InitializerList_tok;_tmp689.f1=({struct Cyc_List_List*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->hd=({struct _tuple18*_tmp68B=
_cycalloc(sizeof(*_tmp68B));_tmp68B->f1=0;_tmp68B->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68B;});_tmp68A->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp68A;});_tmp689;});
_tmp688;});break;case 202: _LL2B9: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_InitializerList_tok_struct
_tmp68D;_tmp68D.tag=Cyc_InitializerList_tok;_tmp68D.f1=({struct Cyc_List_List*
_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->hd=({struct _tuple18*_tmp68F=
_cycalloc(sizeof(*_tmp68F));_tmp68F->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68F->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68F;});_tmp68E->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp68E;});_tmp68D;});_tmp68C;});break;case 203: _LL2BA: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp690=_cycalloc(sizeof(*_tmp690));
_tmp690[0]=({struct Cyc_DesignatorList_tok_struct _tmp691;_tmp691.tag=Cyc_DesignatorList_tok;
_tmp691.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp691;});_tmp690;});
break;case 204: _LL2BB: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp692=
_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_DesignatorList_tok_struct
_tmp693;_tmp693.tag=Cyc_DesignatorList_tok;_tmp693.f1=({struct Cyc_List_List*
_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp694->tl=0;_tmp694;});
_tmp693;});_tmp692;});break;case 205: _LL2BC: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_DesignatorList_tok_struct
_tmp696;_tmp696.tag=Cyc_DesignatorList_tok;_tmp696.f1=({struct Cyc_List_List*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp697->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp697;});_tmp696;});
_tmp695;});break;case 206: _LL2BD: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Designator_tok_struct
_tmp699;_tmp699.tag=Cyc_Designator_tok;_tmp699.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp69B;_tmp69B.tag=0;_tmp69B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69B;});_tmp69A;}));
_tmp699;});_tmp698;});break;case 207: _LL2BE: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Designator_tok_struct
_tmp69D;_tmp69D.tag=Cyc_Designator_tok;_tmp69D.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_Absyn_FieldName_struct
_tmp69F;_tmp69F.tag=1;_tmp69F.f1=({struct _tagged_arr*_tmp6A0=_cycalloc(sizeof(*
_tmp6A0));_tmp6A0[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6A0;});_tmp69F;});_tmp69E;}));_tmp69D;});_tmp69C;});
break;case 208: _LL2BF: {struct _tuple15 _tmp6A2;struct Cyc_Absyn_Tqual _tmp6A3;struct
Cyc_List_List*_tmp6A4;struct Cyc_List_List*_tmp6A5;struct _tuple15*_tmp6A1=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A2=*_tmp6A1;
_tmp6A3=_tmp6A2.f1;_tmp6A4=_tmp6A2.f2;_tmp6A5=_tmp6A2.f3;{void*_tmp6A6=Cyc_Parse_speclist2typ(
_tmp6A4,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6A5 != 0)({void*_tmp6A7[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6A8="ignoring attributes in type";
_tag_arr(_tmp6A8,sizeof(char),_get_zero_arr_size(_tmp6A8,28));}),_tag_arr(
_tmp6A7,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6AA;_tmp6AA.tag=Cyc_ParamDecl_tok;_tmp6AA.f1=({struct _tuple2*_tmp6AB=
_cycalloc(sizeof(*_tmp6AB));_tmp6AB->f1=0;_tmp6AB->f2=_tmp6A3;_tmp6AB->f3=
_tmp6A6;_tmp6AB;});_tmp6AA;});_tmp6A9;});break;}}case 209: _LL2C0: {struct _tuple15
_tmp6AD;struct Cyc_Absyn_Tqual _tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_List_List*
_tmp6B0;struct _tuple15*_tmp6AC=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD=*_tmp6AC;_tmp6AE=
_tmp6AD.f1;_tmp6AF=_tmp6AD.f2;_tmp6B0=_tmp6AD.f3;{void*_tmp6B1=Cyc_Parse_speclist2typ(
_tmp6AF,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp6B2=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp6B3=Cyc_Parse_apply_tms(_tmp6AE,_tmp6B1,_tmp6B0,_tmp6B2);if(_tmp6B3.f3 != 0)({
void*_tmp6B4[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6B5="bad type params, ignoring";_tag_arr(_tmp6B5,sizeof(char),
_get_zero_arr_size(_tmp6B5,26));}),_tag_arr(_tmp6B4,sizeof(void*),0));});if(
_tmp6B3.f4 != 0)({void*_tmp6B6[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6B7="bad specifiers, ignoring";_tag_arr(_tmp6B7,sizeof(char),
_get_zero_arr_size(_tmp6B7,25));}),_tag_arr(_tmp6B6,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));
_tmp6B8[0]=({struct Cyc_ParamDecl_tok_struct _tmp6B9;_tmp6B9.tag=Cyc_ParamDecl_tok;
_tmp6B9.f1=({struct _tuple2*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->f1=0;
_tmp6BA->f2=_tmp6B3.f1;_tmp6BA->f3=_tmp6B3.f2;_tmp6BA;});_tmp6B9;});_tmp6B8;});
break;}}case 210: _LL2C1: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6BB=
_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Type_tok_struct _tmp6BC;
_tmp6BC.tag=Cyc_Type_tok;_tmp6BC.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6BC;});_tmp6BB;});
break;case 211: _LL2C2: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6BD=_cycalloc(
sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Type_tok_struct _tmp6BE;_tmp6BE.tag=Cyc_Type_tok;
_tmp6BE.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6BF=_cycalloc(
sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6C0;_tmp6C0.tag=
20;_tmp6C0.f1=0;_tmp6C0;});_tmp6BF;}));_tmp6BE;});_tmp6BD;});break;case 212:
_LL2C3: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6C1=_cycalloc(sizeof(*
_tmp6C1));_tmp6C1[0]=({struct Cyc_Type_tok_struct _tmp6C2;_tmp6C2.tag=Cyc_Type_tok;
_tmp6C2.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6C3=_cycalloc(
sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6C4;_tmp6C4.tag=
20;_tmp6C4.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6C4;});_tmp6C3;}));_tmp6C2;});_tmp6C1;});break;case 213:
_LL2C4: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6C5=_cycalloc(sizeof(*
_tmp6C5));_tmp6C5[0]=({struct Cyc_Type_tok_struct _tmp6C6;_tmp6C6.tag=Cyc_Type_tok;
_tmp6C6.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6C7=_cycalloc(
sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6C8;_tmp6C8.tag=
21;_tmp6C8.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6C8;});_tmp6C7;}));_tmp6C6;});_tmp6C5;});break;case
214: _LL2C5: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6C9=_cycalloc(sizeof(*
_tmp6C9));_tmp6C9[0]=({struct Cyc_Type_tok_struct _tmp6CA;_tmp6CA.tag=Cyc_Type_tok;
_tmp6CA.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6CB=_cycalloc(
sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6CC;_tmp6CC.tag=
20;_tmp6CC.f1=({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp6CD->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6CD;});_tmp6CC;});_tmp6CB;}));_tmp6CA;});_tmp6C9;});
break;case 215: _LL2C6: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_TypeList_tok_struct _tmp6CF;
_tmp6CF.tag=Cyc_TypeList_tok;_tmp6CF.f1=({struct Cyc_List_List*_tmp6D0=_cycalloc(
sizeof(*_tmp6D0));_tmp6D0->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D0->tl=0;_tmp6D0;});
_tmp6CF;});_tmp6CE;});break;case 216: _LL2C7: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_TypeList_tok_struct
_tmp6D2;_tmp6D2.tag=Cyc_TypeList_tok;_tmp6D2.f1=({struct Cyc_List_List*_tmp6D3=
_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D3->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D3;});_tmp6D2;});
_tmp6D1;});break;case 217: _LL2C8: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6D5;_tmp6D5.tag=Cyc_AbstractDeclarator_tok;_tmp6D5.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D6;});_tmp6D5;});
_tmp6D4;});break;case 218: _LL2C9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 219: _LL2CA: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6D8;_tmp6D8.tag=Cyc_AbstractDeclarator_tok;_tmp6D8.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6D9;});
_tmp6D8;});_tmp6D7;});break;case 220: _LL2CB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 221: _LL2CC:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6DA=_cycalloc(sizeof(*
_tmp6DA));_tmp6DA[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6DB;_tmp6DB.tag=
Cyc_AbstractDeclarator_tok;_tmp6DB.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->tms=({struct Cyc_List_List*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6DF;_tmp6DF.tag=0;_tmp6DF.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DF;});_tmp6DE;}));
_tmp6DD->tl=0;_tmp6DD;});_tmp6DC;});_tmp6DB;});_tmp6DA;});break;case 222: _LL2CD:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6E0=_cycalloc(sizeof(*
_tmp6E0));_tmp6E0[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6E1;_tmp6E1.tag=
Cyc_AbstractDeclarator_tok;_tmp6E1.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->tms=({struct Cyc_List_List*_tmp6E3=
_cycalloc(sizeof(*_tmp6E3));_tmp6E3->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6E5;_tmp6E5.tag=0;_tmp6E5.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E5;});_tmp6E4;}));
_tmp6E3->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6E3;});_tmp6E2;});
_tmp6E1;});_tmp6E0;});break;case 223: _LL2CE: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6E7;_tmp6E7.tag=Cyc_AbstractDeclarator_tok;_tmp6E7.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->tms=({struct Cyc_List_List*_tmp6E9=
_cycalloc(sizeof(*_tmp6E9));_tmp6E9->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6EB;_tmp6EB.tag=1;_tmp6EB.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6EB.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EB;});_tmp6EA;}));
_tmp6E9->tl=0;_tmp6E9;});_tmp6E8;});_tmp6E7;});_tmp6E6;});break;case 224: _LL2CF:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6EC=_cycalloc(sizeof(*
_tmp6EC));_tmp6EC[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6ED;_tmp6ED.tag=
Cyc_AbstractDeclarator_tok;_tmp6ED.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE->tms=({struct Cyc_List_List*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6F1;_tmp6F1.tag=1;_tmp6F1.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F1.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F1;});_tmp6F0;}));
_tmp6EF->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6EF;});_tmp6EE;});
_tmp6ED;});_tmp6EC;});break;case 225: _LL2D0: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6F3;_tmp6F3.tag=Cyc_AbstractDeclarator_tok;_tmp6F3.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->tms=({struct Cyc_List_List*_tmp6F5=
_cycalloc(sizeof(*_tmp6F5));_tmp6F5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F7;_tmp6F7.tag=3;_tmp6F7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F9;_tmp6F9.tag=1;_tmp6F9.f1=0;_tmp6F9.f2=0;_tmp6F9.f3=0;_tmp6F9.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F9.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F9;});_tmp6F8;}));
_tmp6F7;});_tmp6F6;}));_tmp6F5->tl=0;_tmp6F5;});_tmp6F4;});_tmp6F3;});_tmp6F2;});
break;case 226: _LL2D1: {struct _tuple14 _tmp6FB;struct Cyc_List_List*_tmp6FC;int
_tmp6FD;struct Cyc_Absyn_VarargInfo*_tmp6FE;struct Cyc_Core_Opt*_tmp6FF;struct Cyc_List_List*
_tmp700;struct _tuple14*_tmp6FA=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6FB=*_tmp6FA;_tmp6FC=_tmp6FB.f1;_tmp6FD=_tmp6FB.f2;
_tmp6FE=_tmp6FB.f3;_tmp6FF=_tmp6FB.f4;_tmp700=_tmp6FB.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp702;_tmp702.tag=Cyc_AbstractDeclarator_tok;
_tmp702.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp703=_cycalloc(sizeof(*
_tmp703));_tmp703->tms=({struct Cyc_List_List*_tmp704=_cycalloc(sizeof(*_tmp704));
_tmp704->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp706;_tmp706.tag=3;_tmp706.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp708;_tmp708.tag=1;_tmp708.f1=_tmp6FC;_tmp708.f2=_tmp6FD;_tmp708.f3=_tmp6FE;
_tmp708.f4=_tmp6FF;_tmp708.f5=_tmp700;_tmp708;});_tmp707;}));_tmp706;});_tmp705;}));
_tmp704->tl=0;_tmp704;});_tmp703;});_tmp702;});_tmp701;});break;}case 227: _LL2D2:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp709=_cycalloc(sizeof(*
_tmp709));_tmp709[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp70A;_tmp70A.tag=
Cyc_AbstractDeclarator_tok;_tmp70A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B->tms=({struct Cyc_List_List*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp70E;_tmp70E.tag=3;_tmp70E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp710;_tmp710.tag=1;_tmp710.f1=0;_tmp710.f2=0;_tmp710.f3=0;_tmp710.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp710.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp710;});_tmp70F;}));
_tmp70E;});_tmp70D;}));_tmp70C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp70C;});_tmp70B;});
_tmp70A;});_tmp709;});break;case 228: _LL2D3: {struct _tuple14 _tmp712;struct Cyc_List_List*
_tmp713;int _tmp714;struct Cyc_Absyn_VarargInfo*_tmp715;struct Cyc_Core_Opt*_tmp716;
struct Cyc_List_List*_tmp717;struct _tuple14*_tmp711=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp712=*_tmp711;_tmp713=
_tmp712.f1;_tmp714=_tmp712.f2;_tmp715=_tmp712.f3;_tmp716=_tmp712.f4;_tmp717=
_tmp712.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp719;_tmp719.tag=Cyc_AbstractDeclarator_tok;_tmp719.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->tms=({struct Cyc_List_List*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp71D;_tmp71D.tag=3;_tmp71D.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp71F;_tmp71F.tag=1;_tmp71F.f1=_tmp713;_tmp71F.f2=_tmp714;_tmp71F.f3=_tmp715;
_tmp71F.f4=_tmp716;_tmp71F.f5=_tmp717;_tmp71F;});_tmp71E;}));_tmp71D;});_tmp71C;}));
_tmp71B->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp71B;});_tmp71A;});
_tmp719;});_tmp718;});break;}case 229: _LL2D4: {struct Cyc_List_List*_tmp720=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp722;_tmp722.tag=Cyc_AbstractDeclarator_tok;
_tmp722.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp723=_cycalloc(sizeof(*
_tmp723));_tmp723->tms=({struct Cyc_List_List*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp725=
_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp726;_tmp726.tag=4;_tmp726.f1=_tmp720;_tmp726.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp726.f3=0;
_tmp726;});_tmp725;}));_tmp724->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp724;});_tmp723;});
_tmp722;});_tmp721;});break;}case 230: _LL2D5: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp728;_tmp728.tag=Cyc_AbstractDeclarator_tok;_tmp728.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729->tms=({struct Cyc_List_List*_tmp72A=
_cycalloc(sizeof(*_tmp72A));_tmp72A->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp72C;_tmp72C.tag=5;_tmp72C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp72C.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp72C;});_tmp72B;}));
_tmp72A->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp72A;});_tmp729;});
_tmp728;});_tmp727;});break;case 231: _LL2D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 232: _LL2D7: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 233: _LL2D8:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 234:
_LL2D9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
235: _LL2DA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 236: _LL2DB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 237: _LL2DC: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp72D="`H";
_tag_arr(_tmp72D,sizeof(char),_get_zero_arr_size(_tmp72D,3));}))== 0)Cyc_Parse_err(({
const char*_tmp72E="bad occurrence of heap region `H";_tag_arr(_tmp72E,sizeof(
char),_get_zero_arr_size(_tmp72E,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->name=({
struct _tagged_arr*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp73C;});_tmp739->identity=
0;_tmp739->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp73A=_cycalloc(
sizeof(*_tmp73A));_tmp73A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73B;_tmp73B.tag=0;
_tmp73B.f1=(void*)((void*)3);_tmp73B;});_tmp73A;}));_tmp739;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({
struct Cyc_Absyn_VarType_struct _tmp738;_tmp738.tag=1;_tmp738.f1=tv;_tmp738;});
_tmp737;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72F=_cycalloc(sizeof(*
_tmp72F));_tmp72F[0]=({struct Cyc_Stmt_tok_struct _tmp730;_tmp730.tag=Cyc_Stmt_tok;
_tmp730.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp731=
_cycalloc(sizeof(*_tmp731));_tmp731[0]=({struct Cyc_Absyn_Region_s_struct _tmp732;
_tmp732.tag=18;_tmp732.f1=tv;_tmp732.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733->f1=(void*)0;_tmp733->f2=({struct
_tagged_arr*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp734;});_tmp733;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp735=_cycalloc(sizeof(*_tmp735));
_tmp735[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp736;_tmp736.tag=15;_tmp736.f1=(
void*)t;_tmp736;});_tmp735;}),0);_tmp732.f3=0;_tmp732.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp732;});_tmp731;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp730;});
_tmp72F;});break;}case 238: _LL2DD: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp73D="H";
_tag_arr(_tmp73D,sizeof(char),_get_zero_arr_size(_tmp73D,2));}))== 0)Cyc_Parse_err(({
const char*_tmp73E="bad occurrence of heap region `H";_tag_arr(_tmp73E,sizeof(
char),_get_zero_arr_size(_tmp73E,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->name=({
struct _tagged_arr*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp74F;_tmp74F.tag=0;_tmp74F.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp74D[1]={&
_tmp74F};Cyc_aprintf(({const char*_tmp74E="`%s";_tag_arr(_tmp74E,sizeof(char),
_get_zero_arr_size(_tmp74E,4));}),_tag_arr(_tmp74D,sizeof(void*),1));}});_tmp74C;});
_tmp749->identity=0;_tmp749->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp74B;_tmp74B.tag=0;_tmp74B.f1=(void*)((void*)3);_tmp74B;});_tmp74A;}));
_tmp749;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp747=_cycalloc(
sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_VarType_struct _tmp748;_tmp748.tag=
1;_tmp748.f1=tv;_tmp748;});_tmp747;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({struct Cyc_Stmt_tok_struct
_tmp740;_tmp740.tag=Cyc_Stmt_tok;_tmp740.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({
struct Cyc_Absyn_Region_s_struct _tmp742;_tmp742.tag=18;_tmp742.f1=tv;_tmp742.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp743=_cycalloc(sizeof(*_tmp743));
_tmp743->f1=(void*)0;_tmp743->f2=({struct _tagged_arr*_tmp744=_cycalloc(sizeof(*
_tmp744));_tmp744[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp744;});_tmp743;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp746;_tmp746.tag=15;_tmp746.f1=(void*)t;_tmp746;});_tmp745;}),0);_tmp742.f3=0;
_tmp742.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp742;});_tmp741;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp740;});
_tmp73F;});break;}case 239: _LL2DE: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp750="resetable";
_tag_arr(_tmp750,sizeof(char),_get_zero_arr_size(_tmp750,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp751="expecting [resetable]";_tag_arr(_tmp751,sizeof(char),
_get_zero_arr_size(_tmp751,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp752="`H";_tag_arr(_tmp752,sizeof(char),
_get_zero_arr_size(_tmp752,3));}))== 0)Cyc_Parse_err(({const char*_tmp753="bad occurrence of heap region `H";
_tag_arr(_tmp753,sizeof(char),_get_zero_arr_size(_tmp753,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->name=({
struct _tagged_arr*_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp761;});_tmp75E->identity=
0;_tmp75E->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp75F=_cycalloc(
sizeof(*_tmp75F));_tmp75F[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp760;_tmp760.tag=0;
_tmp760.f1=(void*)((void*)3);_tmp760;});_tmp75F;}));_tmp75E;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({
struct Cyc_Absyn_VarType_struct _tmp75D;_tmp75D.tag=1;_tmp75D.f1=tv;_tmp75D;});
_tmp75C;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp754=_cycalloc(sizeof(*
_tmp754));_tmp754[0]=({struct Cyc_Stmt_tok_struct _tmp755;_tmp755.tag=Cyc_Stmt_tok;
_tmp755.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp756=
_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Absyn_Region_s_struct _tmp757;
_tmp757.tag=18;_tmp757.f1=tv;_tmp757.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->f1=(void*)0;_tmp758->f2=({struct
_tagged_arr*_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp759;});_tmp758;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp75A=_cycalloc(sizeof(*_tmp75A));
_tmp75A[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp75B;_tmp75B.tag=15;_tmp75B.f1=(
void*)t;_tmp75B;});_tmp75A;}),0);_tmp757.f3=1;_tmp757.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp757;});_tmp756;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp755;});
_tmp754;});break;}case 240: _LL2DF: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp762="resetable";
_tag_arr(_tmp762,sizeof(char),_get_zero_arr_size(_tmp762,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp763="expecting `resetable'";_tag_arr(_tmp763,sizeof(char),
_get_zero_arr_size(_tmp763,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp764="H";_tag_arr(_tmp764,sizeof(char),
_get_zero_arr_size(_tmp764,2));}))== 0)Cyc_Parse_err(({const char*_tmp765="bad occurrence of heap region `H";
_tag_arr(_tmp765,sizeof(char),_get_zero_arr_size(_tmp765,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->name=({
struct _tagged_arr*_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp776;_tmp776.tag=0;_tmp776.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp774[1]={&
_tmp776};Cyc_aprintf(({const char*_tmp775="`%s";_tag_arr(_tmp775,sizeof(char),
_get_zero_arr_size(_tmp775,4));}),_tag_arr(_tmp774,sizeof(void*),1));}});_tmp773;});
_tmp770->identity=0;_tmp770->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp772;_tmp772.tag=0;_tmp772.f1=(void*)((void*)3);_tmp772;});_tmp771;}));
_tmp770;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp76E=_cycalloc(
sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_VarType_struct _tmp76F;_tmp76F.tag=
1;_tmp76F.f1=tv;_tmp76F;});_tmp76E;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Stmt_tok_struct
_tmp767;_tmp767.tag=Cyc_Stmt_tok;_tmp767.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({
struct Cyc_Absyn_Region_s_struct _tmp769;_tmp769.tag=18;_tmp769.f1=tv;_tmp769.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp76A=_cycalloc(sizeof(*_tmp76A));
_tmp76A->f1=(void*)0;_tmp76A->f2=({struct _tagged_arr*_tmp76B=_cycalloc(sizeof(*
_tmp76B));_tmp76B[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp76B;});_tmp76A;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp76D;_tmp76D.tag=15;_tmp76D.f1=(void*)t;_tmp76D;});_tmp76C;}),0);_tmp769.f3=1;
_tmp769.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp769;});_tmp768;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp767;});
_tmp766;});break;}case 241: _LL2E0: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777[0]=({struct Cyc_Stmt_tok_struct
_tmp778;_tmp778.tag=Cyc_Stmt_tok;_tmp778.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp77A;_tmp77A.tag=20;_tmp77A.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77A;});_tmp779;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp778;});_tmp777;});break;case 242: _LL2E1: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({
struct Cyc_Stmt_tok_struct _tmp77C;_tmp77C.tag=Cyc_Stmt_tok;_tmp77C.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp77E;_tmp77E.tag=13;_tmp77E.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77E;});_tmp77D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp77C;});_tmp77B;});break;case 243: _LL2E2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({
struct Cyc_Stmt_tok_struct _tmp780;_tmp780.tag=Cyc_Stmt_tok;_tmp780.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp781=_cycalloc(sizeof(*_tmp781));
_tmp781[0]=({struct Cyc_Absyn_Splice_s_struct _tmp782;_tmp782.tag=14;_tmp782.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp782;});_tmp781;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp780;});
_tmp77F;});break;case 244: _LL2E3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp783=
_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Stmt_tok_struct _tmp784;
_tmp784.tag=Cyc_Stmt_tok;_tmp784.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785[0]=({struct Cyc_Absyn_Label_s_struct
_tmp786;_tmp786.tag=15;_tmp786.f1=({struct _tagged_arr*_tmp787=_cycalloc(sizeof(*
_tmp787));_tmp787[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp787;});_tmp786.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp786;});_tmp785;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp784;});
_tmp783;});break;case 245: _LL2E4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp788=
_cycalloc(sizeof(*_tmp788));_tmp788[0]=({struct Cyc_Stmt_tok_struct _tmp789;
_tmp789.tag=Cyc_Stmt_tok;_tmp789.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp789;});
_tmp788;});break;case 246: _LL2E5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78A=
_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({struct Cyc_Stmt_tok_struct _tmp78B;
_tmp78B.tag=Cyc_Stmt_tok;_tmp78B.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78B;});
_tmp78A;});break;case 247: _LL2E6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78C=
_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Stmt_tok_struct _tmp78D;
_tmp78D.tag=Cyc_Stmt_tok;_tmp78D.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78D;});
_tmp78C;});break;case 248: _LL2E7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78E=
_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({struct Cyc_Stmt_tok_struct _tmp78F;
_tmp78F.tag=Cyc_Stmt_tok;_tmp78F.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78F;});
_tmp78E;});break;case 249: _LL2E8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 250: _LL2E9: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Stmt_tok_struct
_tmp791;_tmp791.tag=Cyc_Stmt_tok;_tmp791.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp791;});
_tmp790;});break;case 251: _LL2EA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Stmt_tok_struct _tmp793;
_tmp793.tag=Cyc_Stmt_tok;_tmp793.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp793;});_tmp792;});break;case 252: _LL2EB: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Stmt_tok_struct
_tmp795;_tmp795.tag=Cyc_Stmt_tok;_tmp795.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp795;});_tmp794;});
break;case 253: _LL2EC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 254: _LL2ED: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Stmt_tok_struct
_tmp797;_tmp797.tag=Cyc_Stmt_tok;_tmp797.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp797;});
_tmp796;});break;case 255: _LL2EE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp798=
_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Stmt_tok_struct _tmp799;
_tmp799.tag=Cyc_Stmt_tok;_tmp799.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp79B;_tmp79B.tag=1;_tmp79B.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79B;});_tmp79A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp799;});_tmp798;});break;
case 256: _LL2EF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp79C=_cycalloc(
sizeof(*_tmp79C));_tmp79C[0]=({struct Cyc_Stmt_tok_struct _tmp79D;_tmp79D.tag=Cyc_Stmt_tok;
_tmp79D.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp79F;_tmp79F.tag=1;_tmp79F.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79F;});_tmp79E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp79D;});_tmp79C;});
break;case 257: _LL2F0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A0=_cycalloc(
sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Stmt_tok_struct _tmp7A1;_tmp7A1.tag=Cyc_Stmt_tok;
_tmp7A1.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A1;});
_tmp7A0;});break;case 258: _LL2F1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Stmt_tok_struct _tmp7A3;
_tmp7A3.tag=Cyc_Stmt_tok;_tmp7A3.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A3;});
_tmp7A2;});break;case 259: _LL2F2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Stmt_tok_struct _tmp7A5;
_tmp7A5.tag=Cyc_Stmt_tok;_tmp7A5.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A5;});
_tmp7A4;});break;case 260: _LL2F3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A6=
_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Stmt_tok_struct _tmp7A7;
_tmp7A7.tag=Cyc_Stmt_tok;_tmp7A7.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A7;});
_tmp7A6;});break;case 261: _LL2F4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7A8=
_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Stmt_tok_struct _tmp7A9;
_tmp7A9.tag=Cyc_Stmt_tok;_tmp7A9.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A9;});
_tmp7A8;});break;case 262: _LL2F5: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp7AA="C";
_tag_arr(_tmp7AA,sizeof(char),_get_zero_arr_size(_tmp7AA,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp7AB="only switch \"C\" { ... } is allowed";_tag_arr(_tmp7AB,
sizeof(char),_get_zero_arr_size(_tmp7AB,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({
struct Cyc_Stmt_tok_struct _tmp7AD;_tmp7AD.tag=Cyc_Stmt_tok;_tmp7AD.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp7AF;_tmp7AF.tag=10;_tmp7AF.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp7AF.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7AF;});_tmp7AE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7AD;});
_tmp7AC;});break;case 263: _LL2F6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B0=
_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_Stmt_tok_struct _tmp7B1;
_tmp7B1.tag=Cyc_Stmt_tok;_tmp7B1.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B1;});
_tmp7B0;});break;case 264: _LL2F7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B2=
_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Stmt_tok_struct _tmp7B3;
_tmp7B3.tag=Cyc_Stmt_tok;_tmp7B3.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B3;});
_tmp7B2;});break;case 265: _LL2F8: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp7B5;_tmp7B5.tag=Cyc_SwitchClauseList_tok;_tmp7B5.f1=0;_tmp7B5;});_tmp7B4;});
break;case 266: _LL2F9: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp7B7;_tmp7B7.tag=Cyc_SwitchClauseList_tok;_tmp7B7.f1=({struct Cyc_List_List*
_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->hd=({struct Cyc_Absyn_Switch_clause*
_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp7B9->pat_vars=0;_tmp7B9->where_clause=0;
_tmp7B9->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7B9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B9;});
_tmp7B8->tl=0;_tmp7B8;});_tmp7B7;});_tmp7B6;});break;case 267: _LL2FA: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));
_tmp7BA[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7BB;_tmp7BB.tag=Cyc_SwitchClauseList_tok;
_tmp7BB.f1=({struct Cyc_List_List*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC->hd=({
struct Cyc_Absyn_Switch_clause*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp7BD->pat_vars=0;_tmp7BD->where_clause=0;_tmp7BD->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7BD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7BD;});
_tmp7BC->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7BC;});_tmp7BB;});_tmp7BA;});break;case 268: _LL2FB:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7BE=_cycalloc(sizeof(*
_tmp7BE));_tmp7BE[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7BF;_tmp7BF.tag=
Cyc_SwitchClauseList_tok;_tmp7BF.f1=({struct Cyc_List_List*_tmp7C0=_cycalloc(
sizeof(*_tmp7C0));_tmp7C0->hd=({struct Cyc_Absyn_Switch_clause*_tmp7C1=_cycalloc(
sizeof(*_tmp7C1));_tmp7C1->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7C1->pat_vars=0;
_tmp7C1->where_clause=0;_tmp7C1->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp7C1;});
_tmp7C0->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7C0;});_tmp7BF;});_tmp7BE;});break;case 269: _LL2FC:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7C2=_cycalloc(sizeof(*
_tmp7C2));_tmp7C2[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7C3;_tmp7C3.tag=
Cyc_SwitchClauseList_tok;_tmp7C3.f1=({struct Cyc_List_List*_tmp7C4=_cycalloc(
sizeof(*_tmp7C4));_tmp7C4->hd=({struct Cyc_Absyn_Switch_clause*_tmp7C5=_cycalloc(
sizeof(*_tmp7C5));_tmp7C5->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7C5->pat_vars=0;
_tmp7C5->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7C5->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7C5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7C5;});
_tmp7C4->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7C4;});_tmp7C3;});_tmp7C2;});break;case 270: _LL2FD:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7C6=_cycalloc(sizeof(*
_tmp7C6));_tmp7C6[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7C7;_tmp7C7.tag=
Cyc_SwitchClauseList_tok;_tmp7C7.f1=({struct Cyc_List_List*_tmp7C8=_cycalloc(
sizeof(*_tmp7C8));_tmp7C8->hd=({struct Cyc_Absyn_Switch_clause*_tmp7C9=_cycalloc(
sizeof(*_tmp7C9));_tmp7C9->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp7C9->pat_vars=0;
_tmp7C9->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7C9->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C9->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7C9;});
_tmp7C8->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7C8;});_tmp7C7;});_tmp7C6;});break;case 271: _LL2FE:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7CA=_cycalloc(sizeof(*
_tmp7CA));_tmp7CA[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7CB;_tmp7CB.tag=
Cyc_SwitchClauseList_tok;_tmp7CB.f1=0;_tmp7CB;});_tmp7CA;});break;case 272: _LL2FF:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7CC=_cycalloc(sizeof(*
_tmp7CC));_tmp7CC[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7CD;_tmp7CD.tag=
Cyc_SwitchCClauseList_tok;_tmp7CD.f1=0;_tmp7CD;});_tmp7CC;});break;case 273:
_LL300: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7CE=_cycalloc(
sizeof(*_tmp7CE));_tmp7CE[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7CF;
_tmp7CF.tag=Cyc_SwitchCClauseList_tok;_tmp7CF.f1=({struct Cyc_List_List*_tmp7D0=
_cycalloc(sizeof(*_tmp7D0));_tmp7D0->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7D1=
_cycalloc(sizeof(*_tmp7D1));_tmp7D1->cnst_exp=0;_tmp7D1->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp7D1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7D1;});
_tmp7D0->tl=0;_tmp7D0;});_tmp7CF;});_tmp7CE;});break;case 274: _LL301: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));
_tmp7D2[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7D3;_tmp7D3.tag=Cyc_SwitchCClauseList_tok;
_tmp7D3.f1=({struct Cyc_List_List*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp7D5->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp7D5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7D5;});
_tmp7D4->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7D4;});_tmp7D3;});_tmp7D2;});break;case 275: _LL302:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7D6=_cycalloc(sizeof(*
_tmp7D6));_tmp7D6[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7D7;_tmp7D7.tag=
Cyc_SwitchCClauseList_tok;_tmp7D7.f1=({struct Cyc_List_List*_tmp7D8=_cycalloc(
sizeof(*_tmp7D8));_tmp7D8->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7D9=_cycalloc(
sizeof(*_tmp7D9));_tmp7D9->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7D9->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp7D9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7D9;});
_tmp7D8->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7D8;});_tmp7D7;});_tmp7D6;});break;case 276: _LL303:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7DA=_cycalloc(sizeof(*
_tmp7DA));_tmp7DA[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7DB;_tmp7DB.tag=
Cyc_SwitchCClauseList_tok;_tmp7DB.f1=0;_tmp7DB;});_tmp7DA;});break;case 277:
_LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7DC=_cycalloc(sizeof(*
_tmp7DC));_tmp7DC[0]=({struct Cyc_Stmt_tok_struct _tmp7DD;_tmp7DD.tag=Cyc_Stmt_tok;
_tmp7DD.f1=Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DD;});
_tmp7DC;});break;case 278: _LL305: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7DE=
_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({struct Cyc_Stmt_tok_struct _tmp7DF;
_tmp7DF.tag=Cyc_Stmt_tok;_tmp7DF.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DF;});
_tmp7DE;});break;case 279: _LL306: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E0=
_cycalloc(sizeof(*_tmp7E0));_tmp7E0[0]=({struct Cyc_Stmt_tok_struct _tmp7E1;
_tmp7E1.tag=Cyc_Stmt_tok;_tmp7E1.f1=Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E1;});
_tmp7E0;});break;case 280: _LL307: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E2=
_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Stmt_tok_struct _tmp7E3;
_tmp7E3.tag=Cyc_Stmt_tok;_tmp7E3.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E3;});
_tmp7E2;});break;case 281: _LL308: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E4=
_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Stmt_tok_struct _tmp7E5;
_tmp7E5.tag=Cyc_Stmt_tok;_tmp7E5.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
_tmp7E4;});break;case 282: _LL309: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E6=
_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Stmt_tok_struct _tmp7E7;
_tmp7E7.tag=Cyc_Stmt_tok;_tmp7E7.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E7;});
_tmp7E6;});break;case 283: _LL30A: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E8=
_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Stmt_tok_struct _tmp7E9;
_tmp7E9.tag=Cyc_Stmt_tok;_tmp7E9.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E9;});
_tmp7E8;});break;case 284: _LL30B: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EA=
_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Stmt_tok_struct _tmp7EB;
_tmp7EB.tag=Cyc_Stmt_tok;_tmp7EB.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7EB;});
_tmp7EA;});break;case 285: _LL30C: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EC=
_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Stmt_tok_struct _tmp7ED;
_tmp7ED.tag=Cyc_Stmt_tok;_tmp7ED.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7ED;});
_tmp7EC;});break;case 286: _LL30D: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EE=
_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=({struct Cyc_Stmt_tok_struct _tmp7EF;
_tmp7EF.tag=Cyc_Stmt_tok;_tmp7EF.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EF;});
_tmp7EE;});break;case 287: _LL30E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F0=
_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({struct Cyc_Stmt_tok_struct _tmp7F1;
_tmp7F1.tag=Cyc_Stmt_tok;_tmp7F1.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F1;});
_tmp7F0;});break;case 288: _LL30F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F2=
_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Stmt_tok_struct _tmp7F3;
_tmp7F3.tag=Cyc_Stmt_tok;_tmp7F3.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F3;});
_tmp7F2;});break;case 289: _LL310: {struct Cyc_List_List*_tmp7F4=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp7F5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F6=
_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Stmt_tok_struct _tmp7F7;
_tmp7F7.tag=Cyc_Stmt_tok;_tmp7F7.f1=Cyc_Parse_flatten_declarations(_tmp7F4,
_tmp7F5);_tmp7F7;});_tmp7F6;});break;}case 290: _LL311: {struct Cyc_List_List*
_tmp7F8=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7F9=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({
struct Cyc_Stmt_tok_struct _tmp7FB;_tmp7FB.tag=Cyc_Stmt_tok;_tmp7FB.f1=Cyc_Parse_flatten_declarations(
_tmp7F8,_tmp7F9);_tmp7FB;});_tmp7FA;});break;}case 291: _LL312: {struct Cyc_List_List*
_tmp7FC=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7FD=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({
struct Cyc_Stmt_tok_struct _tmp7FF;_tmp7FF.tag=Cyc_Stmt_tok;_tmp7FF.f1=Cyc_Parse_flatten_declarations(
_tmp7FC,_tmp7FD);_tmp7FF;});_tmp7FE;});break;}case 292: _LL313: {struct Cyc_List_List*
_tmp800=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp801=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({
struct Cyc_Stmt_tok_struct _tmp803;_tmp803.tag=Cyc_Stmt_tok;_tmp803.f1=Cyc_Parse_flatten_declarations(
_tmp800,_tmp801);_tmp803;});_tmp802;});break;}case 293: _LL314: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({
struct Cyc_Stmt_tok_struct _tmp805;_tmp805.tag=Cyc_Stmt_tok;_tmp805.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp805;});_tmp804;});break;case 294: _LL315: {struct
Cyc_List_List*_tmp806=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(
struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp807=_cycalloc(sizeof(*_tmp807));
_tmp807[0]=({struct Cyc_Stmt_tok_struct _tmp808;_tmp808.tag=Cyc_Stmt_tok;_tmp808.f1=
Cyc_Absyn_forarray_stmt(_tmp806,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp808;});
_tmp807;});break;}case 295: _LL316: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Stmt_tok_struct
_tmp80A;_tmp80A.tag=Cyc_Stmt_tok;_tmp80A.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp80A;});
_tmp809;});break;case 296: _LL317: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80C=
_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Stmt_tok_struct _tmp80D;
_tmp80D.tag=Cyc_Stmt_tok;_tmp80D.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp80D;});
_tmp80C;});break;case 297: _LL318: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80E=
_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=({struct Cyc_Stmt_tok_struct _tmp80F;
_tmp80F.tag=Cyc_Stmt_tok;_tmp80F.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp80F;});
_tmp80E;});break;case 298: _LL319: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp810=
_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Stmt_tok_struct _tmp811;
_tmp811.tag=Cyc_Stmt_tok;_tmp811.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp811;});
_tmp810;});break;case 299: _LL31A: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp812=
_cycalloc(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Stmt_tok_struct _tmp813;
_tmp813.tag=Cyc_Stmt_tok;_tmp813.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp813;});_tmp812;});break;case 300: _LL31B: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=({
struct Cyc_Stmt_tok_struct _tmp815;_tmp815.tag=Cyc_Stmt_tok;_tmp815.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp815;});_tmp814;});break;case 301: _LL31C: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({
struct Cyc_Stmt_tok_struct _tmp817;_tmp817.tag=Cyc_Stmt_tok;_tmp817.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp817;});_tmp816;});break;case 302: _LL31D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({
struct Cyc_Stmt_tok_struct _tmp819;_tmp819.tag=Cyc_Stmt_tok;_tmp819.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp819;});_tmp818;});break;case 303: _LL31E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 304: _LL31F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 305:
_LL320: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp81A=_cycalloc(sizeof(*
_tmp81A));_tmp81A[0]=({struct Cyc_Pattern_tok_struct _tmp81B;_tmp81B.tag=Cyc_Pattern_tok;
_tmp81B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp81B;});
_tmp81A;});break;case 306: _LL321: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 307: _LL322: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Pattern_tok_struct
_tmp81D;_tmp81D.tag=Cyc_Pattern_tok;_tmp81D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp81D;});
_tmp81C;});break;case 308: _LL323: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 309: _LL324: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_Pattern_tok_struct
_tmp81F;_tmp81F.tag=Cyc_Pattern_tok;_tmp81F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp81F;});
_tmp81E;});break;case 310: _LL325: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 311: _LL326: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Pattern_tok_struct
_tmp821;_tmp821.tag=Cyc_Pattern_tok;_tmp821.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp821;});
_tmp820;});break;case 312: _LL327: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 313: _LL328: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Pattern_tok_struct
_tmp823;_tmp823.tag=Cyc_Pattern_tok;_tmp823.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp823;});
_tmp822;});break;case 314: _LL329: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 315: _LL32A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[0]=({struct Cyc_Pattern_tok_struct
_tmp825;_tmp825.tag=Cyc_Pattern_tok;_tmp825.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp825;});
_tmp824;});break;case 316: _LL32B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 317: _LL32C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Pattern_tok_struct
_tmp827;_tmp827.tag=Cyc_Pattern_tok;_tmp827.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp827;});
_tmp826;});break;case 318: _LL32D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Pattern_tok_struct
_tmp829;_tmp829.tag=Cyc_Pattern_tok;_tmp829.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp829;});
_tmp828;});break;case 319: _LL32E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 320: _LL32F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({struct Cyc_Pattern_tok_struct
_tmp82B;_tmp82B.tag=Cyc_Pattern_tok;_tmp82B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp82B;});
_tmp82A;});break;case 321: _LL330: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Pattern_tok_struct
_tmp82D;_tmp82D.tag=Cyc_Pattern_tok;_tmp82D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp82D;});
_tmp82C;});break;case 322: _LL331: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_Pattern_tok_struct
_tmp82F;_tmp82F.tag=Cyc_Pattern_tok;_tmp82F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp82F;});
_tmp82E;});break;case 323: _LL332: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830[0]=({struct Cyc_Pattern_tok_struct
_tmp831;_tmp831.tag=Cyc_Pattern_tok;_tmp831.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp831;});
_tmp830;});break;case 324: _LL333: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL334: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Pattern_tok_struct
_tmp833;_tmp833.tag=Cyc_Pattern_tok;_tmp833.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp833;});
_tmp832;});break;case 326: _LL335: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Pattern_tok_struct
_tmp835;_tmp835.tag=Cyc_Pattern_tok;_tmp835.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp835;});
_tmp834;});break;case 327: _LL336: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL337: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Pattern_tok_struct
_tmp837;_tmp837.tag=Cyc_Pattern_tok;_tmp837.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp837;});
_tmp836;});break;case 329: _LL338: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Pattern_tok_struct
_tmp839;_tmp839.tag=Cyc_Pattern_tok;_tmp839.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp839;});
_tmp838;});break;case 330: _LL339: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL33A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Pattern_tok_struct
_tmp83B;_tmp83B.tag=Cyc_Pattern_tok;_tmp83B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp83B;});
_tmp83A;});break;case 332: _LL33B: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Pattern_tok_struct
_tmp83D;_tmp83D.tag=Cyc_Pattern_tok;_tmp83D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp83D;});
_tmp83C;});break;case 333: _LL33C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Pattern_tok_struct
_tmp83F;_tmp83F.tag=Cyc_Pattern_tok;_tmp83F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp83F;});
_tmp83E;});break;case 334: _LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL33E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Pattern_tok_struct
_tmp841;_tmp841.tag=Cyc_Pattern_tok;_tmp841.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)])).f3,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp841;});
_tmp840;});break;case 336: _LL33F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL340: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842[0]=({struct Cyc_Pattern_tok_struct
_tmp843;_tmp843.tag=Cyc_Pattern_tok;_tmp843.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp843;});_tmp842;});
break;case 338: _LL341: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp844=
_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Pattern_tok_struct _tmp845;
_tmp845.tag=Cyc_Pattern_tok;_tmp845.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp845;});
_tmp844;});break;case 339: _LL342: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Pattern_tok_struct
_tmp847;_tmp847.tag=Cyc_Pattern_tok;_tmp847.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp847;});_tmp846;});break;case 340: _LL343: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848[
0]=({struct Cyc_Pattern_tok_struct _tmp849;_tmp849.tag=Cyc_Pattern_tok;_tmp849.f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp849;});
_tmp848;});break;case 341: _LL344: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({struct Cyc_Pattern_tok_struct
_tmp84B;_tmp84B.tag=Cyc_Pattern_tok;_tmp84B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp84C=_cycalloc(sizeof(*
_tmp84C));_tmp84C[0]=({struct Cyc_Absyn_StructField_struct _tmp84D;_tmp84D.tag=0;
_tmp84D.f1=({struct _tagged_arr*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp84E;});_tmp84D;});_tmp84C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp84B;});
_tmp84A;});break;case 342: _LL345: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F[0]=({struct Cyc_Pattern_tok_struct
_tmp850;_tmp850.tag=Cyc_Pattern_tok;_tmp850.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp851=_cycalloc_atomic(
sizeof(*_tmp851));_tmp851[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp852;_tmp852.tag=
1;_tmp852.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp852;});_tmp851;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp850;});_tmp84F;});break;case 343: _LL346: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 344: _LL347:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 345:
_LL348: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp853=_cycalloc(sizeof(*
_tmp853));_tmp853[0]=({struct Cyc_Pattern_tok_struct _tmp854;_tmp854.tag=Cyc_Pattern_tok;
_tmp854.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
_tmp853;});break;case 346: _LL349: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855[0]=({struct Cyc_Pattern_tok_struct
_tmp856;_tmp856.tag=Cyc_Pattern_tok;_tmp856.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp856;});_tmp855;});
break;case 347: _LL34A: {struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp857=(void*)e->r;
void*_tmp858;void*_tmp859;char _tmp85A;void*_tmp85B;void*_tmp85C;short _tmp85D;
void*_tmp85E;void*_tmp85F;int _tmp860;void*_tmp861;struct _tagged_arr _tmp862;void*
_tmp863;void*_tmp864;void*_tmp865;_LL34D: if(*((int*)_tmp857)!= 0)goto _LL34F;
_tmp858=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp857)->f1;if(_tmp858 <= (void*)
1  || *((int*)_tmp858)!= 0)goto _LL34F;_tmp859=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp858)->f1;_tmp85A=((struct Cyc_Absyn_Char_c_struct*)_tmp858)->f2;_LL34E: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866[
0]=({struct Cyc_Pattern_tok_struct _tmp867;_tmp867.tag=Cyc_Pattern_tok;_tmp867.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*_tmp868=
_cycalloc_atomic(sizeof(*_tmp868));_tmp868[0]=({struct Cyc_Absyn_Char_p_struct
_tmp869;_tmp869.tag=8;_tmp869.f1=_tmp85A;_tmp869;});_tmp868;}),e->loc);_tmp867;});
_tmp866;});goto _LL34C;_LL34F: if(*((int*)_tmp857)!= 0)goto _LL351;_tmp85B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp857)->f1;if(_tmp85B <= (void*)1  || *((int*)
_tmp85B)!= 1)goto _LL351;_tmp85C=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp85B)->f1;
_tmp85D=((struct Cyc_Absyn_Short_c_struct*)_tmp85B)->f2;_LL350: yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A[0]=({
struct Cyc_Pattern_tok_struct _tmp86B;_tmp86B.tag=Cyc_Pattern_tok;_tmp86B.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[
0]=({struct Cyc_Absyn_Int_p_struct _tmp86D;_tmp86D.tag=7;_tmp86D.f1=(void*)_tmp85C;
_tmp86D.f2=(int)_tmp85D;_tmp86D;});_tmp86C;}),e->loc);_tmp86B;});_tmp86A;});goto
_LL34C;_LL351: if(*((int*)_tmp857)!= 0)goto _LL353;_tmp85E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp857)->f1;if(_tmp85E <= (void*)1  || *((int*)_tmp85E)!= 2)goto _LL353;_tmp85F=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp85E)->f1;_tmp860=((struct Cyc_Absyn_Int_c_struct*)
_tmp85E)->f2;_LL352: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp86E=
_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Pattern_tok_struct _tmp86F;
_tmp86F.tag=Cyc_Pattern_tok;_tmp86F.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Absyn_Int_p_struct
_tmp871;_tmp871.tag=7;_tmp871.f1=(void*)_tmp85F;_tmp871.f2=_tmp860;_tmp871;});
_tmp870;}),e->loc);_tmp86F;});_tmp86E;});goto _LL34C;_LL353: if(*((int*)_tmp857)!= 
0)goto _LL355;_tmp861=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp857)->f1;if(
_tmp861 <= (void*)1  || *((int*)_tmp861)!= 4)goto _LL355;_tmp862=((struct Cyc_Absyn_Float_c_struct*)
_tmp861)->f1;_LL354: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp872=
_cycalloc(sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Pattern_tok_struct _tmp873;
_tmp873.tag=Cyc_Pattern_tok;_tmp873.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*
_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874[0]=({struct Cyc_Absyn_Float_p_struct
_tmp875;_tmp875.tag=9;_tmp875.f1=_tmp862;_tmp875;});_tmp874;}),e->loc);_tmp873;});
_tmp872;});goto _LL34C;_LL355: if(*((int*)_tmp857)!= 0)goto _LL357;_tmp863=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp857)->f1;if((int)_tmp863 != 0)goto _LL357;
_LL356: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp876=_cycalloc(sizeof(*
_tmp876));_tmp876[0]=({struct Cyc_Pattern_tok_struct _tmp877;_tmp877.tag=Cyc_Pattern_tok;
_tmp877.f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp877;});_tmp876;});goto _LL34C;
_LL357: if(*((int*)_tmp857)!= 0)goto _LL359;_tmp864=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp857)->f1;if(_tmp864 <= (void*)1  || *((int*)_tmp864)!= 5)goto _LL359;_LL358: Cyc_Parse_err(({
const char*_tmp878="strings cannot occur within patterns";_tag_arr(_tmp878,
sizeof(char),_get_zero_arr_size(_tmp878,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL34C;
_LL359: if(*((int*)_tmp857)!= 0)goto _LL35B;_tmp865=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp857)->f1;if(_tmp865 <= (void*)1  || *((int*)_tmp865)!= 3)goto _LL35B;_LL35A: Cyc_Parse_unimp(({
const char*_tmp879="long long's in patterns";_tag_arr(_tmp879,sizeof(char),
_get_zero_arr_size(_tmp879,24));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL34C;
_LL35B:;_LL35C: Cyc_Parse_err(({const char*_tmp87A="bad constant in case";_tag_arr(
_tmp87A,sizeof(char),_get_zero_arr_size(_tmp87A,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL34C:;}break;}
case 348: _LL34B: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp87B=_cycalloc(
sizeof(*_tmp87B));_tmp87B[0]=({struct Cyc_Pattern_tok_struct _tmp87C;_tmp87C.tag=
Cyc_Pattern_tok;_tmp87C.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp87E;_tmp87E.tag=12;_tmp87E.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp87E;});_tmp87D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87C;});
_tmp87B;});break;case 349: _LL35D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Pattern_tok_struct
_tmp880;_tmp880.tag=Cyc_Pattern_tok;_tmp880.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Tuple_p_struct*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct
Cyc_Absyn_Tuple_p_struct _tmp882;_tmp882.tag=3;_tmp882.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp882;});_tmp881;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp880;});_tmp87F;});break;case 350: _LL35E: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[
0]=({struct Cyc_Pattern_tok_struct _tmp884;_tmp884.tag=Cyc_Pattern_tok;_tmp884.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp885=
_cycalloc(sizeof(*_tmp885));_tmp885[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp886;_tmp886.tag=13;_tmp886.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp886.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp886;});_tmp885;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp884;});_tmp883;});break;case 351: _LL35F: {struct
Cyc_List_List*_tmp887=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888[0]=({
struct Cyc_Pattern_tok_struct _tmp889;_tmp889.tag=Cyc_Pattern_tok;_tmp889.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp88A=_cycalloc(sizeof(*_tmp88A));
_tmp88A[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp88B;_tmp88B.tag=5;_tmp88B.f1=({
struct Cyc_Absyn_AggrInfo _tmp88C;_tmp88C.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp88E;_tmp88E.tag=0;_tmp88E.f1=(void*)((void*)0);_tmp88E.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp88E;});_tmp88D;}));
_tmp88C.targs=0;_tmp88C;});_tmp88B.f2=_tmp887;_tmp88B.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp88B;});_tmp88A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp889;});_tmp888;});break;}case 352: _LL360: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[
0]=({struct Cyc_Pattern_tok_struct _tmp890;_tmp890.tag=Cyc_Pattern_tok;_tmp890.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp891=_cycalloc(
sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp892;_tmp892.tag=
4;_tmp892.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp892;});_tmp891;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp890;});
_tmp88F;});break;case 353: _LL361: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Pattern_tok_struct
_tmp894;_tmp894.tag=Cyc_Pattern_tok;_tmp894.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp896;_tmp896.tag=4;_tmp896.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp897=_cycalloc(sizeof(*_tmp897));
_tmp897[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp898;_tmp898.tag=4;_tmp898.f1=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp898;});_tmp897;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp896;});
_tmp895;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp894;});_tmp893;});break;case 354: _LL362: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[
0]=({struct Cyc_Pattern_tok_struct _tmp89A;_tmp89A.tag=Cyc_Pattern_tok;_tmp89A.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp89B=_cycalloc(
sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Absyn_Reference_p_struct _tmp89C;_tmp89C.tag=
1;_tmp89C.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp89D=_cycalloc(sizeof(*
_tmp89D));_tmp89D->f1=(void*)0;_tmp89D->f2=({struct _tagged_arr*_tmp89E=_cycalloc(
sizeof(*_tmp89E));_tmp89E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp89E;});_tmp89D;}),(void*)
0,0);_tmp89C;});_tmp89B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89A;});
_tmp899;});break;case 355: _LL363: {void*_tmp89F=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8A8=_cycalloc(sizeof(*_tmp8A8));_tmp8A8[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8A9;_tmp8A9.tag=0;_tmp8A9.f1=(void*)((void*)5);_tmp8A9;});_tmp8A8;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[
0]=({struct Cyc_Pattern_tok_struct _tmp8A1;_tmp8A1.tag=Cyc_Pattern_tok;_tmp8A1.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp8A2=_cycalloc(
sizeof(*_tmp8A2));_tmp8A2[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp8A3;_tmp8A3.tag=
2;_tmp8A3.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp89F);
_tmp8A3.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp8A4=_cycalloc(sizeof(*
_tmp8A4));_tmp8A4->f1=(void*)0;_tmp8A4->f2=({struct _tagged_arr*_tmp8A5=_cycalloc(
sizeof(*_tmp8A5));_tmp8A5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8A5;});_tmp8A4;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp8A6=_cycalloc(sizeof(*_tmp8A6));
_tmp8A6[0]=({struct Cyc_Absyn_TagType_struct _tmp8A7;_tmp8A7.tag=17;_tmp8A7.f1=(
void*)_tmp89F;_tmp8A7;});_tmp8A6;}),0);_tmp8A3;});_tmp8A2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A1;});
_tmp8A0;});break;}case 356: _LL364: {struct Cyc_Absyn_Tvar*_tmp8AA=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8B6;_tmp8B6.tag=0;_tmp8B6.f1=(void*)((void*)
5);_tmp8B6;});_tmp8B5;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8AB=
_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Pattern_tok_struct _tmp8AC;
_tmp8AC.tag=Cyc_Pattern_tok;_tmp8AC.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp8AE;_tmp8AE.tag=2;_tmp8AE.f1=_tmp8AA;_tmp8AE.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF->f1=(void*)0;_tmp8AF->f2=({
struct _tagged_arr*_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8B0;});_tmp8AF;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp8B1=_cycalloc(sizeof(*_tmp8B1));
_tmp8B1[0]=({struct Cyc_Absyn_TagType_struct _tmp8B2;_tmp8B2.tag=17;_tmp8B2.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));
_tmp8B3[0]=({struct Cyc_Absyn_VarType_struct _tmp8B4;_tmp8B4.tag=1;_tmp8B4.f1=
_tmp8AA;_tmp8B4;});_tmp8B3;}));_tmp8B2;});_tmp8B1;}),0);_tmp8AE;});_tmp8AD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8AC;});_tmp8AB;});break;}case 357: _LL365: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));
_tmp8B7[0]=({struct Cyc_PatternList_tok_struct _tmp8B8;_tmp8B8.tag=Cyc_PatternList_tok;
_tmp8B8.f1=({struct Cyc_List_List*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9->hd=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp8B9->tl=0;_tmp8B9;});_tmp8B8;});_tmp8B7;});break;case 358: _LL366: yyval=(void*)({
struct Cyc_PatternList_tok_struct*_tmp8BA=_cycalloc(sizeof(*_tmp8BA));_tmp8BA[0]=({
struct Cyc_PatternList_tok_struct _tmp8BB;_tmp8BB.tag=Cyc_PatternList_tok;_tmp8BB.f1=({
struct Cyc_List_List*_tmp8BC=_cycalloc(sizeof(*_tmp8BC));_tmp8BC->hd=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8BC->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8BC;});_tmp8BB;});
_tmp8BA;});break;case 359: _LL367: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({struct Cyc_FieldPattern_tok_struct
_tmp8BE;_tmp8BE.tag=Cyc_FieldPattern_tok;_tmp8BE.f1=({struct _tuple11*_tmp8BF=
_cycalloc(sizeof(*_tmp8BF));_tmp8BF->f1=0;_tmp8BF->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8BF;});_tmp8BE;});
_tmp8BD;});break;case 360: _LL368: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp8C0=_cycalloc(sizeof(*_tmp8C0));_tmp8C0[0]=({struct Cyc_FieldPattern_tok_struct
_tmp8C1;_tmp8C1.tag=Cyc_FieldPattern_tok;_tmp8C1.f1=({struct _tuple11*_tmp8C2=
_cycalloc(sizeof(*_tmp8C2));_tmp8C2->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8C2->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C2;});_tmp8C1;});
_tmp8C0;});break;case 361: _LL369: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp8C3=_cycalloc(sizeof(*_tmp8C3));_tmp8C3[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp8C4;_tmp8C4.tag=Cyc_FieldPatternList_tok;_tmp8C4.f1=({struct Cyc_List_List*
_tmp8C5=_cycalloc(sizeof(*_tmp8C5));_tmp8C5->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C5->tl=0;_tmp8C5;});
_tmp8C4;});_tmp8C3;});break;case 362: _LL36A: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp8C6=_cycalloc(sizeof(*_tmp8C6));_tmp8C6[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp8C7;_tmp8C7.tag=Cyc_FieldPatternList_tok;_tmp8C7.f1=({struct Cyc_List_List*
_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C8->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8C8;});_tmp8C7;});
_tmp8C6;});break;case 363: _LL36B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 364: _LL36C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({struct Cyc_Exp_tok_struct _tmp8CA;
_tmp8CA.tag=Cyc_Exp_tok;_tmp8CA.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CA;});
_tmp8C9;});break;case 365: _LL36D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 366: _LL36E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Exp_tok_struct _tmp8CC;
_tmp8CC.tag=Cyc_Exp_tok;_tmp8CC.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CC;});
_tmp8CB;});break;case 367: _LL36F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp8CD=_cycalloc(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Primopopt_tok_struct
_tmp8CE;_tmp8CE.tag=Cyc_Primopopt_tok;_tmp8CE.f1=0;_tmp8CE;});_tmp8CD;});break;
case 368: _LL370: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8CF=_cycalloc(
sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Primopopt_tok_struct _tmp8D0;_tmp8D0.tag=
Cyc_Primopopt_tok;_tmp8D0.f1=({struct Cyc_Core_Opt*_tmp8D1=_cycalloc(sizeof(*
_tmp8D1));_tmp8D1->v=(void*)((void*)1);_tmp8D1;});_tmp8D0;});_tmp8CF;});break;
case 369: _LL371: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8D2=_cycalloc(
sizeof(*_tmp8D2));_tmp8D2[0]=({struct Cyc_Primopopt_tok_struct _tmp8D3;_tmp8D3.tag=
Cyc_Primopopt_tok;_tmp8D3.f1=({struct Cyc_Core_Opt*_tmp8D4=_cycalloc(sizeof(*
_tmp8D4));_tmp8D4->v=(void*)((void*)3);_tmp8D4;});_tmp8D3;});_tmp8D2;});break;
case 370: _LL372: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8D5=_cycalloc(
sizeof(*_tmp8D5));_tmp8D5[0]=({struct Cyc_Primopopt_tok_struct _tmp8D6;_tmp8D6.tag=
Cyc_Primopopt_tok;_tmp8D6.f1=({struct Cyc_Core_Opt*_tmp8D7=_cycalloc(sizeof(*
_tmp8D7));_tmp8D7->v=(void*)((void*)4);_tmp8D7;});_tmp8D6;});_tmp8D5;});break;
case 371: _LL373: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8D8=_cycalloc(
sizeof(*_tmp8D8));_tmp8D8[0]=({struct Cyc_Primopopt_tok_struct _tmp8D9;_tmp8D9.tag=
Cyc_Primopopt_tok;_tmp8D9.f1=({struct Cyc_Core_Opt*_tmp8DA=_cycalloc(sizeof(*
_tmp8DA));_tmp8DA->v=(void*)((void*)0);_tmp8DA;});_tmp8D9;});_tmp8D8;});break;
case 372: _LL374: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8DB=_cycalloc(
sizeof(*_tmp8DB));_tmp8DB[0]=({struct Cyc_Primopopt_tok_struct _tmp8DC;_tmp8DC.tag=
Cyc_Primopopt_tok;_tmp8DC.f1=({struct Cyc_Core_Opt*_tmp8DD=_cycalloc(sizeof(*
_tmp8DD));_tmp8DD->v=(void*)((void*)2);_tmp8DD;});_tmp8DC;});_tmp8DB;});break;
case 373: _LL375: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8DE=_cycalloc(
sizeof(*_tmp8DE));_tmp8DE[0]=({struct Cyc_Primopopt_tok_struct _tmp8DF;_tmp8DF.tag=
Cyc_Primopopt_tok;_tmp8DF.f1=({struct Cyc_Core_Opt*_tmp8E0=_cycalloc(sizeof(*
_tmp8E0));_tmp8E0->v=(void*)((void*)16);_tmp8E0;});_tmp8DF;});_tmp8DE;});break;
case 374: _LL376: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8E1=_cycalloc(
sizeof(*_tmp8E1));_tmp8E1[0]=({struct Cyc_Primopopt_tok_struct _tmp8E2;_tmp8E2.tag=
Cyc_Primopopt_tok;_tmp8E2.f1=({struct Cyc_Core_Opt*_tmp8E3=_cycalloc(sizeof(*
_tmp8E3));_tmp8E3->v=(void*)((void*)17);_tmp8E3;});_tmp8E2;});_tmp8E1;});break;
case 375: _LL377: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8E4=_cycalloc(
sizeof(*_tmp8E4));_tmp8E4[0]=({struct Cyc_Primopopt_tok_struct _tmp8E5;_tmp8E5.tag=
Cyc_Primopopt_tok;_tmp8E5.f1=({struct Cyc_Core_Opt*_tmp8E6=_cycalloc(sizeof(*
_tmp8E6));_tmp8E6->v=(void*)((void*)13);_tmp8E6;});_tmp8E5;});_tmp8E4;});break;
case 376: _LL378: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8E7=_cycalloc(
sizeof(*_tmp8E7));_tmp8E7[0]=({struct Cyc_Primopopt_tok_struct _tmp8E8;_tmp8E8.tag=
Cyc_Primopopt_tok;_tmp8E8.f1=({struct Cyc_Core_Opt*_tmp8E9=_cycalloc(sizeof(*
_tmp8E9));_tmp8E9->v=(void*)((void*)15);_tmp8E9;});_tmp8E8;});_tmp8E7;});break;
case 377: _LL379: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp8EA=_cycalloc(
sizeof(*_tmp8EA));_tmp8EA[0]=({struct Cyc_Primopopt_tok_struct _tmp8EB;_tmp8EB.tag=
Cyc_Primopopt_tok;_tmp8EB.f1=({struct Cyc_Core_Opt*_tmp8EC=_cycalloc(sizeof(*
_tmp8EC));_tmp8EC->v=(void*)((void*)14);_tmp8EC;});_tmp8EB;});_tmp8EA;});break;
case 378: _LL37A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 379: _LL37B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8ED=_cycalloc(
sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Exp_tok_struct _tmp8EE;_tmp8EE.tag=Cyc_Exp_tok;
_tmp8EE.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EE;});
_tmp8ED;});break;case 380: _LL37C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EF=
_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Exp_tok_struct _tmp8F0;_tmp8F0.tag=
Cyc_Exp_tok;_tmp8F0.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F0;});
_tmp8EF;});break;case 381: _LL37D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F1=
_cycalloc(sizeof(*_tmp8F1));_tmp8F1[0]=({struct Cyc_Exp_tok_struct _tmp8F2;_tmp8F2.tag=
Cyc_Exp_tok;_tmp8F2.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F2;});
_tmp8F1;});break;case 382: _LL37E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F3=
_cycalloc(sizeof(*_tmp8F3));_tmp8F3[0]=({struct Cyc_Exp_tok_struct _tmp8F4;_tmp8F4.tag=
Cyc_Exp_tok;_tmp8F4.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F4;});
_tmp8F3;});break;case 383: _LL37F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F5=
_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct Cyc_Exp_tok_struct _tmp8F6;_tmp8F6.tag=
Cyc_Exp_tok;_tmp8F6.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F6;});
_tmp8F5;});break;case 384: _LL380: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F7=
_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Exp_tok_struct _tmp8F8;_tmp8F8.tag=
Cyc_Exp_tok;_tmp8F8.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F8;});
_tmp8F7;});break;case 385: _LL381: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 386: _LL382: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 387: _LL383: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8F9=_cycalloc(sizeof(*_tmp8F9));_tmp8F9[0]=({struct Cyc_Exp_tok_struct _tmp8FA;
_tmp8FA.tag=Cyc_Exp_tok;_tmp8FA.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FA;});
_tmp8F9;});break;case 388: _LL384: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 389: _LL385: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB[0]=({struct Cyc_Exp_tok_struct _tmp8FC;
_tmp8FC.tag=Cyc_Exp_tok;_tmp8FC.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FC;});
_tmp8FB;});break;case 390: _LL386: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 391: _LL387: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8FD=_cycalloc(sizeof(*_tmp8FD));_tmp8FD[0]=({struct Cyc_Exp_tok_struct _tmp8FE;
_tmp8FE.tag=Cyc_Exp_tok;_tmp8FE.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FE;});
_tmp8FD;});break;case 392: _LL388: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 393: _LL389: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF[0]=({struct Cyc_Exp_tok_struct _tmp900;
_tmp900.tag=Cyc_Exp_tok;_tmp900.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp900;});
_tmp8FF;});break;case 394: _LL38A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 395: _LL38B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp901=_cycalloc(sizeof(*_tmp901));_tmp901[0]=({struct Cyc_Exp_tok_struct _tmp902;
_tmp902.tag=Cyc_Exp_tok;_tmp902.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp902;});
_tmp901;});break;case 396: _LL38C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 397: _LL38D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp903=_cycalloc(sizeof(*_tmp903));_tmp903[0]=({struct Cyc_Exp_tok_struct _tmp904;
_tmp904.tag=Cyc_Exp_tok;_tmp904.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp904;});
_tmp903;});break;case 398: _LL38E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp905=
_cycalloc(sizeof(*_tmp905));_tmp905[0]=({struct Cyc_Exp_tok_struct _tmp906;_tmp906.tag=
Cyc_Exp_tok;_tmp906.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp906;});
_tmp905;});break;case 399: _LL38F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 400: _LL390: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907[0]=({struct Cyc_Exp_tok_struct _tmp908;
_tmp908.tag=Cyc_Exp_tok;_tmp908.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp908;});
_tmp907;});break;case 401: _LL391: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp909=
_cycalloc(sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Exp_tok_struct _tmp90A;_tmp90A.tag=
Cyc_Exp_tok;_tmp90A.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90A;});
_tmp909;});break;case 402: _LL392: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp90B=
_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=({struct Cyc_Exp_tok_struct _tmp90C;_tmp90C.tag=
Cyc_Exp_tok;_tmp90C.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90C;});
_tmp90B;});break;case 403: _LL393: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp90D=
_cycalloc(sizeof(*_tmp90D));_tmp90D[0]=({struct Cyc_Exp_tok_struct _tmp90E;_tmp90E.tag=
Cyc_Exp_tok;_tmp90E.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90E;});
_tmp90D;});break;case 404: _LL394: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL395: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp90F=_cycalloc(sizeof(*_tmp90F));_tmp90F[0]=({struct Cyc_Exp_tok_struct _tmp910;
_tmp910.tag=Cyc_Exp_tok;_tmp910.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp910;});
_tmp90F;});break;case 406: _LL396: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp911=
_cycalloc(sizeof(*_tmp911));_tmp911[0]=({struct Cyc_Exp_tok_struct _tmp912;_tmp912.tag=
Cyc_Exp_tok;_tmp912.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp912;});
_tmp911;});break;case 407: _LL397: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 408: _LL398: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp913=_cycalloc(sizeof(*_tmp913));_tmp913[0]=({struct Cyc_Exp_tok_struct _tmp914;
_tmp914.tag=Cyc_Exp_tok;_tmp914.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp914;});
_tmp913;});break;case 409: _LL399: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp915=
_cycalloc(sizeof(*_tmp915));_tmp915[0]=({struct Cyc_Exp_tok_struct _tmp916;_tmp916.tag=
Cyc_Exp_tok;_tmp916.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp916;});
_tmp915;});break;case 410: _LL39A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 411: _LL39B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp917=_cycalloc(sizeof(*_tmp917));_tmp917[0]=({struct Cyc_Exp_tok_struct _tmp918;
_tmp918.tag=Cyc_Exp_tok;_tmp918.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp918;});
_tmp917;});break;case 412: _LL39C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp919=
_cycalloc(sizeof(*_tmp919));_tmp919[0]=({struct Cyc_Exp_tok_struct _tmp91A;_tmp91A.tag=
Cyc_Exp_tok;_tmp91A.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp91A;});
_tmp919;});break;case 413: _LL39D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp91B=
_cycalloc(sizeof(*_tmp91B));_tmp91B[0]=({struct Cyc_Exp_tok_struct _tmp91C;_tmp91C.tag=
Cyc_Exp_tok;_tmp91C.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp91C;});
_tmp91B;});break;case 414: _LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL39F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp91D=_cycalloc(sizeof(*_tmp91D));_tmp91D[0]=({struct Cyc_Exp_tok_struct _tmp91E;
_tmp91E.tag=Cyc_Exp_tok;_tmp91E.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp91E;});
_tmp91D;});break;case 416: _LL3A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3A1: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp91F=_cycalloc(sizeof(*_tmp91F));_tmp91F[0]=({struct Cyc_Exp_tok_struct _tmp920;
_tmp920.tag=Cyc_Exp_tok;_tmp920.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp920;});
_tmp91F;});break;case 418: _LL3A2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp921=
_cycalloc(sizeof(*_tmp921));_tmp921[0]=({struct Cyc_Exp_tok_struct _tmp922;_tmp922.tag=
Cyc_Exp_tok;_tmp922.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp922;});
_tmp921;});break;case 419: _LL3A3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp923=
_cycalloc(sizeof(*_tmp923));_tmp923[0]=({struct Cyc_Exp_tok_struct _tmp924;_tmp924.tag=
Cyc_Exp_tok;_tmp924.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp924;});
_tmp923;});break;case 420: _LL3A4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp925=
_cycalloc(sizeof(*_tmp925));_tmp925[0]=({struct Cyc_Exp_tok_struct _tmp926;_tmp926.tag=
Cyc_Exp_tok;_tmp926.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp926;});
_tmp925;});break;case 421: _LL3A5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3A6: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp927=_cycalloc(sizeof(*_tmp927));_tmp927[0]=({struct Cyc_Exp_tok_struct _tmp928;
_tmp928.tag=Cyc_Exp_tok;_tmp928.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp928;});
_tmp927;});break;case 423: _LL3A7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp929=
_cycalloc(sizeof(*_tmp929));_tmp929[0]=({struct Cyc_Exp_tok_struct _tmp92A;_tmp92A.tag=
Cyc_Exp_tok;_tmp92A.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92A;});
_tmp929;});break;case 424: _LL3A8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp92B=
_cycalloc(sizeof(*_tmp92B));_tmp92B[0]=({struct Cyc_Exp_tok_struct _tmp92C;_tmp92C.tag=
Cyc_Exp_tok;_tmp92C.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92C;});
_tmp92B;});break;case 425: _LL3A9: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp92D=
_cycalloc(sizeof(*_tmp92D));_tmp92D[0]=({struct Cyc_Exp_tok_struct _tmp92E;_tmp92E.tag=
Cyc_Exp_tok;_tmp92E.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp92F=_cycalloc(sizeof(*_tmp92F));_tmp92F[0]=({struct Cyc_Absyn_StructField_struct
_tmp930;_tmp930.tag=0;_tmp930.f1=({struct _tagged_arr*_tmp931=_cycalloc(sizeof(*
_tmp931));_tmp931[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp931;});_tmp930;});_tmp92F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92E;});
_tmp92D;});break;case 426: _LL3AA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp932=
_cycalloc(sizeof(*_tmp932));_tmp932[0]=({struct Cyc_Exp_tok_struct _tmp933;_tmp933.tag=
Cyc_Exp_tok;_tmp933.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp934=_cycalloc_atomic(sizeof(*_tmp934));_tmp934[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp935;_tmp935.tag=1;_tmp935.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp935;});_tmp934;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp933;});_tmp932;});break;case 427: _LL3AB: {struct
Cyc_Position_Segment*_tmp936=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp937=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp936,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp938=_cycalloc(
sizeof(*_tmp938));_tmp938[0]=({struct Cyc_Exp_tok_struct _tmp939;_tmp939.tag=Cyc_Exp_tok;
_tmp939.f1=Cyc_Absyn_gentyp_exp(_tmp937,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp939;});
_tmp938;});break;}case 428: _LL3AC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp93A=
_cycalloc(sizeof(*_tmp93A));_tmp93A[0]=({struct Cyc_Exp_tok_struct _tmp93B;_tmp93B.tag=
Cyc_Exp_tok;_tmp93B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp93C=_cycalloc(sizeof(*_tmp93C));_tmp93C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp93D;_tmp93D.tag=35;_tmp93D.f1=({struct Cyc_Absyn_MallocInfo _tmp93E;_tmp93E.is_calloc=
0;_tmp93E.rgn=0;_tmp93E.elt_type=0;_tmp93E.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp93E.fat_result=0;
_tmp93E;});_tmp93D;});_tmp93C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp93B;});
_tmp93A;});break;case 429: _LL3AD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp93F=
_cycalloc(sizeof(*_tmp93F));_tmp93F[0]=({struct Cyc_Exp_tok_struct _tmp940;_tmp940.tag=
Cyc_Exp_tok;_tmp940.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp941=_cycalloc(sizeof(*_tmp941));_tmp941[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp942;_tmp942.tag=35;_tmp942.f1=({struct Cyc_Absyn_MallocInfo _tmp943;_tmp943.is_calloc=
0;_tmp943.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp943.elt_type=0;
_tmp943.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp943.fat_result=0;_tmp943;});_tmp942;});_tmp941;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp940;});
_tmp93F;});break;case 430: _LL3AE: {void*_tmp945;struct _tuple2 _tmp944=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp945=_tmp944.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp946=_cycalloc(sizeof(*_tmp946));
_tmp946[0]=({struct Cyc_Exp_tok_struct _tmp947;_tmp947.tag=Cyc_Exp_tok;_tmp947.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp948=_cycalloc(
sizeof(*_tmp948));_tmp948[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp949;_tmp949.tag=
35;_tmp949.f1=({struct Cyc_Absyn_MallocInfo _tmp94A;_tmp94A.is_calloc=1;_tmp94A.rgn=
0;_tmp94A.elt_type=({void**_tmp94B=_cycalloc(sizeof(*_tmp94B));_tmp94B[0]=
_tmp945;_tmp94B;});_tmp94A.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp94A.fat_result=0;
_tmp94A;});_tmp949;});_tmp948;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp947;});
_tmp946;});break;}case 431: _LL3AF: {void*_tmp94D;struct _tuple2 _tmp94C=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp94D=_tmp94C.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp94E=_cycalloc(sizeof(*_tmp94E));
_tmp94E[0]=({struct Cyc_Exp_tok_struct _tmp94F;_tmp94F.tag=Cyc_Exp_tok;_tmp94F.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp950=_cycalloc(
sizeof(*_tmp950));_tmp950[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp951;_tmp951.tag=
35;_tmp951.f1=({struct Cyc_Absyn_MallocInfo _tmp952;_tmp952.is_calloc=1;_tmp952.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp952.elt_type=({void**_tmp953=_cycalloc(sizeof(*_tmp953));
_tmp953[0]=_tmp94D;_tmp953;});_tmp952.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp952.fat_result=0;
_tmp952;});_tmp951;});_tmp950;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94F;});
_tmp94E;});break;}case 432: _LL3B0: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp954=_cycalloc(sizeof(*_tmp954));_tmp954[0]=({struct Cyc_Primop_tok_struct
_tmp955;_tmp955.tag=Cyc_Primop_tok;_tmp955.f1=(void*)((void*)12);_tmp955;});
_tmp954;});break;case 433: _LL3B1: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp956=_cycalloc(sizeof(*_tmp956));_tmp956[0]=({struct Cyc_Primop_tok_struct
_tmp957;_tmp957.tag=Cyc_Primop_tok;_tmp957.f1=(void*)((void*)11);_tmp957;});
_tmp956;});break;case 434: _LL3B2: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp958=_cycalloc(sizeof(*_tmp958));_tmp958[0]=({struct Cyc_Primop_tok_struct
_tmp959;_tmp959.tag=Cyc_Primop_tok;_tmp959.f1=(void*)((void*)2);_tmp959;});
_tmp958;});break;case 435: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 436: _LL3B4: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp95A=_cycalloc(sizeof(*_tmp95A));_tmp95A[0]=({struct Cyc_Exp_tok_struct _tmp95B;
_tmp95B.tag=Cyc_Exp_tok;_tmp95B.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95B;});
_tmp95A;});break;case 437: _LL3B5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp95C=
_cycalloc(sizeof(*_tmp95C));_tmp95C[0]=({struct Cyc_Exp_tok_struct _tmp95D;_tmp95D.tag=
Cyc_Exp_tok;_tmp95D.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95D;});
_tmp95C;});break;case 438: _LL3B6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp95E=
_cycalloc(sizeof(*_tmp95E));_tmp95E[0]=({struct Cyc_Exp_tok_struct _tmp95F;_tmp95F.tag=
Cyc_Exp_tok;_tmp95F.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95F;});
_tmp95E;});break;case 439: _LL3B7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp960=
_cycalloc(sizeof(*_tmp960));_tmp960[0]=({struct Cyc_Exp_tok_struct _tmp961;_tmp961.tag=
Cyc_Exp_tok;_tmp961.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp962;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp961;});
_tmp960;});break;case 440: _LL3B8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp963=
_cycalloc(sizeof(*_tmp963));_tmp963[0]=({struct Cyc_Exp_tok_struct _tmp964;_tmp964.tag=
Cyc_Exp_tok;_tmp964.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp965=_cycalloc(sizeof(*_tmp965));_tmp965[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp965;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp964;});
_tmp963;});break;case 441: _LL3B9: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp966=
_cycalloc(sizeof(*_tmp966));_tmp966[0]=({struct Cyc_Exp_tok_struct _tmp967;_tmp967.tag=
Cyc_Exp_tok;_tmp967.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp967;});
_tmp966;});break;case 442: _LL3BA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp968=
_cycalloc(sizeof(*_tmp968));_tmp968[0]=({struct Cyc_Exp_tok_struct _tmp969;_tmp969.tag=
Cyc_Exp_tok;_tmp969.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp969;});
_tmp968;});break;case 443: _LL3BB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96A=
_cycalloc(sizeof(*_tmp96A));_tmp96A[0]=({struct Cyc_Exp_tok_struct _tmp96B;_tmp96B.tag=
Cyc_Exp_tok;_tmp96B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp96C=_cycalloc(sizeof(*_tmp96C));_tmp96C[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp96D;_tmp96D.tag=27;_tmp96D.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp96D.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp96D;});_tmp96C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp96B;});_tmp96A;});break;case 444: _LL3BC: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp96E=_cycalloc(sizeof(*_tmp96E));_tmp96E[0]=({
struct Cyc_Exp_tok_struct _tmp96F;_tmp96F.tag=Cyc_Exp_tok;_tmp96F.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp970=_cycalloc(sizeof(*_tmp970));
_tmp970[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp971;_tmp971.tag=27;_tmp971.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp971.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp971;});_tmp970;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96F;});
_tmp96E;});break;case 445: _LL3BD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp972=
_cycalloc(sizeof(*_tmp972));_tmp972[0]=({struct Cyc_Exp_tok_struct _tmp973;_tmp973.tag=
Cyc_Exp_tok;_tmp973.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp974=_cycalloc(sizeof(*_tmp974));_tmp974[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp975;_tmp975.tag=39;_tmp975.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp975;});_tmp974;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp973;});_tmp972;});break;case 446: _LL3BE: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp976=_cycalloc(sizeof(*_tmp976));_tmp976[0]=({
struct Cyc_Exp_tok_struct _tmp977;_tmp977.tag=Cyc_Exp_tok;_tmp977.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp978=_cycalloc(sizeof(*_tmp978));
_tmp978[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp979;_tmp979.tag=38;_tmp979.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp979;});_tmp978;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp977;});
_tmp976;});break;case 447: _LL3BF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp97A=
_cycalloc(sizeof(*_tmp97A));_tmp97A[0]=({struct Cyc_Exp_tok_struct _tmp97B;_tmp97B.tag=
Cyc_Exp_tok;_tmp97B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp97C=_cycalloc(sizeof(*_tmp97C));_tmp97C[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp97D;_tmp97D.tag=2;_tmp97D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp97D;});_tmp97C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97B;});
_tmp97A;});break;case 448: _LL3C0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 449: _LL3C1: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97E=_cycalloc(sizeof(*_tmp97E));_tmp97E[0]=({struct Cyc_Exp_tok_struct _tmp97F;
_tmp97F.tag=Cyc_Exp_tok;_tmp97F.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97F;});
_tmp97E;});break;case 450: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 451: _LL3C3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp980=_cycalloc(sizeof(*_tmp980));_tmp980[0]=({struct Cyc_Exp_tok_struct _tmp981;
_tmp981.tag=Cyc_Exp_tok;_tmp981.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp981;});
_tmp980;});break;case 452: _LL3C4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp982=
_cycalloc(sizeof(*_tmp982));_tmp982[0]=({struct Cyc_Exp_tok_struct _tmp983;_tmp983.tag=
Cyc_Exp_tok;_tmp983.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp983;});
_tmp982;});break;case 453: _LL3C5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp984=
_cycalloc(sizeof(*_tmp984));_tmp984[0]=({struct Cyc_Exp_tok_struct _tmp985;_tmp985.tag=
Cyc_Exp_tok;_tmp985.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp985;});
_tmp984;});break;case 454: _LL3C6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp986=
_cycalloc(sizeof(*_tmp986));_tmp986[0]=({struct Cyc_Exp_tok_struct _tmp987;_tmp987.tag=
Cyc_Exp_tok;_tmp987.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp987;});
_tmp986;});break;case 455: _LL3C7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp988=
_cycalloc(sizeof(*_tmp988));_tmp988[0]=({struct Cyc_Exp_tok_struct _tmp989;_tmp989.tag=
Cyc_Exp_tok;_tmp989.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp98A=_cycalloc(sizeof(*_tmp98A));_tmp98A[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp98B;_tmp98B.tag=30;_tmp98B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp98B.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp98B.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp98B.f4=0;
_tmp98B;});_tmp98A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp989;});
_tmp988;});break;case 456: _LL3C8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98C=
_cycalloc(sizeof(*_tmp98C));_tmp98C[0]=({struct Cyc_Exp_tok_struct _tmp98D;_tmp98D.tag=
Cyc_Exp_tok;_tmp98D.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98D;});
_tmp98C;});break;case 457: _LL3C9: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp98E=_cycalloc(sizeof(*_tmp98E));_tmp98E[0]=({struct Cyc_ExpList_tok_struct
_tmp98F;_tmp98F.tag=Cyc_ExpList_tok;_tmp98F.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp98F;});_tmp98E;});
break;case 458: _LL3CA: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp990=
_cycalloc(sizeof(*_tmp990));_tmp990[0]=({struct Cyc_ExpList_tok_struct _tmp991;
_tmp991.tag=Cyc_ExpList_tok;_tmp991.f1=({struct Cyc_List_List*_tmp992=_cycalloc(
sizeof(*_tmp992));_tmp992->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp992->tl=0;_tmp992;});
_tmp991;});_tmp990;});break;case 459: _LL3CB: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp993=_cycalloc(sizeof(*_tmp993));_tmp993[0]=({struct Cyc_ExpList_tok_struct
_tmp994;_tmp994.tag=Cyc_ExpList_tok;_tmp994.f1=({struct Cyc_List_List*_tmp995=
_cycalloc(sizeof(*_tmp995));_tmp995->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp995->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp995;});_tmp994;});
_tmp993;});break;case 460: _LL3CC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp996=
_cycalloc(sizeof(*_tmp996));_tmp996[0]=({struct Cyc_Exp_tok_struct _tmp997;_tmp997.tag=
Cyc_Exp_tok;_tmp997.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp997;});
_tmp996;});break;case 461: _LL3CD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp998=
_cycalloc(sizeof(*_tmp998));_tmp998[0]=({struct Cyc_Exp_tok_struct _tmp999;_tmp999.tag=
Cyc_Exp_tok;_tmp999.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp999;});
_tmp998;});break;case 462: _LL3CE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99A=
_cycalloc(sizeof(*_tmp99A));_tmp99A[0]=({struct Cyc_Exp_tok_struct _tmp99B;_tmp99B.tag=
Cyc_Exp_tok;_tmp99B.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99B;});
_tmp99A;});break;case 463: _LL3CF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99C=
_cycalloc(sizeof(*_tmp99C));_tmp99C[0]=({struct Cyc_Exp_tok_struct _tmp99D;_tmp99D.tag=
Cyc_Exp_tok;_tmp99D.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99D;});
_tmp99C;});break;case 464: _LL3D0: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp99E=_cycalloc(sizeof(*_tmp99E));_tmp99E[0]=({struct Cyc_QualId_tok_struct
_tmp99F;_tmp99F.tag=Cyc_QualId_tok;_tmp99F.f1=({struct _tuple1*_tmp9A0=_cycalloc(
sizeof(*_tmp9A0));_tmp9A0->f1=Cyc_Absyn_rel_ns_null;_tmp9A0->f2=({struct
_tagged_arr*_tmp9A1=_cycalloc(sizeof(*_tmp9A1));_tmp9A1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9A1;});_tmp9A0;});
_tmp99F;});_tmp99E;});break;case 465: _LL3D1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 466: _LL3D2: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp9A2=_cycalloc(sizeof(*_tmp9A2));_tmp9A2[
0]=({struct Cyc_QualId_tok_struct _tmp9A3;_tmp9A3.tag=Cyc_QualId_tok;_tmp9A3.f1=({
struct _tuple1*_tmp9A4=_cycalloc(sizeof(*_tmp9A4));_tmp9A4->f1=Cyc_Absyn_rel_ns_null;
_tmp9A4->f2=({struct _tagged_arr*_tmp9A5=_cycalloc(sizeof(*_tmp9A5));_tmp9A5[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp9A5;});_tmp9A4;});_tmp9A3;});_tmp9A2;});break;case 467: _LL3D3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 468: _LL3D4: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 469: _LL3D5:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 470:
_LL3D6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
471: _LL3D7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 472: _LL3D8: break;case 473: _LL3D9:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL3DA: break;}
yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(474,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(132,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6697) && Cyc_yycheck[_check_known_subscript_notnull(6698,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6698,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(132,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(957,yystate)];if(yyn > - 32768  && yyn < 6697){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 263 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6698,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(263,x)])
+ 15,count ++);}msg=({unsigned int _tmp9A6=(unsigned int)(sze + 15);char*_tmp9A7=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp9A6 + 1));struct _tagged_arr
_tmp9A9=_tag_arr(_tmp9A7,sizeof(char),_tmp9A6 + 1);{unsigned int _tmp9A8=_tmp9A6;
unsigned int i;for(i=0;i < _tmp9A8;i ++){_tmp9A7[i]='\000';}_tmp9A7[_tmp9A8]=(char)
0;}_tmp9A9;});Cyc_strcpy(msg,({const char*_tmp9AA="parse error";_tag_arr(_tmp9AA,
sizeof(char),_get_zero_arr_size(_tmp9AA,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 263 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(6698,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmp9AB=", expecting `";
_tag_arr(_tmp9AB,sizeof(char),_get_zero_arr_size(_tmp9AB,14));}):(struct
_tagged_arr)({const char*_tmp9AC=" or `";_tag_arr(_tmp9AC,sizeof(char),
_get_zero_arr_size(_tmp9AC,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(263,x)]);Cyc_strcat(msg,({const char*_tmp9AD="'";
_tag_arr(_tmp9AD,sizeof(char),_get_zero_arr_size(_tmp9AD,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmp9AE="parse error";
_tag_arr(_tmp9AE,sizeof(char),_get_zero_arr_size(_tmp9AE,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(957,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 6697) || Cyc_yycheck[_check_known_subscript_notnull(
6698,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6698,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 956)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp9B8=v;struct _tuple13*_tmp9B9;
struct _tuple13 _tmp9BA;int _tmp9BB;char _tmp9BC;short _tmp9BD;struct _tagged_arr
_tmp9BE;struct Cyc_Core_Opt*_tmp9BF;struct Cyc_Core_Opt*_tmp9C0;struct Cyc_Core_Opt
_tmp9C1;struct _tagged_arr*_tmp9C2;struct _tuple1*_tmp9C3;struct _tuple1 _tmp9C4;
void*_tmp9C5;struct _tagged_arr*_tmp9C6;_LL3DD: if(*((void**)_tmp9B8)!= Cyc_Int_tok)
goto _LL3DF;_tmp9B9=((struct Cyc_Int_tok_struct*)_tmp9B8)->f1;_tmp9BA=*_tmp9B9;
_tmp9BB=_tmp9BA.f2;_LL3DE:({struct Cyc_Int_pa_struct _tmp9C9;_tmp9C9.tag=1;_tmp9C9.f1=(
unsigned int)_tmp9BB;{void*_tmp9C7[1]={& _tmp9C9};Cyc_fprintf(Cyc_stderr,({const
char*_tmp9C8="%d";_tag_arr(_tmp9C8,sizeof(char),_get_zero_arr_size(_tmp9C8,3));}),
_tag_arr(_tmp9C7,sizeof(void*),1));}});goto _LL3DC;_LL3DF: if(*((void**)_tmp9B8)!= 
Cyc_Char_tok)goto _LL3E1;_tmp9BC=((struct Cyc_Char_tok_struct*)_tmp9B8)->f1;_LL3E0:({
struct Cyc_Int_pa_struct _tmp9CC;_tmp9CC.tag=1;_tmp9CC.f1=(unsigned int)((int)
_tmp9BC);{void*_tmp9CA[1]={& _tmp9CC};Cyc_fprintf(Cyc_stderr,({const char*_tmp9CB="%c";
_tag_arr(_tmp9CB,sizeof(char),_get_zero_arr_size(_tmp9CB,3));}),_tag_arr(_tmp9CA,
sizeof(void*),1));}});goto _LL3DC;_LL3E1: if(*((void**)_tmp9B8)!= Cyc_Short_tok)
goto _LL3E3;_tmp9BD=((struct Cyc_Short_tok_struct*)_tmp9B8)->f1;_LL3E2:({struct Cyc_Int_pa_struct
_tmp9CF;_tmp9CF.tag=1;_tmp9CF.f1=(unsigned int)((int)_tmp9BD);{void*_tmp9CD[1]={&
_tmp9CF};Cyc_fprintf(Cyc_stderr,({const char*_tmp9CE="%ds";_tag_arr(_tmp9CE,
sizeof(char),_get_zero_arr_size(_tmp9CE,4));}),_tag_arr(_tmp9CD,sizeof(void*),1));}});
goto _LL3DC;_LL3E3: if(*((void**)_tmp9B8)!= Cyc_String_tok)goto _LL3E5;_tmp9BE=((
struct Cyc_String_tok_struct*)_tmp9B8)->f1;_LL3E4:({struct Cyc_String_pa_struct
_tmp9D2;_tmp9D2.tag=0;_tmp9D2.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp9BE);{
void*_tmp9D0[1]={& _tmp9D2};Cyc_fprintf(Cyc_stderr,({const char*_tmp9D1="\"%s\"";
_tag_arr(_tmp9D1,sizeof(char),_get_zero_arr_size(_tmp9D1,5));}),_tag_arr(_tmp9D0,
sizeof(void*),1));}});goto _LL3DC;_LL3E5: if(*((void**)_tmp9B8)!= Cyc_Stringopt_tok)
goto _LL3E7;_tmp9BF=((struct Cyc_Stringopt_tok_struct*)_tmp9B8)->f1;if(_tmp9BF != 0)
goto _LL3E7;_LL3E6:({void*_tmp9D3[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp9D4="NULL";_tag_arr(_tmp9D4,sizeof(char),_get_zero_arr_size(_tmp9D4,5));}),
_tag_arr(_tmp9D3,sizeof(void*),0));});goto _LL3DC;_LL3E7: if(*((void**)_tmp9B8)!= 
Cyc_Stringopt_tok)goto _LL3E9;_tmp9C0=((struct Cyc_Stringopt_tok_struct*)_tmp9B8)->f1;
if(_tmp9C0 == 0)goto _LL3E9;_tmp9C1=*_tmp9C0;_tmp9C2=(struct _tagged_arr*)_tmp9C1.v;
_LL3E8:({struct Cyc_String_pa_struct _tmp9D7;_tmp9D7.tag=0;_tmp9D7.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp9C2);{void*_tmp9D5[1]={& _tmp9D7};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9D6="\"%s\"";_tag_arr(_tmp9D6,sizeof(char),
_get_zero_arr_size(_tmp9D6,5));}),_tag_arr(_tmp9D5,sizeof(void*),1));}});goto
_LL3DC;_LL3E9: if(*((void**)_tmp9B8)!= Cyc_QualId_tok)goto _LL3EB;_tmp9C3=((struct
Cyc_QualId_tok_struct*)_tmp9B8)->f1;_tmp9C4=*_tmp9C3;_tmp9C5=_tmp9C4.f1;_tmp9C6=
_tmp9C4.f2;_LL3EA: {struct Cyc_List_List*_tmp9D8=0;{void*_tmp9D9=_tmp9C5;struct
Cyc_List_List*_tmp9DA;struct Cyc_List_List*_tmp9DB;_LL3EE: if(_tmp9D9 <= (void*)1
 || *((int*)_tmp9D9)!= 0)goto _LL3F0;_tmp9DA=((struct Cyc_Absyn_Rel_n_struct*)
_tmp9D9)->f1;_LL3EF: _tmp9D8=_tmp9DA;goto _LL3ED;_LL3F0: if(_tmp9D9 <= (void*)1  || *((
int*)_tmp9D9)!= 1)goto _LL3F2;_tmp9DB=((struct Cyc_Absyn_Abs_n_struct*)_tmp9D9)->f1;
_LL3F1: _tmp9D8=_tmp9DB;goto _LL3ED;_LL3F2: if((int)_tmp9D9 != 0)goto _LL3ED;_LL3F3:
goto _LL3ED;_LL3ED:;}for(0;_tmp9D8 != 0;_tmp9D8=_tmp9D8->tl){({struct Cyc_String_pa_struct
_tmp9DE;_tmp9DE.tag=0;_tmp9DE.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp9D8->hd));{void*_tmp9DC[1]={& _tmp9DE};Cyc_fprintf(Cyc_stderr,({
const char*_tmp9DD="%s::";_tag_arr(_tmp9DD,sizeof(char),_get_zero_arr_size(
_tmp9DD,5));}),_tag_arr(_tmp9DC,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp9E1;_tmp9E1.tag=0;_tmp9E1.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp9C6);{
void*_tmp9DF[1]={& _tmp9E1};Cyc_fprintf(Cyc_stderr,({const char*_tmp9E0="%s::";
_tag_arr(_tmp9E0,sizeof(char),_get_zero_arr_size(_tmp9E0,5));}),_tag_arr(_tmp9DF,
sizeof(void*),1));}});goto _LL3DC;}_LL3EB:;_LL3EC:({void*_tmp9E2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9E3="?";_tag_arr(_tmp9E3,sizeof(char),
_get_zero_arr_size(_tmp9E3,2));}),_tag_arr(_tmp9E2,sizeof(void*),0));});goto
_LL3DC;_LL3DC:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmp9E4=_cycalloc(
sizeof(*_tmp9E4));_tmp9E4->v=Cyc_Lexing_from_file(f);_tmp9E4;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
