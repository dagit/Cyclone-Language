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
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;
struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
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
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct Cyc_Absyn_Conref*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*f3;}
;struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual
f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern void*Cyc_Absyn_rel_ns_null;
int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
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
_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Conref*_tmp1F;struct Cyc_Position_Segment*
_tmp20;_LL1: if(_tmp1A <= (void*)3  || *((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct
Cyc_Absyn_ArrayType_struct*)_tmp1A)->f1;_tmp1C=(void*)_tmp1B.elt_type;_tmp1D=
_tmp1B.tq;_tmp1E=_tmp1B.num_elts;_tmp1F=_tmp1B.zero_term;_tmp20=_tmp1B.zt_loc;
_LL2: return Cyc_Absyn_starb_typ(_tmp1C,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->v=(void*)((void*)3);_tmp21;}),0):(void*)
2,_tmp1D,_tmp1E == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_Upper_b_struct _tmp23;
_tmp23.tag=0;_tmp23.f1=(struct Cyc_Absyn_Exp*)_tmp1E;_tmp23;});_tmp22;}),_tmp1F);
_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=
Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,
struct _tuple2*t){return({struct _tuple9*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->f1=(*
t).f2;_tmp24->f2=(*t).f3;_tmp24;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp25=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp26;
_LL6: if(_tmp25 <= (void*)2  || *((int*)_tmp25)!= 0)goto _LL8;_tmp26=((struct Cyc_Absyn_Var_d_struct*)
_tmp25)->f1;_LL7: return _tmp26;_LL8:;_LL9:({void*_tmp27[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,({const char*
_tmp28="bad declaration in `forarray' statement";_tag_arr(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,40));}),_tag_arr(_tmp27,sizeof(void*),0));});_LL5:;}
static int Cyc_Parse_is_typeparam(void*tm){void*_tmp29=tm;_LLB: if(*((int*)_tmp29)
!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _tagged_arr s,void*k){if(Cyc_zstrcmp((struct _tagged_arr)s,({const char*
_tmp2A="`H";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,3));}))== 0)
return(void*)2;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp2B=
_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Absyn_VarType_struct _tmp2C;
_tmp2C.tag=1;_tmp2C.f1=({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D->name=({struct _tagged_arr*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=s;
_tmp2E;});_tmp2D->identity=0;_tmp2D->kind=(void*)k;_tmp2D;});_tmp2C;});_tmp2B;});}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{
void*_tmp2F=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp30;void*_tmp31;_LL10:
if(*((int*)_tmp2F)!= 0)goto _LL12;_tmp30=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp2F)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp32=_cycalloc(
sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp33;_tmp33.tag=0;
_tmp33.f1=(void*)_tmp30;_tmp33;});_tmp32;});goto _LLF;_LL12: if(*((int*)_tmp2F)!= 
1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp34=_cycalloc(
sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp35;_tmp35.tag=
1;_tmp35.f1=0;_tmp35;});_tmp34;});goto _LLF;_LL14: if(*((int*)_tmp2F)!= 2)goto _LLF;
_tmp31=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2F)->f2;_LL15: k=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=(void*)
_tmp31;_tmp37;});_tmp36;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp38=
_cycalloc(sizeof(*_tmp38));_tmp38->name=t->name;_tmp38->identity=0;_tmp38->kind=(
void*)k;_tmp38;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp39=t;struct Cyc_Absyn_Tvar*_tmp3A;_LL17: if(_tmp39 <= (void*)3
 || *((int*)_tmp39)!= 1)goto _LL19;_tmp3A=((struct Cyc_Absyn_VarType_struct*)
_tmp39)->f1;_LL18: return _tmp3A;_LL19:;_LL1A:({void*_tmp3B[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,({const char*_tmp3C="expecting a list of type variables, not types";_tag_arr(
_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,46));}),_tag_arr(_tmp3B,sizeof(
void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
return(void*)({struct Cyc_Absyn_VarType_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));
_tmp3D[0]=({struct Cyc_Absyn_VarType_struct _tmp3E;_tmp3E.tag=1;_tmp3E.f1=pr;
_tmp3E;});_tmp3D;});}static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*
_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp40;struct Cyc_Absyn_Tvar
_tmp41;void*_tmp42;void**_tmp43;_LL1C: if(_tmp3F <= (void*)3  || *((int*)_tmp3F)!= 
1)goto _LL1E;_tmp40=((struct Cyc_Absyn_VarType_struct*)_tmp3F)->f1;_tmp41=*_tmp40;
_tmp42=(void*)_tmp41.kind;_tmp43=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3F)->f1).kind;_LL1D: {void*_tmp44=Cyc_Absyn_compress_kb(*_tmp43);_LL21: if(*((
int*)_tmp44)!= 1)goto _LL23;_LL22:*_tmp43=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp46;
_tmp46.tag=0;_tmp46.f1=(void*)k;_tmp46;});_tmp45;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp47=(void*)tms->hd;void*_tmp48;
_LL26: if(*((int*)_tmp47)!= 3)goto _LL28;_tmp48=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp47)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp49=_tmp48;struct Cyc_List_List*_tmp4A;_LL2B: if(*((int*)_tmp49)!= 1)
goto _LL2D;_LL2C:({void*_tmp4B[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4C="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_arr(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,93));}),_tag_arr(_tmp4B,
sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp49)!= 0)goto _LL2A;_tmp4A=((
struct Cyc_Absyn_NoTypes_struct*)_tmp49)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tds))({void*_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp4E="wrong number of parameter declarations in old-style function declaration";
_tag_arr(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,73));}),_tag_arr(_tmp4D,
sizeof(void*),0));});{struct Cyc_List_List*_tmp4F=0;for(0;_tmp4A != 0;_tmp4A=
_tmp4A->tl){struct Cyc_List_List*_tmp50=tds;for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
struct Cyc_Absyn_Decl*_tmp51=(struct Cyc_Absyn_Decl*)_tmp50->hd;void*_tmp52=(void*)
_tmp51->r;struct Cyc_Absyn_Vardecl*_tmp53;_LL30: if(_tmp52 <= (void*)2  || *((int*)
_tmp52)!= 0)goto _LL32;_tmp53=((struct Cyc_Absyn_Var_d_struct*)_tmp52)->f1;_LL31:
if(Cyc_zstrptrcmp((*_tmp53->name).f2,(struct _tagged_arr*)_tmp4A->hd)!= 0)
continue;if(_tmp53->initializer != 0)({void*_tmp54[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp51->loc,({
const char*_tmp55="initializer found in parameter declaration";_tag_arr(_tmp55,
sizeof(char),_get_zero_arr_size(_tmp55,43));}),_tag_arr(_tmp54,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp53->name))({void*_tmp56[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmp51->loc,({const char*_tmp57="namespaces forbidden in parameter declarations";
_tag_arr(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,47));}),_tag_arr(_tmp56,
sizeof(void*),0));});_tmp4F=({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->hd=({struct _tuple2*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=({
struct Cyc_Core_Opt*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->v=(*_tmp53->name).f2;
_tmp5A;});_tmp59->f2=_tmp53->tq;_tmp59->f3=(void*)_tmp53->type;_tmp59;});_tmp58->tl=
_tmp4F;_tmp58;});goto L;_LL32:;_LL33:({void*_tmp5B[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp51->loc,({
const char*_tmp5C="nonvariable declaration in parameter type";_tag_arr(_tmp5C,
sizeof(char),_get_zero_arr_size(_tmp5C,42));}),_tag_arr(_tmp5B,sizeof(void*),0));});
_LL2F:;}L: if(_tmp50 == 0)({struct Cyc_String_pa_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp4A->hd));{void*
_tmp5D[1]={& _tmp5F};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp5E="%s is not given a type";
_tag_arr(_tmp5E,sizeof(char),_get_zero_arr_size(_tmp5E,23));}),_tag_arr(_tmp5D,
sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_Function_mod_struct _tmp62;
_tmp62.tag=3;_tmp62.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp63=
_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_WithTypes_struct _tmp64;
_tmp64.tag=1;_tmp64.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4F);_tmp64.f2=0;_tmp64.f3=0;_tmp64.f4=0;_tmp64.f5=0;_tmp64;});_tmp63;}));
_tmp62;});_tmp61;}));_tmp60->tl=0;_tmp60;});}_LL2A:;}goto _LL29;_LL28:;_LL29:
return({struct Cyc_List_List*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->hd=(void*)((
void*)tms->hd);_tmp65->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp65;});
_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp66=
_cycalloc(sizeof(*_tmp66));_tmp66->id=d->id;_tmp66->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp66;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp67=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp67 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp67 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp68="bad storage class on function";
_tag_arr(_tmp68,sizeof(char),_get_zero_arr_size(_tmp68,30));}),loc);goto _LL34;
_LL34:;}}{void*_tmp6A;struct Cyc_Core_Opt*_tmp6B;struct _tuple5 _tmp69=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{struct Cyc_Absyn_Tqual _tmp6D;void*
_tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_List_List*_tmp70;struct _tuple6 _tmp6C=
Cyc_Parse_apply_tms(tq,_tmp6A,atts,d->tms);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
_tmp6F=_tmp6C.f3;_tmp70=_tmp6C.f4;if(_tmp6B != 0)({void*_tmp71[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp72="nested type declaration within function prototype";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,50));}),_tag_arr(_tmp71,
sizeof(void*),0));});if(_tmp6F != 0)({void*_tmp73[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp74="bad type params, ignoring";_tag_arr(_tmp74,sizeof(char),
_get_zero_arr_size(_tmp74,26));}),_tag_arr(_tmp73,sizeof(void*),0));});{void*
_tmp75=_tmp6E;struct Cyc_Absyn_FnInfo _tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Core_Opt*
_tmp78;void*_tmp79;struct Cyc_List_List*_tmp7A;int _tmp7B;struct Cyc_Absyn_VarargInfo*
_tmp7C;struct Cyc_List_List*_tmp7D;struct Cyc_List_List*_tmp7E;_LL3C: if(_tmp75 <= (
void*)3  || *((int*)_tmp75)!= 8)goto _LL3E;_tmp76=((struct Cyc_Absyn_FnType_struct*)
_tmp75)->f1;_tmp77=_tmp76.tvars;_tmp78=_tmp76.effect;_tmp79=(void*)_tmp76.ret_typ;
_tmp7A=_tmp76.args;_tmp7B=_tmp76.c_varargs;_tmp7C=_tmp76.cyc_varargs;_tmp7D=
_tmp76.rgn_po;_tmp7E=_tmp76.attributes;_LL3D: {struct Cyc_List_List*_tmp7F=0;{
struct Cyc_List_List*_tmp80=_tmp7A;for(0;_tmp80 != 0;_tmp80=_tmp80->tl){struct
_tuple2 _tmp82;struct Cyc_Core_Opt*_tmp83;struct Cyc_Absyn_Tqual _tmp84;void*_tmp85;
struct _tuple2*_tmp81=(struct _tuple2*)_tmp80->hd;_tmp82=*_tmp81;_tmp83=_tmp82.f1;
_tmp84=_tmp82.f2;_tmp85=_tmp82.f3;if(_tmp83 == 0){Cyc_Parse_err(({const char*
_tmp86="missing argument variable in function prototype";_tag_arr(_tmp86,sizeof(
char),_get_zero_arr_size(_tmp86,48));}),loc);_tmp7F=({struct Cyc_List_List*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87->hd=({struct _tuple10*_tmp88=_cycalloc(sizeof(*
_tmp88));_tmp88->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",
sizeof(char),2);_tmp88->f2=_tmp84;_tmp88->f3=_tmp85;_tmp88;});_tmp87->tl=_tmp7F;
_tmp87;});}else{_tmp7F=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->hd=({struct _tuple10*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->f1=(struct
_tagged_arr*)_tmp83->v;_tmp8B->f2=_tmp84;_tmp8B->f3=_tmp85;_tmp8B;});_tmp8A->tl=
_tmp7F;_tmp8A;});}}}return({struct Cyc_Absyn_Fndecl*_tmp8C=_cycalloc(sizeof(*
_tmp8C));_tmp8C->sc=(void*)sc;_tmp8C->name=d->id;_tmp8C->tvs=_tmp77;_tmp8C->is_inline=
is_inline;_tmp8C->effect=_tmp78;_tmp8C->ret_type=(void*)_tmp79;_tmp8C->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7F);_tmp8C->c_varargs=
_tmp7B;_tmp8C->cyc_varargs=_tmp7C;_tmp8C->rgn_po=_tmp7D;_tmp8C->body=body;_tmp8C->cached_typ=
0;_tmp8C->param_vardecls=0;_tmp8C->attributes=Cyc_List_append(_tmp7E,_tmp70);
_tmp8C;});}_LL3E:;_LL3F:({void*_tmp8D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp8E="declarator is not a function prototype";_tag_arr(_tmp8E,sizeof(char),
_get_zero_arr_size(_tmp8E,39));}),_tag_arr(_tmp8D,sizeof(void*),0));});_LL3B:;}}}}}
static char _tmp8F[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp8F,_tmp8F,_tmp8F + 52};static char
_tmp90[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp90,_tmp90,_tmp90 + 63};static char
_tmp91[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp91,_tmp91,_tmp91 + 50};static char _tmp92[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp92,_tmp92,_tmp92 + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp93=(void*)ts->hd;void*_tmp94;
struct Cyc_Position_Segment*_tmp95;struct Cyc_Position_Segment*_tmp96;struct Cyc_Position_Segment*
_tmp97;struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*_tmp99;
struct Cyc_Absyn_Decl*_tmp9A;_LL41: if(*((int*)_tmp93)!= 4)goto _LL43;_tmp94=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp93)->f1;_tmp95=((struct Cyc_Parse_Type_spec_struct*)
_tmp93)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp95);last_loc=
_tmp95;seen_type=1;t=_tmp94;goto _LL40;_LL43: if(*((int*)_tmp93)!= 0)goto _LL45;
_tmp96=((struct Cyc_Parse_Signed_spec_struct*)_tmp93)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp96);if(seen_type)Cyc_Parse_err(({const char*_tmp9B="signed qualifier must come before type";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,39));}),_tmp96);last_loc=
_tmp96;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp93)!= 1)goto _LL47;
_tmp97=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp93)->f1;_LL46: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmp97);if(seen_type)Cyc_Parse_err(({const char*
_tmp9C="signed qualifier must come before type";_tag_arr(_tmp9C,sizeof(char),
_get_zero_arr_size(_tmp9C,39));}),_tmp97);last_loc=_tmp97;seen_sign=1;sgn=(void*)
1;goto _LL40;_LL47: if(*((int*)_tmp93)!= 2)goto _LL49;_tmp98=((struct Cyc_Parse_Short_spec_struct*)
_tmp93)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp9D="integral size may appear only once within a type specifier";
_tag_arr(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,59));}),_tmp98);if(
seen_type)Cyc_Parse_err(({const char*_tmp9E="short modifier must come before base type";
_tag_arr(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,42));}),_tmp98);last_loc=
_tmp98;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp93)!= 3)goto _LL4B;
_tmp99=((struct Cyc_Parse_Long_spec_struct*)_tmp93)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({
const char*_tmp9F="long modifier must come before base type";_tag_arr(_tmp9F,
sizeof(char),_get_zero_arr_size(_tmp9F,41));}),_tmp99);if(seen_size){void*_tmpA0=
sz;_LL4E: if((int)_tmpA0 != 2)goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51:
Cyc_Parse_err(({const char*_tmpA1="extra long in type specifier";_tag_arr(_tmpA1,
sizeof(char),_get_zero_arr_size(_tmpA1,29));}),_tmp99);goto _LL4D;_LL4D:;}
last_loc=_tmp99;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp93)!= 5)goto _LL40;
_tmp9A=((struct Cyc_Parse_Decl_spec_struct*)_tmp93)->f1;_LL4C: last_loc=_tmp9A->loc;
if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpA2=(void*)_tmp9A->r;struct
Cyc_Absyn_Aggrdecl*_tmpA3;struct Cyc_Absyn_Tuniondecl*_tmpA4;struct Cyc_Absyn_Enumdecl*
_tmpA5;_LL53: if(_tmpA2 <= (void*)2  || *((int*)_tmpA2)!= 4)goto _LL55;_tmpA3=((
struct Cyc_Absyn_Aggr_d_struct*)_tmpA2)->f1;_LL54: {struct Cyc_List_List*_tmpA6=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA3->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_AggrType_struct _tmpA8;_tmpA8.tag=10;
_tmpA8.f1=({struct Cyc_Absyn_AggrInfo _tmpA9;_tmpA9.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(void*)((void*)
_tmpA3->kind);_tmpAB.f2=_tmpA3->name;_tmpAB;});_tmpAA;}));_tmpA9.targs=_tmpA6;
_tmpA9;});_tmpA8;});_tmpA7;});if(_tmpA3->impl != 0)declopt=({struct Cyc_Core_Opt*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->v=_tmp9A;_tmpAC;});goto _LL52;}_LL55:
if(_tmpA2 <= (void*)2  || *((int*)_tmpA2)!= 5)goto _LL57;_tmpA4=((struct Cyc_Absyn_Tunion_d_struct*)
_tmpA2)->f1;_LL56: {struct Cyc_List_List*_tmpAD=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA4->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAF;_tmpAF.tag=2;_tmpAF.f1=({struct Cyc_Absyn_TunionInfo
_tmpB0;_tmpB0.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB2;_tmpB2.tag=1;_tmpB2.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB3=_cycalloc(
sizeof(*_tmpB3));_tmpB3[0]=_tmpA4;_tmpB3;});_tmpB2;});_tmpB1;}));_tmpB0.targs=
_tmpAD;_tmpB0.rgn=(void*)((void*)2);_tmpB0;});_tmpAF;});_tmpAE;});if(_tmpA4->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=
_tmp9A;_tmpB4;});goto _LL52;}_LL57: if(_tmpA2 <= (void*)2  || *((int*)_tmpA2)!= 6)
goto _LL59;_tmpA5=((struct Cyc_Absyn_Enum_d_struct*)_tmpA2)->f1;_LL58: t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({
struct Cyc_Absyn_EnumType_struct _tmpB6;_tmpB6.tag=12;_tmpB6.f1=_tmpA5->name;
_tmpB6.f2=0;_tmpB6;});_tmpB5;});declopt=({struct Cyc_Core_Opt*_tmpB7=_cycalloc(
sizeof(*_tmpB7));_tmpB7->v=_tmp9A;_tmpB7;});goto _LL52;_LL59:;_LL5A:({void*_tmpB8[
0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp9A->loc,({const char*_tmpB9="bad declaration within type specifier";
_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,38));}),_tag_arr(_tmpB8,
sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9A->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpBA[0]={};
Cyc_Tcutil_warn(last_loc,({const char*_tmpBB="missing type within specifier";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,30));}),_tag_arr(_tmpBA,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBC=_cycalloc(
sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_IntType_struct _tmpBD;_tmpBD.tag=5;
_tmpBD.f1=(void*)sgn;_tmpBD.f2=(void*)sz;_tmpBD;});_tmpBC;});}else{if(seen_sign){
void*_tmpBE=t;void*_tmpBF;_LL5C: if(_tmpBE <= (void*)3  || *((int*)_tmpBE)!= 5)goto
_LL5E;_tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBE)->f2;_LL5D: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({
struct Cyc_Absyn_IntType_struct _tmpC1;_tmpC1.tag=5;_tmpC1.f1=(void*)sgn;_tmpC1.f2=(
void*)_tmpBF;_tmpC1;});_tmpC0;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const
char*_tmpC2="sign specification on non-integral type";_tag_arr(_tmpC2,sizeof(
char),_get_zero_arr_size(_tmpC2,40));}),last_loc);goto _LL5B;_LL5B:;}if(seen_size){
void*_tmpC3=t;void*_tmpC4;_LL61: if(_tmpC3 <= (void*)3  || *((int*)_tmpC3)!= 5)goto
_LL63;_tmpC4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC3)->f1;_LL62: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({
struct Cyc_Absyn_IntType_struct _tmpC6;_tmpC6.tag=5;_tmpC6.f1=(void*)_tmpC4;_tmpC6.f2=(
void*)sz;_tmpC6;});_tmpC5;});goto _LL60;_LL63: if(_tmpC3 <= (void*)3  || *((int*)
_tmpC3)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpC7=
_cycalloc_atomic(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC8;_tmpC8.tag=6;_tmpC8.f1=1;_tmpC8;});_tmpC7;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpC9="size qualifier on non-integral type";_tag_arr(_tmpC9,sizeof(
char),_get_zero_arr_size(_tmpC9,36));}),last_loc);goto _LL60;_LL60:;}}return({
struct _tuple5 _tmpCA;_tmpCA.f1=t;_tmpCA.f2=declopt;_tmpCA;});}static struct Cyc_List_List*
Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCB=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCC=_tmpCB->id;
struct Cyc_Absyn_Tqual _tmpCE;void*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_List_List*
_tmpD1;struct _tuple6 _tmpCD=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCB->tms);
_tmpCE=_tmpCD.f1;_tmpCF=_tmpCD.f2;_tmpD0=_tmpCD.f3;_tmpD1=_tmpCD.f4;return({
struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=({struct _tuple7*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->f1=_tmpCC;_tmpD3->f2=_tmpCE;_tmpD3->f3=
_tmpCF;_tmpD3->f4=_tmpD0;_tmpD3->f5=_tmpD1;_tmpD3;});_tmpD2->tl=Cyc_Parse_apply_tmss(
_tmpCE,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD2;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD4;_tmpD4.f1=
tq;_tmpD4.f2=t;_tmpD4.f3=0;_tmpD4.f4=atts;_tmpD4;});{void*_tmpD5=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Position_Segment*_tmpD7;struct Cyc_Absyn_Exp*
_tmpD8;struct Cyc_Absyn_Conref*_tmpD9;struct Cyc_Position_Segment*_tmpDA;void*
_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_Position_Segment*_tmpDD;struct Cyc_Absyn_PtrAtts
_tmpDE;struct Cyc_Absyn_Tqual _tmpDF;struct Cyc_Position_Segment*_tmpE0;struct Cyc_List_List*
_tmpE1;_LL68: if(*((int*)_tmpD5)!= 0)goto _LL6A;_tmpD6=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpD5)->f1;_tmpD7=((struct Cyc_Absyn_Carray_mod_struct*)_tmpD5)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD6,
_tmpD7),atts,tms->tl);_LL6A: if(*((int*)_tmpD5)!= 1)goto _LL6C;_tmpD8=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD5)->f1;_tmpD9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD5)->f2;_tmpDA=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD5)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpD8,
_tmpD9,_tmpDA),atts,tms->tl);_LL6C: if(*((int*)_tmpD5)!= 3)goto _LL6E;_tmpDB=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpD5)->f1;_LL6D: {void*_tmpE2=_tmpDB;
struct Cyc_List_List*_tmpE3;int _tmpE4;struct Cyc_Absyn_VarargInfo*_tmpE5;struct Cyc_Core_Opt*
_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_Position_Segment*_tmpE8;_LL75: if(*((
int*)_tmpE2)!= 1)goto _LL77;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE2)->f1;
_tmpE4=((struct Cyc_Absyn_WithTypes_struct*)_tmpE2)->f2;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE2)->f3;_tmpE6=((struct Cyc_Absyn_WithTypes_struct*)_tmpE2)->f4;_tmpE7=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE2)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=(void*)((
void*)as->hd);_tmpE9->tl=fn_atts;_tmpE9;});else{new_atts=({struct Cyc_List_List*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)((void*)as->hd);_tmpEA->tl=
new_atts;_tmpEA;});}}}if(tms->tl != 0){void*_tmpEB=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpEC;_LL7A: if(*((int*)_tmpEB)!= 
4)goto _LL7C;_tmpEC=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpEB)->f1;_LL7B:
typvars=_tmpEC;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE4  && _tmpE5 == 0) && _tmpE3 != 0) && _tmpE3->tl == 0) && (*((struct _tuple2*)
_tmpE3->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE3->hd)).f3 == (void*)0)_tmpE3=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE3);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE6,t,_tmpE3,_tmpE4,_tmpE5,_tmpE7,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE2)!= 0)goto _LL74;_tmpE8=((struct Cyc_Absyn_NoTypes_struct*)_tmpE2)->f2;_LL78:({
void*_tmpED[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(_tmpE8,({const char*_tmpEE="function declaration without parameter types";
_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,45));}),_tag_arr(_tmpED,
sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD5)!= 4)goto _LL70;_tmpDC=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD5)->f1;_tmpDD=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD5)->f2;_LL6F: if(tms->tl == 0)return({struct _tuple6 _tmpEF;_tmpEF.f1=tq;_tmpEF.f2=
t;_tmpEF.f3=_tmpDC;_tmpEF.f4=atts;_tmpEF;});({void*_tmpF0[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmpDD,({const char*_tmpF1="type parameters must appear before function arguments in declarator";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,68));}),_tag_arr(_tmpF0,
sizeof(void*),0));});_LL70: if(*((int*)_tmpD5)!= 2)goto _LL72;_tmpDE=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD5)->f1;_tmpDF=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD5)->f2;_LL71:
return Cyc_Parse_apply_tms(_tmpDF,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_PointerType_struct
_tmpF3;_tmpF3.tag=4;_tmpF3.f1=({struct Cyc_Absyn_PtrInfo _tmpF4;_tmpF4.elt_typ=(
void*)t;_tmpF4.elt_tq=tq;_tmpF4.ptr_atts=_tmpDE;_tmpF4;});_tmpF3;});_tmpF2;}),
atts,tms->tl);_LL72: if(*((int*)_tmpD5)!= 5)goto _LL67;_tmpE0=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD5)->f1;_tmpE1=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD5)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpE1),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpF6;struct Cyc_Core_Opt*_tmpF7;struct _tuple5 _tmpF5=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;if(_tmpF7 != 0)({void*_tmpF8[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmpF9="ignoring nested type declaration(s) in specifier list";
_tag_arr(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,54));}),_tag_arr(_tmpF8,
sizeof(void*),0));});return _tmpF6;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t){struct _tuple7 _tmpFB;struct
_tuple1*_tmpFC;struct Cyc_Absyn_Tqual _tmpFD;void*_tmpFE;struct Cyc_List_List*
_tmpFF;struct Cyc_List_List*_tmp100;struct _tuple7*_tmpFA=t;_tmpFB=*_tmpFA;_tmpFC=
_tmpFB.f1;_tmpFD=_tmpFB.f2;_tmpFE=_tmpFB.f3;_tmpFF=_tmpFB.f4;_tmp100=_tmpFB.f5;
Cyc_Lex_register_typedef(_tmpFC);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmp101=_tmpFE;struct Cyc_Core_Opt*_tmp102;_LL7F: if(_tmp101 <= (void*)3
 || *((int*)_tmp101)!= 0)goto _LL81;_tmp102=((struct Cyc_Absyn_Evar_struct*)
_tmp101)->f1;_LL80: type=0;if(_tmp102 == 0)kind=({struct Cyc_Core_Opt*_tmp103=
_cycalloc(sizeof(*_tmp103));_tmp103->v=(void*)((void*)2);_tmp103;});else{kind=
_tmp102;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104->v=(void*)_tmpFE;_tmp104;});goto _LL7E;_LL7E:;}
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp106;
_tmp106.tag=7;_tmp106.f1=({struct Cyc_Absyn_Typedefdecl*_tmp107=_cycalloc(sizeof(*
_tmp107));_tmp107->name=_tmpFC;_tmp107->tvs=_tmpFF;_tmp107->kind=kind;_tmp107->defn=
type;_tmp107->atts=_tmp100;_tmp107->tq=_tmpFD;_tmp107;});_tmp106;});_tmp105;}),
loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,
struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp109;_tmp109.tag=11;_tmp109.f1=d;_tmp109.f2=s;_tmp109;});_tmp108;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10B;struct Cyc_Absyn_Tqual _tmp10C;struct
Cyc_List_List*_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Parse_Declaration_spec*
_tmp10A=ds;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.tq;_tmp10D=_tmp10B.type_specs;
_tmp10E=_tmp10B.attributes;if(_tmp10C.loc == 0)_tmp10C.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp10F="inline is allowed only on function definitions";
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
_tmp11C;_LL93: if(_tmp119 <= (void*)2  || *((int*)_tmp119)!= 6)goto _LL95;_tmp11A=((
struct Cyc_Absyn_Enum_d_struct*)_tmp119)->f1;_LL94:(void*)(_tmp11A->sc=(void*)s);
if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp11D="bad attributes on enum";
_tag_arr(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,23));}),loc);goto _LL92;
_LL95: if(_tmp119 <= (void*)2  || *((int*)_tmp119)!= 4)goto _LL97;_tmp11B=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp119)->f1;_LL96:(void*)(_tmp11B->sc=(void*)s);
_tmp11B->attributes=_tmp10E;goto _LL92;_LL97: if(_tmp119 <= (void*)2  || *((int*)
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
_tmp131;_LL9C: if(_tmp121 <= (void*)3  || *((int*)_tmp121)!= 10)goto _LL9E;_tmp122=((
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
void*)3  || *((int*)_tmp121)!= 2)goto _LLA0;_tmp127=((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1;_tmp128=(void*)_tmp127.tunion_info;if(*((int*)_tmp128)!= 1)goto
_LLA0;_tmp129=((struct Cyc_Absyn_KnownTunion_struct*)_tmp128)->f1;_LL9F: if(
_tmp10E != 0)Cyc_Parse_err(({const char*_tmp139="bad attributes on tunion";
_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,25));}),loc);return({
struct Cyc_List_List*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));
_tmp13B[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp13C;_tmp13C.tag=5;_tmp13C.f1=*
_tmp129;_tmp13C;});_tmp13B;}),loc);_tmp13A->tl=0;_tmp13A;});_LLA0: if(_tmp121 <= (
void*)3  || *((int*)_tmp121)!= 2)goto _LLA2;_tmp12A=((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1;_tmp12B=(void*)_tmp12A.tunion_info;if(*((int*)_tmp12B)!= 0)goto
_LLA2;_tmp12C=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12B)->f1;_tmp12D=
_tmp12C.name;_tmp12E=_tmp12C.is_xtunion;_tmp12F=_tmp12A.targs;_LLA1: {struct Cyc_List_List*
_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12F);struct Cyc_Absyn_Decl*_tmp13E=Cyc_Absyn_tunion_decl(s,_tmp12D,
_tmp13D,0,_tmp12E,loc);if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp13F="bad attributes on tunion";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,25));}),loc);return({
struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->hd=_tmp13E;
_tmp140->tl=0;_tmp140;});}_LLA2: if(_tmp121 <= (void*)3  || *((int*)_tmp121)!= 12)
goto _LLA4;_tmp130=((struct Cyc_Absyn_EnumType_struct*)_tmp121)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp141=({struct Cyc_Absyn_Enumdecl*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->sc=(void*)s;_tmp147->name=_tmp130;_tmp147->fields=0;_tmp147;});
if(_tmp10E != 0)Cyc_Parse_err(({const char*_tmp142="bad attributes on enum";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,23));}),loc);return({
struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=({struct Cyc_Absyn_Decl*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp146;_tmp146.tag=6;_tmp146.f1=_tmp141;_tmp146;});_tmp145;}));_tmp144->loc=loc;
_tmp144;});_tmp143->tl=0;_tmp143;});}_LLA4: if(_tmp121 <= (void*)3  || *((int*)
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
_LLA9: if(_tmp153 <= (void*)2  || *((int*)_tmp153)!= 4)goto _LLAB;_tmp154=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp153)->f1;_LLAA:(void*)(_tmp154->sc=(void*)s);
_tmp154->attributes=_tmp10E;_tmp10E=0;goto _LLA8;_LLAB: if(_tmp153 <= (void*)2  || *((
int*)_tmp153)!= 5)goto _LLAD;_tmp155=((struct Cyc_Absyn_Tunion_d_struct*)_tmp153)->f1;
_LLAC:(void*)(_tmp155->sc=(void*)s);goto _LLA8;_LLAD: if(_tmp153 <= (void*)2  || *((
int*)_tmp153)!= 6)goto _LLAF;_tmp156=((struct Cyc_Absyn_Enum_d_struct*)_tmp153)->f1;
_LLAE:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp157="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,70));}),loc);goto _LLA8;
_LLA8:;}decls=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->hd=
d;_tmp158->tl=decls;_tmp158;});}return decls;}}else{if(_tmp115.f2 != 0)Cyc_Parse_unimp(({
const char*_tmp159="nested type declaration within declarator";_tag_arr(_tmp159,
sizeof(char),_get_zero_arr_size(_tmp159,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp15A=_tmp151;for(0;_tmp15A != 0;(_tmp15A=_tmp15A->tl,
_tmp114=_tmp114->tl)){struct _tuple7 _tmp15C;struct _tuple1*_tmp15D;struct Cyc_Absyn_Tqual
_tmp15E;void*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;
struct _tuple7*_tmp15B=(struct _tuple7*)_tmp15A->hd;_tmp15C=*_tmp15B;_tmp15D=
_tmp15C.f1;_tmp15E=_tmp15C.f2;_tmp15F=_tmp15C.f3;_tmp160=_tmp15C.f4;_tmp161=
_tmp15C.f5;if(_tmp160 != 0)({void*_tmp162[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp163="bad type params, ignoring";_tag_arr(_tmp163,sizeof(char),
_get_zero_arr_size(_tmp163,26));}),_tag_arr(_tmp162,sizeof(void*),0));});if(
_tmp114 == 0)({void*_tmp164[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp165="unexpected NULL in parse!";
_tag_arr(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,26));}),_tag_arr(
_tmp164,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)
_tmp114->hd;struct Cyc_Absyn_Vardecl*_tmp167=Cyc_Absyn_new_vardecl(_tmp15D,
_tmp15F,_tmp166);_tmp167->tq=_tmp15E;(void*)(_tmp167->sc=(void*)s);_tmp167->attributes=
_tmp161;{struct Cyc_Absyn_Decl*_tmp168=({struct Cyc_Absyn_Decl*_tmp16A=_cycalloc(
sizeof(*_tmp16A));_tmp16A->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=_tmp167;_tmp16C;});_tmp16B;}));_tmp16A->loc=loc;
_tmp16A;});decls=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));
_tmp169->hd=_tmp168;_tmp169->tl=decls;_tmp169;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_tagged_arr)s)== 1)switch(*((const char*)_check_unknown_subscript(s,sizeof(char),
0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3:
return(void*)2;case 'R': _LLB4: return(void*)3;case 'E': _LLB5: return(void*)4;case 'I':
_LLB6: return(void*)5;default: _LLB7: break;}Cyc_Parse_err((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _tagged_arr)((
struct _tagged_arr)s);{void*_tmp16D[1]={& _tmp16F};Cyc_aprintf(({const char*_tmp16E="bad kind: %s";
_tag_arr(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,13));}),_tag_arr(
_tmp16D,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp170=(void*)p->r;struct _tuple1*_tmp171;struct Cyc_Absyn_Vardecl*
_tmp172;struct Cyc_Absyn_Pat*_tmp173;void*_tmp174;int _tmp175;char _tmp176;struct
_tagged_arr _tmp177;struct _tuple1*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Exp*
_tmp17A;_LLBA: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 12)goto _LLBC;_tmp171=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp170)->f1;_LLBB: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));
_tmp17B[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp17C;_tmp17C.tag=2;_tmp17C.f1=
_tmp171;_tmp17C;});_tmp17B;}),p->loc);_LLBC: if(_tmp170 <= (void*)2  || *((int*)
_tmp170)!= 1)goto _LLBE;_tmp172=((struct Cyc_Absyn_Reference_p_struct*)_tmp170)->f1;
_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp17E;_tmp17E.tag=2;_tmp17E.f1=_tmp172->name;_tmp17E;});_tmp17D;}),p->loc),p->loc);
_LLBE: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 4)goto _LLC0;_tmp173=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp170)->f1;_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp173),p->loc);_LLC0: if((int)_tmp170 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(
p->loc);_LLC2: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 7)goto _LLC4;_tmp174=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp170)->f1;_tmp175=((struct Cyc_Absyn_Int_p_struct*)
_tmp170)->f2;_LLC3: return Cyc_Absyn_int_exp(_tmp174,_tmp175,p->loc);_LLC4: if(
_tmp170 <= (void*)2  || *((int*)_tmp170)!= 8)goto _LLC6;_tmp176=((struct Cyc_Absyn_Char_p_struct*)
_tmp170)->f1;_LLC5: return Cyc_Absyn_char_exp(_tmp176,p->loc);_LLC6: if(_tmp170 <= (
void*)2  || *((int*)_tmp170)!= 9)goto _LLC8;_tmp177=((struct Cyc_Absyn_Float_p_struct*)
_tmp170)->f1;_LLC7: return Cyc_Absyn_float_exp(_tmp177,p->loc);_LLC8: if(_tmp170 <= (
void*)2  || *((int*)_tmp170)!= 13)goto _LLCA;_tmp178=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp170)->f1;_tmp179=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp170)->f2;_LLC9: {
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp180;_tmp180.tag=2;_tmp180.f1=_tmp178;_tmp180;});_tmp17F;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp179);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 14)goto _LLCC;
_tmp17A=((struct Cyc_Absyn_Exp_p_struct*)_tmp170)->f1;_LLCB: return _tmp17A;_LLCC:;
_LLCD: Cyc_Parse_err(({const char*_tmp181="cannot mix patterns and expressions in case";
_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,44));}),p->loc);return
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
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchClauseList_tok[
25]="\000\000\000\000SwitchClauseList_tok\000";struct Cyc_SwitchClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[24]="\000\000\000\000TunionFieldList_tok\000";
struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[
20]="\000\000\000\000TunionField_tok\000";struct Cyc_TunionField_tok_struct{char*
tag;struct Cyc_Absyn_Tunionfield*f1;};char Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok\000";
struct Cyc_TypeList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[
25]="\000\000\000\000TypeModifierList_tok\000";struct Cyc_TypeModifierList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[16]="\000\000\000\000TypeOpt_tok\000";
struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[
17]="\000\000\000\000TypeQual_tok\000";struct Cyc_TypeQual_tok_struct{char*tag;
struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[22]="\000\000\000\000TypeSpecifier_tok\000";
struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok\000";
struct Cyc_Type_tok_struct{char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1\000";
struct _tuple16{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*
f3;};struct Cyc_YY1_struct{char*tag;struct _tuple16*f1;};char Cyc_YY2[8]="\000\000\000\000YY2\000";
struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY3[8]="\000\000\000\000YY3\000";
struct Cyc_YY3_struct{char*tag;struct _tuple14*f1;};char Cyc_YY4[8]="\000\000\000\000YY4\000";
struct Cyc_YY4_struct{char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY5[8]="\000\000\000\000YY5\000";
struct Cyc_YY5_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL\000";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";static char _tmp183[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp183,
_tmp183,_tmp183 + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp184=yy1;struct _tuple13*_tmp185;_LLCF: if(*((void**)_tmp184)!= Cyc_Int_tok)
goto _LLD1;_tmp185=((struct Cyc_Int_tok_struct*)_tmp184)->f1;_LLD0: yyzzz=_tmp185;
goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLCE:;}return
yyzzz;}static char _tmp187[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp187,_tmp187,_tmp187 + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp188=yy1;char _tmp189;_LLD4: if(*((void**)_tmp188)!= Cyc_Char_tok)
goto _LLD6;_tmp189=((struct Cyc_Char_tok_struct*)_tmp188)->f1;_LLD5: yyzzz=_tmp189;
goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return
yyzzz;}static char _tmp18B[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp18B,_tmp18B,_tmp18B + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp18C=yy1;struct _tagged_arr _tmp18D;
_LLD9: if(*((void**)_tmp18C)!= Cyc_String_tok)goto _LLDB;_tmp18D=((struct Cyc_String_tok_struct*)
_tmp18C)->f1;_LLDA: yyzzz=_tmp18D;goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLD8:;}return yyzzz;}static char _tmp18F[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp18F,
_tmp18F,_tmp18F + 54}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*
yyzzz;{void*_tmp190=yy1;struct _tuple16*_tmp191;_LLDE: if(*((void**)_tmp190)!= Cyc_YY1)
goto _LLE0;_tmp191=((struct Cyc_YY1_struct*)_tmp190)->f1;_LLDF: yyzzz=_tmp191;goto
_LLDD;_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}
static char _tmp193[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp194=yy1;struct Cyc_Absyn_Conref*
_tmp195;_LLE3: if(*((void**)_tmp194)!= Cyc_YY2)goto _LLE5;_tmp195=((struct Cyc_YY2_struct*)
_tmp194)->f1;_LLE4: yyzzz=_tmp195;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLE2:;}return yyzzz;}static char _tmp197[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp197,_tmp197,_tmp197 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp198=yy1;struct
Cyc_Absyn_Exp*_tmp199;_LLE8: if(*((void**)_tmp198)!= Cyc_Exp_tok)goto _LLEA;
_tmp199=((struct Cyc_Exp_tok_struct*)_tmp198)->f1;_LLE9: yyzzz=_tmp199;goto _LLE7;
_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLE7:;}return yyzzz;}static
char _tmp19B[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19C=yy1;struct Cyc_List_List*
_tmp19D;_LLED: if(*((void**)_tmp19C)!= Cyc_ExpList_tok)goto _LLEF;_tmp19D=((struct
Cyc_ExpList_tok_struct*)_tmp19C)->f1;_LLEE: yyzzz=_tmp19D;goto _LLEC;_LLEF:;_LLF0:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLEC:;}return yyzzz;}static char
_tmp19F[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp19F,_tmp19F,_tmp19F + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1A0=yy1;struct Cyc_List_List*_tmp1A1;_LLF2: if(*((void**)_tmp1A0)
!= Cyc_InitializerList_tok)goto _LLF4;_tmp1A1=((struct Cyc_InitializerList_tok_struct*)
_tmp1A0)->f1;_LLF3: yyzzz=_tmp1A1;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLF1:;}return yyzzz;}static char _tmp1A3[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp1A3,_tmp1A3,_tmp1A3 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp1A4=yy1;void*_tmp1A5;_LLF7: if(*((void**)_tmp1A4)
!= Cyc_Primop_tok)goto _LLF9;_tmp1A5=(void*)((struct Cyc_Primop_tok_struct*)
_tmp1A4)->f1;_LLF8: yyzzz=_tmp1A5;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLF6:;}return yyzzz;}static char _tmp1A7[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp1A7,_tmp1A7,_tmp1A7 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp1A8=yy1;struct Cyc_Core_Opt*_tmp1A9;_LLFC: if(*((void**)_tmp1A8)!= Cyc_Primopopt_tok)
goto _LLFE;_tmp1A9=((struct Cyc_Primopopt_tok_struct*)_tmp1A8)->f1;_LLFD: yyzzz=
_tmp1A9;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLFB:;}return yyzzz;}static char _tmp1AB[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1AB,_tmp1AB,_tmp1AB + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp1AC=yy1;
struct _tuple1*_tmp1AD;_LL101: if(*((void**)_tmp1AC)!= Cyc_QualId_tok)goto _LL103;
_tmp1AD=((struct Cyc_QualId_tok_struct*)_tmp1AC)->f1;_LL102: yyzzz=_tmp1AD;goto
_LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL100:;}return
yyzzz;}static char _tmp1AF[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1AF,_tmp1AF,_tmp1AF + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1B0=yy1;struct Cyc_Absyn_Stmt*
_tmp1B1;_LL106: if(*((void**)_tmp1B0)!= Cyc_Stmt_tok)goto _LL108;_tmp1B1=((struct
Cyc_Stmt_tok_struct*)_tmp1B0)->f1;_LL107: yyzzz=_tmp1B1;goto _LL105;_LL108:;_LL109:(
int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL105:;}return yyzzz;}static char _tmp1B3[
24]="list_t<switch_clause_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B4=yy1;struct Cyc_List_List*
_tmp1B5;_LL10B: if(*((void**)_tmp1B4)!= Cyc_SwitchClauseList_tok)goto _LL10D;
_tmp1B5=((struct Cyc_SwitchClauseList_tok_struct*)_tmp1B4)->f1;_LL10C: yyzzz=
_tmp1B5;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL10A:;}return yyzzz;}static char _tmp1B7[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1B7,_tmp1B7,_tmp1B7 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1B8=yy1;
struct Cyc_Absyn_Pat*_tmp1B9;_LL110: if(*((void**)_tmp1B8)!= Cyc_Pattern_tok)goto
_LL112;_tmp1B9=((struct Cyc_Pattern_tok_struct*)_tmp1B8)->f1;_LL111: yyzzz=_tmp1B9;
goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL10F:;}
return yyzzz;}static char _tmp1BB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,_tmp1BB + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1BC=yy1;struct Cyc_List_List*_tmp1BD;_LL115: if(*((void**)_tmp1BC)
!= Cyc_PatternList_tok)goto _LL117;_tmp1BD=((struct Cyc_PatternList_tok_struct*)
_tmp1BC)->f1;_LL116: yyzzz=_tmp1BD;goto _LL114;_LL117:;_LL118:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL114:;}return yyzzz;}static char _tmp1BF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1BF,_tmp1BF,_tmp1BF + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp1C0=yy1;struct _tuple11*_tmp1C1;_LL11A: if(*((void**)
_tmp1C0)!= Cyc_FieldPattern_tok)goto _LL11C;_tmp1C1=((struct Cyc_FieldPattern_tok_struct*)
_tmp1C0)->f1;_LL11B: yyzzz=_tmp1C1;goto _LL119;_LL11C:;_LL11D:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL119:;}return yyzzz;}static char _tmp1C3[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1C3,_tmp1C3,_tmp1C3 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C4=yy1;struct Cyc_List_List*
_tmp1C5;_LL11F: if(*((void**)_tmp1C4)!= Cyc_FieldPatternList_tok)goto _LL121;
_tmp1C5=((struct Cyc_FieldPatternList_tok_struct*)_tmp1C4)->f1;_LL120: yyzzz=
_tmp1C5;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL11E:;}return yyzzz;}static char _tmp1C7[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1C7,_tmp1C7,_tmp1C7 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1C8=yy1;
struct Cyc_Absyn_Fndecl*_tmp1C9;_LL124: if(*((void**)_tmp1C8)!= Cyc_FnDecl_tok)
goto _LL126;_tmp1C9=((struct Cyc_FnDecl_tok_struct*)_tmp1C8)->f1;_LL125: yyzzz=
_tmp1C9;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL123:;}return yyzzz;}static char _tmp1CB[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;_LL129: if(*((void**)_tmp1CC)!= Cyc_DeclList_tok)
goto _LL12B;_tmp1CD=((struct Cyc_DeclList_tok_struct*)_tmp1CC)->f1;_LL12A: yyzzz=
_tmp1CD;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL128:;}return yyzzz;}static char _tmp1CF[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1D0=yy1;struct Cyc_Parse_Declaration_spec*_tmp1D1;_LL12E: if(*((
void**)_tmp1D0)!= Cyc_DeclSpec_tok)goto _LL130;_tmp1D1=((struct Cyc_DeclSpec_tok_struct*)
_tmp1D0)->f1;_LL12F: yyzzz=_tmp1D1;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL12D:;}return yyzzz;}static char _tmp1D3[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1D3,_tmp1D3,_tmp1D3 + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1D4=yy1;struct _tuple12*_tmp1D5;_LL133: if(*((void**)
_tmp1D4)!= Cyc_InitDecl_tok)goto _LL135;_tmp1D5=((struct Cyc_InitDecl_tok_struct*)
_tmp1D4)->f1;_LL134: yyzzz=_tmp1D5;goto _LL132;_LL135:;_LL136:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL132:;}return yyzzz;}static char _tmp1D7[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1D7,_tmp1D7,_tmp1D7 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D8=yy1;struct Cyc_List_List*
_tmp1D9;_LL138: if(*((void**)_tmp1D8)!= Cyc_InitDeclList_tok)goto _LL13A;_tmp1D9=((
struct Cyc_InitDeclList_tok_struct*)_tmp1D8)->f1;_LL139: yyzzz=_tmp1D9;goto _LL137;
_LL13A:;_LL13B:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL137:;}return
yyzzz;}static char _tmp1DB[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1DC=yy1;void*
_tmp1DD;_LL13D: if(*((void**)_tmp1DC)!= Cyc_StorageClass_tok)goto _LL13F;_tmp1DD=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1DC)->f1;_LL13E: yyzzz=_tmp1DD;
goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL13C:;}return yyzzz;}static char _tmp1DF[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1E0=yy1;void*
_tmp1E1;_LL142: if(*((void**)_tmp1E0)!= Cyc_TypeSpecifier_tok)goto _LL144;_tmp1E1=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1E0)->f1;_LL143: yyzzz=_tmp1E1;
goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL141:;}return yyzzz;}static char _tmp1E3[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1E4=yy1;void*
_tmp1E5;_LL147: if(*((void**)_tmp1E4)!= Cyc_StructOrUnion_tok)goto _LL149;_tmp1E5=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1E4)->f1;_LL148: yyzzz=_tmp1E5;
goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL146:;}return yyzzz;}static char _tmp1E7[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1E7,_tmp1E7,_tmp1E7 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1E8=yy1;struct Cyc_Absyn_Tqual _tmp1E9;_LL14C: if(*((void**)_tmp1E8)!= Cyc_TypeQual_tok)
goto _LL14E;_tmp1E9=((struct Cyc_TypeQual_tok_struct*)_tmp1E8)->f1;_LL14D: yyzzz=
_tmp1E9;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL14B:;}return yyzzz;}static char _tmp1EB[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1EB,_tmp1EB,_tmp1EB + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EC=yy1;struct Cyc_List_List*
_tmp1ED;_LL151: if(*((void**)_tmp1EC)!= Cyc_AggrFieldDeclList_tok)goto _LL153;
_tmp1ED=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1EC)->f1;_LL152: yyzzz=
_tmp1ED;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL150:;}return yyzzz;}static char _tmp1EF[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1EF,_tmp1EF,_tmp1EF + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F0=yy1;struct Cyc_List_List*
_tmp1F1;_LL156: if(*((void**)_tmp1F0)!= Cyc_AggrFieldDeclListList_tok)goto _LL158;
_tmp1F1=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1F0)->f1;_LL157: yyzzz=
_tmp1F1;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL155:;}return yyzzz;}static char _tmp1F3[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1F3,_tmp1F3,_tmp1F3 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F4=yy1;struct Cyc_List_List*
_tmp1F5;_LL15B: if(*((void**)_tmp1F4)!= Cyc_TypeModifierList_tok)goto _LL15D;
_tmp1F5=((struct Cyc_TypeModifierList_tok_struct*)_tmp1F4)->f1;_LL15C: yyzzz=
_tmp1F5;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL15A:;}return yyzzz;}static char _tmp1F7[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1F8=yy1;struct Cyc_Parse_Declarator*_tmp1F9;_LL160: if(*((void**)
_tmp1F8)!= Cyc_Declarator_tok)goto _LL162;_tmp1F9=((struct Cyc_Declarator_tok_struct*)
_tmp1F8)->f1;_LL161: yyzzz=_tmp1F9;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL15F:;}return yyzzz;}static char _tmp1FB[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1FB,_tmp1FB,_tmp1FB + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1FC=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1FD;_LL165: if(*((void**)_tmp1FC)!= Cyc_AbstractDeclarator_tok)goto _LL167;
_tmp1FD=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1FC)->f1;_LL166: yyzzz=
_tmp1FD;goto _LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL164:;}return yyzzz;}static char _tmp1FF[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp200=yy1;int _tmp201;_LL16A: if(*((void**)_tmp200)!= 
Cyc_Bool_tok)goto _LL16C;_tmp201=((struct Cyc_Bool_tok_struct*)_tmp200)->f1;_LL16B:
yyzzz=_tmp201;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL169:;}return yyzzz;}static char _tmp203[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp204=yy1;void*_tmp205;_LL16F: if(*((void**)_tmp204)
!= Cyc_Scope_tok)goto _LL171;_tmp205=(void*)((struct Cyc_Scope_tok_struct*)_tmp204)->f1;
_LL170: yyzzz=_tmp205;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL16E:;}return yyzzz;}static char _tmp207[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp208=yy1;struct Cyc_Absyn_Tunionfield*_tmp209;_LL174: if(*((void**)
_tmp208)!= Cyc_TunionField_tok)goto _LL176;_tmp209=((struct Cyc_TunionField_tok_struct*)
_tmp208)->f1;_LL175: yyzzz=_tmp209;goto _LL173;_LL176:;_LL177:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL173:;}return yyzzz;}static char _tmp20B[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp20B,_tmp20B,_tmp20B + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20C=yy1;struct Cyc_List_List*
_tmp20D;_LL179: if(*((void**)_tmp20C)!= Cyc_TunionFieldList_tok)goto _LL17B;
_tmp20D=((struct Cyc_TunionFieldList_tok_struct*)_tmp20C)->f1;_LL17A: yyzzz=
_tmp20D;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL178:;}return yyzzz;}static char _tmp20F[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp20F,_tmp20F,_tmp20F + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp210=yy1;struct _tuple15*_tmp211;_LL17E: if(*((void**)
_tmp210)!= Cyc_QualSpecList_tok)goto _LL180;_tmp211=((struct Cyc_QualSpecList_tok_struct*)
_tmp210)->f1;_LL17F: yyzzz=_tmp211;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL17D:;}return yyzzz;}static char _tmp213[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp213,_tmp213,_tmp213 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp214=yy1;struct Cyc_List_List*_tmp215;_LL183:
if(*((void**)_tmp214)!= Cyc_IdList_tok)goto _LL185;_tmp215=((struct Cyc_IdList_tok_struct*)
_tmp214)->f1;_LL184: yyzzz=_tmp215;goto _LL182;_LL185:;_LL186:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL182:;}return yyzzz;}static char _tmp217[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp217,_tmp217,_tmp217 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp218=yy1;struct _tuple2*_tmp219;_LL188: if(*((void**)
_tmp218)!= Cyc_ParamDecl_tok)goto _LL18A;_tmp219=((struct Cyc_ParamDecl_tok_struct*)
_tmp218)->f1;_LL189: yyzzz=_tmp219;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL187:;}return yyzzz;}static char _tmp21B[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp21B,_tmp21B,_tmp21B + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp21C=yy1;struct Cyc_List_List*
_tmp21D;_LL18D: if(*((void**)_tmp21C)!= Cyc_ParamDeclList_tok)goto _LL18F;_tmp21D=((
struct Cyc_ParamDeclList_tok_struct*)_tmp21C)->f1;_LL18E: yyzzz=_tmp21D;goto _LL18C;
_LL18F:;_LL190:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL18C:;}return
yyzzz;}static char _tmp21F[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp21F,
_tmp21F,_tmp21F + 107}};struct _tuple14*Cyc_yyget_YY3(void*yy1){struct _tuple14*
yyzzz;{void*_tmp220=yy1;struct _tuple14*_tmp221;_LL192: if(*((void**)_tmp220)!= 
Cyc_YY3)goto _LL194;_tmp221=((struct Cyc_YY3_struct*)_tmp220)->f1;_LL193: yyzzz=
_tmp221;goto _LL191;_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_YY3);_LL191:;}
return yyzzz;}static char _tmp223[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp224=yy1;struct Cyc_List_List*_tmp225;_LL197: if(*((void**)_tmp224)!= Cyc_TypeList_tok)
goto _LL199;_tmp225=((struct Cyc_TypeList_tok_struct*)_tmp224)->f1;_LL198: yyzzz=
_tmp225;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL196:;}return yyzzz;}static char _tmp227[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp227,
_tmp227,_tmp227 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp228=yy1;struct Cyc_List_List*_tmp229;_LL19C:
if(*((void**)_tmp228)!= Cyc_DesignatorList_tok)goto _LL19E;_tmp229=((struct Cyc_DesignatorList_tok_struct*)
_tmp228)->f1;_LL19D: yyzzz=_tmp229;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL19B:;}return yyzzz;}static char _tmp22B[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp22B,_tmp22B,_tmp22B + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp22C=yy1;void*_tmp22D;_LL1A1: if(*((void**)_tmp22C)!= Cyc_Designator_tok)
goto _LL1A3;_tmp22D=(void*)((struct Cyc_Designator_tok_struct*)_tmp22C)->f1;_LL1A2:
yyzzz=_tmp22D;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL1A0:;}return yyzzz;}static char _tmp22F[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp230=yy1;void*_tmp231;_LL1A6: if(*((void**)_tmp230)
!= Cyc_Kind_tok)goto _LL1A8;_tmp231=(void*)((struct Cyc_Kind_tok_struct*)_tmp230)->f1;
_LL1A7: yyzzz=_tmp231;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL1A5:;}return yyzzz;}static char _tmp233[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp234=yy1;void*_tmp235;_LL1AB: if(*((void**)_tmp234)
!= Cyc_Type_tok)goto _LL1AD;_tmp235=(void*)((struct Cyc_Type_tok_struct*)_tmp234)->f1;
_LL1AC: yyzzz=_tmp235;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL1AA:;}return yyzzz;}static char _tmp237[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp237,
_tmp237,_tmp237 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp238=yy1;struct Cyc_List_List*_tmp239;_LL1B0:
if(*((void**)_tmp238)!= Cyc_AttributeList_tok)goto _LL1B2;_tmp239=((struct Cyc_AttributeList_tok_struct*)
_tmp238)->f1;_LL1B1: yyzzz=_tmp239;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1AF:;}return yyzzz;}static char _tmp23B[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp23B,_tmp23B,_tmp23B + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp23C=yy1;void*_tmp23D;_LL1B5: if(*((void**)_tmp23C)!= Cyc_Attribute_tok)
goto _LL1B7;_tmp23D=(void*)((struct Cyc_Attribute_tok_struct*)_tmp23C)->f1;_LL1B6:
yyzzz=_tmp23D;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1B4:;}return yyzzz;}static char _tmp23F[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp240=yy1;struct Cyc_Absyn_Enumfield*_tmp241;_LL1BA: if(*((void**)
_tmp240)!= Cyc_Enumfield_tok)goto _LL1BC;_tmp241=((struct Cyc_Enumfield_tok_struct*)
_tmp240)->f1;_LL1BB: yyzzz=_tmp241;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1B9:;}return yyzzz;}static char _tmp243[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp243,_tmp243,_tmp243 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp244=yy1;struct Cyc_List_List*
_tmp245;_LL1BF: if(*((void**)_tmp244)!= Cyc_EnumfieldList_tok)goto _LL1C1;_tmp245=((
struct Cyc_EnumfieldList_tok_struct*)_tmp244)->f1;_LL1C0: yyzzz=_tmp245;goto _LL1BE;
_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1BE:;}return
yyzzz;}static char _tmp247[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp248=yy1;struct Cyc_Core_Opt*_tmp249;_LL1C4: if(*((void**)_tmp248)!= Cyc_TypeOpt_tok)
goto _LL1C6;_tmp249=((struct Cyc_TypeOpt_tok_struct*)_tmp248)->f1;_LL1C5: yyzzz=
_tmp249;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1C3:;}return yyzzz;}static char _tmp24B[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp24B,
_tmp24B,_tmp24B + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp24C=yy1;struct Cyc_List_List*_tmp24D;_LL1C9:
if(*((void**)_tmp24C)!= Cyc_Rgnorder_tok)goto _LL1CB;_tmp24D=((struct Cyc_Rgnorder_tok_struct*)
_tmp24C)->f1;_LL1CA: yyzzz=_tmp24D;goto _LL1C8;_LL1CB:;_LL1CC:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1C8:;}return yyzzz;}static char _tmp24F[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp24F,
_tmp24F,_tmp24F + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY4(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp250=yy1;struct Cyc_Absyn_Conref*_tmp251;_LL1CE: if(*((void**)
_tmp250)!= Cyc_YY4)goto _LL1D0;_tmp251=((struct Cyc_YY4_struct*)_tmp250)->f1;
_LL1CF: yyzzz=_tmp251;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL1CD:;}return yyzzz;}static char _tmp253[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp253,
_tmp253,_tmp253 + 40}};struct Cyc_List_List*Cyc_yyget_YY5(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp254=yy1;struct Cyc_List_List*_tmp255;_LL1D3: if(*((void**)_tmp254)
!= Cyc_YY5)goto _LL1D5;_tmp255=((struct Cyc_YY5_struct*)_tmp254)->f1;_LL1D4: yyzzz=
_tmp255;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY5);_LL1D2:;}
return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp256;_tmp256.timestamp=0;_tmp256.first_line=0;_tmp256.first_column=0;_tmp256.last_line=
0;_tmp256.last_column=0;_tmp256;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[357]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,128,2,2,110,126,123,2,107,108,116,120,105,124,113,125,2,2,2,2,
2,2,2,2,2,2,112,102,111,106,119,118,117,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,114,2,115,122,109,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,103,121,104,127,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101};static char _tmp257[2]="$";static char _tmp258[6]="error";static char
_tmp259[12]="$undefined.";static char _tmp25A[5]="AUTO";static char _tmp25B[9]="REGISTER";
static char _tmp25C[7]="STATIC";static char _tmp25D[7]="EXTERN";static char _tmp25E[8]="TYPEDEF";
static char _tmp25F[5]="VOID";static char _tmp260[5]="CHAR";static char _tmp261[6]="SHORT";
static char _tmp262[4]="INT";static char _tmp263[5]="LONG";static char _tmp264[6]="FLOAT";
static char _tmp265[7]="DOUBLE";static char _tmp266[7]="SIGNED";static char _tmp267[9]="UNSIGNED";
static char _tmp268[6]="CONST";static char _tmp269[9]="VOLATILE";static char _tmp26A[9]="RESTRICT";
static char _tmp26B[7]="STRUCT";static char _tmp26C[6]="UNION";static char _tmp26D[5]="CASE";
static char _tmp26E[8]="DEFAULT";static char _tmp26F[7]="INLINE";static char _tmp270[7]="SIZEOF";
static char _tmp271[9]="OFFSETOF";static char _tmp272[3]="IF";static char _tmp273[5]="ELSE";
static char _tmp274[7]="SWITCH";static char _tmp275[6]="WHILE";static char _tmp276[3]="DO";
static char _tmp277[4]="FOR";static char _tmp278[5]="GOTO";static char _tmp279[9]="CONTINUE";
static char _tmp27A[6]="BREAK";static char _tmp27B[7]="RETURN";static char _tmp27C[5]="ENUM";
static char _tmp27D[8]="NULL_kw";static char _tmp27E[4]="LET";static char _tmp27F[6]="THROW";
static char _tmp280[4]="TRY";static char _tmp281[6]="CATCH";static char _tmp282[7]="EXPORT";
static char _tmp283[4]="NEW";static char _tmp284[9]="ABSTRACT";static char _tmp285[9]="FALLTHRU";
static char _tmp286[6]="USING";static char _tmp287[10]="NAMESPACE";static char _tmp288[
7]="TUNION";static char _tmp289[8]="XTUNION";static char _tmp28A[7]="MALLOC";static
char _tmp28B[8]="RMALLOC";static char _tmp28C[7]="CALLOC";static char _tmp28D[8]="RCALLOC";
static char _tmp28E[9]="REGION_T";static char _tmp28F[9]="SIZEOF_T";static char
_tmp290[6]="TAG_T";static char _tmp291[7]="REGION";static char _tmp292[5]="RNEW";
static char _tmp293[8]="REGIONS";static char _tmp294[13]="RESET_REGION";static char
_tmp295[4]="GEN";static char _tmp296[14]="NOZEROTERM_kw";static char _tmp297[12]="ZEROTERM_kw";
static char _tmp298[7]="PORTON";static char _tmp299[8]="PORTOFF";static char _tmp29A[7]="PTR_OP";
static char _tmp29B[7]="INC_OP";static char _tmp29C[7]="DEC_OP";static char _tmp29D[8]="LEFT_OP";
static char _tmp29E[9]="RIGHT_OP";static char _tmp29F[6]="LE_OP";static char _tmp2A0[6]="GE_OP";
static char _tmp2A1[6]="EQ_OP";static char _tmp2A2[6]="NE_OP";static char _tmp2A3[7]="AND_OP";
static char _tmp2A4[6]="OR_OP";static char _tmp2A5[11]="MUL_ASSIGN";static char
_tmp2A6[11]="DIV_ASSIGN";static char _tmp2A7[11]="MOD_ASSIGN";static char _tmp2A8[11]="ADD_ASSIGN";
static char _tmp2A9[11]="SUB_ASSIGN";static char _tmp2AA[12]="LEFT_ASSIGN";static
char _tmp2AB[13]="RIGHT_ASSIGN";static char _tmp2AC[11]="AND_ASSIGN";static char
_tmp2AD[11]="XOR_ASSIGN";static char _tmp2AE[10]="OR_ASSIGN";static char _tmp2AF[9]="ELLIPSIS";
static char _tmp2B0[11]="LEFT_RIGHT";static char _tmp2B1[12]="COLON_COLON";static
char _tmp2B2[11]="IDENTIFIER";static char _tmp2B3[17]="INTEGER_CONSTANT";static char
_tmp2B4[7]="STRING";static char _tmp2B5[19]="CHARACTER_CONSTANT";static char _tmp2B6[
18]="FLOATING_CONSTANT";static char _tmp2B7[9]="TYPE_VAR";static char _tmp2B8[13]="TYPEDEF_NAME";
static char _tmp2B9[16]="QUAL_IDENTIFIER";static char _tmp2BA[18]="QUAL_TYPEDEF_NAME";
static char _tmp2BB[13]="TYPE_INTEGER";static char _tmp2BC[10]="ATTRIBUTE";static
char _tmp2BD[4]="';'";static char _tmp2BE[4]="'{'";static char _tmp2BF[4]="'}'";
static char _tmp2C0[4]="','";static char _tmp2C1[4]="'='";static char _tmp2C2[4]="'('";
static char _tmp2C3[4]="')'";static char _tmp2C4[4]="'_'";static char _tmp2C5[4]="'$'";
static char _tmp2C6[4]="'<'";static char _tmp2C7[4]="':'";static char _tmp2C8[4]="'.'";
static char _tmp2C9[4]="'['";static char _tmp2CA[4]="']'";static char _tmp2CB[4]="'*'";
static char _tmp2CC[4]="'@'";static char _tmp2CD[4]="'?'";static char _tmp2CE[4]="'>'";
static char _tmp2CF[4]="'+'";static char _tmp2D0[4]="'|'";static char _tmp2D1[4]="'^'";
static char _tmp2D2[4]="'&'";static char _tmp2D3[4]="'-'";static char _tmp2D4[4]="'/'";
static char _tmp2D5[4]="'%'";static char _tmp2D6[4]="'~'";static char _tmp2D7[4]="'!'";
static char _tmp2D8[5]="prog";static char _tmp2D9[17]="translation_unit";static char
_tmp2DA[12]="export_list";static char _tmp2DB[19]="export_list_values";static char
_tmp2DC[21]="external_declaration";static char _tmp2DD[20]="function_definition";
static char _tmp2DE[21]="function_definition2";static char _tmp2DF[13]="using_action";
static char _tmp2E0[15]="unusing_action";static char _tmp2E1[17]="namespace_action";
static char _tmp2E2[19]="unnamespace_action";static char _tmp2E3[12]="declaration";
static char _tmp2E4[17]="declaration_list";static char _tmp2E5[23]="declaration_specifiers";
static char _tmp2E6[24]="storage_class_specifier";static char _tmp2E7[15]="attributes_opt";
static char _tmp2E8[11]="attributes";static char _tmp2E9[15]="attribute_list";static
char _tmp2EA[10]="attribute";static char _tmp2EB[15]="type_specifier";static char
_tmp2EC[25]="type_specifier_notypedef";static char _tmp2ED[5]="kind";static char
_tmp2EE[15]="type_qualifier";static char _tmp2EF[15]="enum_specifier";static char
_tmp2F0[11]="enum_field";static char _tmp2F1[22]="enum_declaration_list";static
char _tmp2F2[26]="struct_or_union_specifier";static char _tmp2F3[16]="type_params_opt";
static char _tmp2F4[16]="struct_or_union";static char _tmp2F5[24]="struct_declaration_list";
static char _tmp2F6[25]="struct_declaration_list0";static char _tmp2F7[21]="init_declarator_list";
static char _tmp2F8[22]="init_declarator_list0";static char _tmp2F9[16]="init_declarator";
static char _tmp2FA[19]="struct_declaration";static char _tmp2FB[25]="specifier_qualifier_list";
static char _tmp2FC[35]="notypedef_specifier_qualifier_list";static char _tmp2FD[23]="struct_declarator_list";
static char _tmp2FE[24]="struct_declarator_list0";static char _tmp2FF[18]="struct_declarator";
static char _tmp300[17]="tunion_specifier";static char _tmp301[18]="tunion_or_xtunion";
static char _tmp302[17]="tunionfield_list";static char _tmp303[18]="tunionfield_scope";
static char _tmp304[12]="tunionfield";static char _tmp305[11]="declarator";static
char _tmp306[23]="declarator_withtypedef";static char _tmp307[18]="direct_declarator";
static char _tmp308[30]="direct_declarator_withtypedef";static char _tmp309[8]="pointer";
static char _tmp30A[12]="one_pointer";static char _tmp30B[23]="pointer_null_and_bound";
static char _tmp30C[14]="pointer_bound";static char _tmp30D[18]="zeroterm_qual_opt";
static char _tmp30E[8]="rgn_opt";static char _tmp30F[11]="tqual_list";static char
_tmp310[20]="parameter_type_list";static char _tmp311[9]="type_var";static char
_tmp312[16]="optional_effect";static char _tmp313[19]="optional_rgn_order";static
char _tmp314[10]="rgn_order";static char _tmp315[16]="optional_inject";static char
_tmp316[11]="effect_set";static char _tmp317[14]="atomic_effect";static char _tmp318[
11]="region_set";static char _tmp319[15]="parameter_list";static char _tmp31A[22]="parameter_declaration";
static char _tmp31B[16]="identifier_list";static char _tmp31C[17]="identifier_list0";
static char _tmp31D[12]="initializer";static char _tmp31E[18]="array_initializer";
static char _tmp31F[17]="initializer_list";static char _tmp320[12]="designation";
static char _tmp321[16]="designator_list";static char _tmp322[11]="designator";
static char _tmp323[10]="type_name";static char _tmp324[14]="any_type_name";static
char _tmp325[15]="type_name_list";static char _tmp326[20]="abstract_declarator";
static char _tmp327[27]="direct_abstract_declarator";static char _tmp328[10]="statement";
static char _tmp329[18]="labeled_statement";static char _tmp32A[21]="expression_statement";
static char _tmp32B[19]="compound_statement";static char _tmp32C[16]="block_item_list";
static char _tmp32D[20]="selection_statement";static char _tmp32E[15]="switch_clauses";
static char _tmp32F[20]="iteration_statement";static char _tmp330[15]="jump_statement";
static char _tmp331[12]="exp_pattern";static char _tmp332[20]="conditional_pattern";
static char _tmp333[19]="logical_or_pattern";static char _tmp334[20]="logical_and_pattern";
static char _tmp335[21]="inclusive_or_pattern";static char _tmp336[21]="exclusive_or_pattern";
static char _tmp337[12]="and_pattern";static char _tmp338[17]="equality_pattern";
static char _tmp339[19]="relational_pattern";static char _tmp33A[14]="shift_pattern";
static char _tmp33B[17]="additive_pattern";static char _tmp33C[23]="multiplicative_pattern";
static char _tmp33D[13]="cast_pattern";static char _tmp33E[14]="unary_pattern";
static char _tmp33F[16]="postfix_pattern";static char _tmp340[16]="primary_pattern";
static char _tmp341[8]="pattern";static char _tmp342[19]="tuple_pattern_list";static
char _tmp343[14]="field_pattern";static char _tmp344[19]="field_pattern_list";
static char _tmp345[11]="expression";static char _tmp346[22]="assignment_expression";
static char _tmp347[20]="assignment_operator";static char _tmp348[23]="conditional_expression";
static char _tmp349[20]="constant_expression";static char _tmp34A[22]="logical_or_expression";
static char _tmp34B[23]="logical_and_expression";static char _tmp34C[24]="inclusive_or_expression";
static char _tmp34D[24]="exclusive_or_expression";static char _tmp34E[15]="and_expression";
static char _tmp34F[20]="equality_expression";static char _tmp350[22]="relational_expression";
static char _tmp351[17]="shift_expression";static char _tmp352[20]="additive_expression";
static char _tmp353[26]="multiplicative_expression";static char _tmp354[16]="cast_expression";
static char _tmp355[17]="unary_expression";static char _tmp356[15]="unary_operator";
static char _tmp357[19]="postfix_expression";static char _tmp358[19]="primary_expression";
static char _tmp359[25]="argument_expression_list";static char _tmp35A[26]="argument_expression_list0";
static char _tmp35B[9]="constant";static char _tmp35C[20]="qual_opt_identifier";
static char _tmp35D[17]="qual_opt_typedef";static char _tmp35E[18]="struct_union_name";
static char _tmp35F[11]="field_name";static char _tmp360[12]="right_angle";static
struct _tagged_arr Cyc_yytname[266]={{_tmp257,_tmp257,_tmp257 + 2},{_tmp258,_tmp258,
_tmp258 + 6},{_tmp259,_tmp259,_tmp259 + 12},{_tmp25A,_tmp25A,_tmp25A + 5},{_tmp25B,
_tmp25B,_tmp25B + 9},{_tmp25C,_tmp25C,_tmp25C + 7},{_tmp25D,_tmp25D,_tmp25D + 7},{
_tmp25E,_tmp25E,_tmp25E + 8},{_tmp25F,_tmp25F,_tmp25F + 5},{_tmp260,_tmp260,
_tmp260 + 5},{_tmp261,_tmp261,_tmp261 + 6},{_tmp262,_tmp262,_tmp262 + 4},{_tmp263,
_tmp263,_tmp263 + 5},{_tmp264,_tmp264,_tmp264 + 6},{_tmp265,_tmp265,_tmp265 + 7},{
_tmp266,_tmp266,_tmp266 + 7},{_tmp267,_tmp267,_tmp267 + 9},{_tmp268,_tmp268,
_tmp268 + 6},{_tmp269,_tmp269,_tmp269 + 9},{_tmp26A,_tmp26A,_tmp26A + 9},{_tmp26B,
_tmp26B,_tmp26B + 7},{_tmp26C,_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D,_tmp26D + 5},{
_tmp26E,_tmp26E,_tmp26E + 8},{_tmp26F,_tmp26F,_tmp26F + 7},{_tmp270,_tmp270,
_tmp270 + 7},{_tmp271,_tmp271,_tmp271 + 9},{_tmp272,_tmp272,_tmp272 + 3},{_tmp273,
_tmp273,_tmp273 + 5},{_tmp274,_tmp274,_tmp274 + 7},{_tmp275,_tmp275,_tmp275 + 6},{
_tmp276,_tmp276,_tmp276 + 3},{_tmp277,_tmp277,_tmp277 + 4},{_tmp278,_tmp278,
_tmp278 + 5},{_tmp279,_tmp279,_tmp279 + 9},{_tmp27A,_tmp27A,_tmp27A + 6},{_tmp27B,
_tmp27B,_tmp27B + 7},{_tmp27C,_tmp27C,_tmp27C + 5},{_tmp27D,_tmp27D,_tmp27D + 8},{
_tmp27E,_tmp27E,_tmp27E + 4},{_tmp27F,_tmp27F,_tmp27F + 6},{_tmp280,_tmp280,
_tmp280 + 4},{_tmp281,_tmp281,_tmp281 + 6},{_tmp282,_tmp282,_tmp282 + 7},{_tmp283,
_tmp283,_tmp283 + 4},{_tmp284,_tmp284,_tmp284 + 9},{_tmp285,_tmp285,_tmp285 + 9},{
_tmp286,_tmp286,_tmp286 + 6},{_tmp287,_tmp287,_tmp287 + 10},{_tmp288,_tmp288,
_tmp288 + 7},{_tmp289,_tmp289,_tmp289 + 8},{_tmp28A,_tmp28A,_tmp28A + 7},{_tmp28B,
_tmp28B,_tmp28B + 8},{_tmp28C,_tmp28C,_tmp28C + 7},{_tmp28D,_tmp28D,_tmp28D + 8},{
_tmp28E,_tmp28E,_tmp28E + 9},{_tmp28F,_tmp28F,_tmp28F + 9},{_tmp290,_tmp290,
_tmp290 + 6},{_tmp291,_tmp291,_tmp291 + 7},{_tmp292,_tmp292,_tmp292 + 5},{_tmp293,
_tmp293,_tmp293 + 8},{_tmp294,_tmp294,_tmp294 + 13},{_tmp295,_tmp295,_tmp295 + 4},{
_tmp296,_tmp296,_tmp296 + 14},{_tmp297,_tmp297,_tmp297 + 12},{_tmp298,_tmp298,
_tmp298 + 7},{_tmp299,_tmp299,_tmp299 + 8},{_tmp29A,_tmp29A,_tmp29A + 7},{_tmp29B,
_tmp29B,_tmp29B + 7},{_tmp29C,_tmp29C,_tmp29C + 7},{_tmp29D,_tmp29D,_tmp29D + 8},{
_tmp29E,_tmp29E,_tmp29E + 9},{_tmp29F,_tmp29F,_tmp29F + 6},{_tmp2A0,_tmp2A0,
_tmp2A0 + 6},{_tmp2A1,_tmp2A1,_tmp2A1 + 6},{_tmp2A2,_tmp2A2,_tmp2A2 + 6},{_tmp2A3,
_tmp2A3,_tmp2A3 + 7},{_tmp2A4,_tmp2A4,_tmp2A4 + 6},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{
_tmp2A6,_tmp2A6,_tmp2A6 + 11},{_tmp2A7,_tmp2A7,_tmp2A7 + 11},{_tmp2A8,_tmp2A8,
_tmp2A8 + 11},{_tmp2A9,_tmp2A9,_tmp2A9 + 11},{_tmp2AA,_tmp2AA,_tmp2AA + 12},{
_tmp2AB,_tmp2AB,_tmp2AB + 13},{_tmp2AC,_tmp2AC,_tmp2AC + 11},{_tmp2AD,_tmp2AD,
_tmp2AD + 11},{_tmp2AE,_tmp2AE,_tmp2AE + 10},{_tmp2AF,_tmp2AF,_tmp2AF + 9},{_tmp2B0,
_tmp2B0,_tmp2B0 + 11},{_tmp2B1,_tmp2B1,_tmp2B1 + 12},{_tmp2B2,_tmp2B2,_tmp2B2 + 11},{
_tmp2B3,_tmp2B3,_tmp2B3 + 17},{_tmp2B4,_tmp2B4,_tmp2B4 + 7},{_tmp2B5,_tmp2B5,
_tmp2B5 + 19},{_tmp2B6,_tmp2B6,_tmp2B6 + 18},{_tmp2B7,_tmp2B7,_tmp2B7 + 9},{_tmp2B8,
_tmp2B8,_tmp2B8 + 13},{_tmp2B9,_tmp2B9,_tmp2B9 + 16},{_tmp2BA,_tmp2BA,_tmp2BA + 18},{
_tmp2BB,_tmp2BB,_tmp2BB + 13},{_tmp2BC,_tmp2BC,_tmp2BC + 10},{_tmp2BD,_tmp2BD,
_tmp2BD + 4},{_tmp2BE,_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,
_tmp2C0,_tmp2C0 + 4},{_tmp2C1,_tmp2C1,_tmp2C1 + 4},{_tmp2C2,_tmp2C2,_tmp2C2 + 4},{
_tmp2C3,_tmp2C3,_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 4},{_tmp2C5,_tmp2C5,
_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 4},{_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,
_tmp2C8,_tmp2C8 + 4},{_tmp2C9,_tmp2C9,_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{
_tmp2CB,_tmp2CB,_tmp2CB + 4},{_tmp2CC,_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD,
_tmp2CD + 4},{_tmp2CE,_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF,_tmp2CF + 4},{_tmp2D0,
_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 4},{_tmp2D2,_tmp2D2,_tmp2D2 + 4},{
_tmp2D3,_tmp2D3,_tmp2D3 + 4},{_tmp2D4,_tmp2D4,_tmp2D4 + 4},{_tmp2D5,_tmp2D5,
_tmp2D5 + 4},{_tmp2D6,_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7,_tmp2D7 + 4},{_tmp2D8,
_tmp2D8,_tmp2D8 + 5},{_tmp2D9,_tmp2D9,_tmp2D9 + 17},{_tmp2DA,_tmp2DA,_tmp2DA + 12},{
_tmp2DB,_tmp2DB,_tmp2DB + 19},{_tmp2DC,_tmp2DC,_tmp2DC + 21},{_tmp2DD,_tmp2DD,
_tmp2DD + 20},{_tmp2DE,_tmp2DE,_tmp2DE + 21},{_tmp2DF,_tmp2DF,_tmp2DF + 13},{
_tmp2E0,_tmp2E0,_tmp2E0 + 15},{_tmp2E1,_tmp2E1,_tmp2E1 + 17},{_tmp2E2,_tmp2E2,
_tmp2E2 + 19},{_tmp2E3,_tmp2E3,_tmp2E3 + 12},{_tmp2E4,_tmp2E4,_tmp2E4 + 17},{
_tmp2E5,_tmp2E5,_tmp2E5 + 23},{_tmp2E6,_tmp2E6,_tmp2E6 + 24},{_tmp2E7,_tmp2E7,
_tmp2E7 + 15},{_tmp2E8,_tmp2E8,_tmp2E8 + 11},{_tmp2E9,_tmp2E9,_tmp2E9 + 15},{
_tmp2EA,_tmp2EA,_tmp2EA + 10},{_tmp2EB,_tmp2EB,_tmp2EB + 15},{_tmp2EC,_tmp2EC,
_tmp2EC + 25},{_tmp2ED,_tmp2ED,_tmp2ED + 5},{_tmp2EE,_tmp2EE,_tmp2EE + 15},{_tmp2EF,
_tmp2EF,_tmp2EF + 15},{_tmp2F0,_tmp2F0,_tmp2F0 + 11},{_tmp2F1,_tmp2F1,_tmp2F1 + 22},{
_tmp2F2,_tmp2F2,_tmp2F2 + 26},{_tmp2F3,_tmp2F3,_tmp2F3 + 16},{_tmp2F4,_tmp2F4,
_tmp2F4 + 16},{_tmp2F5,_tmp2F5,_tmp2F5 + 24},{_tmp2F6,_tmp2F6,_tmp2F6 + 25},{
_tmp2F7,_tmp2F7,_tmp2F7 + 21},{_tmp2F8,_tmp2F8,_tmp2F8 + 22},{_tmp2F9,_tmp2F9,
_tmp2F9 + 16},{_tmp2FA,_tmp2FA,_tmp2FA + 19},{_tmp2FB,_tmp2FB,_tmp2FB + 25},{
_tmp2FC,_tmp2FC,_tmp2FC + 35},{_tmp2FD,_tmp2FD,_tmp2FD + 23},{_tmp2FE,_tmp2FE,
_tmp2FE + 24},{_tmp2FF,_tmp2FF,_tmp2FF + 18},{_tmp300,_tmp300,_tmp300 + 17},{
_tmp301,_tmp301,_tmp301 + 18},{_tmp302,_tmp302,_tmp302 + 17},{_tmp303,_tmp303,
_tmp303 + 18},{_tmp304,_tmp304,_tmp304 + 12},{_tmp305,_tmp305,_tmp305 + 11},{
_tmp306,_tmp306,_tmp306 + 23},{_tmp307,_tmp307,_tmp307 + 18},{_tmp308,_tmp308,
_tmp308 + 30},{_tmp309,_tmp309,_tmp309 + 8},{_tmp30A,_tmp30A,_tmp30A + 12},{_tmp30B,
_tmp30B,_tmp30B + 23},{_tmp30C,_tmp30C,_tmp30C + 14},{_tmp30D,_tmp30D,_tmp30D + 18},{
_tmp30E,_tmp30E,_tmp30E + 8},{_tmp30F,_tmp30F,_tmp30F + 11},{_tmp310,_tmp310,
_tmp310 + 20},{_tmp311,_tmp311,_tmp311 + 9},{_tmp312,_tmp312,_tmp312 + 16},{_tmp313,
_tmp313,_tmp313 + 19},{_tmp314,_tmp314,_tmp314 + 10},{_tmp315,_tmp315,_tmp315 + 16},{
_tmp316,_tmp316,_tmp316 + 11},{_tmp317,_tmp317,_tmp317 + 14},{_tmp318,_tmp318,
_tmp318 + 11},{_tmp319,_tmp319,_tmp319 + 15},{_tmp31A,_tmp31A,_tmp31A + 22},{
_tmp31B,_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C,_tmp31C + 17},{_tmp31D,_tmp31D,
_tmp31D + 12},{_tmp31E,_tmp31E,_tmp31E + 18},{_tmp31F,_tmp31F,_tmp31F + 17},{
_tmp320,_tmp320,_tmp320 + 12},{_tmp321,_tmp321,_tmp321 + 16},{_tmp322,_tmp322,
_tmp322 + 11},{_tmp323,_tmp323,_tmp323 + 10},{_tmp324,_tmp324,_tmp324 + 14},{
_tmp325,_tmp325,_tmp325 + 15},{_tmp326,_tmp326,_tmp326 + 20},{_tmp327,_tmp327,
_tmp327 + 27},{_tmp328,_tmp328,_tmp328 + 10},{_tmp329,_tmp329,_tmp329 + 18},{
_tmp32A,_tmp32A,_tmp32A + 21},{_tmp32B,_tmp32B,_tmp32B + 19},{_tmp32C,_tmp32C,
_tmp32C + 16},{_tmp32D,_tmp32D,_tmp32D + 20},{_tmp32E,_tmp32E,_tmp32E + 15},{
_tmp32F,_tmp32F,_tmp32F + 20},{_tmp330,_tmp330,_tmp330 + 15},{_tmp331,_tmp331,
_tmp331 + 12},{_tmp332,_tmp332,_tmp332 + 20},{_tmp333,_tmp333,_tmp333 + 19},{
_tmp334,_tmp334,_tmp334 + 20},{_tmp335,_tmp335,_tmp335 + 21},{_tmp336,_tmp336,
_tmp336 + 21},{_tmp337,_tmp337,_tmp337 + 12},{_tmp338,_tmp338,_tmp338 + 17},{
_tmp339,_tmp339,_tmp339 + 19},{_tmp33A,_tmp33A,_tmp33A + 14},{_tmp33B,_tmp33B,
_tmp33B + 17},{_tmp33C,_tmp33C,_tmp33C + 23},{_tmp33D,_tmp33D,_tmp33D + 13},{
_tmp33E,_tmp33E,_tmp33E + 14},{_tmp33F,_tmp33F,_tmp33F + 16},{_tmp340,_tmp340,
_tmp340 + 16},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 19},{_tmp343,
_tmp343,_tmp343 + 14},{_tmp344,_tmp344,_tmp344 + 19},{_tmp345,_tmp345,_tmp345 + 11},{
_tmp346,_tmp346,_tmp346 + 22},{_tmp347,_tmp347,_tmp347 + 20},{_tmp348,_tmp348,
_tmp348 + 23},{_tmp349,_tmp349,_tmp349 + 20},{_tmp34A,_tmp34A,_tmp34A + 22},{
_tmp34B,_tmp34B,_tmp34B + 23},{_tmp34C,_tmp34C,_tmp34C + 24},{_tmp34D,_tmp34D,
_tmp34D + 24},{_tmp34E,_tmp34E,_tmp34E + 15},{_tmp34F,_tmp34F,_tmp34F + 20},{
_tmp350,_tmp350,_tmp350 + 22},{_tmp351,_tmp351,_tmp351 + 17},{_tmp352,_tmp352,
_tmp352 + 20},{_tmp353,_tmp353,_tmp353 + 26},{_tmp354,_tmp354,_tmp354 + 16},{
_tmp355,_tmp355,_tmp355 + 17},{_tmp356,_tmp356,_tmp356 + 15},{_tmp357,_tmp357,
_tmp357 + 19},{_tmp358,_tmp358,_tmp358 + 19},{_tmp359,_tmp359,_tmp359 + 25},{
_tmp35A,_tmp35A,_tmp35A + 26},{_tmp35B,_tmp35B,_tmp35B + 9},{_tmp35C,_tmp35C,
_tmp35C + 20},{_tmp35D,_tmp35D,_tmp35D + 17},{_tmp35E,_tmp35E,_tmp35E + 18},{
_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 12}};static short Cyc_yyr1[
479]={0,129,130,130,130,130,130,130,130,130,130,130,131,131,132,132,133,133,133,
134,134,134,134,135,135,136,137,138,139,140,140,140,140,141,141,142,142,142,142,
142,142,142,142,142,142,143,143,143,143,143,143,143,144,144,145,146,146,147,147,
147,147,147,147,148,148,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
149,149,149,149,149,149,149,150,151,151,151,152,152,152,153,153,154,154,154,155,
155,155,156,156,157,157,158,158,159,159,160,161,161,162,162,163,164,164,164,164,
164,164,165,165,165,165,165,165,166,167,167,168,168,168,169,169,169,170,170,171,
171,171,171,172,172,172,173,173,174,174,175,175,176,176,176,176,176,176,176,176,
176,176,177,177,177,177,177,177,177,177,177,177,177,178,178,179,180,180,180,181,
181,181,182,182,182,183,183,183,184,184,185,185,185,185,186,186,187,187,188,188,
189,189,190,190,191,191,192,192,192,192,193,193,194,194,195,195,195,196,197,197,
198,198,199,199,199,199,200,200,200,200,201,202,202,203,203,204,204,205,205,205,
205,205,206,206,207,207,207,208,208,208,208,208,208,208,208,208,208,208,209,209,
209,209,209,209,209,209,209,209,209,210,211,211,212,212,213,213,213,213,213,213,
214,214,214,214,215,215,215,215,215,215,216,216,216,216,216,216,216,216,216,216,
216,216,216,216,217,217,217,217,217,217,217,217,218,219,219,220,220,221,221,222,
222,223,223,224,224,225,225,225,226,226,226,226,226,227,227,227,228,228,228,229,
229,229,229,230,230,231,231,231,231,231,231,231,232,233,234,234,234,234,234,234,
234,234,234,234,234,234,235,235,236,236,237,237,238,238,239,239,240,240,240,240,
240,240,240,240,240,240,240,241,241,241,241,241,241,241,242,243,243,244,244,245,
245,246,246,247,247,248,248,248,249,249,249,249,249,250,250,250,251,251,251,252,
252,252,252,253,253,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,255,255,255,256,256,256,256,256,256,256,256,256,256,257,257,257,257,257,257,
257,257,257,258,259,259,260,260,260,260,261,261,262,262,263,263,264,264,265,265};
static short Cyc_yyr2[479]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,
2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,
3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,
1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,
1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,3,0,1,1,0,1,1,
0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,
3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,
5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,
3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,
4,2,6,6,1,1,1,3,1,1,4,4,5,2,2,2,4,4,1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,
5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,
2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,
1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[953]={0,18,45,46,47,48,50,65,68,
69,70,71,72,73,74,75,87,88,89,103,104,41,0,0,51,0,0,136,137,0,0,84,0,0,469,193,
471,470,472,85,0,0,66,0,178,178,177,1,0,16,0,0,17,0,35,43,37,63,39,76,77,0,78,
184,0,147,0,172,181,79,151,101,49,48,42,0,91,468,0,469,465,466,467,0,353,0,0,0,0,
216,0,355,356,25,27,0,0,0,0,0,0,0,0,0,0,0,0,175,176,2,0,0,0,0,29,0,109,110,112,
36,44,38,40,105,473,474,101,186,0,185,0,33,0,0,19,195,0,0,160,148,173,183,182,
184,0,64,0,49,95,0,93,0,469,361,0,0,0,0,0,0,0,0,0,0,101,0,0,455,0,0,0,0,0,442,
440,441,0,371,373,386,394,396,398,400,402,404,407,412,415,418,422,0,424,443,454,
453,0,362,360,32,0,0,101,0,0,0,119,115,117,234,236,0,0,0,9,10,475,476,194,86,0,0,
152,67,214,0,211,0,0,0,3,0,5,0,30,0,0,0,20,0,106,107,0,100,101,0,0,0,0,0,0,0,0,0,
0,0,0,0,469,269,271,277,273,0,275,257,258,259,0,260,261,262,0,34,21,112,201,217,
0,0,197,195,0,241,0,181,0,52,0,0,96,92,0,0,0,0,0,432,0,388,422,0,389,390,0,0,0,0,
0,0,0,425,426,0,0,0,0,428,429,427,0,354,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
376,377,378,379,380,381,382,383,384,385,375,0,430,0,449,450,0,0,0,457,0,101,365,
0,218,0,0,0,0,237,209,0,120,125,121,123,116,118,195,0,243,235,244,478,477,0,81,
82,83,58,57,0,55,153,195,213,149,243,215,161,162,0,80,0,180,179,26,0,28,0,111,
113,220,219,22,98,108,0,0,0,127,128,130,0,101,0,134,0,0,0,0,0,0,304,305,306,0,0,
308,0,0,0,0,0,0,278,274,112,276,272,270,202,0,0,0,208,196,203,156,0,0,0,197,158,
0,159,154,181,187,53,102,0,97,393,94,90,363,364,0,0,0,221,0,0,225,0,0,0,230,0,0,
0,0,0,0,0,0,0,456,463,0,462,372,395,0,397,399,401,403,405,406,410,411,408,409,
413,414,416,417,419,420,421,374,448,445,0,447,0,0,0,0,357,31,0,367,369,0,358,0,0,
238,126,122,124,0,197,0,181,0,245,195,0,0,256,240,0,0,0,119,0,101,195,0,0,171,
150,212,4,6,0,131,114,0,0,197,101,142,0,0,0,0,0,0,0,303,307,0,0,0,264,0,0,0,268,
0,23,195,0,205,0,0,198,0,157,201,189,242,155,187,174,0,7,0,431,0,0,233,0,222,0,
226,229,231,436,0,0,0,0,0,0,0,0,423,459,0,0,446,444,0,0,366,368,359,0,239,210,
252,0,246,247,181,0,197,0,181,0,0,0,0,54,56,0,163,0,197,0,0,181,0,129,132,105,
135,144,143,0,0,138,0,0,0,0,0,0,0,0,0,283,309,0,0,0,0,24,197,0,206,204,0,197,0,
188,0,8,0,0,0,232,223,227,0,0,0,0,391,392,435,461,0,464,387,458,460,0,370,251,
249,254,0,255,248,181,0,61,59,60,164,167,0,169,170,165,181,0,133,145,142,142,279,
283,289,0,0,0,0,0,0,0,0,0,0,0,0,310,0,0,0,267,191,207,199,190,195,13,0,0,434,433,
0,228,437,0,0,451,0,253,250,0,168,166,99,0,141,140,0,0,0,291,0,0,0,299,0,0,0,0,0,
0,0,0,0,0,0,311,312,314,316,318,320,322,324,327,332,335,338,342,344,351,352,0,0,
282,263,266,0,0,197,12,14,0,0,0,0,452,0,0,280,281,290,292,293,0,301,300,0,295,0,
0,0,0,348,0,0,345,283,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,346,284,0,200,192,
15,0,0,0,0,146,294,302,296,297,0,0,0,0,283,285,315,0,317,319,321,323,325,326,330,
331,328,329,333,334,336,337,339,340,341,0,0,224,438,0,62,298,347,0,343,286,0,283,
265,0,0,0,313,283,287,439,350,349,288,0,0,0};static short Cyc_yydefgoto[137]={950,
47,613,775,48,49,261,50,408,51,410,52,132,53,54,469,206,392,393,207,57,218,208,
59,148,149,60,145,61,239,240,115,116,117,241,209,377,420,421,422,62,63,676,677,
678,64,423,65,397,66,67,68,107,143,128,610,545,69,546,461,602,453,457,458,372,
281,226,88,89,485,413,486,487,488,489,210,283,284,547,383,264,265,266,267,268,
269,763,270,271,813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,
363,364,536,537,272,177,352,178,475,179,180,181,182,183,184,185,186,187,188,189,
190,191,192,193,501,502,194,195,71,776,219,387};static short Cyc_yypact[953]={2355,
- -32768,- -32768,- -32768,- -32768,- 2,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
2891,206,1129,- -32768,56,147,- -32768,- -32768,141,162,200,161,232,- -32768,266,-
-32768,- -32768,- -32768,- -32768,271,174,282,273,299,299,- -32768,- -32768,2236,-
-32768,193,220,- -32768,230,2891,2891,2891,- -32768,2891,- -32768,- -32768,560,-
-32768,88,2834,244,138,544,471,- -32768,- -32768,300,322,342,- -32768,56,352,-
-32768,1175,233,- -32768,- -32768,- -32768,5002,- -32768,356,377,1175,391,390,400,-
-32768,205,- -32768,- -32768,5522,5522,5522,2236,2236,- 55,409,480,414,- 55,5743,
1865,- -32768,- -32768,- -32768,2236,2473,2236,2473,- -32768,422,424,- -32768,2766,-
-32768,- -32768,- -32768,- -32768,5743,- -32768,- -32768,300,- -32768,56,- -32768,1492,
- -32768,2834,230,- -32768,5507,5522,3438,- -32768,244,- -32768,- -32768,- -32768,88,
5522,- -32768,2473,- -32768,438,451,453,56,450,- -32768,159,5209,458,5002,5051,492,
496,500,526,531,300,5258,5258,- -32768,1986,536,5337,5337,5337,- -32768,- -32768,-
-32768,- 16,- -32768,- -32768,- 39,555,532,523,551,410,169,502,314,133,- -32768,1046,
5337,121,11,- -32768,573,1175,- -32768,- -32768,- -32768,598,5002,300,1175,576,221,
5743,5758,5743,613,- -32768,- 5,- 5,- 5,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,68,587,- -32768,- -32768,701,21,- -32768,597,- 24,595,- -32768,615,- -32768,621,
- -32768,480,3530,2834,- -32768,635,5743,- -32768,589,656,97,638,664,669,2977,686,
706,712,714,3622,2977,177,50,698,739,- -32768,- -32768,1618,1618,230,1618,- -32768,
- -32768,- -32768,749,- -32768,- -32768,- -32768,122,- -32768,- -32768,766,764,- -32768,
74,768,762,339,770,760,17,471,774,791,17,790,56,- -32768,5002,792,776,778,1986,-
-32768,5743,- -32768,- -32768,3070,- -32768,821,5002,5002,5002,5002,5002,794,1986,-
-32768,- -32768,1618,795,373,5002,- -32768,- -32768,- -32768,5002,- -32768,5337,5002,
5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,
5337,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,5002,- -32768,- 55,- -32768,- -32768,3714,- 55,5002,- -32768,788,300,-
-32768,395,- -32768,371,941,402,5522,- -32768,797,802,- -32768,5758,5758,5758,-
-32768,- -32768,2594,3806,171,- -32768,293,- -32768,- -32768,74,- -32768,- -32768,-
-32768,- -32768,817,818,820,- -32768,2538,- -32768,521,503,- -32768,- -32768,- -32768,
5743,- -32768,2107,- -32768,- -32768,- -32768,2236,- -32768,2236,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,743,5002,826,824,- -32768,823,368,300,56,
829,5002,5002,5002,903,1744,834,- -32768,- -32768,- -32768,452,895,- -32768,3898,
2977,843,849,5002,2977,- -32768,- -32768,2766,- -32768,- -32768,- -32768,- -32768,5743,
835,265,- -32768,- -32768,827,- -32768,74,838,5640,762,- -32768,5522,- -32768,- -32768,
471,769,- -32768,- -32768,1099,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,840,
844,852,- -32768,- 55,5002,- -32768,486,3530,236,- -32768,472,845,846,847,484,5743,
850,853,5130,- -32768,- -32768,855,848,- -32768,555,255,532,523,551,410,169,169,502,
502,502,502,314,314,133,133,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,857,
- -32768,152,5522,3346,1175,- -32768,- -32768,1175,- -32768,- -32768,607,- -32768,66,
858,- -32768,- -32768,- -32768,- -32768,859,762,860,471,854,293,5625,5522,3990,-
-32768,- -32768,754,863,68,2650,864,300,5507,5522,4082,- -32768,521,- -32768,-
-32768,- -32768,705,- -32768,- -32768,589,5002,762,300,727,504,515,519,866,4174,
4266,524,- -32768,- -32768,872,876,873,- -32768,861,867,534,- -32768,2834,- -32768,
881,5522,- -32768,880,74,- -32768,868,- -32768,131,- -32768,760,- -32768,769,- -32768,
883,- -32768,2236,885,361,878,- -32768,875,- -32768,3162,- -32768,- -32768,- -32768,-
-32768,5002,966,5002,5051,884,885,888,3346,- -32768,- -32768,5002,5002,- -32768,-
-32768,17,647,- -32768,- -32768,- -32768,941,- -32768,- -32768,- -32768,890,- -32768,-
-32768,471,891,762,17,471,886,542,892,896,- -32768,- -32768,897,- -32768,900,762,
901,17,471,898,- -32768,- -32768,5743,- -32768,- -32768,- -32768,889,56,570,2977,907,
2977,5002,4358,579,4450,588,4542,787,- -32768,910,912,52,913,- -32768,762,96,-
-32768,- -32768,918,762,5743,- -32768,421,- -32768,908,911,5002,- -32768,- -32768,-
-32768,3530,592,915,919,- -32768,821,- -32768,- -32768,661,- -32768,- -32768,- -32768,
- -32768,3346,- -32768,- -32768,- -32768,- -32768,917,- -32768,- -32768,471,926,- -32768,
- -32768,- -32768,- -32768,- -32768,921,- -32768,- -32768,- -32768,471,916,- -32768,923,
72,727,995,787,- -32768,629,2977,642,4634,2977,659,4726,4818,616,1134,914,927,-
-32768,2977,2977,942,- -32768,- -32768,- -32768,932,- -32768,881,- -32768,936,668,-
-32768,- -32768,276,- -32768,- -32768,5743,1016,- -32768,3254,- -32768,- -32768,937,-
-32768,- -32768,- -32768,5743,- -32768,- -32768,2977,939,943,- -32768,2977,2977,670,-
-32768,2977,2977,672,2977,676,4910,5386,940,2107,5337,944,- -32768,- 9,- -32768,968,
928,703,763,175,796,378,189,- -32768,- -32768,- -32768,- -32768,976,5337,1618,-
-32768,- -32768,- -32768,946,74,762,- -32768,- -32768,580,5002,951,953,- -32768,969,
722,- -32768,- -32768,- -32768,- -32768,- -32768,2977,- -32768,- -32768,2977,- -32768,
2977,2977,731,1986,- -32768,5743,954,- -32768,1305,5337,5002,5337,5337,5337,5337,
5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5337,5002,- -32768,-
-32768,972,- -32768,- -32768,- -32768,753,958,5743,960,- -32768,- -32768,- -32768,-
-32768,- -32768,2977,961,965,5337,787,- -32768,555,294,532,523,523,410,169,169,502,
502,502,502,314,314,133,133,- -32768,- -32768,- -32768,307,2977,- -32768,- -32768,964,
- -32768,- -32768,885,441,- -32768,- -32768,5002,1305,- -32768,967,970,974,- -32768,
787,- -32768,- -32768,- -32768,- -32768,- -32768,1073,1074,- -32768};static short Cyc_yypgoto[
137]={- -32768,556,- -32768,237,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 59,- 78,- 11,- -32768,- -32768,0,518,- -32768,69,- 144,979,39,- -32768,- -32768,
- 150,- -32768,80,- -32768,408,- -32768,- -32768,- -32768,851,856,184,488,- -32768,-
-32768,511,- -32768,- -32768,135,- -32768,- -32768,63,- 79,1019,- 316,144,- -32768,-
-32768,1042,- 228,945,481,- 101,- 57,- 108,148,253,487,490,- 379,- 409,- 94,- 300,- 100,-
-32768,- 160,- 136,- 497,- 211,- -32768,605,- 138,117,- 107,- 26,- 206,29,- -32768,- -32768,
- 12,- 246,- -32768,- 645,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 4,894,454,- -32768,186,654,- -32768,- 148,- 318,- 129,- 314,- 312,- 321,775,- 313,
- 326,- 307,- 303,- 275,- 121,583,334,- -32768,- -32768,- 285,- -32768,- 3,318,- 33,1060,-
341,- 18};static short Cyc_yytable[5869]={55,293,510,511,507,131,129,555,504,299,74,
225,506,524,509,446,447,527,449,90,91,55,302,512,513,514,515,280,125,303,314,516,
517,640,279,282,216,288,322,58,237,102,217,119,120,121,600,122,55,317,318,319,
134,133,55,55,55,467,55,131,58,518,519,375,55,138,384,497,866,56,353,262,526,273,
153,91,412,674,675,323,405,603,566,198,91,390,129,58,384,320,56,72,321,58,58,58,
386,58,55,55,360,571,567,58,103,796,238,133,566,867,55,55,55,55,385,386,118,56,
55,263,274,133,465,56,56,56,402,56,361,403,55,646,55,56,454,719,385,58,58,138,
473,441,617,766,474,396,55,34,371,58,58,58,58,597,37,589,534,58,479,391,480,442,
567,767,443,221,618,56,56,58,35,58,496,463,645,550,- 139,455,273,56,56,56,56,382,
35,58,386,56,354,355,356,401,550,91,388,389,275,127,399,56,91,56,262,262,770,262,
243,374,144,401,426,140,211,212,213,56,386,520,521,522,906,456,452,228,451,415,
133,320,357,34,375,375,375,278,358,359,37,55,94,- 195,608,330,331,- 195,309,41,376,
874,875,338,263,263,95,263,262,294,671,320,339,340,936,55,55,98,55,34,466,638,
295,176,471,474,37,96,706,40,58,431,379,439,332,41,367,438,440,380,876,470,333,
224,44,45,46,945,877,110,111,34,275,949,58,58,263,58,37,882,56,242,202,75,225,97,
203,55,883,884,560,35,70,224,650,34,112,113,427,370,448,621,37,456,56,56,114,56,
99,- 217,474,41,- 217,560,76,92,622,93,154,40,44,45,46,483,484,135,58,381,315,136,
100,629,137,70,320,35,561,535,91,401,70,636,398,599,595,70,104,583,374,374,374,
633,101,124,105,320,56,554,70,401,424,366,841,131,373,401,378,150,40,559,92,565,
371,320,551,773,106,456,552,92,937,553,55,711,55,144,320,376,376,376,70,70,570,
938,70,133,639,727,242,146,474,731,70,70,70,70,55,534,336,147,596,133,337,437,
742,278,530,653,462,654,244,58,55,58,652,70,216,705,665,151,667,603,217,34,710,
664,666,196,70,36,37,38,197,150,590,58,55,533,594,418,320,56,320,56,662,499,315,
371,328,329,539,58,721,695,490,662,715,199,494,200,315,700,880,716,531,56,881,
532,787,575,201,531,609,505,538,58,34,711,92,790,220,56,36,37,38,92,222,381,234,
774,561,641,91,235,642,91,216,941,141,142,273,401,217,398,401,56,400,290,456,528,
913,914,910,911,554,780,907,70,586,291,909,320,912,292,400,154,424,224,710,298,
565,915,916,917,918,34,334,335,711,919,920,320,37,224,624,70,607,694,133,887,224,
41,224,320,619,620,628,942,34,55,44,45,46,304,36,37,38,305,109,921,922,306,150,
320,395,606,679,55,578,579,580,380,584,905,320,722,40,680,320,710,687,681,562,
320,593,324,563,307,58,564,730,224,308,320,535,91,693,316,842,326,224,733,609,
741,734,34,58,325,214,215,673,36,37,38,44,45,46,123,56,837,230,231,232,233,125,
34,747,863,327,748,362,36,37,38,34,755,56,369,320,92,36,37,38,365,758,864,944,
320,648,394,418,320,846,406,781,419,289,424,404,44,45,46,749,886,751,643,644,400,
424,696,400,424,808,407,379,320,902,672,903,409,70,380,70,44,45,46,674,675,320,
224,400,797,297,416,300,300,400,373,576,428,224,320,311,312,799,723,724,300,300,
300,930,829,91,425,229,923,924,925,320,784,785,803,684,686,839,429,262,840,300,
320,430,320,852,456,855,320,798,935,857,802,16,17,18,943,70,286,34,432,833,834,
34,433,36,37,38,729,36,37,38,444,262,125,395,761,762,712,418,739,434,380,435,44,
45,46,263,44,45,46,847,870,871,402,850,851,896,55,853,854,34,856,320,872,873,901,
36,37,38,769,40,657,658,659,772,92,418,445,92,450,263,452,242,928,320,44,45,46,
542,543,544,55,878,879,752,754,58,757,236,760,460,300,459,400,464,262,386,897,
793,794,898,224,899,900,400,468,414,400,40,779,472,477,476,478,322,529,56,495,
540,498,58,300,541,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,
300,300,556,558,557,263,572,573,932,70,577,581,56,574,585,587,55,591,592,801,598,
616,805,807,604,601,614,615,625,626,627,635,35,414,939,631,630,491,492,493,92,
634,568,637,569,647,649,651,500,660,663,682,503,688,224,58,689,77,691,690,692,
278,697,890,703,699,632,707,708,713,717,745,859,746,718,176,726,728,735,732,300,
765,736,737,523,56,738,740,750,500,764,743,771,768,777,78,788,778,791,124,782,
795,783,786,831,892,612,789,792,832,152,80,549,81,82,836,835,37,838,843,845,848,
868,849,315,862,83,869,84,85,885,908,483,484,865,86,500,893,894,895,904,927,87,
888,929,300,931,933,934,926,940,951,952,946,661,891,947,92,744,300,948,223,670,
139,411,108,287,889,702,698,701,623,500,830,417,368,725,- 11,1,508,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,126,0,21,341,342,343,344,345,346,347,348,349,
350,0,0,22,0,23,0,0,414,611,0,24,0,25,26,27,28,0,0,351,0,29,30,31,300,124,809,
810,0,0,0,32,33,0,77,0,704,0,0,77,0,0,0,0,0,0,0,0,0,0,0,414,0,0,0,0,0,34,0,0,0,0,
35,36,37,38,39,40,0,0,- 11,0,78,41,656,42,43,78,300,0,77,0,44,45,46,669,300,79,80,
0,81,82,152,80,37,81,82,0,0,37,0,0,0,83,0,84,85,0,811,0,84,85,86,0,0,0,0,86,78,
87,0,812,0,0,87,173,0,0,174,175,0,0,0,152,80,0,81,82,0,0,37,414,0,0,0,0,0,0,714,
83,0,84,85,414,0,0,720,0,86,0,0,0,0,0,0,87,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,761,762,21,155,156,245,0,246,247,248,249,250,251,
252,253,22,77,23,157,254,0,0,158,24,255,0,0,27,28,159,160,161,162,29,30,31,256,
163,414,257,164,0,0,0,0,0,165,166,0,0,0,414,0,0,0,0,0,0,0,0,0,0,0,0,0,861,0,0,
300,258,80,167,81,82,35,36,37,38,39,40,259,130,0,0,0,168,300,42,227,0,0,0,0,0,
170,0,0,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,414,0,0,0,0,0,0,0,0,0,300,0,300,
300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,300,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,155,156,245,300,246,247,248,249,250,251,252,253,22,77,23,157,254,
0,0,158,24,255,0,0,27,28,159,160,161,162,29,30,31,256,163,0,257,164,0,0,0,0,0,
165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,80,167,81,82,35,36,37,38,
39,40,259,130,260,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,
175,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,155,156,245,0,246,
247,248,249,250,251,252,253,22,77,23,157,254,0,0,158,24,255,0,0,27,28,159,160,
161,162,29,30,31,256,163,0,257,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,258,80,167,81,82,35,36,37,38,39,40,259,130,0,0,0,168,0,42,227,0,0,
0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,2,3,4,73,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,155,156,0,0,0,0,0,0,0,0,0,0,22,77,23,157,0,0,0,158,24,0,0,
0,27,28,159,160,161,162,29,30,31,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,39,40,582,0,0,0,0,168,0,42,
227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,77,0,157,0,0,0,158,0,0,0,0,27,
28,159,160,161,162,29,30,31,0,163,204,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,39,40,0,205,0,0,0,168,0,42,
227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,77,0,157,0,0,0,158,0,0,0,0,27,
28,159,160,161,162,29,30,31,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,39,40,0,313,0,0,0,168,0,42,227,
0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,77,0,157,0,0,0,158,0,0,0,0,27,28,159,
160,161,162,29,30,31,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,39,40,0,0,0,0,0,168,0,42,227,0,0,0,0,0,
170,0,0,0,171,0,0,172,173,0,0,174,175,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,
0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,
0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,
41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,
30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,43,0,0,0,27,28,44,45,46,0,
29,30,31,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,
276,0,0,34,0,22,0,0,35,36,37,38,39,40,278,0,0,27,28,395,0,42,43,29,30,31,380,0,
44,45,46,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,276,0,0,0,
0,22,0,0,35,36,0,38,39,40,278,0,0,27,28,379,0,42,43,29,30,31,380,0,44,45,46,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,
0,0,0,0,0,418,0,42,43,0,0,0,0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,28,0,0,
0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,35,36,0,38,39,40,0,130,0,22,236,23,0,42,43,0,0,24,0,0,0,27,
28,0,0,0,0,29,30,31,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
0,0,0,0,0,0,0,0,0,0,0,0,22,0,35,36,0,38,39,40,24,130,0,0,27,28,0,42,43,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,155,156,245,0,246,247,248,249,250,251,252,
253,0,77,0,157,254,0,0,158,0,255,0,0,0,0,159,160,161,162,0,0,0,256,163,0,257,164,
0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,80,167,81,82,0,0,
37,0,0,0,259,130,0,0,0,168,0,0,169,0,0,0,0,0,170,0,155,156,171,0,0,172,173,481,0,
174,175,0,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,
0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,
0,0,0,0,301,482,0,0,168,0,0,169,0,0,483,484,0,170,155,156,0,171,0,0,172,173,0,0,
174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,
0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,
0,0,0,301,709,0,0,168,0,0,169,0,0,483,484,0,170,155,156,0,171,0,0,172,173,0,0,
174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,
0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,
0,0,0,301,844,0,0,168,0,0,169,0,0,483,484,0,170,155,156,0,171,0,0,172,173,0,0,
174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,
0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,
0,0,0,301,0,0,0,168,0,0,169,0,0,483,484,0,170,155,156,0,171,0,0,172,173,0,0,174,
175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,
0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,
0,0,0,0,0,168,0,0,169,0,0,0,0,285,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,
0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,
166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,
0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,
0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,436,0,0,0,0,
168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,
158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,525,0,
169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,
0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,
0,548,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,
159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,588,0,169,0,0,0,0,0,
170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,
160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,0,655,170,
155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,
161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,0,668,170,155,
156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,
162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,167,81,82,0,0,37,0,0,0,683,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,
171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,
0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,167,81,82,0,0,37,0,0,0,685,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,
0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,
163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,753,0,169,0,0,0,0,0,170,155,156,0,171,0,0,
172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,
0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,
82,0,0,37,0,0,0,0,0,0,0,0,168,756,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,
0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,
0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,
37,0,0,0,759,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,
175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,
0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,
0,0,0,0,0,168,800,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,
0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,
166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,
0,0,168,804,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,
0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,
806,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,
158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,858,0,
169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,
0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,0,
0,0,0,0,0,0,0,0,0,0,77,0,0,0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,168,
0,0,169,164,0,0,0,0,170,165,166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,
0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,155,156,0,168,0,0,169,0,0,0,0,0,170,77,0,
0,171,0,0,172,173,0,0,174,175,0,159,160,161,162,0,0,0,0,0,0,0,164,0,0,0,0,0,165,
166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,632,
155,156,0,168,0,0,169,0,0,0,0,0,170,77,0,0,171,0,0,172,173,0,0,174,175,0,159,160,
161,162,0,0,0,0,0,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,
0,77,0,0,0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,296,0,0,169,164,0,0,0,
0,170,165,166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,
82,0,0,37,0,0,0,0,0,155,156,0,310,0,0,169,0,0,0,0,0,170,77,0,0,171,0,0,172,173,0,
0,174,175,0,159,160,161,162,0,0,0,0,0,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,
0,0,0,0,0,0,0,0,0,0,0,77,0,0,0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,
168,0,0,169,164,0,0,0,0,170,165,166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,
0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,860,0,0,169,0,0,0,0,0,170,0,0,0,
171,0,0,172,173,0,0,174,175,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,22,0,0,29,30,31,0,0,
0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,204,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,277,0,0,0,
0,35,36,0,38,39,40,278,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,205,0,0,0,0,0,42,43,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,22,0,0,29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,0,0,35,36,0,38,39,40,278,605,0,0,
0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,27,28,
0,22,0,0,29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,0,0,0,39,40,0,0,0,0,0,
0,0,42,43};static short Cyc_yycheck[5869]={0,151,328,329,325,64,63,386,322,157,21,
105,324,354,327,261,262,358,264,23,23,21,158,330,331,332,333,135,61,158,168,334,
335,530,135,135,91,144,77,0,118,41,97,54,55,56,455,58,48,170,171,172,64,64,54,55,
56,285,58,118,21,336,337,207,64,65,71,313,77,0,191,130,357,132,78,78,236,5,6,118,
104,460,398,87,87,17,143,48,71,105,21,93,108,54,55,56,120,58,98,99,89,419,402,64,
41,750,118,118,424,118,110,111,112,113,119,120,53,48,118,130,132,132,105,54,55,
56,105,58,117,108,130,540,132,64,60,632,119,98,99,139,290,91,483,91,292,224,146,
91,205,110,111,112,113,453,98,440,367,118,296,91,298,111,462,111,114,102,484,98,
99,130,96,132,310,281,108,381,104,103,237,110,111,112,113,209,96,146,120,118,67,
68,69,224,398,196,212,213,133,109,224,130,203,132,261,262,108,264,126,207,111,
242,113,67,95,96,97,146,120,338,339,340,865,278,91,106,102,237,237,105,107,91,
374,375,376,102,113,114,98,237,91,108,468,72,73,112,164,107,207,72,73,116,261,
262,111,264,313,96,574,105,125,126,905,261,262,102,264,91,284,115,109,83,288,419,
98,111,615,101,237,248,107,102,111,107,202,254,107,114,111,287,119,105,116,117,
118,938,119,102,103,91,235,944,261,262,313,264,98,116,237,123,103,103,404,111,
107,313,125,126,395,96,0,135,548,91,102,103,244,104,263,487,98,386,261,262,102,
264,102,102,484,107,105,418,22,23,106,25,111,101,116,117,118,113,114,107,313,209,
168,111,90,495,114,41,105,96,395,367,367,398,48,112,224,104,448,53,90,432,374,
375,376,498,107,61,107,105,313,383,66,418,242,201,112,448,206,424,208,75,101,395,
78,397,455,105,107,701,103,460,111,87,112,114,408,620,410,111,105,374,375,376,98,
99,418,112,102,432,529,651,240,103,574,655,110,111,112,113,432,644,120,93,448,
448,124,253,668,102,362,551,105,552,128,408,448,410,551,133,91,92,562,103,563,
836,97,91,620,562,562,107,146,97,98,99,91,151,441,432,472,102,445,107,105,408,
105,410,559,108,296,540,74,75,369,448,636,597,304,570,628,102,308,105,310,605,
120,628,105,432,124,108,732,425,106,105,469,323,108,472,91,724,196,743,107,448,
97,98,99,203,108,379,102,104,559,531,531,105,534,534,91,92,63,64,595,570,97,395,
573,472,224,105,601,359,872,873,869,870,550,711,866,235,102,104,868,105,871,106,
242,111,418,379,724,107,566,874,875,876,877,91,70,71,785,878,879,105,98,395,108,
263,465,595,595,831,402,107,404,105,104,105,108,934,91,595,116,117,118,107,97,98,
99,107,48,880,881,107,290,105,107,463,108,613,428,429,430,114,432,865,105,639,
101,108,105,785,102,108,107,105,444,76,111,107,595,114,654,453,107,105,644,644,
108,107,782,122,462,105,609,667,108,91,613,121,98,99,576,97,98,99,116,117,118,
103,595,773,110,111,112,113,703,91,102,811,123,105,103,97,98,99,91,102,613,107,
105,367,97,98,99,91,102,812,938,105,546,108,107,105,792,104,108,112,146,559,107,
116,117,118,679,830,681,104,105,395,570,598,398,573,102,104,107,105,860,575,862,
104,408,114,410,116,117,118,5,6,105,551,418,108,155,104,157,158,424,559,426,107,
562,105,165,166,108,104,105,170,171,172,894,761,761,103,106,882,883,884,105,104,
105,108,582,583,102,107,831,105,191,105,107,105,108,836,108,105,753,904,108,756,
17,18,19,937,472,137,91,107,765,766,91,91,97,98,99,653,97,98,99,107,865,840,107,
22,23,625,107,665,102,114,102,116,117,118,831,116,117,118,795,122,123,105,799,
800,108,831,803,804,91,806,105,74,75,108,97,98,99,695,101,91,92,93,700,531,107,
112,534,104,865,91,672,104,105,116,117,118,374,375,376,865,70,71,682,683,831,685,
106,687,112,292,108,559,108,938,120,852,747,748,855,701,857,858,570,115,236,573,
101,707,104,119,104,119,77,111,831,107,105,108,865,322,104,324,325,326,327,328,
329,330,331,332,333,334,335,336,337,338,339,340,107,105,108,938,102,105,901,613,
103,30,865,112,102,42,938,96,91,755,107,91,758,759,108,120,108,105,105,105,105,
105,96,301,927,104,108,305,306,307,644,108,408,108,410,108,108,115,316,108,108,
107,320,103,792,938,102,38,119,108,115,102,104,837,103,119,103,111,115,25,108,
104,808,677,108,811,108,108,108,115,419,91,108,108,352,938,108,108,103,357,102,
115,96,102,108,76,92,108,104,703,107,28,105,108,112,841,472,108,107,104,91,92,
380,94,95,105,96,98,104,25,105,104,76,102,860,107,107,121,109,110,76,867,113,114,
112,116,404,108,107,92,108,91,123,119,108,484,108,108,105,885,108,0,0,108,558,
840,108,761,672,498,108,104,573,66,235,45,143,836,609,601,605,488,440,761,240,
203,644,0,1,326,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,61,- 1,24,78,79,
80,81,82,83,84,85,86,87,- 1,- 1,37,- 1,39,- 1,- 1,487,43,- 1,45,- 1,47,48,49,50,- 1,- 1,
106,- 1,55,56,57,574,840,25,26,- 1,- 1,- 1,65,66,- 1,38,- 1,613,- 1,- 1,38,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,530,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,
104,- 1,76,107,553,109,110,76,628,- 1,38,- 1,116,117,118,564,636,91,92,- 1,94,95,91,
92,98,94,95,- 1,- 1,98,- 1,- 1,- 1,107,- 1,109,110,- 1,107,- 1,109,110,116,- 1,- 1,- 1,- 1,
116,76,123,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,- 1,- 1,91,92,- 1,94,95,- 1,- 1,98,
620,- 1,- 1,- 1,- 1,- 1,- 1,627,107,- 1,109,110,632,- 1,- 1,635,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,
123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,
49,50,51,52,53,54,55,56,57,58,59,711,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,724,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,809,- 1,- 1,812,91,92,93,94,95,96,97,98,99,100,
101,102,103,- 1,- 1,- 1,107,830,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,785,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,866,- 1,868,869,
870,871,872,873,874,875,876,877,878,879,880,881,882,883,884,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,904,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,937,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,
92,93,94,95,96,97,98,99,100,101,102,103,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,
116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,
44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,
98,99,100,101,102,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,
- 1,123,124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,50,
51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,102,- 1,
- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,60,
- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,- 1,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,
- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,
98,99,100,101,- 1,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,
- 1,123,124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,
54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,
107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,0,1,-
1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,- 1,109,
110,- 1,- 1,- 1,- 1,- 1,116,117,118,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,
- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,
100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,1,- 1,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,37,- 1,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,
49,50,116,117,118,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,37,- 1,- 1,96,97,98,99,100,
101,102,- 1,- 1,49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,- 1,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,37,- 1,- 1,
96,97,- 1,99,100,101,102,- 1,- 1,49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,
- 1,116,117,118,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,
- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,103,- 1,37,106,39,-
1,109,110,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
96,97,- 1,99,100,101,45,103,- 1,- 1,49,50,- 1,109,110,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,25,26,27,- 1,29,
30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,
- 1,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,102,103,- 1,- 1,- 1,107,- 1,- 1,
110,- 1,- 1,- 1,- 1,- 1,116,- 1,25,26,120,- 1,- 1,123,124,32,- 1,127,128,- 1,- 1,38,- 1,40,-
1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,-
1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,
- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,
114,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,-
1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,
- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,
- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,
- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,-
1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,
- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,
- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,-
1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,107,
- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,
94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,-
1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,
92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,
25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,
115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,-
1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,
- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,-
1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,
- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,
- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,
- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,-
1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,-
1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,
120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,
93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,
26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,51,52,53,54,- 1,
- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,25,26,-
1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,
51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,
25,26,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,51,52,
53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,- 1,- 1,123,124,- 1,- 1,
127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,51,52,
53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,- 1,- 1,123,124,- 1,- 1,
127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,
- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,96,97,- 1,
99,100,101,- 1,103,- 1,- 1,- 1,- 1,- 1,109,110,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
49,50,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,
102,88,- 1,- 1,- 1,- 1,- 1,109,110,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,
49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,96,- 1,- 1,- 1,100,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _tagged_arr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple17{struct Cyc_Position_Segment*
f1;struct _tuple1*f2;int f3;};struct _tuple18{struct _tagged_arr f1;void*f2;};static
char _tmp430[8]="stdcall";static char _tmp431[6]="cdecl";static char _tmp432[9]="fastcall";
static char _tmp433[9]="noreturn";static char _tmp434[6]="const";static char _tmp435[8]="aligned";
static char _tmp436[7]="packed";static char _tmp437[7]="shared";static char _tmp438[7]="unused";
static char _tmp439[5]="weak";static char _tmp43A[10]="dllimport";static char _tmp43B[
10]="dllexport";static char _tmp43C[23]="no_instrument_function";static char _tmp43D[
12]="constructor";static char _tmp43E[11]="destructor";static char _tmp43F[22]="no_check_memory_usage";
static char _tmp440[5]="pure";struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp9F6=10000;short*_tmp9F7=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp9F6));{unsigned int _tmp9F8=
_tmp9F6;unsigned int i;for(i=0;i < _tmp9F8;i ++){_tmp9F7[i]=(short)0;}}_tmp9F7;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp9F3=10000;void**_tmp9F4=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp9F3));{unsigned int _tmp9F5=
_tmp9F3;unsigned int i;for(i=0;i < _tmp9F5;i ++){_tmp9F4[i]=Cyc_yylval;}}_tmp9F4;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp9F0=
10000;struct Cyc_Yyltype*_tmp9F1=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp9F0));{unsigned int _tmp9F2=_tmp9F0;
unsigned int i;for(i=0;i < _tmp9F2;i ++){_tmp9F1[i]=Cyc_yynewloc();}}_tmp9F1;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp361="parser stack overflow";_tag_arr(_tmp361,sizeof(char),_get_zero_arr_size(
_tmp361,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(953,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar <= 356?(
int)Cyc_yytranslate[_check_known_subscript_notnull(357,Cyc_yychar)]: 266;}yyn +=
yychar1;if((yyn < 0  || yyn > 5868) || Cyc_yycheck[_check_known_subscript_notnull(
5869,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5869,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 952)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(953,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(479,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1D7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1D8: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp362;_push_handler(& _tmp362);{int _tmp364=0;if(setjmp(_tmp362.handler))
_tmp364=1;if(!_tmp364){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp363=(void*)_exn_thrown;void*_tmp366=_tmp363;_LL1DB: if(*((void**)_tmp366)
!= Cyc_Core_Failure)goto _LL1DD;_LL1DC: x=0;goto _LL1DA;_LL1DD:;_LL1DE:(void)_throw(
_tmp366);_LL1DA:;}}}{struct _handler_cons _tmp367;_push_handler(& _tmp367);{int
_tmp369=0;if(setjmp(_tmp367.handler))_tmp369=1;if(!_tmp369){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp368=(void*)_exn_thrown;void*_tmp36B=_tmp368;_LL1E0: if(*((void**)_tmp36B)
!= Cyc_Core_Failure)goto _LL1E2;_LL1E1: y=0;goto _LL1DF;_LL1E2:;_LL1E3:(void)_throw(
_tmp36B);_LL1DF:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp36C=
_cycalloc(sizeof(*_tmp36C));_tmp36C[0]=({struct Cyc_DeclList_tok_struct _tmp36D;
_tmp36D.tag=Cyc_DeclList_tok;_tmp36D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp36D;});_tmp36C;});break;}
case 3: _LL1D9: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp36E=_cycalloc(
sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_DeclList_tok_struct _tmp36F;_tmp36F.tag=
Cyc_DeclList_tok;_tmp36F.f1=({struct Cyc_List_List*_tmp370=_cycalloc(sizeof(*
_tmp370));_tmp370->hd=({struct Cyc_Absyn_Decl*_tmp371=_cycalloc(sizeof(*_tmp371));
_tmp371->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp372=_cycalloc(
sizeof(*_tmp372));_tmp372[0]=({struct Cyc_Absyn_Using_d_struct _tmp373;_tmp373.tag=
9;_tmp373.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp373.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp373;});_tmp372;}));
_tmp371->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp371;});_tmp370->tl=0;_tmp370;});_tmp36F;});
_tmp36E;});Cyc_Lex_leave_using();break;case 4: _LL1E4: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_DeclList_tok_struct
_tmp375;_tmp375.tag=Cyc_DeclList_tok;_tmp375.f1=({struct Cyc_List_List*_tmp376=
_cycalloc(sizeof(*_tmp376));_tmp376->hd=({struct Cyc_Absyn_Decl*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_Absyn_Using_d_struct
_tmp379;_tmp379.tag=9;_tmp379.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp379.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp379;});_tmp378;}));
_tmp377->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp377;});_tmp376->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp376;});_tmp375;});
_tmp374;});break;case 5: _LL1E5: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=({struct Cyc_DeclList_tok_struct
_tmp37B;_tmp37B.tag=Cyc_DeclList_tok;_tmp37B.f1=({struct Cyc_List_List*_tmp37C=
_cycalloc(sizeof(*_tmp37C));_tmp37C->hd=({struct Cyc_Absyn_Decl*_tmp37D=_cycalloc(
sizeof(*_tmp37D));_tmp37D->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp37F;_tmp37F.tag=8;_tmp37F.f1=({struct _tagged_arr*_tmp380=_cycalloc(sizeof(*
_tmp380));_tmp380[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp380;});_tmp37F.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37F;});_tmp37E;}));
_tmp37D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp37D;});_tmp37C->tl=0;_tmp37C;});_tmp37B;});
_tmp37A;});Cyc_Lex_leave_namespace();break;case 6: _LL1E6: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp381;
_push_handler(& _tmp381);{int _tmp383=0;if(setjmp(_tmp381.handler))_tmp383=1;if(!
_tmp383){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp382=(void*)_exn_thrown;void*
_tmp385=_tmp382;_LL1E9: if(*((void**)_tmp385)!= Cyc_Core_Failure)goto _LL1EB;
_LL1EA: nspace=({const char*_tmp386="";_tag_arr(_tmp386,sizeof(char),
_get_zero_arr_size(_tmp386,1));});goto _LL1E8;_LL1EB:;_LL1EC:(void)_throw(_tmp385);
_LL1E8:;}}}{struct _handler_cons _tmp387;_push_handler(& _tmp387);{int _tmp389=0;if(
setjmp(_tmp387.handler))_tmp389=1;if(!_tmp389){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp388=(void*)_exn_thrown;void*_tmp38B=_tmp388;_LL1EE: if(*((void**)_tmp38B)
!= Cyc_Core_Failure)goto _LL1F0;_LL1EF: x=0;goto _LL1ED;_LL1F0:;_LL1F1:(void)_throw(
_tmp38B);_LL1ED:;}}}{struct _handler_cons _tmp38C;_push_handler(& _tmp38C);{int
_tmp38E=0;if(setjmp(_tmp38C.handler))_tmp38E=1;if(!_tmp38E){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp38D=(void*)_exn_thrown;void*_tmp390=_tmp38D;_LL1F3: if(*((void**)_tmp390)
!= Cyc_Core_Failure)goto _LL1F5;_LL1F4: y=0;goto _LL1F2;_LL1F5:;_LL1F6:(void)_throw(
_tmp390);_LL1F2:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp391=
_cycalloc(sizeof(*_tmp391));_tmp391[0]=({struct Cyc_DeclList_tok_struct _tmp392;
_tmp392.tag=Cyc_DeclList_tok;_tmp392.f1=({struct Cyc_List_List*_tmp393=_cycalloc(
sizeof(*_tmp393));_tmp393->hd=({struct Cyc_Absyn_Decl*_tmp394=_cycalloc(sizeof(*
_tmp394));_tmp394->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp395=
_cycalloc(sizeof(*_tmp395));_tmp395[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp396;_tmp396.tag=8;_tmp396.f1=({struct _tagged_arr*_tmp397=_cycalloc(sizeof(*
_tmp397));_tmp397[0]=nspace;_tmp397;});_tmp396.f2=x;_tmp396;});_tmp395;}));
_tmp394->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp394;});_tmp393->tl=y;_tmp393;});_tmp392;});
_tmp391;});break;}case 7: _LL1E7: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp398="C";
_tag_arr(_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,2));}))== 0)yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({
struct Cyc_DeclList_tok_struct _tmp39A;_tmp39A.tag=Cyc_DeclList_tok;_tmp39A.f1=({
struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->hd=({struct Cyc_Absyn_Decl*
_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp39E;_tmp39E.tag=10;_tmp39E.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39E;});_tmp39D;}));
_tmp39C->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp39C;});_tmp39B->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39B;});_tmp39A;});
_tmp399;});else{if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp39F="C include";
_tag_arr(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3A0="expecting \"C\" or \"C include\"";_tag_arr(_tmp3A0,sizeof(
char),_get_zero_arr_size(_tmp3A0,29));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({
struct Cyc_DeclList_tok_struct _tmp3A2;_tmp3A2.tag=Cyc_DeclList_tok;_tmp3A2.f1=({
struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->hd=({struct Cyc_Absyn_Decl*
_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3A6;_tmp3A6.tag=11;_tmp3A6.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A6.f2=0;_tmp3A6;});
_tmp3A5;}));_tmp3A4->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3A4;});
_tmp3A3->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3A3;});_tmp3A2;});_tmp3A1;});}break;case 8: _LL1F7: if(Cyc_strcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3A7="C include";_tag_arr(_tmp3A7,sizeof(char),
_get_zero_arr_size(_tmp3A7,10));}))!= 0)Cyc_Parse_err(({const char*_tmp3A8="expecting \"C include\"";
_tag_arr(_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,22));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_DeclList_tok_struct _tmp3AA;_tmp3AA.tag=Cyc_DeclList_tok;
_tmp3AA.f1=({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->hd=({
struct Cyc_Absyn_Decl*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->r=(void*)((
void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3AD=_cycalloc(sizeof(*
_tmp3AD));_tmp3AD[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3AE;_tmp3AE.tag=
11;_tmp3AE.f1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3AE.f2=exs;_tmp3AE;});_tmp3AD;}));_tmp3AC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3AC;});
_tmp3AB->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3AB;});_tmp3AA;});_tmp3A9;});break;}case 9: _LL1F8: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({
struct Cyc_DeclList_tok_struct _tmp3B0;_tmp3B0.tag=Cyc_DeclList_tok;_tmp3B0.f1=({
struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->hd=({struct Cyc_Absyn_Decl*
_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->r=(void*)((void*)0);_tmp3B2->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3B2;});_tmp3B1->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B1;});_tmp3B0;});
_tmp3AF;});break;case 10: _LL1F9: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_DeclList_tok_struct
_tmp3B4;_tmp3B4.tag=Cyc_DeclList_tok;_tmp3B4.f1=({struct Cyc_List_List*_tmp3B5=
_cycalloc(sizeof(*_tmp3B5));_tmp3B5->hd=({struct Cyc_Absyn_Decl*_tmp3B6=_cycalloc(
sizeof(*_tmp3B6));_tmp3B6->r=(void*)((void*)1);_tmp3B6->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3B6;});
_tmp3B5->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B5;});_tmp3B4;});_tmp3B3;});break;case 11: _LL1FA: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({
struct Cyc_DeclList_tok_struct _tmp3B8;_tmp3B8.tag=Cyc_DeclList_tok;_tmp3B8.f1=0;
_tmp3B8;});_tmp3B7;});break;case 12: _LL1FB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 13: _LL1FC: yyval=(
void*)({struct Cyc_YY5_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({
struct Cyc_YY5_struct _tmp3BA;_tmp3BA.tag=Cyc_YY5;_tmp3BA.f1=0;_tmp3BA;});_tmp3B9;});
break;case 14: _LL1FD: yyval=(void*)({struct Cyc_YY5_struct*_tmp3BB=_cycalloc(
sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_YY5_struct _tmp3BC;_tmp3BC.tag=Cyc_YY5;
_tmp3BC.f1=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=({
struct _tuple17*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BE->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3BE->f3=0;_tmp3BE;});_tmp3BD->tl=0;_tmp3BD;});_tmp3BC;});_tmp3BB;});break;
case 15: _LL1FE: yyval=(void*)({struct Cyc_YY5_struct*_tmp3BF=_cycalloc(sizeof(*
_tmp3BF));_tmp3BF[0]=({struct Cyc_YY5_struct _tmp3C0;_tmp3C0.tag=Cyc_YY5;_tmp3C0.f1=({
struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->hd=({struct
_tuple17*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C2->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3C2->f3=0;_tmp3C2;});_tmp3C1->tl=Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C1;});_tmp3C0;});
_tmp3BF;});break;case 16: _LL1FF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({struct Cyc_DeclList_tok_struct
_tmp3C4;_tmp3C4.tag=Cyc_DeclList_tok;_tmp3C4.f1=({struct Cyc_List_List*_tmp3C5=
_cycalloc(sizeof(*_tmp3C5));_tmp3C5->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3C7;_tmp3C7.tag=1;_tmp3C7.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C7;});_tmp3C6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3C5->tl=0;
_tmp3C5;});_tmp3C4;});_tmp3C3;});break;case 17: _LL200: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL201: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));
_tmp3C8[0]=({struct Cyc_DeclList_tok_struct _tmp3C9;_tmp3C9.tag=Cyc_DeclList_tok;
_tmp3C9.f1=0;_tmp3C9;});_tmp3C8;});break;case 19: _LL202: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_FnDecl_tok_struct
_tmp3CB;_tmp3CB.tag=Cyc_FnDecl_tok;_tmp3CB.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3CB;});
_tmp3CA;});break;case 20: _LL203: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_FnDecl_tok_struct
_tmp3CD;_tmp3CD.tag=Cyc_FnDecl_tok;_tmp3CD.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3CE;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3CD;});
_tmp3CC;});break;case 21: _LL204: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D0;_tmp3D0.tag=Cyc_FnDecl_tok;_tmp3D0.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D0;});
_tmp3CF;});break;case 22: _LL205: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_FnDecl_tok;_tmp3D2.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3D3;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D2;});
_tmp3D1;});break;case 23: _LL206: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D5;_tmp3D5.tag=Cyc_FnDecl_tok;_tmp3D5.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3D6;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D5;});
_tmp3D4;});break;case 24: _LL207: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D8;_tmp3D8.tag=Cyc_FnDecl_tok;_tmp3D8.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3D9;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D8;});
_tmp3D7;});break;case 25: _LL208: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL209: Cyc_Lex_leave_using();
break;case 27: _LL20A: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp3DA=
_cycalloc(sizeof(*_tmp3DA));_tmp3DA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3DA;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL20B: Cyc_Lex_leave_namespace();
break;case 29: _LL20C: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3DB=
_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_DeclList_tok_struct _tmp3DC;
_tmp3DC.tag=Cyc_DeclList_tok;_tmp3DC.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3DC;});
_tmp3DB;});break;case 30: _LL20D: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD[0]=({struct Cyc_DeclList_tok_struct
_tmp3DE;_tmp3DE.tag=Cyc_DeclList_tok;_tmp3DE.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DE;});
_tmp3DD;});break;case 31: _LL20E: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_DeclList_tok_struct
_tmp3E0;_tmp3E0.tag=Cyc_DeclList_tok;_tmp3E0.f1=({struct Cyc_List_List*_tmp3E1=
_cycalloc(sizeof(*_tmp3E1));_tmp3E1->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E1->tl=0;
_tmp3E1;});_tmp3E0;});_tmp3DF;});break;case 32: _LL20F: {struct Cyc_List_List*
_tmp3E2=0;{struct Cyc_List_List*_tmp3E3=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3E3 != 0;
_tmp3E3=_tmp3E3->tl){struct _tagged_arr*_tmp3E4=(struct _tagged_arr*)_tmp3E3->hd;
struct _tuple1*_tmp3E5=({struct _tuple1*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));
_tmp3E8->f1=Cyc_Absyn_rel_ns_null;_tmp3E8->f2=_tmp3E4;_tmp3E8;});struct Cyc_Absyn_Vardecl*
_tmp3E6=Cyc_Absyn_new_vardecl(_tmp3E5,Cyc_Absyn_wildtyp(0),0);_tmp3E2=({struct
Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->hd=_tmp3E6;_tmp3E7->tl=
_tmp3E2;_tmp3E7;});}}_tmp3E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3E2);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3E9=
_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=({struct Cyc_DeclList_tok_struct _tmp3EA;
_tmp3EA.tag=Cyc_DeclList_tok;_tmp3EA.f1=({struct Cyc_List_List*_tmp3EB=_cycalloc(
sizeof(*_tmp3EB));_tmp3EB->hd=Cyc_Absyn_letv_decl(_tmp3E2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EB->tl=0;
_tmp3EB;});_tmp3EA;});_tmp3E9;});break;}case 33: _LL210: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL211: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));
_tmp3EC[0]=({struct Cyc_DeclList_tok_struct _tmp3ED;_tmp3ED.tag=Cyc_DeclList_tok;
_tmp3ED.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3ED;});_tmp3EC;});
break;case 35: _LL212: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3EE=
_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_DeclSpec_tok_struct _tmp3EF;
_tmp3EF.tag=Cyc_DeclSpec_tok;_tmp3EF.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->sc=({struct Cyc_Core_Opt*_tmp3F1=
_cycalloc(sizeof(*_tmp3F1));_tmp3F1->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F1;});_tmp3F0->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3F0->type_specs=0;_tmp3F0->is_inline=0;_tmp3F0->attributes=
0;_tmp3F0;});_tmp3EF;});_tmp3EE;});break;case 36: _LL213: if((Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*
_tmp3F2[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp3F3="Only one storage class is allowed in a declaration";_tag_arr(_tmp3F3,
sizeof(char),_get_zero_arr_size(_tmp3F3,51));}),_tag_arr(_tmp3F2,sizeof(void*),0));});
yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));
_tmp3F4[0]=({struct Cyc_DeclSpec_tok_struct _tmp3F5;_tmp3F5.tag=Cyc_DeclSpec_tok;
_tmp3F5.f1=({struct Cyc_Parse_Declaration_spec*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));
_tmp3F6->sc=({struct Cyc_Core_Opt*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp3F7;});_tmp3F6->tq=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3F6->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3F6->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3F6->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3F6;});
_tmp3F5;});_tmp3F4;});break;case 37: _LL214: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8[0]=({struct Cyc_DeclSpec_tok_struct
_tmp3F9;_tmp3F9.tag=Cyc_DeclSpec_tok;_tmp3F9.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->sc=0;_tmp3FA->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FA->type_specs=({struct Cyc_List_List*_tmp3FB=
_cycalloc(sizeof(*_tmp3FB));_tmp3FB->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FB->tl=0;_tmp3FB;});
_tmp3FA->is_inline=0;_tmp3FA->attributes=0;_tmp3FA;});_tmp3F9;});_tmp3F8;});
break;case 38: _LL215: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3FC=
_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_DeclSpec_tok_struct _tmp3FD;
_tmp3FD.tag=Cyc_DeclSpec_tok;_tmp3FD.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3FE->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3FE->type_specs=({
struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3FF->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3FF;});
_tmp3FE->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp3FE->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3FE;});
_tmp3FD;});_tmp3FC;});break;case 39: _LL216: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400[0]=({struct Cyc_DeclSpec_tok_struct
_tmp401;_tmp401.tag=Cyc_DeclSpec_tok;_tmp401.f1=({struct Cyc_Parse_Declaration_spec*
_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->sc=0;_tmp402->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp402->type_specs=0;
_tmp402->is_inline=0;_tmp402->attributes=0;_tmp402;});_tmp401;});_tmp400;});
break;case 40: _LL217: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp403=
_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_DeclSpec_tok_struct _tmp404;
_tmp404.tag=Cyc_DeclSpec_tok;_tmp404.f1=({struct Cyc_Parse_Declaration_spec*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp405->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp405->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp405->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp405->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp405;});_tmp404;});_tmp403;});break;case 41: _LL218: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406[0]=({struct Cyc_DeclSpec_tok_struct
_tmp407;_tmp407.tag=Cyc_DeclSpec_tok;_tmp407.f1=({struct Cyc_Parse_Declaration_spec*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->sc=0;_tmp408->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp408->type_specs=0;_tmp408->is_inline=1;_tmp408->attributes=
0;_tmp408;});_tmp407;});_tmp406;});break;case 42: _LL219: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=({struct Cyc_DeclSpec_tok_struct
_tmp40A;_tmp40A.tag=Cyc_DeclSpec_tok;_tmp40A.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40B->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40B->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp40B->is_inline=1;_tmp40B->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40B;});
_tmp40A;});_tmp409;});break;case 43: _LL21A: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_DeclSpec_tok_struct
_tmp40D;_tmp40D.tag=Cyc_DeclSpec_tok;_tmp40D.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->sc=0;_tmp40E->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp40E->type_specs=0;_tmp40E->is_inline=0;_tmp40E->attributes=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp40E;});_tmp40D;});_tmp40C;});break;case 44: _LL21B: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({
struct Cyc_DeclSpec_tok_struct _tmp410;_tmp410.tag=Cyc_DeclSpec_tok;_tmp410.f1=({
struct Cyc_Parse_Declaration_spec*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;
_tmp411->tq=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq;_tmp411->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp411->is_inline=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;
_tmp411->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp411;});
_tmp410;});_tmp40F;});break;case 45: _LL21C: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_StorageClass_tok_struct
_tmp413;_tmp413.tag=Cyc_StorageClass_tok;_tmp413.f1=(void*)((void*)4);_tmp413;});
_tmp412;});break;case 46: _LL21D: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414[0]=({struct Cyc_StorageClass_tok_struct
_tmp415;_tmp415.tag=Cyc_StorageClass_tok;_tmp415.f1=(void*)((void*)5);_tmp415;});
_tmp414;});break;case 47: _LL21E: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({struct Cyc_StorageClass_tok_struct
_tmp417;_tmp417.tag=Cyc_StorageClass_tok;_tmp417.f1=(void*)((void*)3);_tmp417;});
_tmp416;});break;case 48: _LL21F: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_StorageClass_tok_struct
_tmp419;_tmp419.tag=Cyc_StorageClass_tok;_tmp419.f1=(void*)((void*)1);_tmp419;});
_tmp418;});break;case 49: _LL220: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp41A="C";
_tag_arr(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp41B="only extern or extern \"C\" is allowed";_tag_arr(_tmp41B,
sizeof(char),_get_zero_arr_size(_tmp41B,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C[0]=({
struct Cyc_StorageClass_tok_struct _tmp41D;_tmp41D.tag=Cyc_StorageClass_tok;
_tmp41D.f1=(void*)((void*)2);_tmp41D;});_tmp41C;});break;case 50: _LL221: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));
_tmp41E[0]=({struct Cyc_StorageClass_tok_struct _tmp41F;_tmp41F.tag=Cyc_StorageClass_tok;
_tmp41F.f1=(void*)((void*)0);_tmp41F;});_tmp41E;});break;case 51: _LL222: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp420=_cycalloc(sizeof(*_tmp420));
_tmp420[0]=({struct Cyc_StorageClass_tok_struct _tmp421;_tmp421.tag=Cyc_StorageClass_tok;
_tmp421.f1=(void*)((void*)6);_tmp421;});_tmp420;});break;case 52: _LL223: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp422=_cycalloc(sizeof(*_tmp422));
_tmp422[0]=({struct Cyc_AttributeList_tok_struct _tmp423;_tmp423.tag=Cyc_AttributeList_tok;
_tmp423.f1=0;_tmp423;});_tmp422;});break;case 53: _LL224: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54: _LL225: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 55: _LL226:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp424=_cycalloc(sizeof(*
_tmp424));_tmp424[0]=({struct Cyc_AttributeList_tok_struct _tmp425;_tmp425.tag=Cyc_AttributeList_tok;
_tmp425.f1=({struct Cyc_List_List*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp426->tl=0;_tmp426;});_tmp425;});_tmp424;});break;case 56:
_LL227: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp427=_cycalloc(
sizeof(*_tmp427));_tmp427[0]=({struct Cyc_AttributeList_tok_struct _tmp428;_tmp428.tag=
Cyc_AttributeList_tok;_tmp428.f1=({struct Cyc_List_List*_tmp429=_cycalloc(sizeof(*
_tmp429));_tmp429->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp429->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp429;});_tmp428;});
_tmp427;});break;case 57: _LL228: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple18 att_map[17]={{{_tmp430,_tmp430,_tmp430 + 8},(
void*)0},{{_tmp431,_tmp431,_tmp431 + 6},(void*)1},{{_tmp432,_tmp432,_tmp432 + 9},(
void*)2},{{_tmp433,_tmp433,_tmp433 + 9},(void*)3},{{_tmp434,_tmp434,_tmp434 + 6},(
void*)4},{{_tmp435,_tmp435,_tmp435 + 8},(void*)& att_aligned},{{_tmp436,_tmp436,
_tmp436 + 7},(void*)5},{{_tmp437,_tmp437,_tmp437 + 7},(void*)7},{{_tmp438,_tmp438,
_tmp438 + 7},(void*)8},{{_tmp439,_tmp439,_tmp439 + 5},(void*)9},{{_tmp43A,_tmp43A,
_tmp43A + 10},(void*)10},{{_tmp43B,_tmp43B,_tmp43B + 10},(void*)11},{{_tmp43C,
_tmp43C,_tmp43C + 23},(void*)12},{{_tmp43D,_tmp43D,_tmp43D + 12},(void*)13},{{
_tmp43E,_tmp43E,_tmp43E + 11},(void*)14},{{_tmp43F,_tmp43F,_tmp43F + 22},(void*)15},{{
_tmp440,_tmp440,_tmp440 + 5},(void*)16}};struct _tagged_arr _tmp42A=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp42A,sizeof(char))> 4  && *((const char*)_check_unknown_subscript(_tmp42A,
sizeof(char),0))== '_') && *((const char*)_check_unknown_subscript(_tmp42A,
sizeof(char),1))== '_') && *((const char*)_check_unknown_subscript(_tmp42A,
sizeof(char),(int)(_get_arr_size(_tmp42A,sizeof(char))- 2)))== '_') && *((const
char*)_check_unknown_subscript(_tmp42A,sizeof(char),(int)(_get_arr_size(_tmp42A,
sizeof(char))- 3)))== '_')_tmp42A=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp42A,2,_get_arr_size(_tmp42A,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp42A,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp42B=_cycalloc(sizeof(*
_tmp42B));_tmp42B[0]=({struct Cyc_Attribute_tok_struct _tmp42C;_tmp42C.tag=Cyc_Attribute_tok;
_tmp42C.f1=(void*)(att_map[i]).f2;_tmp42C;});_tmp42B;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp42D="unrecognized attribute";_tag_arr(_tmp42D,sizeof(char),
_get_zero_arr_size(_tmp42D,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({
struct Cyc_Attribute_tok_struct _tmp42F;_tmp42F.tag=Cyc_Attribute_tok;_tmp42F.f1=(
void*)((void*)1);_tmp42F;});_tmp42E;});}break;}}case 58: _LL229: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({
struct Cyc_Attribute_tok_struct _tmp443;_tmp443.tag=Cyc_Attribute_tok;_tmp443.f1=(
void*)((void*)4);_tmp443;});_tmp442;});break;case 59: _LL22A: {struct _tagged_arr
_tmp444=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp446;int _tmp447;struct _tuple13*_tmp445=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp446=*_tmp445;
_tmp447=_tmp446.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp444,({const char*
_tmp448="regparm";_tag_arr(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,8));}))
== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp444,({const char*_tmp449="__regparm__";
_tag_arr(_tmp449,sizeof(char),_get_zero_arr_size(_tmp449,12));}))== 0){if(
_tmp447 < 0  || _tmp447 > 3)Cyc_Parse_err(({const char*_tmp44A="regparm requires value between 0 and 3";
_tag_arr(_tmp44A,sizeof(char),_get_zero_arr_size(_tmp44A,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp44B=_cycalloc_atomic(sizeof(*_tmp44B));
_tmp44B[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp44C;_tmp44C.tag=0;_tmp44C.f1=
_tmp447;_tmp44C;});_tmp44B;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp444,({
const char*_tmp44D="aligned";_tag_arr(_tmp44D,sizeof(char),_get_zero_arr_size(
_tmp44D,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp444,({const char*
_tmp44E="__aligned__";_tag_arr(_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,
12));}))== 0){if(_tmp447 < 0)Cyc_Parse_err(({const char*_tmp44F="aligned requires positive power of two";
_tag_arr(_tmp44F,sizeof(char),_get_zero_arr_size(_tmp44F,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp447;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp450="aligned requires positive power of two";
_tag_arr(_tmp450,sizeof(char),_get_zero_arr_size(_tmp450,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp451=_cycalloc_atomic(sizeof(*_tmp451));
_tmp451[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp452;_tmp452.tag=1;_tmp452.f1=
_tmp447;_tmp452;});_tmp451;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp444,({
const char*_tmp453="initializes";_tag_arr(_tmp453,sizeof(char),_get_zero_arr_size(
_tmp453,12));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp444,({const char*
_tmp454="__initializes__";_tag_arr(_tmp454,sizeof(char),_get_zero_arr_size(
_tmp454,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp455=
_cycalloc_atomic(sizeof(*_tmp455));_tmp455[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp456;_tmp456.tag=4;_tmp456.f1=_tmp447;_tmp456;});_tmp455;});else{Cyc_Parse_err(({
const char*_tmp457="unrecognized attribute";_tag_arr(_tmp457,sizeof(char),
_get_zero_arr_size(_tmp457,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp458=_cycalloc(sizeof(*_tmp458));
_tmp458[0]=({struct Cyc_Attribute_tok_struct _tmp459;_tmp459.tag=Cyc_Attribute_tok;
_tmp459.f1=(void*)a;_tmp459;});_tmp458;});break;}}case 60: _LL22B: {struct
_tagged_arr _tmp45A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp45B=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp45A,({const char*_tmp45C="section";_tag_arr(_tmp45C,sizeof(
char),_get_zero_arr_size(_tmp45C,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)
_tmp45A,({const char*_tmp45D="__section__";_tag_arr(_tmp45D,sizeof(char),
_get_zero_arr_size(_tmp45D,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Absyn_Section_att_struct
_tmp45F;_tmp45F.tag=2;_tmp45F.f1=_tmp45B;_tmp45F;});_tmp45E;});else{Cyc_Parse_err(({
const char*_tmp460="unrecognized attribute";_tag_arr(_tmp460,sizeof(char),
_get_zero_arr_size(_tmp460,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461[0]=({struct Cyc_Attribute_tok_struct _tmp462;_tmp462.tag=Cyc_Attribute_tok;
_tmp462.f1=(void*)a;_tmp462;});_tmp461;});break;}case 61: _LL22C: {struct
_tagged_arr _tmp463=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp464=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp463,({const char*_tmp465="__mode__";_tag_arr(_tmp465,
sizeof(char),_get_zero_arr_size(_tmp465,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp467;_tmp467.tag=5;_tmp467.f1=_tmp464;_tmp467;});_tmp466;});else{Cyc_Parse_err(({
const char*_tmp468="unrecognized attribute";_tag_arr(_tmp468,sizeof(char),
_get_zero_arr_size(_tmp468,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp469=_cycalloc(sizeof(*_tmp469));
_tmp469[0]=({struct Cyc_Attribute_tok_struct _tmp46A;_tmp46A.tag=Cyc_Attribute_tok;
_tmp46A.f1=(void*)a;_tmp46A;});_tmp469;});break;}case 62: _LL22D: {struct
_tagged_arr _tmp46B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp46C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp46E;struct
_tuple13 _tmp46D=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp46E=_tmp46D.f2;{int _tmp470;struct _tuple13 _tmp46F=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp470=_tmp46F.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp46B,({const char*_tmp471="format";
_tag_arr(_tmp471,sizeof(char),_get_zero_arr_size(_tmp471,7));}))== 0  || Cyc_zstrcmp((
struct _tagged_arr)_tmp46B,({const char*_tmp472="__format__";_tag_arr(_tmp472,
sizeof(char),_get_zero_arr_size(_tmp472,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp46C,({const char*_tmp473="printf";_tag_arr(_tmp473,sizeof(char),
_get_zero_arr_size(_tmp473,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474[0]=({struct Cyc_Absyn_Format_att_struct
_tmp475;_tmp475.tag=3;_tmp475.f1=(void*)((void*)0);_tmp475.f2=_tmp46E;_tmp475.f3=
_tmp470;_tmp475;});_tmp474;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp46C,({
const char*_tmp476="scanf";_tag_arr(_tmp476,sizeof(char),_get_zero_arr_size(
_tmp476,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp477=
_cycalloc(sizeof(*_tmp477));_tmp477[0]=({struct Cyc_Absyn_Format_att_struct
_tmp478;_tmp478.tag=3;_tmp478.f1=(void*)((void*)1);_tmp478.f2=_tmp46E;_tmp478.f3=
_tmp470;_tmp478;});_tmp477;});else{Cyc_Parse_err(({const char*_tmp479="unrecognized format type";
_tag_arr(_tmp479,sizeof(char),_get_zero_arr_size(_tmp479,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp47A="unrecognized attribute";_tag_arr(_tmp47A,sizeof(char),
_get_zero_arr_size(_tmp47A,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({
struct Cyc_Attribute_tok_struct _tmp47C;_tmp47C.tag=Cyc_Attribute_tok;_tmp47C.f1=(
void*)a;_tmp47C;});_tmp47B;});break;}}}case 63: _LL22E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL22F: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));
_tmp47D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp47E;_tmp47E.tag=Cyc_TypeSpecifier_tok;
_tmp47E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp480;_tmp480.tag=16;_tmp480.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp480.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp480.f3=0;_tmp480.f4=
0;_tmp480;});_tmp47F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47E;});
_tmp47D;});break;case 65: _LL230: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp482;_tmp482.tag=Cyc_TypeSpecifier_tok;_tmp482.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp482;});_tmp481;});break;case 66: _LL231: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp484;_tmp484.tag=Cyc_TypeSpecifier_tok;
_tmp484.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp484;});
_tmp483;});break;case 67: _LL232: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp486;_tmp486.tag=Cyc_TypeSpecifier_tok;_tmp486.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp487=_cycalloc(sizeof(*_tmp487));
_tmp487->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp487;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp486;});
_tmp485;});break;case 68: _LL233: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp489;_tmp489.tag=Cyc_TypeSpecifier_tok;_tmp489.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp489;});
_tmp488;});break;case 69: _LL234: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48B;_tmp48B.tag=Cyc_TypeSpecifier_tok;_tmp48B.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Parse_Short_spec_struct
_tmp48D;_tmp48D.tag=2;_tmp48D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp48D;});
_tmp48C;}));_tmp48B;});_tmp48A;});break;case 70: _LL235: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48F;_tmp48F.tag=Cyc_TypeSpecifier_tok;_tmp48F.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48F;});
_tmp48E;});break;case 71: _LL236: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp491;_tmp491.tag=Cyc_TypeSpecifier_tok;_tmp491.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492[0]=({struct Cyc_Parse_Long_spec_struct
_tmp493;_tmp493.tag=3;_tmp493.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp493;});
_tmp492;}));_tmp491;});_tmp490;});break;case 72: _LL237: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp495;_tmp495.tag=Cyc_TypeSpecifier_tok;_tmp495.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp495;});
_tmp494;});break;case 73: _LL238: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp497;_tmp497.tag=Cyc_TypeSpecifier_tok;_tmp497.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp497;});
_tmp496;});break;case 74: _LL239: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp499;_tmp499.tag=Cyc_TypeSpecifier_tok;_tmp499.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp49B;_tmp49B.tag=0;_tmp49B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49B;});
_tmp49A;}));_tmp499;});_tmp498;});break;case 75: _LL23A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49D;_tmp49D.tag=Cyc_TypeSpecifier_tok;_tmp49D.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp49F;_tmp49F.tag=1;_tmp49F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49F;});
_tmp49E;}));_tmp49D;});_tmp49C;});break;case 76: _LL23B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL23C: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL23D:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL23E: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4A0=_cycalloc(
sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4A1;_tmp4A1.tag=
Cyc_TypeSpecifier_tok;_tmp4A1.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A1;});
_tmp4A0;});break;case 80: _LL23F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_TypeSpecifier_tok;_tmp4A3.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));
_tmp4A4[0]=({struct Cyc_Absyn_TupleType_struct _tmp4A5;_tmp4A5.tag=9;_tmp4A5.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4A5;});_tmp4A4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4A3;});_tmp4A2;});break;case 81: _LL240: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));
_tmp4A6[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4A7;_tmp4A7.tag=Cyc_TypeSpecifier_tok;
_tmp4A7.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4A9;_tmp4A9.tag=15;_tmp4A9.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A9;});_tmp4A8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4A7;});_tmp4A6;});break;case 82: _LL241: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));
_tmp4AA[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4AB;_tmp4AB.tag=Cyc_TypeSpecifier_tok;
_tmp4AB.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp4AD;_tmp4AD.tag=14;_tmp4AD.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AD;});_tmp4AC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AB;});_tmp4AA;});break;case 83: _LL242: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));
_tmp4AE[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4AF;_tmp4AF.tag=Cyc_TypeSpecifier_tok;
_tmp4AF.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_Absyn_TagType_struct
_tmp4B1;_tmp4B1.tag=17;_tmp4B1.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B1;});_tmp4B0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AF;});_tmp4AE;});break;case 84: _LL243: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));
_tmp4B2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B3;_tmp4B3.tag=Cyc_TypeSpecifier_tok;
_tmp4B3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Absyn_TagType_struct
_tmp4B5;_tmp4B5.tag=17;_tmp4B5.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->v=(void*)((void*)5);_tmp4B6;}),0);
_tmp4B5;});_tmp4B4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B3;});
_tmp4B2;});break;case 85: _LL244: {struct _tuple13 _tmp4B8;int _tmp4B9;struct _tuple13*
_tmp4B7=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4B8=*_tmp4B7;_tmp4B9=_tmp4B8.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4BB;_tmp4BB.tag=Cyc_TypeSpecifier_tok;_tmp4BB.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp4BC=_cycalloc_atomic(sizeof(*_tmp4BC));
_tmp4BC[0]=({struct Cyc_Absyn_TypeInt_struct _tmp4BD;_tmp4BD.tag=18;_tmp4BD.f1=
_tmp4B9;_tmp4BD;});_tmp4BC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BB;});
_tmp4BA;});break;}case 86: _LL245: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp4BE=
_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({struct Cyc_Kind_tok_struct _tmp4BF;
_tmp4BF.tag=Cyc_Kind_tok;_tmp4BF.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BF;});
_tmp4BE;});break;case 87: _LL246: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_TypeQual_tok_struct
_tmp4C1;_tmp4C1.tag=Cyc_TypeQual_tok;_tmp4C1.f1=({struct Cyc_Absyn_Tqual _tmp4C2;
_tmp4C2.print_const=1;_tmp4C2.q_volatile=0;_tmp4C2.q_restrict=0;_tmp4C2.real_const=
1;_tmp4C2.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4C2;});_tmp4C1;});_tmp4C0;});break;case 88: _LL247:
yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));
_tmp4C3[0]=({struct Cyc_TypeQual_tok_struct _tmp4C4;_tmp4C4.tag=Cyc_TypeQual_tok;
_tmp4C4.f1=({struct Cyc_Absyn_Tqual _tmp4C5;_tmp4C5.print_const=0;_tmp4C5.q_volatile=
1;_tmp4C5.q_restrict=0;_tmp4C5.real_const=0;_tmp4C5.loc=0;_tmp4C5;});_tmp4C4;});
_tmp4C3;});break;case 89: _LL248: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_TypeQual_tok_struct
_tmp4C7;_tmp4C7.tag=Cyc_TypeQual_tok;_tmp4C7.f1=({struct Cyc_Absyn_Tqual _tmp4C8;
_tmp4C8.print_const=0;_tmp4C8.q_volatile=0;_tmp4C8.q_restrict=1;_tmp4C8.real_const=
0;_tmp4C8.loc=0;_tmp4C8;});_tmp4C7;});_tmp4C6;});break;case 90: _LL249: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4CA;_tmp4CA.tag=Cyc_TypeSpecifier_tok;
_tmp4CA.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4CB=_cycalloc(
sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4CC;_tmp4CC.tag=
5;_tmp4CC.f1=({struct Cyc_Absyn_Decl*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));
_tmp4CE[0]=({struct Cyc_Absyn_Enum_d_struct _tmp4CF;_tmp4CF.tag=6;_tmp4CF.f1=({
struct Cyc_Absyn_Enumdecl*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->sc=(void*)((
void*)2);_tmp4D0->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4D0->fields=({struct Cyc_Core_Opt*_tmp4D1=_cycalloc(
sizeof(*_tmp4D1));_tmp4D1->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D1;});_tmp4D0;});
_tmp4CF;});_tmp4CE;}));_tmp4CD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4CD;});
_tmp4CC;});_tmp4CB;}));_tmp4CA;});_tmp4C9;});break;case 91: _LL24A: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4D3;_tmp4D3.tag=Cyc_TypeSpecifier_tok;
_tmp4D3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({struct Cyc_Absyn_EnumType_struct
_tmp4D5;_tmp4D5.tag=12;_tmp4D5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D5.f2=0;_tmp4D5;});
_tmp4D4;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D3;});_tmp4D2;});break;case 92: _LL24B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));
_tmp4D6[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4D7;_tmp4D7.tag=Cyc_TypeSpecifier_tok;
_tmp4D7.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4D8=_cycalloc(
sizeof(*_tmp4D8));_tmp4D8[0]=({struct Cyc_Parse_Type_spec_struct _tmp4D9;_tmp4D9.tag=
4;_tmp4D9.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp4DA=
_cycalloc(sizeof(*_tmp4DA));_tmp4DA[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4DB;_tmp4DB.tag=13;_tmp4DB.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DB;});_tmp4DA;}));
_tmp4D9.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4D9;});_tmp4D8;}));_tmp4D7;});_tmp4D6;});break;
case 93: _LL24C: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4DC=_cycalloc(
sizeof(*_tmp4DC));_tmp4DC[0]=({struct Cyc_Enumfield_tok_struct _tmp4DD;_tmp4DD.tag=
Cyc_Enumfield_tok;_tmp4DD.f1=({struct Cyc_Absyn_Enumfield*_tmp4DE=_cycalloc(
sizeof(*_tmp4DE));_tmp4DE->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4DE->tag=0;_tmp4DE->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4DE;});_tmp4DD;});_tmp4DC;});break;case 94: _LL24D:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));
_tmp4DF[0]=({struct Cyc_Enumfield_tok_struct _tmp4E0;_tmp4E0.tag=Cyc_Enumfield_tok;
_tmp4E0.f1=({struct Cyc_Absyn_Enumfield*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));
_tmp4E1->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4E1->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4E1;});
_tmp4E0;});_tmp4DF;});break;case 95: _LL24E: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4E3;_tmp4E3.tag=Cyc_EnumfieldList_tok;_tmp4E3.f1=({struct Cyc_List_List*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E4->tl=0;_tmp4E4;});
_tmp4E3;});_tmp4E2;});break;case 96: _LL24F: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4E6;_tmp4E6.tag=Cyc_EnumfieldList_tok;_tmp4E6.f1=({struct Cyc_List_List*
_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E7->tl=0;_tmp4E7;});
_tmp4E6;});_tmp4E5;});break;case 97: _LL250: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4E9;_tmp4E9.tag=Cyc_EnumfieldList_tok;_tmp4E9.f1=({struct Cyc_List_List*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EA->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EA;});_tmp4E9;});
_tmp4E8;});break;case 98: _LL251: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4EC;_tmp4EC.tag=Cyc_TypeSpecifier_tok;_tmp4EC.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));
_tmp4ED[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4EE;_tmp4EE.tag=11;_tmp4EE.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4EE.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EE;});_tmp4ED;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4EC;});_tmp4EB;});break;case 99: _LL252: {struct
Cyc_List_List*_tmp4EF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4F2;_tmp4F2.tag=Cyc_TypeSpecifier_tok;
_tmp4F2.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4F3=_cycalloc(
sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4F4;_tmp4F4.tag=
5;_tmp4F4.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4EF,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4F0,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F4;});
_tmp4F3;}));_tmp4F2;});_tmp4F1;});break;}case 100: _LL253: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4F6;_tmp4F6.tag=Cyc_TypeSpecifier_tok;_tmp4F6.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));
_tmp4F7[0]=({struct Cyc_Absyn_AggrType_struct _tmp4F8;_tmp4F8.tag=10;_tmp4F8.f1=({
struct Cyc_Absyn_AggrInfo _tmp4F9;_tmp4F9.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FB.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FB;});_tmp4FA;}));
_tmp4F9.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F9;});_tmp4F8;});_tmp4F7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F6;});
_tmp4F5;});break;case 101: _LL254: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_TypeList_tok_struct
_tmp4FD;_tmp4FD.tag=Cyc_TypeList_tok;_tmp4FD.f1=0;_tmp4FD;});_tmp4FC;});break;
case 102: _LL255: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp4FE=_cycalloc(
sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_TypeList_tok_struct _tmp4FF;_tmp4FF.tag=
Cyc_TypeList_tok;_tmp4FF.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4FF;});_tmp4FE;});
break;case 103: _LL256: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp500=
_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp501;_tmp501.tag=Cyc_StructOrUnion_tok;_tmp501.f1=(void*)((void*)0);_tmp501;});
_tmp500;});break;case 104: _LL257: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp503;_tmp503.tag=Cyc_StructOrUnion_tok;_tmp503.f1=(void*)((void*)1);_tmp503;});
_tmp502;});break;case 105: _LL258: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp505;_tmp505.tag=Cyc_AggrFieldDeclList_tok;_tmp505.f1=0;_tmp505;});_tmp504;});
break;case 106: _LL259: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp507;_tmp507.tag=Cyc_AggrFieldDeclList_tok;_tmp507.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp507;});_tmp506;});
break;case 107: _LL25A: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp509;_tmp509.tag=Cyc_AggrFieldDeclListList_tok;_tmp509.f1=({struct Cyc_List_List*
_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50A->tl=0;_tmp50A;});
_tmp509;});_tmp508;});break;case 108: _LL25B: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp50C;_tmp50C.tag=Cyc_AggrFieldDeclListList_tok;_tmp50C.f1=({struct Cyc_List_List*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50D->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50D;});_tmp50C;});
_tmp50B;});break;case 109: _LL25C: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_InitDeclList_tok_struct
_tmp50F;_tmp50F.tag=Cyc_InitDeclList_tok;_tmp50F.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp50F;});_tmp50E;});
break;case 110: _LL25D: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp510=
_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_InitDeclList_tok_struct
_tmp511;_tmp511.tag=Cyc_InitDeclList_tok;_tmp511.f1=({struct Cyc_List_List*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp512->tl=0;_tmp512;});
_tmp511;});_tmp510;});break;case 111: _LL25E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513[0]=({struct Cyc_InitDeclList_tok_struct
_tmp514;_tmp514.tag=Cyc_InitDeclList_tok;_tmp514.f1=({struct Cyc_List_List*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp515->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp515;});_tmp514;});
_tmp513;});break;case 112: _LL25F: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_InitDecl_tok_struct
_tmp517;_tmp517.tag=Cyc_InitDecl_tok;_tmp517.f1=({struct _tuple12*_tmp518=
_cycalloc(sizeof(*_tmp518));_tmp518->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp518->f2=0;_tmp518;});
_tmp517;});_tmp516;});break;case 113: _LL260: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519[0]=({struct Cyc_InitDecl_tok_struct
_tmp51A;_tmp51A.tag=Cyc_InitDecl_tok;_tmp51A.f1=({struct _tuple12*_tmp51B=
_cycalloc(sizeof(*_tmp51B));_tmp51B->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51B->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp51B;});_tmp51A;});_tmp519;});break;case 114: _LL261: {struct _tuple15 _tmp51D;
struct Cyc_Absyn_Tqual _tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*
_tmp520;struct _tuple15*_tmp51C=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51D=*_tmp51C;_tmp51E=
_tmp51D.f1;_tmp51F=_tmp51D.f2;_tmp520=_tmp51D.f3;if(_tmp51E.loc == 0)_tmp51E.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp522;struct Cyc_List_List*
_tmp523;struct _tuple0 _tmp521=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp522=_tmp521.f1;_tmp523=_tmp521.f2;{void*_tmp524=Cyc_Parse_speclist2typ(
_tmp51F,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp525=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp51E,_tmp524,_tmp522,_tmp520),_tmp523);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp527;_tmp527.tag=Cyc_AggrFieldDeclList_tok;_tmp527.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp525);_tmp527;});_tmp526;});break;}}}case 115:
_LL262: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp528=_cycalloc(
sizeof(*_tmp528));_tmp528[0]=({struct Cyc_QualSpecList_tok_struct _tmp529;_tmp529.tag=
Cyc_QualSpecList_tok;_tmp529.f1=({struct _tuple15*_tmp52A=_cycalloc(sizeof(*
_tmp52A));_tmp52A->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52A->f2=({
struct Cyc_List_List*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52B->tl=0;_tmp52B;});
_tmp52A->f3=0;_tmp52A;});_tmp529;});_tmp528;});break;case 116: _LL263: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C[0]=({
struct Cyc_QualSpecList_tok_struct _tmp52D;_tmp52D.tag=Cyc_QualSpecList_tok;
_tmp52D.f1=({struct _tuple15*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp52E->f2=({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*
_tmp52F));_tmp52F->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52F->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp52F;});_tmp52E->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp52E;});_tmp52D;});_tmp52C;});break;case 117: _LL264: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp530=_cycalloc(sizeof(*_tmp530));
_tmp530[0]=({struct Cyc_QualSpecList_tok_struct _tmp531;_tmp531.tag=Cyc_QualSpecList_tok;
_tmp531.f1=({struct _tuple15*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp532->f2=0;_tmp532->f3=
0;_tmp532;});_tmp531;});_tmp530;});break;case 118: _LL265: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533[0]=({struct Cyc_QualSpecList_tok_struct
_tmp534;_tmp534.tag=Cyc_QualSpecList_tok;_tmp534.f1=({struct _tuple15*_tmp535=
_cycalloc(sizeof(*_tmp535));_tmp535->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp535->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp535->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp535;});_tmp534;});
_tmp533;});break;case 119: _LL266: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_QualSpecList_tok_struct
_tmp537;_tmp537.tag=Cyc_QualSpecList_tok;_tmp537.f1=({struct _tuple15*_tmp538=
_cycalloc(sizeof(*_tmp538));_tmp538->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp538->f2=0;
_tmp538->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp538;});_tmp537;});_tmp536;});break;case 120: _LL267:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp539=_cycalloc(sizeof(*
_tmp539));_tmp539[0]=({struct Cyc_QualSpecList_tok_struct _tmp53A;_tmp53A.tag=Cyc_QualSpecList_tok;
_tmp53A.f1=({struct _tuple15*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp53B->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53B->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp53B;});_tmp53A;});
_tmp539;});break;case 121: _LL268: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_QualSpecList_tok_struct
_tmp53D;_tmp53D.tag=Cyc_QualSpecList_tok;_tmp53D.f1=({struct _tuple15*_tmp53E=
_cycalloc(sizeof(*_tmp53E));_tmp53E->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp53E->f2=({
struct Cyc_List_List*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53F->tl=0;_tmp53F;});
_tmp53E->f3=0;_tmp53E;});_tmp53D;});_tmp53C;});break;case 122: _LL269: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540[0]=({
struct Cyc_QualSpecList_tok_struct _tmp541;_tmp541.tag=Cyc_QualSpecList_tok;
_tmp541.f1=({struct _tuple15*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp542->f2=({struct Cyc_List_List*_tmp543=_cycalloc(sizeof(*
_tmp543));_tmp543->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp543->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp543;});_tmp542->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp542;});_tmp541;});_tmp540;});break;case 123: _LL26A: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp544=_cycalloc(sizeof(*_tmp544));
_tmp544[0]=({struct Cyc_QualSpecList_tok_struct _tmp545;_tmp545.tag=Cyc_QualSpecList_tok;
_tmp545.f1=({struct _tuple15*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp546->f2=0;_tmp546->f3=
0;_tmp546;});_tmp545;});_tmp544;});break;case 124: _LL26B: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_QualSpecList_tok_struct
_tmp548;_tmp548.tag=Cyc_QualSpecList_tok;_tmp548.f1=({struct _tuple15*_tmp549=
_cycalloc(sizeof(*_tmp549));_tmp549->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp549->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp549->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp549;});_tmp548;});
_tmp547;});break;case 125: _LL26C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A[0]=({struct Cyc_QualSpecList_tok_struct
_tmp54B;_tmp54B.tag=Cyc_QualSpecList_tok;_tmp54B.f1=({struct _tuple15*_tmp54C=
_cycalloc(sizeof(*_tmp54C));_tmp54C->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp54C->f2=0;
_tmp54C->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp54C;});_tmp54B;});_tmp54A;});break;case 126: _LL26D:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp54D=_cycalloc(sizeof(*
_tmp54D));_tmp54D[0]=({struct Cyc_QualSpecList_tok_struct _tmp54E;_tmp54E.tag=Cyc_QualSpecList_tok;
_tmp54E.f1=({struct _tuple15*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp54F->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp54F->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp54F;});_tmp54E;});
_tmp54D;});break;case 127: _LL26E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({struct Cyc_InitDeclList_tok_struct
_tmp551;_tmp551.tag=Cyc_InitDeclList_tok;_tmp551.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp551;});_tmp550;});
break;case 128: _LL26F: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_InitDeclList_tok_struct
_tmp553;_tmp553.tag=Cyc_InitDeclList_tok;_tmp553.f1=({struct Cyc_List_List*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp554->tl=0;_tmp554;});
_tmp553;});_tmp552;});break;case 129: _LL270: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_InitDeclList_tok_struct
_tmp556;_tmp556.tag=Cyc_InitDeclList_tok;_tmp556.f1=({struct Cyc_List_List*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp557->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp557;});_tmp556;});
_tmp555;});break;case 130: _LL271: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558[0]=({struct Cyc_InitDecl_tok_struct
_tmp559;_tmp559.tag=Cyc_InitDecl_tok;_tmp559.f1=({struct _tuple12*_tmp55A=
_cycalloc(sizeof(*_tmp55A));_tmp55A->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55A->f2=0;_tmp55A;});
_tmp559;});_tmp558;});break;case 131: _LL272: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_InitDecl_tok_struct
_tmp55C;_tmp55C.tag=Cyc_InitDecl_tok;_tmp55C.f1=({struct _tuple12*_tmp55D=
_cycalloc(sizeof(*_tmp55D));_tmp55D->f1=({struct Cyc_Parse_Declarator*_tmp55E=
_cycalloc(sizeof(*_tmp55E));_tmp55E->id=({struct _tuple1*_tmp55F=_cycalloc(
sizeof(*_tmp55F));_tmp55F->f1=Cyc_Absyn_rel_ns_null;_tmp55F->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp55F;});_tmp55E->tms=
0;_tmp55E;});_tmp55D->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55D;});_tmp55C;});
_tmp55B;});break;case 132: _LL273: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_InitDecl_tok_struct
_tmp562;_tmp562.tag=Cyc_InitDecl_tok;_tmp562.f1=({struct _tuple12*_tmp563=
_cycalloc(sizeof(*_tmp563));_tmp563->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp563->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp563;});_tmp562;});_tmp561;});break;case 133: _LL274: {struct Cyc_List_List*
_tmp564=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp566;_tmp566.tag=Cyc_TypeSpecifier_tok;
_tmp566.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp567=_cycalloc(
sizeof(*_tmp567));_tmp567[0]=({struct Cyc_Parse_Decl_spec_struct _tmp568;_tmp568.tag=
5;_tmp568.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp564,({struct Cyc_Core_Opt*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp569;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp568;});
_tmp567;}));_tmp566;});_tmp565;});break;}case 134: _LL275: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp56B;_tmp56B.tag=Cyc_TypeSpecifier_tok;_tmp56B.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C[0]=({struct Cyc_Absyn_TunionType_struct _tmp56D;_tmp56D.tag=2;_tmp56D.f1=({
struct Cyc_Absyn_TunionInfo _tmp56E;_tmp56E.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp570;_tmp570.tag=0;_tmp570.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp571;
_tmp571.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp571.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp571;});_tmp570;});
_tmp56F;}));_tmp56E.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56E.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56E;});_tmp56D;});
_tmp56C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp56B;});_tmp56A;});break;case 135: _LL276: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp572=_cycalloc(sizeof(*_tmp572));
_tmp572[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp573;_tmp573.tag=Cyc_TypeSpecifier_tok;
_tmp573.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp575;_tmp575.tag=3;_tmp575.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp576;
_tmp576.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp578;_tmp578.tag=0;_tmp578.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp579;
_tmp579.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp579.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp579.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp579;});_tmp578;});
_tmp577;}));_tmp576.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp576;});_tmp575;});
_tmp574;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp573;});_tmp572;});break;case 136: _LL277: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp57A=_cycalloc_atomic(sizeof(*_tmp57A));
_tmp57A[0]=({struct Cyc_Bool_tok_struct _tmp57B;_tmp57B.tag=Cyc_Bool_tok;_tmp57B.f1=
0;_tmp57B;});_tmp57A;});break;case 137: _LL278: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp57C=_cycalloc_atomic(sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_Bool_tok_struct
_tmp57D;_tmp57D.tag=Cyc_Bool_tok;_tmp57D.f1=1;_tmp57D;});_tmp57C;});break;case
138: _LL279: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp57E=_cycalloc(
sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_TunionFieldList_tok_struct _tmp57F;
_tmp57F.tag=Cyc_TunionFieldList_tok;_tmp57F.f1=({struct Cyc_List_List*_tmp580=
_cycalloc(sizeof(*_tmp580));_tmp580->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp580->tl=0;_tmp580;});
_tmp57F;});_tmp57E;});break;case 139: _LL27A: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp582;_tmp582.tag=Cyc_TunionFieldList_tok;_tmp582.f1=({struct Cyc_List_List*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp583->tl=0;_tmp583;});
_tmp582;});_tmp581;});break;case 140: _LL27B: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp585;_tmp585.tag=Cyc_TunionFieldList_tok;_tmp585.f1=({struct Cyc_List_List*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp586->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp586;});_tmp585;});
_tmp584;});break;case 141: _LL27C: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp588;_tmp588.tag=Cyc_TunionFieldList_tok;_tmp588.f1=({struct Cyc_List_List*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp589->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp589;});_tmp588;});
_tmp587;});break;case 142: _LL27D: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_Scope_tok_struct
_tmp58B;_tmp58B.tag=Cyc_Scope_tok;_tmp58B.f1=(void*)((void*)2);_tmp58B;});
_tmp58A;});break;case 143: _LL27E: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_Scope_tok_struct
_tmp58D;_tmp58D.tag=Cyc_Scope_tok;_tmp58D.f1=(void*)((void*)3);_tmp58D;});
_tmp58C;});break;case 144: _LL27F: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E[0]=({struct Cyc_Scope_tok_struct
_tmp58F;_tmp58F.tag=Cyc_Scope_tok;_tmp58F.f1=(void*)((void*)0);_tmp58F;});
_tmp58E;});break;case 145: _LL280: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_TunionField_tok_struct
_tmp591;_tmp591.tag=Cyc_TunionField_tok;_tmp591.f1=({struct Cyc_Absyn_Tunionfield*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp592->typs=0;_tmp592->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp592->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp592;});_tmp591;});
_tmp590;});break;case 146: _LL281: {struct Cyc_List_List*_tmp593=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594[0]=({
struct Cyc_TunionField_tok_struct _tmp595;_tmp595.tag=Cyc_TunionField_tok;_tmp595.f1=({
struct Cyc_Absyn_Tunionfield*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp596->typs=_tmp593;_tmp596->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp596->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp596;});_tmp595;});_tmp594;});break;}case 147: _LL282: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 148: _LL283: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp597=_cycalloc(sizeof(*_tmp597));
_tmp597[0]=({struct Cyc_Declarator_tok_struct _tmp598;_tmp598.tag=Cyc_Declarator_tok;
_tmp598.f1=({struct Cyc_Parse_Declarator*_tmp599=_cycalloc(sizeof(*_tmp599));
_tmp599->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp599->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp599;});
_tmp598;});_tmp597;});break;case 149: _LL284: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 150: _LL285: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp59A=_cycalloc(sizeof(*_tmp59A));
_tmp59A[0]=({struct Cyc_Declarator_tok_struct _tmp59B;_tmp59B.tag=Cyc_Declarator_tok;
_tmp59B.f1=({struct Cyc_Parse_Declarator*_tmp59C=_cycalloc(sizeof(*_tmp59C));
_tmp59C->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp59C->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp59C;});
_tmp59B;});_tmp59A;});break;case 151: _LL286: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_Declarator_tok_struct
_tmp59E;_tmp59E.tag=Cyc_Declarator_tok;_tmp59E.f1=({struct Cyc_Parse_Declarator*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59F->tms=0;_tmp59F;});
_tmp59E;});_tmp59D;});break;case 152: _LL287: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 153: _LL288: {
struct Cyc_Parse_Declarator*_tmp5A0=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A0->tms=({struct Cyc_List_List*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5A3;_tmp5A3.tag=5;_tmp5A3.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5A3.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5A3;});_tmp5A2;}));_tmp5A1->tl=_tmp5A0->tms;_tmp5A1;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 154:
_LL289: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5A4=_cycalloc(sizeof(*
_tmp5A4));_tmp5A4[0]=({struct Cyc_Declarator_tok_struct _tmp5A5;_tmp5A5.tag=Cyc_Declarator_tok;
_tmp5A5.f1=({struct Cyc_Parse_Declarator*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
_tmp5A6->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5A6->tms=({struct Cyc_List_List*_tmp5A7=_cycalloc(
sizeof(*_tmp5A7));_tmp5A7->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5A9;_tmp5A9.tag=0;_tmp5A9.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A9.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A9;});
_tmp5A8;}));_tmp5A7->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A7;});_tmp5A6;});
_tmp5A5;});_tmp5A4;});break;case 155: _LL28A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA[0]=({struct Cyc_Declarator_tok_struct
_tmp5AB;_tmp5AB.tag=Cyc_Declarator_tok;_tmp5AB.f1=({struct Cyc_Parse_Declarator*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5AC->tms=({
struct Cyc_List_List*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));
_tmp5AE[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5AF;_tmp5AF.tag=1;_tmp5AF.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5AF.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5AF.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5AF;});_tmp5AE;}));_tmp5AD->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5AD;});
_tmp5AC;});_tmp5AB;});_tmp5AA;});break;case 156: _LL28B: {struct _tuple14 _tmp5B1;
struct Cyc_List_List*_tmp5B2;int _tmp5B3;struct Cyc_Absyn_VarargInfo*_tmp5B4;struct
Cyc_Core_Opt*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct _tuple14*_tmp5B0=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B1=*_tmp5B0;
_tmp5B2=_tmp5B1.f1;_tmp5B3=_tmp5B1.f2;_tmp5B4=_tmp5B1.f3;_tmp5B5=_tmp5B1.f4;
_tmp5B6=_tmp5B1.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5B7=
_cycalloc(sizeof(*_tmp5B7));_tmp5B7[0]=({struct Cyc_Declarator_tok_struct _tmp5B8;
_tmp5B8.tag=Cyc_Declarator_tok;_tmp5B8.f1=({struct Cyc_Parse_Declarator*_tmp5B9=
_cycalloc(sizeof(*_tmp5B9));_tmp5B9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5B9->tms=({
struct Cyc_List_List*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));
_tmp5BB[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5BC;_tmp5BC.tag=3;_tmp5BC.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5BD=_cycalloc(sizeof(*
_tmp5BD));_tmp5BD[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5BE;_tmp5BE.tag=1;
_tmp5BE.f1=_tmp5B2;_tmp5BE.f2=_tmp5B3;_tmp5BE.f3=_tmp5B4;_tmp5BE.f4=_tmp5B5;
_tmp5BE.f5=_tmp5B6;_tmp5BE;});_tmp5BD;}));_tmp5BC;});_tmp5BB;}));_tmp5BA->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp5BA;});_tmp5B9;});_tmp5B8;});_tmp5B7;});break;}case 157: _LL28C:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));
_tmp5BF[0]=({struct Cyc_Declarator_tok_struct _tmp5C0;_tmp5C0.tag=Cyc_Declarator_tok;
_tmp5C0.f1=({struct Cyc_Parse_Declarator*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
_tmp5C1->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5C1->tms=({struct Cyc_List_List*_tmp5C2=_cycalloc(
sizeof(*_tmp5C2));_tmp5C2->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5C4;_tmp5C4.tag=3;_tmp5C4.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5C6;_tmp5C6.tag=1;_tmp5C6.f1=0;_tmp5C6.f2=0;_tmp5C6.f3=0;_tmp5C6.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C6.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C6;});_tmp5C5;}));
_tmp5C4;});_tmp5C3;}));_tmp5C2->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5C2;});_tmp5C1;});
_tmp5C0;});_tmp5BF;});break;case 158: _LL28D: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7[0]=({struct Cyc_Declarator_tok_struct
_tmp5C8;_tmp5C8.tag=Cyc_Declarator_tok;_tmp5C8.f1=({struct Cyc_Parse_Declarator*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5C9->tms=({
struct Cyc_List_List*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));
_tmp5CB[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5CC;_tmp5CC.tag=3;_tmp5CC.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));
_tmp5CD[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5CE;_tmp5CE.tag=0;_tmp5CE.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CE.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5CE;});
_tmp5CD;}));_tmp5CC;});_tmp5CB;}));_tmp5CA->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5CA;});_tmp5C9;});
_tmp5C8;});_tmp5C7;});break;case 159: _LL28E: {struct Cyc_List_List*_tmp5CF=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0[0]=({
struct Cyc_Declarator_tok_struct _tmp5D1;_tmp5D1.tag=Cyc_Declarator_tok;_tmp5D1.f1=({
struct Cyc_Parse_Declarator*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5D2->tms=({
struct Cyc_List_List*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));
_tmp5D4[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp5D5;_tmp5D5.tag=4;_tmp5D5.f1=
_tmp5CF;_tmp5D5.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5D5.f3=0;
_tmp5D5;});_tmp5D4;}));_tmp5D3->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5D3;});_tmp5D2;});
_tmp5D1;});_tmp5D0;});break;}case 160: _LL28F: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_Declarator_tok_struct
_tmp5D7;_tmp5D7.tag=Cyc_Declarator_tok;_tmp5D7.f1=({struct Cyc_Parse_Declarator*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5D8->tms=({
struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));
_tmp5DA[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5DB;_tmp5DB.tag=5;_tmp5DB.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5DB.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DB;});_tmp5DA;}));
_tmp5D9->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5D9;});_tmp5D8;});_tmp5D7;});_tmp5D6;});break;case
161: _LL290: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5DC=_cycalloc(
sizeof(*_tmp5DC));_tmp5DC[0]=({struct Cyc_Declarator_tok_struct _tmp5DD;_tmp5DD.tag=
Cyc_Declarator_tok;_tmp5DD.f1=({struct Cyc_Parse_Declarator*_tmp5DE=_cycalloc(
sizeof(*_tmp5DE));_tmp5DE->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DE->tms=0;_tmp5DE;});
_tmp5DD;});_tmp5DC;});break;case 162: _LL291: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Declarator_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_Declarator_tok;_tmp5E0.f1=({struct Cyc_Parse_Declarator*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E1->tms=0;_tmp5E1;});
_tmp5E0;});_tmp5DF;});break;case 163: _LL292: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 164: _LL293: {
struct Cyc_Parse_Declarator*_tmp5E2=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E2->tms=({struct Cyc_List_List*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5E5;_tmp5E5.tag=5;_tmp5E5.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5E5.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5E5;});_tmp5E4;}));_tmp5E3->tl=_tmp5E2->tms;_tmp5E3;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165:
_LL294: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5E6=_cycalloc(sizeof(*
_tmp5E6));_tmp5E6[0]=({struct Cyc_Declarator_tok_struct _tmp5E7;_tmp5E7.tag=Cyc_Declarator_tok;
_tmp5E7.f1=({struct Cyc_Parse_Declarator*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));
_tmp5E8->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5E8->tms=({struct Cyc_List_List*_tmp5E9=_cycalloc(
sizeof(*_tmp5E9));_tmp5E9->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5EB;_tmp5EB.tag=0;_tmp5EB.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EB.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5EB;});
_tmp5EA;}));_tmp5E9->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5E9;});_tmp5E8;});
_tmp5E7;});_tmp5E6;});break;case 166: _LL295: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC[0]=({struct Cyc_Declarator_tok_struct
_tmp5ED;_tmp5ED.tag=Cyc_Declarator_tok;_tmp5ED.f1=({struct Cyc_Parse_Declarator*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5EE->tms=({
struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));
_tmp5F0[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5F1;_tmp5F1.tag=1;_tmp5F1.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5F1.f2=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5F1.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5F1;});_tmp5F0;}));_tmp5EF->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5EF;});
_tmp5EE;});_tmp5ED;});_tmp5EC;});break;case 167: _LL296: {struct _tuple14 _tmp5F3;
struct Cyc_List_List*_tmp5F4;int _tmp5F5;struct Cyc_Absyn_VarargInfo*_tmp5F6;struct
Cyc_Core_Opt*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct _tuple14*_tmp5F2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F3=*_tmp5F2;
_tmp5F4=_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;_tmp5F6=_tmp5F3.f3;_tmp5F7=_tmp5F3.f4;
_tmp5F8=_tmp5F3.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5F9=
_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Declarator_tok_struct _tmp5FA;
_tmp5FA.tag=Cyc_Declarator_tok;_tmp5FA.f1=({struct Cyc_Parse_Declarator*_tmp5FB=
_cycalloc(sizeof(*_tmp5FB));_tmp5FB->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5FB->tms=({
struct Cyc_List_List*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));
_tmp5FD[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5FE;_tmp5FE.tag=3;_tmp5FE.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5FF=_cycalloc(sizeof(*
_tmp5FF));_tmp5FF[0]=({struct Cyc_Absyn_WithTypes_struct _tmp600;_tmp600.tag=1;
_tmp600.f1=_tmp5F4;_tmp600.f2=_tmp5F5;_tmp600.f3=_tmp5F6;_tmp600.f4=_tmp5F7;
_tmp600.f5=_tmp5F8;_tmp600;});_tmp5FF;}));_tmp5FE;});_tmp5FD;}));_tmp5FC->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp5FC;});_tmp5FB;});_tmp5FA;});_tmp5F9;});break;}case 168: _LL297:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp601=_cycalloc(sizeof(*_tmp601));
_tmp601[0]=({struct Cyc_Declarator_tok_struct _tmp602;_tmp602.tag=Cyc_Declarator_tok;
_tmp602.f1=({struct Cyc_Parse_Declarator*_tmp603=_cycalloc(sizeof(*_tmp603));
_tmp603->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp603->tms=({struct Cyc_List_List*_tmp604=_cycalloc(
sizeof(*_tmp604));_tmp604->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp606;_tmp606.tag=3;_tmp606.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp608;_tmp608.tag=1;_tmp608.f1=0;_tmp608.f2=0;_tmp608.f3=0;_tmp608.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp608.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp608;});_tmp607;}));
_tmp606;});_tmp605;}));_tmp604->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp604;});_tmp603;});
_tmp602;});_tmp601;});break;case 169: _LL298: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Declarator_tok_struct
_tmp60A;_tmp60A.tag=Cyc_Declarator_tok;_tmp60A.f1=({struct Cyc_Parse_Declarator*
_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp60B->tms=({
struct Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));
_tmp60D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp60E;_tmp60E.tag=3;_tmp60E.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));
_tmp60F[0]=({struct Cyc_Absyn_NoTypes_struct _tmp610;_tmp610.tag=0;_tmp610.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp610.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp610;});
_tmp60F;}));_tmp60E;});_tmp60D;}));_tmp60C->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp60C;});_tmp60B;});
_tmp60A;});_tmp609;});break;case 170: _LL299: {struct Cyc_List_List*_tmp611=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612[0]=({
struct Cyc_Declarator_tok_struct _tmp613;_tmp613.tag=Cyc_Declarator_tok;_tmp613.f1=({
struct Cyc_Parse_Declarator*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp614->tms=({
struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp616=_cycalloc(sizeof(*_tmp616));
_tmp616[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp617;_tmp617.tag=4;_tmp617.f1=
_tmp611;_tmp617.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp617.f3=0;
_tmp617;});_tmp616;}));_tmp615->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp615;});_tmp614;});
_tmp613;});_tmp612;});break;}case 171: _LL29A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Declarator_tok_struct
_tmp619;_tmp619.tag=Cyc_Declarator_tok;_tmp619.f1=({struct Cyc_Parse_Declarator*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp61A->tms=({
struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));
_tmp61C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp61D;_tmp61D.tag=5;_tmp61D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp61D.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61D;});_tmp61C;}));
_tmp61B->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp61B;});_tmp61A;});_tmp619;});_tmp618;});break;case
172: _LL29B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 173: _LL29C: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp61E=
_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp61F;_tmp61F.tag=Cyc_TypeModifierList_tok;_tmp61F.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp61F;});_tmp61E;});
break;case 174: _LL29D: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp622;_tmp622.tag=5;_tmp622.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp622.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp622;});_tmp621;}));_tmp620->tl=ans;_tmp620;});{struct Cyc_Absyn_PtrLoc*
_tmp623=({struct Cyc_Absyn_PtrLoc*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->ptr_loc=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;
_tmp62A->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp62A->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp62A;});ans=({struct Cyc_List_List*_tmp624=
_cycalloc(sizeof(*_tmp624));_tmp624->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp626;_tmp626.tag=2;_tmp626.f1=({struct Cyc_Absyn_PtrAtts _tmp627;_tmp627.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp627.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp627.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp627.zero_term=
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp627.ptrloc=_tmp623;_tmp627;});_tmp626.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp626;});_tmp625;}));
_tmp624->tl=ans;_tmp624;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp629;_tmp629.tag=Cyc_TypeModifierList_tok;_tmp629.f1=ans;_tmp629;});_tmp628;});
break;}}case 175: _LL29E: yyval=(void*)({struct Cyc_YY1_struct*_tmp62B=_cycalloc(
sizeof(*_tmp62B));_tmp62B[0]=({struct Cyc_YY1_struct _tmp62C;_tmp62C.tag=Cyc_YY1;
_tmp62C.f1=({struct _tuple16*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp62D->f2=
Cyc_Absyn_true_conref;_tmp62D->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62D;});_tmp62C;});
_tmp62B;});break;case 176: _LL29F: yyval=(void*)({struct Cyc_YY1_struct*_tmp62E=
_cycalloc(sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_YY1_struct _tmp62F;_tmp62F.tag=
Cyc_YY1;_tmp62F.f1=({struct _tuple16*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp630->f2=Cyc_Absyn_false_conref;_tmp630->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp630;});_tmp62F;});
_tmp62E;});break;case 177: _LL2A0: yyval=(void*)({struct Cyc_YY1_struct*_tmp631=
_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_YY1_struct _tmp632;_tmp632.tag=
Cyc_YY1;_tmp632.f1=({struct _tuple16*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp633->f2=Cyc_Absyn_true_conref;_tmp633->f3=Cyc_Absyn_bounds_unknown_conref;
_tmp633;});_tmp632;});_tmp631;});break;case 178: _LL2A1: yyval=(void*)({struct Cyc_YY2_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_YY2_struct _tmp635;
_tmp635.tag=Cyc_YY2;_tmp635.f1=Cyc_Absyn_bounds_one_conref;_tmp635;});_tmp634;});
break;case 179: _LL2A2: yyval=(void*)({struct Cyc_YY2_struct*_tmp636=_cycalloc(
sizeof(*_tmp636));_tmp636[0]=({struct Cyc_YY2_struct _tmp637;_tmp637.tag=Cyc_YY2;
_tmp637.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp638=
_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_Upper_b_struct _tmp639;
_tmp639.tag=0;_tmp639.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp639;});_tmp638;}));_tmp637;});_tmp636;});break;case
180: _LL2A3: yyval=(void*)({struct Cyc_YY2_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));
_tmp63A[0]=({struct Cyc_YY2_struct _tmp63B;_tmp63B.tag=Cyc_YY2;_tmp63B.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));
_tmp63C[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp63D;_tmp63D.tag=1;_tmp63D.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp63D;});_tmp63C;}));_tmp63B;});_tmp63A;});break;case 181: _LL2A4: yyval=(
void*)({struct Cyc_YY4_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E[0]=({
struct Cyc_YY4_struct _tmp63F;_tmp63F.tag=Cyc_YY4;_tmp63F.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp63F;});_tmp63E;});break;case 182: _LL2A5: yyval=(void*)({
struct Cyc_YY4_struct*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640[0]=({struct Cyc_YY4_struct
_tmp641;_tmp641.tag=Cyc_YY4;_tmp641.f1=Cyc_Absyn_true_conref;_tmp641;});_tmp640;});
break;case 183: _LL2A6: yyval=(void*)({struct Cyc_YY4_struct*_tmp642=_cycalloc(
sizeof(*_tmp642));_tmp642[0]=({struct Cyc_YY4_struct _tmp643;_tmp643.tag=Cyc_YY4;
_tmp643.f1=Cyc_Absyn_false_conref;_tmp643;});_tmp642;});break;case 184: _LL2A7:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp644=_cycalloc(sizeof(*_tmp644));
_tmp644[0]=({struct Cyc_Type_tok_struct _tmp645;_tmp645.tag=Cyc_Type_tok;_tmp645.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp646=_cycalloc(sizeof(*_tmp646));
_tmp646->v=(void*)((void*)3);_tmp646;}),0);_tmp645;});_tmp644;});break;case 185:
_LL2A8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 186: _LL2A9: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({
struct Cyc_Type_tok_struct _tmp648;_tmp648.tag=Cyc_Type_tok;_tmp648.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->v=(void*)((void*)
3);_tmp649;}),0);_tmp648;});_tmp647;});break;case 187: _LL2AA: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A[0]=({
struct Cyc_TypeQual_tok_struct _tmp64B;_tmp64B.tag=Cyc_TypeQual_tok;_tmp64B.f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp64B;});_tmp64A;});break;case 188: _LL2AB: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));
_tmp64C[0]=({struct Cyc_TypeQual_tok_struct _tmp64D;_tmp64D.tag=Cyc_TypeQual_tok;
_tmp64D.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp64D;});_tmp64C;});
break;case 189: _LL2AC: yyval=(void*)({struct Cyc_YY3_struct*_tmp64E=_cycalloc(
sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_YY3_struct _tmp64F;_tmp64F.tag=Cyc_YY3;
_tmp64F.f1=({struct _tuple14*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp650->f2=0;
_tmp650->f3=0;_tmp650->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp650->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp650;});_tmp64F;});
_tmp64E;});break;case 190: _LL2AD: yyval=(void*)({struct Cyc_YY3_struct*_tmp651=
_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_YY3_struct _tmp652;_tmp652.tag=
Cyc_YY3;_tmp652.f1=({struct _tuple14*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp653->f2=1;
_tmp653->f3=0;_tmp653->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp653->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp653;});_tmp652;});
_tmp651;});break;case 191: _LL2AE: {struct _tuple2 _tmp655;struct Cyc_Core_Opt*
_tmp656;struct Cyc_Absyn_Tqual _tmp657;void*_tmp658;struct _tuple2*_tmp654=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp655=*_tmp654;
_tmp656=_tmp655.f1;_tmp657=_tmp655.f2;_tmp658=_tmp655.f3;{struct Cyc_Absyn_VarargInfo*
_tmp659=({struct Cyc_Absyn_VarargInfo*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->name=
_tmp656;_tmp65D->tq=_tmp657;_tmp65D->type=(void*)_tmp658;_tmp65D->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp65D;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({
struct Cyc_YY3_struct _tmp65B;_tmp65B.tag=Cyc_YY3;_tmp65B.f1=({struct _tuple14*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->f1=0;_tmp65C->f2=0;_tmp65C->f3=
_tmp659;_tmp65C->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp65C->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65C;});_tmp65B;});
_tmp65A;});break;}}case 192: _LL2AF: {struct _tuple2 _tmp65F;struct Cyc_Core_Opt*
_tmp660;struct Cyc_Absyn_Tqual _tmp661;void*_tmp662;struct _tuple2*_tmp65E=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65F=*_tmp65E;
_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;{struct Cyc_Absyn_VarargInfo*
_tmp663=({struct Cyc_Absyn_VarargInfo*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->name=
_tmp660;_tmp667->tq=_tmp661;_tmp667->type=(void*)_tmp662;_tmp667->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp667;});yyval=(
void*)({struct Cyc_YY3_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664[0]=({
struct Cyc_YY3_struct _tmp665;_tmp665.tag=Cyc_YY3;_tmp665.f1=({struct _tuple14*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp666->f2=0;_tmp666->f3=
_tmp663;_tmp666->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp666->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666;});_tmp665;});
_tmp664;});break;}}case 193: _LL2B0: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({struct Cyc_Type_tok_struct
_tmp669;_tmp669.tag=Cyc_Type_tok;_tmp669.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp66B;_tmp66B.tag=1;_tmp66B.f1=0;_tmp66B;});_tmp66A;}));_tmp669;});_tmp668;});
break;case 194: _LL2B1: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp66C=_cycalloc(
sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Type_tok_struct _tmp66D;_tmp66D.tag=Cyc_Type_tok;
_tmp66D.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp66F;_tmp66F.tag=0;_tmp66F.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66F;});_tmp66E;}));
_tmp66D;});_tmp66C;});break;case 195: _LL2B2: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_TypeOpt_tok_struct
_tmp671;_tmp671.tag=Cyc_TypeOpt_tok;_tmp671.f1=0;_tmp671;});_tmp670;});break;
case 196: _LL2B3: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp672=_cycalloc(
sizeof(*_tmp672));_tmp672[0]=({struct Cyc_TypeOpt_tok_struct _tmp673;_tmp673.tag=
Cyc_TypeOpt_tok;_tmp673.f1=({struct Cyc_Core_Opt*_tmp674=_cycalloc(sizeof(*
_tmp674));_tmp674->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp675=
_cycalloc(sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_JoinEff_struct _tmp676;
_tmp676.tag=20;_tmp676.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676;});_tmp675;}));
_tmp674;});_tmp673;});_tmp672;});break;case 197: _LL2B4: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677[0]=({struct Cyc_Rgnorder_tok_struct
_tmp678;_tmp678.tag=Cyc_Rgnorder_tok;_tmp678.f1=0;_tmp678;});_tmp677;});break;
case 198: _LL2B5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 199: _LL2B6: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp679=
_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Rgnorder_tok_struct _tmp67A;
_tmp67A.tag=Cyc_Rgnorder_tok;_tmp67A.f1=({struct Cyc_List_List*_tmp67B=_cycalloc(
sizeof(*_tmp67B));_tmp67B->hd=({struct _tuple4*_tmp67C=_cycalloc(sizeof(*_tmp67C));
_tmp67C->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp67F=_cycalloc(sizeof(*
_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp680;_tmp680.tag=20;
_tmp680.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp680;});_tmp67F;});_tmp67C->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp67E;_tmp67E.tag=0;_tmp67E.f1=(void*)((void*)3);_tmp67E;});_tmp67D;}));
_tmp67C;});_tmp67B->tl=0;_tmp67B;});_tmp67A;});_tmp679;});break;case 200: _LL2B7:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp681=_cycalloc(sizeof(*_tmp681));
_tmp681[0]=({struct Cyc_Rgnorder_tok_struct _tmp682;_tmp682.tag=Cyc_Rgnorder_tok;
_tmp682.f1=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->hd=({
struct _tuple4*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp688;_tmp688.tag=20;_tmp688.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp688;});_tmp687;});
_tmp684->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp686;_tmp686.tag=0;_tmp686.f1=(void*)((void*)3);_tmp686;});_tmp685;}));
_tmp684;});_tmp683->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp683;});_tmp682;});
_tmp681;});break;case 201: _LL2B8: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp689=
_cycalloc_atomic(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Bool_tok_struct _tmp68A;
_tmp68A.tag=Cyc_Bool_tok;_tmp68A.f1=0;_tmp68A;});_tmp689;});break;case 202: _LL2B9:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp68B="inject";
_tag_arr(_tmp68B,sizeof(char),_get_zero_arr_size(_tmp68B,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp68C="missing type in function declaration";_tag_arr(_tmp68C,
sizeof(char),_get_zero_arr_size(_tmp68C,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp68D=_cycalloc_atomic(sizeof(*_tmp68D));_tmp68D[0]=({
struct Cyc_Bool_tok_struct _tmp68E;_tmp68E.tag=Cyc_Bool_tok;_tmp68E.f1=1;_tmp68E;});
_tmp68D;});break;case 203: _LL2BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 204: _LL2BB: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_TypeList_tok_struct
_tmp690;_tmp690.tag=Cyc_TypeList_tok;_tmp690.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp690;});_tmp68F;});
break;case 205: _LL2BC: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp691=
_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_TypeList_tok_struct _tmp692;
_tmp692.tag=Cyc_TypeList_tok;_tmp692.f1=0;_tmp692;});_tmp691;});break;case 206:
_LL2BD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 207: _LL2BE: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp693=_cycalloc(
sizeof(*_tmp693));_tmp693[0]=({struct Cyc_TypeList_tok_struct _tmp694;_tmp694.tag=
Cyc_TypeList_tok;_tmp694.f1=({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*
_tmp695));_tmp695->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp696=
_cycalloc(sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp697;
_tmp697.tag=21;_tmp697.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp697;});_tmp696;}));
_tmp695->tl=0;_tmp695;});_tmp694;});_tmp693;});break;case 208: _LL2BF: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp698=_cycalloc(sizeof(*
_tmp698));_tmp698[0]=({struct Cyc_TypeList_tok_struct _tmp699;_tmp699.tag=Cyc_TypeList_tok;
_tmp699.f1=({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp69A->tl=0;_tmp69A;});_tmp699;});_tmp698;});break;case 209: _LL2C0: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69B=_cycalloc(sizeof(*
_tmp69B));_tmp69B[0]=({struct Cyc_TypeList_tok_struct _tmp69C;_tmp69C.tag=Cyc_TypeList_tok;
_tmp69C.f1=({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp69E=_cycalloc(sizeof(*
_tmp69E));_tmp69E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp69F;_tmp69F.tag=19;
_tmp69F.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69F;});_tmp69E;}));_tmp69D->tl=0;_tmp69D;});_tmp69C;});
_tmp69B;});break;case 210: _LL2C1: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));
_tmp6A0[0]=({struct Cyc_TypeList_tok_struct _tmp6A1;_tmp6A1.tag=Cyc_TypeList_tok;
_tmp6A1.f1=({struct Cyc_List_List*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6A3=_cycalloc(sizeof(*
_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6A4;_tmp6A4.tag=19;
_tmp6A4.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp6A4;});_tmp6A3;}));_tmp6A2->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A2;});_tmp6A1;});
_tmp6A0;});break;case 211: _LL2C2: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6A6;_tmp6A6.tag=Cyc_ParamDeclList_tok;_tmp6A6.f1=({struct Cyc_List_List*
_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A7->tl=0;_tmp6A7;});
_tmp6A6;});_tmp6A5;});break;case 212: _LL2C3: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6A9;_tmp6A9.tag=Cyc_ParamDeclList_tok;_tmp6A9.f1=({struct Cyc_List_List*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AA->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AA;});_tmp6A9;});
_tmp6A8;});break;case 213: _LL2C4: {struct _tuple15 _tmp6AC;struct Cyc_Absyn_Tqual
_tmp6AD;struct Cyc_List_List*_tmp6AE;struct Cyc_List_List*_tmp6AF;struct _tuple15*
_tmp6AB=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6AC=*_tmp6AB;_tmp6AD=_tmp6AC.f1;_tmp6AE=_tmp6AC.f2;
_tmp6AF=_tmp6AC.f3;if(_tmp6AD.loc == 0)_tmp6AD.loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp6B1;struct _tuple1*_tmp6B2;struct Cyc_List_List*_tmp6B3;struct Cyc_Parse_Declarator*
_tmp6B0=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6B1=*_tmp6B0;_tmp6B2=_tmp6B1.id;_tmp6B3=_tmp6B1.tms;{void*
_tmp6B4=Cyc_Parse_speclist2typ(_tmp6AE,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp6B6;void*_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_List_List*_tmp6B9;
struct _tuple6 _tmp6B5=Cyc_Parse_apply_tms(_tmp6AD,_tmp6B4,_tmp6AF,_tmp6B3);
_tmp6B6=_tmp6B5.f1;_tmp6B7=_tmp6B5.f2;_tmp6B8=_tmp6B5.f3;_tmp6B9=_tmp6B5.f4;if(
_tmp6B8 != 0)Cyc_Parse_err(({const char*_tmp6BA="parameter with bad type params";
_tag_arr(_tmp6BA,sizeof(char),_get_zero_arr_size(_tmp6BA,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6B2))Cyc_Parse_err(({const char*_tmp6BB="parameter cannot be qualified with a namespace";
_tag_arr(_tmp6BB,sizeof(char),_get_zero_arr_size(_tmp6BB,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6BC=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6C2=_cycalloc(sizeof(*
_tmp6C2));_tmp6C2->v=(*_tmp6B2).f2;_tmp6C2;});if(_tmp6B9 != 0)({void*_tmp6BD[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6BE="extra attributes on parameter, ignoring";
_tag_arr(_tmp6BE,sizeof(char),_get_zero_arr_size(_tmp6BE,40));}),_tag_arr(
_tmp6BD,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6C0;_tmp6C0.tag=Cyc_ParamDecl_tok;_tmp6C0.f1=({struct _tuple2*_tmp6C1=
_cycalloc(sizeof(*_tmp6C1));_tmp6C1->f1=_tmp6BC;_tmp6C1->f2=_tmp6B6;_tmp6C1->f3=
_tmp6B7;_tmp6C1;});_tmp6C0;});_tmp6BF;});break;}}}}case 214: _LL2C5: {struct
_tuple15 _tmp6C4;struct Cyc_Absyn_Tqual _tmp6C5;struct Cyc_List_List*_tmp6C6;struct
Cyc_List_List*_tmp6C7;struct _tuple15*_tmp6C3=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C4=*_tmp6C3;_tmp6C5=
_tmp6C4.f1;_tmp6C6=_tmp6C4.f2;_tmp6C7=_tmp6C4.f3;if(_tmp6C5.loc == 0)_tmp6C5.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6C8=Cyc_Parse_speclist2typ(_tmp6C6,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6C7 != 0)({
void*_tmp6C9[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6CA="bad attributes on parameter, ignoring";_tag_arr(_tmp6CA,sizeof(char),
_get_zero_arr_size(_tmp6CA,38));}),_tag_arr(_tmp6C9,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));
_tmp6CB[0]=({struct Cyc_ParamDecl_tok_struct _tmp6CC;_tmp6CC.tag=Cyc_ParamDecl_tok;
_tmp6CC.f1=({struct _tuple2*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->f1=0;
_tmp6CD->f2=_tmp6C5;_tmp6CD->f3=_tmp6C8;_tmp6CD;});_tmp6CC;});_tmp6CB;});break;}}
case 215: _LL2C6: {struct _tuple15 _tmp6CF;struct Cyc_Absyn_Tqual _tmp6D0;struct Cyc_List_List*
_tmp6D1;struct Cyc_List_List*_tmp6D2;struct _tuple15*_tmp6CE=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CF=*_tmp6CE;
_tmp6D0=_tmp6CF.f1;_tmp6D1=_tmp6CF.f2;_tmp6D2=_tmp6CF.f3;if(_tmp6D0.loc == 0)
_tmp6D0.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6D3=Cyc_Parse_speclist2typ(_tmp6D1,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6D4=(Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual _tmp6D6;void*_tmp6D7;struct Cyc_List_List*
_tmp6D8;struct Cyc_List_List*_tmp6D9;struct _tuple6 _tmp6D5=Cyc_Parse_apply_tms(
_tmp6D0,_tmp6D3,_tmp6D2,_tmp6D4);_tmp6D6=_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;_tmp6D8=
_tmp6D5.f3;_tmp6D9=_tmp6D5.f4;if(_tmp6D8 != 0)({void*_tmp6DA[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6DB="bad type parameters on formal argument, ignoring";
_tag_arr(_tmp6DB,sizeof(char),_get_zero_arr_size(_tmp6DB,49));}),_tag_arr(
_tmp6DA,sizeof(void*),0));});if(_tmp6D9 != 0)({void*_tmp6DC[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6DD="bad attributes on parameter, ignoring";
_tag_arr(_tmp6DD,sizeof(char),_get_zero_arr_size(_tmp6DD,38));}),_tag_arr(
_tmp6DC,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6DF;_tmp6DF.tag=Cyc_ParamDecl_tok;_tmp6DF.f1=({struct _tuple2*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0->f1=0;_tmp6E0->f2=_tmp6D6;_tmp6E0->f3=
_tmp6D7;_tmp6E0;});_tmp6DF;});_tmp6DE;});break;}}case 216: _LL2C7: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({
struct Cyc_IdList_tok_struct _tmp6E2;_tmp6E2.tag=Cyc_IdList_tok;_tmp6E2.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp6E2;});_tmp6E1;});
break;case 217: _LL2C8: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6E3=
_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_IdList_tok_struct _tmp6E4;
_tmp6E4.tag=Cyc_IdList_tok;_tmp6E4.f1=({struct Cyc_List_List*_tmp6E5=_cycalloc(
sizeof(*_tmp6E5));_tmp6E5->hd=({struct _tagged_arr*_tmp6E6=_cycalloc(sizeof(*
_tmp6E6));_tmp6E6[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6E6;});_tmp6E5->tl=0;_tmp6E5;});_tmp6E4;});_tmp6E3;});
break;case 218: _LL2C9: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6E7=
_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_IdList_tok_struct _tmp6E8;
_tmp6E8.tag=Cyc_IdList_tok;_tmp6E8.f1=({struct Cyc_List_List*_tmp6E9=_cycalloc(
sizeof(*_tmp6E9));_tmp6E9->hd=({struct _tagged_arr*_tmp6EA=_cycalloc(sizeof(*
_tmp6EA));_tmp6EA[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6EA;});_tmp6E9->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E9;});_tmp6E8;});
_tmp6E7;});break;case 219: _LL2CA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 220: _LL2CB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 221: _LL2CC: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Exp_tok_struct _tmp6EC;
_tmp6EC.tag=Cyc_Exp_tok;_tmp6EC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6EE;_tmp6EE.tag=36;_tmp6EE.f1=0;_tmp6EE.f2=0;_tmp6EE;});_tmp6ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6EC;});
_tmp6EB;});break;case 222: _LL2CD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Exp_tok_struct _tmp6F0;_tmp6F0.tag=
Cyc_Exp_tok;_tmp6F0.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6F2;_tmp6F2.tag=36;_tmp6F2.f1=0;_tmp6F2.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6F2;});_tmp6F1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp6F0;});_tmp6EF;});break;case 223: _LL2CE: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({
struct Cyc_Exp_tok_struct _tmp6F4;_tmp6F4.tag=Cyc_Exp_tok;_tmp6F4.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));
_tmp6F5[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6F6;_tmp6F6.tag=36;
_tmp6F6.f1=0;_tmp6F6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp6F6;});_tmp6F5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F4;});
_tmp6F3;});break;case 224: _LL2CF: {struct Cyc_Absyn_Vardecl*_tmp6F7=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->f1=(void*)0;_tmp6FC->f2=({
struct _tagged_arr*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6FD;});_tmp6FC;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6F7->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));
_tmp6F8[0]=({struct Cyc_Exp_tok_struct _tmp6F9;_tmp6F9.tag=Cyc_Exp_tok;_tmp6F9.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp6FB;_tmp6FB.tag=29;_tmp6FB.f1=_tmp6F7;_tmp6FB.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6FB.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FB.f4=0;
_tmp6FB;});_tmp6FA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F9;});
_tmp6F8;});break;}case 225: _LL2D0: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_InitializerList_tok_struct
_tmp6FF;_tmp6FF.tag=Cyc_InitializerList_tok;_tmp6FF.f1=({struct Cyc_List_List*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->hd=({struct _tuple19*_tmp701=
_cycalloc(sizeof(*_tmp701));_tmp701->f1=0;_tmp701->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp701;});_tmp700->tl=0;
_tmp700;});_tmp6FF;});_tmp6FE;});break;case 226: _LL2D1: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_InitializerList_tok_struct
_tmp703;_tmp703.tag=Cyc_InitializerList_tok;_tmp703.f1=({struct Cyc_List_List*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704->hd=({struct _tuple19*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp705->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp705;});_tmp704->tl=
0;_tmp704;});_tmp703;});_tmp702;});break;case 227: _LL2D2: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({struct Cyc_InitializerList_tok_struct
_tmp707;_tmp707.tag=Cyc_InitializerList_tok;_tmp707.f1=({struct Cyc_List_List*
_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708->hd=({struct _tuple19*_tmp709=
_cycalloc(sizeof(*_tmp709));_tmp709->f1=0;_tmp709->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp709;});_tmp708->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp708;});_tmp707;});
_tmp706;});break;case 228: _LL2D3: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_InitializerList_tok_struct
_tmp70B;_tmp70B.tag=Cyc_InitializerList_tok;_tmp70B.f1=({struct Cyc_List_List*
_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=({struct _tuple19*_tmp70D=
_cycalloc(sizeof(*_tmp70D));_tmp70D->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70D->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70D;});_tmp70C->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp70C;});_tmp70B;});_tmp70A;});break;case 229: _LL2D4: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));
_tmp70E[0]=({struct Cyc_DesignatorList_tok_struct _tmp70F;_tmp70F.tag=Cyc_DesignatorList_tok;
_tmp70F.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp70F;});_tmp70E;});
break;case 230: _LL2D5: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp710=
_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_DesignatorList_tok_struct
_tmp711;_tmp711.tag=Cyc_DesignatorList_tok;_tmp711.f1=({struct Cyc_List_List*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp712->tl=0;_tmp712;});
_tmp711;});_tmp710;});break;case 231: _LL2D6: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_DesignatorList_tok_struct
_tmp714;_tmp714.tag=Cyc_DesignatorList_tok;_tmp714.f1=({struct Cyc_List_List*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp715->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp715;});_tmp714;});
_tmp713;});break;case 232: _LL2D7: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Designator_tok_struct
_tmp717;_tmp717.tag=Cyc_Designator_tok;_tmp717.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp719;_tmp719.tag=0;_tmp719.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp719;});_tmp718;}));
_tmp717;});_tmp716;});break;case 233: _LL2D8: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Designator_tok_struct
_tmp71B;_tmp71B.tag=Cyc_Designator_tok;_tmp71B.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_FieldName_struct
_tmp71D;_tmp71D.tag=1;_tmp71D.f1=({struct _tagged_arr*_tmp71E=_cycalloc(sizeof(*
_tmp71E));_tmp71E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp71E;});_tmp71D;});_tmp71C;}));_tmp71B;});_tmp71A;});
break;case 234: _LL2D9: {struct _tuple15 _tmp720;struct Cyc_Absyn_Tqual _tmp721;struct
Cyc_List_List*_tmp722;struct Cyc_List_List*_tmp723;struct _tuple15*_tmp71F=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp720=*_tmp71F;
_tmp721=_tmp720.f1;_tmp722=_tmp720.f2;_tmp723=_tmp720.f3;{void*_tmp724=Cyc_Parse_speclist2typ(
_tmp722,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp723 != 0)({void*_tmp725[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp726="ignoring attributes in type";
_tag_arr(_tmp726,sizeof(char),_get_zero_arr_size(_tmp726,28));}),_tag_arr(
_tmp725,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_ParamDecl_tok_struct
_tmp728;_tmp728.tag=Cyc_ParamDecl_tok;_tmp728.f1=({struct _tuple2*_tmp729=
_cycalloc(sizeof(*_tmp729));_tmp729->f1=0;_tmp729->f2=_tmp721;_tmp729->f3=
_tmp724;_tmp729;});_tmp728;});_tmp727;});break;}}case 235: _LL2DA: {struct _tuple15
_tmp72B;struct Cyc_Absyn_Tqual _tmp72C;struct Cyc_List_List*_tmp72D;struct Cyc_List_List*
_tmp72E;struct _tuple15*_tmp72A=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72B=*_tmp72A;_tmp72C=
_tmp72B.f1;_tmp72D=_tmp72B.f2;_tmp72E=_tmp72B.f3;{void*_tmp72F=Cyc_Parse_speclist2typ(
_tmp72D,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp730=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp731=Cyc_Parse_apply_tms(_tmp72C,_tmp72F,_tmp72E,_tmp730);if(_tmp731.f3 != 0)({
void*_tmp732[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp733="bad type params, ignoring";_tag_arr(_tmp733,sizeof(char),
_get_zero_arr_size(_tmp733,26));}),_tag_arr(_tmp732,sizeof(void*),0));});if(
_tmp731.f4 != 0)({void*_tmp734[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp735="bad specifiers, ignoring";_tag_arr(_tmp735,sizeof(char),
_get_zero_arr_size(_tmp735,25));}),_tag_arr(_tmp734,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp736=_cycalloc(sizeof(*_tmp736));
_tmp736[0]=({struct Cyc_ParamDecl_tok_struct _tmp737;_tmp737.tag=Cyc_ParamDecl_tok;
_tmp737.f1=({struct _tuple2*_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738->f1=0;
_tmp738->f2=_tmp731.f1;_tmp738->f3=_tmp731.f2;_tmp738;});_tmp737;});_tmp736;});
break;}}case 236: _LL2DB: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp739=
_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Type_tok_struct _tmp73A;
_tmp73A.tag=Cyc_Type_tok;_tmp73A.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp73A;});_tmp739;});
break;case 237: _LL2DC: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp73B=_cycalloc(
sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Type_tok_struct _tmp73C;_tmp73C.tag=Cyc_Type_tok;
_tmp73C.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp73E;_tmp73E.tag=
20;_tmp73E.f1=0;_tmp73E;});_tmp73D;}));_tmp73C;});_tmp73B;});break;case 238:
_LL2DD: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp73F=_cycalloc(sizeof(*
_tmp73F));_tmp73F[0]=({struct Cyc_Type_tok_struct _tmp740;_tmp740.tag=Cyc_Type_tok;
_tmp740.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_JoinEff_struct _tmp742;_tmp742.tag=
20;_tmp742.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp742;});_tmp741;}));_tmp740;});_tmp73F;});break;case 239:
_LL2DE: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp743=_cycalloc(sizeof(*
_tmp743));_tmp743[0]=({struct Cyc_Type_tok_struct _tmp744;_tmp744.tag=Cyc_Type_tok;
_tmp744.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp745=_cycalloc(
sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp746;_tmp746.tag=
21;_tmp746.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp746;});_tmp745;}));_tmp744;});_tmp743;});break;case
240: _LL2DF: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp747=_cycalloc(sizeof(*
_tmp747));_tmp747[0]=({struct Cyc_Type_tok_struct _tmp748;_tmp748.tag=Cyc_Type_tok;
_tmp748.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp749=_cycalloc(
sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Absyn_JoinEff_struct _tmp74A;_tmp74A.tag=
20;_tmp74A.f1=({struct Cyc_List_List*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp74B->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp74B;});_tmp74A;});_tmp749;}));_tmp748;});_tmp747;});
break;case 241: _LL2E0: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp74C=
_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_TypeList_tok_struct _tmp74D;
_tmp74D.tag=Cyc_TypeList_tok;_tmp74D.f1=({struct Cyc_List_List*_tmp74E=_cycalloc(
sizeof(*_tmp74E));_tmp74E->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74E->tl=0;_tmp74E;});
_tmp74D;});_tmp74C;});break;case 242: _LL2E1: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=({struct Cyc_TypeList_tok_struct
_tmp750;_tmp750.tag=Cyc_TypeList_tok;_tmp750.f1=({struct Cyc_List_List*_tmp751=
_cycalloc(sizeof(*_tmp751));_tmp751->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp751->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp751;});_tmp750;});
_tmp74F;});break;case 243: _LL2E2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp753;_tmp753.tag=Cyc_AbstractDeclarator_tok;_tmp753.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp754;});_tmp753;});
_tmp752;});break;case 244: _LL2E3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 245: _LL2E4: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp756;_tmp756.tag=Cyc_AbstractDeclarator_tok;_tmp756.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp757;});
_tmp756;});_tmp755;});break;case 246: _LL2E5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 247: _LL2E6:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp758=_cycalloc(sizeof(*
_tmp758));_tmp758[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp759;_tmp759.tag=
Cyc_AbstractDeclarator_tok;_tmp759.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->tms=({struct Cyc_List_List*_tmp75B=
_cycalloc(sizeof(*_tmp75B));_tmp75B->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp75D;_tmp75D.tag=0;_tmp75D.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75D.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75D;});
_tmp75C;}));_tmp75B->tl=0;_tmp75B;});_tmp75A;});_tmp759;});_tmp758;});break;case
248: _LL2E7: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp75F;_tmp75F.tag=Cyc_AbstractDeclarator_tok;_tmp75F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->tms=({struct Cyc_List_List*_tmp761=
_cycalloc(sizeof(*_tmp761));_tmp761->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp763;_tmp763.tag=0;_tmp763.f1=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp763.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp763;});
_tmp762;}));_tmp761->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp761;});_tmp760;});
_tmp75F;});_tmp75E;});break;case 249: _LL2E8: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp765;_tmp765.tag=Cyc_AbstractDeclarator_tok;_tmp765.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->tms=({struct Cyc_List_List*_tmp767=
_cycalloc(sizeof(*_tmp767));_tmp767->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp769;_tmp769.tag=1;_tmp769.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp769.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp769.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp769;});
_tmp768;}));_tmp767->tl=0;_tmp767;});_tmp766;});_tmp765;});_tmp764;});break;case
250: _LL2E9: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp76B;_tmp76B.tag=Cyc_AbstractDeclarator_tok;_tmp76B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->tms=({struct Cyc_List_List*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp76F;_tmp76F.tag=1;_tmp76F.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp76F.f2=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76F.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp76F;});
_tmp76E;}));_tmp76D->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp76D;});_tmp76C;});
_tmp76B;});_tmp76A;});break;case 251: _LL2EA: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp771;_tmp771.tag=Cyc_AbstractDeclarator_tok;_tmp771.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772->tms=({struct Cyc_List_List*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp775;_tmp775.tag=3;_tmp775.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp777;_tmp777.tag=1;_tmp777.f1=0;_tmp777.f2=0;_tmp777.f3=0;_tmp777.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp777.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp777;});_tmp776;}));
_tmp775;});_tmp774;}));_tmp773->tl=0;_tmp773;});_tmp772;});_tmp771;});_tmp770;});
break;case 252: _LL2EB: {struct _tuple14 _tmp779;struct Cyc_List_List*_tmp77A;int
_tmp77B;struct Cyc_Absyn_VarargInfo*_tmp77C;struct Cyc_Core_Opt*_tmp77D;struct Cyc_List_List*
_tmp77E;struct _tuple14*_tmp778=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp779=*_tmp778;_tmp77A=_tmp779.f1;_tmp77B=_tmp779.f2;
_tmp77C=_tmp779.f3;_tmp77D=_tmp779.f4;_tmp77E=_tmp779.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp780;_tmp780.tag=Cyc_AbstractDeclarator_tok;
_tmp780.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp781=_cycalloc(sizeof(*
_tmp781));_tmp781->tms=({struct Cyc_List_List*_tmp782=_cycalloc(sizeof(*_tmp782));
_tmp782->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp783=
_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp784;_tmp784.tag=3;_tmp784.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp786;_tmp786.tag=1;_tmp786.f1=_tmp77A;_tmp786.f2=_tmp77B;_tmp786.f3=_tmp77C;
_tmp786.f4=_tmp77D;_tmp786.f5=_tmp77E;_tmp786;});_tmp785;}));_tmp784;});_tmp783;}));
_tmp782->tl=0;_tmp782;});_tmp781;});_tmp780;});_tmp77F;});break;}case 253: _LL2EC:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp787=_cycalloc(sizeof(*
_tmp787));_tmp787[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp788;_tmp788.tag=
Cyc_AbstractDeclarator_tok;_tmp788.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->tms=({struct Cyc_List_List*_tmp78A=
_cycalloc(sizeof(*_tmp78A));_tmp78A->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp78C;_tmp78C.tag=3;_tmp78C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp78E;_tmp78E.tag=1;_tmp78E.f1=0;_tmp78E.f2=0;_tmp78E.f3=0;_tmp78E.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp78E.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp78E;});_tmp78D;}));
_tmp78C;});_tmp78B;}));_tmp78A->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp78A;});_tmp789;});
_tmp788;});_tmp787;});break;case 254: _LL2ED: {struct _tuple14 _tmp790;struct Cyc_List_List*
_tmp791;int _tmp792;struct Cyc_Absyn_VarargInfo*_tmp793;struct Cyc_Core_Opt*_tmp794;
struct Cyc_List_List*_tmp795;struct _tuple14*_tmp78F=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp790=*_tmp78F;_tmp791=
_tmp790.f1;_tmp792=_tmp790.f2;_tmp793=_tmp790.f3;_tmp794=_tmp790.f4;_tmp795=
_tmp790.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp796=
_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp797;_tmp797.tag=Cyc_AbstractDeclarator_tok;_tmp797.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798->tms=({struct Cyc_List_List*_tmp799=
_cycalloc(sizeof(*_tmp799));_tmp799->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp79B;_tmp79B.tag=3;_tmp79B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp79D;_tmp79D.tag=1;_tmp79D.f1=_tmp791;_tmp79D.f2=_tmp792;_tmp79D.f3=_tmp793;
_tmp79D.f4=_tmp794;_tmp79D.f5=_tmp795;_tmp79D;});_tmp79C;}));_tmp79B;});_tmp79A;}));
_tmp799->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp799;});_tmp798;});
_tmp797;});_tmp796;});break;}case 255: _LL2EE: {struct Cyc_List_List*_tmp79E=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp7A0;_tmp7A0.tag=Cyc_AbstractDeclarator_tok;
_tmp7A0.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp7A1=_cycalloc(sizeof(*
_tmp7A1));_tmp7A1->tms=({struct Cyc_List_List*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));
_tmp7A2->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp7A3=
_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp7A4;_tmp7A4.tag=4;_tmp7A4.f1=_tmp79E;_tmp7A4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7A4.f3=0;
_tmp7A4;});_tmp7A3;}));_tmp7A2->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7A2;});_tmp7A1;});
_tmp7A0;});_tmp79F;});break;}case 256: _LL2EF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7A6;_tmp7A6.tag=Cyc_AbstractDeclarator_tok;_tmp7A6.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7->tms=({struct Cyc_List_List*_tmp7A8=
_cycalloc(sizeof(*_tmp7A8));_tmp7A8->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp7AA;_tmp7AA.tag=5;_tmp7AA.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7AA.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7AA;});_tmp7A9;}));
_tmp7A8->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp7A8;});_tmp7A7;});
_tmp7A6;});_tmp7A5;});break;case 257: _LL2F0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 258: _LL2F1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 259: _LL2F2:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 260:
_LL2F3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
261: _LL2F4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 262: _LL2F5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 263: _LL2F6: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7AB="`H";
_tag_arr(_tmp7AB,sizeof(char),_get_zero_arr_size(_tmp7AB,3));}))== 0)Cyc_Parse_err(({
const char*_tmp7AC="bad occurrence of heap region `H";_tag_arr(_tmp7AC,sizeof(
char),_get_zero_arr_size(_tmp7AC,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->name=({
struct _tagged_arr*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7BA;});_tmp7B7->identity=
0;_tmp7B7->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7B8=_cycalloc(
sizeof(*_tmp7B8));_tmp7B8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7B9;_tmp7B9.tag=0;
_tmp7B9.f1=(void*)((void*)3);_tmp7B9;});_tmp7B8;}));_tmp7B7;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({
struct Cyc_Absyn_VarType_struct _tmp7B6;_tmp7B6.tag=1;_tmp7B6.f1=tv;_tmp7B6;});
_tmp7B5;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7AD=_cycalloc(sizeof(*
_tmp7AD));_tmp7AD[0]=({struct Cyc_Stmt_tok_struct _tmp7AE;_tmp7AE.tag=Cyc_Stmt_tok;
_tmp7AE.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Absyn_Region_s_struct _tmp7B0;
_tmp7B0.tag=15;_tmp7B0.f1=tv;_tmp7B0.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1->f1=(void*)0;_tmp7B1->f2=({struct
_tagged_arr*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B2;});_tmp7B1;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));
_tmp7B3[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7B4;_tmp7B4.tag=15;_tmp7B4.f1=(
void*)t;_tmp7B4;});_tmp7B3;}),0);_tmp7B0.f3=0;_tmp7B0.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7B0;});_tmp7AF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7AE;});
_tmp7AD;});break;}case 264: _LL2F7: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7BB="H";
_tag_arr(_tmp7BB,sizeof(char),_get_zero_arr_size(_tmp7BB,2));}))== 0)Cyc_Parse_err(({
const char*_tmp7BC="bad occurrence of heap region `H";_tag_arr(_tmp7BC,sizeof(
char),_get_zero_arr_size(_tmp7BC,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7->name=({
struct _tagged_arr*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp7CD;_tmp7CD.tag=0;_tmp7CD.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7CB[1]={&
_tmp7CD};Cyc_aprintf(({const char*_tmp7CC="`%s";_tag_arr(_tmp7CC,sizeof(char),
_get_zero_arr_size(_tmp7CC,4));}),_tag_arr(_tmp7CB,sizeof(void*),1));}});_tmp7CA;});
_tmp7C7->identity=0;_tmp7C7->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7C9;_tmp7C9.tag=0;_tmp7C9.f1=(void*)((void*)3);_tmp7C9;});_tmp7C8;}));
_tmp7C7;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7C5=_cycalloc(
sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_VarType_struct _tmp7C6;_tmp7C6.tag=
1;_tmp7C6.f1=tv;_tmp7C6;});_tmp7C5;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Stmt_tok_struct
_tmp7BE;_tmp7BE.tag=Cyc_Stmt_tok;_tmp7BE.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7C0;_tmp7C0.tag=15;_tmp7C0.f1=tv;_tmp7C0.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));
_tmp7C1->f1=(void*)0;_tmp7C1->f2=({struct _tagged_arr*_tmp7C2=_cycalloc(sizeof(*
_tmp7C2));_tmp7C2[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7C2;});_tmp7C1;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7C4;_tmp7C4.tag=15;_tmp7C4.f1=(void*)t;_tmp7C4;});_tmp7C3;}),0);_tmp7C0.f3=0;
_tmp7C0.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7C0;});_tmp7BF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BE;});
_tmp7BD;});break;}case 265: _LL2F8: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp7CE="resetable";
_tag_arr(_tmp7CE,sizeof(char),_get_zero_arr_size(_tmp7CE,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7CF="expecting [resetable]";_tag_arr(_tmp7CF,sizeof(char),
_get_zero_arr_size(_tmp7CF,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp7D0="`H";_tag_arr(_tmp7D0,sizeof(char),
_get_zero_arr_size(_tmp7D0,3));}))== 0)Cyc_Parse_err(({const char*_tmp7D1="bad occurrence of heap region `H";
_tag_arr(_tmp7D1,sizeof(char),_get_zero_arr_size(_tmp7D1,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC->name=({
struct _tagged_arr*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7DF;});_tmp7DC->identity=
0;_tmp7DC->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7DD=_cycalloc(
sizeof(*_tmp7DD));_tmp7DD[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7DE;_tmp7DE.tag=0;
_tmp7DE.f1=(void*)((void*)3);_tmp7DE;});_tmp7DD;}));_tmp7DC;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=({
struct Cyc_Absyn_VarType_struct _tmp7DB;_tmp7DB.tag=1;_tmp7DB.f1=tv;_tmp7DB;});
_tmp7DA;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7D2=_cycalloc(sizeof(*
_tmp7D2));_tmp7D2[0]=({struct Cyc_Stmt_tok_struct _tmp7D3;_tmp7D3.tag=Cyc_Stmt_tok;
_tmp7D3.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7D4=
_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Absyn_Region_s_struct _tmp7D5;
_tmp7D5.tag=15;_tmp7D5.f1=tv;_tmp7D5.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7D6=_cycalloc(sizeof(*_tmp7D6));_tmp7D6->f1=(void*)0;_tmp7D6->f2=({struct
_tagged_arr*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7D7;});_tmp7D6;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));
_tmp7D8[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7D9;_tmp7D9.tag=15;_tmp7D9.f1=(
void*)t;_tmp7D9;});_tmp7D8;}),0);_tmp7D5.f3=1;_tmp7D5.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7D5;});_tmp7D4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D3;});
_tmp7D2;});break;}case 266: _LL2F9: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7E0="resetable";
_tag_arr(_tmp7E0,sizeof(char),_get_zero_arr_size(_tmp7E0,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7E1="expecting `resetable'";_tag_arr(_tmp7E1,sizeof(char),
_get_zero_arr_size(_tmp7E1,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp7E2="H";_tag_arr(_tmp7E2,sizeof(char),
_get_zero_arr_size(_tmp7E2,2));}))== 0)Cyc_Parse_err(({const char*_tmp7E3="bad occurrence of heap region `H";
_tag_arr(_tmp7E3,sizeof(char),_get_zero_arr_size(_tmp7E3,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE->name=({
struct _tagged_arr*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp7F4;_tmp7F4.tag=0;_tmp7F4.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7F2[1]={&
_tmp7F4};Cyc_aprintf(({const char*_tmp7F3="`%s";_tag_arr(_tmp7F3,sizeof(char),
_get_zero_arr_size(_tmp7F3,4));}),_tag_arr(_tmp7F2,sizeof(void*),1));}});_tmp7F1;});
_tmp7EE->identity=0;_tmp7EE->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7F0;_tmp7F0.tag=0;_tmp7F0.f1=(void*)((void*)3);_tmp7F0;});_tmp7EF;}));
_tmp7EE;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7EC=_cycalloc(
sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Absyn_VarType_struct _tmp7ED;_tmp7ED.tag=
1;_tmp7ED.f1=tv;_tmp7ED;});_tmp7EC;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Stmt_tok_struct
_tmp7E5;_tmp7E5.tag=Cyc_Stmt_tok;_tmp7E5.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7E7;_tmp7E7.tag=15;_tmp7E7.f1=tv;_tmp7E7.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));
_tmp7E8->f1=(void*)0;_tmp7E8->f2=({struct _tagged_arr*_tmp7E9=_cycalloc(sizeof(*
_tmp7E9));_tmp7E9[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7E9;});_tmp7E8;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7EB;_tmp7EB.tag=15;_tmp7EB.f1=(void*)t;_tmp7EB;});_tmp7EA;}),0);_tmp7E7.f3=1;
_tmp7E7.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
_tmp7E4;});break;}case 267: _LL2FA: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Stmt_tok_struct
_tmp7F6;_tmp7F6.tag=Cyc_Stmt_tok;_tmp7F6.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp7F8;_tmp7F8.tag=16;_tmp7F8.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F8;});_tmp7F7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F6;});_tmp7F5;});break;case 268: _LL2FB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({
struct Cyc_Stmt_tok_struct _tmp7FA;_tmp7FA.tag=Cyc_Stmt_tok;_tmp7FA.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));
_tmp7FB[0]=({struct Cyc_Absyn_Label_s_struct _tmp7FC;_tmp7FC.tag=12;_tmp7FC.f1=({
struct _tagged_arr*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7FD;});_tmp7FC.f2=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp7FC;});_tmp7FB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FA;});
_tmp7F9;});break;case 269: _LL2FC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7FE=
_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({struct Cyc_Stmt_tok_struct _tmp7FF;
_tmp7FF.tag=Cyc_Stmt_tok;_tmp7FF.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FF;});
_tmp7FE;});break;case 270: _LL2FD: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp800=
_cycalloc(sizeof(*_tmp800));_tmp800[0]=({struct Cyc_Stmt_tok_struct _tmp801;
_tmp801.tag=Cyc_Stmt_tok;_tmp801.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp801;});
_tmp800;});break;case 271: _LL2FE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp802=
_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Stmt_tok_struct _tmp803;
_tmp803.tag=Cyc_Stmt_tok;_tmp803.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp803;});
_tmp802;});break;case 272: _LL2FF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 273: _LL300: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Stmt_tok_struct
_tmp805;_tmp805.tag=Cyc_Stmt_tok;_tmp805.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp805;});_tmp804;});break;case 274: _LL301: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806[0]=({struct Cyc_Stmt_tok_struct
_tmp807;_tmp807.tag=Cyc_Stmt_tok;_tmp807.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp807;});_tmp806;});
break;case 275: _LL302: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 276: _LL303: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Stmt_tok_struct
_tmp809;_tmp809.tag=Cyc_Stmt_tok;_tmp809.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp809;});
_tmp808;});break;case 277: _LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80A=
_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({struct Cyc_Stmt_tok_struct _tmp80B;
_tmp80B.tag=Cyc_Stmt_tok;_tmp80B.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp80D;_tmp80D.tag=1;_tmp80D.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80D;});_tmp80C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp80B;});_tmp80A;});break;
case 278: _LL305: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80E=_cycalloc(
sizeof(*_tmp80E));_tmp80E[0]=({struct Cyc_Stmt_tok_struct _tmp80F;_tmp80F.tag=Cyc_Stmt_tok;
_tmp80F.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp811;_tmp811.tag=1;_tmp811.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp811;});_tmp810;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp80F;});_tmp80E;});
break;case 279: _LL306: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp812=_cycalloc(
sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Stmt_tok_struct _tmp813;_tmp813.tag=Cyc_Stmt_tok;
_tmp813.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp813;});
_tmp812;});break;case 280: _LL307: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Stmt_tok_struct _tmp815;
_tmp815.tag=Cyc_Stmt_tok;_tmp815.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp815;});
_tmp814;});break;case 281: _LL308: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp816=
_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Stmt_tok_struct _tmp817;
_tmp817.tag=Cyc_Stmt_tok;_tmp817.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp817;});
_tmp816;});break;case 282: _LL309: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp818=
_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Stmt_tok_struct _tmp819;
_tmp819.tag=Cyc_Stmt_tok;_tmp819.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp819;});
_tmp818;});break;case 283: _LL30A: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp81B;_tmp81B.tag=Cyc_SwitchClauseList_tok;_tmp81B.f1=0;_tmp81B;});_tmp81A;});
break;case 284: _LL30B: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp81D;_tmp81D.tag=Cyc_SwitchClauseList_tok;_tmp81D.f1=({struct Cyc_List_List*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->hd=({struct Cyc_Absyn_Switch_clause*
_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp81F->pat_vars=0;_tmp81F->where_clause=0;
_tmp81F->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp81F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp81F;});
_tmp81E->tl=0;_tmp81E;});_tmp81D;});_tmp81C;});break;case 285: _LL30C: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp820=_cycalloc(sizeof(*_tmp820));
_tmp820[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp821;_tmp821.tag=Cyc_SwitchClauseList_tok;
_tmp821.f1=({struct Cyc_List_List*_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822->hd=({
struct Cyc_Absyn_Switch_clause*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp823->pat_vars=0;_tmp823->where_clause=0;_tmp823->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp823->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp823;});
_tmp822->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp822;});_tmp821;});_tmp820;});break;case 286: _LL30D:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp824=_cycalloc(sizeof(*
_tmp824));_tmp824[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp825;_tmp825.tag=
Cyc_SwitchClauseList_tok;_tmp825.f1=({struct Cyc_List_List*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826->hd=({struct Cyc_Absyn_Switch_clause*_tmp827=_cycalloc(
sizeof(*_tmp827));_tmp827->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp827->pat_vars=0;
_tmp827->where_clause=0;_tmp827->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp827->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp827;});
_tmp826->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp826;});_tmp825;});_tmp824;});break;case 287: _LL30E:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp828=_cycalloc(sizeof(*
_tmp828));_tmp828[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp829;_tmp829.tag=
Cyc_SwitchClauseList_tok;_tmp829.f1=({struct Cyc_List_List*_tmp82A=_cycalloc(
sizeof(*_tmp82A));_tmp82A->hd=({struct Cyc_Absyn_Switch_clause*_tmp82B=_cycalloc(
sizeof(*_tmp82B));_tmp82B->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp82B->pat_vars=0;
_tmp82B->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp82B->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp82B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp82B;});
_tmp82A->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp82A;});_tmp829;});_tmp828;});break;case 288: _LL30F:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp82C=_cycalloc(sizeof(*
_tmp82C));_tmp82C[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp82D;_tmp82D.tag=
Cyc_SwitchClauseList_tok;_tmp82D.f1=({struct Cyc_List_List*_tmp82E=_cycalloc(
sizeof(*_tmp82E));_tmp82E->hd=({struct Cyc_Absyn_Switch_clause*_tmp82F=_cycalloc(
sizeof(*_tmp82F));_tmp82F->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp82F->pat_vars=0;
_tmp82F->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp82F->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp82F->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp82F;});
_tmp82E->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp82E;});_tmp82D;});_tmp82C;});break;case 289: _LL310:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp830=_cycalloc(sizeof(*_tmp830));
_tmp830[0]=({struct Cyc_Stmt_tok_struct _tmp831;_tmp831.tag=Cyc_Stmt_tok;_tmp831.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp831;});_tmp830;});break;case 290: _LL311: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({
struct Cyc_Stmt_tok_struct _tmp833;_tmp833.tag=Cyc_Stmt_tok;_tmp833.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp833;});_tmp832;});break;case 291: _LL312: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({
struct Cyc_Stmt_tok_struct _tmp835;_tmp835.tag=Cyc_Stmt_tok;_tmp835.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp835;});
_tmp834;});break;case 292: _LL313: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp836=
_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Stmt_tok_struct _tmp837;
_tmp837.tag=Cyc_Stmt_tok;_tmp837.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp837;});
_tmp836;});break;case 293: _LL314: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp838=
_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Stmt_tok_struct _tmp839;
_tmp839.tag=Cyc_Stmt_tok;_tmp839.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp839;});
_tmp838;});break;case 294: _LL315: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp83A=
_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Stmt_tok_struct _tmp83B;
_tmp83B.tag=Cyc_Stmt_tok;_tmp83B.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp83B;});
_tmp83A;});break;case 295: _LL316: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp83C=
_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Stmt_tok_struct _tmp83D;
_tmp83D.tag=Cyc_Stmt_tok;_tmp83D.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
_tmp83C;});break;case 296: _LL317: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp83E=
_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Stmt_tok_struct _tmp83F;
_tmp83F.tag=Cyc_Stmt_tok;_tmp83F.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
_tmp83E;});break;case 297: _LL318: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp840=
_cycalloc(sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Stmt_tok_struct _tmp841;
_tmp841.tag=Cyc_Stmt_tok;_tmp841.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
_tmp840;});break;case 298: _LL319: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp842=
_cycalloc(sizeof(*_tmp842));_tmp842[0]=({struct Cyc_Stmt_tok_struct _tmp843;
_tmp843.tag=Cyc_Stmt_tok;_tmp843.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
_tmp842;});break;case 299: _LL31A: {struct Cyc_List_List*_tmp844=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp845=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp846=
_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Stmt_tok_struct _tmp847;
_tmp847.tag=Cyc_Stmt_tok;_tmp847.f1=Cyc_Parse_flatten_declarations(_tmp844,
_tmp845);_tmp847;});_tmp846;});break;}case 300: _LL31B: {struct Cyc_List_List*
_tmp848=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp849=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({
struct Cyc_Stmt_tok_struct _tmp84B;_tmp84B.tag=Cyc_Stmt_tok;_tmp84B.f1=Cyc_Parse_flatten_declarations(
_tmp848,_tmp849);_tmp84B;});_tmp84A;});break;}case 301: _LL31C: {struct Cyc_List_List*
_tmp84C=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp84D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({
struct Cyc_Stmt_tok_struct _tmp84F;_tmp84F.tag=Cyc_Stmt_tok;_tmp84F.f1=Cyc_Parse_flatten_declarations(
_tmp84C,_tmp84D);_tmp84F;});_tmp84E;});break;}case 302: _LL31D: {struct Cyc_List_List*
_tmp850=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp851=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({
struct Cyc_Stmt_tok_struct _tmp853;_tmp853.tag=Cyc_Stmt_tok;_tmp853.f1=Cyc_Parse_flatten_declarations(
_tmp850,_tmp851);_tmp853;});_tmp852;});break;}case 303: _LL31E: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854[0]=({
struct Cyc_Stmt_tok_struct _tmp855;_tmp855.tag=Cyc_Stmt_tok;_tmp855.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp856;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp855;});
_tmp854;});break;case 304: _LL31F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp857=
_cycalloc(sizeof(*_tmp857));_tmp857[0]=({struct Cyc_Stmt_tok_struct _tmp858;
_tmp858.tag=Cyc_Stmt_tok;_tmp858.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp858;});
_tmp857;});break;case 305: _LL320: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp859=
_cycalloc(sizeof(*_tmp859));_tmp859[0]=({struct Cyc_Stmt_tok_struct _tmp85A;
_tmp85A.tag=Cyc_Stmt_tok;_tmp85A.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp85A;});
_tmp859;});break;case 306: _LL321: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85B=
_cycalloc(sizeof(*_tmp85B));_tmp85B[0]=({struct Cyc_Stmt_tok_struct _tmp85C;
_tmp85C.tag=Cyc_Stmt_tok;_tmp85C.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp85C;});
_tmp85B;});break;case 307: _LL322: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85D=
_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({struct Cyc_Stmt_tok_struct _tmp85E;
_tmp85E.tag=Cyc_Stmt_tok;_tmp85E.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp85E;});_tmp85D;});break;case 308: _LL323: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F[0]=({
struct Cyc_Stmt_tok_struct _tmp860;_tmp860.tag=Cyc_Stmt_tok;_tmp860.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp860;});_tmp85F;});break;case 309: _LL324: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=({
struct Cyc_Stmt_tok_struct _tmp862;_tmp862.tag=Cyc_Stmt_tok;_tmp862.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp862;});_tmp861;});break;case 310: _LL325: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863[0]=({
struct Cyc_Stmt_tok_struct _tmp864;_tmp864.tag=Cyc_Stmt_tok;_tmp864.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp864;});_tmp863;});break;case 311: _LL326: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 312: _LL327:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 313:
_LL328: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp865=_cycalloc(sizeof(*
_tmp865));_tmp865[0]=({struct Cyc_Pattern_tok_struct _tmp866;_tmp866.tag=Cyc_Pattern_tok;
_tmp866.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp866;});
_tmp865;});break;case 314: _LL329: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 315: _LL32A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867[0]=({struct Cyc_Pattern_tok_struct
_tmp868;_tmp868.tag=Cyc_Pattern_tok;_tmp868.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp868;});
_tmp867;});break;case 316: _LL32B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 317: _LL32C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869[0]=({struct Cyc_Pattern_tok_struct
_tmp86A;_tmp86A.tag=Cyc_Pattern_tok;_tmp86A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp86A;});
_tmp869;});break;case 318: _LL32D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 319: _LL32E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({struct Cyc_Pattern_tok_struct
_tmp86C;_tmp86C.tag=Cyc_Pattern_tok;_tmp86C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp86C;});
_tmp86B;});break;case 320: _LL32F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 321: _LL330: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D[0]=({struct Cyc_Pattern_tok_struct
_tmp86E;_tmp86E.tag=Cyc_Pattern_tok;_tmp86E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp86E;});
_tmp86D;});break;case 322: _LL331: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 323: _LL332: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp86F=_cycalloc(sizeof(*_tmp86F));_tmp86F[0]=({struct Cyc_Pattern_tok_struct
_tmp870;_tmp870.tag=Cyc_Pattern_tok;_tmp870.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp870;});
_tmp86F;});break;case 324: _LL333: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL334: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp871=_cycalloc(sizeof(*_tmp871));_tmp871[0]=({struct Cyc_Pattern_tok_struct
_tmp872;_tmp872.tag=Cyc_Pattern_tok;_tmp872.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp872;});
_tmp871;});break;case 326: _LL335: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp873=_cycalloc(sizeof(*_tmp873));_tmp873[0]=({struct Cyc_Pattern_tok_struct
_tmp874;_tmp874.tag=Cyc_Pattern_tok;_tmp874.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp874;});
_tmp873;});break;case 327: _LL336: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL337: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875[0]=({struct Cyc_Pattern_tok_struct
_tmp876;_tmp876.tag=Cyc_Pattern_tok;_tmp876.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp876;});
_tmp875;});break;case 329: _LL338: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp877=_cycalloc(sizeof(*_tmp877));_tmp877[0]=({struct Cyc_Pattern_tok_struct
_tmp878;_tmp878.tag=Cyc_Pattern_tok;_tmp878.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp878;});
_tmp877;});break;case 330: _LL339: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Pattern_tok_struct
_tmp87A;_tmp87A.tag=Cyc_Pattern_tok;_tmp87A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87A;});
_tmp879;});break;case 331: _LL33A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87B=_cycalloc(sizeof(*_tmp87B));_tmp87B[0]=({struct Cyc_Pattern_tok_struct
_tmp87C;_tmp87C.tag=Cyc_Pattern_tok;_tmp87C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87C;});
_tmp87B;});break;case 332: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL33C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=({struct Cyc_Pattern_tok_struct
_tmp87E;_tmp87E.tag=Cyc_Pattern_tok;_tmp87E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87E;});
_tmp87D;});break;case 334: _LL33D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Pattern_tok_struct
_tmp880;_tmp880.tag=Cyc_Pattern_tok;_tmp880.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp880;});
_tmp87F;});break;case 335: _LL33E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL33F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct Cyc_Pattern_tok_struct
_tmp882;_tmp882.tag=Cyc_Pattern_tok;_tmp882.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp882;});
_tmp881;});break;case 337: _LL340: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[0]=({struct Cyc_Pattern_tok_struct
_tmp884;_tmp884.tag=Cyc_Pattern_tok;_tmp884.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp884;});
_tmp883;});break;case 338: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 339: _LL342: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885[0]=({struct Cyc_Pattern_tok_struct
_tmp886;_tmp886.tag=Cyc_Pattern_tok;_tmp886.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp886;});
_tmp885;});break;case 340: _LL343: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Pattern_tok_struct
_tmp888;_tmp888.tag=Cyc_Pattern_tok;_tmp888.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp888;});
_tmp887;});break;case 341: _LL344: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({struct Cyc_Pattern_tok_struct
_tmp88A;_tmp88A.tag=Cyc_Pattern_tok;_tmp88A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88A;});
_tmp889;});break;case 342: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 343: _LL346: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Pattern_tok_struct
_tmp88C;_tmp88C.tag=Cyc_Pattern_tok;_tmp88C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)])).f3,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88C;});
_tmp88B;});break;case 344: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL348: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Pattern_tok_struct
_tmp88E;_tmp88E.tag=Cyc_Pattern_tok;_tmp88E.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp88E;});_tmp88D;});
break;case 346: _LL349: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp88F=
_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Pattern_tok_struct _tmp890;
_tmp890.tag=Cyc_Pattern_tok;_tmp890.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp890;});
_tmp88F;});break;case 347: _LL34A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Pattern_tok_struct
_tmp892;_tmp892.tag=Cyc_Pattern_tok;_tmp892.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp892;});_tmp891;});break;case 348: _LL34B: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[
0]=({struct Cyc_Pattern_tok_struct _tmp894;_tmp894.tag=Cyc_Pattern_tok;_tmp894.f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp894;});
_tmp893;});break;case 349: _LL34C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Pattern_tok_struct
_tmp896;_tmp896.tag=Cyc_Pattern_tok;_tmp896.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp897=_cycalloc(sizeof(*
_tmp897));_tmp897[0]=({struct Cyc_Absyn_StructField_struct _tmp898;_tmp898.tag=0;
_tmp898.f1=({struct _tagged_arr*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp899;});_tmp898;});_tmp897;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp896;});
_tmp895;});break;case 350: _LL34D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Pattern_tok_struct
_tmp89B;_tmp89B.tag=Cyc_Pattern_tok;_tmp89B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp89C=_cycalloc_atomic(
sizeof(*_tmp89C));_tmp89C[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp89D;_tmp89D.tag=
1;_tmp89D.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp89D;});_tmp89C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp89B;});_tmp89A;});break;case 351: _LL34E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 352: _LL34F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 353:
_LL350: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp89E=_cycalloc(sizeof(*
_tmp89E));_tmp89E[0]=({struct Cyc_Pattern_tok_struct _tmp89F;_tmp89F.tag=Cyc_Pattern_tok;
_tmp89F.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89F;});
_tmp89E;});break;case 354: _LL351: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({struct Cyc_Pattern_tok_struct
_tmp8A1;_tmp8A1.tag=Cyc_Pattern_tok;_tmp8A1.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8A1;});_tmp8A0;});
break;case 355: _LL352: {struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp8A2=(void*)e->r;
void*_tmp8A3;void*_tmp8A4;char _tmp8A5;void*_tmp8A6;void*_tmp8A7;short _tmp8A8;
void*_tmp8A9;void*_tmp8AA;int _tmp8AB;void*_tmp8AC;struct _tagged_arr _tmp8AD;void*
_tmp8AE;void*_tmp8AF;void*_tmp8B0;_LL355: if(*((int*)_tmp8A2)!= 0)goto _LL357;
_tmp8A3=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8A2)->f1;if(_tmp8A3 <= (void*)
1  || *((int*)_tmp8A3)!= 0)goto _LL357;_tmp8A4=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp8A3)->f1;_tmp8A5=((struct Cyc_Absyn_Char_c_struct*)_tmp8A3)->f2;_LL356: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[
0]=({struct Cyc_Pattern_tok_struct _tmp8B2;_tmp8B2.tag=Cyc_Pattern_tok;_tmp8B2.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*_tmp8B3=
_cycalloc_atomic(sizeof(*_tmp8B3));_tmp8B3[0]=({struct Cyc_Absyn_Char_p_struct
_tmp8B4;_tmp8B4.tag=8;_tmp8B4.f1=_tmp8A5;_tmp8B4;});_tmp8B3;}),e->loc);_tmp8B2;});
_tmp8B1;});goto _LL354;_LL357: if(*((int*)_tmp8A2)!= 0)goto _LL359;_tmp8A6=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8A2)->f1;if(_tmp8A6 <= (void*)1  || *((int*)
_tmp8A6)!= 1)goto _LL359;_tmp8A7=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp8A6)->f1;
_tmp8A8=((struct Cyc_Absyn_Short_c_struct*)_tmp8A6)->f2;_LL358: yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({
struct Cyc_Pattern_tok_struct _tmp8B6;_tmp8B6.tag=Cyc_Pattern_tok;_tmp8B6.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7[
0]=({struct Cyc_Absyn_Int_p_struct _tmp8B8;_tmp8B8.tag=7;_tmp8B8.f1=(void*)_tmp8A7;
_tmp8B8.f2=(int)_tmp8A8;_tmp8B8;});_tmp8B7;}),e->loc);_tmp8B6;});_tmp8B5;});goto
_LL354;_LL359: if(*((int*)_tmp8A2)!= 0)goto _LL35B;_tmp8A9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8A2)->f1;if(_tmp8A9 <= (void*)1  || *((int*)_tmp8A9)!= 2)goto _LL35B;_tmp8AA=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp8A9)->f1;_tmp8AB=((struct Cyc_Absyn_Int_c_struct*)
_tmp8A9)->f2;_LL35A: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8B9=
_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=({struct Cyc_Pattern_tok_struct _tmp8BA;
_tmp8BA.tag=Cyc_Pattern_tok;_tmp8BA.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp8BB=_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=({struct Cyc_Absyn_Int_p_struct
_tmp8BC;_tmp8BC.tag=7;_tmp8BC.f1=(void*)_tmp8AA;_tmp8BC.f2=_tmp8AB;_tmp8BC;});
_tmp8BB;}),e->loc);_tmp8BA;});_tmp8B9;});goto _LL354;_LL35B: if(*((int*)_tmp8A2)!= 
0)goto _LL35D;_tmp8AC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8A2)->f1;if(
_tmp8AC <= (void*)1  || *((int*)_tmp8AC)!= 4)goto _LL35D;_tmp8AD=((struct Cyc_Absyn_Float_c_struct*)
_tmp8AC)->f1;_LL35C: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8BD=
_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({struct Cyc_Pattern_tok_struct _tmp8BE;
_tmp8BE.tag=Cyc_Pattern_tok;_tmp8BE.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*
_tmp8BF=_cycalloc(sizeof(*_tmp8BF));_tmp8BF[0]=({struct Cyc_Absyn_Float_p_struct
_tmp8C0;_tmp8C0.tag=9;_tmp8C0.f1=_tmp8AD;_tmp8C0;});_tmp8BF;}),e->loc);_tmp8BE;});
_tmp8BD;});goto _LL354;_LL35D: if(*((int*)_tmp8A2)!= 0)goto _LL35F;_tmp8AE=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8A2)->f1;if((int)_tmp8AE != 0)goto _LL35F;
_LL35E: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8C1=_cycalloc(sizeof(*
_tmp8C1));_tmp8C1[0]=({struct Cyc_Pattern_tok_struct _tmp8C2;_tmp8C2.tag=Cyc_Pattern_tok;
_tmp8C2.f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp8C2;});_tmp8C1;});goto _LL354;
_LL35F: if(*((int*)_tmp8A2)!= 0)goto _LL361;_tmp8AF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8A2)->f1;if(_tmp8AF <= (void*)1  || *((int*)_tmp8AF)!= 5)goto _LL361;_LL360: Cyc_Parse_err(({
const char*_tmp8C3="strings cannot occur within patterns";_tag_arr(_tmp8C3,
sizeof(char),_get_zero_arr_size(_tmp8C3,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL354;
_LL361: if(*((int*)_tmp8A2)!= 0)goto _LL363;_tmp8B0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8A2)->f1;if(_tmp8B0 <= (void*)1  || *((int*)_tmp8B0)!= 3)goto _LL363;_LL362: Cyc_Parse_unimp(({
const char*_tmp8C4="long long's in patterns";_tag_arr(_tmp8C4,sizeof(char),
_get_zero_arr_size(_tmp8C4,24));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL354;
_LL363:;_LL364: Cyc_Parse_err(({const char*_tmp8C5="bad constant in case";_tag_arr(
_tmp8C5,sizeof(char),_get_zero_arr_size(_tmp8C5,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL354:;}break;}
case 356: _LL353: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8C6=_cycalloc(
sizeof(*_tmp8C6));_tmp8C6[0]=({struct Cyc_Pattern_tok_struct _tmp8C7;_tmp8C7.tag=
Cyc_Pattern_tok;_tmp8C7.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp8C9;_tmp8C9.tag=12;_tmp8C9.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C9;});_tmp8C8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C7;});
_tmp8C6;});break;case 357: _LL365: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8CA=_cycalloc(sizeof(*_tmp8CA));_tmp8CA[0]=({struct Cyc_Pattern_tok_struct
_tmp8CB;_tmp8CB.tag=Cyc_Pattern_tok;_tmp8CB.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Tuple_p_struct*_tmp8CC=_cycalloc(sizeof(*_tmp8CC));_tmp8CC[0]=({struct
Cyc_Absyn_Tuple_p_struct _tmp8CD;_tmp8CD.tag=3;_tmp8CD.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8CD;});_tmp8CC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8CB;});_tmp8CA;});break;case 358: _LL366: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));_tmp8CE[
0]=({struct Cyc_Pattern_tok_struct _tmp8CF;_tmp8CF.tag=Cyc_Pattern_tok;_tmp8CF.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp8D0=
_cycalloc(sizeof(*_tmp8D0));_tmp8D0[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp8D1;_tmp8D1.tag=13;_tmp8D1.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8D1.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8D1;});_tmp8D0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8CF;});_tmp8CE;});break;case 359: _LL367: {struct
Cyc_List_List*_tmp8D2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({
struct Cyc_Pattern_tok_struct _tmp8D4;_tmp8D4.tag=Cyc_Pattern_tok;_tmp8D4.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));
_tmp8D5[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp8D6;_tmp8D6.tag=5;_tmp8D6.f1=({
struct Cyc_Absyn_AggrInfo _tmp8D7;_tmp8D7.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8D8=_cycalloc(sizeof(*_tmp8D8));_tmp8D8[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp8D9;_tmp8D9.tag=0;_tmp8D9.f1=(void*)((void*)0);_tmp8D9.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8D9;});_tmp8D8;}));
_tmp8D7.targs=0;_tmp8D7;});_tmp8D6.f2=_tmp8D2;_tmp8D6.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8D6;});_tmp8D5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8D4;});_tmp8D3;});break;}case 360: _LL368: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA[
0]=({struct Cyc_Pattern_tok_struct _tmp8DB;_tmp8DB.tag=Cyc_Pattern_tok;_tmp8DB.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp8DC=_cycalloc(
sizeof(*_tmp8DC));_tmp8DC[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp8DD;_tmp8DD.tag=
4;_tmp8DD.f1=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8DD;});_tmp8DC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8DB;});
_tmp8DA;});break;case 361: _LL369: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8DE=_cycalloc(sizeof(*_tmp8DE));_tmp8DE[0]=({struct Cyc_Pattern_tok_struct
_tmp8DF;_tmp8DF.tag=Cyc_Pattern_tok;_tmp8DF.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp8E0=_cycalloc(sizeof(*_tmp8E0));_tmp8E0[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp8E1;_tmp8E1.tag=4;_tmp8E1.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));
_tmp8E2[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp8E3;_tmp8E3.tag=4;_tmp8E3.f1=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp8E3;});_tmp8E2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E1;});
_tmp8E0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8DF;});_tmp8DE;});break;case 362: _LL36A: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8E4=_cycalloc(sizeof(*_tmp8E4));_tmp8E4[
0]=({struct Cyc_Pattern_tok_struct _tmp8E5;_tmp8E5.tag=Cyc_Pattern_tok;_tmp8E5.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp8E6=_cycalloc(
sizeof(*_tmp8E6));_tmp8E6[0]=({struct Cyc_Absyn_Reference_p_struct _tmp8E7;_tmp8E7.tag=
1;_tmp8E7.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp8E8=_cycalloc(sizeof(*
_tmp8E8));_tmp8E8->f1=(void*)0;_tmp8E8->f2=({struct _tagged_arr*_tmp8E9=_cycalloc(
sizeof(*_tmp8E9));_tmp8E9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8E9;});_tmp8E8;}),(void*)
0,0);_tmp8E7;});_tmp8E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E5;});
_tmp8E4;});break;case 363: _LL36B: {void*_tmp8EA=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8F3=_cycalloc(sizeof(*_tmp8F3));_tmp8F3[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8F4;_tmp8F4.tag=0;_tmp8F4.f1=(void*)((void*)5);_tmp8F4;});_tmp8F3;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8EB=_cycalloc(sizeof(*_tmp8EB));_tmp8EB[
0]=({struct Cyc_Pattern_tok_struct _tmp8EC;_tmp8EC.tag=Cyc_Pattern_tok;_tmp8EC.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp8ED=_cycalloc(
sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp8EE;_tmp8EE.tag=
2;_tmp8EE.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp8EA);
_tmp8EE.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp8EF=_cycalloc(sizeof(*
_tmp8EF));_tmp8EF->f1=(void*)0;_tmp8EF->f2=({struct _tagged_arr*_tmp8F0=_cycalloc(
sizeof(*_tmp8F0));_tmp8F0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8F0;});_tmp8EF;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp8F1=_cycalloc(sizeof(*_tmp8F1));
_tmp8F1[0]=({struct Cyc_Absyn_TagType_struct _tmp8F2;_tmp8F2.tag=17;_tmp8F2.f1=(
void*)_tmp8EA;_tmp8F2;});_tmp8F1;}),0);_tmp8EE;});_tmp8ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EC;});
_tmp8EB;});break;}case 364: _LL36C: {struct Cyc_Absyn_Tvar*_tmp8F5=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp901;_tmp901.tag=0;_tmp901.f1=(void*)((void*)
5);_tmp901;});_tmp900;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8F6=
_cycalloc(sizeof(*_tmp8F6));_tmp8F6[0]=({struct Cyc_Pattern_tok_struct _tmp8F7;
_tmp8F7.tag=Cyc_Pattern_tok;_tmp8F7.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp8F8=_cycalloc(sizeof(*_tmp8F8));_tmp8F8[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp8F9;_tmp8F9.tag=2;_tmp8F9.f1=_tmp8F5;_tmp8F9.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->f1=(void*)0;_tmp8FA->f2=({
struct _tagged_arr*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8FB;});_tmp8FA;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp8FC=_cycalloc(sizeof(*_tmp8FC));
_tmp8FC[0]=({struct Cyc_Absyn_TagType_struct _tmp8FD;_tmp8FD.tag=17;_tmp8FD.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));
_tmp8FE[0]=({struct Cyc_Absyn_VarType_struct _tmp8FF;_tmp8FF.tag=1;_tmp8FF.f1=
_tmp8F5;_tmp8FF;});_tmp8FE;}));_tmp8FD;});_tmp8FC;}),0);_tmp8F9;});_tmp8F8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8F7;});_tmp8F6;});break;}case 365: _LL36D: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp902=_cycalloc(sizeof(*_tmp902));
_tmp902[0]=({struct Cyc_PatternList_tok_struct _tmp903;_tmp903.tag=Cyc_PatternList_tok;
_tmp903.f1=({struct Cyc_List_List*_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904->hd=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp904->tl=0;_tmp904;});_tmp903;});_tmp902;});break;case 366: _LL36E: yyval=(void*)({
struct Cyc_PatternList_tok_struct*_tmp905=_cycalloc(sizeof(*_tmp905));_tmp905[0]=({
struct Cyc_PatternList_tok_struct _tmp906;_tmp906.tag=Cyc_PatternList_tok;_tmp906.f1=({
struct Cyc_List_List*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907->hd=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp907->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp907;});_tmp906;});
_tmp905;});break;case 367: _LL36F: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908[0]=({struct Cyc_FieldPattern_tok_struct
_tmp909;_tmp909.tag=Cyc_FieldPattern_tok;_tmp909.f1=({struct _tuple11*_tmp90A=
_cycalloc(sizeof(*_tmp90A));_tmp90A->f1=0;_tmp90A->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90A;});_tmp909;});
_tmp908;});break;case 368: _LL370: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp90B=_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=({struct Cyc_FieldPattern_tok_struct
_tmp90C;_tmp90C.tag=Cyc_FieldPattern_tok;_tmp90C.f1=({struct _tuple11*_tmp90D=
_cycalloc(sizeof(*_tmp90D));_tmp90D->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp90D->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90D;});_tmp90C;});
_tmp90B;});break;case 369: _LL371: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp90F;_tmp90F.tag=Cyc_FieldPatternList_tok;_tmp90F.f1=({struct Cyc_List_List*
_tmp910=_cycalloc(sizeof(*_tmp910));_tmp910->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp910->tl=0;_tmp910;});
_tmp90F;});_tmp90E;});break;case 370: _LL372: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp911=_cycalloc(sizeof(*_tmp911));_tmp911[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp912;_tmp912.tag=Cyc_FieldPatternList_tok;_tmp912.f1=({struct Cyc_List_List*
_tmp913=_cycalloc(sizeof(*_tmp913));_tmp913->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp913->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp913;});_tmp912;});
_tmp911;});break;case 371: _LL373: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 372: _LL374: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914[0]=({struct Cyc_Exp_tok_struct _tmp915;
_tmp915.tag=Cyc_Exp_tok;_tmp915.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp915;});
_tmp914;});break;case 373: _LL375: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 374: _LL376: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp916=_cycalloc(sizeof(*_tmp916));_tmp916[0]=({struct Cyc_Exp_tok_struct _tmp917;
_tmp917.tag=Cyc_Exp_tok;_tmp917.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp917;});
_tmp916;});break;case 375: _LL377: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp918=_cycalloc(sizeof(*_tmp918));_tmp918[0]=({struct Cyc_Primopopt_tok_struct
_tmp919;_tmp919.tag=Cyc_Primopopt_tok;_tmp919.f1=0;_tmp919;});_tmp918;});break;
case 376: _LL378: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp91A=_cycalloc(
sizeof(*_tmp91A));_tmp91A[0]=({struct Cyc_Primopopt_tok_struct _tmp91B;_tmp91B.tag=
Cyc_Primopopt_tok;_tmp91B.f1=({struct Cyc_Core_Opt*_tmp91C=_cycalloc(sizeof(*
_tmp91C));_tmp91C->v=(void*)((void*)1);_tmp91C;});_tmp91B;});_tmp91A;});break;
case 377: _LL379: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp91D=_cycalloc(
sizeof(*_tmp91D));_tmp91D[0]=({struct Cyc_Primopopt_tok_struct _tmp91E;_tmp91E.tag=
Cyc_Primopopt_tok;_tmp91E.f1=({struct Cyc_Core_Opt*_tmp91F=_cycalloc(sizeof(*
_tmp91F));_tmp91F->v=(void*)((void*)3);_tmp91F;});_tmp91E;});_tmp91D;});break;
case 378: _LL37A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp920=_cycalloc(
sizeof(*_tmp920));_tmp920[0]=({struct Cyc_Primopopt_tok_struct _tmp921;_tmp921.tag=
Cyc_Primopopt_tok;_tmp921.f1=({struct Cyc_Core_Opt*_tmp922=_cycalloc(sizeof(*
_tmp922));_tmp922->v=(void*)((void*)4);_tmp922;});_tmp921;});_tmp920;});break;
case 379: _LL37B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp923=_cycalloc(
sizeof(*_tmp923));_tmp923[0]=({struct Cyc_Primopopt_tok_struct _tmp924;_tmp924.tag=
Cyc_Primopopt_tok;_tmp924.f1=({struct Cyc_Core_Opt*_tmp925=_cycalloc(sizeof(*
_tmp925));_tmp925->v=(void*)((void*)0);_tmp925;});_tmp924;});_tmp923;});break;
case 380: _LL37C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp926=_cycalloc(
sizeof(*_tmp926));_tmp926[0]=({struct Cyc_Primopopt_tok_struct _tmp927;_tmp927.tag=
Cyc_Primopopt_tok;_tmp927.f1=({struct Cyc_Core_Opt*_tmp928=_cycalloc(sizeof(*
_tmp928));_tmp928->v=(void*)((void*)2);_tmp928;});_tmp927;});_tmp926;});break;
case 381: _LL37D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp929=_cycalloc(
sizeof(*_tmp929));_tmp929[0]=({struct Cyc_Primopopt_tok_struct _tmp92A;_tmp92A.tag=
Cyc_Primopopt_tok;_tmp92A.f1=({struct Cyc_Core_Opt*_tmp92B=_cycalloc(sizeof(*
_tmp92B));_tmp92B->v=(void*)((void*)16);_tmp92B;});_tmp92A;});_tmp929;});break;
case 382: _LL37E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp92C=_cycalloc(
sizeof(*_tmp92C));_tmp92C[0]=({struct Cyc_Primopopt_tok_struct _tmp92D;_tmp92D.tag=
Cyc_Primopopt_tok;_tmp92D.f1=({struct Cyc_Core_Opt*_tmp92E=_cycalloc(sizeof(*
_tmp92E));_tmp92E->v=(void*)((void*)17);_tmp92E;});_tmp92D;});_tmp92C;});break;
case 383: _LL37F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp92F=_cycalloc(
sizeof(*_tmp92F));_tmp92F[0]=({struct Cyc_Primopopt_tok_struct _tmp930;_tmp930.tag=
Cyc_Primopopt_tok;_tmp930.f1=({struct Cyc_Core_Opt*_tmp931=_cycalloc(sizeof(*
_tmp931));_tmp931->v=(void*)((void*)13);_tmp931;});_tmp930;});_tmp92F;});break;
case 384: _LL380: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp932=_cycalloc(
sizeof(*_tmp932));_tmp932[0]=({struct Cyc_Primopopt_tok_struct _tmp933;_tmp933.tag=
Cyc_Primopopt_tok;_tmp933.f1=({struct Cyc_Core_Opt*_tmp934=_cycalloc(sizeof(*
_tmp934));_tmp934->v=(void*)((void*)15);_tmp934;});_tmp933;});_tmp932;});break;
case 385: _LL381: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp935=_cycalloc(
sizeof(*_tmp935));_tmp935[0]=({struct Cyc_Primopopt_tok_struct _tmp936;_tmp936.tag=
Cyc_Primopopt_tok;_tmp936.f1=({struct Cyc_Core_Opt*_tmp937=_cycalloc(sizeof(*
_tmp937));_tmp937->v=(void*)((void*)14);_tmp937;});_tmp936;});_tmp935;});break;
case 386: _LL382: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 387: _LL383: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp938=_cycalloc(
sizeof(*_tmp938));_tmp938[0]=({struct Cyc_Exp_tok_struct _tmp939;_tmp939.tag=Cyc_Exp_tok;
_tmp939.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp939;});
_tmp938;});break;case 388: _LL384: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp93A=
_cycalloc(sizeof(*_tmp93A));_tmp93A[0]=({struct Cyc_Exp_tok_struct _tmp93B;_tmp93B.tag=
Cyc_Exp_tok;_tmp93B.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp93B;});
_tmp93A;});break;case 389: _LL385: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp93C=
_cycalloc(sizeof(*_tmp93C));_tmp93C[0]=({struct Cyc_Exp_tok_struct _tmp93D;_tmp93D.tag=
Cyc_Exp_tok;_tmp93D.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp93D;});
_tmp93C;});break;case 390: _LL386: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp93E=
_cycalloc(sizeof(*_tmp93E));_tmp93E[0]=({struct Cyc_Exp_tok_struct _tmp93F;_tmp93F.tag=
Cyc_Exp_tok;_tmp93F.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp93F;});
_tmp93E;});break;case 391: _LL387: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp940=
_cycalloc(sizeof(*_tmp940));_tmp940[0]=({struct Cyc_Exp_tok_struct _tmp941;_tmp941.tag=
Cyc_Exp_tok;_tmp941.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp941;});
_tmp940;});break;case 392: _LL388: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp942=
_cycalloc(sizeof(*_tmp942));_tmp942[0]=({struct Cyc_Exp_tok_struct _tmp943;_tmp943.tag=
Cyc_Exp_tok;_tmp943.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp943;});
_tmp942;});break;case 393: _LL389: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 394: _LL38A: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 395: _LL38B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp944=_cycalloc(sizeof(*_tmp944));_tmp944[0]=({struct Cyc_Exp_tok_struct _tmp945;
_tmp945.tag=Cyc_Exp_tok;_tmp945.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp945;});
_tmp944;});break;case 396: _LL38C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 397: _LL38D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp946=_cycalloc(sizeof(*_tmp946));_tmp946[0]=({struct Cyc_Exp_tok_struct _tmp947;
_tmp947.tag=Cyc_Exp_tok;_tmp947.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp947;});
_tmp946;});break;case 398: _LL38E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 399: _LL38F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp948=_cycalloc(sizeof(*_tmp948));_tmp948[0]=({struct Cyc_Exp_tok_struct _tmp949;
_tmp949.tag=Cyc_Exp_tok;_tmp949.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp949;});
_tmp948;});break;case 400: _LL390: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 401: _LL391: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp94A=_cycalloc(sizeof(*_tmp94A));_tmp94A[0]=({struct Cyc_Exp_tok_struct _tmp94B;
_tmp94B.tag=Cyc_Exp_tok;_tmp94B.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94B;});
_tmp94A;});break;case 402: _LL392: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 403: _LL393: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp94C=_cycalloc(sizeof(*_tmp94C));_tmp94C[0]=({struct Cyc_Exp_tok_struct _tmp94D;
_tmp94D.tag=Cyc_Exp_tok;_tmp94D.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94D;});
_tmp94C;});break;case 404: _LL394: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL395: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp94E=_cycalloc(sizeof(*_tmp94E));_tmp94E[0]=({struct Cyc_Exp_tok_struct _tmp94F;
_tmp94F.tag=Cyc_Exp_tok;_tmp94F.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94F;});
_tmp94E;});break;case 406: _LL396: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp950=
_cycalloc(sizeof(*_tmp950));_tmp950[0]=({struct Cyc_Exp_tok_struct _tmp951;_tmp951.tag=
Cyc_Exp_tok;_tmp951.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp951;});
_tmp950;});break;case 407: _LL397: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 408: _LL398: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp952=_cycalloc(sizeof(*_tmp952));_tmp952[0]=({struct Cyc_Exp_tok_struct _tmp953;
_tmp953.tag=Cyc_Exp_tok;_tmp953.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp953;});
_tmp952;});break;case 409: _LL399: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp954=
_cycalloc(sizeof(*_tmp954));_tmp954[0]=({struct Cyc_Exp_tok_struct _tmp955;_tmp955.tag=
Cyc_Exp_tok;_tmp955.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp955;});
_tmp954;});break;case 410: _LL39A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp956=
_cycalloc(sizeof(*_tmp956));_tmp956[0]=({struct Cyc_Exp_tok_struct _tmp957;_tmp957.tag=
Cyc_Exp_tok;_tmp957.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp957;});
_tmp956;});break;case 411: _LL39B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp958=
_cycalloc(sizeof(*_tmp958));_tmp958[0]=({struct Cyc_Exp_tok_struct _tmp959;_tmp959.tag=
Cyc_Exp_tok;_tmp959.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp959;});
_tmp958;});break;case 412: _LL39C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL39D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp95A=_cycalloc(sizeof(*_tmp95A));_tmp95A[0]=({struct Cyc_Exp_tok_struct _tmp95B;
_tmp95B.tag=Cyc_Exp_tok;_tmp95B.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95B;});
_tmp95A;});break;case 414: _LL39E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp95C=
_cycalloc(sizeof(*_tmp95C));_tmp95C[0]=({struct Cyc_Exp_tok_struct _tmp95D;_tmp95D.tag=
Cyc_Exp_tok;_tmp95D.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95D;});
_tmp95C;});break;case 415: _LL39F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3A0: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp95E=_cycalloc(sizeof(*_tmp95E));_tmp95E[0]=({struct Cyc_Exp_tok_struct _tmp95F;
_tmp95F.tag=Cyc_Exp_tok;_tmp95F.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp95F;});
_tmp95E;});break;case 417: _LL3A1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp960=
_cycalloc(sizeof(*_tmp960));_tmp960[0]=({struct Cyc_Exp_tok_struct _tmp961;_tmp961.tag=
Cyc_Exp_tok;_tmp961.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp961;});
_tmp960;});break;case 418: _LL3A2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3A3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962[0]=({struct Cyc_Exp_tok_struct _tmp963;
_tmp963.tag=Cyc_Exp_tok;_tmp963.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp963;});
_tmp962;});break;case 420: _LL3A4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp964=
_cycalloc(sizeof(*_tmp964));_tmp964[0]=({struct Cyc_Exp_tok_struct _tmp965;_tmp965.tag=
Cyc_Exp_tok;_tmp965.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp965;});
_tmp964;});break;case 421: _LL3A5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp966=
_cycalloc(sizeof(*_tmp966));_tmp966[0]=({struct Cyc_Exp_tok_struct _tmp967;_tmp967.tag=
Cyc_Exp_tok;_tmp967.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp967;});
_tmp966;});break;case 422: _LL3A6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 423: _LL3A7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp968=_cycalloc(sizeof(*_tmp968));_tmp968[0]=({struct Cyc_Exp_tok_struct _tmp969;
_tmp969.tag=Cyc_Exp_tok;_tmp969.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp969;});
_tmp968;});break;case 424: _LL3A8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 425: _LL3A9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp96A=_cycalloc(sizeof(*_tmp96A));_tmp96A[0]=({struct Cyc_Exp_tok_struct _tmp96B;
_tmp96B.tag=Cyc_Exp_tok;_tmp96B.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96B;});
_tmp96A;});break;case 426: _LL3AA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96C=
_cycalloc(sizeof(*_tmp96C));_tmp96C[0]=({struct Cyc_Exp_tok_struct _tmp96D;_tmp96D.tag=
Cyc_Exp_tok;_tmp96D.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96D;});
_tmp96C;});break;case 427: _LL3AB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96E=
_cycalloc(sizeof(*_tmp96E));_tmp96E[0]=({struct Cyc_Exp_tok_struct _tmp96F;_tmp96F.tag=
Cyc_Exp_tok;_tmp96F.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96F;});
_tmp96E;});break;case 428: _LL3AC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp970=
_cycalloc(sizeof(*_tmp970));_tmp970[0]=({struct Cyc_Exp_tok_struct _tmp971;_tmp971.tag=
Cyc_Exp_tok;_tmp971.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp971;});
_tmp970;});break;case 429: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 430: _LL3AE: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp972=_cycalloc(sizeof(*_tmp972));_tmp972[0]=({struct Cyc_Exp_tok_struct _tmp973;
_tmp973.tag=Cyc_Exp_tok;_tmp973.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp973;});
_tmp972;});break;case 431: _LL3AF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp974=
_cycalloc(sizeof(*_tmp974));_tmp974[0]=({struct Cyc_Exp_tok_struct _tmp975;_tmp975.tag=
Cyc_Exp_tok;_tmp975.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp975;});
_tmp974;});break;case 432: _LL3B0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp976=
_cycalloc(sizeof(*_tmp976));_tmp976[0]=({struct Cyc_Exp_tok_struct _tmp977;_tmp977.tag=
Cyc_Exp_tok;_tmp977.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp977;});
_tmp976;});break;case 433: _LL3B1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp978=
_cycalloc(sizeof(*_tmp978));_tmp978[0]=({struct Cyc_Exp_tok_struct _tmp979;_tmp979.tag=
Cyc_Exp_tok;_tmp979.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp97A=_cycalloc(sizeof(*_tmp97A));_tmp97A[0]=({struct Cyc_Absyn_StructField_struct
_tmp97B;_tmp97B.tag=0;_tmp97B.f1=({struct _tagged_arr*_tmp97C=_cycalloc(sizeof(*
_tmp97C));_tmp97C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp97C;});_tmp97B;});_tmp97A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp979;});
_tmp978;});break;case 434: _LL3B2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp97D=
_cycalloc(sizeof(*_tmp97D));_tmp97D[0]=({struct Cyc_Exp_tok_struct _tmp97E;_tmp97E.tag=
Cyc_Exp_tok;_tmp97E.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp97F=_cycalloc_atomic(sizeof(*_tmp97F));_tmp97F[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp980;_tmp980.tag=1;_tmp980.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp980;});_tmp97F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp97E;});_tmp97D;});break;case 435: _LL3B3: {struct
Cyc_Position_Segment*_tmp981=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp982=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp981,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp983=_cycalloc(
sizeof(*_tmp983));_tmp983[0]=({struct Cyc_Exp_tok_struct _tmp984;_tmp984.tag=Cyc_Exp_tok;
_tmp984.f1=Cyc_Absyn_gentyp_exp(_tmp982,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp984;});
_tmp983;});break;}case 436: _LL3B4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp985=
_cycalloc(sizeof(*_tmp985));_tmp985[0]=({struct Cyc_Exp_tok_struct _tmp986;_tmp986.tag=
Cyc_Exp_tok;_tmp986.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp987=_cycalloc(sizeof(*_tmp987));_tmp987[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp988;_tmp988.tag=35;_tmp988.f1=({struct Cyc_Absyn_MallocInfo _tmp989;_tmp989.is_calloc=
0;_tmp989.rgn=0;_tmp989.elt_type=0;_tmp989.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp989.fat_result=0;
_tmp989;});_tmp988;});_tmp987;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp986;});
_tmp985;});break;case 437: _LL3B5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98A=
_cycalloc(sizeof(*_tmp98A));_tmp98A[0]=({struct Cyc_Exp_tok_struct _tmp98B;_tmp98B.tag=
Cyc_Exp_tok;_tmp98B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp98C=_cycalloc(sizeof(*_tmp98C));_tmp98C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp98D;_tmp98D.tag=35;_tmp98D.f1=({struct Cyc_Absyn_MallocInfo _tmp98E;_tmp98E.is_calloc=
0;_tmp98E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp98E.elt_type=0;
_tmp98E.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp98E.fat_result=0;_tmp98E;});_tmp98D;});_tmp98C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98B;});
_tmp98A;});break;case 438: _LL3B6: {void*_tmp990;struct _tuple2 _tmp98F=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp990=_tmp98F.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp991=_cycalloc(sizeof(*_tmp991));
_tmp991[0]=({struct Cyc_Exp_tok_struct _tmp992;_tmp992.tag=Cyc_Exp_tok;_tmp992.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp993=_cycalloc(
sizeof(*_tmp993));_tmp993[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp994;_tmp994.tag=
35;_tmp994.f1=({struct Cyc_Absyn_MallocInfo _tmp995;_tmp995.is_calloc=1;_tmp995.rgn=
0;_tmp995.elt_type=({void**_tmp996=_cycalloc(sizeof(*_tmp996));_tmp996[0]=
_tmp990;_tmp996;});_tmp995.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp995.fat_result=0;
_tmp995;});_tmp994;});_tmp993;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp992;});
_tmp991;});break;}case 439: _LL3B7: {void*_tmp998;struct _tuple2 _tmp997=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp998=_tmp997.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp999=_cycalloc(sizeof(*_tmp999));
_tmp999[0]=({struct Cyc_Exp_tok_struct _tmp99A;_tmp99A.tag=Cyc_Exp_tok;_tmp99A.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp99B=_cycalloc(
sizeof(*_tmp99B));_tmp99B[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp99C;_tmp99C.tag=
35;_tmp99C.f1=({struct Cyc_Absyn_MallocInfo _tmp99D;_tmp99D.is_calloc=1;_tmp99D.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp99D.elt_type=({void**_tmp99E=_cycalloc(sizeof(*_tmp99E));
_tmp99E[0]=_tmp998;_tmp99E;});_tmp99D.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp99D.fat_result=0;
_tmp99D;});_tmp99C;});_tmp99B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99A;});
_tmp999;});break;}case 440: _LL3B8: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp99F=_cycalloc(sizeof(*_tmp99F));_tmp99F[0]=({struct Cyc_Primop_tok_struct
_tmp9A0;_tmp9A0.tag=Cyc_Primop_tok;_tmp9A0.f1=(void*)((void*)12);_tmp9A0;});
_tmp99F;});break;case 441: _LL3B9: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9A1=_cycalloc(sizeof(*_tmp9A1));_tmp9A1[0]=({struct Cyc_Primop_tok_struct
_tmp9A2;_tmp9A2.tag=Cyc_Primop_tok;_tmp9A2.f1=(void*)((void*)11);_tmp9A2;});
_tmp9A1;});break;case 442: _LL3BA: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9A3=_cycalloc(sizeof(*_tmp9A3));_tmp9A3[0]=({struct Cyc_Primop_tok_struct
_tmp9A4;_tmp9A4.tag=Cyc_Primop_tok;_tmp9A4.f1=(void*)((void*)2);_tmp9A4;});
_tmp9A3;});break;case 443: _LL3BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3BC: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9A5=_cycalloc(sizeof(*_tmp9A5));_tmp9A5[0]=({struct Cyc_Exp_tok_struct _tmp9A6;
_tmp9A6.tag=Cyc_Exp_tok;_tmp9A6.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A6;});
_tmp9A5;});break;case 445: _LL3BD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A7=
_cycalloc(sizeof(*_tmp9A7));_tmp9A7[0]=({struct Cyc_Exp_tok_struct _tmp9A8;_tmp9A8.tag=
Cyc_Exp_tok;_tmp9A8.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A8;});
_tmp9A7;});break;case 446: _LL3BE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A9=
_cycalloc(sizeof(*_tmp9A9));_tmp9A9[0]=({struct Cyc_Exp_tok_struct _tmp9AA;_tmp9AA.tag=
Cyc_Exp_tok;_tmp9AA.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AA;});
_tmp9A9;});break;case 447: _LL3BF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AB=
_cycalloc(sizeof(*_tmp9AB));_tmp9AB[0]=({struct Cyc_Exp_tok_struct _tmp9AC;_tmp9AC.tag=
Cyc_Exp_tok;_tmp9AC.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9AD=_cycalloc(sizeof(*_tmp9AD));_tmp9AD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9AD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AC;});
_tmp9AB;});break;case 448: _LL3C0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AE=
_cycalloc(sizeof(*_tmp9AE));_tmp9AE[0]=({struct Cyc_Exp_tok_struct _tmp9AF;_tmp9AF.tag=
Cyc_Exp_tok;_tmp9AF.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9B0=_cycalloc(sizeof(*_tmp9B0));_tmp9B0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9B0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AF;});
_tmp9AE;});break;case 449: _LL3C1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B1=
_cycalloc(sizeof(*_tmp9B1));_tmp9B1[0]=({struct Cyc_Exp_tok_struct _tmp9B2;_tmp9B2.tag=
Cyc_Exp_tok;_tmp9B2.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B2;});
_tmp9B1;});break;case 450: _LL3C2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B3=
_cycalloc(sizeof(*_tmp9B3));_tmp9B3[0]=({struct Cyc_Exp_tok_struct _tmp9B4;_tmp9B4.tag=
Cyc_Exp_tok;_tmp9B4.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B4;});
_tmp9B3;});break;case 451: _LL3C3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B5=
_cycalloc(sizeof(*_tmp9B5));_tmp9B5[0]=({struct Cyc_Exp_tok_struct _tmp9B6;_tmp9B6.tag=
Cyc_Exp_tok;_tmp9B6.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp9B7=_cycalloc(sizeof(*_tmp9B7));_tmp9B7[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp9B8;_tmp9B8.tag=27;_tmp9B8.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp9B8.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp9B8;});_tmp9B7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9B6;});_tmp9B5;});break;case 452: _LL3C4: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp9B9=_cycalloc(sizeof(*_tmp9B9));_tmp9B9[0]=({
struct Cyc_Exp_tok_struct _tmp9BA;_tmp9BA.tag=Cyc_Exp_tok;_tmp9BA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp9BB=_cycalloc(sizeof(*_tmp9BB));
_tmp9BB[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp9BC;_tmp9BC.tag=27;_tmp9BC.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp9BC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp9BC;});_tmp9BB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BA;});
_tmp9B9;});break;case 453: _LL3C5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BD=
_cycalloc(sizeof(*_tmp9BD));_tmp9BD[0]=({struct Cyc_Exp_tok_struct _tmp9BE;_tmp9BE.tag=
Cyc_Exp_tok;_tmp9BE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp9BF=_cycalloc(sizeof(*_tmp9BF));_tmp9BF[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp9C0;_tmp9C0.tag=2;_tmp9C0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9C0;});_tmp9BF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BE;});
_tmp9BD;});break;case 454: _LL3C6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 455: _LL3C7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9C1=_cycalloc(sizeof(*_tmp9C1));_tmp9C1[0]=({struct Cyc_Exp_tok_struct _tmp9C2;
_tmp9C2.tag=Cyc_Exp_tok;_tmp9C2.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C2;});
_tmp9C1;});break;case 456: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 457: _LL3C9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9C3=_cycalloc(sizeof(*_tmp9C3));_tmp9C3[0]=({struct Cyc_Exp_tok_struct _tmp9C4;
_tmp9C4.tag=Cyc_Exp_tok;_tmp9C4.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C4;});
_tmp9C3;});break;case 458: _LL3CA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C5=
_cycalloc(sizeof(*_tmp9C5));_tmp9C5[0]=({struct Cyc_Exp_tok_struct _tmp9C6;_tmp9C6.tag=
Cyc_Exp_tok;_tmp9C6.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C6;});
_tmp9C5;});break;case 459: _LL3CB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C7=
_cycalloc(sizeof(*_tmp9C7));_tmp9C7[0]=({struct Cyc_Exp_tok_struct _tmp9C8;_tmp9C8.tag=
Cyc_Exp_tok;_tmp9C8.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C8;});
_tmp9C7;});break;case 460: _LL3CC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C9=
_cycalloc(sizeof(*_tmp9C9));_tmp9C9[0]=({struct Cyc_Exp_tok_struct _tmp9CA;_tmp9CA.tag=
Cyc_Exp_tok;_tmp9CA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp9CB=_cycalloc(sizeof(*_tmp9CB));_tmp9CB[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp9CC;_tmp9CC.tag=30;_tmp9CC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp9CC.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9CC.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp9CC.f4=0;
_tmp9CC;});_tmp9CB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9CA;});
_tmp9C9;});break;case 461: _LL3CD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9CD=
_cycalloc(sizeof(*_tmp9CD));_tmp9CD[0]=({struct Cyc_Exp_tok_struct _tmp9CE;_tmp9CE.tag=
Cyc_Exp_tok;_tmp9CE.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9CE;});
_tmp9CD;});break;case 462: _LL3CE: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp9CF=_cycalloc(sizeof(*_tmp9CF));_tmp9CF[0]=({struct Cyc_ExpList_tok_struct
_tmp9D0;_tmp9D0.tag=Cyc_ExpList_tok;_tmp9D0.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp9D0;});_tmp9CF;});
break;case 463: _LL3CF: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp9D1=
_cycalloc(sizeof(*_tmp9D1));_tmp9D1[0]=({struct Cyc_ExpList_tok_struct _tmp9D2;
_tmp9D2.tag=Cyc_ExpList_tok;_tmp9D2.f1=({struct Cyc_List_List*_tmp9D3=_cycalloc(
sizeof(*_tmp9D3));_tmp9D3->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9D3->tl=0;_tmp9D3;});
_tmp9D2;});_tmp9D1;});break;case 464: _LL3D0: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp9D4=_cycalloc(sizeof(*_tmp9D4));_tmp9D4[0]=({struct Cyc_ExpList_tok_struct
_tmp9D5;_tmp9D5.tag=Cyc_ExpList_tok;_tmp9D5.f1=({struct Cyc_List_List*_tmp9D6=
_cycalloc(sizeof(*_tmp9D6));_tmp9D6->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9D6->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9D6;});_tmp9D5;});
_tmp9D4;});break;case 465: _LL3D1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D7=
_cycalloc(sizeof(*_tmp9D7));_tmp9D7[0]=({struct Cyc_Exp_tok_struct _tmp9D8;_tmp9D8.tag=
Cyc_Exp_tok;_tmp9D8.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D8;});
_tmp9D7;});break;case 466: _LL3D2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D9=
_cycalloc(sizeof(*_tmp9D9));_tmp9D9[0]=({struct Cyc_Exp_tok_struct _tmp9DA;_tmp9DA.tag=
Cyc_Exp_tok;_tmp9DA.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DA;});
_tmp9D9;});break;case 467: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DB=
_cycalloc(sizeof(*_tmp9DB));_tmp9DB[0]=({struct Cyc_Exp_tok_struct _tmp9DC;_tmp9DC.tag=
Cyc_Exp_tok;_tmp9DC.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DC;});
_tmp9DB;});break;case 468: _LL3D4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DD=
_cycalloc(sizeof(*_tmp9DD));_tmp9DD[0]=({struct Cyc_Exp_tok_struct _tmp9DE;_tmp9DE.tag=
Cyc_Exp_tok;_tmp9DE.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DE;});
_tmp9DD;});break;case 469: _LL3D5: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp9DF=_cycalloc(sizeof(*_tmp9DF));_tmp9DF[0]=({struct Cyc_QualId_tok_struct
_tmp9E0;_tmp9E0.tag=Cyc_QualId_tok;_tmp9E0.f1=({struct _tuple1*_tmp9E1=_cycalloc(
sizeof(*_tmp9E1));_tmp9E1->f1=Cyc_Absyn_rel_ns_null;_tmp9E1->f2=({struct
_tagged_arr*_tmp9E2=_cycalloc(sizeof(*_tmp9E2));_tmp9E2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9E2;});_tmp9E1;});
_tmp9E0;});_tmp9DF;});break;case 470: _LL3D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 471: _LL3D7: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmp9E3=_cycalloc(sizeof(*_tmp9E3));_tmp9E3[
0]=({struct Cyc_QualId_tok_struct _tmp9E4;_tmp9E4.tag=Cyc_QualId_tok;_tmp9E4.f1=({
struct _tuple1*_tmp9E5=_cycalloc(sizeof(*_tmp9E5));_tmp9E5->f1=Cyc_Absyn_rel_ns_null;
_tmp9E5->f2=({struct _tagged_arr*_tmp9E6=_cycalloc(sizeof(*_tmp9E6));_tmp9E6[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp9E6;});_tmp9E5;});_tmp9E4;});_tmp9E3;});break;case 472: _LL3D8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 473: _LL3D9: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 474: _LL3DA:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475:
_LL3DB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
476: _LL3DC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 477: _LL3DD: break;case 478: _LL3DE:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL3DF: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(479,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(137,yyn - 129)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5868) && Cyc_yycheck[_check_known_subscript_notnull(5869,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5869,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(137,yyn - 129)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(953,yystate)];if(yyn > - 32768  && yyn < 5868){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 266 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5869,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(266,x)])
+ 15,count ++);}msg=({unsigned int _tmp9E7=(unsigned int)(sze + 15);char*_tmp9E8=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp9E7 + 1));struct _tagged_arr
_tmp9EA=_tag_arr(_tmp9E8,sizeof(char),_tmp9E7 + 1);{unsigned int _tmp9E9=_tmp9E7;
unsigned int i;for(i=0;i < _tmp9E9;i ++){_tmp9E8[i]='\000';}_tmp9E8[_tmp9E9]=(char)
0;}_tmp9EA;});Cyc_strcpy(msg,({const char*_tmp9EB="parse error";_tag_arr(_tmp9EB,
sizeof(char),_get_zero_arr_size(_tmp9EB,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 266 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5869,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmp9EC=", expecting `";
_tag_arr(_tmp9EC,sizeof(char),_get_zero_arr_size(_tmp9EC,14));}):(struct
_tagged_arr)({const char*_tmp9ED=" or `";_tag_arr(_tmp9ED,sizeof(char),
_get_zero_arr_size(_tmp9ED,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(266,x)]);Cyc_strcat(msg,({const char*_tmp9EE="'";
_tag_arr(_tmp9EE,sizeof(char),_get_zero_arr_size(_tmp9EE,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmp9EF="parse error";
_tag_arr(_tmp9EF,sizeof(char),_get_zero_arr_size(_tmp9EF,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(953,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 5868) || Cyc_yycheck[_check_known_subscript_notnull(
5869,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5869,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 952)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp9F9=v;struct _tuple13*_tmp9FA;
struct _tuple13 _tmp9FB;int _tmp9FC;char _tmp9FD;short _tmp9FE;struct _tagged_arr
_tmp9FF;struct Cyc_Core_Opt*_tmpA00;struct Cyc_Core_Opt*_tmpA01;struct Cyc_Core_Opt
_tmpA02;struct _tagged_arr*_tmpA03;struct _tuple1*_tmpA04;struct _tuple1 _tmpA05;
void*_tmpA06;struct _tagged_arr*_tmpA07;_LL3E2: if(*((void**)_tmp9F9)!= Cyc_Int_tok)
goto _LL3E4;_tmp9FA=((struct Cyc_Int_tok_struct*)_tmp9F9)->f1;_tmp9FB=*_tmp9FA;
_tmp9FC=_tmp9FB.f2;_LL3E3:({struct Cyc_Int_pa_struct _tmpA0A;_tmpA0A.tag=1;_tmpA0A.f1=(
unsigned int)_tmp9FC;{void*_tmpA08[1]={& _tmpA0A};Cyc_fprintf(Cyc_stderr,({const
char*_tmpA09="%d";_tag_arr(_tmpA09,sizeof(char),_get_zero_arr_size(_tmpA09,3));}),
_tag_arr(_tmpA08,sizeof(void*),1));}});goto _LL3E1;_LL3E4: if(*((void**)_tmp9F9)!= 
Cyc_Char_tok)goto _LL3E6;_tmp9FD=((struct Cyc_Char_tok_struct*)_tmp9F9)->f1;_LL3E5:({
struct Cyc_Int_pa_struct _tmpA0D;_tmpA0D.tag=1;_tmpA0D.f1=(unsigned int)((int)
_tmp9FD);{void*_tmpA0B[1]={& _tmpA0D};Cyc_fprintf(Cyc_stderr,({const char*_tmpA0C="%c";
_tag_arr(_tmpA0C,sizeof(char),_get_zero_arr_size(_tmpA0C,3));}),_tag_arr(_tmpA0B,
sizeof(void*),1));}});goto _LL3E1;_LL3E6: if(*((void**)_tmp9F9)!= Cyc_Short_tok)
goto _LL3E8;_tmp9FE=((struct Cyc_Short_tok_struct*)_tmp9F9)->f1;_LL3E7:({struct Cyc_Int_pa_struct
_tmpA10;_tmpA10.tag=1;_tmpA10.f1=(unsigned int)((int)_tmp9FE);{void*_tmpA0E[1]={&
_tmpA10};Cyc_fprintf(Cyc_stderr,({const char*_tmpA0F="%ds";_tag_arr(_tmpA0F,
sizeof(char),_get_zero_arr_size(_tmpA0F,4));}),_tag_arr(_tmpA0E,sizeof(void*),1));}});
goto _LL3E1;_LL3E8: if(*((void**)_tmp9F9)!= Cyc_String_tok)goto _LL3EA;_tmp9FF=((
struct Cyc_String_tok_struct*)_tmp9F9)->f1;_LL3E9:({struct Cyc_String_pa_struct
_tmpA13;_tmpA13.tag=0;_tmpA13.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp9FF);{
void*_tmpA11[1]={& _tmpA13};Cyc_fprintf(Cyc_stderr,({const char*_tmpA12="\"%s\"";
_tag_arr(_tmpA12,sizeof(char),_get_zero_arr_size(_tmpA12,5));}),_tag_arr(_tmpA11,
sizeof(void*),1));}});goto _LL3E1;_LL3EA: if(*((void**)_tmp9F9)!= Cyc_Stringopt_tok)
goto _LL3EC;_tmpA00=((struct Cyc_Stringopt_tok_struct*)_tmp9F9)->f1;if(_tmpA00 != 0)
goto _LL3EC;_LL3EB:({void*_tmpA14[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpA15="NULL";_tag_arr(_tmpA15,sizeof(char),_get_zero_arr_size(_tmpA15,5));}),
_tag_arr(_tmpA14,sizeof(void*),0));});goto _LL3E1;_LL3EC: if(*((void**)_tmp9F9)!= 
Cyc_Stringopt_tok)goto _LL3EE;_tmpA01=((struct Cyc_Stringopt_tok_struct*)_tmp9F9)->f1;
if(_tmpA01 == 0)goto _LL3EE;_tmpA02=*_tmpA01;_tmpA03=(struct _tagged_arr*)_tmpA02.v;
_LL3ED:({struct Cyc_String_pa_struct _tmpA18;_tmpA18.tag=0;_tmpA18.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmpA03);{void*_tmpA16[1]={& _tmpA18};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA17="\"%s\"";_tag_arr(_tmpA17,sizeof(char),
_get_zero_arr_size(_tmpA17,5));}),_tag_arr(_tmpA16,sizeof(void*),1));}});goto
_LL3E1;_LL3EE: if(*((void**)_tmp9F9)!= Cyc_QualId_tok)goto _LL3F0;_tmpA04=((struct
Cyc_QualId_tok_struct*)_tmp9F9)->f1;_tmpA05=*_tmpA04;_tmpA06=_tmpA05.f1;_tmpA07=
_tmpA05.f2;_LL3EF: {struct Cyc_List_List*_tmpA19=0;{void*_tmpA1A=_tmpA06;struct
Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1C;_LL3F3: if(_tmpA1A <= (void*)1
 || *((int*)_tmpA1A)!= 0)goto _LL3F5;_tmpA1B=((struct Cyc_Absyn_Rel_n_struct*)
_tmpA1A)->f1;_LL3F4: _tmpA19=_tmpA1B;goto _LL3F2;_LL3F5: if(_tmpA1A <= (void*)1  || *((
int*)_tmpA1A)!= 1)goto _LL3F7;_tmpA1C=((struct Cyc_Absyn_Abs_n_struct*)_tmpA1A)->f1;
_LL3F6: _tmpA19=_tmpA1C;goto _LL3F2;_LL3F7: if((int)_tmpA1A != 0)goto _LL3F2;_LL3F8:
goto _LL3F2;_LL3F2:;}for(0;_tmpA19 != 0;_tmpA19=_tmpA19->tl){({struct Cyc_String_pa_struct
_tmpA1F;_tmpA1F.tag=0;_tmpA1F.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmpA19->hd));{void*_tmpA1D[1]={& _tmpA1F};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA1E="%s::";_tag_arr(_tmpA1E,sizeof(char),_get_zero_arr_size(
_tmpA1E,5));}),_tag_arr(_tmpA1D,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmpA22;_tmpA22.tag=0;_tmpA22.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmpA07);{
void*_tmpA20[1]={& _tmpA22};Cyc_fprintf(Cyc_stderr,({const char*_tmpA21="%s::";
_tag_arr(_tmpA21,sizeof(char),_get_zero_arr_size(_tmpA21,5));}),_tag_arr(_tmpA20,
sizeof(void*),1));}});goto _LL3E1;}_LL3F0:;_LL3F1:({void*_tmpA23[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA24="?";_tag_arr(_tmpA24,sizeof(char),
_get_zero_arr_size(_tmpA24,2));}),_tag_arr(_tmpA23,sizeof(void*),0));});goto
_LL3E1;_LL3E1:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmpA25=_cycalloc(
sizeof(*_tmpA25));_tmpA25->v=Cyc_Lexing_from_file(f);_tmpA25;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
