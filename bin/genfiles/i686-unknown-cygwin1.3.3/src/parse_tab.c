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
static inline struct _tagged_arr
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
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
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
void*Cyc_Absyn_double_typ(int);extern struct _tuple1*Cyc_Absyn_exn_name;void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr
f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct
_tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
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
static short Cyc_yyr1[417]={0,131,132,132,132,132,132,132,132,133,133,134,134,134,
134,135,135,136,137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,142,
142,142,142,143,143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,147,
148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
148,149,150,150,150,151,151,151,152,152,153,153,154,154,154,155,155,156,156,157,
158,158,159,160,160,161,161,162,163,163,163,163,163,163,164,165,165,166,166,166,
167,167,167,168,168,169,169,169,169,170,170,170,171,171,172,172,173,173,173,173,
173,173,173,173,173,173,173,174,174,175,176,176,176,177,177,177,178,178,178,179,
179,179,180,180,181,181,181,181,182,182,183,183,184,184,185,185,186,186,187,187,
188,188,188,188,189,189,190,190,191,191,191,192,193,193,194,194,195,195,195,195,
196,196,196,196,197,198,198,199,199,200,200,201,201,201,201,201,202,202,203,203,
203,204,204,204,204,204,204,204,204,204,204,204,205,205,205,205,205,205,205,205,
205,205,205,205,205,206,207,207,208,208,209,209,209,209,209,209,210,210,210,210,
210,211,211,211,211,211,211,212,212,212,212,213,213,213,213,213,213,213,213,213,
213,213,213,213,213,213,214,214,214,214,214,214,214,214,215,215,215,215,215,215,
215,215,215,215,215,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,
221,221,221,221,221,221,221,221,222,222,222,222,222,222,222,223,224,224,225,225,
226,226,227,227,228,228,229,229,229,230,230,230,230,230,231,231,231,232,232,232,
233,233,233,233,234,234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,
235,235,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,
238,238,238,238,238,238,239,240,240,241,241,241,241,242,242,243,243,244,244,245,
245,246,246};static short Cyc_yyr2[417]={0,1,2,3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,
2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,2,4,4,4,1,4,1,1,1,1,5,2,4,1,3,1,3,4,8,3,0,3,1,1,1,1,2,1,1,3,1,3,
3,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,4,5,4,5,4,4,2,2,1,
1,2,5,2,2,1,0,3,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,
3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,
3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,5,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,
0,3,4,5,5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,
1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,
3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,
3,4,3,3,2,2,6,7,4,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static
short Cyc_yydefact[854]={0,140,37,38,39,40,42,54,57,58,59,60,61,62,63,64,76,77,78,
91,92,33,0,0,43,0,0,117,118,0,0,0,407,162,409,408,410,73,0,0,55,0,147,147,146,1,
0,9,0,0,10,0,27,35,29,31,65,66,0,67,153,0,128,0,141,150,68,130,89,41,40,34,0,80,
294,407,290,293,292,0,288,0,0,0,0,0,185,0,295,17,19,0,0,0,0,0,0,0,0,0,144,145,2,
0,0,0,0,21,0,96,97,99,28,36,30,32,0,411,412,89,155,0,154,0,25,0,0,11,164,0,0,138,
129,142,152,151,153,139,0,69,0,41,84,0,82,0,0,0,300,291,299,24,0,0,89,0,0,0,106,
102,104,203,205,0,0,0,413,414,163,75,0,131,56,0,0,180,0,0,406,0,0,0,0,0,0,0,0,0,
89,0,0,403,393,404,405,0,0,0,0,378,0,376,377,0,0,309,322,330,332,334,336,338,340,
343,348,351,354,358,0,360,379,392,391,3,0,5,0,22,0,0,0,12,0,93,94,0,88,89,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,407,240,242,248,244,0,246,226,227,228,0,229,230,231,0,
307,26,13,99,170,186,0,0,166,164,0,210,0,0,150,0,44,0,0,0,81,0,0,289,301,0,187,0,
0,0,0,206,178,0,107,103,105,164,0,212,204,213,416,415,0,71,72,74,50,49,0,47,0,
182,0,184,70,0,0,368,0,324,358,0,325,326,0,0,0,0,0,0,0,0,0,361,362,0,0,0,0,364,
365,363,149,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,312,313,314,315,316,317,
318,319,320,321,311,0,366,0,385,386,0,0,0,395,0,89,18,0,20,0,98,100,189,188,14,
86,95,0,0,108,109,111,89,0,115,0,0,0,0,0,0,0,281,282,283,0,0,285,0,0,237,238,0,0,
0,0,0,249,245,99,247,243,241,0,171,0,0,0,177,165,172,134,0,0,0,166,136,0,137,0,
132,150,156,45,90,0,85,329,83,79,296,0,23,0,0,0,0,199,303,305,0,297,0,0,207,0,
166,0,150,0,214,164,0,0,225,209,0,0,0,181,0,0,0,190,194,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,394,401,0,400,331,0,333,335,337,339,341,342,346,347,344,345,349,350,352,353,
355,356,357,310,384,381,0,383,0,0,0,4,6,112,101,0,0,166,89,123,0,0,0,0,0,0,0,0,
280,284,0,0,0,0,233,0,0,0,239,0,15,308,164,0,174,0,0,167,0,135,170,158,211,133,
156,143,7,302,202,0,304,198,200,298,0,208,179,221,0,215,216,150,0,166,0,150,0,0,
0,0,46,48,367,0,0,191,0,195,389,390,0,372,0,0,0,0,0,0,0,0,359,397,0,0,382,380,0,
0,110,113,0,116,125,124,0,0,119,0,0,0,0,0,0,0,0,0,0,255,286,0,0,0,0,0,16,166,0,
175,173,0,166,0,157,201,306,220,218,223,0,224,217,150,0,51,52,0,0,0,192,196,0,0,
0,0,327,328,371,399,0,402,323,396,398,0,0,114,126,123,123,250,0,255,265,0,0,0,0,
0,0,0,0,0,0,0,0,287,0,0,0,0,236,160,176,168,159,164,222,219,0,370,369,0,197,373,
0,0,387,0,87,0,122,121,0,261,0,0,267,0,0,0,275,0,0,0,0,0,0,0,0,254,0,232,235,0,0,
166,0,0,0,0,388,0,251,0,0,0,252,266,268,269,0,277,276,0,271,0,0,0,0,255,256,0,0,
169,161,0,0,0,0,127,0,0,253,270,278,272,273,0,0,255,257,0,0,53,193,374,0,261,262,
274,255,258,279,234,0,261,263,255,259,375,264,260,0,0,0};static short Cyc_yydefgoto[
116]={851,45,46,47,257,48,397,49,399,50,230,51,52,462,53,319,320,54,168,55,56,
142,143,57,139,58,232,233,108,109,110,234,161,408,409,410,59,60,653,654,655,61,
62,63,64,65,100,136,121,594,485,66,486,453,586,445,449,450,302,278,175,85,86,504,
402,505,506,476,477,162,280,281,487,310,260,261,262,263,264,265,733,794,266,267,
293,294,479,480,268,269,385,205,468,206,207,208,209,210,211,212,213,214,215,216,
217,218,219,220,521,522,221,222,68,119,169,314};static short Cyc_yypact[854]={2337,
- -32768,- -32768,- -32768,- -32768,- 28,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
3073,161,891,- -32768,128,- 56,- -32768,- -32768,- 41,- 2,58,- -32768,29,- -32768,-
-32768,- -32768,- -32768,72,221,114,115,127,127,- -32768,- -32768,2216,- -32768,37,
227,- -32768,431,3073,3073,3073,3073,- -32768,- -32768,197,- -32768,54,2944,61,73,
224,238,- -32768,128,145,168,183,- -32768,128,180,- -32768,81,- -32768,- -32768,-
-32768,980,- -32768,214,258,262,980,259,263,261,363,- -32768,- -32768,5320,5320,
5320,201,294,313,201,5498,1839,- -32768,- -32768,- -32768,2216,2457,2216,2457,-
-32768,334,326,- -32768,2875,- -32768,- -32768,- -32768,- -32768,5498,- -32768,- -32768,
145,- -32768,128,- -32768,1460,- -32768,2944,431,- -32768,5263,5320,3601,- -32768,61,
- -32768,- -32768,- -32768,54,- -32768,5320,- -32768,2457,- -32768,341,350,366,128,367,
980,- -32768,- -32768,- -32768,- -32768,388,4817,145,980,375,17,5498,5498,5498,200,-
-32768,- 15,- 15,- 15,- -32768,- -32768,- -32768,- -32768,12,- -32768,- -32768,576,335,-
-32768,5045,378,- -32768,4817,4893,387,392,403,405,438,440,445,145,5121,5121,-
-32768,- -32768,- -32768,- -32768,1962,450,5197,5197,- -32768,5197,- -32768,- -32768,7,
458,- -32768,2,452,441,457,473,429,69,448,406,147,- -32768,724,5197,301,8,- -32768,
471,- -32768,496,- -32768,500,- -32768,221,3677,2944,- -32768,506,5498,- -32768,503,
515,298,501,104,516,3161,518,536,546,548,3753,3161,97,552,3161,3161,94,555,540,-
-32768,- -32768,1588,1588,431,1588,- -32768,- -32768,- -32768,550,- -32768,- -32768,-
-32768,199,- -32768,- -32768,- -32768,559,586,- -32768,- 7,582,575,248,584,585,4,589,
238,587,599,4,610,128,- -32768,4817,614,- -32768,- -32768,424,- -32768,251,831,432,
5320,- -32768,611,617,- -32768,- -32768,- -32768,2757,3829,40,- -32768,154,- -32768,-
-32768,- 7,- -32768,- -32768,- -32768,- -32768,616,618,615,2644,- -32768,664,- -32768,-
-32768,5498,1962,- -32768,5498,- -32768,- -32768,3249,- -32768,647,4817,2577,4817,
4817,4817,4817,4817,620,1962,- -32768,- -32768,1588,621,453,2085,- -32768,- -32768,-
-32768,- -32768,- -32768,5197,4817,5197,5197,5197,5197,5197,5197,5197,5197,5197,
5197,5197,5197,5197,5197,5197,5197,5197,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4817,- -32768,201,- -32768,- -32768,
3905,201,4817,- -32768,625,145,- -32768,2216,- -32768,2216,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,4817,629,627,- -32768,626,145,128,634,4817,633,
4817,4817,712,1716,639,- -32768,- -32768,- -32768,255,703,- -32768,3981,3013,- -32768,
- -32768,3161,649,658,4817,3161,- -32768,- -32768,2875,- -32768,- -32768,- -32768,4817,
- -32768,5498,644,105,- -32768,- -32768,631,- -32768,- 7,646,5393,575,- -32768,5320,-
-32768,4817,- -32768,238,410,- -32768,- -32768,2216,- -32768,- -32768,- -32768,- -32768,
- -32768,980,- -32768,201,4817,980,231,- -32768,- -32768,- -32768,137,- -32768,80,660,
- -32768,650,575,652,238,645,154,5378,5320,4057,- -32768,- -32768,422,654,12,-
-32768,656,659,673,- -32768,- -32768,270,3677,465,661,221,469,665,671,672,472,5498,
662,677,4969,- -32768,- -32768,675,678,452,149,441,457,473,429,69,69,448,448,448,
448,406,406,147,147,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,676,- -32768,
- 48,5320,3513,- -32768,- -32768,- -32768,- -32768,503,4817,575,145,580,479,4817,483,
491,682,4133,4209,274,- -32768,- -32768,686,688,684,690,- -32768,674,679,495,-
-32768,2944,- -32768,- -32768,693,5320,- -32768,694,- 7,- -32768,680,- -32768,120,-
-32768,585,- -32768,410,- -32768,- -32768,- -32768,- -32768,681,- -32768,- -32768,-
-32768,- -32768,831,- -32768,- -32768,- -32768,707,- -32768,- -32768,238,708,575,4,238,
702,710,713,716,- -32768,- -32768,698,234,714,- -32768,3337,- -32768,- -32768,- -32768,
2944,- -32768,4817,796,4817,4893,717,698,719,3513,- -32768,- -32768,4817,4817,-
-32768,- -32768,4,279,- -32768,- -32768,5498,- -32768,- -32768,- -32768,723,128,277,
3161,504,727,3161,4817,4285,286,4361,293,4437,596,- -32768,730,4817,742,19,732,-
-32768,575,146,- -32768,- -32768,739,575,5498,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,731,- -32768,- -32768,238,745,- -32768,- -32768,733,735,4817,- -32768,
- -32768,3677,525,737,738,- -32768,647,- -32768,- -32768,329,- -32768,- -32768,- -32768,
- -32768,3513,743,- -32768,746,49,580,813,748,596,- -32768,527,3161,530,4513,3161,
533,4589,4665,296,980,736,750,- -32768,311,3161,3161,753,- -32768,- -32768,- -32768,
747,- -32768,693,- -32768,- -32768,749,- -32768,- -32768,343,- -32768,- -32768,5498,833,
- -32768,3425,- -32768,5498,- -32768,- -32768,3161,624,754,757,- -32768,3161,3161,562,
- -32768,3161,3161,564,3161,568,4741,- 31,1588,- -32768,4817,- -32768,- -32768,741,- 7,
575,769,4817,755,758,- -32768,573,- -32768,4817,751,761,- -32768,- -32768,- -32768,-
-32768,3161,- -32768,- -32768,3161,- -32768,3161,3161,578,4817,1164,- -32768,590,777,
- -32768,- -32768,764,348,766,5498,- -32768,383,1588,- -32768,- -32768,- -32768,-
-32768,- -32768,3161,393,596,- -32768,3161,3161,- -32768,- -32768,- -32768,767,1332,-
-32768,- -32768,1164,- -32768,- -32768,- -32768,768,624,- -32768,596,- -32768,- -32768,
- -32768,- -32768,883,884,- -32768};static short Cyc_yypgoto[116]={- -32768,90,- -32768,
549,- -32768,- -32768,- -32768,- -32768,- -32768,- 60,- 45,- 21,- -32768,- -32768,- 50,389,
- -32768,332,789,153,- -32768,- -32768,- 125,- -32768,- 58,- -32768,239,- -32768,- -32768,
- -32768,663,666,56,- -32768,- -32768,339,- -32768,- -32768,- 4,- -32768,- -32768,5,- 39,
- 14,- -32768,- -32768,851,- 257,759,303,- 124,- 33,- 117,- 329,117,308,316,- 306,- 363,-
96,- 305,770,- -32768,- 176,- 171,- 510,- 237,- -32768,426,- 192,- 74,- 128,- 23,- 222,196,-
-32768,- -32768,- 38,- 252,- -32768,- 620,- 549,- -32768,- -32768,- 9,752,300,- -32768,314,
205,- -32768,- 157,- 379,- 165,553,556,547,557,558,353,207,342,349,20,692,- -32768,-
-32768,- -32768,- 360,- -32768,- -32768,13,854,- -32768,- 298,- 119};static short Cyc_yytable[
5611]={71,124,174,347,276,437,438,495,440,333,286,277,131,67,87,334,125,163,164,
165,291,499,330,127,132,203,460,122,552,317,545,112,113,114,115,73,88,90,89,646,
126,158,158,158,96,315,316,807,158,158,133,124,67,401,651,652,111,446,311,67,475,
236,443,258,67,270,158,69,571,644,146,117,91,231,1,150,67,311,158,158,137,355,
131,808,584,144,490,271,158,543,126,33,88,546,517,598,297,88,447,393,763,490,259,
122,126,318,312,313,158,158,158,92,737,353,457,33,67,67,67,67,605,94,356,300,301,
312,590,394,707,313,342,272,738,467,237,500,102,501,309,67,581,103,104,363,364,
158,587,308,306,499,324,516,33,67,173,- 120,307,607,144,323,88,455,458,466,38,120,
32,464,88,128,270,93,235,35,129,597,648,130,322,414,95,39,365,158,173,- 186,67,
432,829,604,366,- 186,404,223,224,225,226,258,258,416,258,427,313,33,592,428,97,
433,840,126,434,583,417,444,303,304,305,350,351,847,352,32,1,98,275,482,850,649,
35,- 164,287,609,99,272,- 164,463,259,259,386,259,411,67,448,602,160,160,160,603,
67,158,467,160,160,174,32,741,158,38,138,443,494,35,491,642,439,371,72,492,313,
160,493,158,67,140,372,373,158,158,141,158,448,160,160,687,132,145,258,845,478,
235,32,160,308,158,166,849,34,35,36,158,167,144,116,442,204,134,135,323,306,443,
88,160,160,160,32,509,307,467,42,43,44,35,147,635,695,259,96,166,694,39,626,105,
106,167,67,284,67,549,600,42,43,44,42,43,44,740,473,474,160,67,743,148,275,685,
556,472,149,689,454,568,565,443,173,151,613,443,475,611,153,572,387,388,389,152,
612,744,624,173,665,124,625,716,173,591,443,711,160,717,699,712,725,539,540,541,
578,158,443,728,467,126,775,579,170,443,158,173,443,158,126,390,67,138,67,413,
301,779,391,392,126,448,645,443,171,159,159,159,557,16,17,18,159,159,1,746,403,
755,228,419,227,756,494,158,158,426,325,326,430,431,159,698,301,288,160,443,833,
813,289,786,443,160,159,159,596,703,674,158,599,296,154,704,159,155,679,290,160,
699,292,587,67,160,160,295,160,299,88,709,329,550,88,551,159,159,159,443,688,335,
160,836,158,650,336,173,160,443,1,361,362,839,675,348,173,337,489,338,629,616,
617,618,270,699,367,368,67,751,32,809,710,159,369,370,357,35,158,470,471,107,698,
403,639,39,673,481,471,511,512,513,339,173,340,42,43,44,448,341,520,595,828,126,
349,411,425,787,519,443,354,159,358,67,837,124,531,532,533,534,627,443,395,1,630,
443,698,634,443,359,844,651,652,846,656,443,542,231,658,443,478,520,32,360,160,
158,659,443,396,35,672,443,398,160,126,415,160,39,405,719,443,593,88,407,731,732,
412,42,43,44,418,835,420,784,573,421,158,159,577,520,752,443,764,443,159,766,443,
348,770,443,160,160,792,793,580,507,422,510,423,159,436,514,441,348,159,159,429,
159,790,435,520,1,229,715,160,32,524,799,443,802,443,159,35,804,443,444,- 183,159,
818,326,321,- 183,- 183,826,443,452,- 183,451,307,456,42,43,44,459,615,830,443,160,
38,158,461,235,547,313,158,535,536,403,759,760,529,530,465,258,537,538,469,483,
776,484,496,498,355,497,515,559,518,561,562,553,566,160,173,554,548,558,555,560,
563,567,88,569,593,574,258,576,448,575,582,585,403,588,259,32,33,606,258,608,610,
619,35,621,623,158,- 212,622,628,636,321,- 212,- 212,631,258,159,- 212,258,307,632,
633,637,640,643,159,259,641,159,660,666,667,668,669,670,671,275,682,259,676,678,
160,638,374,375,376,377,378,379,380,381,382,383,173,259,684,686,259,690,691,701,
692,159,159,693,705,696,706,714,403,384,720,160,734,736,739,742,702,747,745,761,
748,403,749,753,708,159,754,757,777,782,718,762,758,721,778,783,788,785,795,796,
811,814,816,820,817,821,328,74,831,331,331,832,657,834,843,848,662,664,159,344,
345,852,853,508,172,620,713,331,331,400,331,647,101,285,681,680,279,406,812,677,
601,683,403,526,160,298,523,0,331,160,118,525,159,0,527,403,528,765,0,0,769,0,32,
76,0,77,78,74,0,35,780,781,0,0,0,0,0,79,0,0,80,81,0,700,473,474,0,82,0,0,0,0,83,
84,0,791,0,0,0,403,797,798,0,0,800,801,0,803,160,0,0,0,722,724,0,727,0,730,0,159,
331,735,75,76,0,77,78,0,0,35,0,0,0,822,0,0,823,79,824,825,80,81,0,0,0,0,0,82,750,
0,159,0,83,84,0,0,74,0,0,0,838,0,0,0,841,842,0,0,0,0,0,0,0,0,0,0,0,768,0,0,772,
774,0,0,0,331,0,331,331,331,331,331,331,331,331,331,331,331,331,331,331,331,331,
331,0,0,0,0,0,0,0,32,76,0,77,78,0,0,35,0,0,0,0,159,0,0,79,806,159,80,81,810,0,0,
0,0,82,331,815,0,0,83,84,0,819,0,0,0,0,0,0,0,0,0,0,0,0,0,0,827,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,159,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,331,2,3,
4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,731,732,21,176,177,238,0,239,240,
241,242,243,244,245,246,22,178,23,179,247,0,180,24,248,331,0,27,28,249,181,182,
250,251,183,184,185,186,29,30,31,252,187,0,253,188,0,0,0,189,190,0,0,0,0,0,0,0,0,
0,0,0,331,0,0,0,0,0,0,0,0,0,254,191,192,193,194,33,34,35,36,37,38,255,123,0,0,
195,0,0,40,196,0,0,0,0,0,197,0,0,0,198,199,200,0,0,0,0,201,202,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,331,0,0,0,0,0,0,0,331,2,3,4,70,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,792,793,21,176,177,238,0,239,240,241,242,243,
244,245,246,22,178,23,179,247,0,180,24,248,0,0,27,28,249,181,182,250,251,183,184,
185,186,29,30,31,252,187,0,253,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,254,191,192,193,194,33,34,35,36,37,38,255,123,0,0,195,0,0,40,196,0,0,0,
0,0,197,0,0,0,198,199,200,0,0,0,0,201,202,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,176,177,238,0,239,240,241,242,243,244,245,246,22,178,23,179,
247,0,180,24,248,0,0,27,28,249,181,182,250,251,183,184,185,186,29,30,31,252,187,
0,253,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,191,192,
193,194,33,34,35,36,37,38,255,123,256,0,195,0,0,40,196,0,0,0,0,0,197,0,0,0,198,
199,200,0,0,0,0,201,202,2,3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
176,177,238,0,239,240,241,242,243,244,245,246,22,178,23,179,247,0,180,24,248,0,0,
27,28,249,181,182,250,251,183,184,185,186,29,30,31,252,187,0,253,188,0,0,0,189,
190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,191,192,193,194,33,34,35,36,37,
38,255,123,0,0,195,0,0,40,196,0,0,0,0,0,197,0,0,0,198,199,200,0,0,0,0,201,202,2,
3,4,70,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,176,177,0,0,0,0,0,0,0,0,0,
0,22,178,23,179,0,0,180,24,0,0,0,27,28,0,181,182,0,0,183,184,185,186,29,30,31,0,
187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,
193,194,33,34,35,36,37,38,564,0,0,0,195,0,0,40,196,0,0,0,0,0,197,0,0,0,198,199,
200,0,0,0,0,201,202,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,176,177,0,0,0,0,
0,0,0,0,0,0,22,178,0,179,0,0,180,0,0,0,0,27,28,0,181,182,0,0,183,184,185,186,29,
30,31,0,187,156,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
191,192,193,194,33,34,35,36,37,38,0,157,0,0,195,0,0,40,196,0,0,0,0,0,197,0,0,0,
198,199,200,0,0,0,0,201,202,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,176,177,
0,0,0,0,0,0,0,0,0,0,22,178,0,179,0,0,180,0,0,0,0,27,28,0,181,182,0,0,183,184,185,
186,29,30,31,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,32,191,192,193,194,33,34,35,36,37,38,0,346,0,0,195,0,0,40,196,0,0,0,0,0,197,0,
0,0,198,199,200,0,0,0,0,201,202,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,176,
177,0,0,0,0,0,0,0,0,0,0,22,178,0,179,0,0,180,0,0,0,0,27,28,0,181,182,0,0,183,184,
185,186,29,30,31,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,191,192,193,194,33,34,35,36,37,38,0,0,0,0,195,0,0,40,196,0,0,0,0,0,197,
0,0,0,198,199,200,0,0,0,0,201,202,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,
0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,0,0,0,0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,0,0,0,0,42,43,44,- 8,1,0,2,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,0,0,0,0,39,0,0,
40,41,0,0,0,0,0,42,43,44,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,
29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,
0,33,34,35,36,37,38,0,0,- 8,0,39,0,0,40,41,0,0,0,0,0,42,43,44,1,0,2,3,4,70,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,
24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,1,0,0,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,32,0,0,0,0,33,34,35,36,37,38,22,0,0,0,39,0,0,
40,41,0,0,27,28,0,42,43,44,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,273,0,0,32,0,0,0,0,33,34,35,36,37,38,275,0,0,0,321,- 164,0,
40,41,0,- 164,0,307,0,42,43,44,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,273,0,0,0,0,0,0,0,33,34,0,36,
37,38,275,0,0,0,306,0,0,40,41,0,0,0,307,0,42,43,44,2,3,4,70,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,0,0,27,
28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,22,229,23,0,0,40,41,
24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,2,3,4,70,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,38,0,123,22,0,23,0,
0,40,41,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,2,3,4,70,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,33,34,0,36,37,38,24,0,0,0,
27,28,0,40,41,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,38,0,0,0,0,0,0,0,40,41,176,177,238,0,239,
240,241,242,243,244,245,246,0,178,0,179,247,0,180,0,248,0,0,0,0,249,181,182,250,
251,183,184,185,186,0,0,0,252,187,0,253,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,254,191,192,193,194,0,0,35,0,0,0,255,123,0,0,195,0,0,0,282,
176,177,0,0,0,197,0,502,0,198,199,200,0,178,0,179,201,202,180,0,0,0,0,0,0,0,181,
182,0,0,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,0,0,0,0,332,503,0,195,0,0,0,282,
176,177,473,474,0,197,0,0,0,198,199,200,0,178,0,179,201,202,180,0,0,0,0,0,0,0,
181,182,0,0,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,0,0,0,0,332,697,0,195,0,0,0,
282,176,177,473,474,0,197,0,0,0,198,199,200,0,178,0,179,201,202,180,0,0,0,0,0,0,
0,181,182,0,0,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,0,0,0,0,332,789,0,195,0,0,0,
282,176,177,473,474,0,197,0,0,0,198,199,200,0,178,0,179,201,202,180,0,0,0,0,0,0,
0,181,182,0,0,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,0,0,0,0,332,0,0,195,0,0,0,
282,176,177,473,474,0,197,0,0,0,198,199,200,0,178,0,179,201,202,180,0,0,0,0,0,0,
0,181,182,0,0,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,0,0,0,
282,0,178,0,179,283,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,
0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
191,192,193,194,0,0,35,176,177,0,0,332,0,0,195,0,0,0,282,0,178,0,179,0,197,180,0,
0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,
190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,
0,424,0,0,0,195,0,0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,182,201,
202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,0,0,0,282,0,178,
0,179,488,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,
187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,
193,194,0,0,35,176,177,0,0,0,0,0,195,544,0,0,282,0,178,0,179,0,197,180,0,0,198,
199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,
0,0,195,570,0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,182,201,202,
183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,0,0,0,282,0,178,0,
179,614,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,
0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,
194,0,0,35,176,177,0,661,0,0,0,195,0,0,0,282,0,178,0,179,0,197,180,0,0,198,199,
200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,663,0,0,
0,195,0,0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,182,201,202,183,
184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,723,0,0,282,0,178,0,179,
0,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,
188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,
0,35,176,177,0,0,0,0,0,195,726,0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,
181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,729,0,0,0,195,0,
0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,
186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,767,0,0,282,0,178,0,179,0,197,
180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,
0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,
176,177,0,0,0,0,0,195,771,0,0,282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,
182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,195,773,0,0,
282,0,178,0,179,0,197,180,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,
0,0,0,187,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,
192,193,194,0,0,35,176,177,0,0,0,0,0,195,805,0,0,282,0,178,0,179,0,197,180,0,0,
198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,187,0,0,188,0,0,0,189,
190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,
0,0,0,0,0,195,0,0,0,282,0,178,0,0,0,197,0,0,0,198,199,200,0,0,181,182,201,202,
183,184,185,186,0,0,0,0,0,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,332,0,0,195,0,0,0,282,0,178,0,0,
0,197,0,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,0,0,0,188,0,
0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,
176,177,0,0,638,0,0,195,0,0,0,282,0,178,0,0,0,197,0,0,0,198,199,200,0,0,181,182,
201,202,183,184,185,186,0,0,0,0,0,0,0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,191,192,193,194,0,0,35,176,177,0,0,0,0,0,327,0,0,0,282,0,
178,0,0,0,197,0,0,0,198,199,200,0,0,181,182,201,202,183,184,185,186,0,0,0,0,0,0,
0,188,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,192,193,194,
0,0,35,176,177,0,0,0,0,0,343,0,0,0,282,0,178,0,0,0,197,0,0,0,198,199,200,0,0,181,
182,201,202,183,184,185,186,0,0,0,0,0,0,0,188,0,0,0,189,190,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,32,191,192,193,194,0,0,35,0,0,22,0,0,0,0,195,0,
0,0,282,0,27,28,0,0,197,0,0,0,198,199,200,29,30,31,0,201,202,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,273,0,0,274,22,0,0,0,33,34,0,36,37,38,
275,27,28,0,0,0,0,40,41,0,0,0,29,30,31,0,0,156,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,33,34,0,36,37,38,0,157,
27,28,0,0,22,40,41,0,0,0,0,29,30,31,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,
0,0,0,0,0,0,0,0,273,0,0,0,0,0,0,0,33,34,0,36,37,38,275,589,0,0,0,0,0,40,41,33,34,
0,36,37,38,0,0,0,0,0,0,0,40,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,
37,38,0,0,0,0,0,0,0,40,41};static short Cyc_yycheck[5611]={21,61,98,195,128,257,
258,313,260,180,138,128,62,0,23,180,61,91,92,93,145,326,179,61,63,99,283,60,407,
17,390,52,53,54,55,22,23,93,25,549,61,91,92,93,39,164,165,78,98,99,64,111,39,229,
5,6,51,64,73,46,297,119,110,123,51,125,116,95,428,117,79,58,113,111,1,84,63,73,
128,129,67,79,132,114,447,72,308,125,138,387,111,98,79,391,346,474,154,84,105,91,
720,323,123,136,125,93,121,122,158,159,160,113,93,106,110,98,103,104,105,106,483,
92,120,106,157,121,455,119,638,122,188,126,113,290,121,327,46,329,161,126,445,
104,105,74,75,195,452,161,108,454,173,343,98,140,98,106,116,486,145,173,147,278,
281,288,103,111,93,286,155,108,230,113,116,100,113,473,555,116,173,237,108,108,
113,233,128,104,173,93,808,109,121,110,230,103,104,105,106,257,258,95,260,104,
122,98,461,108,92,113,828,230,116,106,108,93,158,159,160,197,198,839,200,93,1,
108,104,299,846,556,100,109,140,488,105,228,114,285,257,258,218,260,235,228,275,
106,91,92,93,110,235,299,407,98,99,349,93,109,306,103,113,110,310,100,108,114,
259,118,105,113,122,116,116,321,259,105,127,128,326,327,95,329,313,128,129,612,
323,105,346,836,297,233,93,138,306,343,93,844,99,100,101,349,99,288,105,104,99,
67,68,321,108,110,297,158,159,160,93,336,116,474,118,119,120,100,108,515,622,346,
321,93,94,108,506,104,105,99,321,130,323,395,107,118,119,120,118,119,120,674,115,
116,195,336,679,93,104,610,412,104,94,614,110,104,420,110,306,104,492,110,603,
491,107,429,69,70,71,110,491,680,106,321,104,439,110,104,326,457,110,106,233,110,
625,110,104,371,372,373,439,445,110,104,555,420,104,439,108,110,454,349,110,457,
429,108,397,113,399,115,447,104,115,116,439,452,548,110,109,91,92,93,413,17,18,
19,98,99,1,690,229,106,110,241,104,110,490,491,492,247,109,110,250,251,116,625,
483,110,299,110,106,784,106,114,110,306,128,129,471,634,581,515,475,153,105,634,
138,108,589,107,321,712,109,783,465,326,327,93,329,108,471,642,108,397,475,399,
158,159,160,110,613,108,343,114,548,557,108,445,349,110,1,76,77,114,582,195,454,
108,307,108,509,93,94,95,578,756,72,73,509,699,93,777,645,195,122,123,78,100,582,
109,110,104,712,332,518,108,578,109,110,338,339,340,108,491,108,118,119,120,585,
108,349,465,808,578,108,554,246,753,109,110,106,233,125,554,820,629,363,364,365,
366,109,110,105,1,109,110,756,109,110,126,836,5,6,839,109,110,385,629,109,110,
603,390,93,124,445,649,109,110,106,100,109,110,106,454,629,108,457,108,106,109,
110,462,603,114,22,23,105,118,119,120,108,817,108,744,432,93,680,299,436,428,109,
110,109,110,306,109,110,327,109,110,491,492,22,23,443,335,104,337,104,321,114,
341,106,343,326,327,108,329,758,108,459,1,107,654,515,93,356,109,110,109,110,343,
100,109,110,93,104,349,109,110,108,109,110,109,110,114,114,109,116,109,118,119,
120,108,493,109,110,548,103,753,117,649,392,122,758,367,368,506,716,717,361,362,
106,777,369,370,106,110,731,106,108,110,79,109,108,415,109,417,418,104,420,582,
680,110,113,105,114,108,30,104,731,42,593,98,808,435,783,93,108,122,549,109,777,
93,98,109,820,109,117,109,100,109,93,817,104,110,109,109,108,109,110,110,836,445,
114,839,116,110,110,106,109,109,454,808,110,457,108,105,104,109,104,121,117,104,
117,820,106,121,649,105,80,81,82,83,84,85,86,87,88,89,758,836,109,109,839,117,
110,25,109,491,492,109,109,113,109,106,625,107,105,680,104,93,104,98,633,94,109,
28,109,638,109,108,641,515,110,106,114,98,656,105,108,659,106,110,25,110,106,104,
121,94,109,114,108,106,176,38,93,179,180,109,560,109,109,109,564,565,548,189,190,
0,0,336,97,498,649,197,198,228,200,554,43,136,593,589,128,233,783,585,476,603,
699,358,753,155,355,- 1,218,758,58,357,582,- 1,359,712,360,723,- 1,- 1,726,- 1,93,94,
- 1,96,97,38,- 1,100,736,737,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,631,115,116,- 1,
118,- 1,- 1,- 1,- 1,123,124,- 1,761,- 1,- 1,- 1,756,766,767,- 1,- 1,770,771,- 1,773,817,- 1,
- 1,- 1,660,661,- 1,663,- 1,665,- 1,649,290,669,93,94,- 1,96,97,- 1,- 1,100,- 1,- 1,- 1,799,
- 1,- 1,802,108,804,805,111,112,- 1,- 1,- 1,- 1,- 1,118,696,- 1,680,- 1,123,124,- 1,- 1,38,
- 1,- 1,- 1,826,- 1,- 1,- 1,830,831,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,725,- 1,- 1,728,729,
- 1,- 1,- 1,355,- 1,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,
373,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,753,- 1,- 1,108,775,
758,111,112,779,- 1,- 1,- 1,- 1,118,407,786,- 1,- 1,123,124,- 1,792,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,807,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,817,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
474,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,
31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,518,- 1,48,49,50,51,52,53,54,55,56,
57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
555,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,105,- 1,-
1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,634,- 1,- 1,- 1,- 1,- 1,- 1,- 1,642,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,
45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,
99,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,
122,123,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,
101,102,103,104,105,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,
124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,-
1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,
- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,- 1,
- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,
130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,
61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,108,- 1,- 1,111,
112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,
- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,
- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,
- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,
49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,
102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,122,123,124,- 1,
- 1,- 1,- 1,129,130,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,
103,- 1,- 1,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,-
1,- 1,- 1,- 1,118,119,120,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,
103,- 1,- 1,106,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,
- 1,- 1,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,37,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,48,
49,- 1,118,119,120,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,
101,102,103,104,- 1,- 1,- 1,108,109,- 1,111,112,- 1,114,- 1,116,- 1,118,119,120,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,- 1,
116,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,
107,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,37,- 1,39,- 1,- 1,111,112,44,- 1,- 1,- 1,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,98,99,- 1,101,102,103,
44,- 1,- 1,- 1,48,49,- 1,111,112,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,25,26,27,- 1,29,30,31,32,33,
34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,
- 1,62,63,- 1,65,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,112,
25,26,- 1,- 1,- 1,118,- 1,32,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,-
1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,
- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,
- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,
116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,40,129,130,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,
105,106,- 1,108,- 1,- 1,- 1,112,25,26,115,116,- 1,118,- 1,- 1,- 1,122,123,124,- 1,38,- 1,
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
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,
- 1,- 1,37,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,112,- 1,48,49,- 1,- 1,118,- 1,- 1,- 1,122,123,124,59,
60,61,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,90,- 1,- 1,93,37,- 1,- 1,- 1,98,99,- 1,101,102,103,104,48,49,- 1,- 1,- 1,- 1,111,
112,- 1,- 1,- 1,59,60,61,- 1,- 1,64,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,98,99,- 1,101,102,103,- 1,105,48,49,-
1,- 1,37,111,112,- 1,- 1,- 1,- 1,59,60,61,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,
102,103,104,90,- 1,- 1,- 1,- 1,- 1,111,112,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,
112};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _tagged_arr);
int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;
static int Cyc_yynerrs=0;struct _tuple17{struct _tagged_arr f1;void*f2;};static char
_tmp3D1[8]="stdcall";static char _tmp3D2[6]="cdecl";static char _tmp3D3[9]="fastcall";
static char _tmp3D4[9]="noreturn";static char _tmp3D5[6]="const";static char _tmp3D6[8]="aligned";
static char _tmp3D7[7]="packed";static char _tmp3D8[7]="shared";static char _tmp3D9[7]="unused";
static char _tmp3DA[5]="weak";static char _tmp3DB[10]="dllimport";static char _tmp3DC[
10]="dllexport";static char _tmp3DD[23]="no_instrument_function";static char _tmp3DE[
12]="constructor";static char _tmp3DF[11]="destructor";static char _tmp3E0[22]="no_check_memory_usage";
static char _tmp3E1[5]="pure";struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8EA=10000;short*_tmp8EB=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp8EA));{unsigned int _tmp8EC=
_tmp8EA;unsigned int i;for(i=0;i < _tmp8EC;i ++){_tmp8EB[i]=(short)0;}}_tmp8EB;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp8E7=10000;void**_tmp8E8=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp8E7));{unsigned int _tmp8E9=
_tmp8E7;unsigned int i;for(i=0;i < _tmp8E9;i ++){_tmp8E8[i]=Cyc_yylval;}}_tmp8E8;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp8E4=
10000;struct Cyc_Yyltype*_tmp8E5=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp8E4));{unsigned int _tmp8E6=_tmp8E4;
unsigned int i;for(i=0;i < _tmp8E6;i ++){_tmp8E5[i]=Cyc_yynewloc();}}_tmp8E5;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp33F="parser stack overflow";_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(
_tmp33F,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(854,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar <= 358: 0)?(
int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]: 247;}yyn +=
yychar1;if((yyn < 0?1: yyn > 5610)?1: Cyc_yycheck[_check_known_subscript_notnull(
5611,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5611,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 853)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(854,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(417,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1C2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1C3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp340=
_cycalloc(sizeof(*_tmp340));_tmp340[0]=({struct Cyc_DeclList_tok_struct _tmp341;
_tmp341.tag=Cyc_DeclList_tok;_tmp341.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp341;});_tmp340;});
break;case 3: _LL1C4: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp342=
_cycalloc(sizeof(*_tmp342));_tmp342[0]=({struct Cyc_DeclList_tok_struct _tmp343;
_tmp343.tag=Cyc_DeclList_tok;_tmp343.f1=({struct Cyc_List_List*_tmp344=_cycalloc(
sizeof(*_tmp344));_tmp344->hd=({struct Cyc_Absyn_Decl*_tmp345=_cycalloc(sizeof(*
_tmp345));_tmp345->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp346=
_cycalloc(sizeof(*_tmp346));_tmp346[0]=({struct Cyc_Absyn_Using_d_struct _tmp347;
_tmp347.tag=9;_tmp347.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp347.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp347;});_tmp346;}));
_tmp345->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp345;});_tmp344->tl=0;_tmp344;});_tmp343;});
_tmp342;});Cyc_Lex_leave_using();break;case 4: _LL1C5: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_DeclList_tok_struct
_tmp349;_tmp349.tag=Cyc_DeclList_tok;_tmp349.f1=({struct Cyc_List_List*_tmp34A=
_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=({struct Cyc_Absyn_Decl*_tmp34B=_cycalloc(
sizeof(*_tmp34B));_tmp34B->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Absyn_Using_d_struct
_tmp34D;_tmp34D.tag=9;_tmp34D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp34D.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp34D;});_tmp34C;}));
_tmp34B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp34B;});_tmp34A->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp34A;});_tmp349;});
_tmp348;});break;case 5: _LL1C6: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_DeclList_tok_struct
_tmp34F;_tmp34F.tag=Cyc_DeclList_tok;_tmp34F.f1=({struct Cyc_List_List*_tmp350=
_cycalloc(sizeof(*_tmp350));_tmp350->hd=({struct Cyc_Absyn_Decl*_tmp351=_cycalloc(
sizeof(*_tmp351));_tmp351->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp353;_tmp353.tag=8;_tmp353.f1=({struct _tagged_arr*_tmp354=_cycalloc(sizeof(*
_tmp354));_tmp354[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp354;});_tmp353.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp353;});_tmp352;}));
_tmp351->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp351;});_tmp350->tl=0;_tmp350;});_tmp34F;});
_tmp34E;});Cyc_Lex_leave_namespace();break;case 6: _LL1C7: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_DeclList_tok_struct
_tmp356;_tmp356.tag=Cyc_DeclList_tok;_tmp356.f1=({struct Cyc_List_List*_tmp357=
_cycalloc(sizeof(*_tmp357));_tmp357->hd=({struct Cyc_Absyn_Decl*_tmp358=_cycalloc(
sizeof(*_tmp358));_tmp358->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp35A;_tmp35A.tag=8;_tmp35A.f1=({struct _tagged_arr*_tmp35B=_cycalloc(sizeof(*
_tmp35B));_tmp35B[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp35B;});_tmp35A.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp35A;});_tmp359;}));
_tmp358->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp358;});_tmp357->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp357;});_tmp356;});
_tmp355;});break;case 7: _LL1C8: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp35C="C";
_tag_arr(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp35D="only extern \"C\" { ... } is allowed";_tag_arr(_tmp35D,
sizeof(char),_get_zero_arr_size(_tmp35D,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E[0]=({
struct Cyc_DeclList_tok_struct _tmp35F;_tmp35F.tag=Cyc_DeclList_tok;_tmp35F.f1=({
struct Cyc_List_List*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->hd=({struct Cyc_Absyn_Decl*
_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp363;_tmp363.tag=10;_tmp363.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp363;});_tmp362;}));
_tmp361->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp361;});_tmp360->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp360;});_tmp35F;});
_tmp35E;});break;case 8: _LL1C9: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({struct Cyc_DeclList_tok_struct
_tmp365;_tmp365.tag=Cyc_DeclList_tok;_tmp365.f1=0;_tmp365;});_tmp364;});break;
case 9: _LL1CA: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp366=_cycalloc(
sizeof(*_tmp366));_tmp366[0]=({struct Cyc_DeclList_tok_struct _tmp367;_tmp367.tag=
Cyc_DeclList_tok;_tmp367.f1=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*
_tmp368));_tmp368->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp36A;_tmp36A.tag=1;_tmp36A.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp36A;});_tmp369;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp368->tl=0;
_tmp368;});_tmp367;});_tmp366;});break;case 10: _LL1CB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 11: _LL1CC: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B[
0]=({struct Cyc_FnDecl_tok_struct _tmp36C;_tmp36C.tag=Cyc_FnDecl_tok;_tmp36C.f1=
Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp36C;});
_tmp36B;});break;case 12: _LL1CD: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_FnDecl_tok_struct
_tmp36E;_tmp36E.tag=Cyc_FnDecl_tok;_tmp36E.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp36F;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp36E;});
_tmp36D;});break;case 13: _LL1CE: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370[0]=({struct Cyc_FnDecl_tok_struct
_tmp371;_tmp371.tag=Cyc_FnDecl_tok;_tmp371.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp371;});
_tmp370;});break;case 14: _LL1CF: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=({struct Cyc_FnDecl_tok_struct
_tmp373;_tmp373.tag=Cyc_FnDecl_tok;_tmp373.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp374;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp373;});
_tmp372;});break;case 15: _LL1D0: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375[0]=({struct Cyc_FnDecl_tok_struct
_tmp376;_tmp376.tag=Cyc_FnDecl_tok;_tmp376.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp377;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp376;});
_tmp375;});break;case 16: _LL1D1: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_FnDecl_tok_struct
_tmp379;_tmp379.tag=Cyc_FnDecl_tok;_tmp379.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp37A;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp379;});
_tmp378;});break;case 17: _LL1D2: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL1D3: Cyc_Lex_leave_using();
break;case 19: _LL1D4: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp37B=
_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37B;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 20: _LL1D5: Cyc_Lex_leave_namespace();
break;case 21: _LL1D6: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp37C=
_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=({struct Cyc_DeclList_tok_struct _tmp37D;
_tmp37D.tag=Cyc_DeclList_tok;_tmp37D.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp37D;});
_tmp37C;});break;case 22: _LL1D7: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_DeclList_tok_struct
_tmp37F;_tmp37F.tag=Cyc_DeclList_tok;_tmp37F.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp37F;});
_tmp37E;});break;case 23: _LL1D8: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380[0]=({struct Cyc_DeclList_tok_struct
_tmp381;_tmp381.tag=Cyc_DeclList_tok;_tmp381.f1=({struct Cyc_List_List*_tmp382=
_cycalloc(sizeof(*_tmp382));_tmp382->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp382->tl=0;
_tmp382;});_tmp381;});_tmp380;});break;case 24: _LL1D9: {struct Cyc_List_List*
_tmp383=0;{struct Cyc_List_List*_tmp384=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp384 != 0;
_tmp384=_tmp384->tl){struct _tagged_arr*_tmp385=(struct _tagged_arr*)_tmp384->hd;
struct _tuple1*_tmp386=({struct _tuple1*_tmp389=_cycalloc(sizeof(*_tmp389));
_tmp389->f1=Cyc_Absyn_rel_ns_null;_tmp389->f2=_tmp385;_tmp389;});struct Cyc_Absyn_Vardecl*
_tmp387=Cyc_Absyn_new_vardecl(_tmp386,Cyc_Absyn_wildtyp(0),0);_tmp383=({struct
Cyc_List_List*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->hd=_tmp387;_tmp388->tl=
_tmp383;_tmp388;});}}_tmp383=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp383);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp38A=
_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_DeclList_tok_struct _tmp38B;
_tmp38B.tag=Cyc_DeclList_tok;_tmp38B.f1=({struct Cyc_List_List*_tmp38C=_cycalloc(
sizeof(*_tmp38C));_tmp38C->hd=Cyc_Absyn_letv_decl(_tmp383,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp38C->tl=0;
_tmp38C;});_tmp38B;});_tmp38A;});break;}case 25: _LL1DA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL1DB: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp38D=_cycalloc(sizeof(*_tmp38D));
_tmp38D[0]=({struct Cyc_DeclList_tok_struct _tmp38E;_tmp38E.tag=Cyc_DeclList_tok;
_tmp38E.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp38E;});_tmp38D;});
break;case 27: _LL1DC: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp38F=
_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_DeclSpec_tok_struct _tmp390;
_tmp390.tag=Cyc_DeclSpec_tok;_tmp390.f1=({struct Cyc_Parse_Declaration_spec*
_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->sc=({struct Cyc_Core_Opt*_tmp392=
_cycalloc(sizeof(*_tmp392));_tmp392->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp392;});_tmp391->tq=Cyc_Absyn_empty_tqual();
_tmp391->type_specs=0;_tmp391->is_inline=0;_tmp391->attributes=0;_tmp391;});
_tmp390;});_tmp38F;});break;case 28: _LL1DD: if((Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*_tmp393[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp394="Only one storage class is allowed in a declaration";
_tag_arr(_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,51));}),_tag_arr(
_tmp393,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395[0]=({struct Cyc_DeclSpec_tok_struct
_tmp396;_tmp396.tag=Cyc_DeclSpec_tok;_tmp396.f1=({struct Cyc_Parse_Declaration_spec*
_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->sc=({struct Cyc_Core_Opt*_tmp398=
_cycalloc(sizeof(*_tmp398));_tmp398->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp398;});_tmp397->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp397->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp397->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp397->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp397;});_tmp396;});_tmp395;});break;case 29: _LL1DE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({struct Cyc_DeclSpec_tok_struct
_tmp39A;_tmp39A.tag=Cyc_DeclSpec_tok;_tmp39A.f1=({struct Cyc_Parse_Declaration_spec*
_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->sc=0;_tmp39B->tq=Cyc_Absyn_empty_tqual();
_tmp39B->type_specs=({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));
_tmp39C->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39C->tl=0;_tmp39C;});
_tmp39B->is_inline=0;_tmp39B->attributes=0;_tmp39B;});_tmp39A;});_tmp399;});
break;case 30: _LL1DF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp39D=
_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_DeclSpec_tok_struct _tmp39E;
_tmp39E.tag=Cyc_DeclSpec_tok;_tmp39E.f1=({struct Cyc_Parse_Declaration_spec*
_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp39F->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp39F->type_specs=({
struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A0->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3A0;});
_tmp39F->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp39F->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp39F;});
_tmp39E;});_tmp39D;});break;case 31: _LL1E0: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3A2;_tmp3A2.tag=Cyc_DeclSpec_tok;_tmp3A2.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->sc=0;_tmp3A3->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A3->type_specs=0;
_tmp3A3->is_inline=0;_tmp3A3->attributes=0;_tmp3A3;});_tmp3A2;});_tmp3A1;});
break;case 32: _LL1E1: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_DeclSpec_tok_struct _tmp3A5;
_tmp3A5.tag=Cyc_DeclSpec_tok;_tmp3A5.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3A6->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp3A6->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp3A6->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3A6->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3A6;});_tmp3A5;});_tmp3A4;});break;case 33: _LL1E2: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3A8;_tmp3A8.tag=Cyc_DeclSpec_tok;_tmp3A8.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->sc=0;_tmp3A9->tq=Cyc_Absyn_empty_tqual();
_tmp3A9->type_specs=0;_tmp3A9->is_inline=1;_tmp3A9->attributes=0;_tmp3A9;});
_tmp3A8;});_tmp3A7;});break;case 34: _LL1E3: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3AB;_tmp3AB.tag=Cyc_DeclSpec_tok;_tmp3AB.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3AC->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3AC->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3AC->is_inline=1;_tmp3AC->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3AC;});
_tmp3AB;});_tmp3AA;});break;case 35: _LL1E4: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3AE;_tmp3AE.tag=Cyc_DeclSpec_tok;_tmp3AE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->sc=0;_tmp3AF->tq=Cyc_Absyn_empty_tqual();
_tmp3AF->type_specs=0;_tmp3AF->is_inline=0;_tmp3AF->attributes=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3AF;});_tmp3AE;});
_tmp3AD;});break;case 36: _LL1E5: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3B1;_tmp3B1.tag=Cyc_DeclSpec_tok;_tmp3B1.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3B2->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3B2->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3B2->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3B2->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp3B2;});
_tmp3B1;});_tmp3B0;});break;case 37: _LL1E6: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_StorageClass_tok_struct
_tmp3B4;_tmp3B4.tag=Cyc_StorageClass_tok;_tmp3B4.f1=(void*)((void*)4);_tmp3B4;});
_tmp3B3;});break;case 38: _LL1E7: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5[0]=({struct Cyc_StorageClass_tok_struct
_tmp3B6;_tmp3B6.tag=Cyc_StorageClass_tok;_tmp3B6.f1=(void*)((void*)5);_tmp3B6;});
_tmp3B5;});break;case 39: _LL1E8: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_StorageClass_tok_struct
_tmp3B8;_tmp3B8.tag=Cyc_StorageClass_tok;_tmp3B8.f1=(void*)((void*)3);_tmp3B8;});
_tmp3B7;});break;case 40: _LL1E9: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_StorageClass_tok_struct
_tmp3BA;_tmp3BA.tag=Cyc_StorageClass_tok;_tmp3BA.f1=(void*)((void*)1);_tmp3BA;});
_tmp3B9;});break;case 41: _LL1EA: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp3BB="C";
_tag_arr(_tmp3BB,sizeof(char),_get_zero_arr_size(_tmp3BB,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp3BC="only extern or extern \"C\" is allowed";_tag_arr(_tmp3BC,
sizeof(char),_get_zero_arr_size(_tmp3BC,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({
struct Cyc_StorageClass_tok_struct _tmp3BE;_tmp3BE.tag=Cyc_StorageClass_tok;
_tmp3BE.f1=(void*)((void*)2);_tmp3BE;});_tmp3BD;});break;case 42: _LL1EB: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));
_tmp3BF[0]=({struct Cyc_StorageClass_tok_struct _tmp3C0;_tmp3C0.tag=Cyc_StorageClass_tok;
_tmp3C0.f1=(void*)((void*)0);_tmp3C0;});_tmp3BF;});break;case 43: _LL1EC: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));
_tmp3C1[0]=({struct Cyc_StorageClass_tok_struct _tmp3C2;_tmp3C2.tag=Cyc_StorageClass_tok;
_tmp3C2.f1=(void*)((void*)6);_tmp3C2;});_tmp3C1;});break;case 44: _LL1ED: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));
_tmp3C3[0]=({struct Cyc_AttributeList_tok_struct _tmp3C4;_tmp3C4.tag=Cyc_AttributeList_tok;
_tmp3C4.f1=0;_tmp3C4;});_tmp3C3;});break;case 45: _LL1EE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 46: _LL1EF: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 47: _LL1F0:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3C5=_cycalloc(sizeof(*
_tmp3C5));_tmp3C5[0]=({struct Cyc_AttributeList_tok_struct _tmp3C6;_tmp3C6.tag=Cyc_AttributeList_tok;
_tmp3C6.f1=({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C7->tl=0;_tmp3C7;});_tmp3C6;});_tmp3C5;});break;case 48:
_LL1F1: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp3C8=_cycalloc(
sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_AttributeList_tok_struct _tmp3C9;_tmp3C9.tag=
Cyc_AttributeList_tok;_tmp3C9.f1=({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*
_tmp3CA));_tmp3CA->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3CA->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CA;});_tmp3C9;});
_tmp3C8;});break;case 49: _LL1F2: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple17 att_map[17]={{{_tmp3D1,_tmp3D1,_tmp3D1 + 8},(
void*)0},{{_tmp3D2,_tmp3D2,_tmp3D2 + 6},(void*)1},{{_tmp3D3,_tmp3D3,_tmp3D3 + 9},(
void*)2},{{_tmp3D4,_tmp3D4,_tmp3D4 + 9},(void*)3},{{_tmp3D5,_tmp3D5,_tmp3D5 + 6},(
void*)4},{{_tmp3D6,_tmp3D6,_tmp3D6 + 8},(void*)& att_aligned},{{_tmp3D7,_tmp3D7,
_tmp3D7 + 7},(void*)5},{{_tmp3D8,_tmp3D8,_tmp3D8 + 7},(void*)7},{{_tmp3D9,_tmp3D9,
_tmp3D9 + 7},(void*)8},{{_tmp3DA,_tmp3DA,_tmp3DA + 5},(void*)9},{{_tmp3DB,_tmp3DB,
_tmp3DB + 10},(void*)10},{{_tmp3DC,_tmp3DC,_tmp3DC + 10},(void*)11},{{_tmp3DD,
_tmp3DD,_tmp3DD + 23},(void*)12},{{_tmp3DE,_tmp3DE,_tmp3DE + 12},(void*)13},{{
_tmp3DF,_tmp3DF,_tmp3DF + 11},(void*)14},{{_tmp3E0,_tmp3E0,_tmp3E0 + 22},(void*)15},{{
_tmp3E1,_tmp3E1,_tmp3E1 + 5},(void*)16}};struct _tagged_arr _tmp3CB=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp3CB,sizeof(char))> 4?*((const char*)_check_unknown_subscript(_tmp3CB,sizeof(
char),0))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3CB,sizeof(char),1))
== '_': 0)?*((const char*)_check_unknown_subscript(_tmp3CB,sizeof(char),(int)(
_get_arr_size(_tmp3CB,sizeof(char))- 2)))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp3CB,sizeof(char),(int)(_get_arr_size(_tmp3CB,
sizeof(char))- 3)))== '_': 0)_tmp3CB=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp3CB,2,_get_arr_size(_tmp3CB,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp3CB,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3CC=_cycalloc(sizeof(*
_tmp3CC));_tmp3CC[0]=({struct Cyc_Attribute_tok_struct _tmp3CD;_tmp3CD.tag=Cyc_Attribute_tok;
_tmp3CD.f1=(void*)(att_map[i]).f2;_tmp3CD;});_tmp3CC;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp3CE="unrecognized attribute";_tag_arr(_tmp3CE,sizeof(char),
_get_zero_arr_size(_tmp3CE,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({
struct Cyc_Attribute_tok_struct _tmp3D0;_tmp3D0.tag=Cyc_Attribute_tok;_tmp3D0.f1=(
void*)((void*)1);_tmp3D0;});_tmp3CF;});}break;}}case 50: _LL1F3: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({
struct Cyc_Attribute_tok_struct _tmp3E4;_tmp3E4.tag=Cyc_Attribute_tok;_tmp3E4.f1=(
void*)((void*)4);_tmp3E4;});_tmp3E3;});break;case 51: _LL1F4: {struct _tagged_arr
_tmp3E5=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp3E7;int _tmp3E8;struct _tuple13*_tmp3E6=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3E7=*_tmp3E6;
_tmp3E8=_tmp3E7.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({const char*
_tmp3E9="regparm";_tag_arr(_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,8));}))
== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({const char*_tmp3EA="__regparm__";
_tag_arr(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,12));}))== 0){if(
_tmp3E8 < 0?1: _tmp3E8 > 3)Cyc_Parse_err(({const char*_tmp3EB="regparm requires value between 0 and 3";
_tag_arr(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3EC=_cycalloc_atomic(sizeof(*_tmp3EC));
_tmp3EC[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3ED;_tmp3ED.tag=0;_tmp3ED.f1=
_tmp3E8;_tmp3ED;});_tmp3EC;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({
const char*_tmp3EE="aligned";_tag_arr(_tmp3EE,sizeof(char),_get_zero_arr_size(
_tmp3EE,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({const char*_tmp3EF="__aligned__";
_tag_arr(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,12));}))== 0){if(
_tmp3E8 < 0)Cyc_Parse_err(({const char*_tmp3F0="aligned requires positive power of two";
_tag_arr(_tmp3F0,sizeof(char),_get_zero_arr_size(_tmp3F0,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp3E8;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp3F1="aligned requires positive power of two";
_tag_arr(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3F2=_cycalloc_atomic(sizeof(*_tmp3F2));
_tmp3F2[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3F3;_tmp3F3.tag=1;_tmp3F3.f1=
_tmp3E8;_tmp3F3;});_tmp3F2;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({
const char*_tmp3F4="initializes";_tag_arr(_tmp3F4,sizeof(char),_get_zero_arr_size(
_tmp3F4,12));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)_tmp3E5,({const char*
_tmp3F5="__initializes__";_tag_arr(_tmp3F5,sizeof(char),_get_zero_arr_size(
_tmp3F5,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp3F6=
_cycalloc_atomic(sizeof(*_tmp3F6));_tmp3F6[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp3F7;_tmp3F7.tag=4;_tmp3F7.f1=_tmp3E8;_tmp3F7;});_tmp3F6;});else{Cyc_Parse_err(({
const char*_tmp3F8="unrecognized attribute";_tag_arr(_tmp3F8,sizeof(char),
_get_zero_arr_size(_tmp3F8,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));
_tmp3F9[0]=({struct Cyc_Attribute_tok_struct _tmp3FA;_tmp3FA.tag=Cyc_Attribute_tok;
_tmp3FA.f1=(void*)a;_tmp3FA;});_tmp3F9;});break;}}case 52: _LL1F5: {struct
_tagged_arr _tmp3FB=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3FC=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp3FB,({const char*_tmp3FD="section";_tag_arr(_tmp3FD,sizeof(
char),_get_zero_arr_size(_tmp3FD,8));}))== 0?1: Cyc_zstrcmp((struct _tagged_arr)
_tmp3FB,({const char*_tmp3FE="__section__";_tag_arr(_tmp3FE,sizeof(char),
_get_zero_arr_size(_tmp3FE,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({struct Cyc_Absyn_Section_att_struct
_tmp400;_tmp400.tag=2;_tmp400.f1=_tmp3FC;_tmp400;});_tmp3FF;});else{Cyc_Parse_err(({
const char*_tmp401="unrecognized attribute";_tag_arr(_tmp401,sizeof(char),
_get_zero_arr_size(_tmp401,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp402=_cycalloc(sizeof(*_tmp402));
_tmp402[0]=({struct Cyc_Attribute_tok_struct _tmp403;_tmp403.tag=Cyc_Attribute_tok;
_tmp403.f1=(void*)a;_tmp403;});_tmp402;});break;}case 53: _LL1F6: {struct
_tagged_arr _tmp404=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp405=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp407;struct
_tuple13 _tmp406=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp407=_tmp406.f2;{int _tmp409;struct _tuple13 _tmp408=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp409=_tmp408.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp404,({const char*_tmp40A="format";
_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(_tmp40A,7));}))== 0?1: Cyc_zstrcmp((
struct _tagged_arr)_tmp404,({const char*_tmp40B="__format__";_tag_arr(_tmp40B,
sizeof(char),_get_zero_arr_size(_tmp40B,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp405,({const char*_tmp40C="printf";_tag_arr(_tmp40C,sizeof(char),
_get_zero_arr_size(_tmp40C,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D[0]=({struct Cyc_Absyn_Format_att_struct
_tmp40E;_tmp40E.tag=3;_tmp40E.f1=(void*)((void*)0);_tmp40E.f2=_tmp407;_tmp40E.f3=
_tmp409;_tmp40E;});_tmp40D;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp405,({
const char*_tmp40F="scanf";_tag_arr(_tmp40F,sizeof(char),_get_zero_arr_size(
_tmp40F,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp410=
_cycalloc(sizeof(*_tmp410));_tmp410[0]=({struct Cyc_Absyn_Format_att_struct
_tmp411;_tmp411.tag=3;_tmp411.f1=(void*)((void*)1);_tmp411.f2=_tmp407;_tmp411.f3=
_tmp409;_tmp411;});_tmp410;});else{Cyc_Parse_err(({const char*_tmp412="unrecognized format type";
_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp413="unrecognized attribute";_tag_arr(_tmp413,sizeof(char),
_get_zero_arr_size(_tmp413,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414[0]=({
struct Cyc_Attribute_tok_struct _tmp415;_tmp415.tag=Cyc_Attribute_tok;_tmp415.f1=(
void*)a;_tmp415;});_tmp414;});break;}}}case 54: _LL1F7: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp417;_tmp417.tag=Cyc_TypeSpecifier_tok;_tmp417.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp417;});_tmp416;});break;case 55: _LL1F8: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp418=_cycalloc(sizeof(*_tmp418));
_tmp418[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp419;_tmp419.tag=Cyc_TypeSpecifier_tok;
_tmp419.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp419;});
_tmp418;});break;case 56: _LL1F9: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp41B;_tmp41B.tag=Cyc_TypeSpecifier_tok;_tmp41B.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp41C=_cycalloc(sizeof(*_tmp41C));
_tmp41C->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp41C;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp41B;});
_tmp41A;});break;case 57: _LL1FA: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp41E;_tmp41E.tag=Cyc_TypeSpecifier_tok;_tmp41E.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp41E;});
_tmp41D;});break;case 58: _LL1FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp420;_tmp420.tag=Cyc_TypeSpecifier_tok;_tmp420.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Parse_Short_spec_struct
_tmp422;_tmp422.tag=2;_tmp422.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp422;});
_tmp421;}));_tmp420;});_tmp41F;});break;case 59: _LL1FC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp424;_tmp424.tag=Cyc_TypeSpecifier_tok;_tmp424.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp424;});
_tmp423;});break;case 60: _LL1FD: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp426;_tmp426.tag=Cyc_TypeSpecifier_tok;_tmp426.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_Parse_Long_spec_struct
_tmp428;_tmp428.tag=3;_tmp428.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp428;});
_tmp427;}));_tmp426;});_tmp425;});break;case 61: _LL1FE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp42A;_tmp42A.tag=Cyc_TypeSpecifier_tok;_tmp42A.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42A;});
_tmp429;});break;case 62: _LL1FF: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp42C;_tmp42C.tag=Cyc_TypeSpecifier_tok;_tmp42C.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42C;});
_tmp42B;});break;case 63: _LL200: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp42E;_tmp42E.tag=Cyc_TypeSpecifier_tok;_tmp42E.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp430;});
_tmp42F;}));_tmp42E;});_tmp42D;});break;case 64: _LL201: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp432;_tmp432.tag=Cyc_TypeSpecifier_tok;_tmp432.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp434;_tmp434.tag=1;_tmp434.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp434;});
_tmp433;}));_tmp432;});_tmp431;});break;case 65: _LL202: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 66: _LL203: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 67: _LL204:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 68:
_LL205: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp435=_cycalloc(
sizeof(*_tmp435));_tmp435[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp436;_tmp436.tag=
Cyc_TypeSpecifier_tok;_tmp436.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp436;});
_tmp435;});break;case 69: _LL206: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp438;_tmp438.tag=Cyc_TypeSpecifier_tok;_tmp438.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp439=_cycalloc(sizeof(*_tmp439));
_tmp439[0]=({struct Cyc_Absyn_TypedefType_struct _tmp43A;_tmp43A.tag=16;_tmp43A.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp43A.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp43A.f3=0;_tmp43A.f4=0;_tmp43A;});_tmp439;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp438;});
_tmp437;});break;case 70: _LL207: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43C;_tmp43C.tag=Cyc_TypeSpecifier_tok;_tmp43C.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));
_tmp43D[0]=({struct Cyc_Absyn_TupleType_struct _tmp43E;_tmp43E.tag=9;_tmp43E.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp43E;});_tmp43D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp43C;});_tmp43B;});break;case 71: _LL208: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));
_tmp43F[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp440;_tmp440.tag=Cyc_TypeSpecifier_tok;
_tmp440.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp442;_tmp442.tag=15;_tmp442.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp442;});_tmp441;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp440;});_tmp43F;});break;case 72: _LL209: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp443=_cycalloc(sizeof(*_tmp443));
_tmp443[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp444;_tmp444.tag=Cyc_TypeSpecifier_tok;
_tmp444.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp446;_tmp446.tag=14;_tmp446.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp446;});_tmp445;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp444;});_tmp443;});break;case 73: _LL20A: {struct
_tuple13 _tmp448;int _tmp449;struct _tuple13*_tmp447=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp448=*_tmp447;_tmp449=
_tmp448.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp44A=_cycalloc(
sizeof(*_tmp44A));_tmp44A[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp44B;_tmp44B.tag=
Cyc_TypeSpecifier_tok;_tmp44B.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp44C=_cycalloc_atomic(sizeof(*_tmp44C));_tmp44C[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp44D;_tmp44D.tag=18;_tmp44D.f1=_tmp449;_tmp44D;});_tmp44C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44B;});
_tmp44A;});break;}case 74: _LL20B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp44F;_tmp44F.tag=Cyc_TypeSpecifier_tok;_tmp44F.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp450=_cycalloc(sizeof(*_tmp450));
_tmp450[0]=({struct Cyc_Absyn_TagType_struct _tmp451;_tmp451.tag=17;_tmp451.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp451;});_tmp450;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44F;});
_tmp44E;});break;case 75: _LL20C: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp452=
_cycalloc(sizeof(*_tmp452));_tmp452[0]=({struct Cyc_Kind_tok_struct _tmp453;
_tmp453.tag=Cyc_Kind_tok;_tmp453.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp453;});
_tmp452;});break;case 76: _LL20D: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp454=_cycalloc_atomic(sizeof(*_tmp454));_tmp454[0]=({struct Cyc_TypeQual_tok_struct
_tmp455;_tmp455.tag=Cyc_TypeQual_tok;_tmp455.f1=({struct Cyc_Absyn_Tqual _tmp456;
_tmp456.q_const=1;_tmp456.q_volatile=0;_tmp456.q_restrict=0;_tmp456;});_tmp455;});
_tmp454;});break;case 77: _LL20E: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp457=_cycalloc_atomic(sizeof(*_tmp457));_tmp457[0]=({struct Cyc_TypeQual_tok_struct
_tmp458;_tmp458.tag=Cyc_TypeQual_tok;_tmp458.f1=({struct Cyc_Absyn_Tqual _tmp459;
_tmp459.q_const=0;_tmp459.q_volatile=1;_tmp459.q_restrict=0;_tmp459;});_tmp458;});
_tmp457;});break;case 78: _LL20F: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp45A=_cycalloc_atomic(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_TypeQual_tok_struct
_tmp45B;_tmp45B.tag=Cyc_TypeQual_tok;_tmp45B.f1=({struct Cyc_Absyn_Tqual _tmp45C;
_tmp45C.q_const=0;_tmp45C.q_volatile=0;_tmp45C.q_restrict=1;_tmp45C;});_tmp45B;});
_tmp45A;});break;case 79: _LL210: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp45E;_tmp45E.tag=Cyc_TypeSpecifier_tok;_tmp45E.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp460;_tmp460.tag=5;_tmp460.f1=({struct Cyc_Absyn_Decl*_tmp461=_cycalloc(
sizeof(*_tmp461));_tmp461->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp463;_tmp463.tag=6;_tmp463.f1=({struct Cyc_Absyn_Enumdecl*_tmp464=_cycalloc(
sizeof(*_tmp464));_tmp464->sc=(void*)((void*)2);_tmp464->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp464->fields=({
struct Cyc_Core_Opt*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp465;});_tmp464;});
_tmp463;});_tmp462;}));_tmp461->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp461;});
_tmp460;});_tmp45F;}));_tmp45E;});_tmp45D;});break;case 80: _LL211: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp467;_tmp467.tag=Cyc_TypeSpecifier_tok;
_tmp467.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_Absyn_EnumType_struct
_tmp469;_tmp469.tag=12;_tmp469.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp469.f2=0;_tmp469;});
_tmp468;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp467;});_tmp466;});break;case 81: _LL212: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));
_tmp46A[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp46B;_tmp46B.tag=Cyc_TypeSpecifier_tok;
_tmp46B.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp46C=_cycalloc(
sizeof(*_tmp46C));_tmp46C[0]=({struct Cyc_Parse_Type_spec_struct _tmp46D;_tmp46D.tag=
4;_tmp46D.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp46E=
_cycalloc(sizeof(*_tmp46E));_tmp46E[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp46F;_tmp46F.tag=13;_tmp46F.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46F;});_tmp46E;}));
_tmp46D.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp46D;});_tmp46C;}));_tmp46B;});_tmp46A;});break;
case 82: _LL213: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp470=_cycalloc(
sizeof(*_tmp470));_tmp470[0]=({struct Cyc_Enumfield_tok_struct _tmp471;_tmp471.tag=
Cyc_Enumfield_tok;_tmp471.f1=({struct Cyc_Absyn_Enumfield*_tmp472=_cycalloc(
sizeof(*_tmp472));_tmp472->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp472->tag=0;_tmp472->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp472;});_tmp471;});_tmp470;});break;case 83: _LL214:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp473=_cycalloc(sizeof(*_tmp473));
_tmp473[0]=({struct Cyc_Enumfield_tok_struct _tmp474;_tmp474.tag=Cyc_Enumfield_tok;
_tmp474.f1=({struct Cyc_Absyn_Enumfield*_tmp475=_cycalloc(sizeof(*_tmp475));
_tmp475->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp475->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp475->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp475;});
_tmp474;});_tmp473;});break;case 84: _LL215: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp477;_tmp477.tag=Cyc_EnumfieldList_tok;_tmp477.f1=({struct Cyc_List_List*
_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp478->tl=0;_tmp478;});
_tmp477;});_tmp476;});break;case 85: _LL216: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp47A;_tmp47A.tag=Cyc_EnumfieldList_tok;_tmp47A.f1=({struct Cyc_List_List*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp47B->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp47B;});_tmp47A;});
_tmp479;});break;case 86: _LL217: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47D;_tmp47D.tag=Cyc_TypeSpecifier_tok;_tmp47D.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));
_tmp47E[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp47F;_tmp47F.tag=11;_tmp47F.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp47F.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47F;});_tmp47E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47D;});_tmp47C;});break;case 87: _LL218: {struct
Cyc_List_List*_tmp480=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp481=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp483;_tmp483.tag=Cyc_TypeSpecifier_tok;
_tmp483.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp484=_cycalloc(
sizeof(*_tmp484));_tmp484[0]=({struct Cyc_Parse_Decl_spec_struct _tmp485;_tmp485.tag=
5;_tmp485.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp480,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp481,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp485;});
_tmp484;}));_tmp483;});_tmp482;});break;}case 88: _LL219: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp487;_tmp487.tag=Cyc_TypeSpecifier_tok;_tmp487.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp488=_cycalloc(sizeof(*_tmp488));
_tmp488[0]=({struct Cyc_Absyn_AggrType_struct _tmp489;_tmp489.tag=10;_tmp489.f1=({
struct Cyc_Absyn_AggrInfo _tmp48A;_tmp48A.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp48C;_tmp48C.tag=0;_tmp48C.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp48C.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48C;});_tmp48B;}));
_tmp48A.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp48A;});_tmp489;});_tmp488;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp487;});
_tmp486;});break;case 89: _LL21A: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_TypeList_tok_struct
_tmp48E;_tmp48E.tag=Cyc_TypeList_tok;_tmp48E.f1=0;_tmp48E;});_tmp48D;});break;
case 90: _LL21B: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp48F=_cycalloc(
sizeof(*_tmp48F));_tmp48F[0]=({struct Cyc_TypeList_tok_struct _tmp490;_tmp490.tag=
Cyc_TypeList_tok;_tmp490.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp490;});_tmp48F;});
break;case 91: _LL21C: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp491=
_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp492;_tmp492.tag=Cyc_StructOrUnion_tok;_tmp492.f1=(void*)((void*)0);_tmp492;});
_tmp491;});break;case 92: _LL21D: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp494;_tmp494.tag=Cyc_StructOrUnion_tok;_tmp494.f1=(void*)((void*)1);_tmp494;});
_tmp493;});break;case 93: _LL21E: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp496;_tmp496.tag=Cyc_AggrFieldDeclList_tok;_tmp496.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp496;});_tmp495;});
break;case 94: _LL21F: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp498;_tmp498.tag=Cyc_AggrFieldDeclListList_tok;_tmp498.f1=({struct Cyc_List_List*
_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp499->tl=0;_tmp499;});
_tmp498;});_tmp497;});break;case 95: _LL220: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp49B;_tmp49B.tag=Cyc_AggrFieldDeclListList_tok;_tmp49B.f1=({struct Cyc_List_List*
_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49C->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49C;});_tmp49B;});
_tmp49A;});break;case 96: _LL221: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=({struct Cyc_InitDeclList_tok_struct
_tmp49E;_tmp49E.tag=Cyc_InitDeclList_tok;_tmp49E.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp49E;});_tmp49D;});
break;case 97: _LL222: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp49F=
_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_InitDeclList_tok;_tmp4A0.f1=({struct Cyc_List_List*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A1->tl=0;_tmp4A1;});
_tmp4A0;});_tmp49F;});break;case 98: _LL223: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_InitDeclList_tok;_tmp4A3.f1=({struct Cyc_List_List*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A4->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A4;});_tmp4A3;});
_tmp4A2;});break;case 99: _LL224: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_InitDecl_tok_struct
_tmp4A6;_tmp4A6.tag=Cyc_InitDecl_tok;_tmp4A6.f1=({struct _tuple12*_tmp4A7=
_cycalloc(sizeof(*_tmp4A7));_tmp4A7->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A7->f2=0;_tmp4A7;});
_tmp4A6;});_tmp4A5;});break;case 100: _LL225: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_InitDecl_tok_struct
_tmp4A9;_tmp4A9.tag=Cyc_InitDecl_tok;_tmp4A9.f1=({struct _tuple12*_tmp4AA=
_cycalloc(sizeof(*_tmp4AA));_tmp4AA->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4AA->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4AA;});_tmp4A9;});_tmp4A8;});break;case 101: _LL226: {struct _tuple15 _tmp4AC;
struct Cyc_Absyn_Tqual _tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_List_List*
_tmp4AF;struct _tuple15*_tmp4AB=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4AC=*_tmp4AB;_tmp4AD=
_tmp4AC.f1;_tmp4AE=_tmp4AC.f2;_tmp4AF=_tmp4AC.f3;{struct Cyc_List_List*_tmp4B1;
struct Cyc_List_List*_tmp4B2;struct _tuple0 _tmp4B0=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4B1=_tmp4B0.f1;
_tmp4B2=_tmp4B0.f2;{void*_tmp4B3=Cyc_Parse_speclist2typ(_tmp4AE,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4AD,_tmp4B3,_tmp4B1,_tmp4AF),_tmp4B2);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp4B5=_cycalloc(sizeof(*
_tmp4B5));_tmp4B5[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp4B6;_tmp4B6.tag=
Cyc_AggrFieldDeclList_tok;_tmp4B6.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4B4);
_tmp4B6;});_tmp4B5;});break;}}}case 102: _LL227: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4B8;_tmp4B8.tag=Cyc_QualSpecList_tok;_tmp4B8.f1=({struct _tuple15*_tmp4B9=
_cycalloc(sizeof(*_tmp4B9));_tmp4B9->f1=Cyc_Absyn_empty_tqual();_tmp4B9->f2=({
struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4BA->tl=0;_tmp4BA;});
_tmp4B9->f3=0;_tmp4B9;});_tmp4B8;});_tmp4B7;});break;case 103: _LL228: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({
struct Cyc_QualSpecList_tok_struct _tmp4BC;_tmp4BC.tag=Cyc_QualSpecList_tok;
_tmp4BC.f1=({struct _tuple15*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4BD->f2=({struct Cyc_List_List*_tmp4BE=_cycalloc(sizeof(*
_tmp4BE));_tmp4BE->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BE->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4BE;});_tmp4BD->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp4BD;});_tmp4BC;});_tmp4BB;});break;case 104: _LL229: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));
_tmp4BF[0]=({struct Cyc_QualSpecList_tok_struct _tmp4C0;_tmp4C0.tag=Cyc_QualSpecList_tok;
_tmp4C0.f1=({struct _tuple15*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C1->f2=0;_tmp4C1->f3=
0;_tmp4C1;});_tmp4C0;});_tmp4BF;});break;case 105: _LL22A: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4C3;_tmp4C3.tag=Cyc_QualSpecList_tok;_tmp4C3.f1=({struct _tuple15*_tmp4C4=
_cycalloc(sizeof(*_tmp4C4));_tmp4C4->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp4C4->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4C4->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4C4;});_tmp4C3;});
_tmp4C2;});break;case 106: _LL22B: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_QualSpecList_tok_struct
_tmp4C6;_tmp4C6.tag=Cyc_QualSpecList_tok;_tmp4C6.f1=({struct _tuple15*_tmp4C7=
_cycalloc(sizeof(*_tmp4C7));_tmp4C7->f1=Cyc_Absyn_empty_tqual();_tmp4C7->f2=0;
_tmp4C7->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4C7;});_tmp4C6;});_tmp4C5;});break;case 107: _LL22C:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp4C8=_cycalloc(sizeof(*
_tmp4C8));_tmp4C8[0]=({struct Cyc_QualSpecList_tok_struct _tmp4C9;_tmp4C9.tag=Cyc_QualSpecList_tok;
_tmp4C9.f1=({struct _tuple15*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp4CA->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4CA->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp4CA;});_tmp4C9;});
_tmp4C8;});break;case 108: _LL22D: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CC;_tmp4CC.tag=Cyc_InitDeclList_tok;_tmp4CC.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4CC;});_tmp4CB;});
break;case 109: _LL22E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp4CD=
_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4CE;_tmp4CE.tag=Cyc_InitDeclList_tok;_tmp4CE.f1=({struct Cyc_List_List*
_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CF->tl=0;_tmp4CF;});
_tmp4CE;});_tmp4CD;});break;case 110: _LL22F: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0[0]=({struct Cyc_InitDeclList_tok_struct
_tmp4D1;_tmp4D1.tag=Cyc_InitDeclList_tok;_tmp4D1.f1=({struct Cyc_List_List*
_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D2->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D2;});_tmp4D1;});
_tmp4D0;});break;case 111: _LL230: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D4;_tmp4D4.tag=Cyc_InitDecl_tok;_tmp4D4.f1=({struct _tuple12*_tmp4D5=
_cycalloc(sizeof(*_tmp4D5));_tmp4D5->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D5->f2=0;_tmp4D5;});
_tmp4D4;});_tmp4D3;});break;case 112: _LL231: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6[0]=({struct Cyc_InitDecl_tok_struct
_tmp4D7;_tmp4D7.tag=Cyc_InitDecl_tok;_tmp4D7.f1=({struct _tuple12*_tmp4D8=
_cycalloc(sizeof(*_tmp4D8));_tmp4D8->f1=({struct Cyc_Parse_Declarator*_tmp4D9=
_cycalloc(sizeof(*_tmp4D9));_tmp4D9->id=({struct _tuple1*_tmp4DA=_cycalloc(
sizeof(*_tmp4DA));_tmp4DA->f1=Cyc_Absyn_rel_ns_null;_tmp4DA->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp4DA;});_tmp4D9->tms=
0;_tmp4D9;});_tmp4D8->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D8;});_tmp4D7;});
_tmp4D6;});break;case 113: _LL232: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC[0]=({struct Cyc_InitDecl_tok_struct
_tmp4DD;_tmp4DD.tag=Cyc_InitDecl_tok;_tmp4DD.f1=({struct _tuple12*_tmp4DE=
_cycalloc(sizeof(*_tmp4DE));_tmp4DE->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DE->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4DE;});_tmp4DD;});_tmp4DC;});break;case 114: _LL233: {struct Cyc_List_List*
_tmp4DF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4E1;_tmp4E1.tag=Cyc_TypeSpecifier_tok;
_tmp4E1.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4E2=_cycalloc(
sizeof(*_tmp4E2));_tmp4E2[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4E3;_tmp4E3.tag=
5;_tmp4E3.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp4DF,({struct Cyc_Core_Opt*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E4;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E3;});
_tmp4E2;}));_tmp4E1;});_tmp4E0;});break;}case 115: _LL234: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4E6;_tmp4E6.tag=Cyc_TypeSpecifier_tok;_tmp4E6.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));
_tmp4E7[0]=({struct Cyc_Absyn_TunionType_struct _tmp4E8;_tmp4E8.tag=2;_tmp4E8.f1=({
struct Cyc_Absyn_TunionInfo _tmp4E9;_tmp4E9.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp4EB;_tmp4EB.tag=0;_tmp4EB.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp4EC;
_tmp4EC.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4EC.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4EC;});_tmp4EB;});
_tmp4EA;}));_tmp4E9.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E9.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E9;});_tmp4E8;});
_tmp4E7;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4E6;});_tmp4E5;});break;case 116: _LL235: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));
_tmp4ED[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4EE;_tmp4EE.tag=Cyc_TypeSpecifier_tok;
_tmp4EE.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp4F0;_tmp4F0.tag=3;_tmp4F0.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp4F1;
_tmp4F1.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp4F3;_tmp4F3.tag=0;_tmp4F3.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp4F4;
_tmp4F4.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4F4.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F4.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp4F4;});_tmp4F3;});
_tmp4F2;}));_tmp4F1.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F1;});_tmp4F0;});
_tmp4EF;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4EE;});_tmp4ED;});break;case 117: _LL236: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp4F5=_cycalloc_atomic(sizeof(*_tmp4F5));
_tmp4F5[0]=({struct Cyc_Bool_tok_struct _tmp4F6;_tmp4F6.tag=Cyc_Bool_tok;_tmp4F6.f1=
0;_tmp4F6;});_tmp4F5;});break;case 118: _LL237: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp4F7=_cycalloc_atomic(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_Bool_tok_struct
_tmp4F8;_tmp4F8.tag=Cyc_Bool_tok;_tmp4F8.f1=1;_tmp4F8;});_tmp4F7;});break;case
119: _LL238: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4F9=_cycalloc(
sizeof(*_tmp4F9));_tmp4F9[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4FA;
_tmp4FA.tag=Cyc_TunionFieldList_tok;_tmp4FA.f1=({struct Cyc_List_List*_tmp4FB=
_cycalloc(sizeof(*_tmp4FB));_tmp4FB->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB->tl=0;_tmp4FB;});
_tmp4FA;});_tmp4F9;});break;case 120: _LL239: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp4FD;_tmp4FD.tag=Cyc_TunionFieldList_tok;_tmp4FD.f1=({struct Cyc_List_List*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FE->tl=0;_tmp4FE;});
_tmp4FD;});_tmp4FC;});break;case 121: _LL23A: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp500;_tmp500.tag=Cyc_TunionFieldList_tok;_tmp500.f1=({struct Cyc_List_List*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp501->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp501;});_tmp500;});
_tmp4FF;});break;case 122: _LL23B: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp503;_tmp503.tag=Cyc_TunionFieldList_tok;_tmp503.f1=({struct Cyc_List_List*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp504->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp504;});_tmp503;});
_tmp502;});break;case 123: _LL23C: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Scope_tok_struct
_tmp506;_tmp506.tag=Cyc_Scope_tok;_tmp506.f1=(void*)((void*)2);_tmp506;});
_tmp505;});break;case 124: _LL23D: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507[0]=({struct Cyc_Scope_tok_struct
_tmp508;_tmp508.tag=Cyc_Scope_tok;_tmp508.f1=(void*)((void*)3);_tmp508;});
_tmp507;});break;case 125: _LL23E: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509[0]=({struct Cyc_Scope_tok_struct
_tmp50A;_tmp50A.tag=Cyc_Scope_tok;_tmp50A.f1=(void*)((void*)0);_tmp50A;});
_tmp509;});break;case 126: _LL23F: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_TunionField_tok_struct
_tmp50C;_tmp50C.tag=Cyc_TunionField_tok;_tmp50C.f1=({struct Cyc_Absyn_Tunionfield*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50D->typs=0;_tmp50D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp50D->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50D;});_tmp50C;});
_tmp50B;});break;case 127: _LL240: {struct Cyc_List_List*_tmp50E=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({
struct Cyc_TunionField_tok_struct _tmp510;_tmp510.tag=Cyc_TunionField_tok;_tmp510.f1=({
struct Cyc_Absyn_Tunionfield*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp511->typs=_tmp50E;_tmp511->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp511->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp511;});_tmp510;});_tmp50F;});break;}case 128: _LL241: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 129: _LL242: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp512=_cycalloc(sizeof(*_tmp512));
_tmp512[0]=({struct Cyc_Declarator_tok_struct _tmp513;_tmp513.tag=Cyc_Declarator_tok;
_tmp513.f1=({struct Cyc_Parse_Declarator*_tmp514=_cycalloc(sizeof(*_tmp514));
_tmp514->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp514->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp514;});
_tmp513;});_tmp512;});break;case 130: _LL243: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_Declarator_tok_struct
_tmp516;_tmp516.tag=Cyc_Declarator_tok;_tmp516.f1=({struct Cyc_Parse_Declarator*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp517->tms=0;_tmp517;});
_tmp516;});_tmp515;});break;case 131: _LL244: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 132: _LL245:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp518=_cycalloc(sizeof(*_tmp518));
_tmp518[0]=({struct Cyc_Declarator_tok_struct _tmp519;_tmp519.tag=Cyc_Declarator_tok;
_tmp519.f1=({struct Cyc_Parse_Declarator*_tmp51A=_cycalloc(sizeof(*_tmp51A));
_tmp51A->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp51A->tms=({struct Cyc_List_List*_tmp51B=_cycalloc(
sizeof(*_tmp51B));_tmp51B->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp51D;_tmp51D.tag=0;_tmp51D.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51D;});_tmp51C;}));
_tmp51B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp51B;});_tmp51A;});_tmp519;});_tmp518;});break;case
133: _LL246: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp51E=_cycalloc(
sizeof(*_tmp51E));_tmp51E[0]=({struct Cyc_Declarator_tok_struct _tmp51F;_tmp51F.tag=
Cyc_Declarator_tok;_tmp51F.f1=({struct Cyc_Parse_Declarator*_tmp520=_cycalloc(
sizeof(*_tmp520));_tmp520->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp520->tms=({
struct Cyc_List_List*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp522=_cycalloc(sizeof(*_tmp522));
_tmp522[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp523;_tmp523.tag=1;_tmp523.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp523.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp523;});_tmp522;}));_tmp521->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp521;});_tmp520;});
_tmp51F;});_tmp51E;});break;case 134: _LL247: {struct _tuple14 _tmp525;struct Cyc_List_List*
_tmp526;int _tmp527;struct Cyc_Absyn_VarargInfo*_tmp528;struct Cyc_Core_Opt*_tmp529;
struct Cyc_List_List*_tmp52A;struct _tuple14*_tmp524=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp525=*_tmp524;_tmp526=
_tmp525.f1;_tmp527=_tmp525.f2;_tmp528=_tmp525.f3;_tmp529=_tmp525.f4;_tmp52A=
_tmp525.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp52B=_cycalloc(
sizeof(*_tmp52B));_tmp52B[0]=({struct Cyc_Declarator_tok_struct _tmp52C;_tmp52C.tag=
Cyc_Declarator_tok;_tmp52C.f1=({struct Cyc_Parse_Declarator*_tmp52D=_cycalloc(
sizeof(*_tmp52D));_tmp52D->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp52D->tms=({
struct Cyc_List_List*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));
_tmp52F[0]=({struct Cyc_Absyn_Function_mod_struct _tmp530;_tmp530.tag=3;_tmp530.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp531=_cycalloc(sizeof(*
_tmp531));_tmp531[0]=({struct Cyc_Absyn_WithTypes_struct _tmp532;_tmp532.tag=1;
_tmp532.f1=_tmp526;_tmp532.f2=_tmp527;_tmp532.f3=_tmp528;_tmp532.f4=_tmp529;
_tmp532.f5=_tmp52A;_tmp532;});_tmp531;}));_tmp530;});_tmp52F;}));_tmp52E->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp52E;});_tmp52D;});_tmp52C;});_tmp52B;});break;}case 135: _LL248:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp533=_cycalloc(sizeof(*_tmp533));
_tmp533[0]=({struct Cyc_Declarator_tok_struct _tmp534;_tmp534.tag=Cyc_Declarator_tok;
_tmp534.f1=({struct Cyc_Parse_Declarator*_tmp535=_cycalloc(sizeof(*_tmp535));
_tmp535->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp535->tms=({struct Cyc_List_List*_tmp536=_cycalloc(
sizeof(*_tmp536));_tmp536->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp538;_tmp538.tag=3;_tmp538.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp53A;_tmp53A.tag=1;_tmp53A.f1=0;_tmp53A.f2=0;_tmp53A.f3=0;_tmp53A.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp53A.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53A;});_tmp539;}));
_tmp538;});_tmp537;}));_tmp536->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp536;});_tmp535;});
_tmp534;});_tmp533;});break;case 136: _LL249: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({struct Cyc_Declarator_tok_struct
_tmp53C;_tmp53C.tag=Cyc_Declarator_tok;_tmp53C.f1=({struct Cyc_Parse_Declarator*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp53D->tms=({
struct Cyc_List_List*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));
_tmp53F[0]=({struct Cyc_Absyn_Function_mod_struct _tmp540;_tmp540.tag=3;_tmp540.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp541=_cycalloc(sizeof(*_tmp541));
_tmp541[0]=({struct Cyc_Absyn_NoTypes_struct _tmp542;_tmp542.tag=0;_tmp542.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp542.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp542;});
_tmp541;}));_tmp540;});_tmp53F;}));_tmp53E->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp53E;});_tmp53D;});
_tmp53C;});_tmp53B;});break;case 137: _LL24A: {struct Cyc_List_List*_tmp543=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544[0]=({
struct Cyc_Declarator_tok_struct _tmp545;_tmp545.tag=Cyc_Declarator_tok;_tmp545.f1=({
struct Cyc_Parse_Declarator*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp546->tms=({
struct Cyc_List_List*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp548=_cycalloc(sizeof(*_tmp548));
_tmp548[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp549;_tmp549.tag=4;_tmp549.f1=
_tmp543;_tmp549.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp549.f3=0;
_tmp549;});_tmp548;}));_tmp547->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp547;});_tmp546;});
_tmp545;});_tmp544;});break;}case 138: _LL24B: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A[0]=({struct Cyc_Declarator_tok_struct
_tmp54B;_tmp54B.tag=Cyc_Declarator_tok;_tmp54B.f1=({struct Cyc_Parse_Declarator*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp54C->tms=({
struct Cyc_List_List*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));
_tmp54E[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp54F;_tmp54F.tag=5;_tmp54F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp54F.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54F;});_tmp54E;}));
_tmp54D->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp54D;});_tmp54C;});_tmp54B;});_tmp54A;});break;case
139: _LL24C: Cyc_Parse_err(({const char*_tmp550="identifier has not been declared as a typedef";
_tag_arr(_tmp550,sizeof(char),_get_zero_arr_size(_tmp550,46));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[0]=({
struct Cyc_Declarator_tok_struct _tmp552;_tmp552.tag=Cyc_Declarator_tok;_tmp552.f1=({
struct Cyc_Parse_Declarator*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp553->tms=0;_tmp553;});
_tmp552;});_tmp551;});break;case 140: _LL24D: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554[0]=({struct Cyc_Declarator_tok_struct
_tmp555;_tmp555.tag=Cyc_Declarator_tok;_tmp555.f1=({struct Cyc_Parse_Declarator*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->id=Cyc_Absyn_exn_name;_tmp556->tms=
0;_tmp556;});_tmp555;});_tmp554;});break;case 141: _LL24E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 142: _LL24F: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp557=_cycalloc(sizeof(*_tmp557));
_tmp557[0]=({struct Cyc_TypeModifierList_tok_struct _tmp558;_tmp558.tag=Cyc_TypeModifierList_tok;
_tmp558.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp558;});_tmp557;});
break;case 143: _LL250: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp55B;_tmp55B.tag=5;_tmp55B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp55B.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp55B;});_tmp55A;}));_tmp559->tl=ans;_tmp559;});ans=({
struct Cyc_List_List*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=(void*)((
void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));
_tmp55D[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp55E;_tmp55E.tag=2;_tmp55E.f1=({
struct Cyc_Absyn_PtrAtts _tmp55F;_tmp55F.rgn=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp55F.nullable=(*Cyc_yyget_YY1(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp55F.bounds=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f2;
_tmp55F.zero_term=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp55F;});_tmp55E.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55E;});_tmp55D;}));
_tmp55C->tl=ans;_tmp55C;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp561;_tmp561.tag=Cyc_TypeModifierList_tok;_tmp561.f1=ans;_tmp561;});_tmp560;});
break;}case 144: _LL251: yyval=(void*)({struct Cyc_YY1_struct*_tmp562=_cycalloc(
sizeof(*_tmp562));_tmp562[0]=({struct Cyc_YY1_struct _tmp563;_tmp563.tag=Cyc_YY1;
_tmp563.f1=({struct _tuple16*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->f1=Cyc_Absyn_true_conref;
_tmp564->f2=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp564;});_tmp563;});_tmp562;});break;case 145: _LL252: yyval=(void*)({struct Cyc_YY1_struct*
_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({struct Cyc_YY1_struct _tmp566;
_tmp566.tag=Cyc_YY1;_tmp566.f1=({struct _tuple16*_tmp567=_cycalloc(sizeof(*
_tmp567));_tmp567->f1=Cyc_Absyn_false_conref;_tmp567->f2=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp567;});_tmp566;});
_tmp565;});break;case 146: _LL253: yyval=(void*)({struct Cyc_YY1_struct*_tmp568=
_cycalloc(sizeof(*_tmp568));_tmp568[0]=({struct Cyc_YY1_struct _tmp569;_tmp569.tag=
Cyc_YY1;_tmp569.f1=({struct _tuple16*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->f1=
Cyc_Absyn_true_conref;_tmp56A->f2=Cyc_Absyn_bounds_unknown_conref;_tmp56A;});
_tmp569;});_tmp568;});break;case 147: _LL254: yyval=(void*)({struct Cyc_YY2_struct*
_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B[0]=({struct Cyc_YY2_struct _tmp56C;
_tmp56C.tag=Cyc_YY2;_tmp56C.f1=Cyc_Absyn_bounds_one_conref;_tmp56C;});_tmp56B;});
break;case 148: _LL255: yyval=(void*)({struct Cyc_YY2_struct*_tmp56D=_cycalloc(
sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_YY2_struct _tmp56E;_tmp56E.tag=Cyc_YY2;
_tmp56E.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp56F=
_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp570;
_tmp570.tag=0;_tmp570.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp570;});_tmp56F;}));_tmp56E;});_tmp56D;});break;case
149: _LL256: yyval=(void*)({struct Cyc_YY2_struct*_tmp571=_cycalloc(sizeof(*_tmp571));
_tmp571[0]=({struct Cyc_YY2_struct _tmp572;_tmp572.tag=Cyc_YY2;_tmp572.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp574;_tmp574.tag=1;_tmp574.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp574;});_tmp573;}));_tmp572;});_tmp571;});break;case 150: _LL257: yyval=(
void*)({struct Cyc_YY4_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({
struct Cyc_YY4_struct _tmp576;_tmp576.tag=Cyc_YY4;_tmp576.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp576;});_tmp575;});break;case 151: _LL258: yyval=(void*)({
struct Cyc_YY4_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577[0]=({struct Cyc_YY4_struct
_tmp578;_tmp578.tag=Cyc_YY4;_tmp578.f1=Cyc_Absyn_true_conref;_tmp578;});_tmp577;});
break;case 152: _LL259: yyval=(void*)({struct Cyc_YY4_struct*_tmp579=_cycalloc(
sizeof(*_tmp579));_tmp579[0]=({struct Cyc_YY4_struct _tmp57A;_tmp57A.tag=Cyc_YY4;
_tmp57A.f1=Cyc_Absyn_false_conref;_tmp57A;});_tmp579;});break;case 153: _LL25A:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));
_tmp57B[0]=({struct Cyc_Type_tok_struct _tmp57C;_tmp57C.tag=Cyc_Type_tok;_tmp57C.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp57D=_cycalloc(sizeof(*_tmp57D));
_tmp57D->v=(void*)((void*)3);_tmp57D;}),0);_tmp57C;});_tmp57B;});break;case 154:
_LL25B: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 155: _LL25C: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({
struct Cyc_Type_tok_struct _tmp57F;_tmp57F.tag=Cyc_Type_tok;_tmp57F.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->v=(void*)((void*)
3);_tmp580;}),0);_tmp57F;});_tmp57E;});break;case 156: _LL25D: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp581=_cycalloc_atomic(sizeof(*_tmp581));_tmp581[
0]=({struct Cyc_TypeQual_tok_struct _tmp582;_tmp582.tag=Cyc_TypeQual_tok;_tmp582.f1=
Cyc_Absyn_empty_tqual();_tmp582;});_tmp581;});break;case 157: _LL25E: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp583=_cycalloc_atomic(sizeof(*_tmp583));_tmp583[
0]=({struct Cyc_TypeQual_tok_struct _tmp584;_tmp584.tag=Cyc_TypeQual_tok;_tmp584.f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp584;});_tmp583;});
break;case 158: _LL25F: yyval=(void*)({struct Cyc_YY3_struct*_tmp585=_cycalloc(
sizeof(*_tmp585));_tmp585[0]=({struct Cyc_YY3_struct _tmp586;_tmp586.tag=Cyc_YY3;
_tmp586.f1=({struct _tuple14*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp587->f2=0;
_tmp587->f3=0;_tmp587->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp587->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp587;});_tmp586;});
_tmp585;});break;case 159: _LL260: yyval=(void*)({struct Cyc_YY3_struct*_tmp588=
_cycalloc(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_YY3_struct _tmp589;_tmp589.tag=
Cyc_YY3;_tmp589.f1=({struct _tuple14*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp58A->f2=1;
_tmp58A->f3=0;_tmp58A->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58A->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58A;});_tmp589;});
_tmp588;});break;case 160: _LL261: {struct _tuple2 _tmp58C;struct Cyc_Core_Opt*
_tmp58D;struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58F;struct _tuple2*_tmp58B=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp58C=*_tmp58B;
_tmp58D=_tmp58C.f1;_tmp58E=_tmp58C.f2;_tmp58F=_tmp58C.f3;{struct Cyc_Absyn_VarargInfo*
_tmp590=({struct Cyc_Absyn_VarargInfo*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->name=
_tmp58D;_tmp594->tq=_tmp58E;_tmp594->type=(void*)_tmp58F;_tmp594->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp594;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591[0]=({
struct Cyc_YY3_struct _tmp592;_tmp592.tag=Cyc_YY3;_tmp592.f1=({struct _tuple14*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->f1=0;_tmp593->f2=0;_tmp593->f3=
_tmp590;_tmp593->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp593->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp593;});_tmp592;});
_tmp591;});break;}}case 161: _LL262: {struct _tuple2 _tmp596;struct Cyc_Core_Opt*
_tmp597;struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;struct _tuple2*_tmp595=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp596=*_tmp595;
_tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;{struct Cyc_Absyn_VarargInfo*
_tmp59A=({struct Cyc_Absyn_VarargInfo*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->name=
_tmp597;_tmp59E->tq=_tmp598;_tmp59E->type=(void*)_tmp599;_tmp59E->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp59E;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B[0]=({
struct Cyc_YY3_struct _tmp59C;_tmp59C.tag=Cyc_YY3;_tmp59C.f1=({struct _tuple14*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp59D->f2=0;_tmp59D->f3=
_tmp59A;_tmp59D->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp59D->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59D;});_tmp59C;});
_tmp59B;});break;}}case 162: _LL263: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({struct Cyc_Type_tok_struct
_tmp5A0;_tmp5A0.tag=Cyc_Type_tok;_tmp5A0.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5A2;_tmp5A2.tag=1;_tmp5A2.f1=0;_tmp5A2;});_tmp5A1;}));_tmp5A0;});_tmp59F;});
break;case 163: _LL264: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5A3=_cycalloc(
sizeof(*_tmp5A3));_tmp5A3[0]=({struct Cyc_Type_tok_struct _tmp5A4;_tmp5A4.tag=Cyc_Type_tok;
_tmp5A4.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5A6;_tmp5A6.tag=0;_tmp5A6.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A6;});_tmp5A5;}));
_tmp5A4;});_tmp5A3;});break;case 164: _LL265: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7[0]=({struct Cyc_TypeOpt_tok_struct
_tmp5A8;_tmp5A8.tag=Cyc_TypeOpt_tok;_tmp5A8.f1=0;_tmp5A8;});_tmp5A7;});break;
case 165: _LL266: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp5A9=_cycalloc(
sizeof(*_tmp5A9));_tmp5A9[0]=({struct Cyc_TypeOpt_tok_struct _tmp5AA;_tmp5AA.tag=
Cyc_TypeOpt_tok;_tmp5AA.f1=({struct Cyc_Core_Opt*_tmp5AB=_cycalloc(sizeof(*
_tmp5AB));_tmp5AB->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5AC=
_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5AD;
_tmp5AD.tag=20;_tmp5AD.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AD;});_tmp5AC;}));
_tmp5AB;});_tmp5AA;});_tmp5A9;});break;case 166: _LL267: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE[0]=({struct Cyc_Rgnorder_tok_struct
_tmp5AF;_tmp5AF.tag=Cyc_Rgnorder_tok;_tmp5AF.f1=0;_tmp5AF;});_tmp5AE;});break;
case 167: _LL268: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 168: _LL269: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5B0=
_cycalloc(sizeof(*_tmp5B0));_tmp5B0[0]=({struct Cyc_Rgnorder_tok_struct _tmp5B1;
_tmp5B1.tag=Cyc_Rgnorder_tok;_tmp5B1.f1=({struct Cyc_List_List*_tmp5B2=_cycalloc(
sizeof(*_tmp5B2));_tmp5B2->hd=({struct _tuple4*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));
_tmp5B3->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5B6=_cycalloc(sizeof(*
_tmp5B6));_tmp5B6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5B7;_tmp5B7.tag=20;
_tmp5B7.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5B7;});_tmp5B6;});_tmp5B3->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5B5;_tmp5B5.tag=0;_tmp5B5.f1=(void*)((void*)3);_tmp5B5;});_tmp5B4;}));
_tmp5B3;});_tmp5B2->tl=0;_tmp5B2;});_tmp5B1;});_tmp5B0;});break;case 169: _LL26A:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));
_tmp5B8[0]=({struct Cyc_Rgnorder_tok_struct _tmp5B9;_tmp5B9.tag=Cyc_Rgnorder_tok;
_tmp5B9.f1=({struct Cyc_List_List*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->hd=({
struct _tuple4*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5BF;_tmp5BF.tag=20;_tmp5BF.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5BF;});_tmp5BE;});
_tmp5BB->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5BD;_tmp5BD.tag=0;_tmp5BD.f1=(void*)((void*)3);_tmp5BD;});_tmp5BC;}));
_tmp5BB;});_tmp5BA->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BA;});_tmp5B9;});
_tmp5B8;});break;case 170: _LL26B: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp5C0=
_cycalloc_atomic(sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Bool_tok_struct _tmp5C1;
_tmp5C1.tag=Cyc_Bool_tok;_tmp5C1.f1=0;_tmp5C1;});_tmp5C0;});break;case 171: _LL26C:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5C2="inject";
_tag_arr(_tmp5C2,sizeof(char),_get_zero_arr_size(_tmp5C2,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5C3="missing type in function declaration";_tag_arr(_tmp5C3,
sizeof(char),_get_zero_arr_size(_tmp5C3,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp5C4=_cycalloc_atomic(sizeof(*_tmp5C4));_tmp5C4[0]=({
struct Cyc_Bool_tok_struct _tmp5C5;_tmp5C5.tag=Cyc_Bool_tok;_tmp5C5.f1=1;_tmp5C5;});
_tmp5C4;});break;case 172: _LL26D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 173: _LL26E: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6[0]=({struct Cyc_TypeList_tok_struct
_tmp5C7;_tmp5C7.tag=Cyc_TypeList_tok;_tmp5C7.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5C7;});_tmp5C6;});
break;case 174: _LL26F: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8[0]=({struct Cyc_TypeList_tok_struct _tmp5C9;
_tmp5C9.tag=Cyc_TypeList_tok;_tmp5C9.f1=0;_tmp5C9;});_tmp5C8;});break;case 175:
_LL270: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 176: _LL271: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5CA=_cycalloc(
sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_TypeList_tok_struct _tmp5CB;_tmp5CB.tag=
Cyc_TypeList_tok;_tmp5CB.f1=({struct Cyc_List_List*_tmp5CC=_cycalloc(sizeof(*
_tmp5CC));_tmp5CC->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5CD=
_cycalloc(sizeof(*_tmp5CD));_tmp5CD[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5CE;
_tmp5CE.tag=21;_tmp5CE.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CE;});_tmp5CD;}));
_tmp5CC->tl=0;_tmp5CC;});_tmp5CB;});_tmp5CA;});break;case 177: _LL272: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5CF=_cycalloc(sizeof(*
_tmp5CF));_tmp5CF[0]=({struct Cyc_TypeList_tok_struct _tmp5D0;_tmp5D0.tag=Cyc_TypeList_tok;
_tmp5D0.f1=({struct Cyc_List_List*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5D1->tl=0;_tmp5D1;});_tmp5D0;});_tmp5CF;});break;case 178: _LL273: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp5D2=_cycalloc(sizeof(*
_tmp5D2));_tmp5D2[0]=({struct Cyc_TypeList_tok_struct _tmp5D3;_tmp5D3.tag=Cyc_TypeList_tok;
_tmp5D3.f1=({struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5D5=_cycalloc(sizeof(*
_tmp5D5));_tmp5D5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5D6;_tmp5D6.tag=19;
_tmp5D6.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5D6;});_tmp5D5;}));_tmp5D4->tl=0;_tmp5D4;});_tmp5D3;});
_tmp5D2;});break;case 179: _LL274: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));
_tmp5D7[0]=({struct Cyc_TypeList_tok_struct _tmp5D8;_tmp5D8.tag=Cyc_TypeList_tok;
_tmp5D8.f1=({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5DA=_cycalloc(sizeof(*
_tmp5DA));_tmp5DA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5DB;_tmp5DB.tag=19;
_tmp5DB.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5DB;});_tmp5DA;}));_tmp5D9->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D9;});_tmp5D8;});
_tmp5D7;});break;case 180: _LL275: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5DD;_tmp5DD.tag=Cyc_ParamDeclList_tok;_tmp5DD.f1=({struct Cyc_List_List*
_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DE->tl=0;_tmp5DE;});
_tmp5DD;});_tmp5DC;});break;case 181: _LL276: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_ParamDeclList_tok;_tmp5E0.f1=({struct Cyc_List_List*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E1->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E1;});_tmp5E0;});
_tmp5DF;});break;case 182: _LL277: {struct _tuple15 _tmp5E3;struct Cyc_Absyn_Tqual
_tmp5E4;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*_tmp5E6;struct _tuple15*
_tmp5E2=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5E3=*_tmp5E2;_tmp5E4=_tmp5E3.f1;_tmp5E5=_tmp5E3.f2;
_tmp5E6=_tmp5E3.f3;{struct Cyc_Parse_Declarator _tmp5E8;struct _tuple1*_tmp5E9;
struct Cyc_List_List*_tmp5EA;struct Cyc_Parse_Declarator*_tmp5E7=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E8=*_tmp5E7;
_tmp5E9=_tmp5E8.id;_tmp5EA=_tmp5E8.tms;{void*_tmp5EB=Cyc_Parse_speclist2typ(
_tmp5E5,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp5ED;void*_tmp5EE;struct
Cyc_List_List*_tmp5EF;struct Cyc_List_List*_tmp5F0;struct _tuple6 _tmp5EC=Cyc_Parse_apply_tms(
_tmp5E4,_tmp5EB,_tmp5E6,_tmp5EA);_tmp5ED=_tmp5EC.f1;_tmp5EE=_tmp5EC.f2;_tmp5EF=
_tmp5EC.f3;_tmp5F0=_tmp5EC.f4;if(_tmp5EF != 0)Cyc_Parse_err(({const char*_tmp5F1="parameter with bad type params";
_tag_arr(_tmp5F1,sizeof(char),_get_zero_arr_size(_tmp5F1,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp5E9))Cyc_Parse_err(({const char*_tmp5F2="parameter cannot be qualified with a namespace";
_tag_arr(_tmp5F2,sizeof(char),_get_zero_arr_size(_tmp5F2,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp5F3=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp5F9=_cycalloc(sizeof(*
_tmp5F9));_tmp5F9->v=(*_tmp5E9).f2;_tmp5F9;});if(_tmp5F0 != 0)({void*_tmp5F4[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp5F5="extra attributes on parameter, ignoring";
_tag_arr(_tmp5F5,sizeof(char),_get_zero_arr_size(_tmp5F5,40));}),_tag_arr(
_tmp5F4,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_ParamDecl_tok_struct
_tmp5F7;_tmp5F7.tag=Cyc_ParamDecl_tok;_tmp5F7.f1=({struct _tuple2*_tmp5F8=
_cycalloc(sizeof(*_tmp5F8));_tmp5F8->f1=_tmp5F3;_tmp5F8->f2=_tmp5ED;_tmp5F8->f3=
_tmp5EE;_tmp5F8;});_tmp5F7;});_tmp5F6;});break;}}}}case 183: _LL278: {struct
_tuple15 _tmp5FB;struct Cyc_Absyn_Tqual _tmp5FC;struct Cyc_List_List*_tmp5FD;struct
Cyc_List_List*_tmp5FE;struct _tuple15*_tmp5FA=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FB=*_tmp5FA;_tmp5FC=
_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;_tmp5FE=_tmp5FB.f3;{void*_tmp5FF=Cyc_Parse_speclist2typ(
_tmp5FD,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp5FE != 0)({void*_tmp600[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp601="bad attributes on parameter, ignoring";
_tag_arr(_tmp601,sizeof(char),_get_zero_arr_size(_tmp601,38));}),_tag_arr(
_tmp600,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602[0]=({struct Cyc_ParamDecl_tok_struct
_tmp603;_tmp603.tag=Cyc_ParamDecl_tok;_tmp603.f1=({struct _tuple2*_tmp604=
_cycalloc(sizeof(*_tmp604));_tmp604->f1=0;_tmp604->f2=_tmp5FC;_tmp604->f3=
_tmp5FF;_tmp604;});_tmp603;});_tmp602;});break;}}case 184: _LL279: {struct _tuple15
_tmp606;struct Cyc_Absyn_Tqual _tmp607;struct Cyc_List_List*_tmp608;struct Cyc_List_List*
_tmp609;struct _tuple15*_tmp605=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp606=*_tmp605;_tmp607=
_tmp606.f1;_tmp608=_tmp606.f2;_tmp609=_tmp606.f3;{void*_tmp60A=Cyc_Parse_speclist2typ(
_tmp608,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp60B=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual
_tmp60D;void*_tmp60E;struct Cyc_List_List*_tmp60F;struct Cyc_List_List*_tmp610;
struct _tuple6 _tmp60C=Cyc_Parse_apply_tms(_tmp607,_tmp60A,_tmp609,_tmp60B);
_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;_tmp60F=_tmp60C.f3;_tmp610=_tmp60C.f4;if(
_tmp60F != 0)({void*_tmp611[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp612="bad type parameters on formal argument, ignoring";_tag_arr(_tmp612,
sizeof(char),_get_zero_arr_size(_tmp612,49));}),_tag_arr(_tmp611,sizeof(void*),0));});
if(_tmp610 != 0)({void*_tmp613[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp614="bad attributes on parameter, ignoring";_tag_arr(_tmp614,sizeof(char),
_get_zero_arr_size(_tmp614,38));}),_tag_arr(_tmp613,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp615=_cycalloc(sizeof(*_tmp615));
_tmp615[0]=({struct Cyc_ParamDecl_tok_struct _tmp616;_tmp616.tag=Cyc_ParamDecl_tok;
_tmp616.f1=({struct _tuple2*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=0;
_tmp617->f2=_tmp60D;_tmp617->f3=_tmp60E;_tmp617;});_tmp616;});_tmp615;});break;}}
case 185: _LL27A: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp618=_cycalloc(
sizeof(*_tmp618));_tmp618[0]=({struct Cyc_IdList_tok_struct _tmp619;_tmp619.tag=
Cyc_IdList_tok;_tmp619.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp619;});_tmp618;});break;case 186: _LL27B: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A[0]=({struct Cyc_IdList_tok_struct
_tmp61B;_tmp61B.tag=Cyc_IdList_tok;_tmp61B.f1=({struct Cyc_List_List*_tmp61C=
_cycalloc(sizeof(*_tmp61C));_tmp61C->hd=({struct _tagged_arr*_tmp61D=_cycalloc(
sizeof(*_tmp61D));_tmp61D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61D;});_tmp61C->tl=0;
_tmp61C;});_tmp61B;});_tmp61A;});break;case 187: _LL27C: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({struct Cyc_IdList_tok_struct
_tmp61F;_tmp61F.tag=Cyc_IdList_tok;_tmp61F.f1=({struct Cyc_List_List*_tmp620=
_cycalloc(sizeof(*_tmp620));_tmp620->hd=({struct _tagged_arr*_tmp621=_cycalloc(
sizeof(*_tmp621));_tmp621[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp621;});_tmp620->tl=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp620;});_tmp61F;});
_tmp61E;});break;case 188: _LL27D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 189: _LL27E: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 190: _LL27F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622[0]=({struct Cyc_Exp_tok_struct _tmp623;
_tmp623.tag=Cyc_Exp_tok;_tmp623.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp625;_tmp625.tag=34;_tmp625.f1=0;_tmp625.f2=0;_tmp625;});_tmp624;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp623;});
_tmp622;});break;case 191: _LL280: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp626=
_cycalloc(sizeof(*_tmp626));_tmp626[0]=({struct Cyc_Exp_tok_struct _tmp627;_tmp627.tag=
Cyc_Exp_tok;_tmp627.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp629;_tmp629.tag=34;_tmp629.f1=0;_tmp629.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp629;});_tmp628;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp627;});_tmp626;});break;case 192: _LL281: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A[0]=({
struct Cyc_Exp_tok_struct _tmp62B;_tmp62B.tag=Cyc_Exp_tok;_tmp62B.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));
_tmp62C[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp62D;_tmp62D.tag=34;
_tmp62D.f1=0;_tmp62D.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp62D;});_tmp62C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp62B;});
_tmp62A;});break;case 193: _LL282: {struct Cyc_Absyn_Vardecl*_tmp62E=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->f1=(void*)0;_tmp634->f2=({
struct _tagged_arr*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp635;});_tmp634;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp62E->tq=({
struct Cyc_Absyn_Tqual _tmp62F;_tmp62F.q_const=1;_tmp62F.q_volatile=0;_tmp62F.q_restrict=
1;_tmp62F;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp630=_cycalloc(sizeof(*
_tmp630));_tmp630[0]=({struct Cyc_Exp_tok_struct _tmp631;_tmp631.tag=Cyc_Exp_tok;
_tmp631.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp633;_tmp633.tag=27;_tmp633.f1=_tmp62E;_tmp633.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp633.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp633.f4=0;
_tmp633;});_tmp632;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp631;});
_tmp630;});break;}case 194: _LL283: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636[0]=({struct Cyc_InitializerList_tok_struct
_tmp637;_tmp637.tag=Cyc_InitializerList_tok;_tmp637.f1=({struct Cyc_List_List*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->hd=({struct _tuple18*_tmp639=
_cycalloc(sizeof(*_tmp639));_tmp639->f1=0;_tmp639->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp639;});_tmp638->tl=0;
_tmp638;});_tmp637;});_tmp636;});break;case 195: _LL284: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A[0]=({struct Cyc_InitializerList_tok_struct
_tmp63B;_tmp63B.tag=Cyc_InitializerList_tok;_tmp63B.f1=({struct Cyc_List_List*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->hd=({struct _tuple18*_tmp63D=
_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp63D->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63D;});_tmp63C->tl=
0;_tmp63C;});_tmp63B;});_tmp63A;});break;case 196: _LL285: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E[0]=({struct Cyc_InitializerList_tok_struct
_tmp63F;_tmp63F.tag=Cyc_InitializerList_tok;_tmp63F.f1=({struct Cyc_List_List*
_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->hd=({struct _tuple18*_tmp641=
_cycalloc(sizeof(*_tmp641));_tmp641->f1=0;_tmp641->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp641;});_tmp640->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp640;});_tmp63F;});
_tmp63E;});break;case 197: _LL286: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_InitializerList_tok_struct
_tmp643;_tmp643.tag=Cyc_InitializerList_tok;_tmp643.f1=({struct Cyc_List_List*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->hd=({struct _tuple18*_tmp645=
_cycalloc(sizeof(*_tmp645));_tmp645->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp645->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp645;});_tmp644->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp644;});_tmp643;});_tmp642;});break;case 198: _LL287: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp646=_cycalloc(sizeof(*_tmp646));
_tmp646[0]=({struct Cyc_DesignatorList_tok_struct _tmp647;_tmp647.tag=Cyc_DesignatorList_tok;
_tmp647.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp647;});_tmp646;});
break;case 199: _LL288: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp648=
_cycalloc(sizeof(*_tmp648));_tmp648[0]=({struct Cyc_DesignatorList_tok_struct
_tmp649;_tmp649.tag=Cyc_DesignatorList_tok;_tmp649.f1=({struct Cyc_List_List*
_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64A->tl=0;_tmp64A;});
_tmp649;});_tmp648;});break;case 200: _LL289: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_DesignatorList_tok_struct
_tmp64C;_tmp64C.tag=Cyc_DesignatorList_tok;_tmp64C.f1=({struct Cyc_List_List*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64D->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64D;});_tmp64C;});
_tmp64B;});break;case 201: _LL28A: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Designator_tok_struct
_tmp64F;_tmp64F.tag=Cyc_Designator_tok;_tmp64F.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp651;_tmp651.tag=0;_tmp651.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp651;});_tmp650;}));
_tmp64F;});_tmp64E;});break;case 202: _LL28B: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_Designator_tok_struct
_tmp653;_tmp653.tag=Cyc_Designator_tok;_tmp653.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_Absyn_FieldName_struct
_tmp655;_tmp655.tag=1;_tmp655.f1=({struct _tagged_arr*_tmp656=_cycalloc(sizeof(*
_tmp656));_tmp656[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp656;});_tmp655;});_tmp654;}));_tmp653;});_tmp652;});
break;case 203: _LL28C: {struct _tuple15 _tmp658;struct Cyc_Absyn_Tqual _tmp659;struct
Cyc_List_List*_tmp65A;struct Cyc_List_List*_tmp65B;struct _tuple15*_tmp657=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp658=*_tmp657;
_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;_tmp65B=_tmp658.f3;{void*_tmp65C=Cyc_Parse_speclist2typ(
_tmp65A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp65B != 0)({void*_tmp65D[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp65E="ignoring attributes in type";
_tag_arr(_tmp65E,sizeof(char),_get_zero_arr_size(_tmp65E,28));}),_tag_arr(
_tmp65D,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_ParamDecl_tok_struct
_tmp660;_tmp660.tag=Cyc_ParamDecl_tok;_tmp660.f1=({struct _tuple2*_tmp661=
_cycalloc(sizeof(*_tmp661));_tmp661->f1=0;_tmp661->f2=_tmp659;_tmp661->f3=
_tmp65C;_tmp661;});_tmp660;});_tmp65F;});break;}}case 204: _LL28D: {struct _tuple15
_tmp663;struct Cyc_Absyn_Tqual _tmp664;struct Cyc_List_List*_tmp665;struct Cyc_List_List*
_tmp666;struct _tuple15*_tmp662=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp663=*_tmp662;_tmp664=
_tmp663.f1;_tmp665=_tmp663.f2;_tmp666=_tmp663.f3;{void*_tmp667=Cyc_Parse_speclist2typ(
_tmp665,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp668=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp669=Cyc_Parse_apply_tms(_tmp664,_tmp667,_tmp666,_tmp668);if(_tmp669.f3 != 0)({
void*_tmp66A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp66B="bad type params, ignoring";_tag_arr(_tmp66B,sizeof(char),
_get_zero_arr_size(_tmp66B,26));}),_tag_arr(_tmp66A,sizeof(void*),0));});if(
_tmp669.f4 != 0)({void*_tmp66C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp66D="bad specifiers, ignoring";_tag_arr(_tmp66D,sizeof(char),
_get_zero_arr_size(_tmp66D,25));}),_tag_arr(_tmp66C,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp66E=_cycalloc(sizeof(*_tmp66E));
_tmp66E[0]=({struct Cyc_ParamDecl_tok_struct _tmp66F;_tmp66F.tag=Cyc_ParamDecl_tok;
_tmp66F.f1=({struct _tuple2*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->f1=0;
_tmp670->f2=_tmp669.f1;_tmp670->f3=_tmp669.f2;_tmp670;});_tmp66F;});_tmp66E;});
break;}}case 205: _LL28E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671[0]=({struct Cyc_Type_tok_struct _tmp672;
_tmp672.tag=Cyc_Type_tok;_tmp672.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp672;});_tmp671;});
break;case 206: _LL28F: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp673=_cycalloc(
sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Type_tok_struct _tmp674;_tmp674.tag=Cyc_Type_tok;
_tmp674.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp675=_cycalloc(
sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_JoinEff_struct _tmp676;_tmp676.tag=
20;_tmp676.f1=0;_tmp676;});_tmp675;}));_tmp674;});_tmp673;});break;case 207:
_LL290: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp677=_cycalloc(sizeof(*
_tmp677));_tmp677[0]=({struct Cyc_Type_tok_struct _tmp678;_tmp678.tag=Cyc_Type_tok;
_tmp678.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp679=_cycalloc(
sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Absyn_JoinEff_struct _tmp67A;_tmp67A.tag=
20;_tmp67A.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp67A;});_tmp679;}));_tmp678;});_tmp677;});break;case 208:
_LL291: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp67B=_cycalloc(sizeof(*
_tmp67B));_tmp67B[0]=({struct Cyc_Type_tok_struct _tmp67C;_tmp67C.tag=Cyc_Type_tok;
_tmp67C.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp67D=_cycalloc(
sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp67E;_tmp67E.tag=
21;_tmp67E.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp67E;});_tmp67D;}));_tmp67C;});_tmp67B;});break;case
209: _LL292: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp67F=_cycalloc(sizeof(*
_tmp67F));_tmp67F[0]=({struct Cyc_Type_tok_struct _tmp680;_tmp680.tag=Cyc_Type_tok;
_tmp680.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp681=_cycalloc(
sizeof(*_tmp681));_tmp681[0]=({struct Cyc_Absyn_JoinEff_struct _tmp682;_tmp682.tag=
20;_tmp682.f1=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp683->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp683;});_tmp682;});_tmp681;}));_tmp680;});_tmp67F;});
break;case 210: _LL293: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_TypeList_tok_struct _tmp685;
_tmp685.tag=Cyc_TypeList_tok;_tmp685.f1=({struct Cyc_List_List*_tmp686=_cycalloc(
sizeof(*_tmp686));_tmp686->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp686->tl=0;_tmp686;});
_tmp685;});_tmp684;});break;case 211: _LL294: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_TypeList_tok_struct
_tmp688;_tmp688.tag=Cyc_TypeList_tok;_tmp688.f1=({struct Cyc_List_List*_tmp689=
_cycalloc(sizeof(*_tmp689));_tmp689->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp689->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp689;});_tmp688;});
_tmp687;});break;case 212: _LL295: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp68B;_tmp68B.tag=Cyc_AbstractDeclarator_tok;_tmp68B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68C;});_tmp68B;});
_tmp68A;});break;case 213: _LL296: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 214: _LL297: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp68E;_tmp68E.tag=Cyc_AbstractDeclarator_tok;_tmp68E.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp68F;});
_tmp68E;});_tmp68D;});break;case 215: _LL298: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 216: _LL299:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp690=_cycalloc(sizeof(*
_tmp690));_tmp690[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp691;_tmp691.tag=
Cyc_AbstractDeclarator_tok;_tmp691.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->tms=({struct Cyc_List_List*_tmp693=
_cycalloc(sizeof(*_tmp693));_tmp693->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp695;_tmp695.tag=0;_tmp695.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp695;});_tmp694;}));
_tmp693->tl=0;_tmp693;});_tmp692;});_tmp691;});_tmp690;});break;case 217: _LL29A:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp696=_cycalloc(sizeof(*
_tmp696));_tmp696[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp697;_tmp697.tag=
Cyc_AbstractDeclarator_tok;_tmp697.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tms=({struct Cyc_List_List*_tmp699=
_cycalloc(sizeof(*_tmp699));_tmp699->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp69B;_tmp69B.tag=0;_tmp69B.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69B;});_tmp69A;}));
_tmp699->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp699;});_tmp698;});
_tmp697;});_tmp696;});break;case 218: _LL29B: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp69D;_tmp69D.tag=Cyc_AbstractDeclarator_tok;_tmp69D.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->tms=({struct Cyc_List_List*_tmp69F=
_cycalloc(sizeof(*_tmp69F));_tmp69F->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6A1;_tmp6A1.tag=1;_tmp6A1.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A1.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A1;});_tmp6A0;}));
_tmp69F->tl=0;_tmp69F;});_tmp69E;});_tmp69D;});_tmp69C;});break;case 219: _LL29C:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6A2=_cycalloc(sizeof(*
_tmp6A2));_tmp6A2[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6A3;_tmp6A3.tag=
Cyc_AbstractDeclarator_tok;_tmp6A3.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->tms=({struct Cyc_List_List*_tmp6A5=
_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6A7;_tmp6A7.tag=1;_tmp6A7.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A7.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A7;});_tmp6A6;}));
_tmp6A5->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6A5;});_tmp6A4;});
_tmp6A3;});_tmp6A2;});break;case 220: _LL29D: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6A9;_tmp6A9.tag=Cyc_AbstractDeclarator_tok;_tmp6A9.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->tms=({struct Cyc_List_List*_tmp6AB=
_cycalloc(sizeof(*_tmp6AB));_tmp6AB->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6AD;_tmp6AD.tag=3;_tmp6AD.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6AF;_tmp6AF.tag=1;_tmp6AF.f1=0;_tmp6AF.f2=0;_tmp6AF.f3=0;_tmp6AF.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AF.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AF;});_tmp6AE;}));
_tmp6AD;});_tmp6AC;}));_tmp6AB->tl=0;_tmp6AB;});_tmp6AA;});_tmp6A9;});_tmp6A8;});
break;case 221: _LL29E: {struct _tuple14 _tmp6B1;struct Cyc_List_List*_tmp6B2;int
_tmp6B3;struct Cyc_Absyn_VarargInfo*_tmp6B4;struct Cyc_Core_Opt*_tmp6B5;struct Cyc_List_List*
_tmp6B6;struct _tuple14*_tmp6B0=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6B1=*_tmp6B0;_tmp6B2=_tmp6B1.f1;_tmp6B3=_tmp6B1.f2;
_tmp6B4=_tmp6B1.f3;_tmp6B5=_tmp6B1.f4;_tmp6B6=_tmp6B1.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6B8;_tmp6B8.tag=Cyc_AbstractDeclarator_tok;
_tmp6B8.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6B9=_cycalloc(sizeof(*
_tmp6B9));_tmp6B9->tms=({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));
_tmp6BA->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6BB=
_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6BC;_tmp6BC.tag=3;_tmp6BC.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6BE;_tmp6BE.tag=1;_tmp6BE.f1=_tmp6B2;_tmp6BE.f2=_tmp6B3;_tmp6BE.f3=_tmp6B4;
_tmp6BE.f4=_tmp6B5;_tmp6BE.f5=_tmp6B6;_tmp6BE;});_tmp6BD;}));_tmp6BC;});_tmp6BB;}));
_tmp6BA->tl=0;_tmp6BA;});_tmp6B9;});_tmp6B8;});_tmp6B7;});break;}case 222: _LL29F:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6BF=_cycalloc(sizeof(*
_tmp6BF));_tmp6BF[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp6C0;_tmp6C0.tag=
Cyc_AbstractDeclarator_tok;_tmp6C0.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->tms=({struct Cyc_List_List*_tmp6C2=
_cycalloc(sizeof(*_tmp6C2));_tmp6C2->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6C4;_tmp6C4.tag=3;_tmp6C4.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6C6;_tmp6C6.tag=1;_tmp6C6.f1=0;_tmp6C6.f2=0;_tmp6C6.f3=0;_tmp6C6.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C6.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C6;});_tmp6C5;}));
_tmp6C4;});_tmp6C3;}));_tmp6C2->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6C2;});_tmp6C1;});
_tmp6C0;});_tmp6BF;});break;case 223: _LL2A0: {struct _tuple14 _tmp6C8;struct Cyc_List_List*
_tmp6C9;int _tmp6CA;struct Cyc_Absyn_VarargInfo*_tmp6CB;struct Cyc_Core_Opt*_tmp6CC;
struct Cyc_List_List*_tmp6CD;struct _tuple14*_tmp6C7=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C8=*_tmp6C7;_tmp6C9=
_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;_tmp6CB=_tmp6C8.f3;_tmp6CC=_tmp6C8.f4;_tmp6CD=
_tmp6C8.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6CF;_tmp6CF.tag=Cyc_AbstractDeclarator_tok;_tmp6CF.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tms=({struct Cyc_List_List*_tmp6D1=
_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D3;_tmp6D3.tag=3;_tmp6D3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D5;_tmp6D5.tag=1;_tmp6D5.f1=_tmp6C9;_tmp6D5.f2=_tmp6CA;_tmp6D5.f3=_tmp6CB;
_tmp6D5.f4=_tmp6CC;_tmp6D5.f5=_tmp6CD;_tmp6D5;});_tmp6D4;}));_tmp6D3;});_tmp6D2;}));
_tmp6D1->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6D1;});_tmp6D0;});
_tmp6CF;});_tmp6CE;});break;}case 224: _LL2A1: {struct Cyc_List_List*_tmp6D6=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp6D8;_tmp6D8.tag=Cyc_AbstractDeclarator_tok;
_tmp6D8.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6D9=_cycalloc(sizeof(*
_tmp6D9));_tmp6D9->tms=({struct Cyc_List_List*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp6DB=
_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6DC;_tmp6DC.tag=4;_tmp6DC.f1=_tmp6D6;_tmp6DC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6DC.f3=0;
_tmp6DC;});_tmp6DB;}));_tmp6DA->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6DA;});_tmp6D9;});
_tmp6D8;});_tmp6D7;});break;}case 225: _LL2A2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp6DE;_tmp6DE.tag=Cyc_AbstractDeclarator_tok;_tmp6DE.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->tms=({struct Cyc_List_List*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6E2;_tmp6E2.tag=5;_tmp6E2.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6E2.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E2;});_tmp6E1;}));
_tmp6E0->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp6E0;});_tmp6DF;});
_tmp6DE;});_tmp6DD;});break;case 226: _LL2A3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 227: _LL2A4: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 228: _LL2A5:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 229:
_LL2A6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
230: _LL2A7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 231: _LL2A8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 232: _LL2A9: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp6E3="`H";
_tag_arr(_tmp6E3,sizeof(char),_get_zero_arr_size(_tmp6E3,3));}))== 0)Cyc_Parse_err(({
const char*_tmp6E4="bad occurrence of heap region `H";_tag_arr(_tmp6E4,sizeof(
char),_get_zero_arr_size(_tmp6E4,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->name=({
struct _tagged_arr*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6F2;});_tmp6EF->identity=
0;_tmp6EF->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6F0=_cycalloc(
sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6F1;_tmp6F1.tag=0;
_tmp6F1.f1=(void*)((void*)3);_tmp6F1;});_tmp6F0;}));_tmp6EF;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({
struct Cyc_Absyn_VarType_struct _tmp6EE;_tmp6EE.tag=1;_tmp6EE.f1=tv;_tmp6EE;});
_tmp6ED;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E5=_cycalloc(sizeof(*
_tmp6E5));_tmp6E5[0]=({struct Cyc_Stmt_tok_struct _tmp6E6;_tmp6E6.tag=Cyc_Stmt_tok;
_tmp6E6.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp6E7=
_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Absyn_Region_s_struct _tmp6E8;
_tmp6E8.tag=18;_tmp6E8.f1=tv;_tmp6E8.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->f1=(void*)0;_tmp6E9->f2=({struct
_tagged_arr*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EA;});_tmp6E9;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));
_tmp6EB[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6EC;_tmp6EC.tag=15;_tmp6EC.f1=(
void*)t;_tmp6EC;});_tmp6EB;}),0);_tmp6E8.f3=0;_tmp6E8.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E8;});_tmp6E7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6E6;});
_tmp6E5;});break;}case 233: _LL2AA: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp6F3="H";
_tag_arr(_tmp6F3,sizeof(char),_get_zero_arr_size(_tmp6F3,2));}))== 0)Cyc_Parse_err(({
const char*_tmp6F4="bad occurrence of heap region `H";_tag_arr(_tmp6F4,sizeof(
char),_get_zero_arr_size(_tmp6F4,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF->name=({
struct _tagged_arr*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp705;_tmp705.tag=0;_tmp705.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp703[1]={&
_tmp705};Cyc_aprintf(({const char*_tmp704="`%s";_tag_arr(_tmp704,sizeof(char),
_get_zero_arr_size(_tmp704,4));}),_tag_arr(_tmp703,sizeof(void*),1));}});_tmp702;});
_tmp6FF->identity=0;_tmp6FF->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp701;_tmp701.tag=0;_tmp701.f1=(void*)((void*)3);_tmp701;});_tmp700;}));
_tmp6FF;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6FD=_cycalloc(
sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_VarType_struct _tmp6FE;_tmp6FE.tag=
1;_tmp6FE.f1=tv;_tmp6FE;});_tmp6FD;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Stmt_tok_struct
_tmp6F6;_tmp6F6.tag=Cyc_Stmt_tok;_tmp6F6.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({
struct Cyc_Absyn_Region_s_struct _tmp6F8;_tmp6F8.tag=18;_tmp6F8.f1=tv;_tmp6F8.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));
_tmp6F9->f1=(void*)0;_tmp6F9->f2=({struct _tagged_arr*_tmp6FA=_cycalloc(sizeof(*
_tmp6FA));_tmp6FA[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6FA;});_tmp6F9;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp6FC;_tmp6FC.tag=15;_tmp6FC.f1=(void*)t;_tmp6FC;});_tmp6FB;}),0);_tmp6F8.f3=0;
_tmp6F8.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6F8;});_tmp6F7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F6;});
_tmp6F5;});break;}case 234: _LL2AB: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp706="resetable";
_tag_arr(_tmp706,sizeof(char),_get_zero_arr_size(_tmp706,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp707="expecting [resetable]";_tag_arr(_tmp707,sizeof(char),
_get_zero_arr_size(_tmp707,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp708="`H";_tag_arr(_tmp708,sizeof(char),
_get_zero_arr_size(_tmp708,3));}))== 0)Cyc_Parse_err(({const char*_tmp709="bad occurrence of heap region `H";
_tag_arr(_tmp709,sizeof(char),_get_zero_arr_size(_tmp709,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714->name=({
struct _tagged_arr*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp717;});_tmp714->identity=
0;_tmp714->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp715=_cycalloc(
sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp716;_tmp716.tag=0;
_tmp716.f1=(void*)((void*)3);_tmp716;});_tmp715;}));_tmp714;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({
struct Cyc_Absyn_VarType_struct _tmp713;_tmp713.tag=1;_tmp713.f1=tv;_tmp713;});
_tmp712;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70A=_cycalloc(sizeof(*
_tmp70A));_tmp70A[0]=({struct Cyc_Stmt_tok_struct _tmp70B;_tmp70B.tag=Cyc_Stmt_tok;
_tmp70B.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_Region_s_struct _tmp70D;
_tmp70D.tag=18;_tmp70D.f1=tv;_tmp70D.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->f1=(void*)0;_tmp70E->f2=({struct
_tagged_arr*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70F;});_tmp70E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp710=_cycalloc(sizeof(*_tmp710));
_tmp710[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp711;_tmp711.tag=15;_tmp711.f1=(
void*)t;_tmp711;});_tmp710;}),0);_tmp70D.f3=1;_tmp70D.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70D;});_tmp70C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp70B;});
_tmp70A;});break;}case 235: _LL2AC: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp718="resetable";
_tag_arr(_tmp718,sizeof(char),_get_zero_arr_size(_tmp718,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp719="expecting `resetable'";_tag_arr(_tmp719,sizeof(char),
_get_zero_arr_size(_tmp719,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp71A="H";_tag_arr(_tmp71A,sizeof(char),
_get_zero_arr_size(_tmp71A,2));}))== 0)Cyc_Parse_err(({const char*_tmp71B="bad occurrence of heap region `H";
_tag_arr(_tmp71B,sizeof(char),_get_zero_arr_size(_tmp71B,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726->name=({
struct _tagged_arr*_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp72C;_tmp72C.tag=0;_tmp72C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp72A[1]={&
_tmp72C};Cyc_aprintf(({const char*_tmp72B="`%s";_tag_arr(_tmp72B,sizeof(char),
_get_zero_arr_size(_tmp72B,4));}),_tag_arr(_tmp72A,sizeof(void*),1));}});_tmp729;});
_tmp726->identity=0;_tmp726->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp728;_tmp728.tag=0;_tmp728.f1=(void*)((void*)3);_tmp728;});_tmp727;}));
_tmp726;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp724=_cycalloc(
sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_VarType_struct _tmp725;_tmp725.tag=
1;_tmp725.f1=tv;_tmp725;});_tmp724;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Stmt_tok_struct
_tmp71D;_tmp71D.tag=Cyc_Stmt_tok;_tmp71D.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({
struct Cyc_Absyn_Region_s_struct _tmp71F;_tmp71F.tag=18;_tmp71F.f1=tv;_tmp71F.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp720=_cycalloc(sizeof(*_tmp720));
_tmp720->f1=(void*)0;_tmp720->f2=({struct _tagged_arr*_tmp721=_cycalloc(sizeof(*
_tmp721));_tmp721[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp721;});_tmp720;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp723;_tmp723.tag=15;_tmp723.f1=(void*)t;_tmp723;});_tmp722;}),0);_tmp71F.f3=1;
_tmp71F.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp71F;});_tmp71E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp71D;});
_tmp71C;});break;}case 236: _LL2AD: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({struct Cyc_Stmt_tok_struct
_tmp72E;_tmp72E.tag=Cyc_Stmt_tok;_tmp72E.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp730;_tmp730.tag=20;_tmp730.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp730;});_tmp72F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp72E;});_tmp72D;});break;case 237: _LL2AE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731[0]=({
struct Cyc_Stmt_tok_struct _tmp732;_tmp732.tag=Cyc_Stmt_tok;_tmp732.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp734;_tmp734.tag=13;_tmp734.f1=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp734;});_tmp733;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp732;});_tmp731;});break;case 238: _LL2AF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735[0]=({
struct Cyc_Stmt_tok_struct _tmp736;_tmp736.tag=Cyc_Stmt_tok;_tmp736.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp737=_cycalloc(sizeof(*_tmp737));
_tmp737[0]=({struct Cyc_Absyn_Splice_s_struct _tmp738;_tmp738.tag=14;_tmp738.f1=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp738;});_tmp737;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp736;});
_tmp735;});break;case 239: _LL2B0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp739=
_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Stmt_tok_struct _tmp73A;
_tmp73A.tag=Cyc_Stmt_tok;_tmp73A.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Absyn_Label_s_struct
_tmp73C;_tmp73C.tag=15;_tmp73C.f1=({struct _tagged_arr*_tmp73D=_cycalloc(sizeof(*
_tmp73D));_tmp73D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp73D;});_tmp73C.f2=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp73C;});_tmp73B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73A;});
_tmp739;});break;case 240: _LL2B1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp73E=
_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=({struct Cyc_Stmt_tok_struct _tmp73F;
_tmp73F.tag=Cyc_Stmt_tok;_tmp73F.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73F;});
_tmp73E;});break;case 241: _LL2B2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp740=
_cycalloc(sizeof(*_tmp740));_tmp740[0]=({struct Cyc_Stmt_tok_struct _tmp741;
_tmp741.tag=Cyc_Stmt_tok;_tmp741.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp741;});
_tmp740;});break;case 242: _LL2B3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp742=
_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Stmt_tok_struct _tmp743;
_tmp743.tag=Cyc_Stmt_tok;_tmp743.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp743;});
_tmp742;});break;case 243: _LL2B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 244: _LL2B5: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=({struct Cyc_Stmt_tok_struct
_tmp745;_tmp745.tag=Cyc_Stmt_tok;_tmp745.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp745;});_tmp744;});break;case 245: _LL2B6: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746[0]=({struct Cyc_Stmt_tok_struct
_tmp747;_tmp747.tag=Cyc_Stmt_tok;_tmp747.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp747;});_tmp746;});
break;case 246: _LL2B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 247: _LL2B8: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Stmt_tok_struct
_tmp749;_tmp749.tag=Cyc_Stmt_tok;_tmp749.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp749;});
_tmp748;});break;case 248: _LL2B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp74A=
_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Stmt_tok_struct _tmp74B;
_tmp74B.tag=Cyc_Stmt_tok;_tmp74B.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp74D;_tmp74D.tag=1;_tmp74D.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74D;});_tmp74C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp74B;});_tmp74A;});break;
case 249: _LL2BA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp74E=_cycalloc(
sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_Stmt_tok_struct _tmp74F;_tmp74F.tag=Cyc_Stmt_tok;
_tmp74F.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp751;_tmp751.tag=1;_tmp751.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp751;});_tmp750;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp74F;});_tmp74E;});
break;case 250: _LL2BB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp752=_cycalloc(
sizeof(*_tmp752));_tmp752[0]=({struct Cyc_Stmt_tok_struct _tmp753;_tmp753.tag=Cyc_Stmt_tok;
_tmp753.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp753;});
_tmp752;});break;case 251: _LL2BC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp754=
_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Stmt_tok_struct _tmp755;
_tmp755.tag=Cyc_Stmt_tok;_tmp755.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp755;});
_tmp754;});break;case 252: _LL2BD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp756=
_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Stmt_tok_struct _tmp757;
_tmp757.tag=Cyc_Stmt_tok;_tmp757.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp757;});
_tmp756;});break;case 253: _LL2BE: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp758="C";
_tag_arr(_tmp758,sizeof(char),_get_zero_arr_size(_tmp758,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp759="only switch \"C\" { ... } is allowed";_tag_arr(_tmp759,
sizeof(char),_get_zero_arr_size(_tmp759,35));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A[0]=({
struct Cyc_Stmt_tok_struct _tmp75B;_tmp75B.tag=Cyc_Stmt_tok;_tmp75B.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_SwitchC_s_struct*_tmp75C=_cycalloc(sizeof(*_tmp75C));
_tmp75C[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp75D;_tmp75D.tag=10;_tmp75D.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp75D.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp75D;});_tmp75C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75B;});
_tmp75A;});break;case 254: _LL2BF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Stmt_tok_struct _tmp75F;
_tmp75F.tag=Cyc_Stmt_tok;_tmp75F.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75F;});
_tmp75E;});break;case 255: _LL2C0: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp761;_tmp761.tag=Cyc_SwitchClauseList_tok;_tmp761.f1=0;_tmp761;});_tmp760;});
break;case 256: _LL2C1: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp763;_tmp763.tag=Cyc_SwitchClauseList_tok;_tmp763.f1=({struct Cyc_List_List*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->hd=({struct Cyc_Absyn_Switch_clause*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp765->pat_vars=0;_tmp765->where_clause=0;
_tmp765->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp765->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp765;});
_tmp764->tl=0;_tmp764;});_tmp763;});_tmp762;});break;case 257: _LL2C2: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp766=_cycalloc(sizeof(*_tmp766));
_tmp766[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp767;_tmp767.tag=Cyc_SwitchClauseList_tok;
_tmp767.f1=({struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=({
struct Cyc_Absyn_Switch_clause*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp769->pat_vars=0;_tmp769->where_clause=0;_tmp769->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp769->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp769;});
_tmp768->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp768;});_tmp767;});_tmp766;});break;case 258: _LL2C3:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp76A=_cycalloc(sizeof(*
_tmp76A));_tmp76A[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp76B;_tmp76B.tag=
Cyc_SwitchClauseList_tok;_tmp76B.f1=({struct Cyc_List_List*_tmp76C=_cycalloc(
sizeof(*_tmp76C));_tmp76C->hd=({struct Cyc_Absyn_Switch_clause*_tmp76D=_cycalloc(
sizeof(*_tmp76D));_tmp76D->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp76D->pat_vars=0;
_tmp76D->where_clause=0;_tmp76D->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp76D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp76D;});
_tmp76C->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp76C;});_tmp76B;});_tmp76A;});break;case 259: _LL2C4:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp76E=_cycalloc(sizeof(*
_tmp76E));_tmp76E[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp76F;_tmp76F.tag=
Cyc_SwitchClauseList_tok;_tmp76F.f1=({struct Cyc_List_List*_tmp770=_cycalloc(
sizeof(*_tmp770));_tmp770->hd=({struct Cyc_Absyn_Switch_clause*_tmp771=_cycalloc(
sizeof(*_tmp771));_tmp771->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp771->pat_vars=0;
_tmp771->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp771->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp771->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp771;});
_tmp770->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp770;});_tmp76F;});_tmp76E;});break;case 260: _LL2C5:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp772=_cycalloc(sizeof(*
_tmp772));_tmp772[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp773;_tmp773.tag=
Cyc_SwitchClauseList_tok;_tmp773.f1=({struct Cyc_List_List*_tmp774=_cycalloc(
sizeof(*_tmp774));_tmp774->hd=({struct Cyc_Absyn_Switch_clause*_tmp775=_cycalloc(
sizeof(*_tmp775));_tmp775->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp775->pat_vars=0;
_tmp775->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp775->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp775->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp775;});
_tmp774->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp774;});_tmp773;});_tmp772;});break;case 261: _LL2C6:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp776=_cycalloc(sizeof(*
_tmp776));_tmp776[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp777;_tmp777.tag=
Cyc_SwitchCClauseList_tok;_tmp777.f1=0;_tmp777;});_tmp776;});break;case 262:
_LL2C7: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp778=_cycalloc(
sizeof(*_tmp778));_tmp778[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp779;
_tmp779.tag=Cyc_SwitchCClauseList_tok;_tmp779.f1=({struct Cyc_List_List*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp77B=
_cycalloc(sizeof(*_tmp77B));_tmp77B->cnst_exp=0;_tmp77B->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_break_stmt(
0),0);_tmp77B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77B;});
_tmp77A->tl=0;_tmp77A;});_tmp779;});_tmp778;});break;case 263: _LL2C8: yyval=(void*)({
struct Cyc_SwitchCClauseList_tok_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));
_tmp77C[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp77D;_tmp77D.tag=Cyc_SwitchCClauseList_tok;
_tmp77D.f1=({struct Cyc_List_List*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp77F->body=Cyc_Absyn_fallthru_stmt(0,0);_tmp77F->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77F;});
_tmp77E->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp77E;});_tmp77D;});_tmp77C;});break;case 264: _LL2C9:
yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp780=_cycalloc(sizeof(*
_tmp780));_tmp780[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp781;_tmp781.tag=
Cyc_SwitchCClauseList_tok;_tmp781.f1=({struct Cyc_List_List*_tmp782=_cycalloc(
sizeof(*_tmp782));_tmp782->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp783=_cycalloc(
sizeof(*_tmp783));_tmp783->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp783->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Absyn_fallthru_stmt(0,0),0);_tmp783->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp783;});
_tmp782->tl=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp782;});_tmp781;});_tmp780;});break;case 265: _LL2CA:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp784=_cycalloc(sizeof(*_tmp784));
_tmp784[0]=({struct Cyc_Stmt_tok_struct _tmp785;_tmp785.tag=Cyc_Stmt_tok;_tmp785.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp785;});_tmp784;});break;case 266: _LL2CB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786[0]=({
struct Cyc_Stmt_tok_struct _tmp787;_tmp787.tag=Cyc_Stmt_tok;_tmp787.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp787;});_tmp786;});break;case 267: _LL2CC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788[0]=({
struct Cyc_Stmt_tok_struct _tmp789;_tmp789.tag=Cyc_Stmt_tok;_tmp789.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp789;});
_tmp788;});break;case 268: _LL2CD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78A=
_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({struct Cyc_Stmt_tok_struct _tmp78B;
_tmp78B.tag=Cyc_Stmt_tok;_tmp78B.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78B;});
_tmp78A;});break;case 269: _LL2CE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78C=
_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Stmt_tok_struct _tmp78D;
_tmp78D.tag=Cyc_Stmt_tok;_tmp78D.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78D;});
_tmp78C;});break;case 270: _LL2CF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp78E=
_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({struct Cyc_Stmt_tok_struct _tmp78F;
_tmp78F.tag=Cyc_Stmt_tok;_tmp78F.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78F;});
_tmp78E;});break;case 271: _LL2D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp790=
_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Stmt_tok_struct _tmp791;
_tmp791.tag=Cyc_Stmt_tok;_tmp791.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp791;});
_tmp790;});break;case 272: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Stmt_tok_struct _tmp793;
_tmp793.tag=Cyc_Stmt_tok;_tmp793.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp793;});
_tmp792;});break;case 273: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp794=
_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Stmt_tok_struct _tmp795;
_tmp795.tag=Cyc_Stmt_tok;_tmp795.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp795;});
_tmp794;});break;case 274: _LL2D3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp796=
_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Stmt_tok_struct _tmp797;
_tmp797.tag=Cyc_Stmt_tok;_tmp797.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp797;});
_tmp796;});break;case 275: _LL2D4: {struct Cyc_List_List*_tmp798=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp799=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp79A=
_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Stmt_tok_struct _tmp79B;
_tmp79B.tag=Cyc_Stmt_tok;_tmp79B.f1=Cyc_Parse_flatten_declarations(_tmp798,
_tmp799);_tmp79B;});_tmp79A;});break;}case 276: _LL2D5: {struct Cyc_List_List*
_tmp79C=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp79D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=({
struct Cyc_Stmt_tok_struct _tmp79F;_tmp79F.tag=Cyc_Stmt_tok;_tmp79F.f1=Cyc_Parse_flatten_declarations(
_tmp79C,_tmp79D);_tmp79F;});_tmp79E;});break;}case 277: _LL2D6: {struct Cyc_List_List*
_tmp7A0=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp7A1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({
struct Cyc_Stmt_tok_struct _tmp7A3;_tmp7A3.tag=Cyc_Stmt_tok;_tmp7A3.f1=Cyc_Parse_flatten_declarations(
_tmp7A0,_tmp7A1);_tmp7A3;});_tmp7A2;});break;}case 278: _LL2D7: {struct Cyc_List_List*
_tmp7A4=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp7A5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({
struct Cyc_Stmt_tok_struct _tmp7A7;_tmp7A7.tag=Cyc_Stmt_tok;_tmp7A7.f1=Cyc_Parse_flatten_declarations(
_tmp7A4,_tmp7A5);_tmp7A7;});_tmp7A6;});break;}case 279: _LL2D8: {struct Cyc_List_List*
_tmp7A8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({
struct Cyc_Stmt_tok_struct _tmp7AA;_tmp7AA.tag=Cyc_Stmt_tok;_tmp7AA.f1=Cyc_Absyn_forarray_stmt(
_tmp7A8,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7AA;});_tmp7A9;});break;}case 280: _LL2D9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({
struct Cyc_Stmt_tok_struct _tmp7AC;_tmp7AC.tag=Cyc_Stmt_tok;_tmp7AC.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7AC;});
_tmp7AB;});break;case 281: _LL2DA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AE=
_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Stmt_tok_struct _tmp7AF;
_tmp7AF.tag=Cyc_Stmt_tok;_tmp7AF.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7AF;});
_tmp7AE;});break;case 282: _LL2DB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B0=
_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_Stmt_tok_struct _tmp7B1;
_tmp7B1.tag=Cyc_Stmt_tok;_tmp7B1.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7B1;});
_tmp7B0;});break;case 283: _LL2DC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B2=
_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Stmt_tok_struct _tmp7B3;
_tmp7B3.tag=Cyc_Stmt_tok;_tmp7B3.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7B3;});
_tmp7B2;});break;case 284: _LL2DD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B4=
_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Stmt_tok_struct _tmp7B5;
_tmp7B5.tag=Cyc_Stmt_tok;_tmp7B5.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7B5;});_tmp7B4;});break;case 285: _LL2DE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({
struct Cyc_Stmt_tok_struct _tmp7B7;_tmp7B7.tag=Cyc_Stmt_tok;_tmp7B7.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp7B7;});_tmp7B6;});break;case 286: _LL2DF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8[0]=({
struct Cyc_Stmt_tok_struct _tmp7B9;_tmp7B9.tag=Cyc_Stmt_tok;_tmp7B9.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp7B9;});_tmp7B8;});break;case 287: _LL2E0: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=({
struct Cyc_Stmt_tok_struct _tmp7BB;_tmp7BB.tag=Cyc_Stmt_tok;_tmp7BB.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp7BB;});_tmp7BA;});break;case 288: _LL2E1: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC[
0]=({struct Cyc_Pattern_tok_struct _tmp7BD;_tmp7BD.tag=Cyc_Pattern_tok;_tmp7BD.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BD;});
_tmp7BC;});break;case 289: _LL2E2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 290: _LL2E3: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Pattern_tok_struct
_tmp7BF;_tmp7BF.tag=Cyc_Pattern_tok;_tmp7BF.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0[0]=({struct
Cyc_Absyn_Int_p_struct _tmp7C1;_tmp7C1.tag=1;_tmp7C1.f1=(void*)(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp7C1.f2=(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp7C1;});_tmp7C0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7BF;});_tmp7BE;});break;case 291: _LL2E4: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2[
0]=({struct Cyc_Pattern_tok_struct _tmp7C3;_tmp7C3.tag=Cyc_Pattern_tok;_tmp7C3.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp7C4=_cycalloc(
sizeof(*_tmp7C4));_tmp7C4[0]=({struct Cyc_Absyn_Int_p_struct _tmp7C5;_tmp7C5.tag=1;
_tmp7C5.f1=(void*)((void*)0);_tmp7C5.f2=-(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp7C5;});_tmp7C4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7C3;});_tmp7C2;});break;case 292: _LL2E5: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[
0]=({struct Cyc_Pattern_tok_struct _tmp7C7;_tmp7C7.tag=Cyc_Pattern_tok;_tmp7C7.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp7C8=_cycalloc(
sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Absyn_Float_p_struct _tmp7C9;_tmp7C9.tag=
3;_tmp7C9.f1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7C9;});_tmp7C8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C7;});
_tmp7C6;});break;case 293: _LL2E6: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({struct Cyc_Pattern_tok_struct
_tmp7CB;_tmp7CB.tag=Cyc_Pattern_tok;_tmp7CB.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp7CC=_cycalloc_atomic(sizeof(*_tmp7CC));_tmp7CC[0]=({
struct Cyc_Absyn_Char_p_struct _tmp7CD;_tmp7CD.tag=2;_tmp7CD.f1=Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7CD;});_tmp7CC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7CB;});_tmp7CA;});break;case 294: _LL2E7: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE[
0]=({struct Cyc_Pattern_tok_struct _tmp7CF;_tmp7CF.tag=Cyc_Pattern_tok;_tmp7CF.f1=
Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CF;});
_tmp7CE;});break;case 295: _LL2E8: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Pattern_tok_struct
_tmp7D1;_tmp7D1.tag=Cyc_Pattern_tok;_tmp7D1.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp7D3;_tmp7D3.tag=11;_tmp7D3.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7D3;});_tmp7D2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7D1;});_tmp7D0;});break;case 296: _LL2E9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4[
0]=({struct Cyc_Pattern_tok_struct _tmp7D5;_tmp7D5.tag=Cyc_Pattern_tok;_tmp7D5.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7D6=_cycalloc(
sizeof(*_tmp7D6));_tmp7D6[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7D7;_tmp7D7.tag=
4;_tmp7D7.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]));_tmp7D7;});_tmp7D6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D5;});
_tmp7D4;});break;case 297: _LL2EA: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8[0]=({struct Cyc_Pattern_tok_struct
_tmp7D9;_tmp7D9.tag=Cyc_Pattern_tok;_tmp7D9.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownCall_p_struct*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp7DB;_tmp7DB.tag=12;_tmp7DB.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7DB.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7DB;});_tmp7DA;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7D9;});_tmp7D8;});break;case 298: _LL2EB: {struct
Cyc_List_List*_tmp7DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({
struct Cyc_Pattern_tok_struct _tmp7DE;_tmp7DE.tag=Cyc_Pattern_tok;_tmp7DE.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));
_tmp7DF[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7E0;_tmp7E0.tag=7;_tmp7E0.f1=({
struct Cyc_Absyn_AggrInfo _tmp7E1;_tmp7E1.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp7E3;_tmp7E3.tag=0;_tmp7E3.f1=(void*)((void*)0);_tmp7E3.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7E3;});_tmp7E2;}));
_tmp7E1.targs=0;_tmp7E1;});_tmp7E0.f2=_tmp7DC;_tmp7E0.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7E0;});_tmp7DF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7DE;});_tmp7DD;});break;}case 299: _LL2EC: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[
0]=({struct Cyc_Pattern_tok_struct _tmp7E5;_tmp7E5.tag=Cyc_Pattern_tok;_tmp7E5.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E6=_cycalloc(
sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E7;_tmp7E7.tag=
5;_tmp7E7.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
_tmp7E4;});break;case 300: _LL2ED: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Pattern_tok_struct
_tmp7E9;_tmp7E9.tag=Cyc_Pattern_tok;_tmp7E9.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp7EB;_tmp7EB.tag=6;_tmp7EB.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC->f1=(void*)0;_tmp7EC->f2=({
struct _tagged_arr*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7ED;});_tmp7EC;}),(
void*)0,0);_tmp7EB;});_tmp7EA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E9;});
_tmp7E8;});break;case 301: _LL2EE: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=({struct Cyc_PatternList_tok_struct
_tmp7EF;_tmp7EF.tag=Cyc_PatternList_tok;_tmp7EF.f1=({struct Cyc_List_List*_tmp7F0=
_cycalloc(sizeof(*_tmp7F0));_tmp7F0->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F0->tl=0;_tmp7F0;});
_tmp7EF;});_tmp7EE;});break;case 302: _LL2EF: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({struct Cyc_PatternList_tok_struct
_tmp7F2;_tmp7F2.tag=Cyc_PatternList_tok;_tmp7F2.f1=({struct Cyc_List_List*_tmp7F3=
_cycalloc(sizeof(*_tmp7F3));_tmp7F3->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F3->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F3;});_tmp7F2;});
_tmp7F1;});break;case 303: _LL2F0: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_FieldPattern_tok_struct
_tmp7F5;_tmp7F5.tag=Cyc_FieldPattern_tok;_tmp7F5.f1=({struct _tuple11*_tmp7F6=
_cycalloc(sizeof(*_tmp7F6));_tmp7F6->f1=0;_tmp7F6->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F6;});_tmp7F5;});
_tmp7F4;});break;case 304: _LL2F1: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_FieldPattern_tok_struct
_tmp7F8;_tmp7F8.tag=Cyc_FieldPattern_tok;_tmp7F8.f1=({struct _tuple11*_tmp7F9=
_cycalloc(sizeof(*_tmp7F9));_tmp7F9->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F9->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F9;});_tmp7F8;});
_tmp7F7;});break;case 305: _LL2F2: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp7FB;_tmp7FB.tag=Cyc_FieldPatternList_tok;_tmp7FB.f1=({struct Cyc_List_List*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7FC->tl=0;_tmp7FC;});
_tmp7FB;});_tmp7FA;});break;case 306: _LL2F3: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp7FE;_tmp7FE.tag=Cyc_FieldPatternList_tok;_tmp7FE.f1=({struct Cyc_List_List*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7FF->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7FF;});_tmp7FE;});
_tmp7FD;});break;case 307: _LL2F4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 308: _LL2F5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=({struct Cyc_Exp_tok_struct _tmp801;
_tmp801.tag=Cyc_Exp_tok;_tmp801.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp801;});
_tmp800;});break;case 309: _LL2F6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 310: _LL2F7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Exp_tok_struct _tmp803;
_tmp803.tag=Cyc_Exp_tok;_tmp803.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp803;});
_tmp802;});break;case 311: _LL2F8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Primopopt_tok_struct
_tmp805;_tmp805.tag=Cyc_Primopopt_tok;_tmp805.f1=0;_tmp805;});_tmp804;});break;
case 312: _LL2F9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp806=_cycalloc(
sizeof(*_tmp806));_tmp806[0]=({struct Cyc_Primopopt_tok_struct _tmp807;_tmp807.tag=
Cyc_Primopopt_tok;_tmp807.f1=({struct Cyc_Core_Opt*_tmp808=_cycalloc(sizeof(*
_tmp808));_tmp808->v=(void*)((void*)1);_tmp808;});_tmp807;});_tmp806;});break;
case 313: _LL2FA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp809=_cycalloc(
sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Primopopt_tok_struct _tmp80A;_tmp80A.tag=
Cyc_Primopopt_tok;_tmp80A.f1=({struct Cyc_Core_Opt*_tmp80B=_cycalloc(sizeof(*
_tmp80B));_tmp80B->v=(void*)((void*)3);_tmp80B;});_tmp80A;});_tmp809;});break;
case 314: _LL2FB: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp80C=_cycalloc(
sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Primopopt_tok_struct _tmp80D;_tmp80D.tag=
Cyc_Primopopt_tok;_tmp80D.f1=({struct Cyc_Core_Opt*_tmp80E=_cycalloc(sizeof(*
_tmp80E));_tmp80E->v=(void*)((void*)4);_tmp80E;});_tmp80D;});_tmp80C;});break;
case 315: _LL2FC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp80F=_cycalloc(
sizeof(*_tmp80F));_tmp80F[0]=({struct Cyc_Primopopt_tok_struct _tmp810;_tmp810.tag=
Cyc_Primopopt_tok;_tmp810.f1=({struct Cyc_Core_Opt*_tmp811=_cycalloc(sizeof(*
_tmp811));_tmp811->v=(void*)((void*)0);_tmp811;});_tmp810;});_tmp80F;});break;
case 316: _LL2FD: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp812=_cycalloc(
sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Primopopt_tok_struct _tmp813;_tmp813.tag=
Cyc_Primopopt_tok;_tmp813.f1=({struct Cyc_Core_Opt*_tmp814=_cycalloc(sizeof(*
_tmp814));_tmp814->v=(void*)((void*)2);_tmp814;});_tmp813;});_tmp812;});break;
case 317: _LL2FE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp815=_cycalloc(
sizeof(*_tmp815));_tmp815[0]=({struct Cyc_Primopopt_tok_struct _tmp816;_tmp816.tag=
Cyc_Primopopt_tok;_tmp816.f1=({struct Cyc_Core_Opt*_tmp817=_cycalloc(sizeof(*
_tmp817));_tmp817->v=(void*)((void*)16);_tmp817;});_tmp816;});_tmp815;});break;
case 318: _LL2FF: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp818=_cycalloc(
sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Primopopt_tok_struct _tmp819;_tmp819.tag=
Cyc_Primopopt_tok;_tmp819.f1=({struct Cyc_Core_Opt*_tmp81A=_cycalloc(sizeof(*
_tmp81A));_tmp81A->v=(void*)((void*)17);_tmp81A;});_tmp819;});_tmp818;});break;
case 319: _LL300: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp81B=_cycalloc(
sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Primopopt_tok_struct _tmp81C;_tmp81C.tag=
Cyc_Primopopt_tok;_tmp81C.f1=({struct Cyc_Core_Opt*_tmp81D=_cycalloc(sizeof(*
_tmp81D));_tmp81D->v=(void*)((void*)13);_tmp81D;});_tmp81C;});_tmp81B;});break;
case 320: _LL301: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp81E=_cycalloc(
sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_Primopopt_tok_struct _tmp81F;_tmp81F.tag=
Cyc_Primopopt_tok;_tmp81F.f1=({struct Cyc_Core_Opt*_tmp820=_cycalloc(sizeof(*
_tmp820));_tmp820->v=(void*)((void*)15);_tmp820;});_tmp81F;});_tmp81E;});break;
case 321: _LL302: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp821=_cycalloc(
sizeof(*_tmp821));_tmp821[0]=({struct Cyc_Primopopt_tok_struct _tmp822;_tmp822.tag=
Cyc_Primopopt_tok;_tmp822.f1=({struct Cyc_Core_Opt*_tmp823=_cycalloc(sizeof(*
_tmp823));_tmp823->v=(void*)((void*)14);_tmp823;});_tmp822;});_tmp821;});break;
case 322: _LL303: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 323: _LL304: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp824=_cycalloc(
sizeof(*_tmp824));_tmp824[0]=({struct Cyc_Exp_tok_struct _tmp825;_tmp825.tag=Cyc_Exp_tok;
_tmp825.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp825;});
_tmp824;});break;case 324: _LL305: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Exp_tok_struct _tmp827;_tmp827.tag=
Cyc_Exp_tok;_tmp827.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp827;});
_tmp826;});break;case 325: _LL306: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp828=
_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Exp_tok_struct _tmp829;_tmp829.tag=
Cyc_Exp_tok;_tmp829.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp829;});
_tmp828;});break;case 326: _LL307: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82A=
_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({struct Cyc_Exp_tok_struct _tmp82B;_tmp82B.tag=
Cyc_Exp_tok;_tmp82B.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82B;});
_tmp82A;});break;case 327: _LL308: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82C=
_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Exp_tok_struct _tmp82D;_tmp82D.tag=
Cyc_Exp_tok;_tmp82D.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82D;});
_tmp82C;});break;case 328: _LL309: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82E=
_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_Exp_tok_struct _tmp82F;_tmp82F.tag=
Cyc_Exp_tok;_tmp82F.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82F;});
_tmp82E;});break;case 329: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 330: _LL30B: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 331: _LL30C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830[0]=({struct Cyc_Exp_tok_struct _tmp831;
_tmp831.tag=Cyc_Exp_tok;_tmp831.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp831;});
_tmp830;});break;case 332: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL30E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Exp_tok_struct _tmp833;
_tmp833.tag=Cyc_Exp_tok;_tmp833.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp833;});
_tmp832;});break;case 334: _LL30F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL310: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Exp_tok_struct _tmp835;
_tmp835.tag=Cyc_Exp_tok;_tmp835.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp835;});
_tmp834;});break;case 336: _LL311: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL312: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Exp_tok_struct _tmp837;
_tmp837.tag=Cyc_Exp_tok;_tmp837.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp837;});
_tmp836;});break;case 338: _LL313: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 339: _LL314: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Exp_tok_struct _tmp839;
_tmp839.tag=Cyc_Exp_tok;_tmp839.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp839;});
_tmp838;});break;case 340: _LL315: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL316: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Exp_tok_struct _tmp83B;
_tmp83B.tag=Cyc_Exp_tok;_tmp83B.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83B;});
_tmp83A;});break;case 342: _LL317: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83C=
_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Exp_tok_struct _tmp83D;_tmp83D.tag=
Cyc_Exp_tok;_tmp83D.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
_tmp83C;});break;case 343: _LL318: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 344: _LL319: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Exp_tok_struct _tmp83F;
_tmp83F.tag=Cyc_Exp_tok;_tmp83F.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
_tmp83E;});break;case 345: _LL31A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp840=
_cycalloc(sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Exp_tok_struct _tmp841;_tmp841.tag=
Cyc_Exp_tok;_tmp841.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
_tmp840;});break;case 346: _LL31B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp842=
_cycalloc(sizeof(*_tmp842));_tmp842[0]=({struct Cyc_Exp_tok_struct _tmp843;_tmp843.tag=
Cyc_Exp_tok;_tmp843.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
_tmp842;});break;case 347: _LL31C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp844=
_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Exp_tok_struct _tmp845;_tmp845.tag=
Cyc_Exp_tok;_tmp845.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
_tmp844;});break;case 348: _LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL31E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Exp_tok_struct _tmp847;
_tmp847.tag=Cyc_Exp_tok;_tmp847.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
_tmp846;});break;case 350: _LL31F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp848=
_cycalloc(sizeof(*_tmp848));_tmp848[0]=({struct Cyc_Exp_tok_struct _tmp849;_tmp849.tag=
Cyc_Exp_tok;_tmp849.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
_tmp848;});break;case 351: _LL320: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL321: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({struct Cyc_Exp_tok_struct _tmp84B;
_tmp84B.tag=Cyc_Exp_tok;_tmp84B.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
_tmp84A;});break;case 353: _LL322: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp84C=
_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_Exp_tok_struct _tmp84D;_tmp84D.tag=
Cyc_Exp_tok;_tmp84D.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
_tmp84C;});break;case 354: _LL323: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL324: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({struct Cyc_Exp_tok_struct _tmp84F;
_tmp84F.tag=Cyc_Exp_tok;_tmp84F.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
_tmp84E;});break;case 356: _LL325: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp850=
_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_Exp_tok_struct _tmp851;_tmp851.tag=
Cyc_Exp_tok;_tmp851.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
_tmp850;});break;case 357: _LL326: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp852=
_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Exp_tok_struct _tmp853;_tmp853.tag=
Cyc_Exp_tok;_tmp853.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
_tmp852;});break;case 358: _LL327: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 359: _LL328: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854[0]=({struct Cyc_Exp_tok_struct _tmp855;
_tmp855.tag=Cyc_Exp_tok;_tmp855.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
_tmp854;});break;case 360: _LL329: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 361: _LL32A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_Exp_tok_struct _tmp857;
_tmp857.tag=Cyc_Exp_tok;_tmp857.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
_tmp856;});break;case 362: _LL32B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp858=
_cycalloc(sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Exp_tok_struct _tmp859;_tmp859.tag=
Cyc_Exp_tok;_tmp859.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
_tmp858;});break;case 363: _LL32C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp85A=
_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Exp_tok_struct _tmp85B;_tmp85B.tag=
Cyc_Exp_tok;_tmp85B.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
_tmp85A;});break;case 364: _LL32D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp85C=
_cycalloc(sizeof(*_tmp85C));_tmp85C[0]=({struct Cyc_Exp_tok_struct _tmp85D;_tmp85D.tag=
Cyc_Exp_tok;_tmp85D.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
_tmp85C;});break;case 365: _LL32E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 366: _LL32F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp85E=_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_Exp_tok_struct _tmp85F;
_tmp85F.tag=Cyc_Exp_tok;_tmp85F.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
_tmp85E;});break;case 367: _LL330: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Exp_tok_struct _tmp861;_tmp861.tag=
Cyc_Exp_tok;_tmp861.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
_tmp860;});break;case 368: _LL331: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp862=
_cycalloc(sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Exp_tok_struct _tmp863;_tmp863.tag=
Cyc_Exp_tok;_tmp863.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
_tmp862;});break;case 369: _LL332: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp864=
_cycalloc(sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Exp_tok_struct _tmp865;_tmp865.tag=
Cyc_Exp_tok;_tmp865.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Absyn_StructField_struct
_tmp867;_tmp867.tag=0;_tmp867.f1=({struct _tagged_arr*_tmp868=_cycalloc(sizeof(*
_tmp868));_tmp868[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp868;});_tmp867;});_tmp866;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
_tmp864;});break;case 370: _LL333: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp869=
_cycalloc(sizeof(*_tmp869));_tmp869[0]=({struct Cyc_Exp_tok_struct _tmp86A;_tmp86A.tag=
Cyc_Exp_tok;_tmp86A.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp86B=_cycalloc_atomic(sizeof(*_tmp86B));_tmp86B[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp86C;_tmp86C.tag=1;_tmp86C.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp86C;});_tmp86B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp86A;});_tmp869;});break;case 371: _LL334: {struct
Cyc_Position_Segment*_tmp86D=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp86E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp86D,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp86F=_cycalloc(
sizeof(*_tmp86F));_tmp86F[0]=({struct Cyc_Exp_tok_struct _tmp870;_tmp870.tag=Cyc_Exp_tok;
_tmp870.f1=Cyc_Absyn_gentyp_exp(_tmp86E,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp870;});
_tmp86F;});break;}case 372: _LL335: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp871=
_cycalloc(sizeof(*_tmp871));_tmp871[0]=({struct Cyc_Exp_tok_struct _tmp872;_tmp872.tag=
Cyc_Exp_tok;_tmp872.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp873=_cycalloc(sizeof(*_tmp873));_tmp873[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp874;_tmp874.tag=33;_tmp874.f1=({struct Cyc_Absyn_MallocInfo _tmp875;_tmp875.is_calloc=
0;_tmp875.rgn=0;_tmp875.elt_type=0;_tmp875.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp875.fat_result=0;
_tmp875;});_tmp874;});_tmp873;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp872;});
_tmp871;});break;case 373: _LL336: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp876=
_cycalloc(sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Exp_tok_struct _tmp877;_tmp877.tag=
Cyc_Exp_tok;_tmp877.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp879;_tmp879.tag=33;_tmp879.f1=({struct Cyc_Absyn_MallocInfo _tmp87A;_tmp87A.is_calloc=
0;_tmp87A.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp87A.elt_type=0;
_tmp87A.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp87A.fat_result=0;_tmp87A;});_tmp879;});_tmp878;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
_tmp876;});break;case 374: _LL337: {void*_tmp87C;struct _tuple2 _tmp87B=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp87C=_tmp87B.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp87D=_cycalloc(sizeof(*_tmp87D));
_tmp87D[0]=({struct Cyc_Exp_tok_struct _tmp87E;_tmp87E.tag=Cyc_Exp_tok;_tmp87E.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp87F=_cycalloc(
sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp880;_tmp880.tag=
33;_tmp880.f1=({struct Cyc_Absyn_MallocInfo _tmp881;_tmp881.is_calloc=1;_tmp881.rgn=
0;_tmp881.elt_type=({void**_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882[0]=
_tmp87C;_tmp882;});_tmp881.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp881.fat_result=0;
_tmp881;});_tmp880;});_tmp87F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
_tmp87D;});break;}case 375: _LL338: {void*_tmp884;struct _tuple2 _tmp883=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp884=_tmp883.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp885=_cycalloc(sizeof(*_tmp885));
_tmp885[0]=({struct Cyc_Exp_tok_struct _tmp886;_tmp886.tag=Cyc_Exp_tok;_tmp886.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp887=_cycalloc(
sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp888;_tmp888.tag=
33;_tmp888.f1=({struct Cyc_Absyn_MallocInfo _tmp889;_tmp889.is_calloc=1;_tmp889.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp889.elt_type=({void**_tmp88A=_cycalloc(sizeof(*_tmp88A));
_tmp88A[0]=_tmp884;_tmp88A;});_tmp889.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp889.fat_result=0;
_tmp889;});_tmp888;});_tmp887;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
_tmp885;});break;}case 376: _LL339: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Primop_tok_struct
_tmp88C;_tmp88C.tag=Cyc_Primop_tok;_tmp88C.f1=(void*)((void*)12);_tmp88C;});
_tmp88B;});break;case 377: _LL33A: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Primop_tok_struct
_tmp88E;_tmp88E.tag=Cyc_Primop_tok;_tmp88E.f1=(void*)((void*)11);_tmp88E;});
_tmp88D;});break;case 378: _LL33B: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Primop_tok_struct
_tmp890;_tmp890.tag=Cyc_Primop_tok;_tmp890.f1=(void*)((void*)2);_tmp890;});
_tmp88F;});break;case 379: _LL33C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 380: _LL33D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Exp_tok_struct _tmp892;
_tmp892.tag=Cyc_Exp_tok;_tmp892.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
_tmp891;});break;case 381: _LL33E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp893=
_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Exp_tok_struct _tmp894;_tmp894.tag=
Cyc_Exp_tok;_tmp894.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp894;});
_tmp893;});break;case 382: _LL33F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp895=
_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Exp_tok_struct _tmp896;_tmp896.tag=
Cyc_Exp_tok;_tmp896.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp896;});
_tmp895;});break;case 383: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp897=
_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Exp_tok_struct _tmp898;_tmp898.tag=
Cyc_Exp_tok;_tmp898.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp899;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
_tmp897;});break;case 384: _LL341: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89A=
_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Exp_tok_struct _tmp89B;_tmp89B.tag=
Cyc_Exp_tok;_tmp89B.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp89C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89B;});
_tmp89A;});break;case 385: _LL342: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89D=
_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=({struct Cyc_Exp_tok_struct _tmp89E;_tmp89E.tag=
Cyc_Exp_tok;_tmp89E.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89E;});
_tmp89D;});break;case 386: _LL343: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp89F=
_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Exp_tok_struct _tmp8A0;_tmp8A0.tag=
Cyc_Exp_tok;_tmp8A0.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A0;});
_tmp89F;});break;case 387: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A1=
_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct Cyc_Exp_tok_struct _tmp8A2;_tmp8A2.tag=
Cyc_Exp_tok;_tmp8A2.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp8A4;_tmp8A4.tag=25;_tmp8A4.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8A4.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8A4;});_tmp8A3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8A2;});_tmp8A1;});break;case 388: _LL345: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({
struct Cyc_Exp_tok_struct _tmp8A6;_tmp8A6.tag=Cyc_Exp_tok;_tmp8A6.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8A7=_cycalloc(sizeof(*_tmp8A7));
_tmp8A7[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8A8;_tmp8A8.tag=25;_tmp8A8.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp8A8.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp8A8;});_tmp8A7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
_tmp8A5;});break;case 389: _LL346: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Exp_tok_struct _tmp8AA;_tmp8AA.tag=
Cyc_Exp_tok;_tmp8AA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp8AC;_tmp8AC.tag=37;_tmp8AC.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8AC;});_tmp8AB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8AA;});_tmp8A9;});break;case 390: _LL347: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({
struct Cyc_Exp_tok_struct _tmp8AE;_tmp8AE.tag=Cyc_Exp_tok;_tmp8AE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Codegen_e_struct*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));
_tmp8AF[0]=({struct Cyc_Absyn_Codegen_e_struct _tmp8B0;_tmp8B0.tag=36;_tmp8B0.f1=
Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8B0;});_tmp8AF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AE;});
_tmp8AD;});break;case 391: _LL348: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B1=
_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Exp_tok_struct _tmp8B2;_tmp8B2.tag=
Cyc_Exp_tok;_tmp8B2.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp8B4;_tmp8B4.tag=2;_tmp8B4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B4;});_tmp8B3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
_tmp8B1;});break;case 392: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 393: _LL34A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Exp_tok_struct _tmp8B6;
_tmp8B6.tag=Cyc_Exp_tok;_tmp8B6.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B6;});
_tmp8B5;});break;case 394: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 395: _LL34C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7[0]=({struct Cyc_Exp_tok_struct _tmp8B8;
_tmp8B8.tag=Cyc_Exp_tok;_tmp8B8.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B8;});
_tmp8B7;});break;case 396: _LL34D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8B9=
_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=({struct Cyc_Exp_tok_struct _tmp8BA;_tmp8BA.tag=
Cyc_Exp_tok;_tmp8BA.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BA;});
_tmp8B9;});break;case 397: _LL34E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BB=
_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=({struct Cyc_Exp_tok_struct _tmp8BC;_tmp8BC.tag=
Cyc_Exp_tok;_tmp8BC.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BC;});
_tmp8BB;});break;case 398: _LL34F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8BD=
_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({struct Cyc_Exp_tok_struct _tmp8BE;_tmp8BE.tag=
Cyc_Exp_tok;_tmp8BE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp8BF=_cycalloc(sizeof(*_tmp8BF));_tmp8BF[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp8C0;_tmp8C0.tag=28;_tmp8C0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8C0.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8C0.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8C0.f4=0;
_tmp8C0;});_tmp8BF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BE;});
_tmp8BD;});break;case 399: _LL350: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8C1=
_cycalloc(sizeof(*_tmp8C1));_tmp8C1[0]=({struct Cyc_Exp_tok_struct _tmp8C2;_tmp8C2.tag=
Cyc_Exp_tok;_tmp8C2.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C2;});
_tmp8C1;});break;case 400: _LL351: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp8C3=_cycalloc(sizeof(*_tmp8C3));_tmp8C3[0]=({struct Cyc_ExpList_tok_struct
_tmp8C4;_tmp8C4.tag=Cyc_ExpList_tok;_tmp8C4.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp8C4;});_tmp8C3;});
break;case 401: _LL352: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp8C5=
_cycalloc(sizeof(*_tmp8C5));_tmp8C5[0]=({struct Cyc_ExpList_tok_struct _tmp8C6;
_tmp8C6.tag=Cyc_ExpList_tok;_tmp8C6.f1=({struct Cyc_List_List*_tmp8C7=_cycalloc(
sizeof(*_tmp8C7));_tmp8C7->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C7->tl=0;_tmp8C7;});
_tmp8C6;});_tmp8C5;});break;case 402: _LL353: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8[0]=({struct Cyc_ExpList_tok_struct
_tmp8C9;_tmp8C9.tag=Cyc_ExpList_tok;_tmp8C9.f1=({struct Cyc_List_List*_tmp8CA=
_cycalloc(sizeof(*_tmp8CA));_tmp8CA->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8CA->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8CA;});_tmp8C9;});
_tmp8C8;});break;case 403: _LL354: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CB=
_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Exp_tok_struct _tmp8CC;_tmp8CC.tag=
Cyc_Exp_tok;_tmp8CC.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CC;});
_tmp8CB;});break;case 404: _LL355: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CD=
_cycalloc(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Exp_tok_struct _tmp8CE;_tmp8CE.tag=
Cyc_Exp_tok;_tmp8CE.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CE;});
_tmp8CD;});break;case 405: _LL356: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Exp_tok_struct _tmp8D0;_tmp8D0.tag=
Cyc_Exp_tok;_tmp8D0.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D0;});
_tmp8CF;});break;case 406: _LL357: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp8D1=
_cycalloc(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Exp_tok_struct _tmp8D2;_tmp8D2.tag=
Cyc_Exp_tok;_tmp8D2.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D2;});
_tmp8D1;});break;case 407: _LL358: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_QualId_tok_struct
_tmp8D4;_tmp8D4.tag=Cyc_QualId_tok;_tmp8D4.f1=({struct _tuple1*_tmp8D5=_cycalloc(
sizeof(*_tmp8D5));_tmp8D5->f1=Cyc_Absyn_rel_ns_null;_tmp8D5->f2=({struct
_tagged_arr*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));_tmp8D6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8D6;});_tmp8D5;});
_tmp8D4;});_tmp8D3;});break;case 408: _LL359: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 409: _LL35A: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp8D7=_cycalloc(sizeof(*_tmp8D7));_tmp8D7[
0]=({struct Cyc_QualId_tok_struct _tmp8D8;_tmp8D8.tag=Cyc_QualId_tok;_tmp8D8.f1=({
struct _tuple1*_tmp8D9=_cycalloc(sizeof(*_tmp8D9));_tmp8D9->f1=Cyc_Absyn_rel_ns_null;
_tmp8D9->f2=({struct _tagged_arr*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp8DA;});_tmp8D9;});_tmp8D8;});_tmp8D7;});break;case 410: _LL35B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 411: _LL35C: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 412: _LL35D:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 413:
_LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
414: _LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 415: _LL360: break;case 416: _LL361:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL362: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(417,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(116,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0?yystate <= 
5610: 0)?Cyc_yycheck[_check_known_subscript_notnull(5611,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)]: 0)yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5611,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(116,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(854,yystate)];if(yyn > - 32768?yyn < 5610: 0){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 247 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5611,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)])
+ 15,count ++);}msg=({unsigned int _tmp8DB=(unsigned int)(sze + 15);char*_tmp8DC=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp8DB + 1));struct _tagged_arr
_tmp8DE=_tag_arr(_tmp8DC,sizeof(char),_tmp8DB + 1);{unsigned int _tmp8DD=_tmp8DB;
unsigned int i;for(i=0;i < _tmp8DD;i ++){_tmp8DC[i]='\000';}_tmp8DC[_tmp8DD]=(char)
0;}_tmp8DE;});Cyc_strcpy(msg,({const char*_tmp8DF="parse error";_tag_arr(_tmp8DF,
sizeof(char),_get_zero_arr_size(_tmp8DF,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 247 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5611,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?({const char*_tmp8E0=", expecting `";_tag_arr(_tmp8E0,
sizeof(char),_get_zero_arr_size(_tmp8E0,14));}):({const char*_tmp8E1=" or `";
_tag_arr(_tmp8E1,sizeof(char),_get_zero_arr_size(_tmp8E1,6));})));Cyc_strcat(msg,(
struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(247,x)]);Cyc_strcat(
msg,({const char*_tmp8E2="'";_tag_arr(_tmp8E2,sizeof(char),_get_zero_arr_size(
_tmp8E2,2));}));count ++;}}}Cyc_yyerror((struct _tagged_arr)msg);}else{Cyc_yyerror(({
const char*_tmp8E3="parse error";_tag_arr(_tmp8E3,sizeof(char),_get_zero_arr_size(
_tmp8E3,12));}));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)
return 1;Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0)return 1;yyvsp_offset --;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];yylsp_offset --;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(854,yystate)];if(yyn == - 32768)
goto yyerrdefault;yyn +=1;if((yyn < 0?1: yyn > 5610)?1: Cyc_yycheck[
_check_known_subscript_notnull(5611,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5611,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 853)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp8ED=v;struct _tuple13*_tmp8EE;
struct _tuple13 _tmp8EF;int _tmp8F0;char _tmp8F1;short _tmp8F2;struct _tagged_arr
_tmp8F3;struct Cyc_Core_Opt*_tmp8F4;struct Cyc_Core_Opt*_tmp8F5;struct Cyc_Core_Opt
_tmp8F6;struct _tagged_arr*_tmp8F7;struct _tuple1*_tmp8F8;struct _tuple1 _tmp8F9;
void*_tmp8FA;struct _tagged_arr*_tmp8FB;_LL365: if(*((void**)_tmp8ED)!= Cyc_Int_tok)
goto _LL367;_tmp8EE=((struct Cyc_Int_tok_struct*)_tmp8ED)->f1;_tmp8EF=*_tmp8EE;
_tmp8F0=_tmp8EF.f2;_LL366:({struct Cyc_Int_pa_struct _tmp8FE;_tmp8FE.tag=1;_tmp8FE.f1=(
unsigned int)_tmp8F0;{void*_tmp8FC[1]={& _tmp8FE};Cyc_fprintf(Cyc_stderr,({const
char*_tmp8FD="%d";_tag_arr(_tmp8FD,sizeof(char),_get_zero_arr_size(_tmp8FD,3));}),
_tag_arr(_tmp8FC,sizeof(void*),1));}});goto _LL364;_LL367: if(*((void**)_tmp8ED)!= 
Cyc_Char_tok)goto _LL369;_tmp8F1=((struct Cyc_Char_tok_struct*)_tmp8ED)->f1;_LL368:({
struct Cyc_Int_pa_struct _tmp901;_tmp901.tag=1;_tmp901.f1=(unsigned int)((int)
_tmp8F1);{void*_tmp8FF[1]={& _tmp901};Cyc_fprintf(Cyc_stderr,({const char*_tmp900="%c";
_tag_arr(_tmp900,sizeof(char),_get_zero_arr_size(_tmp900,3));}),_tag_arr(_tmp8FF,
sizeof(void*),1));}});goto _LL364;_LL369: if(*((void**)_tmp8ED)!= Cyc_Short_tok)
goto _LL36B;_tmp8F2=((struct Cyc_Short_tok_struct*)_tmp8ED)->f1;_LL36A:({struct Cyc_Int_pa_struct
_tmp904;_tmp904.tag=1;_tmp904.f1=(unsigned int)((int)_tmp8F2);{void*_tmp902[1]={&
_tmp904};Cyc_fprintf(Cyc_stderr,({const char*_tmp903="%ds";_tag_arr(_tmp903,
sizeof(char),_get_zero_arr_size(_tmp903,4));}),_tag_arr(_tmp902,sizeof(void*),1));}});
goto _LL364;_LL36B: if(*((void**)_tmp8ED)!= Cyc_String_tok)goto _LL36D;_tmp8F3=((
struct Cyc_String_tok_struct*)_tmp8ED)->f1;_LL36C:({struct Cyc_String_pa_struct
_tmp907;_tmp907.tag=0;_tmp907.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp8F3);{
void*_tmp905[1]={& _tmp907};Cyc_fprintf(Cyc_stderr,({const char*_tmp906="\"%s\"";
_tag_arr(_tmp906,sizeof(char),_get_zero_arr_size(_tmp906,5));}),_tag_arr(_tmp905,
sizeof(void*),1));}});goto _LL364;_LL36D: if(*((void**)_tmp8ED)!= Cyc_Stringopt_tok)
goto _LL36F;_tmp8F4=((struct Cyc_Stringopt_tok_struct*)_tmp8ED)->f1;if(_tmp8F4 != 0)
goto _LL36F;_LL36E:({void*_tmp908[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp909="NULL";_tag_arr(_tmp909,sizeof(char),_get_zero_arr_size(_tmp909,5));}),
_tag_arr(_tmp908,sizeof(void*),0));});goto _LL364;_LL36F: if(*((void**)_tmp8ED)!= 
Cyc_Stringopt_tok)goto _LL371;_tmp8F5=((struct Cyc_Stringopt_tok_struct*)_tmp8ED)->f1;
if(_tmp8F5 == 0)goto _LL371;_tmp8F6=*_tmp8F5;_tmp8F7=(struct _tagged_arr*)_tmp8F6.v;
_LL370:({struct Cyc_String_pa_struct _tmp90C;_tmp90C.tag=0;_tmp90C.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp8F7);{void*_tmp90A[1]={& _tmp90C};Cyc_fprintf(
Cyc_stderr,({const char*_tmp90B="\"%s\"";_tag_arr(_tmp90B,sizeof(char),
_get_zero_arr_size(_tmp90B,5));}),_tag_arr(_tmp90A,sizeof(void*),1));}});goto
_LL364;_LL371: if(*((void**)_tmp8ED)!= Cyc_QualId_tok)goto _LL373;_tmp8F8=((struct
Cyc_QualId_tok_struct*)_tmp8ED)->f1;_tmp8F9=*_tmp8F8;_tmp8FA=_tmp8F9.f1;_tmp8FB=
_tmp8F9.f2;_LL372: {struct Cyc_List_List*_tmp90D=0;{void*_tmp90E=_tmp8FA;struct
Cyc_List_List*_tmp90F;struct Cyc_List_List*_tmp910;_LL376: if(_tmp90E <= (void*)1?1:*((
int*)_tmp90E)!= 0)goto _LL378;_tmp90F=((struct Cyc_Absyn_Rel_n_struct*)_tmp90E)->f1;
_LL377: _tmp90D=_tmp90F;goto _LL375;_LL378: if(_tmp90E <= (void*)1?1:*((int*)_tmp90E)
!= 1)goto _LL37A;_tmp910=((struct Cyc_Absyn_Abs_n_struct*)_tmp90E)->f1;_LL379:
_tmp90D=_tmp910;goto _LL375;_LL37A: if((int)_tmp90E != 0)goto _LL375;_LL37B: goto
_LL375;_LL375:;}for(0;_tmp90D != 0;_tmp90D=_tmp90D->tl){({struct Cyc_String_pa_struct
_tmp913;_tmp913.tag=0;_tmp913.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp90D->hd));{void*_tmp911[1]={& _tmp913};Cyc_fprintf(Cyc_stderr,({
const char*_tmp912="%s::";_tag_arr(_tmp912,sizeof(char),_get_zero_arr_size(
_tmp912,5));}),_tag_arr(_tmp911,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp916;_tmp916.tag=0;_tmp916.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp8FB);{
void*_tmp914[1]={& _tmp916};Cyc_fprintf(Cyc_stderr,({const char*_tmp915="%s::";
_tag_arr(_tmp915,sizeof(char),_get_zero_arr_size(_tmp915,5));}),_tag_arr(_tmp914,
sizeof(void*),1));}});goto _LL364;}_LL373:;_LL374:({void*_tmp917[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp918="?";_tag_arr(_tmp918,sizeof(char),
_get_zero_arr_size(_tmp918,2));}),_tag_arr(_tmp917,sizeof(void*),0));});goto
_LL364;_LL364:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmp919=_cycalloc(
sizeof(*_tmp919));_tmp919->v=Cyc_Lexing_from_file(f);_tmp919;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
