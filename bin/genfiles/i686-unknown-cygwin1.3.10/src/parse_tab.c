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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
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
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct
Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct
Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);int Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
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
void*)3?1:*((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct Cyc_Absyn_ArrayType_struct*)
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
Cyc_Lex_register_typedef(_tmpF9);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmpFE=_tmpFB;struct Cyc_Core_Opt*_tmpFF;_LL7F: if(_tmpFE <= (void*)3?1:*((
int*)_tmpFE)!= 0)goto _LL81;_tmpFF=((struct Cyc_Absyn_Evar_struct*)_tmpFE)->f1;
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
_tmp12E;_LL9C: if(_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 10)goto _LL9E;_tmp11F=((
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
void*)3?1:*((int*)_tmp11E)!= 2)goto _LLA0;_tmp124=((struct Cyc_Absyn_TunionType_struct*)
_tmp11E)->f1;_tmp125=(void*)_tmp124.tunion_info;if(*((int*)_tmp125)!= 1)goto
_LLA0;_tmp126=((struct Cyc_Absyn_KnownTunion_struct*)_tmp125)->f1;_LL9F: if(
_tmp10B != 0)Cyc_Parse_err(({const char*_tmp136="bad attributes on tunion";
_tag_arr(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,25));}),loc);return({
struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp138=_cycalloc(sizeof(*_tmp138));
_tmp138[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp139;_tmp139.tag=5;_tmp139.f1=*
_tmp126;_tmp139;});_tmp138;}),loc);_tmp137->tl=0;_tmp137;});_LLA0: if(_tmp11E <= (
void*)3?1:*((int*)_tmp11E)!= 2)goto _LLA2;_tmp127=((struct Cyc_Absyn_TunionType_struct*)
_tmp11E)->f1;_tmp128=(void*)_tmp127.tunion_info;if(*((int*)_tmp128)!= 0)goto
_LLA2;_tmp129=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp128)->f1;_tmp12A=
_tmp129.name;_tmp12B=_tmp129.is_xtunion;_tmp12C=_tmp127.targs;_LLA1: {struct Cyc_List_List*
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12C);struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_tunion_decl(s,_tmp12A,
_tmp13A,0,_tmp12B,loc);if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on tunion";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,25));}),loc);return({
struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=_tmp13B;
_tmp13D->tl=0;_tmp13D;});}_LLA2: if(_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 12)
goto _LLA4;_tmp12D=((struct Cyc_Absyn_EnumType_struct*)_tmp11E)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp13E=({struct Cyc_Absyn_Enumdecl*_tmp144=_cycalloc(sizeof(*
_tmp144));_tmp144->sc=(void*)s;_tmp144->name=_tmp12D;_tmp144->fields=0;_tmp144;});
if(_tmp10B != 0)Cyc_Parse_err(({const char*_tmp13F="bad attributes on enum";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,23));}),loc);return({
struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=({struct Cyc_Absyn_Decl*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp143;_tmp143.tag=6;_tmp143.f1=_tmp13E;_tmp143;});_tmp142;}));_tmp141->loc=loc;
_tmp141;});_tmp140->tl=0;_tmp140;});}_LLA4: if(_tmp11E <= (void*)3?1:*((int*)
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
d;_tmp155->tl=decls;_tmp155;});}if(_tmp10B != 0)({void*_tmp156[0]={};Cyc_Tcutil_warn(
loc,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp10B->hd));{
void*_tmp157[1]={& _tmp159};Cyc_aprintf(({const char*_tmp158="attribute %s in typedef; ignoring";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,34));}),_tag_arr(
_tmp157,sizeof(void*),1));}}),_tag_arr(_tmp156,sizeof(void*),0));});return decls;}}
else{if(_tmp112.f2 != 0)Cyc_Parse_unimp(({const char*_tmp15A="nested type declaration within declarator";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,42));}),loc);{struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp15B=_tmp14E;for(0;_tmp15B != 0;(_tmp15B=_tmp15B->tl,
_tmp111=_tmp111->tl)){struct _tuple7 _tmp15D;struct _tuple1*_tmp15E;struct Cyc_Absyn_Tqual
_tmp15F;void*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;
struct _tuple7*_tmp15C=(struct _tuple7*)_tmp15B->hd;_tmp15D=*_tmp15C;_tmp15E=
_tmp15D.f1;_tmp15F=_tmp15D.f2;_tmp160=_tmp15D.f3;_tmp161=_tmp15D.f4;_tmp162=
_tmp15D.f5;if(_tmp161 != 0)({void*_tmp163[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp164="bad type params, ignoring";_tag_arr(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,26));}),_tag_arr(_tmp163,sizeof(void*),0));});if(
_tmp111 == 0)({void*_tmp165[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp166="unexpected NULL in parse!";
_tag_arr(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,26));}),_tag_arr(
_tmp165,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp167=(struct Cyc_Absyn_Exp*)
_tmp111->hd;struct Cyc_Absyn_Vardecl*_tmp168=Cyc_Absyn_new_vardecl(_tmp15E,
_tmp160,_tmp167);_tmp168->tq=_tmp15F;(void*)(_tmp168->sc=(void*)s);_tmp168->attributes=
_tmp162;{struct Cyc_Absyn_Decl*_tmp169=({struct Cyc_Absyn_Decl*_tmp16B=_cycalloc(
sizeof(*_tmp16B));_tmp16B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp16D;_tmp16D.tag=0;_tmp16D.f1=_tmp168;_tmp16D;});_tmp16C;}));_tmp16B->loc=loc;
_tmp16B;});decls=({struct Cyc_List_List*_tmp16A=_cycalloc(sizeof(*_tmp16A));
_tmp16A->hd=_tmp169;_tmp16A->tl=decls;_tmp16A;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3:
return(void*)2;case 'R': _LLB4: return(void*)3;case 'E': _LLB5: return(void*)4;case 'I':
_LLB6: return(void*)5;default: _LLB7: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp16E[1]={& _tmp170};Cyc_aprintf(({const char*_tmp16F="bad kind: %s";
_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,13));}),_tag_arr(
_tmp16E,sizeof(void*),1));}}),loc);return(void*)2;}char Cyc_AbstractDeclarator_tok[
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
static char _tmp172[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp172,_tmp172,_tmp172 + 15}};struct _tuple13*Cyc_yyget_Int_tok(
void*yy1){struct _tuple13*yyzzz;{void*_tmp173=yy1;struct _tuple13*_tmp174;_LLBA:
if(*((void**)_tmp173)!= Cyc_Int_tok)goto _LLBC;_tmp174=((struct Cyc_Int_tok_struct*)
_tmp173)->f1;_LLBB: yyzzz=_tmp174;goto _LLB9;_LLBC:;_LLBD:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLB9:;}return yyzzz;}static char _tmp176[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp176,_tmp176,_tmp176 + 5}};char Cyc_yyget_Char_tok(
void*yy1){char yyzzz;{void*_tmp177=yy1;char _tmp178;_LLBF: if(*((void**)_tmp177)!= 
Cyc_Char_tok)goto _LLC1;_tmp178=((struct Cyc_Char_tok_struct*)_tmp177)->f1;_LLC0:
yyzzz=_tmp178;goto _LLBE;_LLC1:;_LLC2:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLBE:;}return yyzzz;}static char _tmp17A[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp17B=
yy1;struct _tagged_arr _tmp17C;_LLC4: if(*((void**)_tmp17B)!= Cyc_String_tok)goto
_LLC6;_tmp17C=((struct Cyc_String_tok_struct*)_tmp17B)->f1;_LLC5: yyzzz=_tmp17C;
goto _LLC3;_LLC6:;_LLC7:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLC3:;}return
yyzzz;}static char _tmp17E[38]="$(conref_t<bool>,conref_t<bounds_t>)@";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp17E,_tmp17E,
_tmp17E + 38}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*yyzzz;{void*
_tmp17F=yy1;struct _tuple16*_tmp180;_LLC9: if(*((void**)_tmp17F)!= Cyc_YY1)goto
_LLCB;_tmp180=((struct Cyc_YY1_struct*)_tmp17F)->f1;_LLCA: yyzzz=_tmp180;goto _LLC8;
_LLCB:;_LLCC:(int)_throw((void*)& Cyc_yyfail_YY1);_LLC8:;}return yyzzz;}static char
_tmp182[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp182,_tmp182,_tmp182 + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(
void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp183=yy1;struct Cyc_Absyn_Conref*
_tmp184;_LLCE: if(*((void**)_tmp183)!= Cyc_YY2)goto _LLD0;_tmp184=((struct Cyc_YY2_struct*)
_tmp183)->f1;_LLCF: yyzzz=_tmp184;goto _LLCD;_LLD0:;_LLD1:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLCD:;}return yyzzz;}static char _tmp186[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp186,_tmp186,_tmp186 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp187=yy1;struct
Cyc_Absyn_Exp*_tmp188;_LLD3: if(*((void**)_tmp187)!= Cyc_Exp_tok)goto _LLD5;
_tmp188=((struct Cyc_Exp_tok_struct*)_tmp187)->f1;_LLD4: yyzzz=_tmp188;goto _LLD2;
_LLD5:;_LLD6:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLD2:;}return yyzzz;}static
char _tmp18A[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp18A,_tmp18A,_tmp18A + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18B=yy1;struct Cyc_List_List*
_tmp18C;_LLD8: if(*((void**)_tmp18B)!= Cyc_ExpList_tok)goto _LLDA;_tmp18C=((struct
Cyc_ExpList_tok_struct*)_tmp18B)->f1;_LLD9: yyzzz=_tmp18C;goto _LLD7;_LLDA:;_LLDB:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD7:;}return yyzzz;}static char
_tmp18E[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp18E,_tmp18E,_tmp18E + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp18F=yy1;struct Cyc_List_List*_tmp190;_LLDD: if(*((void**)_tmp18F)
!= Cyc_InitializerList_tok)goto _LLDF;_tmp190=((struct Cyc_InitializerList_tok_struct*)
_tmp18F)->f1;_LLDE: yyzzz=_tmp190;goto _LLDC;_LLDF:;_LLE0:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLDC:;}return yyzzz;}static char _tmp192[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp193=yy1;void*_tmp194;_LLE2: if(*((void**)_tmp193)
!= Cyc_Primop_tok)goto _LLE4;_tmp194=(void*)((struct Cyc_Primop_tok_struct*)
_tmp193)->f1;_LLE3: yyzzz=_tmp194;goto _LLE1;_LLE4:;_LLE5:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLE1:;}return yyzzz;}static char _tmp196[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp197=yy1;struct Cyc_Core_Opt*_tmp198;_LLE7: if(*((void**)_tmp197)!= Cyc_Primopopt_tok)
goto _LLE9;_tmp198=((struct Cyc_Primopopt_tok_struct*)_tmp197)->f1;_LLE8: yyzzz=
_tmp198;goto _LLE6;_LLE9:;_LLEA:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE6:;}return yyzzz;}static char _tmp19A[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp19A,_tmp19A,_tmp19A + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp19B=yy1;
struct _tuple1*_tmp19C;_LLEC: if(*((void**)_tmp19B)!= Cyc_QualId_tok)goto _LLEE;
_tmp19C=((struct Cyc_QualId_tok_struct*)_tmp19B)->f1;_LLED: yyzzz=_tmp19C;goto
_LLEB;_LLEE:;_LLEF:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLEB:;}return
yyzzz;}static char _tmp19E[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp19E,_tmp19E,_tmp19E + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp19F=yy1;struct Cyc_Absyn_Stmt*
_tmp1A0;_LLF1: if(*((void**)_tmp19F)!= Cyc_Stmt_tok)goto _LLF3;_tmp1A0=((struct Cyc_Stmt_tok_struct*)
_tmp19F)->f1;_LLF2: yyzzz=_tmp1A0;goto _LLF0;_LLF3:;_LLF4:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF0:;}return yyzzz;}static char _tmp1A2[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp1A2,_tmp1A2,_tmp1A2 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A3=yy1;struct Cyc_List_List*
_tmp1A4;_LLF6: if(*((void**)_tmp1A3)!= Cyc_SwitchClauseList_tok)goto _LLF8;_tmp1A4=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp1A3)->f1;_LLF7: yyzzz=_tmp1A4;goto
_LLF5;_LLF8:;_LLF9:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF5:;}
return yyzzz;}static char _tmp1A6[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp1A6,_tmp1A6,_tmp1A6 + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1A7=yy1;struct Cyc_List_List*_tmp1A8;_LLFB: if(*((void**)_tmp1A7)
!= Cyc_SwitchCClauseList_tok)goto _LLFD;_tmp1A8=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp1A7)->f1;_LLFC: yyzzz=_tmp1A8;goto _LLFA;_LLFD:;_LLFE:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFA:;}return yyzzz;}static char _tmp1AA[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1AB=yy1;
struct Cyc_Absyn_Pat*_tmp1AC;_LL100: if(*((void**)_tmp1AB)!= Cyc_Pattern_tok)goto
_LL102;_tmp1AC=((struct Cyc_Pattern_tok_struct*)_tmp1AB)->f1;_LL101: yyzzz=_tmp1AC;
goto _LLFF;_LL102:;_LL103:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LLFF:;}
return yyzzz;}static char _tmp1AE[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1AF=yy1;struct Cyc_List_List*_tmp1B0;_LL105: if(*((void**)_tmp1AF)
!= Cyc_PatternList_tok)goto _LL107;_tmp1B0=((struct Cyc_PatternList_tok_struct*)
_tmp1AF)->f1;_LL106: yyzzz=_tmp1B0;goto _LL104;_LL107:;_LL108:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL104:;}return yyzzz;}static char _tmp1B2[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1B3=yy1;struct _tuple11*_tmp1B4;_LL10A: if(*((void**)
_tmp1B3)!= Cyc_FieldPattern_tok)goto _LL10C;_tmp1B4=((struct Cyc_FieldPattern_tok_struct*)
_tmp1B3)->f1;_LL10B: yyzzz=_tmp1B4;goto _LL109;_LL10C:;_LL10D:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL109:;}return yyzzz;}static char _tmp1B6[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL10F: if(*((void**)_tmp1B7)!= Cyc_FieldPatternList_tok)goto _LL111;
_tmp1B8=((struct Cyc_FieldPatternList_tok_struct*)_tmp1B7)->f1;_LL110: yyzzz=
_tmp1B8;goto _LL10E;_LL111:;_LL112:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL10E:;}return yyzzz;}static char _tmp1BA[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1BA,_tmp1BA,_tmp1BA + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1BB=yy1;
struct Cyc_Absyn_Fndecl*_tmp1BC;_LL114: if(*((void**)_tmp1BB)!= Cyc_FnDecl_tok)
goto _LL116;_tmp1BC=((struct Cyc_FnDecl_tok_struct*)_tmp1BB)->f1;_LL115: yyzzz=
_tmp1BC;goto _LL113;_LL116:;_LL117:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL113:;}return yyzzz;}static char _tmp1BE[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1BF=yy1;struct Cyc_List_List*_tmp1C0;_LL119: if(*((void**)_tmp1BF)!= Cyc_DeclList_tok)
goto _LL11B;_tmp1C0=((struct Cyc_DeclList_tok_struct*)_tmp1BF)->f1;_LL11A: yyzzz=
_tmp1C0;goto _LL118;_LL11B:;_LL11C:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL118:;}return yyzzz;}static char _tmp1C2[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1C3=yy1;struct Cyc_Parse_Declaration_spec*_tmp1C4;_LL11E: if(*((
void**)_tmp1C3)!= Cyc_DeclSpec_tok)goto _LL120;_tmp1C4=((struct Cyc_DeclSpec_tok_struct*)
_tmp1C3)->f1;_LL11F: yyzzz=_tmp1C4;goto _LL11D;_LL120:;_LL121:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11D:;}return yyzzz;}static char _tmp1C6[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1C6,_tmp1C6,_tmp1C6 + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1C7=yy1;struct _tuple12*_tmp1C8;_LL123: if(*((void**)
_tmp1C7)!= Cyc_InitDecl_tok)goto _LL125;_tmp1C8=((struct Cyc_InitDecl_tok_struct*)
_tmp1C7)->f1;_LL124: yyzzz=_tmp1C8;goto _LL122;_LL125:;_LL126:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL122:;}return yyzzz;}static char _tmp1CA[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1CA,_tmp1CA,_tmp1CA + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CB=yy1;struct Cyc_List_List*
_tmp1CC;_LL128: if(*((void**)_tmp1CB)!= Cyc_InitDeclList_tok)goto _LL12A;_tmp1CC=((
struct Cyc_InitDeclList_tok_struct*)_tmp1CB)->f1;_LL129: yyzzz=_tmp1CC;goto _LL127;
_LL12A:;_LL12B:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL127:;}return
yyzzz;}static char _tmp1CE[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1CF=yy1;void*
_tmp1D0;_LL12D: if(*((void**)_tmp1CF)!= Cyc_StorageClass_tok)goto _LL12F;_tmp1D0=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1CF)->f1;_LL12E: yyzzz=_tmp1D0;
goto _LL12C;_LL12F:;_LL130:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12C:;}return yyzzz;}static char _tmp1D2[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1D2,_tmp1D2,_tmp1D2 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1D3=yy1;void*
_tmp1D4;_LL132: if(*((void**)_tmp1D3)!= Cyc_TypeSpecifier_tok)goto _LL134;_tmp1D4=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1D3)->f1;_LL133: yyzzz=_tmp1D4;
goto _LL131;_LL134:;_LL135:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL131:;}return yyzzz;}static char _tmp1D6[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1D7=yy1;void*
_tmp1D8;_LL137: if(*((void**)_tmp1D7)!= Cyc_StructOrUnion_tok)goto _LL139;_tmp1D8=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1D7)->f1;_LL138: yyzzz=_tmp1D8;
goto _LL136;_LL139:;_LL13A:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL136:;}return yyzzz;}static char _tmp1DA[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1DB=yy1;struct Cyc_Absyn_Tqual _tmp1DC;_LL13C: if(*((void**)_tmp1DB)!= Cyc_TypeQual_tok)
goto _LL13E;_tmp1DC=((struct Cyc_TypeQual_tok_struct*)_tmp1DB)->f1;_LL13D: yyzzz=
_tmp1DC;goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13B:;}return yyzzz;}static char _tmp1DE[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1DE,_tmp1DE,_tmp1DE + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1DF=yy1;struct Cyc_List_List*
_tmp1E0;_LL141: if(*((void**)_tmp1DF)!= Cyc_AggrFieldDeclList_tok)goto _LL143;
_tmp1E0=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1DF)->f1;_LL142: yyzzz=
_tmp1E0;goto _LL140;_LL143:;_LL144:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL140:;}return yyzzz;}static char _tmp1E2[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1E2,_tmp1E2,_tmp1E2 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL146: if(*((void**)_tmp1E3)!= Cyc_AggrFieldDeclListList_tok)goto _LL148;
_tmp1E4=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1E3)->f1;_LL147: yyzzz=
_tmp1E4;goto _LL145;_LL148:;_LL149:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL145:;}return yyzzz;}static char _tmp1E6[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1E6,_tmp1E6,_tmp1E6 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E7=yy1;struct Cyc_List_List*
_tmp1E8;_LL14B: if(*((void**)_tmp1E7)!= Cyc_TypeModifierList_tok)goto _LL14D;
_tmp1E8=((struct Cyc_TypeModifierList_tok_struct*)_tmp1E7)->f1;_LL14C: yyzzz=
_tmp1E8;goto _LL14A;_LL14D:;_LL14E:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14A:;}return yyzzz;}static char _tmp1EA[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1EB=yy1;struct Cyc_Parse_Declarator*_tmp1EC;_LL150: if(*((void**)
_tmp1EB)!= Cyc_Declarator_tok)goto _LL152;_tmp1EC=((struct Cyc_Declarator_tok_struct*)
_tmp1EB)->f1;_LL151: yyzzz=_tmp1EC;goto _LL14F;_LL152:;_LL153:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL14F:;}return yyzzz;}static char _tmp1EE[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1EE,_tmp1EE,_tmp1EE + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1EF=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1F0;_LL155: if(*((void**)_tmp1EF)!= Cyc_AbstractDeclarator_tok)goto _LL157;
_tmp1F0=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1EF)->f1;_LL156: yyzzz=
_tmp1F0;goto _LL154;_LL157:;_LL158:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL154:;}return yyzzz;}static char _tmp1F2[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1F3=yy1;int _tmp1F4;_LL15A: if(*((void**)_tmp1F3)!= 
Cyc_Bool_tok)goto _LL15C;_tmp1F4=((struct Cyc_Bool_tok_struct*)_tmp1F3)->f1;_LL15B:
yyzzz=_tmp1F4;goto _LL159;_LL15C:;_LL15D:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL159:;}return yyzzz;}static char _tmp1F6[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1F7=yy1;void*_tmp1F8;_LL15F: if(*((void**)_tmp1F7)
!= Cyc_Scope_tok)goto _LL161;_tmp1F8=(void*)((struct Cyc_Scope_tok_struct*)_tmp1F7)->f1;
_LL160: yyzzz=_tmp1F8;goto _LL15E;_LL161:;_LL162:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL15E:;}return yyzzz;}static char _tmp1FA[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp1FB=yy1;struct Cyc_Absyn_Tunionfield*_tmp1FC;_LL164: if(*((void**)
_tmp1FB)!= Cyc_TunionField_tok)goto _LL166;_tmp1FC=((struct Cyc_TunionField_tok_struct*)
_tmp1FB)->f1;_LL165: yyzzz=_tmp1FC;goto _LL163;_LL166:;_LL167:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL163:;}return yyzzz;}static char _tmp1FE[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1FE,_tmp1FE,_tmp1FE + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FF=yy1;struct Cyc_List_List*
_tmp200;_LL169: if(*((void**)_tmp1FF)!= Cyc_TunionFieldList_tok)goto _LL16B;
_tmp200=((struct Cyc_TunionFieldList_tok_struct*)_tmp1FF)->f1;_LL16A: yyzzz=
_tmp200;goto _LL168;_LL16B:;_LL16C:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL168:;}return yyzzz;}static char _tmp202[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp202,_tmp202,_tmp202 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp203=yy1;struct _tuple15*_tmp204;_LL16E: if(*((void**)
_tmp203)!= Cyc_QualSpecList_tok)goto _LL170;_tmp204=((struct Cyc_QualSpecList_tok_struct*)
_tmp203)->f1;_LL16F: yyzzz=_tmp204;goto _LL16D;_LL170:;_LL171:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16D:;}return yyzzz;}static char _tmp206[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp206,_tmp206,_tmp206 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp207=yy1;struct Cyc_List_List*_tmp208;_LL173:
if(*((void**)_tmp207)!= Cyc_IdList_tok)goto _LL175;_tmp208=((struct Cyc_IdList_tok_struct*)
_tmp207)->f1;_LL174: yyzzz=_tmp208;goto _LL172;_LL175:;_LL176:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL172:;}return yyzzz;}static char _tmp20A[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp20A,_tmp20A,_tmp20A + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp20B=yy1;struct _tuple2*_tmp20C;_LL178: if(*((void**)
_tmp20B)!= Cyc_ParamDecl_tok)goto _LL17A;_tmp20C=((struct Cyc_ParamDecl_tok_struct*)
_tmp20B)->f1;_LL179: yyzzz=_tmp20C;goto _LL177;_LL17A:;_LL17B:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL177:;}return yyzzz;}static char _tmp20E[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp20E,_tmp20E,_tmp20E + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20F=yy1;struct Cyc_List_List*
_tmp210;_LL17D: if(*((void**)_tmp20F)!= Cyc_ParamDeclList_tok)goto _LL17F;_tmp210=((
struct Cyc_ParamDeclList_tok_struct*)_tmp20F)->f1;_LL17E: yyzzz=_tmp210;goto _LL17C;
_LL17F:;_LL180:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17C:;}return
yyzzz;}static char _tmp212[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp212,
_tmp212,_tmp212 + 107}};struct _tuple14*Cyc_yyget_YY3(void*yy1){struct _tuple14*
yyzzz;{void*_tmp213=yy1;struct _tuple14*_tmp214;_LL182: if(*((void**)_tmp213)!= 
Cyc_YY3)goto _LL184;_tmp214=((struct Cyc_YY3_struct*)_tmp213)->f1;_LL183: yyzzz=
_tmp214;goto _LL181;_LL184:;_LL185:(int)_throw((void*)& Cyc_yyfail_YY3);_LL181:;}
return yyzzz;}static char _tmp216[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp217=yy1;struct Cyc_List_List*_tmp218;_LL187: if(*((void**)_tmp217)!= Cyc_TypeList_tok)
goto _LL189;_tmp218=((struct Cyc_TypeList_tok_struct*)_tmp217)->f1;_LL188: yyzzz=
_tmp218;goto _LL186;_LL189:;_LL18A:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL186:;}return yyzzz;}static char _tmp21A[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp21A,
_tmp21A,_tmp21A + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp21B=yy1;struct Cyc_List_List*_tmp21C;_LL18C:
if(*((void**)_tmp21B)!= Cyc_DesignatorList_tok)goto _LL18E;_tmp21C=((struct Cyc_DesignatorList_tok_struct*)
_tmp21B)->f1;_LL18D: yyzzz=_tmp21C;goto _LL18B;_LL18E:;_LL18F:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18B:;}return yyzzz;}static char _tmp21E[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp21E,_tmp21E,_tmp21E + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp21F=yy1;void*_tmp220;_LL191: if(*((void**)_tmp21F)!= Cyc_Designator_tok)
goto _LL193;_tmp220=(void*)((struct Cyc_Designator_tok_struct*)_tmp21F)->f1;_LL192:
yyzzz=_tmp220;goto _LL190;_LL193:;_LL194:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL190:;}return yyzzz;}static char _tmp222[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp223=yy1;void*_tmp224;_LL196: if(*((void**)_tmp223)
!= Cyc_Kind_tok)goto _LL198;_tmp224=(void*)((struct Cyc_Kind_tok_struct*)_tmp223)->f1;
_LL197: yyzzz=_tmp224;goto _LL195;_LL198:;_LL199:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL195:;}return yyzzz;}static char _tmp226[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp227=yy1;void*_tmp228;_LL19B: if(*((void**)_tmp227)
!= Cyc_Type_tok)goto _LL19D;_tmp228=(void*)((struct Cyc_Type_tok_struct*)_tmp227)->f1;
_LL19C: yyzzz=_tmp228;goto _LL19A;_LL19D:;_LL19E:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19A:;}return yyzzz;}static char _tmp22A[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp22A,
_tmp22A,_tmp22A + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp22B=yy1;struct Cyc_List_List*_tmp22C;_LL1A0:
if(*((void**)_tmp22B)!= Cyc_AttributeList_tok)goto _LL1A2;_tmp22C=((struct Cyc_AttributeList_tok_struct*)
_tmp22B)->f1;_LL1A1: yyzzz=_tmp22C;goto _LL19F;_LL1A2:;_LL1A3:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL19F:;}return yyzzz;}static char _tmp22E[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp22E,_tmp22E,_tmp22E + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp22F=yy1;void*_tmp230;_LL1A5: if(*((void**)_tmp22F)!= Cyc_Attribute_tok)
goto _LL1A7;_tmp230=(void*)((struct Cyc_Attribute_tok_struct*)_tmp22F)->f1;_LL1A6:
yyzzz=_tmp230;goto _LL1A4;_LL1A7:;_LL1A8:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A4:;}return yyzzz;}static char _tmp232[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp233=yy1;struct Cyc_Absyn_Enumfield*_tmp234;_LL1AA: if(*((void**)
_tmp233)!= Cyc_Enumfield_tok)goto _LL1AC;_tmp234=((struct Cyc_Enumfield_tok_struct*)
_tmp233)->f1;_LL1AB: yyzzz=_tmp234;goto _LL1A9;_LL1AC:;_LL1AD:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1A9:;}return yyzzz;}static char _tmp236[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp236,_tmp236,_tmp236 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp237=yy1;struct Cyc_List_List*
_tmp238;_LL1AF: if(*((void**)_tmp237)!= Cyc_EnumfieldList_tok)goto _LL1B1;_tmp238=((
struct Cyc_EnumfieldList_tok_struct*)_tmp237)->f1;_LL1B0: yyzzz=_tmp238;goto _LL1AE;
_LL1B1:;_LL1B2:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1AE:;}return
yyzzz;}static char _tmp23A[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp23B=yy1;struct Cyc_Core_Opt*_tmp23C;_LL1B4: if(*((void**)_tmp23B)!= Cyc_TypeOpt_tok)
goto _LL1B6;_tmp23C=((struct Cyc_TypeOpt_tok_struct*)_tmp23B)->f1;_LL1B5: yyzzz=
_tmp23C;goto _LL1B3;_LL1B6:;_LL1B7:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B3:;}return yyzzz;}static char _tmp23E[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp23E,
_tmp23E,_tmp23E + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp23F=yy1;struct Cyc_List_List*_tmp240;_LL1B9:
if(*((void**)_tmp23F)!= Cyc_Rgnorder_tok)goto _LL1BB;_tmp240=((struct Cyc_Rgnorder_tok_struct*)
_tmp23F)->f1;_LL1BA: yyzzz=_tmp240;goto _LL1B8;_LL1BB:;_LL1BC:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1B8:;}return yyzzz;}static char _tmp242[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp242,
_tmp242,_tmp242 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY4(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp243=yy1;struct Cyc_Absyn_Conref*_tmp244;_LL1BE: if(*((void**)
_tmp243)!= Cyc_YY4)goto _LL1C0;_tmp244=((struct Cyc_YY4_struct*)_tmp243)->f1;
_LL1BF: yyzzz=_tmp244;goto _LL1BD;_LL1C0:;_LL1C1:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL1BD:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp245;_tmp245.timestamp=0;_tmp245.first_line=0;
_tmp245.first_column=0;_tmp245.last_line=0;_tmp245.last_column=0;_tmp245;});}
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
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp246[
2]="$";static char _tmp247[6]="error";static char _tmp248[12]="$undefined.";static
char _tmp249[5]="AUTO";static char _tmp24A[9]="REGISTER";static char _tmp24B[7]="STATIC";
static char _tmp24C[7]="EXTERN";static char _tmp24D[8]="TYPEDEF";static char _tmp24E[5]="VOID";
static char _tmp24F[5]="CHAR";static char _tmp250[6]="SHORT";static char _tmp251[4]="INT";
static char _tmp252[5]="LONG";static char _tmp253[6]="FLOAT";static char _tmp254[7]="DOUBLE";
static char _tmp255[7]="SIGNED";static char _tmp256[9]="UNSIGNED";static char _tmp257[
6]="CONST";static char _tmp258[9]="VOLATILE";static char _tmp259[9]="RESTRICT";
static char _tmp25A[7]="STRUCT";static char _tmp25B[6]="UNION";static char _tmp25C[5]="CASE";
static char _tmp25D[8]="DEFAULT";static char _tmp25E[7]="INLINE";static char _tmp25F[7]="SIZEOF";
static char _tmp260[9]="OFFSETOF";static char _tmp261[3]="IF";static char _tmp262[5]="ELSE";
static char _tmp263[7]="SWITCH";static char _tmp264[6]="WHILE";static char _tmp265[3]="DO";
static char _tmp266[4]="FOR";static char _tmp267[5]="GOTO";static char _tmp268[9]="CONTINUE";
static char _tmp269[6]="BREAK";static char _tmp26A[7]="RETURN";static char _tmp26B[5]="ENUM";
static char _tmp26C[8]="NULL_kw";static char _tmp26D[4]="LET";static char _tmp26E[6]="THROW";
static char _tmp26F[4]="TRY";static char _tmp270[6]="CATCH";static char _tmp271[4]="NEW";
static char _tmp272[9]="ABSTRACT";static char _tmp273[9]="FALLTHRU";static char
_tmp274[6]="USING";static char _tmp275[10]="NAMESPACE";static char _tmp276[7]="TUNION";
static char _tmp277[8]="XTUNION";static char _tmp278[9]="FORARRAY";static char _tmp279[
5]="FILL";static char _tmp27A[8]="CODEGEN";static char _tmp27B[4]="CUT";static char
_tmp27C[7]="SPLICE";static char _tmp27D[7]="MALLOC";static char _tmp27E[8]="RMALLOC";
static char _tmp27F[7]="CALLOC";static char _tmp280[8]="RCALLOC";static char _tmp281[9]="REGION_T";
static char _tmp282[9]="SIZEOF_T";static char _tmp283[6]="TAG_T";static char _tmp284[7]="REGION";
static char _tmp285[5]="RNEW";static char _tmp286[8]="REGIONS";static char _tmp287[13]="RESET_REGION";
static char _tmp288[4]="GEN";static char _tmp289[14]="NOZEROTERM_kw";static char
_tmp28A[12]="ZEROTERM_kw";static char _tmp28B[7]="PTR_OP";static char _tmp28C[7]="INC_OP";
static char _tmp28D[7]="DEC_OP";static char _tmp28E[8]="LEFT_OP";static char _tmp28F[9]="RIGHT_OP";
static char _tmp290[6]="LE_OP";static char _tmp291[6]="GE_OP";static char _tmp292[6]="EQ_OP";
static char _tmp293[6]="NE_OP";static char _tmp294[7]="AND_OP";static char _tmp295[6]="OR_OP";
static char _tmp296[11]="MUL_ASSIGN";static char _tmp297[11]="DIV_ASSIGN";static char
_tmp298[11]="MOD_ASSIGN";static char _tmp299[11]="ADD_ASSIGN";static char _tmp29A[11]="SUB_ASSIGN";
static char _tmp29B[12]="LEFT_ASSIGN";static char _tmp29C[13]="RIGHT_ASSIGN";static
char _tmp29D[11]="AND_ASSIGN";static char _tmp29E[11]="XOR_ASSIGN";static char
_tmp29F[10]="OR_ASSIGN";static char _tmp2A0[9]="ELLIPSIS";static char _tmp2A1[11]="LEFT_RIGHT";
static char _tmp2A2[12]="COLON_COLON";static char _tmp2A3[11]="IDENTIFIER";static
char _tmp2A4[17]="INTEGER_CONSTANT";static char _tmp2A5[7]="STRING";static char
_tmp2A6[19]="CHARACTER_CONSTANT";static char _tmp2A7[18]="FLOATING_CONSTANT";
static char _tmp2A8[9]="TYPE_VAR";static char _tmp2A9[13]="TYPEDEF_NAME";static char
_tmp2AA[16]="QUAL_IDENTIFIER";static char _tmp2AB[18]="QUAL_TYPEDEF_NAME";static
char _tmp2AC[13]="TYPE_INTEGER";static char _tmp2AD[10]="ATTRIBUTE";static char
_tmp2AE[4]="';'";static char _tmp2AF[4]="'{'";static char _tmp2B0[4]="'}'";static
char _tmp2B1[4]="'='";static char _tmp2B2[4]="'('";static char _tmp2B3[4]="')'";
static char _tmp2B4[4]="','";static char _tmp2B5[4]="'_'";static char _tmp2B6[4]="'$'";
static char _tmp2B7[4]="'<'";static char _tmp2B8[4]="':'";static char _tmp2B9[4]="'.'";
static char _tmp2BA[4]="'['";static char _tmp2BB[4]="']'";static char _tmp2BC[4]="'*'";
static char _tmp2BD[4]="'@'";static char _tmp2BE[4]="'?'";static char _tmp2BF[4]="'>'";
static char _tmp2C0[4]="'+'";static char _tmp2C1[4]="'-'";static char _tmp2C2[4]="'&'";
static char _tmp2C3[4]="'|'";static char _tmp2C4[4]="'^'";static char _tmp2C5[4]="'/'";
static char _tmp2C6[4]="'%'";static char _tmp2C7[4]="'~'";static char _tmp2C8[4]="'!'";
static char _tmp2C9[5]="prog";static char _tmp2CA[17]="translation_unit";static char
_tmp2CB[21]="external_declaration";static char _tmp2CC[20]="function_definition";
static char _tmp2CD[21]="function_definition2";static char _tmp2CE[13]="using_action";
static char _tmp2CF[15]="unusing_action";static char _tmp2D0[17]="namespace_action";
static char _tmp2D1[19]="unnamespace_action";static char _tmp2D2[12]="declaration";
static char _tmp2D3[17]="declaration_list";static char _tmp2D4[23]="declaration_specifiers";
static char _tmp2D5[24]="storage_class_specifier";static char _tmp2D6[15]="attributes_opt";
static char _tmp2D7[11]="attributes";static char _tmp2D8[15]="attribute_list";static
char _tmp2D9[10]="attribute";static char _tmp2DA[15]="type_specifier";static char
_tmp2DB[5]="kind";static char _tmp2DC[15]="type_qualifier";static char _tmp2DD[15]="enum_specifier";
static char _tmp2DE[11]="enum_field";static char _tmp2DF[22]="enum_declaration_list";
static char _tmp2E0[26]="struct_or_union_specifier";static char _tmp2E1[16]="type_params_opt";
static char _tmp2E2[16]="struct_or_union";static char _tmp2E3[24]="struct_declaration_list";
static char _tmp2E4[25]="struct_declaration_list0";static char _tmp2E5[21]="init_declarator_list";
static char _tmp2E6[22]="init_declarator_list0";static char _tmp2E7[16]="init_declarator";
static char _tmp2E8[19]="struct_declaration";static char _tmp2E9[25]="specifier_qualifier_list";
static char _tmp2EA[23]="struct_declarator_list";static char _tmp2EB[24]="struct_declarator_list0";
static char _tmp2EC[18]="struct_declarator";static char _tmp2ED[17]="tunion_specifier";
static char _tmp2EE[18]="tunion_or_xtunion";static char _tmp2EF[17]="tunionfield_list";
static char _tmp2F0[18]="tunionfield_scope";static char _tmp2F1[12]="tunionfield";
static char _tmp2F2[11]="declarator";static char _tmp2F3[18]="direct_declarator";
static char _tmp2F4[8]="pointer";static char _tmp2F5[12]="one_pointer";static char
_tmp2F6[23]="pointer_null_and_bound";static char _tmp2F7[14]="pointer_bound";
static char _tmp2F8[18]="zeroterm_qual_opt";static char _tmp2F9[8]="rgn_opt";static
char _tmp2FA[11]="tqual_list";static char _tmp2FB[20]="parameter_type_list";static
char _tmp2FC[9]="type_var";static char _tmp2FD[16]="optional_effect";static char
_tmp2FE[19]="optional_rgn_order";static char _tmp2FF[10]="rgn_order";static char
_tmp300[16]="optional_inject";static char _tmp301[11]="effect_set";static char
_tmp302[14]="atomic_effect";static char _tmp303[11]="region_set";static char _tmp304[
15]="parameter_list";static char _tmp305[22]="parameter_declaration";static char
_tmp306[16]="identifier_list";static char _tmp307[17]="identifier_list0";static
char _tmp308[12]="initializer";static char _tmp309[18]="array_initializer";static
char _tmp30A[17]="initializer_list";static char _tmp30B[12]="designation";static
char _tmp30C[16]="designator_list";static char _tmp30D[11]="designator";static char
_tmp30E[10]="type_name";static char _tmp30F[14]="any_type_name";static char _tmp310[
15]="type_name_list";static char _tmp311[20]="abstract_declarator";static char
_tmp312[27]="direct_abstract_declarator";static char _tmp313[10]="statement";
static char _tmp314[18]="labeled_statement";static char _tmp315[21]="expression_statement";
static char _tmp316[19]="compound_statement";static char _tmp317[16]="block_item_list";
static char _tmp318[20]="selection_statement";static char _tmp319[15]="switch_clauses";
static char _tmp31A[16]="switchC_clauses";static char _tmp31B[20]="iteration_statement";
static char _tmp31C[15]="jump_statement";static char _tmp31D[8]="pattern";static char
_tmp31E[19]="tuple_pattern_list";static char _tmp31F[14]="field_pattern";static
char _tmp320[19]="field_pattern_list";static char _tmp321[11]="expression";static
char _tmp322[22]="assignment_expression";static char _tmp323[20]="assignment_operator";
static char _tmp324[23]="conditional_expression";static char _tmp325[20]="constant_expression";
static char _tmp326[22]="logical_or_expression";static char _tmp327[23]="logical_and_expression";
static char _tmp328[24]="inclusive_or_expression";static char _tmp329[24]="exclusive_or_expression";
static char _tmp32A[15]="and_expression";static char _tmp32B[20]="equality_expression";
static char _tmp32C[22]="relational_expression";static char _tmp32D[17]="shift_expression";
static char _tmp32E[20]="additive_expression";static char _tmp32F[26]="multiplicative_expression";
static char _tmp330[16]="cast_expression";static char _tmp331[17]="unary_expression";
static char _tmp332[15]="unary_operator";static char _tmp333[19]="postfix_expression";
static char _tmp334[19]="primary_expression";static char _tmp335[25]="argument_expression_list";
static char _tmp336[26]="argument_expression_list0";static char _tmp337[9]="constant";
static char _tmp338[20]="qual_opt_identifier";static char _tmp339[17]="qual_opt_typedef";
static char _tmp33A[18]="struct_union_name";static char _tmp33B[11]="field_name";
static char _tmp33C[12]="right_angle";static struct _tagged_arr Cyc_yytname[247]={{
_tmp246,_tmp246,_tmp246 + 2},{_tmp247,_tmp247,_tmp247 + 6},{_tmp248,_tmp248,
_tmp248 + 12},{_tmp249,_tmp249,_tmp249 + 5},{_tmp24A,_tmp24A,_tmp24A + 9},{_tmp24B,
_tmp24B,_tmp24B + 7},{_tmp24C,_tmp24C,_tmp24C + 7},{_tmp24D,_tmp24D,_tmp24D + 8},{
_tmp24E,_tmp24E,_tmp24E + 5},{_tmp24F,_tmp24F,_tmp24F + 5},{_tmp250,_tmp250,
_tmp250 + 6},{_tmp251,_tmp251,_tmp251 + 4},{_tmp252,_tmp252,_tmp252 + 5},{_tmp253,
_tmp253,_tmp253 + 6},{_tmp254,_tmp254,_tmp254 + 7},{_tmp255,_tmp255,_tmp255 + 7},{
_tmp256,_tmp256,_tmp256 + 9},{_tmp257,_tmp257,_tmp257 + 6},{_tmp258,_tmp258,
_tmp258 + 9},{_tmp259,_tmp259,_tmp259 + 9},{_tmp25A,_tmp25A,_tmp25A + 7},{_tmp25B,
_tmp25B,_tmp25B + 6},{_tmp25C,_tmp25C,_tmp25C + 5},{_tmp25D,_tmp25D,_tmp25D + 8},{
_tmp25E,_tmp25E,_tmp25E + 7},{_tmp25F,_tmp25F,_tmp25F + 7},{_tmp260,_tmp260,
_tmp260 + 9},{_tmp261,_tmp261,_tmp261 + 3},{_tmp262,_tmp262,_tmp262 + 5},{_tmp263,
_tmp263,_tmp263 + 7},{_tmp264,_tmp264,_tmp264 + 6},{_tmp265,_tmp265,_tmp265 + 3},{
_tmp266,_tmp266,_tmp266 + 4},{_tmp267,_tmp267,_tmp267 + 5},{_tmp268,_tmp268,
_tmp268 + 9},{_tmp269,_tmp269,_tmp269 + 6},{_tmp26A,_tmp26A,_tmp26A + 7},{_tmp26B,
_tmp26B,_tmp26B + 5},{_tmp26C,_tmp26C,_tmp26C + 8},{_tmp26D,_tmp26D,_tmp26D + 4},{
_tmp26E,_tmp26E,_tmp26E + 6},{_tmp26F,_tmp26F,_tmp26F + 4},{_tmp270,_tmp270,
_tmp270 + 6},{_tmp271,_tmp271,_tmp271 + 4},{_tmp272,_tmp272,_tmp272 + 9},{_tmp273,
_tmp273,_tmp273 + 9},{_tmp274,_tmp274,_tmp274 + 6},{_tmp275,_tmp275,_tmp275 + 10},{
_tmp276,_tmp276,_tmp276 + 7},{_tmp277,_tmp277,_tmp277 + 8},{_tmp278,_tmp278,
_tmp278 + 9},{_tmp279,_tmp279,_tmp279 + 5},{_tmp27A,_tmp27A,_tmp27A + 8},{_tmp27B,
_tmp27B,_tmp27B + 4},{_tmp27C,_tmp27C,_tmp27C + 7},{_tmp27D,_tmp27D,_tmp27D + 7},{
_tmp27E,_tmp27E,_tmp27E + 8},{_tmp27F,_tmp27F,_tmp27F + 7},{_tmp280,_tmp280,
_tmp280 + 8},{_tmp281,_tmp281,_tmp281 + 9},{_tmp282,_tmp282,_tmp282 + 9},{_tmp283,
_tmp283,_tmp283 + 6},{_tmp284,_tmp284,_tmp284 + 7},{_tmp285,_tmp285,_tmp285 + 5},{
_tmp286,_tmp286,_tmp286 + 8},{_tmp287,_tmp287,_tmp287 + 13},{_tmp288,_tmp288,
_tmp288 + 4},{_tmp289,_tmp289,_tmp289 + 14},{_tmp28A,_tmp28A,_tmp28A + 12},{_tmp28B,
_tmp28B,_tmp28B + 7},{_tmp28C,_tmp28C,_tmp28C + 7},{_tmp28D,_tmp28D,_tmp28D + 7},{
_tmp28E,_tmp28E,_tmp28E + 8},{_tmp28F,_tmp28F,_tmp28F + 9},{_tmp290,_tmp290,
_tmp290 + 6},{_tmp291,_tmp291,_tmp291 + 6},{_tmp292,_tmp292,_tmp292 + 6},{_tmp293,
_tmp293,_tmp293 + 6},{_tmp294,_tmp294,_tmp294 + 7},{_tmp295,_tmp295,_tmp295 + 6},{
_tmp296,_tmp296,_tmp296 + 11},{_tmp297,_tmp297,_tmp297 + 11},{_tmp298,_tmp298,
_tmp298 + 11},{_tmp299,_tmp299,_tmp299 + 11},{_tmp29A,_tmp29A,_tmp29A + 11},{
_tmp29B,_tmp29B,_tmp29B + 12},{_tmp29C,_tmp29C,_tmp29C + 13},{_tmp29D,_tmp29D,
_tmp29D + 11},{_tmp29E,_tmp29E,_tmp29E + 11},{_tmp29F,_tmp29F,_tmp29F + 10},{
_tmp2A0,_tmp2A0,_tmp2A0 + 9},{_tmp2A1,_tmp2A1,_tmp2A1 + 11},{_tmp2A2,_tmp2A2,
_tmp2A2 + 12},{_tmp2A3,_tmp2A3,_tmp2A3 + 11},{_tmp2A4,_tmp2A4,_tmp2A4 + 17},{
_tmp2A5,_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 19},{_tmp2A7,_tmp2A7,
_tmp2A7 + 18},{_tmp2A8,_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9,_tmp2A9 + 13},{_tmp2AA,
_tmp2AA,_tmp2AA + 16},{_tmp2AB,_tmp2AB,_tmp2AB + 18},{_tmp2AC,_tmp2AC,_tmp2AC + 13},{
_tmp2AD,_tmp2AD,_tmp2AD + 10},{_tmp2AE,_tmp2AE,_tmp2AE + 4},{_tmp2AF,_tmp2AF,
_tmp2AF + 4},{_tmp2B0,_tmp2B0,_tmp2B0 + 4},{_tmp2B1,_tmp2B1,_tmp2B1 + 4},{_tmp2B2,
_tmp2B2,_tmp2B2 + 4},{_tmp2B3,_tmp2B3,_tmp2B3 + 4},{_tmp2B4,_tmp2B4,_tmp2B4 + 4},{
_tmp2B5,_tmp2B5,_tmp2B5 + 4},{_tmp2B6,_tmp2B6,_tmp2B6 + 4},{_tmp2B7,_tmp2B7,
_tmp2B7 + 4},{_tmp2B8,_tmp2B8,_tmp2B8 + 4},{_tmp2B9,_tmp2B9,_tmp2B9 + 4},{_tmp2BA,
_tmp2BA,_tmp2BA + 4},{_tmp2BB,_tmp2BB,_tmp2BB + 4},{_tmp2BC,_tmp2BC,_tmp2BC + 4},{
_tmp2BD,_tmp2BD,_tmp2BD + 4},{_tmp2BE,_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,
_tmp2BF + 4},{_tmp2C0,_tmp2C0,_tmp2C0 + 4},{_tmp2C1,_tmp2C1,_tmp2C1 + 4},{_tmp2C2,
_tmp2C2,_tmp2C2 + 4},{_tmp2C3,_tmp2C3,_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 4},{
_tmp2C5,_tmp2C5,_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 4},{_tmp2C7,_tmp2C7,
_tmp2C7 + 4},{_tmp2C8,_tmp2C8,_tmp2C8 + 4},{_tmp2C9,_tmp2C9,_tmp2C9 + 5},{_tmp2CA,
_tmp2CA,_tmp2CA + 17},{_tmp2CB,_tmp2CB,_tmp2CB + 21},{_tmp2CC,_tmp2CC,_tmp2CC + 20},{
_tmp2CD,_tmp2CD,_tmp2CD + 21},{_tmp2CE,_tmp2CE,_tmp2CE + 13},{_tmp2CF,_tmp2CF,
_tmp2CF + 15},{_tmp2D0,_tmp2D0,_tmp2D0 + 17},{_tmp2D1,_tmp2D1,_tmp2D1 + 19},{
_tmp2D2,_tmp2D2,_tmp2D2 + 12},{_tmp2D3,_tmp2D3,_tmp2D3 + 17},{_tmp2D4,_tmp2D4,
_tmp2D4 + 23},{_tmp2D5,_tmp2D5,_tmp2D5 + 24},{_tmp2D6,_tmp2D6,_tmp2D6 + 15},{
_tmp2D7,_tmp2D7,_tmp2D7 + 11},{_tmp2D8,_tmp2D8,_tmp2D8 + 15},{_tmp2D9,_tmp2D9,
_tmp2D9 + 10},{_tmp2DA,_tmp2DA,_tmp2DA + 15},{_tmp2DB,_tmp2DB,_tmp2DB + 5},{_tmp2DC,
_tmp2DC,_tmp2DC + 15},{_tmp2DD,_tmp2DD,_tmp2DD + 15},{_tmp2DE,_tmp2DE,_tmp2DE + 11},{
_tmp2DF,_tmp2DF,_tmp2DF + 22},{_tmp2E0,_tmp2E0,_tmp2E0 + 26},{_tmp2E1,_tmp2E1,
_tmp2E1 + 16},{_tmp2E2,_tmp2E2,_tmp2E2 + 16},{_tmp2E3,_tmp2E3,_tmp2E3 + 24},{
_tmp2E4,_tmp2E4,_tmp2E4 + 25},{_tmp2E5,_tmp2E5,_tmp2E5 + 21},{_tmp2E6,_tmp2E6,
_tmp2E6 + 22},{_tmp2E7,_tmp2E7,_tmp2E7 + 16},{_tmp2E8,_tmp2E8,_tmp2E8 + 19},{
_tmp2E9,_tmp2E9,_tmp2E9 + 25},{_tmp2EA,_tmp2EA,_tmp2EA + 23},{_tmp2EB,_tmp2EB,
_tmp2EB + 24},{_tmp2EC,_tmp2EC,_tmp2EC + 18},{_tmp2ED,_tmp2ED,_tmp2ED + 17},{
_tmp2EE,_tmp2EE,_tmp2EE + 18},{_tmp2EF,_tmp2EF,_tmp2EF + 17},{_tmp2F0,_tmp2F0,
_tmp2F0 + 18},{_tmp2F1,_tmp2F1,_tmp2F1 + 12},{_tmp2F2,_tmp2F2,_tmp2F2 + 11},{
_tmp2F3,_tmp2F3,_tmp2F3 + 18},{_tmp2F4,_tmp2F4,_tmp2F4 + 8},{_tmp2F5,_tmp2F5,
_tmp2F5 + 12},{_tmp2F6,_tmp2F6,_tmp2F6 + 23},{_tmp2F7,_tmp2F7,_tmp2F7 + 14},{
_tmp2F8,_tmp2F8,_tmp2F8 + 18},{_tmp2F9,_tmp2F9,_tmp2F9 + 8},{_tmp2FA,_tmp2FA,
_tmp2FA + 11},{_tmp2FB,_tmp2FB,_tmp2FB + 20},{_tmp2FC,_tmp2FC,_tmp2FC + 9},{_tmp2FD,
_tmp2FD,_tmp2FD + 16},{_tmp2FE,_tmp2FE,_tmp2FE + 19},{_tmp2FF,_tmp2FF,_tmp2FF + 10},{
_tmp300,_tmp300,_tmp300 + 16},{_tmp301,_tmp301,_tmp301 + 11},{_tmp302,_tmp302,
_tmp302 + 14},{_tmp303,_tmp303,_tmp303 + 11},{_tmp304,_tmp304,_tmp304 + 15},{
_tmp305,_tmp305,_tmp305 + 22},{_tmp306,_tmp306,_tmp306 + 16},{_tmp307,_tmp307,
_tmp307 + 17},{_tmp308,_tmp308,_tmp308 + 12},{_tmp309,_tmp309,_tmp309 + 18},{
_tmp30A,_tmp30A,_tmp30A + 17},{_tmp30B,_tmp30B,_tmp30B + 12},{_tmp30C,_tmp30C,
_tmp30C + 16},{_tmp30D,_tmp30D,_tmp30D + 11},{_tmp30E,_tmp30E,_tmp30E + 10},{
_tmp30F,_tmp30F,_tmp30F + 14},{_tmp310,_tmp310,_tmp310 + 15},{_tmp311,_tmp311,
_tmp311 + 20},{_tmp312,_tmp312,_tmp312 + 27},{_tmp313,_tmp313,_tmp313 + 10},{
_tmp314,_tmp314,_tmp314 + 18},{_tmp315,_tmp315,_tmp315 + 21},{_tmp316,_tmp316,
_tmp316 + 19},{_tmp317,_tmp317,_tmp317 + 16},{_tmp318,_tmp318,_tmp318 + 20},{
_tmp319,_tmp319,_tmp319 + 15},{_tmp31A,_tmp31A,_tmp31A + 16},{_tmp31B,_tmp31B,
_tmp31B + 20},{_tmp31C,_tmp31C,_tmp31C + 15},{_tmp31D,_tmp31D,_tmp31D + 8},{_tmp31E,
_tmp31E,_tmp31E + 19},{_tmp31F,_tmp31F,_tmp31F + 14},{_tmp320,_tmp320,_tmp320 + 19},{
_tmp321,_tmp321,_tmp321 + 11},{_tmp322,_tmp322,_tmp322 + 22},{_tmp323,_tmp323,
_tmp323 + 20},{_tmp324,_tmp324,_tmp324 + 23},{_tmp325,_tmp325,_tmp325 + 20},{
_tmp326,_tmp326,_tmp326 + 22},{_tmp327,_tmp327,_tmp327 + 23},{_tmp328,_tmp328,
_tmp328 + 24},{_tmp329,_tmp329,_tmp329 + 24},{_tmp32A,_tmp32A,_tmp32A + 15},{
_tmp32B,_tmp32B,_tmp32B + 20},{_tmp32C,_tmp32C,_tmp32C + 22},{_tmp32D,_tmp32D,
_tmp32D + 17},{_tmp32E,_tmp32E,_tmp32E + 20},{_tmp32F,_tmp32F,_tmp32F + 26},{
_tmp330,_tmp330,_tmp330 + 16},{_tmp331,_tmp331,_tmp331 + 17},{_tmp332,_tmp332,
_tmp332 + 15},{_tmp333,_tmp333,_tmp333 + 19},{_tmp334,_tmp334,_tmp334 + 19},{
_tmp335,_tmp335,_tmp335 + 25},{_tmp336,_tmp336,_tmp336 + 26},{_tmp337,_tmp337,
_tmp337 + 9},{_tmp338,_tmp338,_tmp338 + 20},{_tmp339,_tmp339,_tmp339 + 17},{_tmp33A,
_tmp33A,_tmp33A + 18},{_tmp33B,_tmp33B,_tmp33B + 11},{_tmp33C,_tmp33C,_tmp33C + 12}};
static short Cyc_yyr1[435]={0,131,132,132,132,132,132,132,132,133,133,133,134,134,
134,134,135,135,136,137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,
142,142,142,142,143,143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,
147,147,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
148,148,148,148,149,150,150,150,151,151,151,152,152,153,153,153,154,154,154,155,
155,156,156,157,157,158,158,159,160,160,161,161,162,163,163,163,163,163,163,164,
165,165,166,166,166,167,167,167,168,168,169,169,169,169,170,170,170,171,171,172,
172,173,173,173,173,173,173,173,173,173,173,173,174,174,175,176,176,176,177,177,
177,178,178,178,179,179,179,180,180,181,181,181,181,182,182,183,183,184,184,185,
185,186,186,187,187,188,188,188,188,189,189,190,190,191,191,191,192,193,193,194,
194,195,195,195,195,196,196,196,196,197,198,198,199,199,200,200,201,201,201,201,
201,202,202,203,203,203,204,204,204,204,204,204,204,204,204,204,204,205,205,205,
205,205,205,205,205,205,205,205,205,205,206,207,207,207,208,208,208,209,209,209,
209,209,209,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,212,212,
212,212,212,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
213,214,214,214,214,214,214,214,214,215,215,215,215,215,215,215,215,215,215,215,
215,215,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,221,221,221,
221,221,221,221,221,222,222,222,222,222,222,222,223,224,224,225,225,226,226,227,
227,228,228,229,229,229,230,230,230,230,230,231,231,231,232,232,232,233,233,233,
233,234,234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,
236,236,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,
238,238,238,238,239,240,240,241,241,241,241,242,242,243,243,244,244,245,245,246,
246};static short Cyc_yyr2[435]={0,1,2,3,5,3,5,6,0,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,
3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,1,3,1,1,1,1,1,1,
1,1,1,1,1,1,2,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,
3,3,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,4,5,4,5,4,4,2,
2,1,2,5,2,2,1,0,3,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,
1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,
4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,5,2,2,3,1,2,2,2,3,3,1,2,1,2,1,2,5,7,2,7,2,8,6,2,
0,3,4,5,6,7,4,0,3,4,5,4,5,2,7,2,6,7,7,8,7,8,8,9,6,7,7,8,2,9,3,2,2,2,3,2,4,5,1,3,
1,2,1,1,1,1,4,4,5,2,2,4,4,1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,
5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,
2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,3,2,5,4,5,5,1,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[883]={0,11,38,39,40,41,43,56,59,60,
61,62,63,64,65,66,79,80,81,95,96,34,0,0,44,0,0,122,123,0,0,76,425,167,427,426,
428,77,0,0,57,0,152,152,151,1,0,9,0,0,10,0,28,36,30,32,67,68,0,69,158,0,133,0,
146,155,70,135,93,42,41,35,0,83,309,425,305,308,307,0,303,0,0,0,0,0,190,0,310,18,
20,0,0,0,0,0,0,0,0,0,0,149,150,2,0,0,0,0,22,0,101,102,104,29,37,31,33,97,429,430,
93,160,0,159,0,26,0,0,12,169,0,0,144,134,147,157,156,158,145,0,71,0,42,87,0,85,0,
0,425,0,0,315,306,314,25,0,0,93,0,0,0,111,107,109,208,210,0,0,0,431,432,168,78,0,
0,136,58,188,0,185,0,0,424,0,0,0,0,0,0,0,0,0,93,0,0,421,410,422,423,0,0,0,0,395,
0,393,394,0,0,326,339,347,349,351,353,355,357,360,365,368,371,375,0,377,396,409,
408,3,0,5,0,23,0,0,0,13,0,98,99,0,92,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,425,
245,248,0,0,0,0,231,232,233,0,234,235,236,0,324,27,14,104,175,191,0,0,171,169,0,
215,0,0,155,0,45,0,0,88,84,0,0,0,0,304,318,0,192,0,0,0,0,211,183,0,112,108,110,
169,0,217,209,218,434,433,0,73,74,75,51,50,0,48,137,169,187,217,189,72,0,0,385,0,
341,375,0,342,343,0,0,0,0,0,0,0,0,0,378,379,0,0,0,0,0,381,382,380,154,153,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,329,330,331,332,333,334,335,336,337,338,328,0,
383,0,402,403,0,0,0,413,0,93,19,0,21,0,103,105,194,193,15,90,100,0,0,113,114,116,
93,0,120,247,250,259,0,261,0,0,278,0,280,0,293,0,0,296,297,298,0,264,0,300,0,0,0,
242,243,0,0,0,0,0,256,252,104,254,249,246,0,176,0,0,0,182,170,177,140,0,0,0,171,
142,0,143,0,138,155,161,46,94,0,89,346,86,82,316,317,311,0,24,0,0,0,0,204,320,
322,0,312,0,0,212,0,171,0,155,0,219,169,0,0,230,214,0,0,0,111,186,0,0,0,195,199,
0,0,0,0,0,0,0,0,0,0,0,0,412,0,0,411,419,0,418,348,0,350,352,354,356,358,359,363,
364,361,362,366,367,369,370,372,373,374,327,401,398,0,400,0,0,0,4,6,117,106,0,0,
171,93,128,0,0,0,0,0,0,0,0,295,299,0,0,0,0,238,0,0,0,244,0,16,325,169,0,179,0,0,
172,0,141,175,163,216,139,161,148,7,319,207,0,321,203,205,313,0,213,184,226,0,
220,221,155,0,171,0,155,0,0,0,0,47,49,384,0,0,196,0,200,406,407,0,389,0,0,0,0,0,
0,0,0,376,415,0,0,399,397,0,0,115,118,97,121,130,129,0,0,124,0,0,0,0,0,0,0,0,0,0,
265,301,0,0,0,0,0,17,171,0,180,178,0,171,0,162,206,323,225,223,228,0,229,222,155,
54,0,52,53,0,0,0,197,201,0,0,0,0,344,345,388,417,0,420,340,414,416,0,0,119,131,
128,128,257,0,265,277,0,0,0,0,0,0,0,0,0,0,0,0,302,0,0,0,0,241,165,181,173,164,
169,227,224,0,387,386,0,202,390,0,0,404,0,91,0,127,126,0,272,0,0,281,0,0,0,289,0,
0,0,0,0,0,0,0,0,263,0,237,240,0,0,171,0,0,0,0,405,0,258,0,0,0,260,279,282,283,0,
291,290,0,285,0,0,0,0,0,0,266,0,0,174,166,0,0,0,0,132,0,0,0,262,284,292,286,287,
0,271,0,265,267,0,0,55,198,391,0,0,0,273,288,0,268,294,239,0,276,272,274,265,269,
392,275,270,0,0,0};static short Cyc_yydefgoto[116]={880,45,46,47,262,48,406,49,408,
263,234,264,52,480,53,326,327,54,171,55,56,143,144,57,140,58,236,237,109,110,111,
238,164,417,418,419,59,60,675,676,677,61,62,63,64,65,101,137,122,616,505,66,506,
471,608,463,467,468,309,283,179,85,86,525,411,526,527,496,497,165,285,286,507,
317,265,266,267,268,269,270,756,818,271,272,300,301,499,500,273,274,394,209,486,
210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,543,544,225,226,68,
120,172,321};static short Cyc_yypact[883]={3055,- -32768,- -32768,- -32768,- -32768,
113,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,3902,217,728,- -32768,85,- 53,-
-32768,- -32768,- 29,126,139,- -32768,51,- -32768,- -32768,- -32768,- -32768,111,232,
163,177,165,165,- -32768,- -32768,2934,- -32768,12,284,- -32768,679,3902,3902,3902,
3902,- -32768,- -32768,164,- -32768,16,3773,253,- 35,384,397,- -32768,85,223,252,220,
- -32768,85,260,- -32768,162,- -32768,- -32768,- -32768,807,- -32768,246,308,312,807,
321,322,330,236,- -32768,- -32768,5998,5998,5998,101,338,579,339,101,6176,2680,-
-32768,- -32768,- -32768,2934,3175,2934,3175,- -32768,355,346,- -32768,3704,- -32768,
- -32768,- -32768,- -32768,6176,- -32768,- -32768,223,- -32768,85,- -32768,1887,- -32768,
3773,679,- -32768,3958,5998,4335,- -32768,253,- -32768,- -32768,- -32768,16,- -32768,
5998,- -32768,3175,- -32768,373,381,382,85,17,378,390,807,- -32768,- -32768,- -32768,
- -32768,425,5551,223,807,415,33,6176,6176,6176,208,- -32768,- 46,- 46,- 46,- -32768,-
-32768,- -32768,- -32768,18,421,- -32768,- -32768,124,385,- -32768,5779,436,- -32768,
5551,5627,438,447,450,454,456,461,468,223,5855,5855,- -32768,- -32768,- -32768,-
-32768,2141,474,5931,5931,- -32768,5931,- -32768,- -32768,- 37,434,- -32768,- 64,511,
427,466,470,439,- 32,448,412,103,- -32768,910,5931,222,- 52,- -32768,482,- -32768,493,
- -32768,500,- -32768,579,4411,3773,- -32768,503,6176,- -32768,809,506,186,258,13,
102,30,1090,47,522,515,517,4487,2247,- 49,518,2353,2353,- 42,520,516,- -32768,-
-32768,1237,1367,679,1497,- -32768,- -32768,- -32768,550,- -32768,- -32768,- -32768,-
66,- -32768,- -32768,- -32768,560,566,- -32768,31,564,576,167,569,562,- 24,584,397,
577,599,- 24,600,85,- -32768,5551,602,589,591,- -32768,- -32768,433,- -32768,210,607,
441,5998,- -32768,604,610,- -32768,- -32768,- -32768,3473,4563,58,- -32768,265,-
-32768,- -32768,31,- -32768,- -32768,- -32768,- -32768,618,615,622,- -32768,3360,-
-32768,267,- -32768,- -32768,6176,2141,- -32768,6176,- -32768,- -32768,3983,- -32768,
662,5551,3293,5551,5551,5551,5551,5551,628,2141,- -32768,- -32768,639,2017,643,458,
2803,- -32768,- -32768,- -32768,- -32768,- -32768,5931,5551,5931,5931,5931,5931,5931,
5931,5931,5931,5931,5931,5931,5931,5931,5931,5931,5931,5931,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5551,-
-32768,101,- -32768,- -32768,4639,101,5551,- -32768,646,223,- -32768,2934,- -32768,
2934,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5551,660,661,-
-32768,642,223,85,669,- -32768,- -32768,- -32768,5551,- -32768,659,5551,- -32768,5551,
674,752,- -32768,2557,692,- -32768,- -32768,- -32768,292,674,759,- -32768,4715,3842,
674,- -32768,- -32768,2353,711,719,5551,2353,- -32768,- -32768,3704,- -32768,- -32768,
- -32768,5551,- -32768,6176,705,152,- -32768,- -32768,694,- -32768,31,708,6071,576,-
-32768,5998,- -32768,5551,- -32768,397,509,- -32768,- -32768,2934,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,807,- -32768,101,5551,807,118,- -32768,-
-32768,- -32768,106,- -32768,45,716,- -32768,710,576,717,397,703,265,6056,5998,4791,
- -32768,- -32768,444,718,18,3586,- -32768,722,723,742,- -32768,- -32768,194,4411,463,
732,579,471,727,733,734,475,6176,741,- -32768,736,5703,- -32768,- -32768,744,745,
511,219,427,466,470,439,- 32,- 32,448,448,448,448,412,412,103,103,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,747,- -32768,119,5998,4247,- -32768,- -32768,-
-32768,- -32768,809,5551,576,223,592,492,5551,495,504,746,4867,4943,295,- -32768,-
-32768,753,757,748,758,- -32768,743,750,534,- -32768,3773,- -32768,- -32768,765,5998,
- -32768,764,31,- -32768,754,- -32768,145,- -32768,562,- -32768,509,- -32768,- -32768,-
-32768,- -32768,756,- -32768,- -32768,- -32768,- -32768,607,- -32768,- -32768,- -32768,
767,- -32768,- -32768,397,768,576,- 24,397,761,538,770,771,- -32768,- -32768,769,195,
772,- -32768,4071,- -32768,- -32768,- -32768,3773,- -32768,5551,838,5551,5627,774,769,
777,4247,- -32768,- -32768,5551,5551,- -32768,- -32768,- 24,324,- -32768,- -32768,6176,
- -32768,- -32768,- -32768,781,85,305,2353,540,783,2353,5551,5019,306,5095,307,5171,
616,- -32768,787,5551,800,- 31,790,- -32768,576,48,- -32768,- -32768,798,576,6176,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,788,- -32768,- -32768,397,- -32768,
811,- -32768,- -32768,797,799,5551,- -32768,- -32768,4411,543,803,802,- -32768,662,-
-32768,- -32768,345,- -32768,- -32768,- -32768,- -32768,4247,810,- -32768,805,20,592,
892,816,616,- -32768,552,2353,555,5247,2353,571,5323,5399,319,657,808,818,- -32768,
340,2353,2353,834,- -32768,- -32768,- -32768,824,- -32768,765,- -32768,- -32768,828,-
-32768,- -32768,249,- -32768,- -32768,6176,914,- -32768,4159,- -32768,6176,- -32768,-
-32768,2353,698,835,836,- -32768,2353,2353,629,- -32768,2353,2353,636,2353,652,
5475,829,- 44,2017,- -32768,5551,- -32768,- -32768,821,31,576,850,5551,837,839,-
-32768,667,- -32768,2429,840,842,- -32768,- -32768,- -32768,- -32768,2353,- -32768,-
-32768,2353,- -32768,2353,2353,675,2017,5551,1627,- -32768,683,856,- -32768,- -32768,
843,347,844,6176,- -32768,841,344,2017,- -32768,- -32768,- -32768,- -32768,- -32768,
2353,- -32768,352,616,- -32768,2353,2353,- -32768,- -32768,- -32768,847,2017,1757,-
-32768,- -32768,1627,- -32768,- -32768,- -32768,849,- -32768,698,- -32768,616,- -32768,
- -32768,- -32768,- -32768,951,959,- -32768};static short Cyc_yypgoto[116]={- -32768,
202,- -32768,617,- -32768,- -32768,- -32768,- -32768,- -32768,529,- 54,570,- -32768,-
-32768,- 39,442,- -32768,84,864,42,- -32768,- -32768,- 128,- -32768,- 13,- -32768,293,-
-32768,- -32768,- -32768,735,729,- 11,- -32768,- -32768,392,- -32768,- -32768,49,-
-32768,- -32768,110,- 55,- 45,- -32768,- -32768,926,- 252,833,356,- 116,- 58,- 125,- 220,
166,361,367,- 314,- 435,- 98,- 310,848,- -32768,- 216,- 181,- 559,- 294,- -32768,484,- 149,
- 71,- 130,32,- 222,255,- -32768,- -32768,- 14,- 73,- -32768,- 660,- 768,- -32768,- -32768,-
7,823,357,- -32768,147,80,- -32768,- 155,- 384,- 179,620,619,621,632,633,435,95,431,
432,467,688,- -32768,- -32768,- -32768,- 388,- -32768,- -32768,41,929,- -32768,- 363,-
122};static short Cyc_yytable[6289]={96,178,123,341,282,342,515,126,133,291,495,
567,668,281,426,364,87,410,296,134,166,167,168,132,520,673,674,318,338,207,606,
431,574,565,832,324,478,568,460,402,90,67,372,373,461,322,323,128,435,318,356,
450,161,161,161,444,365,593,32,445,161,161,760,73,88,35,89,403,627,362,833,451,
149,39,452,319,320,153,161,123,67,374,761,786,91,320,475,67,177,375,161,161,67,
510,132,464,874,319,235,118,161,730,308,428,67,878,239,240,138,510,620,325,276,
145,33,297,104,105,177,315,88,427,161,161,161,88,- 125,121,298,33,619,33,331,163,
163,163,465,67,432,307,485,163,163,94,304,67,67,67,67,97,310,311,312,603,626,436,
609,764,473,163,161,112,520,241,476,484,313,320,67,482,320,163,163,856,314,162,
162,162,32,350,208,163,67,162,162,35,521,145,522,455,456,88,458,670,169,868,316,
429,161,88,170,162,537,163,163,163,174,876,69,332,430,289,624,162,162,879,625,32,
67,95,413,380,466,162,35,622,239,614,423,461,381,382,329,493,494,502,666,277,462,
92,314,163,42,43,44,162,162,162,103,280,33,481,93,612,- 169,98,631,32,605,- 169,
178,485,466,34,35,36,- 191,161,315,117,100,280,- 191,67,161,147,133,472,514,163,67,
718,539,162,331,99,629,330,169,717,519,396,397,398,170,161,161,498,161,139,646,
422,177,303,647,67,227,228,229,230,32,648,161,412,492,142,313,35,177,161,461,162,
72,177,314,32,42,43,44,461,399,495,35,664,145,38,139,400,401,485,39,157,277,292,
158,88,357,177,163,420,42,43,44,722,150,163,38,141,671,461,32,129,424,810,425,
146,130,35,38,131,67,163,67,511,457,329,163,163,512,163,707,513,635,314,711,67,
634,657,106,107,162,571,767,163,509,633,590,162,441,687,163,151,461,600,613,461,
152,308,578,739,748,751,466,162,709,740,461,461,162,162,412,162,485,798,161,154,
532,533,534,461,734,721,155,161,735,162,161,156,542,97,667,722,162,601,803,308,
173,67,175,67,461,778,177,860,461,779,232,461,864,231,769,177,461,579,135,136,
867,553,554,555,556,514,161,161,564,726,763,727,696,542,161,766,357,293,618,722,
701,294,621,295,528,147,531,609,333,334,535,161,357,299,177,434,42,43,44,163,774,
443,310,732,448,449,546,710,163,370,371,163,302,721,376,377,615,306,67,542,16,17,
18,50,328,161,88,697,378,379,88,638,639,640,363,602,490,491,337,733,343,162,569,
466,501,491,367,163,163,344,162,542,345,162,67,163,346,721,347,161,672,541,461,
348,51,67,649,461,581,50,349,583,163,584,652,461,358,588,656,461,695,404,838,366,
125,71,368,637,369,162,162,673,674,405,598,678,461,162,680,461,407,412,572,414,
573,421,163,681,461,437,51,67,498,438,162,439,113,114,115,116,446,811,453,174,
454,127,161,50,50,50,50,235,754,755,651,125,808,694,461,74,163,713,714,742,461,
412,775,461,162,275,459,615,799,462,239,787,461,161,789,461,88,233,359,360,50,
361,32,469,51,51,51,51,474,35,793,461,127,814,320,617,420,39,162,395,470,177,477,
862,479,74,127,42,43,44,148,76,38,77,78,595,483,35,487,599,488,51,489,163,503,79,
504,738,80,81,816,817,493,494,517,82,516,412,834,679,83,84,518,684,686,725,536,
161,823,461,412,364,161,731,163,826,461,800,538,466,148,76,540,77,78,162,577,35,
853,570,855,828,461,275,575,79,74,582,80,81,177,576,32,865,580,82,843,334,424,35,
83,84,585,108,852,461,162,39,782,783,872,873,857,461,875,88,589,42,43,44,723,591,
412,161,127,551,552,557,558,596,559,560,597,604,33,412,607,610,163,628,632,75,76,
163,77,78,630,641,35,745,747,643,750,644,753,645,79,653,758,80,81,650,659,654,
655,74,82,561,562,563,658,83,84,662,682,663,665,690,688,412,162,689,691,724,692,
162,773,693,336,280,698,339,339,704,660,700,706,708,712,715,716,352,353,728,163,
719,729,737,743,339,339,757,339,759,762,791,765,768,795,797,148,76,32,77,78,770,
771,35,772,35,339,776,777,781,530,79,780,39,80,81,784,785,801,416,802,82,162,42,
43,44,83,84,806,741,807,50,744,50,809,812,820,819,836,831,839,830,841,842,847,
858,835,881,859,861,846,863,871,840,877,882,642,529,176,845,736,587,415,409,669,
102,290,703,702,837,699,594,51,284,51,854,623,305,705,339,545,547,125,119,548,0,
383,384,385,386,387,388,389,390,391,392,549,788,550,0,792,0,127,661,0,0,0,0,50,0,
804,805,127,393,0,0,0,0,0,0,0,0,0,127,0,0,0,0,0,0,0,0,0,0,0,815,0,0,0,0,821,822,
0,0,824,825,0,827,339,51,339,339,339,339,339,339,339,339,339,339,339,339,339,339,
339,339,339,0,0,0,0,0,0,0,848,0,0,849,0,850,851,0,0,0,0,0,0,433,0,0,0,0,0,0,0,0,
0,0,0,0,339,0,0,866,0,0,0,0,869,870,0,180,181,243,0,244,245,246,247,248,249,250,
251,0,182,275,183,252,0,184,0,253,0,0,0,0,254,185,186,255,256,187,188,189,190,0,
0,0,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,127,0,0,0,0,0,0,0,0,0,125,0,
339,259,195,196,197,198,0,0,35,0,0,0,260,124,0,0,199,0,0,0,287,0,0,0,0,0,201,0,0,
0,202,203,204,0,0,0,0,205,206,127,0,0,0,0,0,0,339,0,0,0,0,0,0,0,0,0,447,0,2,3,4,
70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 255,- 255,21,180,181,243,339,244,245,
246,247,248,249,250,251,22,182,23,183,252,0,184,24,253,0,0,27,28,254,185,186,255,
256,187,188,189,190,29,30,31,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,259,195,196,197,198,33,34,35,36,37,38,260,124,- 255,339,199,
0,0,40,200,0,0,339,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,447,0,2,3,4,70,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,- 251,- 251,21,180,181,243,0,244,245,246,247,
248,249,250,251,22,182,23,183,252,0,184,24,253,0,0,27,28,254,185,186,255,256,187,
188,189,190,29,30,31,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,259,195,196,197,198,33,34,35,36,37,38,260,124,- 251,0,199,0,0,40,
200,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,447,0,2,3,4,70,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,- 253,- 253,21,180,181,243,0,244,245,246,247,248,249,
250,251,22,182,23,183,252,0,184,24,253,0,0,27,28,254,185,186,255,256,187,188,189,
190,29,30,31,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,259,195,196,197,198,33,34,35,36,37,38,260,124,- 253,0,199,0,0,40,200,0,0,0,
0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,447,0,2,3,4,70,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,754,755,21,180,181,243,0,244,245,246,247,248,249,250,251,22,
182,23,183,252,0,184,24,253,0,0,27,28,254,185,186,255,256,187,188,189,190,29,30,
31,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,259,
195,196,197,198,33,34,35,36,37,38,260,124,- 265,0,199,0,0,40,200,0,0,0,0,0,201,0,
0,0,202,203,204,0,0,0,0,205,206,447,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,816,817,21,180,181,243,0,244,245,246,247,248,249,250,251,22,182,23,183,
252,0,184,24,253,0,0,27,28,254,185,186,255,256,187,188,189,190,29,30,31,257,191,
0,258,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,259,195,196,
197,198,33,34,35,36,37,38,260,124,- 272,0,199,0,0,40,200,0,0,0,0,0,201,0,0,0,202,
203,204,0,0,0,0,205,206,242,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,180,181,243,0,244,245,246,247,248,249,250,251,22,182,23,183,252,0,184,24,
253,0,0,27,28,254,185,186,255,256,187,188,189,190,29,30,31,257,191,0,258,192,0,0,
0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,259,195,196,197,198,33,34,35,
36,37,38,260,124,261,0,199,0,0,40,200,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,
205,206,447,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,
243,0,244,245,246,247,248,249,250,251,22,182,23,183,252,0,184,24,253,0,0,27,28,
254,185,186,255,256,187,188,189,190,29,30,31,257,191,0,258,192,0,0,0,193,194,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,259,195,196,197,198,33,34,35,36,37,38,260,
124,0,0,199,0,0,40,200,0,0,0,0,0,201,0,0,0,202,203,204,354,0,0,0,205,206,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,180,181,0,0,0,0,0,0,0,0,0,0,22,182,0,183,
0,0,184,0,0,0,0,27,28,0,185,186,0,0,187,188,189,190,29,30,31,0,191,0,0,192,0,0,0,
193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,33,34,35,36,
37,38,0,355,0,442,199,0,0,40,200,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,
180,181,243,0,244,245,246,247,248,249,250,251,0,182,0,183,252,0,184,0,253,0,0,0,
0,254,185,186,255,256,187,188,189,190,0,0,0,257,191,0,258,192,0,0,0,193,194,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,259,195,196,197,198,0,0,35,0,0,0,260,124,0,
447,199,0,0,0,287,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,180,181,243,0,
244,245,246,247,248,249,250,251,0,182,0,183,252,0,184,0,253,0,0,0,0,254,185,186,
255,256,187,188,189,190,0,0,0,257,191,0,258,192,0,0,0,193,194,0,0,0,0,0,844,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,259,195,196,197,198,0,0,35,180,181,0,260,124,0,0,199,0,
0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,
190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,195,196,197,198,0,0,35,0,0,0,0,0,0,0,199,0,0,0,287,0,0,0,0,0,201,0,0,0,202,
203,204,0,0,0,0,205,206,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
180,181,0,0,0,0,0,0,0,0,0,0,22,182,23,183,0,0,184,24,0,0,0,27,28,0,185,186,0,0,
187,188,189,190,29,30,31,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,32,195,196,197,198,33,34,35,36,37,38,586,0,0,0,199,0,0,40,200,0,0,
0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,0,180,181,0,0,0,0,0,0,0,0,0,0,22,182,0,183,0,0,184,0,0,0,0,27,28,0,185,
186,0,0,187,188,189,190,29,30,31,0,191,159,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,33,34,35,36,37,38,0,160,0,0,199,0,0,
40,200,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,180,181,0,0,0,0,0,0,0,0,0,0,22,182,0,183,0,0,184,0,0,0,0,27,
28,0,185,186,0,0,187,188,189,190,29,30,31,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,33,34,35,36,37,38,0,0,0,0,199,
0,0,40,200,0,0,0,0,0,201,0,0,0,202,203,204,0,0,0,0,205,206,- 8,1,0,2,3,4,5,6,7,8,
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
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,32,0,0,0,0,33,34,35,36,37,
38,280,0,0,0,329,0,0,40,41,0,0,0,314,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,
0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,0,0,0,
0,0,33,34,0,36,37,38,280,0,0,0,313,0,0,40,41,0,0,0,314,0,42,43,44,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,
28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,0,0,39,0,0,40,41,0,0,0,0,0,42,43,44,
2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,
22,0,23,0,0,0,0,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,
3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,
0,124,22,233,23,0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,
0,0,0,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,
36,37,38,0,124,22,0,23,0,0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,
4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
33,34,0,36,37,38,24,0,0,0,27,28,0,40,41,0,0,0,0,0,0,29,30,31,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,33,34,0,36,37,
38,27,28,180,181,0,0,0,40,41,523,0,29,30,31,0,182,0,183,0,0,184,0,0,0,0,0,0,0,
185,186,0,0,187,188,189,190,0,0,0,0,191,0,278,192,0,279,0,193,194,0,33,34,0,36,
37,38,280,0,0,0,0,0,0,40,41,0,0,0,0,0,32,195,196,197,198,0,0,35,0,0,0,0,340,524,
0,199,0,0,0,287,180,181,493,494,0,201,0,0,0,202,203,204,0,182,0,183,205,206,184,
0,0,0,0,0,0,0,185,186,0,0,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,0,0,0,0,340,720,
0,199,0,0,0,287,180,181,493,494,0,201,0,0,0,202,203,204,0,182,0,183,205,206,184,
0,0,0,0,0,0,0,185,186,0,0,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,0,0,0,0,340,813,
0,199,0,0,0,287,180,181,493,494,0,201,0,0,0,202,203,204,0,182,0,183,205,206,184,
0,0,0,0,0,0,0,185,186,0,0,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,0,0,0,0,340,0,0,
199,0,0,0,287,180,181,493,494,0,201,0,0,0,202,203,204,0,182,0,183,205,206,184,0,
0,0,0,0,0,0,185,186,0,0,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,
199,0,0,0,287,0,182,0,183,288,201,184,0,0,202,203,204,0,0,185,186,205,206,187,
188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,340,0,0,199,0,0,0,287,0,182,0,183,
0,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,
192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,
0,35,180,181,0,440,0,0,0,199,0,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,
185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,0,0,
0,287,0,182,0,183,508,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,
190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,566,0,0,287,0,182,0,183,0,201,
184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,
0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,
180,181,0,0,0,0,0,199,592,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,185,
186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,0,0,0,
287,0,182,0,183,636,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,
0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
195,196,197,198,0,0,35,180,181,0,683,0,0,0,199,0,0,0,287,0,182,0,183,0,201,184,0,
0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,
194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,
0,685,0,0,0,199,0,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,185,186,205,
206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,746,0,0,287,0,
182,0,183,0,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,
191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,
197,198,0,0,35,180,181,0,0,0,0,0,199,749,0,0,287,0,182,0,183,0,201,184,0,0,202,
203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,752,
0,0,0,199,0,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,185,186,205,206,
187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,790,0,0,287,0,182,0,
183,0,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,
0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,
0,0,35,180,181,0,0,0,0,0,199,794,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,
0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,
796,0,0,287,0,182,0,183,0,201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,
189,190,0,0,0,0,191,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,199,829,0,0,287,0,182,0,183,0,
201,184,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,191,0,0,192,
0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,
35,180,181,0,0,0,0,0,199,0,0,0,287,0,182,0,0,0,201,0,0,0,202,203,204,0,0,185,186,
205,206,187,188,189,190,0,0,0,0,0,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,340,0,0,199,0,0,0,287,0,
182,0,0,0,201,0,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,0,0,0,
0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,
0,0,35,180,181,0,0,660,0,0,199,0,0,0,287,0,182,0,0,0,201,0,0,0,202,203,204,0,0,
185,186,205,206,187,188,189,190,0,0,0,0,0,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,197,198,0,0,35,180,181,0,0,0,0,0,335,0,0,0,
287,0,182,0,0,0,201,0,0,0,202,203,204,0,0,185,186,205,206,187,188,189,190,0,0,0,
0,0,0,0,192,0,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,195,196,
197,198,0,0,35,180,181,0,0,0,0,0,351,0,0,0,287,0,182,0,0,0,201,0,0,0,202,203,204,
0,0,185,186,205,206,187,188,189,190,0,0,0,0,0,0,0,192,0,0,0,193,194,0,0,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,32,195,196,197,198,0,0,35,0,0,0,22,0,0,
0,199,0,0,0,287,0,0,27,28,0,201,0,0,0,202,203,204,0,29,30,31,205,206,159,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,
33,34,0,36,37,38,0,160,27,28,0,0,22,40,41,0,0,0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,
0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,0,0,0,0,0,33,34,0,36,37,38,280,
611,0,0,0,0,0,40,41,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,
0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41};static short Cyc_yycheck[6289]={
39,99,60,184,129,184,320,61,63,139,304,399,571,129,1,79,23,233,146,64,91,92,93,
62,334,5,6,73,183,100,465,1,416,396,78,17,288,400,104,91,93,0,74,75,110,167,168,
61,1,73,199,93,91,92,93,104,120,445,93,108,99,100,93,22,23,100,25,119,503,106,
114,113,79,108,116,121,122,84,117,137,39,113,113,743,113,122,110,46,99,121,129,
130,51,315,133,64,864,121,112,58,139,660,160,1,63,873,117,120,67,331,494,93,126,
72,98,98,104,105,129,164,79,108,161,162,163,84,106,111,111,98,493,98,177,91,92,
93,105,96,108,106,295,99,100,92,157,104,105,106,107,39,161,162,163,463,109,108,
470,109,283,117,199,51,472,122,286,293,108,122,127,291,122,129,130,833,116,91,92,
93,93,192,100,139,141,99,100,100,335,146,337,262,263,150,265,577,93,855,164,95,
237,158,99,117,351,161,162,163,96,867,95,177,108,131,106,129,130,875,110,93,177,
108,234,118,280,139,100,107,237,479,241,110,127,128,108,115,116,306,117,127,93,
113,116,199,118,119,120,161,162,163,46,104,98,290,113,473,109,92,508,93,106,114,
358,416,320,99,100,101,104,306,313,105,105,104,110,232,313,113,331,110,317,237,
239,644,355,199,329,108,506,177,93,94,329,69,70,71,99,334,335,304,337,113,106,
115,313,156,110,264,104,105,106,107,93,527,351,233,104,95,108,100,329,358,110,
237,105,334,116,93,118,119,120,110,108,625,100,114,293,103,113,115,116,494,108,
105,232,141,108,304,199,358,306,239,118,119,120,647,108,313,103,105,578,110,93,
108,104,114,106,105,113,100,103,116,329,329,331,108,264,108,334,335,113,337,632,
116,512,116,636,344,511,536,104,105,306,404,702,351,314,511,104,313,251,104,358,
93,110,457,475,110,94,465,421,104,104,104,470,329,634,110,110,110,334,335,340,
337,577,104,463,104,346,347,348,110,106,647,110,472,110,351,475,107,358,329,570,
735,358,457,104,503,108,406,109,408,110,106,463,106,110,110,110,110,114,104,712,
472,110,422,67,68,114,372,373,374,375,510,511,512,394,656,696,656,603,399,519,
701,335,110,491,779,611,106,495,107,343,113,345,807,109,110,349,536,351,109,511,
246,118,119,120,463,722,252,519,664,255,256,365,635,472,76,77,475,93,735,72,73,
480,108,483,445,17,18,19,0,109,570,491,604,122,123,495,93,94,95,106,461,109,110,
108,667,108,463,401,607,109,110,125,511,512,108,472,477,108,475,519,519,108,779,
108,604,579,109,110,108,0,530,109,110,427,46,108,430,536,432,109,110,108,436,109,
110,600,105,808,78,61,21,126,513,124,511,512,5,6,106,453,109,110,519,109,110,106,
527,406,106,408,105,570,109,110,93,46,576,625,104,536,104,52,53,54,55,108,776,
108,519,114,61,671,104,105,106,107,651,22,23,530,112,767,109,110,38,604,109,110,
109,110,571,109,110,570,126,106,615,1,93,671,109,110,702,109,110,625,107,201,202,
141,204,93,109,104,105,106,107,109,100,109,110,112,781,122,483,576,108,604,222,
114,702,108,842,117,38,126,118,119,120,93,94,103,96,97,450,106,100,106,454,121,
141,121,671,110,108,106,676,111,112,22,23,115,116,109,118,108,647,801,582,123,
124,110,586,587,655,108,776,109,110,660,79,781,663,702,109,110,754,109,807,93,94,
109,96,97,671,114,100,831,113,833,109,110,234,104,108,38,108,111,112,781,110,93,
846,105,118,109,110,104,100,123,124,30,104,109,110,702,108,739,740,863,864,109,
110,867,754,104,118,119,120,653,42,722,842,234,370,371,376,377,98,378,379,93,108,
98,735,122,109,776,109,117,93,94,781,96,97,109,109,100,682,683,109,685,110,687,
93,108,110,691,111,112,109,106,110,110,38,118,380,381,382,109,123,124,109,108,
110,109,109,105,779,776,104,104,25,121,781,719,117,180,104,106,183,184,117,105,
121,109,109,117,109,109,193,194,109,842,113,109,106,105,201,202,104,204,93,104,
748,98,109,751,752,93,94,93,96,97,94,109,100,109,100,222,108,110,108,344,108,106,
108,111,112,28,105,114,114,106,118,842,118,119,120,123,124,98,678,110,406,681,
408,110,25,104,106,121,114,94,798,109,108,106,93,803,0,109,109,114,114,109,810,
109,0,518,344,98,816,671,436,237,232,576,43,137,615,611,807,607,446,406,129,408,
832,496,158,625,295,364,366,457,58,367,- 1,80,81,82,83,84,85,86,87,88,89,368,746,
369,- 1,749,- 1,436,540,- 1,- 1,- 1,- 1,483,- 1,759,760,446,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,457,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,- 1,- 1,789,790,- 1,- 1,793,794,- 1,
796,364,483,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,823,- 1,- 1,826,- 1,828,829,- 1,- 1,- 1,- 1,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,852,- 1,- 1,- 1,- 1,857,858,- 1,25,26,27,- 1,29,30,31,
32,33,34,35,36,- 1,38,600,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,
58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,600,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,651,- 1,494,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,108,
- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,651,- 1,
- 1,- 1,- 1,- 1,- 1,540,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,22,23,24,25,26,27,577,29,30,31,32,33,34,35,36,37,38,39,40,
41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,
- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,98,99,100,101,102,103,104,105,106,656,108,- 1,- 1,111,112,- 1,- 1,664,- 1,- 1,
118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,
- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,
- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,
44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,
98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,
- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,
- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,
101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,
102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,
- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,1,- 1,
- 1,- 1,129,130,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,
57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,1,108,
- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,25,26,
27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,
54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,
1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,
25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,
52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,
105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,
129,130,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,-
1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,
130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,
58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,
- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,
- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,
- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,
- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,
123,124,- 1,- 1,- 1,- 1,129,130,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,
47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,
100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,
46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,
100,101,102,103,- 1,- 1,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,37,- 1,- 1,- 1,108,- 1,- 1,111,
112,- 1,- 1,48,49,- 1,118,119,120,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,
- 1,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,116,- 1,118,119,
120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,116,- 1,118,119,120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,-
1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,
- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,107,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,
105,37,- 1,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,
- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,98,99,- 1,101,102,103,44,- 1,- 1,- 1,48,49,- 1,111,112,- 1,- 1,- 1,
- 1,- 1,- 1,59,60,61,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,48,49,25,26,- 1,
- 1,- 1,111,112,32,- 1,59,60,61,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,90,66,- 1,93,- 1,70,71,- 1,98,99,- 1,101,102,103,
104,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,-
1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,
40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,
66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,-
1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,
- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,
130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,-
1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,-
1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,
- 1,- 1,- 1,112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,
57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,105,- 1,- 1,108,- 1,
- 1,- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,
58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,104,- 1,- 1,- 1,108,- 1,- 1,
- 1,112,- 1,38,- 1,40,- 1,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,-
1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,
112,- 1,38,- 1,40,117,118,43,- 1,- 1,122,123,124,- 1,- 1,51,52,129,130,55,56,57,58,- 1,
- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,25,26,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,
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
_tmp3EB[8]="stdcall";static char _tmp3EC[6]="cdecl";static char _tmp3ED[9]="fastcall";
static char _tmp3EE[9]="noreturn";static char _tmp3EF[6]="const";static char _tmp3F0[8]="aligned";
static char _tmp3F1[7]="packed";static char _tmp3F2[7]="shared";static char _tmp3F3[7]="unused";
static char _tmp3F4[5]="weak";static char _tmp3F5[10]="dllimport";static char _tmp3F6[
10]="dllexport";static char _tmp3F7[23]="no_instrument_function";static char _tmp3F8[
12]="constructor";static char _tmp3F9[11]="destructor";static char _tmp3FA[22]="no_check_memory_usage";
static char _tmp3FB[5]="pure";struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp944=10000;short*_tmp945=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp944));{unsigned int _tmp946=
_tmp944;unsigned int i;for(i=0;i < _tmp946;i ++){_tmp945[i]=(short)0;}}_tmp945;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp941=10000;void**_tmp942=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp941));{unsigned int _tmp943=
_tmp941;unsigned int i;for(i=0;i < _tmp943;i ++){_tmp942[i]=Cyc_yylval;}}_tmp942;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp93E=
10000;struct Cyc_Yyltype*_tmp93F=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp93E));{unsigned int _tmp940=_tmp93E;
unsigned int i;for(i=0;i < _tmp940;i ++){_tmp93F[i]=Cyc_yynewloc();}}_tmp93F;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp33D="parser stack overflow";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(883,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar <= 358: 0)?(
int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 247;}yyn +=
yychar1;if((yyn < 0?1: yyn > 6288)?1: Cyc_yycheck[_check_known_subscript_notnull(
6289,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6289,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 882)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(883,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(435,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1C2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1C3: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp33E;_push_handler(& _tmp33E);{int _tmp340=0;if(setjmp(_tmp33E.handler))
_tmp340=1;if(!_tmp340){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp33F=(void*)_exn_thrown;void*_tmp342=_tmp33F;_LL1C6: if(*((void**)_tmp342)
!= Cyc_Core_Failure)goto _LL1C8;_LL1C7: x=0;goto _LL1C5;_LL1C8:;_LL1C9:(void)_throw(
_tmp342);_LL1C5:;}}}{struct _handler_cons _tmp343;_push_handler(& _tmp343);{int
_tmp345=0;if(setjmp(_tmp343.handler))_tmp345=1;if(!_tmp345){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp344=(void*)_exn_thrown;void*_tmp347=_tmp344;_LL1CB: if(*((void**)_tmp347)
!= Cyc_Core_Failure)goto _LL1CD;_LL1CC: y=0;goto _LL1CA;_LL1CD:;_LL1CE:(void)_throw(
_tmp347);_LL1CA:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp348=
_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_DeclList_tok_struct _tmp349;
_tmp349.tag=Cyc_DeclList_tok;_tmp349.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp349;});_tmp348;});break;}
case 3: _LL1C4: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34A=_cycalloc(
sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_DeclList_tok_struct _tmp34B;_tmp34B.tag=
Cyc_DeclList_tok;_tmp34B.f1=({struct Cyc_List_List*_tmp34C=_cycalloc(sizeof(*
_tmp34C));_tmp34C->hd=({struct Cyc_Absyn_Decl*_tmp34D=_cycalloc(sizeof(*_tmp34D));
_tmp34D->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp34E=_cycalloc(
sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_Absyn_Using_d_struct _tmp34F;_tmp34F.tag=
9;_tmp34F.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp34F.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp34F;});_tmp34E;}));
_tmp34D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp34D;});_tmp34C->tl=0;_tmp34C;});_tmp34B;});
_tmp34A;});Cyc_Lex_leave_using();break;case 4: _LL1CF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_DeclList_tok_struct
_tmp351;_tmp351.tag=Cyc_DeclList_tok;_tmp351.f1=({struct Cyc_List_List*_tmp352=
_cycalloc(sizeof(*_tmp352));_tmp352->hd=({struct Cyc_Absyn_Decl*_tmp353=_cycalloc(
sizeof(*_tmp353));_tmp353->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Absyn_Using_d_struct
_tmp355;_tmp355.tag=9;_tmp355.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp355.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp355;});_tmp354;}));
_tmp353->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp353;});_tmp352->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp352;});_tmp351;});
_tmp350;});break;case 5: _LL1D0: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_DeclList_tok_struct
_tmp357;_tmp357.tag=Cyc_DeclList_tok;_tmp357.f1=({struct Cyc_List_List*_tmp358=
_cycalloc(sizeof(*_tmp358));_tmp358->hd=({struct Cyc_Absyn_Decl*_tmp359=_cycalloc(
sizeof(*_tmp359));_tmp359->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp35B;_tmp35B.tag=8;_tmp35B.f1=({struct _tagged_arr*_tmp35C=_cycalloc(sizeof(*
_tmp35C));_tmp35C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp35C;});_tmp35B.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp35B;});_tmp35A;}));
_tmp359->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp359;});_tmp358->tl=0;_tmp358;});_tmp357;});
_tmp356;});Cyc_Lex_leave_namespace();break;case 6: _LL1D1: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp35D;
_push_handler(& _tmp35D);{int _tmp35F=0;if(setjmp(_tmp35D.handler))_tmp35F=1;if(!
_tmp35F){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp35E=(void*)_exn_thrown;void*
_tmp361=_tmp35E;_LL1D4: if(*((void**)_tmp361)!= Cyc_Core_Failure)goto _LL1D6;
_LL1D5: nspace=({const char*_tmp362="";_tag_arr(_tmp362,sizeof(char),
_get_zero_arr_size(_tmp362,1));});goto _LL1D3;_LL1D6:;_LL1D7:(void)_throw(_tmp361);
_LL1D3:;}}}{struct _handler_cons _tmp363;_push_handler(& _tmp363);{int _tmp365=0;if(
setjmp(_tmp363.handler))_tmp365=1;if(!_tmp365){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp364=(void*)_exn_thrown;void*_tmp367=_tmp364;_LL1D9: if(*((void**)_tmp367)
!= Cyc_Core_Failure)goto _LL1DB;_LL1DA: x=0;goto _LL1D8;_LL1DB:;_LL1DC:(void)_throw(
_tmp367);_LL1D8:;}}}{struct _handler_cons _tmp368;_push_handler(& _tmp368);{int
_tmp36A=0;if(setjmp(_tmp368.handler))_tmp36A=1;if(!_tmp36A){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp369=(void*)_exn_thrown;void*_tmp36C=_tmp369;_LL1DE: if(*((void**)_tmp36C)
!= Cyc_Core_Failure)goto _LL1E0;_LL1DF: y=0;goto _LL1DD;_LL1E0:;_LL1E1:(void)_throw(
_tmp36C);_LL1DD:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp36D=
_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_DeclList_tok_struct _tmp36E;
_tmp36E.tag=Cyc_DeclList_tok;_tmp36E.f1=({struct Cyc_List_List*_tmp36F=_cycalloc(
sizeof(*_tmp36F));_tmp36F->hd=({struct Cyc_Absyn_Decl*_tmp370=_cycalloc(sizeof(*
_tmp370));_tmp370->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp371=
_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp372;_tmp372.tag=8;_tmp372.f1=({struct _tagged_arr*_tmp373=_cycalloc(sizeof(*
_tmp373));_tmp373[0]=nspace;_tmp373;});_tmp372.f2=x;_tmp372;});_tmp371;}));
_tmp370->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp370;});_tmp36F->tl=y;_tmp36F;});_tmp36E;});
_tmp36D;});break;}case 7: _LL1D2: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp374="C";
_tag_arr(_tmp374,sizeof(char),_get_zero_arr_size(_tmp374,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp375="only extern \"C\" { ... } is allowed";_tag_arr(_tmp375,
sizeof(char),_get_zero_arr_size(_tmp375,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376[0]=({
struct Cyc_DeclList_tok_struct _tmp377;_tmp377.tag=Cyc_DeclList_tok;_tmp377.f1=({
struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->hd=({struct Cyc_Absyn_Decl*
_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp37B;_tmp37B.tag=10;_tmp37B.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp37B;});_tmp37A;}));
_tmp379->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp379;});_tmp378->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp378;});_tmp377;});
_tmp376;});break;case 8: _LL1E2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=({struct Cyc_DeclList_tok_struct
_tmp37D;_tmp37D.tag=Cyc_DeclList_tok;_tmp37D.f1=0;_tmp37D;});_tmp37C;});break;
case 9: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp37E=_cycalloc(
sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_DeclList_tok_struct _tmp37F;_tmp37F.tag=
Cyc_DeclList_tok;_tmp37F.f1=({struct Cyc_List_List*_tmp380=_cycalloc(sizeof(*
_tmp380));_tmp380->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp382;_tmp382.tag=1;_tmp382.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp382;});_tmp381;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp380->tl=0;
_tmp380;});_tmp37F;});_tmp37E;});break;case 10: _LL1E4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1E5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp383=_cycalloc(sizeof(*_tmp383));
_tmp383[0]=({struct Cyc_DeclList_tok_struct _tmp384;_tmp384.tag=Cyc_DeclList_tok;
_tmp384.f1=0;_tmp384;});_tmp383;});break;case 12: _LL1E6: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({struct Cyc_FnDecl_tok_struct
_tmp386;_tmp386.tag=Cyc_FnDecl_tok;_tmp386.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp386;});
_tmp385;});break;case 13: _LL1E7: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387[0]=({struct Cyc_FnDecl_tok_struct
_tmp388;_tmp388.tag=Cyc_FnDecl_tok;_tmp388.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp389;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp388;});
_tmp387;});break;case 14: _LL1E8: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_FnDecl_tok_struct
_tmp38B;_tmp38B.tag=Cyc_FnDecl_tok;_tmp38B.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38B;});
_tmp38A;});break;case 15: _LL1E9: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_FnDecl_tok_struct
_tmp38D;_tmp38D.tag=Cyc_FnDecl_tok;_tmp38D.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp38E;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38D;});
_tmp38C;});break;case 16: _LL1EA: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_FnDecl_tok_struct
_tmp390;_tmp390.tag=Cyc_FnDecl_tok;_tmp390.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp391;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp390;});
_tmp38F;});break;case 17: _LL1EB: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392[0]=({struct Cyc_FnDecl_tok_struct
_tmp393;_tmp393.tag=Cyc_FnDecl_tok;_tmp393.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp394;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp393;});
_tmp392;});break;case 18: _LL1EC: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 19: _LL1ED: Cyc_Lex_leave_using();
break;case 20: _LL1EE: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp395=
_cycalloc(sizeof(*_tmp395));_tmp395[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp395;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 21: _LL1EF: Cyc_Lex_leave_namespace();
break;case 22: _LL1F0: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp396=
_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_DeclList_tok_struct _tmp397;
_tmp397.tag=Cyc_DeclList_tok;_tmp397.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp397;});
_tmp396;});break;case 23: _LL1F1: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_DeclList_tok_struct
_tmp399;_tmp399.tag=Cyc_DeclList_tok;_tmp399.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp399;});
_tmp398;});break;case 24: _LL1F2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_DeclList_tok_struct
_tmp39B;_tmp39B.tag=Cyc_DeclList_tok;_tmp39B.f1=({struct Cyc_List_List*_tmp39C=
_cycalloc(sizeof(*_tmp39C));_tmp39C->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp39C->tl=0;
_tmp39C;});_tmp39B;});_tmp39A;});break;case 25: _LL1F3: {struct Cyc_List_List*
_tmp39D=0;{struct Cyc_List_List*_tmp39E=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp39E != 0;
_tmp39E=_tmp39E->tl){struct _tagged_arr*_tmp39F=(struct _tagged_arr*)_tmp39E->hd;
struct _tuple1*_tmp3A0=({struct _tuple1*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));
_tmp3A3->f1=Cyc_Absyn_rel_ns_null;_tmp3A3->f2=_tmp39F;_tmp3A3;});struct Cyc_Absyn_Vardecl*
_tmp3A1=Cyc_Absyn_new_vardecl(_tmp3A0,Cyc_Absyn_wildtyp(0),0);_tmp39D=({struct
Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->hd=_tmp3A1;_tmp3A2->tl=
_tmp39D;_tmp3A2;});}}_tmp39D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp39D);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_DeclList_tok_struct _tmp3A5;
_tmp3A5.tag=Cyc_DeclList_tok;_tmp3A5.f1=({struct Cyc_List_List*_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6->hd=Cyc_Absyn_letv_decl(_tmp39D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A6->tl=0;
_tmp3A6;});_tmp3A5;});_tmp3A4;});break;}case 26: _LL1F4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL1F5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));
_tmp3A7[0]=({struct Cyc_DeclList_tok_struct _tmp3A8;_tmp3A8.tag=Cyc_DeclList_tok;
_tmp3A8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3A8;});_tmp3A7;});
break;case 28: _LL1F6: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3A9=
_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_DeclSpec_tok_struct _tmp3AA;
_tmp3AA.tag=Cyc_DeclSpec_tok;_tmp3AA.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->sc=({struct Cyc_Core_Opt*_tmp3AC=
_cycalloc(sizeof(*_tmp3AC));_tmp3AC->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3AC;});_tmp3AB->tq=Cyc_Absyn_empty_tqual();
_tmp3AB->type_specs=0;_tmp3AB->is_inline=0;_tmp3AB->attributes=0;_tmp3AB;});
_tmp3AA;});_tmp3A9;});break;case 29: _LL1F7: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp3AD[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp3AE="Only one storage class is allowed in a declaration";
_tag_arr(_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,51));}),_tag_arr(
_tmp3AD,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B0;_tmp3B0.tag=Cyc_DeclSpec_tok;_tmp3B0.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->sc=({struct Cyc_Core_Opt*_tmp3B2=
_cycalloc(sizeof(*_tmp3B2));_tmp3B2->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B2;});_tmp3B1->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3B1->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3B1->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3B1->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3B1;});_tmp3B0;});_tmp3AF;});break;case 30: _LL1F8: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B4;_tmp3B4.tag=Cyc_DeclSpec_tok;_tmp3B4.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->sc=0;_tmp3B5->tq=Cyc_Absyn_empty_tqual();
_tmp3B5->type_specs=({struct Cyc_List_List*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));
_tmp3B6->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B6->tl=0;_tmp3B6;});
_tmp3B5->is_inline=0;_tmp3B5->attributes=0;_tmp3B5;});_tmp3B4;});_tmp3B3;});
break;case 31: _LL1F9: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3B7=
_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_DeclSpec_tok_struct _tmp3B8;
_tmp3B8.tag=Cyc_DeclSpec_tok;_tmp3B8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3B9->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3B9->type_specs=({
struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3BA->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3BA;});
_tmp3B9->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3B9->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3B9;});
_tmp3B8;});_tmp3B7;});break;case 32: _LL1FA: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3BC;_tmp3BC.tag=Cyc_DeclSpec_tok;_tmp3BC.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->sc=0;_tmp3BD->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BD->type_specs=0;
_tmp3BD->is_inline=0;_tmp3BD->attributes=0;_tmp3BD;});_tmp3BC;});_tmp3BB;});
break;case 33: _LL1FB: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3BE=
_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_DeclSpec_tok_struct _tmp3BF;
_tmp3BF.tag=Cyc_DeclSpec_tok;_tmp3BF.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3C0->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3C0->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3C0->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3C0->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3C0;});_tmp3BF;});_tmp3BE;});break;case 34: _LL1FC: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C2;_tmp3C2.tag=Cyc_DeclSpec_tok;_tmp3C2.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->sc=0;_tmp3C3->tq=Cyc_Absyn_empty_tqual();
_tmp3C3->type_specs=0;_tmp3C3->is_inline=1;_tmp3C3->attributes=0;_tmp3C3;});
_tmp3C2;});_tmp3C1;});break;case 35: _LL1FD: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C5;_tmp3C5.tag=Cyc_DeclSpec_tok;_tmp3C5.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3C6->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3C6->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3C6->is_inline=1;_tmp3C6->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3C6;});
_tmp3C5;});_tmp3C4;});break;case 36: _LL1FE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C8;_tmp3C8.tag=Cyc_DeclSpec_tok;_tmp3C8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->sc=0;_tmp3C9->tq=Cyc_Absyn_empty_tqual();
_tmp3C9->type_specs=0;_tmp3C9->is_inline=0;_tmp3C9->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C9;});_tmp3C8;});
_tmp3C7;});break;case 37: _LL1FF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CB;_tmp3CB.tag=Cyc_DeclSpec_tok;_tmp3CB.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3CC->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3CC->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3CC->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3CC->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3CC;});
_tmp3CB;});_tmp3CA;});break;case 38: _LL200: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_StorageClass_tok_struct
_tmp3CE;_tmp3CE.tag=Cyc_StorageClass_tok;_tmp3CE.f1=(void*)((void*)4);_tmp3CE;});
_tmp3CD;});break;case 39: _LL201: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D0;_tmp3D0.tag=Cyc_StorageClass_tok;_tmp3D0.f1=(void*)((void*)5);_tmp3D0;});
_tmp3CF;});break;case 40: _LL202: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_StorageClass_tok;_tmp3D2.f1=(void*)((void*)3);_tmp3D2;});
_tmp3D1;});break;case 41: _LL203: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D4;_tmp3D4.tag=Cyc_StorageClass_tok;_tmp3D4.f1=(void*)((void*)1);_tmp3D4;});
_tmp3D3;});break;case 42: _LL204: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3D5="C";
_tag_arr(_tmp3D5,sizeof(char),_get_zero_arr_size(_tmp3D5,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3D6="only extern or extern \"C\" is allowed";_tag_arr(_tmp3D6,
sizeof(char),_get_zero_arr_size(_tmp3D6,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({
struct Cyc_StorageClass_tok_struct _tmp3D8;_tmp3D8.tag=Cyc_StorageClass_tok;
_tmp3D8.f1=(void*)((void*)2);_tmp3D8;});_tmp3D7;});break;case 43: _LL205: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));
_tmp3D9[0]=({struct Cyc_StorageClass_tok_struct _tmp3DA;_tmp3DA.tag=Cyc_StorageClass_tok;
_tmp3DA.f1=(void*)((void*)0);_tmp3DA;});_tmp3D9;});break;case 44: _LL206: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));
_tmp3DB[0]=({struct Cyc_StorageClass_tok_struct _tmp3DC;_tmp3DC.tag=Cyc_StorageClass_tok;
_tmp3DC.f1=(void*)((void*)6);_tmp3DC;});_tmp3DB;});break;case 45: _LL207: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD[0]=({struct Cyc_AttributeList_tok_struct _tmp3DE;_tmp3DE.tag=Cyc_AttributeList_tok;
_tmp3DE.f1=0;_tmp3DE;});_tmp3DD;});break;case 46: _LL208: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 47: _LL209: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 48: _LL20A:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3DF=_cycalloc(sizeof(*
_tmp3DF));_tmp3DF[0]=({struct Cyc_AttributeList_tok_struct _tmp3E0;_tmp3E0.tag=Cyc_AttributeList_tok;
_tmp3E0.f1=({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3E1->tl=0;_tmp3E1;});_tmp3E0;});_tmp3DF;});break;case 49:
_LL20B: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3E2=_cycalloc(
sizeof(*_tmp3E2));_tmp3E2[0]=({struct Cyc_AttributeList_tok_struct _tmp3E3;_tmp3E3.tag=
Cyc_AttributeList_tok;_tmp3E3.f1=({struct Cyc_List_List*_tmp3E4=_cycalloc(sizeof(*
_tmp3E4));_tmp3E4->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E4->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E4;});_tmp3E3;});
_tmp3E2;});break;case 50: _LL20C: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple17 att_map[17]={{{_tmp3EB,_tmp3EB,_tmp3EB + 8},(
void*)0},{{_tmp3EC,_tmp3EC,_tmp3EC + 6},(void*)1},{{_tmp3ED,_tmp3ED,_tmp3ED + 9},(
void*)2},{{_tmp3EE,_tmp3EE,_tmp3EE + 9},(void*)3},{{_tmp3EF,_tmp3EF,_tmp3EF + 6},(
void*)4},{{_tmp3F0,_tmp3F0,_tmp3F0 + 8},(void*)& att_aligned},{{_tmp3F1,_tmp3F1,
_tmp3F1 + 7},(void*)5},{{_tmp3F2,_tmp3F2,_tmp3F2 + 7},(void*)7},{{_tmp3F3,_tmp3F3,
_tmp3F3 + 7},(void*)8},{{_tmp3F4,_tmp3F4,_tmp3F4 + 5},(void*)9},{{_tmp3F5,_tmp3F5,
_tmp3F5 + 10},(void*)10},{{_tmp3F6,_tmp3F6,_tmp3F6 + 10},(void*)11},{{_tmp3F7,
_tmp3F7,_tmp3F7 + 23},(void*)12},{{_tmp3F8,_tmp3F8,_tmp3F8 + 12},(void*)13},{{
_tmp3F9,_tmp3F9,_tmp3F9 + 11},(void*)14},{{_tmp3FA,_tmp3FA,_tmp3FA + 22},(void*)15},{{
_tmp3FB,_tmp3FB,_tmp3FB + 5},(void*)16}};struct _tagged_arr _tmp3E5=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp3E5,sizeof(char))> 4?*((const char*)_check_unknown_subscript(_tmp3E5,sizeof(
char),0))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E5,sizeof(char),1))
== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E5,sizeof(char),(int)(
_get_arr_size(_tmp3E5,sizeof(char))- 2)))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp3E5,sizeof(char),(int)(_get_arr_size(_tmp3E5,
sizeof(char))- 3)))== '_': 0)_tmp3E5=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp3E5,2,_get_arr_size(_tmp3E5,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp3E5,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3E6=_cycalloc(sizeof(*
_tmp3E6));_tmp3E6[0]=({struct Cyc_Attribute_tok_struct _tmp3E7;_tmp3E7.tag=Cyc_Attribute_tok;
_tmp3E7.f1=(void*)(att_map[i]).f2;_tmp3E7;});_tmp3E6;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp3E8="unrecognized attribute";_tag_arr(_tmp3E8,sizeof(char),
_get_zero_arr_size(_tmp3E8,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=({
struct Cyc_Attribute_tok_struct _tmp3EA;_tmp3EA.tag=Cyc_Attribute_tok;_tmp3EA.f1=(
void*)((void*)1);_tmp3EA;});_tmp3E9;});}break;}}case 51: _LL20D: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD[0]=({
struct Cyc_Attribute_tok_struct _tmp3FE;_tmp3FE.tag=Cyc_Attribute_tok;_tmp3FE.f1=(
void*)((void*)4);_tmp3FE;});_tmp3FD;});break;case 52: _LL20E: {struct _tagged_arr
_tmp3FF=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp401;int _tmp402;struct _tuple13*_tmp400=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp401=*_tmp400;
_tmp402=_tmp401.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({const char*
_tmp403="regparm";_tag_arr(_tmp403,sizeof(char),_get_zero_arr_size(_tmp403,8));}))
== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({const char*_tmp404="__regparm__";
_tag_arr(_tmp404,sizeof(char),_get_zero_arr_size(_tmp404,12));}))== 0){if(
_tmp402 < 0?1: _tmp402 > 3)Cyc_Parse_err(({const char*_tmp405="regparm requires value between 0 and 3";
_tag_arr(_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp406=_cycalloc_atomic(sizeof(*_tmp406));
_tmp406[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp407;_tmp407.tag=0;_tmp407.f1=
_tmp402;_tmp407;});_tmp406;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({
const char*_tmp408="aligned";_tag_arr(_tmp408,sizeof(char),_get_zero_arr_size(
_tmp408,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({const char*_tmp409="__aligned__";
_tag_arr(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,12));}))== 0){if(
_tmp402 < 0)Cyc_Parse_err(({const char*_tmp40A="aligned requires positive power of two";
_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(_tmp40A,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp402;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp40B="aligned requires positive power of two";
_tag_arr(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp40C=_cycalloc_atomic(sizeof(*_tmp40C));
_tmp40C[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp40D;_tmp40D.tag=1;_tmp40D.f1=
_tmp402;_tmp40D;});_tmp40C;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({
const char*_tmp40E="initializes";_tag_arr(_tmp40E,sizeof(char),_get_zero_arr_size(
_tmp40E,12));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3FF,({const char*
_tmp40F="__initializes__";_tag_arr(_tmp40F,sizeof(char),_get_zero_arr_size(
_tmp40F,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp410=
_cycalloc_atomic(sizeof(*_tmp410));_tmp410[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp411;_tmp411.tag=4;_tmp411.f1=_tmp402;_tmp411;});_tmp410;});else{Cyc_Parse_err(({
const char*_tmp412="unrecognized attribute";_tag_arr(_tmp412,sizeof(char),
_get_zero_arr_size(_tmp412,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp413=_cycalloc(sizeof(*_tmp413));
_tmp413[0]=({struct Cyc_Attribute_tok_struct _tmp414;_tmp414.tag=Cyc_Attribute_tok;
_tmp414.f1=(void*)a;_tmp414;});_tmp413;});break;}}case 53: _LL20F: {struct
_tagged_arr _tmp415=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp416=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp415,({const char*_tmp417="section";_tag_arr(_tmp417,sizeof(
char),_get_zero_arr_size(_tmp417,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)
_tmp415,({const char*_tmp418="__section__";_tag_arr(_tmp418,sizeof(char),
_get_zero_arr_size(_tmp418,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_Section_att_struct
_tmp41A;_tmp41A.tag=2;_tmp41A.f1=_tmp416;_tmp41A;});_tmp419;});else{Cyc_Parse_err(({
const char*_tmp41B="unrecognized attribute";_tag_arr(_tmp41B,sizeof(char),
_get_zero_arr_size(_tmp41B,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));
_tmp41C[0]=({struct Cyc_Attribute_tok_struct _tmp41D;_tmp41D.tag=Cyc_Attribute_tok;
_tmp41D.f1=(void*)a;_tmp41D;});_tmp41C;});break;}case 54: _LL210: {struct
_tagged_arr _tmp41E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp41F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp41E,({const char*_tmp420="__mode__";_tag_arr(_tmp420,
sizeof(char),_get_zero_arr_size(_tmp420,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp422;_tmp422.tag=5;_tmp422.f1=_tmp41F;_tmp422;});_tmp421;});else{Cyc_Parse_err(({
const char*_tmp423="unrecognized attribute";_tag_arr(_tmp423,sizeof(char),
_get_zero_arr_size(_tmp423,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp424=_cycalloc(sizeof(*_tmp424));
_tmp424[0]=({struct Cyc_Attribute_tok_struct _tmp425;_tmp425.tag=Cyc_Attribute_tok;
_tmp425.f1=(void*)a;_tmp425;});_tmp424;});break;}case 55: _LL211: {struct
_tagged_arr _tmp426=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp427=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp429;struct
_tuple13 _tmp428=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp429=_tmp428.f2;{int _tmp42B;struct _tuple13 _tmp42A=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp42B=_tmp42A.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp426,({const char*_tmp42C="format";
_tag_arr(_tmp42C,sizeof(char),_get_zero_arr_size(_tmp42C,7));}))== 0?1: Cyc_zstrcmp((
struct _tagged_arr)_tmp426,({const char*_tmp42D="__format__";_tag_arr(_tmp42D,
sizeof(char),_get_zero_arr_size(_tmp42D,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp427,({const char*_tmp42E="printf";_tag_arr(_tmp42E,sizeof(char),
_get_zero_arr_size(_tmp42E,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Absyn_Format_att_struct
_tmp430;_tmp430.tag=3;_tmp430.f1=(void*)((void*)0);_tmp430.f2=_tmp429;_tmp430.f3=
_tmp42B;_tmp430;});_tmp42F;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp427,({
const char*_tmp431="scanf";_tag_arr(_tmp431,sizeof(char),_get_zero_arr_size(
_tmp431,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp432=
_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_Absyn_Format_att_struct
_tmp433;_tmp433.tag=3;_tmp433.f1=(void*)((void*)1);_tmp433.f2=_tmp429;_tmp433.f3=
_tmp42B;_tmp433;});_tmp432;});else{Cyc_Parse_err(({const char*_tmp434="unrecognized format type";
_tag_arr(_tmp434,sizeof(char),_get_zero_arr_size(_tmp434,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp435="unrecognized attribute";_tag_arr(_tmp435,sizeof(char),
_get_zero_arr_size(_tmp435,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({
struct Cyc_Attribute_tok_struct _tmp437;_tmp437.tag=Cyc_Attribute_tok;_tmp437.f1=(
void*)a;_tmp437;});_tmp436;});break;}}}case 56: _LL212: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp439;_tmp439.tag=Cyc_TypeSpecifier_tok;_tmp439.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp439;});_tmp438;});break;case 57: _LL213: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));
_tmp43A[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp43B;_tmp43B.tag=Cyc_TypeSpecifier_tok;
_tmp43B.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43B;});
_tmp43A;});break;case 58: _LL214: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43D;_tmp43D.tag=Cyc_TypeSpecifier_tok;_tmp43D.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp43E=_cycalloc(sizeof(*_tmp43E));
_tmp43E->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp43E;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43D;});
_tmp43C;});break;case 59: _LL215: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp440;_tmp440.tag=Cyc_TypeSpecifier_tok;_tmp440.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp440;});
_tmp43F;});break;case 60: _LL216: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp442;_tmp442.tag=Cyc_TypeSpecifier_tok;_tmp442.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_Parse_Short_spec_struct
_tmp444;_tmp444.tag=2;_tmp444.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp444;});
_tmp443;}));_tmp442;});_tmp441;});break;case 61: _LL217: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp446;_tmp446.tag=Cyc_TypeSpecifier_tok;_tmp446.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp446;});
_tmp445;});break;case 62: _LL218: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp448;_tmp448.tag=Cyc_TypeSpecifier_tok;_tmp448.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449[0]=({struct Cyc_Parse_Long_spec_struct
_tmp44A;_tmp44A.tag=3;_tmp44A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp44A;});
_tmp449;}));_tmp448;});_tmp447;});break;case 63: _LL219: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44C;_tmp44C.tag=Cyc_TypeSpecifier_tok;_tmp44C.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44C;});
_tmp44B;});break;case 64: _LL21A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44E;_tmp44E.tag=Cyc_TypeSpecifier_tok;_tmp44E.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44E;});
_tmp44D;});break;case 65: _LL21B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp450;_tmp450.tag=Cyc_TypeSpecifier_tok;_tmp450.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp452;_tmp452.tag=0;_tmp452.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp452;});
_tmp451;}));_tmp450;});_tmp44F;});break;case 66: _LL21C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp454;_tmp454.tag=Cyc_TypeSpecifier_tok;_tmp454.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp456;_tmp456.tag=1;_tmp456.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp456;});
_tmp455;}));_tmp454;});_tmp453;});break;case 67: _LL21D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68: _LL21E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL21F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 70:
_LL220: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp457=_cycalloc(
sizeof(*_tmp457));_tmp457[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp458;_tmp458.tag=
Cyc_TypeSpecifier_tok;_tmp458.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp458;});
_tmp457;});break;case 71: _LL221: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45A;_tmp45A.tag=Cyc_TypeSpecifier_tok;_tmp45A.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));
_tmp45B[0]=({struct Cyc_Absyn_TypedefType_struct _tmp45C;_tmp45C.tag=16;_tmp45C.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp45C.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp45C.f3=0;_tmp45C.f4=0;_tmp45C;});_tmp45B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45A;});
_tmp459;});break;case 72: _LL222: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45E;_tmp45E.tag=Cyc_TypeSpecifier_tok;_tmp45E.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));
_tmp45F[0]=({struct Cyc_Absyn_TupleType_struct _tmp460;_tmp460.tag=9;_tmp460.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp460;});_tmp45F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp45E;});_tmp45D;});break;case 73: _LL223: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp462;_tmp462.tag=Cyc_TypeSpecifier_tok;
_tmp462.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp464;_tmp464.tag=15;_tmp464.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp464;});_tmp463;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp462;});_tmp461;});break;case 74: _LL224: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp465=_cycalloc(sizeof(*_tmp465));
_tmp465[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp466;_tmp466.tag=Cyc_TypeSpecifier_tok;
_tmp466.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp468;_tmp468.tag=14;_tmp468.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp468;});_tmp467;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp466;});_tmp465;});break;case 75: _LL225: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp469=_cycalloc(sizeof(*_tmp469));
_tmp469[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp46A;_tmp46A.tag=Cyc_TypeSpecifier_tok;
_tmp46A.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_Absyn_TagType_struct
_tmp46C;_tmp46C.tag=17;_tmp46C.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46C;});_tmp46B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46A;});_tmp469;});break;case 76: _LL226: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp46E;_tmp46E.tag=Cyc_TypeSpecifier_tok;
_tmp46E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_TagType_struct
_tmp470;_tmp470.tag=17;_tmp470.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->v=(void*)((void*)5);_tmp471;}),0);
_tmp470;});_tmp46F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46E;});
_tmp46D;});break;case 77: _LL227: {struct _tuple13 _tmp473;int _tmp474;struct _tuple13*
_tmp472=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp473=*_tmp472;_tmp474=_tmp473.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp476;_tmp476.tag=Cyc_TypeSpecifier_tok;_tmp476.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp477=_cycalloc_atomic(sizeof(*_tmp477));
_tmp477[0]=({struct Cyc_Absyn_TypeInt_struct _tmp478;_tmp478.tag=18;_tmp478.f1=
_tmp474;_tmp478;});_tmp477;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp476;});
_tmp475;});break;}case 78: _LL228: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp479=
_cycalloc(sizeof(*_tmp479));_tmp479[0]=({struct Cyc_Kind_tok_struct _tmp47A;
_tmp47A.tag=Cyc_Kind_tok;_tmp47A.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47A;});
_tmp479;});break;case 79: _LL229: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp47B=_cycalloc_atomic(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_TypeQual_tok_struct
_tmp47C;_tmp47C.tag=Cyc_TypeQual_tok;_tmp47C.f1=({struct Cyc_Absyn_Tqual _tmp47D;
_tmp47D.print_const=1;_tmp47D.q_volatile=0;_tmp47D.q_restrict=0;_tmp47D.real_const=
1;_tmp47D;});_tmp47C;});_tmp47B;});break;case 80: _LL22A: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp47E=_cycalloc_atomic(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_TypeQual_tok_struct
_tmp47F;_tmp47F.tag=Cyc_TypeQual_tok;_tmp47F.f1=({struct Cyc_Absyn_Tqual _tmp480;
_tmp480.print_const=0;_tmp480.q_volatile=1;_tmp480.q_restrict=0;_tmp480.real_const=
0;_tmp480;});_tmp47F;});_tmp47E;});break;case 81: _LL22B: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp481=_cycalloc_atomic(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_TypeQual_tok_struct
_tmp482;_tmp482.tag=Cyc_TypeQual_tok;_tmp482.f1=({struct Cyc_Absyn_Tqual _tmp483;
_tmp483.print_const=0;_tmp483.q_volatile=0;_tmp483.q_restrict=1;_tmp483.real_const=
0;_tmp483;});_tmp482;});_tmp481;});break;case 82: _LL22C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp485;_tmp485.tag=Cyc_TypeSpecifier_tok;_tmp485.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp487;_tmp487.tag=5;_tmp487.f1=({struct Cyc_Absyn_Decl*_tmp488=_cycalloc(
sizeof(*_tmp488));_tmp488->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp48A;_tmp48A.tag=6;_tmp48A.f1=({struct Cyc_Absyn_Enumdecl*_tmp48B=_cycalloc(
sizeof(*_tmp48B));_tmp48B->sc=(void*)((void*)2);_tmp48B->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp48B->fields=({
struct Cyc_Core_Opt*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48C;});_tmp48B;});
_tmp48A;});_tmp489;}));_tmp488->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp488;});
_tmp487;});_tmp486;}));_tmp485;});_tmp484;});break;case 83: _LL22D: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp48E;_tmp48E.tag=Cyc_TypeSpecifier_tok;
_tmp48E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F[0]=({struct Cyc_Absyn_EnumType_struct
_tmp490;_tmp490.tag=12;_tmp490.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp490.f2=0;_tmp490;});
_tmp48F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48E;});_tmp48D;});break;case 84: _LL22E: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp491=_cycalloc(sizeof(*_tmp491));
_tmp491[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp492;_tmp492.tag=Cyc_TypeSpecifier_tok;
_tmp492.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp493=_cycalloc(
sizeof(*_tmp493));_tmp493[0]=({struct Cyc_Parse_Type_spec_struct _tmp494;_tmp494.tag=
4;_tmp494.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp495=
_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp496;_tmp496.tag=13;_tmp496.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp496;});_tmp495;}));
_tmp494.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp494;});_tmp493;}));_tmp492;});_tmp491;});break;
case 85: _LL22F: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp497=_cycalloc(
sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Enumfield_tok_struct _tmp498;_tmp498.tag=
Cyc_Enumfield_tok;_tmp498.f1=({struct Cyc_Absyn_Enumfield*_tmp499=_cycalloc(
sizeof(*_tmp499));_tmp499->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp499->tag=0;_tmp499->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp499;});_tmp498;});_tmp497;});break;case 86: _LL230:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));
_tmp49A[0]=({struct Cyc_Enumfield_tok_struct _tmp49B;_tmp49B.tag=Cyc_Enumfield_tok;
_tmp49B.f1=({struct Cyc_Absyn_Enumfield*_tmp49C=_cycalloc(sizeof(*_tmp49C));
_tmp49C->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp49C->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49C;});
_tmp49B;});_tmp49A;});break;case 87: _LL231: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp49E;_tmp49E.tag=Cyc_EnumfieldList_tok;_tmp49E.f1=({struct Cyc_List_List*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49F->tl=0;_tmp49F;});
_tmp49E;});_tmp49D;});break;case 88: _LL232: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4A1;_tmp4A1.tag=Cyc_EnumfieldList_tok;_tmp4A1.f1=({struct Cyc_List_List*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A2->tl=0;_tmp4A2;});
_tmp4A1;});_tmp4A0;});break;case 89: _LL233: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4A4;_tmp4A4.tag=Cyc_EnumfieldList_tok;_tmp4A4.f1=({struct Cyc_List_List*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A5->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A5;});_tmp4A4;});
_tmp4A3;});break;case 90: _LL234: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A7;_tmp4A7.tag=Cyc_TypeSpecifier_tok;_tmp4A7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));
_tmp4A8[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4A9;_tmp4A9.tag=11;_tmp4A9.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4A9.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A9;});_tmp4A8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4A7;});_tmp4A6;});break;case 91: _LL235: {struct
Cyc_List_List*_tmp4AA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4AB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4AD;_tmp4AD.tag=Cyc_TypeSpecifier_tok;
_tmp4AD.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4AE=_cycalloc(
sizeof(*_tmp4AE));_tmp4AE[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4AF;_tmp4AF.tag=
5;_tmp4AF.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4AA,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4AB,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AF;});
_tmp4AE;}));_tmp4AD;});_tmp4AC;});break;}case 92: _LL236: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4B1;_tmp4B1.tag=Cyc_TypeSpecifier_tok;_tmp4B1.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));
_tmp4B2[0]=({struct Cyc_Absyn_AggrType_struct _tmp4B3;_tmp4B3.tag=10;_tmp4B3.f1=({
struct Cyc_Absyn_AggrInfo _tmp4B4;_tmp4B4.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4B6;_tmp4B6.tag=0;_tmp4B6.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B6.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B6;});_tmp4B5;}));
_tmp4B4.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4B4;});_tmp4B3;});_tmp4B2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B1;});
_tmp4B0;});break;case 93: _LL237: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_TypeList_tok_struct
_tmp4B8;_tmp4B8.tag=Cyc_TypeList_tok;_tmp4B8.f1=0;_tmp4B8;});_tmp4B7;});break;
case 94: _LL238: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp4B9=_cycalloc(
sizeof(*_tmp4B9));_tmp4B9[0]=({struct Cyc_TypeList_tok_struct _tmp4BA;_tmp4BA.tag=
Cyc_TypeList_tok;_tmp4BA.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4BA;});_tmp4B9;});
break;case 95: _LL239: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp4BB=
_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4BC;_tmp4BC.tag=Cyc_StructOrUnion_tok;_tmp4BC.f1=(void*)((void*)0);_tmp4BC;});
_tmp4BB;});break;case 96: _LL23A: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4BE;_tmp4BE.tag=Cyc_StructOrUnion_tok;_tmp4BE.f1=(void*)((void*)1);_tmp4BE;});
_tmp4BD;});break;case 97: _LL23B: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4C0;_tmp4C0.tag=Cyc_AggrFieldDeclList_tok;_tmp4C0.f1=0;_tmp4C0;});_tmp4BF;});
break;case 98: _LL23C: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4C2;_tmp4C2.tag=Cyc_AggrFieldDeclList_tok;_tmp4C2.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4C2;});_tmp4C1;});
break;case 99: _LL23D: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4C4;_tmp4C4.tag=Cyc_AggrFieldDeclListList_tok;_tmp4C4.f1=({struct Cyc_List_List*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C5->tl=0;_tmp4C5;});
_tmp4C4;});_tmp4C3;});break;case 100: _LL23E: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4C7;_tmp4C7.tag=Cyc_AggrFieldDeclListList_tok;_tmp4C7.f1=({struct Cyc_List_List*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C8->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C8;});_tmp4C7;});
_tmp4C6;});break;case 101: _LL23F: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CA;_tmp4CA.tag=Cyc_InitDeclList_tok;_tmp4CA.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4CA;});_tmp4C9;});
break;case 102: _LL240: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4CB=
_cycalloc(sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CC;_tmp4CC.tag=Cyc_InitDeclList_tok;_tmp4CC.f1=({struct Cyc_List_List*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CD->tl=0;_tmp4CD;});
_tmp4CC;});_tmp4CB;});break;case 103: _LL241: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CF;_tmp4CF.tag=Cyc_InitDeclList_tok;_tmp4CF.f1=({struct Cyc_List_List*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D0->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D0;});_tmp4CF;});
_tmp4CE;});break;case 104: _LL242: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D2;_tmp4D2.tag=Cyc_InitDecl_tok;_tmp4D2.f1=({struct _tuple12*_tmp4D3=
_cycalloc(sizeof(*_tmp4D3));_tmp4D3->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D3->f2=0;_tmp4D3;});
_tmp4D2;});_tmp4D1;});break;case 105: _LL243: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D5;_tmp4D5.tag=Cyc_InitDecl_tok;_tmp4D5.f1=({struct _tuple12*_tmp4D6=
_cycalloc(sizeof(*_tmp4D6));_tmp4D6->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D6->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4D6;});_tmp4D5;});_tmp4D4;});break;case 106: _LL244: {struct _tuple15 _tmp4D8;
struct Cyc_Absyn_Tqual _tmp4D9;struct Cyc_List_List*_tmp4DA;struct Cyc_List_List*
_tmp4DB;struct _tuple15*_tmp4D7=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D8=*_tmp4D7;_tmp4D9=
_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;_tmp4DB=_tmp4D8.f3;{struct Cyc_List_List*_tmp4DD;
struct Cyc_List_List*_tmp4DE;struct _tuple0 _tmp4DC=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4DD=_tmp4DC.f1;
_tmp4DE=_tmp4DC.f2;{void*_tmp4DF=Cyc_Parse_speclist2typ(_tmp4DA,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4D9,_tmp4DF,_tmp4DD,_tmp4DB),_tmp4DE);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4E1=_cycalloc(sizeof(*
_tmp4E1));_tmp4E1[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp4E2;_tmp4E2.tag=
Cyc_AggrFieldDeclList_tok;_tmp4E2.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4E0);
_tmp4E2;});_tmp4E1;});break;}}}case 107: _LL245: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4E4;_tmp4E4.tag=Cyc_QualSpecList_tok;_tmp4E4.f1=({struct _tuple15*_tmp4E5=
_cycalloc(sizeof(*_tmp4E5));_tmp4E5->f1=Cyc_Absyn_empty_tqual();_tmp4E5->f2=({
struct Cyc_List_List*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E6->tl=0;_tmp4E6;});
_tmp4E5->f3=0;_tmp4E5;});_tmp4E4;});_tmp4E3;});break;case 108: _LL246: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({
struct Cyc_QualSpecList_tok_struct _tmp4E8;_tmp4E8.tag=Cyc_QualSpecList_tok;
_tmp4E8.f1=({struct _tuple15*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4E9->f2=({struct Cyc_List_List*_tmp4EA=_cycalloc(sizeof(*
_tmp4EA));_tmp4EA->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EA->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4EA;});_tmp4E9->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp4E9;});_tmp4E8;});_tmp4E7;});break;case 109: _LL247: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));
_tmp4EB[0]=({struct Cyc_QualSpecList_tok_struct _tmp4EC;_tmp4EC.tag=Cyc_QualSpecList_tok;
_tmp4EC.f1=({struct _tuple15*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4ED->f2=0;_tmp4ED->f3=
0;_tmp4ED;});_tmp4EC;});_tmp4EB;});break;case 110: _LL248: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4EF;_tmp4EF.tag=Cyc_QualSpecList_tok;_tmp4EF.f1=({struct _tuple15*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp4F0->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F0->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4F0;});_tmp4EF;});
_tmp4EE;});break;case 111: _LL249: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4F2;_tmp4F2.tag=Cyc_QualSpecList_tok;_tmp4F2.f1=({struct _tuple15*_tmp4F3=
_cycalloc(sizeof(*_tmp4F3));_tmp4F3->f1=Cyc_Absyn_empty_tqual();_tmp4F3->f2=0;
_tmp4F3->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4F3;});_tmp4F2;});_tmp4F1;});break;case 112: _LL24A:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp4F4=_cycalloc(sizeof(*
_tmp4F4));_tmp4F4[0]=({struct Cyc_QualSpecList_tok_struct _tmp4F5;_tmp4F5.tag=Cyc_QualSpecList_tok;
_tmp4F5.f1=({struct _tuple15*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4F6->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F6->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp4F6;});_tmp4F5;});
_tmp4F4;});break;case 113: _LL24B: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4F8;_tmp4F8.tag=Cyc_InitDeclList_tok;_tmp4F8.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4F8;});_tmp4F7;});
break;case 114: _LL24C: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4F9=
_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4FA;_tmp4FA.tag=Cyc_InitDeclList_tok;_tmp4FA.f1=({struct Cyc_List_List*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB->tl=0;_tmp4FB;});
_tmp4FA;});_tmp4F9;});break;case 115: _LL24D: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4FD;_tmp4FD.tag=Cyc_InitDeclList_tok;_tmp4FD.f1=({struct Cyc_List_List*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FE->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FE;});_tmp4FD;});
_tmp4FC;});break;case 116: _LL24E: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_InitDecl_tok_struct
_tmp500;_tmp500.tag=Cyc_InitDecl_tok;_tmp500.f1=({struct _tuple12*_tmp501=
_cycalloc(sizeof(*_tmp501));_tmp501->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp501->f2=0;_tmp501;});
_tmp500;});_tmp4FF;});break;case 117: _LL24F: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_InitDecl_tok_struct
_tmp503;_tmp503.tag=Cyc_InitDecl_tok;_tmp503.f1=({struct _tuple12*_tmp504=
_cycalloc(sizeof(*_tmp504));_tmp504->f1=({struct Cyc_Parse_Declarator*_tmp505=
_cycalloc(sizeof(*_tmp505));_tmp505->id=({struct _tuple1*_tmp506=_cycalloc(
sizeof(*_tmp506));_tmp506->f1=Cyc_Absyn_rel_ns_null;_tmp506->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp506;});_tmp505->tms=
0;_tmp505;});_tmp504->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp504;});_tmp503;});
_tmp502;});break;case 118: _LL250: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_InitDecl_tok_struct
_tmp509;_tmp509.tag=Cyc_InitDecl_tok;_tmp509.f1=({struct _tuple12*_tmp50A=
_cycalloc(sizeof(*_tmp50A));_tmp50A->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp50A->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp50A;});_tmp509;});_tmp508;});break;case 119: _LL251: {struct Cyc_List_List*
_tmp50B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp50D;_tmp50D.tag=Cyc_TypeSpecifier_tok;
_tmp50D.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp50E=_cycalloc(
sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_Parse_Decl_spec_struct _tmp50F;_tmp50F.tag=
5;_tmp50F.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp50B,({struct Cyc_Core_Opt*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp510;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50F;});
_tmp50E;}));_tmp50D;});_tmp50C;});break;}case 120: _LL252: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp512;_tmp512.tag=Cyc_TypeSpecifier_tok;_tmp512.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp513=_cycalloc(sizeof(*_tmp513));
_tmp513[0]=({struct Cyc_Absyn_TunionType_struct _tmp514;_tmp514.tag=2;_tmp514.f1=({
struct Cyc_Absyn_TunionInfo _tmp515;_tmp515.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp517;_tmp517.tag=0;_tmp517.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp518;
_tmp518.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp518.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp518;});_tmp517;});
_tmp516;}));_tmp515.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp515.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp515;});_tmp514;});
_tmp513;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp512;});_tmp511;});break;case 121: _LL253: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp519=_cycalloc(sizeof(*_tmp519));
_tmp519[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp51A;_tmp51A.tag=Cyc_TypeSpecifier_tok;
_tmp51A.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp51C;_tmp51C.tag=3;_tmp51C.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp51D;
_tmp51D.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp51F;_tmp51F.tag=0;_tmp51F.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp520;
_tmp520.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp520.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp520.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp520;});_tmp51F;});
_tmp51E;}));_tmp51D.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51D;});_tmp51C;});
_tmp51B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp51A;});_tmp519;});break;case 122: _LL254: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp521=_cycalloc_atomic(sizeof(*_tmp521));
_tmp521[0]=({struct Cyc_Bool_tok_struct _tmp522;_tmp522.tag=Cyc_Bool_tok;_tmp522.f1=
0;_tmp522;});_tmp521;});break;case 123: _LL255: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp523=_cycalloc_atomic(sizeof(*_tmp523));_tmp523[0]=({struct Cyc_Bool_tok_struct
_tmp524;_tmp524.tag=Cyc_Bool_tok;_tmp524.f1=1;_tmp524;});_tmp523;});break;case
124: _LL256: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp525=_cycalloc(
sizeof(*_tmp525));_tmp525[0]=({struct Cyc_TunionFieldList_tok_struct _tmp526;
_tmp526.tag=Cyc_TunionFieldList_tok;_tmp526.f1=({struct Cyc_List_List*_tmp527=
_cycalloc(sizeof(*_tmp527));_tmp527->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp527->tl=0;_tmp527;});
_tmp526;});_tmp525;});break;case 125: _LL257: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp529;_tmp529.tag=Cyc_TunionFieldList_tok;_tmp529.f1=({struct Cyc_List_List*
_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52A->tl=0;_tmp52A;});
_tmp529;});_tmp528;});break;case 126: _LL258: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp52C;_tmp52C.tag=Cyc_TunionFieldList_tok;_tmp52C.f1=({struct Cyc_List_List*
_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52D->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52D;});_tmp52C;});
_tmp52B;});break;case 127: _LL259: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp52F;_tmp52F.tag=Cyc_TunionFieldList_tok;_tmp52F.f1=({struct Cyc_List_List*
_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp530->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp530;});_tmp52F;});
_tmp52E;});break;case 128: _LL25A: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531[0]=({struct Cyc_Scope_tok_struct
_tmp532;_tmp532.tag=Cyc_Scope_tok;_tmp532.f1=(void*)((void*)2);_tmp532;});
_tmp531;});break;case 129: _LL25B: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533[0]=({struct Cyc_Scope_tok_struct
_tmp534;_tmp534.tag=Cyc_Scope_tok;_tmp534.f1=(void*)((void*)3);_tmp534;});
_tmp533;});break;case 130: _LL25C: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535[0]=({struct Cyc_Scope_tok_struct
_tmp536;_tmp536.tag=Cyc_Scope_tok;_tmp536.f1=(void*)((void*)0);_tmp536;});
_tmp535;});break;case 131: _LL25D: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({struct Cyc_TunionField_tok_struct
_tmp538;_tmp538.tag=Cyc_TunionField_tok;_tmp538.f1=({struct Cyc_Absyn_Tunionfield*
_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp539->typs=0;_tmp539->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp539->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp539;});_tmp538;});
_tmp537;});break;case 132: _LL25E: {struct Cyc_List_List*_tmp53A=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({
struct Cyc_TunionField_tok_struct _tmp53C;_tmp53C.tag=Cyc_TunionField_tok;_tmp53C.f1=({
struct Cyc_Absyn_Tunionfield*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp53D->typs=_tmp53A;_tmp53D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp53D->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp53D;});_tmp53C;});_tmp53B;});break;}case 133: _LL25F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 134: _LL260: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));
_tmp53E[0]=({struct Cyc_Declarator_tok_struct _tmp53F;_tmp53F.tag=Cyc_Declarator_tok;
_tmp53F.f1=({struct Cyc_Parse_Declarator*_tmp540=_cycalloc(sizeof(*_tmp540));
_tmp540->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp540->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp540;});
_tmp53F;});_tmp53E;});break;case 135: _LL261: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541[0]=({struct Cyc_Declarator_tok_struct
_tmp542;_tmp542.tag=Cyc_Declarator_tok;_tmp542.f1=({struct Cyc_Parse_Declarator*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp543->tms=0;_tmp543;});
_tmp542;});_tmp541;});break;case 136: _LL262: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 137: _LL263: {
struct Cyc_Parse_Declarator*_tmp544=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp544->tms=({struct Cyc_List_List*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp547;_tmp547.tag=5;_tmp547.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp547.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp547;});_tmp546;}));_tmp545->tl=_tmp544->tms;_tmp545;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 138:
_LL264: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp548=_cycalloc(sizeof(*
_tmp548));_tmp548[0]=({struct Cyc_Declarator_tok_struct _tmp549;_tmp549.tag=Cyc_Declarator_tok;
_tmp549.f1=({struct Cyc_Parse_Declarator*_tmp54A=_cycalloc(sizeof(*_tmp54A));
_tmp54A->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp54A->tms=({struct Cyc_List_List*_tmp54B=_cycalloc(
sizeof(*_tmp54B));_tmp54B->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp54D;_tmp54D.tag=0;_tmp54D.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54D;});_tmp54C;}));
_tmp54B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp54B;});_tmp54A;});_tmp549;});_tmp548;});break;case
139: _LL265: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp54E=_cycalloc(
sizeof(*_tmp54E));_tmp54E[0]=({struct Cyc_Declarator_tok_struct _tmp54F;_tmp54F.tag=
Cyc_Declarator_tok;_tmp54F.f1=({struct Cyc_Parse_Declarator*_tmp550=_cycalloc(
sizeof(*_tmp550));_tmp550->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp550->tms=({
struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp553;_tmp553.tag=1;_tmp553.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp553.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp553;});_tmp552;}));_tmp551->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp551;});_tmp550;});
_tmp54F;});_tmp54E;});break;case 140: _LL266: {struct _tuple14 _tmp555;struct Cyc_List_List*
_tmp556;int _tmp557;struct Cyc_Absyn_VarargInfo*_tmp558;struct Cyc_Core_Opt*_tmp559;
struct Cyc_List_List*_tmp55A;struct _tuple14*_tmp554=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp555=*_tmp554;_tmp556=
_tmp555.f1;_tmp557=_tmp555.f2;_tmp558=_tmp555.f3;_tmp559=_tmp555.f4;_tmp55A=
_tmp555.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp55B=_cycalloc(
sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Declarator_tok_struct _tmp55C;_tmp55C.tag=
Cyc_Declarator_tok;_tmp55C.f1=({struct Cyc_Parse_Declarator*_tmp55D=_cycalloc(
sizeof(*_tmp55D));_tmp55D->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp55D->tms=({
struct Cyc_List_List*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F[0]=({struct Cyc_Absyn_Function_mod_struct _tmp560;_tmp560.tag=3;_tmp560.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp561=_cycalloc(sizeof(*
_tmp561));_tmp561[0]=({struct Cyc_Absyn_WithTypes_struct _tmp562;_tmp562.tag=1;
_tmp562.f1=_tmp556;_tmp562.f2=_tmp557;_tmp562.f3=_tmp558;_tmp562.f4=_tmp559;
_tmp562.f5=_tmp55A;_tmp562;});_tmp561;}));_tmp560;});_tmp55F;}));_tmp55E->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp55E;});_tmp55D;});_tmp55C;});_tmp55B;});break;}case 141: _LL267:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp563=_cycalloc(sizeof(*_tmp563));
_tmp563[0]=({struct Cyc_Declarator_tok_struct _tmp564;_tmp564.tag=Cyc_Declarator_tok;
_tmp564.f1=({struct Cyc_Parse_Declarator*_tmp565=_cycalloc(sizeof(*_tmp565));
_tmp565->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp565->tms=({struct Cyc_List_List*_tmp566=_cycalloc(
sizeof(*_tmp566));_tmp566->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp568;_tmp568.tag=3;_tmp568.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp56A;_tmp56A.tag=1;_tmp56A.f1=0;_tmp56A.f2=0;_tmp56A.f3=0;_tmp56A.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56A.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56A;});_tmp569;}));
_tmp568;});_tmp567;}));_tmp566->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp566;});_tmp565;});
_tmp564;});_tmp563;});break;case 142: _LL268: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B[0]=({struct Cyc_Declarator_tok_struct
_tmp56C;_tmp56C.tag=Cyc_Declarator_tok;_tmp56C.f1=({struct Cyc_Parse_Declarator*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp56D->tms=({
struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F[0]=({struct Cyc_Absyn_Function_mod_struct _tmp570;_tmp570.tag=3;_tmp570.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp571=_cycalloc(sizeof(*_tmp571));
_tmp571[0]=({struct Cyc_Absyn_NoTypes_struct _tmp572;_tmp572.tag=0;_tmp572.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp572.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp572;});
_tmp571;}));_tmp570;});_tmp56F;}));_tmp56E->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp56E;});_tmp56D;});
_tmp56C;});_tmp56B;});break;case 143: _LL269: {struct Cyc_List_List*_tmp573=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({
struct Cyc_Declarator_tok_struct _tmp575;_tmp575.tag=Cyc_Declarator_tok;_tmp575.f1=({
struct Cyc_Parse_Declarator*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp576->tms=({
struct Cyc_List_List*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp578=_cycalloc(sizeof(*_tmp578));
_tmp578[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp579;_tmp579.tag=4;_tmp579.f1=
_tmp573;_tmp579.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp579.f3=0;
_tmp579;});_tmp578;}));_tmp577->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp577;});_tmp576;});
_tmp575;});_tmp574;});break;}case 144: _LL26A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_Declarator_tok_struct
_tmp57B;_tmp57B.tag=Cyc_Declarator_tok;_tmp57B.f1=({struct Cyc_Parse_Declarator*
_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp57C->tms=({
struct Cyc_List_List*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));
_tmp57E[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp57F;_tmp57F.tag=5;_tmp57F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp57F.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57F;});_tmp57E;}));
_tmp57D->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp57D;});_tmp57C;});_tmp57B;});_tmp57A;});break;case
145: _LL26B: Cyc_Parse_err(({const char*_tmp580="identifier has not been declared as a typedef";
_tag_arr(_tmp580,sizeof(char),_get_zero_arr_size(_tmp580,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({
struct Cyc_Declarator_tok_struct _tmp582;_tmp582.tag=Cyc_Declarator_tok;_tmp582.f1=({
struct Cyc_Parse_Declarator*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp583->tms=0;_tmp583;});
_tmp582;});_tmp581;});break;case 146: _LL26C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 147: _LL26D: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_TypeModifierList_tok_struct _tmp585;_tmp585.tag=Cyc_TypeModifierList_tok;
_tmp585.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp585;});_tmp584;});
break;case 148: _LL26E: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp588;_tmp588.tag=5;_tmp588.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp588.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp588;});_tmp587;}));_tmp586->tl=ans;_tmp586;});ans=({
struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=(void*)((
void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));
_tmp58A[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp58B;_tmp58B.tag=2;_tmp58B.f1=({
struct Cyc_Absyn_PtrAtts _tmp58C;_tmp58C.rgn=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp58C.nullable=(*Cyc_yyget_YY1(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp58C.bounds=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f2;
_tmp58C.zero_term=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp58C;});_tmp58B.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58B;});_tmp58A;}));
_tmp589->tl=ans;_tmp589;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp58E;_tmp58E.tag=Cyc_TypeModifierList_tok;_tmp58E.f1=ans;_tmp58E;});_tmp58D;});
break;}case 149: _LL26F: yyval=(void*)({struct Cyc_YY1_struct*_tmp58F=_cycalloc(
sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_YY1_struct _tmp590;_tmp590.tag=Cyc_YY1;
_tmp590.f1=({struct _tuple16*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->f1=Cyc_Absyn_true_conref;
_tmp591->f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp591;});_tmp590;});_tmp58F;});break;case 150: _LL270: yyval=(void*)({struct Cyc_YY1_struct*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_YY1_struct _tmp593;
_tmp593.tag=Cyc_YY1;_tmp593.f1=({struct _tuple16*_tmp594=_cycalloc(sizeof(*
_tmp594));_tmp594->f1=Cyc_Absyn_false_conref;_tmp594->f2=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp594;});_tmp593;});
_tmp592;});break;case 151: _LL271: yyval=(void*)({struct Cyc_YY1_struct*_tmp595=
_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_YY1_struct _tmp596;_tmp596.tag=
Cyc_YY1;_tmp596.f1=({struct _tuple16*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->f1=
Cyc_Absyn_true_conref;_tmp597->f2=Cyc_Absyn_bounds_unknown_conref;_tmp597;});
_tmp596;});_tmp595;});break;case 152: _LL272: yyval=(void*)({struct Cyc_YY2_struct*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_YY2_struct _tmp599;
_tmp599.tag=Cyc_YY2;_tmp599.f1=Cyc_Absyn_bounds_one_conref;_tmp599;});_tmp598;});
break;case 153: _LL273: yyval=(void*)({struct Cyc_YY2_struct*_tmp59A=_cycalloc(
sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_YY2_struct _tmp59B;_tmp59B.tag=Cyc_YY2;
_tmp59B.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp59C=
_cycalloc(sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp59D;
_tmp59D.tag=0;_tmp59D.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp59D;});_tmp59C;}));_tmp59B;});_tmp59A;});break;case
154: _LL274: yyval=(void*)({struct Cyc_YY2_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));
_tmp59E[0]=({struct Cyc_YY2_struct _tmp59F;_tmp59F.tag=Cyc_YY2;_tmp59F.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));
_tmp5A0[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp5A1;_tmp5A1.tag=1;_tmp5A1.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp5A1;});_tmp5A0;}));_tmp59F;});_tmp59E;});break;case 155: _LL275: yyval=(
void*)({struct Cyc_YY4_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({
struct Cyc_YY4_struct _tmp5A3;_tmp5A3.tag=Cyc_YY4;_tmp5A3.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5A3;});_tmp5A2;});break;case 156: _LL276: yyval=(void*)({
struct Cyc_YY4_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4[0]=({struct Cyc_YY4_struct
_tmp5A5;_tmp5A5.tag=Cyc_YY4;_tmp5A5.f1=Cyc_Absyn_true_conref;_tmp5A5;});_tmp5A4;});
break;case 157: _LL277: yyval=(void*)({struct Cyc_YY4_struct*_tmp5A6=_cycalloc(
sizeof(*_tmp5A6));_tmp5A6[0]=({struct Cyc_YY4_struct _tmp5A7;_tmp5A7.tag=Cyc_YY4;
_tmp5A7.f1=Cyc_Absyn_false_conref;_tmp5A7;});_tmp5A6;});break;case 158: _LL278:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));
_tmp5A8[0]=({struct Cyc_Type_tok_struct _tmp5A9;_tmp5A9.tag=Cyc_Type_tok;_tmp5A9.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));
_tmp5AA->v=(void*)((void*)3);_tmp5AA;}),0);_tmp5A9;});_tmp5A8;});break;case 159:
_LL279: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 160: _LL27A: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({
struct Cyc_Type_tok_struct _tmp5AC;_tmp5AC.tag=Cyc_Type_tok;_tmp5AC.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->v=(void*)((void*)
3);_tmp5AD;}),0);_tmp5AC;});_tmp5AB;});break;case 161: _LL27B: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5AE=_cycalloc_atomic(sizeof(*_tmp5AE));_tmp5AE[
0]=({struct Cyc_TypeQual_tok_struct _tmp5AF;_tmp5AF.tag=Cyc_TypeQual_tok;_tmp5AF.f1=
Cyc_Absyn_empty_tqual();_tmp5AF;});_tmp5AE;});break;case 162: _LL27C: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5B0=_cycalloc_atomic(sizeof(*_tmp5B0));_tmp5B0[
0]=({struct Cyc_TypeQual_tok_struct _tmp5B1;_tmp5B1.tag=Cyc_TypeQual_tok;_tmp5B1.f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5B1;});_tmp5B0;});
break;case 163: _LL27D: yyval=(void*)({struct Cyc_YY3_struct*_tmp5B2=_cycalloc(
sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_YY3_struct _tmp5B3;_tmp5B3.tag=Cyc_YY3;
_tmp5B3.f1=({struct _tuple14*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5B4->f2=0;
_tmp5B4->f3=0;_tmp5B4->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B4->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B4;});_tmp5B3;});
_tmp5B2;});break;case 164: _LL27E: yyval=(void*)({struct Cyc_YY3_struct*_tmp5B5=
_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_YY3_struct _tmp5B6;_tmp5B6.tag=
Cyc_YY3;_tmp5B6.f1=({struct _tuple14*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp5B7->f2=1;
_tmp5B7->f3=0;_tmp5B7->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B7->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B7;});_tmp5B6;});
_tmp5B5;});break;case 165: _LL27F: {struct _tuple2 _tmp5B9;struct Cyc_Core_Opt*
_tmp5BA;struct Cyc_Absyn_Tqual _tmp5BB;void*_tmp5BC;struct _tuple2*_tmp5B8=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5B9=*_tmp5B8;
_tmp5BA=_tmp5B9.f1;_tmp5BB=_tmp5B9.f2;_tmp5BC=_tmp5B9.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5BD=({struct Cyc_Absyn_VarargInfo*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->name=
_tmp5BA;_tmp5C1->tq=_tmp5BB;_tmp5C1->type=(void*)_tmp5BC;_tmp5C1->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5C1;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({
struct Cyc_YY3_struct _tmp5BF;_tmp5BF.tag=Cyc_YY3;_tmp5BF.f1=({struct _tuple14*
_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->f1=0;_tmp5C0->f2=0;_tmp5C0->f3=
_tmp5BD;_tmp5C0->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5C0->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C0;});_tmp5BF;});
_tmp5BE;});break;}}case 166: _LL280: {struct _tuple2 _tmp5C3;struct Cyc_Core_Opt*
_tmp5C4;struct Cyc_Absyn_Tqual _tmp5C5;void*_tmp5C6;struct _tuple2*_tmp5C2=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C3=*_tmp5C2;
_tmp5C4=_tmp5C3.f1;_tmp5C5=_tmp5C3.f2;_tmp5C6=_tmp5C3.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5C7=({struct Cyc_Absyn_VarargInfo*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->name=
_tmp5C4;_tmp5CB->tq=_tmp5C5;_tmp5CB->type=(void*)_tmp5C6;_tmp5CB->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5CB;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));_tmp5C8[0]=({
struct Cyc_YY3_struct _tmp5C9;_tmp5C9.tag=Cyc_YY3;_tmp5C9.f1=({struct _tuple14*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5CA->f2=0;_tmp5CA->f3=
_tmp5C7;_tmp5CA->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5CA->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5CA;});_tmp5C9;});
_tmp5C8;});break;}}case 167: _LL281: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Type_tok_struct
_tmp5CD;_tmp5CD.tag=Cyc_Type_tok;_tmp5CD.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5CF;_tmp5CF.tag=1;_tmp5CF.f1=0;_tmp5CF;});_tmp5CE;}));_tmp5CD;});_tmp5CC;});
break;case 168: _LL282: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5D0=_cycalloc(
sizeof(*_tmp5D0));_tmp5D0[0]=({struct Cyc_Type_tok_struct _tmp5D1;_tmp5D1.tag=Cyc_Type_tok;
_tmp5D1.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D3;});_tmp5D2;}));
_tmp5D1;});_tmp5D0;});break;case 169: _LL283: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5D5;_tmp5D5.tag=Cyc_TypeOpt_tok;_tmp5D5.f1=0;_tmp5D5;});_tmp5D4;});break;
case 170: _LL284: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5D6=_cycalloc(
sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_TypeOpt_tok_struct _tmp5D7;_tmp5D7.tag=
Cyc_TypeOpt_tok;_tmp5D7.f1=({struct Cyc_Core_Opt*_tmp5D8=_cycalloc(sizeof(*
_tmp5D8));_tmp5D8->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5D9=
_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5DA;
_tmp5DA.tag=20;_tmp5DA.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DA;});_tmp5D9;}));
_tmp5D8;});_tmp5D7;});_tmp5D6;});break;case 171: _LL285: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5DC;_tmp5DC.tag=Cyc_Rgnorder_tok;_tmp5DC.f1=0;_tmp5DC;});_tmp5DB;});break;
case 172: _LL286: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 173: _LL287: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5DD=
_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_Rgnorder_tok_struct _tmp5DE;
_tmp5DE.tag=Cyc_Rgnorder_tok;_tmp5DE.f1=({struct Cyc_List_List*_tmp5DF=_cycalloc(
sizeof(*_tmp5DF));_tmp5DF->hd=({struct _tuple4*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));
_tmp5E0->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5E3=_cycalloc(sizeof(*
_tmp5E3));_tmp5E3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5E4;_tmp5E4.tag=20;
_tmp5E4.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5E4;});_tmp5E3;});_tmp5E0->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5E2;_tmp5E2.tag=0;_tmp5E2.f1=(void*)((void*)3);_tmp5E2;});_tmp5E1;}));
_tmp5E0;});_tmp5DF->tl=0;_tmp5DF;});_tmp5DE;});_tmp5DD;});break;case 174: _LL288:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));
_tmp5E5[0]=({struct Cyc_Rgnorder_tok_struct _tmp5E6;_tmp5E6.tag=Cyc_Rgnorder_tok;
_tmp5E6.f1=({struct Cyc_List_List*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->hd=({
struct _tuple4*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5EC;_tmp5EC.tag=20;_tmp5EC.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5EC;});_tmp5EB;});
_tmp5E8->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5EA;_tmp5EA.tag=0;_tmp5EA.f1=(void*)((void*)3);_tmp5EA;});_tmp5E9;}));
_tmp5E8;});_tmp5E7->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E7;});_tmp5E6;});
_tmp5E5;});break;case 175: _LL289: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp5ED=
_cycalloc_atomic(sizeof(*_tmp5ED));_tmp5ED[0]=({struct Cyc_Bool_tok_struct _tmp5EE;
_tmp5EE.tag=Cyc_Bool_tok;_tmp5EE.f1=0;_tmp5EE;});_tmp5ED;});break;case 176: _LL28A:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5EF="inject";
_tag_arr(_tmp5EF,sizeof(char),_get_zero_arr_size(_tmp5EF,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5F0="missing type in function declaration";_tag_arr(_tmp5F0,
sizeof(char),_get_zero_arr_size(_tmp5F0,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp5F1=_cycalloc_atomic(sizeof(*_tmp5F1));_tmp5F1[0]=({
struct Cyc_Bool_tok_struct _tmp5F2;_tmp5F2.tag=Cyc_Bool_tok;_tmp5F2.f1=1;_tmp5F2;});
_tmp5F1;});break;case 177: _LL28B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 178: _LL28C: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3[0]=({struct Cyc_TypeList_tok_struct
_tmp5F4;_tmp5F4.tag=Cyc_TypeList_tok;_tmp5F4.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5F4;});_tmp5F3;});
break;case 179: _LL28D: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5F5=
_cycalloc(sizeof(*_tmp5F5));_tmp5F5[0]=({struct Cyc_TypeList_tok_struct _tmp5F6;
_tmp5F6.tag=Cyc_TypeList_tok;_tmp5F6.f1=0;_tmp5F6;});_tmp5F5;});break;case 180:
_LL28E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 181: _LL28F: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5F7=_cycalloc(
sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_TypeList_tok_struct _tmp5F8;_tmp5F8.tag=
Cyc_TypeList_tok;_tmp5F8.f1=({struct Cyc_List_List*_tmp5F9=_cycalloc(sizeof(*
_tmp5F9));_tmp5F9->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5FA=
_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5FB;
_tmp5FB.tag=21;_tmp5FB.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FB;});_tmp5FA;}));
_tmp5F9->tl=0;_tmp5F9;});_tmp5F8;});_tmp5F7;});break;case 182: _LL290: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5FC=_cycalloc(sizeof(*
_tmp5FC));_tmp5FC[0]=({struct Cyc_TypeList_tok_struct _tmp5FD;_tmp5FD.tag=Cyc_TypeList_tok;
_tmp5FD.f1=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5FE->tl=0;_tmp5FE;});_tmp5FD;});_tmp5FC;});break;case 183: _LL291: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5FF=_cycalloc(sizeof(*
_tmp5FF));_tmp5FF[0]=({struct Cyc_TypeList_tok_struct _tmp600;_tmp600.tag=Cyc_TypeList_tok;
_tmp600.f1=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp602=_cycalloc(sizeof(*
_tmp602));_tmp602[0]=({struct Cyc_Absyn_AccessEff_struct _tmp603;_tmp603.tag=19;
_tmp603.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp603;});_tmp602;}));_tmp601->tl=0;_tmp601;});_tmp600;});
_tmp5FF;});break;case 184: _LL292: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp604=_cycalloc(sizeof(*_tmp604));
_tmp604[0]=({struct Cyc_TypeList_tok_struct _tmp605;_tmp605.tag=Cyc_TypeList_tok;
_tmp605.f1=({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp607=_cycalloc(sizeof(*
_tmp607));_tmp607[0]=({struct Cyc_Absyn_AccessEff_struct _tmp608;_tmp608.tag=19;
_tmp608.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp608;});_tmp607;}));_tmp606->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp606;});_tmp605;});
_tmp604;});break;case 185: _LL293: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp60A;_tmp60A.tag=Cyc_ParamDeclList_tok;_tmp60A.f1=({struct Cyc_List_List*
_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60B->tl=0;_tmp60B;});
_tmp60A;});_tmp609;});break;case 186: _LL294: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp60D;_tmp60D.tag=Cyc_ParamDeclList_tok;_tmp60D.f1=({struct Cyc_List_List*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60E->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60E;});_tmp60D;});
_tmp60C;});break;case 187: _LL295: {struct _tuple15 _tmp610;struct Cyc_Absyn_Tqual
_tmp611;struct Cyc_List_List*_tmp612;struct Cyc_List_List*_tmp613;struct _tuple15*
_tmp60F=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp610=*_tmp60F;_tmp611=_tmp610.f1;_tmp612=_tmp610.f2;
_tmp613=_tmp610.f3;{struct Cyc_Parse_Declarator _tmp615;struct _tuple1*_tmp616;
struct Cyc_List_List*_tmp617;struct Cyc_Parse_Declarator*_tmp614=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp615=*_tmp614;
_tmp616=_tmp615.id;_tmp617=_tmp615.tms;{void*_tmp618=Cyc_Parse_speclist2typ(
_tmp612,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp61A;void*_tmp61B;struct
Cyc_List_List*_tmp61C;struct Cyc_List_List*_tmp61D;struct _tuple6 _tmp619=Cyc_Parse_apply_tms(
_tmp611,_tmp618,_tmp613,_tmp617);_tmp61A=_tmp619.f1;_tmp61B=_tmp619.f2;_tmp61C=
_tmp619.f3;_tmp61D=_tmp619.f4;if(_tmp61C != 0)Cyc_Parse_err(({const char*_tmp61E="parameter with bad type params";
_tag_arr(_tmp61E,sizeof(char),_get_zero_arr_size(_tmp61E,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp616))Cyc_Parse_err(({const char*_tmp61F="parameter cannot be qualified with a namespace";
_tag_arr(_tmp61F,sizeof(char),_get_zero_arr_size(_tmp61F,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp620=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp626=_cycalloc(sizeof(*
_tmp626));_tmp626->v=(*_tmp616).f2;_tmp626;});if(_tmp61D != 0)({void*_tmp621[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp622="extra attributes on parameter, ignoring";
_tag_arr(_tmp622,sizeof(char),_get_zero_arr_size(_tmp622,40));}),_tag_arr(
_tmp621,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_ParamDecl_tok_struct
_tmp624;_tmp624.tag=Cyc_ParamDecl_tok;_tmp624.f1=({struct _tuple2*_tmp625=
_cycalloc(sizeof(*_tmp625));_tmp625->f1=_tmp620;_tmp625->f2=_tmp61A;_tmp625->f3=
_tmp61B;_tmp625;});_tmp624;});_tmp623;});break;}}}}case 188: _LL296: {struct
_tuple15 _tmp628;struct Cyc_Absyn_Tqual _tmp629;struct Cyc_List_List*_tmp62A;struct
Cyc_List_List*_tmp62B;struct _tuple15*_tmp627=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp628=*_tmp627;_tmp629=
_tmp628.f1;_tmp62A=_tmp628.f2;_tmp62B=_tmp628.f3;{void*_tmp62C=Cyc_Parse_speclist2typ(
_tmp62A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp62B != 0)({void*_tmp62D[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp62E="bad attributes on parameter, ignoring";
_tag_arr(_tmp62E,sizeof(char),_get_zero_arr_size(_tmp62E,38));}),_tag_arr(
_tmp62D,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_ParamDecl_tok_struct
_tmp630;_tmp630.tag=Cyc_ParamDecl_tok;_tmp630.f1=({struct _tuple2*_tmp631=
_cycalloc(sizeof(*_tmp631));_tmp631->f1=0;_tmp631->f2=_tmp629;_tmp631->f3=
_tmp62C;_tmp631;});_tmp630;});_tmp62F;});break;}}case 189: _LL297: {struct _tuple15
_tmp633;struct Cyc_Absyn_Tqual _tmp634;struct Cyc_List_List*_tmp635;struct Cyc_List_List*
_tmp636;struct _tuple15*_tmp632=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp633=*_tmp632;_tmp634=
_tmp633.f1;_tmp635=_tmp633.f2;_tmp636=_tmp633.f3;{void*_tmp637=Cyc_Parse_speclist2typ(
_tmp635,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp638=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual
_tmp63A;void*_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_List_List*_tmp63D;
struct _tuple6 _tmp639=Cyc_Parse_apply_tms(_tmp634,_tmp637,_tmp636,_tmp638);
_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;_tmp63C=_tmp639.f3;_tmp63D=_tmp639.f4;if(
_tmp63C != 0)({void*_tmp63E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp63F="bad type parameters on formal argument, ignoring";_tag_arr(_tmp63F,
sizeof(char),_get_zero_arr_size(_tmp63F,49));}),_tag_arr(_tmp63E,sizeof(void*),0));});
if(_tmp63D != 0)({void*_tmp640[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp641="bad attributes on parameter, ignoring";_tag_arr(_tmp641,sizeof(char),
_get_zero_arr_size(_tmp641,38));}),_tag_arr(_tmp640,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp642=_cycalloc(sizeof(*_tmp642));
_tmp642[0]=({struct Cyc_ParamDecl_tok_struct _tmp643;_tmp643.tag=Cyc_ParamDecl_tok;
_tmp643.f1=({struct _tuple2*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->f1=0;
_tmp644->f2=_tmp63A;_tmp644->f3=_tmp63B;_tmp644;});_tmp643;});_tmp642;});break;}}
case 190: _LL298: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp645=_cycalloc(
sizeof(*_tmp645));_tmp645[0]=({struct Cyc_IdList_tok_struct _tmp646;_tmp646.tag=
Cyc_IdList_tok;_tmp646.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp646;});_tmp645;});break;case 191: _LL299: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_IdList_tok_struct
_tmp648;_tmp648.tag=Cyc_IdList_tok;_tmp648.f1=({struct Cyc_List_List*_tmp649=
_cycalloc(sizeof(*_tmp649));_tmp649->hd=({struct _tagged_arr*_tmp64A=_cycalloc(
sizeof(*_tmp64A));_tmp64A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64A;});_tmp649->tl=0;
_tmp649;});_tmp648;});_tmp647;});break;case 192: _LL29A: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_IdList_tok_struct
_tmp64C;_tmp64C.tag=Cyc_IdList_tok;_tmp64C.f1=({struct Cyc_List_List*_tmp64D=
_cycalloc(sizeof(*_tmp64D));_tmp64D->hd=({struct _tagged_arr*_tmp64E=_cycalloc(
sizeof(*_tmp64E));_tmp64E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64E;});_tmp64D->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp64D;});_tmp64C;});
_tmp64B;});break;case 193: _LL29B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 194: _LL29C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 195: _LL29D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_Exp_tok_struct _tmp650;
_tmp650.tag=Cyc_Exp_tok;_tmp650.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp652;_tmp652.tag=34;_tmp652.f1=0;_tmp652.f2=0;_tmp652;});_tmp651;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp650;});
_tmp64F;});break;case 196: _LL29E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp653=
_cycalloc(sizeof(*_tmp653));_tmp653[0]=({struct Cyc_Exp_tok_struct _tmp654;_tmp654.tag=
Cyc_Exp_tok;_tmp654.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp656;_tmp656.tag=34;_tmp656.f1=0;_tmp656.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp656;});_tmp655;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp654;});_tmp653;});break;case 197: _LL29F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657[0]=({
struct Cyc_Exp_tok_struct _tmp658;_tmp658.tag=Cyc_Exp_tok;_tmp658.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp659=_cycalloc(sizeof(*_tmp659));
_tmp659[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp65A;_tmp65A.tag=34;
_tmp65A.f1=0;_tmp65A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp65A;});_tmp659;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp658;});
_tmp657;});break;case 198: _LL2A0: {struct Cyc_Absyn_Vardecl*_tmp65B=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->f1=(void*)0;_tmp660->f2=({
struct _tagged_arr*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp661;});_tmp660;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp65B->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));
_tmp65C[0]=({struct Cyc_Exp_tok_struct _tmp65D;_tmp65D.tag=Cyc_Exp_tok;_tmp65D.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp65E=
_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp65F;_tmp65F.tag=27;_tmp65F.f1=_tmp65B;_tmp65F.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp65F.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F.f4=0;
_tmp65F;});_tmp65E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp65D;});
_tmp65C;});break;}case 199: _LL2A1: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_InitializerList_tok_struct
_tmp663;_tmp663.tag=Cyc_InitializerList_tok;_tmp663.f1=({struct Cyc_List_List*
_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->hd=({struct _tuple18*_tmp665=
_cycalloc(sizeof(*_tmp665));_tmp665->f1=0;_tmp665->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp665;});_tmp664->tl=0;
_tmp664;});_tmp663;});_tmp662;});break;case 200: _LL2A2: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666[0]=({struct Cyc_InitializerList_tok_struct
_tmp667;_tmp667.tag=Cyc_InitializerList_tok;_tmp667.f1=({struct Cyc_List_List*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->hd=({struct _tuple18*_tmp669=
_cycalloc(sizeof(*_tmp669));_tmp669->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp669->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp669;});_tmp668->tl=
0;_tmp668;});_tmp667;});_tmp666;});break;case 201: _LL2A3: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_InitializerList_tok_struct
_tmp66B;_tmp66B.tag=Cyc_InitializerList_tok;_tmp66B.f1=({struct Cyc_List_List*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->hd=({struct _tuple18*_tmp66D=
_cycalloc(sizeof(*_tmp66D));_tmp66D->f1=0;_tmp66D->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66D;});_tmp66C->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66C;});_tmp66B;});
_tmp66A;});break;case 202: _LL2A4: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E[0]=({struct Cyc_InitializerList_tok_struct
_tmp66F;_tmp66F.tag=Cyc_InitializerList_tok;_tmp66F.f1=({struct Cyc_List_List*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->hd=({struct _tuple18*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp671->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp671;});_tmp670->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp670;});_tmp66F;});_tmp66E;});break;case 203: _LL2A5: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp672=_cycalloc(sizeof(*_tmp672));
_tmp672[0]=({struct Cyc_DesignatorList_tok_struct _tmp673;_tmp673.tag=Cyc_DesignatorList_tok;
_tmp673.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp673;});_tmp672;});
break;case 204: _LL2A6: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp674=
_cycalloc(sizeof(*_tmp674));_tmp674[0]=({struct Cyc_DesignatorList_tok_struct
_tmp675;_tmp675.tag=Cyc_DesignatorList_tok;_tmp675.f1=({struct Cyc_List_List*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676->tl=0;_tmp676;});
_tmp675;});_tmp674;});break;case 205: _LL2A7: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677[0]=({struct Cyc_DesignatorList_tok_struct
_tmp678;_tmp678.tag=Cyc_DesignatorList_tok;_tmp678.f1=({struct Cyc_List_List*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp679->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp679;});_tmp678;});
_tmp677;});break;case 206: _LL2A8: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Designator_tok_struct
_tmp67B;_tmp67B.tag=Cyc_Designator_tok;_tmp67B.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp67D;_tmp67D.tag=0;_tmp67D.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67D;});_tmp67C;}));
_tmp67B;});_tmp67A;});break;case 207: _LL2A9: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_Designator_tok_struct
_tmp67F;_tmp67F.tag=Cyc_Designator_tok;_tmp67F.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_FieldName_struct
_tmp681;_tmp681.tag=1;_tmp681.f1=({struct _tagged_arr*_tmp682=_cycalloc(sizeof(*
_tmp682));_tmp682[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp682;});_tmp681;});_tmp680;}));_tmp67F;});_tmp67E;});
break;case 208: _LL2AA: {struct _tuple15 _tmp684;struct Cyc_Absyn_Tqual _tmp685;struct
Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp687;struct _tuple15*_tmp683=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp684=*_tmp683;
_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;_tmp687=_tmp684.f3;{void*_tmp688=Cyc_Parse_speclist2typ(
_tmp686,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp687 != 0)({void*_tmp689[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp68A="ignoring attributes in type";
_tag_arr(_tmp68A,sizeof(char),_get_zero_arr_size(_tmp68A,28));}),_tag_arr(
_tmp689,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_ParamDecl_tok_struct
_tmp68C;_tmp68C.tag=Cyc_ParamDecl_tok;_tmp68C.f1=({struct _tuple2*_tmp68D=
_cycalloc(sizeof(*_tmp68D));_tmp68D->f1=0;_tmp68D->f2=_tmp685;_tmp68D->f3=
_tmp688;_tmp68D;});_tmp68C;});_tmp68B;});break;}}case 209: _LL2AB: {struct _tuple15
_tmp68F;struct Cyc_Absyn_Tqual _tmp690;struct Cyc_List_List*_tmp691;struct Cyc_List_List*
_tmp692;struct _tuple15*_tmp68E=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68F=*_tmp68E;_tmp690=
_tmp68F.f1;_tmp691=_tmp68F.f2;_tmp692=_tmp68F.f3;{void*_tmp693=Cyc_Parse_speclist2typ(
_tmp691,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp694=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp695=Cyc_Parse_apply_tms(_tmp690,_tmp693,_tmp692,_tmp694);if(_tmp695.f3 != 0)({
void*_tmp696[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp697="bad type params, ignoring";_tag_arr(_tmp697,sizeof(char),
_get_zero_arr_size(_tmp697,26));}),_tag_arr(_tmp696,sizeof(void*),0));});if(
_tmp695.f4 != 0)({void*_tmp698[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp699="bad specifiers, ignoring";_tag_arr(_tmp699,sizeof(char),
_get_zero_arr_size(_tmp699,25));}),_tag_arr(_tmp698,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));
_tmp69A[0]=({struct Cyc_ParamDecl_tok_struct _tmp69B;_tmp69B.tag=Cyc_ParamDecl_tok;
_tmp69B.f1=({struct _tuple2*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->f1=0;
_tmp69C->f2=_tmp695.f1;_tmp69C->f3=_tmp695.f2;_tmp69C;});_tmp69B;});_tmp69A;});
break;}}case 210: _LL2AC: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Type_tok_struct _tmp69E;
_tmp69E.tag=Cyc_Type_tok;_tmp69E.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp69E;});_tmp69D;});
break;case 211: _LL2AD: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp69F=_cycalloc(
sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_Type_tok_struct _tmp6A0;_tmp6A0.tag=Cyc_Type_tok;
_tmp6A0.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6A1=_cycalloc(
sizeof(*_tmp6A1));_tmp6A1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6A2;_tmp6A2.tag=
20;_tmp6A2.f1=0;_tmp6A2;});_tmp6A1;}));_tmp6A0;});_tmp69F;});break;case 212:
_LL2AE: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6A3=_cycalloc(sizeof(*
_tmp6A3));_tmp6A3[0]=({struct Cyc_Type_tok_struct _tmp6A4;_tmp6A4.tag=Cyc_Type_tok;
_tmp6A4.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6A5=_cycalloc(
sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6A6;_tmp6A6.tag=
20;_tmp6A6.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6A6;});_tmp6A5;}));_tmp6A4;});_tmp6A3;});break;case 213:
_LL2AF: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6A7=_cycalloc(sizeof(*
_tmp6A7));_tmp6A7[0]=({struct Cyc_Type_tok_struct _tmp6A8;_tmp6A8.tag=Cyc_Type_tok;
_tmp6A8.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A9=_cycalloc(
sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6AA;_tmp6AA.tag=
21;_tmp6AA.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6AA;});_tmp6A9;}));_tmp6A8;});_tmp6A7;});break;case
214: _LL2B0: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6AB=_cycalloc(sizeof(*
_tmp6AB));_tmp6AB[0]=({struct Cyc_Type_tok_struct _tmp6AC;_tmp6AC.tag=Cyc_Type_tok;
_tmp6AC.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6AD=_cycalloc(
sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6AE;_tmp6AE.tag=
20;_tmp6AE.f1=({struct Cyc_List_List*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp6AF->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6AF;});_tmp6AE;});_tmp6AD;}));_tmp6AC;});_tmp6AB;});
break;case 215: _LL2B1: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6B0=
_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_TypeList_tok_struct _tmp6B1;
_tmp6B1.tag=Cyc_TypeList_tok;_tmp6B1.f1=({struct Cyc_List_List*_tmp6B2=_cycalloc(
sizeof(*_tmp6B2));_tmp6B2->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B2->tl=0;_tmp6B2;});
_tmp6B1;});_tmp6B0;});break;case 216: _LL2B2: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_TypeList_tok_struct
_tmp6B4;_tmp6B4.tag=Cyc_TypeList_tok;_tmp6B4.f1=({struct Cyc_List_List*_tmp6B5=
_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B5->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B5;});_tmp6B4;});
_tmp6B3;});break;case 217: _LL2B3: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6B7;_tmp6B7.tag=Cyc_AbstractDeclarator_tok;_tmp6B7.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B8;});_tmp6B7;});
_tmp6B6;});break;case 218: _LL2B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 219: _LL2B5: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6BA;_tmp6BA.tag=Cyc_AbstractDeclarator_tok;_tmp6BA.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6BB;});
_tmp6BA;});_tmp6B9;});break;case 220: _LL2B6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 221: _LL2B7:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6BC=_cycalloc(sizeof(*
_tmp6BC));_tmp6BC[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6BD;_tmp6BD.tag=
Cyc_AbstractDeclarator_tok;_tmp6BD.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->tms=({struct Cyc_List_List*_tmp6BF=
_cycalloc(sizeof(*_tmp6BF));_tmp6BF->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C1;});_tmp6C0;}));
_tmp6BF->tl=0;_tmp6BF;});_tmp6BE;});_tmp6BD;});_tmp6BC;});break;case 222: _LL2B8:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6C2=_cycalloc(sizeof(*
_tmp6C2));_tmp6C2[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6C3;_tmp6C3.tag=
Cyc_AbstractDeclarator_tok;_tmp6C3.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->tms=({struct Cyc_List_List*_tmp6C5=
_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C7;_tmp6C7.tag=0;_tmp6C7.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C7;});_tmp6C6;}));
_tmp6C5->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6C5;});_tmp6C4;});
_tmp6C3;});_tmp6C2;});break;case 223: _LL2B9: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6C9;_tmp6C9.tag=Cyc_AbstractDeclarator_tok;_tmp6C9.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->tms=({struct Cyc_List_List*_tmp6CB=
_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6CD;_tmp6CD.tag=1;_tmp6CD.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CD.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CD;});_tmp6CC;}));
_tmp6CB->tl=0;_tmp6CB;});_tmp6CA;});_tmp6C9;});_tmp6C8;});break;case 224: _LL2BA:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6CE=_cycalloc(sizeof(*
_tmp6CE));_tmp6CE[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6CF;_tmp6CF.tag=
Cyc_AbstractDeclarator_tok;_tmp6CF.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tms=({struct Cyc_List_List*_tmp6D1=
_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6D3;_tmp6D3.tag=1;_tmp6D3.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D3.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D3;});_tmp6D2;}));
_tmp6D1->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6D1;});_tmp6D0;});
_tmp6CF;});_tmp6CE;});break;case 225: _LL2BB: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6D5;_tmp6D5.tag=Cyc_AbstractDeclarator_tok;_tmp6D5.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->tms=({struct Cyc_List_List*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D9;_tmp6D9.tag=3;_tmp6D9.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6DB;_tmp6DB.tag=1;_tmp6DB.f1=0;_tmp6DB.f2=0;_tmp6DB.f3=0;_tmp6DB.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DB.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DB;});_tmp6DA;}));
_tmp6D9;});_tmp6D8;}));_tmp6D7->tl=0;_tmp6D7;});_tmp6D6;});_tmp6D5;});_tmp6D4;});
break;case 226: _LL2BC: {struct _tuple14 _tmp6DD;struct Cyc_List_List*_tmp6DE;int
_tmp6DF;struct Cyc_Absyn_VarargInfo*_tmp6E0;struct Cyc_Core_Opt*_tmp6E1;struct Cyc_List_List*
_tmp6E2;struct _tuple14*_tmp6DC=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6DD=*_tmp6DC;_tmp6DE=_tmp6DD.f1;_tmp6DF=_tmp6DD.f2;
_tmp6E0=_tmp6DD.f3;_tmp6E1=_tmp6DD.f4;_tmp6E2=_tmp6DD.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6E4;_tmp6E4.tag=Cyc_AbstractDeclarator_tok;
_tmp6E4.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E5=_cycalloc(sizeof(*
_tmp6E5));_tmp6E5->tms=({struct Cyc_List_List*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));
_tmp6E6->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6E7=
_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E8;_tmp6E8.tag=3;_tmp6E8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6EA;_tmp6EA.tag=1;_tmp6EA.f1=_tmp6DE;_tmp6EA.f2=_tmp6DF;_tmp6EA.f3=_tmp6E0;
_tmp6EA.f4=_tmp6E1;_tmp6EA.f5=_tmp6E2;_tmp6EA;});_tmp6E9;}));_tmp6E8;});_tmp6E7;}));
_tmp6E6->tl=0;_tmp6E6;});_tmp6E5;});_tmp6E4;});_tmp6E3;});break;}case 227: _LL2BD:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6EB=_cycalloc(sizeof(*
_tmp6EB));_tmp6EB[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6EC;_tmp6EC.tag=
Cyc_AbstractDeclarator_tok;_tmp6EC.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->tms=({struct Cyc_List_List*_tmp6EE=
_cycalloc(sizeof(*_tmp6EE));_tmp6EE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F0;_tmp6F0.tag=3;_tmp6F0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F2;_tmp6F2.tag=1;_tmp6F2.f1=0;_tmp6F2.f2=0;_tmp6F2.f3=0;_tmp6F2.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F2.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F2;});_tmp6F1;}));
_tmp6F0;});_tmp6EF;}));_tmp6EE->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6EE;});_tmp6ED;});
_tmp6EC;});_tmp6EB;});break;case 228: _LL2BE: {struct _tuple14 _tmp6F4;struct Cyc_List_List*
_tmp6F5;int _tmp6F6;struct Cyc_Absyn_VarargInfo*_tmp6F7;struct Cyc_Core_Opt*_tmp6F8;
struct Cyc_List_List*_tmp6F9;struct _tuple14*_tmp6F3=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F4=*_tmp6F3;_tmp6F5=
_tmp6F4.f1;_tmp6F6=_tmp6F4.f2;_tmp6F7=_tmp6F4.f3;_tmp6F8=_tmp6F4.f4;_tmp6F9=
_tmp6F4.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6FB;_tmp6FB.tag=Cyc_AbstractDeclarator_tok;_tmp6FB.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->tms=({struct Cyc_List_List*_tmp6FD=
_cycalloc(sizeof(*_tmp6FD));_tmp6FD->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6FF;_tmp6FF.tag=3;_tmp6FF.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp701;_tmp701.tag=1;_tmp701.f1=_tmp6F5;_tmp701.f2=_tmp6F6;_tmp701.f3=_tmp6F7;
_tmp701.f4=_tmp6F8;_tmp701.f5=_tmp6F9;_tmp701;});_tmp700;}));_tmp6FF;});_tmp6FE;}));
_tmp6FD->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6FD;});_tmp6FC;});
_tmp6FB;});_tmp6FA;});break;}case 229: _LL2BF: {struct Cyc_List_List*_tmp702=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp704;_tmp704.tag=Cyc_AbstractDeclarator_tok;
_tmp704.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp705=_cycalloc(sizeof(*
_tmp705));_tmp705->tms=({struct Cyc_List_List*_tmp706=_cycalloc(sizeof(*_tmp706));
_tmp706->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp707=
_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp708;_tmp708.tag=4;_tmp708.f1=_tmp702;_tmp708.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp708.f3=0;
_tmp708;});_tmp707;}));_tmp706->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp706;});_tmp705;});
_tmp704;});_tmp703;});break;}case 230: _LL2C0: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp70A;_tmp70A.tag=Cyc_AbstractDeclarator_tok;_tmp70A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B->tms=({struct Cyc_List_List*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp70E;_tmp70E.tag=5;_tmp70E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp70E.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70E;});_tmp70D;}));
_tmp70C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp70C;});_tmp70B;});
_tmp70A;});_tmp709;});break;case 231: _LL2C1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 232: _LL2C2: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 233: _LL2C3:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 234:
_LL2C4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
235: _LL2C5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 236: _LL2C6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 237: _LL2C7: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp70F="`H";
_tag_arr(_tmp70F,sizeof(char),_get_zero_arr_size(_tmp70F,3));}))== 0)Cyc_Parse_err(({
const char*_tmp710="bad occurrence of heap region `H";_tag_arr(_tmp710,sizeof(
char),_get_zero_arr_size(_tmp710,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->name=({
struct _tagged_arr*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp71E;});_tmp71B->identity=
0;_tmp71B->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp71C=_cycalloc(
sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp71D;_tmp71D.tag=0;
_tmp71D.f1=(void*)((void*)3);_tmp71D;});_tmp71C;}));_tmp71B;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({
struct Cyc_Absyn_VarType_struct _tmp71A;_tmp71A.tag=1;_tmp71A.f1=tv;_tmp71A;});
_tmp719;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp711=_cycalloc(sizeof(*
_tmp711));_tmp711[0]=({struct Cyc_Stmt_tok_struct _tmp712;_tmp712.tag=Cyc_Stmt_tok;
_tmp712.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp713=
_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_Region_s_struct _tmp714;
_tmp714.tag=18;_tmp714.f1=tv;_tmp714.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715->f1=(void*)0;_tmp715->f2=({struct
_tagged_arr*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp716;});_tmp715;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp717=_cycalloc(sizeof(*_tmp717));
_tmp717[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp718;_tmp718.tag=15;_tmp718.f1=(
void*)t;_tmp718;});_tmp717;}),0);_tmp714.f3=0;_tmp714.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp714;});_tmp713;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp712;});
_tmp711;});break;}case 238: _LL2C8: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp71F="H";
_tag_arr(_tmp71F,sizeof(char),_get_zero_arr_size(_tmp71F,2));}))== 0)Cyc_Parse_err(({
const char*_tmp720="bad occurrence of heap region `H";_tag_arr(_tmp720,sizeof(
char),_get_zero_arr_size(_tmp720,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->name=({
struct _tagged_arr*_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp731;_tmp731.tag=0;_tmp731.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp72F[1]={&
_tmp731};Cyc_aprintf(({const char*_tmp730="`%s";_tag_arr(_tmp730,sizeof(char),
_get_zero_arr_size(_tmp730,4));}),_tag_arr(_tmp72F,sizeof(void*),1));}});_tmp72E;});
_tmp72B->identity=0;_tmp72B->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp72D;_tmp72D.tag=0;_tmp72D.f1=(void*)((void*)3);_tmp72D;});_tmp72C;}));
_tmp72B;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp729=_cycalloc(
sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Absyn_VarType_struct _tmp72A;_tmp72A.tag=
1;_tmp72A.f1=tv;_tmp72A;});_tmp729;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Stmt_tok_struct
_tmp722;_tmp722.tag=Cyc_Stmt_tok;_tmp722.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=({
struct Cyc_Absyn_Region_s_struct _tmp724;_tmp724.tag=18;_tmp724.f1=tv;_tmp724.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp725=_cycalloc(sizeof(*_tmp725));
_tmp725->f1=(void*)0;_tmp725->f2=({struct _tagged_arr*_tmp726=_cycalloc(sizeof(*
_tmp726));_tmp726[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp726;});_tmp725;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp728;_tmp728.tag=15;_tmp728.f1=(void*)t;_tmp728;});_tmp727;}),0);_tmp724.f3=0;
_tmp724.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp724;});_tmp723;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp722;});
_tmp721;});break;}case 239: _LL2C9: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp732="resetable";
_tag_arr(_tmp732,sizeof(char),_get_zero_arr_size(_tmp732,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp733="expecting [resetable]";_tag_arr(_tmp733,sizeof(char),
_get_zero_arr_size(_tmp733,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp734="`H";_tag_arr(_tmp734,sizeof(char),
_get_zero_arr_size(_tmp734,3));}))== 0)Cyc_Parse_err(({const char*_tmp735="bad occurrence of heap region `H";
_tag_arr(_tmp735,sizeof(char),_get_zero_arr_size(_tmp735,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->name=({
struct _tagged_arr*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp743;});_tmp740->identity=
0;_tmp740->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp742;_tmp742.tag=0;
_tmp742.f1=(void*)((void*)3);_tmp742;});_tmp741;}));_tmp740;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=({
struct Cyc_Absyn_VarType_struct _tmp73F;_tmp73F.tag=1;_tmp73F.f1=tv;_tmp73F;});
_tmp73E;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp736=_cycalloc(sizeof(*
_tmp736));_tmp736[0]=({struct Cyc_Stmt_tok_struct _tmp737;_tmp737.tag=Cyc_Stmt_tok;
_tmp737.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp738=
_cycalloc(sizeof(*_tmp738));_tmp738[0]=({struct Cyc_Absyn_Region_s_struct _tmp739;
_tmp739.tag=18;_tmp739.f1=tv;_tmp739.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A->f1=(void*)0;_tmp73A->f2=({struct
_tagged_arr*_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73B;});_tmp73A;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp73C=_cycalloc(sizeof(*_tmp73C));
_tmp73C[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp73D;_tmp73D.tag=15;_tmp73D.f1=(
void*)t;_tmp73D;});_tmp73C;}),0);_tmp739.f3=1;_tmp739.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp739;});_tmp738;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp737;});
_tmp736;});break;}case 240: _LL2CA: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp744="resetable";
_tag_arr(_tmp744,sizeof(char),_get_zero_arr_size(_tmp744,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp745="expecting `resetable'";_tag_arr(_tmp745,sizeof(char),
_get_zero_arr_size(_tmp745,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp746="H";_tag_arr(_tmp746,sizeof(char),
_get_zero_arr_size(_tmp746,2));}))== 0)Cyc_Parse_err(({const char*_tmp747="bad occurrence of heap region `H";
_tag_arr(_tmp747,sizeof(char),_get_zero_arr_size(_tmp747,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752->name=({
struct _tagged_arr*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp758;_tmp758.tag=0;_tmp758.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp756[1]={&
_tmp758};Cyc_aprintf(({const char*_tmp757="`%s";_tag_arr(_tmp757,sizeof(char),
_get_zero_arr_size(_tmp757,4));}),_tag_arr(_tmp756,sizeof(void*),1));}});_tmp755;});
_tmp752->identity=0;_tmp752->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp754;_tmp754.tag=0;_tmp754.f1=(void*)((void*)3);_tmp754;});_tmp753;}));
_tmp752;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp750=_cycalloc(
sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_VarType_struct _tmp751;_tmp751.tag=
1;_tmp751.f1=tv;_tmp751;});_tmp750;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Stmt_tok_struct
_tmp749;_tmp749.tag=Cyc_Stmt_tok;_tmp749.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({
struct Cyc_Absyn_Region_s_struct _tmp74B;_tmp74B.tag=18;_tmp74B.f1=tv;_tmp74B.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp74C=_cycalloc(sizeof(*_tmp74C));
_tmp74C->f1=(void*)0;_tmp74C->f2=({struct _tagged_arr*_tmp74D=_cycalloc(sizeof(*
_tmp74D));_tmp74D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp74D;});_tmp74C;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp74F;_tmp74F.tag=15;_tmp74F.f1=(void*)t;_tmp74F;});_tmp74E;}),0);_tmp74B.f3=1;
_tmp74B.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp74B;});_tmp74A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp749;});
_tmp748;});break;}case 241: _LL2CB: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({struct Cyc_Stmt_tok_struct
_tmp75A;_tmp75A.tag=Cyc_Stmt_tok;_tmp75A.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp75C;_tmp75C.tag=20;_tmp75C.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75C;});_tmp75B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75A;});_tmp759;});break;case 242: _LL2CC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({
struct Cyc_Stmt_tok_struct _tmp75E;_tmp75E.tag=Cyc_Stmt_tok;_tmp75E.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp760;_tmp760.tag=13;_tmp760.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp760;});_tmp75F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75E;});_tmp75D;});break;case 243: _LL2CD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=({
struct Cyc_Stmt_tok_struct _tmp762;_tmp762.tag=Cyc_Stmt_tok;_tmp762.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp763=_cycalloc(sizeof(*_tmp763));
_tmp763[0]=({struct Cyc_Absyn_Splice_s_struct _tmp764;_tmp764.tag=14;_tmp764.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp764;});_tmp763;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp762;});
_tmp761;});break;case 244: _LL2CE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp765=
_cycalloc(sizeof(*_tmp765));_tmp765[0]=({struct Cyc_Stmt_tok_struct _tmp766;
_tmp766.tag=Cyc_Stmt_tok;_tmp766.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767[0]=({struct Cyc_Absyn_Label_s_struct
_tmp768;_tmp768.tag=15;_tmp768.f1=({struct _tagged_arr*_tmp769=_cycalloc(sizeof(*
_tmp769));_tmp769[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp769;});_tmp768.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp768;});_tmp767;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp766;});
_tmp765;});break;case 245: _LL2CF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_Stmt_tok_struct _tmp76B;
_tmp76B.tag=Cyc_Stmt_tok;_tmp76B.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76B;});
_tmp76A;});break;case 246: _LL2D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76C=
_cycalloc(sizeof(*_tmp76C));_tmp76C[0]=({struct Cyc_Stmt_tok_struct _tmp76D;
_tmp76D.tag=Cyc_Stmt_tok;_tmp76D.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76D;});
_tmp76C;});break;case 247: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Stmt_tok_struct _tmp76F;
_tmp76F.tag=Cyc_Stmt_tok;_tmp76F.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp76F;});
_tmp76E;});break;case 248: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp770=
_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_Stmt_tok_struct _tmp771;
_tmp771.tag=Cyc_Stmt_tok;_tmp771.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp771;});
_tmp770;});break;case 249: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 250: _LL2D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772[0]=({struct Cyc_Stmt_tok_struct
_tmp773;_tmp773.tag=Cyc_Stmt_tok;_tmp773.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp773;});
_tmp772;});break;case 251: _LL2D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp774=
_cycalloc(sizeof(*_tmp774));_tmp774[0]=({struct Cyc_Stmt_tok_struct _tmp775;
_tmp775.tag=Cyc_Stmt_tok;_tmp775.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp775;});_tmp774;});break;case 252: _LL2D6: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_Stmt_tok_struct
_tmp777;_tmp777.tag=Cyc_Stmt_tok;_tmp777.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp777;});_tmp776;});
break;case 253: _LL2D7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 254: _LL2D8: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Stmt_tok_struct
_tmp779;_tmp779.tag=Cyc_Stmt_tok;_tmp779.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp779;});
_tmp778;});break;case 255: _LL2D9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Stmt_tok_struct _tmp77B;
_tmp77B.tag=Cyc_Stmt_tok;_tmp77B.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp77D;_tmp77D.tag=1;_tmp77D.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77D;});_tmp77C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp77B;});_tmp77A;});break;
case 256: _LL2DA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp77E=_cycalloc(
sizeof(*_tmp77E));_tmp77E[0]=({struct Cyc_Stmt_tok_struct _tmp77F;_tmp77F.tag=Cyc_Stmt_tok;
_tmp77F.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp781;_tmp781.tag=1;_tmp781.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp781;});_tmp780;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp77F;});_tmp77E;});
break;case 257: _LL2DB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp782=_cycalloc(
sizeof(*_tmp782));_tmp782[0]=({struct Cyc_Stmt_tok_struct _tmp783;_tmp783.tag=Cyc_Stmt_tok;
_tmp783.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp783;});
_tmp782;});break;case 258: _LL2DC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp784=
_cycalloc(sizeof(*_tmp784));_tmp784[0]=({struct Cyc_Stmt_tok_struct _tmp785;
_tmp785.tag=Cyc_Stmt_tok;_tmp785.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp785;});
_tmp784;});break;case 259: _LL2DD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp786=
_cycalloc(sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Stmt_tok_struct _tmp787;
_tmp787.tag=Cyc_Stmt_tok;_tmp787.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp787;});
_tmp786;});break;case 260: _LL2DE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp788=
_cycalloc(sizeof(*_tmp788));_tmp788[0]=({struct Cyc_Stmt_tok_struct _tmp789;
_tmp789.tag=Cyc_Stmt_tok;_tmp789.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp789;});
_tmp788;});break;case 261: _LL2DF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78A=
_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({struct Cyc_Stmt_tok_struct _tmp78B;
_tmp78B.tag=Cyc_Stmt_tok;_tmp78B.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78B;});
_tmp78A;});break;case 262: _LL2E0: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp78C="C";
_tag_arr(_tmp78C,sizeof(char),_get_zero_arr_size(_tmp78C,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp78D="only switch \"C\" { ... } is allowed";_tag_arr(_tmp78D,
sizeof(char),_get_zero_arr_size(_tmp78D,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({
struct Cyc_Stmt_tok_struct _tmp78F;_tmp78F.tag=Cyc_Stmt_tok;_tmp78F.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp790=_cycalloc(sizeof(*_tmp790));
_tmp790[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp791;_tmp791.tag=10;_tmp791.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp791.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp791;});_tmp790;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78F;});
_tmp78E;});break;case 263: _LL2E1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Stmt_tok_struct _tmp793;
_tmp793.tag=Cyc_Stmt_tok;_tmp793.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp793;});
_tmp792;});break;case 264: _LL2E2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp794=
_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Stmt_tok_struct _tmp795;
_tmp795.tag=Cyc_Stmt_tok;_tmp795.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp795;});
_tmp794;});break;case 265: _LL2E3: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp797;_tmp797.tag=Cyc_SwitchClauseList_tok;_tmp797.f1=0;_tmp797;});_tmp796;});
break;case 266: _LL2E4: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp799;_tmp799.tag=Cyc_SwitchClauseList_tok;_tmp799.f1=({struct Cyc_List_List*
_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A->hd=({struct Cyc_Absyn_Switch_clause*
_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp79B->pat_vars=0;_tmp79B->where_clause=0;
_tmp79B->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp79B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp79B;});
_tmp79A->tl=0;_tmp79A;});_tmp799;});_tmp798;});break;case 267: _LL2E5: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp79C=_cycalloc(sizeof(*_tmp79C));
_tmp79C[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp79D;_tmp79D.tag=Cyc_SwitchClauseList_tok;
_tmp79D.f1=({struct Cyc_List_List*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->hd=({
struct Cyc_Absyn_Switch_clause*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp79F->pat_vars=0;_tmp79F->where_clause=0;_tmp79F->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp79F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp79F;});
_tmp79E->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp79E;});_tmp79D;});_tmp79C;});break;case 268: _LL2E6:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A0=_cycalloc(sizeof(*
_tmp7A0));_tmp7A0[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7A1;_tmp7A1.tag=
Cyc_SwitchClauseList_tok;_tmp7A1.f1=({struct Cyc_List_List*_tmp7A2=_cycalloc(
sizeof(*_tmp7A2));_tmp7A2->hd=({struct Cyc_Absyn_Switch_clause*_tmp7A3=_cycalloc(
sizeof(*_tmp7A3));_tmp7A3->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7A3->pat_vars=0;
_tmp7A3->where_clause=0;_tmp7A3->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp7A3;});
_tmp7A2->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A2;});_tmp7A1;});_tmp7A0;});break;case 269: _LL2E7:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A4=_cycalloc(sizeof(*
_tmp7A4));_tmp7A4[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7A5;_tmp7A5.tag=
Cyc_SwitchClauseList_tok;_tmp7A5.f1=({struct Cyc_List_List*_tmp7A6=_cycalloc(
sizeof(*_tmp7A6));_tmp7A6->hd=({struct Cyc_Absyn_Switch_clause*_tmp7A7=_cycalloc(
sizeof(*_tmp7A7));_tmp7A7->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7A7->pat_vars=0;
_tmp7A7->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A7->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7A7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A7;});
_tmp7A6->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A6;});_tmp7A5;});_tmp7A4;});break;case 270: _LL2E8:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A8=_cycalloc(sizeof(*
_tmp7A8));_tmp7A8[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7A9;_tmp7A9.tag=
Cyc_SwitchClauseList_tok;_tmp7A9.f1=({struct Cyc_List_List*_tmp7AA=_cycalloc(
sizeof(*_tmp7AA));_tmp7AA->hd=({struct Cyc_Absyn_Switch_clause*_tmp7AB=_cycalloc(
sizeof(*_tmp7AB));_tmp7AB->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp7AB->pat_vars=0;
_tmp7AB->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7AB->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7AB;});
_tmp7AA->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7AA;});_tmp7A9;});_tmp7A8;});break;case 271: _LL2E9:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7AC=_cycalloc(sizeof(*
_tmp7AC));_tmp7AC[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7AD;_tmp7AD.tag=
Cyc_SwitchClauseList_tok;_tmp7AD.f1=0;_tmp7AD;});_tmp7AC;});break;case 272: _LL2EA:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7AE=_cycalloc(sizeof(*
_tmp7AE));_tmp7AE[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7AF;_tmp7AF.tag=
Cyc_SwitchCClauseList_tok;_tmp7AF.f1=0;_tmp7AF;});_tmp7AE;});break;case 273:
_LL2EB: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7B0=_cycalloc(
sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B1;
_tmp7B1.tag=Cyc_SwitchCClauseList_tok;_tmp7B1.f1=({struct Cyc_List_List*_tmp7B2=
_cycalloc(sizeof(*_tmp7B2));_tmp7B2->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3->cnst_exp=0;_tmp7B3->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp7B3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B3;});
_tmp7B2->tl=0;_tmp7B2;});_tmp7B1;});_tmp7B0;});break;case 274: _LL2EC: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));
_tmp7B4[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B5;_tmp7B5.tag=Cyc_SwitchCClauseList_tok;
_tmp7B5.f1=({struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp7B7->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp7B7->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B7;});
_tmp7B6->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7B6;});_tmp7B5;});_tmp7B4;});break;case 275: _LL2ED:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7B8=_cycalloc(sizeof(*
_tmp7B8));_tmp7B8[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B9;_tmp7B9.tag=
Cyc_SwitchCClauseList_tok;_tmp7B9.f1=({struct Cyc_List_List*_tmp7BA=_cycalloc(
sizeof(*_tmp7BA));_tmp7BA->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7BB=_cycalloc(
sizeof(*_tmp7BB));_tmp7BB->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7BB->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp7BB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7BB;});
_tmp7BA->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7BA;});_tmp7B9;});_tmp7B8;});break;case 276: _LL2EE:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7BC=_cycalloc(sizeof(*
_tmp7BC));_tmp7BC[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7BD;_tmp7BD.tag=
Cyc_SwitchCClauseList_tok;_tmp7BD.f1=0;_tmp7BD;});_tmp7BC;});break;case 277:
_LL2EF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BE=_cycalloc(sizeof(*
_tmp7BE));_tmp7BE[0]=({struct Cyc_Stmt_tok_struct _tmp7BF;_tmp7BF.tag=Cyc_Stmt_tok;
_tmp7BF.f1=Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BF;});
_tmp7BE;});break;case 278: _LL2F0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C0=
_cycalloc(sizeof(*_tmp7C0));_tmp7C0[0]=({struct Cyc_Stmt_tok_struct _tmp7C1;
_tmp7C1.tag=Cyc_Stmt_tok;_tmp7C1.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C1;});
_tmp7C0;});break;case 279: _LL2F1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C2=
_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Stmt_tok_struct _tmp7C3;
_tmp7C3.tag=Cyc_Stmt_tok;_tmp7C3.f1=Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C3;});
_tmp7C2;});break;case 280: _LL2F2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C4=
_cycalloc(sizeof(*_tmp7C4));_tmp7C4[0]=({struct Cyc_Stmt_tok_struct _tmp7C5;
_tmp7C5.tag=Cyc_Stmt_tok;_tmp7C5.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C5;});
_tmp7C4;});break;case 281: _LL2F3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C6=
_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Stmt_tok_struct _tmp7C7;
_tmp7C7.tag=Cyc_Stmt_tok;_tmp7C7.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C7;});
_tmp7C6;});break;case 282: _LL2F4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C8=
_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Stmt_tok_struct _tmp7C9;
_tmp7C9.tag=Cyc_Stmt_tok;_tmp7C9.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C9;});
_tmp7C8;});break;case 283: _LL2F5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CA=
_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({struct Cyc_Stmt_tok_struct _tmp7CB;
_tmp7CB.tag=Cyc_Stmt_tok;_tmp7CB.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CB;});
_tmp7CA;});break;case 284: _LL2F6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CC=
_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({struct Cyc_Stmt_tok_struct _tmp7CD;
_tmp7CD.tag=Cyc_Stmt_tok;_tmp7CD.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7CD;});
_tmp7CC;});break;case 285: _LL2F7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CE=
_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=({struct Cyc_Stmt_tok_struct _tmp7CF;
_tmp7CF.tag=Cyc_Stmt_tok;_tmp7CF.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CF;});
_tmp7CE;});break;case 286: _LL2F8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D0=
_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Stmt_tok_struct _tmp7D1;
_tmp7D1.tag=Cyc_Stmt_tok;_tmp7D1.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D1;});
_tmp7D0;});break;case 287: _LL2F9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D2=
_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Stmt_tok_struct _tmp7D3;
_tmp7D3.tag=Cyc_Stmt_tok;_tmp7D3.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D3;});
_tmp7D2;});break;case 288: _LL2FA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D4=
_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Stmt_tok_struct _tmp7D5;
_tmp7D5.tag=Cyc_Stmt_tok;_tmp7D5.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D5;});
_tmp7D4;});break;case 289: _LL2FB: {struct Cyc_List_List*_tmp7D6=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp7D7=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D8=
_cycalloc(sizeof(*_tmp7D8));_tmp7D8[0]=({struct Cyc_Stmt_tok_struct _tmp7D9;
_tmp7D9.tag=Cyc_Stmt_tok;_tmp7D9.f1=Cyc_Parse_flatten_declarations(_tmp7D6,
_tmp7D7);_tmp7D9;});_tmp7D8;});break;}case 290: _LL2FC: {struct Cyc_List_List*
_tmp7DA=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7DB=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=({
struct Cyc_Stmt_tok_struct _tmp7DD;_tmp7DD.tag=Cyc_Stmt_tok;_tmp7DD.f1=Cyc_Parse_flatten_declarations(
_tmp7DA,_tmp7DB);_tmp7DD;});_tmp7DC;});break;}case 291: _LL2FD: {struct Cyc_List_List*
_tmp7DE=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7DF=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));_tmp7E0[0]=({
struct Cyc_Stmt_tok_struct _tmp7E1;_tmp7E1.tag=Cyc_Stmt_tok;_tmp7E1.f1=Cyc_Parse_flatten_declarations(
_tmp7DE,_tmp7DF);_tmp7E1;});_tmp7E0;});break;}case 292: _LL2FE: {struct Cyc_List_List*
_tmp7E2=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp7E3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({
struct Cyc_Stmt_tok_struct _tmp7E5;_tmp7E5.tag=Cyc_Stmt_tok;_tmp7E5.f1=Cyc_Parse_flatten_declarations(
_tmp7E2,_tmp7E3);_tmp7E5;});_tmp7E4;});break;}case 293: _LL2FF: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({
struct Cyc_Stmt_tok_struct _tmp7E7;_tmp7E7.tag=Cyc_Stmt_tok;_tmp7E7.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E7;});_tmp7E6;});break;case 294: _LL300: {struct
Cyc_List_List*_tmp7E8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(
struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));
_tmp7E9[0]=({struct Cyc_Stmt_tok_struct _tmp7EA;_tmp7EA.tag=Cyc_Stmt_tok;_tmp7EA.f1=
Cyc_Absyn_forarray_stmt(_tmp7E8,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EA;});
_tmp7E9;});break;}case 295: _LL301: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7EB=_cycalloc(sizeof(*_tmp7EB));_tmp7EB[0]=({struct Cyc_Stmt_tok_struct
_tmp7EC;_tmp7EC.tag=Cyc_Stmt_tok;_tmp7EC.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7EC;});
_tmp7EB;});break;case 296: _LL302: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EE=
_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=({struct Cyc_Stmt_tok_struct _tmp7EF;
_tmp7EF.tag=Cyc_Stmt_tok;_tmp7EF.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7EF;});
_tmp7EE;});break;case 297: _LL303: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F0=
_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({struct Cyc_Stmt_tok_struct _tmp7F1;
_tmp7F1.tag=Cyc_Stmt_tok;_tmp7F1.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7F1;});
_tmp7F0;});break;case 298: _LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F2=
_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Stmt_tok_struct _tmp7F3;
_tmp7F3.tag=Cyc_Stmt_tok;_tmp7F3.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7F3;});
_tmp7F2;});break;case 299: _LL305: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F4=
_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_Stmt_tok_struct _tmp7F5;
_tmp7F5.tag=Cyc_Stmt_tok;_tmp7F5.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7F5;});_tmp7F4;});break;case 300: _LL306: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({
struct Cyc_Stmt_tok_struct _tmp7F7;_tmp7F7.tag=Cyc_Stmt_tok;_tmp7F7.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7F7;});_tmp7F6;});break;case 301: _LL307: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({
struct Cyc_Stmt_tok_struct _tmp7F9;_tmp7F9.tag=Cyc_Stmt_tok;_tmp7F9.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp7F9;});_tmp7F8;});break;case 302: _LL308: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({
struct Cyc_Stmt_tok_struct _tmp7FB;_tmp7FB.tag=Cyc_Stmt_tok;_tmp7FB.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp7FB;});_tmp7FA;});break;case 303: _LL309: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[
0]=({struct Cyc_Pattern_tok_struct _tmp7FD;_tmp7FD.tag=Cyc_Pattern_tok;_tmp7FD.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FD;});
_tmp7FC;});break;case 304: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 305: _LL30B: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({struct Cyc_Pattern_tok_struct
_tmp7FF;_tmp7FF.tag=Cyc_Pattern_tok;_tmp7FF.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=({struct
Cyc_Absyn_Int_p_struct _tmp801;_tmp801.tag=7;_tmp801.f1=(void*)(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp801.f2=(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp801;});_tmp800;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7FF;});_tmp7FE;});break;case 306: _LL30C: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[
0]=({struct Cyc_Pattern_tok_struct _tmp803;_tmp803.tag=Cyc_Pattern_tok;_tmp803.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp804=_cycalloc(
sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Absyn_Int_p_struct _tmp805;_tmp805.tag=7;
_tmp805.f1=(void*)((void*)0);_tmp805.f2=-(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp805;});_tmp804;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp803;});_tmp802;});break;case 307: _LL30D: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806[
0]=({struct Cyc_Pattern_tok_struct _tmp807;_tmp807.tag=Cyc_Pattern_tok;_tmp807.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp808=_cycalloc(
sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Absyn_Float_p_struct _tmp809;_tmp809.tag=
9;_tmp809.f1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp809;});_tmp808;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp807;});
_tmp806;});break;case 308: _LL30E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({struct Cyc_Pattern_tok_struct
_tmp80B;_tmp80B.tag=Cyc_Pattern_tok;_tmp80B.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp80C=_cycalloc_atomic(sizeof(*_tmp80C));_tmp80C[0]=({
struct Cyc_Absyn_Char_p_struct _tmp80D;_tmp80D.tag=8;_tmp80D.f1=Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80D;});_tmp80C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80B;});_tmp80A;});break;case 309: _LL30F: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[
0]=({struct Cyc_Pattern_tok_struct _tmp80F;_tmp80F.tag=Cyc_Pattern_tok;_tmp80F.f1=
Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80F;});
_tmp80E;});break;case 310: _LL310: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Pattern_tok_struct
_tmp811;_tmp811.tag=Cyc_Pattern_tok;_tmp811.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp813;_tmp813.tag=12;_tmp813.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp813;});_tmp812;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp811;});_tmp810;});break;case 311: _LL311: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[
0]=({struct Cyc_Pattern_tok_struct _tmp815;_tmp815.tag=Cyc_Pattern_tok;_tmp815.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp816=_cycalloc(
sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp817;_tmp817.tag=
3;_tmp817.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]));_tmp817;});_tmp816;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp815;});
_tmp814;});break;case 312: _LL312: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Pattern_tok_struct
_tmp819;_tmp819.tag=Cyc_Pattern_tok;_tmp819.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownCall_p_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp81B;_tmp81B.tag=13;_tmp81B.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp81B.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp81B;});_tmp81A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp819;});_tmp818;});break;case 313: _LL313: {struct
Cyc_List_List*_tmp81C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D[0]=({
struct Cyc_Pattern_tok_struct _tmp81E;_tmp81E.tag=Cyc_Pattern_tok;_tmp81E.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp81F=_cycalloc(sizeof(*_tmp81F));
_tmp81F[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp820;_tmp820.tag=5;_tmp820.f1=({
struct Cyc_Absyn_AggrInfo _tmp821;_tmp821.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp823;_tmp823.tag=0;_tmp823.f1=(void*)((void*)0);_tmp823.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp823;});_tmp822;}));
_tmp821.targs=0;_tmp821;});_tmp820.f2=_tmp81C;_tmp820.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp820;});_tmp81F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp81E;});_tmp81D;});break;}case 314: _LL314: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[
0]=({struct Cyc_Pattern_tok_struct _tmp825;_tmp825.tag=Cyc_Pattern_tok;_tmp825.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp827;_tmp827.tag=
4;_tmp827.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp827;});_tmp826;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp825;});
_tmp824;});break;case 315: _LL315: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Pattern_tok_struct
_tmp829;_tmp829.tag=Cyc_Pattern_tok;_tmp829.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp82B;_tmp82B.tag=1;_tmp82B.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C->f1=(void*)0;_tmp82C->f2=({
struct _tagged_arr*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp82D;});_tmp82C;}),(
void*)0,0);_tmp82B;});_tmp82A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp829;});
_tmp828;});break;case 316: _LL316: {void*_tmp82E=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp837=_cycalloc(sizeof(*_tmp837));_tmp837[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp838;_tmp838.tag=0;_tmp838.f1=(void*)((void*)5);_tmp838;});_tmp837;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[
0]=({struct Cyc_Pattern_tok_struct _tmp830;_tmp830.tag=Cyc_Pattern_tok;_tmp830.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp831=_cycalloc(
sizeof(*_tmp831));_tmp831[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp832;_tmp832.tag=
2;_tmp832.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp82E);
_tmp832.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp833=_cycalloc(sizeof(*
_tmp833));_tmp833->f1=(void*)0;_tmp833->f2=({struct _tagged_arr*_tmp834=_cycalloc(
sizeof(*_tmp834));_tmp834[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp834;});_tmp833;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp835=_cycalloc(sizeof(*_tmp835));
_tmp835[0]=({struct Cyc_Absyn_TagType_struct _tmp836;_tmp836.tag=17;_tmp836.f1=(
void*)_tmp82E;_tmp836;});_tmp835;}),0);_tmp832;});_tmp831;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp830;});
_tmp82F;});break;}case 317: _LL317: {struct Cyc_Absyn_Tvar*_tmp839=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp845;_tmp845.tag=0;_tmp845.f1=(void*)((void*)
5);_tmp845;});_tmp844;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp83A=
_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Pattern_tok_struct _tmp83B;
_tmp83B.tag=Cyc_Pattern_tok;_tmp83B.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp83D;_tmp83D.tag=2;_tmp83D.f1=_tmp839;_tmp83D.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->f1=(void*)0;_tmp83E->f2=({
struct _tagged_arr*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp83F;});_tmp83E;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp840=_cycalloc(sizeof(*_tmp840));
_tmp840[0]=({struct Cyc_Absyn_TagType_struct _tmp841;_tmp841.tag=17;_tmp841.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp842=_cycalloc(sizeof(*_tmp842));
_tmp842[0]=({struct Cyc_Absyn_VarType_struct _tmp843;_tmp843.tag=1;_tmp843.f1=
_tmp839;_tmp843;});_tmp842;}));_tmp841;});_tmp840;}),0);_tmp83D;});_tmp83C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp83B;});_tmp83A;});break;}case 318: _LL318: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp846=_cycalloc(sizeof(*_tmp846));
_tmp846[0]=({struct Cyc_PatternList_tok_struct _tmp847;_tmp847.tag=Cyc_PatternList_tok;
_tmp847.f1=({struct Cyc_List_List*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848->hd=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp848->tl=0;_tmp848;});_tmp847;});_tmp846;});break;case 319: _LL319: yyval=(void*)({
struct Cyc_PatternList_tok_struct*_tmp849=_cycalloc(sizeof(*_tmp849));_tmp849[0]=({
struct Cyc_PatternList_tok_struct _tmp84A;_tmp84A.tag=Cyc_PatternList_tok;_tmp84A.f1=({
struct Cyc_List_List*_tmp84B=_cycalloc(sizeof(*_tmp84B));_tmp84B->hd=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp84B->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp84B;});_tmp84A;});
_tmp849;});break;case 320: _LL31A: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_FieldPattern_tok_struct
_tmp84D;_tmp84D.tag=Cyc_FieldPattern_tok;_tmp84D.f1=({struct _tuple11*_tmp84E=
_cycalloc(sizeof(*_tmp84E));_tmp84E->f1=0;_tmp84E->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp84E;});_tmp84D;});
_tmp84C;});break;case 321: _LL31B: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F[0]=({struct Cyc_FieldPattern_tok_struct
_tmp850;_tmp850.tag=Cyc_FieldPattern_tok;_tmp850.f1=({struct _tuple11*_tmp851=
_cycalloc(sizeof(*_tmp851));_tmp851->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp851->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp851;});_tmp850;});
_tmp84F;});break;case 322: _LL31C: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp853;_tmp853.tag=Cyc_FieldPatternList_tok;_tmp853.f1=({struct Cyc_List_List*
_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp854->tl=0;_tmp854;});
_tmp853;});_tmp852;});break;case 323: _LL31D: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp856;_tmp856.tag=Cyc_FieldPatternList_tok;_tmp856.f1=({struct Cyc_List_List*
_tmp857=_cycalloc(sizeof(*_tmp857));_tmp857->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp857->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp857;});_tmp856;});
_tmp855;});break;case 324: _LL31E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL31F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Exp_tok_struct _tmp859;
_tmp859.tag=Cyc_Exp_tok;_tmp859.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
_tmp858;});break;case 326: _LL320: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL321: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Exp_tok_struct _tmp85B;
_tmp85B.tag=Cyc_Exp_tok;_tmp85B.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
_tmp85A;});break;case 328: _LL322: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C[0]=({struct Cyc_Primopopt_tok_struct
_tmp85D;_tmp85D.tag=Cyc_Primopopt_tok;_tmp85D.f1=0;_tmp85D;});_tmp85C;});break;
case 329: _LL323: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp85E=_cycalloc(
sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_Primopopt_tok_struct _tmp85F;_tmp85F.tag=
Cyc_Primopopt_tok;_tmp85F.f1=({struct Cyc_Core_Opt*_tmp860=_cycalloc(sizeof(*
_tmp860));_tmp860->v=(void*)((void*)1);_tmp860;});_tmp85F;});_tmp85E;});break;
case 330: _LL324: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp861=_cycalloc(
sizeof(*_tmp861));_tmp861[0]=({struct Cyc_Primopopt_tok_struct _tmp862;_tmp862.tag=
Cyc_Primopopt_tok;_tmp862.f1=({struct Cyc_Core_Opt*_tmp863=_cycalloc(sizeof(*
_tmp863));_tmp863->v=(void*)((void*)3);_tmp863;});_tmp862;});_tmp861;});break;
case 331: _LL325: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp864=_cycalloc(
sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Primopopt_tok_struct _tmp865;_tmp865.tag=
Cyc_Primopopt_tok;_tmp865.f1=({struct Cyc_Core_Opt*_tmp866=_cycalloc(sizeof(*
_tmp866));_tmp866->v=(void*)((void*)4);_tmp866;});_tmp865;});_tmp864;});break;
case 332: _LL326: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp867=_cycalloc(
sizeof(*_tmp867));_tmp867[0]=({struct Cyc_Primopopt_tok_struct _tmp868;_tmp868.tag=
Cyc_Primopopt_tok;_tmp868.f1=({struct Cyc_Core_Opt*_tmp869=_cycalloc(sizeof(*
_tmp869));_tmp869->v=(void*)((void*)0);_tmp869;});_tmp868;});_tmp867;});break;
case 333: _LL327: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp86A=_cycalloc(
sizeof(*_tmp86A));_tmp86A[0]=({struct Cyc_Primopopt_tok_struct _tmp86B;_tmp86B.tag=
Cyc_Primopopt_tok;_tmp86B.f1=({struct Cyc_Core_Opt*_tmp86C=_cycalloc(sizeof(*
_tmp86C));_tmp86C->v=(void*)((void*)2);_tmp86C;});_tmp86B;});_tmp86A;});break;
case 334: _LL328: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp86D=_cycalloc(
sizeof(*_tmp86D));_tmp86D[0]=({struct Cyc_Primopopt_tok_struct _tmp86E;_tmp86E.tag=
Cyc_Primopopt_tok;_tmp86E.f1=({struct Cyc_Core_Opt*_tmp86F=_cycalloc(sizeof(*
_tmp86F));_tmp86F->v=(void*)((void*)16);_tmp86F;});_tmp86E;});_tmp86D;});break;
case 335: _LL329: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp870=_cycalloc(
sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Primopopt_tok_struct _tmp871;_tmp871.tag=
Cyc_Primopopt_tok;_tmp871.f1=({struct Cyc_Core_Opt*_tmp872=_cycalloc(sizeof(*
_tmp872));_tmp872->v=(void*)((void*)17);_tmp872;});_tmp871;});_tmp870;});break;
case 336: _LL32A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp873=_cycalloc(
sizeof(*_tmp873));_tmp873[0]=({struct Cyc_Primopopt_tok_struct _tmp874;_tmp874.tag=
Cyc_Primopopt_tok;_tmp874.f1=({struct Cyc_Core_Opt*_tmp875=_cycalloc(sizeof(*
_tmp875));_tmp875->v=(void*)((void*)13);_tmp875;});_tmp874;});_tmp873;});break;
case 337: _LL32B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp876=_cycalloc(
sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Primopopt_tok_struct _tmp877;_tmp877.tag=
Cyc_Primopopt_tok;_tmp877.f1=({struct Cyc_Core_Opt*_tmp878=_cycalloc(sizeof(*
_tmp878));_tmp878->v=(void*)((void*)15);_tmp878;});_tmp877;});_tmp876;});break;
case 338: _LL32C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp879=_cycalloc(
sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Primopopt_tok_struct _tmp87A;_tmp87A.tag=
Cyc_Primopopt_tok;_tmp87A.f1=({struct Cyc_Core_Opt*_tmp87B=_cycalloc(sizeof(*
_tmp87B));_tmp87B->v=(void*)((void*)14);_tmp87B;});_tmp87A;});_tmp879;});break;
case 339: _LL32D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 340: _LL32E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87C=_cycalloc(
sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Exp_tok_struct _tmp87D;_tmp87D.tag=Cyc_Exp_tok;
_tmp87D.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87D;});
_tmp87C;});break;case 341: _LL32F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87E=
_cycalloc(sizeof(*_tmp87E));_tmp87E[0]=({struct Cyc_Exp_tok_struct _tmp87F;_tmp87F.tag=
Cyc_Exp_tok;_tmp87F.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87F;});
_tmp87E;});break;case 342: _LL330: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp880=
_cycalloc(sizeof(*_tmp880));_tmp880[0]=({struct Cyc_Exp_tok_struct _tmp881;_tmp881.tag=
Cyc_Exp_tok;_tmp881.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp881;});
_tmp880;});break;case 343: _LL331: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp882=
_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Exp_tok_struct _tmp883;_tmp883.tag=
Cyc_Exp_tok;_tmp883.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp883;});
_tmp882;});break;case 344: _LL332: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp884=
_cycalloc(sizeof(*_tmp884));_tmp884[0]=({struct Cyc_Exp_tok_struct _tmp885;_tmp885.tag=
Cyc_Exp_tok;_tmp885.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
_tmp884;});break;case 345: _LL333: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp886=
_cycalloc(sizeof(*_tmp886));_tmp886[0]=({struct Cyc_Exp_tok_struct _tmp887;_tmp887.tag=
Cyc_Exp_tok;_tmp887.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
_tmp886;});break;case 346: _LL334: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 347: _LL335: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 348: _LL336: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888[0]=({struct Cyc_Exp_tok_struct _tmp889;
_tmp889.tag=Cyc_Exp_tok;_tmp889.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp889;});
_tmp888;});break;case 349: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 350: _LL338: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88A=_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=({struct Cyc_Exp_tok_struct _tmp88B;
_tmp88B.tag=Cyc_Exp_tok;_tmp88B.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
_tmp88A;});break;case 351: _LL339: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL33A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Exp_tok_struct _tmp88D;
_tmp88D.tag=Cyc_Exp_tok;_tmp88D.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88D;});
_tmp88C;});break;case 353: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 354: _LL33C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=({struct Cyc_Exp_tok_struct _tmp88F;
_tmp88F.tag=Cyc_Exp_tok;_tmp88F.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88F;});
_tmp88E;});break;case 355: _LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 356: _LL33E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890[0]=({struct Cyc_Exp_tok_struct _tmp891;
_tmp891.tag=Cyc_Exp_tok;_tmp891.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp891;});
_tmp890;});break;case 357: _LL33F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 358: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Exp_tok_struct _tmp893;
_tmp893.tag=Cyc_Exp_tok;_tmp893.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp893;});
_tmp892;});break;case 359: _LL341: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp894=
_cycalloc(sizeof(*_tmp894));_tmp894[0]=({struct Cyc_Exp_tok_struct _tmp895;_tmp895.tag=
Cyc_Exp_tok;_tmp895.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp895;});
_tmp894;});break;case 360: _LL342: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 361: _LL343: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Exp_tok_struct _tmp897;
_tmp897.tag=Cyc_Exp_tok;_tmp897.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
_tmp896;});break;case 362: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp898=
_cycalloc(sizeof(*_tmp898));_tmp898[0]=({struct Cyc_Exp_tok_struct _tmp899;_tmp899.tag=
Cyc_Exp_tok;_tmp899.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
_tmp898;});break;case 363: _LL345: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89A=
_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Exp_tok_struct _tmp89B;_tmp89B.tag=
Cyc_Exp_tok;_tmp89B.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89B;});
_tmp89A;});break;case 364: _LL346: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89C=
_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=({struct Cyc_Exp_tok_struct _tmp89D;_tmp89D.tag=
Cyc_Exp_tok;_tmp89D.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89D;});
_tmp89C;});break;case 365: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 366: _LL348: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Exp_tok_struct _tmp89F;
_tmp89F.tag=Cyc_Exp_tok;_tmp89F.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89F;});
_tmp89E;});break;case 367: _LL349: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A0=
_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({struct Cyc_Exp_tok_struct _tmp8A1;_tmp8A1.tag=
Cyc_Exp_tok;_tmp8A1.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A1;});
_tmp8A0;});break;case 368: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 369: _LL34B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2[0]=({struct Cyc_Exp_tok_struct _tmp8A3;
_tmp8A3.tag=Cyc_Exp_tok;_tmp8A3.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A3;});
_tmp8A2;});break;case 370: _LL34C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A4=
_cycalloc(sizeof(*_tmp8A4));_tmp8A4[0]=({struct Cyc_Exp_tok_struct _tmp8A5;_tmp8A5.tag=
Cyc_Exp_tok;_tmp8A5.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A5;});
_tmp8A4;});break;case 371: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 372: _LL34E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6[0]=({struct Cyc_Exp_tok_struct _tmp8A7;
_tmp8A7.tag=Cyc_Exp_tok;_tmp8A7.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A7;});
_tmp8A6;});break;case 373: _LL34F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A8=
_cycalloc(sizeof(*_tmp8A8));_tmp8A8[0]=({struct Cyc_Exp_tok_struct _tmp8A9;_tmp8A9.tag=
Cyc_Exp_tok;_tmp8A9.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A9;});
_tmp8A8;});break;case 374: _LL350: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8AA=
_cycalloc(sizeof(*_tmp8AA));_tmp8AA[0]=({struct Cyc_Exp_tok_struct _tmp8AB;_tmp8AB.tag=
Cyc_Exp_tok;_tmp8AB.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AB;});
_tmp8AA;});break;case 375: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 376: _LL352: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=({struct Cyc_Exp_tok_struct _tmp8AD;
_tmp8AD.tag=Cyc_Exp_tok;_tmp8AD.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AD;});
_tmp8AC;});break;case 377: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 378: _LL354: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8AE=_cycalloc(sizeof(*_tmp8AE));_tmp8AE[0]=({struct Cyc_Exp_tok_struct _tmp8AF;
_tmp8AF.tag=Cyc_Exp_tok;_tmp8AF.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AF;});
_tmp8AE;});break;case 379: _LL355: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B0=
_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=({struct Cyc_Exp_tok_struct _tmp8B1;_tmp8B1.tag=
Cyc_Exp_tok;_tmp8B1.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B1;});
_tmp8B0;});break;case 380: _LL356: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B2=
_cycalloc(sizeof(*_tmp8B2));_tmp8B2[0]=({struct Cyc_Exp_tok_struct _tmp8B3;_tmp8B3.tag=
Cyc_Exp_tok;_tmp8B3.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B3;});
_tmp8B2;});break;case 381: _LL357: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B4=
_cycalloc(sizeof(*_tmp8B4));_tmp8B4[0]=({struct Cyc_Exp_tok_struct _tmp8B5;_tmp8B5.tag=
Cyc_Exp_tok;_tmp8B5.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B5;});
_tmp8B4;});break;case 382: _LL358: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 383: _LL359: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8B6=_cycalloc(sizeof(*_tmp8B6));_tmp8B6[0]=({struct Cyc_Exp_tok_struct _tmp8B7;
_tmp8B7.tag=Cyc_Exp_tok;_tmp8B7.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B7;});
_tmp8B6;});break;case 384: _LL35A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B8=
_cycalloc(sizeof(*_tmp8B8));_tmp8B8[0]=({struct Cyc_Exp_tok_struct _tmp8B9;_tmp8B9.tag=
Cyc_Exp_tok;_tmp8B9.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B9;});
_tmp8B8;});break;case 385: _LL35B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BA=
_cycalloc(sizeof(*_tmp8BA));_tmp8BA[0]=({struct Cyc_Exp_tok_struct _tmp8BB;_tmp8BB.tag=
Cyc_Exp_tok;_tmp8BB.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BB;});
_tmp8BA;});break;case 386: _LL35C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BC=
_cycalloc(sizeof(*_tmp8BC));_tmp8BC[0]=({struct Cyc_Exp_tok_struct _tmp8BD;_tmp8BD.tag=
Cyc_Exp_tok;_tmp8BD.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp8BE=_cycalloc(sizeof(*_tmp8BE));_tmp8BE[0]=({struct Cyc_Absyn_StructField_struct
_tmp8BF;_tmp8BF.tag=0;_tmp8BF.f1=({struct _tagged_arr*_tmp8C0=_cycalloc(sizeof(*
_tmp8C0));_tmp8C0[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp8C0;});_tmp8BF;});_tmp8BE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BD;});
_tmp8BC;});break;case 387: _LL35D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C1=
_cycalloc(sizeof(*_tmp8C1));_tmp8C1[0]=({struct Cyc_Exp_tok_struct _tmp8C2;_tmp8C2.tag=
Cyc_Exp_tok;_tmp8C2.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp8C3=_cycalloc_atomic(sizeof(*_tmp8C3));_tmp8C3[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp8C4;_tmp8C4.tag=1;_tmp8C4.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8C4;});_tmp8C3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8C2;});_tmp8C1;});break;case 388: _LL35E: {struct
Cyc_Position_Segment*_tmp8C5=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp8C6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp8C5,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C7=_cycalloc(
sizeof(*_tmp8C7));_tmp8C7[0]=({struct Cyc_Exp_tok_struct _tmp8C8;_tmp8C8.tag=Cyc_Exp_tok;
_tmp8C8.f1=Cyc_Absyn_gentyp_exp(_tmp8C6,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C8;});
_tmp8C7;});break;}case 389: _LL35F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C9=
_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({struct Cyc_Exp_tok_struct _tmp8CA;_tmp8CA.tag=
Cyc_Exp_tok;_tmp8CA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8CC;_tmp8CC.tag=33;_tmp8CC.f1=({struct Cyc_Absyn_MallocInfo _tmp8CD;_tmp8CD.is_calloc=
0;_tmp8CD.rgn=0;_tmp8CD.elt_type=0;_tmp8CD.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8CD.fat_result=0;
_tmp8CD;});_tmp8CC;});_tmp8CB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CA;});
_tmp8C9;});break;case 390: _LL360: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CE=
_cycalloc(sizeof(*_tmp8CE));_tmp8CE[0]=({struct Cyc_Exp_tok_struct _tmp8CF;_tmp8CF.tag=
Cyc_Exp_tok;_tmp8CF.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8D0=_cycalloc(sizeof(*_tmp8D0));_tmp8D0[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8D1;_tmp8D1.tag=33;_tmp8D1.f1=({struct Cyc_Absyn_MallocInfo _tmp8D2;_tmp8D2.is_calloc=
0;_tmp8D2.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8D2.elt_type=0;
_tmp8D2.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8D2.fat_result=0;_tmp8D2;});_tmp8D1;});_tmp8D0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CF;});
_tmp8CE;});break;case 391: _LL361: {void*_tmp8D4;struct _tuple2 _tmp8D3=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8D4=_tmp8D3.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));
_tmp8D5[0]=({struct Cyc_Exp_tok_struct _tmp8D6;_tmp8D6.tag=Cyc_Exp_tok;_tmp8D6.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8D7=_cycalloc(
sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8D8;_tmp8D8.tag=
33;_tmp8D8.f1=({struct Cyc_Absyn_MallocInfo _tmp8D9;_tmp8D9.is_calloc=1;_tmp8D9.rgn=
0;_tmp8D9.elt_type=({void**_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA[0]=
_tmp8D4;_tmp8DA;});_tmp8D9.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8D9.fat_result=0;
_tmp8D9;});_tmp8D8;});_tmp8D7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D6;});
_tmp8D5;});break;}case 392: _LL362: {void*_tmp8DC;struct _tuple2 _tmp8DB=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8DC=_tmp8DB.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8DD=_cycalloc(sizeof(*_tmp8DD));
_tmp8DD[0]=({struct Cyc_Exp_tok_struct _tmp8DE;_tmp8DE.tag=Cyc_Exp_tok;_tmp8DE.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8DF=_cycalloc(
sizeof(*_tmp8DF));_tmp8DF[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8E0;_tmp8E0.tag=
33;_tmp8E0.f1=({struct Cyc_Absyn_MallocInfo _tmp8E1;_tmp8E1.is_calloc=1;_tmp8E1.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp8E1.elt_type=({void**_tmp8E2=_cycalloc(sizeof(*_tmp8E2));
_tmp8E2[0]=_tmp8DC;_tmp8E2;});_tmp8E1.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8E1.fat_result=0;
_tmp8E1;});_tmp8E0;});_tmp8DF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8DE;});
_tmp8DD;});break;}case 393: _LL363: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E3=_cycalloc(sizeof(*_tmp8E3));_tmp8E3[0]=({struct Cyc_Primop_tok_struct
_tmp8E4;_tmp8E4.tag=Cyc_Primop_tok;_tmp8E4.f1=(void*)((void*)12);_tmp8E4;});
_tmp8E3;});break;case 394: _LL364: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E5=_cycalloc(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Primop_tok_struct
_tmp8E6;_tmp8E6.tag=Cyc_Primop_tok;_tmp8E6.f1=(void*)((void*)11);_tmp8E6;});
_tmp8E5;});break;case 395: _LL365: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({struct Cyc_Primop_tok_struct
_tmp8E8;_tmp8E8.tag=Cyc_Primop_tok;_tmp8E8.f1=(void*)((void*)2);_tmp8E8;});
_tmp8E7;});break;case 396: _LL366: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 397: _LL367: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8E9=_cycalloc(sizeof(*_tmp8E9));_tmp8E9[0]=({struct Cyc_Exp_tok_struct _tmp8EA;
_tmp8EA.tag=Cyc_Exp_tok;_tmp8EA.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EA;});
_tmp8E9;});break;case 398: _LL368: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EB=
_cycalloc(sizeof(*_tmp8EB));_tmp8EB[0]=({struct Cyc_Exp_tok_struct _tmp8EC;_tmp8EC.tag=
Cyc_Exp_tok;_tmp8EC.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EC;});
_tmp8EB;});break;case 399: _LL369: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8ED=
_cycalloc(sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Exp_tok_struct _tmp8EE;_tmp8EE.tag=
Cyc_Exp_tok;_tmp8EE.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EE;});
_tmp8ED;});break;case 400: _LL36A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EF=
_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Exp_tok_struct _tmp8F0;_tmp8F0.tag=
Cyc_Exp_tok;_tmp8F0.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8F1=_cycalloc(sizeof(*_tmp8F1));_tmp8F1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8F1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F0;});
_tmp8EF;});break;case 401: _LL36B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F2=
_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=({struct Cyc_Exp_tok_struct _tmp8F3;_tmp8F3.tag=
Cyc_Exp_tok;_tmp8F3.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8F4=_cycalloc(sizeof(*_tmp8F4));_tmp8F4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8F4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F3;});
_tmp8F2;});break;case 402: _LL36C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F5=
_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct Cyc_Exp_tok_struct _tmp8F6;_tmp8F6.tag=
Cyc_Exp_tok;_tmp8F6.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F6;});
_tmp8F5;});break;case 403: _LL36D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F7=
_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Exp_tok_struct _tmp8F8;_tmp8F8.tag=
Cyc_Exp_tok;_tmp8F8.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F8;});
_tmp8F7;});break;case 404: _LL36E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F9=
_cycalloc(sizeof(*_tmp8F9));_tmp8F9[0]=({struct Cyc_Exp_tok_struct _tmp8FA;_tmp8FA.tag=
Cyc_Exp_tok;_tmp8FA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp8FC;_tmp8FC.tag=25;_tmp8FC.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8FC.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8FC;});_tmp8FB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8FA;});_tmp8F9;});break;case 405: _LL36F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8FD=_cycalloc(sizeof(*_tmp8FD));_tmp8FD[0]=({
struct Cyc_Exp_tok_struct _tmp8FE;_tmp8FE.tag=Cyc_Exp_tok;_tmp8FE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));
_tmp8FF[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp900;_tmp900.tag=25;_tmp900.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp900.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp900;});_tmp8FF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FE;});
_tmp8FD;});break;case 406: _LL370: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp901=
_cycalloc(sizeof(*_tmp901));_tmp901[0]=({struct Cyc_Exp_tok_struct _tmp902;_tmp902.tag=
Cyc_Exp_tok;_tmp902.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp903=_cycalloc(sizeof(*_tmp903));_tmp903[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp904;_tmp904.tag=37;_tmp904.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp904;});_tmp903;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp902;});_tmp901;});break;case 407: _LL371: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp905=_cycalloc(sizeof(*_tmp905));_tmp905[0]=({
struct Cyc_Exp_tok_struct _tmp906;_tmp906.tag=Cyc_Exp_tok;_tmp906.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp907=_cycalloc(sizeof(*_tmp907));
_tmp907[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp908;_tmp908.tag=36;_tmp908.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp908;});_tmp907;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp906;});
_tmp905;});break;case 408: _LL372: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp909=
_cycalloc(sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Exp_tok_struct _tmp90A;_tmp90A.tag=
Cyc_Exp_tok;_tmp90A.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp90B=_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp90C;_tmp90C.tag=2;_tmp90C.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90C;});_tmp90B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90A;});
_tmp909;});break;case 409: _LL373: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 410: _LL374: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D[0]=({struct Cyc_Exp_tok_struct _tmp90E;
_tmp90E.tag=Cyc_Exp_tok;_tmp90E.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90E;});
_tmp90D;});break;case 411: _LL375: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 412: _LL376: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp90F=_cycalloc(sizeof(*_tmp90F));_tmp90F[0]=({struct Cyc_Exp_tok_struct _tmp910;
_tmp910.tag=Cyc_Exp_tok;_tmp910.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp910;});
_tmp90F;});break;case 413: _LL377: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp911=
_cycalloc(sizeof(*_tmp911));_tmp911[0]=({struct Cyc_Exp_tok_struct _tmp912;_tmp912.tag=
Cyc_Exp_tok;_tmp912.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp912;});
_tmp911;});break;case 414: _LL378: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp913=
_cycalloc(sizeof(*_tmp913));_tmp913[0]=({struct Cyc_Exp_tok_struct _tmp914;_tmp914.tag=
Cyc_Exp_tok;_tmp914.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp914;});
_tmp913;});break;case 415: _LL379: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp915=
_cycalloc(sizeof(*_tmp915));_tmp915[0]=({struct Cyc_Exp_tok_struct _tmp916;_tmp916.tag=
Cyc_Exp_tok;_tmp916.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp916;});
_tmp915;});break;case 416: _LL37A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp917=
_cycalloc(sizeof(*_tmp917));_tmp917[0]=({struct Cyc_Exp_tok_struct _tmp918;_tmp918.tag=
Cyc_Exp_tok;_tmp918.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp919=_cycalloc(sizeof(*_tmp919));_tmp919[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp91A;_tmp91A.tag=28;_tmp91A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp91A.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp91A.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp91A.f4=0;
_tmp91A;});_tmp919;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp918;});
_tmp917;});break;case 417: _LL37B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp91B=
_cycalloc(sizeof(*_tmp91B));_tmp91B[0]=({struct Cyc_Exp_tok_struct _tmp91C;_tmp91C.tag=
Cyc_Exp_tok;_tmp91C.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp91C;});
_tmp91B;});break;case 418: _LL37C: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp91D=_cycalloc(sizeof(*_tmp91D));_tmp91D[0]=({struct Cyc_ExpList_tok_struct
_tmp91E;_tmp91E.tag=Cyc_ExpList_tok;_tmp91E.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp91E;});_tmp91D;});
break;case 419: _LL37D: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp91F=
_cycalloc(sizeof(*_tmp91F));_tmp91F[0]=({struct Cyc_ExpList_tok_struct _tmp920;
_tmp920.tag=Cyc_ExpList_tok;_tmp920.f1=({struct Cyc_List_List*_tmp921=_cycalloc(
sizeof(*_tmp921));_tmp921->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp921->tl=0;_tmp921;});
_tmp920;});_tmp91F;});break;case 420: _LL37E: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp922=_cycalloc(sizeof(*_tmp922));_tmp922[0]=({struct Cyc_ExpList_tok_struct
_tmp923;_tmp923.tag=Cyc_ExpList_tok;_tmp923.f1=({struct Cyc_List_List*_tmp924=
_cycalloc(sizeof(*_tmp924));_tmp924->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp924->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp924;});_tmp923;});
_tmp922;});break;case 421: _LL37F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp925=
_cycalloc(sizeof(*_tmp925));_tmp925[0]=({struct Cyc_Exp_tok_struct _tmp926;_tmp926.tag=
Cyc_Exp_tok;_tmp926.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp926;});
_tmp925;});break;case 422: _LL380: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp927=
_cycalloc(sizeof(*_tmp927));_tmp927[0]=({struct Cyc_Exp_tok_struct _tmp928;_tmp928.tag=
Cyc_Exp_tok;_tmp928.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp928;});
_tmp927;});break;case 423: _LL381: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp929=
_cycalloc(sizeof(*_tmp929));_tmp929[0]=({struct Cyc_Exp_tok_struct _tmp92A;_tmp92A.tag=
Cyc_Exp_tok;_tmp92A.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92A;});
_tmp929;});break;case 424: _LL382: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp92B=
_cycalloc(sizeof(*_tmp92B));_tmp92B[0]=({struct Cyc_Exp_tok_struct _tmp92C;_tmp92C.tag=
Cyc_Exp_tok;_tmp92C.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92C;});
_tmp92B;});break;case 425: _LL383: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D[0]=({struct Cyc_QualId_tok_struct
_tmp92E;_tmp92E.tag=Cyc_QualId_tok;_tmp92E.f1=({struct _tuple1*_tmp92F=_cycalloc(
sizeof(*_tmp92F));_tmp92F->f1=Cyc_Absyn_rel_ns_null;_tmp92F->f2=({struct
_tagged_arr*_tmp930=_cycalloc(sizeof(*_tmp930));_tmp930[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp930;});_tmp92F;});
_tmp92E;});_tmp92D;});break;case 426: _LL384: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 427: _LL385: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp931=_cycalloc(sizeof(*_tmp931));_tmp931[
0]=({struct Cyc_QualId_tok_struct _tmp932;_tmp932.tag=Cyc_QualId_tok;_tmp932.f1=({
struct _tuple1*_tmp933=_cycalloc(sizeof(*_tmp933));_tmp933->f1=Cyc_Absyn_rel_ns_null;
_tmp933->f2=({struct _tagged_arr*_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp934;});_tmp933;});_tmp932;});_tmp931;});break;case 428: _LL386: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 429: _LL387: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 430: _LL388:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 431:
_LL389: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
432: _LL38A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 433: _LL38B: break;case 434: _LL38C:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL38D: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(435,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(116,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0?yystate <= 
6288: 0)?Cyc_yycheck[_check_known_subscript_notnull(6289,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)]: 0)yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6289,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(116,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(883,yystate)];if(yyn > - 32768?yyn < 6288: 0){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 247 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6289,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)])
+ 15,count ++);}msg=({unsigned int _tmp935=(unsigned int)(sze + 15);char*_tmp936=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp935 + 1));struct _tagged_arr
_tmp938=_tag_arr(_tmp936,sizeof(char),_tmp935 + 1);{unsigned int _tmp937=_tmp935;
unsigned int i;for(i=0;i < _tmp937;i ++){_tmp936[i]='\000';}_tmp936[_tmp937]=(char)
0;}_tmp938;});Cyc_strcpy(msg,({const char*_tmp939="parse error";_tag_arr(_tmp939,
sizeof(char),_get_zero_arr_size(_tmp939,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 247 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(6289,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmp93A=", expecting `";
_tag_arr(_tmp93A,sizeof(char),_get_zero_arr_size(_tmp93A,14));}):(struct
_tagged_arr)({const char*_tmp93B=" or `";_tag_arr(_tmp93B,sizeof(char),
_get_zero_arr_size(_tmp93B,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(247,x)]);Cyc_strcat(msg,({const char*_tmp93C="'";
_tag_arr(_tmp93C,sizeof(char),_get_zero_arr_size(_tmp93C,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmp93D="parse error";
_tag_arr(_tmp93D,sizeof(char),_get_zero_arr_size(_tmp93D,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(883,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0?1: yyn > 6288)?1: Cyc_yycheck[_check_known_subscript_notnull(6289,
yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[_check_known_subscript_notnull(
6289,yyn)];if(yyn < 0){if(yyn == - 32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{
if(yyn == 0)goto yyerrpop;}if(yyn == 882)return 0;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;}
void Cyc_yyprint(int i,void*v){void*_tmp947=v;struct _tuple13*_tmp948;struct
_tuple13 _tmp949;int _tmp94A;char _tmp94B;short _tmp94C;struct _tagged_arr _tmp94D;
struct Cyc_Core_Opt*_tmp94E;struct Cyc_Core_Opt*_tmp94F;struct Cyc_Core_Opt _tmp950;
struct _tagged_arr*_tmp951;struct _tuple1*_tmp952;struct _tuple1 _tmp953;void*
_tmp954;struct _tagged_arr*_tmp955;_LL390: if(*((void**)_tmp947)!= Cyc_Int_tok)
goto _LL392;_tmp948=((struct Cyc_Int_tok_struct*)_tmp947)->f1;_tmp949=*_tmp948;
_tmp94A=_tmp949.f2;_LL391:({struct Cyc_Int_pa_struct _tmp958;_tmp958.tag=1;_tmp958.f1=(
unsigned int)_tmp94A;{void*_tmp956[1]={& _tmp958};Cyc_fprintf(Cyc_stderr,({const
char*_tmp957="%d";_tag_arr(_tmp957,sizeof(char),_get_zero_arr_size(_tmp957,3));}),
_tag_arr(_tmp956,sizeof(void*),1));}});goto _LL38F;_LL392: if(*((void**)_tmp947)!= 
Cyc_Char_tok)goto _LL394;_tmp94B=((struct Cyc_Char_tok_struct*)_tmp947)->f1;_LL393:({
struct Cyc_Int_pa_struct _tmp95B;_tmp95B.tag=1;_tmp95B.f1=(unsigned int)((int)
_tmp94B);{void*_tmp959[1]={& _tmp95B};Cyc_fprintf(Cyc_stderr,({const char*_tmp95A="%c";
_tag_arr(_tmp95A,sizeof(char),_get_zero_arr_size(_tmp95A,3));}),_tag_arr(_tmp959,
sizeof(void*),1));}});goto _LL38F;_LL394: if(*((void**)_tmp947)!= Cyc_Short_tok)
goto _LL396;_tmp94C=((struct Cyc_Short_tok_struct*)_tmp947)->f1;_LL395:({struct Cyc_Int_pa_struct
_tmp95E;_tmp95E.tag=1;_tmp95E.f1=(unsigned int)((int)_tmp94C);{void*_tmp95C[1]={&
_tmp95E};Cyc_fprintf(Cyc_stderr,({const char*_tmp95D="%ds";_tag_arr(_tmp95D,
sizeof(char),_get_zero_arr_size(_tmp95D,4));}),_tag_arr(_tmp95C,sizeof(void*),1));}});
goto _LL38F;_LL396: if(*((void**)_tmp947)!= Cyc_String_tok)goto _LL398;_tmp94D=((
struct Cyc_String_tok_struct*)_tmp947)->f1;_LL397:({struct Cyc_String_pa_struct
_tmp961;_tmp961.tag=0;_tmp961.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp94D);{
void*_tmp95F[1]={& _tmp961};Cyc_fprintf(Cyc_stderr,({const char*_tmp960="\"%s\"";
_tag_arr(_tmp960,sizeof(char),_get_zero_arr_size(_tmp960,5));}),_tag_arr(_tmp95F,
sizeof(void*),1));}});goto _LL38F;_LL398: if(*((void**)_tmp947)!= Cyc_Stringopt_tok)
goto _LL39A;_tmp94E=((struct Cyc_Stringopt_tok_struct*)_tmp947)->f1;if(_tmp94E != 0)
goto _LL39A;_LL399:({void*_tmp962[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp963="NULL";_tag_arr(_tmp963,sizeof(char),_get_zero_arr_size(_tmp963,5));}),
_tag_arr(_tmp962,sizeof(void*),0));});goto _LL38F;_LL39A: if(*((void**)_tmp947)!= 
Cyc_Stringopt_tok)goto _LL39C;_tmp94F=((struct Cyc_Stringopt_tok_struct*)_tmp947)->f1;
if(_tmp94F == 0)goto _LL39C;_tmp950=*_tmp94F;_tmp951=(struct _tagged_arr*)_tmp950.v;
_LL39B:({struct Cyc_String_pa_struct _tmp966;_tmp966.tag=0;_tmp966.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp951);{void*_tmp964[1]={& _tmp966};Cyc_fprintf(
Cyc_stderr,({const char*_tmp965="\"%s\"";_tag_arr(_tmp965,sizeof(char),
_get_zero_arr_size(_tmp965,5));}),_tag_arr(_tmp964,sizeof(void*),1));}});goto
_LL38F;_LL39C: if(*((void**)_tmp947)!= Cyc_QualId_tok)goto _LL39E;_tmp952=((struct
Cyc_QualId_tok_struct*)_tmp947)->f1;_tmp953=*_tmp952;_tmp954=_tmp953.f1;_tmp955=
_tmp953.f2;_LL39D: {struct Cyc_List_List*_tmp967=0;{void*_tmp968=_tmp954;struct
Cyc_List_List*_tmp969;struct Cyc_List_List*_tmp96A;_LL3A1: if(_tmp968 <= (void*)1?1:*((
int*)_tmp968)!= 0)goto _LL3A3;_tmp969=((struct Cyc_Absyn_Rel_n_struct*)_tmp968)->f1;
_LL3A2: _tmp967=_tmp969;goto _LL3A0;_LL3A3: if(_tmp968 <= (void*)1?1:*((int*)_tmp968)
!= 1)goto _LL3A5;_tmp96A=((struct Cyc_Absyn_Abs_n_struct*)_tmp968)->f1;_LL3A4:
_tmp967=_tmp96A;goto _LL3A0;_LL3A5: if((int)_tmp968 != 0)goto _LL3A0;_LL3A6: goto
_LL3A0;_LL3A0:;}for(0;_tmp967 != 0;_tmp967=_tmp967->tl){({struct Cyc_String_pa_struct
_tmp96D;_tmp96D.tag=0;_tmp96D.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp967->hd));{void*_tmp96B[1]={& _tmp96D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp96C="%s::";_tag_arr(_tmp96C,sizeof(char),_get_zero_arr_size(
_tmp96C,5));}),_tag_arr(_tmp96B,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp970;_tmp970.tag=0;_tmp970.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp955);{
void*_tmp96E[1]={& _tmp970};Cyc_fprintf(Cyc_stderr,({const char*_tmp96F="%s::";
_tag_arr(_tmp96F,sizeof(char),_get_zero_arr_size(_tmp96F,5));}),_tag_arr(_tmp96E,
sizeof(void*),1));}});goto _LL38F;}_LL39E:;_LL39F:({void*_tmp971[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp972="?";_tag_arr(_tmp972,sizeof(char),
_get_zero_arr_size(_tmp972,2));}),_tag_arr(_tmp971,sizeof(void*),0));});goto
_LL38F;_LL38F:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmp973=_cycalloc(
sizeof(*_tmp973));_tmp973->v=Cyc_Lexing_from_file(f);_tmp973;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
