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
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,
struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr
f1;};extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;
struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;
int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct
_tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;
struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
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
struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern void*Cyc_Absyn_rel_ns_null;
int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
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
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct
Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts);struct _tagged_arr Cyc_Absyn_attribute2string(void*);int
Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);void Cyc_Lex_register_typedef(struct _tuple1*s);void
Cyc_Lex_enter_namespace(struct _tagged_arr*);void Cyc_Lex_leave_namespace();void
Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Core_Opt*Cyc_Parse_lbuf=0;static struct Cyc_List_List*Cyc_Parse_parse_result=
0;static void Cyc_Parse_err(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((
struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((
void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _tagged_arr msg,struct
Cyc_Position_Segment*sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{struct Cyc_String_pa_struct _tmp2;
_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
sg));{void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
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
void*)3?1:*((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A)->f1;_tmp1C=(void*)_tmp1B.elt_type;_tmp1D=_tmp1B.tq;_tmp1E=_tmp1B.num_elts;
_tmp1F=_tmp1B.zero_term;_LL2: return Cyc_Absyn_starb_typ(_tmp1C,argposn?Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(void*)((void*)3);
_tmp20;}),0):(void*)2,_tmp1D,_tmp1E == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp1E);_tmp22;});
_tmp21;}),_tmp1F);_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(
struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
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
loc,void*t){void*_tmp38=t;struct Cyc_Absyn_Tvar*_tmp39;_LL17: if(_tmp38 <= (void*)3?
1:*((int*)_tmp38)!= 1)goto _LL19;_tmp39=((struct Cyc_Absyn_VarType_struct*)_tmp38)->f1;
_LL18: return _tmp39;_LL19:;_LL1A:({void*_tmp3A[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp3B="expecting a list of type variables, not types";_tag_arr(_tmp3B,sizeof(
char),_get_zero_arr_size(_tmp3B,46));}),_tag_arr(_tmp3A,sizeof(void*),0));});
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({
struct Cyc_Absyn_VarType_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=pr;_tmp3D;});_tmp3C;});}
static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*_tmp3E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp3F;struct Cyc_Absyn_Tvar _tmp40;void*_tmp41;void**
_tmp42;_LL1C: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 1)goto _LL1E;_tmp3F=((
struct Cyc_Absyn_VarType_struct*)_tmp3E)->f1;_tmp40=*_tmp3F;_tmp41=(void*)_tmp40.kind;
_tmp42=(void**)&(*((struct Cyc_Absyn_VarType_struct*)_tmp3E)->f1).kind;_LL1D: {
void*_tmp43=Cyc_Absyn_compress_kb(*_tmp42);_LL21: if(*((int*)_tmp43)!= 1)goto
_LL23;_LL22:*_tmp42=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp44=_cycalloc(
sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp45;_tmp45.tag=0;
_tmp45.f1=(void*)k;_tmp45;});_tmp44;});return;_LL23:;_LL24: return;_LL20:;}_LL1E:;
_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp46=(void*)tms->hd;void*_tmp47;
_LL26: if(*((int*)_tmp46)!= 3)goto _LL28;_tmp47=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp46)->f1;_LL27: if(tms->tl == 0?1:(Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd)?((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0:
0)){void*_tmp48=_tmp47;struct Cyc_List_List*_tmp49;_LL2B: if(*((int*)_tmp48)!= 1)
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
void*)3?1:*((int*)_tmp74)!= 8)goto _LL3E;_tmp75=((struct Cyc_Absyn_FnType_struct*)
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
if(declopt == 0?!seen_type: 0){seen_type=1;{void*_tmpA1=(void*)_tmp99->r;struct Cyc_Absyn_Aggrdecl*
_tmpA2;struct Cyc_Absyn_Tuniondecl*_tmpA3;struct Cyc_Absyn_Enumdecl*_tmpA4;_LL53:
if(*((int*)_tmpA1)!= 4)goto _LL55;_tmpA2=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA1)->f1;
_LL54: {struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA2->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA7;_tmpA7.tag=10;_tmpA7.f1=({struct Cyc_Absyn_AggrInfo _tmpA8;_tmpA8.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpA9=_cycalloc(sizeof(*
_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpAA;_tmpAA.tag=0;
_tmpAA.f1=(void*)((void*)_tmpA2->kind);_tmpAA.f2=_tmpA2->name;_tmpAA;});_tmpA9;}));
_tmpA8.targs=_tmpA5;_tmpA8;});_tmpA7;});_tmpA6;});if(_tmpA2->impl != 0)declopt=({
struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->v=_tmp99;_tmpAB;});
goto _LL52;}_LL55: if(*((int*)_tmpA1)!= 5)goto _LL57;_tmpA3=((struct Cyc_Absyn_Tunion_d_struct*)
_tmpA1)->f1;_LL56: {struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA3->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=({struct Cyc_Absyn_TunionInfo
_tmpAF;_tmpAF.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB1;_tmpB1.tag=1;_tmpB1.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB2=_cycalloc(
sizeof(*_tmpB2));_tmpB2[0]=_tmpA3;_tmpB2;});_tmpB1;});_tmpB0;}));_tmpAF.targs=
_tmpAC;_tmpAF.rgn=(void*)((void*)2);_tmpAF;});_tmpAE;});_tmpAD;});if(_tmpA3->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->v=
_tmp99;_tmpB3;});goto _LL52;}_LL57: if(*((int*)_tmpA1)!= 6)goto _LL59;_tmpA4=((
struct Cyc_Absyn_Enum_d_struct*)_tmpA1)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_EnumType_struct
_tmpB5;_tmpB5.tag=12;_tmpB5.f1=_tmpA4->name;_tmpB5.f2=0;_tmpB5;});_tmpB4;});
declopt=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=_tmp99;
_tmpB6;});goto _LL52;_LL59:;_LL5A:({void*_tmpB7[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp99->loc,({
const char*_tmpB8="bad declaration within type specifier";_tag_arr(_tmpB8,sizeof(
char),_get_zero_arr_size(_tmpB8,38));}),_tag_arr(_tmpB7,sizeof(void*),0));});
_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp99->loc);}goto _LL40;_LL40:;}if(!
seen_type){if(!seen_sign?!seen_size: 0)({void*_tmpB9[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBA="missing type within specifier";_tag_arr(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,30));}),_tag_arr(_tmpB9,sizeof(void*),0));});
t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));
_tmpBB[0]=({struct Cyc_Absyn_IntType_struct _tmpBC;_tmpBC.tag=5;_tmpBC.f1=(void*)
sgn;_tmpBC.f2=(void*)sz;_tmpBC;});_tmpBB;});}else{if(seen_sign){void*_tmpBD=t;
void*_tmpBE;_LL5C: if(_tmpBD <= (void*)3?1:*((int*)_tmpBD)!= 5)goto _LL5E;_tmpBE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f2;_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_IntType_struct
_tmpC0;_tmpC0.tag=5;_tmpC0.f1=(void*)sgn;_tmpC0.f2=(void*)_tmpBE;_tmpC0;});
_tmpBF;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const char*_tmpC1="sign specification on non-integral type";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,40));}),last_loc);goto
_LL5B;_LL5B:;}if(seen_size){void*_tmpC2=t;void*_tmpC3;_LL61: if(_tmpC2 <= (void*)3?
1:*((int*)_tmpC2)!= 5)goto _LL63;_tmpC3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC2)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC4=_cycalloc(
sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_IntType_struct _tmpC5;_tmpC5.tag=5;
_tmpC5.f1=(void*)_tmpC3;_tmpC5.f2=(void*)sz;_tmpC5;});_tmpC4;});goto _LL60;_LL63:
if(_tmpC2 <= (void*)3?1:*((int*)_tmpC2)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpC6=_cycalloc_atomic(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_DoubleType_struct
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
_LL79:;}if(((((!_tmpE1?_tmpE2 == 0: 0)?_tmpE0 != 0: 0)?_tmpE0->tl == 0: 0)?(*((struct
_tuple2*)_tmpE0->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmpE0->hd)).f3 == (void*)0: 0)
_tmpE0=0;t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE0);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ(typvars,_tmpE3,t,_tmpE0,_tmpE1,_tmpE2,_tmpE4,fn_atts),
new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)_tmpDF)
!= 0)goto _LL74;_tmpE5=((struct Cyc_Absyn_NoTypes_struct*)_tmpDF)->f2;_LL78:({void*
_tmpEA[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmpE5,({const char*_tmpEB="function declaration without parameter types";
_tag_arr(_tmpEB,sizeof(char),_get_zero_arr_size(_tmpEB,45));}),_tag_arr(_tmpEA,
sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD4)!= 4)goto _LL70;_tmpD9=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD4)->f1;_tmpDA=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD4)->f2;_LL6F: if(tms->tl == 0)return({struct _tuple6 _tmpEC;_tmpEC.f1=tq;_tmpEC.f2=
t;_tmpEC.f3=_tmpD9;_tmpEC.f4=atts;_tmpEC;});({void*_tmpED[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmpDA,({const char*_tmpEE="type parameters must appear before function arguments in declarator";
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
Cyc_Lex_register_typedef(_tmpF9);if(_tmpFD != 0)Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp100;_tmp100.tag=0;_tmp100.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmpFD->hd));{void*_tmpFE[1]={&
_tmp100};Cyc_aprintf(({const char*_tmpFF="bad attribute %s within typedef";
_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,32));}),_tag_arr(_tmpFE,
sizeof(void*),1));}}),loc);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{
void*_tmp101=_tmpFB;struct Cyc_Core_Opt*_tmp102;_LL7F: if(_tmp101 <= (void*)3?1:*((
int*)_tmp101)!= 0)goto _LL81;_tmp102=((struct Cyc_Absyn_Evar_struct*)_tmp101)->f1;
_LL80: type=0;if(_tmp102 == 0)kind=({struct Cyc_Core_Opt*_tmp103=_cycalloc(sizeof(*
_tmp103));_tmp103->v=(void*)((void*)2);_tmp103;});else{kind=_tmp102;}goto _LL7E;
_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmp104=_cycalloc(sizeof(*_tmp104));
_tmp104->v=(void*)_tmpFB;_tmp104;});goto _LL7E;_LL7E:;}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp105=_cycalloc(sizeof(*_tmp105));
_tmp105[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp106;_tmp106.tag=7;_tmp106.f1=({
struct Cyc_Absyn_Typedefdecl*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->name=
_tmpF9;_tmp107->tvs=_tmpFC;_tmp107->kind=kind;_tmp107->defn=type;_tmp107;});
_tmp106;});_tmp105;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp109;_tmp109.tag=12;_tmp109.f1=d;_tmp109.f2=s;
_tmp109;});_tmp108;}),Cyc_Position_segment_join(d->loc,s->loc));}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(
Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10B;struct Cyc_Absyn_Tqual _tmp10C;struct
Cyc_List_List*_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Parse_Declaration_spec*
_tmp10A=ds;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.tq;_tmp10D=_tmp10B.type_specs;
_tmp10E=_tmp10B.attributes;if(ds->is_inline)Cyc_Parse_err(({const char*_tmp10F="inline is allowed only on function definitions";
_tag_arr(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,47));}),loc);if(_tmp10D
== 0){Cyc_Parse_err(({const char*_tmp110="missing type specifiers in declaration";
_tag_arr(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,39));}),loc);return 0;}{
void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp111=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp111 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp111 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp111 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp111 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp111 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp111 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp111 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp113;struct Cyc_List_List*
_tmp114;struct _tuple0 _tmp112=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmp113=_tmp112.f1;_tmp114=_tmp112.f2;{int exps_empty=1;{struct Cyc_List_List*
es=_tmp114;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple5 _tmp115=Cyc_Parse_collapse_type_specifiers(
_tmp10D,loc);if(_tmp113 == 0){void*_tmp117;struct Cyc_Core_Opt*_tmp118;struct
_tuple5 _tmp116=_tmp115;_tmp117=_tmp116.f1;_tmp118=_tmp116.f2;if(_tmp118 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp118->v;{void*_tmp119=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp11A;struct Cyc_Absyn_Aggrdecl*_tmp11B;struct Cyc_Absyn_Tuniondecl*
_tmp11C;_LL93: if(*((int*)_tmp119)!= 6)goto _LL95;_tmp11A=((struct Cyc_Absyn_Enum_d_struct*)
_tmp119)->f1;_LL94:(void*)(_tmp11A->sc=(void*)s);if(_tmp10E != 0)Cyc_Parse_err(({
const char*_tmp11D="bad attributes on enum";_tag_arr(_tmp11D,sizeof(char),
_get_zero_arr_size(_tmp11D,23));}),loc);goto _LL92;_LL95: if(*((int*)_tmp119)!= 4)
goto _LL97;_tmp11B=((struct Cyc_Absyn_Aggr_d_struct*)_tmp119)->f1;_LL96:(void*)(
_tmp11B->sc=(void*)s);_tmp11B->attributes=_tmp10E;goto _LL92;_LL97: if(*((int*)
_tmp119)!= 5)goto _LL99;_tmp11C=((struct Cyc_Absyn_Tunion_d_struct*)_tmp119)->f1;
_LL98:(void*)(_tmp11C->sc=(void*)s);if(_tmp10E != 0)Cyc_Parse_err(({const char*
_tmp11E="bad attributes on tunion";_tag_arr(_tmp11E,sizeof(char),
_get_zero_arr_size(_tmp11E,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp11F="bad declaration";_tag_arr(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,16));}),loc);return 0;_LL92:;}return({struct Cyc_List_List*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->hd=d;_tmp120->tl=0;_tmp120;});}
else{void*_tmp121=_tmp117;struct Cyc_Absyn_AggrInfo _tmp122;void*_tmp123;void*
_tmp124;struct _tuple1*_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_TunionInfo
_tmp127;void*_tmp128;struct Cyc_Absyn_Tuniondecl**_tmp129;struct Cyc_Absyn_TunionInfo
_tmp12A;void*_tmp12B;struct Cyc_Absyn_UnknownTunionInfo _tmp12C;struct _tuple1*
_tmp12D;int _tmp12E;struct Cyc_List_List*_tmp12F;struct _tuple1*_tmp130;struct Cyc_List_List*
_tmp131;_LL9C: if(_tmp121 <= (void*)3?1:*((int*)_tmp121)!= 10)goto _LL9E;_tmp122=((
struct Cyc_Absyn_AggrType_struct*)_tmp121)->f1;_tmp123=(void*)_tmp122.aggr_info;
if(*((int*)_tmp123)!= 0)goto _LL9E;_tmp124=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp123)->f1;_tmp125=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp123)->f2;_tmp126=
_tmp122.targs;_LL9D: {struct Cyc_List_List*_tmp132=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp126);
struct Cyc_Absyn_Aggrdecl*_tmp133=({struct Cyc_Absyn_Aggrdecl*_tmp138=_cycalloc(
sizeof(*_tmp138));_tmp138->kind=(void*)_tmp124;_tmp138->sc=(void*)s;_tmp138->name=
_tmp125;_tmp138->tvs=_tmp132;_tmp138->impl=0;_tmp138->attributes=0;_tmp138;});
if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp134="bad attributes on type declaration";
_tag_arr(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,35));}),loc);return({
struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp136=_cycalloc(sizeof(*_tmp136));
_tmp136[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp137;_tmp137.tag=4;_tmp137.f1=
_tmp133;_tmp137;});_tmp136;}),loc);_tmp135->tl=0;_tmp135;});}_LL9E: if(_tmp121 <= (
void*)3?1:*((int*)_tmp121)!= 2)goto _LLA0;_tmp127=((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1;_tmp128=(void*)_tmp127.tunion_info;if(*((int*)_tmp128)!= 1)goto
_LLA0;_tmp129=((struct Cyc_Absyn_KnownTunion_struct*)_tmp128)->f1;_LL9F: if(
_tmp10E != 0)Cyc_Parse_err(({const char*_tmp139="bad attributes on tunion";
_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,25));}),loc);return({
struct Cyc_List_List*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));
_tmp13B[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp13C;_tmp13C.tag=5;_tmp13C.f1=*
_tmp129;_tmp13C;});_tmp13B;}),loc);_tmp13A->tl=0;_tmp13A;});_LLA0: if(_tmp121 <= (
void*)3?1:*((int*)_tmp121)!= 2)goto _LLA2;_tmp12A=((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1;_tmp12B=(void*)_tmp12A.tunion_info;if(*((int*)_tmp12B)!= 0)goto
_LLA2;_tmp12C=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12B)->f1;_tmp12D=
_tmp12C.name;_tmp12E=_tmp12C.is_xtunion;_tmp12F=_tmp12A.targs;_LLA1: {struct Cyc_List_List*
_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12F);struct Cyc_Absyn_Decl*_tmp13E=Cyc_Absyn_tunion_decl(s,_tmp12D,
_tmp13D,0,_tmp12E,loc);if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp13F="bad attributes on tunion";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,25));}),loc);return({
struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=_tmp13E;
_tmp140->tl=0;_tmp140;});}_LLA2: if(_tmp121 <= (void*)3?1:*((int*)_tmp121)!= 12)
goto _LLA4;_tmp130=((struct Cyc_Absyn_EnumType_struct*)_tmp121)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp141=({struct Cyc_Absyn_Enumdecl*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->sc=(void*)s;_tmp147->name=_tmp130;_tmp147->fields=0;_tmp147;});
if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp142="bad attributes on enum";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,23));}),loc);return({
struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=({struct Cyc_Absyn_Decl*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp146;_tmp146.tag=6;_tmp146.f1=_tmp141;_tmp146;});_tmp145;}));_tmp144->loc=loc;
_tmp144;});_tmp143->tl=0;_tmp143;});}_LLA4: if(_tmp121 <= (void*)3?1:*((int*)
_tmp121)!= 13)goto _LLA6;_tmp131=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp121)->f1;
_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp148=({struct Cyc_Absyn_Enumdecl*_tmp14E=
_cycalloc(sizeof(*_tmp14E));_tmp14E->sc=(void*)s;_tmp14E->name=Cyc_Parse_gensym_enum();
_tmp14E->fields=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->v=
_tmp131;_tmp14F;});_tmp14E;});if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp149="bad attributes on enum";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,23));}),loc);return({
struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=({struct Cyc_Absyn_Decl*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14D;_tmp14D.tag=6;_tmp14D.f1=_tmp148;_tmp14D;});_tmp14C;}));_tmp14B->loc=loc;
_tmp14B;});_tmp14A->tl=0;_tmp14A;});}_LLA6:;_LLA7: Cyc_Parse_err(({const char*
_tmp150="missing declarator";_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(
_tmp150,19));}),loc);return 0;_LL9B:;}}else{void*t=_tmp115.f1;struct Cyc_List_List*
_tmp151=Cyc_Parse_apply_tmss(_tmp10C,t,_tmp113,_tmp10E);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp152="initializer in typedef declaration";
_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,35));}),loc);{struct Cyc_List_List*
decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp151);if(_tmp115.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp115.f2)->v;{void*_tmp153=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp154;struct Cyc_Absyn_Tuniondecl*_tmp155;struct Cyc_Absyn_Enumdecl*_tmp156;
_LLA9: if(*((int*)_tmp153)!= 4)goto _LLAB;_tmp154=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp153)->f1;_LLAA:(void*)(_tmp154->sc=(void*)s);_tmp154->attributes=_tmp10E;
_tmp10E=0;goto _LLA8;_LLAB: if(*((int*)_tmp153)!= 5)goto _LLAD;_tmp155=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp153)->f1;_LLAC:(void*)(_tmp155->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp153)!= 6)goto _LLAF;_tmp156=((struct Cyc_Absyn_Enum_d_struct*)_tmp153)->f1;
_LLAE:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp157="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,70));}),loc);goto _LLA8;
_LLA8:;}decls=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->hd=
d;_tmp158->tl=decls;_tmp158;});}if(_tmp10E != 0)Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp10E->hd));{void*_tmp159[
1]={& _tmp15B};Cyc_aprintf(({const char*_tmp15A="bad attribute %s in typedef";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,28));}),_tag_arr(
_tmp159,sizeof(void*),1));}}),loc);return decls;}}else{if(_tmp115.f2 != 0)Cyc_Parse_unimp(({
const char*_tmp15C="nested type declaration within declarator";_tag_arr(_tmp15C,
sizeof(char),_get_zero_arr_size(_tmp15C,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp15D=_tmp151;for(0;_tmp15D != 0;(_tmp15D=_tmp15D->tl,
_tmp114=_tmp114->tl)){struct _tuple7 _tmp15F;struct _tuple1*_tmp160;struct Cyc_Absyn_Tqual
_tmp161;void*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp164;
struct _tuple7*_tmp15E=(struct _tuple7*)_tmp15D->hd;_tmp15F=*_tmp15E;_tmp160=
_tmp15F.f1;_tmp161=_tmp15F.f2;_tmp162=_tmp15F.f3;_tmp163=_tmp15F.f4;_tmp164=
_tmp15F.f5;if(_tmp163 != 0)({void*_tmp165[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp166="bad type params, ignoring";_tag_arr(_tmp166,sizeof(char),
_get_zero_arr_size(_tmp166,26));}),_tag_arr(_tmp165,sizeof(void*),0));});if(
_tmp114 == 0)({void*_tmp167[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp168="unexpected NULL in parse!";
_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,26));}),_tag_arr(
_tmp167,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp169=(struct Cyc_Absyn_Exp*)
_tmp114->hd;struct Cyc_Absyn_Vardecl*_tmp16A=Cyc_Absyn_new_vardecl(_tmp160,
_tmp162,_tmp169);_tmp16A->tq=_tmp161;(void*)(_tmp16A->sc=(void*)s);_tmp16A->attributes=
_tmp164;{struct Cyc_Absyn_Decl*_tmp16B=({struct Cyc_Absyn_Decl*_tmp16D=_cycalloc(
sizeof(*_tmp16D));_tmp16D->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=_tmp16A;_tmp16F;});_tmp16E;}));_tmp16D->loc=loc;
_tmp16D;});decls=({struct Cyc_List_List*_tmp16C=_cycalloc(sizeof(*_tmp16C));
_tmp16C->hd=_tmp16B;_tmp16C->tl=decls;_tmp16C;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3:
return(void*)2;case 'R': _LLB4: return(void*)3;case 'E': _LLB5: return(void*)4;case 'I':
_LLB6: return(void*)5;default: _LLB7: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp170[1]={& _tmp172};Cyc_aprintf(({const char*_tmp171="bad kind: %s";
_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,13));}),_tag_arr(
_tmp170,sizeof(void*),1));}}),loc);return(void*)2;}char Cyc_AbstractDeclarator_tok[
27]="\000\000\000\000AbstractDeclarator_tok\000";struct Cyc_AbstractDeclarator_tok_struct{
char*tag;struct Cyc_Parse_Abstractdeclarator*f1;};char Cyc_AggrFieldDeclListList_tok[
30]="\000\000\000\000AggrFieldDeclListList_tok\000";struct Cyc_AggrFieldDeclListList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok\000";
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
static char _tmp174[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp174,_tmp174,_tmp174 + 15}};struct _tuple13*Cyc_yyget_Int_tok(
void*yy1){struct _tuple13*yyzzz;{void*_tmp175=yy1;struct _tuple13*_tmp176;_LLBA:
if(*((void**)_tmp175)!= Cyc_Int_tok)goto _LLBC;_tmp176=((struct Cyc_Int_tok_struct*)
_tmp175)->f1;_LLBB: yyzzz=_tmp176;goto _LLB9;_LLBC:;_LLBD:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLB9:;}return yyzzz;}static char _tmp178[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp178,_tmp178,_tmp178 + 5}};char Cyc_yyget_Char_tok(
void*yy1){char yyzzz;{void*_tmp179=yy1;char _tmp17A;_LLBF: if(*((void**)_tmp179)!= 
Cyc_Char_tok)goto _LLC1;_tmp17A=((struct Cyc_Char_tok_struct*)_tmp179)->f1;_LLC0:
yyzzz=_tmp17A;goto _LLBE;_LLC1:;_LLC2:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLBE:;}return yyzzz;}static char _tmp17C[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp17C,_tmp17C,_tmp17C + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp17D=
yy1;struct _tagged_arr _tmp17E;_LLC4: if(*((void**)_tmp17D)!= Cyc_String_tok)goto
_LLC6;_tmp17E=((struct Cyc_String_tok_struct*)_tmp17D)->f1;_LLC5: yyzzz=_tmp17E;
goto _LLC3;_LLC6:;_LLC7:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLC3:;}return
yyzzz;}static char _tmp180[38]="$(conref_t<bool>,conref_t<bounds_t>)@";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp180,_tmp180,
_tmp180 + 38}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*yyzzz;{void*
_tmp181=yy1;struct _tuple16*_tmp182;_LLC9: if(*((void**)_tmp181)!= Cyc_YY1)goto
_LLCB;_tmp182=((struct Cyc_YY1_struct*)_tmp181)->f1;_LLCA: yyzzz=_tmp182;goto _LLC8;
_LLCB:;_LLCC:(int)_throw((void*)& Cyc_yyfail_YY1);_LLC8:;}return yyzzz;}static char
_tmp184[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp184,_tmp184,_tmp184 + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(
void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp185=yy1;struct Cyc_Absyn_Conref*
_tmp186;_LLCE: if(*((void**)_tmp185)!= Cyc_YY2)goto _LLD0;_tmp186=((struct Cyc_YY2_struct*)
_tmp185)->f1;_LLCF: yyzzz=_tmp186;goto _LLCD;_LLD0:;_LLD1:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLCD:;}return yyzzz;}static char _tmp188[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp188,_tmp188,_tmp188 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp189=yy1;struct
Cyc_Absyn_Exp*_tmp18A;_LLD3: if(*((void**)_tmp189)!= Cyc_Exp_tok)goto _LLD5;
_tmp18A=((struct Cyc_Exp_tok_struct*)_tmp189)->f1;_LLD4: yyzzz=_tmp18A;goto _LLD2;
_LLD5:;_LLD6:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLD2:;}return yyzzz;}static
char _tmp18C[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp18C,_tmp18C,_tmp18C + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18D=yy1;struct Cyc_List_List*
_tmp18E;_LLD8: if(*((void**)_tmp18D)!= Cyc_ExpList_tok)goto _LLDA;_tmp18E=((struct
Cyc_ExpList_tok_struct*)_tmp18D)->f1;_LLD9: yyzzz=_tmp18E;goto _LLD7;_LLDA:;_LLDB:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD7:;}return yyzzz;}static char
_tmp190[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp190,_tmp190,_tmp190 + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp191=yy1;struct Cyc_List_List*_tmp192;_LLDD: if(*((void**)_tmp191)
!= Cyc_InitializerList_tok)goto _LLDF;_tmp192=((struct Cyc_InitializerList_tok_struct*)
_tmp191)->f1;_LLDE: yyzzz=_tmp192;goto _LLDC;_LLDF:;_LLE0:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLDC:;}return yyzzz;}static char _tmp194[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp194,_tmp194,_tmp194 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp195=yy1;void*_tmp196;_LLE2: if(*((void**)_tmp195)
!= Cyc_Primop_tok)goto _LLE4;_tmp196=(void*)((struct Cyc_Primop_tok_struct*)
_tmp195)->f1;_LLE3: yyzzz=_tmp196;goto _LLE1;_LLE4:;_LLE5:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLE1:;}return yyzzz;}static char _tmp198[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp199=yy1;struct Cyc_Core_Opt*_tmp19A;_LLE7: if(*((void**)_tmp199)!= Cyc_Primopopt_tok)
goto _LLE9;_tmp19A=((struct Cyc_Primopopt_tok_struct*)_tmp199)->f1;_LLE8: yyzzz=
_tmp19A;goto _LLE6;_LLE9:;_LLEA:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE6:;}return yyzzz;}static char _tmp19C[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp19C,_tmp19C,_tmp19C + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp19D=yy1;
struct _tuple1*_tmp19E;_LLEC: if(*((void**)_tmp19D)!= Cyc_QualId_tok)goto _LLEE;
_tmp19E=((struct Cyc_QualId_tok_struct*)_tmp19D)->f1;_LLED: yyzzz=_tmp19E;goto
_LLEB;_LLEE:;_LLEF:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLEB:;}return
yyzzz;}static char _tmp1A0[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1A0,_tmp1A0,_tmp1A0 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1A1=yy1;struct Cyc_Absyn_Stmt*
_tmp1A2;_LLF1: if(*((void**)_tmp1A1)!= Cyc_Stmt_tok)goto _LLF3;_tmp1A2=((struct Cyc_Stmt_tok_struct*)
_tmp1A1)->f1;_LLF2: yyzzz=_tmp1A2;goto _LLF0;_LLF3:;_LLF4:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF0:;}return yyzzz;}static char _tmp1A4[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp1A4,_tmp1A4,_tmp1A4 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A5=yy1;struct Cyc_List_List*
_tmp1A6;_LLF6: if(*((void**)_tmp1A5)!= Cyc_SwitchClauseList_tok)goto _LLF8;_tmp1A6=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp1A5)->f1;_LLF7: yyzzz=_tmp1A6;goto
_LLF5;_LLF8:;_LLF9:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF5:;}
return yyzzz;}static char _tmp1A8[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8,_tmp1A8 + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1A9=yy1;struct Cyc_List_List*_tmp1AA;_LLFB: if(*((void**)_tmp1A9)
!= Cyc_SwitchCClauseList_tok)goto _LLFD;_tmp1AA=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp1A9)->f1;_LLFC: yyzzz=_tmp1AA;goto _LLFA;_LLFD:;_LLFE:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFA:;}return yyzzz;}static char _tmp1AC[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1AC,_tmp1AC,_tmp1AC + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1AD=yy1;
struct Cyc_Absyn_Pat*_tmp1AE;_LL100: if(*((void**)_tmp1AD)!= Cyc_Pattern_tok)goto
_LL102;_tmp1AE=((struct Cyc_Pattern_tok_struct*)_tmp1AD)->f1;_LL101: yyzzz=_tmp1AE;
goto _LLFF;_LL102:;_LL103:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LLFF:;}
return yyzzz;}static char _tmp1B0[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1B1=yy1;struct Cyc_List_List*_tmp1B2;_LL105: if(*((void**)_tmp1B1)
!= Cyc_PatternList_tok)goto _LL107;_tmp1B2=((struct Cyc_PatternList_tok_struct*)
_tmp1B1)->f1;_LL106: yyzzz=_tmp1B2;goto _LL104;_LL107:;_LL108:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL104:;}return yyzzz;}static char _tmp1B4[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1B4,_tmp1B4,_tmp1B4 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1B5=yy1;struct _tuple11*_tmp1B6;_LL10A: if(*((void**)
_tmp1B5)!= Cyc_FieldPattern_tok)goto _LL10C;_tmp1B6=((struct Cyc_FieldPattern_tok_struct*)
_tmp1B5)->f1;_LL10B: yyzzz=_tmp1B6;goto _LL109;_LL10C:;_LL10D:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL109:;}return yyzzz;}static char _tmp1B8[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1B8,_tmp1B8,_tmp1B8 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B9=yy1;struct Cyc_List_List*
_tmp1BA;_LL10F: if(*((void**)_tmp1B9)!= Cyc_FieldPatternList_tok)goto _LL111;
_tmp1BA=((struct Cyc_FieldPatternList_tok_struct*)_tmp1B9)->f1;_LL110: yyzzz=
_tmp1BA;goto _LL10E;_LL111:;_LL112:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL10E:;}return yyzzz;}static char _tmp1BC[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1BD=yy1;
struct Cyc_Absyn_Fndecl*_tmp1BE;_LL114: if(*((void**)_tmp1BD)!= Cyc_FnDecl_tok)
goto _LL116;_tmp1BE=((struct Cyc_FnDecl_tok_struct*)_tmp1BD)->f1;_LL115: yyzzz=
_tmp1BE;goto _LL113;_LL116:;_LL117:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL113:;}return yyzzz;}static char _tmp1C0[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1C0,_tmp1C0,_tmp1C0 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1C1=yy1;struct Cyc_List_List*_tmp1C2;_LL119: if(*((void**)_tmp1C1)!= Cyc_DeclList_tok)
goto _LL11B;_tmp1C2=((struct Cyc_DeclList_tok_struct*)_tmp1C1)->f1;_LL11A: yyzzz=
_tmp1C2;goto _LL118;_LL11B:;_LL11C:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL118:;}return yyzzz;}static char _tmp1C4[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1C4,_tmp1C4,_tmp1C4 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1C5=yy1;struct Cyc_Parse_Declaration_spec*_tmp1C6;_LL11E: if(*((
void**)_tmp1C5)!= Cyc_DeclSpec_tok)goto _LL120;_tmp1C6=((struct Cyc_DeclSpec_tok_struct*)
_tmp1C5)->f1;_LL11F: yyzzz=_tmp1C6;goto _LL11D;_LL120:;_LL121:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11D:;}return yyzzz;}static char _tmp1C8[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1C8,_tmp1C8,_tmp1C8 + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1C9=yy1;struct _tuple12*_tmp1CA;_LL123: if(*((void**)
_tmp1C9)!= Cyc_InitDecl_tok)goto _LL125;_tmp1CA=((struct Cyc_InitDecl_tok_struct*)
_tmp1C9)->f1;_LL124: yyzzz=_tmp1CA;goto _LL122;_LL125:;_LL126:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL122:;}return yyzzz;}static char _tmp1CC[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1CC,_tmp1CC,_tmp1CC + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CD=yy1;struct Cyc_List_List*
_tmp1CE;_LL128: if(*((void**)_tmp1CD)!= Cyc_InitDeclList_tok)goto _LL12A;_tmp1CE=((
struct Cyc_InitDeclList_tok_struct*)_tmp1CD)->f1;_LL129: yyzzz=_tmp1CE;goto _LL127;
_LL12A:;_LL12B:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL127:;}return
yyzzz;}static char _tmp1D0[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1D0,_tmp1D0,_tmp1D0 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1D1=yy1;void*
_tmp1D2;_LL12D: if(*((void**)_tmp1D1)!= Cyc_StorageClass_tok)goto _LL12F;_tmp1D2=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1D1)->f1;_LL12E: yyzzz=_tmp1D2;
goto _LL12C;_LL12F:;_LL130:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12C:;}return yyzzz;}static char _tmp1D4[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1D5=yy1;void*
_tmp1D6;_LL132: if(*((void**)_tmp1D5)!= Cyc_TypeSpecifier_tok)goto _LL134;_tmp1D6=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1D5)->f1;_LL133: yyzzz=_tmp1D6;
goto _LL131;_LL134:;_LL135:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL131:;}return yyzzz;}static char _tmp1D8[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1D8,_tmp1D8,_tmp1D8 + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1D9=yy1;void*
_tmp1DA;_LL137: if(*((void**)_tmp1D9)!= Cyc_StructOrUnion_tok)goto _LL139;_tmp1DA=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1D9)->f1;_LL138: yyzzz=_tmp1DA;
goto _LL136;_LL139:;_LL13A:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL136:;}return yyzzz;}static char _tmp1DC[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1DC,_tmp1DC,_tmp1DC + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1DD=yy1;struct Cyc_Absyn_Tqual _tmp1DE;_LL13C: if(*((void**)_tmp1DD)!= Cyc_TypeQual_tok)
goto _LL13E;_tmp1DE=((struct Cyc_TypeQual_tok_struct*)_tmp1DD)->f1;_LL13D: yyzzz=
_tmp1DE;goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13B:;}return yyzzz;}static char _tmp1E0[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1E0,_tmp1E0,_tmp1E0 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E1=yy1;struct Cyc_List_List*
_tmp1E2;_LL141: if(*((void**)_tmp1E1)!= Cyc_AggrFieldDeclList_tok)goto _LL143;
_tmp1E2=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1E1)->f1;_LL142: yyzzz=
_tmp1E2;goto _LL140;_LL143:;_LL144:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL140:;}return yyzzz;}static char _tmp1E4[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1E4,_tmp1E4,_tmp1E4 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E5=yy1;struct Cyc_List_List*
_tmp1E6;_LL146: if(*((void**)_tmp1E5)!= Cyc_AggrFieldDeclListList_tok)goto _LL148;
_tmp1E6=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1E5)->f1;_LL147: yyzzz=
_tmp1E6;goto _LL145;_LL148:;_LL149:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL145:;}return yyzzz;}static char _tmp1E8[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1E8,_tmp1E8,_tmp1E8 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E9=yy1;struct Cyc_List_List*
_tmp1EA;_LL14B: if(*((void**)_tmp1E9)!= Cyc_TypeModifierList_tok)goto _LL14D;
_tmp1EA=((struct Cyc_TypeModifierList_tok_struct*)_tmp1E9)->f1;_LL14C: yyzzz=
_tmp1EA;goto _LL14A;_LL14D:;_LL14E:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14A:;}return yyzzz;}static char _tmp1EC[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1ED=yy1;struct Cyc_Parse_Declarator*_tmp1EE;_LL150: if(*((void**)
_tmp1ED)!= Cyc_Declarator_tok)goto _LL152;_tmp1EE=((struct Cyc_Declarator_tok_struct*)
_tmp1ED)->f1;_LL151: yyzzz=_tmp1EE;goto _LL14F;_LL152:;_LL153:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL14F:;}return yyzzz;}static char _tmp1F0[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1F0,_tmp1F0,_tmp1F0 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1F1=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1F2;_LL155: if(*((void**)_tmp1F1)!= Cyc_AbstractDeclarator_tok)goto _LL157;
_tmp1F2=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1F1)->f1;_LL156: yyzzz=
_tmp1F2;goto _LL154;_LL157:;_LL158:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL154:;}return yyzzz;}static char _tmp1F4[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1F4,_tmp1F4,_tmp1F4 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1F5=yy1;int _tmp1F6;_LL15A: if(*((void**)_tmp1F5)!= 
Cyc_Bool_tok)goto _LL15C;_tmp1F6=((struct Cyc_Bool_tok_struct*)_tmp1F5)->f1;_LL15B:
yyzzz=_tmp1F6;goto _LL159;_LL15C:;_LL15D:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL159:;}return yyzzz;}static char _tmp1F8[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1F9=yy1;void*_tmp1FA;_LL15F: if(*((void**)_tmp1F9)
!= Cyc_Scope_tok)goto _LL161;_tmp1FA=(void*)((struct Cyc_Scope_tok_struct*)_tmp1F9)->f1;
_LL160: yyzzz=_tmp1FA;goto _LL15E;_LL161:;_LL162:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL15E:;}return yyzzz;}static char _tmp1FC[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp1FD=yy1;struct Cyc_Absyn_Tunionfield*_tmp1FE;_LL164: if(*((void**)
_tmp1FD)!= Cyc_TunionField_tok)goto _LL166;_tmp1FE=((struct Cyc_TunionField_tok_struct*)
_tmp1FD)->f1;_LL165: yyzzz=_tmp1FE;goto _LL163;_LL166:;_LL167:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL163:;}return yyzzz;}static char _tmp200[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp200,_tmp200,_tmp200 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp201=yy1;struct Cyc_List_List*
_tmp202;_LL169: if(*((void**)_tmp201)!= Cyc_TunionFieldList_tok)goto _LL16B;
_tmp202=((struct Cyc_TunionFieldList_tok_struct*)_tmp201)->f1;_LL16A: yyzzz=
_tmp202;goto _LL168;_LL16B:;_LL16C:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL168:;}return yyzzz;}static char _tmp204[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp204,_tmp204,_tmp204 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp205=yy1;struct _tuple15*_tmp206;_LL16E: if(*((void**)
_tmp205)!= Cyc_QualSpecList_tok)goto _LL170;_tmp206=((struct Cyc_QualSpecList_tok_struct*)
_tmp205)->f1;_LL16F: yyzzz=_tmp206;goto _LL16D;_LL170:;_LL171:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16D:;}return yyzzz;}static char _tmp208[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp208,_tmp208,_tmp208 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp209=yy1;struct Cyc_List_List*_tmp20A;_LL173:
if(*((void**)_tmp209)!= Cyc_IdList_tok)goto _LL175;_tmp20A=((struct Cyc_IdList_tok_struct*)
_tmp209)->f1;_LL174: yyzzz=_tmp20A;goto _LL172;_LL175:;_LL176:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL172:;}return yyzzz;}static char _tmp20C[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp20C,_tmp20C,_tmp20C + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp20D=yy1;struct _tuple2*_tmp20E;_LL178: if(*((void**)
_tmp20D)!= Cyc_ParamDecl_tok)goto _LL17A;_tmp20E=((struct Cyc_ParamDecl_tok_struct*)
_tmp20D)->f1;_LL179: yyzzz=_tmp20E;goto _LL177;_LL17A:;_LL17B:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL177:;}return yyzzz;}static char _tmp210[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp210,_tmp210,_tmp210 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp211=yy1;struct Cyc_List_List*
_tmp212;_LL17D: if(*((void**)_tmp211)!= Cyc_ParamDeclList_tok)goto _LL17F;_tmp212=((
struct Cyc_ParamDeclList_tok_struct*)_tmp211)->f1;_LL17E: yyzzz=_tmp212;goto _LL17C;
_LL17F:;_LL180:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17C:;}return
yyzzz;}static char _tmp214[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp214,
_tmp214,_tmp214 + 107}};struct _tuple14*Cyc_yyget_YY3(void*yy1){struct _tuple14*
yyzzz;{void*_tmp215=yy1;struct _tuple14*_tmp216;_LL182: if(*((void**)_tmp215)!= 
Cyc_YY3)goto _LL184;_tmp216=((struct Cyc_YY3_struct*)_tmp215)->f1;_LL183: yyzzz=
_tmp216;goto _LL181;_LL184:;_LL185:(int)_throw((void*)& Cyc_yyfail_YY3);_LL181:;}
return yyzzz;}static char _tmp218[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp218,_tmp218,_tmp218 + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp219=yy1;struct Cyc_List_List*_tmp21A;_LL187: if(*((void**)_tmp219)!= Cyc_TypeList_tok)
goto _LL189;_tmp21A=((struct Cyc_TypeList_tok_struct*)_tmp219)->f1;_LL188: yyzzz=
_tmp21A;goto _LL186;_LL189:;_LL18A:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL186:;}return yyzzz;}static char _tmp21C[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp21C,
_tmp21C,_tmp21C + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp21D=yy1;struct Cyc_List_List*_tmp21E;_LL18C:
if(*((void**)_tmp21D)!= Cyc_DesignatorList_tok)goto _LL18E;_tmp21E=((struct Cyc_DesignatorList_tok_struct*)
_tmp21D)->f1;_LL18D: yyzzz=_tmp21E;goto _LL18B;_LL18E:;_LL18F:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18B:;}return yyzzz;}static char _tmp220[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp220,_tmp220,_tmp220 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp221=yy1;void*_tmp222;_LL191: if(*((void**)_tmp221)!= Cyc_Designator_tok)
goto _LL193;_tmp222=(void*)((struct Cyc_Designator_tok_struct*)_tmp221)->f1;_LL192:
yyzzz=_tmp222;goto _LL190;_LL193:;_LL194:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL190:;}return yyzzz;}static char _tmp224[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp225=yy1;void*_tmp226;_LL196: if(*((void**)_tmp225)
!= Cyc_Kind_tok)goto _LL198;_tmp226=(void*)((struct Cyc_Kind_tok_struct*)_tmp225)->f1;
_LL197: yyzzz=_tmp226;goto _LL195;_LL198:;_LL199:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL195:;}return yyzzz;}static char _tmp228[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp229=yy1;void*_tmp22A;_LL19B: if(*((void**)_tmp229)
!= Cyc_Type_tok)goto _LL19D;_tmp22A=(void*)((struct Cyc_Type_tok_struct*)_tmp229)->f1;
_LL19C: yyzzz=_tmp22A;goto _LL19A;_LL19D:;_LL19E:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19A:;}return yyzzz;}static char _tmp22C[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp22C,
_tmp22C,_tmp22C + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp22D=yy1;struct Cyc_List_List*_tmp22E;_LL1A0:
if(*((void**)_tmp22D)!= Cyc_AttributeList_tok)goto _LL1A2;_tmp22E=((struct Cyc_AttributeList_tok_struct*)
_tmp22D)->f1;_LL1A1: yyzzz=_tmp22E;goto _LL19F;_LL1A2:;_LL1A3:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL19F:;}return yyzzz;}static char _tmp230[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp230,_tmp230,_tmp230 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp231=yy1;void*_tmp232;_LL1A5: if(*((void**)_tmp231)!= Cyc_Attribute_tok)
goto _LL1A7;_tmp232=(void*)((struct Cyc_Attribute_tok_struct*)_tmp231)->f1;_LL1A6:
yyzzz=_tmp232;goto _LL1A4;_LL1A7:;_LL1A8:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A4:;}return yyzzz;}static char _tmp234[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp235=yy1;struct Cyc_Absyn_Enumfield*_tmp236;_LL1AA: if(*((void**)
_tmp235)!= Cyc_Enumfield_tok)goto _LL1AC;_tmp236=((struct Cyc_Enumfield_tok_struct*)
_tmp235)->f1;_LL1AB: yyzzz=_tmp236;goto _LL1A9;_LL1AC:;_LL1AD:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1A9:;}return yyzzz;}static char _tmp238[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp238,_tmp238,_tmp238 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp239=yy1;struct Cyc_List_List*
_tmp23A;_LL1AF: if(*((void**)_tmp239)!= Cyc_EnumfieldList_tok)goto _LL1B1;_tmp23A=((
struct Cyc_EnumfieldList_tok_struct*)_tmp239)->f1;_LL1B0: yyzzz=_tmp23A;goto _LL1AE;
_LL1B1:;_LL1B2:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1AE:;}return
yyzzz;}static char _tmp23C[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp23C,_tmp23C,_tmp23C + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp23D=yy1;struct Cyc_Core_Opt*_tmp23E;_LL1B4: if(*((void**)_tmp23D)!= Cyc_TypeOpt_tok)
goto _LL1B6;_tmp23E=((struct Cyc_TypeOpt_tok_struct*)_tmp23D)->f1;_LL1B5: yyzzz=
_tmp23E;goto _LL1B3;_LL1B6:;_LL1B7:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B3:;}return yyzzz;}static char _tmp240[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp240,
_tmp240,_tmp240 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp241=yy1;struct Cyc_List_List*_tmp242;_LL1B9:
if(*((void**)_tmp241)!= Cyc_Rgnorder_tok)goto _LL1BB;_tmp242=((struct Cyc_Rgnorder_tok_struct*)
_tmp241)->f1;_LL1BA: yyzzz=_tmp242;goto _LL1B8;_LL1BB:;_LL1BC:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1B8:;}return yyzzz;}static char _tmp244[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp244,
_tmp244,_tmp244 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY4(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp245=yy1;struct Cyc_Absyn_Conref*_tmp246;_LL1BE: if(*((void**)
_tmp245)!= Cyc_YY4)goto _LL1C0;_tmp246=((struct Cyc_YY4_struct*)_tmp245)->f1;
_LL1BF: yyzzz=_tmp246;goto _LL1BD;_LL1C0:;_LL1C1:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL1BD:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp247;_tmp247.timestamp=0;_tmp247.first_line=0;
_tmp247.first_column=0;_tmp247.last_line=0;_tmp247.last_column=0;_tmp247;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[359]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,130,2,2,112,128,124,2,
108,109,118,122,110,123,115,127,2,2,2,2,2,2,2,2,2,2,114,104,113,107,121,120,119,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,116,2,117,126,111,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,105,125,106,129,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp248[
2]="$";static char _tmp249[6]="error";static char _tmp24A[12]="$undefined.";static
char _tmp24B[5]="AUTO";static char _tmp24C[9]="REGISTER";static char _tmp24D[7]="STATIC";
static char _tmp24E[7]="EXTERN";static char _tmp24F[8]="TYPEDEF";static char _tmp250[5]="VOID";
static char _tmp251[5]="CHAR";static char _tmp252[6]="SHORT";static char _tmp253[4]="INT";
static char _tmp254[5]="LONG";static char _tmp255[6]="FLOAT";static char _tmp256[7]="DOUBLE";
static char _tmp257[7]="SIGNED";static char _tmp258[9]="UNSIGNED";static char _tmp259[
6]="CONST";static char _tmp25A[9]="VOLATILE";static char _tmp25B[9]="RESTRICT";
static char _tmp25C[7]="STRUCT";static char _tmp25D[6]="UNION";static char _tmp25E[5]="CASE";
static char _tmp25F[8]="DEFAULT";static char _tmp260[7]="INLINE";static char _tmp261[7]="SIZEOF";
static char _tmp262[9]="OFFSETOF";static char _tmp263[3]="IF";static char _tmp264[5]="ELSE";
static char _tmp265[7]="SWITCH";static char _tmp266[6]="WHILE";static char _tmp267[3]="DO";
static char _tmp268[4]="FOR";static char _tmp269[5]="GOTO";static char _tmp26A[9]="CONTINUE";
static char _tmp26B[6]="BREAK";static char _tmp26C[7]="RETURN";static char _tmp26D[5]="ENUM";
static char _tmp26E[8]="NULL_kw";static char _tmp26F[4]="LET";static char _tmp270[6]="THROW";
static char _tmp271[4]="TRY";static char _tmp272[6]="CATCH";static char _tmp273[4]="NEW";
static char _tmp274[9]="ABSTRACT";static char _tmp275[9]="FALLTHRU";static char
_tmp276[6]="USING";static char _tmp277[10]="NAMESPACE";static char _tmp278[7]="TUNION";
static char _tmp279[8]="XTUNION";static char _tmp27A[9]="FORARRAY";static char _tmp27B[
5]="FILL";static char _tmp27C[8]="CODEGEN";static char _tmp27D[4]="CUT";static char
_tmp27E[7]="SPLICE";static char _tmp27F[7]="MALLOC";static char _tmp280[8]="RMALLOC";
static char _tmp281[7]="CALLOC";static char _tmp282[8]="RCALLOC";static char _tmp283[9]="REGION_T";
static char _tmp284[9]="SIZEOF_T";static char _tmp285[6]="TAG_T";static char _tmp286[7]="REGION";
static char _tmp287[5]="RNEW";static char _tmp288[8]="REGIONS";static char _tmp289[13]="RESET_REGION";
static char _tmp28A[4]="GEN";static char _tmp28B[14]="NOZEROTERM_kw";static char
_tmp28C[12]="ZEROTERM_kw";static char _tmp28D[7]="PTR_OP";static char _tmp28E[7]="INC_OP";
static char _tmp28F[7]="DEC_OP";static char _tmp290[8]="LEFT_OP";static char _tmp291[9]="RIGHT_OP";
static char _tmp292[6]="LE_OP";static char _tmp293[6]="GE_OP";static char _tmp294[6]="EQ_OP";
static char _tmp295[6]="NE_OP";static char _tmp296[7]="AND_OP";static char _tmp297[6]="OR_OP";
static char _tmp298[11]="MUL_ASSIGN";static char _tmp299[11]="DIV_ASSIGN";static char
_tmp29A[11]="MOD_ASSIGN";static char _tmp29B[11]="ADD_ASSIGN";static char _tmp29C[11]="SUB_ASSIGN";
static char _tmp29D[12]="LEFT_ASSIGN";static char _tmp29E[13]="RIGHT_ASSIGN";static
char _tmp29F[11]="AND_ASSIGN";static char _tmp2A0[11]="XOR_ASSIGN";static char
_tmp2A1[10]="OR_ASSIGN";static char _tmp2A2[9]="ELLIPSIS";static char _tmp2A3[11]="LEFT_RIGHT";
static char _tmp2A4[12]="COLON_COLON";static char _tmp2A5[11]="IDENTIFIER";static
char _tmp2A6[17]="INTEGER_CONSTANT";static char _tmp2A7[7]="STRING";static char
_tmp2A8[19]="CHARACTER_CONSTANT";static char _tmp2A9[18]="FLOATING_CONSTANT";
static char _tmp2AA[9]="TYPE_VAR";static char _tmp2AB[13]="TYPEDEF_NAME";static char
_tmp2AC[16]="QUAL_IDENTIFIER";static char _tmp2AD[18]="QUAL_TYPEDEF_NAME";static
char _tmp2AE[13]="TYPE_INTEGER";static char _tmp2AF[10]="ATTRIBUTE";static char
_tmp2B0[4]="';'";static char _tmp2B1[4]="'{'";static char _tmp2B2[4]="'}'";static
char _tmp2B3[4]="'='";static char _tmp2B4[4]="'('";static char _tmp2B5[4]="')'";
static char _tmp2B6[4]="','";static char _tmp2B7[4]="'_'";static char _tmp2B8[4]="'$'";
static char _tmp2B9[4]="'<'";static char _tmp2BA[4]="':'";static char _tmp2BB[4]="'.'";
static char _tmp2BC[4]="'['";static char _tmp2BD[4]="']'";static char _tmp2BE[4]="'*'";
static char _tmp2BF[4]="'@'";static char _tmp2C0[4]="'?'";static char _tmp2C1[4]="'>'";
static char _tmp2C2[4]="'+'";static char _tmp2C3[4]="'-'";static char _tmp2C4[4]="'&'";
static char _tmp2C5[4]="'|'";static char _tmp2C6[4]="'^'";static char _tmp2C7[4]="'/'";
static char _tmp2C8[4]="'%'";static char _tmp2C9[4]="'~'";static char _tmp2CA[4]="'!'";
static char _tmp2CB[5]="prog";static char _tmp2CC[17]="translation_unit";static char
_tmp2CD[21]="external_declaration";static char _tmp2CE[20]="function_definition";
static char _tmp2CF[21]="function_definition2";static char _tmp2D0[13]="using_action";
static char _tmp2D1[15]="unusing_action";static char _tmp2D2[17]="namespace_action";
static char _tmp2D3[19]="unnamespace_action";static char _tmp2D4[12]="declaration";
static char _tmp2D5[17]="declaration_list";static char _tmp2D6[23]="declaration_specifiers";
static char _tmp2D7[24]="storage_class_specifier";static char _tmp2D8[15]="attributes_opt";
static char _tmp2D9[11]="attributes";static char _tmp2DA[15]="attribute_list";static
char _tmp2DB[10]="attribute";static char _tmp2DC[15]="type_specifier";static char
_tmp2DD[5]="kind";static char _tmp2DE[15]="type_qualifier";static char _tmp2DF[15]="enum_specifier";
static char _tmp2E0[11]="enum_field";static char _tmp2E1[22]="enum_declaration_list";
static char _tmp2E2[26]="struct_or_union_specifier";static char _tmp2E3[16]="type_params_opt";
static char _tmp2E4[16]="struct_or_union";static char _tmp2E5[24]="struct_declaration_list";
static char _tmp2E6[25]="struct_declaration_list0";static char _tmp2E7[21]="init_declarator_list";
static char _tmp2E8[22]="init_declarator_list0";static char _tmp2E9[16]="init_declarator";
static char _tmp2EA[19]="struct_declaration";static char _tmp2EB[25]="specifier_qualifier_list";
static char _tmp2EC[23]="struct_declarator_list";static char _tmp2ED[24]="struct_declarator_list0";
static char _tmp2EE[18]="struct_declarator";static char _tmp2EF[17]="tunion_specifier";
static char _tmp2F0[18]="tunion_or_xtunion";static char _tmp2F1[17]="tunionfield_list";
static char _tmp2F2[18]="tunionfield_scope";static char _tmp2F3[12]="tunionfield";
static char _tmp2F4[11]="declarator";static char _tmp2F5[18]="direct_declarator";
static char _tmp2F6[8]="pointer";static char _tmp2F7[12]="one_pointer";static char
_tmp2F8[23]="pointer_null_and_bound";static char _tmp2F9[14]="pointer_bound";
static char _tmp2FA[18]="zeroterm_qual_opt";static char _tmp2FB[8]="rgn_opt";static
char _tmp2FC[11]="tqual_list";static char _tmp2FD[20]="parameter_type_list";static
char _tmp2FE[9]="type_var";static char _tmp2FF[16]="optional_effect";static char
_tmp300[19]="optional_rgn_order";static char _tmp301[10]="rgn_order";static char
_tmp302[16]="optional_inject";static char _tmp303[11]="effect_set";static char
_tmp304[14]="atomic_effect";static char _tmp305[11]="region_set";static char _tmp306[
15]="parameter_list";static char _tmp307[22]="parameter_declaration";static char
_tmp308[16]="identifier_list";static char _tmp309[17]="identifier_list0";static
char _tmp30A[12]="initializer";static char _tmp30B[18]="array_initializer";static
char _tmp30C[17]="initializer_list";static char _tmp30D[12]="designation";static
char _tmp30E[16]="designator_list";static char _tmp30F[11]="designator";static char
_tmp310[10]="type_name";static char _tmp311[14]="any_type_name";static char _tmp312[
15]="type_name_list";static char _tmp313[20]="abstract_declarator";static char
_tmp314[27]="direct_abstract_declarator";static char _tmp315[10]="statement";
static char _tmp316[18]="labeled_statement";static char _tmp317[21]="expression_statement";
static char _tmp318[19]="compound_statement";static char _tmp319[16]="block_item_list";
static char _tmp31A[20]="selection_statement";static char _tmp31B[15]="switch_clauses";
static char _tmp31C[16]="switchC_clauses";static char _tmp31D[20]="iteration_statement";
static char _tmp31E[15]="jump_statement";static char _tmp31F[8]="pattern";static char
_tmp320[19]="tuple_pattern_list";static char _tmp321[14]="field_pattern";static
char _tmp322[19]="field_pattern_list";static char _tmp323[11]="expression";static
char _tmp324[22]="assignment_expression";static char _tmp325[20]="assignment_operator";
static char _tmp326[23]="conditional_expression";static char _tmp327[20]="constant_expression";
static char _tmp328[22]="logical_or_expression";static char _tmp329[23]="logical_and_expression";
static char _tmp32A[24]="inclusive_or_expression";static char _tmp32B[24]="exclusive_or_expression";
static char _tmp32C[15]="and_expression";static char _tmp32D[20]="equality_expression";
static char _tmp32E[22]="relational_expression";static char _tmp32F[17]="shift_expression";
static char _tmp330[20]="additive_expression";static char _tmp331[26]="multiplicative_expression";
static char _tmp332[16]="cast_expression";static char _tmp333[17]="unary_expression";
static char _tmp334[15]="unary_operator";static char _tmp335[19]="postfix_expression";
static char _tmp336[19]="primary_expression";static char _tmp337[25]="argument_expression_list";
static char _tmp338[26]="argument_expression_list0";static char _tmp339[9]="constant";
static char _tmp33A[20]="qual_opt_identifier";static char _tmp33B[17]="qual_opt_typedef";
static char _tmp33C[18]="struct_union_name";static char _tmp33D[11]="field_name";
static char _tmp33E[12]="right_angle";static struct _tagged_arr Cyc_yytname[247]={{
_tmp248,_tmp248,_tmp248 + 2},{_tmp249,_tmp249,_tmp249 + 6},{_tmp24A,_tmp24A,
_tmp24A + 12},{_tmp24B,_tmp24B,_tmp24B + 5},{_tmp24C,_tmp24C,_tmp24C + 9},{_tmp24D,
_tmp24D,_tmp24D + 7},{_tmp24E,_tmp24E,_tmp24E + 7},{_tmp24F,_tmp24F,_tmp24F + 8},{
_tmp250,_tmp250,_tmp250 + 5},{_tmp251,_tmp251,_tmp251 + 5},{_tmp252,_tmp252,
_tmp252 + 6},{_tmp253,_tmp253,_tmp253 + 4},{_tmp254,_tmp254,_tmp254 + 5},{_tmp255,
_tmp255,_tmp255 + 6},{_tmp256,_tmp256,_tmp256 + 7},{_tmp257,_tmp257,_tmp257 + 7},{
_tmp258,_tmp258,_tmp258 + 9},{_tmp259,_tmp259,_tmp259 + 6},{_tmp25A,_tmp25A,
_tmp25A + 9},{_tmp25B,_tmp25B,_tmp25B + 9},{_tmp25C,_tmp25C,_tmp25C + 7},{_tmp25D,
_tmp25D,_tmp25D + 6},{_tmp25E,_tmp25E,_tmp25E + 5},{_tmp25F,_tmp25F,_tmp25F + 8},{
_tmp260,_tmp260,_tmp260 + 7},{_tmp261,_tmp261,_tmp261 + 7},{_tmp262,_tmp262,
_tmp262 + 9},{_tmp263,_tmp263,_tmp263 + 3},{_tmp264,_tmp264,_tmp264 + 5},{_tmp265,
_tmp265,_tmp265 + 7},{_tmp266,_tmp266,_tmp266 + 6},{_tmp267,_tmp267,_tmp267 + 3},{
_tmp268,_tmp268,_tmp268 + 4},{_tmp269,_tmp269,_tmp269 + 5},{_tmp26A,_tmp26A,
_tmp26A + 9},{_tmp26B,_tmp26B,_tmp26B + 6},{_tmp26C,_tmp26C,_tmp26C + 7},{_tmp26D,
_tmp26D,_tmp26D + 5},{_tmp26E,_tmp26E,_tmp26E + 8},{_tmp26F,_tmp26F,_tmp26F + 4},{
_tmp270,_tmp270,_tmp270 + 6},{_tmp271,_tmp271,_tmp271 + 4},{_tmp272,_tmp272,
_tmp272 + 6},{_tmp273,_tmp273,_tmp273 + 4},{_tmp274,_tmp274,_tmp274 + 9},{_tmp275,
_tmp275,_tmp275 + 9},{_tmp276,_tmp276,_tmp276 + 6},{_tmp277,_tmp277,_tmp277 + 10},{
_tmp278,_tmp278,_tmp278 + 7},{_tmp279,_tmp279,_tmp279 + 8},{_tmp27A,_tmp27A,
_tmp27A + 9},{_tmp27B,_tmp27B,_tmp27B + 5},{_tmp27C,_tmp27C,_tmp27C + 8},{_tmp27D,
_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,_tmp27E + 7},{_tmp27F,_tmp27F,_tmp27F + 7},{
_tmp280,_tmp280,_tmp280 + 8},{_tmp281,_tmp281,_tmp281 + 7},{_tmp282,_tmp282,
_tmp282 + 8},{_tmp283,_tmp283,_tmp283 + 9},{_tmp284,_tmp284,_tmp284 + 9},{_tmp285,
_tmp285,_tmp285 + 6},{_tmp286,_tmp286,_tmp286 + 7},{_tmp287,_tmp287,_tmp287 + 5},{
_tmp288,_tmp288,_tmp288 + 8},{_tmp289,_tmp289,_tmp289 + 13},{_tmp28A,_tmp28A,
_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 14},{_tmp28C,_tmp28C,_tmp28C + 12},{_tmp28D,
_tmp28D,_tmp28D + 7},{_tmp28E,_tmp28E,_tmp28E + 7},{_tmp28F,_tmp28F,_tmp28F + 7},{
_tmp290,_tmp290,_tmp290 + 8},{_tmp291,_tmp291,_tmp291 + 9},{_tmp292,_tmp292,
_tmp292 + 6},{_tmp293,_tmp293,_tmp293 + 6},{_tmp294,_tmp294,_tmp294 + 6},{_tmp295,
_tmp295,_tmp295 + 6},{_tmp296,_tmp296,_tmp296 + 7},{_tmp297,_tmp297,_tmp297 + 6},{
_tmp298,_tmp298,_tmp298 + 11},{_tmp299,_tmp299,_tmp299 + 11},{_tmp29A,_tmp29A,
_tmp29A + 11},{_tmp29B,_tmp29B,_tmp29B + 11},{_tmp29C,_tmp29C,_tmp29C + 11},{
_tmp29D,_tmp29D,_tmp29D + 12},{_tmp29E,_tmp29E,_tmp29E + 13},{_tmp29F,_tmp29F,
_tmp29F + 11},{_tmp2A0,_tmp2A0,_tmp2A0 + 11},{_tmp2A1,_tmp2A1,_tmp2A1 + 10},{
_tmp2A2,_tmp2A2,_tmp2A2 + 9},{_tmp2A3,_tmp2A3,_tmp2A3 + 11},{_tmp2A4,_tmp2A4,
_tmp2A4 + 12},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{_tmp2A6,_tmp2A6,_tmp2A6 + 17},{
_tmp2A7,_tmp2A7,_tmp2A7 + 7},{_tmp2A8,_tmp2A8,_tmp2A8 + 19},{_tmp2A9,_tmp2A9,
_tmp2A9 + 18},{_tmp2AA,_tmp2AA,_tmp2AA + 9},{_tmp2AB,_tmp2AB,_tmp2AB + 13},{_tmp2AC,
_tmp2AC,_tmp2AC + 16},{_tmp2AD,_tmp2AD,_tmp2AD + 18},{_tmp2AE,_tmp2AE,_tmp2AE + 13},{
_tmp2AF,_tmp2AF,_tmp2AF + 10},{_tmp2B0,_tmp2B0,_tmp2B0 + 4},{_tmp2B1,_tmp2B1,
_tmp2B1 + 4},{_tmp2B2,_tmp2B2,_tmp2B2 + 4},{_tmp2B3,_tmp2B3,_tmp2B3 + 4},{_tmp2B4,
_tmp2B4,_tmp2B4 + 4},{_tmp2B5,_tmp2B5,_tmp2B5 + 4},{_tmp2B6,_tmp2B6,_tmp2B6 + 4},{
_tmp2B7,_tmp2B7,_tmp2B7 + 4},{_tmp2B8,_tmp2B8,_tmp2B8 + 4},{_tmp2B9,_tmp2B9,
_tmp2B9 + 4},{_tmp2BA,_tmp2BA,_tmp2BA + 4},{_tmp2BB,_tmp2BB,_tmp2BB + 4},{_tmp2BC,
_tmp2BC,_tmp2BC + 4},{_tmp2BD,_tmp2BD,_tmp2BD + 4},{_tmp2BE,_tmp2BE,_tmp2BE + 4},{
_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,_tmp2C0,_tmp2C0 + 4},{_tmp2C1,_tmp2C1,
_tmp2C1 + 4},{_tmp2C2,_tmp2C2,_tmp2C2 + 4},{_tmp2C3,_tmp2C3,_tmp2C3 + 4},{_tmp2C4,
_tmp2C4,_tmp2C4 + 4},{_tmp2C5,_tmp2C5,_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 4},{
_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,_tmp2C8,_tmp2C8 + 4},{_tmp2C9,_tmp2C9,
_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{_tmp2CB,_tmp2CB,_tmp2CB + 5},{_tmp2CC,
_tmp2CC,_tmp2CC + 17},{_tmp2CD,_tmp2CD,_tmp2CD + 21},{_tmp2CE,_tmp2CE,_tmp2CE + 20},{
_tmp2CF,_tmp2CF,_tmp2CF + 21},{_tmp2D0,_tmp2D0,_tmp2D0 + 13},{_tmp2D1,_tmp2D1,
_tmp2D1 + 15},{_tmp2D2,_tmp2D2,_tmp2D2 + 17},{_tmp2D3,_tmp2D3,_tmp2D3 + 19},{
_tmp2D4,_tmp2D4,_tmp2D4 + 12},{_tmp2D5,_tmp2D5,_tmp2D5 + 17},{_tmp2D6,_tmp2D6,
_tmp2D6 + 23},{_tmp2D7,_tmp2D7,_tmp2D7 + 24},{_tmp2D8,_tmp2D8,_tmp2D8 + 15},{
_tmp2D9,_tmp2D9,_tmp2D9 + 11},{_tmp2DA,_tmp2DA,_tmp2DA + 15},{_tmp2DB,_tmp2DB,
_tmp2DB + 10},{_tmp2DC,_tmp2DC,_tmp2DC + 15},{_tmp2DD,_tmp2DD,_tmp2DD + 5},{_tmp2DE,
_tmp2DE,_tmp2DE + 15},{_tmp2DF,_tmp2DF,_tmp2DF + 15},{_tmp2E0,_tmp2E0,_tmp2E0 + 11},{
_tmp2E1,_tmp2E1,_tmp2E1 + 22},{_tmp2E2,_tmp2E2,_tmp2E2 + 26},{_tmp2E3,_tmp2E3,
_tmp2E3 + 16},{_tmp2E4,_tmp2E4,_tmp2E4 + 16},{_tmp2E5,_tmp2E5,_tmp2E5 + 24},{
_tmp2E6,_tmp2E6,_tmp2E6 + 25},{_tmp2E7,_tmp2E7,_tmp2E7 + 21},{_tmp2E8,_tmp2E8,
_tmp2E8 + 22},{_tmp2E9,_tmp2E9,_tmp2E9 + 16},{_tmp2EA,_tmp2EA,_tmp2EA + 19},{
_tmp2EB,_tmp2EB,_tmp2EB + 25},{_tmp2EC,_tmp2EC,_tmp2EC + 23},{_tmp2ED,_tmp2ED,
_tmp2ED + 24},{_tmp2EE,_tmp2EE,_tmp2EE + 18},{_tmp2EF,_tmp2EF,_tmp2EF + 17},{
_tmp2F0,_tmp2F0,_tmp2F0 + 18},{_tmp2F1,_tmp2F1,_tmp2F1 + 17},{_tmp2F2,_tmp2F2,
_tmp2F2 + 18},{_tmp2F3,_tmp2F3,_tmp2F3 + 12},{_tmp2F4,_tmp2F4,_tmp2F4 + 11},{
_tmp2F5,_tmp2F5,_tmp2F5 + 18},{_tmp2F6,_tmp2F6,_tmp2F6 + 8},{_tmp2F7,_tmp2F7,
_tmp2F7 + 12},{_tmp2F8,_tmp2F8,_tmp2F8 + 23},{_tmp2F9,_tmp2F9,_tmp2F9 + 14},{
_tmp2FA,_tmp2FA,_tmp2FA + 18},{_tmp2FB,_tmp2FB,_tmp2FB + 8},{_tmp2FC,_tmp2FC,
_tmp2FC + 11},{_tmp2FD,_tmp2FD,_tmp2FD + 20},{_tmp2FE,_tmp2FE,_tmp2FE + 9},{_tmp2FF,
_tmp2FF,_tmp2FF + 16},{_tmp300,_tmp300,_tmp300 + 19},{_tmp301,_tmp301,_tmp301 + 10},{
_tmp302,_tmp302,_tmp302 + 16},{_tmp303,_tmp303,_tmp303 + 11},{_tmp304,_tmp304,
_tmp304 + 14},{_tmp305,_tmp305,_tmp305 + 11},{_tmp306,_tmp306,_tmp306 + 15},{
_tmp307,_tmp307,_tmp307 + 22},{_tmp308,_tmp308,_tmp308 + 16},{_tmp309,_tmp309,
_tmp309 + 17},{_tmp30A,_tmp30A,_tmp30A + 12},{_tmp30B,_tmp30B,_tmp30B + 18},{
_tmp30C,_tmp30C,_tmp30C + 17},{_tmp30D,_tmp30D,_tmp30D + 12},{_tmp30E,_tmp30E,
_tmp30E + 16},{_tmp30F,_tmp30F,_tmp30F + 11},{_tmp310,_tmp310,_tmp310 + 10},{
_tmp311,_tmp311,_tmp311 + 14},{_tmp312,_tmp312,_tmp312 + 15},{_tmp313,_tmp313,
_tmp313 + 20},{_tmp314,_tmp314,_tmp314 + 27},{_tmp315,_tmp315,_tmp315 + 10},{
_tmp316,_tmp316,_tmp316 + 18},{_tmp317,_tmp317,_tmp317 + 21},{_tmp318,_tmp318,
_tmp318 + 19},{_tmp319,_tmp319,_tmp319 + 16},{_tmp31A,_tmp31A,_tmp31A + 20},{
_tmp31B,_tmp31B,_tmp31B + 15},{_tmp31C,_tmp31C,_tmp31C + 16},{_tmp31D,_tmp31D,
_tmp31D + 20},{_tmp31E,_tmp31E,_tmp31E + 15},{_tmp31F,_tmp31F,_tmp31F + 8},{_tmp320,
_tmp320,_tmp320 + 19},{_tmp321,_tmp321,_tmp321 + 14},{_tmp322,_tmp322,_tmp322 + 19},{
_tmp323,_tmp323,_tmp323 + 11},{_tmp324,_tmp324,_tmp324 + 22},{_tmp325,_tmp325,
_tmp325 + 20},{_tmp326,_tmp326,_tmp326 + 23},{_tmp327,_tmp327,_tmp327 + 20},{
_tmp328,_tmp328,_tmp328 + 22},{_tmp329,_tmp329,_tmp329 + 23},{_tmp32A,_tmp32A,
_tmp32A + 24},{_tmp32B,_tmp32B,_tmp32B + 24},{_tmp32C,_tmp32C,_tmp32C + 15},{
_tmp32D,_tmp32D,_tmp32D + 20},{_tmp32E,_tmp32E,_tmp32E + 22},{_tmp32F,_tmp32F,
_tmp32F + 17},{_tmp330,_tmp330,_tmp330 + 20},{_tmp331,_tmp331,_tmp331 + 26},{
_tmp332,_tmp332,_tmp332 + 16},{_tmp333,_tmp333,_tmp333 + 17},{_tmp334,_tmp334,
_tmp334 + 15},{_tmp335,_tmp335,_tmp335 + 19},{_tmp336,_tmp336,_tmp336 + 19},{
_tmp337,_tmp337,_tmp337 + 25},{_tmp338,_tmp338,_tmp338 + 26},{_tmp339,_tmp339,
_tmp339 + 9},{_tmp33A,_tmp33A,_tmp33A + 20},{_tmp33B,_tmp33B,_tmp33B + 17},{_tmp33C,
_tmp33C,_tmp33C + 18},{_tmp33D,_tmp33D,_tmp33D + 11},{_tmp33E,_tmp33E,_tmp33E + 12}};
static short Cyc_yyr1[430]={0,131,132,132,132,132,132,132,132,133,133,133,134,134,
134,134,135,135,136,137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,
142,142,142,142,143,143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,
147,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
148,148,149,150,150,150,151,151,151,152,152,153,153,154,154,154,155,155,156,156,
157,157,158,158,159,160,160,161,161,162,163,163,163,163,163,163,164,165,165,166,
166,166,167,167,167,168,168,169,169,169,169,170,170,170,171,171,172,172,173,173,
173,173,173,173,173,173,173,173,174,174,175,176,176,176,177,177,177,178,178,178,
179,179,179,180,180,181,181,181,181,182,182,183,183,184,184,185,185,186,186,187,
187,188,188,188,188,189,189,190,190,191,191,191,192,193,193,194,194,195,195,195,
195,196,196,196,196,197,198,198,199,199,200,200,201,201,201,201,201,202,202,203,
203,203,204,204,204,204,204,204,204,204,204,204,204,205,205,205,205,205,205,205,
205,205,205,205,205,205,206,207,207,207,208,208,208,209,209,209,209,209,209,210,
210,210,210,210,210,210,210,211,211,211,211,211,211,211,212,212,212,212,212,213,
213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,214,214,214,
214,214,214,214,214,215,215,215,215,215,215,215,215,215,215,215,215,215,215,216,
216,217,217,218,218,219,219,220,220,221,221,221,221,221,221,221,221,221,221,221,
222,222,222,222,222,222,222,223,224,224,225,225,226,226,227,227,228,228,229,229,
229,230,230,230,230,230,231,231,231,232,232,232,233,233,233,233,234,234,235,235,
235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,237,237,237,
237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,238,238,239,
240,240,241,241,241,241,242,242,243,243,244,244,245,245,246,246};static short Cyc_yyr2[
430]={0,1,2,3,5,3,5,6,0,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,
2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,1,4,1,
1,1,1,5,2,4,1,3,1,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,1,3,1,2,3,7,
4,6,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,5,4,5,4,4,2,2,1,2,5,2,2,1,0,3,3,0,1,1,0,1,1,
0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,
3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,
5,2,2,3,1,2,2,2,3,3,1,2,1,2,1,2,5,7,2,7,2,8,6,2,0,3,4,5,6,7,4,0,3,4,5,4,5,2,7,2,
6,7,7,8,7,8,8,9,6,7,7,8,2,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,4,1,3,1,2,
1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,
2,2,6,7,4,4,1,1,1,3,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short
Cyc_yydefact[876]={0,11,38,39,40,41,43,55,58,59,60,61,62,63,64,65,77,78,79,92,93,
34,0,0,44,0,0,119,120,0,0,0,420,163,422,421,423,74,0,0,56,0,148,148,147,1,0,9,0,
0,10,0,28,36,30,32,66,67,0,68,154,0,130,0,142,151,69,132,90,42,41,35,0,81,305,
420,301,304,303,0,299,0,0,0,0,0,186,0,306,18,20,0,0,0,0,0,0,0,0,0,145,146,2,0,0,
0,0,22,0,98,99,101,29,37,31,33,94,424,425,90,156,0,155,0,26,0,0,12,165,0,0,140,
131,143,153,152,154,141,0,70,0,42,85,0,83,0,0,420,0,0,311,302,310,25,0,0,90,0,0,
0,108,104,106,204,206,0,0,0,426,427,164,76,0,133,57,184,0,181,0,0,419,0,0,0,0,0,
0,0,0,0,90,0,0,416,405,417,418,0,0,0,0,390,0,388,389,0,0,321,334,342,344,346,348,
350,352,355,360,363,366,370,0,372,391,404,403,3,0,5,0,23,0,0,0,13,0,95,96,0,89,
90,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,420,241,244,0,0,0,0,227,228,229,0,230,231,
232,0,319,27,14,101,171,187,0,0,167,165,0,211,0,0,151,0,45,0,0,0,82,0,0,0,300,
313,0,188,0,0,0,0,207,179,0,109,105,107,165,0,213,205,214,429,428,0,72,73,75,51,
50,0,48,165,183,213,185,71,0,0,380,0,336,370,0,337,338,0,0,0,0,0,0,0,0,0,373,374,
0,0,0,0,0,376,377,375,150,149,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,324,325,326,
327,328,329,330,331,332,333,323,0,378,0,397,398,0,0,0,408,0,90,19,0,21,0,100,102,
190,189,15,87,97,0,0,110,111,113,90,0,117,243,246,255,0,257,0,0,274,0,276,0,289,
0,0,292,293,294,0,260,0,296,0,0,0,238,239,0,0,0,0,0,252,248,101,250,245,242,0,
172,0,0,0,178,166,173,136,0,0,0,167,138,0,139,0,134,151,157,46,91,0,86,341,84,80,
312,307,0,24,0,0,0,0,200,315,317,0,308,0,0,208,0,167,0,151,0,215,165,0,0,226,210,
0,0,0,182,0,0,0,191,195,0,0,0,0,0,0,0,0,0,0,0,0,407,0,0,406,414,0,413,343,0,345,
347,349,351,353,354,358,359,356,357,361,362,364,365,367,368,369,322,396,393,0,
395,0,0,0,4,6,114,103,0,0,167,90,125,0,0,0,0,0,0,0,0,291,295,0,0,0,0,234,0,0,0,
240,0,16,320,165,0,175,0,0,168,0,137,171,159,212,135,157,144,7,314,203,0,316,199,
201,309,0,209,180,222,0,216,217,151,0,167,0,151,0,0,0,0,47,49,379,0,0,192,0,196,
401,402,0,384,0,0,0,0,0,0,0,0,371,410,0,0,394,392,0,0,112,115,94,118,127,126,0,0,
121,0,0,0,0,0,0,0,0,0,0,261,297,0,0,0,0,0,17,167,0,176,174,0,167,0,158,202,318,
221,219,224,0,225,218,151,0,52,53,0,0,0,193,197,0,0,0,0,339,340,383,412,0,415,
335,409,411,0,0,116,128,125,125,253,0,261,273,0,0,0,0,0,0,0,0,0,0,0,0,298,0,0,0,
0,237,161,177,169,160,165,223,220,0,382,381,0,198,385,0,0,399,0,88,0,124,123,0,
268,0,0,277,0,0,0,285,0,0,0,0,0,0,0,0,0,259,0,233,236,0,0,167,0,0,0,0,400,0,254,
0,0,0,256,275,278,279,0,287,286,0,281,0,0,0,0,0,0,262,0,0,170,162,0,0,0,0,129,0,
0,0,258,280,288,282,283,0,267,0,261,263,0,0,54,194,386,0,0,0,269,284,0,264,290,
235,0,272,268,270,261,265,387,271,266,0,0,0};static short Cyc_yydefgoto[116]={873,
45,46,47,260,48,402,49,404,261,232,262,52,476,53,323,324,54,170,55,56,142,143,57,
139,58,234,235,108,109,110,236,163,413,414,415,59,60,669,670,671,61,62,63,64,65,
100,136,121,610,500,66,501,467,602,459,463,464,306,281,177,85,86,519,407,520,521,
491,492,164,283,284,502,314,263,264,265,266,267,268,749,811,269,270,297,298,494,
495,271,272,390,207,482,208,209,210,211,212,213,214,215,216,217,218,219,220,221,
222,537,538,223,224,68,119,171,318};static short Cyc_yypact[876]={3027,- -32768,-
-32768,- -32768,- -32768,- 12,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3761,- 43,
1102,- -32768,156,- 24,- -32768,- -32768,- 9,39,56,- -32768,86,- -32768,- -32768,- -32768,
- -32768,138,411,170,159,199,199,- -32768,- -32768,2906,- -32768,106,419,- -32768,222,
3761,3761,3761,3761,- -32768,- -32768,303,- -32768,- 6,3632,190,19,374,163,- -32768,
156,204,258,270,- -32768,156,268,- -32768,214,- -32768,- -32768,- -32768,2369,- -32768,
284,291,358,2369,321,351,360,240,- -32768,- -32768,5857,5857,5857,134,364,398,134,
6035,2652,- -32768,- -32768,- -32768,2906,3147,2906,3147,- -32768,395,425,- -32768,
3563,- -32768,- -32768,- -32768,- -32768,6035,- -32768,- -32768,204,- -32768,156,-
-32768,1753,- -32768,3632,222,- -32768,3817,5857,4194,- -32768,190,- -32768,- -32768,
- -32768,- 6,- -32768,5857,- -32768,3147,- -32768,434,440,444,156,464,453,463,2369,-
-32768,- -32768,- -32768,- -32768,482,5410,204,2369,469,- 58,6035,6035,6035,390,-
-32768,11,11,11,- -32768,- -32768,- -32768,- -32768,0,- -32768,- -32768,239,369,-
-32768,5638,473,- -32768,5410,5486,478,494,496,498,503,516,520,204,5714,5714,-
-32768,- -32768,- -32768,- -32768,2007,532,5790,5790,- -32768,5790,- -32768,- -32768,-
15,542,- -32768,- 55,630,581,527,538,472,123,483,437,148,- -32768,649,5790,143,- 17,
- -32768,610,- -32768,611,- -32768,612,- -32768,411,4270,3632,- -32768,613,6035,-
-32768,450,615,164,409,40,22,41,2113,43,623,619,624,4346,2219,49,631,2325,2325,
83,632,627,- -32768,- -32768,1064,1233,222,1363,- -32768,- -32768,- -32768,621,-
-32768,- -32768,- -32768,276,- -32768,- -32768,- -32768,636,652,- -32768,130,637,634,
278,642,638,99,644,163,645,650,99,655,156,- -32768,5410,657,643,- -32768,- -32768,
474,- -32768,307,930,486,5857,- -32768,656,659,- -32768,- -32768,- -32768,3445,4422,
166,- -32768,259,- -32768,- -32768,130,- -32768,- -32768,- -32768,- -32768,662,658,661,
3332,- -32768,238,- -32768,- -32768,6035,2007,- -32768,6035,- -32768,- -32768,3842,-
-32768,693,5410,3265,5410,5410,5410,5410,5410,666,2007,- -32768,- -32768,669,1883,
671,499,2775,- -32768,- -32768,- -32768,- -32768,- -32768,5790,5410,5790,5790,5790,
5790,5790,5790,5790,5790,5790,5790,5790,5790,5790,5790,5790,5790,5790,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
5410,- -32768,134,- -32768,- -32768,4498,134,5410,- -32768,668,204,- -32768,2906,-
-32768,2906,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5410,678,665,
- -32768,670,204,156,680,- -32768,- -32768,- -32768,5410,- -32768,675,5410,- -32768,
5410,682,757,- -32768,2529,684,- -32768,- -32768,- -32768,314,682,747,- -32768,4574,
3701,682,- -32768,- -32768,2325,692,699,5410,2325,- -32768,- -32768,3563,- -32768,-
-32768,- -32768,5410,- -32768,6035,686,201,- -32768,- -32768,673,- -32768,130,687,
5930,634,- -32768,5857,- -32768,5410,- -32768,163,523,- -32768,- -32768,2906,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,2369,- -32768,134,5410,2369,237,- -32768,-
-32768,- -32768,203,- -32768,68,700,- -32768,690,634,691,163,688,259,5915,5857,4650,
- -32768,- -32768,506,694,0,- -32768,695,698,708,- -32768,- -32768,260,4270,509,701,
411,511,702,703,706,517,6035,709,- -32768,705,5562,- -32768,- -32768,710,711,630,
285,581,527,538,472,123,123,483,483,483,483,437,437,148,148,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,714,- -32768,185,5857,4106,- -32768,- -32768,- -32768,
- -32768,450,5410,634,204,587,521,5410,526,528,716,4726,4802,317,- -32768,- -32768,
704,722,720,726,- -32768,713,715,533,- -32768,3632,- -32768,- -32768,727,5857,-
-32768,729,130,- -32768,718,- -32768,180,- -32768,638,- -32768,523,- -32768,- -32768,-
-32768,- -32768,719,- -32768,- -32768,- -32768,- -32768,930,- -32768,- -32768,- -32768,
728,- -32768,- -32768,163,731,634,99,163,725,733,735,736,- -32768,- -32768,741,275,
734,- -32768,3930,- -32768,- -32768,- -32768,3632,- -32768,5410,823,5410,5486,740,741,
742,4106,- -32768,- -32768,5410,5410,- -32768,- -32768,99,327,- -32768,- -32768,6035,-
-32768,- -32768,- -32768,744,156,328,2325,537,748,2325,5410,4878,330,4954,331,5030,
628,- -32768,750,5410,762,- 18,752,- -32768,634,112,- -32768,- -32768,761,634,6035,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,751,- -32768,- -32768,163,769,-
-32768,- -32768,756,758,5410,- -32768,- -32768,4270,551,760,759,- -32768,693,- -32768,
- -32768,348,- -32768,- -32768,- -32768,- -32768,4106,765,- -32768,764,81,587,838,770,
628,- -32768,559,2325,564,5106,2325,569,5182,5258,343,883,763,768,- -32768,345,
2325,2325,778,- -32768,- -32768,- -32768,772,- -32768,727,- -32768,- -32768,773,-
-32768,- -32768,356,- -32768,- -32768,6035,853,- -32768,4018,- -32768,6035,- -32768,-
-32768,2325,648,774,775,- -32768,2325,2325,572,- -32768,2325,2325,575,2325,578,
5334,771,- 26,1883,- -32768,5410,- -32768,- -32768,766,130,634,792,5410,779,781,-
-32768,584,- -32768,2401,776,786,- -32768,- -32768,- -32768,- -32768,2325,- -32768,-
-32768,2325,- -32768,2325,2325,586,1883,5410,1493,- -32768,588,800,- -32768,- -32768,
789,371,795,6035,- -32768,794,377,1883,- -32768,- -32768,- -32768,- -32768,- -32768,
2325,- -32768,402,628,- -32768,2325,2325,- -32768,- -32768,- -32768,797,1883,1623,-
-32768,- -32768,1493,- -32768,- -32768,- -32768,801,- -32768,648,- -32768,628,- -32768,
- -32768,- -32768,- -32768,909,913,- -32768};static short Cyc_yypgoto[116]={- -32768,60,
- -32768,574,- -32768,- -32768,- -32768,- -32768,- -32768,18,- 52,561,- -32768,- -32768,-
62,403,- -32768,126,818,46,- -32768,- -32768,- 123,- -32768,- 10,- -32768,252,- -32768,-
-32768,- -32768,689,683,- 20,- -32768,- -32768,352,- -32768,- -32768,- 33,- -32768,-
-32768,8,- 36,26,- -32768,- -32768,880,- 243,788,318,- 112,- 56,- 121,- 264,128,324,329,
- 312,- 401,- 97,- 277,803,- -32768,- 225,- 168,- 484,- 298,- -32768,441,- 152,- 80,- 118,90,
- 141,75,- -32768,- -32768,- 40,- 135,- -32768,- 548,- 756,- -32768,- -32768,- 21,777,316,-
-32768,226,184,- -32768,- 173,- 378,- 167,573,595,596,594,571,336,220,337,339,- 174,
577,- -32768,- -32768,- -32768,- 323,- -32768,- -32768,10,902,- -32768,- 341,- 128};
static short Cyc_yytable[6148]={131,176,87,490,122,510,406,280,334,125,67,165,166,
167,337,338,279,321,50,205,289,127,294,424,360,355,356,132,357,160,160,160,73,88,
568,89,160,160,319,320,33,422,427,474,431,352,391,96,304,67,32,559,825,514,160,
562,67,35,148,111,600,67,72,152,50,361,160,160,117,90,131,233,561,67,398,753,160,
137,175,124,122,662,144,69,315,274,667,668,826,88,133,358,33,322,88,754,237,621,
160,160,160,867,399,305,91,120,102,317,175,238,871,614,32,67,67,67,67,425,587,35,
481,50,50,50,50,451,452,39,454,124,426,239,316,317,275,160,67,162,162,162,307,
308,309,273,162,162,301,613,423,428,67,432,92,440,603,144,472,441,50,88,469,478,
162,225,226,227,228,88,480,93,723,505,315,160,162,162,446,620,94,515,346,516,597,
326,162,67,505,- 122,779,312,317,514,409,664,460,531,447,368,369,448,290,327,555,
556,557,606,162,162,162,471,103,104,392,393,394,237,533,161,161,161,316,757,462,
497,161,161,477,168,33,419,134,135,608,169,317,461,370,623,275,481,67,160,161,
162,371,416,95,67,160,32,273,395,509,313,161,161,35,176,396,397,625,462,97,160,
161,328,376,98,160,160,453,160,67,458,310,377,378,138,849,418,493,162,311,206,
278,160,161,161,161,- 165,175,132,160,38,- 165,457,642,711,128,33,861,144,660,129,
99,175,130,599,665,618,175,88,869,619,287,32,481,138,- 187,430,872,490,35,161,-
187,439,107,146,444,445,39,32,32,96,175,67,312,67,35,35,42,43,44,715,616,156,325,
325,157,162,67,327,488,489,311,311,162,42,43,44,655,161,38,140,703,141,640,506,
168,710,641,162,507,145,169,508,162,162,651,162,456,300,278,701,150,628,457,705,
468,629,565,607,149,162,627,457,32,160,481,658,162,594,34,35,36,305,160,572,116,
160,462,487,67,595,67,408,714,457,584,760,50,681,50,353,457,153,756,457,573,161,
715,759,732,727,741,744,161,728,733,175,457,457,305,509,160,160,661,791,175,796,
581,161,151,457,771,457,161,161,772,161,588,154,566,762,567,612,457,155,160,615,
803,124,172,161,715,437,690,853,329,330,161,457,719,720,695,725,175,457,603,67,
767,857,42,43,44,504,88,50,310,229,88,704,160,714,32,162,311,173,42,43,44,35,457,
420,162,421,860,162,691,39,408,589,609,105,106,593,526,527,528,42,43,44,645,726,
67,230,160,831,536,611,16,17,18,32,291,462,292,714,366,367,35,293,162,162,689,
372,373,353,39,374,375,51,295,666,412,522,146,525,42,43,44,529,296,353,558,299,
162,303,416,536,67,333,71,485,486,161,339,540,547,548,549,550,667,668,161,496,
486,161,493,632,633,634,340,160,341,233,342,51,535,457,162,343,273,112,113,114,
115,804,643,457,646,457,126,563,344,536,650,457,345,88,672,457,161,161,160,674,
457,675,457,801,354,596,688,457,162,237,735,457,359,575,747,748,577,364,578,609,
161,536,582,827,768,457,365,124,51,51,51,51,780,457,809,810,126,782,457,592,175,
807,786,457,731,816,457,855,819,457,126,821,457,846,161,848,631,836,330,845,457,
850,457,775,776,51,545,546,858,408,363,160,362,551,552,162,160,553,554,400,433,
401,403,410,417,865,866,434,161,868,793,455,435,379,380,381,382,383,384,385,386,
387,388,442,449,450,162,231,462,458,465,734,466,408,737,470,473,38,175,332,389,
88,335,335,317,479,475,483,484,499,498,512,348,349,511,513,360,160,530,570,335,
335,532,335,534,564,569,576,571,574,420,579,583,585,590,161,591,126,598,601,604,
335,33,622,624,639,673,635,637,626,678,680,638,682,644,653,647,648,781,162,649,
785,652,656,162,657,161,659,676,408,683,797,798,684,685,278,687,718,686,692,698,
700,408,694,702,724,706,707,708,709,654,712,717,721,730,722,808,736,750,752,755,
814,815,758,761,817,818,763,820,764,777,765,769,770,335,773,774,716,795,778,799,
794,805,813,812,162,800,802,792,824,832,829,834,835,839,841,840,851,842,161,843,
844,852,408,161,524,738,740,854,743,864,746,856,874,870,751,408,875,523,174,636,
729,411,405,859,74,663,101,288,862,863,697,830,696,693,282,617,539,302,699,544,
335,766,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,408,
541,543,542,118,161,0,51,0,51,0,784,74,0,788,790,0,0,0,0,147,76,0,77,78,0,0,35,0,
0,0,0,0,335,0,79,0,126,80,81,0,0,0,0,0,82,0,126,0,0,83,84,0,0,0,0,0,0,126,0,0,
823,0,0,0,0,828,147,76,0,77,78,0,833,35,0,0,0,0,838,0,0,79,0,51,80,81,0,0,488,
489,0,82,0,0,847,0,83,84,0,0,0,0,0,0,0,0,0,0,443,335,2,3,4,70,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,- 251,- 251,21,178,179,241,0,242,243,244,245,246,247,248,
249,22,180,23,181,250,0,182,24,251,0,335,27,28,252,183,184,253,254,185,186,187,
188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,74,0,0,0,0,0,0,0,335,0,0,0,
0,0,0,126,0,257,193,194,195,196,33,34,35,36,37,38,258,123,- 251,0,197,0,0,40,198,
0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,75,76,0,77,78,0,0,35,0,0,0,126,0,
0,0,79,0,0,80,81,0,0,0,0,0,82,0,0,0,0,83,84,335,0,0,0,0,0,0,443,335,2,3,4,70,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,- 247,- 247,21,178,179,241,0,242,243,244,245,
246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,
186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,37,38,258,123,- 247,0,197,0,0,40,
198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,443,0,2,3,4,70,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,- 249,- 249,21,178,179,241,0,242,243,244,245,246,247,
248,249,22,180,23,181,250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,186,187,
188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,257,193,194,195,196,33,34,35,36,37,38,258,123,- 249,0,197,0,0,40,198,0,0,0,
0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,443,0,2,3,4,70,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,747,748,21,178,179,241,0,242,243,244,245,246,247,248,249,22,
180,23,181,250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,
31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,
193,194,195,196,33,34,35,36,37,38,258,123,- 261,0,197,0,0,40,198,0,0,0,0,0,199,0,
0,0,200,201,202,0,0,0,0,203,204,443,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,809,810,21,178,179,241,0,242,243,244,245,246,247,248,249,22,180,23,181,
250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,
0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,
195,196,33,34,35,36,37,38,258,123,- 268,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,
201,202,0,0,0,0,203,204,240,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,178,179,241,0,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,
251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,
36,37,38,258,123,259,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,
203,204,443,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,
241,0,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,27,28,
252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,37,38,258,
123,0,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,350,0,0,0,203,204,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,
0,0,182,0,0,0,0,27,28,0,183,184,0,0,185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,
191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,
37,38,0,351,0,429,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,
178,179,241,0,242,243,244,245,246,247,248,249,0,180,0,181,250,0,182,0,251,0,0,0,
0,252,183,184,253,254,185,186,187,188,0,0,0,255,189,0,256,190,0,0,0,191,192,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,0,0,35,0,0,0,258,123,0,
438,197,0,0,0,285,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,178,179,241,0,
242,243,244,245,246,247,248,249,0,180,0,181,250,0,182,0,251,0,0,0,0,252,183,184,
253,254,185,186,187,188,0,0,0,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,0,0,35,0,0,0,258,123,0,443,197,0,0,0,
285,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,178,179,241,0,242,243,244,
245,246,247,248,249,0,180,0,181,250,0,182,0,251,0,0,0,0,252,183,184,253,254,185,
186,187,188,0,0,0,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,837,0,0,0,0,74,0,0,0,
0,0,0,0,0,0,0,257,193,194,195,196,0,0,35,178,179,0,258,123,0,0,197,0,0,0,285,0,
180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,147,
76,189,77,78,190,0,35,0,191,192,0,0,0,0,79,0,0,80,81,0,0,0,0,0,82,0,0,0,0,83,84,
32,193,194,195,196,0,0,35,0,0,0,0,0,0,0,197,0,0,0,285,0,0,0,0,0,199,0,0,0,200,
201,202,0,0,0,0,203,204,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
178,179,0,0,0,0,0,0,0,0,0,0,22,180,23,181,0,0,182,24,0,0,0,27,28,0,183,184,0,0,
185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,580,0,0,0,197,0,0,40,198,0,0,
0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,28,0,183,
184,0,0,185,186,187,188,29,30,31,0,189,158,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,0,159,0,0,197,0,0,
40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,
28,0,183,184,0,0,185,186,187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,33,34,35,36,37,38,0,0,0,0,197,
0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,- 8,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,
0,0,0,0,42,43,44,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,
34,35,36,37,38,0,0,0,0,39,0,0,40,41,0,0,0,0,0,42,43,44,1,0,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,
25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,0,0,0,0,
42,43,44,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,0,0,0,0,0,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,
0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,32,0,0,0,0,33,34,35,
36,37,38,22,0,0,0,39,0,0,40,41,0,0,27,28,0,42,43,44,0,0,0,0,0,29,30,31,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,32,0,0,0,0,33,34,35,36,37,
38,278,0,0,0,325,0,0,40,41,0,0,0,311,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,
0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,
0,0,33,34,0,36,37,38,278,0,0,0,310,0,0,40,41,0,0,0,311,0,42,43,44,2,3,4,70,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,22,231,23,
0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,
22,0,23,0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,4,70,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,33,34,0,36,37,
38,24,0,0,0,27,28,0,40,41,0,0,0,0,0,0,29,30,31,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,33,34,0,36,37,38,27,28,178,179,
0,0,0,40,41,517,0,29,30,31,0,180,0,181,0,0,182,0,0,0,0,0,0,0,183,184,0,0,185,186,
187,188,0,0,0,0,189,0,276,190,0,277,0,191,192,0,33,34,0,36,37,38,278,0,0,0,0,0,0,
40,41,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,336,518,0,197,0,0,0,285,178,
179,488,489,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,183,
184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,336,713,0,197,0,0,0,285,
178,179,488,489,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,
183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,336,806,0,197,0,0,0,
285,178,179,488,489,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,
0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,0,336,0,0,197,0,0,0,
285,178,179,488,489,0,199,0,0,0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,
0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,
285,0,180,0,181,286,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,
0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,0,0,35,178,179,0,0,336,0,0,197,0,0,0,285,0,180,0,181,0,199,182,0,
0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,436,0,0,0,197,0,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,
204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,
0,181,503,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,
189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,
195,196,0,0,35,178,179,0,0,0,0,0,197,560,0,0,285,0,180,0,181,0,199,182,0,0,200,
201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,
0,0,197,586,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,0,
181,630,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,
0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,
196,0,0,35,178,179,0,677,0,0,0,197,0,0,0,285,0,180,0,181,0,199,182,0,0,200,201,
202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,679,0,0,
0,197,0,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,
186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,739,0,0,285,0,180,0,181,
0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,
190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,
0,35,178,179,0,0,0,0,0,197,742,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,
183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,745,0,0,0,197,0,
0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,
188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,783,0,0,285,0,180,0,181,0,199,
182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,0,0,0,197,787,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,789,0,0,
285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,
0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,
194,195,196,0,0,35,178,179,0,0,0,0,0,197,822,0,0,285,0,180,0,181,0,199,182,0,0,
200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,0,0,0,0,197,0,0,0,285,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,336,0,0,197,0,0,0,285,0,180,0,0,
0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,
0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,654,0,0,197,0,0,0,285,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,
203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,331,0,0,0,285,0,
180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,
0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,
0,0,35,178,179,0,0,0,0,0,347,0,0,0,285,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,32,193,194,195,196,0,0,35,0,0,0,22,0,0,0,197,0,0,
0,285,0,0,27,28,0,199,0,0,0,200,201,202,0,29,30,31,203,204,158,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,33,34,0,
36,37,38,0,159,27,28,0,0,22,40,41,0,0,0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,0,0,29,
30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,0,0,33,34,0,36,37,38,278,605,0,0,0,
0,0,40,41,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,
0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41};static short Cyc_yycheck[6148]={62,98,23,
301,60,317,231,128,181,61,0,91,92,93,182,182,128,17,0,99,138,61,145,1,79,199,200,
63,202,91,92,93,22,23,412,25,98,99,166,167,98,1,1,286,1,197,220,39,106,39,93,392,
78,330,116,396,46,100,79,51,461,51,105,84,46,120,128,129,58,93,132,111,395,63,91,
93,138,67,98,61,136,565,72,95,73,125,5,6,114,79,64,106,98,93,84,113,116,498,160,
161,162,857,119,159,113,111,46,122,128,119,866,489,93,103,104,105,106,95,441,100,
293,103,104,105,106,260,261,108,263,111,108,121,121,122,126,197,126,91,92,93,160,
161,162,125,98,99,156,488,108,108,140,108,113,104,466,145,284,108,140,149,281,
289,116,103,104,105,106,157,291,113,654,312,73,235,128,129,93,109,92,331,190,333,
459,175,138,175,327,106,736,163,122,468,232,571,64,347,113,74,75,116,140,175,376,
377,378,469,160,161,162,110,104,105,69,70,71,235,351,91,92,93,121,109,278,303,98,
99,288,93,98,239,67,68,475,99,122,105,113,501,230,412,230,303,116,197,121,237,
108,237,310,93,232,108,314,163,128,129,100,354,115,116,503,317,92,325,138,175,
118,108,330,331,262,333,262,93,108,127,128,113,826,115,301,235,116,99,104,347,
160,161,162,109,310,327,354,103,114,110,521,638,108,98,848,291,117,113,105,325,
116,106,572,106,330,301,860,110,130,93,489,113,104,244,868,619,100,197,110,250,
104,113,253,254,108,93,93,325,354,325,310,327,100,100,118,119,120,641,107,105,
108,108,108,303,340,325,115,116,116,116,310,118,119,120,534,235,103,105,628,95,
106,108,93,94,110,325,113,105,99,116,330,331,530,333,104,155,104,626,93,506,110,
630,110,507,400,471,108,347,506,110,93,459,571,114,354,453,99,100,101,461,468,
417,105,471,466,104,402,453,404,231,641,110,104,696,402,104,404,197,110,104,690,
110,418,303,728,695,104,106,104,104,310,110,110,459,110,110,498,505,506,507,564,
104,468,104,432,325,94,110,106,110,330,331,110,333,442,110,402,706,404,486,110,
107,530,490,114,453,108,347,772,249,597,106,109,110,354,110,650,650,605,658,506,
110,800,479,715,114,118,119,120,311,486,479,108,104,490,629,564,728,93,459,116,
109,118,119,120,100,110,104,468,106,114,471,598,108,336,446,476,104,105,450,342,
343,344,118,119,120,524,661,524,110,598,801,354,479,17,18,19,93,110,601,106,772,
76,77,100,107,506,507,594,72,73,331,108,122,123,0,98,573,114,339,113,341,118,119,
120,345,109,347,390,93,530,108,570,395,570,108,21,109,110,459,108,361,368,369,
370,371,5,6,468,109,110,471,619,93,94,95,108,665,108,645,108,46,109,110,564,108,
594,52,53,54,55,769,109,110,109,110,61,397,108,441,109,110,108,619,109,110,506,
507,696,109,110,109,110,760,108,457,109,110,598,665,109,110,106,423,22,23,426,
126,428,609,530,473,432,794,109,110,124,645,103,104,105,106,109,110,22,23,111,
109,110,449,696,774,109,110,670,109,110,835,109,110,125,109,110,824,564,826,508,
109,110,109,110,109,110,732,733,140,366,367,839,521,125,769,78,372,373,665,774,
374,375,105,93,106,106,106,105,856,857,104,598,860,747,106,104,80,81,82,83,84,85,
86,87,88,89,108,108,114,696,107,800,93,109,672,114,565,675,109,108,103,774,178,
107,747,181,182,122,106,117,106,121,106,110,109,191,192,108,110,79,835,108,110,
199,200,109,202,109,113,104,108,114,105,104,30,104,42,98,665,93,232,108,122,109,
220,98,109,109,93,576,109,109,117,580,581,110,105,109,106,110,110,739,769,110,
742,109,109,774,110,696,109,108,641,104,752,753,109,104,104,117,649,121,106,117,
109,654,121,109,657,117,110,109,109,105,113,25,109,106,109,777,105,104,93,104,
782,783,98,109,786,787,94,789,109,28,109,108,110,293,106,108,647,106,105,98,114,
25,104,106,835,110,110,1,114,94,121,109,108,114,816,106,93,819,769,821,822,109,
715,774,340,676,677,109,679,109,681,114,0,109,685,728,0,340,97,513,665,235,230,
845,38,570,43,136,850,851,609,800,605,601,128,491,360,157,619,365,360,712,362,
363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,772,362,364,363,
58,835,- 1,402,- 1,404,- 1,741,38,- 1,744,745,- 1,- 1,- 1,- 1,93,94,- 1,96,97,- 1,- 1,100,-
1,- 1,- 1,- 1,- 1,412,- 1,108,- 1,432,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,442,- 1,- 1,123,124,
- 1,- 1,- 1,- 1,- 1,- 1,453,- 1,- 1,791,- 1,- 1,- 1,- 1,796,93,94,- 1,96,97,- 1,803,100,- 1,- 1,
- 1,- 1,809,- 1,- 1,108,- 1,479,111,112,- 1,- 1,115,116,- 1,118,- 1,- 1,825,- 1,123,124,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1,489,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,
534,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,-
1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,571,- 1,- 1,- 1,- 1,- 1,- 1,594,- 1,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,
122,123,124,- 1,- 1,- 1,- 1,129,130,93,94,- 1,96,97,- 1,- 1,100,- 1,- 1,- 1,645,- 1,- 1,- 1,
108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,- 1,123,124,650,- 1,- 1,- 1,- 1,- 1,- 1,1,
658,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,
31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,
- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,
36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,
63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,-
1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,
65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,-
1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,
40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,
- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,
94,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,
118,- 1,- 1,- 1,122,123,124,1,- 1,- 1,- 1,129,130,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,
- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,- 1,105,- 1,1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
123,124,- 1,- 1,- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,-
1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,
- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,
40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,
- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,
94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,
- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,
38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,
65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,
- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,93,94,63,96,97,
66,- 1,100,- 1,70,71,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,- 1,
123,124,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,
- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,
- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,
- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,- 1,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,
- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,
58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,
- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,0,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,
- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,
99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,- 1,
- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,
101,102,103,37,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,48,49,- 1,118,119,120,- 1,- 1,- 1,- 1,
- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,
- 1,111,112,- 1,- 1,- 1,116,- 1,118,119,120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,
102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,116,- 1,118,119,120,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,107,39,- 1,- 1,111,112,44,- 1,- 1,- 1,
48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,
103,- 1,105,37,- 1,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,98,99,- 1,101,102,103,44,- 1,- 1,- 1,48,49,- 1,111,112,
- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,48,49,
25,26,- 1,- 1,- 1,111,112,32,- 1,59,60,61,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,90,66,- 1,93,- 1,70,71,- 1,98,99,- 1,101,
102,103,104,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,
- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,-
1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,
63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,
115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,-
1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,
40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,
118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,
55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,
108,- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,
56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,
- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,
57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,
- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,
58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,
- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,
- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,
112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,-
1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,
112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,-
1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,
- 1,38,- 1,40,117,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,-
1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,-
1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,
- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,
38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,
63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,-
1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,
- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,
40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,
- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,-
1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,
118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,
- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,
94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,
43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,
- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,
- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,
- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,
70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,
122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,
- 1,- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,
- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,
- 1,- 1,- 1,37,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,48,49,- 1,118,- 1,- 1,- 1,122,123,124,- 1,
59,60,61,129,130,64,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,37,- 1,- 1,98,99,- 1,101,102,103,- 1,105,48,49,- 1,- 1,37,
111,112,- 1,- 1,- 1,- 1,59,60,61,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,
104,90,- 1,- 1,- 1,- 1,- 1,111,112,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112};
char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _tagged_arr);
int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;
static int Cyc_yynerrs=0;struct _tuple17{struct _tagged_arr f1;void*f2;};static char
_tmp3ED[8]="stdcall";static char _tmp3EE[6]="cdecl";static char _tmp3EF[9]="fastcall";
static char _tmp3F0[9]="noreturn";static char _tmp3F1[6]="const";static char _tmp3F2[8]="aligned";
static char _tmp3F3[7]="packed";static char _tmp3F4[7]="shared";static char _tmp3F5[7]="unused";
static char _tmp3F6[5]="weak";static char _tmp3F7[10]="dllimport";static char _tmp3F8[
10]="dllexport";static char _tmp3F9[23]="no_instrument_function";static char _tmp3FA[
12]="constructor";static char _tmp3FB[11]="destructor";static char _tmp3FC[22]="no_check_memory_usage";
static char _tmp3FD[5]="pure";struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp926=10000;short*_tmp927=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp926));{unsigned int _tmp928=
_tmp926;unsigned int i;for(i=0;i < _tmp928;i ++){_tmp927[i]=(short)0;}}_tmp927;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp923=10000;void**_tmp924=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp923));{unsigned int _tmp925=
_tmp923;unsigned int i;for(i=0;i < _tmp925;i ++){_tmp924[i]=Cyc_yylval;}}_tmp924;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp920=
10000;struct Cyc_Yyltype*_tmp921=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp920));{unsigned int _tmp922=_tmp920;
unsigned int i;for(i=0;i < _tmp922;i ++){_tmp921[i]=Cyc_yynewloc();}}_tmp921;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp33F="parser stack overflow";_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(
_tmp33F,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(876,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar <= 358: 0)?(
int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 247;}yyn +=
yychar1;if((yyn < 0?1: yyn > 6147)?1: Cyc_yycheck[_check_known_subscript_notnull(
6148,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6148,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 875)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(876,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(430,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1C2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1C3: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp340;_push_handler(& _tmp340);{int _tmp342=0;if(setjmp(_tmp340.handler))
_tmp342=1;if(!_tmp342){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp341=(void*)_exn_thrown;void*_tmp344=_tmp341;_LL1C6: if(*((void**)_tmp344)
!= Cyc_Core_Failure)goto _LL1C8;_LL1C7: x=0;goto _LL1C5;_LL1C8:;_LL1C9:(void)_throw(
_tmp344);_LL1C5:;}}}{struct _handler_cons _tmp345;_push_handler(& _tmp345);{int
_tmp347=0;if(setjmp(_tmp345.handler))_tmp347=1;if(!_tmp347){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp346=(void*)_exn_thrown;void*_tmp349=_tmp346;_LL1CB: if(*((void**)_tmp349)
!= Cyc_Core_Failure)goto _LL1CD;_LL1CC: y=0;goto _LL1CA;_LL1CD:;_LL1CE:(void)_throw(
_tmp349);_LL1CA:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34A=
_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_DeclList_tok_struct _tmp34B;
_tmp34B.tag=Cyc_DeclList_tok;_tmp34B.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp34B;});_tmp34A;});break;}
case 3: _LL1C4: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34C=_cycalloc(
sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_DeclList_tok_struct _tmp34D;_tmp34D.tag=
Cyc_DeclList_tok;_tmp34D.f1=({struct Cyc_List_List*_tmp34E=_cycalloc(sizeof(*
_tmp34E));_tmp34E->hd=({struct Cyc_Absyn_Decl*_tmp34F=_cycalloc(sizeof(*_tmp34F));
_tmp34F->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp350=_cycalloc(
sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_Using_d_struct _tmp351;_tmp351.tag=
9;_tmp351.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp351.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp351;});_tmp350;}));
_tmp34F->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp34F;});_tmp34E->tl=0;_tmp34E;});_tmp34D;});
_tmp34C;});Cyc_Lex_leave_using();break;case 4: _LL1CF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_DeclList_tok_struct
_tmp353;_tmp353.tag=Cyc_DeclList_tok;_tmp353.f1=({struct Cyc_List_List*_tmp354=
_cycalloc(sizeof(*_tmp354));_tmp354->hd=({struct Cyc_Absyn_Decl*_tmp355=_cycalloc(
sizeof(*_tmp355));_tmp355->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_Absyn_Using_d_struct
_tmp357;_tmp357.tag=9;_tmp357.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp357.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp357;});_tmp356;}));
_tmp355->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp355;});_tmp354->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp354;});_tmp353;});
_tmp352;});break;case 5: _LL1D0: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_DeclList_tok_struct
_tmp359;_tmp359.tag=Cyc_DeclList_tok;_tmp359.f1=({struct Cyc_List_List*_tmp35A=
_cycalloc(sizeof(*_tmp35A));_tmp35A->hd=({struct Cyc_Absyn_Decl*_tmp35B=_cycalloc(
sizeof(*_tmp35B));_tmp35B->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp35D;_tmp35D.tag=8;_tmp35D.f1=({struct _tagged_arr*_tmp35E=_cycalloc(sizeof(*
_tmp35E));_tmp35E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp35E;});_tmp35D.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp35D;});_tmp35C;}));
_tmp35B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp35B;});_tmp35A->tl=0;_tmp35A;});_tmp359;});
_tmp358;});Cyc_Lex_leave_namespace();break;case 6: _LL1D1: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp35F;
_push_handler(& _tmp35F);{int _tmp361=0;if(setjmp(_tmp35F.handler))_tmp361=1;if(!
_tmp361){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp360=(void*)_exn_thrown;void*
_tmp363=_tmp360;_LL1D4: if(*((void**)_tmp363)!= Cyc_Core_Failure)goto _LL1D6;
_LL1D5: nspace=({const char*_tmp364="";_tag_arr(_tmp364,sizeof(char),
_get_zero_arr_size(_tmp364,1));});goto _LL1D3;_LL1D6:;_LL1D7:(void)_throw(_tmp363);
_LL1D3:;}}}{struct _handler_cons _tmp365;_push_handler(& _tmp365);{int _tmp367=0;if(
setjmp(_tmp365.handler))_tmp367=1;if(!_tmp367){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp366=(void*)_exn_thrown;void*_tmp369=_tmp366;_LL1D9: if(*((void**)_tmp369)
!= Cyc_Core_Failure)goto _LL1DB;_LL1DA: x=0;goto _LL1D8;_LL1DB:;_LL1DC:(void)_throw(
_tmp369);_LL1D8:;}}}{struct _handler_cons _tmp36A;_push_handler(& _tmp36A);{int
_tmp36C=0;if(setjmp(_tmp36A.handler))_tmp36C=1;if(!_tmp36C){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp36B=(void*)_exn_thrown;void*_tmp36E=_tmp36B;_LL1DE: if(*((void**)_tmp36E)
!= Cyc_Core_Failure)goto _LL1E0;_LL1DF: y=0;goto _LL1DD;_LL1E0:;_LL1E1:(void)_throw(
_tmp36E);_LL1DD:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp36F=
_cycalloc(sizeof(*_tmp36F));_tmp36F[0]=({struct Cyc_DeclList_tok_struct _tmp370;
_tmp370.tag=Cyc_DeclList_tok;_tmp370.f1=({struct Cyc_List_List*_tmp371=_cycalloc(
sizeof(*_tmp371));_tmp371->hd=({struct Cyc_Absyn_Decl*_tmp372=_cycalloc(sizeof(*
_tmp372));_tmp372->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp373=
_cycalloc(sizeof(*_tmp373));_tmp373[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp374;_tmp374.tag=8;_tmp374.f1=({struct _tagged_arr*_tmp375=_cycalloc(sizeof(*
_tmp375));_tmp375[0]=nspace;_tmp375;});_tmp374.f2=x;_tmp374;});_tmp373;}));
_tmp372->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp372;});_tmp371->tl=y;_tmp371;});_tmp370;});
_tmp36F;});break;}case 7: _LL1D2: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp376="C";
_tag_arr(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp377="only extern \"C\" { ... } is allowed";_tag_arr(_tmp377,
sizeof(char),_get_zero_arr_size(_tmp377,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({
struct Cyc_DeclList_tok_struct _tmp379;_tmp379.tag=Cyc_DeclList_tok;_tmp379.f1=({
struct Cyc_List_List*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->hd=({struct Cyc_Absyn_Decl*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp37D;_tmp37D.tag=10;_tmp37D.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp37D;});_tmp37C;}));
_tmp37B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp37B;});_tmp37A->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37A;});_tmp379;});
_tmp378;});break;case 8: _LL1E2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_DeclList_tok_struct
_tmp37F;_tmp37F.tag=Cyc_DeclList_tok;_tmp37F.f1=0;_tmp37F;});_tmp37E;});break;
case 9: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp380=_cycalloc(
sizeof(*_tmp380));_tmp380[0]=({struct Cyc_DeclList_tok_struct _tmp381;_tmp381.tag=
Cyc_DeclList_tok;_tmp381.f1=({struct Cyc_List_List*_tmp382=_cycalloc(sizeof(*
_tmp382));_tmp382->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp384;_tmp384.tag=1;_tmp384.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp384;});_tmp383;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp382->tl=0;
_tmp382;});_tmp381;});_tmp380;});break;case 10: _LL1E4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1E5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp385=_cycalloc(sizeof(*_tmp385));
_tmp385[0]=({struct Cyc_DeclList_tok_struct _tmp386;_tmp386.tag=Cyc_DeclList_tok;
_tmp386.f1=0;_tmp386;});_tmp385;});break;case 12: _LL1E6: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387[0]=({struct Cyc_FnDecl_tok_struct
_tmp388;_tmp388.tag=Cyc_FnDecl_tok;_tmp388.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp388;});
_tmp387;});break;case 13: _LL1E7: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389[0]=({struct Cyc_FnDecl_tok_struct
_tmp38A;_tmp38A.tag=Cyc_FnDecl_tok;_tmp38A.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38B;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38A;});
_tmp389;});break;case 14: _LL1E8: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_FnDecl_tok_struct
_tmp38D;_tmp38D.tag=Cyc_FnDecl_tok;_tmp38D.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38D;});
_tmp38C;});break;case 15: _LL1E9: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_FnDecl_tok_struct
_tmp38F;_tmp38F.tag=Cyc_FnDecl_tok;_tmp38F.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp390;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38F;});
_tmp38E;});break;case 16: _LL1EA: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391[0]=({struct Cyc_FnDecl_tok_struct
_tmp392;_tmp392.tag=Cyc_FnDecl_tok;_tmp392.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp393;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp392;});
_tmp391;});break;case 17: _LL1EB: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=({struct Cyc_FnDecl_tok_struct
_tmp395;_tmp395.tag=Cyc_FnDecl_tok;_tmp395.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp396;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp395;});
_tmp394;});break;case 18: _LL1EC: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 19: _LL1ED: Cyc_Lex_leave_using();
break;case 20: _LL1EE: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp397=
_cycalloc(sizeof(*_tmp397));_tmp397[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp397;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 21: _LL1EF: Cyc_Lex_leave_namespace();
break;case 22: _LL1F0: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp398=
_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_DeclList_tok_struct _tmp399;
_tmp399.tag=Cyc_DeclList_tok;_tmp399.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp399;});
_tmp398;});break;case 23: _LL1F1: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_DeclList_tok_struct
_tmp39B;_tmp39B.tag=Cyc_DeclList_tok;_tmp39B.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp39B;});
_tmp39A;});break;case 24: _LL1F2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_DeclList_tok_struct
_tmp39D;_tmp39D.tag=Cyc_DeclList_tok;_tmp39D.f1=({struct Cyc_List_List*_tmp39E=
_cycalloc(sizeof(*_tmp39E));_tmp39E->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp39E->tl=0;
_tmp39E;});_tmp39D;});_tmp39C;});break;case 25: _LL1F3: {struct Cyc_List_List*
_tmp39F=0;{struct Cyc_List_List*_tmp3A0=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3A0 != 0;
_tmp3A0=_tmp3A0->tl){struct _tagged_arr*_tmp3A1=(struct _tagged_arr*)_tmp3A0->hd;
struct _tuple1*_tmp3A2=({struct _tuple1*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));
_tmp3A5->f1=Cyc_Absyn_rel_ns_null;_tmp3A5->f2=_tmp3A1;_tmp3A5;});struct Cyc_Absyn_Vardecl*
_tmp3A3=Cyc_Absyn_new_vardecl(_tmp3A2,Cyc_Absyn_wildtyp(0),0);_tmp39F=({struct
Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->hd=_tmp3A3;_tmp3A4->tl=
_tmp39F;_tmp3A4;});}}_tmp39F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp39F);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A6=
_cycalloc(sizeof(*_tmp3A6));_tmp3A6[0]=({struct Cyc_DeclList_tok_struct _tmp3A7;
_tmp3A7.tag=Cyc_DeclList_tok;_tmp3A7.f1=({struct Cyc_List_List*_tmp3A8=_cycalloc(
sizeof(*_tmp3A8));_tmp3A8->hd=Cyc_Absyn_letv_decl(_tmp39F,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A8->tl=0;
_tmp3A8;});_tmp3A7;});_tmp3A6;});break;}case 26: _LL1F4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL1F5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_DeclList_tok_struct _tmp3AA;_tmp3AA.tag=Cyc_DeclList_tok;
_tmp3AA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3AA;});_tmp3A9;});
break;case 28: _LL1F6: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3AB=
_cycalloc(sizeof(*_tmp3AB));_tmp3AB[0]=({struct Cyc_DeclSpec_tok_struct _tmp3AC;
_tmp3AC.tag=Cyc_DeclSpec_tok;_tmp3AC.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->sc=({struct Cyc_Core_Opt*_tmp3AE=
_cycalloc(sizeof(*_tmp3AE));_tmp3AE->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3AE;});_tmp3AD->tq=Cyc_Absyn_empty_tqual();
_tmp3AD->type_specs=0;_tmp3AD->is_inline=0;_tmp3AD->attributes=0;_tmp3AD;});
_tmp3AC;});_tmp3AB;});break;case 29: _LL1F7: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp3AF[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp3B0="Only one storage class is allowed in a declaration";
_tag_arr(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,51));}),_tag_arr(
_tmp3AF,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B2;_tmp3B2.tag=Cyc_DeclSpec_tok;_tmp3B2.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->sc=({struct Cyc_Core_Opt*_tmp3B4=
_cycalloc(sizeof(*_tmp3B4));_tmp3B4->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B4;});_tmp3B3->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3B3->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3B3->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3B3->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3B3;});_tmp3B2;});_tmp3B1;});break;case 30: _LL1F8: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B6;_tmp3B6.tag=Cyc_DeclSpec_tok;_tmp3B6.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->sc=0;_tmp3B7->tq=Cyc_Absyn_empty_tqual();
_tmp3B7->type_specs=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));
_tmp3B8->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B8->tl=0;_tmp3B8;});
_tmp3B7->is_inline=0;_tmp3B7->attributes=0;_tmp3B7;});_tmp3B6;});_tmp3B5;});
break;case 31: _LL1F9: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3B9=
_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_DeclSpec_tok_struct _tmp3BA;
_tmp3BA.tag=Cyc_DeclSpec_tok;_tmp3BA.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3BB->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3BB->type_specs=({
struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3BC->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3BC;});
_tmp3BB->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3BB->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3BB;});
_tmp3BA;});_tmp3B9;});break;case 32: _LL1FA: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3BE;_tmp3BE.tag=Cyc_DeclSpec_tok;_tmp3BE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->sc=0;_tmp3BF->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BF->type_specs=0;
_tmp3BF->is_inline=0;_tmp3BF->attributes=0;_tmp3BF;});_tmp3BE;});_tmp3BD;});
break;case 33: _LL1FB: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3C0=
_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_DeclSpec_tok_struct _tmp3C1;
_tmp3C1.tag=Cyc_DeclSpec_tok;_tmp3C1.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3C2->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3C2->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3C2->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3C2->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3C2;});_tmp3C1;});_tmp3C0;});break;case 34: _LL1FC: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C4;_tmp3C4.tag=Cyc_DeclSpec_tok;_tmp3C4.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->sc=0;_tmp3C5->tq=Cyc_Absyn_empty_tqual();
_tmp3C5->type_specs=0;_tmp3C5->is_inline=1;_tmp3C5->attributes=0;_tmp3C5;});
_tmp3C4;});_tmp3C3;});break;case 35: _LL1FD: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C7;_tmp3C7.tag=Cyc_DeclSpec_tok;_tmp3C7.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3C8->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3C8->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3C8->is_inline=1;_tmp3C8->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3C8;});
_tmp3C7;});_tmp3C6;});break;case 36: _LL1FE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CA;_tmp3CA.tag=Cyc_DeclSpec_tok;_tmp3CA.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->sc=0;_tmp3CB->tq=Cyc_Absyn_empty_tqual();
_tmp3CB->type_specs=0;_tmp3CB->is_inline=0;_tmp3CB->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CB;});_tmp3CA;});
_tmp3C9;});break;case 37: _LL1FF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CD;_tmp3CD.tag=Cyc_DeclSpec_tok;_tmp3CD.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3CE->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3CE->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3CE->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3CE->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3CE;});
_tmp3CD;});_tmp3CC;});break;case 38: _LL200: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D0;_tmp3D0.tag=Cyc_StorageClass_tok;_tmp3D0.f1=(void*)((void*)4);_tmp3D0;});
_tmp3CF;});break;case 39: _LL201: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_StorageClass_tok;_tmp3D2.f1=(void*)((void*)5);_tmp3D2;});
_tmp3D1;});break;case 40: _LL202: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D4;_tmp3D4.tag=Cyc_StorageClass_tok;_tmp3D4.f1=(void*)((void*)3);_tmp3D4;});
_tmp3D3;});break;case 41: _LL203: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D6;_tmp3D6.tag=Cyc_StorageClass_tok;_tmp3D6.f1=(void*)((void*)1);_tmp3D6;});
_tmp3D5;});break;case 42: _LL204: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3D7="C";
_tag_arr(_tmp3D7,sizeof(char),_get_zero_arr_size(_tmp3D7,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3D8="only extern or extern \"C\" is allowed";_tag_arr(_tmp3D8,
sizeof(char),_get_zero_arr_size(_tmp3D8,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=({
struct Cyc_StorageClass_tok_struct _tmp3DA;_tmp3DA.tag=Cyc_StorageClass_tok;
_tmp3DA.f1=(void*)((void*)2);_tmp3DA;});_tmp3D9;});break;case 43: _LL205: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));
_tmp3DB[0]=({struct Cyc_StorageClass_tok_struct _tmp3DC;_tmp3DC.tag=Cyc_StorageClass_tok;
_tmp3DC.f1=(void*)((void*)0);_tmp3DC;});_tmp3DB;});break;case 44: _LL206: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD[0]=({struct Cyc_StorageClass_tok_struct _tmp3DE;_tmp3DE.tag=Cyc_StorageClass_tok;
_tmp3DE.f1=(void*)((void*)6);_tmp3DE;});_tmp3DD;});break;case 45: _LL207: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));
_tmp3DF[0]=({struct Cyc_AttributeList_tok_struct _tmp3E0;_tmp3E0.tag=Cyc_AttributeList_tok;
_tmp3E0.f1=0;_tmp3E0;});_tmp3DF;});break;case 46: _LL208: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 47: _LL209: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 48: _LL20A:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3E1=_cycalloc(sizeof(*
_tmp3E1));_tmp3E1[0]=({struct Cyc_AttributeList_tok_struct _tmp3E2;_tmp3E2.tag=Cyc_AttributeList_tok;
_tmp3E2.f1=({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3E3->tl=0;_tmp3E3;});_tmp3E2;});_tmp3E1;});break;case 49:
_LL20B: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3E4=_cycalloc(
sizeof(*_tmp3E4));_tmp3E4[0]=({struct Cyc_AttributeList_tok_struct _tmp3E5;_tmp3E5.tag=
Cyc_AttributeList_tok;_tmp3E5.f1=({struct Cyc_List_List*_tmp3E6=_cycalloc(sizeof(*
_tmp3E6));_tmp3E6->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E6->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E6;});_tmp3E5;});
_tmp3E4;});break;case 50: _LL20C: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple17 att_map[17]={{{_tmp3ED,_tmp3ED,_tmp3ED + 8},(
void*)0},{{_tmp3EE,_tmp3EE,_tmp3EE + 6},(void*)1},{{_tmp3EF,_tmp3EF,_tmp3EF + 9},(
void*)2},{{_tmp3F0,_tmp3F0,_tmp3F0 + 9},(void*)3},{{_tmp3F1,_tmp3F1,_tmp3F1 + 6},(
void*)4},{{_tmp3F2,_tmp3F2,_tmp3F2 + 8},(void*)& att_aligned},{{_tmp3F3,_tmp3F3,
_tmp3F3 + 7},(void*)5},{{_tmp3F4,_tmp3F4,_tmp3F4 + 7},(void*)7},{{_tmp3F5,_tmp3F5,
_tmp3F5 + 7},(void*)8},{{_tmp3F6,_tmp3F6,_tmp3F6 + 5},(void*)9},{{_tmp3F7,_tmp3F7,
_tmp3F7 + 10},(void*)10},{{_tmp3F8,_tmp3F8,_tmp3F8 + 10},(void*)11},{{_tmp3F9,
_tmp3F9,_tmp3F9 + 23},(void*)12},{{_tmp3FA,_tmp3FA,_tmp3FA + 12},(void*)13},{{
_tmp3FB,_tmp3FB,_tmp3FB + 11},(void*)14},{{_tmp3FC,_tmp3FC,_tmp3FC + 22},(void*)15},{{
_tmp3FD,_tmp3FD,_tmp3FD + 5},(void*)16}};struct _tagged_arr _tmp3E7=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp3E7,sizeof(char))> 4?*((const char*)_check_unknown_subscript(_tmp3E7,sizeof(
char),0))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E7,sizeof(char),1))
== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E7,sizeof(char),(int)(
_get_arr_size(_tmp3E7,sizeof(char))- 2)))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp3E7,sizeof(char),(int)(_get_arr_size(_tmp3E7,
sizeof(char))- 3)))== '_': 0)_tmp3E7=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp3E7,2,_get_arr_size(_tmp3E7,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp3E7,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3E8=_cycalloc(sizeof(*
_tmp3E8));_tmp3E8[0]=({struct Cyc_Attribute_tok_struct _tmp3E9;_tmp3E9.tag=Cyc_Attribute_tok;
_tmp3E9.f1=(void*)(att_map[i]).f2;_tmp3E9;});_tmp3E8;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp3EA="unrecognized attribute";_tag_arr(_tmp3EA,sizeof(char),
_get_zero_arr_size(_tmp3EA,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({
struct Cyc_Attribute_tok_struct _tmp3EC;_tmp3EC.tag=Cyc_Attribute_tok;_tmp3EC.f1=(
void*)((void*)1);_tmp3EC;});_tmp3EB;});}break;}}case 51: _LL20D: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({
struct Cyc_Attribute_tok_struct _tmp400;_tmp400.tag=Cyc_Attribute_tok;_tmp400.f1=(
void*)((void*)4);_tmp400;});_tmp3FF;});break;case 52: _LL20E: {struct _tagged_arr
_tmp401=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp403;int _tmp404;struct _tuple13*_tmp402=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp403=*_tmp402;
_tmp404=_tmp403.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp401,({const char*
_tmp405="regparm";_tag_arr(_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,8));}))
== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp401,({const char*_tmp406="__regparm__";
_tag_arr(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,12));}))== 0){if(
_tmp404 < 0?1: _tmp404 > 3)Cyc_Parse_err(({const char*_tmp407="regparm requires value between 0 and 3";
_tag_arr(_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp408=_cycalloc_atomic(sizeof(*_tmp408));
_tmp408[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp409;_tmp409.tag=0;_tmp409.f1=
_tmp404;_tmp409;});_tmp408;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp401,({
const char*_tmp40A="aligned";_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(
_tmp40A,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp401,({const char*_tmp40B="__aligned__";
_tag_arr(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,12));}))== 0){if(
_tmp404 < 0)Cyc_Parse_err(({const char*_tmp40C="aligned requires positive power of two";
_tag_arr(_tmp40C,sizeof(char),_get_zero_arr_size(_tmp40C,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp404;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp40D="aligned requires positive power of two";
_tag_arr(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp40E=_cycalloc_atomic(sizeof(*_tmp40E));
_tmp40E[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp40F;_tmp40F.tag=1;_tmp40F.f1=
_tmp404;_tmp40F;});_tmp40E;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp401,({
const char*_tmp410="initializes";_tag_arr(_tmp410,sizeof(char),_get_zero_arr_size(
_tmp410,12));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp401,({const char*
_tmp411="__initializes__";_tag_arr(_tmp411,sizeof(char),_get_zero_arr_size(
_tmp411,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp412=
_cycalloc_atomic(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp413;_tmp413.tag=4;_tmp413.f1=_tmp404;_tmp413;});_tmp412;});else{Cyc_Parse_err(({
const char*_tmp414="unrecognized attribute";_tag_arr(_tmp414,sizeof(char),
_get_zero_arr_size(_tmp414,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp415=_cycalloc(sizeof(*_tmp415));
_tmp415[0]=({struct Cyc_Attribute_tok_struct _tmp416;_tmp416.tag=Cyc_Attribute_tok;
_tmp416.f1=(void*)a;_tmp416;});_tmp415;});break;}}case 53: _LL20F: {struct
_tagged_arr _tmp417=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp418=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp417,({const char*_tmp419="section";_tag_arr(_tmp419,sizeof(
char),_get_zero_arr_size(_tmp419,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)
_tmp417,({const char*_tmp41A="__section__";_tag_arr(_tmp41A,sizeof(char),
_get_zero_arr_size(_tmp41A,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_Absyn_Section_att_struct
_tmp41C;_tmp41C.tag=2;_tmp41C.f1=_tmp418;_tmp41C;});_tmp41B;});else{Cyc_Parse_err(({
const char*_tmp41D="unrecognized attribute";_tag_arr(_tmp41D,sizeof(char),
_get_zero_arr_size(_tmp41D,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));
_tmp41E[0]=({struct Cyc_Attribute_tok_struct _tmp41F;_tmp41F.tag=Cyc_Attribute_tok;
_tmp41F.f1=(void*)a;_tmp41F;});_tmp41E;});break;}case 54: _LL210: {struct
_tagged_arr _tmp420=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp421=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp423;struct
_tuple13 _tmp422=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp423=_tmp422.f2;{int _tmp425;struct _tuple13 _tmp424=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp425=_tmp424.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp420,({const char*_tmp426="format";
_tag_arr(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,7));}))== 0?1: Cyc_zstrcmp((
struct _tagged_arr)_tmp420,({const char*_tmp427="__format__";_tag_arr(_tmp427,
sizeof(char),_get_zero_arr_size(_tmp427,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp421,({const char*_tmp428="printf";_tag_arr(_tmp428,sizeof(char),
_get_zero_arr_size(_tmp428,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Absyn_Format_att_struct
_tmp42A;_tmp42A.tag=3;_tmp42A.f1=(void*)((void*)0);_tmp42A.f2=_tmp423;_tmp42A.f3=
_tmp425;_tmp42A;});_tmp429;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp421,({
const char*_tmp42B="scanf";_tag_arr(_tmp42B,sizeof(char),_get_zero_arr_size(
_tmp42B,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp42C=
_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_Absyn_Format_att_struct
_tmp42D;_tmp42D.tag=3;_tmp42D.f1=(void*)((void*)1);_tmp42D.f2=_tmp423;_tmp42D.f3=
_tmp425;_tmp42D;});_tmp42C;});else{Cyc_Parse_err(({const char*_tmp42E="unrecognized format type";
_tag_arr(_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp42F="unrecognized attribute";_tag_arr(_tmp42F,sizeof(char),
_get_zero_arr_size(_tmp42F,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=({
struct Cyc_Attribute_tok_struct _tmp431;_tmp431.tag=Cyc_Attribute_tok;_tmp431.f1=(
void*)a;_tmp431;});_tmp430;});break;}}}case 55: _LL211: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp433;_tmp433.tag=Cyc_TypeSpecifier_tok;_tmp433.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp433;});_tmp432;});break;case 56: _LL212: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp434=_cycalloc(sizeof(*_tmp434));
_tmp434[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp435;_tmp435.tag=Cyc_TypeSpecifier_tok;
_tmp435.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp435;});
_tmp434;});break;case 57: _LL213: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp437;_tmp437.tag=Cyc_TypeSpecifier_tok;_tmp437.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));
_tmp438->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp438;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp437;});
_tmp436;});break;case 58: _LL214: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43A;_tmp43A.tag=Cyc_TypeSpecifier_tok;_tmp43A.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43A;});
_tmp439;});break;case 59: _LL215: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43C;_tmp43C.tag=Cyc_TypeSpecifier_tok;_tmp43C.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Parse_Short_spec_struct
_tmp43E;_tmp43E.tag=2;_tmp43E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp43E;});
_tmp43D;}));_tmp43C;});_tmp43B;});break;case 60: _LL216: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp440;_tmp440.tag=Cyc_TypeSpecifier_tok;_tmp440.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp440;});
_tmp43F;});break;case 61: _LL217: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp442;_tmp442.tag=Cyc_TypeSpecifier_tok;_tmp442.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_Parse_Long_spec_struct
_tmp444;_tmp444.tag=3;_tmp444.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp444;});
_tmp443;}));_tmp442;});_tmp441;});break;case 62: _LL218: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp446;_tmp446.tag=Cyc_TypeSpecifier_tok;_tmp446.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp446;});
_tmp445;});break;case 63: _LL219: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp448;_tmp448.tag=Cyc_TypeSpecifier_tok;_tmp448.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp448;});
_tmp447;});break;case 64: _LL21A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44A;_tmp44A.tag=Cyc_TypeSpecifier_tok;_tmp44A.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp44C;_tmp44C.tag=0;_tmp44C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp44C;});
_tmp44B;}));_tmp44A;});_tmp449;});break;case 65: _LL21B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44E;_tmp44E.tag=Cyc_TypeSpecifier_tok;_tmp44E.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp450;_tmp450.tag=1;_tmp450.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp450;});
_tmp44F;}));_tmp44E;});_tmp44D;});break;case 66: _LL21C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 67: _LL21D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68: _LL21E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69:
_LL21F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp451=_cycalloc(
sizeof(*_tmp451));_tmp451[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp452;_tmp452.tag=
Cyc_TypeSpecifier_tok;_tmp452.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp452;});
_tmp451;});break;case 70: _LL220: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp454;_tmp454.tag=Cyc_TypeSpecifier_tok;_tmp454.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp455=_cycalloc(sizeof(*_tmp455));
_tmp455[0]=({struct Cyc_Absyn_TypedefType_struct _tmp456;_tmp456.tag=16;_tmp456.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp456.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp456.f3=0;_tmp456.f4=0;_tmp456;});_tmp455;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp454;});
_tmp453;});break;case 71: _LL221: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp458;_tmp458.tag=Cyc_TypeSpecifier_tok;_tmp458.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp459=_cycalloc(sizeof(*_tmp459));
_tmp459[0]=({struct Cyc_Absyn_TupleType_struct _tmp45A;_tmp45A.tag=9;_tmp45A.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp45A;});_tmp459;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp458;});_tmp457;});break;case 72: _LL222: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));
_tmp45B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp45C;_tmp45C.tag=Cyc_TypeSpecifier_tok;
_tmp45C.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp45E;_tmp45E.tag=15;_tmp45E.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45E;});_tmp45D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp45C;});_tmp45B;});break;case 73: _LL223: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));
_tmp45F[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp460;_tmp460.tag=Cyc_TypeSpecifier_tok;
_tmp460.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp462;_tmp462.tag=14;_tmp462.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp462;});_tmp461;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp460;});_tmp45F;});break;case 74: _LL224: {struct
_tuple13 _tmp464;int _tmp465;struct _tuple13*_tmp463=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp464=*_tmp463;_tmp465=
_tmp464.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp467;_tmp467.tag=
Cyc_TypeSpecifier_tok;_tmp467.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp468=_cycalloc_atomic(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp469;_tmp469.tag=18;_tmp469.f1=_tmp465;_tmp469;});_tmp468;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp467;});
_tmp466;});break;}case 75: _LL225: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp46B;_tmp46B.tag=Cyc_TypeSpecifier_tok;_tmp46B.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));
_tmp46C[0]=({struct Cyc_Absyn_TagType_struct _tmp46D;_tmp46D.tag=17;_tmp46D.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp46D;});_tmp46C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46B;});
_tmp46A;});break;case 76: _LL226: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp46E=
_cycalloc(sizeof(*_tmp46E));_tmp46E[0]=({struct Cyc_Kind_tok_struct _tmp46F;
_tmp46F.tag=Cyc_Kind_tok;_tmp46F.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46F;});
_tmp46E;});break;case 77: _LL227: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp470=_cycalloc_atomic(sizeof(*_tmp470));_tmp470[0]=({struct Cyc_TypeQual_tok_struct
_tmp471;_tmp471.tag=Cyc_TypeQual_tok;_tmp471.f1=({struct Cyc_Absyn_Tqual _tmp472;
_tmp472.q_const=1;_tmp472.q_volatile=0;_tmp472.q_restrict=0;_tmp472;});_tmp471;});
_tmp470;});break;case 78: _LL228: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp473=_cycalloc_atomic(sizeof(*_tmp473));_tmp473[0]=({struct Cyc_TypeQual_tok_struct
_tmp474;_tmp474.tag=Cyc_TypeQual_tok;_tmp474.f1=({struct Cyc_Absyn_Tqual _tmp475;
_tmp475.q_const=0;_tmp475.q_volatile=1;_tmp475.q_restrict=0;_tmp475;});_tmp474;});
_tmp473;});break;case 79: _LL229: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp476=_cycalloc_atomic(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_TypeQual_tok_struct
_tmp477;_tmp477.tag=Cyc_TypeQual_tok;_tmp477.f1=({struct Cyc_Absyn_Tqual _tmp478;
_tmp478.q_const=0;_tmp478.q_volatile=0;_tmp478.q_restrict=1;_tmp478;});_tmp477;});
_tmp476;});break;case 80: _LL22A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47A;_tmp47A.tag=Cyc_TypeSpecifier_tok;_tmp47A.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp47C;_tmp47C.tag=5;_tmp47C.f1=({struct Cyc_Absyn_Decl*_tmp47D=_cycalloc(
sizeof(*_tmp47D));_tmp47D->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp47F;_tmp47F.tag=6;_tmp47F.f1=({struct Cyc_Absyn_Enumdecl*_tmp480=_cycalloc(
sizeof(*_tmp480));_tmp480->sc=(void*)((void*)2);_tmp480->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp480->fields=({
struct Cyc_Core_Opt*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp481;});_tmp480;});
_tmp47F;});_tmp47E;}));_tmp47D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp47D;});
_tmp47C;});_tmp47B;}));_tmp47A;});_tmp479;});break;case 81: _LL22B: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp483;_tmp483.tag=Cyc_TypeSpecifier_tok;
_tmp483.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484[0]=({struct Cyc_Absyn_EnumType_struct
_tmp485;_tmp485.tag=12;_tmp485.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp485.f2=0;_tmp485;});
_tmp484;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp483;});_tmp482;});break;case 82: _LL22C: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp486=_cycalloc(sizeof(*_tmp486));
_tmp486[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp487;_tmp487.tag=Cyc_TypeSpecifier_tok;
_tmp487.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp488=_cycalloc(
sizeof(*_tmp488));_tmp488[0]=({struct Cyc_Parse_Type_spec_struct _tmp489;_tmp489.tag=
4;_tmp489.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp48A=
_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp48B;_tmp48B.tag=13;_tmp48B.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48B;});_tmp48A;}));
_tmp489.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp489;});_tmp488;}));_tmp487;});_tmp486;});break;
case 83: _LL22D: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp48C=_cycalloc(
sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Enumfield_tok_struct _tmp48D;_tmp48D.tag=
Cyc_Enumfield_tok;_tmp48D.f1=({struct Cyc_Absyn_Enumfield*_tmp48E=_cycalloc(
sizeof(*_tmp48E));_tmp48E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp48E->tag=0;_tmp48E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp48E;});_tmp48D;});_tmp48C;});break;case 84: _LL22E:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F[0]=({struct Cyc_Enumfield_tok_struct _tmp490;_tmp490.tag=Cyc_Enumfield_tok;
_tmp490.f1=({struct Cyc_Absyn_Enumfield*_tmp491=_cycalloc(sizeof(*_tmp491));
_tmp491->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp491->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp491->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp491;});
_tmp490;});_tmp48F;});break;case 85: _LL22F: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp493;_tmp493.tag=Cyc_EnumfieldList_tok;_tmp493.f1=({struct Cyc_List_List*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp494->tl=0;_tmp494;});
_tmp493;});_tmp492;});break;case 86: _LL230: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp496;_tmp496.tag=Cyc_EnumfieldList_tok;_tmp496.f1=({struct Cyc_List_List*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp497->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp497;});_tmp496;});
_tmp495;});break;case 87: _LL231: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp499;_tmp499.tag=Cyc_TypeSpecifier_tok;_tmp499.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));
_tmp49A[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp49B;_tmp49B.tag=11;_tmp49B.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp49B.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49B;});_tmp49A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp499;});_tmp498;});break;case 88: _LL232: {struct
Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp49D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp49F;_tmp49F.tag=Cyc_TypeSpecifier_tok;
_tmp49F.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4A0=_cycalloc(
sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4A1;_tmp4A1.tag=
5;_tmp4A1.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp49C,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp49D,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A1;});
_tmp4A0;}));_tmp49F;});_tmp49E;});break;}case 89: _LL233: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_TypeSpecifier_tok;_tmp4A3.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));
_tmp4A4[0]=({struct Cyc_Absyn_AggrType_struct _tmp4A5;_tmp4A5.tag=10;_tmp4A5.f1=({
struct Cyc_Absyn_AggrInfo _tmp4A6;_tmp4A6.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A8.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A8;});_tmp4A7;}));
_tmp4A6.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4A6;});_tmp4A5;});_tmp4A4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A3;});
_tmp4A2;});break;case 90: _LL234: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_TypeList_tok_struct
_tmp4AA;_tmp4AA.tag=Cyc_TypeList_tok;_tmp4AA.f1=0;_tmp4AA;});_tmp4A9;});break;
case 91: _LL235: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp4AB=_cycalloc(
sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_TypeList_tok_struct _tmp4AC;_tmp4AC.tag=
Cyc_TypeList_tok;_tmp4AC.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4AC;});_tmp4AB;});
break;case 92: _LL236: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp4AD=
_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4AE;_tmp4AE.tag=Cyc_StructOrUnion_tok;_tmp4AE.f1=(void*)((void*)0);_tmp4AE;});
_tmp4AD;});break;case 93: _LL237: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4B0;_tmp4B0.tag=Cyc_StructOrUnion_tok;_tmp4B0.f1=(void*)((void*)1);_tmp4B0;});
_tmp4AF;});break;case 94: _LL238: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4B2;_tmp4B2.tag=Cyc_AggrFieldDeclList_tok;_tmp4B2.f1=0;_tmp4B2;});_tmp4B1;});
break;case 95: _LL239: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4B4;_tmp4B4.tag=Cyc_AggrFieldDeclList_tok;_tmp4B4.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4B4;});_tmp4B3;});
break;case 96: _LL23A: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4B6;_tmp4B6.tag=Cyc_AggrFieldDeclListList_tok;_tmp4B6.f1=({struct Cyc_List_List*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B7->tl=0;_tmp4B7;});
_tmp4B6;});_tmp4B5;});break;case 97: _LL23B: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4B9;_tmp4B9.tag=Cyc_AggrFieldDeclListList_tok;_tmp4B9.f1=({struct Cyc_List_List*
_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4BA->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BA;});_tmp4B9;});
_tmp4B8;});break;case 98: _LL23C: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4BC;_tmp4BC.tag=Cyc_InitDeclList_tok;_tmp4BC.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4BC;});_tmp4BB;});
break;case 99: _LL23D: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4BD=
_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4BE;_tmp4BE.tag=Cyc_InitDeclList_tok;_tmp4BE.f1=({struct Cyc_List_List*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4BF->tl=0;_tmp4BF;});
_tmp4BE;});_tmp4BD;});break;case 100: _LL23E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4C1;_tmp4C1.tag=Cyc_InitDeclList_tok;_tmp4C1.f1=({struct Cyc_List_List*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C2->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C2;});_tmp4C1;});
_tmp4C0;});break;case 101: _LL23F: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_InitDecl_tok_struct
_tmp4C4;_tmp4C4.tag=Cyc_InitDecl_tok;_tmp4C4.f1=({struct _tuple12*_tmp4C5=
_cycalloc(sizeof(*_tmp4C5));_tmp4C5->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C5->f2=0;_tmp4C5;});
_tmp4C4;});_tmp4C3;});break;case 102: _LL240: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_InitDecl_tok_struct
_tmp4C7;_tmp4C7.tag=Cyc_InitDecl_tok;_tmp4C7.f1=({struct _tuple12*_tmp4C8=
_cycalloc(sizeof(*_tmp4C8));_tmp4C8->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C8->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4C8;});_tmp4C7;});_tmp4C6;});break;case 103: _LL241: {struct _tuple15 _tmp4CA;
struct Cyc_Absyn_Tqual _tmp4CB;struct Cyc_List_List*_tmp4CC;struct Cyc_List_List*
_tmp4CD;struct _tuple15*_tmp4C9=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4CA=*_tmp4C9;_tmp4CB=
_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;_tmp4CD=_tmp4CA.f3;{struct Cyc_List_List*_tmp4CF;
struct Cyc_List_List*_tmp4D0;struct _tuple0 _tmp4CE=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4CF=_tmp4CE.f1;
_tmp4D0=_tmp4CE.f2;{void*_tmp4D1=Cyc_Parse_speclist2typ(_tmp4CC,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4CB,_tmp4D1,_tmp4CF,_tmp4CD),_tmp4D0);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4D3=_cycalloc(sizeof(*
_tmp4D3));_tmp4D3[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp4D4;_tmp4D4.tag=
Cyc_AggrFieldDeclList_tok;_tmp4D4.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4D2);
_tmp4D4;});_tmp4D3;});break;}}}case 104: _LL242: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4D6;_tmp4D6.tag=Cyc_QualSpecList_tok;_tmp4D6.f1=({struct _tuple15*_tmp4D7=
_cycalloc(sizeof(*_tmp4D7));_tmp4D7->f1=Cyc_Absyn_empty_tqual();_tmp4D7->f2=({
struct Cyc_List_List*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D8->tl=0;_tmp4D8;});
_tmp4D7->f3=0;_tmp4D7;});_tmp4D6;});_tmp4D5;});break;case 105: _LL243: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({
struct Cyc_QualSpecList_tok_struct _tmp4DA;_tmp4DA.tag=Cyc_QualSpecList_tok;
_tmp4DA.f1=({struct _tuple15*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4DB->f2=({struct Cyc_List_List*_tmp4DC=_cycalloc(sizeof(*
_tmp4DC));_tmp4DC->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DC->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4DC;});_tmp4DB->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp4DB;});_tmp4DA;});_tmp4D9;});break;case 106: _LL244: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));
_tmp4DD[0]=({struct Cyc_QualSpecList_tok_struct _tmp4DE;_tmp4DE.tag=Cyc_QualSpecList_tok;
_tmp4DE.f1=({struct _tuple15*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4DF->f2=0;_tmp4DF->f3=
0;_tmp4DF;});_tmp4DE;});_tmp4DD;});break;case 107: _LL245: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4E1;_tmp4E1.tag=Cyc_QualSpecList_tok;_tmp4E1.f1=({struct _tuple15*_tmp4E2=
_cycalloc(sizeof(*_tmp4E2));_tmp4E2->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp4E2->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4E2->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4E2;});_tmp4E1;});
_tmp4E0;});break;case 108: _LL246: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4E4;_tmp4E4.tag=Cyc_QualSpecList_tok;_tmp4E4.f1=({struct _tuple15*_tmp4E5=
_cycalloc(sizeof(*_tmp4E5));_tmp4E5->f1=Cyc_Absyn_empty_tqual();_tmp4E5->f2=0;
_tmp4E5->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E5;});_tmp4E4;});_tmp4E3;});break;case 109: _LL247:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp4E6=_cycalloc(sizeof(*
_tmp4E6));_tmp4E6[0]=({struct Cyc_QualSpecList_tok_struct _tmp4E7;_tmp4E7.tag=Cyc_QualSpecList_tok;
_tmp4E7.f1=({struct _tuple15*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4E8->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4E8->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp4E8;});_tmp4E7;});
_tmp4E6;});break;case 110: _LL248: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4EA;_tmp4EA.tag=Cyc_InitDeclList_tok;_tmp4EA.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4EA;});_tmp4E9;});
break;case 111: _LL249: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4EB=
_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4EC;_tmp4EC.tag=Cyc_InitDeclList_tok;_tmp4EC.f1=({struct Cyc_List_List*
_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4ED->tl=0;_tmp4ED;});
_tmp4EC;});_tmp4EB;});break;case 112: _LL24A: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4EF;_tmp4EF.tag=Cyc_InitDeclList_tok;_tmp4EF.f1=({struct Cyc_List_List*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F0->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F0;});_tmp4EF;});
_tmp4EE;});break;case 113: _LL24B: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_InitDecl_tok_struct
_tmp4F2;_tmp4F2.tag=Cyc_InitDecl_tok;_tmp4F2.f1=({struct _tuple12*_tmp4F3=
_cycalloc(sizeof(*_tmp4F3));_tmp4F3->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F3->f2=0;_tmp4F3;});
_tmp4F2;});_tmp4F1;});break;case 114: _LL24C: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_InitDecl_tok_struct
_tmp4F5;_tmp4F5.tag=Cyc_InitDecl_tok;_tmp4F5.f1=({struct _tuple12*_tmp4F6=
_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=({struct Cyc_Parse_Declarator*_tmp4F7=
_cycalloc(sizeof(*_tmp4F7));_tmp4F7->id=({struct _tuple1*_tmp4F8=_cycalloc(
sizeof(*_tmp4F8));_tmp4F8->f1=Cyc_Absyn_rel_ns_null;_tmp4F8->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp4F8;});_tmp4F7->tms=
0;_tmp4F7;});_tmp4F6->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F6;});_tmp4F5;});
_tmp4F4;});break;case 115: _LL24D: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA[0]=({struct Cyc_InitDecl_tok_struct
_tmp4FB;_tmp4FB.tag=Cyc_InitDecl_tok;_tmp4FB.f1=({struct _tuple12*_tmp4FC=
_cycalloc(sizeof(*_tmp4FC));_tmp4FC->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FC->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4FC;});_tmp4FB;});_tmp4FA;});break;case 116: _LL24E: {struct Cyc_List_List*
_tmp4FD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4FF;_tmp4FF.tag=Cyc_TypeSpecifier_tok;
_tmp4FF.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp500=_cycalloc(
sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Parse_Decl_spec_struct _tmp501;_tmp501.tag=
5;_tmp501.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp4FD,({struct Cyc_Core_Opt*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp502;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp501;});
_tmp500;}));_tmp4FF;});_tmp4FE;});break;}case 117: _LL24F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp504;_tmp504.tag=Cyc_TypeSpecifier_tok;_tmp504.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp505=_cycalloc(sizeof(*_tmp505));
_tmp505[0]=({struct Cyc_Absyn_TunionType_struct _tmp506;_tmp506.tag=2;_tmp506.f1=({
struct Cyc_Absyn_TunionInfo _tmp507;_tmp507.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp509;_tmp509.tag=0;_tmp509.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp50A;
_tmp50A.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp50A.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp50A;});_tmp509;});
_tmp508;}));_tmp507.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp507.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp507;});_tmp506;});
_tmp505;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp504;});_tmp503;});break;case 118: _LL250: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp50B=_cycalloc(sizeof(*_tmp50B));
_tmp50B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp50C;_tmp50C.tag=Cyc_TypeSpecifier_tok;
_tmp50C.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp50E;_tmp50E.tag=3;_tmp50E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp50F;
_tmp50F.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp511;_tmp511.tag=0;_tmp511.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp512;
_tmp512.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp512.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp512.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp512;});_tmp511;});
_tmp510;}));_tmp50F.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50F;});_tmp50E;});
_tmp50D;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp50C;});_tmp50B;});break;case 119: _LL251: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp513=_cycalloc_atomic(sizeof(*_tmp513));
_tmp513[0]=({struct Cyc_Bool_tok_struct _tmp514;_tmp514.tag=Cyc_Bool_tok;_tmp514.f1=
0;_tmp514;});_tmp513;});break;case 120: _LL252: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp515=_cycalloc_atomic(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_Bool_tok_struct
_tmp516;_tmp516.tag=Cyc_Bool_tok;_tmp516.f1=1;_tmp516;});_tmp515;});break;case
121: _LL253: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp517=_cycalloc(
sizeof(*_tmp517));_tmp517[0]=({struct Cyc_TunionFieldList_tok_struct _tmp518;
_tmp518.tag=Cyc_TunionFieldList_tok;_tmp518.f1=({struct Cyc_List_List*_tmp519=
_cycalloc(sizeof(*_tmp519));_tmp519->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519->tl=0;_tmp519;});
_tmp518;});_tmp517;});break;case 122: _LL254: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp51B;_tmp51B.tag=Cyc_TunionFieldList_tok;_tmp51B.f1=({struct Cyc_List_List*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51C->tl=0;_tmp51C;});
_tmp51B;});_tmp51A;});break;case 123: _LL255: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp51E;_tmp51E.tag=Cyc_TunionFieldList_tok;_tmp51E.f1=({struct Cyc_List_List*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51F->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F;});_tmp51E;});
_tmp51D;});break;case 124: _LL256: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp521;_tmp521.tag=Cyc_TunionFieldList_tok;_tmp521.f1=({struct Cyc_List_List*
_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp522->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp522;});_tmp521;});
_tmp520;});break;case 125: _LL257: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523[0]=({struct Cyc_Scope_tok_struct
_tmp524;_tmp524.tag=Cyc_Scope_tok;_tmp524.f1=(void*)((void*)2);_tmp524;});
_tmp523;});break;case 126: _LL258: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525[0]=({struct Cyc_Scope_tok_struct
_tmp526;_tmp526.tag=Cyc_Scope_tok;_tmp526.f1=(void*)((void*)3);_tmp526;});
_tmp525;});break;case 127: _LL259: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527[0]=({struct Cyc_Scope_tok_struct
_tmp528;_tmp528.tag=Cyc_Scope_tok;_tmp528.f1=(void*)((void*)0);_tmp528;});
_tmp527;});break;case 128: _LL25A: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529[0]=({struct Cyc_TunionField_tok_struct
_tmp52A;_tmp52A.tag=Cyc_TunionField_tok;_tmp52A.f1=({struct Cyc_Absyn_Tunionfield*
_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52B->typs=0;_tmp52B->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp52B->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52B;});_tmp52A;});
_tmp529;});break;case 129: _LL25B: {struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D[0]=({
struct Cyc_TunionField_tok_struct _tmp52E;_tmp52E.tag=Cyc_TunionField_tok;_tmp52E.f1=({
struct Cyc_Absyn_Tunionfield*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp52F->typs=_tmp52C;_tmp52F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp52F->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp52F;});_tmp52E;});_tmp52D;});break;}case 130: _LL25C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 131: _LL25D: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp530=_cycalloc(sizeof(*_tmp530));
_tmp530[0]=({struct Cyc_Declarator_tok_struct _tmp531;_tmp531.tag=Cyc_Declarator_tok;
_tmp531.f1=({struct Cyc_Parse_Declarator*_tmp532=_cycalloc(sizeof(*_tmp532));
_tmp532->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp532->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp532;});
_tmp531;});_tmp530;});break;case 132: _LL25E: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533[0]=({struct Cyc_Declarator_tok_struct
_tmp534;_tmp534.tag=Cyc_Declarator_tok;_tmp534.f1=({struct Cyc_Parse_Declarator*
_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp535->tms=0;_tmp535;});
_tmp534;});_tmp533;});break;case 133: _LL25F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 134: _LL260:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp536=_cycalloc(sizeof(*_tmp536));
_tmp536[0]=({struct Cyc_Declarator_tok_struct _tmp537;_tmp537.tag=Cyc_Declarator_tok;
_tmp537.f1=({struct Cyc_Parse_Declarator*_tmp538=_cycalloc(sizeof(*_tmp538));
_tmp538->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp538->tms=({struct Cyc_List_List*_tmp539=_cycalloc(
sizeof(*_tmp539));_tmp539->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp53B;_tmp53B.tag=0;_tmp53B.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53B;});_tmp53A;}));
_tmp539->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp539;});_tmp538;});_tmp537;});_tmp536;});break;case
135: _LL261: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp53C=_cycalloc(
sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Declarator_tok_struct _tmp53D;_tmp53D.tag=
Cyc_Declarator_tok;_tmp53D.f1=({struct Cyc_Parse_Declarator*_tmp53E=_cycalloc(
sizeof(*_tmp53E));_tmp53E->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp53E->tms=({
struct Cyc_List_List*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp540=_cycalloc(sizeof(*_tmp540));
_tmp540[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp541;_tmp541.tag=1;_tmp541.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp541.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp541;});_tmp540;}));_tmp53F->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp53F;});_tmp53E;});
_tmp53D;});_tmp53C;});break;case 136: _LL262: {struct _tuple14 _tmp543;struct Cyc_List_List*
_tmp544;int _tmp545;struct Cyc_Absyn_VarargInfo*_tmp546;struct Cyc_Core_Opt*_tmp547;
struct Cyc_List_List*_tmp548;struct _tuple14*_tmp542=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp543=*_tmp542;_tmp544=
_tmp543.f1;_tmp545=_tmp543.f2;_tmp546=_tmp543.f3;_tmp547=_tmp543.f4;_tmp548=
_tmp543.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp549=_cycalloc(
sizeof(*_tmp549));_tmp549[0]=({struct Cyc_Declarator_tok_struct _tmp54A;_tmp54A.tag=
Cyc_Declarator_tok;_tmp54A.f1=({struct Cyc_Parse_Declarator*_tmp54B=_cycalloc(
sizeof(*_tmp54B));_tmp54B->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp54B->tms=({
struct Cyc_List_List*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));
_tmp54D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp54E;_tmp54E.tag=3;_tmp54E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp54F=_cycalloc(sizeof(*
_tmp54F));_tmp54F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp550;_tmp550.tag=1;
_tmp550.f1=_tmp544;_tmp550.f2=_tmp545;_tmp550.f3=_tmp546;_tmp550.f4=_tmp547;
_tmp550.f5=_tmp548;_tmp550;});_tmp54F;}));_tmp54E;});_tmp54D;}));_tmp54C->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp54C;});_tmp54B;});_tmp54A;});_tmp549;});break;}case 137: _LL263:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp551=_cycalloc(sizeof(*_tmp551));
_tmp551[0]=({struct Cyc_Declarator_tok_struct _tmp552;_tmp552.tag=Cyc_Declarator_tok;
_tmp552.f1=({struct Cyc_Parse_Declarator*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp553->tms=({struct Cyc_List_List*_tmp554=_cycalloc(
sizeof(*_tmp554));_tmp554->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp556;_tmp556.tag=3;_tmp556.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp558;_tmp558.tag=1;_tmp558.f1=0;_tmp558.f2=0;_tmp558.f3=0;_tmp558.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp558.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp558;});_tmp557;}));
_tmp556;});_tmp555;}));_tmp554->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp554;});_tmp553;});
_tmp552;});_tmp551;});break;case 138: _LL264: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Declarator_tok_struct
_tmp55A;_tmp55A.tag=Cyc_Declarator_tok;_tmp55A.f1=({struct Cyc_Parse_Declarator*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp55B->tms=({
struct Cyc_List_List*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));
_tmp55D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp55E;_tmp55E.tag=3;_tmp55E.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F[0]=({struct Cyc_Absyn_NoTypes_struct _tmp560;_tmp560.tag=0;_tmp560.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp560.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp560;});
_tmp55F;}));_tmp55E;});_tmp55D;}));_tmp55C->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp55C;});_tmp55B;});
_tmp55A;});_tmp559;});break;case 139: _LL265: {struct Cyc_List_List*_tmp561=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({
struct Cyc_Declarator_tok_struct _tmp563;_tmp563.tag=Cyc_Declarator_tok;_tmp563.f1=({
struct Cyc_Parse_Declarator*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp564->tms=({
struct Cyc_List_List*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp566=_cycalloc(sizeof(*_tmp566));
_tmp566[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp567;_tmp567.tag=4;_tmp567.f1=
_tmp561;_tmp567.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp567.f3=0;
_tmp567;});_tmp566;}));_tmp565->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp565;});_tmp564;});
_tmp563;});_tmp562;});break;}case 140: _LL266: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568[0]=({struct Cyc_Declarator_tok_struct
_tmp569;_tmp569.tag=Cyc_Declarator_tok;_tmp569.f1=({struct Cyc_Parse_Declarator*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp56A->tms=({
struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp56D;_tmp56D.tag=5;_tmp56D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp56D.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56D;});_tmp56C;}));
_tmp56B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp56B;});_tmp56A;});_tmp569;});_tmp568;});break;case
141: _LL267: Cyc_Parse_err(({const char*_tmp56E="identifier has not been declared as a typedef";
_tag_arr(_tmp56E,sizeof(char),_get_zero_arr_size(_tmp56E,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({
struct Cyc_Declarator_tok_struct _tmp570;_tmp570.tag=Cyc_Declarator_tok;_tmp570.f1=({
struct Cyc_Parse_Declarator*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp571->tms=0;_tmp571;});
_tmp570;});_tmp56F;});break;case 142: _LL268: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 143: _LL269: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp572=_cycalloc(sizeof(*_tmp572));
_tmp572[0]=({struct Cyc_TypeModifierList_tok_struct _tmp573;_tmp573.tag=Cyc_TypeModifierList_tok;
_tmp573.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp573;});_tmp572;});
break;case 144: _LL26A: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp576;_tmp576.tag=5;_tmp576.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp576.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp576;});_tmp575;}));_tmp574->tl=ans;_tmp574;});ans=({
struct Cyc_List_List*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->hd=(void*)((
void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp578=_cycalloc(sizeof(*_tmp578));
_tmp578[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp579;_tmp579.tag=2;_tmp579.f1=({
struct Cyc_Absyn_PtrAtts _tmp57A;_tmp57A.rgn=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp57A.nullable=(*Cyc_yyget_YY1(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp57A.bounds=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f2;
_tmp57A.zero_term=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp57A;});_tmp579.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp579;});_tmp578;}));
_tmp577->tl=ans;_tmp577;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp57C;_tmp57C.tag=Cyc_TypeModifierList_tok;_tmp57C.f1=ans;_tmp57C;});_tmp57B;});
break;}case 145: _LL26B: yyval=(void*)({struct Cyc_YY1_struct*_tmp57D=_cycalloc(
sizeof(*_tmp57D));_tmp57D[0]=({struct Cyc_YY1_struct _tmp57E;_tmp57E.tag=Cyc_YY1;
_tmp57E.f1=({struct _tuple16*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->f1=Cyc_Absyn_true_conref;
_tmp57F->f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp57F;});_tmp57E;});_tmp57D;});break;case 146: _LL26C: yyval=(void*)({struct Cyc_YY1_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_YY1_struct _tmp581;
_tmp581.tag=Cyc_YY1;_tmp581.f1=({struct _tuple16*_tmp582=_cycalloc(sizeof(*
_tmp582));_tmp582->f1=Cyc_Absyn_false_conref;_tmp582->f2=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp582;});_tmp581;});
_tmp580;});break;case 147: _LL26D: yyval=(void*)({struct Cyc_YY1_struct*_tmp583=
_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_YY1_struct _tmp584;_tmp584.tag=
Cyc_YY1;_tmp584.f1=({struct _tuple16*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->f1=
Cyc_Absyn_true_conref;_tmp585->f2=Cyc_Absyn_bounds_unknown_conref;_tmp585;});
_tmp584;});_tmp583;});break;case 148: _LL26E: yyval=(void*)({struct Cyc_YY2_struct*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_YY2_struct _tmp587;
_tmp587.tag=Cyc_YY2;_tmp587.f1=Cyc_Absyn_bounds_one_conref;_tmp587;});_tmp586;});
break;case 149: _LL26F: yyval=(void*)({struct Cyc_YY2_struct*_tmp588=_cycalloc(
sizeof(*_tmp588));_tmp588[0]=({struct Cyc_YY2_struct _tmp589;_tmp589.tag=Cyc_YY2;
_tmp589.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp58A=
_cycalloc(sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_Absyn_Upper_b_struct _tmp58B;
_tmp58B.tag=0;_tmp58B.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp58B;});_tmp58A;}));_tmp589;});_tmp588;});break;case
150: _LL270: yyval=(void*)({struct Cyc_YY2_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));
_tmp58C[0]=({struct Cyc_YY2_struct _tmp58D;_tmp58D.tag=Cyc_YY2;_tmp58D.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));
_tmp58E[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp58F;_tmp58F.tag=1;_tmp58F.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp58F;});_tmp58E;}));_tmp58D;});_tmp58C;});break;case 151: _LL271: yyval=(
void*)({struct Cyc_YY4_struct*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({
struct Cyc_YY4_struct _tmp591;_tmp591.tag=Cyc_YY4;_tmp591.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp591;});_tmp590;});break;case 152: _LL272: yyval=(void*)({
struct Cyc_YY4_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_YY4_struct
_tmp593;_tmp593.tag=Cyc_YY4;_tmp593.f1=Cyc_Absyn_true_conref;_tmp593;});_tmp592;});
break;case 153: _LL273: yyval=(void*)({struct Cyc_YY4_struct*_tmp594=_cycalloc(
sizeof(*_tmp594));_tmp594[0]=({struct Cyc_YY4_struct _tmp595;_tmp595.tag=Cyc_YY4;
_tmp595.f1=Cyc_Absyn_false_conref;_tmp595;});_tmp594;});break;case 154: _LL274:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596[0]=({struct Cyc_Type_tok_struct _tmp597;_tmp597.tag=Cyc_Type_tok;_tmp597.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp598=_cycalloc(sizeof(*_tmp598));
_tmp598->v=(void*)((void*)3);_tmp598;}),0);_tmp597;});_tmp596;});break;case 155:
_LL275: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL276: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({
struct Cyc_Type_tok_struct _tmp59A;_tmp59A.tag=Cyc_Type_tok;_tmp59A.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->v=(void*)((void*)
3);_tmp59B;}),0);_tmp59A;});_tmp599;});break;case 157: _LL277: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp59C=_cycalloc_atomic(sizeof(*_tmp59C));_tmp59C[
0]=({struct Cyc_TypeQual_tok_struct _tmp59D;_tmp59D.tag=Cyc_TypeQual_tok;_tmp59D.f1=
Cyc_Absyn_empty_tqual();_tmp59D;});_tmp59C;});break;case 158: _LL278: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp59E=_cycalloc_atomic(sizeof(*_tmp59E));_tmp59E[
0]=({struct Cyc_TypeQual_tok_struct _tmp59F;_tmp59F.tag=Cyc_TypeQual_tok;_tmp59F.f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp59F;});_tmp59E;});
break;case 159: _LL279: yyval=(void*)({struct Cyc_YY3_struct*_tmp5A0=_cycalloc(
sizeof(*_tmp5A0));_tmp5A0[0]=({struct Cyc_YY3_struct _tmp5A1;_tmp5A1.tag=Cyc_YY3;
_tmp5A1.f1=({struct _tuple14*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5A2->f2=0;
_tmp5A2->f3=0;_tmp5A2->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A2->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A2;});_tmp5A1;});
_tmp5A0;});break;case 160: _LL27A: yyval=(void*)({struct Cyc_YY3_struct*_tmp5A3=
_cycalloc(sizeof(*_tmp5A3));_tmp5A3[0]=({struct Cyc_YY3_struct _tmp5A4;_tmp5A4.tag=
Cyc_YY3;_tmp5A4.f1=({struct _tuple14*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp5A5->f2=1;
_tmp5A5->f3=0;_tmp5A5->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A5->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A5;});_tmp5A4;});
_tmp5A3;});break;case 161: _LL27B: {struct _tuple2 _tmp5A7;struct Cyc_Core_Opt*
_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;struct _tuple2*_tmp5A6=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A7=*_tmp5A6;
_tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;_tmp5AA=_tmp5A7.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5AB=({struct Cyc_Absyn_VarargInfo*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->name=
_tmp5A8;_tmp5AF->tq=_tmp5A9;_tmp5AF->type=(void*)_tmp5AA;_tmp5AF->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5AF;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({
struct Cyc_YY3_struct _tmp5AD;_tmp5AD.tag=Cyc_YY3;_tmp5AD.f1=({struct _tuple14*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->f1=0;_tmp5AE->f2=0;_tmp5AE->f3=
_tmp5AB;_tmp5AE->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5AE->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AE;});_tmp5AD;});
_tmp5AC;});break;}}case 162: _LL27C: {struct _tuple2 _tmp5B1;struct Cyc_Core_Opt*
_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B3;void*_tmp5B4;struct _tuple2*_tmp5B0=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5B1=*_tmp5B0;
_tmp5B2=_tmp5B1.f1;_tmp5B3=_tmp5B1.f2;_tmp5B4=_tmp5B1.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5B5=({struct Cyc_Absyn_VarargInfo*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->name=
_tmp5B2;_tmp5B9->tq=_tmp5B3;_tmp5B9->type=(void*)_tmp5B4;_tmp5B9->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5B9;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=({
struct Cyc_YY3_struct _tmp5B7;_tmp5B7.tag=Cyc_YY3;_tmp5B7.f1=({struct _tuple14*
_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5B8->f2=0;_tmp5B8->f3=
_tmp5B5;_tmp5B8->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5B8->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B8;});_tmp5B7;});
_tmp5B6;});break;}}case 163: _LL27D: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({struct Cyc_Type_tok_struct
_tmp5BB;_tmp5BB.tag=Cyc_Type_tok;_tmp5BB.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5BD;_tmp5BD.tag=1;_tmp5BD.f1=0;_tmp5BD;});_tmp5BC;}));_tmp5BB;});_tmp5BA;});
break;case 164: _LL27E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5BE=_cycalloc(
sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Type_tok_struct _tmp5BF;_tmp5BF.tag=Cyc_Type_tok;
_tmp5BF.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5C1;_tmp5C1.tag=0;_tmp5C1.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C1;});_tmp5C0;}));
_tmp5BF;});_tmp5BE;});break;case 165: _LL27F: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5C3;_tmp5C3.tag=Cyc_TypeOpt_tok;_tmp5C3.f1=0;_tmp5C3;});_tmp5C2;});break;
case 166: _LL280: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5C4=_cycalloc(
sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_TypeOpt_tok_struct _tmp5C5;_tmp5C5.tag=
Cyc_TypeOpt_tok;_tmp5C5.f1=({struct Cyc_Core_Opt*_tmp5C6=_cycalloc(sizeof(*
_tmp5C6));_tmp5C6->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5C7=
_cycalloc(sizeof(*_tmp5C7));_tmp5C7[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5C8;
_tmp5C8.tag=20;_tmp5C8.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C8;});_tmp5C7;}));
_tmp5C6;});_tmp5C5;});_tmp5C4;});break;case 167: _LL281: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5CA;_tmp5CA.tag=Cyc_Rgnorder_tok;_tmp5CA.f1=0;_tmp5CA;});_tmp5C9;});break;
case 168: _LL282: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 169: _LL283: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5CB=
_cycalloc(sizeof(*_tmp5CB));_tmp5CB[0]=({struct Cyc_Rgnorder_tok_struct _tmp5CC;
_tmp5CC.tag=Cyc_Rgnorder_tok;_tmp5CC.f1=({struct Cyc_List_List*_tmp5CD=_cycalloc(
sizeof(*_tmp5CD));_tmp5CD->hd=({struct _tuple4*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));
_tmp5CE->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5D1=_cycalloc(sizeof(*
_tmp5D1));_tmp5D1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5D2;_tmp5D2.tag=20;
_tmp5D2.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5D2;});_tmp5D1;});_tmp5CE->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5D0;_tmp5D0.tag=0;_tmp5D0.f1=(void*)((void*)3);_tmp5D0;});_tmp5CF;}));
_tmp5CE;});_tmp5CD->tl=0;_tmp5CD;});_tmp5CC;});_tmp5CB;});break;case 170: _LL284:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));
_tmp5D3[0]=({struct Cyc_Rgnorder_tok_struct _tmp5D4;_tmp5D4.tag=Cyc_Rgnorder_tok;
_tmp5D4.f1=({struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=({
struct _tuple4*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5DA;_tmp5DA.tag=20;_tmp5DA.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5DA;});_tmp5D9;});
_tmp5D6->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5D8;_tmp5D8.tag=0;_tmp5D8.f1=(void*)((void*)3);_tmp5D8;});_tmp5D7;}));
_tmp5D6;});_tmp5D5->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D5;});_tmp5D4;});
_tmp5D3;});break;case 171: _LL285: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp5DB=
_cycalloc_atomic(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Bool_tok_struct _tmp5DC;
_tmp5DC.tag=Cyc_Bool_tok;_tmp5DC.f1=0;_tmp5DC;});_tmp5DB;});break;case 172: _LL286:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5DD="inject";
_tag_arr(_tmp5DD,sizeof(char),_get_zero_arr_size(_tmp5DD,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5DE="missing type in function declaration";_tag_arr(_tmp5DE,
sizeof(char),_get_zero_arr_size(_tmp5DE,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp5DF=_cycalloc_atomic(sizeof(*_tmp5DF));_tmp5DF[0]=({
struct Cyc_Bool_tok_struct _tmp5E0;_tmp5E0.tag=Cyc_Bool_tok;_tmp5E0.f1=1;_tmp5E0;});
_tmp5DF;});break;case 173: _LL287: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 174: _LL288: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_TypeList_tok_struct
_tmp5E2;_tmp5E2.tag=Cyc_TypeList_tok;_tmp5E2.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5E2;});_tmp5E1;});
break;case 175: _LL289: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5E3=
_cycalloc(sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_TypeList_tok_struct _tmp5E4;
_tmp5E4.tag=Cyc_TypeList_tok;_tmp5E4.f1=0;_tmp5E4;});_tmp5E3;});break;case 176:
_LL28A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 177: _LL28B: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5E5=_cycalloc(
sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_TypeList_tok_struct _tmp5E6;_tmp5E6.tag=
Cyc_TypeList_tok;_tmp5E6.f1=({struct Cyc_List_List*_tmp5E7=_cycalloc(sizeof(*
_tmp5E7));_tmp5E7->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5E8=
_cycalloc(sizeof(*_tmp5E8));_tmp5E8[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5E9;
_tmp5E9.tag=21;_tmp5E9.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E9;});_tmp5E8;}));
_tmp5E7->tl=0;_tmp5E7;});_tmp5E6;});_tmp5E5;});break;case 178: _LL28C: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5EA=_cycalloc(sizeof(*
_tmp5EA));_tmp5EA[0]=({struct Cyc_TypeList_tok_struct _tmp5EB;_tmp5EB.tag=Cyc_TypeList_tok;
_tmp5EB.f1=({struct Cyc_List_List*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5EC->tl=0;_tmp5EC;});_tmp5EB;});_tmp5EA;});break;case 179: _LL28D: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5ED=_cycalloc(sizeof(*
_tmp5ED));_tmp5ED[0]=({struct Cyc_TypeList_tok_struct _tmp5EE;_tmp5EE.tag=Cyc_TypeList_tok;
_tmp5EE.f1=({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5F0=_cycalloc(sizeof(*
_tmp5F0));_tmp5F0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5F1;_tmp5F1.tag=19;
_tmp5F1.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5F1;});_tmp5F0;}));_tmp5EF->tl=0;_tmp5EF;});_tmp5EE;});
_tmp5ED;});break;case 180: _LL28E: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));
_tmp5F2[0]=({struct Cyc_TypeList_tok_struct _tmp5F3;_tmp5F3.tag=Cyc_TypeList_tok;
_tmp5F3.f1=({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5F5=_cycalloc(sizeof(*
_tmp5F5));_tmp5F5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5F6;_tmp5F6.tag=19;
_tmp5F6.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5F6;});_tmp5F5;}));_tmp5F4->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F4;});_tmp5F3;});
_tmp5F2;});break;case 181: _LL28F: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5F8;_tmp5F8.tag=Cyc_ParamDeclList_tok;_tmp5F8.f1=({struct Cyc_List_List*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F9->tl=0;_tmp5F9;});
_tmp5F8;});_tmp5F7;});break;case 182: _LL290: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5FB;_tmp5FB.tag=Cyc_ParamDeclList_tok;_tmp5FB.f1=({struct Cyc_List_List*
_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FC->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FC;});_tmp5FB;});
_tmp5FA;});break;case 183: _LL291: {struct _tuple15 _tmp5FE;struct Cyc_Absyn_Tqual
_tmp5FF;struct Cyc_List_List*_tmp600;struct Cyc_List_List*_tmp601;struct _tuple15*
_tmp5FD=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5FE=*_tmp5FD;_tmp5FF=_tmp5FE.f1;_tmp600=_tmp5FE.f2;
_tmp601=_tmp5FE.f3;{struct Cyc_Parse_Declarator _tmp603;struct _tuple1*_tmp604;
struct Cyc_List_List*_tmp605;struct Cyc_Parse_Declarator*_tmp602=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp603=*_tmp602;
_tmp604=_tmp603.id;_tmp605=_tmp603.tms;{void*_tmp606=Cyc_Parse_speclist2typ(
_tmp600,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp608;void*_tmp609;struct
Cyc_List_List*_tmp60A;struct Cyc_List_List*_tmp60B;struct _tuple6 _tmp607=Cyc_Parse_apply_tms(
_tmp5FF,_tmp606,_tmp601,_tmp605);_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;_tmp60A=
_tmp607.f3;_tmp60B=_tmp607.f4;if(_tmp60A != 0)Cyc_Parse_err(({const char*_tmp60C="parameter with bad type params";
_tag_arr(_tmp60C,sizeof(char),_get_zero_arr_size(_tmp60C,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp604))Cyc_Parse_err(({const char*_tmp60D="parameter cannot be qualified with a namespace";
_tag_arr(_tmp60D,sizeof(char),_get_zero_arr_size(_tmp60D,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp60E=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp614=_cycalloc(sizeof(*
_tmp614));_tmp614->v=(*_tmp604).f2;_tmp614;});if(_tmp60B != 0)({void*_tmp60F[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp610="extra attributes on parameter, ignoring";
_tag_arr(_tmp610,sizeof(char),_get_zero_arr_size(_tmp610,40));}),_tag_arr(
_tmp60F,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_ParamDecl_tok_struct
_tmp612;_tmp612.tag=Cyc_ParamDecl_tok;_tmp612.f1=({struct _tuple2*_tmp613=
_cycalloc(sizeof(*_tmp613));_tmp613->f1=_tmp60E;_tmp613->f2=_tmp608;_tmp613->f3=
_tmp609;_tmp613;});_tmp612;});_tmp611;});break;}}}}case 184: _LL292: {struct
_tuple15 _tmp616;struct Cyc_Absyn_Tqual _tmp617;struct Cyc_List_List*_tmp618;struct
Cyc_List_List*_tmp619;struct _tuple15*_tmp615=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp616=*_tmp615;_tmp617=
_tmp616.f1;_tmp618=_tmp616.f2;_tmp619=_tmp616.f3;{void*_tmp61A=Cyc_Parse_speclist2typ(
_tmp618,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp619 != 0)({void*_tmp61B[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp61C="bad attributes on parameter, ignoring";
_tag_arr(_tmp61C,sizeof(char),_get_zero_arr_size(_tmp61C,38));}),_tag_arr(
_tmp61B,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D[0]=({struct Cyc_ParamDecl_tok_struct
_tmp61E;_tmp61E.tag=Cyc_ParamDecl_tok;_tmp61E.f1=({struct _tuple2*_tmp61F=
_cycalloc(sizeof(*_tmp61F));_tmp61F->f1=0;_tmp61F->f2=_tmp617;_tmp61F->f3=
_tmp61A;_tmp61F;});_tmp61E;});_tmp61D;});break;}}case 185: _LL293: {struct _tuple15
_tmp621;struct Cyc_Absyn_Tqual _tmp622;struct Cyc_List_List*_tmp623;struct Cyc_List_List*
_tmp624;struct _tuple15*_tmp620=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp621=*_tmp620;_tmp622=
_tmp621.f1;_tmp623=_tmp621.f2;_tmp624=_tmp621.f3;{void*_tmp625=Cyc_Parse_speclist2typ(
_tmp623,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp626=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual
_tmp628;void*_tmp629;struct Cyc_List_List*_tmp62A;struct Cyc_List_List*_tmp62B;
struct _tuple6 _tmp627=Cyc_Parse_apply_tms(_tmp622,_tmp625,_tmp624,_tmp626);
_tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_tmp62A=_tmp627.f3;_tmp62B=_tmp627.f4;if(
_tmp62A != 0)({void*_tmp62C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62D="bad type parameters on formal argument, ignoring";_tag_arr(_tmp62D,
sizeof(char),_get_zero_arr_size(_tmp62D,49));}),_tag_arr(_tmp62C,sizeof(void*),0));});
if(_tmp62B != 0)({void*_tmp62E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62F="bad attributes on parameter, ignoring";_tag_arr(_tmp62F,sizeof(char),
_get_zero_arr_size(_tmp62F,38));}),_tag_arr(_tmp62E,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp630=_cycalloc(sizeof(*_tmp630));
_tmp630[0]=({struct Cyc_ParamDecl_tok_struct _tmp631;_tmp631.tag=Cyc_ParamDecl_tok;
_tmp631.f1=({struct _tuple2*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->f1=0;
_tmp632->f2=_tmp628;_tmp632->f3=_tmp629;_tmp632;});_tmp631;});_tmp630;});break;}}
case 186: _LL294: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp633=_cycalloc(
sizeof(*_tmp633));_tmp633[0]=({struct Cyc_IdList_tok_struct _tmp634;_tmp634.tag=
Cyc_IdList_tok;_tmp634.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp634;});_tmp633;});break;case 187: _LL295: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[0]=({struct Cyc_IdList_tok_struct
_tmp636;_tmp636.tag=Cyc_IdList_tok;_tmp636.f1=({struct Cyc_List_List*_tmp637=
_cycalloc(sizeof(*_tmp637));_tmp637->hd=({struct _tagged_arr*_tmp638=_cycalloc(
sizeof(*_tmp638));_tmp638[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp638;});_tmp637->tl=0;
_tmp637;});_tmp636;});_tmp635;});break;case 188: _LL296: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_IdList_tok_struct
_tmp63A;_tmp63A.tag=Cyc_IdList_tok;_tmp63A.f1=({struct Cyc_List_List*_tmp63B=
_cycalloc(sizeof(*_tmp63B));_tmp63B->hd=({struct _tagged_arr*_tmp63C=_cycalloc(
sizeof(*_tmp63C));_tmp63C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63C;});_tmp63B->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp63B;});_tmp63A;});
_tmp639;});break;case 189: _LL297: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 190: _LL298: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 191: _LL299: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Exp_tok_struct _tmp63E;
_tmp63E.tag=Cyc_Exp_tok;_tmp63E.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp640;_tmp640.tag=34;_tmp640.f1=0;_tmp640.f2=0;_tmp640;});_tmp63F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp63E;});
_tmp63D;});break;case 192: _LL29A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp641=
_cycalloc(sizeof(*_tmp641));_tmp641[0]=({struct Cyc_Exp_tok_struct _tmp642;_tmp642.tag=
Cyc_Exp_tok;_tmp642.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp644;_tmp644.tag=34;_tmp644.f1=0;_tmp644.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp644;});_tmp643;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp642;});_tmp641;});break;case 193: _LL29B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({
struct Cyc_Exp_tok_struct _tmp646;_tmp646.tag=Cyc_Exp_tok;_tmp646.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp647=_cycalloc(sizeof(*_tmp647));
_tmp647[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp648;_tmp648.tag=34;
_tmp648.f1=0;_tmp648.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp648;});_tmp647;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp646;});
_tmp645;});break;case 194: _LL29C: {struct Cyc_Absyn_Vardecl*_tmp649=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->f1=(void*)0;_tmp64F->f2=({
struct _tagged_arr*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp650;});_tmp64F;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp649->tq=({
struct Cyc_Absyn_Tqual _tmp64A;_tmp64A.q_const=1;_tmp64A.q_volatile=0;_tmp64A.q_restrict=
1;_tmp64A;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp64B=_cycalloc(sizeof(*
_tmp64B));_tmp64B[0]=({struct Cyc_Exp_tok_struct _tmp64C;_tmp64C.tag=Cyc_Exp_tok;
_tmp64C.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp64E;_tmp64E.tag=27;_tmp64E.f1=_tmp649;_tmp64E.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp64E.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64E.f4=0;
_tmp64E;});_tmp64D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp64C;});
_tmp64B;});break;}case 195: _LL29D: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_InitializerList_tok_struct
_tmp652;_tmp652.tag=Cyc_InitializerList_tok;_tmp652.f1=({struct Cyc_List_List*
_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->hd=({struct _tuple18*_tmp654=
_cycalloc(sizeof(*_tmp654));_tmp654->f1=0;_tmp654->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp654;});_tmp653->tl=0;
_tmp653;});_tmp652;});_tmp651;});break;case 196: _LL29E: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_InitializerList_tok_struct
_tmp656;_tmp656.tag=Cyc_InitializerList_tok;_tmp656.f1=({struct Cyc_List_List*
_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->hd=({struct _tuple18*_tmp658=
_cycalloc(sizeof(*_tmp658));_tmp658->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp658->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp658;});_tmp657->tl=
0;_tmp657;});_tmp656;});_tmp655;});break;case 197: _LL29F: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_InitializerList_tok_struct
_tmp65A;_tmp65A.tag=Cyc_InitializerList_tok;_tmp65A.f1=({struct Cyc_List_List*
_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=({struct _tuple18*_tmp65C=
_cycalloc(sizeof(*_tmp65C));_tmp65C->f1=0;_tmp65C->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65C;});_tmp65B->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65B;});_tmp65A;});
_tmp659;});break;case 198: _LL2A0: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_InitializerList_tok_struct
_tmp65E;_tmp65E.tag=Cyc_InitializerList_tok;_tmp65E.f1=({struct Cyc_List_List*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->hd=({struct _tuple18*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp660->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp660;});_tmp65F->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp65F;});_tmp65E;});_tmp65D;});break;case 199: _LL2A1: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp661=_cycalloc(sizeof(*_tmp661));
_tmp661[0]=({struct Cyc_DesignatorList_tok_struct _tmp662;_tmp662.tag=Cyc_DesignatorList_tok;
_tmp662.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp662;});_tmp661;});
break;case 200: _LL2A2: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp663=
_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_DesignatorList_tok_struct
_tmp664;_tmp664.tag=Cyc_DesignatorList_tok;_tmp664.f1=({struct Cyc_List_List*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp665->tl=0;_tmp665;});
_tmp664;});_tmp663;});break;case 201: _LL2A3: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666[0]=({struct Cyc_DesignatorList_tok_struct
_tmp667;_tmp667.tag=Cyc_DesignatorList_tok;_tmp667.f1=({struct Cyc_List_List*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp668->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp668;});_tmp667;});
_tmp666;});break;case 202: _LL2A4: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Designator_tok_struct
_tmp66A;_tmp66A.tag=Cyc_Designator_tok;_tmp66A.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp66C;_tmp66C.tag=0;_tmp66C.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66C;});_tmp66B;}));
_tmp66A;});_tmp669;});break;case 203: _LL2A5: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Designator_tok_struct
_tmp66E;_tmp66E.tag=Cyc_Designator_tok;_tmp66E.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_FieldName_struct
_tmp670;_tmp670.tag=1;_tmp670.f1=({struct _tagged_arr*_tmp671=_cycalloc(sizeof(*
_tmp671));_tmp671[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp671;});_tmp670;});_tmp66F;}));_tmp66E;});_tmp66D;});
break;case 204: _LL2A6: {struct _tuple15 _tmp673;struct Cyc_Absyn_Tqual _tmp674;struct
Cyc_List_List*_tmp675;struct Cyc_List_List*_tmp676;struct _tuple15*_tmp672=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp673=*_tmp672;
_tmp674=_tmp673.f1;_tmp675=_tmp673.f2;_tmp676=_tmp673.f3;{void*_tmp677=Cyc_Parse_speclist2typ(
_tmp675,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp676 != 0)({void*_tmp678[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp679="ignoring attributes in type";
_tag_arr(_tmp679,sizeof(char),_get_zero_arr_size(_tmp679,28));}),_tag_arr(
_tmp678,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_ParamDecl_tok_struct
_tmp67B;_tmp67B.tag=Cyc_ParamDecl_tok;_tmp67B.f1=({struct _tuple2*_tmp67C=
_cycalloc(sizeof(*_tmp67C));_tmp67C->f1=0;_tmp67C->f2=_tmp674;_tmp67C->f3=
_tmp677;_tmp67C;});_tmp67B;});_tmp67A;});break;}}case 205: _LL2A7: {struct _tuple15
_tmp67E;struct Cyc_Absyn_Tqual _tmp67F;struct Cyc_List_List*_tmp680;struct Cyc_List_List*
_tmp681;struct _tuple15*_tmp67D=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67E=*_tmp67D;_tmp67F=
_tmp67E.f1;_tmp680=_tmp67E.f2;_tmp681=_tmp67E.f3;{void*_tmp682=Cyc_Parse_speclist2typ(
_tmp680,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp683=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp684=Cyc_Parse_apply_tms(_tmp67F,_tmp682,_tmp681,_tmp683);if(_tmp684.f3 != 0)({
void*_tmp685[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp686="bad type params, ignoring";_tag_arr(_tmp686,sizeof(char),
_get_zero_arr_size(_tmp686,26));}),_tag_arr(_tmp685,sizeof(void*),0));});if(
_tmp684.f4 != 0)({void*_tmp687[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp688="bad specifiers, ignoring";_tag_arr(_tmp688,sizeof(char),
_get_zero_arr_size(_tmp688,25));}),_tag_arr(_tmp687,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp689=_cycalloc(sizeof(*_tmp689));
_tmp689[0]=({struct Cyc_ParamDecl_tok_struct _tmp68A;_tmp68A.tag=Cyc_ParamDecl_tok;
_tmp68A.f1=({struct _tuple2*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->f1=0;
_tmp68B->f2=_tmp684.f1;_tmp68B->f3=_tmp684.f2;_tmp68B;});_tmp68A;});_tmp689;});
break;}}case 206: _LL2A8: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp68C=
_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_Type_tok_struct _tmp68D;
_tmp68D.tag=Cyc_Type_tok;_tmp68D.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp68D;});_tmp68C;});
break;case 207: _LL2A9: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp68E=_cycalloc(
sizeof(*_tmp68E));_tmp68E[0]=({struct Cyc_Type_tok_struct _tmp68F;_tmp68F.tag=Cyc_Type_tok;
_tmp68F.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp690=_cycalloc(
sizeof(*_tmp690));_tmp690[0]=({struct Cyc_Absyn_JoinEff_struct _tmp691;_tmp691.tag=
20;_tmp691.f1=0;_tmp691;});_tmp690;}));_tmp68F;});_tmp68E;});break;case 208:
_LL2AA: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp692=_cycalloc(sizeof(*
_tmp692));_tmp692[0]=({struct Cyc_Type_tok_struct _tmp693;_tmp693.tag=Cyc_Type_tok;
_tmp693.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp694=_cycalloc(
sizeof(*_tmp694));_tmp694[0]=({struct Cyc_Absyn_JoinEff_struct _tmp695;_tmp695.tag=
20;_tmp695.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp695;});_tmp694;}));_tmp693;});_tmp692;});break;case 209:
_LL2AB: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp696=_cycalloc(sizeof(*
_tmp696));_tmp696[0]=({struct Cyc_Type_tok_struct _tmp697;_tmp697.tag=Cyc_Type_tok;
_tmp697.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp698=_cycalloc(
sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp699;_tmp699.tag=
21;_tmp699.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp699;});_tmp698;}));_tmp697;});_tmp696;});break;case
210: _LL2AC: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp69A=_cycalloc(sizeof(*
_tmp69A));_tmp69A[0]=({struct Cyc_Type_tok_struct _tmp69B;_tmp69B.tag=Cyc_Type_tok;
_tmp69B.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp69C=_cycalloc(
sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp69D;_tmp69D.tag=
20;_tmp69D.f1=({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp69E->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp69E;});_tmp69D;});_tmp69C;}));_tmp69B;});_tmp69A;});
break;case 211: _LL2AD: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69F=
_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_TypeList_tok_struct _tmp6A0;
_tmp6A0.tag=Cyc_TypeList_tok;_tmp6A0.f1=({struct Cyc_List_List*_tmp6A1=_cycalloc(
sizeof(*_tmp6A1));_tmp6A1->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A1->tl=0;_tmp6A1;});
_tmp6A0;});_tmp69F;});break;case 212: _LL2AE: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2[0]=({struct Cyc_TypeList_tok_struct
_tmp6A3;_tmp6A3.tag=Cyc_TypeList_tok;_tmp6A3.f1=({struct Cyc_List_List*_tmp6A4=
_cycalloc(sizeof(*_tmp6A4));_tmp6A4->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A4->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A4;});_tmp6A3;});
_tmp6A2;});break;case 213: _LL2AF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6A6;_tmp6A6.tag=Cyc_AbstractDeclarator_tok;_tmp6A6.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A7;});_tmp6A6;});
_tmp6A5;});break;case 214: _LL2B0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 215: _LL2B1: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6A9;_tmp6A9.tag=Cyc_AbstractDeclarator_tok;_tmp6A9.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6AA;});
_tmp6A9;});_tmp6A8;});break;case 216: _LL2B2: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 217: _LL2B3:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6AB=_cycalloc(sizeof(*
_tmp6AB));_tmp6AB[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6AC;_tmp6AC.tag=
Cyc_AbstractDeclarator_tok;_tmp6AC.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tms=({struct Cyc_List_List*_tmp6AE=
_cycalloc(sizeof(*_tmp6AE));_tmp6AE->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6B0;_tmp6B0.tag=0;_tmp6B0.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B0;});_tmp6AF;}));
_tmp6AE->tl=0;_tmp6AE;});_tmp6AD;});_tmp6AC;});_tmp6AB;});break;case 218: _LL2B4:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6B1=_cycalloc(sizeof(*
_tmp6B1));_tmp6B1[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6B2;_tmp6B2.tag=
Cyc_AbstractDeclarator_tok;_tmp6B2.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->tms=({struct Cyc_List_List*_tmp6B4=
_cycalloc(sizeof(*_tmp6B4));_tmp6B4->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6B6;_tmp6B6.tag=0;_tmp6B6.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B6;});_tmp6B5;}));
_tmp6B4->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6B4;});_tmp6B3;});
_tmp6B2;});_tmp6B1;});break;case 219: _LL2B5: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6B8;_tmp6B8.tag=Cyc_AbstractDeclarator_tok;_tmp6B8.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tms=({struct Cyc_List_List*_tmp6BA=
_cycalloc(sizeof(*_tmp6BA));_tmp6BA->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6BC;_tmp6BC.tag=1;_tmp6BC.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BC.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BC;});_tmp6BB;}));
_tmp6BA->tl=0;_tmp6BA;});_tmp6B9;});_tmp6B8;});_tmp6B7;});break;case 220: _LL2B6:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6BD=_cycalloc(sizeof(*
_tmp6BD));_tmp6BD[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6BE;_tmp6BE.tag=
Cyc_AbstractDeclarator_tok;_tmp6BE.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->tms=({struct Cyc_List_List*_tmp6C0=
_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6C2;_tmp6C2.tag=1;_tmp6C2.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C2.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C2;});_tmp6C1;}));
_tmp6C0->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6C0;});_tmp6BF;});
_tmp6BE;});_tmp6BD;});break;case 221: _LL2B7: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6C4;_tmp6C4.tag=Cyc_AbstractDeclarator_tok;_tmp6C4.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->tms=({struct Cyc_List_List*_tmp6C6=
_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6C8;_tmp6C8.tag=3;_tmp6C8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6CA;_tmp6CA.tag=1;_tmp6CA.f1=0;_tmp6CA.f2=0;_tmp6CA.f3=0;_tmp6CA.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CA.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CA;});_tmp6C9;}));
_tmp6C8;});_tmp6C7;}));_tmp6C6->tl=0;_tmp6C6;});_tmp6C5;});_tmp6C4;});_tmp6C3;});
break;case 222: _LL2B8: {struct _tuple14 _tmp6CC;struct Cyc_List_List*_tmp6CD;int
_tmp6CE;struct Cyc_Absyn_VarargInfo*_tmp6CF;struct Cyc_Core_Opt*_tmp6D0;struct Cyc_List_List*
_tmp6D1;struct _tuple14*_tmp6CB=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6CC=*_tmp6CB;_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;
_tmp6CF=_tmp6CC.f3;_tmp6D0=_tmp6CC.f4;_tmp6D1=_tmp6CC.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6D3;_tmp6D3.tag=Cyc_AbstractDeclarator_tok;
_tmp6D3.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6D4=_cycalloc(sizeof(*
_tmp6D4));_tmp6D4->tms=({struct Cyc_List_List*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));
_tmp6D5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6D6=
_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D7;_tmp6D7.tag=3;_tmp6D7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D9;_tmp6D9.tag=1;_tmp6D9.f1=_tmp6CD;_tmp6D9.f2=_tmp6CE;_tmp6D9.f3=_tmp6CF;
_tmp6D9.f4=_tmp6D0;_tmp6D9.f5=_tmp6D1;_tmp6D9;});_tmp6D8;}));_tmp6D7;});_tmp6D6;}));
_tmp6D5->tl=0;_tmp6D5;});_tmp6D4;});_tmp6D3;});_tmp6D2;});break;}case 223: _LL2B9:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6DA=_cycalloc(sizeof(*
_tmp6DA));_tmp6DA[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6DB;_tmp6DB.tag=
Cyc_AbstractDeclarator_tok;_tmp6DB.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->tms=({struct Cyc_List_List*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6DF;_tmp6DF.tag=3;_tmp6DF.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E1;_tmp6E1.tag=1;_tmp6E1.f1=0;_tmp6E1.f2=0;_tmp6E1.f3=0;_tmp6E1.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E1.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E1;});_tmp6E0;}));
_tmp6DF;});_tmp6DE;}));_tmp6DD->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6DD;});_tmp6DC;});
_tmp6DB;});_tmp6DA;});break;case 224: _LL2BA: {struct _tuple14 _tmp6E3;struct Cyc_List_List*
_tmp6E4;int _tmp6E5;struct Cyc_Absyn_VarargInfo*_tmp6E6;struct Cyc_Core_Opt*_tmp6E7;
struct Cyc_List_List*_tmp6E8;struct _tuple14*_tmp6E2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E3=*_tmp6E2;_tmp6E4=
_tmp6E3.f1;_tmp6E5=_tmp6E3.f2;_tmp6E6=_tmp6E3.f3;_tmp6E7=_tmp6E3.f4;_tmp6E8=
_tmp6E3.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6E9=
_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6EA;_tmp6EA.tag=Cyc_AbstractDeclarator_tok;_tmp6EA.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->tms=({struct Cyc_List_List*_tmp6EC=
_cycalloc(sizeof(*_tmp6EC));_tmp6EC->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6EE;_tmp6EE.tag=3;_tmp6EE.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F0;_tmp6F0.tag=1;_tmp6F0.f1=_tmp6E4;_tmp6F0.f2=_tmp6E5;_tmp6F0.f3=_tmp6E6;
_tmp6F0.f4=_tmp6E7;_tmp6F0.f5=_tmp6E8;_tmp6F0;});_tmp6EF;}));_tmp6EE;});_tmp6ED;}));
_tmp6EC->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6EC;});_tmp6EB;});
_tmp6EA;});_tmp6E9;});break;}case 225: _LL2BB: {struct Cyc_List_List*_tmp6F1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6F3;_tmp6F3.tag=Cyc_AbstractDeclarator_tok;
_tmp6F3.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F4=_cycalloc(sizeof(*
_tmp6F4));_tmp6F4->tms=({struct Cyc_List_List*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));
_tmp6F5->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp6F6=
_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6F7;_tmp6F7.tag=4;_tmp6F7.f1=_tmp6F1;_tmp6F7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F7.f3=0;
_tmp6F7;});_tmp6F6;}));_tmp6F5->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6F5;});_tmp6F4;});
_tmp6F3;});_tmp6F2;});break;}case 226: _LL2BC: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6F9;_tmp6F9.tag=Cyc_AbstractDeclarator_tok;_tmp6F9.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->tms=({struct Cyc_List_List*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6FD;_tmp6FD.tag=5;_tmp6FD.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FD.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FD;});_tmp6FC;}));
_tmp6FB->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp6FB;});_tmp6FA;});
_tmp6F9;});_tmp6F8;});break;case 227: _LL2BD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 228: _LL2BE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 229: _LL2BF:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230:
_LL2C0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
231: _LL2C1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 232: _LL2C2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 233: _LL2C3: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp6FE="`H";
_tag_arr(_tmp6FE,sizeof(char),_get_zero_arr_size(_tmp6FE,3));}))== 0)Cyc_Parse_err(({
const char*_tmp6FF="bad occurrence of heap region `H";_tag_arr(_tmp6FF,sizeof(
char),_get_zero_arr_size(_tmp6FF,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->name=({
struct _tagged_arr*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp70D;});_tmp70A->identity=
0;_tmp70A->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp70B=_cycalloc(
sizeof(*_tmp70B));_tmp70B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp70C;_tmp70C.tag=0;
_tmp70C.f1=(void*)((void*)3);_tmp70C;});_tmp70B;}));_tmp70A;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({
struct Cyc_Absyn_VarType_struct _tmp709;_tmp709.tag=1;_tmp709.f1=tv;_tmp709;});
_tmp708;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp700=_cycalloc(sizeof(*
_tmp700));_tmp700[0]=({struct Cyc_Stmt_tok_struct _tmp701;_tmp701.tag=Cyc_Stmt_tok;
_tmp701.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp702=
_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_Region_s_struct _tmp703;
_tmp703.tag=18;_tmp703.f1=tv;_tmp703.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704->f1=(void*)0;_tmp704->f2=({struct
_tagged_arr*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp705;});_tmp704;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp706=_cycalloc(sizeof(*_tmp706));
_tmp706[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp707;_tmp707.tag=15;_tmp707.f1=(
void*)t;_tmp707;});_tmp706;}),0);_tmp703.f3=0;_tmp703.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp703;});_tmp702;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp701;});
_tmp700;});break;}case 234: _LL2C4: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp70E="H";
_tag_arr(_tmp70E,sizeof(char),_get_zero_arr_size(_tmp70E,2));}))== 0)Cyc_Parse_err(({
const char*_tmp70F="bad occurrence of heap region `H";_tag_arr(_tmp70F,sizeof(
char),_get_zero_arr_size(_tmp70F,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->name=({
struct _tagged_arr*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp720;_tmp720.tag=0;_tmp720.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp71E[1]={&
_tmp720};Cyc_aprintf(({const char*_tmp71F="`%s";_tag_arr(_tmp71F,sizeof(char),
_get_zero_arr_size(_tmp71F,4));}),_tag_arr(_tmp71E,sizeof(void*),1));}});_tmp71D;});
_tmp71A->identity=0;_tmp71A->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp71C;_tmp71C.tag=0;_tmp71C.f1=(void*)((void*)3);_tmp71C;});_tmp71B;}));
_tmp71A;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp718=_cycalloc(
sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Absyn_VarType_struct _tmp719;_tmp719.tag=
1;_tmp719.f1=tv;_tmp719;});_tmp718;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_Stmt_tok_struct
_tmp711;_tmp711.tag=Cyc_Stmt_tok;_tmp711.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({
struct Cyc_Absyn_Region_s_struct _tmp713;_tmp713.tag=18;_tmp713.f1=tv;_tmp713.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp714=_cycalloc(sizeof(*_tmp714));
_tmp714->f1=(void*)0;_tmp714->f2=({struct _tagged_arr*_tmp715=_cycalloc(sizeof(*
_tmp715));_tmp715[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp715;});_tmp714;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp717;_tmp717.tag=15;_tmp717.f1=(void*)t;_tmp717;});_tmp716;}),0);_tmp713.f3=0;
_tmp713.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp713;});_tmp712;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp711;});
_tmp710;});break;}case 235: _LL2C5: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp721="resetable";
_tag_arr(_tmp721,sizeof(char),_get_zero_arr_size(_tmp721,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp722="expecting [resetable]";_tag_arr(_tmp722,sizeof(char),
_get_zero_arr_size(_tmp722,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp723="`H";_tag_arr(_tmp723,sizeof(char),
_get_zero_arr_size(_tmp723,3));}))== 0)Cyc_Parse_err(({const char*_tmp724="bad occurrence of heap region `H";
_tag_arr(_tmp724,sizeof(char),_get_zero_arr_size(_tmp724,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F->name=({
struct _tagged_arr*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp732;});_tmp72F->identity=
0;_tmp72F->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp730=_cycalloc(
sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp731;_tmp731.tag=0;
_tmp731.f1=(void*)((void*)3);_tmp731;});_tmp730;}));_tmp72F;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({
struct Cyc_Absyn_VarType_struct _tmp72E;_tmp72E.tag=1;_tmp72E.f1=tv;_tmp72E;});
_tmp72D;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp725=_cycalloc(sizeof(*
_tmp725));_tmp725[0]=({struct Cyc_Stmt_tok_struct _tmp726;_tmp726.tag=Cyc_Stmt_tok;
_tmp726.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp727=
_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_Absyn_Region_s_struct _tmp728;
_tmp728.tag=18;_tmp728.f1=tv;_tmp728.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729->f1=(void*)0;_tmp729->f2=({struct
_tagged_arr*_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72A;});_tmp729;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp72B=_cycalloc(sizeof(*_tmp72B));
_tmp72B[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp72C;_tmp72C.tag=15;_tmp72C.f1=(
void*)t;_tmp72C;});_tmp72B;}),0);_tmp728.f3=1;_tmp728.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp728;});_tmp727;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp726;});
_tmp725;});break;}case 236: _LL2C6: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp733="resetable";
_tag_arr(_tmp733,sizeof(char),_get_zero_arr_size(_tmp733,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp734="expecting `resetable'";_tag_arr(_tmp734,sizeof(char),
_get_zero_arr_size(_tmp734,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp735="H";_tag_arr(_tmp735,sizeof(char),
_get_zero_arr_size(_tmp735,2));}))== 0)Cyc_Parse_err(({const char*_tmp736="bad occurrence of heap region `H";
_tag_arr(_tmp736,sizeof(char),_get_zero_arr_size(_tmp736,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->name=({
struct _tagged_arr*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp747;_tmp747.tag=0;_tmp747.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp745[1]={&
_tmp747};Cyc_aprintf(({const char*_tmp746="`%s";_tag_arr(_tmp746,sizeof(char),
_get_zero_arr_size(_tmp746,4));}),_tag_arr(_tmp745,sizeof(void*),1));}});_tmp744;});
_tmp741->identity=0;_tmp741->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp743;_tmp743.tag=0;_tmp743.f1=(void*)((void*)3);_tmp743;});_tmp742;}));
_tmp741;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp73F=_cycalloc(
sizeof(*_tmp73F));_tmp73F[0]=({struct Cyc_Absyn_VarType_struct _tmp740;_tmp740.tag=
1;_tmp740.f1=tv;_tmp740;});_tmp73F;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Stmt_tok_struct
_tmp738;_tmp738.tag=Cyc_Stmt_tok;_tmp738.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({
struct Cyc_Absyn_Region_s_struct _tmp73A;_tmp73A.tag=18;_tmp73A.f1=tv;_tmp73A.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp73B=_cycalloc(sizeof(*_tmp73B));
_tmp73B->f1=(void*)0;_tmp73B->f2=({struct _tagged_arr*_tmp73C=_cycalloc(sizeof(*
_tmp73C));_tmp73C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp73C;});_tmp73B;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp73E;_tmp73E.tag=15;_tmp73E.f1=(void*)t;_tmp73E;});_tmp73D;}),0);_tmp73A.f3=1;
_tmp73A.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp73A;});_tmp739;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp738;});
_tmp737;});break;}case 237: _LL2C7: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Stmt_tok_struct
_tmp749;_tmp749.tag=Cyc_Stmt_tok;_tmp749.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp74B;_tmp74B.tag=20;_tmp74B.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp74B;});_tmp74A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp749;});_tmp748;});break;case 238: _LL2C8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({
struct Cyc_Stmt_tok_struct _tmp74D;_tmp74D.tag=Cyc_Stmt_tok;_tmp74D.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp74F;_tmp74F.tag=13;_tmp74F.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74F;});_tmp74E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp74D;});_tmp74C;});break;case 239: _LL2C9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=({
struct Cyc_Stmt_tok_struct _tmp751;_tmp751.tag=Cyc_Stmt_tok;_tmp751.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp752=_cycalloc(sizeof(*_tmp752));
_tmp752[0]=({struct Cyc_Absyn_Splice_s_struct _tmp753;_tmp753.tag=14;_tmp753.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp753;});_tmp752;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp751;});
_tmp750;});break;case 240: _LL2CA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp754=
_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Stmt_tok_struct _tmp755;
_tmp755.tag=Cyc_Stmt_tok;_tmp755.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Absyn_Label_s_struct
_tmp757;_tmp757.tag=15;_tmp757.f1=({struct _tagged_arr*_tmp758=_cycalloc(sizeof(*
_tmp758));_tmp758[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp758;});_tmp757.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp757;});_tmp756;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp755;});
_tmp754;});break;case 241: _LL2CB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp759=
_cycalloc(sizeof(*_tmp759));_tmp759[0]=({struct Cyc_Stmt_tok_struct _tmp75A;
_tmp75A.tag=Cyc_Stmt_tok;_tmp75A.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75A;});
_tmp759;});break;case 242: _LL2CC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp75B=
_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=({struct Cyc_Stmt_tok_struct _tmp75C;
_tmp75C.tag=Cyc_Stmt_tok;_tmp75C.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75C;});
_tmp75B;});break;case 243: _LL2CD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp75D=
_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({struct Cyc_Stmt_tok_struct _tmp75E;
_tmp75E.tag=Cyc_Stmt_tok;_tmp75E.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp75E;});
_tmp75D;});break;case 244: _LL2CE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp75F=
_cycalloc(sizeof(*_tmp75F));_tmp75F[0]=({struct Cyc_Stmt_tok_struct _tmp760;
_tmp760.tag=Cyc_Stmt_tok;_tmp760.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp760;});
_tmp75F;});break;case 245: _LL2CF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 246: _LL2D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=({struct Cyc_Stmt_tok_struct
_tmp762;_tmp762.tag=Cyc_Stmt_tok;_tmp762.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp762;});
_tmp761;});break;case 247: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp763=
_cycalloc(sizeof(*_tmp763));_tmp763[0]=({struct Cyc_Stmt_tok_struct _tmp764;
_tmp764.tag=Cyc_Stmt_tok;_tmp764.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp764;});_tmp763;});break;case 248: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765[0]=({struct Cyc_Stmt_tok_struct
_tmp766;_tmp766.tag=Cyc_Stmt_tok;_tmp766.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp766;});_tmp765;});
break;case 249: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 250: _LL2D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767[0]=({struct Cyc_Stmt_tok_struct
_tmp768;_tmp768.tag=Cyc_Stmt_tok;_tmp768.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp768;});
_tmp767;});break;case 251: _LL2D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp769=
_cycalloc(sizeof(*_tmp769));_tmp769[0]=({struct Cyc_Stmt_tok_struct _tmp76A;
_tmp76A.tag=Cyc_Stmt_tok;_tmp76A.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp76C;_tmp76C.tag=1;_tmp76C.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76C;});_tmp76B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp76A;});_tmp769;});break;
case 252: _LL2D6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76D=_cycalloc(
sizeof(*_tmp76D));_tmp76D[0]=({struct Cyc_Stmt_tok_struct _tmp76E;_tmp76E.tag=Cyc_Stmt_tok;
_tmp76E.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp770;_tmp770.tag=1;_tmp770.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp770;});_tmp76F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp76E;});_tmp76D;});
break;case 253: _LL2D7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp771=_cycalloc(
sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Stmt_tok_struct _tmp772;_tmp772.tag=Cyc_Stmt_tok;
_tmp772.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp772;});
_tmp771;});break;case 254: _LL2D8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Stmt_tok_struct _tmp774;
_tmp774.tag=Cyc_Stmt_tok;_tmp774.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp774;});
_tmp773;});break;case 255: _LL2D9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp775=
_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Stmt_tok_struct _tmp776;
_tmp776.tag=Cyc_Stmt_tok;_tmp776.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp776;});
_tmp775;});break;case 256: _LL2DA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp777=
_cycalloc(sizeof(*_tmp777));_tmp777[0]=({struct Cyc_Stmt_tok_struct _tmp778;
_tmp778.tag=Cyc_Stmt_tok;_tmp778.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp778;});
_tmp777;});break;case 257: _LL2DB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp779=
_cycalloc(sizeof(*_tmp779));_tmp779[0]=({struct Cyc_Stmt_tok_struct _tmp77A;
_tmp77A.tag=Cyc_Stmt_tok;_tmp77A.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77A;});
_tmp779;});break;case 258: _LL2DC: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp77B="C";
_tag_arr(_tmp77B,sizeof(char),_get_zero_arr_size(_tmp77B,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp77C="only switch \"C\" { ... } is allowed";_tag_arr(_tmp77C,
sizeof(char),_get_zero_arr_size(_tmp77C,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[0]=({
struct Cyc_Stmt_tok_struct _tmp77E;_tmp77E.tag=Cyc_Stmt_tok;_tmp77E.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));
_tmp77F[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp780;_tmp780.tag=10;_tmp780.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp780.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp780;});_tmp77F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77E;});
_tmp77D;});break;case 259: _LL2DD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp781=
_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Stmt_tok_struct _tmp782;
_tmp782.tag=Cyc_Stmt_tok;_tmp782.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp782;});
_tmp781;});break;case 260: _LL2DE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp783=
_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Stmt_tok_struct _tmp784;
_tmp784.tag=Cyc_Stmt_tok;_tmp784.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp784;});
_tmp783;});break;case 261: _LL2DF: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp786;_tmp786.tag=Cyc_SwitchClauseList_tok;_tmp786.f1=0;_tmp786;});_tmp785;});
break;case 262: _LL2E0: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp787=_cycalloc(sizeof(*_tmp787));_tmp787[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp788;_tmp788.tag=Cyc_SwitchClauseList_tok;_tmp788.f1=({struct Cyc_List_List*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->hd=({struct Cyc_Absyn_Switch_clause*
_tmp78A=_cycalloc(sizeof(*_tmp78A));_tmp78A->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp78A->pat_vars=0;_tmp78A->where_clause=0;
_tmp78A->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp78A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp78A;});
_tmp789->tl=0;_tmp789;});_tmp788;});_tmp787;});break;case 263: _LL2E1: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));
_tmp78B[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp78C;_tmp78C.tag=Cyc_SwitchClauseList_tok;
_tmp78C.f1=({struct Cyc_List_List*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D->hd=({
struct Cyc_Absyn_Switch_clause*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp78E->pat_vars=0;_tmp78E->where_clause=0;_tmp78E->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp78E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp78E;});
_tmp78D->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp78D;});_tmp78C;});_tmp78B;});break;case 264: _LL2E2:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp78F=_cycalloc(sizeof(*
_tmp78F));_tmp78F[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp790;_tmp790.tag=
Cyc_SwitchClauseList_tok;_tmp790.f1=({struct Cyc_List_List*_tmp791=_cycalloc(
sizeof(*_tmp791));_tmp791->hd=({struct Cyc_Absyn_Switch_clause*_tmp792=_cycalloc(
sizeof(*_tmp792));_tmp792->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp792->pat_vars=0;
_tmp792->where_clause=0;_tmp792->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp792->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp792;});
_tmp791->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp791;});_tmp790;});_tmp78F;});break;case 265: _LL2E3:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp793=_cycalloc(sizeof(*
_tmp793));_tmp793[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp794;_tmp794.tag=
Cyc_SwitchClauseList_tok;_tmp794.f1=({struct Cyc_List_List*_tmp795=_cycalloc(
sizeof(*_tmp795));_tmp795->hd=({struct Cyc_Absyn_Switch_clause*_tmp796=_cycalloc(
sizeof(*_tmp796));_tmp796->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp796->pat_vars=0;
_tmp796->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp796->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp796->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp796;});
_tmp795->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp795;});_tmp794;});_tmp793;});break;case 266: _LL2E4:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp797=_cycalloc(sizeof(*
_tmp797));_tmp797[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp798;_tmp798.tag=
Cyc_SwitchClauseList_tok;_tmp798.f1=({struct Cyc_List_List*_tmp799=_cycalloc(
sizeof(*_tmp799));_tmp799->hd=({struct Cyc_Absyn_Switch_clause*_tmp79A=_cycalloc(
sizeof(*_tmp79A));_tmp79A->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp79A->pat_vars=0;
_tmp79A->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp79A->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp79A;});
_tmp799->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp799;});_tmp798;});_tmp797;});break;case 267: _LL2E5:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp79B=_cycalloc(sizeof(*
_tmp79B));_tmp79B[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp79C;_tmp79C.tag=
Cyc_SwitchClauseList_tok;_tmp79C.f1=0;_tmp79C;});_tmp79B;});break;case 268: _LL2E6:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp79D=_cycalloc(sizeof(*
_tmp79D));_tmp79D[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp79E;_tmp79E.tag=
Cyc_SwitchCClauseList_tok;_tmp79E.f1=0;_tmp79E;});_tmp79D;});break;case 269:
_LL2E7: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp79F=_cycalloc(
sizeof(*_tmp79F));_tmp79F[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7A0;
_tmp7A0.tag=Cyc_SwitchCClauseList_tok;_tmp7A0.f1=({struct Cyc_List_List*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2->cnst_exp=0;_tmp7A2->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp7A2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A2;});
_tmp7A1->tl=0;_tmp7A1;});_tmp7A0;});_tmp79F;});break;case 270: _LL2E8: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));
_tmp7A3[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7A4;_tmp7A4.tag=Cyc_SwitchCClauseList_tok;
_tmp7A4.f1=({struct Cyc_List_List*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp7A6->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp7A6->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A6;});
_tmp7A5->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A5;});_tmp7A4;});_tmp7A3;});break;case 271: _LL2E9:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7A7=_cycalloc(sizeof(*
_tmp7A7));_tmp7A7[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7A8;_tmp7A8.tag=
Cyc_SwitchCClauseList_tok;_tmp7A8.f1=({struct Cyc_List_List*_tmp7A9=_cycalloc(
sizeof(*_tmp7A9));_tmp7A9->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7AA=_cycalloc(
sizeof(*_tmp7AA));_tmp7AA->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7AA->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp7AA->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7AA;});
_tmp7A9->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A9;});_tmp7A8;});_tmp7A7;});break;case 272: _LL2EA:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7AB=_cycalloc(sizeof(*
_tmp7AB));_tmp7AB[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7AC;_tmp7AC.tag=
Cyc_SwitchCClauseList_tok;_tmp7AC.f1=0;_tmp7AC;});_tmp7AB;});break;case 273:
_LL2EB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AD=_cycalloc(sizeof(*
_tmp7AD));_tmp7AD[0]=({struct Cyc_Stmt_tok_struct _tmp7AE;_tmp7AE.tag=Cyc_Stmt_tok;
_tmp7AE.f1=Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7AE;});
_tmp7AD;});break;case 274: _LL2EC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Stmt_tok_struct _tmp7B0;
_tmp7B0.tag=Cyc_Stmt_tok;_tmp7B0.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B0;});
_tmp7AF;});break;case 275: _LL2ED: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B1=
_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Stmt_tok_struct _tmp7B2;
_tmp7B2.tag=Cyc_Stmt_tok;_tmp7B2.f1=Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B2;});
_tmp7B1;});break;case 276: _LL2EE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Stmt_tok_struct _tmp7B4;
_tmp7B4.tag=Cyc_Stmt_tok;_tmp7B4.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B4;});
_tmp7B3;});break;case 277: _LL2EF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B5=
_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({struct Cyc_Stmt_tok_struct _tmp7B6;
_tmp7B6.tag=Cyc_Stmt_tok;_tmp7B6.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B6;});
_tmp7B5;});break;case 278: _LL2F0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B7=
_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({struct Cyc_Stmt_tok_struct _tmp7B8;
_tmp7B8.tag=Cyc_Stmt_tok;_tmp7B8.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B8;});
_tmp7B7;});break;case 279: _LL2F1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B9=
_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Stmt_tok_struct _tmp7BA;
_tmp7BA.tag=Cyc_Stmt_tok;_tmp7BA.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BA;});
_tmp7B9;});break;case 280: _LL2F2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BB=
_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Stmt_tok_struct _tmp7BC;
_tmp7BC.tag=Cyc_Stmt_tok;_tmp7BC.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7BC;});
_tmp7BB;});break;case 281: _LL2F3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BD=
_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Stmt_tok_struct _tmp7BE;
_tmp7BE.tag=Cyc_Stmt_tok;_tmp7BE.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BE;});
_tmp7BD;});break;case 282: _LL2F4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BF=
_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({struct Cyc_Stmt_tok_struct _tmp7C0;
_tmp7C0.tag=Cyc_Stmt_tok;_tmp7C0.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C0;});
_tmp7BF;});break;case 283: _LL2F5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C1=
_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=({struct Cyc_Stmt_tok_struct _tmp7C2;
_tmp7C2.tag=Cyc_Stmt_tok;_tmp7C2.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C2;});
_tmp7C1;});break;case 284: _LL2F6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C3=
_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Stmt_tok_struct _tmp7C4;
_tmp7C4.tag=Cyc_Stmt_tok;_tmp7C4.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C4;});
_tmp7C3;});break;case 285: _LL2F7: {struct Cyc_List_List*_tmp7C5=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp7C6=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C7=
_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Stmt_tok_struct _tmp7C8;
_tmp7C8.tag=Cyc_Stmt_tok;_tmp7C8.f1=Cyc_Parse_flatten_declarations(_tmp7C5,
_tmp7C6);_tmp7C8;});_tmp7C7;});break;}case 286: _LL2F8: {struct Cyc_List_List*
_tmp7C9=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7CA=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({
struct Cyc_Stmt_tok_struct _tmp7CC;_tmp7CC.tag=Cyc_Stmt_tok;_tmp7CC.f1=Cyc_Parse_flatten_declarations(
_tmp7C9,_tmp7CA);_tmp7CC;});_tmp7CB;});break;}case 287: _LL2F9: {struct Cyc_List_List*
_tmp7CD=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7CE=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=({
struct Cyc_Stmt_tok_struct _tmp7D0;_tmp7D0.tag=Cyc_Stmt_tok;_tmp7D0.f1=Cyc_Parse_flatten_declarations(
_tmp7CD,_tmp7CE);_tmp7D0;});_tmp7CF;});break;}case 288: _LL2FA: {struct Cyc_List_List*
_tmp7D1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp7D2=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({
struct Cyc_Stmt_tok_struct _tmp7D4;_tmp7D4.tag=Cyc_Stmt_tok;_tmp7D4.f1=Cyc_Parse_flatten_declarations(
_tmp7D1,_tmp7D2);_tmp7D4;});_tmp7D3;});break;}case 289: _LL2FB: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({
struct Cyc_Stmt_tok_struct _tmp7D6;_tmp7D6.tag=Cyc_Stmt_tok;_tmp7D6.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7D6;});_tmp7D5;});break;case 290: _LL2FC: {struct
Cyc_List_List*_tmp7D7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(
struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));
_tmp7D8[0]=({struct Cyc_Stmt_tok_struct _tmp7D9;_tmp7D9.tag=Cyc_Stmt_tok;_tmp7D9.f1=
Cyc_Absyn_forarray_stmt(_tmp7D7,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D9;});
_tmp7D8;});break;}case 291: _LL2FD: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=({struct Cyc_Stmt_tok_struct
_tmp7DB;_tmp7DB.tag=Cyc_Stmt_tok;_tmp7DB.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7DC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7DB;});
_tmp7DA;});break;case 292: _LL2FE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7DD=
_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({struct Cyc_Stmt_tok_struct _tmp7DE;
_tmp7DE.tag=Cyc_Stmt_tok;_tmp7DE.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7DE;});
_tmp7DD;});break;case 293: _LL2FF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7DF=
_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=({struct Cyc_Stmt_tok_struct _tmp7E0;
_tmp7E0.tag=Cyc_Stmt_tok;_tmp7E0.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7E0;});
_tmp7DF;});break;case 294: _LL300: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E1=
_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=({struct Cyc_Stmt_tok_struct _tmp7E2;
_tmp7E2.tag=Cyc_Stmt_tok;_tmp7E2.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7E2;});
_tmp7E1;});break;case 295: _LL301: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E3=
_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Stmt_tok_struct _tmp7E4;
_tmp7E4.tag=Cyc_Stmt_tok;_tmp7E4.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7E4;});_tmp7E3;});break;case 296: _LL302: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({
struct Cyc_Stmt_tok_struct _tmp7E6;_tmp7E6.tag=Cyc_Stmt_tok;_tmp7E6.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7E6;});_tmp7E5;});break;case 297: _LL303: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=({
struct Cyc_Stmt_tok_struct _tmp7E8;_tmp7E8.tag=Cyc_Stmt_tok;_tmp7E8.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp7E8;});_tmp7E7;});break;case 298: _LL304: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=({
struct Cyc_Stmt_tok_struct _tmp7EA;_tmp7EA.tag=Cyc_Stmt_tok;_tmp7EA.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp7EA;});_tmp7E9;});break;case 299: _LL305: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));_tmp7EB[
0]=({struct Cyc_Pattern_tok_struct _tmp7EC;_tmp7EC.tag=Cyc_Pattern_tok;_tmp7EC.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EC;});
_tmp7EB;});break;case 300: _LL306: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 301: _LL307: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({struct Cyc_Pattern_tok_struct
_tmp7EE;_tmp7EE.tag=Cyc_Pattern_tok;_tmp7EE.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct
Cyc_Absyn_Int_p_struct _tmp7F0;_tmp7F0.tag=7;_tmp7F0.f1=(void*)(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp7F0.f2=(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp7F0;});_tmp7EF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7EE;});_tmp7ED;});break;case 302: _LL308: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[
0]=({struct Cyc_Pattern_tok_struct _tmp7F2;_tmp7F2.tag=Cyc_Pattern_tok;_tmp7F2.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp7F3=_cycalloc(
sizeof(*_tmp7F3));_tmp7F3[0]=({struct Cyc_Absyn_Int_p_struct _tmp7F4;_tmp7F4.tag=7;
_tmp7F4.f1=(void*)((void*)0);_tmp7F4.f2=-(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp7F4;});_tmp7F3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F2;});_tmp7F1;});break;case 303: _LL309: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[
0]=({struct Cyc_Pattern_tok_struct _tmp7F6;_tmp7F6.tag=Cyc_Pattern_tok;_tmp7F6.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp7F7=_cycalloc(
sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_Absyn_Float_p_struct _tmp7F8;_tmp7F8.tag=
9;_tmp7F8.f1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F8;});_tmp7F7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F6;});
_tmp7F5;});break;case 304: _LL30A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({struct Cyc_Pattern_tok_struct
_tmp7FA;_tmp7FA.tag=Cyc_Pattern_tok;_tmp7FA.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp7FB=_cycalloc_atomic(sizeof(*_tmp7FB));_tmp7FB[0]=({
struct Cyc_Absyn_Char_p_struct _tmp7FC;_tmp7FC.tag=8;_tmp7FC.f1=Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7FC;});_tmp7FB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7FA;});_tmp7F9;});break;case 305: _LL30B: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[
0]=({struct Cyc_Pattern_tok_struct _tmp7FE;_tmp7FE.tag=Cyc_Pattern_tok;_tmp7FE.f1=
Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FE;});
_tmp7FD;});break;case 306: _LL30C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({struct Cyc_Pattern_tok_struct
_tmp800;_tmp800.tag=Cyc_Pattern_tok;_tmp800.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp802;_tmp802.tag=12;_tmp802.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp802;});_tmp801;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp800;});_tmp7FF;});break;case 307: _LL30D: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803[
0]=({struct Cyc_Pattern_tok_struct _tmp804;_tmp804.tag=Cyc_Pattern_tok;_tmp804.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp805=_cycalloc(
sizeof(*_tmp805));_tmp805[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp806;_tmp806.tag=
3;_tmp806.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]));_tmp806;});_tmp805;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp804;});
_tmp803;});break;case 308: _LL30E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Pattern_tok_struct
_tmp808;_tmp808.tag=Cyc_Pattern_tok;_tmp808.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownCall_p_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp80A;_tmp80A.tag=13;_tmp80A.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp80A.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp80A;});_tmp809;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp808;});_tmp807;});break;case 309: _LL30F: {struct
Cyc_List_List*_tmp80B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({
struct Cyc_Pattern_tok_struct _tmp80D;_tmp80D.tag=Cyc_Pattern_tok;_tmp80D.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp80F;_tmp80F.tag=5;_tmp80F.f1=({
struct Cyc_Absyn_AggrInfo _tmp810;_tmp810.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp812;_tmp812.tag=0;_tmp812.f1=(void*)((void*)0);_tmp812.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp812;});_tmp811;}));
_tmp810.targs=0;_tmp810;});_tmp80F.f2=_tmp80B;_tmp80F.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp80F;});_tmp80E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80D;});_tmp80C;});break;}case 310: _LL310: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[
0]=({struct Cyc_Pattern_tok_struct _tmp814;_tmp814.tag=Cyc_Pattern_tok;_tmp814.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp815=_cycalloc(
sizeof(*_tmp815));_tmp815[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp816;_tmp816.tag=
4;_tmp816.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp816;});_tmp815;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp814;});
_tmp813;});break;case 311: _LL311: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=({struct Cyc_Pattern_tok_struct
_tmp818;_tmp818.tag=Cyc_Pattern_tok;_tmp818.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp81A;_tmp81A.tag=1;_tmp81A.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B->f1=(void*)0;_tmp81B->f2=({
struct _tagged_arr*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81C;});_tmp81B;}),(
void*)0,0);_tmp81A;});_tmp819;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp818;});
_tmp817;});break;case 312: _LL312: {void*_tmp81D=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp827;_tmp827.tag=0;_tmp827.f1=(void*)((void*)5);_tmp827;});_tmp826;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[
0]=({struct Cyc_Pattern_tok_struct _tmp81F;_tmp81F.tag=Cyc_Pattern_tok;_tmp81F.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp820=_cycalloc(
sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp821;_tmp821.tag=
2;_tmp821.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp81D);
_tmp821.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp822=_cycalloc(sizeof(*
_tmp822));_tmp822->f1=(void*)0;_tmp822->f2=({struct _tagged_arr*_tmp823=_cycalloc(
sizeof(*_tmp823));_tmp823[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp823;});_tmp822;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp824=_cycalloc(sizeof(*_tmp824));
_tmp824[0]=({struct Cyc_Absyn_TagType_struct _tmp825;_tmp825.tag=17;_tmp825.f1=(
void*)_tmp81D;_tmp825;});_tmp824;}),0);_tmp821;});_tmp820;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81F;});
_tmp81E;});break;}case 313: _LL313: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_PatternList_tok_struct
_tmp829;_tmp829.tag=Cyc_PatternList_tok;_tmp829.f1=({struct Cyc_List_List*_tmp82A=
_cycalloc(sizeof(*_tmp82A));_tmp82A->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp82A->tl=0;_tmp82A;});
_tmp829;});_tmp828;});break;case 314: _LL314: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_PatternList_tok_struct
_tmp82C;_tmp82C.tag=Cyc_PatternList_tok;_tmp82C.f1=({struct Cyc_List_List*_tmp82D=
_cycalloc(sizeof(*_tmp82D));_tmp82D->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp82D->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp82D;});_tmp82C;});
_tmp82B;});break;case 315: _LL315: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_FieldPattern_tok_struct
_tmp82F;_tmp82F.tag=Cyc_FieldPattern_tok;_tmp82F.f1=({struct _tuple11*_tmp830=
_cycalloc(sizeof(*_tmp830));_tmp830->f1=0;_tmp830->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp830;});_tmp82F;});
_tmp82E;});break;case 316: _LL316: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=({struct Cyc_FieldPattern_tok_struct
_tmp832;_tmp832.tag=Cyc_FieldPattern_tok;_tmp832.f1=({struct _tuple11*_tmp833=
_cycalloc(sizeof(*_tmp833));_tmp833->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp833->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp833;});_tmp832;});
_tmp831;});break;case 317: _LL317: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp835;_tmp835.tag=Cyc_FieldPatternList_tok;_tmp835.f1=({struct Cyc_List_List*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp836->tl=0;_tmp836;});
_tmp835;});_tmp834;});break;case 318: _LL318: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp837=_cycalloc(sizeof(*_tmp837));_tmp837[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp838;_tmp838.tag=Cyc_FieldPatternList_tok;_tmp838.f1=({struct Cyc_List_List*
_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp839->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp839;});_tmp838;});
_tmp837;});break;case 319: _LL319: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 320: _LL31A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Exp_tok_struct _tmp83B;
_tmp83B.tag=Cyc_Exp_tok;_tmp83B.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83B;});
_tmp83A;});break;case 321: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL31C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Exp_tok_struct _tmp83D;
_tmp83D.tag=Cyc_Exp_tok;_tmp83D.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
_tmp83C;});break;case 323: _LL31D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Primopopt_tok_struct
_tmp83F;_tmp83F.tag=Cyc_Primopopt_tok;_tmp83F.f1=0;_tmp83F;});_tmp83E;});break;
case 324: _LL31E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp840=_cycalloc(
sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Primopopt_tok_struct _tmp841;_tmp841.tag=
Cyc_Primopopt_tok;_tmp841.f1=({struct Cyc_Core_Opt*_tmp842=_cycalloc(sizeof(*
_tmp842));_tmp842->v=(void*)((void*)1);_tmp842;});_tmp841;});_tmp840;});break;
case 325: _LL31F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp843=_cycalloc(
sizeof(*_tmp843));_tmp843[0]=({struct Cyc_Primopopt_tok_struct _tmp844;_tmp844.tag=
Cyc_Primopopt_tok;_tmp844.f1=({struct Cyc_Core_Opt*_tmp845=_cycalloc(sizeof(*
_tmp845));_tmp845->v=(void*)((void*)3);_tmp845;});_tmp844;});_tmp843;});break;
case 326: _LL320: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp846=_cycalloc(
sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Primopopt_tok_struct _tmp847;_tmp847.tag=
Cyc_Primopopt_tok;_tmp847.f1=({struct Cyc_Core_Opt*_tmp848=_cycalloc(sizeof(*
_tmp848));_tmp848->v=(void*)((void*)4);_tmp848;});_tmp847;});_tmp846;});break;
case 327: _LL321: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp849=_cycalloc(
sizeof(*_tmp849));_tmp849[0]=({struct Cyc_Primopopt_tok_struct _tmp84A;_tmp84A.tag=
Cyc_Primopopt_tok;_tmp84A.f1=({struct Cyc_Core_Opt*_tmp84B=_cycalloc(sizeof(*
_tmp84B));_tmp84B->v=(void*)((void*)0);_tmp84B;});_tmp84A;});_tmp849;});break;
case 328: _LL322: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp84C=_cycalloc(
sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_Primopopt_tok_struct _tmp84D;_tmp84D.tag=
Cyc_Primopopt_tok;_tmp84D.f1=({struct Cyc_Core_Opt*_tmp84E=_cycalloc(sizeof(*
_tmp84E));_tmp84E->v=(void*)((void*)2);_tmp84E;});_tmp84D;});_tmp84C;});break;
case 329: _LL323: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp84F=_cycalloc(
sizeof(*_tmp84F));_tmp84F[0]=({struct Cyc_Primopopt_tok_struct _tmp850;_tmp850.tag=
Cyc_Primopopt_tok;_tmp850.f1=({struct Cyc_Core_Opt*_tmp851=_cycalloc(sizeof(*
_tmp851));_tmp851->v=(void*)((void*)16);_tmp851;});_tmp850;});_tmp84F;});break;
case 330: _LL324: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp852=_cycalloc(
sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Primopopt_tok_struct _tmp853;_tmp853.tag=
Cyc_Primopopt_tok;_tmp853.f1=({struct Cyc_Core_Opt*_tmp854=_cycalloc(sizeof(*
_tmp854));_tmp854->v=(void*)((void*)17);_tmp854;});_tmp853;});_tmp852;});break;
case 331: _LL325: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp855=_cycalloc(
sizeof(*_tmp855));_tmp855[0]=({struct Cyc_Primopopt_tok_struct _tmp856;_tmp856.tag=
Cyc_Primopopt_tok;_tmp856.f1=({struct Cyc_Core_Opt*_tmp857=_cycalloc(sizeof(*
_tmp857));_tmp857->v=(void*)((void*)13);_tmp857;});_tmp856;});_tmp855;});break;
case 332: _LL326: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp858=_cycalloc(
sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Primopopt_tok_struct _tmp859;_tmp859.tag=
Cyc_Primopopt_tok;_tmp859.f1=({struct Cyc_Core_Opt*_tmp85A=_cycalloc(sizeof(*
_tmp85A));_tmp85A->v=(void*)((void*)15);_tmp85A;});_tmp859;});_tmp858;});break;
case 333: _LL327: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp85B=_cycalloc(
sizeof(*_tmp85B));_tmp85B[0]=({struct Cyc_Primopopt_tok_struct _tmp85C;_tmp85C.tag=
Cyc_Primopopt_tok;_tmp85C.f1=({struct Cyc_Core_Opt*_tmp85D=_cycalloc(sizeof(*
_tmp85D));_tmp85D->v=(void*)((void*)14);_tmp85D;});_tmp85C;});_tmp85B;});break;
case 334: _LL328: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 335: _LL329: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp85E=_cycalloc(
sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_Exp_tok_struct _tmp85F;_tmp85F.tag=Cyc_Exp_tok;
_tmp85F.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
_tmp85E;});break;case 336: _LL32A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Exp_tok_struct _tmp861;_tmp861.tag=
Cyc_Exp_tok;_tmp861.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
_tmp860;});break;case 337: _LL32B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp862=
_cycalloc(sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Exp_tok_struct _tmp863;_tmp863.tag=
Cyc_Exp_tok;_tmp863.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
_tmp862;});break;case 338: _LL32C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp864=
_cycalloc(sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Exp_tok_struct _tmp865;_tmp865.tag=
Cyc_Exp_tok;_tmp865.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
_tmp864;});break;case 339: _LL32D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Exp_tok_struct _tmp867;_tmp867.tag=
Cyc_Exp_tok;_tmp867.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
_tmp866;});break;case 340: _LL32E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp868=
_cycalloc(sizeof(*_tmp868));_tmp868[0]=({struct Cyc_Exp_tok_struct _tmp869;_tmp869.tag=
Cyc_Exp_tok;_tmp869.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
_tmp868;});break;case 341: _LL32F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 342: _LL330: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 343: _LL331: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A[0]=({struct Cyc_Exp_tok_struct _tmp86B;
_tmp86B.tag=Cyc_Exp_tok;_tmp86B.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
_tmp86A;});break;case 344: _LL332: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL333: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({struct Cyc_Exp_tok_struct _tmp86D;
_tmp86D.tag=Cyc_Exp_tok;_tmp86D.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86D;});
_tmp86C;});break;case 346: _LL334: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 347: _LL335: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Exp_tok_struct _tmp86F;
_tmp86F.tag=Cyc_Exp_tok;_tmp86F.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86F;});
_tmp86E;});break;case 348: _LL336: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL337: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Exp_tok_struct _tmp871;
_tmp871.tag=Cyc_Exp_tok;_tmp871.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
_tmp870;});break;case 350: _LL338: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL339: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Exp_tok_struct _tmp873;
_tmp873.tag=Cyc_Exp_tok;_tmp873.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
_tmp872;});break;case 352: _LL33A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL33B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874[0]=({struct Cyc_Exp_tok_struct _tmp875;
_tmp875.tag=Cyc_Exp_tok;_tmp875.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp875;});
_tmp874;});break;case 354: _LL33C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp876=
_cycalloc(sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Exp_tok_struct _tmp877;_tmp877.tag=
Cyc_Exp_tok;_tmp877.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
_tmp876;});break;case 355: _LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 356: _LL33E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878[0]=({struct Cyc_Exp_tok_struct _tmp879;
_tmp879.tag=Cyc_Exp_tok;_tmp879.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp879;});
_tmp878;});break;case 357: _LL33F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87A=
_cycalloc(sizeof(*_tmp87A));_tmp87A[0]=({struct Cyc_Exp_tok_struct _tmp87B;_tmp87B.tag=
Cyc_Exp_tok;_tmp87B.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
_tmp87A;});break;case 358: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87C=
_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Exp_tok_struct _tmp87D;_tmp87D.tag=
Cyc_Exp_tok;_tmp87D.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87D;});
_tmp87C;});break;case 359: _LL341: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87E=
_cycalloc(sizeof(*_tmp87E));_tmp87E[0]=({struct Cyc_Exp_tok_struct _tmp87F;_tmp87F.tag=
Cyc_Exp_tok;_tmp87F.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87F;});
_tmp87E;});break;case 360: _LL342: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 361: _LL343: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880[0]=({struct Cyc_Exp_tok_struct _tmp881;
_tmp881.tag=Cyc_Exp_tok;_tmp881.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp881;});
_tmp880;});break;case 362: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp882=
_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Exp_tok_struct _tmp883;_tmp883.tag=
Cyc_Exp_tok;_tmp883.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp883;});
_tmp882;});break;case 363: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 364: _LL346: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp884=_cycalloc(sizeof(*_tmp884));_tmp884[0]=({struct Cyc_Exp_tok_struct _tmp885;
_tmp885.tag=Cyc_Exp_tok;_tmp885.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
_tmp884;});break;case 365: _LL347: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp886=
_cycalloc(sizeof(*_tmp886));_tmp886[0]=({struct Cyc_Exp_tok_struct _tmp887;_tmp887.tag=
Cyc_Exp_tok;_tmp887.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
_tmp886;});break;case 366: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 367: _LL349: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888[0]=({struct Cyc_Exp_tok_struct _tmp889;
_tmp889.tag=Cyc_Exp_tok;_tmp889.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp889;});
_tmp888;});break;case 368: _LL34A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp88A=
_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=({struct Cyc_Exp_tok_struct _tmp88B;_tmp88B.tag=
Cyc_Exp_tok;_tmp88B.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
_tmp88A;});break;case 369: _LL34B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp88C=
_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Exp_tok_struct _tmp88D;_tmp88D.tag=
Cyc_Exp_tok;_tmp88D.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88D;});
_tmp88C;});break;case 370: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 371: _LL34D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=({struct Cyc_Exp_tok_struct _tmp88F;
_tmp88F.tag=Cyc_Exp_tok;_tmp88F.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88F;});
_tmp88E;});break;case 372: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 373: _LL34F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890[0]=({struct Cyc_Exp_tok_struct _tmp891;
_tmp891.tag=Cyc_Exp_tok;_tmp891.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp891;});
_tmp890;});break;case 374: _LL350: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp892=
_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Exp_tok_struct _tmp893;_tmp893.tag=
Cyc_Exp_tok;_tmp893.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp893;});
_tmp892;});break;case 375: _LL351: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp894=
_cycalloc(sizeof(*_tmp894));_tmp894[0]=({struct Cyc_Exp_tok_struct _tmp895;_tmp895.tag=
Cyc_Exp_tok;_tmp895.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp895;});
_tmp894;});break;case 376: _LL352: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp896=
_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Exp_tok_struct _tmp897;_tmp897.tag=
Cyc_Exp_tok;_tmp897.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
_tmp896;});break;case 377: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 378: _LL354: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp898=_cycalloc(sizeof(*_tmp898));_tmp898[0]=({struct Cyc_Exp_tok_struct _tmp899;
_tmp899.tag=Cyc_Exp_tok;_tmp899.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
_tmp898;});break;case 379: _LL355: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89A=
_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Exp_tok_struct _tmp89B;_tmp89B.tag=
Cyc_Exp_tok;_tmp89B.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89B;});
_tmp89A;});break;case 380: _LL356: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89C=
_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=({struct Cyc_Exp_tok_struct _tmp89D;_tmp89D.tag=
Cyc_Exp_tok;_tmp89D.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89D;});
_tmp89C;});break;case 381: _LL357: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89E=
_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Exp_tok_struct _tmp89F;_tmp89F.tag=
Cyc_Exp_tok;_tmp89F.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({struct Cyc_Absyn_StructField_struct
_tmp8A1;_tmp8A1.tag=0;_tmp8A1.f1=({struct _tagged_arr*_tmp8A2=_cycalloc(sizeof(*
_tmp8A2));_tmp8A2[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp8A2;});_tmp8A1;});_tmp8A0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89F;});
_tmp89E;});break;case 382: _LL358: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A3=
_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Exp_tok_struct _tmp8A4;_tmp8A4.tag=
Cyc_Exp_tok;_tmp8A4.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp8A5=_cycalloc_atomic(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp8A6;_tmp8A6.tag=1;_tmp8A6.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8A6;});_tmp8A5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8A4;});_tmp8A3;});break;case 383: _LL359: {struct
Cyc_Position_Segment*_tmp8A7=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp8A8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp8A7,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A9=_cycalloc(
sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Exp_tok_struct _tmp8AA;_tmp8AA.tag=Cyc_Exp_tok;
_tmp8AA.f1=Cyc_Absyn_gentyp_exp(_tmp8A8,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AA;});
_tmp8A9;});break;}case 384: _LL35A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8AB=
_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Exp_tok_struct _tmp8AC;_tmp8AC.tag=
Cyc_Exp_tok;_tmp8AC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8AE;_tmp8AE.tag=33;_tmp8AE.f1=({struct Cyc_Absyn_MallocInfo _tmp8AF;_tmp8AF.is_calloc=
0;_tmp8AF.rgn=0;_tmp8AF.elt_type=0;_tmp8AF.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8AF.fat_result=0;
_tmp8AF;});_tmp8AE;});_tmp8AD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AC;});
_tmp8AB;});break;case 385: _LL35B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B0=
_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=({struct Cyc_Exp_tok_struct _tmp8B1;_tmp8B1.tag=
Cyc_Exp_tok;_tmp8B1.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8B2=_cycalloc(sizeof(*_tmp8B2));_tmp8B2[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8B3;_tmp8B3.tag=33;_tmp8B3.f1=({struct Cyc_Absyn_MallocInfo _tmp8B4;_tmp8B4.is_calloc=
0;_tmp8B4.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8B4.elt_type=0;
_tmp8B4.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8B4.fat_result=0;_tmp8B4;});_tmp8B3;});_tmp8B2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B1;});
_tmp8B0;});break;case 386: _LL35C: {void*_tmp8B6;struct _tuple2 _tmp8B5=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8B6=_tmp8B5.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));
_tmp8B7[0]=({struct Cyc_Exp_tok_struct _tmp8B8;_tmp8B8.tag=Cyc_Exp_tok;_tmp8B8.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8B9=_cycalloc(
sizeof(*_tmp8B9));_tmp8B9[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8BA;_tmp8BA.tag=
33;_tmp8BA.f1=({struct Cyc_Absyn_MallocInfo _tmp8BB;_tmp8BB.is_calloc=1;_tmp8BB.rgn=
0;_tmp8BB.elt_type=({void**_tmp8BC=_cycalloc(sizeof(*_tmp8BC));_tmp8BC[0]=
_tmp8B6;_tmp8BC;});_tmp8BB.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8BB.fat_result=0;
_tmp8BB;});_tmp8BA;});_tmp8B9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B8;});
_tmp8B7;});break;}case 387: _LL35D: {void*_tmp8BE;struct _tuple2 _tmp8BD=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8BE=_tmp8BD.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BF=_cycalloc(sizeof(*_tmp8BF));
_tmp8BF[0]=({struct Cyc_Exp_tok_struct _tmp8C0;_tmp8C0.tag=Cyc_Exp_tok;_tmp8C0.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8C1=_cycalloc(
sizeof(*_tmp8C1));_tmp8C1[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8C2;_tmp8C2.tag=
33;_tmp8C2.f1=({struct Cyc_Absyn_MallocInfo _tmp8C3;_tmp8C3.is_calloc=1;_tmp8C3.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp8C3.elt_type=({void**_tmp8C4=_cycalloc(sizeof(*_tmp8C4));
_tmp8C4[0]=_tmp8BE;_tmp8C4;});_tmp8C3.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8C3.fat_result=0;
_tmp8C3;});_tmp8C2;});_tmp8C1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C0;});
_tmp8BF;});break;}case 388: _LL35E: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8C5=_cycalloc(sizeof(*_tmp8C5));_tmp8C5[0]=({struct Cyc_Primop_tok_struct
_tmp8C6;_tmp8C6.tag=Cyc_Primop_tok;_tmp8C6.f1=(void*)((void*)12);_tmp8C6;});
_tmp8C5;});break;case 389: _LL35F: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8C7=_cycalloc(sizeof(*_tmp8C7));_tmp8C7[0]=({struct Cyc_Primop_tok_struct
_tmp8C8;_tmp8C8.tag=Cyc_Primop_tok;_tmp8C8.f1=(void*)((void*)11);_tmp8C8;});
_tmp8C7;});break;case 390: _LL360: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({struct Cyc_Primop_tok_struct
_tmp8CA;_tmp8CA.tag=Cyc_Primop_tok;_tmp8CA.f1=(void*)((void*)2);_tmp8CA;});
_tmp8C9;});break;case 391: _LL361: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL362: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Exp_tok_struct _tmp8CC;
_tmp8CC.tag=Cyc_Exp_tok;_tmp8CC.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CC;});
_tmp8CB;});break;case 393: _LL363: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CD=
_cycalloc(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Exp_tok_struct _tmp8CE;_tmp8CE.tag=
Cyc_Exp_tok;_tmp8CE.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CE;});
_tmp8CD;});break;case 394: _LL364: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Exp_tok_struct _tmp8D0;_tmp8D0.tag=
Cyc_Exp_tok;_tmp8D0.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D0;});
_tmp8CF;});break;case 395: _LL365: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D1=
_cycalloc(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Exp_tok_struct _tmp8D2;_tmp8D2.tag=
Cyc_Exp_tok;_tmp8D2.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8D3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D2;});
_tmp8D1;});break;case 396: _LL366: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D4=
_cycalloc(sizeof(*_tmp8D4));_tmp8D4[0]=({struct Cyc_Exp_tok_struct _tmp8D5;_tmp8D5.tag=
Cyc_Exp_tok;_tmp8D5.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8D6=_cycalloc(sizeof(*_tmp8D6));_tmp8D6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8D6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D5;});
_tmp8D4;});break;case 397: _LL367: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D7=
_cycalloc(sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_Exp_tok_struct _tmp8D8;_tmp8D8.tag=
Cyc_Exp_tok;_tmp8D8.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D8;});
_tmp8D7;});break;case 398: _LL368: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D9=
_cycalloc(sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_Exp_tok_struct _tmp8DA;_tmp8DA.tag=
Cyc_Exp_tok;_tmp8DA.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8DA;});
_tmp8D9;});break;case 399: _LL369: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8DB=
_cycalloc(sizeof(*_tmp8DB));_tmp8DB[0]=({struct Cyc_Exp_tok_struct _tmp8DC;_tmp8DC.tag=
Cyc_Exp_tok;_tmp8DC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp8DD=_cycalloc(sizeof(*_tmp8DD));_tmp8DD[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp8DE;_tmp8DE.tag=25;_tmp8DE.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8DE.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8DE;});_tmp8DD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8DC;});_tmp8DB;});break;case 400: _LL36A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8DF=_cycalloc(sizeof(*_tmp8DF));_tmp8DF[0]=({
struct Cyc_Exp_tok_struct _tmp8E0;_tmp8E0.tag=Cyc_Exp_tok;_tmp8E0.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8E1=_cycalloc(sizeof(*_tmp8E1));
_tmp8E1[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8E2;_tmp8E2.tag=25;_tmp8E2.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp8E2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp8E2;});_tmp8E1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E0;});
_tmp8DF;});break;case 401: _LL36B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8E3=
_cycalloc(sizeof(*_tmp8E3));_tmp8E3[0]=({struct Cyc_Exp_tok_struct _tmp8E4;_tmp8E4.tag=
Cyc_Exp_tok;_tmp8E4.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp8E5=_cycalloc(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp8E6;_tmp8E6.tag=37;_tmp8E6.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8E6;});_tmp8E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8E4;});_tmp8E3;});break;case 402: _LL36C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({
struct Cyc_Exp_tok_struct _tmp8E8;_tmp8E8.tag=Cyc_Exp_tok;_tmp8E8.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp8E9=_cycalloc(sizeof(*_tmp8E9));
_tmp8E9[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp8EA;_tmp8EA.tag=36;_tmp8EA.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8EA;});_tmp8E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E8;});
_tmp8E7;});break;case 403: _LL36D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EB=
_cycalloc(sizeof(*_tmp8EB));_tmp8EB[0]=({struct Cyc_Exp_tok_struct _tmp8EC;_tmp8EC.tag=
Cyc_Exp_tok;_tmp8EC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp8EE;_tmp8EE.tag=2;_tmp8EE.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8EE;});_tmp8ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EC;});
_tmp8EB;});break;case 404: _LL36E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL36F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8EF=_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Exp_tok_struct _tmp8F0;
_tmp8F0.tag=Cyc_Exp_tok;_tmp8F0.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F0;});
_tmp8EF;});break;case 406: _LL370: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 407: _LL371: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8F1=_cycalloc(sizeof(*_tmp8F1));_tmp8F1[0]=({struct Cyc_Exp_tok_struct _tmp8F2;
_tmp8F2.tag=Cyc_Exp_tok;_tmp8F2.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp8F2;});
_tmp8F1;});break;case 408: _LL372: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F3=
_cycalloc(sizeof(*_tmp8F3));_tmp8F3[0]=({struct Cyc_Exp_tok_struct _tmp8F4;_tmp8F4.tag=
Cyc_Exp_tok;_tmp8F4.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F4;});
_tmp8F3;});break;case 409: _LL373: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F5=
_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct Cyc_Exp_tok_struct _tmp8F6;_tmp8F6.tag=
Cyc_Exp_tok;_tmp8F6.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F6;});
_tmp8F5;});break;case 410: _LL374: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F7=
_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Exp_tok_struct _tmp8F8;_tmp8F8.tag=
Cyc_Exp_tok;_tmp8F8.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F8;});
_tmp8F7;});break;case 411: _LL375: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F9=
_cycalloc(sizeof(*_tmp8F9));_tmp8F9[0]=({struct Cyc_Exp_tok_struct _tmp8FA;_tmp8FA.tag=
Cyc_Exp_tok;_tmp8FA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp8FC;_tmp8FC.tag=28;_tmp8FC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8FC.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8FC.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8FC.f4=0;
_tmp8FC;});_tmp8FB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FA;});
_tmp8F9;});break;case 412: _LL376: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8FD=
_cycalloc(sizeof(*_tmp8FD));_tmp8FD[0]=({struct Cyc_Exp_tok_struct _tmp8FE;_tmp8FE.tag=
Cyc_Exp_tok;_tmp8FE.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FE;});
_tmp8FD;});break;case 413: _LL377: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF[0]=({struct Cyc_ExpList_tok_struct
_tmp900;_tmp900.tag=Cyc_ExpList_tok;_tmp900.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp900;});_tmp8FF;});
break;case 414: _LL378: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp901=
_cycalloc(sizeof(*_tmp901));_tmp901[0]=({struct Cyc_ExpList_tok_struct _tmp902;
_tmp902.tag=Cyc_ExpList_tok;_tmp902.f1=({struct Cyc_List_List*_tmp903=_cycalloc(
sizeof(*_tmp903));_tmp903->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp903->tl=0;_tmp903;});
_tmp902;});_tmp901;});break;case 415: _LL379: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904[0]=({struct Cyc_ExpList_tok_struct
_tmp905;_tmp905.tag=Cyc_ExpList_tok;_tmp905.f1=({struct Cyc_List_List*_tmp906=
_cycalloc(sizeof(*_tmp906));_tmp906->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp906->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp906;});_tmp905;});
_tmp904;});break;case 416: _LL37A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp907=
_cycalloc(sizeof(*_tmp907));_tmp907[0]=({struct Cyc_Exp_tok_struct _tmp908;_tmp908.tag=
Cyc_Exp_tok;_tmp908.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp908;});
_tmp907;});break;case 417: _LL37B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp909=
_cycalloc(sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Exp_tok_struct _tmp90A;_tmp90A.tag=
Cyc_Exp_tok;_tmp90A.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90A;});
_tmp909;});break;case 418: _LL37C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp90B=
_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=({struct Cyc_Exp_tok_struct _tmp90C;_tmp90C.tag=
Cyc_Exp_tok;_tmp90C.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90C;});
_tmp90B;});break;case 419: _LL37D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp90D=
_cycalloc(sizeof(*_tmp90D));_tmp90D[0]=({struct Cyc_Exp_tok_struct _tmp90E;_tmp90E.tag=
Cyc_Exp_tok;_tmp90E.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90E;});
_tmp90D;});break;case 420: _LL37E: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp90F=_cycalloc(sizeof(*_tmp90F));_tmp90F[0]=({struct Cyc_QualId_tok_struct
_tmp910;_tmp910.tag=Cyc_QualId_tok;_tmp910.f1=({struct _tuple1*_tmp911=_cycalloc(
sizeof(*_tmp911));_tmp911->f1=Cyc_Absyn_rel_ns_null;_tmp911->f2=({struct
_tagged_arr*_tmp912=_cycalloc(sizeof(*_tmp912));_tmp912[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp912;});_tmp911;});
_tmp910;});_tmp90F;});break;case 421: _LL37F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 422: _LL380: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp913=_cycalloc(sizeof(*_tmp913));_tmp913[
0]=({struct Cyc_QualId_tok_struct _tmp914;_tmp914.tag=Cyc_QualId_tok;_tmp914.f1=({
struct _tuple1*_tmp915=_cycalloc(sizeof(*_tmp915));_tmp915->f1=Cyc_Absyn_rel_ns_null;
_tmp915->f2=({struct _tagged_arr*_tmp916=_cycalloc(sizeof(*_tmp916));_tmp916[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp916;});_tmp915;});_tmp914;});_tmp913;});break;case 423: _LL381: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 424: _LL382: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 425: _LL383:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 426:
_LL384: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL385: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 428: _LL386: break;case 429: _LL387:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL388: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(430,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(116,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0?yystate <= 
6147: 0)?Cyc_yycheck[_check_known_subscript_notnull(6148,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)]: 0)yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6148,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(116,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(876,yystate)];if(yyn > - 32768?yyn < 6147: 0){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 247 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6148,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)])
+ 15,count ++);}msg=({unsigned int _tmp917=(unsigned int)(sze + 15);char*_tmp918=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp917 + 1));struct _tagged_arr
_tmp91A=_tag_arr(_tmp918,sizeof(char),_tmp917 + 1);{unsigned int _tmp919=_tmp917;
unsigned int i;for(i=0;i < _tmp919;i ++){_tmp918[i]='\000';}_tmp918[_tmp919]=(char)
0;}_tmp91A;});Cyc_strcpy(msg,({const char*_tmp91B="parse error";_tag_arr(_tmp91B,
sizeof(char),_get_zero_arr_size(_tmp91B,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 247 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(6148,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?({const char*_tmp91C=", expecting `";_tag_arr(_tmp91C,
sizeof(char),_get_zero_arr_size(_tmp91C,14));}):({const char*_tmp91D=" or `";
_tag_arr(_tmp91D,sizeof(char),_get_zero_arr_size(_tmp91D,6));})));Cyc_strcat(msg,(
struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)]);Cyc_strcat(
msg,({const char*_tmp91E="'";_tag_arr(_tmp91E,sizeof(char),_get_zero_arr_size(
_tmp91E,2));}));count ++;}}}Cyc_yyerror((struct _tagged_arr)msg);}else{Cyc_yyerror(({
const char*_tmp91F="parse error";_tag_arr(_tmp91F,sizeof(char),_get_zero_arr_size(
_tmp91F,12));}));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)
return 1;Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0)return 1;yyvsp_offset --;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];yylsp_offset --;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(876,yystate)];if(yyn == - 32768)
goto yyerrdefault;yyn +=1;if((yyn < 0?1: yyn > 6147)?1: Cyc_yycheck[
_check_known_subscript_notnull(6148,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6148,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 875)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp929=v;struct _tuple13*_tmp92A;
struct _tuple13 _tmp92B;int _tmp92C;char _tmp92D;short _tmp92E;struct _tagged_arr
_tmp92F;struct Cyc_Core_Opt*_tmp930;struct Cyc_Core_Opt*_tmp931;struct Cyc_Core_Opt
_tmp932;struct _tagged_arr*_tmp933;struct _tuple1*_tmp934;struct _tuple1 _tmp935;
void*_tmp936;struct _tagged_arr*_tmp937;_LL38B: if(*((void**)_tmp929)!= Cyc_Int_tok)
goto _LL38D;_tmp92A=((struct Cyc_Int_tok_struct*)_tmp929)->f1;_tmp92B=*_tmp92A;
_tmp92C=_tmp92B.f2;_LL38C:({struct Cyc_Int_pa_struct _tmp93A;_tmp93A.tag=1;_tmp93A.f1=(
unsigned int)_tmp92C;{void*_tmp938[1]={& _tmp93A};Cyc_fprintf(Cyc_stderr,({const
char*_tmp939="%d";_tag_arr(_tmp939,sizeof(char),_get_zero_arr_size(_tmp939,3));}),
_tag_arr(_tmp938,sizeof(void*),1));}});goto _LL38A;_LL38D: if(*((void**)_tmp929)!= 
Cyc_Char_tok)goto _LL38F;_tmp92D=((struct Cyc_Char_tok_struct*)_tmp929)->f1;_LL38E:({
struct Cyc_Int_pa_struct _tmp93D;_tmp93D.tag=1;_tmp93D.f1=(unsigned int)((int)
_tmp92D);{void*_tmp93B[1]={& _tmp93D};Cyc_fprintf(Cyc_stderr,({const char*_tmp93C="%c";
_tag_arr(_tmp93C,sizeof(char),_get_zero_arr_size(_tmp93C,3));}),_tag_arr(_tmp93B,
sizeof(void*),1));}});goto _LL38A;_LL38F: if(*((void**)_tmp929)!= Cyc_Short_tok)
goto _LL391;_tmp92E=((struct Cyc_Short_tok_struct*)_tmp929)->f1;_LL390:({struct Cyc_Int_pa_struct
_tmp940;_tmp940.tag=1;_tmp940.f1=(unsigned int)((int)_tmp92E);{void*_tmp93E[1]={&
_tmp940};Cyc_fprintf(Cyc_stderr,({const char*_tmp93F="%ds";_tag_arr(_tmp93F,
sizeof(char),_get_zero_arr_size(_tmp93F,4));}),_tag_arr(_tmp93E,sizeof(void*),1));}});
goto _LL38A;_LL391: if(*((void**)_tmp929)!= Cyc_String_tok)goto _LL393;_tmp92F=((
struct Cyc_String_tok_struct*)_tmp929)->f1;_LL392:({struct Cyc_String_pa_struct
_tmp943;_tmp943.tag=0;_tmp943.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp92F);{
void*_tmp941[1]={& _tmp943};Cyc_fprintf(Cyc_stderr,({const char*_tmp942="\"%s\"";
_tag_arr(_tmp942,sizeof(char),_get_zero_arr_size(_tmp942,5));}),_tag_arr(_tmp941,
sizeof(void*),1));}});goto _LL38A;_LL393: if(*((void**)_tmp929)!= Cyc_Stringopt_tok)
goto _LL395;_tmp930=((struct Cyc_Stringopt_tok_struct*)_tmp929)->f1;if(_tmp930 != 0)
goto _LL395;_LL394:({void*_tmp944[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp945="NULL";_tag_arr(_tmp945,sizeof(char),_get_zero_arr_size(_tmp945,5));}),
_tag_arr(_tmp944,sizeof(void*),0));});goto _LL38A;_LL395: if(*((void**)_tmp929)!= 
Cyc_Stringopt_tok)goto _LL397;_tmp931=((struct Cyc_Stringopt_tok_struct*)_tmp929)->f1;
if(_tmp931 == 0)goto _LL397;_tmp932=*_tmp931;_tmp933=(struct _tagged_arr*)_tmp932.v;
_LL396:({struct Cyc_String_pa_struct _tmp948;_tmp948.tag=0;_tmp948.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp933);{void*_tmp946[1]={& _tmp948};Cyc_fprintf(
Cyc_stderr,({const char*_tmp947="\"%s\"";_tag_arr(_tmp947,sizeof(char),
_get_zero_arr_size(_tmp947,5));}),_tag_arr(_tmp946,sizeof(void*),1));}});goto
_LL38A;_LL397: if(*((void**)_tmp929)!= Cyc_QualId_tok)goto _LL399;_tmp934=((struct
Cyc_QualId_tok_struct*)_tmp929)->f1;_tmp935=*_tmp934;_tmp936=_tmp935.f1;_tmp937=
_tmp935.f2;_LL398: {struct Cyc_List_List*_tmp949=0;{void*_tmp94A=_tmp936;struct
Cyc_List_List*_tmp94B;struct Cyc_List_List*_tmp94C;_LL39C: if(_tmp94A <= (void*)1?1:*((
int*)_tmp94A)!= 0)goto _LL39E;_tmp94B=((struct Cyc_Absyn_Rel_n_struct*)_tmp94A)->f1;
_LL39D: _tmp949=_tmp94B;goto _LL39B;_LL39E: if(_tmp94A <= (void*)1?1:*((int*)_tmp94A)
!= 1)goto _LL3A0;_tmp94C=((struct Cyc_Absyn_Abs_n_struct*)_tmp94A)->f1;_LL39F:
_tmp949=_tmp94C;goto _LL39B;_LL3A0: if((int)_tmp94A != 0)goto _LL39B;_LL3A1: goto
_LL39B;_LL39B:;}for(0;_tmp949 != 0;_tmp949=_tmp949->tl){({struct Cyc_String_pa_struct
_tmp94F;_tmp94F.tag=0;_tmp94F.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp949->hd));{void*_tmp94D[1]={& _tmp94F};Cyc_fprintf(Cyc_stderr,({
const char*_tmp94E="%s::";_tag_arr(_tmp94E,sizeof(char),_get_zero_arr_size(
_tmp94E,5));}),_tag_arr(_tmp94D,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp952;_tmp952.tag=0;_tmp952.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp937);{
void*_tmp950[1]={& _tmp952};Cyc_fprintf(Cyc_stderr,({const char*_tmp951="%s::";
_tag_arr(_tmp951,sizeof(char),_get_zero_arr_size(_tmp951,5));}),_tag_arr(_tmp950,
sizeof(void*),1));}});goto _LL38A;}_LL399:;_LL39A:({void*_tmp953[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp954="?";_tag_arr(_tmp954,sizeof(char),
_get_zero_arr_size(_tmp954,2));}),_tag_arr(_tmp953,sizeof(void*),0));});goto
_LL38A;_LL38A:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmp955=_cycalloc(
sizeof(*_tmp955));_tmp955->v=Cyc_Lexing_from_file(f);_tmp955;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
