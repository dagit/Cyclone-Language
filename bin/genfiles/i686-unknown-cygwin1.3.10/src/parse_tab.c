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
Cyc_Lex_register_typedef(_tmpF9);if(_tmpFD != 0)({void*_tmpFE[0]={};Cyc_Tcutil_warn(
loc,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmpFD->hd));{
void*_tmpFF[1]={& _tmp101};Cyc_aprintf(({const char*_tmp100="attribute %s within in typedef";
_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,31));}),_tag_arr(_tmpFF,
sizeof(void*),1));}}),_tag_arr(_tmpFE,sizeof(void*),0));});{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmp102=_tmpFB;struct Cyc_Core_Opt*_tmp103;
_LL7F: if(_tmp102 <= (void*)3?1:*((int*)_tmp102)!= 0)goto _LL81;_tmp103=((struct Cyc_Absyn_Evar_struct*)
_tmp102)->f1;_LL80: type=0;if(_tmp103 == 0)kind=({struct Cyc_Core_Opt*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104->v=(void*)((void*)2);_tmp104;});else{kind=
_tmp103;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105->v=(void*)_tmpFB;_tmp105;});goto _LL7E;_LL7E:;}
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp107;
_tmp107.tag=7;_tmp107.f1=({struct Cyc_Absyn_Typedefdecl*_tmp108=_cycalloc(sizeof(*
_tmp108));_tmp108->name=_tmpF9;_tmp108->tvs=_tmpFC;_tmp108->kind=kind;_tmp108->defn=
type;_tmp108;});_tmp107;});_tmp106;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp10A;_tmp10A.tag=12;_tmp10A.f1=d;_tmp10A.f2=s;
_tmp10A;});_tmp109;}),Cyc_Position_segment_join(d->loc,s->loc));}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(
Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10C;struct Cyc_Absyn_Tqual _tmp10D;struct
Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Parse_Declaration_spec*
_tmp10B=ds;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.tq;_tmp10E=_tmp10C.type_specs;
_tmp10F=_tmp10C.attributes;if(ds->is_inline)Cyc_Parse_err(({const char*_tmp110="inline is allowed only on function definitions";
_tag_arr(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,47));}),loc);if(_tmp10E
== 0){Cyc_Parse_err(({const char*_tmp111="missing type specifiers in declaration";
_tag_arr(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,39));}),loc);return 0;}{
void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp112=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp112 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp112 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp112 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp112 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp112 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp112 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp112 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp114;struct Cyc_List_List*
_tmp115;struct _tuple0 _tmp113=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{int exps_empty=1;{struct Cyc_List_List*
es=_tmp115;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple5 _tmp116=Cyc_Parse_collapse_type_specifiers(
_tmp10E,loc);if(_tmp114 == 0){void*_tmp118;struct Cyc_Core_Opt*_tmp119;struct
_tuple5 _tmp117=_tmp116;_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;if(_tmp119 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp119->v;{void*_tmp11A=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp11B;struct Cyc_Absyn_Aggrdecl*_tmp11C;struct Cyc_Absyn_Tuniondecl*
_tmp11D;_LL93: if(*((int*)_tmp11A)!= 6)goto _LL95;_tmp11B=((struct Cyc_Absyn_Enum_d_struct*)
_tmp11A)->f1;_LL94:(void*)(_tmp11B->sc=(void*)s);if(_tmp10F != 0)Cyc_Parse_err(({
const char*_tmp11E="bad attributes on enum";_tag_arr(_tmp11E,sizeof(char),
_get_zero_arr_size(_tmp11E,23));}),loc);goto _LL92;_LL95: if(*((int*)_tmp11A)!= 4)
goto _LL97;_tmp11C=((struct Cyc_Absyn_Aggr_d_struct*)_tmp11A)->f1;_LL96:(void*)(
_tmp11C->sc=(void*)s);_tmp11C->attributes=_tmp10F;goto _LL92;_LL97: if(*((int*)
_tmp11A)!= 5)goto _LL99;_tmp11D=((struct Cyc_Absyn_Tunion_d_struct*)_tmp11A)->f1;
_LL98:(void*)(_tmp11D->sc=(void*)s);if(_tmp10F != 0)Cyc_Parse_err(({const char*
_tmp11F="bad attributes on tunion";_tag_arr(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp120="bad declaration";_tag_arr(_tmp120,sizeof(char),
_get_zero_arr_size(_tmp120,16));}),loc);return 0;_LL92:;}return({struct Cyc_List_List*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=d;_tmp121->tl=0;_tmp121;});}
else{void*_tmp122=_tmp118;struct Cyc_Absyn_AggrInfo _tmp123;void*_tmp124;void*
_tmp125;struct _tuple1*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_TunionInfo
_tmp128;void*_tmp129;struct Cyc_Absyn_Tuniondecl**_tmp12A;struct Cyc_Absyn_TunionInfo
_tmp12B;void*_tmp12C;struct Cyc_Absyn_UnknownTunionInfo _tmp12D;struct _tuple1*
_tmp12E;int _tmp12F;struct Cyc_List_List*_tmp130;struct _tuple1*_tmp131;struct Cyc_List_List*
_tmp132;_LL9C: if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 10)goto _LL9E;_tmp123=((
struct Cyc_Absyn_AggrType_struct*)_tmp122)->f1;_tmp124=(void*)_tmp123.aggr_info;
if(*((int*)_tmp124)!= 0)goto _LL9E;_tmp125=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp124)->f1;_tmp126=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp124)->f2;_tmp127=
_tmp123.targs;_LL9D: {struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp127);
struct Cyc_Absyn_Aggrdecl*_tmp134=({struct Cyc_Absyn_Aggrdecl*_tmp139=_cycalloc(
sizeof(*_tmp139));_tmp139->kind=(void*)_tmp125;_tmp139->sc=(void*)s;_tmp139->name=
_tmp126;_tmp139->tvs=_tmp133;_tmp139->impl=0;_tmp139->attributes=0;_tmp139;});
if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp135="bad attributes on type declaration";
_tag_arr(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,35));}),loc);return({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp138;_tmp138.tag=4;_tmp138.f1=
_tmp134;_tmp138;});_tmp137;}),loc);_tmp136->tl=0;_tmp136;});}_LL9E: if(_tmp122 <= (
void*)3?1:*((int*)_tmp122)!= 2)goto _LLA0;_tmp128=((struct Cyc_Absyn_TunionType_struct*)
_tmp122)->f1;_tmp129=(void*)_tmp128.tunion_info;if(*((int*)_tmp129)!= 1)goto
_LLA0;_tmp12A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp129)->f1;_LL9F: if(
_tmp10F != 0)Cyc_Parse_err(({const char*_tmp13A="bad attributes on tunion";
_tag_arr(_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,25));}),loc);return({
struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp13D;_tmp13D.tag=5;_tmp13D.f1=*
_tmp12A;_tmp13D;});_tmp13C;}),loc);_tmp13B->tl=0;_tmp13B;});_LLA0: if(_tmp122 <= (
void*)3?1:*((int*)_tmp122)!= 2)goto _LLA2;_tmp12B=((struct Cyc_Absyn_TunionType_struct*)
_tmp122)->f1;_tmp12C=(void*)_tmp12B.tunion_info;if(*((int*)_tmp12C)!= 0)goto
_LLA2;_tmp12D=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12C)->f1;_tmp12E=
_tmp12D.name;_tmp12F=_tmp12D.is_xtunion;_tmp130=_tmp12B.targs;_LLA1: {struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp130);struct Cyc_Absyn_Decl*_tmp13F=Cyc_Absyn_tunion_decl(s,_tmp12E,
_tmp13E,0,_tmp12F,loc);if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on tunion";
_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,25));}),loc);return({
struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=_tmp13F;
_tmp141->tl=0;_tmp141;});}_LLA2: if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 12)
goto _LLA4;_tmp131=((struct Cyc_Absyn_EnumType_struct*)_tmp122)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->sc=(void*)s;_tmp148->name=_tmp131;_tmp148->fields=0;_tmp148;});
if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp143="bad attributes on enum";
_tag_arr(_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,23));}),loc);return({
struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=({struct Cyc_Absyn_Decl*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp147;_tmp147.tag=6;_tmp147.f1=_tmp142;_tmp147;});_tmp146;}));_tmp145->loc=loc;
_tmp145;});_tmp144->tl=0;_tmp144;});}_LLA4: if(_tmp122 <= (void*)3?1:*((int*)
_tmp122)!= 13)goto _LLA6;_tmp132=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp122)->f1;
_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp149=({struct Cyc_Absyn_Enumdecl*_tmp14F=
_cycalloc(sizeof(*_tmp14F));_tmp14F->sc=(void*)s;_tmp14F->name=Cyc_Parse_gensym_enum();
_tmp14F->fields=({struct Cyc_Core_Opt*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->v=
_tmp132;_tmp150;});_tmp14F;});if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp14A="bad attributes on enum";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,23));}),loc);return({
struct Cyc_List_List*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=({struct Cyc_Absyn_Decl*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14E;_tmp14E.tag=6;_tmp14E.f1=_tmp149;_tmp14E;});_tmp14D;}));_tmp14C->loc=loc;
_tmp14C;});_tmp14B->tl=0;_tmp14B;});}_LLA6:;_LLA7: Cyc_Parse_err(({const char*
_tmp151="missing declarator";_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(
_tmp151,19));}),loc);return 0;_LL9B:;}}else{void*t=_tmp116.f1;struct Cyc_List_List*
_tmp152=Cyc_Parse_apply_tmss(_tmp10D,t,_tmp114,_tmp10F);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp153="initializer in typedef declaration";
_tag_arr(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,35));}),loc);{struct Cyc_List_List*
decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp152);if(_tmp116.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp116.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp155;struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;
_LLA9: if(*((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp10F;
_tmp10F=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 5)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 6)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp158="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,70));}),loc);goto _LLA8;
_LLA8:;}decls=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=
d;_tmp159->tl=decls;_tmp159;});}if(_tmp10F != 0)({void*_tmp15A[0]={};Cyc_Tcutil_warn(
loc,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp10F->hd));{
void*_tmp15B[1]={& _tmp15D};Cyc_aprintf(({const char*_tmp15C="attribute %s in typedef; ignoring";
_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,34));}),_tag_arr(
_tmp15B,sizeof(void*),1));}}),_tag_arr(_tmp15A,sizeof(void*),0));});return decls;}}
else{if(_tmp116.f2 != 0)Cyc_Parse_unimp(({const char*_tmp15E="nested type declaration within declarator";
_tag_arr(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,42));}),loc);{struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp15F=_tmp152;for(0;_tmp15F != 0;(_tmp15F=_tmp15F->tl,
_tmp115=_tmp115->tl)){struct _tuple7 _tmp161;struct _tuple1*_tmp162;struct Cyc_Absyn_Tqual
_tmp163;void*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;
struct _tuple7*_tmp160=(struct _tuple7*)_tmp15F->hd;_tmp161=*_tmp160;_tmp162=
_tmp161.f1;_tmp163=_tmp161.f2;_tmp164=_tmp161.f3;_tmp165=_tmp161.f4;_tmp166=
_tmp161.f5;if(_tmp165 != 0)({void*_tmp167[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp168="bad type params, ignoring";_tag_arr(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,26));}),_tag_arr(_tmp167,sizeof(void*),0));});if(
_tmp115 == 0)({void*_tmp169[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp16A="unexpected NULL in parse!";
_tag_arr(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,26));}),_tag_arr(
_tmp169,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp16B=(struct Cyc_Absyn_Exp*)
_tmp115->hd;struct Cyc_Absyn_Vardecl*_tmp16C=Cyc_Absyn_new_vardecl(_tmp162,
_tmp164,_tmp16B);_tmp16C->tq=_tmp163;(void*)(_tmp16C->sc=(void*)s);_tmp16C->attributes=
_tmp166;{struct Cyc_Absyn_Decl*_tmp16D=({struct Cyc_Absyn_Decl*_tmp16F=_cycalloc(
sizeof(*_tmp16F));_tmp16F->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=({struct Cyc_Absyn_Var_d_struct
_tmp171;_tmp171.tag=0;_tmp171.f1=_tmp16C;_tmp171;});_tmp170;}));_tmp16F->loc=loc;
_tmp16F;});decls=({struct Cyc_List_List*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E->hd=_tmp16D;_tmp16E->tl=decls;_tmp16E;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3:
return(void*)2;case 'R': _LLB4: return(void*)3;case 'E': _LLB5: return(void*)4;case 'I':
_LLB6: return(void*)5;default: _LLB7: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp172[1]={& _tmp174};Cyc_aprintf(({const char*_tmp173="bad kind: %s";
_tag_arr(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,13));}),_tag_arr(
_tmp172,sizeof(void*),1));}}),loc);return(void*)2;}char Cyc_AbstractDeclarator_tok[
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
static char _tmp176[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp176,_tmp176,_tmp176 + 15}};struct _tuple13*Cyc_yyget_Int_tok(
void*yy1){struct _tuple13*yyzzz;{void*_tmp177=yy1;struct _tuple13*_tmp178;_LLBA:
if(*((void**)_tmp177)!= Cyc_Int_tok)goto _LLBC;_tmp178=((struct Cyc_Int_tok_struct*)
_tmp177)->f1;_LLBB: yyzzz=_tmp178;goto _LLB9;_LLBC:;_LLBD:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLB9:;}return yyzzz;}static char _tmp17A[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 5}};char Cyc_yyget_Char_tok(
void*yy1){char yyzzz;{void*_tmp17B=yy1;char _tmp17C;_LLBF: if(*((void**)_tmp17B)!= 
Cyc_Char_tok)goto _LLC1;_tmp17C=((struct Cyc_Char_tok_struct*)_tmp17B)->f1;_LLC0:
yyzzz=_tmp17C;goto _LLBE;_LLC1:;_LLC2:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLBE:;}return yyzzz;}static char _tmp17E[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp17F=
yy1;struct _tagged_arr _tmp180;_LLC4: if(*((void**)_tmp17F)!= Cyc_String_tok)goto
_LLC6;_tmp180=((struct Cyc_String_tok_struct*)_tmp17F)->f1;_LLC5: yyzzz=_tmp180;
goto _LLC3;_LLC6:;_LLC7:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLC3:;}return
yyzzz;}static char _tmp182[38]="$(conref_t<bool>,conref_t<bounds_t>)@";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp182,_tmp182,
_tmp182 + 38}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*yyzzz;{void*
_tmp183=yy1;struct _tuple16*_tmp184;_LLC9: if(*((void**)_tmp183)!= Cyc_YY1)goto
_LLCB;_tmp184=((struct Cyc_YY1_struct*)_tmp183)->f1;_LLCA: yyzzz=_tmp184;goto _LLC8;
_LLCB:;_LLCC:(int)_throw((void*)& Cyc_yyfail_YY1);_LLC8:;}return yyzzz;}static char
_tmp186[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp186,_tmp186,_tmp186 + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(
void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp187=yy1;struct Cyc_Absyn_Conref*
_tmp188;_LLCE: if(*((void**)_tmp187)!= Cyc_YY2)goto _LLD0;_tmp188=((struct Cyc_YY2_struct*)
_tmp187)->f1;_LLCF: yyzzz=_tmp188;goto _LLCD;_LLD0:;_LLD1:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLCD:;}return yyzzz;}static char _tmp18A[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp18A,_tmp18A,_tmp18A + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp18B=yy1;struct
Cyc_Absyn_Exp*_tmp18C;_LLD3: if(*((void**)_tmp18B)!= Cyc_Exp_tok)goto _LLD5;
_tmp18C=((struct Cyc_Exp_tok_struct*)_tmp18B)->f1;_LLD4: yyzzz=_tmp18C;goto _LLD2;
_LLD5:;_LLD6:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLD2:;}return yyzzz;}static
char _tmp18E[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp18E,_tmp18E,_tmp18E + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18F=yy1;struct Cyc_List_List*
_tmp190;_LLD8: if(*((void**)_tmp18F)!= Cyc_ExpList_tok)goto _LLDA;_tmp190=((struct
Cyc_ExpList_tok_struct*)_tmp18F)->f1;_LLD9: yyzzz=_tmp190;goto _LLD7;_LLDA:;_LLDB:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD7:;}return yyzzz;}static char
_tmp192[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp193=yy1;struct Cyc_List_List*_tmp194;_LLDD: if(*((void**)_tmp193)
!= Cyc_InitializerList_tok)goto _LLDF;_tmp194=((struct Cyc_InitializerList_tok_struct*)
_tmp193)->f1;_LLDE: yyzzz=_tmp194;goto _LLDC;_LLDF:;_LLE0:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLDC:;}return yyzzz;}static char _tmp196[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp197=yy1;void*_tmp198;_LLE2: if(*((void**)_tmp197)
!= Cyc_Primop_tok)goto _LLE4;_tmp198=(void*)((struct Cyc_Primop_tok_struct*)
_tmp197)->f1;_LLE3: yyzzz=_tmp198;goto _LLE1;_LLE4:;_LLE5:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLE1:;}return yyzzz;}static char _tmp19A[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp19A,_tmp19A,_tmp19A + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp19B=yy1;struct Cyc_Core_Opt*_tmp19C;_LLE7: if(*((void**)_tmp19B)!= Cyc_Primopopt_tok)
goto _LLE9;_tmp19C=((struct Cyc_Primopopt_tok_struct*)_tmp19B)->f1;_LLE8: yyzzz=
_tmp19C;goto _LLE6;_LLE9:;_LLEA:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE6:;}return yyzzz;}static char _tmp19E[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp19E,_tmp19E,_tmp19E + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp19F=yy1;
struct _tuple1*_tmp1A0;_LLEC: if(*((void**)_tmp19F)!= Cyc_QualId_tok)goto _LLEE;
_tmp1A0=((struct Cyc_QualId_tok_struct*)_tmp19F)->f1;_LLED: yyzzz=_tmp1A0;goto
_LLEB;_LLEE:;_LLEF:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLEB:;}return
yyzzz;}static char _tmp1A2[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1A3=yy1;struct Cyc_Absyn_Stmt*
_tmp1A4;_LLF1: if(*((void**)_tmp1A3)!= Cyc_Stmt_tok)goto _LLF3;_tmp1A4=((struct Cyc_Stmt_tok_struct*)
_tmp1A3)->f1;_LLF2: yyzzz=_tmp1A4;goto _LLF0;_LLF3:;_LLF4:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF0:;}return yyzzz;}static char _tmp1A6[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp1A6,_tmp1A6,_tmp1A6 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A7=yy1;struct Cyc_List_List*
_tmp1A8;_LLF6: if(*((void**)_tmp1A7)!= Cyc_SwitchClauseList_tok)goto _LLF8;_tmp1A8=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp1A7)->f1;_LLF7: yyzzz=_tmp1A8;goto
_LLF5;_LLF8:;_LLF9:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF5:;}
return yyzzz;}static char _tmp1AA[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1AB=yy1;struct Cyc_List_List*_tmp1AC;_LLFB: if(*((void**)_tmp1AB)
!= Cyc_SwitchCClauseList_tok)goto _LLFD;_tmp1AC=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp1AB)->f1;_LLFC: yyzzz=_tmp1AC;goto _LLFA;_LLFD:;_LLFE:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFA:;}return yyzzz;}static char _tmp1AE[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1AF=yy1;
struct Cyc_Absyn_Pat*_tmp1B0;_LL100: if(*((void**)_tmp1AF)!= Cyc_Pattern_tok)goto
_LL102;_tmp1B0=((struct Cyc_Pattern_tok_struct*)_tmp1AF)->f1;_LL101: yyzzz=_tmp1B0;
goto _LLFF;_LL102:;_LL103:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LLFF:;}
return yyzzz;}static char _tmp1B2[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1B2,_tmp1B2,_tmp1B2 + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1B3=yy1;struct Cyc_List_List*_tmp1B4;_LL105: if(*((void**)_tmp1B3)
!= Cyc_PatternList_tok)goto _LL107;_tmp1B4=((struct Cyc_PatternList_tok_struct*)
_tmp1B3)->f1;_LL106: yyzzz=_tmp1B4;goto _LL104;_LL107:;_LL108:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL104:;}return yyzzz;}static char _tmp1B6[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1B7=yy1;struct _tuple11*_tmp1B8;_LL10A: if(*((void**)
_tmp1B7)!= Cyc_FieldPattern_tok)goto _LL10C;_tmp1B8=((struct Cyc_FieldPattern_tok_struct*)
_tmp1B7)->f1;_LL10B: yyzzz=_tmp1B8;goto _LL109;_LL10C:;_LL10D:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL109:;}return yyzzz;}static char _tmp1BA[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1BA,_tmp1BA,_tmp1BA + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LL10F: if(*((void**)_tmp1BB)!= Cyc_FieldPatternList_tok)goto _LL111;
_tmp1BC=((struct Cyc_FieldPatternList_tok_struct*)_tmp1BB)->f1;_LL110: yyzzz=
_tmp1BC;goto _LL10E;_LL111:;_LL112:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL10E:;}return yyzzz;}static char _tmp1BE[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1BF=yy1;
struct Cyc_Absyn_Fndecl*_tmp1C0;_LL114: if(*((void**)_tmp1BF)!= Cyc_FnDecl_tok)
goto _LL116;_tmp1C0=((struct Cyc_FnDecl_tok_struct*)_tmp1BF)->f1;_LL115: yyzzz=
_tmp1C0;goto _LL113;_LL116:;_LL117:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL113:;}return yyzzz;}static char _tmp1C2[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;_LL119: if(*((void**)_tmp1C3)!= Cyc_DeclList_tok)
goto _LL11B;_tmp1C4=((struct Cyc_DeclList_tok_struct*)_tmp1C3)->f1;_LL11A: yyzzz=
_tmp1C4;goto _LL118;_LL11B:;_LL11C:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL118:;}return yyzzz;}static char _tmp1C6[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1C7=yy1;struct Cyc_Parse_Declaration_spec*_tmp1C8;_LL11E: if(*((
void**)_tmp1C7)!= Cyc_DeclSpec_tok)goto _LL120;_tmp1C8=((struct Cyc_DeclSpec_tok_struct*)
_tmp1C7)->f1;_LL11F: yyzzz=_tmp1C8;goto _LL11D;_LL120:;_LL121:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11D:;}return yyzzz;}static char _tmp1CA[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1CA,_tmp1CA,_tmp1CA + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1CB=yy1;struct _tuple12*_tmp1CC;_LL123: if(*((void**)
_tmp1CB)!= Cyc_InitDecl_tok)goto _LL125;_tmp1CC=((struct Cyc_InitDecl_tok_struct*)
_tmp1CB)->f1;_LL124: yyzzz=_tmp1CC;goto _LL122;_LL125:;_LL126:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL122:;}return yyzzz;}static char _tmp1CE[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1CE,_tmp1CE,_tmp1CE + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CF=yy1;struct Cyc_List_List*
_tmp1D0;_LL128: if(*((void**)_tmp1CF)!= Cyc_InitDeclList_tok)goto _LL12A;_tmp1D0=((
struct Cyc_InitDeclList_tok_struct*)_tmp1CF)->f1;_LL129: yyzzz=_tmp1D0;goto _LL127;
_LL12A:;_LL12B:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL127:;}return
yyzzz;}static char _tmp1D2[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1D2,_tmp1D2,_tmp1D2 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1D3=yy1;void*
_tmp1D4;_LL12D: if(*((void**)_tmp1D3)!= Cyc_StorageClass_tok)goto _LL12F;_tmp1D4=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1D3)->f1;_LL12E: yyzzz=_tmp1D4;
goto _LL12C;_LL12F:;_LL130:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12C:;}return yyzzz;}static char _tmp1D6[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1D7=yy1;void*
_tmp1D8;_LL132: if(*((void**)_tmp1D7)!= Cyc_TypeSpecifier_tok)goto _LL134;_tmp1D8=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1D7)->f1;_LL133: yyzzz=_tmp1D8;
goto _LL131;_LL134:;_LL135:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL131:;}return yyzzz;}static char _tmp1DA[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1DB=yy1;void*
_tmp1DC;_LL137: if(*((void**)_tmp1DB)!= Cyc_StructOrUnion_tok)goto _LL139;_tmp1DC=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1DB)->f1;_LL138: yyzzz=_tmp1DC;
goto _LL136;_LL139:;_LL13A:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL136:;}return yyzzz;}static char _tmp1DE[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1DF=yy1;struct Cyc_Absyn_Tqual _tmp1E0;_LL13C: if(*((void**)_tmp1DF)!= Cyc_TypeQual_tok)
goto _LL13E;_tmp1E0=((struct Cyc_TypeQual_tok_struct*)_tmp1DF)->f1;_LL13D: yyzzz=
_tmp1E0;goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13B:;}return yyzzz;}static char _tmp1E2[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1E2,_tmp1E2,_tmp1E2 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL141: if(*((void**)_tmp1E3)!= Cyc_AggrFieldDeclList_tok)goto _LL143;
_tmp1E4=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1E3)->f1;_LL142: yyzzz=
_tmp1E4;goto _LL140;_LL143:;_LL144:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL140:;}return yyzzz;}static char _tmp1E6[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1E6,_tmp1E6,_tmp1E6 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E7=yy1;struct Cyc_List_List*
_tmp1E8;_LL146: if(*((void**)_tmp1E7)!= Cyc_AggrFieldDeclListList_tok)goto _LL148;
_tmp1E8=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1E7)->f1;_LL147: yyzzz=
_tmp1E8;goto _LL145;_LL148:;_LL149:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL145:;}return yyzzz;}static char _tmp1EA[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1EA,_tmp1EA,_tmp1EA + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EB=yy1;struct Cyc_List_List*
_tmp1EC;_LL14B: if(*((void**)_tmp1EB)!= Cyc_TypeModifierList_tok)goto _LL14D;
_tmp1EC=((struct Cyc_TypeModifierList_tok_struct*)_tmp1EB)->f1;_LL14C: yyzzz=
_tmp1EC;goto _LL14A;_LL14D:;_LL14E:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14A:;}return yyzzz;}static char _tmp1EE[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1EF=yy1;struct Cyc_Parse_Declarator*_tmp1F0;_LL150: if(*((void**)
_tmp1EF)!= Cyc_Declarator_tok)goto _LL152;_tmp1F0=((struct Cyc_Declarator_tok_struct*)
_tmp1EF)->f1;_LL151: yyzzz=_tmp1F0;goto _LL14F;_LL152:;_LL153:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL14F:;}return yyzzz;}static char _tmp1F2[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1F2,_tmp1F2,_tmp1F2 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1F3=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1F4;_LL155: if(*((void**)_tmp1F3)!= Cyc_AbstractDeclarator_tok)goto _LL157;
_tmp1F4=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1F3)->f1;_LL156: yyzzz=
_tmp1F4;goto _LL154;_LL157:;_LL158:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL154:;}return yyzzz;}static char _tmp1F6[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1F7=yy1;int _tmp1F8;_LL15A: if(*((void**)_tmp1F7)!= 
Cyc_Bool_tok)goto _LL15C;_tmp1F8=((struct Cyc_Bool_tok_struct*)_tmp1F7)->f1;_LL15B:
yyzzz=_tmp1F8;goto _LL159;_LL15C:;_LL15D:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL159:;}return yyzzz;}static char _tmp1FA[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1FB=yy1;void*_tmp1FC;_LL15F: if(*((void**)_tmp1FB)
!= Cyc_Scope_tok)goto _LL161;_tmp1FC=(void*)((struct Cyc_Scope_tok_struct*)_tmp1FB)->f1;
_LL160: yyzzz=_tmp1FC;goto _LL15E;_LL161:;_LL162:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL15E:;}return yyzzz;}static char _tmp1FE[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp1FF=yy1;struct Cyc_Absyn_Tunionfield*_tmp200;_LL164: if(*((void**)
_tmp1FF)!= Cyc_TunionField_tok)goto _LL166;_tmp200=((struct Cyc_TunionField_tok_struct*)
_tmp1FF)->f1;_LL165: yyzzz=_tmp200;goto _LL163;_LL166:;_LL167:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL163:;}return yyzzz;}static char _tmp202[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp202,_tmp202,_tmp202 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp203=yy1;struct Cyc_List_List*
_tmp204;_LL169: if(*((void**)_tmp203)!= Cyc_TunionFieldList_tok)goto _LL16B;
_tmp204=((struct Cyc_TunionFieldList_tok_struct*)_tmp203)->f1;_LL16A: yyzzz=
_tmp204;goto _LL168;_LL16B:;_LL16C:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL168:;}return yyzzz;}static char _tmp206[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp206,_tmp206,_tmp206 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp207=yy1;struct _tuple15*_tmp208;_LL16E: if(*((void**)
_tmp207)!= Cyc_QualSpecList_tok)goto _LL170;_tmp208=((struct Cyc_QualSpecList_tok_struct*)
_tmp207)->f1;_LL16F: yyzzz=_tmp208;goto _LL16D;_LL170:;_LL171:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16D:;}return yyzzz;}static char _tmp20A[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp20A,_tmp20A,_tmp20A + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp20B=yy1;struct Cyc_List_List*_tmp20C;_LL173:
if(*((void**)_tmp20B)!= Cyc_IdList_tok)goto _LL175;_tmp20C=((struct Cyc_IdList_tok_struct*)
_tmp20B)->f1;_LL174: yyzzz=_tmp20C;goto _LL172;_LL175:;_LL176:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL172:;}return yyzzz;}static char _tmp20E[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp20E,_tmp20E,_tmp20E + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp20F=yy1;struct _tuple2*_tmp210;_LL178: if(*((void**)
_tmp20F)!= Cyc_ParamDecl_tok)goto _LL17A;_tmp210=((struct Cyc_ParamDecl_tok_struct*)
_tmp20F)->f1;_LL179: yyzzz=_tmp210;goto _LL177;_LL17A:;_LL17B:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL177:;}return yyzzz;}static char _tmp212[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp212,_tmp212,_tmp212 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp213=yy1;struct Cyc_List_List*
_tmp214;_LL17D: if(*((void**)_tmp213)!= Cyc_ParamDeclList_tok)goto _LL17F;_tmp214=((
struct Cyc_ParamDeclList_tok_struct*)_tmp213)->f1;_LL17E: yyzzz=_tmp214;goto _LL17C;
_LL17F:;_LL180:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17C:;}return
yyzzz;}static char _tmp216[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp216,
_tmp216,_tmp216 + 107}};struct _tuple14*Cyc_yyget_YY3(void*yy1){struct _tuple14*
yyzzz;{void*_tmp217=yy1;struct _tuple14*_tmp218;_LL182: if(*((void**)_tmp217)!= 
Cyc_YY3)goto _LL184;_tmp218=((struct Cyc_YY3_struct*)_tmp217)->f1;_LL183: yyzzz=
_tmp218;goto _LL181;_LL184:;_LL185:(int)_throw((void*)& Cyc_yyfail_YY3);_LL181:;}
return yyzzz;}static char _tmp21A[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp21B=yy1;struct Cyc_List_List*_tmp21C;_LL187: if(*((void**)_tmp21B)!= Cyc_TypeList_tok)
goto _LL189;_tmp21C=((struct Cyc_TypeList_tok_struct*)_tmp21B)->f1;_LL188: yyzzz=
_tmp21C;goto _LL186;_LL189:;_LL18A:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL186:;}return yyzzz;}static char _tmp21E[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp21E,
_tmp21E,_tmp21E + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp21F=yy1;struct Cyc_List_List*_tmp220;_LL18C:
if(*((void**)_tmp21F)!= Cyc_DesignatorList_tok)goto _LL18E;_tmp220=((struct Cyc_DesignatorList_tok_struct*)
_tmp21F)->f1;_LL18D: yyzzz=_tmp220;goto _LL18B;_LL18E:;_LL18F:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18B:;}return yyzzz;}static char _tmp222[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp222,_tmp222,_tmp222 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp223=yy1;void*_tmp224;_LL191: if(*((void**)_tmp223)!= Cyc_Designator_tok)
goto _LL193;_tmp224=(void*)((struct Cyc_Designator_tok_struct*)_tmp223)->f1;_LL192:
yyzzz=_tmp224;goto _LL190;_LL193:;_LL194:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL190:;}return yyzzz;}static char _tmp226[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp227=yy1;void*_tmp228;_LL196: if(*((void**)_tmp227)
!= Cyc_Kind_tok)goto _LL198;_tmp228=(void*)((struct Cyc_Kind_tok_struct*)_tmp227)->f1;
_LL197: yyzzz=_tmp228;goto _LL195;_LL198:;_LL199:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL195:;}return yyzzz;}static char _tmp22A[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp22A,_tmp22A,_tmp22A + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp22B=yy1;void*_tmp22C;_LL19B: if(*((void**)_tmp22B)
!= Cyc_Type_tok)goto _LL19D;_tmp22C=(void*)((struct Cyc_Type_tok_struct*)_tmp22B)->f1;
_LL19C: yyzzz=_tmp22C;goto _LL19A;_LL19D:;_LL19E:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19A:;}return yyzzz;}static char _tmp22E[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp22E,
_tmp22E,_tmp22E + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp22F=yy1;struct Cyc_List_List*_tmp230;_LL1A0:
if(*((void**)_tmp22F)!= Cyc_AttributeList_tok)goto _LL1A2;_tmp230=((struct Cyc_AttributeList_tok_struct*)
_tmp22F)->f1;_LL1A1: yyzzz=_tmp230;goto _LL19F;_LL1A2:;_LL1A3:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL19F:;}return yyzzz;}static char _tmp232[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp232,_tmp232,_tmp232 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp233=yy1;void*_tmp234;_LL1A5: if(*((void**)_tmp233)!= Cyc_Attribute_tok)
goto _LL1A7;_tmp234=(void*)((struct Cyc_Attribute_tok_struct*)_tmp233)->f1;_LL1A6:
yyzzz=_tmp234;goto _LL1A4;_LL1A7:;_LL1A8:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A4:;}return yyzzz;}static char _tmp236[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp236,_tmp236,_tmp236 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp237=yy1;struct Cyc_Absyn_Enumfield*_tmp238;_LL1AA: if(*((void**)
_tmp237)!= Cyc_Enumfield_tok)goto _LL1AC;_tmp238=((struct Cyc_Enumfield_tok_struct*)
_tmp237)->f1;_LL1AB: yyzzz=_tmp238;goto _LL1A9;_LL1AC:;_LL1AD:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1A9:;}return yyzzz;}static char _tmp23A[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp23A,_tmp23A,_tmp23A + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp23B=yy1;struct Cyc_List_List*
_tmp23C;_LL1AF: if(*((void**)_tmp23B)!= Cyc_EnumfieldList_tok)goto _LL1B1;_tmp23C=((
struct Cyc_EnumfieldList_tok_struct*)_tmp23B)->f1;_LL1B0: yyzzz=_tmp23C;goto _LL1AE;
_LL1B1:;_LL1B2:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1AE:;}return
yyzzz;}static char _tmp23E[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp23F=yy1;struct Cyc_Core_Opt*_tmp240;_LL1B4: if(*((void**)_tmp23F)!= Cyc_TypeOpt_tok)
goto _LL1B6;_tmp240=((struct Cyc_TypeOpt_tok_struct*)_tmp23F)->f1;_LL1B5: yyzzz=
_tmp240;goto _LL1B3;_LL1B6:;_LL1B7:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B3:;}return yyzzz;}static char _tmp242[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp242,
_tmp242,_tmp242 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp243=yy1;struct Cyc_List_List*_tmp244;_LL1B9:
if(*((void**)_tmp243)!= Cyc_Rgnorder_tok)goto _LL1BB;_tmp244=((struct Cyc_Rgnorder_tok_struct*)
_tmp243)->f1;_LL1BA: yyzzz=_tmp244;goto _LL1B8;_LL1BB:;_LL1BC:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1B8:;}return yyzzz;}static char _tmp246[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp246,
_tmp246,_tmp246 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY4(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp247=yy1;struct Cyc_Absyn_Conref*_tmp248;_LL1BE: if(*((void**)
_tmp247)!= Cyc_YY4)goto _LL1C0;_tmp248=((struct Cyc_YY4_struct*)_tmp247)->f1;
_LL1BF: yyzzz=_tmp248;goto _LL1BD;_LL1C0:;_LL1C1:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL1BD:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp249;_tmp249.timestamp=0;_tmp249.first_line=0;
_tmp249.first_column=0;_tmp249.last_line=0;_tmp249.last_column=0;_tmp249;});}
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
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp24A[
2]="$";static char _tmp24B[6]="error";static char _tmp24C[12]="$undefined.";static
char _tmp24D[5]="AUTO";static char _tmp24E[9]="REGISTER";static char _tmp24F[7]="STATIC";
static char _tmp250[7]="EXTERN";static char _tmp251[8]="TYPEDEF";static char _tmp252[5]="VOID";
static char _tmp253[5]="CHAR";static char _tmp254[6]="SHORT";static char _tmp255[4]="INT";
static char _tmp256[5]="LONG";static char _tmp257[6]="FLOAT";static char _tmp258[7]="DOUBLE";
static char _tmp259[7]="SIGNED";static char _tmp25A[9]="UNSIGNED";static char _tmp25B[
6]="CONST";static char _tmp25C[9]="VOLATILE";static char _tmp25D[9]="RESTRICT";
static char _tmp25E[7]="STRUCT";static char _tmp25F[6]="UNION";static char _tmp260[5]="CASE";
static char _tmp261[8]="DEFAULT";static char _tmp262[7]="INLINE";static char _tmp263[7]="SIZEOF";
static char _tmp264[9]="OFFSETOF";static char _tmp265[3]="IF";static char _tmp266[5]="ELSE";
static char _tmp267[7]="SWITCH";static char _tmp268[6]="WHILE";static char _tmp269[3]="DO";
static char _tmp26A[4]="FOR";static char _tmp26B[5]="GOTO";static char _tmp26C[9]="CONTINUE";
static char _tmp26D[6]="BREAK";static char _tmp26E[7]="RETURN";static char _tmp26F[5]="ENUM";
static char _tmp270[8]="NULL_kw";static char _tmp271[4]="LET";static char _tmp272[6]="THROW";
static char _tmp273[4]="TRY";static char _tmp274[6]="CATCH";static char _tmp275[4]="NEW";
static char _tmp276[9]="ABSTRACT";static char _tmp277[9]="FALLTHRU";static char
_tmp278[6]="USING";static char _tmp279[10]="NAMESPACE";static char _tmp27A[7]="TUNION";
static char _tmp27B[8]="XTUNION";static char _tmp27C[9]="FORARRAY";static char _tmp27D[
5]="FILL";static char _tmp27E[8]="CODEGEN";static char _tmp27F[4]="CUT";static char
_tmp280[7]="SPLICE";static char _tmp281[7]="MALLOC";static char _tmp282[8]="RMALLOC";
static char _tmp283[7]="CALLOC";static char _tmp284[8]="RCALLOC";static char _tmp285[9]="REGION_T";
static char _tmp286[9]="SIZEOF_T";static char _tmp287[6]="TAG_T";static char _tmp288[7]="REGION";
static char _tmp289[5]="RNEW";static char _tmp28A[8]="REGIONS";static char _tmp28B[13]="RESET_REGION";
static char _tmp28C[4]="GEN";static char _tmp28D[14]="NOZEROTERM_kw";static char
_tmp28E[12]="ZEROTERM_kw";static char _tmp28F[7]="PTR_OP";static char _tmp290[7]="INC_OP";
static char _tmp291[7]="DEC_OP";static char _tmp292[8]="LEFT_OP";static char _tmp293[9]="RIGHT_OP";
static char _tmp294[6]="LE_OP";static char _tmp295[6]="GE_OP";static char _tmp296[6]="EQ_OP";
static char _tmp297[6]="NE_OP";static char _tmp298[7]="AND_OP";static char _tmp299[6]="OR_OP";
static char _tmp29A[11]="MUL_ASSIGN";static char _tmp29B[11]="DIV_ASSIGN";static char
_tmp29C[11]="MOD_ASSIGN";static char _tmp29D[11]="ADD_ASSIGN";static char _tmp29E[11]="SUB_ASSIGN";
static char _tmp29F[12]="LEFT_ASSIGN";static char _tmp2A0[13]="RIGHT_ASSIGN";static
char _tmp2A1[11]="AND_ASSIGN";static char _tmp2A2[11]="XOR_ASSIGN";static char
_tmp2A3[10]="OR_ASSIGN";static char _tmp2A4[9]="ELLIPSIS";static char _tmp2A5[11]="LEFT_RIGHT";
static char _tmp2A6[12]="COLON_COLON";static char _tmp2A7[11]="IDENTIFIER";static
char _tmp2A8[17]="INTEGER_CONSTANT";static char _tmp2A9[7]="STRING";static char
_tmp2AA[19]="CHARACTER_CONSTANT";static char _tmp2AB[18]="FLOATING_CONSTANT";
static char _tmp2AC[9]="TYPE_VAR";static char _tmp2AD[13]="TYPEDEF_NAME";static char
_tmp2AE[16]="QUAL_IDENTIFIER";static char _tmp2AF[18]="QUAL_TYPEDEF_NAME";static
char _tmp2B0[13]="TYPE_INTEGER";static char _tmp2B1[10]="ATTRIBUTE";static char
_tmp2B2[4]="';'";static char _tmp2B3[4]="'{'";static char _tmp2B4[4]="'}'";static
char _tmp2B5[4]="'='";static char _tmp2B6[4]="'('";static char _tmp2B7[4]="')'";
static char _tmp2B8[4]="','";static char _tmp2B9[4]="'_'";static char _tmp2BA[4]="'$'";
static char _tmp2BB[4]="'<'";static char _tmp2BC[4]="':'";static char _tmp2BD[4]="'.'";
static char _tmp2BE[4]="'['";static char _tmp2BF[4]="']'";static char _tmp2C0[4]="'*'";
static char _tmp2C1[4]="'@'";static char _tmp2C2[4]="'?'";static char _tmp2C3[4]="'>'";
static char _tmp2C4[4]="'+'";static char _tmp2C5[4]="'-'";static char _tmp2C6[4]="'&'";
static char _tmp2C7[4]="'|'";static char _tmp2C8[4]="'^'";static char _tmp2C9[4]="'/'";
static char _tmp2CA[4]="'%'";static char _tmp2CB[4]="'~'";static char _tmp2CC[4]="'!'";
static char _tmp2CD[5]="prog";static char _tmp2CE[17]="translation_unit";static char
_tmp2CF[21]="external_declaration";static char _tmp2D0[20]="function_definition";
static char _tmp2D1[21]="function_definition2";static char _tmp2D2[13]="using_action";
static char _tmp2D3[15]="unusing_action";static char _tmp2D4[17]="namespace_action";
static char _tmp2D5[19]="unnamespace_action";static char _tmp2D6[12]="declaration";
static char _tmp2D7[17]="declaration_list";static char _tmp2D8[23]="declaration_specifiers";
static char _tmp2D9[24]="storage_class_specifier";static char _tmp2DA[15]="attributes_opt";
static char _tmp2DB[11]="attributes";static char _tmp2DC[15]="attribute_list";static
char _tmp2DD[10]="attribute";static char _tmp2DE[15]="type_specifier";static char
_tmp2DF[5]="kind";static char _tmp2E0[15]="type_qualifier";static char _tmp2E1[15]="enum_specifier";
static char _tmp2E2[11]="enum_field";static char _tmp2E3[22]="enum_declaration_list";
static char _tmp2E4[26]="struct_or_union_specifier";static char _tmp2E5[16]="type_params_opt";
static char _tmp2E6[16]="struct_or_union";static char _tmp2E7[24]="struct_declaration_list";
static char _tmp2E8[25]="struct_declaration_list0";static char _tmp2E9[21]="init_declarator_list";
static char _tmp2EA[22]="init_declarator_list0";static char _tmp2EB[16]="init_declarator";
static char _tmp2EC[19]="struct_declaration";static char _tmp2ED[25]="specifier_qualifier_list";
static char _tmp2EE[23]="struct_declarator_list";static char _tmp2EF[24]="struct_declarator_list0";
static char _tmp2F0[18]="struct_declarator";static char _tmp2F1[17]="tunion_specifier";
static char _tmp2F2[18]="tunion_or_xtunion";static char _tmp2F3[17]="tunionfield_list";
static char _tmp2F4[18]="tunionfield_scope";static char _tmp2F5[12]="tunionfield";
static char _tmp2F6[11]="declarator";static char _tmp2F7[18]="direct_declarator";
static char _tmp2F8[8]="pointer";static char _tmp2F9[12]="one_pointer";static char
_tmp2FA[23]="pointer_null_and_bound";static char _tmp2FB[14]="pointer_bound";
static char _tmp2FC[18]="zeroterm_qual_opt";static char _tmp2FD[8]="rgn_opt";static
char _tmp2FE[11]="tqual_list";static char _tmp2FF[20]="parameter_type_list";static
char _tmp300[9]="type_var";static char _tmp301[16]="optional_effect";static char
_tmp302[19]="optional_rgn_order";static char _tmp303[10]="rgn_order";static char
_tmp304[16]="optional_inject";static char _tmp305[11]="effect_set";static char
_tmp306[14]="atomic_effect";static char _tmp307[11]="region_set";static char _tmp308[
15]="parameter_list";static char _tmp309[22]="parameter_declaration";static char
_tmp30A[16]="identifier_list";static char _tmp30B[17]="identifier_list0";static
char _tmp30C[12]="initializer";static char _tmp30D[18]="array_initializer";static
char _tmp30E[17]="initializer_list";static char _tmp30F[12]="designation";static
char _tmp310[16]="designator_list";static char _tmp311[11]="designator";static char
_tmp312[10]="type_name";static char _tmp313[14]="any_type_name";static char _tmp314[
15]="type_name_list";static char _tmp315[20]="abstract_declarator";static char
_tmp316[27]="direct_abstract_declarator";static char _tmp317[10]="statement";
static char _tmp318[18]="labeled_statement";static char _tmp319[21]="expression_statement";
static char _tmp31A[19]="compound_statement";static char _tmp31B[16]="block_item_list";
static char _tmp31C[20]="selection_statement";static char _tmp31D[15]="switch_clauses";
static char _tmp31E[16]="switchC_clauses";static char _tmp31F[20]="iteration_statement";
static char _tmp320[15]="jump_statement";static char _tmp321[8]="pattern";static char
_tmp322[19]="tuple_pattern_list";static char _tmp323[14]="field_pattern";static
char _tmp324[19]="field_pattern_list";static char _tmp325[11]="expression";static
char _tmp326[22]="assignment_expression";static char _tmp327[20]="assignment_operator";
static char _tmp328[23]="conditional_expression";static char _tmp329[20]="constant_expression";
static char _tmp32A[22]="logical_or_expression";static char _tmp32B[23]="logical_and_expression";
static char _tmp32C[24]="inclusive_or_expression";static char _tmp32D[24]="exclusive_or_expression";
static char _tmp32E[15]="and_expression";static char _tmp32F[20]="equality_expression";
static char _tmp330[22]="relational_expression";static char _tmp331[17]="shift_expression";
static char _tmp332[20]="additive_expression";static char _tmp333[26]="multiplicative_expression";
static char _tmp334[16]="cast_expression";static char _tmp335[17]="unary_expression";
static char _tmp336[15]="unary_operator";static char _tmp337[19]="postfix_expression";
static char _tmp338[19]="primary_expression";static char _tmp339[25]="argument_expression_list";
static char _tmp33A[26]="argument_expression_list0";static char _tmp33B[9]="constant";
static char _tmp33C[20]="qual_opt_identifier";static char _tmp33D[17]="qual_opt_typedef";
static char _tmp33E[18]="struct_union_name";static char _tmp33F[11]="field_name";
static char _tmp340[12]="right_angle";static struct _tagged_arr Cyc_yytname[247]={{
_tmp24A,_tmp24A,_tmp24A + 2},{_tmp24B,_tmp24B,_tmp24B + 6},{_tmp24C,_tmp24C,
_tmp24C + 12},{_tmp24D,_tmp24D,_tmp24D + 5},{_tmp24E,_tmp24E,_tmp24E + 9},{_tmp24F,
_tmp24F,_tmp24F + 7},{_tmp250,_tmp250,_tmp250 + 7},{_tmp251,_tmp251,_tmp251 + 8},{
_tmp252,_tmp252,_tmp252 + 5},{_tmp253,_tmp253,_tmp253 + 5},{_tmp254,_tmp254,
_tmp254 + 6},{_tmp255,_tmp255,_tmp255 + 4},{_tmp256,_tmp256,_tmp256 + 5},{_tmp257,
_tmp257,_tmp257 + 6},{_tmp258,_tmp258,_tmp258 + 7},{_tmp259,_tmp259,_tmp259 + 7},{
_tmp25A,_tmp25A,_tmp25A + 9},{_tmp25B,_tmp25B,_tmp25B + 6},{_tmp25C,_tmp25C,
_tmp25C + 9},{_tmp25D,_tmp25D,_tmp25D + 9},{_tmp25E,_tmp25E,_tmp25E + 7},{_tmp25F,
_tmp25F,_tmp25F + 6},{_tmp260,_tmp260,_tmp260 + 5},{_tmp261,_tmp261,_tmp261 + 8},{
_tmp262,_tmp262,_tmp262 + 7},{_tmp263,_tmp263,_tmp263 + 7},{_tmp264,_tmp264,
_tmp264 + 9},{_tmp265,_tmp265,_tmp265 + 3},{_tmp266,_tmp266,_tmp266 + 5},{_tmp267,
_tmp267,_tmp267 + 7},{_tmp268,_tmp268,_tmp268 + 6},{_tmp269,_tmp269,_tmp269 + 3},{
_tmp26A,_tmp26A,_tmp26A + 4},{_tmp26B,_tmp26B,_tmp26B + 5},{_tmp26C,_tmp26C,
_tmp26C + 9},{_tmp26D,_tmp26D,_tmp26D + 6},{_tmp26E,_tmp26E,_tmp26E + 7},{_tmp26F,
_tmp26F,_tmp26F + 5},{_tmp270,_tmp270,_tmp270 + 8},{_tmp271,_tmp271,_tmp271 + 4},{
_tmp272,_tmp272,_tmp272 + 6},{_tmp273,_tmp273,_tmp273 + 4},{_tmp274,_tmp274,
_tmp274 + 6},{_tmp275,_tmp275,_tmp275 + 4},{_tmp276,_tmp276,_tmp276 + 9},{_tmp277,
_tmp277,_tmp277 + 9},{_tmp278,_tmp278,_tmp278 + 6},{_tmp279,_tmp279,_tmp279 + 10},{
_tmp27A,_tmp27A,_tmp27A + 7},{_tmp27B,_tmp27B,_tmp27B + 8},{_tmp27C,_tmp27C,
_tmp27C + 9},{_tmp27D,_tmp27D,_tmp27D + 5},{_tmp27E,_tmp27E,_tmp27E + 8},{_tmp27F,
_tmp27F,_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 7},{_tmp281,_tmp281,_tmp281 + 7},{
_tmp282,_tmp282,_tmp282 + 8},{_tmp283,_tmp283,_tmp283 + 7},{_tmp284,_tmp284,
_tmp284 + 8},{_tmp285,_tmp285,_tmp285 + 9},{_tmp286,_tmp286,_tmp286 + 9},{_tmp287,
_tmp287,_tmp287 + 6},{_tmp288,_tmp288,_tmp288 + 7},{_tmp289,_tmp289,_tmp289 + 5},{
_tmp28A,_tmp28A,_tmp28A + 8},{_tmp28B,_tmp28B,_tmp28B + 13},{_tmp28C,_tmp28C,
_tmp28C + 4},{_tmp28D,_tmp28D,_tmp28D + 14},{_tmp28E,_tmp28E,_tmp28E + 12},{_tmp28F,
_tmp28F,_tmp28F + 7},{_tmp290,_tmp290,_tmp290 + 7},{_tmp291,_tmp291,_tmp291 + 7},{
_tmp292,_tmp292,_tmp292 + 8},{_tmp293,_tmp293,_tmp293 + 9},{_tmp294,_tmp294,
_tmp294 + 6},{_tmp295,_tmp295,_tmp295 + 6},{_tmp296,_tmp296,_tmp296 + 6},{_tmp297,
_tmp297,_tmp297 + 6},{_tmp298,_tmp298,_tmp298 + 7},{_tmp299,_tmp299,_tmp299 + 6},{
_tmp29A,_tmp29A,_tmp29A + 11},{_tmp29B,_tmp29B,_tmp29B + 11},{_tmp29C,_tmp29C,
_tmp29C + 11},{_tmp29D,_tmp29D,_tmp29D + 11},{_tmp29E,_tmp29E,_tmp29E + 11},{
_tmp29F,_tmp29F,_tmp29F + 12},{_tmp2A0,_tmp2A0,_tmp2A0 + 13},{_tmp2A1,_tmp2A1,
_tmp2A1 + 11},{_tmp2A2,_tmp2A2,_tmp2A2 + 11},{_tmp2A3,_tmp2A3,_tmp2A3 + 10},{
_tmp2A4,_tmp2A4,_tmp2A4 + 9},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{_tmp2A6,_tmp2A6,
_tmp2A6 + 12},{_tmp2A7,_tmp2A7,_tmp2A7 + 11},{_tmp2A8,_tmp2A8,_tmp2A8 + 17},{
_tmp2A9,_tmp2A9,_tmp2A9 + 7},{_tmp2AA,_tmp2AA,_tmp2AA + 19},{_tmp2AB,_tmp2AB,
_tmp2AB + 18},{_tmp2AC,_tmp2AC,_tmp2AC + 9},{_tmp2AD,_tmp2AD,_tmp2AD + 13},{_tmp2AE,
_tmp2AE,_tmp2AE + 16},{_tmp2AF,_tmp2AF,_tmp2AF + 18},{_tmp2B0,_tmp2B0,_tmp2B0 + 13},{
_tmp2B1,_tmp2B1,_tmp2B1 + 10},{_tmp2B2,_tmp2B2,_tmp2B2 + 4},{_tmp2B3,_tmp2B3,
_tmp2B3 + 4},{_tmp2B4,_tmp2B4,_tmp2B4 + 4},{_tmp2B5,_tmp2B5,_tmp2B5 + 4},{_tmp2B6,
_tmp2B6,_tmp2B6 + 4},{_tmp2B7,_tmp2B7,_tmp2B7 + 4},{_tmp2B8,_tmp2B8,_tmp2B8 + 4},{
_tmp2B9,_tmp2B9,_tmp2B9 + 4},{_tmp2BA,_tmp2BA,_tmp2BA + 4},{_tmp2BB,_tmp2BB,
_tmp2BB + 4},{_tmp2BC,_tmp2BC,_tmp2BC + 4},{_tmp2BD,_tmp2BD,_tmp2BD + 4},{_tmp2BE,
_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,_tmp2C0,_tmp2C0 + 4},{
_tmp2C1,_tmp2C1,_tmp2C1 + 4},{_tmp2C2,_tmp2C2,_tmp2C2 + 4},{_tmp2C3,_tmp2C3,
_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 4},{_tmp2C5,_tmp2C5,_tmp2C5 + 4},{_tmp2C6,
_tmp2C6,_tmp2C6 + 4},{_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,_tmp2C8,_tmp2C8 + 4},{
_tmp2C9,_tmp2C9,_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{_tmp2CB,_tmp2CB,
_tmp2CB + 4},{_tmp2CC,_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD,_tmp2CD + 5},{_tmp2CE,
_tmp2CE,_tmp2CE + 17},{_tmp2CF,_tmp2CF,_tmp2CF + 21},{_tmp2D0,_tmp2D0,_tmp2D0 + 20},{
_tmp2D1,_tmp2D1,_tmp2D1 + 21},{_tmp2D2,_tmp2D2,_tmp2D2 + 13},{_tmp2D3,_tmp2D3,
_tmp2D3 + 15},{_tmp2D4,_tmp2D4,_tmp2D4 + 17},{_tmp2D5,_tmp2D5,_tmp2D5 + 19},{
_tmp2D6,_tmp2D6,_tmp2D6 + 12},{_tmp2D7,_tmp2D7,_tmp2D7 + 17},{_tmp2D8,_tmp2D8,
_tmp2D8 + 23},{_tmp2D9,_tmp2D9,_tmp2D9 + 24},{_tmp2DA,_tmp2DA,_tmp2DA + 15},{
_tmp2DB,_tmp2DB,_tmp2DB + 11},{_tmp2DC,_tmp2DC,_tmp2DC + 15},{_tmp2DD,_tmp2DD,
_tmp2DD + 10},{_tmp2DE,_tmp2DE,_tmp2DE + 15},{_tmp2DF,_tmp2DF,_tmp2DF + 5},{_tmp2E0,
_tmp2E0,_tmp2E0 + 15},{_tmp2E1,_tmp2E1,_tmp2E1 + 15},{_tmp2E2,_tmp2E2,_tmp2E2 + 11},{
_tmp2E3,_tmp2E3,_tmp2E3 + 22},{_tmp2E4,_tmp2E4,_tmp2E4 + 26},{_tmp2E5,_tmp2E5,
_tmp2E5 + 16},{_tmp2E6,_tmp2E6,_tmp2E6 + 16},{_tmp2E7,_tmp2E7,_tmp2E7 + 24},{
_tmp2E8,_tmp2E8,_tmp2E8 + 25},{_tmp2E9,_tmp2E9,_tmp2E9 + 21},{_tmp2EA,_tmp2EA,
_tmp2EA + 22},{_tmp2EB,_tmp2EB,_tmp2EB + 16},{_tmp2EC,_tmp2EC,_tmp2EC + 19},{
_tmp2ED,_tmp2ED,_tmp2ED + 25},{_tmp2EE,_tmp2EE,_tmp2EE + 23},{_tmp2EF,_tmp2EF,
_tmp2EF + 24},{_tmp2F0,_tmp2F0,_tmp2F0 + 18},{_tmp2F1,_tmp2F1,_tmp2F1 + 17},{
_tmp2F2,_tmp2F2,_tmp2F2 + 18},{_tmp2F3,_tmp2F3,_tmp2F3 + 17},{_tmp2F4,_tmp2F4,
_tmp2F4 + 18},{_tmp2F5,_tmp2F5,_tmp2F5 + 12},{_tmp2F6,_tmp2F6,_tmp2F6 + 11},{
_tmp2F7,_tmp2F7,_tmp2F7 + 18},{_tmp2F8,_tmp2F8,_tmp2F8 + 8},{_tmp2F9,_tmp2F9,
_tmp2F9 + 12},{_tmp2FA,_tmp2FA,_tmp2FA + 23},{_tmp2FB,_tmp2FB,_tmp2FB + 14},{
_tmp2FC,_tmp2FC,_tmp2FC + 18},{_tmp2FD,_tmp2FD,_tmp2FD + 8},{_tmp2FE,_tmp2FE,
_tmp2FE + 11},{_tmp2FF,_tmp2FF,_tmp2FF + 20},{_tmp300,_tmp300,_tmp300 + 9},{_tmp301,
_tmp301,_tmp301 + 16},{_tmp302,_tmp302,_tmp302 + 19},{_tmp303,_tmp303,_tmp303 + 10},{
_tmp304,_tmp304,_tmp304 + 16},{_tmp305,_tmp305,_tmp305 + 11},{_tmp306,_tmp306,
_tmp306 + 14},{_tmp307,_tmp307,_tmp307 + 11},{_tmp308,_tmp308,_tmp308 + 15},{
_tmp309,_tmp309,_tmp309 + 22},{_tmp30A,_tmp30A,_tmp30A + 16},{_tmp30B,_tmp30B,
_tmp30B + 17},{_tmp30C,_tmp30C,_tmp30C + 12},{_tmp30D,_tmp30D,_tmp30D + 18},{
_tmp30E,_tmp30E,_tmp30E + 17},{_tmp30F,_tmp30F,_tmp30F + 12},{_tmp310,_tmp310,
_tmp310 + 16},{_tmp311,_tmp311,_tmp311 + 11},{_tmp312,_tmp312,_tmp312 + 10},{
_tmp313,_tmp313,_tmp313 + 14},{_tmp314,_tmp314,_tmp314 + 15},{_tmp315,_tmp315,
_tmp315 + 20},{_tmp316,_tmp316,_tmp316 + 27},{_tmp317,_tmp317,_tmp317 + 10},{
_tmp318,_tmp318,_tmp318 + 18},{_tmp319,_tmp319,_tmp319 + 21},{_tmp31A,_tmp31A,
_tmp31A + 19},{_tmp31B,_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C,_tmp31C + 20},{
_tmp31D,_tmp31D,_tmp31D + 15},{_tmp31E,_tmp31E,_tmp31E + 16},{_tmp31F,_tmp31F,
_tmp31F + 20},{_tmp320,_tmp320,_tmp320 + 15},{_tmp321,_tmp321,_tmp321 + 8},{_tmp322,
_tmp322,_tmp322 + 19},{_tmp323,_tmp323,_tmp323 + 14},{_tmp324,_tmp324,_tmp324 + 19},{
_tmp325,_tmp325,_tmp325 + 11},{_tmp326,_tmp326,_tmp326 + 22},{_tmp327,_tmp327,
_tmp327 + 20},{_tmp328,_tmp328,_tmp328 + 23},{_tmp329,_tmp329,_tmp329 + 20},{
_tmp32A,_tmp32A,_tmp32A + 22},{_tmp32B,_tmp32B,_tmp32B + 23},{_tmp32C,_tmp32C,
_tmp32C + 24},{_tmp32D,_tmp32D,_tmp32D + 24},{_tmp32E,_tmp32E,_tmp32E + 15},{
_tmp32F,_tmp32F,_tmp32F + 20},{_tmp330,_tmp330,_tmp330 + 22},{_tmp331,_tmp331,
_tmp331 + 17},{_tmp332,_tmp332,_tmp332 + 20},{_tmp333,_tmp333,_tmp333 + 26},{
_tmp334,_tmp334,_tmp334 + 16},{_tmp335,_tmp335,_tmp335 + 17},{_tmp336,_tmp336,
_tmp336 + 15},{_tmp337,_tmp337,_tmp337 + 19},{_tmp338,_tmp338,_tmp338 + 19},{
_tmp339,_tmp339,_tmp339 + 25},{_tmp33A,_tmp33A,_tmp33A + 26},{_tmp33B,_tmp33B,
_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 20},{_tmp33D,_tmp33D,_tmp33D + 17},{_tmp33E,
_tmp33E,_tmp33E + 18},{_tmp33F,_tmp33F,_tmp33F + 11},{_tmp340,_tmp340,_tmp340 + 12}};
static short Cyc_yyr1[434]={0,131,132,132,132,132,132,132,132,133,133,133,134,134,
134,134,135,135,136,137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,
142,142,142,142,143,143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,
147,147,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
148,148,148,148,149,150,150,150,151,151,151,152,152,153,153,153,154,154,154,155,
155,156,156,157,157,158,158,159,160,160,161,161,162,163,163,163,163,163,163,164,
165,165,166,166,166,167,167,167,168,168,169,169,169,169,170,170,170,171,171,172,
172,173,173,173,173,173,173,173,173,173,173,174,174,175,176,176,176,177,177,177,
178,178,178,179,179,179,180,180,181,181,181,181,182,182,183,183,184,184,185,185,
186,186,187,187,188,188,188,188,189,189,190,190,191,191,191,192,193,193,194,194,
195,195,195,195,196,196,196,196,197,198,198,199,199,200,200,201,201,201,201,201,
202,202,203,203,203,204,204,204,204,204,204,204,204,204,204,204,205,205,205,205,
205,205,205,205,205,205,205,205,205,206,207,207,207,208,208,208,209,209,209,209,
209,209,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,212,212,212,
212,212,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
214,214,214,214,214,214,214,214,215,215,215,215,215,215,215,215,215,215,215,215,
215,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,221,221,221,221,
221,221,221,221,222,222,222,222,222,222,222,223,224,224,225,225,226,226,227,227,
228,228,229,229,229,230,230,230,230,230,231,231,231,232,232,232,233,233,233,233,
234,234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,
236,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,
238,238,238,239,240,240,241,241,241,241,242,242,243,243,244,244,245,245,246,246};
static short Cyc_yyr2[434]={0,1,2,3,5,3,5,6,0,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,
2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,1,3,1,1,1,1,1,1,1,1,
1,1,1,1,2,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,
1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,5,4,5,4,4,2,2,1,2,
5,2,2,1,0,3,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,
1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,
4,4,2,1,1,1,1,1,1,6,3,9,6,5,2,2,3,1,2,2,2,3,3,1,2,1,2,1,2,5,7,2,7,2,8,6,2,0,3,4,
5,6,7,4,0,3,4,5,4,5,2,7,2,6,7,7,8,7,8,8,9,6,7,7,8,2,9,3,2,2,2,3,2,4,5,1,3,1,2,1,
1,1,1,4,4,5,2,2,4,4,1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,
3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,
5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,1,1};static short Cyc_yydefact[879]={0,11,38,39,40,41,43,56,59,60,61,62,
63,64,65,66,79,80,81,95,96,34,0,0,44,0,0,122,123,0,0,76,424,166,426,425,427,77,0,
0,57,0,151,151,150,1,0,9,0,0,10,0,28,36,30,32,67,68,0,69,157,0,133,0,145,154,70,
135,93,42,41,35,0,83,308,424,304,307,306,0,302,0,0,0,0,0,189,0,309,18,20,0,0,0,0,
0,0,0,0,0,148,149,2,0,0,0,0,22,0,101,102,104,29,37,31,33,97,428,429,93,159,0,158,
0,26,0,0,12,168,0,0,143,134,146,156,155,157,144,0,71,0,42,87,0,85,0,0,424,0,0,
314,305,313,25,0,0,93,0,0,0,111,107,109,207,209,0,0,0,430,431,167,78,0,136,58,
187,0,184,0,0,423,0,0,0,0,0,0,0,0,0,93,0,0,420,409,421,422,0,0,0,0,394,0,392,393,
0,0,325,338,346,348,350,352,354,356,359,364,367,370,374,0,376,395,408,407,3,0,5,
0,23,0,0,0,13,0,98,99,0,92,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,424,244,247,0,0,
0,0,230,231,232,0,233,234,235,0,323,27,14,104,174,190,0,0,170,168,0,214,0,0,154,
0,45,0,0,88,84,0,0,0,0,303,317,0,191,0,0,0,0,210,182,0,112,108,110,168,0,216,208,
217,433,432,0,73,74,75,51,50,0,48,168,186,216,188,72,0,0,384,0,340,374,0,341,342,
0,0,0,0,0,0,0,0,0,377,378,0,0,0,0,0,380,381,379,153,152,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,328,329,330,331,332,333,334,335,336,337,327,0,382,0,401,402,0,0,0,
412,0,93,19,0,21,0,103,105,193,192,15,90,100,0,0,113,114,116,93,0,120,246,249,
258,0,260,0,0,277,0,279,0,292,0,0,295,296,297,0,263,0,299,0,0,0,241,242,0,0,0,0,
0,255,251,104,253,248,245,0,175,0,0,0,181,169,176,139,0,0,0,170,141,0,142,0,137,
154,160,46,94,0,89,345,86,82,315,316,310,0,24,0,0,0,0,203,319,321,0,311,0,0,211,
0,170,0,154,0,218,168,0,0,229,213,0,0,0,185,0,0,0,194,198,0,0,0,0,0,0,0,0,0,0,0,
0,411,0,0,410,418,0,417,347,0,349,351,353,355,357,358,362,363,360,361,365,366,
368,369,371,372,373,326,400,397,0,399,0,0,0,4,6,117,106,0,0,170,93,128,0,0,0,0,0,
0,0,0,294,298,0,0,0,0,237,0,0,0,243,0,16,324,168,0,178,0,0,171,0,140,174,162,215,
138,160,147,7,318,206,0,320,202,204,312,0,212,183,225,0,219,220,154,0,170,0,154,
0,0,0,0,47,49,383,0,0,195,0,199,405,406,0,388,0,0,0,0,0,0,0,0,375,414,0,0,398,
396,0,0,115,118,97,121,130,129,0,0,124,0,0,0,0,0,0,0,0,0,0,264,300,0,0,0,0,0,17,
170,0,179,177,0,170,0,161,205,322,224,222,227,0,228,221,154,54,0,52,53,0,0,0,196,
200,0,0,0,0,343,344,387,416,0,419,339,413,415,0,0,119,131,128,128,256,0,264,276,
0,0,0,0,0,0,0,0,0,0,0,0,301,0,0,0,0,240,164,180,172,163,168,226,223,0,386,385,0,
201,389,0,0,403,0,91,0,127,126,0,271,0,0,280,0,0,0,288,0,0,0,0,0,0,0,0,0,262,0,
236,239,0,0,170,0,0,0,0,404,0,257,0,0,0,259,278,281,282,0,290,289,0,284,0,0,0,0,
0,0,265,0,0,173,165,0,0,0,0,132,0,0,0,261,283,291,285,286,0,270,0,264,266,0,0,55,
197,390,0,0,0,272,287,0,267,293,238,0,275,271,273,264,268,391,274,269,0,0,0};
static short Cyc_yydefgoto[116]={876,45,46,47,260,48,403,49,405,261,232,262,52,477,
53,324,325,54,170,55,56,142,143,57,139,58,234,235,108,109,110,236,163,414,415,
416,59,60,671,672,673,61,62,63,64,65,100,136,121,612,502,66,503,468,604,460,464,
465,307,281,177,85,86,521,408,522,523,493,494,164,283,284,504,315,263,264,265,
266,267,268,752,814,269,270,298,299,496,497,271,272,391,207,483,208,209,210,211,
212,213,214,215,216,217,218,219,220,221,222,539,540,223,224,68,119,171,319};
static short Cyc_yypact[879]={3038,- -32768,- -32768,- -32768,- -32768,20,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3772,150,725,- -32768,82,107,- -32768,- -32768,21,90,
128,- -32768,164,- -32768,- -32768,- -32768,- -32768,176,248,195,199,220,220,- -32768,
- -32768,2917,- -32768,213,259,- -32768,101,3772,3772,3772,3772,- -32768,- -32768,221,
- -32768,- 37,3643,121,122,462,424,- -32768,82,222,242,285,- -32768,82,268,- -32768,
181,- -32768,- -32768,- -32768,1010,- -32768,293,339,343,1010,354,384,375,- 57,-
-32768,- -32768,5868,5868,5868,203,412,415,203,6046,2663,- -32768,- -32768,- -32768,
2917,3158,2917,3158,- -32768,425,427,- -32768,3574,- -32768,- -32768,- -32768,- -32768,
6046,- -32768,- -32768,222,- -32768,82,- -32768,1840,- -32768,3643,101,- -32768,3828,
5868,4205,- -32768,121,- -32768,- -32768,- -32768,- 37,- -32768,5868,- -32768,3158,-
-32768,429,435,437,82,- 29,434,443,1010,- -32768,- -32768,- -32768,- -32768,457,5421,
222,1010,447,168,6046,6046,6046,346,- -32768,8,8,8,- -32768,- -32768,- -32768,-
-32768,38,- -32768,- -32768,98,392,- -32768,5649,449,- -32768,5421,5497,451,456,483,
486,488,490,513,222,5725,5725,- -32768,- -32768,- -32768,- -32768,2094,515,5801,5801,
- -32768,5801,- -32768,- -32768,57,529,- -32768,- 36,564,523,528,532,433,133,441,409,
130,- -32768,620,5801,96,42,- -32768,560,- -32768,563,- -32768,577,- -32768,248,4281,
3643,- -32768,579,6046,- -32768,159,594,306,368,12,41,43,2200,45,597,588,608,4357,
2306,167,606,2412,2412,120,610,614,- -32768,- -32768,1186,1320,101,1450,- -32768,-
-32768,- -32768,616,- -32768,- -32768,- -32768,278,- -32768,- -32768,- -32768,633,644,-
-32768,- 15,634,631,280,635,624,- 13,640,424,638,647,- 13,646,82,- -32768,5421,651,
637,639,- -32768,- -32768,408,- -32768,298,977,461,5868,- -32768,652,655,- -32768,-
-32768,- -32768,3456,4433,174,- -32768,335,- -32768,- -32768,- 15,- -32768,- -32768,-
-32768,- -32768,656,657,659,3343,- -32768,77,- -32768,- -32768,6046,2094,- -32768,
6046,- -32768,- -32768,3853,- -32768,686,5421,3276,5421,5421,5421,5421,5421,662,
2094,- -32768,- -32768,669,1970,671,465,2786,- -32768,- -32768,- -32768,- -32768,-
-32768,5801,5421,5801,5801,5801,5801,5801,5801,5801,5801,5801,5801,5801,5801,
5801,5801,5801,5801,5801,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,5421,- -32768,203,- -32768,- -32768,4509,203,5421,-
-32768,668,222,- -32768,2917,- -32768,2917,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,5421,680,675,- -32768,674,222,82,684,- -32768,- -32768,- -32768,5421,
- -32768,682,5421,- -32768,5421,689,764,- -32768,2540,691,- -32768,- -32768,- -32768,
305,689,757,- -32768,4585,3712,689,- -32768,- -32768,2412,702,708,5421,2412,- -32768,
- -32768,3574,- -32768,- -32768,- -32768,5421,- -32768,6046,694,207,- -32768,- -32768,
685,- -32768,- 15,695,5941,631,- -32768,5868,- -32768,5421,- -32768,424,589,- -32768,-
-32768,2917,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1010,- -32768,
203,5421,1010,255,- -32768,- -32768,- -32768,218,- -32768,- 6,710,- -32768,697,631,711,
424,706,335,5926,5868,4661,- -32768,- -32768,534,718,38,- -32768,720,721,737,-
-32768,- -32768,226,4281,468,723,248,475,731,732,734,493,6046,726,- -32768,741,
5573,- -32768,- -32768,744,740,564,251,523,528,532,433,133,133,441,441,441,441,409,
409,130,130,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,745,- -32768,115,5868,
4117,- -32768,- -32768,- -32768,- -32768,159,5421,631,222,605,504,5421,521,524,747,
4737,4813,312,- -32768,- -32768,751,753,749,755,- -32768,739,748,530,- -32768,3643,-
-32768,- -32768,758,5868,- -32768,760,- 15,- -32768,750,- -32768,282,- -32768,624,-
-32768,589,- -32768,- -32768,- -32768,- -32768,752,- -32768,- -32768,- -32768,- -32768,
977,- -32768,- -32768,- -32768,759,- -32768,- -32768,424,761,631,- 13,424,756,536,763,
765,- -32768,- -32768,771,245,766,- -32768,3941,- -32768,- -32768,- -32768,3643,-
-32768,5421,836,5421,5497,768,771,769,4117,- -32768,- -32768,5421,5421,- -32768,-
-32768,- 13,277,- -32768,- -32768,6046,- -32768,- -32768,- -32768,776,82,319,2412,541,
780,2412,5421,4889,324,4965,326,5041,630,- -32768,777,5421,793,44,784,- -32768,631,
30,- -32768,- -32768,792,631,6046,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
782,- -32768,- -32768,424,- -32768,800,- -32768,- -32768,787,789,5421,- -32768,- -32768,
4281,549,791,790,- -32768,686,- -32768,- -32768,334,- -32768,- -32768,- -32768,- -32768,
4117,796,- -32768,795,22,605,876,803,630,- -32768,551,2412,553,5117,2412,558,5193,
5269,349,813,797,799,- -32768,395,2412,2412,814,- -32768,- -32768,- -32768,804,-
-32768,758,- -32768,- -32768,806,- -32768,- -32768,347,- -32768,- -32768,6046,893,-
-32768,4029,- -32768,6046,- -32768,- -32768,2412,649,816,815,- -32768,2412,2412,566,
- -32768,2412,2412,569,2412,571,5345,809,- 3,1970,- -32768,5421,- -32768,- -32768,805,
- 15,631,826,5421,819,822,- -32768,585,- -32768,716,818,827,- -32768,- -32768,- -32768,
- -32768,2412,- -32768,- -32768,2412,- -32768,2412,2412,587,1970,5421,1580,- -32768,
611,841,- -32768,- -32768,830,350,831,6046,- -32768,821,359,1970,- -32768,- -32768,-
-32768,- -32768,- -32768,2412,- -32768,397,630,- -32768,2412,2412,- -32768,- -32768,-
-32768,832,1970,1710,- -32768,- -32768,1580,- -32768,- -32768,- -32768,834,- -32768,
649,- -32768,630,- -32768,- -32768,- -32768,- -32768,944,945,- -32768};static short Cyc_yypgoto[
116]={- -32768,194,- -32768,607,- -32768,- -32768,- -32768,- -32768,- -32768,484,- 55,
372,- -32768,- -32768,- 62,432,- -32768,217,853,26,- -32768,- -32768,- 107,- -32768,- 42,
- -32768,284,- -32768,- -32768,- -32768,722,719,- 66,- -32768,- -32768,387,- -32768,-
-32768,- 5,- -32768,- -32768,14,- 24,- 31,- -32768,- -32768,917,- 193,829,352,- 103,- 50,-
119,- 391,166,364,367,- 307,- 390,- 93,- 313,844,- -32768,- 212,- 165,- 533,- 290,- -32768,
480,- 189,- 77,- 117,72,- 132,272,- -32768,- -32768,- 54,- 172,- -32768,- 661,- 768,- -32768,
- -32768,- 21,823,355,- -32768,214,224,- -32768,- 94,- 378,- 162,617,618,615,619,621,
365,247,362,363,- 28,775,- -32768,- -32768,- -32768,- 340,- -32768,- -32768,1,924,-
-32768,- 352,- 163};static short Cyc_yytable[6159]={131,67,87,320,321,176,125,127,
353,280,122,512,492,423,165,166,167,338,516,407,339,289,205,73,88,279,89,669,670,
160,160,160,175,133,664,570,160,160,294,132,67,561,425,361,428,564,432,67,156,
461,237,157,67,96,160,322,563,233,148,117,316,33,175,152,67,111,160,160,137,295,
131,274,602,144,120,828,160,238,782,608,88,316,296,33,362,88,122,335,452,453,462,
455,870,475,308,309,310,472,160,160,160,874,589,622,67,67,67,67,317,306,623,829,
625,616,302,69,318,162,162,162,424,473,239,726,162,162,479,67,- 125,317,318,323,
313,399,91,160,426,756,615,760,275,67,162,517,328,518,144,599,347,427,88,429,318,
433,162,162,516,757,88,533,605,400,470,359,162,393,394,395,852,237,32,356,357,
160,358,32,67,35,410,318,535,507,35,667,481,326,162,162,162,327,864,32,392,312,
32,666,507,420,35,482,90,35,872,92,396,107,326,369,370,39,875,397,398,447,312,32,
42,43,44,42,43,44,35,162,38,458,478,499,463,128,39,67,662,448,129,314,449,130,67,
705,102,93,160,32,275,175,371,329,377,160,35,417,32,511,372,72,94,378,379,35,175,
162,176,67,160,175,33,39,463,160,160,441,160,413,305,442,454,42,43,44,313,495,
311,610,95,- 190,160,97,714,175,312,- 190,144,160,146,328,168,225,226,227,228,759,
169,88,132,33,762,98,161,161,161,644,627,601,32,161,161,103,104,482,34,35,36,206,
620,99,116,67,621,67,162,492,642,161,290,138,643,162,168,713,96,32,67,653,169,
161,161,140,35,557,558,559,162,718,287,161,39,162,162,567,162,458,618,105,106,
660,42,43,44,301,490,491,51,145,162,459,574,161,161,161,141,162,457,730,278,763,
278,731,458,630,469,- 168,631,71,175,609,- 168,482,160,596,597,149,489,175,67,629,
67,160,458,586,160,354,306,834,161,458,683,463,51,138,575,419,458,735,112,113,
114,115,744,736,747,717,150,126,458,703,458,151,38,707,774,718,175,508,775,511,
160,160,509,663,306,510,161,794,311,409,856,458,153,458,458,806,312,438,42,43,44,
614,706,458,160,617,421,860,422,51,51,51,51,482,692,67,155,126,50,718,162,722,
697,88,723,134,135,88,154,162,605,126,162,799,729,330,331,611,160,458,770,458,
657,367,368,863,51,373,374,765,431,487,488,717,172,161,440,693,173,445,446,67,
161,229,50,375,376,668,162,162,506,230,160,291,647,292,691,161,293,124,354,146,
161,161,300,161,297,463,524,304,527,334,162,340,531,409,354,717,341,161,728,528,
529,530,498,488,161,67,537,458,542,645,458,538,42,43,44,807,648,458,417,50,50,50,
50,342,162,233,343,124,344,568,345,569,495,237,652,458,126,160,16,17,18,273,669,
670,565,674,458,560,549,550,551,552,538,346,88,355,50,830,162,634,635,636,676,
458,175,677,458,360,160,611,577,690,458,579,363,580,804,709,710,584,364,858,738,
458,750,751,365,849,366,851,771,458,783,458,785,458,594,401,538,789,458,402,861,
812,813,734,613,819,458,161,822,458,824,458,598,404,810,411,161,868,869,161,434,
871,435,162,839,331,848,458,538,418,380,381,382,383,384,385,386,387,388,389,160,
175,436,526,443,160,273,840,450,591,853,458,456,595,162,161,161,390,451,796,778,
779,547,548,633,553,554,459,555,556,231,178,179,466,471,467,318,409,474,161,38,
88,480,463,180,476,181,484,485,182,486,501,500,74,513,361,514,183,184,515,532,
185,186,187,188,51,160,51,534,189,536,566,190,161,571,572,191,192,573,576,578,
409,675,421,581,585,680,682,162,587,592,593,600,162,606,126,624,603,33,32,193,
194,195,196,795,126,35,161,75,76,626,77,78,628,197,35,126,637,285,639,641,640,
646,79,199,654,80,81,200,201,202,649,650,82,651,203,204,655,83,84,659,74,51,658,
661,678,684,685,686,687,688,720,278,719,162,689,694,409,702,700,704,696,711,708,
712,721,656,724,725,715,409,753,733,727,161,739,755,50,758,50,761,764,741,743,
766,746,767,749,768,772,773,754,776,777,780,798,147,76,781,77,78,797,802,35,803,
161,805,583,808,816,835,79,815,827,80,81,832,590,837,769,838,82,842,843,854,859,
83,84,124,855,857,867,409,873,877,878,737,638,525,740,174,732,406,333,412,409,
336,336,787,665,101,791,793,699,50,288,349,350,126,833,695,698,282,619,336,336,
701,336,541,544,303,543,118,0,545,0,0,546,0,161,0,0,0,0,161,336,0,0,0,409,0,0,0,
0,0,0,0,0,826,0,0,0,0,831,784,74,0,788,0,126,836,0,0,0,0,0,841,800,801,0,0,0,0,0,
0,0,0,0,0,0,0,0,850,0,0,0,0,0,74,0,0,0,811,0,0,161,0,817,818,0,0,820,821,0,823,0,
0,0,336,0,147,76,0,77,78,0,0,35,0,0,273,0,0,0,0,79,0,0,80,81,0,844,490,491,845,
82,846,847,0,0,83,84,0,147,76,0,77,78,0,0,35,0,0,0,0,0,0,0,79,0,862,80,81,0,0,
865,866,0,82,0,0,124,0,83,84,0,336,0,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,444,336,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 254,- 254,21,
178,179,241,0,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,
27,28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,
37,38,258,123,- 254,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,336,0,0,0,
203,204,0,0,0,0,444,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 250,-
250,21,178,179,241,336,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,
24,251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,
0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,
34,35,36,37,38,258,123,- 250,336,197,0,0,40,198,0,0,336,0,0,199,0,0,0,200,201,202,
0,0,0,0,203,204,444,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 252,-
252,21,178,179,241,0,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,
251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,
36,37,38,258,123,- 252,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,
203,204,444,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,750,751,21,178,
179,241,0,242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,27,
28,252,183,184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,192,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,37,38,
258,123,- 264,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,
444,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,812,813,21,178,179,241,0,
242,243,244,245,246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,27,28,252,183,
184,253,254,185,186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,37,38,258,123,- 271,
0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,240,0,2,3,4,70,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,241,0,242,243,244,245,
246,247,248,249,22,180,23,181,250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,
186,187,188,29,30,31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,257,193,194,195,196,33,34,35,36,37,38,258,123,259,0,197,0,0,40,198,
0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,444,0,2,3,4,70,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,178,179,241,0,242,243,244,245,246,247,248,249,22,
180,23,181,250,0,182,24,251,0,0,27,28,252,183,184,253,254,185,186,187,188,29,30,
31,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,
193,194,195,196,33,34,35,36,37,38,258,123,0,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,
200,201,202,351,0,0,0,203,204,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,
179,0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,28,0,183,184,0,0,185,186,
187,188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,193,194,195,196,33,34,35,36,37,38,0,352,0,430,197,0,0,40,198,0,0,0,0,0,
199,0,0,0,200,201,202,0,0,0,0,203,204,178,179,241,0,242,243,244,245,246,247,248,
249,0,180,0,181,250,0,182,0,251,0,0,0,0,252,183,184,253,254,185,186,187,188,0,0,
0,255,189,0,256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,
193,194,195,196,0,0,35,0,0,0,258,123,0,439,197,0,0,0,285,0,0,0,0,0,199,0,0,0,200,
201,202,0,0,0,0,203,204,178,179,241,0,242,243,244,245,246,247,248,249,0,180,0,
181,250,0,182,0,251,0,0,0,0,252,183,184,253,254,185,186,187,188,0,0,0,255,189,0,
256,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,
196,0,0,35,0,0,0,258,123,0,444,197,0,0,0,285,0,0,0,0,0,199,0,0,0,200,201,202,0,0,
0,0,203,204,178,179,241,0,242,243,244,245,246,247,248,249,0,180,0,181,250,0,182,
0,251,0,0,0,0,252,183,184,253,254,185,186,187,188,0,0,0,255,189,0,256,190,0,0,0,
191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,257,193,194,195,196,0,0,35,0,0,
0,258,123,0,0,197,0,0,0,285,0,0,0,0,0,199,0,0,0,200,201,202,0,0,0,0,203,204,2,3,
4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,
22,180,23,181,0,0,182,24,0,0,0,27,28,0,183,184,0,0,185,186,187,188,29,30,31,0,
189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,
195,196,33,34,35,36,37,38,582,0,0,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,200,201,
202,0,0,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,
0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,28,0,183,184,0,0,185,186,187,188,29,
30,31,0,189,158,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,33,34,35,36,37,38,0,159,0,0,197,0,0,40,198,0,0,0,0,0,199,0,0,0,
200,201,202,0,0,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,
0,0,0,0,0,0,0,0,0,0,22,180,0,181,0,0,182,0,0,0,0,27,28,0,183,184,0,0,185,186,187,
188,29,30,31,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,32,193,194,195,196,33,34,35,36,37,38,0,0,0,0,197,0,0,40,198,0,0,0,0,0,199,0,0,
0,200,201,202,0,0,0,0,203,204,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,
0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
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
0,0,0,0,0,0,0,0,276,0,0,32,0,0,0,0,33,34,35,36,37,38,278,0,0,0,326,0,0,40,41,0,0,
0,312,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,0,0,33,34,0,36,37,38,278,0,0,
0,311,0,0,40,41,0,0,0,312,0,42,43,44,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,0,0,27,28,0,0,0,0,0,0,
0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,22,231,23,0,0,40,41,24,0,0,0,27,28,0,
0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,22,0,23,0,0,40,41,24,0,0,0,
27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,33,34,0,36,37,38,24,0,0,0,27,28,0,40,41,
0,0,0,0,0,0,29,30,31,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,0,0,0,33,34,0,36,37,38,27,28,178,179,0,0,0,40,41,519,0,29,30,
31,0,180,0,181,0,0,182,0,0,0,0,0,0,0,183,184,0,0,185,186,187,188,0,0,0,0,189,0,
276,190,0,277,0,191,192,0,33,34,0,36,37,38,278,0,0,0,0,0,0,40,41,0,0,0,0,0,32,
193,194,195,196,0,0,35,0,0,0,0,337,520,0,197,0,0,0,285,178,179,490,491,0,199,0,0,
0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,183,184,0,0,185,186,187,188,
0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,0,0,35,0,0,0,0,337,716,0,197,0,0,0,285,178,179,490,491,0,199,0,0,
0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,183,184,0,0,185,186,187,188,
0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,0,0,35,0,0,0,0,337,809,0,197,0,0,0,285,178,179,490,491,0,199,0,0,
0,200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,183,184,0,0,185,186,187,188,
0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
193,194,195,196,0,0,35,0,0,0,0,337,0,0,197,0,0,0,285,178,179,490,491,0,199,0,0,0,
200,201,202,0,180,0,181,203,204,182,0,0,0,0,0,0,0,183,184,0,0,185,186,187,188,0,
0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,
194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,0,181,286,199,182,0,0,
200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,0,337,0,0,197,0,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,
204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,437,0,0,0,197,0,0,0,285,0,
180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,
189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,
195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,0,181,505,199,182,0,0,200,
201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,
0,0,197,562,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,588,0,0,285,0,180,0,
181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,
0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,
0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,0,181,632,199,182,0,0,200,201,202,0,
0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,679,0,0,0,197,
0,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,
188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,193,194,195,196,0,0,35,178,179,0,681,0,0,0,197,0,0,0,285,0,180,0,181,0,199,
182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,
0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,0,0,0,197,742,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,745,0,0,
285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,
0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,
194,195,196,0,0,35,178,179,0,748,0,0,0,197,0,0,0,285,0,180,0,181,0,199,182,0,0,
200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,
192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,
0,0,0,0,0,197,786,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,
204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,790,0,0,285,0,
180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,
189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,
195,196,0,0,35,178,179,0,0,0,0,0,197,792,0,0,285,0,180,0,181,0,199,182,0,0,200,
201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,
0,0,197,825,0,0,285,0,180,0,181,0,199,182,0,0,200,201,202,0,0,183,184,203,204,
185,186,187,188,0,0,0,0,189,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,197,0,0,0,285,0,180,0,0,
0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,
0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,
178,179,0,0,337,0,0,197,0,0,0,285,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,
203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,656,0,0,197,0,0,0,285,0,
180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,0,
0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,
0,0,35,178,179,0,0,0,0,0,332,0,0,0,285,0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,
184,203,204,185,186,187,188,0,0,0,0,0,0,0,190,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,32,193,194,195,196,0,0,35,178,179,0,0,0,0,0,348,0,0,0,285,
0,180,0,0,0,199,0,0,0,200,201,202,0,0,183,184,203,204,185,186,187,188,0,0,0,0,0,
0,0,190,0,0,0,191,192,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,32,
193,194,195,196,0,0,35,0,0,0,22,0,0,0,197,0,0,0,285,0,0,27,28,0,199,0,0,0,200,
201,202,0,29,30,31,203,204,158,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,22,0,0,33,34,0,36,37,38,0,159,27,28,0,0,22,40,
41,0,0,0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,
276,0,0,0,0,0,0,0,33,34,0,36,37,38,278,607,0,0,0,0,0,40,41,33,34,0,36,37,38,0,0,
0,0,0,0,0,40,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,38,0,0,0,0,0,0,
0,40,41};static short Cyc_yycheck[6159]={62,0,23,166,167,98,61,61,197,128,60,318,
302,1,91,92,93,182,331,231,182,138,99,22,23,128,25,5,6,91,92,93,98,64,567,413,98,
99,145,63,39,393,1,79,1,397,1,46,105,64,116,108,51,39,116,17,396,111,79,58,73,98,
128,84,63,51,128,129,67,98,132,125,462,72,111,78,138,119,739,470,79,73,111,98,
120,84,136,181,260,261,105,263,860,286,160,161,162,110,160,161,162,869,442,109,
103,104,105,106,121,159,500,114,503,491,156,95,122,91,92,93,108,284,121,656,98,
99,289,126,106,121,122,93,163,91,113,197,95,93,490,109,126,140,116,332,175,334,
145,460,190,108,149,108,122,108,128,129,469,113,157,348,467,119,281,106,138,69,
70,71,829,235,93,199,200,235,202,93,175,100,232,122,352,313,100,574,291,108,160,
161,162,175,851,93,220,116,93,573,328,239,100,293,93,100,863,113,108,104,108,74,
75,108,871,115,116,93,116,93,118,119,120,118,119,120,100,197,103,110,288,304,278,
108,108,230,117,113,113,163,116,116,237,630,46,113,304,93,230,311,113,175,118,
311,100,237,93,315,121,105,92,127,128,100,326,235,355,262,326,331,98,108,318,331,
332,104,334,114,106,108,262,118,119,120,311,302,108,476,108,104,348,92,640,355,
116,110,291,355,113,326,93,103,104,105,106,692,99,302,328,98,697,108,91,92,93,
523,505,106,93,98,99,104,105,413,99,100,101,99,106,105,105,326,110,328,304,621,
106,116,140,113,110,311,93,94,326,93,341,532,99,128,129,105,100,377,378,379,326,
643,130,138,108,331,332,401,334,110,107,104,105,114,118,119,120,155,115,116,0,
105,348,93,418,160,161,162,95,355,104,106,104,698,104,110,110,508,110,109,509,21,
460,472,114,491,460,454,454,108,104,469,403,508,405,469,110,104,472,197,462,804,
197,110,104,467,46,113,419,115,110,104,52,53,54,55,104,110,104,643,93,61,110,628,
110,94,103,632,106,731,508,108,110,507,508,509,113,566,500,116,235,104,108,231,
106,110,104,110,110,114,116,249,118,119,120,488,631,110,532,492,104,114,106,103,
104,105,106,573,599,480,107,111,0,775,460,652,607,488,652,67,68,492,110,469,803,
125,472,104,663,109,110,477,566,110,718,110,536,76,77,114,140,72,73,708,244,109,
110,731,108,304,250,600,109,253,254,526,311,104,46,122,123,575,508,509,312,110,
600,110,526,106,596,326,107,61,332,113,331,332,93,334,109,603,340,108,342,108,
532,108,346,337,348,775,108,348,660,343,344,345,109,110,355,572,109,110,362,109,
110,355,118,119,120,772,109,110,572,103,104,105,106,108,566,647,108,111,108,403,
108,405,621,667,109,110,232,667,17,18,19,125,5,6,398,109,110,391,369,370,371,372,
396,108,621,108,140,797,600,93,94,95,109,110,698,109,110,106,698,611,424,109,110,
427,78,429,763,109,110,433,125,838,109,110,22,23,126,827,124,829,109,110,109,110,
109,110,450,105,442,109,110,106,842,22,23,672,480,109,110,460,109,110,109,110,
458,106,777,106,469,859,860,472,93,863,104,667,109,110,109,110,474,105,80,81,82,
83,84,85,86,87,88,89,772,777,104,341,108,777,232,1,108,447,109,110,106,451,698,
508,509,107,114,750,735,736,367,368,510,373,374,93,375,376,107,25,26,109,109,114,
122,523,108,532,103,750,106,803,38,117,40,106,121,43,121,106,110,38,108,79,109,
51,52,110,108,55,56,57,58,403,838,405,109,63,109,113,66,566,104,110,70,71,114,
105,108,567,578,104,30,104,582,583,772,42,98,93,108,777,109,433,109,122,98,93,94,
95,96,97,1,443,100,600,93,94,109,96,97,117,108,100,454,109,112,109,93,110,109,
108,118,109,111,112,122,123,124,110,110,118,110,129,130,106,123,124,110,38,480,
109,109,108,105,104,109,104,121,25,104,649,838,117,106,643,109,117,109,121,109,
117,109,651,105,109,109,113,656,104,106,659,667,105,93,403,104,405,98,109,678,
679,94,681,109,683,109,108,110,687,106,108,28,106,93,94,105,96,97,114,98,100,110,
698,110,433,25,104,94,108,106,114,111,112,121,443,109,715,108,118,114,106,93,114,
123,124,454,109,109,109,718,109,0,0,674,515,341,677,97,667,230,178,235,731,181,
182,744,572,43,747,748,611,480,136,191,192,596,803,603,607,128,493,199,200,621,
202,361,364,157,363,58,- 1,365,- 1,- 1,366,- 1,772,- 1,- 1,- 1,- 1,777,220,- 1,- 1,- 1,775,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,794,- 1,- 1,- 1,- 1,799,742,38,- 1,745,- 1,647,806,- 1,- 1,- 1,- 1,
- 1,812,755,756,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,828,- 1,- 1,- 1,- 1,- 1,38,- 1,-
1,- 1,780,- 1,- 1,838,- 1,785,786,- 1,- 1,789,790,- 1,792,- 1,- 1,- 1,293,- 1,93,94,- 1,96,
97,- 1,- 1,100,- 1,- 1,596,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,819,115,116,822,118,824,
825,- 1,- 1,123,124,- 1,93,94,- 1,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,848,
111,112,- 1,- 1,853,854,- 1,118,- 1,- 1,647,- 1,123,124,- 1,361,- 1,363,364,365,366,367,
368,369,370,371,372,373,374,375,376,377,378,379,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1,413,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,
36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,
63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,491,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,
- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,536,- 1,- 1,- 1,129,130,- 1,- 1,- 1,- 1,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,573,29,30,31,
32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,
59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,106,652,108,- 1,
- 1,111,112,- 1,- 1,660,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,1,- 1,3,4,
5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
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
65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,
- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,
43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,
70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,
- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,
51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
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
_tmp3EF[8]="stdcall";static char _tmp3F0[6]="cdecl";static char _tmp3F1[9]="fastcall";
static char _tmp3F2[9]="noreturn";static char _tmp3F3[6]="const";static char _tmp3F4[8]="aligned";
static char _tmp3F5[7]="packed";static char _tmp3F6[7]="shared";static char _tmp3F7[7]="unused";
static char _tmp3F8[5]="weak";static char _tmp3F9[10]="dllimport";static char _tmp3FA[
10]="dllexport";static char _tmp3FB[23]="no_instrument_function";static char _tmp3FC[
12]="constructor";static char _tmp3FD[11]="destructor";static char _tmp3FE[22]="no_check_memory_usage";
static char _tmp3FF[5]="pure";struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp945=10000;short*_tmp946=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp945));{unsigned int _tmp947=
_tmp945;unsigned int i;for(i=0;i < _tmp947;i ++){_tmp946[i]=(short)0;}}_tmp946;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp942=10000;void**_tmp943=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp942));{unsigned int _tmp944=
_tmp942;unsigned int i;for(i=0;i < _tmp944;i ++){_tmp943[i]=Cyc_yylval;}}_tmp943;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp93F=
10000;struct Cyc_Yyltype*_tmp940=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp93F));{unsigned int _tmp941=_tmp93F;
unsigned int i;for(i=0;i < _tmp941;i ++){_tmp940[i]=Cyc_yynewloc();}}_tmp940;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp341="parser stack overflow";_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(
_tmp341,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(879,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar <= 358: 0)?(
int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 247;}yyn +=
yychar1;if((yyn < 0?1: yyn > 6158)?1: Cyc_yycheck[_check_known_subscript_notnull(
6159,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6159,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 878)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(879,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(434,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1C2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1C3: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp342;_push_handler(& _tmp342);{int _tmp344=0;if(setjmp(_tmp342.handler))
_tmp344=1;if(!_tmp344){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp343=(void*)_exn_thrown;void*_tmp346=_tmp343;_LL1C6: if(*((void**)_tmp346)
!= Cyc_Core_Failure)goto _LL1C8;_LL1C7: x=0;goto _LL1C5;_LL1C8:;_LL1C9:(void)_throw(
_tmp346);_LL1C5:;}}}{struct _handler_cons _tmp347;_push_handler(& _tmp347);{int
_tmp349=0;if(setjmp(_tmp347.handler))_tmp349=1;if(!_tmp349){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp348=(void*)_exn_thrown;void*_tmp34B=_tmp348;_LL1CB: if(*((void**)_tmp34B)
!= Cyc_Core_Failure)goto _LL1CD;_LL1CC: y=0;goto _LL1CA;_LL1CD:;_LL1CE:(void)_throw(
_tmp34B);_LL1CA:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34C=
_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_DeclList_tok_struct _tmp34D;
_tmp34D.tag=Cyc_DeclList_tok;_tmp34D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp34D;});_tmp34C;});break;}
case 3: _LL1C4: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34E=_cycalloc(
sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_DeclList_tok_struct _tmp34F;_tmp34F.tag=
Cyc_DeclList_tok;_tmp34F.f1=({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*
_tmp350));_tmp350->hd=({struct Cyc_Absyn_Decl*_tmp351=_cycalloc(sizeof(*_tmp351));
_tmp351->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp352=_cycalloc(
sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_Using_d_struct _tmp353;_tmp353.tag=
9;_tmp353.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp353.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp353;});_tmp352;}));
_tmp351->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp351;});_tmp350->tl=0;_tmp350;});_tmp34F;});
_tmp34E;});Cyc_Lex_leave_using();break;case 4: _LL1CF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_DeclList_tok_struct
_tmp355;_tmp355.tag=Cyc_DeclList_tok;_tmp355.f1=({struct Cyc_List_List*_tmp356=
_cycalloc(sizeof(*_tmp356));_tmp356->hd=({struct Cyc_Absyn_Decl*_tmp357=_cycalloc(
sizeof(*_tmp357));_tmp357->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_Using_d_struct
_tmp359;_tmp359.tag=9;_tmp359.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp359.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp359;});_tmp358;}));
_tmp357->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp357;});_tmp356->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp356;});_tmp355;});
_tmp354;});break;case 5: _LL1D0: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_DeclList_tok_struct
_tmp35B;_tmp35B.tag=Cyc_DeclList_tok;_tmp35B.f1=({struct Cyc_List_List*_tmp35C=
_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=({struct Cyc_Absyn_Decl*_tmp35D=_cycalloc(
sizeof(*_tmp35D));_tmp35D->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp35F;_tmp35F.tag=8;_tmp35F.f1=({struct _tagged_arr*_tmp360=_cycalloc(sizeof(*
_tmp360));_tmp360[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp360;});_tmp35F.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp35F;});_tmp35E;}));
_tmp35D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp35D;});_tmp35C->tl=0;_tmp35C;});_tmp35B;});
_tmp35A;});Cyc_Lex_leave_namespace();break;case 6: _LL1D1: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp361;
_push_handler(& _tmp361);{int _tmp363=0;if(setjmp(_tmp361.handler))_tmp363=1;if(!
_tmp363){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp362=(void*)_exn_thrown;void*
_tmp365=_tmp362;_LL1D4: if(*((void**)_tmp365)!= Cyc_Core_Failure)goto _LL1D6;
_LL1D5: nspace=({const char*_tmp366="";_tag_arr(_tmp366,sizeof(char),
_get_zero_arr_size(_tmp366,1));});goto _LL1D3;_LL1D6:;_LL1D7:(void)_throw(_tmp365);
_LL1D3:;}}}{struct _handler_cons _tmp367;_push_handler(& _tmp367);{int _tmp369=0;if(
setjmp(_tmp367.handler))_tmp369=1;if(!_tmp369){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp368=(void*)_exn_thrown;void*_tmp36B=_tmp368;_LL1D9: if(*((void**)_tmp36B)
!= Cyc_Core_Failure)goto _LL1DB;_LL1DA: x=0;goto _LL1D8;_LL1DB:;_LL1DC:(void)_throw(
_tmp36B);_LL1D8:;}}}{struct _handler_cons _tmp36C;_push_handler(& _tmp36C);{int
_tmp36E=0;if(setjmp(_tmp36C.handler))_tmp36E=1;if(!_tmp36E){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp36D=(void*)_exn_thrown;void*_tmp370=_tmp36D;_LL1DE: if(*((void**)_tmp370)
!= Cyc_Core_Failure)goto _LL1E0;_LL1DF: y=0;goto _LL1DD;_LL1E0:;_LL1E1:(void)_throw(
_tmp370);_LL1DD:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp371=
_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_DeclList_tok_struct _tmp372;
_tmp372.tag=Cyc_DeclList_tok;_tmp372.f1=({struct Cyc_List_List*_tmp373=_cycalloc(
sizeof(*_tmp373));_tmp373->hd=({struct Cyc_Absyn_Decl*_tmp374=_cycalloc(sizeof(*
_tmp374));_tmp374->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp375=
_cycalloc(sizeof(*_tmp375));_tmp375[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp376;_tmp376.tag=8;_tmp376.f1=({struct _tagged_arr*_tmp377=_cycalloc(sizeof(*
_tmp377));_tmp377[0]=nspace;_tmp377;});_tmp376.f2=x;_tmp376;});_tmp375;}));
_tmp374->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp374;});_tmp373->tl=y;_tmp373;});_tmp372;});
_tmp371;});break;}case 7: _LL1D2: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp378="C";
_tag_arr(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp379="only extern \"C\" { ... } is allowed";_tag_arr(_tmp379,
sizeof(char),_get_zero_arr_size(_tmp379,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=({
struct Cyc_DeclList_tok_struct _tmp37B;_tmp37B.tag=Cyc_DeclList_tok;_tmp37B.f1=({
struct Cyc_List_List*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->hd=({struct Cyc_Absyn_Decl*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp37F;_tmp37F.tag=10;_tmp37F.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp37F;});_tmp37E;}));
_tmp37D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp37D;});_tmp37C->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37C;});_tmp37B;});
_tmp37A;});break;case 8: _LL1E2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380[0]=({struct Cyc_DeclList_tok_struct
_tmp381;_tmp381.tag=Cyc_DeclList_tok;_tmp381.f1=0;_tmp381;});_tmp380;});break;
case 9: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp382=_cycalloc(
sizeof(*_tmp382));_tmp382[0]=({struct Cyc_DeclList_tok_struct _tmp383;_tmp383.tag=
Cyc_DeclList_tok;_tmp383.f1=({struct Cyc_List_List*_tmp384=_cycalloc(sizeof(*
_tmp384));_tmp384->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp386;_tmp386.tag=1;_tmp386.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp386;});_tmp385;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp384->tl=0;
_tmp384;});_tmp383;});_tmp382;});break;case 10: _LL1E4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1E5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp387=_cycalloc(sizeof(*_tmp387));
_tmp387[0]=({struct Cyc_DeclList_tok_struct _tmp388;_tmp388.tag=Cyc_DeclList_tok;
_tmp388.f1=0;_tmp388;});_tmp387;});break;case 12: _LL1E6: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389[0]=({struct Cyc_FnDecl_tok_struct
_tmp38A;_tmp38A.tag=Cyc_FnDecl_tok;_tmp38A.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38A;});
_tmp389;});break;case 13: _LL1E7: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B[0]=({struct Cyc_FnDecl_tok_struct
_tmp38C;_tmp38C.tag=Cyc_FnDecl_tok;_tmp38C.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38D;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38C;});
_tmp38B;});break;case 14: _LL1E8: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_FnDecl_tok_struct
_tmp38F;_tmp38F.tag=Cyc_FnDecl_tok;_tmp38F.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38F;});
_tmp38E;});break;case 15: _LL1E9: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_FnDecl_tok_struct
_tmp391;_tmp391.tag=Cyc_FnDecl_tok;_tmp391.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp392;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp391;});
_tmp390;});break;case 16: _LL1EA: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393[0]=({struct Cyc_FnDecl_tok_struct
_tmp394;_tmp394.tag=Cyc_FnDecl_tok;_tmp394.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp395;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp394;});
_tmp393;});break;case 17: _LL1EB: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_FnDecl_tok_struct
_tmp397;_tmp397.tag=Cyc_FnDecl_tok;_tmp397.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp398;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp397;});
_tmp396;});break;case 18: _LL1EC: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 19: _LL1ED: Cyc_Lex_leave_using();
break;case 20: _LL1EE: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp399=
_cycalloc(sizeof(*_tmp399));_tmp399[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp399;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 21: _LL1EF: Cyc_Lex_leave_namespace();
break;case 22: _LL1F0: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp39A=
_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_DeclList_tok_struct _tmp39B;
_tmp39B.tag=Cyc_DeclList_tok;_tmp39B.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp39B;});
_tmp39A;});break;case 23: _LL1F1: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_DeclList_tok_struct
_tmp39D;_tmp39D.tag=Cyc_DeclList_tok;_tmp39D.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp39D;});
_tmp39C;});break;case 24: _LL1F2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E[0]=({struct Cyc_DeclList_tok_struct
_tmp39F;_tmp39F.tag=Cyc_DeclList_tok;_tmp39F.f1=({struct Cyc_List_List*_tmp3A0=
_cycalloc(sizeof(*_tmp3A0));_tmp3A0->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3A0->tl=0;
_tmp3A0;});_tmp39F;});_tmp39E;});break;case 25: _LL1F3: {struct Cyc_List_List*
_tmp3A1=0;{struct Cyc_List_List*_tmp3A2=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3A2 != 0;
_tmp3A2=_tmp3A2->tl){struct _tagged_arr*_tmp3A3=(struct _tagged_arr*)_tmp3A2->hd;
struct _tuple1*_tmp3A4=({struct _tuple1*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));
_tmp3A7->f1=Cyc_Absyn_rel_ns_null;_tmp3A7->f2=_tmp3A3;_tmp3A7;});struct Cyc_Absyn_Vardecl*
_tmp3A5=Cyc_Absyn_new_vardecl(_tmp3A4,Cyc_Absyn_wildtyp(0),0);_tmp3A1=({struct
Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->hd=_tmp3A5;_tmp3A6->tl=
_tmp3A1;_tmp3A6;});}}_tmp3A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3A1);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A8=
_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({struct Cyc_DeclList_tok_struct _tmp3A9;
_tmp3A9.tag=Cyc_DeclList_tok;_tmp3A9.f1=({struct Cyc_List_List*_tmp3AA=_cycalloc(
sizeof(*_tmp3AA));_tmp3AA->hd=Cyc_Absyn_letv_decl(_tmp3A1,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3AA->tl=0;
_tmp3AA;});_tmp3A9;});_tmp3A8;});break;}case 26: _LL1F4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL1F5: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));
_tmp3AB[0]=({struct Cyc_DeclList_tok_struct _tmp3AC;_tmp3AC.tag=Cyc_DeclList_tok;
_tmp3AC.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3AC;});_tmp3AB;});
break;case 28: _LL1F6: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3AD=
_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_DeclSpec_tok_struct _tmp3AE;
_tmp3AE.tag=Cyc_DeclSpec_tok;_tmp3AE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->sc=({struct Cyc_Core_Opt*_tmp3B0=
_cycalloc(sizeof(*_tmp3B0));_tmp3B0->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B0;});_tmp3AF->tq=Cyc_Absyn_empty_tqual();
_tmp3AF->type_specs=0;_tmp3AF->is_inline=0;_tmp3AF->attributes=0;_tmp3AF;});
_tmp3AE;});_tmp3AD;});break;case 29: _LL1F7: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp3B1[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp3B2="Only one storage class is allowed in a declaration";
_tag_arr(_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,51));}),_tag_arr(
_tmp3B1,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B4;_tmp3B4.tag=Cyc_DeclSpec_tok;_tmp3B4.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->sc=({struct Cyc_Core_Opt*_tmp3B6=
_cycalloc(sizeof(*_tmp3B6));_tmp3B6->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B6;});_tmp3B5->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3B5->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3B5->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3B5->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3B5;});_tmp3B4;});_tmp3B3;});break;case 30: _LL1F8: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B8;_tmp3B8.tag=Cyc_DeclSpec_tok;_tmp3B8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->sc=0;_tmp3B9->tq=Cyc_Absyn_empty_tqual();
_tmp3B9->type_specs=({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));
_tmp3BA->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BA->tl=0;_tmp3BA;});
_tmp3B9->is_inline=0;_tmp3B9->attributes=0;_tmp3B9;});_tmp3B8;});_tmp3B7;});
break;case 31: _LL1F9: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3BB=
_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_DeclSpec_tok_struct _tmp3BC;
_tmp3BC.tag=Cyc_DeclSpec_tok;_tmp3BC.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3BD->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3BD->type_specs=({
struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3BE->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3BE;});
_tmp3BD->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3BD->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3BD;});
_tmp3BC;});_tmp3BB;});break;case 32: _LL1FA: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C0;_tmp3C0.tag=Cyc_DeclSpec_tok;_tmp3C0.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->sc=0;_tmp3C1->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C1->type_specs=0;
_tmp3C1->is_inline=0;_tmp3C1->attributes=0;_tmp3C1;});_tmp3C0;});_tmp3BF;});
break;case 33: _LL1FB: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3C2=
_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_DeclSpec_tok_struct _tmp3C3;
_tmp3C3.tag=Cyc_DeclSpec_tok;_tmp3C3.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3C4->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3C4->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3C4->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3C4->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3C4;});_tmp3C3;});_tmp3C2;});break;case 34: _LL1FC: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C6;_tmp3C6.tag=Cyc_DeclSpec_tok;_tmp3C6.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->sc=0;_tmp3C7->tq=Cyc_Absyn_empty_tqual();
_tmp3C7->type_specs=0;_tmp3C7->is_inline=1;_tmp3C7->attributes=0;_tmp3C7;});
_tmp3C6;});_tmp3C5;});break;case 35: _LL1FD: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3C9;_tmp3C9.tag=Cyc_DeclSpec_tok;_tmp3C9.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3CA->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3CA->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3CA->is_inline=1;_tmp3CA->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3CA;});
_tmp3C9;});_tmp3C8;});break;case 36: _LL1FE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CC;_tmp3CC.tag=Cyc_DeclSpec_tok;_tmp3CC.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->sc=0;_tmp3CD->tq=Cyc_Absyn_empty_tqual();
_tmp3CD->type_specs=0;_tmp3CD->is_inline=0;_tmp3CD->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CD;});_tmp3CC;});
_tmp3CB;});break;case 37: _LL1FF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3CF;_tmp3CF.tag=Cyc_DeclSpec_tok;_tmp3CF.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3D0->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3D0->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3D0->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3D0->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3D0;});
_tmp3CF;});_tmp3CE;});break;case 38: _LL200: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_StorageClass_tok;_tmp3D2.f1=(void*)((void*)4);_tmp3D2;});
_tmp3D1;});break;case 39: _LL201: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D4;_tmp3D4.tag=Cyc_StorageClass_tok;_tmp3D4.f1=(void*)((void*)5);_tmp3D4;});
_tmp3D3;});break;case 40: _LL202: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D6;_tmp3D6.tag=Cyc_StorageClass_tok;_tmp3D6.f1=(void*)((void*)3);_tmp3D6;});
_tmp3D5;});break;case 41: _LL203: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_StorageClass_tok_struct
_tmp3D8;_tmp3D8.tag=Cyc_StorageClass_tok;_tmp3D8.f1=(void*)((void*)1);_tmp3D8;});
_tmp3D7;});break;case 42: _LL204: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3D9="C";
_tag_arr(_tmp3D9,sizeof(char),_get_zero_arr_size(_tmp3D9,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3DA="only extern or extern \"C\" is allowed";_tag_arr(_tmp3DA,
sizeof(char),_get_zero_arr_size(_tmp3DA,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({
struct Cyc_StorageClass_tok_struct _tmp3DC;_tmp3DC.tag=Cyc_StorageClass_tok;
_tmp3DC.f1=(void*)((void*)2);_tmp3DC;});_tmp3DB;});break;case 43: _LL205: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD[0]=({struct Cyc_StorageClass_tok_struct _tmp3DE;_tmp3DE.tag=Cyc_StorageClass_tok;
_tmp3DE.f1=(void*)((void*)0);_tmp3DE;});_tmp3DD;});break;case 44: _LL206: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));
_tmp3DF[0]=({struct Cyc_StorageClass_tok_struct _tmp3E0;_tmp3E0.tag=Cyc_StorageClass_tok;
_tmp3E0.f1=(void*)((void*)6);_tmp3E0;});_tmp3DF;});break;case 45: _LL207: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));
_tmp3E1[0]=({struct Cyc_AttributeList_tok_struct _tmp3E2;_tmp3E2.tag=Cyc_AttributeList_tok;
_tmp3E2.f1=0;_tmp3E2;});_tmp3E1;});break;case 46: _LL208: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 47: _LL209: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 48: _LL20A:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3E3=_cycalloc(sizeof(*
_tmp3E3));_tmp3E3[0]=({struct Cyc_AttributeList_tok_struct _tmp3E4;_tmp3E4.tag=Cyc_AttributeList_tok;
_tmp3E4.f1=({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3E5->tl=0;_tmp3E5;});_tmp3E4;});_tmp3E3;});break;case 49:
_LL20B: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3E6=_cycalloc(
sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_AttributeList_tok_struct _tmp3E7;_tmp3E7.tag=
Cyc_AttributeList_tok;_tmp3E7.f1=({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*
_tmp3E8));_tmp3E8->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E8->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E8;});_tmp3E7;});
_tmp3E6;});break;case 50: _LL20C: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple17 att_map[17]={{{_tmp3EF,_tmp3EF,_tmp3EF + 8},(
void*)0},{{_tmp3F0,_tmp3F0,_tmp3F0 + 6},(void*)1},{{_tmp3F1,_tmp3F1,_tmp3F1 + 9},(
void*)2},{{_tmp3F2,_tmp3F2,_tmp3F2 + 9},(void*)3},{{_tmp3F3,_tmp3F3,_tmp3F3 + 6},(
void*)4},{{_tmp3F4,_tmp3F4,_tmp3F4 + 8},(void*)& att_aligned},{{_tmp3F5,_tmp3F5,
_tmp3F5 + 7},(void*)5},{{_tmp3F6,_tmp3F6,_tmp3F6 + 7},(void*)7},{{_tmp3F7,_tmp3F7,
_tmp3F7 + 7},(void*)8},{{_tmp3F8,_tmp3F8,_tmp3F8 + 5},(void*)9},{{_tmp3F9,_tmp3F9,
_tmp3F9 + 10},(void*)10},{{_tmp3FA,_tmp3FA,_tmp3FA + 10},(void*)11},{{_tmp3FB,
_tmp3FB,_tmp3FB + 23},(void*)12},{{_tmp3FC,_tmp3FC,_tmp3FC + 12},(void*)13},{{
_tmp3FD,_tmp3FD,_tmp3FD + 11},(void*)14},{{_tmp3FE,_tmp3FE,_tmp3FE + 22},(void*)15},{{
_tmp3FF,_tmp3FF,_tmp3FF + 5},(void*)16}};struct _tagged_arr _tmp3E9=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp3E9,sizeof(char))> 4?*((const char*)_check_unknown_subscript(_tmp3E9,sizeof(
char),0))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E9,sizeof(char),1))
== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3E9,sizeof(char),(int)(
_get_arr_size(_tmp3E9,sizeof(char))- 2)))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp3E9,sizeof(char),(int)(_get_arr_size(_tmp3E9,
sizeof(char))- 3)))== '_': 0)_tmp3E9=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp3E9,2,_get_arr_size(_tmp3E9,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp3E9,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3EA=_cycalloc(sizeof(*
_tmp3EA));_tmp3EA[0]=({struct Cyc_Attribute_tok_struct _tmp3EB;_tmp3EB.tag=Cyc_Attribute_tok;
_tmp3EB.f1=(void*)(att_map[i]).f2;_tmp3EB;});_tmp3EA;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp3EC="unrecognized attribute";_tag_arr(_tmp3EC,sizeof(char),
_get_zero_arr_size(_tmp3EC,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({
struct Cyc_Attribute_tok_struct _tmp3EE;_tmp3EE.tag=Cyc_Attribute_tok;_tmp3EE.f1=(
void*)((void*)1);_tmp3EE;});_tmp3ED;});}break;}}case 51: _LL20D: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({
struct Cyc_Attribute_tok_struct _tmp402;_tmp402.tag=Cyc_Attribute_tok;_tmp402.f1=(
void*)((void*)4);_tmp402;});_tmp401;});break;case 52: _LL20E: {struct _tagged_arr
_tmp403=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp405;int _tmp406;struct _tuple13*_tmp404=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp405=*_tmp404;
_tmp406=_tmp405.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp403,({const char*
_tmp407="regparm";_tag_arr(_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,8));}))
== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp403,({const char*_tmp408="__regparm__";
_tag_arr(_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,12));}))== 0){if(
_tmp406 < 0?1: _tmp406 > 3)Cyc_Parse_err(({const char*_tmp409="regparm requires value between 0 and 3";
_tag_arr(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp40A=_cycalloc_atomic(sizeof(*_tmp40A));
_tmp40A[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=
_tmp406;_tmp40B;});_tmp40A;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp403,({
const char*_tmp40C="aligned";_tag_arr(_tmp40C,sizeof(char),_get_zero_arr_size(
_tmp40C,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp403,({const char*_tmp40D="__aligned__";
_tag_arr(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,12));}))== 0){if(
_tmp406 < 0)Cyc_Parse_err(({const char*_tmp40E="aligned requires positive power of two";
_tag_arr(_tmp40E,sizeof(char),_get_zero_arr_size(_tmp40E,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp406;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp40F="aligned requires positive power of two";
_tag_arr(_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp410=_cycalloc_atomic(sizeof(*_tmp410));
_tmp410[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp411;_tmp411.tag=1;_tmp411.f1=
_tmp406;_tmp411;});_tmp410;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp403,({
const char*_tmp412="initializes";_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(
_tmp412,12));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp403,({const char*
_tmp413="__initializes__";_tag_arr(_tmp413,sizeof(char),_get_zero_arr_size(
_tmp413,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp414=
_cycalloc_atomic(sizeof(*_tmp414));_tmp414[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp415;_tmp415.tag=4;_tmp415.f1=_tmp406;_tmp415;});_tmp414;});else{Cyc_Parse_err(({
const char*_tmp416="unrecognized attribute";_tag_arr(_tmp416,sizeof(char),
_get_zero_arr_size(_tmp416,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp417=_cycalloc(sizeof(*_tmp417));
_tmp417[0]=({struct Cyc_Attribute_tok_struct _tmp418;_tmp418.tag=Cyc_Attribute_tok;
_tmp418.f1=(void*)a;_tmp418;});_tmp417;});break;}}case 53: _LL20F: {struct
_tagged_arr _tmp419=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp41A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp419,({const char*_tmp41B="section";_tag_arr(_tmp41B,sizeof(
char),_get_zero_arr_size(_tmp41B,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)
_tmp419,({const char*_tmp41C="__section__";_tag_arr(_tmp41C,sizeof(char),
_get_zero_arr_size(_tmp41C,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Absyn_Section_att_struct
_tmp41E;_tmp41E.tag=2;_tmp41E.f1=_tmp41A;_tmp41E;});_tmp41D;});else{Cyc_Parse_err(({
const char*_tmp41F="unrecognized attribute";_tag_arr(_tmp41F,sizeof(char),
_get_zero_arr_size(_tmp41F,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp420=_cycalloc(sizeof(*_tmp420));
_tmp420[0]=({struct Cyc_Attribute_tok_struct _tmp421;_tmp421.tag=Cyc_Attribute_tok;
_tmp421.f1=(void*)a;_tmp421;});_tmp420;});break;}case 54: _LL210: {struct
_tagged_arr _tmp422=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp423=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp422,({const char*_tmp424="__mode__";_tag_arr(_tmp424,
sizeof(char),_get_zero_arr_size(_tmp424,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp426;_tmp426.tag=5;_tmp426.f1=_tmp423;_tmp426;});_tmp425;});else{Cyc_Parse_err(({
const char*_tmp427="unrecognized attribute";_tag_arr(_tmp427,sizeof(char),
_get_zero_arr_size(_tmp427,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428[0]=({struct Cyc_Attribute_tok_struct _tmp429;_tmp429.tag=Cyc_Attribute_tok;
_tmp429.f1=(void*)a;_tmp429;});_tmp428;});break;}case 55: _LL211: {struct
_tagged_arr _tmp42A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp42B=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp42D;struct
_tuple13 _tmp42C=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp42D=_tmp42C.f2;{int _tmp42F;struct _tuple13 _tmp42E=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp42F=_tmp42E.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp42A,({const char*_tmp430="format";
_tag_arr(_tmp430,sizeof(char),_get_zero_arr_size(_tmp430,7));}))== 0?1: Cyc_zstrcmp((
struct _tagged_arr)_tmp42A,({const char*_tmp431="__format__";_tag_arr(_tmp431,
sizeof(char),_get_zero_arr_size(_tmp431,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp42B,({const char*_tmp432="printf";_tag_arr(_tmp432,sizeof(char),
_get_zero_arr_size(_tmp432,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_Absyn_Format_att_struct
_tmp434;_tmp434.tag=3;_tmp434.f1=(void*)((void*)0);_tmp434.f2=_tmp42D;_tmp434.f3=
_tmp42F;_tmp434;});_tmp433;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp42B,({
const char*_tmp435="scanf";_tag_arr(_tmp435,sizeof(char),_get_zero_arr_size(
_tmp435,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp436=
_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_Absyn_Format_att_struct
_tmp437;_tmp437.tag=3;_tmp437.f1=(void*)((void*)1);_tmp437.f2=_tmp42D;_tmp437.f3=
_tmp42F;_tmp437;});_tmp436;});else{Cyc_Parse_err(({const char*_tmp438="unrecognized format type";
_tag_arr(_tmp438,sizeof(char),_get_zero_arr_size(_tmp438,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp439="unrecognized attribute";_tag_arr(_tmp439,sizeof(char),
_get_zero_arr_size(_tmp439,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({
struct Cyc_Attribute_tok_struct _tmp43B;_tmp43B.tag=Cyc_Attribute_tok;_tmp43B.f1=(
void*)a;_tmp43B;});_tmp43A;});break;}}}case 56: _LL212: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43D;_tmp43D.tag=Cyc_TypeSpecifier_tok;_tmp43D.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp43D;});_tmp43C;});break;case 57: _LL213: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));
_tmp43E[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp43F;_tmp43F.tag=Cyc_TypeSpecifier_tok;
_tmp43F.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43F;});
_tmp43E;});break;case 58: _LL214: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp441;_tmp441.tag=Cyc_TypeSpecifier_tok;_tmp441.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp442=_cycalloc(sizeof(*_tmp442));
_tmp442->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp442;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp441;});
_tmp440;});break;case 59: _LL215: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp444;_tmp444.tag=Cyc_TypeSpecifier_tok;_tmp444.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp444;});
_tmp443;});break;case 60: _LL216: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp446;_tmp446.tag=Cyc_TypeSpecifier_tok;_tmp446.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447[0]=({struct Cyc_Parse_Short_spec_struct
_tmp448;_tmp448.tag=2;_tmp448.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp448;});
_tmp447;}));_tmp446;});_tmp445;});break;case 61: _LL217: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44A;_tmp44A.tag=Cyc_TypeSpecifier_tok;_tmp44A.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44A;});
_tmp449;});break;case 62: _LL218: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44C;_tmp44C.tag=Cyc_TypeSpecifier_tok;_tmp44C.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_Parse_Long_spec_struct
_tmp44E;_tmp44E.tag=3;_tmp44E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp44E;});
_tmp44D;}));_tmp44C;});_tmp44B;});break;case 63: _LL219: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp450;_tmp450.tag=Cyc_TypeSpecifier_tok;_tmp450.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp450;});
_tmp44F;});break;case 64: _LL21A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp452;_tmp452.tag=Cyc_TypeSpecifier_tok;_tmp452.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp452;});
_tmp451;});break;case 65: _LL21B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp454;_tmp454.tag=Cyc_TypeSpecifier_tok;_tmp454.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp456;_tmp456.tag=0;_tmp456.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp456;});
_tmp455;}));_tmp454;});_tmp453;});break;case 66: _LL21C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp458;_tmp458.tag=Cyc_TypeSpecifier_tok;_tmp458.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp45A;_tmp45A.tag=1;_tmp45A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp45A;});
_tmp459;}));_tmp458;});_tmp457;});break;case 67: _LL21D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68: _LL21E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL21F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 70:
_LL220: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp45B=_cycalloc(
sizeof(*_tmp45B));_tmp45B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp45C;_tmp45C.tag=
Cyc_TypeSpecifier_tok;_tmp45C.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45C;});
_tmp45B;});break;case 71: _LL221: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45E;_tmp45E.tag=Cyc_TypeSpecifier_tok;_tmp45E.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));
_tmp45F[0]=({struct Cyc_Absyn_TypedefType_struct _tmp460;_tmp460.tag=16;_tmp460.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp460.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp460.f3=0;_tmp460.f4=0;_tmp460;});_tmp45F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45E;});
_tmp45D;});break;case 72: _LL222: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp462;_tmp462.tag=Cyc_TypeSpecifier_tok;_tmp462.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp463=_cycalloc(sizeof(*_tmp463));
_tmp463[0]=({struct Cyc_Absyn_TupleType_struct _tmp464;_tmp464.tag=9;_tmp464.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp464;});_tmp463;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp462;});_tmp461;});break;case 73: _LL223: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp465=_cycalloc(sizeof(*_tmp465));
_tmp465[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp466;_tmp466.tag=Cyc_TypeSpecifier_tok;
_tmp466.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp468;_tmp468.tag=15;_tmp468.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp468;});_tmp467;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp466;});_tmp465;});break;case 74: _LL224: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp469=_cycalloc(sizeof(*_tmp469));
_tmp469[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp46A;_tmp46A.tag=Cyc_TypeSpecifier_tok;
_tmp46A.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp46C;_tmp46C.tag=14;_tmp46C.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46C;});_tmp46B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46A;});_tmp469;});break;case 75: _LL225: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp46E;_tmp46E.tag=Cyc_TypeSpecifier_tok;
_tmp46E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_TagType_struct
_tmp470;_tmp470.tag=17;_tmp470.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp470;});_tmp46F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46E;});_tmp46D;});break;case 76: _LL226: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp471=_cycalloc(sizeof(*_tmp471));
_tmp471[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp472;_tmp472.tag=Cyc_TypeSpecifier_tok;
_tmp472.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473[0]=({struct Cyc_Absyn_TagType_struct
_tmp474;_tmp474.tag=17;_tmp474.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->v=(void*)((void*)5);_tmp475;}),0);
_tmp474;});_tmp473;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp472;});
_tmp471;});break;case 77: _LL227: {struct _tuple13 _tmp477;int _tmp478;struct _tuple13*
_tmp476=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp477=*_tmp476;_tmp478=_tmp477.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47A;_tmp47A.tag=Cyc_TypeSpecifier_tok;_tmp47A.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp47B=_cycalloc_atomic(sizeof(*_tmp47B));
_tmp47B[0]=({struct Cyc_Absyn_TypeInt_struct _tmp47C;_tmp47C.tag=18;_tmp47C.f1=
_tmp478;_tmp47C;});_tmp47B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47A;});
_tmp479;});break;}case 78: _LL228: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp47D=
_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=({struct Cyc_Kind_tok_struct _tmp47E;
_tmp47E.tag=Cyc_Kind_tok;_tmp47E.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47E;});
_tmp47D;});break;case 79: _LL229: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp47F=_cycalloc_atomic(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_TypeQual_tok_struct
_tmp480;_tmp480.tag=Cyc_TypeQual_tok;_tmp480.f1=({struct Cyc_Absyn_Tqual _tmp481;
_tmp481.q_const=1;_tmp481.q_volatile=0;_tmp481.q_restrict=0;_tmp481;});_tmp480;});
_tmp47F;});break;case 80: _LL22A: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp482=_cycalloc_atomic(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_TypeQual_tok_struct
_tmp483;_tmp483.tag=Cyc_TypeQual_tok;_tmp483.f1=({struct Cyc_Absyn_Tqual _tmp484;
_tmp484.q_const=0;_tmp484.q_volatile=1;_tmp484.q_restrict=0;_tmp484;});_tmp483;});
_tmp482;});break;case 81: _LL22B: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp485=_cycalloc_atomic(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_TypeQual_tok_struct
_tmp486;_tmp486.tag=Cyc_TypeQual_tok;_tmp486.f1=({struct Cyc_Absyn_Tqual _tmp487;
_tmp487.q_const=0;_tmp487.q_volatile=0;_tmp487.q_restrict=1;_tmp487;});_tmp486;});
_tmp485;});break;case 82: _LL22C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp489;_tmp489.tag=Cyc_TypeSpecifier_tok;_tmp489.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp48B;_tmp48B.tag=5;_tmp48B.f1=({struct Cyc_Absyn_Decl*_tmp48C=_cycalloc(
sizeof(*_tmp48C));_tmp48C->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp48E;_tmp48E.tag=6;_tmp48E.f1=({struct Cyc_Absyn_Enumdecl*_tmp48F=_cycalloc(
sizeof(*_tmp48F));_tmp48F->sc=(void*)((void*)2);_tmp48F->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp48F->fields=({
struct Cyc_Core_Opt*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp490;});_tmp48F;});
_tmp48E;});_tmp48D;}));_tmp48C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp48C;});
_tmp48B;});_tmp48A;}));_tmp489;});_tmp488;});break;case 83: _LL22D: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp492;_tmp492.tag=Cyc_TypeSpecifier_tok;
_tmp492.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_Absyn_EnumType_struct
_tmp494;_tmp494.tag=12;_tmp494.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp494.f2=0;_tmp494;});
_tmp493;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp492;});_tmp491;});break;case 84: _LL22E: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp495=_cycalloc(sizeof(*_tmp495));
_tmp495[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp496;_tmp496.tag=Cyc_TypeSpecifier_tok;
_tmp496.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp497=_cycalloc(
sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Parse_Type_spec_struct _tmp498;_tmp498.tag=
4;_tmp498.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp499=
_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp49A;_tmp49A.tag=13;_tmp49A.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49A;});_tmp499;}));
_tmp498.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp498;});_tmp497;}));_tmp496;});_tmp495;});break;
case 85: _LL22F: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp49B=_cycalloc(
sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_Enumfield_tok_struct _tmp49C;_tmp49C.tag=
Cyc_Enumfield_tok;_tmp49C.f1=({struct Cyc_Absyn_Enumfield*_tmp49D=_cycalloc(
sizeof(*_tmp49D));_tmp49D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49D->tag=0;_tmp49D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp49D;});_tmp49C;});_tmp49B;});break;case 86: _LL230:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));
_tmp49E[0]=({struct Cyc_Enumfield_tok_struct _tmp49F;_tmp49F.tag=Cyc_Enumfield_tok;
_tmp49F.f1=({struct Cyc_Absyn_Enumfield*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));
_tmp4A0->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4A0->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A0;});
_tmp49F;});_tmp49E;});break;case 87: _LL231: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4A2;_tmp4A2.tag=Cyc_EnumfieldList_tok;_tmp4A2.f1=({struct Cyc_List_List*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A3->tl=0;_tmp4A3;});
_tmp4A2;});_tmp4A1;});break;case 88: _LL232: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4A5;_tmp4A5.tag=Cyc_EnumfieldList_tok;_tmp4A5.f1=({struct Cyc_List_List*
_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A6->tl=0;_tmp4A6;});
_tmp4A5;});_tmp4A4;});break;case 89: _LL233: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4A8;_tmp4A8.tag=Cyc_EnumfieldList_tok;_tmp4A8.f1=({struct Cyc_List_List*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A9->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A9;});_tmp4A8;});
_tmp4A7;});break;case 90: _LL234: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4AB;_tmp4AB.tag=Cyc_TypeSpecifier_tok;_tmp4AB.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));
_tmp4AC[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4AD;_tmp4AD.tag=11;_tmp4AD.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4AD.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AD;});_tmp4AC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AB;});_tmp4AA;});break;case 91: _LL235: {struct
Cyc_List_List*_tmp4AE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4AF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4B1;_tmp4B1.tag=Cyc_TypeSpecifier_tok;
_tmp4B1.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4B2=_cycalloc(
sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4B3;_tmp4B3.tag=
5;_tmp4B3.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4AE,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4AF,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B3;});
_tmp4B2;}));_tmp4B1;});_tmp4B0;});break;}case 92: _LL236: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4B5;_tmp4B5.tag=Cyc_TypeSpecifier_tok;_tmp4B5.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));
_tmp4B6[0]=({struct Cyc_Absyn_AggrType_struct _tmp4B7;_tmp4B7.tag=10;_tmp4B7.f1=({
struct Cyc_Absyn_AggrInfo _tmp4B8;_tmp4B8.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4BA;_tmp4BA.tag=0;_tmp4BA.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4BA.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BA;});_tmp4B9;}));
_tmp4B8.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4B8;});_tmp4B7;});_tmp4B6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B5;});
_tmp4B4;});break;case 93: _LL237: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_TypeList_tok_struct
_tmp4BC;_tmp4BC.tag=Cyc_TypeList_tok;_tmp4BC.f1=0;_tmp4BC;});_tmp4BB;});break;
case 94: _LL238: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp4BD=_cycalloc(
sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_TypeList_tok_struct _tmp4BE;_tmp4BE.tag=
Cyc_TypeList_tok;_tmp4BE.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4BE;});_tmp4BD;});
break;case 95: _LL239: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp4BF=
_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4C0;_tmp4C0.tag=Cyc_StructOrUnion_tok;_tmp4C0.f1=(void*)((void*)0);_tmp4C0;});
_tmp4BF;});break;case 96: _LL23A: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp4C2;_tmp4C2.tag=Cyc_StructOrUnion_tok;_tmp4C2.f1=(void*)((void*)1);_tmp4C2;});
_tmp4C1;});break;case 97: _LL23B: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4C4;_tmp4C4.tag=Cyc_AggrFieldDeclList_tok;_tmp4C4.f1=0;_tmp4C4;});_tmp4C3;});
break;case 98: _LL23C: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp4C6;_tmp4C6.tag=Cyc_AggrFieldDeclList_tok;_tmp4C6.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4C6;});_tmp4C5;});
break;case 99: _LL23D: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4C8;_tmp4C8.tag=Cyc_AggrFieldDeclListList_tok;_tmp4C8.f1=({struct Cyc_List_List*
_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C9->tl=0;_tmp4C9;});
_tmp4C8;});_tmp4C7;});break;case 100: _LL23E: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp4CB;_tmp4CB.tag=Cyc_AggrFieldDeclListList_tok;_tmp4CB.f1=({struct Cyc_List_List*
_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CC->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4CC;});_tmp4CB;});
_tmp4CA;});break;case 101: _LL23F: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CE;_tmp4CE.tag=Cyc_InitDeclList_tok;_tmp4CE.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4CE;});_tmp4CD;});
break;case 102: _LL240: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4CF=
_cycalloc(sizeof(*_tmp4CF));_tmp4CF[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4D0;_tmp4D0.tag=Cyc_InitDeclList_tok;_tmp4D0.f1=({struct Cyc_List_List*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D1->tl=0;_tmp4D1;});
_tmp4D0;});_tmp4CF;});break;case 103: _LL241: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4D3;_tmp4D3.tag=Cyc_InitDeclList_tok;_tmp4D3.f1=({struct Cyc_List_List*
_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D4->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D4;});_tmp4D3;});
_tmp4D2;});break;case 104: _LL242: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D6;_tmp4D6.tag=Cyc_InitDecl_tok;_tmp4D6.f1=({struct _tuple12*_tmp4D7=
_cycalloc(sizeof(*_tmp4D7));_tmp4D7->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D7->f2=0;_tmp4D7;});
_tmp4D6;});_tmp4D5;});break;case 105: _LL243: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D9;_tmp4D9.tag=Cyc_InitDecl_tok;_tmp4D9.f1=({struct _tuple12*_tmp4DA=
_cycalloc(sizeof(*_tmp4DA));_tmp4DA->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DA->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4DA;});_tmp4D9;});_tmp4D8;});break;case 106: _LL244: {struct _tuple15 _tmp4DC;
struct Cyc_Absyn_Tqual _tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*
_tmp4DF;struct _tuple15*_tmp4DB=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DC=*_tmp4DB;_tmp4DD=
_tmp4DC.f1;_tmp4DE=_tmp4DC.f2;_tmp4DF=_tmp4DC.f3;{struct Cyc_List_List*_tmp4E1;
struct Cyc_List_List*_tmp4E2;struct _tuple0 _tmp4E0=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4E1=_tmp4E0.f1;
_tmp4E2=_tmp4E0.f2;{void*_tmp4E3=Cyc_Parse_speclist2typ(_tmp4DE,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4DD,_tmp4E3,_tmp4E1,_tmp4DF),_tmp4E2);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4E5=_cycalloc(sizeof(*
_tmp4E5));_tmp4E5[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp4E6;_tmp4E6.tag=
Cyc_AggrFieldDeclList_tok;_tmp4E6.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4E4);
_tmp4E6;});_tmp4E5;});break;}}}case 107: _LL245: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4E8;_tmp4E8.tag=Cyc_QualSpecList_tok;_tmp4E8.f1=({struct _tuple15*_tmp4E9=
_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=Cyc_Absyn_empty_tqual();_tmp4E9->f2=({
struct Cyc_List_List*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EA->tl=0;_tmp4EA;});
_tmp4E9->f3=0;_tmp4E9;});_tmp4E8;});_tmp4E7;});break;case 108: _LL246: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({
struct Cyc_QualSpecList_tok_struct _tmp4EC;_tmp4EC.tag=Cyc_QualSpecList_tok;
_tmp4EC.f1=({struct _tuple15*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4ED->f2=({struct Cyc_List_List*_tmp4EE=_cycalloc(sizeof(*
_tmp4EE));_tmp4EE->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EE->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4EE;});_tmp4ED->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp4ED;});_tmp4EC;});_tmp4EB;});break;case 109: _LL247: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));
_tmp4EF[0]=({struct Cyc_QualSpecList_tok_struct _tmp4F0;_tmp4F0.tag=Cyc_QualSpecList_tok;
_tmp4F0.f1=({struct _tuple15*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F1->f2=0;_tmp4F1->f3=
0;_tmp4F1;});_tmp4F0;});_tmp4EF;});break;case 110: _LL248: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4F3;_tmp4F3.tag=Cyc_QualSpecList_tok;_tmp4F3.f1=({struct _tuple15*_tmp4F4=
_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp4F4->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F4->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4F4;});_tmp4F3;});
_tmp4F2;});break;case 111: _LL249: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4F6;_tmp4F6.tag=Cyc_QualSpecList_tok;_tmp4F6.f1=({struct _tuple15*_tmp4F7=
_cycalloc(sizeof(*_tmp4F7));_tmp4F7->f1=Cyc_Absyn_empty_tqual();_tmp4F7->f2=0;
_tmp4F7->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4F7;});_tmp4F6;});_tmp4F5;});break;case 112: _LL24A:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp4F8=_cycalloc(sizeof(*
_tmp4F8));_tmp4F8[0]=({struct Cyc_QualSpecList_tok_struct _tmp4F9;_tmp4F9.tag=Cyc_QualSpecList_tok;
_tmp4F9.f1=({struct _tuple15*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4FA->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FA->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp4FA;});_tmp4F9;});
_tmp4F8;});break;case 113: _LL24B: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4FC;_tmp4FC.tag=Cyc_InitDeclList_tok;_tmp4FC.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4FC;});_tmp4FB;});
break;case 114: _LL24C: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4FD=
_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4FE;_tmp4FE.tag=Cyc_InitDeclList_tok;_tmp4FE.f1=({struct Cyc_List_List*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FF->tl=0;_tmp4FF;});
_tmp4FE;});_tmp4FD;});break;case 115: _LL24D: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_InitDeclList_tok_struct
_tmp501;_tmp501.tag=Cyc_InitDeclList_tok;_tmp501.f1=({struct Cyc_List_List*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp502->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp502;});_tmp501;});
_tmp500;});break;case 116: _LL24E: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503[0]=({struct Cyc_InitDecl_tok_struct
_tmp504;_tmp504.tag=Cyc_InitDecl_tok;_tmp504.f1=({struct _tuple12*_tmp505=
_cycalloc(sizeof(*_tmp505));_tmp505->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp505->f2=0;_tmp505;});
_tmp504;});_tmp503;});break;case 117: _LL24F: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_InitDecl_tok_struct
_tmp507;_tmp507.tag=Cyc_InitDecl_tok;_tmp507.f1=({struct _tuple12*_tmp508=
_cycalloc(sizeof(*_tmp508));_tmp508->f1=({struct Cyc_Parse_Declarator*_tmp509=
_cycalloc(sizeof(*_tmp509));_tmp509->id=({struct _tuple1*_tmp50A=_cycalloc(
sizeof(*_tmp50A));_tmp50A->f1=Cyc_Absyn_rel_ns_null;_tmp50A->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp50A;});_tmp509->tms=
0;_tmp509;});_tmp508->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp508;});_tmp507;});
_tmp506;});break;case 118: _LL250: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_InitDecl_tok_struct
_tmp50D;_tmp50D.tag=Cyc_InitDecl_tok;_tmp50D.f1=({struct _tuple12*_tmp50E=
_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp50E->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp50E;});_tmp50D;});_tmp50C;});break;case 119: _LL251: {struct Cyc_List_List*
_tmp50F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp511;_tmp511.tag=Cyc_TypeSpecifier_tok;
_tmp511.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp512=_cycalloc(
sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Parse_Decl_spec_struct _tmp513;_tmp513.tag=
5;_tmp513.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp50F,({struct Cyc_Core_Opt*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp514;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp513;});
_tmp512;}));_tmp511;});_tmp510;});break;}case 120: _LL252: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp516;_tmp516.tag=Cyc_TypeSpecifier_tok;_tmp516.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp517=_cycalloc(sizeof(*_tmp517));
_tmp517[0]=({struct Cyc_Absyn_TunionType_struct _tmp518;_tmp518.tag=2;_tmp518.f1=({
struct Cyc_Absyn_TunionInfo _tmp519;_tmp519.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp51B;_tmp51B.tag=0;_tmp51B.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp51C;
_tmp51C.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp51C.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp51C;});_tmp51B;});
_tmp51A;}));_tmp519.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp519;});_tmp518;});
_tmp517;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp516;});_tmp515;});break;case 121: _LL253: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp51D=_cycalloc(sizeof(*_tmp51D));
_tmp51D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp51E;_tmp51E.tag=Cyc_TypeSpecifier_tok;
_tmp51E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp520;_tmp520.tag=3;_tmp520.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp521;
_tmp521.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp523;_tmp523.tag=0;_tmp523.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp524;
_tmp524.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp524.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp524.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp524;});_tmp523;});
_tmp522;}));_tmp521.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521;});_tmp520;});
_tmp51F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp51E;});_tmp51D;});break;case 122: _LL254: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp525=_cycalloc_atomic(sizeof(*_tmp525));
_tmp525[0]=({struct Cyc_Bool_tok_struct _tmp526;_tmp526.tag=Cyc_Bool_tok;_tmp526.f1=
0;_tmp526;});_tmp525;});break;case 123: _LL255: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp527=_cycalloc_atomic(sizeof(*_tmp527));_tmp527[0]=({struct Cyc_Bool_tok_struct
_tmp528;_tmp528.tag=Cyc_Bool_tok;_tmp528.f1=1;_tmp528;});_tmp527;});break;case
124: _LL256: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp529=_cycalloc(
sizeof(*_tmp529));_tmp529[0]=({struct Cyc_TunionFieldList_tok_struct _tmp52A;
_tmp52A.tag=Cyc_TunionFieldList_tok;_tmp52A.f1=({struct Cyc_List_List*_tmp52B=
_cycalloc(sizeof(*_tmp52B));_tmp52B->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52B->tl=0;_tmp52B;});
_tmp52A;});_tmp529;});break;case 125: _LL257: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp52D;_tmp52D.tag=Cyc_TunionFieldList_tok;_tmp52D.f1=({struct Cyc_List_List*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52E->tl=0;_tmp52E;});
_tmp52D;});_tmp52C;});break;case 126: _LL258: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp530;_tmp530.tag=Cyc_TunionFieldList_tok;_tmp530.f1=({struct Cyc_List_List*
_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp531->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp531;});_tmp530;});
_tmp52F;});break;case 127: _LL259: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp533;_tmp533.tag=Cyc_TunionFieldList_tok;_tmp533.f1=({struct Cyc_List_List*
_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp534->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp534;});_tmp533;});
_tmp532;});break;case 128: _LL25A: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535[0]=({struct Cyc_Scope_tok_struct
_tmp536;_tmp536.tag=Cyc_Scope_tok;_tmp536.f1=(void*)((void*)2);_tmp536;});
_tmp535;});break;case 129: _LL25B: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({struct Cyc_Scope_tok_struct
_tmp538;_tmp538.tag=Cyc_Scope_tok;_tmp538.f1=(void*)((void*)3);_tmp538;});
_tmp537;});break;case 130: _LL25C: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539[0]=({struct Cyc_Scope_tok_struct
_tmp53A;_tmp53A.tag=Cyc_Scope_tok;_tmp53A.f1=(void*)((void*)0);_tmp53A;});
_tmp539;});break;case 131: _LL25D: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({struct Cyc_TunionField_tok_struct
_tmp53C;_tmp53C.tag=Cyc_TunionField_tok;_tmp53C.f1=({struct Cyc_Absyn_Tunionfield*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53D->typs=0;_tmp53D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp53D->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53D;});_tmp53C;});
_tmp53B;});break;case 132: _LL25E: {struct Cyc_List_List*_tmp53E=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F[0]=({
struct Cyc_TunionField_tok_struct _tmp540;_tmp540.tag=Cyc_TunionField_tok;_tmp540.f1=({
struct Cyc_Absyn_Tunionfield*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp541->typs=_tmp53E;_tmp541->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp541->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp541;});_tmp540;});_tmp53F;});break;}case 133: _LL25F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 134: _LL260: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp542=_cycalloc(sizeof(*_tmp542));
_tmp542[0]=({struct Cyc_Declarator_tok_struct _tmp543;_tmp543.tag=Cyc_Declarator_tok;
_tmp543.f1=({struct Cyc_Parse_Declarator*_tmp544=_cycalloc(sizeof(*_tmp544));
_tmp544->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp544->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp544;});
_tmp543;});_tmp542;});break;case 135: _LL261: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545[0]=({struct Cyc_Declarator_tok_struct
_tmp546;_tmp546.tag=Cyc_Declarator_tok;_tmp546.f1=({struct Cyc_Parse_Declarator*
_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp547->tms=0;_tmp547;});
_tmp546;});_tmp545;});break;case 136: _LL262: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 137: _LL263:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp548=_cycalloc(sizeof(*_tmp548));
_tmp548[0]=({struct Cyc_Declarator_tok_struct _tmp549;_tmp549.tag=Cyc_Declarator_tok;
_tmp549.f1=({struct Cyc_Parse_Declarator*_tmp54A=_cycalloc(sizeof(*_tmp54A));
_tmp54A->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp54A->tms=({struct Cyc_List_List*_tmp54B=_cycalloc(
sizeof(*_tmp54B));_tmp54B->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp54D;_tmp54D.tag=0;_tmp54D.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54D;});_tmp54C;}));
_tmp54B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp54B;});_tmp54A;});_tmp549;});_tmp548;});break;case
138: _LL264: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp54E=_cycalloc(
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
_tmp54F;});_tmp54E;});break;case 139: _LL265: {struct _tuple14 _tmp555;struct Cyc_List_List*
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
3)]))->tms;_tmp55E;});_tmp55D;});_tmp55C;});_tmp55B;});break;}case 140: _LL266:
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
_tmp564;});_tmp563;});break;case 141: _LL267: yyval=(void*)({struct Cyc_Declarator_tok_struct*
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
_tmp56C;});_tmp56B;});break;case 142: _LL268: {struct Cyc_List_List*_tmp573=((
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
_tmp575;});_tmp574;});break;}case 143: _LL269: yyval=(void*)({struct Cyc_Declarator_tok_struct*
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
144: _LL26A: Cyc_Parse_err(({const char*_tmp580="identifier has not been declared as a typedef";
_tag_arr(_tmp580,sizeof(char),_get_zero_arr_size(_tmp580,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({
struct Cyc_Declarator_tok_struct _tmp582;_tmp582.tag=Cyc_Declarator_tok;_tmp582.f1=({
struct Cyc_Parse_Declarator*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp583->tms=0;_tmp583;});
_tmp582;});_tmp581;});break;case 145: _LL26B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 146: _LL26C: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_TypeModifierList_tok_struct _tmp585;_tmp585.tag=Cyc_TypeModifierList_tok;
_tmp585.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp585;});_tmp584;});
break;case 147: _LL26D: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
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
break;}case 148: _LL26E: yyval=(void*)({struct Cyc_YY1_struct*_tmp58F=_cycalloc(
sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_YY1_struct _tmp590;_tmp590.tag=Cyc_YY1;
_tmp590.f1=({struct _tuple16*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->f1=Cyc_Absyn_true_conref;
_tmp591->f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp591;});_tmp590;});_tmp58F;});break;case 149: _LL26F: yyval=(void*)({struct Cyc_YY1_struct*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_YY1_struct _tmp593;
_tmp593.tag=Cyc_YY1;_tmp593.f1=({struct _tuple16*_tmp594=_cycalloc(sizeof(*
_tmp594));_tmp594->f1=Cyc_Absyn_false_conref;_tmp594->f2=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp594;});_tmp593;});
_tmp592;});break;case 150: _LL270: yyval=(void*)({struct Cyc_YY1_struct*_tmp595=
_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_YY1_struct _tmp596;_tmp596.tag=
Cyc_YY1;_tmp596.f1=({struct _tuple16*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->f1=
Cyc_Absyn_true_conref;_tmp597->f2=Cyc_Absyn_bounds_unknown_conref;_tmp597;});
_tmp596;});_tmp595;});break;case 151: _LL271: yyval=(void*)({struct Cyc_YY2_struct*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_YY2_struct _tmp599;
_tmp599.tag=Cyc_YY2;_tmp599.f1=Cyc_Absyn_bounds_one_conref;_tmp599;});_tmp598;});
break;case 152: _LL272: yyval=(void*)({struct Cyc_YY2_struct*_tmp59A=_cycalloc(
sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_YY2_struct _tmp59B;_tmp59B.tag=Cyc_YY2;
_tmp59B.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp59C=
_cycalloc(sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp59D;
_tmp59D.tag=0;_tmp59D.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp59D;});_tmp59C;}));_tmp59B;});_tmp59A;});break;case
153: _LL273: yyval=(void*)({struct Cyc_YY2_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));
_tmp59E[0]=({struct Cyc_YY2_struct _tmp59F;_tmp59F.tag=Cyc_YY2;_tmp59F.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));
_tmp5A0[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp5A1;_tmp5A1.tag=1;_tmp5A1.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp5A1;});_tmp5A0;}));_tmp59F;});_tmp59E;});break;case 154: _LL274: yyval=(
void*)({struct Cyc_YY4_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({
struct Cyc_YY4_struct _tmp5A3;_tmp5A3.tag=Cyc_YY4;_tmp5A3.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5A3;});_tmp5A2;});break;case 155: _LL275: yyval=(void*)({
struct Cyc_YY4_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4[0]=({struct Cyc_YY4_struct
_tmp5A5;_tmp5A5.tag=Cyc_YY4;_tmp5A5.f1=Cyc_Absyn_true_conref;_tmp5A5;});_tmp5A4;});
break;case 156: _LL276: yyval=(void*)({struct Cyc_YY4_struct*_tmp5A6=_cycalloc(
sizeof(*_tmp5A6));_tmp5A6[0]=({struct Cyc_YY4_struct _tmp5A7;_tmp5A7.tag=Cyc_YY4;
_tmp5A7.f1=Cyc_Absyn_false_conref;_tmp5A7;});_tmp5A6;});break;case 157: _LL277:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));
_tmp5A8[0]=({struct Cyc_Type_tok_struct _tmp5A9;_tmp5A9.tag=Cyc_Type_tok;_tmp5A9.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));
_tmp5AA->v=(void*)((void*)3);_tmp5AA;}),0);_tmp5A9;});_tmp5A8;});break;case 158:
_LL278: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 159: _LL279: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({
struct Cyc_Type_tok_struct _tmp5AC;_tmp5AC.tag=Cyc_Type_tok;_tmp5AC.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->v=(void*)((void*)
3);_tmp5AD;}),0);_tmp5AC;});_tmp5AB;});break;case 160: _LL27A: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5AE=_cycalloc_atomic(sizeof(*_tmp5AE));_tmp5AE[
0]=({struct Cyc_TypeQual_tok_struct _tmp5AF;_tmp5AF.tag=Cyc_TypeQual_tok;_tmp5AF.f1=
Cyc_Absyn_empty_tqual();_tmp5AF;});_tmp5AE;});break;case 161: _LL27B: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp5B0=_cycalloc_atomic(sizeof(*_tmp5B0));_tmp5B0[
0]=({struct Cyc_TypeQual_tok_struct _tmp5B1;_tmp5B1.tag=Cyc_TypeQual_tok;_tmp5B1.f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5B1;});_tmp5B0;});
break;case 162: _LL27C: yyval=(void*)({struct Cyc_YY3_struct*_tmp5B2=_cycalloc(
sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_YY3_struct _tmp5B3;_tmp5B3.tag=Cyc_YY3;
_tmp5B3.f1=({struct _tuple14*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5B4->f2=0;
_tmp5B4->f3=0;_tmp5B4->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B4->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B4;});_tmp5B3;});
_tmp5B2;});break;case 163: _LL27D: yyval=(void*)({struct Cyc_YY3_struct*_tmp5B5=
_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_YY3_struct _tmp5B6;_tmp5B6.tag=
Cyc_YY3;_tmp5B6.f1=({struct _tuple14*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp5B7->f2=1;
_tmp5B7->f3=0;_tmp5B7->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B7->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B7;});_tmp5B6;});
_tmp5B5;});break;case 164: _LL27E: {struct _tuple2 _tmp5B9;struct Cyc_Core_Opt*
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
_tmp5BE;});break;}}case 165: _LL27F: {struct _tuple2 _tmp5C3;struct Cyc_Core_Opt*
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
_tmp5C8;});break;}}case 166: _LL280: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Type_tok_struct
_tmp5CD;_tmp5CD.tag=Cyc_Type_tok;_tmp5CD.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5CF;_tmp5CF.tag=1;_tmp5CF.f1=0;_tmp5CF;});_tmp5CE;}));_tmp5CD;});_tmp5CC;});
break;case 167: _LL281: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5D0=_cycalloc(
sizeof(*_tmp5D0));_tmp5D0[0]=({struct Cyc_Type_tok_struct _tmp5D1;_tmp5D1.tag=Cyc_Type_tok;
_tmp5D1.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D3;});_tmp5D2;}));
_tmp5D1;});_tmp5D0;});break;case 168: _LL282: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5D5;_tmp5D5.tag=Cyc_TypeOpt_tok;_tmp5D5.f1=0;_tmp5D5;});_tmp5D4;});break;
case 169: _LL283: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5D6=_cycalloc(
sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_TypeOpt_tok_struct _tmp5D7;_tmp5D7.tag=
Cyc_TypeOpt_tok;_tmp5D7.f1=({struct Cyc_Core_Opt*_tmp5D8=_cycalloc(sizeof(*
_tmp5D8));_tmp5D8->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5D9=
_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5DA;
_tmp5DA.tag=20;_tmp5DA.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DA;});_tmp5D9;}));
_tmp5D8;});_tmp5D7;});_tmp5D6;});break;case 170: _LL284: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5DC;_tmp5DC.tag=Cyc_Rgnorder_tok;_tmp5DC.f1=0;_tmp5DC;});_tmp5DB;});break;
case 171: _LL285: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 172: _LL286: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5DD=
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
_tmp5E0;});_tmp5DF->tl=0;_tmp5DF;});_tmp5DE;});_tmp5DD;});break;case 173: _LL287:
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
_tmp5E5;});break;case 174: _LL288: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp5ED=
_cycalloc_atomic(sizeof(*_tmp5ED));_tmp5ED[0]=({struct Cyc_Bool_tok_struct _tmp5EE;
_tmp5EE.tag=Cyc_Bool_tok;_tmp5EE.f1=0;_tmp5EE;});_tmp5ED;});break;case 175: _LL289:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5EF="inject";
_tag_arr(_tmp5EF,sizeof(char),_get_zero_arr_size(_tmp5EF,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5F0="missing type in function declaration";_tag_arr(_tmp5F0,
sizeof(char),_get_zero_arr_size(_tmp5F0,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp5F1=_cycalloc_atomic(sizeof(*_tmp5F1));_tmp5F1[0]=({
struct Cyc_Bool_tok_struct _tmp5F2;_tmp5F2.tag=Cyc_Bool_tok;_tmp5F2.f1=1;_tmp5F2;});
_tmp5F1;});break;case 176: _LL28A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 177: _LL28B: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3[0]=({struct Cyc_TypeList_tok_struct
_tmp5F4;_tmp5F4.tag=Cyc_TypeList_tok;_tmp5F4.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5F4;});_tmp5F3;});
break;case 178: _LL28C: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5F5=
_cycalloc(sizeof(*_tmp5F5));_tmp5F5[0]=({struct Cyc_TypeList_tok_struct _tmp5F6;
_tmp5F6.tag=Cyc_TypeList_tok;_tmp5F6.f1=0;_tmp5F6;});_tmp5F5;});break;case 179:
_LL28D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 180: _LL28E: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5F7=_cycalloc(
sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_TypeList_tok_struct _tmp5F8;_tmp5F8.tag=
Cyc_TypeList_tok;_tmp5F8.f1=({struct Cyc_List_List*_tmp5F9=_cycalloc(sizeof(*
_tmp5F9));_tmp5F9->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5FA=
_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5FB;
_tmp5FB.tag=21;_tmp5FB.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FB;});_tmp5FA;}));
_tmp5F9->tl=0;_tmp5F9;});_tmp5F8;});_tmp5F7;});break;case 181: _LL28F: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5FC=_cycalloc(sizeof(*
_tmp5FC));_tmp5FC[0]=({struct Cyc_TypeList_tok_struct _tmp5FD;_tmp5FD.tag=Cyc_TypeList_tok;
_tmp5FD.f1=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5FE->tl=0;_tmp5FE;});_tmp5FD;});_tmp5FC;});break;case 182: _LL290: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5FF=_cycalloc(sizeof(*
_tmp5FF));_tmp5FF[0]=({struct Cyc_TypeList_tok_struct _tmp600;_tmp600.tag=Cyc_TypeList_tok;
_tmp600.f1=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp602=_cycalloc(sizeof(*
_tmp602));_tmp602[0]=({struct Cyc_Absyn_AccessEff_struct _tmp603;_tmp603.tag=19;
_tmp603.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp603;});_tmp602;}));_tmp601->tl=0;_tmp601;});_tmp600;});
_tmp5FF;});break;case 183: _LL291: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp604=_cycalloc(sizeof(*_tmp604));
_tmp604[0]=({struct Cyc_TypeList_tok_struct _tmp605;_tmp605.tag=Cyc_TypeList_tok;
_tmp605.f1=({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp607=_cycalloc(sizeof(*
_tmp607));_tmp607[0]=({struct Cyc_Absyn_AccessEff_struct _tmp608;_tmp608.tag=19;
_tmp608.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp608;});_tmp607;}));_tmp606->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp606;});_tmp605;});
_tmp604;});break;case 184: _LL292: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp60A;_tmp60A.tag=Cyc_ParamDeclList_tok;_tmp60A.f1=({struct Cyc_List_List*
_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60B->tl=0;_tmp60B;});
_tmp60A;});_tmp609;});break;case 185: _LL293: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp60D;_tmp60D.tag=Cyc_ParamDeclList_tok;_tmp60D.f1=({struct Cyc_List_List*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60E->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60E;});_tmp60D;});
_tmp60C;});break;case 186: _LL294: {struct _tuple15 _tmp610;struct Cyc_Absyn_Tqual
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
_tmp61B;_tmp625;});_tmp624;});_tmp623;});break;}}}}case 187: _LL295: {struct
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
_tmp62C;_tmp631;});_tmp630;});_tmp62F;});break;}}case 188: _LL296: {struct _tuple15
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
case 189: _LL297: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp645=_cycalloc(
sizeof(*_tmp645));_tmp645[0]=({struct Cyc_IdList_tok_struct _tmp646;_tmp646.tag=
Cyc_IdList_tok;_tmp646.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp646;});_tmp645;});break;case 190: _LL298: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_IdList_tok_struct
_tmp648;_tmp648.tag=Cyc_IdList_tok;_tmp648.f1=({struct Cyc_List_List*_tmp649=
_cycalloc(sizeof(*_tmp649));_tmp649->hd=({struct _tagged_arr*_tmp64A=_cycalloc(
sizeof(*_tmp64A));_tmp64A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64A;});_tmp649->tl=0;
_tmp649;});_tmp648;});_tmp647;});break;case 191: _LL299: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_IdList_tok_struct
_tmp64C;_tmp64C.tag=Cyc_IdList_tok;_tmp64C.f1=({struct Cyc_List_List*_tmp64D=
_cycalloc(sizeof(*_tmp64D));_tmp64D->hd=({struct _tagged_arr*_tmp64E=_cycalloc(
sizeof(*_tmp64E));_tmp64E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64E;});_tmp64D->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp64D;});_tmp64C;});
_tmp64B;});break;case 192: _LL29A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 193: _LL29B: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 194: _LL29C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_Exp_tok_struct _tmp650;
_tmp650.tag=Cyc_Exp_tok;_tmp650.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp652;_tmp652.tag=34;_tmp652.f1=0;_tmp652.f2=0;_tmp652;});_tmp651;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp650;});
_tmp64F;});break;case 195: _LL29D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp653=
_cycalloc(sizeof(*_tmp653));_tmp653[0]=({struct Cyc_Exp_tok_struct _tmp654;_tmp654.tag=
Cyc_Exp_tok;_tmp654.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp656;_tmp656.tag=34;_tmp656.f1=0;_tmp656.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp656;});_tmp655;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp654;});_tmp653;});break;case 196: _LL29E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657[0]=({
struct Cyc_Exp_tok_struct _tmp658;_tmp658.tag=Cyc_Exp_tok;_tmp658.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp659=_cycalloc(sizeof(*_tmp659));
_tmp659[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp65A;_tmp65A.tag=34;
_tmp65A.f1=0;_tmp65A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp65A;});_tmp659;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp658;});
_tmp657;});break;case 197: _LL29F: {struct Cyc_Absyn_Vardecl*_tmp65B=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->f1=(void*)0;_tmp661->f2=({
struct _tagged_arr*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp662;});_tmp661;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp65B->tq=({
struct Cyc_Absyn_Tqual _tmp65C;_tmp65C.q_const=1;_tmp65C.q_volatile=0;_tmp65C.q_restrict=
1;_tmp65C;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp65D=_cycalloc(sizeof(*
_tmp65D));_tmp65D[0]=({struct Cyc_Exp_tok_struct _tmp65E;_tmp65E.tag=Cyc_Exp_tok;
_tmp65E.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp660;_tmp660.tag=27;_tmp660.f1=_tmp65B;_tmp660.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp660.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp660.f4=0;
_tmp660;});_tmp65F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp65E;});
_tmp65D;});break;}case 198: _LL2A0: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_InitializerList_tok_struct
_tmp664;_tmp664.tag=Cyc_InitializerList_tok;_tmp664.f1=({struct Cyc_List_List*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->hd=({struct _tuple18*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666->f1=0;_tmp666->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666;});_tmp665->tl=0;
_tmp665;});_tmp664;});_tmp663;});break;case 199: _LL2A1: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_InitializerList_tok_struct
_tmp668;_tmp668.tag=Cyc_InitializerList_tok;_tmp668.f1=({struct Cyc_List_List*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->hd=({struct _tuple18*_tmp66A=
_cycalloc(sizeof(*_tmp66A));_tmp66A->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66A->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66A;});_tmp669->tl=
0;_tmp669;});_tmp668;});_tmp667;});break;case 200: _LL2A2: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_InitializerList_tok_struct
_tmp66C;_tmp66C.tag=Cyc_InitializerList_tok;_tmp66C.f1=({struct Cyc_List_List*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->hd=({struct _tuple18*_tmp66E=
_cycalloc(sizeof(*_tmp66E));_tmp66E->f1=0;_tmp66E->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66E;});_tmp66D->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66D;});_tmp66C;});
_tmp66B;});break;case 201: _LL2A3: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_InitializerList_tok_struct
_tmp670;_tmp670.tag=Cyc_InitializerList_tok;_tmp670.f1=({struct Cyc_List_List*
_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->hd=({struct _tuple18*_tmp672=
_cycalloc(sizeof(*_tmp672));_tmp672->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp672->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp672;});_tmp671->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp671;});_tmp670;});_tmp66F;});break;case 202: _LL2A4: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp673=_cycalloc(sizeof(*_tmp673));
_tmp673[0]=({struct Cyc_DesignatorList_tok_struct _tmp674;_tmp674.tag=Cyc_DesignatorList_tok;
_tmp674.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp674;});_tmp673;});
break;case 203: _LL2A5: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp675=
_cycalloc(sizeof(*_tmp675));_tmp675[0]=({struct Cyc_DesignatorList_tok_struct
_tmp676;_tmp676.tag=Cyc_DesignatorList_tok;_tmp676.f1=({struct Cyc_List_List*
_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp677->tl=0;_tmp677;});
_tmp676;});_tmp675;});break;case 204: _LL2A6: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_DesignatorList_tok_struct
_tmp679;_tmp679.tag=Cyc_DesignatorList_tok;_tmp679.f1=({struct Cyc_List_List*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67A->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67A;});_tmp679;});
_tmp678;});break;case 205: _LL2A7: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B[0]=({struct Cyc_Designator_tok_struct
_tmp67C;_tmp67C.tag=Cyc_Designator_tok;_tmp67C.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp67E;_tmp67E.tag=0;_tmp67E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67E;});_tmp67D;}));
_tmp67C;});_tmp67B;});break;case 206: _LL2A8: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Designator_tok_struct
_tmp680;_tmp680.tag=Cyc_Designator_tok;_tmp680.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681[0]=({struct Cyc_Absyn_FieldName_struct
_tmp682;_tmp682.tag=1;_tmp682.f1=({struct _tagged_arr*_tmp683=_cycalloc(sizeof(*
_tmp683));_tmp683[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp683;});_tmp682;});_tmp681;}));_tmp680;});_tmp67F;});
break;case 207: _LL2A9: {struct _tuple15 _tmp685;struct Cyc_Absyn_Tqual _tmp686;struct
Cyc_List_List*_tmp687;struct Cyc_List_List*_tmp688;struct _tuple15*_tmp684=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp685=*_tmp684;
_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;_tmp688=_tmp685.f3;{void*_tmp689=Cyc_Parse_speclist2typ(
_tmp687,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp688 != 0)({void*_tmp68A[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp68B="ignoring attributes in type";
_tag_arr(_tmp68B,sizeof(char),_get_zero_arr_size(_tmp68B,28));}),_tag_arr(
_tmp68A,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_ParamDecl_tok_struct
_tmp68D;_tmp68D.tag=Cyc_ParamDecl_tok;_tmp68D.f1=({struct _tuple2*_tmp68E=
_cycalloc(sizeof(*_tmp68E));_tmp68E->f1=0;_tmp68E->f2=_tmp686;_tmp68E->f3=
_tmp689;_tmp68E;});_tmp68D;});_tmp68C;});break;}}case 208: _LL2AA: {struct _tuple15
_tmp690;struct Cyc_Absyn_Tqual _tmp691;struct Cyc_List_List*_tmp692;struct Cyc_List_List*
_tmp693;struct _tuple15*_tmp68F=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp690=*_tmp68F;_tmp691=
_tmp690.f1;_tmp692=_tmp690.f2;_tmp693=_tmp690.f3;{void*_tmp694=Cyc_Parse_speclist2typ(
_tmp692,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp695=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp696=Cyc_Parse_apply_tms(_tmp691,_tmp694,_tmp693,_tmp695);if(_tmp696.f3 != 0)({
void*_tmp697[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp698="bad type params, ignoring";_tag_arr(_tmp698,sizeof(char),
_get_zero_arr_size(_tmp698,26));}),_tag_arr(_tmp697,sizeof(void*),0));});if(
_tmp696.f4 != 0)({void*_tmp699[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp69A="bad specifiers, ignoring";_tag_arr(_tmp69A,sizeof(char),
_get_zero_arr_size(_tmp69A,25));}),_tag_arr(_tmp699,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));
_tmp69B[0]=({struct Cyc_ParamDecl_tok_struct _tmp69C;_tmp69C.tag=Cyc_ParamDecl_tok;
_tmp69C.f1=({struct _tuple2*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->f1=0;
_tmp69D->f2=_tmp696.f1;_tmp69D->f3=_tmp696.f2;_tmp69D;});_tmp69C;});_tmp69B;});
break;}}case 209: _LL2AB: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp69E=
_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_Type_tok_struct _tmp69F;
_tmp69F.tag=Cyc_Type_tok;_tmp69F.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp69F;});_tmp69E;});
break;case 210: _LL2AC: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6A0=_cycalloc(
sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Type_tok_struct _tmp6A1;_tmp6A1.tag=Cyc_Type_tok;
_tmp6A1.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6A2=_cycalloc(
sizeof(*_tmp6A2));_tmp6A2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6A3;_tmp6A3.tag=
20;_tmp6A3.f1=0;_tmp6A3;});_tmp6A2;}));_tmp6A1;});_tmp6A0;});break;case 211:
_LL2AD: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6A4=_cycalloc(sizeof(*
_tmp6A4));_tmp6A4[0]=({struct Cyc_Type_tok_struct _tmp6A5;_tmp6A5.tag=Cyc_Type_tok;
_tmp6A5.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6A6=_cycalloc(
sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6A7;_tmp6A7.tag=
20;_tmp6A7.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6A7;});_tmp6A6;}));_tmp6A5;});_tmp6A4;});break;case 212:
_LL2AE: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6A8=_cycalloc(sizeof(*
_tmp6A8));_tmp6A8[0]=({struct Cyc_Type_tok_struct _tmp6A9;_tmp6A9.tag=Cyc_Type_tok;
_tmp6A9.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6AA=_cycalloc(
sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6AB;_tmp6AB.tag=
21;_tmp6AB.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6AB;});_tmp6AA;}));_tmp6A9;});_tmp6A8;});break;case
213: _LL2AF: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp6AC=_cycalloc(sizeof(*
_tmp6AC));_tmp6AC[0]=({struct Cyc_Type_tok_struct _tmp6AD;_tmp6AD.tag=Cyc_Type_tok;
_tmp6AD.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6AE=_cycalloc(
sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6AF;_tmp6AF.tag=
20;_tmp6AF.f1=({struct Cyc_List_List*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp6B0->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6B0;});_tmp6AF;});_tmp6AE;}));_tmp6AD;});_tmp6AC;});
break;case 214: _LL2B0: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6B1=
_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_TypeList_tok_struct _tmp6B2;
_tmp6B2.tag=Cyc_TypeList_tok;_tmp6B2.f1=({struct Cyc_List_List*_tmp6B3=_cycalloc(
sizeof(*_tmp6B3));_tmp6B3->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3->tl=0;_tmp6B3;});
_tmp6B2;});_tmp6B1;});break;case 215: _LL2B1: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_TypeList_tok_struct
_tmp6B5;_tmp6B5.tag=Cyc_TypeList_tok;_tmp6B5.f1=({struct Cyc_List_List*_tmp6B6=
_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B6->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B6;});_tmp6B5;});
_tmp6B4;});break;case 216: _LL2B2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6B8;_tmp6B8.tag=Cyc_AbstractDeclarator_tok;_tmp6B8.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B9;});_tmp6B8;});
_tmp6B7;});break;case 217: _LL2B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 218: _LL2B4: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6BB;_tmp6BB.tag=Cyc_AbstractDeclarator_tok;_tmp6BB.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6BC;});
_tmp6BB;});_tmp6BA;});break;case 219: _LL2B5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 220: _LL2B6:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6BD=_cycalloc(sizeof(*
_tmp6BD));_tmp6BD[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6BE;_tmp6BE.tag=
Cyc_AbstractDeclarator_tok;_tmp6BE.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->tms=({struct Cyc_List_List*_tmp6C0=
_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C2;_tmp6C2.tag=0;_tmp6C2.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C2;});_tmp6C1;}));
_tmp6C0->tl=0;_tmp6C0;});_tmp6BF;});_tmp6BE;});_tmp6BD;});break;case 221: _LL2B7:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6C3=_cycalloc(sizeof(*
_tmp6C3));_tmp6C3[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6C4;_tmp6C4.tag=
Cyc_AbstractDeclarator_tok;_tmp6C4.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->tms=({struct Cyc_List_List*_tmp6C6=
_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C8;_tmp6C8.tag=0;_tmp6C8.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C8;});_tmp6C7;}));
_tmp6C6->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6C6;});_tmp6C5;});
_tmp6C4;});_tmp6C3;});break;case 222: _LL2B8: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6CA;_tmp6CA.tag=Cyc_AbstractDeclarator_tok;_tmp6CA.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->tms=({struct Cyc_List_List*_tmp6CC=
_cycalloc(sizeof(*_tmp6CC));_tmp6CC->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6CE;_tmp6CE.tag=1;_tmp6CE.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CE.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CE;});_tmp6CD;}));
_tmp6CC->tl=0;_tmp6CC;});_tmp6CB;});_tmp6CA;});_tmp6C9;});break;case 223: _LL2B9:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6CF=_cycalloc(sizeof(*
_tmp6CF));_tmp6CF[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6D0;_tmp6D0.tag=
Cyc_AbstractDeclarator_tok;_tmp6D0.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->tms=({struct Cyc_List_List*_tmp6D2=
_cycalloc(sizeof(*_tmp6D2));_tmp6D2->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6D4;_tmp6D4.tag=1;_tmp6D4.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D4.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D4;});_tmp6D3;}));
_tmp6D2->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6D2;});_tmp6D1;});
_tmp6D0;});_tmp6CF;});break;case 224: _LL2BA: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6D6;_tmp6D6.tag=Cyc_AbstractDeclarator_tok;_tmp6D6.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->tms=({struct Cyc_List_List*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6DA;_tmp6DA.tag=3;_tmp6DA.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6DC;_tmp6DC.tag=1;_tmp6DC.f1=0;_tmp6DC.f2=0;_tmp6DC.f3=0;_tmp6DC.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DC.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DC;});_tmp6DB;}));
_tmp6DA;});_tmp6D9;}));_tmp6D8->tl=0;_tmp6D8;});_tmp6D7;});_tmp6D6;});_tmp6D5;});
break;case 225: _LL2BB: {struct _tuple14 _tmp6DE;struct Cyc_List_List*_tmp6DF;int
_tmp6E0;struct Cyc_Absyn_VarargInfo*_tmp6E1;struct Cyc_Core_Opt*_tmp6E2;struct Cyc_List_List*
_tmp6E3;struct _tuple14*_tmp6DD=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6DE=*_tmp6DD;_tmp6DF=_tmp6DE.f1;_tmp6E0=_tmp6DE.f2;
_tmp6E1=_tmp6DE.f3;_tmp6E2=_tmp6DE.f4;_tmp6E3=_tmp6DE.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6E5;_tmp6E5.tag=Cyc_AbstractDeclarator_tok;
_tmp6E5.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E6=_cycalloc(sizeof(*
_tmp6E6));_tmp6E6->tms=({struct Cyc_List_List*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));
_tmp6E7->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6E8=
_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E9;_tmp6E9.tag=3;_tmp6E9.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6EB;_tmp6EB.tag=1;_tmp6EB.f1=_tmp6DF;_tmp6EB.f2=_tmp6E0;_tmp6EB.f3=_tmp6E1;
_tmp6EB.f4=_tmp6E2;_tmp6EB.f5=_tmp6E3;_tmp6EB;});_tmp6EA;}));_tmp6E9;});_tmp6E8;}));
_tmp6E7->tl=0;_tmp6E7;});_tmp6E6;});_tmp6E5;});_tmp6E4;});break;}case 226: _LL2BC:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6EC=_cycalloc(sizeof(*
_tmp6EC));_tmp6EC[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6ED;_tmp6ED.tag=
Cyc_AbstractDeclarator_tok;_tmp6ED.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE->tms=({struct Cyc_List_List*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F1;_tmp6F1.tag=3;_tmp6F1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F3;_tmp6F3.tag=1;_tmp6F3.f1=0;_tmp6F3.f2=0;_tmp6F3.f3=0;_tmp6F3.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F3.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F3;});_tmp6F2;}));
_tmp6F1;});_tmp6F0;}));_tmp6EF->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6EF;});_tmp6EE;});
_tmp6ED;});_tmp6EC;});break;case 227: _LL2BD: {struct _tuple14 _tmp6F5;struct Cyc_List_List*
_tmp6F6;int _tmp6F7;struct Cyc_Absyn_VarargInfo*_tmp6F8;struct Cyc_Core_Opt*_tmp6F9;
struct Cyc_List_List*_tmp6FA;struct _tuple14*_tmp6F4=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F5=*_tmp6F4;_tmp6F6=
_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;_tmp6F8=_tmp6F5.f3;_tmp6F9=_tmp6F5.f4;_tmp6FA=
_tmp6F5.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6FC;_tmp6FC.tag=Cyc_AbstractDeclarator_tok;_tmp6FC.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->tms=({struct Cyc_List_List*_tmp6FE=
_cycalloc(sizeof(*_tmp6FE));_tmp6FE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp700;_tmp700.tag=3;_tmp700.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp702;_tmp702.tag=1;_tmp702.f1=_tmp6F6;_tmp702.f2=_tmp6F7;_tmp702.f3=_tmp6F8;
_tmp702.f4=_tmp6F9;_tmp702.f5=_tmp6FA;_tmp702;});_tmp701;}));_tmp700;});_tmp6FF;}));
_tmp6FE->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6FE;});_tmp6FD;});
_tmp6FC;});_tmp6FB;});break;}case 228: _LL2BE: {struct Cyc_List_List*_tmp703=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp705;_tmp705.tag=Cyc_AbstractDeclarator_tok;
_tmp705.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp706=_cycalloc(sizeof(*
_tmp706));_tmp706->tms=({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));
_tmp707->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp709;_tmp709.tag=4;_tmp709.f1=_tmp703;_tmp709.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp709.f3=0;
_tmp709;});_tmp708;}));_tmp707->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp707;});_tmp706;});
_tmp705;});_tmp704;});break;}case 229: _LL2BF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp70B;_tmp70B.tag=Cyc_AbstractDeclarator_tok;_tmp70B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->tms=({struct Cyc_List_List*_tmp70D=
_cycalloc(sizeof(*_tmp70D));_tmp70D->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp70F;_tmp70F.tag=5;_tmp70F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp70F.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70F;});_tmp70E;}));
_tmp70D->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp70D;});_tmp70C;});
_tmp70B;});_tmp70A;});break;case 230: _LL2C0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231: _LL2C1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 232: _LL2C2:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 233:
_LL2C3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
234: _LL2C4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 235: _LL2C5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 236: _LL2C6: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp710="`H";
_tag_arr(_tmp710,sizeof(char),_get_zero_arr_size(_tmp710,3));}))== 0)Cyc_Parse_err(({
const char*_tmp711="bad occurrence of heap region `H";_tag_arr(_tmp711,sizeof(
char),_get_zero_arr_size(_tmp711,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C->name=({
struct _tagged_arr*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp71F;});_tmp71C->identity=
0;_tmp71C->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp71D=_cycalloc(
sizeof(*_tmp71D));_tmp71D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp71E;_tmp71E.tag=0;
_tmp71E.f1=(void*)((void*)3);_tmp71E;});_tmp71D;}));_tmp71C;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({
struct Cyc_Absyn_VarType_struct _tmp71B;_tmp71B.tag=1;_tmp71B.f1=tv;_tmp71B;});
_tmp71A;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp712=_cycalloc(sizeof(*
_tmp712));_tmp712[0]=({struct Cyc_Stmt_tok_struct _tmp713;_tmp713.tag=Cyc_Stmt_tok;
_tmp713.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp714=
_cycalloc(sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Absyn_Region_s_struct _tmp715;
_tmp715.tag=18;_tmp715.f1=tv;_tmp715.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->f1=(void*)0;_tmp716->f2=({struct
_tagged_arr*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp717;});_tmp716;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp719;_tmp719.tag=15;_tmp719.f1=(
void*)t;_tmp719;});_tmp718;}),0);_tmp715.f3=0;_tmp715.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp715;});_tmp714;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp713;});
_tmp712;});break;}case 237: _LL2C7: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp720="H";
_tag_arr(_tmp720,sizeof(char),_get_zero_arr_size(_tmp720,2));}))== 0)Cyc_Parse_err(({
const char*_tmp721="bad occurrence of heap region `H";_tag_arr(_tmp721,sizeof(
char),_get_zero_arr_size(_tmp721,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C->name=({
struct _tagged_arr*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp732;_tmp732.tag=0;_tmp732.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp730[1]={&
_tmp732};Cyc_aprintf(({const char*_tmp731="`%s";_tag_arr(_tmp731,sizeof(char),
_get_zero_arr_size(_tmp731,4));}),_tag_arr(_tmp730,sizeof(void*),1));}});_tmp72F;});
_tmp72C->identity=0;_tmp72C->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp72E;_tmp72E.tag=0;_tmp72E.f1=(void*)((void*)3);_tmp72E;});_tmp72D;}));
_tmp72C;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp72A=_cycalloc(
sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_VarType_struct _tmp72B;_tmp72B.tag=
1;_tmp72B.f1=tv;_tmp72B;});_tmp72A;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Stmt_tok_struct
_tmp723;_tmp723.tag=Cyc_Stmt_tok;_tmp723.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[0]=({
struct Cyc_Absyn_Region_s_struct _tmp725;_tmp725.tag=18;_tmp725.f1=tv;_tmp725.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp726=_cycalloc(sizeof(*_tmp726));
_tmp726->f1=(void*)0;_tmp726->f2=({struct _tagged_arr*_tmp727=_cycalloc(sizeof(*
_tmp727));_tmp727[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp727;});_tmp726;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp729;_tmp729.tag=15;_tmp729.f1=(void*)t;_tmp729;});_tmp728;}),0);_tmp725.f3=0;
_tmp725.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp725;});_tmp724;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp723;});
_tmp722;});break;}case 238: _LL2C8: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp733="resetable";
_tag_arr(_tmp733,sizeof(char),_get_zero_arr_size(_tmp733,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp734="expecting [resetable]";_tag_arr(_tmp734,sizeof(char),
_get_zero_arr_size(_tmp734,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp735="`H";_tag_arr(_tmp735,sizeof(char),
_get_zero_arr_size(_tmp735,3));}))== 0)Cyc_Parse_err(({const char*_tmp736="bad occurrence of heap region `H";
_tag_arr(_tmp736,sizeof(char),_get_zero_arr_size(_tmp736,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->name=({
struct _tagged_arr*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp744;});_tmp741->identity=
0;_tmp741->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp742=_cycalloc(
sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp743;_tmp743.tag=0;
_tmp743.f1=(void*)((void*)3);_tmp743;});_tmp742;}));_tmp741;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({
struct Cyc_Absyn_VarType_struct _tmp740;_tmp740.tag=1;_tmp740.f1=tv;_tmp740;});
_tmp73F;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp737=_cycalloc(sizeof(*
_tmp737));_tmp737[0]=({struct Cyc_Stmt_tok_struct _tmp738;_tmp738.tag=Cyc_Stmt_tok;
_tmp738.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp739=
_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Absyn_Region_s_struct _tmp73A;
_tmp73A.tag=18;_tmp73A.f1=tv;_tmp73A.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B->f1=(void*)0;_tmp73B->f2=({struct
_tagged_arr*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73C;});_tmp73B;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp73D=_cycalloc(sizeof(*_tmp73D));
_tmp73D[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp73E;_tmp73E.tag=15;_tmp73E.f1=(
void*)t;_tmp73E;});_tmp73D;}),0);_tmp73A.f3=1;_tmp73A.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp73A;});_tmp739;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp738;});
_tmp737;});break;}case 239: _LL2C9: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp745="resetable";
_tag_arr(_tmp745,sizeof(char),_get_zero_arr_size(_tmp745,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp746="expecting `resetable'";_tag_arr(_tmp746,sizeof(char),
_get_zero_arr_size(_tmp746,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp747="H";_tag_arr(_tmp747,sizeof(char),
_get_zero_arr_size(_tmp747,2));}))== 0)Cyc_Parse_err(({const char*_tmp748="bad occurrence of heap region `H";
_tag_arr(_tmp748,sizeof(char),_get_zero_arr_size(_tmp748,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->name=({
struct _tagged_arr*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp759;_tmp759.tag=0;_tmp759.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp757[1]={&
_tmp759};Cyc_aprintf(({const char*_tmp758="`%s";_tag_arr(_tmp758,sizeof(char),
_get_zero_arr_size(_tmp758,4));}),_tag_arr(_tmp757,sizeof(void*),1));}});_tmp756;});
_tmp753->identity=0;_tmp753->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp755;_tmp755.tag=0;_tmp755.f1=(void*)((void*)3);_tmp755;});_tmp754;}));
_tmp753;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp751=_cycalloc(
sizeof(*_tmp751));_tmp751[0]=({struct Cyc_Absyn_VarType_struct _tmp752;_tmp752.tag=
1;_tmp752.f1=tv;_tmp752;});_tmp751;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Stmt_tok_struct
_tmp74A;_tmp74A.tag=Cyc_Stmt_tok;_tmp74A.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({
struct Cyc_Absyn_Region_s_struct _tmp74C;_tmp74C.tag=18;_tmp74C.f1=tv;_tmp74C.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp74D=_cycalloc(sizeof(*_tmp74D));
_tmp74D->f1=(void*)0;_tmp74D->f2=({struct _tagged_arr*_tmp74E=_cycalloc(sizeof(*
_tmp74E));_tmp74E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp74E;});_tmp74D;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp750;_tmp750.tag=15;_tmp750.f1=(void*)t;_tmp750;});_tmp74F;}),0);_tmp74C.f3=1;
_tmp74C.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp74C;});_tmp74B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74A;});
_tmp749;});break;}case 240: _LL2CA: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A[0]=({struct Cyc_Stmt_tok_struct
_tmp75B;_tmp75B.tag=Cyc_Stmt_tok;_tmp75B.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp75D;_tmp75D.tag=20;_tmp75D.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75D;});_tmp75C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75B;});_tmp75A;});break;case 241: _LL2CB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({
struct Cyc_Stmt_tok_struct _tmp75F;_tmp75F.tag=Cyc_Stmt_tok;_tmp75F.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp761;_tmp761.tag=13;_tmp761.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp761;});_tmp760;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75F;});_tmp75E;});break;case 242: _LL2CC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[0]=({
struct Cyc_Stmt_tok_struct _tmp763;_tmp763.tag=Cyc_Stmt_tok;_tmp763.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp764=_cycalloc(sizeof(*_tmp764));
_tmp764[0]=({struct Cyc_Absyn_Splice_s_struct _tmp765;_tmp765.tag=14;_tmp765.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp765;});_tmp764;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp763;});
_tmp762;});break;case 243: _LL2CD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp766=
_cycalloc(sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Stmt_tok_struct _tmp767;
_tmp767.tag=Cyc_Stmt_tok;_tmp767.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_Label_s_struct
_tmp769;_tmp769.tag=15;_tmp769.f1=({struct _tagged_arr*_tmp76A=_cycalloc(sizeof(*
_tmp76A));_tmp76A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp76A;});_tmp769.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp769;});_tmp768;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp767;});
_tmp766;});break;case 244: _LL2CE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76B=
_cycalloc(sizeof(*_tmp76B));_tmp76B[0]=({struct Cyc_Stmt_tok_struct _tmp76C;
_tmp76C.tag=Cyc_Stmt_tok;_tmp76C.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76C;});
_tmp76B;});break;case 245: _LL2CF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D[0]=({struct Cyc_Stmt_tok_struct _tmp76E;
_tmp76E.tag=Cyc_Stmt_tok;_tmp76E.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76E;});
_tmp76D;});break;case 246: _LL2D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp76F=
_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({struct Cyc_Stmt_tok_struct _tmp770;
_tmp770.tag=Cyc_Stmt_tok;_tmp770.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp770;});
_tmp76F;});break;case 247: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp771=
_cycalloc(sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Stmt_tok_struct _tmp772;
_tmp772.tag=Cyc_Stmt_tok;_tmp772.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp772;});
_tmp771;});break;case 248: _LL2D2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 249: _LL2D3: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Stmt_tok_struct
_tmp774;_tmp774.tag=Cyc_Stmt_tok;_tmp774.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp774;});
_tmp773;});break;case 250: _LL2D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp775=
_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Stmt_tok_struct _tmp776;
_tmp776.tag=Cyc_Stmt_tok;_tmp776.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp776;});_tmp775;});break;case 251: _LL2D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777[0]=({struct Cyc_Stmt_tok_struct
_tmp778;_tmp778.tag=Cyc_Stmt_tok;_tmp778.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp778;});_tmp777;});
break;case 252: _LL2D6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2D7: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=({struct Cyc_Stmt_tok_struct
_tmp77A;_tmp77A.tag=Cyc_Stmt_tok;_tmp77A.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77A;});
_tmp779;});break;case 254: _LL2D8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp77B=
_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({struct Cyc_Stmt_tok_struct _tmp77C;
_tmp77C.tag=Cyc_Stmt_tok;_tmp77C.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp77E;_tmp77E.tag=1;_tmp77E.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77E;});_tmp77D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp77C;});_tmp77B;});break;
case 255: _LL2D9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp77F=_cycalloc(
sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Stmt_tok_struct _tmp780;_tmp780.tag=Cyc_Stmt_tok;
_tmp780.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp782;_tmp782.tag=1;_tmp782.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp782;});_tmp781;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp780;});_tmp77F;});
break;case 256: _LL2DA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp783=_cycalloc(
sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Stmt_tok_struct _tmp784;_tmp784.tag=Cyc_Stmt_tok;
_tmp784.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp784;});
_tmp783;});break;case 257: _LL2DB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp785=
_cycalloc(sizeof(*_tmp785));_tmp785[0]=({struct Cyc_Stmt_tok_struct _tmp786;
_tmp786.tag=Cyc_Stmt_tok;_tmp786.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp786;});
_tmp785;});break;case 258: _LL2DC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp787=
_cycalloc(sizeof(*_tmp787));_tmp787[0]=({struct Cyc_Stmt_tok_struct _tmp788;
_tmp788.tag=Cyc_Stmt_tok;_tmp788.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp788;});
_tmp787;});break;case 259: _LL2DD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp789=
_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Stmt_tok_struct _tmp78A;
_tmp78A.tag=Cyc_Stmt_tok;_tmp78A.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78A;});
_tmp789;});break;case 260: _LL2DE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78B=
_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({struct Cyc_Stmt_tok_struct _tmp78C;
_tmp78C.tag=Cyc_Stmt_tok;_tmp78C.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78C;});
_tmp78B;});break;case 261: _LL2DF: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp78D="C";
_tag_arr(_tmp78D,sizeof(char),_get_zero_arr_size(_tmp78D,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp78E="only switch \"C\" { ... } is allowed";_tag_arr(_tmp78E,
sizeof(char),_get_zero_arr_size(_tmp78E,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F[0]=({
struct Cyc_Stmt_tok_struct _tmp790;_tmp790.tag=Cyc_Stmt_tok;_tmp790.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp791=_cycalloc(sizeof(*_tmp791));
_tmp791[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp792;_tmp792.tag=10;_tmp792.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp792.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp792;});_tmp791;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp790;});
_tmp78F;});break;case 262: _LL2E0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp793=
_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Stmt_tok_struct _tmp794;
_tmp794.tag=Cyc_Stmt_tok;_tmp794.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp794;});
_tmp793;});break;case 263: _LL2E1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Stmt_tok_struct _tmp796;
_tmp796.tag=Cyc_Stmt_tok;_tmp796.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp796;});
_tmp795;});break;case 264: _LL2E2: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp798;_tmp798.tag=Cyc_SwitchClauseList_tok;_tmp798.f1=0;_tmp798;});_tmp797;});
break;case 265: _LL2E3: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp79A;_tmp79A.tag=Cyc_SwitchClauseList_tok;_tmp79A.f1=({struct Cyc_List_List*
_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->hd=({struct Cyc_Absyn_Switch_clause*
_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp79C->pat_vars=0;_tmp79C->where_clause=0;
_tmp79C->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp79C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp79C;});
_tmp79B->tl=0;_tmp79B;});_tmp79A;});_tmp799;});break;case 266: _LL2E4: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));
_tmp79D[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp79E;_tmp79E.tag=Cyc_SwitchClauseList_tok;
_tmp79E.f1=({struct Cyc_List_List*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->hd=({
struct Cyc_Absyn_Switch_clause*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp7A0->pat_vars=0;_tmp7A0->where_clause=0;_tmp7A0->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7A0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A0;});
_tmp79F->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp79F;});_tmp79E;});_tmp79D;});break;case 267: _LL2E5:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A1=_cycalloc(sizeof(*
_tmp7A1));_tmp7A1[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7A2;_tmp7A2.tag=
Cyc_SwitchClauseList_tok;_tmp7A2.f1=({struct Cyc_List_List*_tmp7A3=_cycalloc(
sizeof(*_tmp7A3));_tmp7A3->hd=({struct Cyc_Absyn_Switch_clause*_tmp7A4=_cycalloc(
sizeof(*_tmp7A4));_tmp7A4->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7A4->pat_vars=0;
_tmp7A4->where_clause=0;_tmp7A4->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp7A4;});
_tmp7A3->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A3;});_tmp7A2;});_tmp7A1;});break;case 268: _LL2E6:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A5=_cycalloc(sizeof(*
_tmp7A5));_tmp7A5[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7A6;_tmp7A6.tag=
Cyc_SwitchClauseList_tok;_tmp7A6.f1=({struct Cyc_List_List*_tmp7A7=_cycalloc(
sizeof(*_tmp7A7));_tmp7A7->hd=({struct Cyc_Absyn_Switch_clause*_tmp7A8=_cycalloc(
sizeof(*_tmp7A8));_tmp7A8->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7A8->pat_vars=0;
_tmp7A8->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A8->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7A8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A8;});
_tmp7A7->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7A7;});_tmp7A6;});_tmp7A5;});break;case 269: _LL2E7:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7A9=_cycalloc(sizeof(*
_tmp7A9));_tmp7A9[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7AA;_tmp7AA.tag=
Cyc_SwitchClauseList_tok;_tmp7AA.f1=({struct Cyc_List_List*_tmp7AB=_cycalloc(
sizeof(*_tmp7AB));_tmp7AB->hd=({struct Cyc_Absyn_Switch_clause*_tmp7AC=_cycalloc(
sizeof(*_tmp7AC));_tmp7AC->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp7AC->pat_vars=0;
_tmp7AC->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7AC->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7AC;});
_tmp7AB->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7AB;});_tmp7AA;});_tmp7A9;});break;case 270: _LL2E8:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp7AD=_cycalloc(sizeof(*
_tmp7AD));_tmp7AD[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp7AE;_tmp7AE.tag=
Cyc_SwitchClauseList_tok;_tmp7AE.f1=0;_tmp7AE;});_tmp7AD;});break;case 271: _LL2E9:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7AF=_cycalloc(sizeof(*
_tmp7AF));_tmp7AF[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B0;_tmp7B0.tag=
Cyc_SwitchCClauseList_tok;_tmp7B0.f1=0;_tmp7B0;});_tmp7AF;});break;case 272:
_LL2EA: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7B1=_cycalloc(
sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B2;
_tmp7B2.tag=Cyc_SwitchCClauseList_tok;_tmp7B2.f1=({struct Cyc_List_List*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7B4=
_cycalloc(sizeof(*_tmp7B4));_tmp7B4->cnst_exp=0;_tmp7B4->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp7B4->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B4;});
_tmp7B3->tl=0;_tmp7B3;});_tmp7B2;});_tmp7B1;});break;case 273: _LL2EB: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));
_tmp7B5[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7B6;_tmp7B6.tag=Cyc_SwitchCClauseList_tok;
_tmp7B6.f1=({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp7B8->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp7B8->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B8;});
_tmp7B7->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7B7;});_tmp7B6;});_tmp7B5;});break;case 274: _LL2EC:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7B9=_cycalloc(sizeof(*
_tmp7B9));_tmp7B9[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7BA;_tmp7BA.tag=
Cyc_SwitchCClauseList_tok;_tmp7BA.f1=({struct Cyc_List_List*_tmp7BB=_cycalloc(
sizeof(*_tmp7BB));_tmp7BB->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp7BC=_cycalloc(
sizeof(*_tmp7BC));_tmp7BC->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7BC->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp7BC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7BC;});
_tmp7BB->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7BB;});_tmp7BA;});_tmp7B9;});break;case 275: _LL2ED:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp7BD=_cycalloc(sizeof(*
_tmp7BD));_tmp7BD[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp7BE;_tmp7BE.tag=
Cyc_SwitchCClauseList_tok;_tmp7BE.f1=0;_tmp7BE;});_tmp7BD;});break;case 276:
_LL2EE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BF=_cycalloc(sizeof(*
_tmp7BF));_tmp7BF[0]=({struct Cyc_Stmt_tok_struct _tmp7C0;_tmp7C0.tag=Cyc_Stmt_tok;
_tmp7C0.f1=Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C0;});
_tmp7BF;});break;case 277: _LL2EF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C1=
_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=({struct Cyc_Stmt_tok_struct _tmp7C2;
_tmp7C2.tag=Cyc_Stmt_tok;_tmp7C2.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C2;});
_tmp7C1;});break;case 278: _LL2F0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C3=
_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Stmt_tok_struct _tmp7C4;
_tmp7C4.tag=Cyc_Stmt_tok;_tmp7C4.f1=Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C4;});
_tmp7C3;});break;case 279: _LL2F1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C5=
_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Stmt_tok_struct _tmp7C6;
_tmp7C6.tag=Cyc_Stmt_tok;_tmp7C6.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C6;});
_tmp7C5;});break;case 280: _LL2F2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C7=
_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Stmt_tok_struct _tmp7C8;
_tmp7C8.tag=Cyc_Stmt_tok;_tmp7C8.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C8;});
_tmp7C7;});break;case 281: _LL2F3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C9=
_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({struct Cyc_Stmt_tok_struct _tmp7CA;
_tmp7CA.tag=Cyc_Stmt_tok;_tmp7CA.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CA;});
_tmp7C9;});break;case 282: _LL2F4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CB=
_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Stmt_tok_struct _tmp7CC;
_tmp7CC.tag=Cyc_Stmt_tok;_tmp7CC.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CC;});
_tmp7CB;});break;case 283: _LL2F5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CD=
_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=({struct Cyc_Stmt_tok_struct _tmp7CE;
_tmp7CE.tag=Cyc_Stmt_tok;_tmp7CE.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7CE;});
_tmp7CD;});break;case 284: _LL2F6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7CF=
_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=({struct Cyc_Stmt_tok_struct _tmp7D0;
_tmp7D0.tag=Cyc_Stmt_tok;_tmp7D0.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D0;});
_tmp7CF;});break;case 285: _LL2F7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D1=
_cycalloc(sizeof(*_tmp7D1));_tmp7D1[0]=({struct Cyc_Stmt_tok_struct _tmp7D2;
_tmp7D2.tag=Cyc_Stmt_tok;_tmp7D2.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D2;});
_tmp7D1;});break;case 286: _LL2F8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D3=
_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({struct Cyc_Stmt_tok_struct _tmp7D4;
_tmp7D4.tag=Cyc_Stmt_tok;_tmp7D4.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D4;});
_tmp7D3;});break;case 287: _LL2F9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D5=
_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Stmt_tok_struct _tmp7D6;
_tmp7D6.tag=Cyc_Stmt_tok;_tmp7D6.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D6;});
_tmp7D5;});break;case 288: _LL2FA: {struct Cyc_List_List*_tmp7D7=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp7D8=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D9=
_cycalloc(sizeof(*_tmp7D9));_tmp7D9[0]=({struct Cyc_Stmt_tok_struct _tmp7DA;
_tmp7DA.tag=Cyc_Stmt_tok;_tmp7DA.f1=Cyc_Parse_flatten_declarations(_tmp7D7,
_tmp7D8);_tmp7DA;});_tmp7D9;});break;}case 289: _LL2FB: {struct Cyc_List_List*
_tmp7DB=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7DC=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({
struct Cyc_Stmt_tok_struct _tmp7DE;_tmp7DE.tag=Cyc_Stmt_tok;_tmp7DE.f1=Cyc_Parse_flatten_declarations(
_tmp7DB,_tmp7DC);_tmp7DE;});_tmp7DD;});break;}case 290: _LL2FC: {struct Cyc_List_List*
_tmp7DF=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7E0=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=({
struct Cyc_Stmt_tok_struct _tmp7E2;_tmp7E2.tag=Cyc_Stmt_tok;_tmp7E2.f1=Cyc_Parse_flatten_declarations(
_tmp7DF,_tmp7E0);_tmp7E2;});_tmp7E1;});break;}case 291: _LL2FD: {struct Cyc_List_List*
_tmp7E3=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp7E4=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({
struct Cyc_Stmt_tok_struct _tmp7E6;_tmp7E6.tag=Cyc_Stmt_tok;_tmp7E6.f1=Cyc_Parse_flatten_declarations(
_tmp7E3,_tmp7E4);_tmp7E6;});_tmp7E5;});break;}case 292: _LL2FE: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=({
struct Cyc_Stmt_tok_struct _tmp7E8;_tmp7E8.tag=Cyc_Stmt_tok;_tmp7E8.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E8;});_tmp7E7;});break;case 293: _LL2FF: {struct
Cyc_List_List*_tmp7E9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(
struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));
_tmp7EA[0]=({struct Cyc_Stmt_tok_struct _tmp7EB;_tmp7EB.tag=Cyc_Stmt_tok;_tmp7EB.f1=
Cyc_Absyn_forarray_stmt(_tmp7E9,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EB;});
_tmp7EA;});break;}case 294: _LL300: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Stmt_tok_struct
_tmp7ED;_tmp7ED.tag=Cyc_Stmt_tok;_tmp7ED.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7EE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7ED;});
_tmp7EC;});break;case 295: _LL301: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EF=
_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Stmt_tok_struct _tmp7F0;
_tmp7F0.tag=Cyc_Stmt_tok;_tmp7F0.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7F0;});
_tmp7EF;});break;case 296: _LL302: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F1=
_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({struct Cyc_Stmt_tok_struct _tmp7F2;
_tmp7F2.tag=Cyc_Stmt_tok;_tmp7F2.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7F2;});
_tmp7F1;});break;case 297: _LL303: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F3=
_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=({struct Cyc_Stmt_tok_struct _tmp7F4;
_tmp7F4.tag=Cyc_Stmt_tok;_tmp7F4.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7F4;});
_tmp7F3;});break;case 298: _LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7F5=
_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Stmt_tok_struct _tmp7F6;
_tmp7F6.tag=Cyc_Stmt_tok;_tmp7F6.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7F6;});_tmp7F5;});break;case 299: _LL305: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({
struct Cyc_Stmt_tok_struct _tmp7F8;_tmp7F8.tag=Cyc_Stmt_tok;_tmp7F8.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7F8;});_tmp7F7;});break;case 300: _LL306: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({
struct Cyc_Stmt_tok_struct _tmp7FA;_tmp7FA.tag=Cyc_Stmt_tok;_tmp7FA.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp7FA;});_tmp7F9;});break;case 301: _LL307: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));_tmp7FB[0]=({
struct Cyc_Stmt_tok_struct _tmp7FC;_tmp7FC.tag=Cyc_Stmt_tok;_tmp7FC.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp7FC;});_tmp7FB;});break;case 302: _LL308: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[
0]=({struct Cyc_Pattern_tok_struct _tmp7FE;_tmp7FE.tag=Cyc_Pattern_tok;_tmp7FE.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FE;});
_tmp7FD;});break;case 303: _LL309: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 304: _LL30A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({struct Cyc_Pattern_tok_struct
_tmp800;_tmp800.tag=Cyc_Pattern_tok;_tmp800.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801[0]=({struct
Cyc_Absyn_Int_p_struct _tmp802;_tmp802.tag=7;_tmp802.f1=(void*)(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp802.f2=(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp802;});_tmp801;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp800;});_tmp7FF;});break;case 305: _LL30B: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803[
0]=({struct Cyc_Pattern_tok_struct _tmp804;_tmp804.tag=Cyc_Pattern_tok;_tmp804.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp805=_cycalloc(
sizeof(*_tmp805));_tmp805[0]=({struct Cyc_Absyn_Int_p_struct _tmp806;_tmp806.tag=7;
_tmp806.f1=(void*)((void*)0);_tmp806.f2=-(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp806;});_tmp805;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp804;});_tmp803;});break;case 306: _LL30C: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[
0]=({struct Cyc_Pattern_tok_struct _tmp808;_tmp808.tag=Cyc_Pattern_tok;_tmp808.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp809=_cycalloc(
sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_Float_p_struct _tmp80A;_tmp80A.tag=
9;_tmp80A.f1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp80A;});_tmp809;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp808;});
_tmp807;});break;case 307: _LL30D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Pattern_tok_struct
_tmp80C;_tmp80C.tag=Cyc_Pattern_tok;_tmp80C.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp80D=_cycalloc_atomic(sizeof(*_tmp80D));_tmp80D[0]=({
struct Cyc_Absyn_Char_p_struct _tmp80E;_tmp80E.tag=8;_tmp80E.f1=Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80E;});_tmp80D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80C;});_tmp80B;});break;case 308: _LL30E: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[
0]=({struct Cyc_Pattern_tok_struct _tmp810;_tmp810.tag=Cyc_Pattern_tok;_tmp810.f1=
Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp810;});
_tmp80F;});break;case 309: _LL30F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({struct Cyc_Pattern_tok_struct
_tmp812;_tmp812.tag=Cyc_Pattern_tok;_tmp812.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp814;_tmp814.tag=12;_tmp814.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp814;});_tmp813;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp812;});_tmp811;});break;case 310: _LL310: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[
0]=({struct Cyc_Pattern_tok_struct _tmp816;_tmp816.tag=Cyc_Pattern_tok;_tmp816.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp817=_cycalloc(
sizeof(*_tmp817));_tmp817[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp818;_tmp818.tag=
3;_tmp818.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]));_tmp818;});_tmp817;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp816;});
_tmp815;});break;case 311: _LL311: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Pattern_tok_struct
_tmp81A;_tmp81A.tag=Cyc_Pattern_tok;_tmp81A.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownCall_p_struct*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp81C;_tmp81C.tag=13;_tmp81C.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp81C.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp81C;});_tmp81B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp81A;});_tmp819;});break;case 312: _LL312: {struct
Cyc_List_List*_tmp81D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({
struct Cyc_Pattern_tok_struct _tmp81F;_tmp81F.tag=Cyc_Pattern_tok;_tmp81F.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp820=_cycalloc(sizeof(*_tmp820));
_tmp820[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp821;_tmp821.tag=5;_tmp821.f1=({
struct Cyc_Absyn_AggrInfo _tmp822;_tmp822.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp824;_tmp824.tag=0;_tmp824.f1=(void*)((void*)0);_tmp824.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp824;});_tmp823;}));
_tmp822.targs=0;_tmp822;});_tmp821.f2=_tmp81D;_tmp821.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp821;});_tmp820;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp81F;});_tmp81E;});break;}case 313: _LL313: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825[
0]=({struct Cyc_Pattern_tok_struct _tmp826;_tmp826.tag=Cyc_Pattern_tok;_tmp826.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp827=_cycalloc(
sizeof(*_tmp827));_tmp827[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp828;_tmp828.tag=
4;_tmp828.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp828;});_tmp827;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp826;});
_tmp825;});break;case 314: _LL314: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Pattern_tok_struct
_tmp82A;_tmp82A.tag=Cyc_Pattern_tok;_tmp82A.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp82C;_tmp82C.tag=1;_tmp82C.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->f1=(void*)0;_tmp82D->f2=({
struct _tagged_arr*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp82E;});_tmp82D;}),(
void*)0,0);_tmp82C;});_tmp82B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82A;});
_tmp829;});break;case 315: _LL315: {void*_tmp82F=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp839;_tmp839.tag=0;_tmp839.f1=(void*)((void*)5);_tmp839;});_tmp838;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830[
0]=({struct Cyc_Pattern_tok_struct _tmp831;_tmp831.tag=Cyc_Pattern_tok;_tmp831.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp832=_cycalloc(
sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp833;_tmp833.tag=
2;_tmp833.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp82F);
_tmp833.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp834=_cycalloc(sizeof(*
_tmp834));_tmp834->f1=(void*)0;_tmp834->f2=({struct _tagged_arr*_tmp835=_cycalloc(
sizeof(*_tmp835));_tmp835[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp835;});_tmp834;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp836=_cycalloc(sizeof(*_tmp836));
_tmp836[0]=({struct Cyc_Absyn_TagType_struct _tmp837;_tmp837.tag=17;_tmp837.f1=(
void*)_tmp82F;_tmp837;});_tmp836;}),0);_tmp833;});_tmp832;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp831;});
_tmp830;});break;}case 316: _LL316: {struct Cyc_Absyn_Tvar*_tmp83A=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp846;_tmp846.tag=0;_tmp846.f1=(void*)((void*)
5);_tmp846;});_tmp845;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp83B=
_cycalloc(sizeof(*_tmp83B));_tmp83B[0]=({struct Cyc_Pattern_tok_struct _tmp83C;
_tmp83C.tag=Cyc_Pattern_tok;_tmp83C.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp83E;_tmp83E.tag=2;_tmp83E.f1=_tmp83A;_tmp83E.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->f1=(void*)0;_tmp83F->f2=({
struct _tagged_arr*_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp840;});_tmp83F;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp841=_cycalloc(sizeof(*_tmp841));
_tmp841[0]=({struct Cyc_Absyn_TagType_struct _tmp842;_tmp842.tag=17;_tmp842.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp843=_cycalloc(sizeof(*_tmp843));
_tmp843[0]=({struct Cyc_Absyn_VarType_struct _tmp844;_tmp844.tag=1;_tmp844.f1=
_tmp83A;_tmp844;});_tmp843;}));_tmp842;});_tmp841;}),0);_tmp83E;});_tmp83D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp83C;});_tmp83B;});break;}case 317: _LL317: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp847=_cycalloc(sizeof(*_tmp847));
_tmp847[0]=({struct Cyc_PatternList_tok_struct _tmp848;_tmp848.tag=Cyc_PatternList_tok;
_tmp848.f1=({struct Cyc_List_List*_tmp849=_cycalloc(sizeof(*_tmp849));_tmp849->hd=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp849->tl=0;_tmp849;});_tmp848;});_tmp847;});break;case 318: _LL318: yyval=(void*)({
struct Cyc_PatternList_tok_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({
struct Cyc_PatternList_tok_struct _tmp84B;_tmp84B.tag=Cyc_PatternList_tok;_tmp84B.f1=({
struct Cyc_List_List*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C->hd=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp84C->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp84C;});_tmp84B;});
_tmp84A;});break;case 319: _LL319: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D[0]=({struct Cyc_FieldPattern_tok_struct
_tmp84E;_tmp84E.tag=Cyc_FieldPattern_tok;_tmp84E.f1=({struct _tuple11*_tmp84F=
_cycalloc(sizeof(*_tmp84F));_tmp84F->f1=0;_tmp84F->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp84F;});_tmp84E;});
_tmp84D;});break;case 320: _LL31A: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_FieldPattern_tok_struct
_tmp851;_tmp851.tag=Cyc_FieldPattern_tok;_tmp851.f1=({struct _tuple11*_tmp852=
_cycalloc(sizeof(*_tmp852));_tmp852->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp852->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp852;});_tmp851;});
_tmp850;});break;case 321: _LL31B: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp853=_cycalloc(sizeof(*_tmp853));_tmp853[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp854;_tmp854.tag=Cyc_FieldPatternList_tok;_tmp854.f1=({struct Cyc_List_List*
_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp855->tl=0;_tmp855;});
_tmp854;});_tmp853;});break;case 322: _LL31C: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp857;_tmp857.tag=Cyc_FieldPatternList_tok;_tmp857.f1=({struct Cyc_List_List*
_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp858->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp858;});_tmp857;});
_tmp856;});break;case 323: _LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 324: _LL31E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859[0]=({struct Cyc_Exp_tok_struct _tmp85A;
_tmp85A.tag=Cyc_Exp_tok;_tmp85A.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
_tmp859;});break;case 325: _LL31F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 326: _LL320: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp85B=_cycalloc(sizeof(*_tmp85B));_tmp85B[0]=({struct Cyc_Exp_tok_struct _tmp85C;
_tmp85C.tag=Cyc_Exp_tok;_tmp85C.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85C;});
_tmp85B;});break;case 327: _LL321: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({struct Cyc_Primopopt_tok_struct
_tmp85E;_tmp85E.tag=Cyc_Primopopt_tok;_tmp85E.f1=0;_tmp85E;});_tmp85D;});break;
case 328: _LL322: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp85F=_cycalloc(
sizeof(*_tmp85F));_tmp85F[0]=({struct Cyc_Primopopt_tok_struct _tmp860;_tmp860.tag=
Cyc_Primopopt_tok;_tmp860.f1=({struct Cyc_Core_Opt*_tmp861=_cycalloc(sizeof(*
_tmp861));_tmp861->v=(void*)((void*)1);_tmp861;});_tmp860;});_tmp85F;});break;
case 329: _LL323: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp862=_cycalloc(
sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Primopopt_tok_struct _tmp863;_tmp863.tag=
Cyc_Primopopt_tok;_tmp863.f1=({struct Cyc_Core_Opt*_tmp864=_cycalloc(sizeof(*
_tmp864));_tmp864->v=(void*)((void*)3);_tmp864;});_tmp863;});_tmp862;});break;
case 330: _LL324: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp865=_cycalloc(
sizeof(*_tmp865));_tmp865[0]=({struct Cyc_Primopopt_tok_struct _tmp866;_tmp866.tag=
Cyc_Primopopt_tok;_tmp866.f1=({struct Cyc_Core_Opt*_tmp867=_cycalloc(sizeof(*
_tmp867));_tmp867->v=(void*)((void*)4);_tmp867;});_tmp866;});_tmp865;});break;
case 331: _LL325: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp868=_cycalloc(
sizeof(*_tmp868));_tmp868[0]=({struct Cyc_Primopopt_tok_struct _tmp869;_tmp869.tag=
Cyc_Primopopt_tok;_tmp869.f1=({struct Cyc_Core_Opt*_tmp86A=_cycalloc(sizeof(*
_tmp86A));_tmp86A->v=(void*)((void*)0);_tmp86A;});_tmp869;});_tmp868;});break;
case 332: _LL326: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp86B=_cycalloc(
sizeof(*_tmp86B));_tmp86B[0]=({struct Cyc_Primopopt_tok_struct _tmp86C;_tmp86C.tag=
Cyc_Primopopt_tok;_tmp86C.f1=({struct Cyc_Core_Opt*_tmp86D=_cycalloc(sizeof(*
_tmp86D));_tmp86D->v=(void*)((void*)2);_tmp86D;});_tmp86C;});_tmp86B;});break;
case 333: _LL327: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp86E=_cycalloc(
sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Primopopt_tok_struct _tmp86F;_tmp86F.tag=
Cyc_Primopopt_tok;_tmp86F.f1=({struct Cyc_Core_Opt*_tmp870=_cycalloc(sizeof(*
_tmp870));_tmp870->v=(void*)((void*)16);_tmp870;});_tmp86F;});_tmp86E;});break;
case 334: _LL328: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp871=_cycalloc(
sizeof(*_tmp871));_tmp871[0]=({struct Cyc_Primopopt_tok_struct _tmp872;_tmp872.tag=
Cyc_Primopopt_tok;_tmp872.f1=({struct Cyc_Core_Opt*_tmp873=_cycalloc(sizeof(*
_tmp873));_tmp873->v=(void*)((void*)17);_tmp873;});_tmp872;});_tmp871;});break;
case 335: _LL329: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp874=_cycalloc(
sizeof(*_tmp874));_tmp874[0]=({struct Cyc_Primopopt_tok_struct _tmp875;_tmp875.tag=
Cyc_Primopopt_tok;_tmp875.f1=({struct Cyc_Core_Opt*_tmp876=_cycalloc(sizeof(*
_tmp876));_tmp876->v=(void*)((void*)13);_tmp876;});_tmp875;});_tmp874;});break;
case 336: _LL32A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp877=_cycalloc(
sizeof(*_tmp877));_tmp877[0]=({struct Cyc_Primopopt_tok_struct _tmp878;_tmp878.tag=
Cyc_Primopopt_tok;_tmp878.f1=({struct Cyc_Core_Opt*_tmp879=_cycalloc(sizeof(*
_tmp879));_tmp879->v=(void*)((void*)15);_tmp879;});_tmp878;});_tmp877;});break;
case 337: _LL32B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp87A=_cycalloc(
sizeof(*_tmp87A));_tmp87A[0]=({struct Cyc_Primopopt_tok_struct _tmp87B;_tmp87B.tag=
Cyc_Primopopt_tok;_tmp87B.f1=({struct Cyc_Core_Opt*_tmp87C=_cycalloc(sizeof(*
_tmp87C));_tmp87C->v=(void*)((void*)14);_tmp87C;});_tmp87B;});_tmp87A;});break;
case 338: _LL32C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 339: _LL32D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87D=_cycalloc(
sizeof(*_tmp87D));_tmp87D[0]=({struct Cyc_Exp_tok_struct _tmp87E;_tmp87E.tag=Cyc_Exp_tok;
_tmp87E.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
_tmp87D;});break;case 340: _LL32E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87F=
_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Exp_tok_struct _tmp880;_tmp880.tag=
Cyc_Exp_tok;_tmp880.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp880;});
_tmp87F;});break;case 341: _LL32F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp881=
_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct Cyc_Exp_tok_struct _tmp882;_tmp882.tag=
Cyc_Exp_tok;_tmp882.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp882;});
_tmp881;});break;case 342: _LL330: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp883=
_cycalloc(sizeof(*_tmp883));_tmp883[0]=({struct Cyc_Exp_tok_struct _tmp884;_tmp884.tag=
Cyc_Exp_tok;_tmp884.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp884;});
_tmp883;});break;case 343: _LL331: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp885=
_cycalloc(sizeof(*_tmp885));_tmp885[0]=({struct Cyc_Exp_tok_struct _tmp886;_tmp886.tag=
Cyc_Exp_tok;_tmp886.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
_tmp885;});break;case 344: _LL332: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp887=
_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Exp_tok_struct _tmp888;_tmp888.tag=
Cyc_Exp_tok;_tmp888.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp888;});
_tmp887;});break;case 345: _LL333: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL334: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 347: _LL335: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({struct Cyc_Exp_tok_struct _tmp88A;
_tmp88A.tag=Cyc_Exp_tok;_tmp88A.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88A;});
_tmp889;});break;case 348: _LL336: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL337: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Exp_tok_struct _tmp88C;
_tmp88C.tag=Cyc_Exp_tok;_tmp88C.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
_tmp88B;});break;case 350: _LL338: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL339: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Exp_tok_struct _tmp88E;
_tmp88E.tag=Cyc_Exp_tok;_tmp88E.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88E;});
_tmp88D;});break;case 352: _LL33A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL33B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Exp_tok_struct _tmp890;
_tmp890.tag=Cyc_Exp_tok;_tmp890.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp890;});
_tmp88F;});break;case 354: _LL33C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL33D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Exp_tok_struct _tmp892;
_tmp892.tag=Cyc_Exp_tok;_tmp892.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
_tmp891;});break;case 356: _LL33E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 357: _LL33F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Exp_tok_struct _tmp894;
_tmp894.tag=Cyc_Exp_tok;_tmp894.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp894;});
_tmp893;});break;case 358: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp895=
_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Exp_tok_struct _tmp896;_tmp896.tag=
Cyc_Exp_tok;_tmp896.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp896;});
_tmp895;});break;case 359: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 360: _LL342: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Exp_tok_struct _tmp898;
_tmp898.tag=Cyc_Exp_tok;_tmp898.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
_tmp897;});break;case 361: _LL343: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp899=
_cycalloc(sizeof(*_tmp899));_tmp899[0]=({struct Cyc_Exp_tok_struct _tmp89A;_tmp89A.tag=
Cyc_Exp_tok;_tmp89A.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89A;});
_tmp899;});break;case 362: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89B=
_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Exp_tok_struct _tmp89C;_tmp89C.tag=
Cyc_Exp_tok;_tmp89C.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89C;});
_tmp89B;});break;case 363: _LL345: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89D=
_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=({struct Cyc_Exp_tok_struct _tmp89E;_tmp89E.tag=
Cyc_Exp_tok;_tmp89E.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89E;});
_tmp89D;});break;case 364: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 365: _LL347: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Exp_tok_struct _tmp8A0;
_tmp8A0.tag=Cyc_Exp_tok;_tmp8A0.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A0;});
_tmp89F;});break;case 366: _LL348: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A1=
_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct Cyc_Exp_tok_struct _tmp8A2;_tmp8A2.tag=
Cyc_Exp_tok;_tmp8A2.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A2;});
_tmp8A1;});break;case 367: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 368: _LL34A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Exp_tok_struct _tmp8A4;
_tmp8A4.tag=Cyc_Exp_tok;_tmp8A4.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
_tmp8A3;});break;case 369: _LL34B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A5=
_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Exp_tok_struct _tmp8A6;_tmp8A6.tag=
Cyc_Exp_tok;_tmp8A6.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
_tmp8A5;});break;case 370: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 371: _LL34D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Exp_tok_struct _tmp8A8;
_tmp8A8.tag=Cyc_Exp_tok;_tmp8A8.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A8;});
_tmp8A7;});break;case 372: _LL34E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Exp_tok_struct _tmp8AA;_tmp8AA.tag=
Cyc_Exp_tok;_tmp8AA.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AA;});
_tmp8A9;});break;case 373: _LL34F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8AB=
_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Exp_tok_struct _tmp8AC;_tmp8AC.tag=
Cyc_Exp_tok;_tmp8AC.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AC;});
_tmp8AB;});break;case 374: _LL350: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 375: _LL351: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Exp_tok_struct _tmp8AE;
_tmp8AE.tag=Cyc_Exp_tok;_tmp8AE.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AE;});
_tmp8AD;});break;case 376: _LL352: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 377: _LL353: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Exp_tok_struct _tmp8B0;
_tmp8B0.tag=Cyc_Exp_tok;_tmp8B0.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B0;});
_tmp8AF;});break;case 378: _LL354: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B1=
_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Exp_tok_struct _tmp8B2;_tmp8B2.tag=
Cyc_Exp_tok;_tmp8B2.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
_tmp8B1;});break;case 379: _LL355: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B3=
_cycalloc(sizeof(*_tmp8B3));_tmp8B3[0]=({struct Cyc_Exp_tok_struct _tmp8B4;_tmp8B4.tag=
Cyc_Exp_tok;_tmp8B4.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B4;});
_tmp8B3;});break;case 380: _LL356: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B5=
_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Exp_tok_struct _tmp8B6;_tmp8B6.tag=
Cyc_Exp_tok;_tmp8B6.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B6;});
_tmp8B5;});break;case 381: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 382: _LL358: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7[0]=({struct Cyc_Exp_tok_struct _tmp8B8;
_tmp8B8.tag=Cyc_Exp_tok;_tmp8B8.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B8;});
_tmp8B7;});break;case 383: _LL359: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B9=
_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=({struct Cyc_Exp_tok_struct _tmp8BA;_tmp8BA.tag=
Cyc_Exp_tok;_tmp8BA.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BA;});
_tmp8B9;});break;case 384: _LL35A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BB=
_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=({struct Cyc_Exp_tok_struct _tmp8BC;_tmp8BC.tag=
Cyc_Exp_tok;_tmp8BC.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BC;});
_tmp8BB;});break;case 385: _LL35B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BD=
_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({struct Cyc_Exp_tok_struct _tmp8BE;_tmp8BE.tag=
Cyc_Exp_tok;_tmp8BE.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp8BF=_cycalloc(sizeof(*_tmp8BF));_tmp8BF[0]=({struct Cyc_Absyn_StructField_struct
_tmp8C0;_tmp8C0.tag=0;_tmp8C0.f1=({struct _tagged_arr*_tmp8C1=_cycalloc(sizeof(*
_tmp8C1));_tmp8C1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp8C1;});_tmp8C0;});_tmp8BF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BE;});
_tmp8BD;});break;case 386: _LL35C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C2=
_cycalloc(sizeof(*_tmp8C2));_tmp8C2[0]=({struct Cyc_Exp_tok_struct _tmp8C3;_tmp8C3.tag=
Cyc_Exp_tok;_tmp8C3.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp8C4=_cycalloc_atomic(sizeof(*_tmp8C4));_tmp8C4[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp8C5;_tmp8C5.tag=1;_tmp8C5.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8C5;});_tmp8C4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8C3;});_tmp8C2;});break;case 387: _LL35D: {struct
Cyc_Position_Segment*_tmp8C6=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp8C7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp8C6,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C8=_cycalloc(
sizeof(*_tmp8C8));_tmp8C8[0]=({struct Cyc_Exp_tok_struct _tmp8C9;_tmp8C9.tag=Cyc_Exp_tok;
_tmp8C9.f1=Cyc_Absyn_gentyp_exp(_tmp8C7,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C9;});
_tmp8C8;});break;}case 388: _LL35E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CA=
_cycalloc(sizeof(*_tmp8CA));_tmp8CA[0]=({struct Cyc_Exp_tok_struct _tmp8CB;_tmp8CB.tag=
Cyc_Exp_tok;_tmp8CB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8CC=_cycalloc(sizeof(*_tmp8CC));_tmp8CC[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8CD;_tmp8CD.tag=33;_tmp8CD.f1=({struct Cyc_Absyn_MallocInfo _tmp8CE;_tmp8CE.is_calloc=
0;_tmp8CE.rgn=0;_tmp8CE.elt_type=0;_tmp8CE.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8CE.fat_result=0;
_tmp8CE;});_tmp8CD;});_tmp8CC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CB;});
_tmp8CA;});break;case 389: _LL35F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Exp_tok_struct _tmp8D0;_tmp8D0.tag=
Cyc_Exp_tok;_tmp8D0.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8D2;_tmp8D2.tag=33;_tmp8D2.f1=({struct Cyc_Absyn_MallocInfo _tmp8D3;_tmp8D3.is_calloc=
0;_tmp8D3.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8D3.elt_type=0;
_tmp8D3.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8D3.fat_result=0;_tmp8D3;});_tmp8D2;});_tmp8D1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D0;});
_tmp8CF;});break;case 390: _LL360: {void*_tmp8D5;struct _tuple2 _tmp8D4=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8D5=_tmp8D4.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));
_tmp8D6[0]=({struct Cyc_Exp_tok_struct _tmp8D7;_tmp8D7.tag=Cyc_Exp_tok;_tmp8D7.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8D8=_cycalloc(
sizeof(*_tmp8D8));_tmp8D8[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8D9;_tmp8D9.tag=
33;_tmp8D9.f1=({struct Cyc_Absyn_MallocInfo _tmp8DA;_tmp8DA.is_calloc=1;_tmp8DA.rgn=
0;_tmp8DA.elt_type=({void**_tmp8DB=_cycalloc(sizeof(*_tmp8DB));_tmp8DB[0]=
_tmp8D5;_tmp8DB;});_tmp8DA.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8DA.fat_result=0;
_tmp8DA;});_tmp8D9;});_tmp8D8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D7;});
_tmp8D6;});break;}case 391: _LL361: {void*_tmp8DD;struct _tuple2 _tmp8DC=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8DD=_tmp8DC.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8DE=_cycalloc(sizeof(*_tmp8DE));
_tmp8DE[0]=({struct Cyc_Exp_tok_struct _tmp8DF;_tmp8DF.tag=Cyc_Exp_tok;_tmp8DF.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp8E0=_cycalloc(
sizeof(*_tmp8E0));_tmp8E0[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp8E1;_tmp8E1.tag=
33;_tmp8E1.f1=({struct Cyc_Absyn_MallocInfo _tmp8E2;_tmp8E2.is_calloc=1;_tmp8E2.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp8E2.elt_type=({void**_tmp8E3=_cycalloc(sizeof(*_tmp8E3));
_tmp8E3[0]=_tmp8DD;_tmp8E3;});_tmp8E2.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8E2.fat_result=0;
_tmp8E2;});_tmp8E1;});_tmp8E0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8DF;});
_tmp8DE;});break;}case 392: _LL362: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E4=_cycalloc(sizeof(*_tmp8E4));_tmp8E4[0]=({struct Cyc_Primop_tok_struct
_tmp8E5;_tmp8E5.tag=Cyc_Primop_tok;_tmp8E5.f1=(void*)((void*)12);_tmp8E5;});
_tmp8E4;});break;case 393: _LL363: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E6=_cycalloc(sizeof(*_tmp8E6));_tmp8E6[0]=({struct Cyc_Primop_tok_struct
_tmp8E7;_tmp8E7.tag=Cyc_Primop_tok;_tmp8E7.f1=(void*)((void*)11);_tmp8E7;});
_tmp8E6;});break;case 394: _LL364: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp8E8=_cycalloc(sizeof(*_tmp8E8));_tmp8E8[0]=({struct Cyc_Primop_tok_struct
_tmp8E9;_tmp8E9.tag=Cyc_Primop_tok;_tmp8E9.f1=(void*)((void*)2);_tmp8E9;});
_tmp8E8;});break;case 395: _LL365: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 396: _LL366: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8EA=_cycalloc(sizeof(*_tmp8EA));_tmp8EA[0]=({struct Cyc_Exp_tok_struct _tmp8EB;
_tmp8EB.tag=Cyc_Exp_tok;_tmp8EB.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EB;});
_tmp8EA;});break;case 397: _LL367: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EC=
_cycalloc(sizeof(*_tmp8EC));_tmp8EC[0]=({struct Cyc_Exp_tok_struct _tmp8ED;_tmp8ED.tag=
Cyc_Exp_tok;_tmp8ED.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8ED;});
_tmp8EC;});break;case 398: _LL368: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8EE=
_cycalloc(sizeof(*_tmp8EE));_tmp8EE[0]=({struct Cyc_Exp_tok_struct _tmp8EF;_tmp8EF.tag=
Cyc_Exp_tok;_tmp8EF.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EF;});
_tmp8EE;});break;case 399: _LL369: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F0=
_cycalloc(sizeof(*_tmp8F0));_tmp8F0[0]=({struct Cyc_Exp_tok_struct _tmp8F1;_tmp8F1.tag=
Cyc_Exp_tok;_tmp8F1.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8F2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F1;});
_tmp8F0;});break;case 400: _LL36A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F3=
_cycalloc(sizeof(*_tmp8F3));_tmp8F3[0]=({struct Cyc_Exp_tok_struct _tmp8F4;_tmp8F4.tag=
Cyc_Exp_tok;_tmp8F4.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8F5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F4;});
_tmp8F3;});break;case 401: _LL36B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F6=
_cycalloc(sizeof(*_tmp8F6));_tmp8F6[0]=({struct Cyc_Exp_tok_struct _tmp8F7;_tmp8F7.tag=
Cyc_Exp_tok;_tmp8F7.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F7;});
_tmp8F6;});break;case 402: _LL36C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8F8=
_cycalloc(sizeof(*_tmp8F8));_tmp8F8[0]=({struct Cyc_Exp_tok_struct _tmp8F9;_tmp8F9.tag=
Cyc_Exp_tok;_tmp8F9.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F9;});
_tmp8F8;});break;case 403: _LL36D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8FA=
_cycalloc(sizeof(*_tmp8FA));_tmp8FA[0]=({struct Cyc_Exp_tok_struct _tmp8FB;_tmp8FB.tag=
Cyc_Exp_tok;_tmp8FB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp8FC=_cycalloc(sizeof(*_tmp8FC));_tmp8FC[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp8FD;_tmp8FD.tag=25;_tmp8FD.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8FD.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8FD;});_tmp8FC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8FB;});_tmp8FA;});break;case 404: _LL36E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));_tmp8FE[0]=({
struct Cyc_Exp_tok_struct _tmp8FF;_tmp8FF.tag=Cyc_Exp_tok;_tmp8FF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp900=_cycalloc(sizeof(*_tmp900));
_tmp900[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp901;_tmp901.tag=25;_tmp901.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp901.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp901;});_tmp900;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FF;});
_tmp8FE;});break;case 405: _LL36F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp902=
_cycalloc(sizeof(*_tmp902));_tmp902[0]=({struct Cyc_Exp_tok_struct _tmp903;_tmp903.tag=
Cyc_Exp_tok;_tmp903.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp905;_tmp905.tag=37;_tmp905.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp905;});_tmp904;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp903;});_tmp902;});break;case 406: _LL370: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp906=_cycalloc(sizeof(*_tmp906));_tmp906[0]=({
struct Cyc_Exp_tok_struct _tmp907;_tmp907.tag=Cyc_Exp_tok;_tmp907.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp908=_cycalloc(sizeof(*_tmp908));
_tmp908[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp909;_tmp909.tag=36;_tmp909.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp909;});_tmp908;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp907;});
_tmp906;});break;case 407: _LL371: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp90A=
_cycalloc(sizeof(*_tmp90A));_tmp90A[0]=({struct Cyc_Exp_tok_struct _tmp90B;_tmp90B.tag=
Cyc_Exp_tok;_tmp90B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp90C=_cycalloc(sizeof(*_tmp90C));_tmp90C[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp90D;_tmp90D.tag=2;_tmp90D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90D;});_tmp90C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90B;});
_tmp90A;});break;case 408: _LL372: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 409: _LL373: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[0]=({struct Cyc_Exp_tok_struct _tmp90F;
_tmp90F.tag=Cyc_Exp_tok;_tmp90F.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90F;});
_tmp90E;});break;case 410: _LL374: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 411: _LL375: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp910=_cycalloc(sizeof(*_tmp910));_tmp910[0]=({struct Cyc_Exp_tok_struct _tmp911;
_tmp911.tag=Cyc_Exp_tok;_tmp911.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp911;});
_tmp910;});break;case 412: _LL376: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp912=
_cycalloc(sizeof(*_tmp912));_tmp912[0]=({struct Cyc_Exp_tok_struct _tmp913;_tmp913.tag=
Cyc_Exp_tok;_tmp913.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp913;});
_tmp912;});break;case 413: _LL377: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp914=
_cycalloc(sizeof(*_tmp914));_tmp914[0]=({struct Cyc_Exp_tok_struct _tmp915;_tmp915.tag=
Cyc_Exp_tok;_tmp915.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp915;});
_tmp914;});break;case 414: _LL378: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp916=
_cycalloc(sizeof(*_tmp916));_tmp916[0]=({struct Cyc_Exp_tok_struct _tmp917;_tmp917.tag=
Cyc_Exp_tok;_tmp917.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp917;});
_tmp916;});break;case 415: _LL379: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp918=
_cycalloc(sizeof(*_tmp918));_tmp918[0]=({struct Cyc_Exp_tok_struct _tmp919;_tmp919.tag=
Cyc_Exp_tok;_tmp919.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp91A=_cycalloc(sizeof(*_tmp91A));_tmp91A[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp91B;_tmp91B.tag=28;_tmp91B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp91B.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp91B.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp91B.f4=0;
_tmp91B;});_tmp91A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp919;});
_tmp918;});break;case 416: _LL37A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp91C=
_cycalloc(sizeof(*_tmp91C));_tmp91C[0]=({struct Cyc_Exp_tok_struct _tmp91D;_tmp91D.tag=
Cyc_Exp_tok;_tmp91D.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp91D;});
_tmp91C;});break;case 417: _LL37B: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp91E=_cycalloc(sizeof(*_tmp91E));_tmp91E[0]=({struct Cyc_ExpList_tok_struct
_tmp91F;_tmp91F.tag=Cyc_ExpList_tok;_tmp91F.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp91F;});_tmp91E;});
break;case 418: _LL37C: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp920=
_cycalloc(sizeof(*_tmp920));_tmp920[0]=({struct Cyc_ExpList_tok_struct _tmp921;
_tmp921.tag=Cyc_ExpList_tok;_tmp921.f1=({struct Cyc_List_List*_tmp922=_cycalloc(
sizeof(*_tmp922));_tmp922->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp922->tl=0;_tmp922;});
_tmp921;});_tmp920;});break;case 419: _LL37D: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp923=_cycalloc(sizeof(*_tmp923));_tmp923[0]=({struct Cyc_ExpList_tok_struct
_tmp924;_tmp924.tag=Cyc_ExpList_tok;_tmp924.f1=({struct Cyc_List_List*_tmp925=
_cycalloc(sizeof(*_tmp925));_tmp925->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp925->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp925;});_tmp924;});
_tmp923;});break;case 420: _LL37E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp926=
_cycalloc(sizeof(*_tmp926));_tmp926[0]=({struct Cyc_Exp_tok_struct _tmp927;_tmp927.tag=
Cyc_Exp_tok;_tmp927.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp927;});
_tmp926;});break;case 421: _LL37F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp928=
_cycalloc(sizeof(*_tmp928));_tmp928[0]=({struct Cyc_Exp_tok_struct _tmp929;_tmp929.tag=
Cyc_Exp_tok;_tmp929.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp929;});
_tmp928;});break;case 422: _LL380: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp92A=
_cycalloc(sizeof(*_tmp92A));_tmp92A[0]=({struct Cyc_Exp_tok_struct _tmp92B;_tmp92B.tag=
Cyc_Exp_tok;_tmp92B.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92B;});
_tmp92A;});break;case 423: _LL381: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp92C=
_cycalloc(sizeof(*_tmp92C));_tmp92C[0]=({struct Cyc_Exp_tok_struct _tmp92D;_tmp92D.tag=
Cyc_Exp_tok;_tmp92D.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp92D;});
_tmp92C;});break;case 424: _LL382: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp92E=_cycalloc(sizeof(*_tmp92E));_tmp92E[0]=({struct Cyc_QualId_tok_struct
_tmp92F;_tmp92F.tag=Cyc_QualId_tok;_tmp92F.f1=({struct _tuple1*_tmp930=_cycalloc(
sizeof(*_tmp930));_tmp930->f1=Cyc_Absyn_rel_ns_null;_tmp930->f2=({struct
_tagged_arr*_tmp931=_cycalloc(sizeof(*_tmp931));_tmp931[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp931;});_tmp930;});
_tmp92F;});_tmp92E;});break;case 425: _LL383: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 426: _LL384: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp932=_cycalloc(sizeof(*_tmp932));_tmp932[
0]=({struct Cyc_QualId_tok_struct _tmp933;_tmp933.tag=Cyc_QualId_tok;_tmp933.f1=({
struct _tuple1*_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934->f1=Cyc_Absyn_rel_ns_null;
_tmp934->f2=({struct _tagged_arr*_tmp935=_cycalloc(sizeof(*_tmp935));_tmp935[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp935;});_tmp934;});_tmp933;});_tmp932;});break;case 427: _LL385: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 428: _LL386: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 429: _LL387:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 430:
_LL388: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL389: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 432: _LL38A: break;case 433: _LL38B:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL38C: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(434,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(116,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0?yystate <= 
6158: 0)?Cyc_yycheck[_check_known_subscript_notnull(6159,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)]: 0)yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6159,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(116,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(879,yystate)];if(yyn > - 32768?yyn < 6158: 0){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 247 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6159,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)])
+ 15,count ++);}msg=({unsigned int _tmp936=(unsigned int)(sze + 15);char*_tmp937=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp936 + 1));struct _tagged_arr
_tmp939=_tag_arr(_tmp937,sizeof(char),_tmp936 + 1);{unsigned int _tmp938=_tmp936;
unsigned int i;for(i=0;i < _tmp938;i ++){_tmp937[i]='\000';}_tmp937[_tmp938]=(char)
0;}_tmp939;});Cyc_strcpy(msg,({const char*_tmp93A="parse error";_tag_arr(_tmp93A,
sizeof(char),_get_zero_arr_size(_tmp93A,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 247 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(6159,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmp93B=", expecting `";
_tag_arr(_tmp93B,sizeof(char),_get_zero_arr_size(_tmp93B,14));}):(struct
_tagged_arr)({const char*_tmp93C=" or `";_tag_arr(_tmp93C,sizeof(char),
_get_zero_arr_size(_tmp93C,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(247,x)]);Cyc_strcat(msg,({const char*_tmp93D="'";
_tag_arr(_tmp93D,sizeof(char),_get_zero_arr_size(_tmp93D,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmp93E="parse error";
_tag_arr(_tmp93E,sizeof(char),_get_zero_arr_size(_tmp93E,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(879,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0?1: yyn > 6158)?1: Cyc_yycheck[_check_known_subscript_notnull(6159,
yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[_check_known_subscript_notnull(
6159,yyn)];if(yyn < 0){if(yyn == - 32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{
if(yyn == 0)goto yyerrpop;}if(yyn == 878)return 0;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;}
void Cyc_yyprint(int i,void*v){void*_tmp948=v;struct _tuple13*_tmp949;struct
_tuple13 _tmp94A;int _tmp94B;char _tmp94C;short _tmp94D;struct _tagged_arr _tmp94E;
struct Cyc_Core_Opt*_tmp94F;struct Cyc_Core_Opt*_tmp950;struct Cyc_Core_Opt _tmp951;
struct _tagged_arr*_tmp952;struct _tuple1*_tmp953;struct _tuple1 _tmp954;void*
_tmp955;struct _tagged_arr*_tmp956;_LL38F: if(*((void**)_tmp948)!= Cyc_Int_tok)
goto _LL391;_tmp949=((struct Cyc_Int_tok_struct*)_tmp948)->f1;_tmp94A=*_tmp949;
_tmp94B=_tmp94A.f2;_LL390:({struct Cyc_Int_pa_struct _tmp959;_tmp959.tag=1;_tmp959.f1=(
unsigned int)_tmp94B;{void*_tmp957[1]={& _tmp959};Cyc_fprintf(Cyc_stderr,({const
char*_tmp958="%d";_tag_arr(_tmp958,sizeof(char),_get_zero_arr_size(_tmp958,3));}),
_tag_arr(_tmp957,sizeof(void*),1));}});goto _LL38E;_LL391: if(*((void**)_tmp948)!= 
Cyc_Char_tok)goto _LL393;_tmp94C=((struct Cyc_Char_tok_struct*)_tmp948)->f1;_LL392:({
struct Cyc_Int_pa_struct _tmp95C;_tmp95C.tag=1;_tmp95C.f1=(unsigned int)((int)
_tmp94C);{void*_tmp95A[1]={& _tmp95C};Cyc_fprintf(Cyc_stderr,({const char*_tmp95B="%c";
_tag_arr(_tmp95B,sizeof(char),_get_zero_arr_size(_tmp95B,3));}),_tag_arr(_tmp95A,
sizeof(void*),1));}});goto _LL38E;_LL393: if(*((void**)_tmp948)!= Cyc_Short_tok)
goto _LL395;_tmp94D=((struct Cyc_Short_tok_struct*)_tmp948)->f1;_LL394:({struct Cyc_Int_pa_struct
_tmp95F;_tmp95F.tag=1;_tmp95F.f1=(unsigned int)((int)_tmp94D);{void*_tmp95D[1]={&
_tmp95F};Cyc_fprintf(Cyc_stderr,({const char*_tmp95E="%ds";_tag_arr(_tmp95E,
sizeof(char),_get_zero_arr_size(_tmp95E,4));}),_tag_arr(_tmp95D,sizeof(void*),1));}});
goto _LL38E;_LL395: if(*((void**)_tmp948)!= Cyc_String_tok)goto _LL397;_tmp94E=((
struct Cyc_String_tok_struct*)_tmp948)->f1;_LL396:({struct Cyc_String_pa_struct
_tmp962;_tmp962.tag=0;_tmp962.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp94E);{
void*_tmp960[1]={& _tmp962};Cyc_fprintf(Cyc_stderr,({const char*_tmp961="\"%s\"";
_tag_arr(_tmp961,sizeof(char),_get_zero_arr_size(_tmp961,5));}),_tag_arr(_tmp960,
sizeof(void*),1));}});goto _LL38E;_LL397: if(*((void**)_tmp948)!= Cyc_Stringopt_tok)
goto _LL399;_tmp94F=((struct Cyc_Stringopt_tok_struct*)_tmp948)->f1;if(_tmp94F != 0)
goto _LL399;_LL398:({void*_tmp963[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp964="NULL";_tag_arr(_tmp964,sizeof(char),_get_zero_arr_size(_tmp964,5));}),
_tag_arr(_tmp963,sizeof(void*),0));});goto _LL38E;_LL399: if(*((void**)_tmp948)!= 
Cyc_Stringopt_tok)goto _LL39B;_tmp950=((struct Cyc_Stringopt_tok_struct*)_tmp948)->f1;
if(_tmp950 == 0)goto _LL39B;_tmp951=*_tmp950;_tmp952=(struct _tagged_arr*)_tmp951.v;
_LL39A:({struct Cyc_String_pa_struct _tmp967;_tmp967.tag=0;_tmp967.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp952);{void*_tmp965[1]={& _tmp967};Cyc_fprintf(
Cyc_stderr,({const char*_tmp966="\"%s\"";_tag_arr(_tmp966,sizeof(char),
_get_zero_arr_size(_tmp966,5));}),_tag_arr(_tmp965,sizeof(void*),1));}});goto
_LL38E;_LL39B: if(*((void**)_tmp948)!= Cyc_QualId_tok)goto _LL39D;_tmp953=((struct
Cyc_QualId_tok_struct*)_tmp948)->f1;_tmp954=*_tmp953;_tmp955=_tmp954.f1;_tmp956=
_tmp954.f2;_LL39C: {struct Cyc_List_List*_tmp968=0;{void*_tmp969=_tmp955;struct
Cyc_List_List*_tmp96A;struct Cyc_List_List*_tmp96B;_LL3A0: if(_tmp969 <= (void*)1?1:*((
int*)_tmp969)!= 0)goto _LL3A2;_tmp96A=((struct Cyc_Absyn_Rel_n_struct*)_tmp969)->f1;
_LL3A1: _tmp968=_tmp96A;goto _LL39F;_LL3A2: if(_tmp969 <= (void*)1?1:*((int*)_tmp969)
!= 1)goto _LL3A4;_tmp96B=((struct Cyc_Absyn_Abs_n_struct*)_tmp969)->f1;_LL3A3:
_tmp968=_tmp96B;goto _LL39F;_LL3A4: if((int)_tmp969 != 0)goto _LL39F;_LL3A5: goto
_LL39F;_LL39F:;}for(0;_tmp968 != 0;_tmp968=_tmp968->tl){({struct Cyc_String_pa_struct
_tmp96E;_tmp96E.tag=0;_tmp96E.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp968->hd));{void*_tmp96C[1]={& _tmp96E};Cyc_fprintf(Cyc_stderr,({
const char*_tmp96D="%s::";_tag_arr(_tmp96D,sizeof(char),_get_zero_arr_size(
_tmp96D,5));}),_tag_arr(_tmp96C,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp971;_tmp971.tag=0;_tmp971.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp956);{
void*_tmp96F[1]={& _tmp971};Cyc_fprintf(Cyc_stderr,({const char*_tmp970="%s::";
_tag_arr(_tmp970,sizeof(char),_get_zero_arr_size(_tmp970,5));}),_tag_arr(_tmp96F,
sizeof(void*),1));}});goto _LL38E;}_LL39D:;_LL39E:({void*_tmp972[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp973="?";_tag_arr(_tmp973,sizeof(char),
_get_zero_arr_size(_tmp973,2));}),_tag_arr(_tmp972,sizeof(void*),0));});goto
_LL38E;_LL38E:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmp974=_cycalloc(
sizeof(*_tmp974));_tmp974->v=Cyc_Lexing_from_file(f);_tmp974;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
