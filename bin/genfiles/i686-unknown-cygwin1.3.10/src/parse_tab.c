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
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_zstrcmp(struct _tagged_arr,struct
_tagged_arr);int Cyc_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct
_tagged_arr Cyc_strcat(struct _tagged_arr dest,struct _tagged_arr src);struct
_tagged_arr Cyc_strcpy(struct _tagged_arr dest,struct _tagged_arr src);struct
_tagged_arr Cyc_substring(struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _tagged_arr);extern
char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*nullable;struct
Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;
struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*
tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct _tuple1*
name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo
f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct _tuple1*
field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int tag;
struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
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
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct
Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
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
_tagged_arr _tmp177;struct _tuple1*_tmp178;struct Cyc_List_List*_tmp179;int _tmp17A;
struct Cyc_Absyn_Exp*_tmp17B;_LLBA: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 12)
goto _LLBC;_tmp171=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp170)->f1;_LLBB:
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp17C=
_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp17D;_tmp17D.tag=2;_tmp17D.f1=_tmp171;_tmp17D;});_tmp17C;}),p->loc);_LLBC: if(
_tmp170 <= (void*)2  || *((int*)_tmp170)!= 1)goto _LLBE;_tmp172=((struct Cyc_Absyn_Reference_p_struct*)
_tmp170)->f1;_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp17F;_tmp17F.tag=2;_tmp17F.f1=_tmp172->name;
_tmp17F;});_tmp17E;}),p->loc),p->loc);_LLBE: if(_tmp170 <= (void*)2  || *((int*)
_tmp170)!= 4)goto _LLC0;_tmp173=((struct Cyc_Absyn_Pointer_p_struct*)_tmp170)->f1;
_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp173),p->loc);_LLC0: if((
int)_tmp170 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(p->loc);_LLC2: if(
_tmp170 <= (void*)2  || *((int*)_tmp170)!= 7)goto _LLC4;_tmp174=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp170)->f1;_tmp175=((struct Cyc_Absyn_Int_p_struct*)_tmp170)->f2;_LLC3: return
Cyc_Absyn_int_exp(_tmp174,_tmp175,p->loc);_LLC4: if(_tmp170 <= (void*)2  || *((int*)
_tmp170)!= 8)goto _LLC6;_tmp176=((struct Cyc_Absyn_Char_p_struct*)_tmp170)->f1;
_LLC5: return Cyc_Absyn_char_exp(_tmp176,p->loc);_LLC6: if(_tmp170 <= (void*)2  || *((
int*)_tmp170)!= 9)goto _LLC8;_tmp177=((struct Cyc_Absyn_Float_p_struct*)_tmp170)->f1;
_LLC7: return Cyc_Absyn_float_exp(_tmp177,p->loc);_LLC8: if(_tmp170 <= (void*)2  || *((
int*)_tmp170)!= 13)goto _LLCA;_tmp178=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp170)->f1;_tmp179=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp170)->f2;
_tmp17A=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp170)->f3;if(_tmp17A != 0)goto
_LLCA;_LLC9: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp181;_tmp181.tag=2;_tmp181.f1=_tmp178;_tmp181;});_tmp180;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp179);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(_tmp170 <= (void*)2  || *((int*)_tmp170)!= 14)goto _LLCC;
_tmp17B=((struct Cyc_Absyn_Exp_p_struct*)_tmp170)->f1;_LLCB: return _tmp17B;_LLCC:;
_LLCD: Cyc_Parse_err(({const char*_tmp182="cannot mix patterns and expressions in case";
_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,44));}),p->loc);return
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
struct _tuple17{struct Cyc_List_List*f1;int f2;};struct Cyc_YY3_struct{char*tag;
struct _tuple17*f1;};char Cyc_YY4[8]="\000\000\000\000YY4\000";struct Cyc_YY4_struct{
char*tag;struct _tuple17*f1;};char Cyc_YY5[8]="\000\000\000\000YY5\000";struct Cyc_YY5_struct{
char*tag;struct _tuple14*f1;};char Cyc_YY6[8]="\000\000\000\000YY6\000";struct Cyc_YY6_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY7[8]="\000\000\000\000YY7\000";
struct Cyc_YY7_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL\000";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";static char _tmp184[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp184,
_tmp184,_tmp184 + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp185=yy1;struct _tuple13*_tmp186;_LLCF: if(*((void**)_tmp185)!= Cyc_Int_tok)
goto _LLD1;_tmp186=((struct Cyc_Int_tok_struct*)_tmp185)->f1;_LLD0: yyzzz=_tmp186;
goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLCE:;}return
yyzzz;}static char _tmp188[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp188,_tmp188,_tmp188 + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp189=yy1;char _tmp18A;_LLD4: if(*((void**)_tmp189)!= Cyc_Char_tok)
goto _LLD6;_tmp18A=((struct Cyc_Char_tok_struct*)_tmp189)->f1;_LLD5: yyzzz=_tmp18A;
goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return
yyzzz;}static char _tmp18C[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp18C,_tmp18C,_tmp18C + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp18D=yy1;struct _tagged_arr _tmp18E;
_LLD9: if(*((void**)_tmp18D)!= Cyc_String_tok)goto _LLDB;_tmp18E=((struct Cyc_String_tok_struct*)
_tmp18D)->f1;_LLDA: yyzzz=_tmp18E;goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLD8:;}return yyzzz;}static char _tmp190[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp190,
_tmp190,_tmp190 + 54}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*
yyzzz;{void*_tmp191=yy1;struct _tuple16*_tmp192;_LLDE: if(*((void**)_tmp191)!= Cyc_YY1)
goto _LLE0;_tmp192=((struct Cyc_YY1_struct*)_tmp191)->f1;_LLDF: yyzzz=_tmp192;goto
_LLDD;_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}
static char _tmp194[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp194,_tmp194,_tmp194 + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp195=yy1;struct Cyc_Absyn_Conref*
_tmp196;_LLE3: if(*((void**)_tmp195)!= Cyc_YY2)goto _LLE5;_tmp196=((struct Cyc_YY2_struct*)
_tmp195)->f1;_LLE4: yyzzz=_tmp196;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLE2:;}return yyzzz;}static char _tmp198[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp199=yy1;struct
Cyc_Absyn_Exp*_tmp19A;_LLE8: if(*((void**)_tmp199)!= Cyc_Exp_tok)goto _LLEA;
_tmp19A=((struct Cyc_Exp_tok_struct*)_tmp199)->f1;_LLE9: yyzzz=_tmp19A;goto _LLE7;
_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLE7:;}return yyzzz;}static
char _tmp19C[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp19C,_tmp19C,_tmp19C + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19D=yy1;struct Cyc_List_List*
_tmp19E;_LLED: if(*((void**)_tmp19D)!= Cyc_ExpList_tok)goto _LLEF;_tmp19E=((struct
Cyc_ExpList_tok_struct*)_tmp19D)->f1;_LLEE: yyzzz=_tmp19E;goto _LLEC;_LLEF:;_LLF0:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLEC:;}return yyzzz;}static char
_tmp1A0[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp1A0,_tmp1A0,_tmp1A0 + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1A1=yy1;struct Cyc_List_List*_tmp1A2;_LLF2: if(*((void**)_tmp1A1)
!= Cyc_InitializerList_tok)goto _LLF4;_tmp1A2=((struct Cyc_InitializerList_tok_struct*)
_tmp1A1)->f1;_LLF3: yyzzz=_tmp1A2;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLF1:;}return yyzzz;}static char _tmp1A4[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp1A4,_tmp1A4,_tmp1A4 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp1A5=yy1;void*_tmp1A6;_LLF7: if(*((void**)_tmp1A5)
!= Cyc_Primop_tok)goto _LLF9;_tmp1A6=(void*)((struct Cyc_Primop_tok_struct*)
_tmp1A5)->f1;_LLF8: yyzzz=_tmp1A6;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLF6:;}return yyzzz;}static char _tmp1A8[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8,_tmp1A8 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp1A9=yy1;struct Cyc_Core_Opt*_tmp1AA;_LLFC: if(*((void**)_tmp1A9)!= Cyc_Primopopt_tok)
goto _LLFE;_tmp1AA=((struct Cyc_Primopopt_tok_struct*)_tmp1A9)->f1;_LLFD: yyzzz=
_tmp1AA;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLFB:;}return yyzzz;}static char _tmp1AC[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1AC,_tmp1AC,_tmp1AC + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp1AD=yy1;
struct _tuple1*_tmp1AE;_LL101: if(*((void**)_tmp1AD)!= Cyc_QualId_tok)goto _LL103;
_tmp1AE=((struct Cyc_QualId_tok_struct*)_tmp1AD)->f1;_LL102: yyzzz=_tmp1AE;goto
_LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL100:;}return
yyzzz;}static char _tmp1B0[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1B1=yy1;struct Cyc_Absyn_Stmt*
_tmp1B2;_LL106: if(*((void**)_tmp1B1)!= Cyc_Stmt_tok)goto _LL108;_tmp1B2=((struct
Cyc_Stmt_tok_struct*)_tmp1B1)->f1;_LL107: yyzzz=_tmp1B2;goto _LL105;_LL108:;_LL109:(
int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL105:;}return yyzzz;}static char _tmp1B4[
24]="list_t<switch_clause_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{_tmp1B4,_tmp1B4,_tmp1B4 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B5=yy1;struct Cyc_List_List*
_tmp1B6;_LL10B: if(*((void**)_tmp1B5)!= Cyc_SwitchClauseList_tok)goto _LL10D;
_tmp1B6=((struct Cyc_SwitchClauseList_tok_struct*)_tmp1B5)->f1;_LL10C: yyzzz=
_tmp1B6;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL10A:;}return yyzzz;}static char _tmp1B8[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1B8,_tmp1B8,_tmp1B8 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1B9=yy1;
struct Cyc_Absyn_Pat*_tmp1BA;_LL110: if(*((void**)_tmp1B9)!= Cyc_Pattern_tok)goto
_LL112;_tmp1BA=((struct Cyc_Pattern_tok_struct*)_tmp1B9)->f1;_LL111: yyzzz=_tmp1BA;
goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL10F:;}
return yyzzz;}static char _tmp1BC[23]="$(list_t<pat_t>,bool)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 23}};struct _tuple17*
Cyc_yyget_YY3(void*yy1){struct _tuple17*yyzzz;{void*_tmp1BD=yy1;struct _tuple17*
_tmp1BE;_LL115: if(*((void**)_tmp1BD)!= Cyc_YY3)goto _LL117;_tmp1BE=((struct Cyc_YY3_struct*)
_tmp1BD)->f1;_LL116: yyzzz=_tmp1BE;goto _LL114;_LL117:;_LL118:(int)_throw((void*)&
Cyc_yyfail_YY3);_LL114:;}return yyzzz;}static char _tmp1C0[14]="list_t<pat_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{
_tmp1C0,_tmp1C0,_tmp1C0 + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C1=yy1;struct Cyc_List_List*_tmp1C2;
_LL11A: if(*((void**)_tmp1C1)!= Cyc_PatternList_tok)goto _LL11C;_tmp1C2=((struct
Cyc_PatternList_tok_struct*)_tmp1C1)->f1;_LL11B: yyzzz=_tmp1C2;goto _LL119;_LL11C:;
_LL11D:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);_LL119:;}return yyzzz;}
static char _tmp1C4[31]="$(list_t<designator_t>,pat_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{_tmp1C4,_tmp1C4,_tmp1C4 + 31}};
struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){struct _tuple11*yyzzz;{void*
_tmp1C5=yy1;struct _tuple11*_tmp1C6;_LL11F: if(*((void**)_tmp1C5)!= Cyc_FieldPattern_tok)
goto _LL121;_tmp1C6=((struct Cyc_FieldPattern_tok_struct*)_tmp1C5)->f1;_LL120:
yyzzz=_tmp1C6;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);
_LL11E:;}return yyzzz;}static char _tmp1C8[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1C8,_tmp1C8,_tmp1C8 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C9=yy1;struct Cyc_List_List*
_tmp1CA;_LL124: if(*((void**)_tmp1C9)!= Cyc_FieldPatternList_tok)goto _LL126;
_tmp1CA=((struct Cyc_FieldPatternList_tok_struct*)_tmp1C9)->f1;_LL125: yyzzz=
_tmp1CA;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL123:;}return yyzzz;}static char _tmp1CC[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1CC,
_tmp1CC,_tmp1CC + 48}};struct _tuple17*Cyc_yyget_YY4(void*yy1){struct _tuple17*
yyzzz;{void*_tmp1CD=yy1;struct _tuple17*_tmp1CE;_LL129: if(*((void**)_tmp1CD)!= 
Cyc_YY4)goto _LL12B;_tmp1CE=((struct Cyc_YY4_struct*)_tmp1CD)->f1;_LL12A: yyzzz=
_tmp1CE;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY4);_LL128:;}
return yyzzz;}static char _tmp1D0[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1D0,_tmp1D0,_tmp1D0 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1D1=yy1;
struct Cyc_Absyn_Fndecl*_tmp1D2;_LL12E: if(*((void**)_tmp1D1)!= Cyc_FnDecl_tok)
goto _LL130;_tmp1D2=((struct Cyc_FnDecl_tok_struct*)_tmp1D1)->f1;_LL12F: yyzzz=
_tmp1D2;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL12D:;}return yyzzz;}static char _tmp1D4[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1D5=yy1;struct Cyc_List_List*_tmp1D6;_LL133: if(*((void**)_tmp1D5)!= Cyc_DeclList_tok)
goto _LL135;_tmp1D6=((struct Cyc_DeclList_tok_struct*)_tmp1D5)->f1;_LL134: yyzzz=
_tmp1D6;goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL132:;}return yyzzz;}static char _tmp1D8[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1D8,_tmp1D8,_tmp1D8 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1D9=yy1;struct Cyc_Parse_Declaration_spec*_tmp1DA;_LL138: if(*((
void**)_tmp1D9)!= Cyc_DeclSpec_tok)goto _LL13A;_tmp1DA=((struct Cyc_DeclSpec_tok_struct*)
_tmp1D9)->f1;_LL139: yyzzz=_tmp1DA;goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL137:;}return yyzzz;}static char _tmp1DC[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1DC,_tmp1DC,_tmp1DC + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1DD=yy1;struct _tuple12*_tmp1DE;_LL13D: if(*((void**)
_tmp1DD)!= Cyc_InitDecl_tok)goto _LL13F;_tmp1DE=((struct Cyc_InitDecl_tok_struct*)
_tmp1DD)->f1;_LL13E: yyzzz=_tmp1DE;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL13C:;}return yyzzz;}static char _tmp1E0[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1E0,_tmp1E0,_tmp1E0 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E1=yy1;struct Cyc_List_List*
_tmp1E2;_LL142: if(*((void**)_tmp1E1)!= Cyc_InitDeclList_tok)goto _LL144;_tmp1E2=((
struct Cyc_InitDeclList_tok_struct*)_tmp1E1)->f1;_LL143: yyzzz=_tmp1E2;goto _LL141;
_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL141:;}return
yyzzz;}static char _tmp1E4[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1E5=yy1;void*
_tmp1E6;_LL147: if(*((void**)_tmp1E5)!= Cyc_StorageClass_tok)goto _LL149;_tmp1E6=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1E5)->f1;_LL148: yyzzz=_tmp1E6;
goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL146:;}return yyzzz;}static char _tmp1E8[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1E9=yy1;void*
_tmp1EA;_LL14C: if(*((void**)_tmp1E9)!= Cyc_TypeSpecifier_tok)goto _LL14E;_tmp1EA=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1E9)->f1;_LL14D: yyzzz=_tmp1EA;
goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL14B:;}return yyzzz;}static char _tmp1EC[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1ED=yy1;void*
_tmp1EE;_LL151: if(*((void**)_tmp1ED)!= Cyc_StructOrUnion_tok)goto _LL153;_tmp1EE=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1ED)->f1;_LL152: yyzzz=_tmp1EE;
goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL150:;}return yyzzz;}static char _tmp1F0[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1F0,_tmp1F0,_tmp1F0 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1F1=yy1;struct Cyc_Absyn_Tqual _tmp1F2;_LL156: if(*((void**)_tmp1F1)!= Cyc_TypeQual_tok)
goto _LL158;_tmp1F2=((struct Cyc_TypeQual_tok_struct*)_tmp1F1)->f1;_LL157: yyzzz=
_tmp1F2;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL155:;}return yyzzz;}static char _tmp1F4[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1F4,_tmp1F4,_tmp1F4 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F5=yy1;struct Cyc_List_List*
_tmp1F6;_LL15B: if(*((void**)_tmp1F5)!= Cyc_AggrFieldDeclList_tok)goto _LL15D;
_tmp1F6=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1F5)->f1;_LL15C: yyzzz=
_tmp1F6;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL15A:;}return yyzzz;}static char _tmp1F8[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1F8,_tmp1F8,_tmp1F8 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F9=yy1;struct Cyc_List_List*
_tmp1FA;_LL160: if(*((void**)_tmp1F9)!= Cyc_AggrFieldDeclListList_tok)goto _LL162;
_tmp1FA=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1F9)->f1;_LL161: yyzzz=
_tmp1FA;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL15F:;}return yyzzz;}static char _tmp1FC[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1FC,_tmp1FC,_tmp1FC + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FD=yy1;struct Cyc_List_List*
_tmp1FE;_LL165: if(*((void**)_tmp1FD)!= Cyc_TypeModifierList_tok)goto _LL167;
_tmp1FE=((struct Cyc_TypeModifierList_tok_struct*)_tmp1FD)->f1;_LL166: yyzzz=
_tmp1FE;goto _LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL164:;}return yyzzz;}static char _tmp200[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp200,_tmp200,_tmp200 + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp201=yy1;struct Cyc_Parse_Declarator*_tmp202;_LL16A: if(*((void**)
_tmp201)!= Cyc_Declarator_tok)goto _LL16C;_tmp202=((struct Cyc_Declarator_tok_struct*)
_tmp201)->f1;_LL16B: yyzzz=_tmp202;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL169:;}return yyzzz;}static char _tmp204[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp204,_tmp204,_tmp204 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp205=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp206;_LL16F: if(*((void**)_tmp205)!= Cyc_AbstractDeclarator_tok)goto _LL171;
_tmp206=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp205)->f1;_LL170: yyzzz=
_tmp206;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL16E:;}return yyzzz;}static char _tmp208[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp209=yy1;int _tmp20A;_LL174: if(*((void**)_tmp209)!= 
Cyc_Bool_tok)goto _LL176;_tmp20A=((struct Cyc_Bool_tok_struct*)_tmp209)->f1;_LL175:
yyzzz=_tmp20A;goto _LL173;_LL176:;_LL177:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL173:;}return yyzzz;}static char _tmp20C[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp20C,_tmp20C,_tmp20C + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp20D=yy1;void*_tmp20E;_LL179: if(*((void**)_tmp20D)
!= Cyc_Scope_tok)goto _LL17B;_tmp20E=(void*)((struct Cyc_Scope_tok_struct*)_tmp20D)->f1;
_LL17A: yyzzz=_tmp20E;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL178:;}return yyzzz;}static char _tmp210[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp211=yy1;struct Cyc_Absyn_Tunionfield*_tmp212;_LL17E: if(*((void**)
_tmp211)!= Cyc_TunionField_tok)goto _LL180;_tmp212=((struct Cyc_TunionField_tok_struct*)
_tmp211)->f1;_LL17F: yyzzz=_tmp212;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL17D:;}return yyzzz;}static char _tmp214[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp214,_tmp214,_tmp214 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp215=yy1;struct Cyc_List_List*
_tmp216;_LL183: if(*((void**)_tmp215)!= Cyc_TunionFieldList_tok)goto _LL185;
_tmp216=((struct Cyc_TunionFieldList_tok_struct*)_tmp215)->f1;_LL184: yyzzz=
_tmp216;goto _LL182;_LL185:;_LL186:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL182:;}return yyzzz;}static char _tmp218[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp218,_tmp218,_tmp218 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp219=yy1;struct _tuple15*_tmp21A;_LL188: if(*((void**)
_tmp219)!= Cyc_QualSpecList_tok)goto _LL18A;_tmp21A=((struct Cyc_QualSpecList_tok_struct*)
_tmp219)->f1;_LL189: yyzzz=_tmp21A;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL187:;}return yyzzz;}static char _tmp21C[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp21C,_tmp21C,_tmp21C + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp21D=yy1;struct Cyc_List_List*_tmp21E;_LL18D:
if(*((void**)_tmp21D)!= Cyc_IdList_tok)goto _LL18F;_tmp21E=((struct Cyc_IdList_tok_struct*)
_tmp21D)->f1;_LL18E: yyzzz=_tmp21E;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL18C:;}return yyzzz;}static char _tmp220[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp220,_tmp220,_tmp220 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp221=yy1;struct _tuple2*_tmp222;_LL192: if(*((void**)
_tmp221)!= Cyc_ParamDecl_tok)goto _LL194;_tmp222=((struct Cyc_ParamDecl_tok_struct*)
_tmp221)->f1;_LL193: yyzzz=_tmp222;goto _LL191;_LL194:;_LL195:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL191:;}return yyzzz;}static char _tmp224[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp224,_tmp224,_tmp224 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp225=yy1;struct Cyc_List_List*
_tmp226;_LL197: if(*((void**)_tmp225)!= Cyc_ParamDeclList_tok)goto _LL199;_tmp226=((
struct Cyc_ParamDeclList_tok_struct*)_tmp225)->f1;_LL198: yyzzz=_tmp226;goto _LL196;
_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL196:;}return
yyzzz;}static char _tmp228[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp228,
_tmp228,_tmp228 + 107}};struct _tuple14*Cyc_yyget_YY5(void*yy1){struct _tuple14*
yyzzz;{void*_tmp229=yy1;struct _tuple14*_tmp22A;_LL19C: if(*((void**)_tmp229)!= 
Cyc_YY5)goto _LL19E;_tmp22A=((struct Cyc_YY5_struct*)_tmp229)->f1;_LL19D: yyzzz=
_tmp22A;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY5);_LL19B:;}
return yyzzz;}static char _tmp22C[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp22C,_tmp22C,_tmp22C + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp22D=yy1;struct Cyc_List_List*_tmp22E;_LL1A1: if(*((void**)_tmp22D)!= Cyc_TypeList_tok)
goto _LL1A3;_tmp22E=((struct Cyc_TypeList_tok_struct*)_tmp22D)->f1;_LL1A2: yyzzz=
_tmp22E;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL1A0:;}return yyzzz;}static char _tmp230[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp230,
_tmp230,_tmp230 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp231=yy1;struct Cyc_List_List*_tmp232;_LL1A6:
if(*((void**)_tmp231)!= Cyc_DesignatorList_tok)goto _LL1A8;_tmp232=((struct Cyc_DesignatorList_tok_struct*)
_tmp231)->f1;_LL1A7: yyzzz=_tmp232;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL1A5:;}return yyzzz;}static char _tmp234[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp234,_tmp234,_tmp234 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp235=yy1;void*_tmp236;_LL1AB: if(*((void**)_tmp235)!= Cyc_Designator_tok)
goto _LL1AD;_tmp236=(void*)((struct Cyc_Designator_tok_struct*)_tmp235)->f1;_LL1AC:
yyzzz=_tmp236;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL1AA:;}return yyzzz;}static char _tmp238[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp239=yy1;void*_tmp23A;_LL1B0: if(*((void**)_tmp239)
!= Cyc_Kind_tok)goto _LL1B2;_tmp23A=(void*)((struct Cyc_Kind_tok_struct*)_tmp239)->f1;
_LL1B1: yyzzz=_tmp23A;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL1AF:;}return yyzzz;}static char _tmp23C[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp23C,_tmp23C,_tmp23C + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp23D=yy1;void*_tmp23E;_LL1B5: if(*((void**)_tmp23D)
!= Cyc_Type_tok)goto _LL1B7;_tmp23E=(void*)((struct Cyc_Type_tok_struct*)_tmp23D)->f1;
_LL1B6: yyzzz=_tmp23E;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL1B4:;}return yyzzz;}static char _tmp240[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp240,
_tmp240,_tmp240 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp241=yy1;struct Cyc_List_List*_tmp242;_LL1BA:
if(*((void**)_tmp241)!= Cyc_AttributeList_tok)goto _LL1BC;_tmp242=((struct Cyc_AttributeList_tok_struct*)
_tmp241)->f1;_LL1BB: yyzzz=_tmp242;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1B9:;}return yyzzz;}static char _tmp244[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp244,_tmp244,_tmp244 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp245=yy1;void*_tmp246;_LL1BF: if(*((void**)_tmp245)!= Cyc_Attribute_tok)
goto _LL1C1;_tmp246=(void*)((struct Cyc_Attribute_tok_struct*)_tmp245)->f1;_LL1C0:
yyzzz=_tmp246;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1BE:;}return yyzzz;}static char _tmp248[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp249=yy1;struct Cyc_Absyn_Enumfield*_tmp24A;_LL1C4: if(*((void**)
_tmp249)!= Cyc_Enumfield_tok)goto _LL1C6;_tmp24A=((struct Cyc_Enumfield_tok_struct*)
_tmp249)->f1;_LL1C5: yyzzz=_tmp24A;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1C3:;}return yyzzz;}static char _tmp24C[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp24C,_tmp24C,_tmp24C + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp24D=yy1;struct Cyc_List_List*
_tmp24E;_LL1C9: if(*((void**)_tmp24D)!= Cyc_EnumfieldList_tok)goto _LL1CB;_tmp24E=((
struct Cyc_EnumfieldList_tok_struct*)_tmp24D)->f1;_LL1CA: yyzzz=_tmp24E;goto _LL1C8;
_LL1CB:;_LL1CC:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1C8:;}return
yyzzz;}static char _tmp250[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp251=yy1;struct Cyc_Core_Opt*_tmp252;_LL1CE: if(*((void**)_tmp251)!= Cyc_TypeOpt_tok)
goto _LL1D0;_tmp252=((struct Cyc_TypeOpt_tok_struct*)_tmp251)->f1;_LL1CF: yyzzz=
_tmp252;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1CD:;}return yyzzz;}static char _tmp254[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp254,
_tmp254,_tmp254 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp255=yy1;struct Cyc_List_List*_tmp256;_LL1D3:
if(*((void**)_tmp255)!= Cyc_Rgnorder_tok)goto _LL1D5;_tmp256=((struct Cyc_Rgnorder_tok_struct*)
_tmp255)->f1;_LL1D4: yyzzz=_tmp256;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1D2:;}return yyzzz;}static char _tmp258[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp258,
_tmp258,_tmp258 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY6(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp259=yy1;struct Cyc_Absyn_Conref*_tmp25A;_LL1D8: if(*((void**)
_tmp259)!= Cyc_YY6)goto _LL1DA;_tmp25A=((struct Cyc_YY6_struct*)_tmp259)->f1;
_LL1D9: yyzzz=_tmp25A;goto _LL1D7;_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY6);
_LL1D7:;}return yyzzz;}static char _tmp25C[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp25C,
_tmp25C,_tmp25C + 40}};struct Cyc_List_List*Cyc_yyget_YY7(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp25D=yy1;struct Cyc_List_List*_tmp25E;_LL1DD: if(*((void**)_tmp25D)
!= Cyc_YY7)goto _LL1DF;_tmp25E=((struct Cyc_YY7_struct*)_tmp25D)->f1;_LL1DE: yyzzz=
_tmp25E;goto _LL1DC;_LL1DF:;_LL1E0:(int)_throw((void*)& Cyc_yyfail_YY7);_LL1DC:;}
return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp25F;_tmp25F.timestamp=0;_tmp25F.first_line=0;_tmp25F.first_column=0;_tmp25F.last_line=
0;_tmp25F.last_column=0;_tmp25F;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
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
99,100,101};static char _tmp260[2]="$";static char _tmp261[6]="error";static char
_tmp262[12]="$undefined.";static char _tmp263[5]="AUTO";static char _tmp264[9]="REGISTER";
static char _tmp265[7]="STATIC";static char _tmp266[7]="EXTERN";static char _tmp267[8]="TYPEDEF";
static char _tmp268[5]="VOID";static char _tmp269[5]="CHAR";static char _tmp26A[6]="SHORT";
static char _tmp26B[4]="INT";static char _tmp26C[5]="LONG";static char _tmp26D[6]="FLOAT";
static char _tmp26E[7]="DOUBLE";static char _tmp26F[7]="SIGNED";static char _tmp270[9]="UNSIGNED";
static char _tmp271[6]="CONST";static char _tmp272[9]="VOLATILE";static char _tmp273[9]="RESTRICT";
static char _tmp274[7]="STRUCT";static char _tmp275[6]="UNION";static char _tmp276[5]="CASE";
static char _tmp277[8]="DEFAULT";static char _tmp278[7]="INLINE";static char _tmp279[7]="SIZEOF";
static char _tmp27A[9]="OFFSETOF";static char _tmp27B[3]="IF";static char _tmp27C[5]="ELSE";
static char _tmp27D[7]="SWITCH";static char _tmp27E[6]="WHILE";static char _tmp27F[3]="DO";
static char _tmp280[4]="FOR";static char _tmp281[5]="GOTO";static char _tmp282[9]="CONTINUE";
static char _tmp283[6]="BREAK";static char _tmp284[7]="RETURN";static char _tmp285[5]="ENUM";
static char _tmp286[8]="NULL_kw";static char _tmp287[4]="LET";static char _tmp288[6]="THROW";
static char _tmp289[4]="TRY";static char _tmp28A[6]="CATCH";static char _tmp28B[7]="EXPORT";
static char _tmp28C[4]="NEW";static char _tmp28D[9]="ABSTRACT";static char _tmp28E[9]="FALLTHRU";
static char _tmp28F[6]="USING";static char _tmp290[10]="NAMESPACE";static char _tmp291[
7]="TUNION";static char _tmp292[8]="XTUNION";static char _tmp293[7]="MALLOC";static
char _tmp294[8]="RMALLOC";static char _tmp295[7]="CALLOC";static char _tmp296[8]="RCALLOC";
static char _tmp297[9]="REGION_T";static char _tmp298[9]="SIZEOF_T";static char
_tmp299[6]="TAG_T";static char _tmp29A[7]="REGION";static char _tmp29B[5]="RNEW";
static char _tmp29C[8]="REGIONS";static char _tmp29D[13]="RESET_REGION";static char
_tmp29E[4]="GEN";static char _tmp29F[14]="NOZEROTERM_kw";static char _tmp2A0[12]="ZEROTERM_kw";
static char _tmp2A1[7]="PORTON";static char _tmp2A2[8]="PORTOFF";static char _tmp2A3[7]="PTR_OP";
static char _tmp2A4[7]="INC_OP";static char _tmp2A5[7]="DEC_OP";static char _tmp2A6[8]="LEFT_OP";
static char _tmp2A7[9]="RIGHT_OP";static char _tmp2A8[6]="LE_OP";static char _tmp2A9[6]="GE_OP";
static char _tmp2AA[6]="EQ_OP";static char _tmp2AB[6]="NE_OP";static char _tmp2AC[7]="AND_OP";
static char _tmp2AD[6]="OR_OP";static char _tmp2AE[11]="MUL_ASSIGN";static char
_tmp2AF[11]="DIV_ASSIGN";static char _tmp2B0[11]="MOD_ASSIGN";static char _tmp2B1[11]="ADD_ASSIGN";
static char _tmp2B2[11]="SUB_ASSIGN";static char _tmp2B3[12]="LEFT_ASSIGN";static
char _tmp2B4[13]="RIGHT_ASSIGN";static char _tmp2B5[11]="AND_ASSIGN";static char
_tmp2B6[11]="XOR_ASSIGN";static char _tmp2B7[10]="OR_ASSIGN";static char _tmp2B8[9]="ELLIPSIS";
static char _tmp2B9[11]="LEFT_RIGHT";static char _tmp2BA[12]="COLON_COLON";static
char _tmp2BB[11]="IDENTIFIER";static char _tmp2BC[17]="INTEGER_CONSTANT";static char
_tmp2BD[7]="STRING";static char _tmp2BE[19]="CHARACTER_CONSTANT";static char _tmp2BF[
18]="FLOATING_CONSTANT";static char _tmp2C0[9]="TYPE_VAR";static char _tmp2C1[13]="TYPEDEF_NAME";
static char _tmp2C2[16]="QUAL_IDENTIFIER";static char _tmp2C3[18]="QUAL_TYPEDEF_NAME";
static char _tmp2C4[13]="TYPE_INTEGER";static char _tmp2C5[10]="ATTRIBUTE";static
char _tmp2C6[4]="';'";static char _tmp2C7[4]="'{'";static char _tmp2C8[4]="'}'";
static char _tmp2C9[4]="','";static char _tmp2CA[4]="'='";static char _tmp2CB[4]="'('";
static char _tmp2CC[4]="')'";static char _tmp2CD[4]="'_'";static char _tmp2CE[4]="'$'";
static char _tmp2CF[4]="'<'";static char _tmp2D0[4]="':'";static char _tmp2D1[4]="'.'";
static char _tmp2D2[4]="'['";static char _tmp2D3[4]="']'";static char _tmp2D4[4]="'*'";
static char _tmp2D5[4]="'@'";static char _tmp2D6[4]="'?'";static char _tmp2D7[4]="'>'";
static char _tmp2D8[4]="'+'";static char _tmp2D9[4]="'|'";static char _tmp2DA[4]="'^'";
static char _tmp2DB[4]="'&'";static char _tmp2DC[4]="'-'";static char _tmp2DD[4]="'/'";
static char _tmp2DE[4]="'%'";static char _tmp2DF[4]="'~'";static char _tmp2E0[4]="'!'";
static char _tmp2E1[5]="prog";static char _tmp2E2[17]="translation_unit";static char
_tmp2E3[12]="export_list";static char _tmp2E4[19]="export_list_values";static char
_tmp2E5[21]="external_declaration";static char _tmp2E6[20]="function_definition";
static char _tmp2E7[21]="function_definition2";static char _tmp2E8[13]="using_action";
static char _tmp2E9[15]="unusing_action";static char _tmp2EA[17]="namespace_action";
static char _tmp2EB[19]="unnamespace_action";static char _tmp2EC[12]="declaration";
static char _tmp2ED[17]="declaration_list";static char _tmp2EE[23]="declaration_specifiers";
static char _tmp2EF[24]="storage_class_specifier";static char _tmp2F0[15]="attributes_opt";
static char _tmp2F1[11]="attributes";static char _tmp2F2[15]="attribute_list";static
char _tmp2F3[10]="attribute";static char _tmp2F4[15]="type_specifier";static char
_tmp2F5[25]="type_specifier_notypedef";static char _tmp2F6[5]="kind";static char
_tmp2F7[15]="type_qualifier";static char _tmp2F8[15]="enum_specifier";static char
_tmp2F9[11]="enum_field";static char _tmp2FA[22]="enum_declaration_list";static
char _tmp2FB[26]="struct_or_union_specifier";static char _tmp2FC[16]="type_params_opt";
static char _tmp2FD[16]="struct_or_union";static char _tmp2FE[24]="struct_declaration_list";
static char _tmp2FF[25]="struct_declaration_list0";static char _tmp300[21]="init_declarator_list";
static char _tmp301[22]="init_declarator_list0";static char _tmp302[16]="init_declarator";
static char _tmp303[19]="struct_declaration";static char _tmp304[25]="specifier_qualifier_list";
static char _tmp305[35]="notypedef_specifier_qualifier_list";static char _tmp306[23]="struct_declarator_list";
static char _tmp307[24]="struct_declarator_list0";static char _tmp308[18]="struct_declarator";
static char _tmp309[17]="tunion_specifier";static char _tmp30A[18]="tunion_or_xtunion";
static char _tmp30B[17]="tunionfield_list";static char _tmp30C[18]="tunionfield_scope";
static char _tmp30D[12]="tunionfield";static char _tmp30E[11]="declarator";static
char _tmp30F[23]="declarator_withtypedef";static char _tmp310[18]="direct_declarator";
static char _tmp311[30]="direct_declarator_withtypedef";static char _tmp312[8]="pointer";
static char _tmp313[12]="one_pointer";static char _tmp314[23]="pointer_null_and_bound";
static char _tmp315[14]="pointer_bound";static char _tmp316[18]="zeroterm_qual_opt";
static char _tmp317[8]="rgn_opt";static char _tmp318[11]="tqual_list";static char
_tmp319[20]="parameter_type_list";static char _tmp31A[9]="type_var";static char
_tmp31B[16]="optional_effect";static char _tmp31C[19]="optional_rgn_order";static
char _tmp31D[10]="rgn_order";static char _tmp31E[16]="optional_inject";static char
_tmp31F[11]="effect_set";static char _tmp320[14]="atomic_effect";static char _tmp321[
11]="region_set";static char _tmp322[15]="parameter_list";static char _tmp323[22]="parameter_declaration";
static char _tmp324[16]="identifier_list";static char _tmp325[17]="identifier_list0";
static char _tmp326[12]="initializer";static char _tmp327[18]="array_initializer";
static char _tmp328[17]="initializer_list";static char _tmp329[12]="designation";
static char _tmp32A[16]="designator_list";static char _tmp32B[11]="designator";
static char _tmp32C[10]="type_name";static char _tmp32D[14]="any_type_name";static
char _tmp32E[15]="type_name_list";static char _tmp32F[20]="abstract_declarator";
static char _tmp330[27]="direct_abstract_declarator";static char _tmp331[10]="statement";
static char _tmp332[18]="labeled_statement";static char _tmp333[21]="expression_statement";
static char _tmp334[19]="compound_statement";static char _tmp335[16]="block_item_list";
static char _tmp336[20]="selection_statement";static char _tmp337[15]="switch_clauses";
static char _tmp338[20]="iteration_statement";static char _tmp339[15]="jump_statement";
static char _tmp33A[12]="exp_pattern";static char _tmp33B[20]="conditional_pattern";
static char _tmp33C[19]="logical_or_pattern";static char _tmp33D[20]="logical_and_pattern";
static char _tmp33E[21]="inclusive_or_pattern";static char _tmp33F[21]="exclusive_or_pattern";
static char _tmp340[12]="and_pattern";static char _tmp341[17]="equality_pattern";
static char _tmp342[19]="relational_pattern";static char _tmp343[14]="shift_pattern";
static char _tmp344[17]="additive_pattern";static char _tmp345[23]="multiplicative_pattern";
static char _tmp346[13]="cast_pattern";static char _tmp347[14]="unary_pattern";
static char _tmp348[16]="postfix_pattern";static char _tmp349[16]="primary_pattern";
static char _tmp34A[8]="pattern";static char _tmp34B[19]="tuple_pattern_list";static
char _tmp34C[20]="tuple_pattern_list0";static char _tmp34D[14]="field_pattern";
static char _tmp34E[19]="field_pattern_list";static char _tmp34F[20]="field_pattern_list0";
static char _tmp350[11]="expression";static char _tmp351[22]="assignment_expression";
static char _tmp352[20]="assignment_operator";static char _tmp353[23]="conditional_expression";
static char _tmp354[20]="constant_expression";static char _tmp355[22]="logical_or_expression";
static char _tmp356[23]="logical_and_expression";static char _tmp357[24]="inclusive_or_expression";
static char _tmp358[24]="exclusive_or_expression";static char _tmp359[15]="and_expression";
static char _tmp35A[20]="equality_expression";static char _tmp35B[22]="relational_expression";
static char _tmp35C[17]="shift_expression";static char _tmp35D[20]="additive_expression";
static char _tmp35E[26]="multiplicative_expression";static char _tmp35F[16]="cast_expression";
static char _tmp360[17]="unary_expression";static char _tmp361[15]="unary_operator";
static char _tmp362[19]="postfix_expression";static char _tmp363[19]="primary_expression";
static char _tmp364[25]="argument_expression_list";static char _tmp365[26]="argument_expression_list0";
static char _tmp366[9]="constant";static char _tmp367[20]="qual_opt_identifier";
static char _tmp368[17]="qual_opt_typedef";static char _tmp369[18]="struct_union_name";
static char _tmp36A[11]="field_name";static char _tmp36B[12]="right_angle";static
struct _tagged_arr Cyc_yytname[268]={{_tmp260,_tmp260,_tmp260 + 2},{_tmp261,_tmp261,
_tmp261 + 6},{_tmp262,_tmp262,_tmp262 + 12},{_tmp263,_tmp263,_tmp263 + 5},{_tmp264,
_tmp264,_tmp264 + 9},{_tmp265,_tmp265,_tmp265 + 7},{_tmp266,_tmp266,_tmp266 + 7},{
_tmp267,_tmp267,_tmp267 + 8},{_tmp268,_tmp268,_tmp268 + 5},{_tmp269,_tmp269,
_tmp269 + 5},{_tmp26A,_tmp26A,_tmp26A + 6},{_tmp26B,_tmp26B,_tmp26B + 4},{_tmp26C,
_tmp26C,_tmp26C + 5},{_tmp26D,_tmp26D,_tmp26D + 6},{_tmp26E,_tmp26E,_tmp26E + 7},{
_tmp26F,_tmp26F,_tmp26F + 7},{_tmp270,_tmp270,_tmp270 + 9},{_tmp271,_tmp271,
_tmp271 + 6},{_tmp272,_tmp272,_tmp272 + 9},{_tmp273,_tmp273,_tmp273 + 9},{_tmp274,
_tmp274,_tmp274 + 7},{_tmp275,_tmp275,_tmp275 + 6},{_tmp276,_tmp276,_tmp276 + 5},{
_tmp277,_tmp277,_tmp277 + 8},{_tmp278,_tmp278,_tmp278 + 7},{_tmp279,_tmp279,
_tmp279 + 7},{_tmp27A,_tmp27A,_tmp27A + 9},{_tmp27B,_tmp27B,_tmp27B + 3},{_tmp27C,
_tmp27C,_tmp27C + 5},{_tmp27D,_tmp27D,_tmp27D + 7},{_tmp27E,_tmp27E,_tmp27E + 6},{
_tmp27F,_tmp27F,_tmp27F + 3},{_tmp280,_tmp280,_tmp280 + 4},{_tmp281,_tmp281,
_tmp281 + 5},{_tmp282,_tmp282,_tmp282 + 9},{_tmp283,_tmp283,_tmp283 + 6},{_tmp284,
_tmp284,_tmp284 + 7},{_tmp285,_tmp285,_tmp285 + 5},{_tmp286,_tmp286,_tmp286 + 8},{
_tmp287,_tmp287,_tmp287 + 4},{_tmp288,_tmp288,_tmp288 + 6},{_tmp289,_tmp289,
_tmp289 + 4},{_tmp28A,_tmp28A,_tmp28A + 6},{_tmp28B,_tmp28B,_tmp28B + 7},{_tmp28C,
_tmp28C,_tmp28C + 4},{_tmp28D,_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E,_tmp28E + 9},{
_tmp28F,_tmp28F,_tmp28F + 6},{_tmp290,_tmp290,_tmp290 + 10},{_tmp291,_tmp291,
_tmp291 + 7},{_tmp292,_tmp292,_tmp292 + 8},{_tmp293,_tmp293,_tmp293 + 7},{_tmp294,
_tmp294,_tmp294 + 8},{_tmp295,_tmp295,_tmp295 + 7},{_tmp296,_tmp296,_tmp296 + 8},{
_tmp297,_tmp297,_tmp297 + 9},{_tmp298,_tmp298,_tmp298 + 9},{_tmp299,_tmp299,
_tmp299 + 6},{_tmp29A,_tmp29A,_tmp29A + 7},{_tmp29B,_tmp29B,_tmp29B + 5},{_tmp29C,
_tmp29C,_tmp29C + 8},{_tmp29D,_tmp29D,_tmp29D + 13},{_tmp29E,_tmp29E,_tmp29E + 4},{
_tmp29F,_tmp29F,_tmp29F + 14},{_tmp2A0,_tmp2A0,_tmp2A0 + 12},{_tmp2A1,_tmp2A1,
_tmp2A1 + 7},{_tmp2A2,_tmp2A2,_tmp2A2 + 8},{_tmp2A3,_tmp2A3,_tmp2A3 + 7},{_tmp2A4,
_tmp2A4,_tmp2A4 + 7},{_tmp2A5,_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 8},{
_tmp2A7,_tmp2A7,_tmp2A7 + 9},{_tmp2A8,_tmp2A8,_tmp2A8 + 6},{_tmp2A9,_tmp2A9,
_tmp2A9 + 6},{_tmp2AA,_tmp2AA,_tmp2AA + 6},{_tmp2AB,_tmp2AB,_tmp2AB + 6},{_tmp2AC,
_tmp2AC,_tmp2AC + 7},{_tmp2AD,_tmp2AD,_tmp2AD + 6},{_tmp2AE,_tmp2AE,_tmp2AE + 11},{
_tmp2AF,_tmp2AF,_tmp2AF + 11},{_tmp2B0,_tmp2B0,_tmp2B0 + 11},{_tmp2B1,_tmp2B1,
_tmp2B1 + 11},{_tmp2B2,_tmp2B2,_tmp2B2 + 11},{_tmp2B3,_tmp2B3,_tmp2B3 + 12},{
_tmp2B4,_tmp2B4,_tmp2B4 + 13},{_tmp2B5,_tmp2B5,_tmp2B5 + 11},{_tmp2B6,_tmp2B6,
_tmp2B6 + 11},{_tmp2B7,_tmp2B7,_tmp2B7 + 10},{_tmp2B8,_tmp2B8,_tmp2B8 + 9},{_tmp2B9,
_tmp2B9,_tmp2B9 + 11},{_tmp2BA,_tmp2BA,_tmp2BA + 12},{_tmp2BB,_tmp2BB,_tmp2BB + 11},{
_tmp2BC,_tmp2BC,_tmp2BC + 17},{_tmp2BD,_tmp2BD,_tmp2BD + 7},{_tmp2BE,_tmp2BE,
_tmp2BE + 19},{_tmp2BF,_tmp2BF,_tmp2BF + 18},{_tmp2C0,_tmp2C0,_tmp2C0 + 9},{_tmp2C1,
_tmp2C1,_tmp2C1 + 13},{_tmp2C2,_tmp2C2,_tmp2C2 + 16},{_tmp2C3,_tmp2C3,_tmp2C3 + 18},{
_tmp2C4,_tmp2C4,_tmp2C4 + 13},{_tmp2C5,_tmp2C5,_tmp2C5 + 10},{_tmp2C6,_tmp2C6,
_tmp2C6 + 4},{_tmp2C7,_tmp2C7,_tmp2C7 + 4},{_tmp2C8,_tmp2C8,_tmp2C8 + 4},{_tmp2C9,
_tmp2C9,_tmp2C9 + 4},{_tmp2CA,_tmp2CA,_tmp2CA + 4},{_tmp2CB,_tmp2CB,_tmp2CB + 4},{
_tmp2CC,_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD,_tmp2CD + 4},{_tmp2CE,_tmp2CE,
_tmp2CE + 4},{_tmp2CF,_tmp2CF,_tmp2CF + 4},{_tmp2D0,_tmp2D0,_tmp2D0 + 4},{_tmp2D1,
_tmp2D1,_tmp2D1 + 4},{_tmp2D2,_tmp2D2,_tmp2D2 + 4},{_tmp2D3,_tmp2D3,_tmp2D3 + 4},{
_tmp2D4,_tmp2D4,_tmp2D4 + 4},{_tmp2D5,_tmp2D5,_tmp2D5 + 4},{_tmp2D6,_tmp2D6,
_tmp2D6 + 4},{_tmp2D7,_tmp2D7,_tmp2D7 + 4},{_tmp2D8,_tmp2D8,_tmp2D8 + 4},{_tmp2D9,
_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB,_tmp2DB + 4},{
_tmp2DC,_tmp2DC,_tmp2DC + 4},{_tmp2DD,_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE,
_tmp2DE + 4},{_tmp2DF,_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0,_tmp2E0 + 4},{_tmp2E1,
_tmp2E1,_tmp2E1 + 5},{_tmp2E2,_tmp2E2,_tmp2E2 + 17},{_tmp2E3,_tmp2E3,_tmp2E3 + 12},{
_tmp2E4,_tmp2E4,_tmp2E4 + 19},{_tmp2E5,_tmp2E5,_tmp2E5 + 21},{_tmp2E6,_tmp2E6,
_tmp2E6 + 20},{_tmp2E7,_tmp2E7,_tmp2E7 + 21},{_tmp2E8,_tmp2E8,_tmp2E8 + 13},{
_tmp2E9,_tmp2E9,_tmp2E9 + 15},{_tmp2EA,_tmp2EA,_tmp2EA + 17},{_tmp2EB,_tmp2EB,
_tmp2EB + 19},{_tmp2EC,_tmp2EC,_tmp2EC + 12},{_tmp2ED,_tmp2ED,_tmp2ED + 17},{
_tmp2EE,_tmp2EE,_tmp2EE + 23},{_tmp2EF,_tmp2EF,_tmp2EF + 24},{_tmp2F0,_tmp2F0,
_tmp2F0 + 15},{_tmp2F1,_tmp2F1,_tmp2F1 + 11},{_tmp2F2,_tmp2F2,_tmp2F2 + 15},{
_tmp2F3,_tmp2F3,_tmp2F3 + 10},{_tmp2F4,_tmp2F4,_tmp2F4 + 15},{_tmp2F5,_tmp2F5,
_tmp2F5 + 25},{_tmp2F6,_tmp2F6,_tmp2F6 + 5},{_tmp2F7,_tmp2F7,_tmp2F7 + 15},{_tmp2F8,
_tmp2F8,_tmp2F8 + 15},{_tmp2F9,_tmp2F9,_tmp2F9 + 11},{_tmp2FA,_tmp2FA,_tmp2FA + 22},{
_tmp2FB,_tmp2FB,_tmp2FB + 26},{_tmp2FC,_tmp2FC,_tmp2FC + 16},{_tmp2FD,_tmp2FD,
_tmp2FD + 16},{_tmp2FE,_tmp2FE,_tmp2FE + 24},{_tmp2FF,_tmp2FF,_tmp2FF + 25},{
_tmp300,_tmp300,_tmp300 + 21},{_tmp301,_tmp301,_tmp301 + 22},{_tmp302,_tmp302,
_tmp302 + 16},{_tmp303,_tmp303,_tmp303 + 19},{_tmp304,_tmp304,_tmp304 + 25},{
_tmp305,_tmp305,_tmp305 + 35},{_tmp306,_tmp306,_tmp306 + 23},{_tmp307,_tmp307,
_tmp307 + 24},{_tmp308,_tmp308,_tmp308 + 18},{_tmp309,_tmp309,_tmp309 + 17},{
_tmp30A,_tmp30A,_tmp30A + 18},{_tmp30B,_tmp30B,_tmp30B + 17},{_tmp30C,_tmp30C,
_tmp30C + 18},{_tmp30D,_tmp30D,_tmp30D + 12},{_tmp30E,_tmp30E,_tmp30E + 11},{
_tmp30F,_tmp30F,_tmp30F + 23},{_tmp310,_tmp310,_tmp310 + 18},{_tmp311,_tmp311,
_tmp311 + 30},{_tmp312,_tmp312,_tmp312 + 8},{_tmp313,_tmp313,_tmp313 + 12},{_tmp314,
_tmp314,_tmp314 + 23},{_tmp315,_tmp315,_tmp315 + 14},{_tmp316,_tmp316,_tmp316 + 18},{
_tmp317,_tmp317,_tmp317 + 8},{_tmp318,_tmp318,_tmp318 + 11},{_tmp319,_tmp319,
_tmp319 + 20},{_tmp31A,_tmp31A,_tmp31A + 9},{_tmp31B,_tmp31B,_tmp31B + 16},{_tmp31C,
_tmp31C,_tmp31C + 19},{_tmp31D,_tmp31D,_tmp31D + 10},{_tmp31E,_tmp31E,_tmp31E + 16},{
_tmp31F,_tmp31F,_tmp31F + 11},{_tmp320,_tmp320,_tmp320 + 14},{_tmp321,_tmp321,
_tmp321 + 11},{_tmp322,_tmp322,_tmp322 + 15},{_tmp323,_tmp323,_tmp323 + 22},{
_tmp324,_tmp324,_tmp324 + 16},{_tmp325,_tmp325,_tmp325 + 17},{_tmp326,_tmp326,
_tmp326 + 12},{_tmp327,_tmp327,_tmp327 + 18},{_tmp328,_tmp328,_tmp328 + 17},{
_tmp329,_tmp329,_tmp329 + 12},{_tmp32A,_tmp32A,_tmp32A + 16},{_tmp32B,_tmp32B,
_tmp32B + 11},{_tmp32C,_tmp32C,_tmp32C + 10},{_tmp32D,_tmp32D,_tmp32D + 14},{
_tmp32E,_tmp32E,_tmp32E + 15},{_tmp32F,_tmp32F,_tmp32F + 20},{_tmp330,_tmp330,
_tmp330 + 27},{_tmp331,_tmp331,_tmp331 + 10},{_tmp332,_tmp332,_tmp332 + 18},{
_tmp333,_tmp333,_tmp333 + 21},{_tmp334,_tmp334,_tmp334 + 19},{_tmp335,_tmp335,
_tmp335 + 16},{_tmp336,_tmp336,_tmp336 + 20},{_tmp337,_tmp337,_tmp337 + 15},{
_tmp338,_tmp338,_tmp338 + 20},{_tmp339,_tmp339,_tmp339 + 15},{_tmp33A,_tmp33A,
_tmp33A + 12},{_tmp33B,_tmp33B,_tmp33B + 20},{_tmp33C,_tmp33C,_tmp33C + 19},{
_tmp33D,_tmp33D,_tmp33D + 20},{_tmp33E,_tmp33E,_tmp33E + 21},{_tmp33F,_tmp33F,
_tmp33F + 21},{_tmp340,_tmp340,_tmp340 + 12},{_tmp341,_tmp341,_tmp341 + 17},{
_tmp342,_tmp342,_tmp342 + 19},{_tmp343,_tmp343,_tmp343 + 14},{_tmp344,_tmp344,
_tmp344 + 17},{_tmp345,_tmp345,_tmp345 + 23},{_tmp346,_tmp346,_tmp346 + 13},{
_tmp347,_tmp347,_tmp347 + 14},{_tmp348,_tmp348,_tmp348 + 16},{_tmp349,_tmp349,
_tmp349 + 16},{_tmp34A,_tmp34A,_tmp34A + 8},{_tmp34B,_tmp34B,_tmp34B + 19},{_tmp34C,
_tmp34C,_tmp34C + 20},{_tmp34D,_tmp34D,_tmp34D + 14},{_tmp34E,_tmp34E,_tmp34E + 19},{
_tmp34F,_tmp34F,_tmp34F + 20},{_tmp350,_tmp350,_tmp350 + 11},{_tmp351,_tmp351,
_tmp351 + 22},{_tmp352,_tmp352,_tmp352 + 20},{_tmp353,_tmp353,_tmp353 + 23},{
_tmp354,_tmp354,_tmp354 + 20},{_tmp355,_tmp355,_tmp355 + 22},{_tmp356,_tmp356,
_tmp356 + 23},{_tmp357,_tmp357,_tmp357 + 24},{_tmp358,_tmp358,_tmp358 + 24},{
_tmp359,_tmp359,_tmp359 + 15},{_tmp35A,_tmp35A,_tmp35A + 20},{_tmp35B,_tmp35B,
_tmp35B + 22},{_tmp35C,_tmp35C,_tmp35C + 17},{_tmp35D,_tmp35D,_tmp35D + 20},{
_tmp35E,_tmp35E,_tmp35E + 26},{_tmp35F,_tmp35F,_tmp35F + 16},{_tmp360,_tmp360,
_tmp360 + 17},{_tmp361,_tmp361,_tmp361 + 15},{_tmp362,_tmp362,_tmp362 + 19},{
_tmp363,_tmp363,_tmp363 + 19},{_tmp364,_tmp364,_tmp364 + 25},{_tmp365,_tmp365,
_tmp365 + 26},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 20},{_tmp368,
_tmp368,_tmp368 + 17},{_tmp369,_tmp369,_tmp369 + 18},{_tmp36A,_tmp36A,_tmp36A + 11},{
_tmp36B,_tmp36B,_tmp36B + 12}};static short Cyc_yyr1[487]={0,129,130,130,130,130,
130,130,130,130,130,130,131,131,132,132,133,133,133,134,134,134,134,135,135,136,
137,138,139,140,140,140,140,141,141,142,142,142,142,142,142,142,142,142,142,143,
143,143,143,143,143,143,144,144,145,146,146,147,147,147,147,147,147,148,148,149,
149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
150,151,151,151,152,152,152,153,153,154,154,154,155,155,155,156,156,157,157,158,
158,159,159,160,161,161,162,162,163,164,164,164,164,164,164,165,165,165,165,165,
165,166,167,167,168,168,168,169,169,169,170,170,171,171,171,171,172,172,172,173,
173,174,174,175,175,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,
177,177,177,177,177,177,178,178,179,180,180,180,181,181,181,182,182,182,183,183,
183,184,184,185,185,185,185,186,186,187,187,188,188,189,189,190,190,191,191,192,
192,192,192,193,193,194,194,195,195,195,196,197,197,198,198,199,199,199,199,200,
200,200,200,201,202,202,203,203,204,204,205,205,205,205,205,206,206,207,207,207,
208,208,208,208,208,208,208,208,208,208,208,209,209,209,209,209,209,209,209,209,
209,209,210,211,211,212,212,213,213,213,213,213,213,214,214,214,214,214,214,215,
215,215,215,215,215,216,216,216,216,216,216,216,216,216,216,216,216,216,216,217,
217,217,217,217,217,217,217,218,219,219,220,220,221,221,222,222,223,223,224,224,
225,225,225,226,226,226,226,226,227,227,227,228,228,228,229,229,229,229,230,230,
231,231,231,231,231,231,231,232,233,234,234,234,234,234,234,234,234,234,234,234,
234,235,235,235,236,236,237,237,238,238,238,239,239,240,240,241,241,242,242,242,
242,242,242,242,242,242,242,242,243,243,243,243,243,243,243,244,245,245,246,246,
247,247,248,248,249,249,250,250,250,251,251,251,251,251,252,252,252,253,253,253,
254,254,254,254,255,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,
256,256,257,257,257,258,258,258,258,258,258,258,258,258,258,259,259,259,259,259,
259,259,259,259,260,261,261,262,262,262,262,263,263,264,264,265,265,266,266,267,
267};static short Cyc_yyr2[487]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,
2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,
1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,
0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,
2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,3,0,1,1,0,
1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,
1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,3,
9,6,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,
3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,
4,1,2,2,4,2,6,6,1,1,1,3,1,1,4,4,5,2,2,2,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,
1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,
3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,
2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[970]={0,18,
45,46,47,48,50,65,68,69,70,71,72,73,74,75,87,88,89,103,104,41,0,0,51,0,0,136,137,
0,0,84,0,0,477,193,479,478,480,85,0,0,66,0,178,178,177,1,0,16,0,0,17,0,35,43,37,
63,39,76,77,0,78,184,0,147,0,172,181,79,151,101,49,48,42,0,91,476,0,477,473,474,
475,0,355,0,0,0,0,216,0,357,358,25,27,0,0,0,0,0,0,0,0,0,0,0,0,175,176,2,0,0,0,0,
29,0,109,110,112,36,44,38,40,105,481,482,101,186,0,185,0,33,0,0,19,195,0,0,160,
148,173,183,182,184,0,64,0,49,95,0,93,0,477,363,0,0,0,0,0,0,0,0,0,0,101,0,0,463,
0,0,0,0,0,450,448,449,0,379,381,394,402,404,406,408,410,412,415,420,423,426,430,
0,432,451,462,461,0,364,362,32,0,0,101,0,0,0,119,115,117,234,236,0,0,0,9,10,483,
484,194,86,0,0,152,67,214,0,211,0,0,0,3,0,5,0,30,0,0,0,20,0,106,107,0,100,101,0,
0,0,0,0,0,0,0,0,0,0,0,0,477,269,271,277,273,0,275,257,258,259,0,260,261,262,0,34,
21,112,201,217,0,0,197,195,0,241,0,181,0,52,0,0,96,92,0,0,0,0,0,440,0,396,430,0,
397,398,0,0,0,0,0,0,0,433,434,0,0,0,0,436,437,435,0,356,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,384,385,386,387,388,389,390,391,392,393,383,0,438,0,457,458,0,0,0,
465,0,101,369,370,0,367,218,0,0,0,0,237,209,0,120,125,121,123,116,118,195,0,243,
235,244,486,485,0,81,82,83,58,57,0,55,153,195,213,149,243,215,161,162,0,80,0,180,
179,26,0,28,0,111,113,220,219,22,98,108,0,0,0,127,128,130,0,101,0,134,0,0,0,0,0,
0,0,0,306,307,308,0,0,310,0,0,0,0,0,0,278,274,112,276,272,270,202,0,0,0,208,196,
203,156,0,0,0,197,158,0,159,154,181,187,53,102,0,97,401,94,90,365,366,0,0,0,221,
0,0,225,0,0,0,230,0,0,0,0,0,0,0,0,0,464,471,0,470,380,403,0,405,407,409,411,413,
414,418,419,416,417,421,422,424,425,427,428,429,382,456,453,0,455,0,0,0,359,0,31,
376,0,372,377,0,374,360,0,0,238,126,122,124,0,197,0,181,0,245,195,0,0,256,240,0,
0,0,119,0,101,195,0,0,171,150,212,4,6,0,131,114,0,0,197,101,142,0,0,0,285,0,0,0,
0,0,305,309,0,0,0,264,0,0,0,268,0,23,195,0,205,0,0,198,0,157,201,189,242,155,187,
174,0,7,0,439,0,0,233,0,222,0,226,229,231,444,0,0,0,0,0,0,0,0,431,467,0,0,454,
452,0,0,368,371,373,361,0,239,210,252,0,246,247,181,0,197,0,181,0,0,0,0,54,56,0,
163,0,197,0,0,181,0,129,132,105,135,144,143,0,0,138,0,0,0,0,0,0,0,0,0,0,0,0,0,
285,311,0,0,0,0,24,197,0,206,204,0,197,0,188,0,8,0,0,0,232,223,227,0,0,0,0,399,
400,443,469,0,472,395,466,468,0,375,378,251,249,254,0,255,248,181,0,61,59,60,164,
167,0,169,170,165,181,0,133,145,142,142,279,285,0,0,0,0,0,0,313,314,316,318,320,
322,324,326,329,334,337,340,344,346,353,354,0,0,282,291,0,0,0,0,0,0,0,0,0,0,312,
0,0,0,267,191,207,199,190,195,13,0,0,442,441,0,228,445,0,0,459,0,253,250,0,168,
166,99,0,141,140,0,0,285,0,350,0,0,347,285,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,348,286,0,293,0,0,0,301,0,0,0,0,0,0,284,263,266,0,0,197,12,14,0,0,0,0,460,0,0,
280,281,0,0,0,0,285,287,317,0,319,321,323,325,327,328,332,333,330,331,335,336,
338,339,341,342,343,0,292,294,295,0,303,302,0,297,0,0,0,0,200,192,15,0,0,0,0,146,
283,349,0,345,288,0,285,296,304,298,299,0,0,224,446,0,62,0,0,315,285,289,300,265,
0,352,351,290,447,0,0,0};static short Cyc_yydefgoto[139]={967,47,621,812,48,49,261,
50,410,51,412,52,132,53,54,473,206,394,395,207,57,218,208,59,148,149,60,145,61,
239,240,115,116,117,241,209,379,422,423,424,62,63,685,686,687,64,425,65,399,66,
67,68,107,143,128,618,551,69,552,465,610,457,461,462,374,281,226,88,89,489,415,
490,491,492,493,210,283,284,553,385,264,265,266,267,268,269,693,270,271,770,771,
772,773,774,775,776,777,778,779,780,781,782,783,784,785,364,365,366,541,542,543,
272,177,352,178,479,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,
505,506,194,195,71,813,219,389};static short Cyc_yypact[970]={2412,- -32768,- -32768,
- -32768,- -32768,- 31,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,2948,251,940,-
-32768,198,- 4,- -32768,- -32768,- 8,- 3,72,139,148,- -32768,170,- -32768,- -32768,-
-32768,- -32768,151,811,182,167,232,232,- -32768,- -32768,2293,- -32768,- 63,383,-
-32768,203,2948,2948,2948,- -32768,2948,- -32768,- -32768,320,- -32768,65,2891,211,
186,465,560,- -32768,- -32768,174,259,216,- -32768,198,265,- -32768,3446,85,- -32768,
- -32768,- -32768,5059,- -32768,285,309,3446,224,304,315,- -32768,51,- -32768,- -32768,
5564,5564,5564,2293,2293,185,326,685,346,185,5738,1803,- -32768,- -32768,- -32768,
2293,2530,2293,2530,- -32768,354,353,- -32768,2823,- -32768,- -32768,- -32768,- -32768,
5738,- -32768,- -32768,174,- -32768,198,- -32768,1430,- -32768,2891,203,- -32768,1132,
5564,3495,- -32768,211,- -32768,- -32768,- -32768,65,5564,- -32768,2530,- -32768,355,
358,360,198,385,- -32768,66,5266,408,5059,5108,445,451,453,460,464,174,5315,5315,
- -32768,1924,497,5394,5394,5394,- -32768,- -32768,- -32768,235,- -32768,- -32768,- 33,
526,492,418,513,454,119,559,35,191,- -32768,729,5394,223,30,- -32768,535,3024,-
-32768,- -32768,- -32768,551,5059,174,3024,549,113,5738,5753,5738,536,- -32768,- 37,
- 37,- 37,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,16,552,- -32768,- -32768,
272,240,- -32768,562,13,563,- -32768,567,- -32768,573,- -32768,685,3587,2891,- -32768,
575,5738,- -32768,740,586,115,593,62,602,3034,642,656,655,657,3679,3034,- 39,91,
658,663,- -32768,- -32768,1556,1556,203,1556,- -32768,- -32768,- -32768,681,- -32768,-
-32768,- -32768,206,- -32768,- -32768,694,696,- -32768,31,715,692,222,720,716,- 35,
560,718,748,- 35,737,198,- -32768,5059,747,761,763,1924,- -32768,5738,- -32768,-
-32768,3127,- -32768,807,5059,5059,5059,5059,5059,786,1924,- -32768,- -32768,1556,
793,279,5059,- -32768,- -32768,- -32768,5059,- -32768,5394,5059,5394,5394,5394,5394,
5394,5394,5394,5394,5394,5394,5394,5394,5394,5394,5394,5394,5394,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5059,-
-32768,185,- -32768,- -32768,3771,185,5059,- -32768,792,174,- -32768,- -32768,800,806,
- -32768,393,3170,805,5564,- -32768,815,820,- -32768,5753,5753,5753,- -32768,- -32768,
2651,3863,267,- -32768,294,- -32768,- -32768,31,- -32768,- -32768,- -32768,- -32768,818,
822,826,- -32768,2595,- -32768,478,156,- -32768,- -32768,- -32768,5738,- -32768,2045,-
-32768,- -32768,- -32768,2293,- -32768,2293,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,606,5059,830,828,- -32768,823,374,174,198,833,5059,5059,831,834,
5059,909,1682,838,- -32768,- -32768,- -32768,402,899,- -32768,3955,3034,849,855,5059,
3034,- -32768,- -32768,2823,- -32768,- -32768,- -32768,- -32768,5738,840,118,- -32768,-
-32768,832,- -32768,31,841,5635,692,- -32768,5564,- -32768,- -32768,560,827,- -32768,
- -32768,2174,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,842,850,863,- -32768,
185,5059,- -32768,589,3587,328,- -32768,433,851,857,858,449,5738,852,860,5187,-
-32768,- -32768,861,865,- -32768,526,253,492,418,513,454,119,119,559,559,559,559,
35,35,191,191,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,868,- -32768,83,
5564,3403,- -32768,3354,- -32768,- -32768,3446,- -32768,- -32768,862,872,- -32768,20,
869,- -32768,- -32768,- -32768,- -32768,871,692,873,560,870,294,5620,5564,4047,-
-32768,- -32768,772,875,16,2707,878,174,1132,5564,4139,- -32768,478,- -32768,-
-32768,- -32768,770,- -32768,- -32768,740,5059,692,174,710,503,514,5059,698,532,866,
4231,4323,546,- -32768,- -32768,877,886,881,- -32768,874,876,553,- -32768,2891,-
-32768,888,5564,- -32768,890,31,- -32768,880,- -32768,157,- -32768,716,- -32768,827,-
-32768,889,- -32768,2293,894,502,891,- -32768,885,- -32768,3219,- -32768,- -32768,-
-32768,- -32768,5059,978,5059,5108,896,894,898,3403,- -32768,- -32768,5059,5059,-
-32768,- -32768,- 35,749,- -32768,- -32768,- -32768,- -32768,3262,- -32768,- -32768,-
-32768,901,- -32768,- -32768,560,902,692,- 35,560,892,557,904,906,- -32768,- -32768,
907,- -32768,910,692,913,- 35,560,912,- -32768,- -32768,5738,- -32768,- -32768,- -32768,
921,198,576,3034,914,922,970,924,929,3034,5059,4415,585,4507,596,4599,698,-
-32768,935,948,- 38,939,- -32768,692,104,- -32768,- -32768,946,692,5738,- -32768,262,
- -32768,937,945,5059,- -32768,- -32768,- -32768,3587,594,947,950,- -32768,807,-
-32768,- -32768,755,- -32768,- -32768,- -32768,- -32768,3403,- -32768,- -32768,- -32768,
- -32768,- -32768,949,- -32768,- -32768,560,967,- -32768,- -32768,- -32768,- -32768,-
-32768,958,- -32768,- -32768,- -32768,560,963,- -32768,962,37,710,1042,698,968,5443,
965,2045,5394,961,- -32768,- 17,- -32768,998,955,768,821,138,829,116,278,- -32768,-
-32768,- -32768,- -32768,1002,5394,1556,- -32768,- -32768,603,3034,609,4691,3034,640,
4783,4875,664,977,- -32768,3034,3034,986,- -32768,- -32768,- -32768,979,- -32768,888,
- -32768,981,668,- -32768,- -32768,327,- -32768,- -32768,5738,1058,- -32768,3311,-
-32768,- -32768,982,- -32768,- -32768,- -32768,5738,- -32768,- -32768,3034,984,698,
1924,- -32768,5738,983,- -32768,1304,5394,5059,5394,5394,5394,5394,5394,5394,5394,
5394,5394,5394,5394,5394,5394,5394,5394,5394,5394,5059,- -32768,- -32768,987,-
-32768,3034,3034,666,- -32768,3034,3034,672,3034,712,4967,- -32768,- -32768,- -32768,
976,31,692,- -32768,- -32768,419,5059,988,992,- -32768,1008,713,- -32768,- -32768,997,
994,999,5394,698,- -32768,526,394,492,418,418,454,119,119,559,559,559,559,35,35,
191,191,- -32768,- -32768,- -32768,396,- -32768,- -32768,- -32768,3034,- -32768,- -32768,
3034,- -32768,3034,3034,719,1014,- -32768,- -32768,- -32768,812,1003,5738,1004,-
-32768,- -32768,894,515,- -32768,- -32768,5059,1304,- -32768,- -32768,- -32768,- -32768,
3034,3034,- -32768,- -32768,1006,- -32768,1007,1009,- -32768,698,- -32768,- -32768,-
-32768,1011,- -32768,- -32768,- -32768,- -32768,1106,1107,- -32768};static short Cyc_yypgoto[
139]={- -32768,168,- -32768,227,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 61,- 113,165,- -32768,- -32768,0,556,- -32768,67,- 183,1012,38,- -32768,-
-32768,- 123,- -32768,- 29,- -32768,442,- -32768,- -32768,- -32768,895,884,229,495,-
-32768,- -32768,547,- -32768,- -32768,161,- -32768,- -32768,543,- 193,1059,- 349,219,-
-32768,- -32768,1082,202,989,512,- 119,63,- 100,- 249,252,520,525,- 386,- 422,- 83,- 357,
- 122,- -32768,- 235,- 154,- 502,- 355,- -32768,643,- 153,- 16,- 78,- 80,- 311,102,- -32768,-
-32768,- 12,- 252,- -32768,- 115,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,27,933,- -32768,484,- -32768,- -32768,324,569,- -32768,- 150,- 372,- 152,
- 304,- 297,- 314,835,- 307,- 303,- 99,- 305,- 291,- 96,404,463,- -32768,- -32768,- 340,-
-32768,228,422,- 53,1095,- 335,- 49};static short Cyc_yytable[5864]={55,414,561,131,
302,237,303,299,125,450,451,511,453,282,539,314,279,530,508,528,513,55,225,531,
377,514,515,510,293,520,521,398,648,392,386,280,386,608,58,110,111,102,683,684,
322,522,523,573,55,577,90,572,134,803,55,55,55,131,55,58,841,501,72,443,55,138,
288,56,444,262,469,273,556,804,317,318,319,572,611,211,212,213,387,388,387,323,
58,94,56,556,228,458,58,58,58,353,58,243,55,55,605,842,58,95,597,153,238,393,96,
573,55,55,55,55,198,56,626,407,55,360,274,56,56,56,655,56,129,35,654,384,55,56,
55,388,459,309,58,58,732,138,388,- 139,478,483,401,484,55,361,58,58,58,58,625,34,
202,336,58,500,203,337,37,35,294,390,391,56,56,477,58,431,58,403,432,369,127,295,
273,56,56,56,56,467,445,97,58,56,74,- 217,320,403,- 217,330,331,377,377,377,154,56,
646,56,262,262,446,262,566,447,129,376,680,35,849,850,807,56,35,429,109,372,614,
119,120,121,607,122,388,417,144,566,428,133,332,516,517,518,519,470,855,55,333,
475,856,98,524,525,526,378,690,34,456,851,99,91,262,36,37,38,629,852,101,278,100,
55,55,397,55,- 195,214,215,373,- 195,382,478,104,724,105,58,216,34,230,231,232,233,
217,133,37,144,140,474,719,34,354,355,356,41,34,263,37,133,539,58,58,37,58,657,
56,114,91,338,455,147,41,320,40,55,289,91,339,340,135,44,45,46,136,225,278,137,
199,466,56,56,357,56,681,534,224,106,358,359,478,603,320,460,34,321,567,404,637,
403,405,37,435,58,242,34,75,545,442,810,320,36,37,38,146,34,224,644,811,403,151,
36,37,38,671,403,381,591,376,376,376,397,56,382,724,671,320,560,382,503,44,45,46,
131,196,723,857,40,540,565,581,571,197,557,133,858,859,558,641,176,559,200,55,34,
55,743,378,378,378,36,37,38,576,201,70,123,91,753,263,263,383,263,478,91,320,220,
630,375,55,380,661,884,604,487,488,400,76,92,675,93,58,673,58,460,55,615,222,647,
234,662,235,806,290,426,291,70,809,34,292,724,674,242,70,36,37,38,58,70,55,56,
263,56,663,420,728,124,729,112,113,471,70,817,58,676,315,611,734,537,154,150,320,
320,92,320,723,56,594,708,943,320,944,92,34,617,567,713,58,298,36,37,38,56,70,70,
373,403,70,368,403,460,328,329,901,902,70,70,70,70,862,898,320,903,326,632,273,
56,904,905,900,598,910,911,244,602,304,682,320,70,560,636,305,297,306,300,300,
650,912,913,651,307,70,311,312,308,571,150,300,300,300,441,574,40,575,44,45,46,
103,568,800,723,896,569,709,707,570,216,718,300,118,91,735,217,383,133,324,55,
316,956,216,955,320,373,224,688,217,325,744,838,400,133,92,320,315,55,689,141,
142,92,224,755,494,334,335,931,498,224,315,224,327,320,362,426,694,58,367,381,
620,221,402,509,700,833,382,320,44,45,46,617,371,70,320,58,396,706,747,125,402,
748,885,408,433,406,56,409,460,839,616,229,275,411,761,418,540,762,893,532,894,
70,224,794,56,427,320,861,958,627,628,224,300,34,797,320,430,320,818,36,37,38,
286,40,320,434,880,863,150,420,320,683,684,865,786,892,691,692,44,45,46,897,300,
262,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,320,889,
437,869,436,906,907,908,909,584,585,659,438,588,439,592,914,915,916,91,448,874,
91,133,320,882,320,601,883,921,449,34,320,275,262,924,942,953,37,426,454,224,456,
55,717,763,92,41,957,375,426,790,224,426,941,236,44,45,46,464,416,452,341,342,
343,344,345,346,347,348,349,350,320,404,402,926,937,402,463,320,300,58,949,468,
959,125,34,70,472,70,351,388,36,37,38,55,476,402,965,16,17,18,420,402,40,582,480,
421,736,737,56,44,45,46,821,822,34,741,666,667,668,745,36,37,38,416,548,549,550,
495,496,497,420,58,756,481,91,482,262,322,504,44,45,46,507,845,846,300,499,864,
847,848,868,70,853,854,502,34,533,876,877,300,56,535,37,242,536,40,544,697,699,
951,320,41,91,546,527,830,831,547,562,504,44,45,46,563,564,578,579,890,580,583,
587,586,589,593,595,460,224,55,599,600,606,824,612,622,555,609,263,624,623,633,
725,92,827,638,92,634,635,639,35,652,919,920,642,643,922,923,695,925,504,645,653,
77,656,701,658,58,669,300,660,672,402,702,703,278,705,716,704,710,766,767,640,
402,712,721,402,720,726,730,263,731,746,77,740,742,56,749,504,750,751,78,764,752,
791,793,754,796,945,799,759,946,757,947,948,765,79,80,789,81,82,788,801,37,802,
300,805,808,70,816,814,78,83,300,84,85,960,961,815,819,820,86,823,224,825,416,
152,80,87,81,82,826,828,37,829,832,834,837,840,843,92,844,768,860,84,85,875,878,
886,879,881,86,888,891,918,769,895,176,87,173,929,934,174,175,935,936,938,939,
416,940,950,968,969,760,263,932,952,954,92,962,963,223,964,867,966,670,871,873,
758,419,139,679,108,665,715,413,930,287,714,711,631,370,739,124,678,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,787,504,126,0,0,315,0,512,0,0,0,0,899,0,0,22,836,0,0,
300,0,0,0,0,0,0,0,27,28,0,917,0,0,29,30,31,0,300,0,0,0,0,0,416,928,0,0,0,0,0,727,
0,0,0,933,416,0,0,733,0,0,0,0,0,0,0,276,0,0,277,0,0,0,0,35,36,0,38,39,40,278,0,0,
0,0,0,0,42,43,0,0,300,0,300,300,300,300,300,300,300,300,300,300,300,300,300,300,
300,300,300,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,416,0,0,0,
0,0,300,0,0,0,0,0,124,416,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,691,
692,21,155,156,245,0,246,247,248,249,250,251,252,253,22,77,23,157,254,0,300,158,
24,255,0,0,27,28,159,160,161,162,29,30,31,256,163,0,257,164,0,0,0,0,0,165,166,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,416,0,0,0,258,80,167,81,82,35,36,37,38,39,40,259,
130,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,2,3,4,73,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,155,156,245,0,246,247,248,249,
250,251,252,253,22,77,23,157,254,0,0,158,24,255,0,0,27,28,159,160,161,162,29,30,
31,256,163,0,257,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
258,80,167,81,82,35,36,37,38,39,40,259,130,260,0,0,168,0,42,227,0,0,0,0,0,170,0,
0,0,171,0,0,172,173,0,0,174,175,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,155,156,245,0,246,247,248,249,250,251,252,253,22,77,23,157,254,0,0,158,
24,255,0,0,27,28,159,160,161,162,29,30,31,256,163,0,257,164,0,0,0,0,0,165,166,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,80,167,81,82,35,36,37,38,39,40,259,
130,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,2,3,4,73,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,155,156,0,0,0,0,0,0,0,0,0,0,22,
77,23,157,0,0,0,158,24,0,0,0,27,28,159,160,161,162,29,30,31,0,163,0,0,164,0,0,0,
0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,
38,39,40,590,0,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,
175,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,
77,0,157,0,0,0,158,0,0,0,0,27,28,159,160,161,162,29,30,31,0,163,204,0,164,0,0,0,
0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,
38,39,40,0,205,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,
175,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,
77,0,157,0,0,0,158,0,0,0,0,27,28,159,160,161,162,29,30,31,0,163,0,0,164,0,0,0,0,
0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,
39,40,0,313,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,22,77,0,
157,0,0,0,158,0,0,0,0,27,28,159,160,161,162,29,30,31,0,163,0,0,164,0,0,0,0,0,165,
166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,35,36,37,38,39,40,
0,0,0,0,0,168,0,42,227,0,0,0,0,0,170,0,0,0,171,0,0,172,173,0,0,174,175,- 11,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,619,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,
41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,
29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,41,0,42,
43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,
0,0,0,0,0,0,32,33,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,34,
0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,43,0,0,0,27,28,44,45,46,0,29,30,
31,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,276,0,
0,34,0,22,0,0,35,36,37,38,39,40,278,0,0,27,28,397,0,42,43,29,30,31,382,0,44,45,
46,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,22,0,
0,35,36,0,38,39,40,278,0,0,27,28,381,0,42,43,29,30,31,382,0,44,45,46,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,
0,420,0,42,43,0,0,0,0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,28,0,0,0,0,29,30,
31,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,35,36,0,38,39,40,0,130,0,22,236,23,0,42,43,0,0,24,0,0,0,27,28,0,0,0,0,
29,30,31,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,35,36,0,38,39,40,24,130,0,0,27,28,0,42,43,0,29,30,31,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,0,38,
39,40,0,0,0,0,0,0,0,42,43,155,156,245,77,246,247,248,249,250,251,252,253,0,77,0,
157,254,0,0,158,0,255,0,0,0,0,159,160,161,162,0,0,0,256,163,0,257,164,0,0,0,78,0,
165,166,0,0,0,0,0,0,0,0,363,0,0,152,80,0,81,82,0,0,37,0,0,258,80,167,81,82,0,83,
37,84,85,0,259,130,0,0,86,168,0,0,169,0,0,87,0,0,170,0,155,156,171,0,0,172,173,
485,0,174,175,0,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,
164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,77,0,0,0,0,0,0,0,0,0,34,80,167,81,82,
0,0,37,0,0,0,0,301,486,0,0,168,0,0,169,0,0,487,488,0,170,155,156,78,171,0,0,172,
173,0,0,174,175,0,77,538,157,0,152,80,158,81,82,0,0,37,0,159,160,161,162,0,0,0,
83,163,84,85,164,0,487,488,0,86,165,166,0,0,0,0,87,0,0,0,0,0,0,77,0,0,0,0,0,0,0,
0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,722,0,0,168,0,0,169,0,0,487,488,0,170,155,
156,78,171,0,0,172,173,0,0,174,175,0,77,738,157,0,152,80,158,81,82,0,0,37,0,159,
160,161,162,0,0,0,83,163,84,85,164,0,487,488,0,86,165,166,0,0,0,0,87,0,0,0,0,0,0,
77,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,887,0,0,168,0,0,169,0,0,
487,488,0,170,155,156,78,171,0,0,172,173,0,0,174,175,0,77,649,157,0,152,80,158,
81,82,0,0,37,0,159,160,161,162,0,0,0,83,163,84,85,164,0,0,0,0,86,165,166,0,0,0,0,
87,0,0,0,0,0,0,77,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,0,0,0,168,
0,0,169,0,0,487,488,0,170,155,156,78,171,0,0,172,173,0,0,174,175,0,77,0,157,0,
152,80,158,81,82,0,0,37,0,159,160,161,162,0,0,0,83,163,84,85,164,0,0,0,0,86,165,
166,0,0,0,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,
0,0,0,168,0,0,169,0,0,0,0,285,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,
157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,301,0,0,
0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,
0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,440,0,0,0,0,168,0,
0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,
0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,529,0,169,0,
0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,
0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,0,554,
170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,
160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,596,0,169,0,0,0,0,0,170,
155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,
161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,0,664,170,155,
156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,
162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,0,0,169,0,0,0,0,677,170,155,156,0,
171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,
0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,167,81,82,0,0,37,0,0,0,696,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,
0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,
163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
167,81,82,0,0,37,0,0,0,698,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,
172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,
0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,
82,0,0,37,0,0,0,0,0,0,0,0,168,792,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,
0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,
0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,
37,0,0,0,0,0,0,0,0,168,795,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,
175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,
0,165,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,
798,0,0,0,0,168,0,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,
0,157,0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,
166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,
0,0,168,866,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,
0,0,0,158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,
870,0,169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,
158,0,0,0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,872,0,
169,0,0,0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,
0,0,0,0,159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,168,927,0,169,0,0,
0,0,0,170,155,156,0,171,0,0,172,173,0,0,174,175,0,77,0,157,0,0,0,158,0,0,0,0,0,0,
159,160,161,162,0,0,0,0,163,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,0,0,0,0,0,
0,0,0,0,0,0,77,0,0,0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,168,0,0,169,
164,0,0,0,0,170,165,166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,34,
80,167,81,82,0,0,37,0,0,0,0,301,155,156,0,168,0,0,169,0,0,0,0,0,170,77,0,0,171,0,
0,172,173,0,0,174,175,0,159,160,161,162,0,0,0,0,0,0,0,164,0,0,0,0,0,165,166,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,0,0,0,640,155,156,
0,168,0,0,169,0,0,0,0,0,170,77,0,0,171,0,0,172,173,0,0,174,175,0,159,160,161,162,
0,0,0,0,0,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,0,0,0,0,0,0,0,0,0,0,0,77,0,0,
0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,296,0,0,169,164,0,0,0,0,170,165,
166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,34,80,167,81,82,0,0,37,0,
0,0,0,0,155,156,0,310,0,0,169,0,0,0,0,0,170,77,0,0,171,0,0,172,173,0,0,174,175,0,
159,160,161,162,0,0,0,0,0,0,0,164,0,0,0,0,0,165,166,0,0,0,0,155,156,0,0,0,0,0,0,
0,0,0,0,0,77,0,0,0,34,80,167,81,82,0,0,37,0,159,160,161,162,0,0,0,168,0,0,169,
164,0,0,0,0,170,165,166,0,171,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,34,
80,167,81,82,0,0,37,0,0,0,0,0,0,0,0,835,0,0,169,0,0,0,0,0,170,0,0,0,171,0,0,172,
173,0,0,174,175,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,204,0,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,35,36,0,
38,39,40,0,205,0,27,28,0,22,42,43,29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,0,0,0,0,0,35,36,0,38,39,40,278,613,0,0,0,0,0,
42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,22,
0,0,29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,0,0,0,39,40,0,0,0,0,0,0,0,
42,43};static short Cyc_yycheck[5864]={0,236,388,64,158,118,158,157,61,261,262,325,
264,135,369,168,135,357,322,354,327,21,105,358,207,328,329,324,151,334,335,224,
534,17,71,135,71,459,0,102,103,41,5,6,77,336,337,404,48,421,23,400,64,91,54,55,
56,118,58,21,77,313,93,102,64,65,144,0,107,130,105,132,383,111,170,171,172,426,
464,95,96,97,119,120,119,118,48,91,21,400,106,60,54,55,56,191,58,126,98,99,457,
118,64,111,444,78,118,91,111,466,110,111,112,113,87,48,488,104,118,89,132,54,55,
56,546,58,63,96,108,209,130,64,132,120,103,164,98,99,640,139,120,104,292,296,224,
298,146,117,110,111,112,113,487,91,103,120,118,310,107,124,98,96,96,212,213,98,
99,290,130,107,132,224,110,202,109,109,237,110,111,112,113,281,91,111,146,118,21,
102,105,242,105,72,73,376,377,378,111,130,115,132,261,262,111,264,397,114,143,
207,580,96,72,73,108,146,96,244,48,104,467,54,55,56,104,58,120,237,111,420,113,
64,111,330,331,332,333,284,120,237,119,288,124,102,338,339,340,207,586,91,91,111,
102,23,313,97,98,99,491,119,107,102,90,261,262,107,264,108,98,99,205,112,114,421,
90,628,107,237,91,91,110,111,112,113,97,118,98,111,67,287,623,91,67,68,69,107,91,
130,98,132,653,261,262,98,264,552,237,102,78,116,102,93,107,105,101,313,146,87,
125,126,107,116,117,118,111,406,102,114,102,105,261,262,107,264,581,362,105,103,
113,114,488,452,105,278,91,108,397,105,499,400,108,98,248,313,123,91,103,371,254,
714,105,97,98,99,103,91,135,112,104,420,103,97,98,99,565,426,107,436,376,377,378,
107,313,114,737,576,105,385,114,108,116,117,118,452,107,628,116,101,369,397,427,
399,91,107,237,125,126,111,502,83,114,105,410,91,412,662,376,377,378,97,98,99,
420,106,0,103,196,674,261,262,209,264,580,203,105,107,106,206,436,208,557,112,
452,113,114,224,22,23,568,25,410,568,412,388,452,469,108,533,102,557,105,708,105,
242,104,41,713,91,106,822,568,240,48,97,98,99,436,53,476,410,313,412,558,107,636,
61,636,102,103,285,66,724,452,569,168,879,644,102,111,75,105,105,78,105,737,436,
102,605,112,105,112,87,91,473,565,613,476,107,97,98,99,452,98,99,459,576,102,201,
579,464,74,75,844,845,110,111,112,113,788,841,105,846,122,108,603,476,847,848,
843,445,853,854,128,449,107,582,105,133,556,108,107,155,107,157,158,536,855,856,
539,107,146,165,166,107,572,151,170,171,172,253,410,101,412,116,117,118,41,107,
701,822,840,111,606,603,114,91,92,191,53,369,647,97,381,436,76,603,107,940,91,92,
105,546,381,108,97,121,663,768,397,452,196,105,296,621,108,63,64,203,397,676,304,
70,71,880,308,404,310,406,123,105,103,420,108,603,91,107,476,102,224,323,102,764,
114,105,116,117,118,617,107,235,105,621,108,108,105,716,242,108,819,104,246,107,
603,104,609,769,472,106,133,104,102,104,653,105,835,359,837,263,457,102,621,103,
105,787,944,104,105,466,292,91,102,105,107,105,108,97,98,99,137,101,105,107,810,
108,290,107,105,5,6,108,691,834,22,23,116,117,118,840,322,788,324,325,326,327,
328,329,330,331,332,333,334,335,336,337,338,339,340,105,829,91,108,107,849,850,
851,852,430,431,554,102,434,102,436,857,858,859,536,107,102,539,603,105,102,105,
448,105,108,112,91,105,235,840,108,896,935,98,565,104,557,91,788,621,688,369,107,
943,565,576,694,568,579,895,106,116,117,118,112,236,263,78,79,80,81,82,83,84,85,
86,87,105,105,397,108,108,400,108,105,421,788,108,108,944,883,91,410,115,412,106,
120,97,98,99,840,104,420,958,17,18,19,107,426,101,428,104,112,104,105,788,116,
117,118,104,105,91,660,91,92,93,664,97,98,99,301,376,377,378,305,306,307,107,840,
677,119,653,119,944,77,316,116,117,118,320,122,123,488,107,792,74,75,795,476,70,
71,108,91,111,802,803,502,840,108,98,681,105,101,108,590,591,104,105,107,691,105,
352,761,762,104,107,357,116,117,118,108,105,102,105,832,112,103,103,107,30,102,
42,879,714,944,96,91,107,746,108,108,382,120,788,91,105,105,633,536,757,108,539,
105,105,104,96,104,865,866,108,105,869,870,107,872,406,108,105,38,108,103,108,
944,108,580,115,108,565,102,108,102,115,103,119,104,25,26,103,576,119,115,579,
111,25,108,840,108,115,38,108,108,944,108,444,108,108,76,103,108,695,696,108,698,
921,700,104,924,115,926,927,108,91,92,104,94,95,112,102,98,91,636,102,96,621,720,
108,76,107,644,109,110,949,950,108,107,105,116,108,829,92,491,91,92,123,94,95,
108,104,98,107,28,103,107,112,76,653,121,107,76,109,110,104,96,25,105,104,116,
105,104,102,120,108,768,123,124,119,108,127,128,107,92,104,108,534,105,91,0,0,
686,944,883,108,108,691,108,108,104,108,794,108,564,797,798,681,240,66,579,45,
559,617,235,879,143,613,609,492,203,653,716,570,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,691,586,61,- 1,- 1,835,- 1,326,- 1,- 1,- 1,- 1,842,- 1,- 1,37,766,- 1,- 1,769,- 1,-
1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,860,- 1,- 1,55,56,57,- 1,787,- 1,- 1,- 1,- 1,- 1,628,874,- 1,- 1,
- 1,- 1,- 1,635,- 1,- 1,- 1,884,640,- 1,- 1,643,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,- 1,-
1,- 1,96,97,- 1,99,100,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,841,- 1,843,844,845,
846,847,848,849,850,851,852,853,854,855,856,857,858,859,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,724,- 1,- 1,- 1,- 1,-
1,895,- 1,- 1,- 1,- 1,- 1,883,737,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,943,44,45,46,- 1,-
1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,822,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,
101,102,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,
53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,102,103,104,
- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,
32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,
59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,107,- 1,109,
110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,
92,93,94,95,96,97,98,99,100,101,102,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,
116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,
100,101,- 1,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,
56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,- 1,103,- 1,- 1,- 1,107,-
1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,
93,94,95,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,
- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,
- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,
98,99,100,101,- 1,- 1,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,0,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,- 1,109,110,- 1,
- 1,- 1,- 1,- 1,116,117,118,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,
49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,
- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
65,66,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,91,-
1,- 1,- 1,- 1,96,97,98,99,100,101,37,- 1,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,49,50,116,
117,118,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,37,- 1,- 1,96,97,98,99,100,101,102,- 1,- 1,
49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,37,- 1,- 1,96,97,- 1,99,
100,101,102,- 1,- 1,49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,
- 1,- 1,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,
118,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,103,- 1,37,106,39,- 1,109,110,- 1,
- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,96,97,- 1,99,
100,101,45,103,- 1,- 1,49,50,- 1,109,110,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,25,26,27,38,29,30,31,32,33,
34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,58,59,- 1,
61,62,- 1,- 1,- 1,76,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,92,- 1,94,95,- 1,- 1,
98,- 1,- 1,91,92,93,94,95,- 1,107,98,109,110,- 1,102,103,- 1,- 1,116,107,- 1,- 1,110,- 1,
- 1,123,- 1,- 1,116,- 1,25,26,120,- 1,- 1,123,124,32,- 1,127,128,- 1,- 1,38,- 1,40,- 1,- 1,-
1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,
- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,76,120,
- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,88,40,- 1,91,92,44,94,95,- 1,- 1,98,- 1,51,52,53,
54,- 1,- 1,- 1,107,59,109,110,62,- 1,113,114,- 1,116,68,69,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,-
1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,
104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,76,120,- 1,- 1,123,124,- 1,- 1,
127,128,- 1,38,88,40,- 1,91,92,44,94,95,- 1,- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,59,
109,110,62,- 1,113,114,- 1,116,68,69,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,- 1,- 1,
110,- 1,- 1,113,114,- 1,116,25,26,76,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,88,40,- 1,
91,92,44,94,95,- 1,- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,59,109,110,62,- 1,- 1,- 1,- 1,
116,68,69,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,
93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,
26,76,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,91,92,44,94,95,- 1,- 1,98,- 1,
51,52,53,54,- 1,- 1,- 1,107,59,109,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,- 1,- 1,- 1,123,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,
127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,
- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,
- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,
- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,-
1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,
- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,
- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,
- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,-
1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,-
1,110,- 1,- 1,- 1,- 1,115,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,-
1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,
120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,-
1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,
92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,
25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,
128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,
- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,
- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,-
1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,-
1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,
- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,
- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,
- 1,- 1,- 1,- 1,- 1,116,25,26,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,51,52,53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,-
1,- 1,- 1,- 1,103,25,26,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,123,
124,- 1,- 1,127,128,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,- 1,103,25,26,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,
- 1,- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,-
1,- 1,123,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,38,- 1,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,
- 1,- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,- 1,120,-
1,- 1,123,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,120,- 1,- 1,
123,124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,
55,56,57,- 1,- 1,60,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,96,97,- 1,99,100,101,- 1,103,- 1,49,50,- 1,37,
109,110,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,102,88,- 1,
- 1,- 1,- 1,- 1,109,110,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,49,50,
- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,96,- 1,- 1,- 1,100,101,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,109,110};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _tagged_arr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*
f1;struct _tuple1*f2;int f3;};struct _tuple19{struct _tagged_arr f1;void*f2;};static
char _tmp43B[8]="stdcall";static char _tmp43C[6]="cdecl";static char _tmp43D[9]="fastcall";
static char _tmp43E[9]="noreturn";static char _tmp43F[6]="const";static char _tmp440[8]="aligned";
static char _tmp441[7]="packed";static char _tmp442[7]="shared";static char _tmp443[7]="unused";
static char _tmp444[5]="weak";static char _tmp445[10]="dllimport";static char _tmp446[
10]="dllexport";static char _tmp447[23]="no_instrument_function";static char _tmp448[
12]="constructor";static char _tmp449[11]="destructor";static char _tmp44A[22]="no_check_memory_usage";
static char _tmp44B[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmpA24=10000;short*_tmpA25=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmpA24));{unsigned int _tmpA26=
_tmpA24;unsigned int i;for(i=0;i < _tmpA26;i ++){_tmpA25[i]=(short)0;}}_tmpA25;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmpA21=10000;void**_tmpA22=(
void**)_cycalloc(_check_times(sizeof(void*),_tmpA21));{unsigned int _tmpA23=
_tmpA21;unsigned int i;for(i=0;i < _tmpA23;i ++){_tmpA22[i]=Cyc_yylval;}}_tmpA22;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmpA1E=
10000;struct Cyc_Yyltype*_tmpA1F=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmpA1E));{unsigned int _tmpA20=_tmpA1E;
unsigned int i;for(i=0;i < _tmpA20;i ++){_tmpA1F[i]=Cyc_yynewloc();}}_tmpA1F;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp36C="parser stack overflow";_tag_arr(_tmp36C,sizeof(char),_get_zero_arr_size(
_tmp36C,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(970,yystate)];if(yyn
== - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar
<= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar <= 356?(
int)Cyc_yytranslate[_check_known_subscript_notnull(357,Cyc_yychar)]: 268;}yyn +=
yychar1;if((yyn < 0  || yyn > 5863) || Cyc_yycheck[_check_known_subscript_notnull(
5864,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5864,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 969)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(970,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(487,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1E2: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp36D;_push_handler(& _tmp36D);{int _tmp36F=0;if(setjmp(_tmp36D.handler))
_tmp36F=1;if(!_tmp36F){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp36E=(void*)_exn_thrown;void*_tmp371=_tmp36E;_LL1E5: if(*((void**)_tmp371)
!= Cyc_Core_Failure)goto _LL1E7;_LL1E6: x=0;goto _LL1E4;_LL1E7:;_LL1E8:(void)_throw(
_tmp371);_LL1E4:;}}}{struct _handler_cons _tmp372;_push_handler(& _tmp372);{int
_tmp374=0;if(setjmp(_tmp372.handler))_tmp374=1;if(!_tmp374){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp373=(void*)_exn_thrown;void*_tmp376=_tmp373;_LL1EA: if(*((void**)_tmp376)
!= Cyc_Core_Failure)goto _LL1EC;_LL1EB: y=0;goto _LL1E9;_LL1EC:;_LL1ED:(void)_throw(
_tmp376);_LL1E9:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp377=
_cycalloc(sizeof(*_tmp377));_tmp377[0]=({struct Cyc_DeclList_tok_struct _tmp378;
_tmp378.tag=Cyc_DeclList_tok;_tmp378.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp378;});_tmp377;});break;}
case 3: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp379=_cycalloc(
sizeof(*_tmp379));_tmp379[0]=({struct Cyc_DeclList_tok_struct _tmp37A;_tmp37A.tag=
Cyc_DeclList_tok;_tmp37A.f1=({struct Cyc_List_List*_tmp37B=_cycalloc(sizeof(*
_tmp37B));_tmp37B->hd=({struct Cyc_Absyn_Decl*_tmp37C=_cycalloc(sizeof(*_tmp37C));
_tmp37C->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp37D=_cycalloc(
sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_Absyn_Using_d_struct _tmp37E;_tmp37E.tag=
9;_tmp37E.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp37E.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37E;});_tmp37D;}));
_tmp37C->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp37C;});_tmp37B->tl=0;_tmp37B;});_tmp37A;});
_tmp379;});Cyc_Lex_leave_using();break;case 4: _LL1EE: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_DeclList_tok_struct
_tmp380;_tmp380.tag=Cyc_DeclList_tok;_tmp380.f1=({struct Cyc_List_List*_tmp381=
_cycalloc(sizeof(*_tmp381));_tmp381->hd=({struct Cyc_Absyn_Decl*_tmp382=_cycalloc(
sizeof(*_tmp382));_tmp382->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Absyn_Using_d_struct
_tmp384;_tmp384.tag=9;_tmp384.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp384.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp384;});_tmp383;}));
_tmp382->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp382;});_tmp381->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp381;});_tmp380;});
_tmp37F;});break;case 5: _LL1EF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({struct Cyc_DeclList_tok_struct
_tmp386;_tmp386.tag=Cyc_DeclList_tok;_tmp386.f1=({struct Cyc_List_List*_tmp387=
_cycalloc(sizeof(*_tmp387));_tmp387->hd=({struct Cyc_Absyn_Decl*_tmp388=_cycalloc(
sizeof(*_tmp388));_tmp388->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp38A;_tmp38A.tag=8;_tmp38A.f1=({struct _tagged_arr*_tmp38B=_cycalloc(sizeof(*
_tmp38B));_tmp38B[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp38B;});_tmp38A.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp38A;});_tmp389;}));
_tmp388->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp388;});_tmp387->tl=0;_tmp387;});_tmp386;});
_tmp385;});Cyc_Lex_leave_namespace();break;case 6: _LL1F0: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp38C;
_push_handler(& _tmp38C);{int _tmp38E=0;if(setjmp(_tmp38C.handler))_tmp38E=1;if(!
_tmp38E){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp38D=(void*)_exn_thrown;void*
_tmp390=_tmp38D;_LL1F3: if(*((void**)_tmp390)!= Cyc_Core_Failure)goto _LL1F5;
_LL1F4: nspace=({const char*_tmp391="";_tag_arr(_tmp391,sizeof(char),
_get_zero_arr_size(_tmp391,1));});goto _LL1F2;_LL1F5:;_LL1F6:(void)_throw(_tmp390);
_LL1F2:;}}}{struct _handler_cons _tmp392;_push_handler(& _tmp392);{int _tmp394=0;if(
setjmp(_tmp392.handler))_tmp394=1;if(!_tmp394){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp393=(void*)_exn_thrown;void*_tmp396=_tmp393;_LL1F8: if(*((void**)_tmp396)
!= Cyc_Core_Failure)goto _LL1FA;_LL1F9: x=0;goto _LL1F7;_LL1FA:;_LL1FB:(void)_throw(
_tmp396);_LL1F7:;}}}{struct _handler_cons _tmp397;_push_handler(& _tmp397);{int
_tmp399=0;if(setjmp(_tmp397.handler))_tmp399=1;if(!_tmp399){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp398=(void*)_exn_thrown;void*_tmp39B=_tmp398;_LL1FD: if(*((void**)_tmp39B)
!= Cyc_Core_Failure)goto _LL1FF;_LL1FE: y=0;goto _LL1FC;_LL1FF:;_LL200:(void)_throw(
_tmp39B);_LL1FC:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp39C=
_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_DeclList_tok_struct _tmp39D;
_tmp39D.tag=Cyc_DeclList_tok;_tmp39D.f1=({struct Cyc_List_List*_tmp39E=_cycalloc(
sizeof(*_tmp39E));_tmp39E->hd=({struct Cyc_Absyn_Decl*_tmp39F=_cycalloc(sizeof(*
_tmp39F));_tmp39F->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A0=
_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A1;_tmp3A1.tag=8;_tmp3A1.f1=({struct _tagged_arr*_tmp3A2=_cycalloc(sizeof(*
_tmp3A2));_tmp3A2[0]=nspace;_tmp3A2;});_tmp3A1.f2=x;_tmp3A1;});_tmp3A0;}));
_tmp39F->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp39F;});_tmp39E->tl=y;_tmp39E;});_tmp39D;});
_tmp39C;});break;}case 7: _LL1F1: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3A3="C";
_tag_arr(_tmp3A3,sizeof(char),_get_zero_arr_size(_tmp3A3,2));}))== 0)yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({
struct Cyc_DeclList_tok_struct _tmp3A5;_tmp3A5.tag=Cyc_DeclList_tok;_tmp3A5.f1=({
struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->hd=({struct Cyc_Absyn_Decl*
_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3A9;_tmp3A9.tag=10;_tmp3A9.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A9;});_tmp3A8;}));
_tmp3A7->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3A7;});_tmp3A6->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A6;});_tmp3A5;});
_tmp3A4;});else{if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3AA="C include";
_tag_arr(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3AB="expecting \"C\" or \"C include\"";_tag_arr(_tmp3AB,sizeof(
char),_get_zero_arr_size(_tmp3AB,29));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({
struct Cyc_DeclList_tok_struct _tmp3AD;_tmp3AD.tag=Cyc_DeclList_tok;_tmp3AD.f1=({
struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->hd=({struct Cyc_Absyn_Decl*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3B1;_tmp3B1.tag=11;_tmp3B1.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B1.f2=0;_tmp3B1;});
_tmp3B0;}));_tmp3AF->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3AF;});
_tmp3AE->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3AE;});_tmp3AD;});_tmp3AC;});}break;case 8: _LL201: if(Cyc_strcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3B2="C include";_tag_arr(_tmp3B2,sizeof(char),
_get_zero_arr_size(_tmp3B2,10));}))!= 0)Cyc_Parse_err(({const char*_tmp3B3="expecting \"C include\"";
_tag_arr(_tmp3B3,sizeof(char),_get_zero_arr_size(_tmp3B3,22));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));
_tmp3B4[0]=({struct Cyc_DeclList_tok_struct _tmp3B5;_tmp3B5.tag=Cyc_DeclList_tok;
_tmp3B5.f1=({struct Cyc_List_List*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->hd=({
struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->r=(void*)((
void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3B8=_cycalloc(sizeof(*
_tmp3B8));_tmp3B8[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3B9;_tmp3B9.tag=
11;_tmp3B9.f1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3B9.f2=exs;_tmp3B9;});_tmp3B8;}));_tmp3B7->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B7;});
_tmp3B6->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B6;});_tmp3B5;});_tmp3B4;});break;}case 9: _LL202: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA[0]=({
struct Cyc_DeclList_tok_struct _tmp3BB;_tmp3BB.tag=Cyc_DeclList_tok;_tmp3BB.f1=({
struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=({struct Cyc_Absyn_Decl*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->r=(void*)((void*)0);_tmp3BD->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3BD;});_tmp3BC->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BC;});_tmp3BB;});
_tmp3BA;});break;case 10: _LL203: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_DeclList_tok_struct
_tmp3BF;_tmp3BF.tag=Cyc_DeclList_tok;_tmp3BF.f1=({struct Cyc_List_List*_tmp3C0=
_cycalloc(sizeof(*_tmp3C0));_tmp3C0->hd=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(
sizeof(*_tmp3C1));_tmp3C1->r=(void*)((void*)1);_tmp3C1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C1;});
_tmp3C0->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C0;});_tmp3BF;});_tmp3BE;});break;case 11: _LL204: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({
struct Cyc_DeclList_tok_struct _tmp3C3;_tmp3C3.tag=Cyc_DeclList_tok;_tmp3C3.f1=0;
_tmp3C3;});_tmp3C2;});break;case 12: _LL205: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 13: _LL206: yyval=(
void*)({struct Cyc_YY7_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[0]=({
struct Cyc_YY7_struct _tmp3C5;_tmp3C5.tag=Cyc_YY7;_tmp3C5.f1=0;_tmp3C5;});_tmp3C4;});
break;case 14: _LL207: yyval=(void*)({struct Cyc_YY7_struct*_tmp3C6=_cycalloc(
sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_YY7_struct _tmp3C7;_tmp3C7.tag=Cyc_YY7;
_tmp3C7.f1=({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->hd=({
struct _tuple18*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C9->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3C9->f3=0;_tmp3C9;});_tmp3C8->tl=0;_tmp3C8;});_tmp3C7;});_tmp3C6;});break;
case 15: _LL208: yyval=(void*)({struct Cyc_YY7_struct*_tmp3CA=_cycalloc(sizeof(*
_tmp3CA));_tmp3CA[0]=({struct Cyc_YY7_struct _tmp3CB;_tmp3CB.tag=Cyc_YY7;_tmp3CB.f1=({
struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->hd=({struct
_tuple18*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3CD->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3CD->f3=0;_tmp3CD;});_tmp3CC->tl=Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CC;});_tmp3CB;});
_tmp3CA;});break;case 16: _LL209: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE[0]=({struct Cyc_DeclList_tok_struct
_tmp3CF;_tmp3CF.tag=Cyc_DeclList_tok;_tmp3CF.f1=({struct Cyc_List_List*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3D2;_tmp3D2.tag=1;_tmp3D2.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D2;});_tmp3D1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D0->tl=0;
_tmp3D0;});_tmp3CF;});_tmp3CE;});break;case 17: _LL20A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL20B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));
_tmp3D3[0]=({struct Cyc_DeclList_tok_struct _tmp3D4;_tmp3D4.tag=Cyc_DeclList_tok;
_tmp3D4.f1=0;_tmp3D4;});_tmp3D3;});break;case 19: _LL20C: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D6;_tmp3D6.tag=Cyc_FnDecl_tok;_tmp3D6.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D6;});
_tmp3D5;});break;case 20: _LL20D: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D8;_tmp3D8.tag=Cyc_FnDecl_tok;_tmp3D8.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3D9;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D8;});
_tmp3D7;});break;case 21: _LL20E: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DB;_tmp3DB.tag=Cyc_FnDecl_tok;_tmp3DB.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DB;});
_tmp3DA;});break;case 22: _LL20F: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DD;_tmp3DD.tag=Cyc_FnDecl_tok;_tmp3DD.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3DE;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DD;});
_tmp3DC;});break;case 23: _LL210: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E0;_tmp3E0.tag=Cyc_FnDecl_tok;_tmp3E0.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E1;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E0;});
_tmp3DF;});break;case 24: _LL211: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E3;_tmp3E3.tag=Cyc_FnDecl_tok;_tmp3E3.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E4;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E3;});
_tmp3E2;});break;case 25: _LL212: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL213: Cyc_Lex_leave_using();
break;case 27: _LL214: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp3E5=
_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E5;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL215: Cyc_Lex_leave_namespace();
break;case 29: _LL216: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3E6=
_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_DeclList_tok_struct _tmp3E7;
_tmp3E7.tag=Cyc_DeclList_tok;_tmp3E7.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3E7;});
_tmp3E6;});break;case 30: _LL217: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_DeclList_tok_struct
_tmp3E9;_tmp3E9.tag=Cyc_DeclList_tok;_tmp3E9.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9;});
_tmp3E8;});break;case 31: _LL218: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=({struct Cyc_DeclList_tok_struct
_tmp3EB;_tmp3EB.tag=Cyc_DeclList_tok;_tmp3EB.f1=({struct Cyc_List_List*_tmp3EC=
_cycalloc(sizeof(*_tmp3EC));_tmp3EC->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EC->tl=0;
_tmp3EC;});_tmp3EB;});_tmp3EA;});break;case 32: _LL219: {struct Cyc_List_List*
_tmp3ED=0;{struct Cyc_List_List*_tmp3EE=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3EE != 0;
_tmp3EE=_tmp3EE->tl){struct _tagged_arr*_tmp3EF=(struct _tagged_arr*)_tmp3EE->hd;
struct _tuple1*_tmp3F0=({struct _tuple1*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));
_tmp3F3->f1=Cyc_Absyn_rel_ns_null;_tmp3F3->f2=_tmp3EF;_tmp3F3;});struct Cyc_Absyn_Vardecl*
_tmp3F1=Cyc_Absyn_new_vardecl(_tmp3F0,Cyc_Absyn_wildtyp(0),0);_tmp3ED=({struct
Cyc_List_List*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->hd=_tmp3F1;_tmp3F2->tl=
_tmp3ED;_tmp3F2;});}}_tmp3ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3ED);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3F4=
_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=({struct Cyc_DeclList_tok_struct _tmp3F5;
_tmp3F5.tag=Cyc_DeclList_tok;_tmp3F5.f1=({struct Cyc_List_List*_tmp3F6=_cycalloc(
sizeof(*_tmp3F6));_tmp3F6->hd=Cyc_Absyn_letv_decl(_tmp3ED,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F6->tl=0;
_tmp3F6;});_tmp3F5;});_tmp3F4;});break;}case 33: _LL21A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL21B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));
_tmp3F7[0]=({struct Cyc_DeclList_tok_struct _tmp3F8;_tmp3F8.tag=Cyc_DeclList_tok;
_tmp3F8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3F8;});_tmp3F7;});
break;case 35: _LL21C: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3F9=
_cycalloc(sizeof(*_tmp3F9));_tmp3F9[0]=({struct Cyc_DeclSpec_tok_struct _tmp3FA;
_tmp3FA.tag=Cyc_DeclSpec_tok;_tmp3FA.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->sc=({struct Cyc_Core_Opt*_tmp3FC=
_cycalloc(sizeof(*_tmp3FC));_tmp3FC->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FC;});_tmp3FB->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FB->type_specs=0;_tmp3FB->is_inline=0;_tmp3FB->attributes=
0;_tmp3FB;});_tmp3FA;});_tmp3F9;});break;case 36: _LL21D: if((Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*
_tmp3FD[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp3FE="Only one storage class is allowed in a declaration";_tag_arr(_tmp3FE,
sizeof(char),_get_zero_arr_size(_tmp3FE,51));}),_tag_arr(_tmp3FD,sizeof(void*),0));});
yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));
_tmp3FF[0]=({struct Cyc_DeclSpec_tok_struct _tmp400;_tmp400.tag=Cyc_DeclSpec_tok;
_tmp400.f1=({struct Cyc_Parse_Declaration_spec*_tmp401=_cycalloc(sizeof(*_tmp401));
_tmp401->sc=({struct Cyc_Core_Opt*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp402;});_tmp401->tq=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp401->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp401->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp401->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp401;});
_tmp400;});_tmp3FF;});break;case 37: _LL21E: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_DeclSpec_tok_struct
_tmp404;_tmp404.tag=Cyc_DeclSpec_tok;_tmp404.f1=({struct Cyc_Parse_Declaration_spec*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->sc=0;_tmp405->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp405->type_specs=({struct Cyc_List_List*_tmp406=
_cycalloc(sizeof(*_tmp406));_tmp406->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp406->tl=0;_tmp406;});
_tmp405->is_inline=0;_tmp405->attributes=0;_tmp405;});_tmp404;});_tmp403;});
break;case 38: _LL21F: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp407=
_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_DeclSpec_tok_struct _tmp408;
_tmp408.tag=Cyc_DeclSpec_tok;_tmp408.f1=({struct Cyc_Parse_Declaration_spec*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp409->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp409->type_specs=({
struct Cyc_List_List*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40A->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40A;});
_tmp409->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp409->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp409;});
_tmp408;});_tmp407;});break;case 39: _LL220: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_DeclSpec_tok_struct
_tmp40C;_tmp40C.tag=Cyc_DeclSpec_tok;_tmp40C.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->sc=0;_tmp40D->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40D->type_specs=0;
_tmp40D->is_inline=0;_tmp40D->attributes=0;_tmp40D;});_tmp40C;});_tmp40B;});
break;case 40: _LL221: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp40E=
_cycalloc(sizeof(*_tmp40E));_tmp40E[0]=({struct Cyc_DeclSpec_tok_struct _tmp40F;
_tmp40F.tag=Cyc_DeclSpec_tok;_tmp40F.f1=({struct Cyc_Parse_Declaration_spec*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp410->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp410->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp410->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp410->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp410;});_tmp40F;});_tmp40E;});break;case 41: _LL222: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411[0]=({struct Cyc_DeclSpec_tok_struct
_tmp412;_tmp412.tag=Cyc_DeclSpec_tok;_tmp412.f1=({struct Cyc_Parse_Declaration_spec*
_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->sc=0;_tmp413->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp413->type_specs=0;_tmp413->is_inline=1;_tmp413->attributes=
0;_tmp413;});_tmp412;});_tmp411;});break;case 42: _LL223: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414[0]=({struct Cyc_DeclSpec_tok_struct
_tmp415;_tmp415.tag=Cyc_DeclSpec_tok;_tmp415.f1=({struct Cyc_Parse_Declaration_spec*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp416->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp416->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp416->is_inline=1;_tmp416->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp416;});
_tmp415;});_tmp414;});break;case 43: _LL224: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_DeclSpec_tok_struct
_tmp418;_tmp418.tag=Cyc_DeclSpec_tok;_tmp418.f1=({struct Cyc_Parse_Declaration_spec*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->sc=0;_tmp419->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp419->type_specs=0;_tmp419->is_inline=0;_tmp419->attributes=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp419;});_tmp418;});_tmp417;});break;case 44: _LL225: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({
struct Cyc_DeclSpec_tok_struct _tmp41B;_tmp41B.tag=Cyc_DeclSpec_tok;_tmp41B.f1=({
struct Cyc_Parse_Declaration_spec*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;
_tmp41C->tq=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq;_tmp41C->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp41C->is_inline=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;
_tmp41C->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp41C;});
_tmp41B;});_tmp41A;});break;case 45: _LL226: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_StorageClass_tok_struct
_tmp41E;_tmp41E.tag=Cyc_StorageClass_tok;_tmp41E.f1=(void*)((void*)4);_tmp41E;});
_tmp41D;});break;case 46: _LL227: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=({struct Cyc_StorageClass_tok_struct
_tmp420;_tmp420.tag=Cyc_StorageClass_tok;_tmp420.f1=(void*)((void*)5);_tmp420;});
_tmp41F;});break;case 47: _LL228: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_StorageClass_tok_struct
_tmp422;_tmp422.tag=Cyc_StorageClass_tok;_tmp422.f1=(void*)((void*)3);_tmp422;});
_tmp421;});break;case 48: _LL229: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_StorageClass_tok_struct
_tmp424;_tmp424.tag=Cyc_StorageClass_tok;_tmp424.f1=(void*)((void*)1);_tmp424;});
_tmp423;});break;case 49: _LL22A: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp425="C";
_tag_arr(_tmp425,sizeof(char),_get_zero_arr_size(_tmp425,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp426="only extern or extern \"C\" is allowed";_tag_arr(_tmp426,
sizeof(char),_get_zero_arr_size(_tmp426,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({
struct Cyc_StorageClass_tok_struct _tmp428;_tmp428.tag=Cyc_StorageClass_tok;
_tmp428.f1=(void*)((void*)2);_tmp428;});_tmp427;});break;case 50: _LL22B: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp429=_cycalloc(sizeof(*_tmp429));
_tmp429[0]=({struct Cyc_StorageClass_tok_struct _tmp42A;_tmp42A.tag=Cyc_StorageClass_tok;
_tmp42A.f1=(void*)((void*)0);_tmp42A;});_tmp429;});break;case 51: _LL22C: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));
_tmp42B[0]=({struct Cyc_StorageClass_tok_struct _tmp42C;_tmp42C.tag=Cyc_StorageClass_tok;
_tmp42C.f1=(void*)((void*)6);_tmp42C;});_tmp42B;});break;case 52: _LL22D: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));
_tmp42D[0]=({struct Cyc_AttributeList_tok_struct _tmp42E;_tmp42E.tag=Cyc_AttributeList_tok;
_tmp42E.f1=0;_tmp42E;});_tmp42D;});break;case 53: _LL22E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54: _LL22F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 55: _LL230:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp42F=_cycalloc(sizeof(*
_tmp42F));_tmp42F[0]=({struct Cyc_AttributeList_tok_struct _tmp430;_tmp430.tag=Cyc_AttributeList_tok;
_tmp430.f1=({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp431->tl=0;_tmp431;});_tmp430;});_tmp42F;});break;case 56:
_LL231: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp432=_cycalloc(
sizeof(*_tmp432));_tmp432[0]=({struct Cyc_AttributeList_tok_struct _tmp433;_tmp433.tag=
Cyc_AttributeList_tok;_tmp433.f1=({struct Cyc_List_List*_tmp434=_cycalloc(sizeof(*
_tmp434));_tmp434->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp434->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp434;});_tmp433;});
_tmp432;});break;case 57: _LL232: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple19 att_map[17]={{{_tmp43B,_tmp43B,_tmp43B + 8},(
void*)0},{{_tmp43C,_tmp43C,_tmp43C + 6},(void*)1},{{_tmp43D,_tmp43D,_tmp43D + 9},(
void*)2},{{_tmp43E,_tmp43E,_tmp43E + 9},(void*)3},{{_tmp43F,_tmp43F,_tmp43F + 6},(
void*)4},{{_tmp440,_tmp440,_tmp440 + 8},(void*)& att_aligned},{{_tmp441,_tmp441,
_tmp441 + 7},(void*)5},{{_tmp442,_tmp442,_tmp442 + 7},(void*)7},{{_tmp443,_tmp443,
_tmp443 + 7},(void*)8},{{_tmp444,_tmp444,_tmp444 + 5},(void*)9},{{_tmp445,_tmp445,
_tmp445 + 10},(void*)10},{{_tmp446,_tmp446,_tmp446 + 10},(void*)11},{{_tmp447,
_tmp447,_tmp447 + 23},(void*)12},{{_tmp448,_tmp448,_tmp448 + 12},(void*)13},{{
_tmp449,_tmp449,_tmp449 + 11},(void*)14},{{_tmp44A,_tmp44A,_tmp44A + 22},(void*)15},{{
_tmp44B,_tmp44B,_tmp44B + 5},(void*)16}};struct _tagged_arr _tmp435=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp435,sizeof(char))> 4  && *((const char*)_check_unknown_subscript(_tmp435,
sizeof(char),0))== '_') && *((const char*)_check_unknown_subscript(_tmp435,
sizeof(char),1))== '_') && *((const char*)_check_unknown_subscript(_tmp435,
sizeof(char),(int)(_get_arr_size(_tmp435,sizeof(char))- 2)))== '_') && *((const
char*)_check_unknown_subscript(_tmp435,sizeof(char),(int)(_get_arr_size(_tmp435,
sizeof(char))- 3)))== '_')_tmp435=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp435,2,_get_arr_size(_tmp435,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp435,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp436=_cycalloc(sizeof(*
_tmp436));_tmp436[0]=({struct Cyc_Attribute_tok_struct _tmp437;_tmp437.tag=Cyc_Attribute_tok;
_tmp437.f1=(void*)(att_map[i]).f2;_tmp437;});_tmp436;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp438="unrecognized attribute";_tag_arr(_tmp438,sizeof(char),
_get_zero_arr_size(_tmp438,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({
struct Cyc_Attribute_tok_struct _tmp43A;_tmp43A.tag=Cyc_Attribute_tok;_tmp43A.f1=(
void*)((void*)1);_tmp43A;});_tmp439;});}break;}}case 58: _LL233: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({
struct Cyc_Attribute_tok_struct _tmp44E;_tmp44E.tag=Cyc_Attribute_tok;_tmp44E.f1=(
void*)((void*)4);_tmp44E;});_tmp44D;});break;case 59: _LL234: {struct _tagged_arr
_tmp44F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp451;int _tmp452;struct _tuple13*_tmp450=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp451=*_tmp450;
_tmp452=_tmp451.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({const char*
_tmp453="regparm";_tag_arr(_tmp453,sizeof(char),_get_zero_arr_size(_tmp453,8));}))
== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({const char*_tmp454="__regparm__";
_tag_arr(_tmp454,sizeof(char),_get_zero_arr_size(_tmp454,12));}))== 0){if(
_tmp452 < 0  || _tmp452 > 3)Cyc_Parse_err(({const char*_tmp455="regparm requires value between 0 and 3";
_tag_arr(_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp456=_cycalloc_atomic(sizeof(*_tmp456));
_tmp456[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp457;_tmp457.tag=0;_tmp457.f1=
_tmp452;_tmp457;});_tmp456;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({
const char*_tmp458="aligned";_tag_arr(_tmp458,sizeof(char),_get_zero_arr_size(
_tmp458,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({const char*
_tmp459="__aligned__";_tag_arr(_tmp459,sizeof(char),_get_zero_arr_size(_tmp459,
12));}))== 0){if(_tmp452 < 0)Cyc_Parse_err(({const char*_tmp45A="aligned requires positive power of two";
_tag_arr(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp452;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp45B="aligned requires positive power of two";
_tag_arr(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp45C=_cycalloc_atomic(sizeof(*_tmp45C));
_tmp45C[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp45D;_tmp45D.tag=1;_tmp45D.f1=
_tmp452;_tmp45D;});_tmp45C;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({
const char*_tmp45E="initializes";_tag_arr(_tmp45E,sizeof(char),_get_zero_arr_size(
_tmp45E,12));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp44F,({const char*
_tmp45F="__initializes__";_tag_arr(_tmp45F,sizeof(char),_get_zero_arr_size(
_tmp45F,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp460=
_cycalloc_atomic(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp461;_tmp461.tag=4;_tmp461.f1=_tmp452;_tmp461;});_tmp460;});else{Cyc_Parse_err(({
const char*_tmp462="unrecognized attribute";_tag_arr(_tmp462,sizeof(char),
_get_zero_arr_size(_tmp462,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp463=_cycalloc(sizeof(*_tmp463));
_tmp463[0]=({struct Cyc_Attribute_tok_struct _tmp464;_tmp464.tag=Cyc_Attribute_tok;
_tmp464.f1=(void*)a;_tmp464;});_tmp463;});break;}}case 60: _LL235: {struct
_tagged_arr _tmp465=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp466=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp465,({const char*_tmp467="section";_tag_arr(_tmp467,sizeof(
char),_get_zero_arr_size(_tmp467,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)
_tmp465,({const char*_tmp468="__section__";_tag_arr(_tmp468,sizeof(char),
_get_zero_arr_size(_tmp468,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_Absyn_Section_att_struct
_tmp46A;_tmp46A.tag=2;_tmp46A.f1=_tmp466;_tmp46A;});_tmp469;});else{Cyc_Parse_err(({
const char*_tmp46B="unrecognized attribute";_tag_arr(_tmp46B,sizeof(char),
_get_zero_arr_size(_tmp46B,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));
_tmp46C[0]=({struct Cyc_Attribute_tok_struct _tmp46D;_tmp46D.tag=Cyc_Attribute_tok;
_tmp46D.f1=(void*)a;_tmp46D;});_tmp46C;});break;}case 61: _LL236: {struct
_tagged_arr _tmp46E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp46F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp46E,({const char*_tmp470="__mode__";_tag_arr(_tmp470,
sizeof(char),_get_zero_arr_size(_tmp470,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp472;_tmp472.tag=5;_tmp472.f1=_tmp46F;_tmp472;});_tmp471;});else{Cyc_Parse_err(({
const char*_tmp473="unrecognized attribute";_tag_arr(_tmp473,sizeof(char),
_get_zero_arr_size(_tmp473,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp474=_cycalloc(sizeof(*_tmp474));
_tmp474[0]=({struct Cyc_Attribute_tok_struct _tmp475;_tmp475.tag=Cyc_Attribute_tok;
_tmp475.f1=(void*)a;_tmp475;});_tmp474;});break;}case 62: _LL237: {struct
_tagged_arr _tmp476=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp477=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp479;struct
_tuple13 _tmp478=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp479=_tmp478.f2;{int _tmp47B;struct _tuple13 _tmp47A=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47B=_tmp47A.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp476,({const char*_tmp47C="format";
_tag_arr(_tmp47C,sizeof(char),_get_zero_arr_size(_tmp47C,7));}))== 0  || Cyc_zstrcmp((
struct _tagged_arr)_tmp476,({const char*_tmp47D="__format__";_tag_arr(_tmp47D,
sizeof(char),_get_zero_arr_size(_tmp47D,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp477,({const char*_tmp47E="printf";_tag_arr(_tmp47E,sizeof(char),
_get_zero_arr_size(_tmp47E,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_Absyn_Format_att_struct
_tmp480;_tmp480.tag=3;_tmp480.f1=(void*)((void*)0);_tmp480.f2=_tmp479;_tmp480.f3=
_tmp47B;_tmp480;});_tmp47F;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp477,({
const char*_tmp481="scanf";_tag_arr(_tmp481,sizeof(char),_get_zero_arr_size(
_tmp481,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp482=
_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_Format_att_struct
_tmp483;_tmp483.tag=3;_tmp483.f1=(void*)((void*)1);_tmp483.f2=_tmp479;_tmp483.f3=
_tmp47B;_tmp483;});_tmp482;});else{Cyc_Parse_err(({const char*_tmp484="unrecognized format type";
_tag_arr(_tmp484,sizeof(char),_get_zero_arr_size(_tmp484,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp485="unrecognized attribute";_tag_arr(_tmp485,sizeof(char),
_get_zero_arr_size(_tmp485,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({
struct Cyc_Attribute_tok_struct _tmp487;_tmp487.tag=Cyc_Attribute_tok;_tmp487.f1=(
void*)a;_tmp487;});_tmp486;});break;}}}case 63: _LL238: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL239: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp488=_cycalloc(sizeof(*_tmp488));
_tmp488[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp489;_tmp489.tag=Cyc_TypeSpecifier_tok;
_tmp489.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp48B;_tmp48B.tag=16;_tmp48B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48B.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp48B.f3=0;_tmp48B.f4=
0;_tmp48B;});_tmp48A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp489;});
_tmp488;});break;case 65: _LL23A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48D;_tmp48D.tag=Cyc_TypeSpecifier_tok;_tmp48D.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48D;});_tmp48C;});break;case 66: _LL23B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));
_tmp48E[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp48F;_tmp48F.tag=Cyc_TypeSpecifier_tok;
_tmp48F.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48F;});
_tmp48E;});break;case 67: _LL23C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp491;_tmp491.tag=Cyc_TypeSpecifier_tok;_tmp491.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp492=_cycalloc(sizeof(*_tmp492));
_tmp492->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp492;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp491;});
_tmp490;});break;case 68: _LL23D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp494;_tmp494.tag=Cyc_TypeSpecifier_tok;_tmp494.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp494;});
_tmp493;});break;case 69: _LL23E: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp496;_tmp496.tag=Cyc_TypeSpecifier_tok;_tmp496.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Parse_Short_spec_struct
_tmp498;_tmp498.tag=2;_tmp498.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp498;});
_tmp497;}));_tmp496;});_tmp495;});break;case 70: _LL23F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49A;_tmp49A.tag=Cyc_TypeSpecifier_tok;_tmp49A.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49A;});
_tmp499;});break;case 71: _LL240: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49C;_tmp49C.tag=Cyc_TypeSpecifier_tok;_tmp49C.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=({struct Cyc_Parse_Long_spec_struct
_tmp49E;_tmp49E.tag=3;_tmp49E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49E;});
_tmp49D;}));_tmp49C;});_tmp49B;});break;case 72: _LL241: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_TypeSpecifier_tok;_tmp4A0.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A0;});
_tmp49F;});break;case 73: _LL242: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A2;_tmp4A2.tag=Cyc_TypeSpecifier_tok;_tmp4A2.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A2;});
_tmp4A1;});break;case 74: _LL243: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A4;_tmp4A4.tag=Cyc_TypeSpecifier_tok;_tmp4A4.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp4A6;_tmp4A6.tag=0;_tmp4A6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A6;});
_tmp4A5;}));_tmp4A4;});_tmp4A3;});break;case 75: _LL244: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A8;_tmp4A8.tag=Cyc_TypeSpecifier_tok;_tmp4A8.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4AA;_tmp4AA.tag=1;_tmp4AA.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AA;});
_tmp4A9;}));_tmp4A8;});_tmp4A7;});break;case 76: _LL245: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL246: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL247:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL248: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AB=_cycalloc(
sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4AC;_tmp4AC.tag=
Cyc_TypeSpecifier_tok;_tmp4AC.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AC;});
_tmp4AB;});break;case 80: _LL249: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4AE;_tmp4AE.tag=Cyc_TypeSpecifier_tok;_tmp4AE.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));
_tmp4AF[0]=({struct Cyc_Absyn_TupleType_struct _tmp4B0;_tmp4B0.tag=9;_tmp4B0.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4B0;});_tmp4AF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AE;});_tmp4AD;});break;case 81: _LL24A: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));
_tmp4B1[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B2;_tmp4B2.tag=Cyc_TypeSpecifier_tok;
_tmp4B2.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4B4;_tmp4B4.tag=15;_tmp4B4.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B4;});_tmp4B3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B2;});_tmp4B1;});break;case 82: _LL24B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));
_tmp4B5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B6;_tmp4B6.tag=Cyc_TypeSpecifier_tok;
_tmp4B6.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp4B8;_tmp4B8.tag=14;_tmp4B8.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B8;});_tmp4B7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B6;});_tmp4B5;});break;case 83: _LL24C: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));
_tmp4B9[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BA;_tmp4BA.tag=Cyc_TypeSpecifier_tok;
_tmp4BA.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_Absyn_TagType_struct
_tmp4BC;_tmp4BC.tag=17;_tmp4BC.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BC;});_tmp4BB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4BA;});_tmp4B9;});break;case 84: _LL24D: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));
_tmp4BD[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BE;_tmp4BE.tag=Cyc_TypeSpecifier_tok;
_tmp4BE.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Absyn_TagType_struct
_tmp4C0;_tmp4C0.tag=17;_tmp4C0.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->v=(void*)((void*)5);_tmp4C1;}),0);
_tmp4C0;});_tmp4BF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BE;});
_tmp4BD;});break;case 85: _LL24E: {struct _tuple13 _tmp4C3;int _tmp4C4;struct _tuple13*
_tmp4C2=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4C3=*_tmp4C2;_tmp4C4=_tmp4C3.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4C6;_tmp4C6.tag=Cyc_TypeSpecifier_tok;_tmp4C6.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp4C7=_cycalloc_atomic(sizeof(*_tmp4C7));
_tmp4C7[0]=({struct Cyc_Absyn_TypeInt_struct _tmp4C8;_tmp4C8.tag=18;_tmp4C8.f1=
_tmp4C4;_tmp4C8;});_tmp4C7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C6;});
_tmp4C5;});break;}case 86: _LL24F: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp4C9=
_cycalloc(sizeof(*_tmp4C9));_tmp4C9[0]=({struct Cyc_Kind_tok_struct _tmp4CA;
_tmp4CA.tag=Cyc_Kind_tok;_tmp4CA.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CA;});
_tmp4C9;});break;case 87: _LL250: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_TypeQual_tok_struct
_tmp4CC;_tmp4CC.tag=Cyc_TypeQual_tok;_tmp4CC.f1=({struct Cyc_Absyn_Tqual _tmp4CD;
_tmp4CD.print_const=1;_tmp4CD.q_volatile=0;_tmp4CD.q_restrict=0;_tmp4CD.real_const=
1;_tmp4CD.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4CD;});_tmp4CC;});_tmp4CB;});break;case 88: _LL251:
yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));
_tmp4CE[0]=({struct Cyc_TypeQual_tok_struct _tmp4CF;_tmp4CF.tag=Cyc_TypeQual_tok;
_tmp4CF.f1=({struct Cyc_Absyn_Tqual _tmp4D0;_tmp4D0.print_const=0;_tmp4D0.q_volatile=
1;_tmp4D0.q_restrict=0;_tmp4D0.real_const=0;_tmp4D0.loc=0;_tmp4D0;});_tmp4CF;});
_tmp4CE;});break;case 89: _LL252: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_TypeQual_tok_struct
_tmp4D2;_tmp4D2.tag=Cyc_TypeQual_tok;_tmp4D2.f1=({struct Cyc_Absyn_Tqual _tmp4D3;
_tmp4D3.print_const=0;_tmp4D3.q_volatile=0;_tmp4D3.q_restrict=1;_tmp4D3.real_const=
0;_tmp4D3.loc=0;_tmp4D3;});_tmp4D2;});_tmp4D1;});break;case 90: _LL253: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4D5;_tmp4D5.tag=Cyc_TypeSpecifier_tok;
_tmp4D5.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4D6=_cycalloc(
sizeof(*_tmp4D6));_tmp4D6[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4D7;_tmp4D7.tag=
5;_tmp4D7.f1=({struct Cyc_Absyn_Decl*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));
_tmp4D9[0]=({struct Cyc_Absyn_Enum_d_struct _tmp4DA;_tmp4DA.tag=6;_tmp4DA.f1=({
struct Cyc_Absyn_Enumdecl*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->sc=(void*)((
void*)2);_tmp4DB->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4DB->fields=({struct Cyc_Core_Opt*_tmp4DC=_cycalloc(
sizeof(*_tmp4DC));_tmp4DC->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DC;});_tmp4DB;});
_tmp4DA;});_tmp4D9;}));_tmp4D8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4D8;});
_tmp4D7;});_tmp4D6;}));_tmp4D5;});_tmp4D4;});break;case 91: _LL254: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4DE;_tmp4DE.tag=Cyc_TypeSpecifier_tok;
_tmp4DE.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF[0]=({struct Cyc_Absyn_EnumType_struct
_tmp4E0;_tmp4E0.tag=12;_tmp4E0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E0.f2=0;_tmp4E0;});
_tmp4DF;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4DE;});_tmp4DD;});break;case 92: _LL255: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));
_tmp4E1[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4E2;_tmp4E2.tag=Cyc_TypeSpecifier_tok;
_tmp4E2.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4E3=_cycalloc(
sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_Parse_Type_spec_struct _tmp4E4;_tmp4E4.tag=
4;_tmp4E4.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp4E5=
_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4E6;_tmp4E6.tag=13;_tmp4E6.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E6;});_tmp4E5;}));
_tmp4E4.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4E4;});_tmp4E3;}));_tmp4E2;});_tmp4E1;});break;
case 93: _LL256: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Enumfield_tok_struct _tmp4E8;_tmp4E8.tag=
Cyc_Enumfield_tok;_tmp4E8.f1=({struct Cyc_Absyn_Enumfield*_tmp4E9=_cycalloc(
sizeof(*_tmp4E9));_tmp4E9->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E9->tag=0;_tmp4E9->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4E9;});_tmp4E8;});_tmp4E7;});break;case 94: _LL257:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));
_tmp4EA[0]=({struct Cyc_Enumfield_tok_struct _tmp4EB;_tmp4EB.tag=Cyc_Enumfield_tok;
_tmp4EB.f1=({struct Cyc_Absyn_Enumfield*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));
_tmp4EC->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4EC->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4EC;});
_tmp4EB;});_tmp4EA;});break;case 95: _LL258: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4EE;_tmp4EE.tag=Cyc_EnumfieldList_tok;_tmp4EE.f1=({struct Cyc_List_List*
_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EF->tl=0;_tmp4EF;});
_tmp4EE;});_tmp4ED;});break;case 96: _LL259: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F1;_tmp4F1.tag=Cyc_EnumfieldList_tok;_tmp4F1.f1=({struct Cyc_List_List*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F2->tl=0;_tmp4F2;});
_tmp4F1;});_tmp4F0;});break;case 97: _LL25A: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F4;_tmp4F4.tag=Cyc_EnumfieldList_tok;_tmp4F4.f1=({struct Cyc_List_List*
_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F5->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F5;});_tmp4F4;});
_tmp4F3;});break;case 98: _LL25B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4F7;_tmp4F7.tag=Cyc_TypeSpecifier_tok;_tmp4F7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));
_tmp4F8[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4F9;_tmp4F9.tag=11;_tmp4F9.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4F9.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F9;});_tmp4F8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F7;});_tmp4F6;});break;case 99: _LL25C: {struct
Cyc_List_List*_tmp4FA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4FB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4FD;_tmp4FD.tag=Cyc_TypeSpecifier_tok;
_tmp4FD.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4FE=_cycalloc(
sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4FF;_tmp4FF.tag=
5;_tmp4FF.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4FA,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4FB,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FF;});
_tmp4FE;}));_tmp4FD;});_tmp4FC;});break;}case 100: _LL25D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp501;_tmp501.tag=Cyc_TypeSpecifier_tok;_tmp501.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp502=_cycalloc(sizeof(*_tmp502));
_tmp502[0]=({struct Cyc_Absyn_AggrType_struct _tmp503;_tmp503.tag=10;_tmp503.f1=({
struct Cyc_Absyn_AggrInfo _tmp504;_tmp504.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp506;_tmp506.tag=0;_tmp506.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp506.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp506;});_tmp505;}));
_tmp504.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp504;});_tmp503;});_tmp502;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp501;});
_tmp500;});break;case 101: _LL25E: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507[0]=({struct Cyc_TypeList_tok_struct
_tmp508;_tmp508.tag=Cyc_TypeList_tok;_tmp508.f1=0;_tmp508;});_tmp507;});break;
case 102: _LL25F: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp509=_cycalloc(
sizeof(*_tmp509));_tmp509[0]=({struct Cyc_TypeList_tok_struct _tmp50A;_tmp50A.tag=
Cyc_TypeList_tok;_tmp50A.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp50A;});_tmp509;});
break;case 103: _LL260: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp50B=
_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp50C;_tmp50C.tag=Cyc_StructOrUnion_tok;_tmp50C.f1=(void*)((void*)0);_tmp50C;});
_tmp50B;});break;case 104: _LL261: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp50E;_tmp50E.tag=Cyc_StructOrUnion_tok;_tmp50E.f1=(void*)((void*)1);_tmp50E;});
_tmp50D;});break;case 105: _LL262: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp510;_tmp510.tag=Cyc_AggrFieldDeclList_tok;_tmp510.f1=0;_tmp510;});_tmp50F;});
break;case 106: _LL263: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp512;_tmp512.tag=Cyc_AggrFieldDeclList_tok;_tmp512.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp512;});_tmp511;});
break;case 107: _LL264: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp514;_tmp514.tag=Cyc_AggrFieldDeclListList_tok;_tmp514.f1=({struct Cyc_List_List*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp515->tl=0;_tmp515;});
_tmp514;});_tmp513;});break;case 108: _LL265: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp517;_tmp517.tag=Cyc_AggrFieldDeclListList_tok;_tmp517.f1=({struct Cyc_List_List*
_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp518->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp518;});_tmp517;});
_tmp516;});break;case 109: _LL266: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51A;_tmp51A.tag=Cyc_InitDeclList_tok;_tmp51A.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp51A;});_tmp519;});
break;case 110: _LL267: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp51B=
_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51C;_tmp51C.tag=Cyc_InitDeclList_tok;_tmp51C.f1=({struct Cyc_List_List*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51D->tl=0;_tmp51D;});
_tmp51C;});_tmp51B;});break;case 111: _LL268: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51F;_tmp51F.tag=Cyc_InitDeclList_tok;_tmp51F.f1=({struct Cyc_List_List*
_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp520->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp520;});_tmp51F;});
_tmp51E;});break;case 112: _LL269: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({struct Cyc_InitDecl_tok_struct
_tmp522;_tmp522.tag=Cyc_InitDecl_tok;_tmp522.f1=({struct _tuple12*_tmp523=
_cycalloc(sizeof(*_tmp523));_tmp523->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp523->f2=0;_tmp523;});
_tmp522;});_tmp521;});break;case 113: _LL26A: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524[0]=({struct Cyc_InitDecl_tok_struct
_tmp525;_tmp525.tag=Cyc_InitDecl_tok;_tmp525.f1=({struct _tuple12*_tmp526=
_cycalloc(sizeof(*_tmp526));_tmp526->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp526->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp526;});_tmp525;});_tmp524;});break;case 114: _LL26B: {struct _tuple15 _tmp528;
struct Cyc_Absyn_Tqual _tmp529;struct Cyc_List_List*_tmp52A;struct Cyc_List_List*
_tmp52B;struct _tuple15*_tmp527=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp528=*_tmp527;_tmp529=
_tmp528.f1;_tmp52A=_tmp528.f2;_tmp52B=_tmp528.f3;if(_tmp529.loc == 0)_tmp529.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp52D;struct Cyc_List_List*
_tmp52E;struct _tuple0 _tmp52C=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;{void*_tmp52F=Cyc_Parse_speclist2typ(
_tmp52A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp529,_tmp52F,_tmp52D,_tmp52B),_tmp52E);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp532;_tmp532.tag=Cyc_AggrFieldDeclList_tok;_tmp532.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp530);_tmp532;});_tmp531;});break;}}}case 115:
_LL26C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp533=_cycalloc(
sizeof(*_tmp533));_tmp533[0]=({struct Cyc_QualSpecList_tok_struct _tmp534;_tmp534.tag=
Cyc_QualSpecList_tok;_tmp534.f1=({struct _tuple15*_tmp535=_cycalloc(sizeof(*
_tmp535));_tmp535->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp535->f2=({
struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp536->tl=0;_tmp536;});
_tmp535->f3=0;_tmp535;});_tmp534;});_tmp533;});break;case 116: _LL26D: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({
struct Cyc_QualSpecList_tok_struct _tmp538;_tmp538.tag=Cyc_QualSpecList_tok;
_tmp538.f1=({struct _tuple15*_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp539->f2=({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53A->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53A;});_tmp539->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp539;});_tmp538;});_tmp537;});break;case 117: _LL26E: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));
_tmp53B[0]=({struct Cyc_QualSpecList_tok_struct _tmp53C;_tmp53C.tag=Cyc_QualSpecList_tok;
_tmp53C.f1=({struct _tuple15*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53D->f2=0;_tmp53D->f3=
0;_tmp53D;});_tmp53C;});_tmp53B;});break;case 118: _LL26F: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_QualSpecList_tok_struct
_tmp53F;_tmp53F.tag=Cyc_QualSpecList_tok;_tmp53F.f1=({struct _tuple15*_tmp540=
_cycalloc(sizeof(*_tmp540));_tmp540->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp540->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp540->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp540;});_tmp53F;});
_tmp53E;});break;case 119: _LL270: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541[0]=({struct Cyc_QualSpecList_tok_struct
_tmp542;_tmp542.tag=Cyc_QualSpecList_tok;_tmp542.f1=({struct _tuple15*_tmp543=
_cycalloc(sizeof(*_tmp543));_tmp543->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp543->f2=0;
_tmp543->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp543;});_tmp542;});_tmp541;});break;case 120: _LL271:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp544=_cycalloc(sizeof(*
_tmp544));_tmp544[0]=({struct Cyc_QualSpecList_tok_struct _tmp545;_tmp545.tag=Cyc_QualSpecList_tok;
_tmp545.f1=({struct _tuple15*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp546->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp546->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp546;});_tmp545;});
_tmp544;});break;case 121: _LL272: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_QualSpecList_tok_struct
_tmp548;_tmp548.tag=Cyc_QualSpecList_tok;_tmp548.f1=({struct _tuple15*_tmp549=
_cycalloc(sizeof(*_tmp549));_tmp549->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp549->f2=({
struct Cyc_List_List*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54A->tl=0;_tmp54A;});
_tmp549->f3=0;_tmp549;});_tmp548;});_tmp547;});break;case 122: _LL273: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({
struct Cyc_QualSpecList_tok_struct _tmp54C;_tmp54C.tag=Cyc_QualSpecList_tok;
_tmp54C.f1=({struct _tuple15*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp54D->f2=({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*
_tmp54E));_tmp54E->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54E->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp54E;});_tmp54D->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp54D;});_tmp54C;});_tmp54B;});break;case 123: _LL274: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));
_tmp54F[0]=({struct Cyc_QualSpecList_tok_struct _tmp550;_tmp550.tag=Cyc_QualSpecList_tok;
_tmp550.f1=({struct _tuple15*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp551->f2=0;_tmp551->f3=
0;_tmp551;});_tmp550;});_tmp54F;});break;case 124: _LL275: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_QualSpecList_tok_struct
_tmp553;_tmp553.tag=Cyc_QualSpecList_tok;_tmp553.f1=({struct _tuple15*_tmp554=
_cycalloc(sizeof(*_tmp554));_tmp554->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp554->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp554->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp554;});_tmp553;});
_tmp552;});break;case 125: _LL276: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_QualSpecList_tok_struct
_tmp556;_tmp556.tag=Cyc_QualSpecList_tok;_tmp556.f1=({struct _tuple15*_tmp557=
_cycalloc(sizeof(*_tmp557));_tmp557->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp557->f2=0;
_tmp557->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp557;});_tmp556;});_tmp555;});break;case 126: _LL277:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp558=_cycalloc(sizeof(*
_tmp558));_tmp558[0]=({struct Cyc_QualSpecList_tok_struct _tmp559;_tmp559.tag=Cyc_QualSpecList_tok;
_tmp559.f1=({struct _tuple15*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp55A->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp55A->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp55A;});_tmp559;});
_tmp558;});break;case 127: _LL278: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_InitDeclList_tok_struct
_tmp55C;_tmp55C.tag=Cyc_InitDeclList_tok;_tmp55C.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp55C;});_tmp55B;});
break;case 128: _LL279: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp55D=
_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_InitDeclList_tok_struct
_tmp55E;_tmp55E.tag=Cyc_InitDeclList_tok;_tmp55E.f1=({struct Cyc_List_List*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55F->tl=0;_tmp55F;});
_tmp55E;});_tmp55D;});break;case 129: _LL27A: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({struct Cyc_InitDeclList_tok_struct
_tmp561;_tmp561.tag=Cyc_InitDeclList_tok;_tmp561.f1=({struct Cyc_List_List*
_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp562->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp562;});_tmp561;});
_tmp560;});break;case 130: _LL27B: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_InitDecl_tok_struct
_tmp564;_tmp564.tag=Cyc_InitDecl_tok;_tmp564.f1=({struct _tuple12*_tmp565=
_cycalloc(sizeof(*_tmp565));_tmp565->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp565->f2=0;_tmp565;});
_tmp564;});_tmp563;});break;case 131: _LL27C: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566[0]=({struct Cyc_InitDecl_tok_struct
_tmp567;_tmp567.tag=Cyc_InitDecl_tok;_tmp567.f1=({struct _tuple12*_tmp568=
_cycalloc(sizeof(*_tmp568));_tmp568->f1=({struct Cyc_Parse_Declarator*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569->id=({struct _tuple1*_tmp56A=_cycalloc(
sizeof(*_tmp56A));_tmp56A->f1=Cyc_Absyn_rel_ns_null;_tmp56A->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp56A;});_tmp569->tms=
0;_tmp569;});_tmp568->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp568;});_tmp567;});
_tmp566;});break;case 132: _LL27D: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_InitDecl_tok_struct
_tmp56D;_tmp56D.tag=Cyc_InitDecl_tok;_tmp56D.f1=({struct _tuple12*_tmp56E=
_cycalloc(sizeof(*_tmp56E));_tmp56E->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56E->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp56E;});_tmp56D;});_tmp56C;});break;case 133: _LL27E: {struct Cyc_List_List*
_tmp56F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp571;_tmp571.tag=Cyc_TypeSpecifier_tok;
_tmp571.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp572=_cycalloc(
sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Parse_Decl_spec_struct _tmp573;_tmp573.tag=
5;_tmp573.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp56F,({struct Cyc_Core_Opt*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp574;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp573;});
_tmp572;}));_tmp571;});_tmp570;});break;}case 134: _LL27F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp576;_tmp576.tag=Cyc_TypeSpecifier_tok;_tmp576.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp577=_cycalloc(sizeof(*_tmp577));
_tmp577[0]=({struct Cyc_Absyn_TunionType_struct _tmp578;_tmp578.tag=2;_tmp578.f1=({
struct Cyc_Absyn_TunionInfo _tmp579;_tmp579.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp57B;_tmp57B.tag=0;_tmp57B.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp57C;
_tmp57C.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp57C.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57C;});_tmp57B;});
_tmp57A;}));_tmp579.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp579.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp579;});_tmp578;});
_tmp577;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp576;});_tmp575;});break;case 135: _LL280: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));
_tmp57D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp57E;_tmp57E.tag=Cyc_TypeSpecifier_tok;
_tmp57E.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp580;_tmp580.tag=3;_tmp580.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp581;
_tmp581.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp583;_tmp583.tag=0;_tmp583.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp584;
_tmp584.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp584.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp584.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp584;});_tmp583;});
_tmp582;}));_tmp581.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp581;});_tmp580;});
_tmp57F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp57E;});_tmp57D;});break;case 136: _LL281: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp585=_cycalloc_atomic(sizeof(*_tmp585));
_tmp585[0]=({struct Cyc_Bool_tok_struct _tmp586;_tmp586.tag=Cyc_Bool_tok;_tmp586.f1=
0;_tmp586;});_tmp585;});break;case 137: _LL282: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp587=_cycalloc_atomic(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Bool_tok_struct
_tmp588;_tmp588.tag=Cyc_Bool_tok;_tmp588.f1=1;_tmp588;});_tmp587;});break;case
138: _LL283: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp589=_cycalloc(
sizeof(*_tmp589));_tmp589[0]=({struct Cyc_TunionFieldList_tok_struct _tmp58A;
_tmp58A.tag=Cyc_TunionFieldList_tok;_tmp58A.f1=({struct Cyc_List_List*_tmp58B=
_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58B->tl=0;_tmp58B;});
_tmp58A;});_tmp589;});break;case 139: _LL284: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp58D;_tmp58D.tag=Cyc_TunionFieldList_tok;_tmp58D.f1=({struct Cyc_List_List*
_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58E->tl=0;_tmp58E;});
_tmp58D;});_tmp58C;});break;case 140: _LL285: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp590;_tmp590.tag=Cyc_TunionFieldList_tok;_tmp590.f1=({struct Cyc_List_List*
_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp591->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp591;});_tmp590;});
_tmp58F;});break;case 141: _LL286: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp593;_tmp593.tag=Cyc_TunionFieldList_tok;_tmp593.f1=({struct Cyc_List_List*
_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp594->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp594;});_tmp593;});
_tmp592;});break;case 142: _LL287: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_Scope_tok_struct
_tmp596;_tmp596.tag=Cyc_Scope_tok;_tmp596.f1=(void*)((void*)2);_tmp596;});
_tmp595;});break;case 143: _LL288: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597[0]=({struct Cyc_Scope_tok_struct
_tmp598;_tmp598.tag=Cyc_Scope_tok;_tmp598.f1=(void*)((void*)3);_tmp598;});
_tmp597;});break;case 144: _LL289: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({struct Cyc_Scope_tok_struct
_tmp59A;_tmp59A.tag=Cyc_Scope_tok;_tmp59A.f1=(void*)((void*)0);_tmp59A;});
_tmp599;});break;case 145: _LL28A: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B[0]=({struct Cyc_TunionField_tok_struct
_tmp59C;_tmp59C.tag=Cyc_TunionField_tok;_tmp59C.f1=({struct Cyc_Absyn_Tunionfield*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59D->typs=0;_tmp59D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp59D->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59D;});_tmp59C;});
_tmp59B;});break;case 146: _LL28B: {struct Cyc_List_List*_tmp59E=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({
struct Cyc_TunionField_tok_struct _tmp5A0;_tmp5A0.tag=Cyc_TunionField_tok;_tmp5A0.f1=({
struct Cyc_Absyn_Tunionfield*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5A1->typs=_tmp59E;_tmp5A1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A1->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp5A1;});_tmp5A0;});_tmp59F;});break;}case 147: _LL28C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 148: _LL28D: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2[0]=({struct Cyc_Declarator_tok_struct _tmp5A3;_tmp5A3.tag=Cyc_Declarator_tok;
_tmp5A3.f1=({struct Cyc_Parse_Declarator*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));
_tmp5A4->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5A4->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5A4;});
_tmp5A3;});_tmp5A2;});break;case 149: _LL28E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 150: _LL28F: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5[0]=({struct Cyc_Declarator_tok_struct _tmp5A6;_tmp5A6.tag=Cyc_Declarator_tok;
_tmp5A6.f1=({struct Cyc_Parse_Declarator*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));
_tmp5A7->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5A7->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5A7;});
_tmp5A6;});_tmp5A5;});break;case 151: _LL290: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8[0]=({struct Cyc_Declarator_tok_struct
_tmp5A9;_tmp5A9.tag=Cyc_Declarator_tok;_tmp5A9.f1=({struct Cyc_Parse_Declarator*
_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AA->tms=0;_tmp5AA;});
_tmp5A9;});_tmp5A8;});break;case 152: _LL291: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 153: _LL292: {
struct Cyc_Parse_Declarator*_tmp5AB=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AB->tms=({struct Cyc_List_List*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5AE;_tmp5AE.tag=5;_tmp5AE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5AE.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5AE;});_tmp5AD;}));_tmp5AC->tl=_tmp5AB->tms;_tmp5AC;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 154:
_LL293: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5AF=_cycalloc(sizeof(*
_tmp5AF));_tmp5AF[0]=({struct Cyc_Declarator_tok_struct _tmp5B0;_tmp5B0.tag=Cyc_Declarator_tok;
_tmp5B0.f1=({struct Cyc_Parse_Declarator*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));
_tmp5B1->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B1->tms=({struct Cyc_List_List*_tmp5B2=_cycalloc(
sizeof(*_tmp5B2));_tmp5B2->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5B4;_tmp5B4.tag=0;_tmp5B4.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B4;});
_tmp5B3;}));_tmp5B2->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B2;});_tmp5B1;});
_tmp5B0;});_tmp5AF;});break;case 155: _LL294: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_Declarator_tok_struct
_tmp5B6;_tmp5B6.tag=Cyc_Declarator_tok;_tmp5B6.f1=({struct Cyc_Parse_Declarator*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5B7->tms=({
struct Cyc_List_List*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));
_tmp5B9[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5BA;_tmp5BA.tag=1;_tmp5BA.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5BA.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5BA.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5BA;});_tmp5B9;}));_tmp5B8->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5B8;});
_tmp5B7;});_tmp5B6;});_tmp5B5;});break;case 156: _LL295: {struct _tuple14 _tmp5BC;
struct Cyc_List_List*_tmp5BD;int _tmp5BE;struct Cyc_Absyn_VarargInfo*_tmp5BF;struct
Cyc_Core_Opt*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct _tuple14*_tmp5BB=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BC=*_tmp5BB;
_tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;_tmp5BF=_tmp5BC.f3;_tmp5C0=_tmp5BC.f4;
_tmp5C1=_tmp5BC.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5C2=
_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_Declarator_tok_struct _tmp5C3;
_tmp5C3.tag=Cyc_Declarator_tok;_tmp5C3.f1=({struct Cyc_Parse_Declarator*_tmp5C4=
_cycalloc(sizeof(*_tmp5C4));_tmp5C4->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5C4->tms=({
struct Cyc_List_List*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));
_tmp5C6[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5C7;_tmp5C7.tag=3;_tmp5C7.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5C8=_cycalloc(sizeof(*
_tmp5C8));_tmp5C8[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5C9;_tmp5C9.tag=1;
_tmp5C9.f1=_tmp5BD;_tmp5C9.f2=_tmp5BE;_tmp5C9.f3=_tmp5BF;_tmp5C9.f4=_tmp5C0;
_tmp5C9.f5=_tmp5C1;_tmp5C9;});_tmp5C8;}));_tmp5C7;});_tmp5C6;}));_tmp5C5->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp5C5;});_tmp5C4;});_tmp5C3;});_tmp5C2;});break;}case 157: _LL296:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));
_tmp5CA[0]=({struct Cyc_Declarator_tok_struct _tmp5CB;_tmp5CB.tag=Cyc_Declarator_tok;
_tmp5CB.f1=({struct Cyc_Parse_Declarator*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));
_tmp5CC->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5CC->tms=({struct Cyc_List_List*_tmp5CD=_cycalloc(
sizeof(*_tmp5CD));_tmp5CD->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5CF;_tmp5CF.tag=3;_tmp5CF.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5D1;_tmp5D1.tag=1;_tmp5D1.f1=0;_tmp5D1.f2=0;_tmp5D1.f3=0;_tmp5D1.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D1.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D1;});_tmp5D0;}));
_tmp5CF;});_tmp5CE;}));_tmp5CD->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5CD;});_tmp5CC;});
_tmp5CB;});_tmp5CA;});break;case 158: _LL297: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Declarator_tok_struct
_tmp5D3;_tmp5D3.tag=Cyc_Declarator_tok;_tmp5D3.f1=({struct Cyc_Parse_Declarator*
_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5D4->tms=({
struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));
_tmp5D6[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D7;_tmp5D7.tag=3;_tmp5D7.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));
_tmp5D8[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5D9;_tmp5D9.tag=0;_tmp5D9.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D9.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5D9;});
_tmp5D8;}));_tmp5D7;});_tmp5D6;}));_tmp5D5->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5D5;});_tmp5D4;});
_tmp5D3;});_tmp5D2;});break;case 159: _LL298: {struct Cyc_List_List*_tmp5DA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({
struct Cyc_Declarator_tok_struct _tmp5DC;_tmp5DC.tag=Cyc_Declarator_tok;_tmp5DC.f1=({
struct Cyc_Parse_Declarator*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5DD->tms=({
struct Cyc_List_List*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));
_tmp5DF[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp5E0;_tmp5E0.tag=4;_tmp5E0.f1=
_tmp5DA;_tmp5E0.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E0.f3=0;
_tmp5E0;});_tmp5DF;}));_tmp5DE->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5DE;});_tmp5DD;});
_tmp5DC;});_tmp5DB;});break;}case 160: _LL299: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_Declarator_tok_struct
_tmp5E2;_tmp5E2.tag=Cyc_Declarator_tok;_tmp5E2.f1=({struct Cyc_Parse_Declarator*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5E3->tms=({
struct Cyc_List_List*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));
_tmp5E5[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5E6;_tmp5E6.tag=5;_tmp5E6.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5E6.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E6;});_tmp5E5;}));
_tmp5E4->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5E4;});_tmp5E3;});_tmp5E2;});_tmp5E1;});break;case
161: _LL29A: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5E7=_cycalloc(
sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Declarator_tok_struct _tmp5E8;_tmp5E8.tag=
Cyc_Declarator_tok;_tmp5E8.f1=({struct Cyc_Parse_Declarator*_tmp5E9=_cycalloc(
sizeof(*_tmp5E9));_tmp5E9->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E9->tms=0;_tmp5E9;});
_tmp5E8;});_tmp5E7;});break;case 162: _LL29B: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_Declarator_tok_struct
_tmp5EB;_tmp5EB.tag=Cyc_Declarator_tok;_tmp5EB.f1=({struct Cyc_Parse_Declarator*
_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EC->tms=0;_tmp5EC;});
_tmp5EB;});_tmp5EA;});break;case 163: _LL29C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 164: _LL29D: {
struct Cyc_Parse_Declarator*_tmp5ED=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5ED->tms=({struct Cyc_List_List*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5F0;_tmp5F0.tag=5;_tmp5F0.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5F0.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5F0;});_tmp5EF;}));_tmp5EE->tl=_tmp5ED->tms;_tmp5EE;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165:
_LL29E: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5F1=_cycalloc(sizeof(*
_tmp5F1));_tmp5F1[0]=({struct Cyc_Declarator_tok_struct _tmp5F2;_tmp5F2.tag=Cyc_Declarator_tok;
_tmp5F2.f1=({struct Cyc_Parse_Declarator*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));
_tmp5F3->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5F3->tms=({struct Cyc_List_List*_tmp5F4=_cycalloc(
sizeof(*_tmp5F4));_tmp5F4->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5F6;_tmp5F6.tag=0;_tmp5F6.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5F6;});
_tmp5F5;}));_tmp5F4->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5F4;});_tmp5F3;});
_tmp5F2;});_tmp5F1;});break;case 166: _LL29F: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_Declarator_tok_struct
_tmp5F8;_tmp5F8.tag=Cyc_Declarator_tok;_tmp5F8.f1=({struct Cyc_Parse_Declarator*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5F9->tms=({
struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));
_tmp5FB[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5FC;_tmp5FC.tag=1;_tmp5FC.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5FC.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5FC.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5FC;});_tmp5FB;}));_tmp5FA->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5FA;});
_tmp5F9;});_tmp5F8;});_tmp5F7;});break;case 167: _LL2A0: {struct _tuple14 _tmp5FE;
struct Cyc_List_List*_tmp5FF;int _tmp600;struct Cyc_Absyn_VarargInfo*_tmp601;struct
Cyc_Core_Opt*_tmp602;struct Cyc_List_List*_tmp603;struct _tuple14*_tmp5FD=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FE=*_tmp5FD;
_tmp5FF=_tmp5FE.f1;_tmp600=_tmp5FE.f2;_tmp601=_tmp5FE.f3;_tmp602=_tmp5FE.f4;
_tmp603=_tmp5FE.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp604=
_cycalloc(sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Declarator_tok_struct _tmp605;
_tmp605.tag=Cyc_Declarator_tok;_tmp605.f1=({struct Cyc_Parse_Declarator*_tmp606=
_cycalloc(sizeof(*_tmp606));_tmp606->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp606->tms=({
struct Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp608=_cycalloc(sizeof(*_tmp608));
_tmp608[0]=({struct Cyc_Absyn_Function_mod_struct _tmp609;_tmp609.tag=3;_tmp609.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp60A=_cycalloc(sizeof(*
_tmp60A));_tmp60A[0]=({struct Cyc_Absyn_WithTypes_struct _tmp60B;_tmp60B.tag=1;
_tmp60B.f1=_tmp5FF;_tmp60B.f2=_tmp600;_tmp60B.f3=_tmp601;_tmp60B.f4=_tmp602;
_tmp60B.f5=_tmp603;_tmp60B;});_tmp60A;}));_tmp609;});_tmp608;}));_tmp607->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp607;});_tmp606;});_tmp605;});_tmp604;});break;}case 168: _LL2A1:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));
_tmp60C[0]=({struct Cyc_Declarator_tok_struct _tmp60D;_tmp60D.tag=Cyc_Declarator_tok;
_tmp60D.f1=({struct Cyc_Parse_Declarator*_tmp60E=_cycalloc(sizeof(*_tmp60E));
_tmp60E->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp60E->tms=({struct Cyc_List_List*_tmp60F=_cycalloc(
sizeof(*_tmp60F));_tmp60F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp611;_tmp611.tag=3;_tmp611.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp613;_tmp613.tag=1;_tmp613.f1=0;_tmp613.f2=0;_tmp613.f3=0;_tmp613.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp613.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp613;});_tmp612;}));
_tmp611;});_tmp610;}));_tmp60F->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp60F;});_tmp60E;});
_tmp60D;});_tmp60C;});break;case 169: _LL2A2: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Declarator_tok_struct
_tmp615;_tmp615.tag=Cyc_Declarator_tok;_tmp615.f1=({struct Cyc_Parse_Declarator*
_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp616->tms=({
struct Cyc_List_List*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp618=_cycalloc(sizeof(*_tmp618));
_tmp618[0]=({struct Cyc_Absyn_Function_mod_struct _tmp619;_tmp619.tag=3;_tmp619.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));
_tmp61A[0]=({struct Cyc_Absyn_NoTypes_struct _tmp61B;_tmp61B.tag=0;_tmp61B.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp61B;});
_tmp61A;}));_tmp619;});_tmp618;}));_tmp617->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp617;});_tmp616;});
_tmp615;});_tmp614;});break;case 170: _LL2A3: {struct Cyc_List_List*_tmp61C=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D[0]=({
struct Cyc_Declarator_tok_struct _tmp61E;_tmp61E.tag=Cyc_Declarator_tok;_tmp61E.f1=({
struct Cyc_Parse_Declarator*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp61F->tms=({
struct Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp621=_cycalloc(sizeof(*_tmp621));
_tmp621[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp622;_tmp622.tag=4;_tmp622.f1=
_tmp61C;_tmp622.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp622.f3=0;
_tmp622;});_tmp621;}));_tmp620->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp620;});_tmp61F;});
_tmp61E;});_tmp61D;});break;}case 171: _LL2A4: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_Declarator_tok_struct
_tmp624;_tmp624.tag=Cyc_Declarator_tok;_tmp624.f1=({struct Cyc_Parse_Declarator*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp625->tms=({
struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp627=_cycalloc(sizeof(*_tmp627));
_tmp627[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp628;_tmp628.tag=5;_tmp628.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp628.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp628;});_tmp627;}));
_tmp626->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp626;});_tmp625;});_tmp624;});_tmp623;});break;case
172: _LL2A5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 173: _LL2A6: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp629=
_cycalloc(sizeof(*_tmp629));_tmp629[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp62A;_tmp62A.tag=Cyc_TypeModifierList_tok;_tmp62A.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp62A;});_tmp629;});
break;case 174: _LL2A7: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp62D;_tmp62D.tag=5;_tmp62D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp62D.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp62D;});_tmp62C;}));_tmp62B->tl=ans;_tmp62B;});{struct Cyc_Absyn_PtrLoc*
_tmp62E=({struct Cyc_Absyn_PtrLoc*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->ptr_loc=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;
_tmp635->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp635->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp635;});ans=({struct Cyc_List_List*_tmp62F=
_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp631;_tmp631.tag=2;_tmp631.f1=({struct Cyc_Absyn_PtrAtts _tmp632;_tmp632.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp632.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp632.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp632.zero_term=
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp632.ptrloc=_tmp62E;_tmp632;});_tmp631.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp631;});_tmp630;}));
_tmp62F->tl=ans;_tmp62F;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp634;_tmp634.tag=Cyc_TypeModifierList_tok;_tmp634.f1=ans;_tmp634;});_tmp633;});
break;}}case 175: _LL2A8: yyval=(void*)({struct Cyc_YY1_struct*_tmp636=_cycalloc(
sizeof(*_tmp636));_tmp636[0]=({struct Cyc_YY1_struct _tmp637;_tmp637.tag=Cyc_YY1;
_tmp637.f1=({struct _tuple16*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp638->f2=
Cyc_Absyn_true_conref;_tmp638->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp638;});_tmp637;});
_tmp636;});break;case 176: _LL2A9: yyval=(void*)({struct Cyc_YY1_struct*_tmp639=
_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_YY1_struct _tmp63A;_tmp63A.tag=
Cyc_YY1;_tmp63A.f1=({struct _tuple16*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp63B->f2=Cyc_Absyn_false_conref;_tmp63B->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63B;});_tmp63A;});
_tmp639;});break;case 177: _LL2AA: yyval=(void*)({struct Cyc_YY1_struct*_tmp63C=
_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({struct Cyc_YY1_struct _tmp63D;_tmp63D.tag=
Cyc_YY1;_tmp63D.f1=({struct _tuple16*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp63E->f2=Cyc_Absyn_true_conref;_tmp63E->f3=Cyc_Absyn_bounds_unknown_conref;
_tmp63E;});_tmp63D;});_tmp63C;});break;case 178: _LL2AB: yyval=(void*)({struct Cyc_YY2_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_YY2_struct _tmp640;
_tmp640.tag=Cyc_YY2;_tmp640.f1=Cyc_Absyn_bounds_one_conref;_tmp640;});_tmp63F;});
break;case 179: _LL2AC: yyval=(void*)({struct Cyc_YY2_struct*_tmp641=_cycalloc(
sizeof(*_tmp641));_tmp641[0]=({struct Cyc_YY2_struct _tmp642;_tmp642.tag=Cyc_YY2;
_tmp642.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp643=
_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Absyn_Upper_b_struct _tmp644;
_tmp644.tag=0;_tmp644.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp644;});_tmp643;}));_tmp642;});_tmp641;});break;case
180: _LL2AD: yyval=(void*)({struct Cyc_YY2_struct*_tmp645=_cycalloc(sizeof(*_tmp645));
_tmp645[0]=({struct Cyc_YY2_struct _tmp646;_tmp646.tag=Cyc_YY2;_tmp646.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp647=_cycalloc(sizeof(*_tmp647));
_tmp647[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp648;_tmp648.tag=1;_tmp648.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp648;});_tmp647;}));_tmp646;});_tmp645;});break;case 181: _LL2AE: yyval=(
void*)({struct Cyc_YY6_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649[0]=({
struct Cyc_YY6_struct _tmp64A;_tmp64A.tag=Cyc_YY6;_tmp64A.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp64A;});_tmp649;});break;case 182: _LL2AF: yyval=(void*)({
struct Cyc_YY6_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_YY6_struct
_tmp64C;_tmp64C.tag=Cyc_YY6;_tmp64C.f1=Cyc_Absyn_true_conref;_tmp64C;});_tmp64B;});
break;case 183: _LL2B0: yyval=(void*)({struct Cyc_YY6_struct*_tmp64D=_cycalloc(
sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_YY6_struct _tmp64E;_tmp64E.tag=Cyc_YY6;
_tmp64E.f1=Cyc_Absyn_false_conref;_tmp64E;});_tmp64D;});break;case 184: _LL2B1:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp64F=_cycalloc(sizeof(*_tmp64F));
_tmp64F[0]=({struct Cyc_Type_tok_struct _tmp650;_tmp650.tag=Cyc_Type_tok;_tmp650.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp651=_cycalloc(sizeof(*_tmp651));
_tmp651->v=(void*)((void*)3);_tmp651;}),0);_tmp650;});_tmp64F;});break;case 185:
_LL2B2: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 186: _LL2B3: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({
struct Cyc_Type_tok_struct _tmp653;_tmp653.tag=Cyc_Type_tok;_tmp653.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)((void*)
3);_tmp654;}),0);_tmp653;});_tmp652;});break;case 187: _LL2B4: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({
struct Cyc_TypeQual_tok_struct _tmp656;_tmp656.tag=Cyc_TypeQual_tok;_tmp656.f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp656;});_tmp655;});break;case 188: _LL2B5: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp657=_cycalloc(sizeof(*_tmp657));
_tmp657[0]=({struct Cyc_TypeQual_tok_struct _tmp658;_tmp658.tag=Cyc_TypeQual_tok;
_tmp658.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp658;});_tmp657;});
break;case 189: _LL2B6: yyval=(void*)({struct Cyc_YY5_struct*_tmp659=_cycalloc(
sizeof(*_tmp659));_tmp659[0]=({struct Cyc_YY5_struct _tmp65A;_tmp65A.tag=Cyc_YY5;
_tmp65A.f1=({struct _tuple14*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp65B->f2=0;
_tmp65B->f3=0;_tmp65B->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65B->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65B;});_tmp65A;});
_tmp659;});break;case 190: _LL2B7: yyval=(void*)({struct Cyc_YY5_struct*_tmp65C=
_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_YY5_struct _tmp65D;_tmp65D.tag=
Cyc_YY5;_tmp65D.f1=({struct _tuple14*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp65E->f2=1;
_tmp65E->f3=0;_tmp65E->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65E->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65E;});_tmp65D;});
_tmp65C;});break;case 191: _LL2B8: {struct _tuple2 _tmp660;struct Cyc_Core_Opt*
_tmp661;struct Cyc_Absyn_Tqual _tmp662;void*_tmp663;struct _tuple2*_tmp65F=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp660=*_tmp65F;
_tmp661=_tmp660.f1;_tmp662=_tmp660.f2;_tmp663=_tmp660.f3;{struct Cyc_Absyn_VarargInfo*
_tmp664=({struct Cyc_Absyn_VarargInfo*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->name=
_tmp661;_tmp668->tq=_tmp662;_tmp668->type=(void*)_tmp663;_tmp668->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp668;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({
struct Cyc_YY5_struct _tmp666;_tmp666.tag=Cyc_YY5;_tmp666.f1=({struct _tuple14*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->f1=0;_tmp667->f2=0;_tmp667->f3=
_tmp664;_tmp667->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp667->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp667;});_tmp666;});
_tmp665;});break;}}case 192: _LL2B9: {struct _tuple2 _tmp66A;struct Cyc_Core_Opt*
_tmp66B;struct Cyc_Absyn_Tqual _tmp66C;void*_tmp66D;struct _tuple2*_tmp669=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66A=*_tmp669;
_tmp66B=_tmp66A.f1;_tmp66C=_tmp66A.f2;_tmp66D=_tmp66A.f3;{struct Cyc_Absyn_VarargInfo*
_tmp66E=({struct Cyc_Absyn_VarargInfo*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->name=
_tmp66B;_tmp672->tq=_tmp66C;_tmp672->type=(void*)_tmp66D;_tmp672->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp672;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({
struct Cyc_YY5_struct _tmp670;_tmp670.tag=Cyc_YY5;_tmp670.f1=({struct _tuple14*
_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp671->f2=0;_tmp671->f3=
_tmp66E;_tmp671->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp671->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp671;});_tmp670;});
_tmp66F;});break;}}case 193: _LL2BA: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Type_tok_struct
_tmp674;_tmp674.tag=Cyc_Type_tok;_tmp674.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp676;_tmp676.tag=1;_tmp676.f1=0;_tmp676;});_tmp675;}));_tmp674;});_tmp673;});
break;case 194: _LL2BB: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp677=_cycalloc(
sizeof(*_tmp677));_tmp677[0]=({struct Cyc_Type_tok_struct _tmp678;_tmp678.tag=Cyc_Type_tok;
_tmp678.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp67A;_tmp67A.tag=0;_tmp67A.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67A;});_tmp679;}));
_tmp678;});_tmp677;});break;case 195: _LL2BC: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B[0]=({struct Cyc_TypeOpt_tok_struct
_tmp67C;_tmp67C.tag=Cyc_TypeOpt_tok;_tmp67C.f1=0;_tmp67C;});_tmp67B;});break;
case 196: _LL2BD: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp67D=_cycalloc(
sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_TypeOpt_tok_struct _tmp67E;_tmp67E.tag=
Cyc_TypeOpt_tok;_tmp67E.f1=({struct Cyc_Core_Opt*_tmp67F=_cycalloc(sizeof(*
_tmp67F));_tmp67F->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp680=
_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_JoinEff_struct _tmp681;
_tmp681.tag=20;_tmp681.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp681;});_tmp680;}));
_tmp67F;});_tmp67E;});_tmp67D;});break;case 197: _LL2BE: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682[0]=({struct Cyc_Rgnorder_tok_struct
_tmp683;_tmp683.tag=Cyc_Rgnorder_tok;_tmp683.f1=0;_tmp683;});_tmp682;});break;
case 198: _LL2BF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 199: _LL2C0: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Rgnorder_tok_struct _tmp685;
_tmp685.tag=Cyc_Rgnorder_tok;_tmp685.f1=({struct Cyc_List_List*_tmp686=_cycalloc(
sizeof(*_tmp686));_tmp686->hd=({struct _tuple4*_tmp687=_cycalloc(sizeof(*_tmp687));
_tmp687->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68A=_cycalloc(sizeof(*
_tmp68A));_tmp68A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68B;_tmp68B.tag=20;
_tmp68B.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp68B;});_tmp68A;});_tmp687->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp689;_tmp689.tag=0;_tmp689.f1=(void*)((void*)3);_tmp689;});_tmp688;}));
_tmp687;});_tmp686->tl=0;_tmp686;});_tmp685;});_tmp684;});break;case 200: _LL2C1:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));
_tmp68C[0]=({struct Cyc_Rgnorder_tok_struct _tmp68D;_tmp68D.tag=Cyc_Rgnorder_tok;
_tmp68D.f1=({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->hd=({
struct _tuple4*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp693;_tmp693.tag=20;_tmp693.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp693;});_tmp692;});
_tmp68F->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp691;_tmp691.tag=0;_tmp691.f1=(void*)((void*)3);_tmp691;});_tmp690;}));
_tmp68F;});_tmp68E->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68E;});_tmp68D;});
_tmp68C;});break;case 201: _LL2C2: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp694=
_cycalloc_atomic(sizeof(*_tmp694));_tmp694[0]=({struct Cyc_Bool_tok_struct _tmp695;
_tmp695.tag=Cyc_Bool_tok;_tmp695.f1=0;_tmp695;});_tmp694;});break;case 202: _LL2C3:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp696="inject";
_tag_arr(_tmp696,sizeof(char),_get_zero_arr_size(_tmp696,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp697="missing type in function declaration";_tag_arr(_tmp697,
sizeof(char),_get_zero_arr_size(_tmp697,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp698=_cycalloc_atomic(sizeof(*_tmp698));_tmp698[0]=({
struct Cyc_Bool_tok_struct _tmp699;_tmp699.tag=Cyc_Bool_tok;_tmp699.f1=1;_tmp699;});
_tmp698;});break;case 203: _LL2C4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 204: _LL2C5: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_TypeList_tok_struct
_tmp69B;_tmp69B.tag=Cyc_TypeList_tok;_tmp69B.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp69B;});_tmp69A;});
break;case 205: _LL2C6: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69C=
_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_TypeList_tok_struct _tmp69D;
_tmp69D.tag=Cyc_TypeList_tok;_tmp69D.f1=0;_tmp69D;});_tmp69C;});break;case 206:
_LL2C7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 207: _LL2C8: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69E=_cycalloc(
sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_TypeList_tok_struct _tmp69F;_tmp69F.tag=
Cyc_TypeList_tok;_tmp69F.f1=({struct Cyc_List_List*_tmp6A0=_cycalloc(sizeof(*
_tmp6A0));_tmp6A0->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A1=
_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A2;
_tmp6A2.tag=21;_tmp6A2.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A2;});_tmp6A1;}));
_tmp6A0->tl=0;_tmp6A0;});_tmp69F;});_tmp69E;});break;case 208: _LL2C9: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A3=_cycalloc(sizeof(*
_tmp6A3));_tmp6A3[0]=({struct Cyc_TypeList_tok_struct _tmp6A4;_tmp6A4.tag=Cyc_TypeList_tok;
_tmp6A4.f1=({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6A5->tl=0;_tmp6A5;});_tmp6A4;});_tmp6A3;});break;case 209: _LL2CA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A6=_cycalloc(sizeof(*
_tmp6A6));_tmp6A6[0]=({struct Cyc_TypeList_tok_struct _tmp6A7;_tmp6A7.tag=Cyc_TypeList_tok;
_tmp6A7.f1=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6A9=_cycalloc(sizeof(*
_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AA;_tmp6AA.tag=19;
_tmp6AA.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6AA;});_tmp6A9;}));_tmp6A8->tl=0;_tmp6A8;});_tmp6A7;});
_tmp6A6;});break;case 210: _LL2CB: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));
_tmp6AB[0]=({struct Cyc_TypeList_tok_struct _tmp6AC;_tmp6AC.tag=Cyc_TypeList_tok;
_tmp6AC.f1=({struct Cyc_List_List*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AE=_cycalloc(sizeof(*
_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AF;_tmp6AF.tag=19;
_tmp6AF.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp6AF;});_tmp6AE;}));_tmp6AD->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AD;});_tmp6AC;});
_tmp6AB;});break;case 211: _LL2CC: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6B1;_tmp6B1.tag=Cyc_ParamDeclList_tok;_tmp6B1.f1=({struct Cyc_List_List*
_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B2->tl=0;_tmp6B2;});
_tmp6B1;});_tmp6B0;});break;case 212: _LL2CD: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6B4;_tmp6B4.tag=Cyc_ParamDeclList_tok;_tmp6B4.f1=({struct Cyc_List_List*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B5->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B5;});_tmp6B4;});
_tmp6B3;});break;case 213: _LL2CE: {struct _tuple15 _tmp6B7;struct Cyc_Absyn_Tqual
_tmp6B8;struct Cyc_List_List*_tmp6B9;struct Cyc_List_List*_tmp6BA;struct _tuple15*
_tmp6B6=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6B7=*_tmp6B6;_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;
_tmp6BA=_tmp6B7.f3;if(_tmp6B8.loc == 0)_tmp6B8.loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp6BC;struct _tuple1*_tmp6BD;struct Cyc_List_List*_tmp6BE;struct Cyc_Parse_Declarator*
_tmp6BB=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6BC=*_tmp6BB;_tmp6BD=_tmp6BC.id;_tmp6BE=_tmp6BC.tms;{void*
_tmp6BF=Cyc_Parse_speclist2typ(_tmp6B9,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp6C1;void*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_List_List*_tmp6C4;
struct _tuple6 _tmp6C0=Cyc_Parse_apply_tms(_tmp6B8,_tmp6BF,_tmp6BA,_tmp6BE);
_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;_tmp6C4=_tmp6C0.f4;if(
_tmp6C3 != 0)Cyc_Parse_err(({const char*_tmp6C5="parameter with bad type params";
_tag_arr(_tmp6C5,sizeof(char),_get_zero_arr_size(_tmp6C5,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6BD))Cyc_Parse_err(({const char*_tmp6C6="parameter cannot be qualified with a namespace";
_tag_arr(_tmp6C6,sizeof(char),_get_zero_arr_size(_tmp6C6,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6C7=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6CD=_cycalloc(sizeof(*
_tmp6CD));_tmp6CD->v=(*_tmp6BD).f2;_tmp6CD;});if(_tmp6C4 != 0)({void*_tmp6C8[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6C9="extra attributes on parameter, ignoring";
_tag_arr(_tmp6C9,sizeof(char),_get_zero_arr_size(_tmp6C9,40));}),_tag_arr(
_tmp6C8,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6CB;_tmp6CB.tag=Cyc_ParamDecl_tok;_tmp6CB.f1=({struct _tuple2*_tmp6CC=
_cycalloc(sizeof(*_tmp6CC));_tmp6CC->f1=_tmp6C7;_tmp6CC->f2=_tmp6C1;_tmp6CC->f3=
_tmp6C2;_tmp6CC;});_tmp6CB;});_tmp6CA;});break;}}}}case 214: _LL2CF: {struct
_tuple15 _tmp6CF;struct Cyc_Absyn_Tqual _tmp6D0;struct Cyc_List_List*_tmp6D1;struct
Cyc_List_List*_tmp6D2;struct _tuple15*_tmp6CE=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CF=*_tmp6CE;_tmp6D0=
_tmp6CF.f1;_tmp6D1=_tmp6CF.f2;_tmp6D2=_tmp6CF.f3;if(_tmp6D0.loc == 0)_tmp6D0.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6D3=Cyc_Parse_speclist2typ(_tmp6D1,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6D2 != 0)({
void*_tmp6D4[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6D5="bad attributes on parameter, ignoring";_tag_arr(_tmp6D5,sizeof(char),
_get_zero_arr_size(_tmp6D5,38));}),_tag_arr(_tmp6D4,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));
_tmp6D6[0]=({struct Cyc_ParamDecl_tok_struct _tmp6D7;_tmp6D7.tag=Cyc_ParamDecl_tok;
_tmp6D7.f1=({struct _tuple2*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->f1=0;
_tmp6D8->f2=_tmp6D0;_tmp6D8->f3=_tmp6D3;_tmp6D8;});_tmp6D7;});_tmp6D6;});break;}}
case 215: _LL2D0: {struct _tuple15 _tmp6DA;struct Cyc_Absyn_Tqual _tmp6DB;struct Cyc_List_List*
_tmp6DC;struct Cyc_List_List*_tmp6DD;struct _tuple15*_tmp6D9=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA=*_tmp6D9;
_tmp6DB=_tmp6DA.f1;_tmp6DC=_tmp6DA.f2;_tmp6DD=_tmp6DA.f3;if(_tmp6DB.loc == 0)
_tmp6DB.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6DE=Cyc_Parse_speclist2typ(_tmp6DC,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6DF=(Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual _tmp6E1;void*_tmp6E2;struct Cyc_List_List*
_tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple6 _tmp6E0=Cyc_Parse_apply_tms(
_tmp6DB,_tmp6DE,_tmp6DD,_tmp6DF);_tmp6E1=_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;_tmp6E3=
_tmp6E0.f3;_tmp6E4=_tmp6E0.f4;if(_tmp6E3 != 0)({void*_tmp6E5[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6E6="bad type parameters on formal argument, ignoring";
_tag_arr(_tmp6E6,sizeof(char),_get_zero_arr_size(_tmp6E6,49));}),_tag_arr(
_tmp6E5,sizeof(void*),0));});if(_tmp6E4 != 0)({void*_tmp6E7[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6E8="bad attributes on parameter, ignoring";
_tag_arr(_tmp6E8,sizeof(char),_get_zero_arr_size(_tmp6E8,38));}),_tag_arr(
_tmp6E7,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6EA;_tmp6EA.tag=Cyc_ParamDecl_tok;_tmp6EA.f1=({struct _tuple2*_tmp6EB=
_cycalloc(sizeof(*_tmp6EB));_tmp6EB->f1=0;_tmp6EB->f2=_tmp6E1;_tmp6EB->f3=
_tmp6E2;_tmp6EB;});_tmp6EA;});_tmp6E9;});break;}}case 216: _LL2D1: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC[0]=({
struct Cyc_IdList_tok_struct _tmp6ED;_tmp6ED.tag=Cyc_IdList_tok;_tmp6ED.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp6ED;});_tmp6EC;});
break;case 217: _LL2D2: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6EE=
_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=({struct Cyc_IdList_tok_struct _tmp6EF;
_tmp6EF.tag=Cyc_IdList_tok;_tmp6EF.f1=({struct Cyc_List_List*_tmp6F0=_cycalloc(
sizeof(*_tmp6F0));_tmp6F0->hd=({struct _tagged_arr*_tmp6F1=_cycalloc(sizeof(*
_tmp6F1));_tmp6F1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6F1;});_tmp6F0->tl=0;_tmp6F0;});_tmp6EF;});_tmp6EE;});
break;case 218: _LL2D3: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6F2=
_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_IdList_tok_struct _tmp6F3;
_tmp6F3.tag=Cyc_IdList_tok;_tmp6F3.f1=({struct Cyc_List_List*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4->hd=({struct _tagged_arr*_tmp6F5=_cycalloc(sizeof(*
_tmp6F5));_tmp6F5[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6F5;});_tmp6F4->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F4;});_tmp6F3;});
_tmp6F2;});break;case 219: _LL2D4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 220: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 221: _LL2D6: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Exp_tok_struct _tmp6F7;
_tmp6F7.tag=Cyc_Exp_tok;_tmp6F7.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6F9;_tmp6F9.tag=36;_tmp6F9.f1=0;_tmp6F9.f2=0;_tmp6F9;});_tmp6F8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F7;});
_tmp6F6;});break;case 222: _LL2D7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Exp_tok_struct _tmp6FB;_tmp6FB.tag=
Cyc_Exp_tok;_tmp6FB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6FD;_tmp6FD.tag=36;_tmp6FD.f1=0;_tmp6FD.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6FD;});_tmp6FC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp6FB;});_tmp6FA;});break;case 223: _LL2D8: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({
struct Cyc_Exp_tok_struct _tmp6FF;_tmp6FF.tag=Cyc_Exp_tok;_tmp6FF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp700=_cycalloc(sizeof(*_tmp700));
_tmp700[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp701;_tmp701.tag=36;
_tmp701.f1=0;_tmp701.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp701;});_tmp700;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6FF;});
_tmp6FE;});break;case 224: _LL2D9: {struct Cyc_Absyn_Vardecl*_tmp702=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707->f1=(void*)0;_tmp707->f2=({
struct _tagged_arr*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp708;});_tmp707;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp702->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp703=_cycalloc(sizeof(*_tmp703));
_tmp703[0]=({struct Cyc_Exp_tok_struct _tmp704;_tmp704.tag=Cyc_Exp_tok;_tmp704.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp706;_tmp706.tag=29;_tmp706.f1=_tmp702;_tmp706.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp706.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp706.f4=0;
_tmp706;});_tmp705;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp704;});
_tmp703;});break;}case 225: _LL2DA: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_InitializerList_tok_struct
_tmp70A;_tmp70A.tag=Cyc_InitializerList_tok;_tmp70A.f1=({struct Cyc_List_List*
_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B->hd=({struct _tuple20*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C->f1=0;_tmp70C->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70C;});_tmp70B->tl=0;
_tmp70B;});_tmp70A;});_tmp709;});break;case 226: _LL2DB: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_InitializerList_tok_struct
_tmp70E;_tmp70E.tag=Cyc_InitializerList_tok;_tmp70E.f1=({struct Cyc_List_List*
_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F->hd=({struct _tuple20*_tmp710=
_cycalloc(sizeof(*_tmp710));_tmp710->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp710->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp710;});_tmp70F->tl=
0;_tmp70F;});_tmp70E;});_tmp70D;});break;case 227: _LL2DC: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[0]=({struct Cyc_InitializerList_tok_struct
_tmp712;_tmp712.tag=Cyc_InitializerList_tok;_tmp712.f1=({struct Cyc_List_List*
_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->hd=({struct _tuple20*_tmp714=
_cycalloc(sizeof(*_tmp714));_tmp714->f1=0;_tmp714->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp714;});_tmp713->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp713;});_tmp712;});
_tmp711;});break;case 228: _LL2DD: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_InitializerList_tok_struct
_tmp716;_tmp716.tag=Cyc_InitializerList_tok;_tmp716.f1=({struct Cyc_List_List*
_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->hd=({struct _tuple20*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp718->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp718;});_tmp717->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp717;});_tmp716;});_tmp715;});break;case 229: _LL2DE: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp719=_cycalloc(sizeof(*_tmp719));
_tmp719[0]=({struct Cyc_DesignatorList_tok_struct _tmp71A;_tmp71A.tag=Cyc_DesignatorList_tok;
_tmp71A.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp71A;});_tmp719;});
break;case 230: _LL2DF: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({struct Cyc_DesignatorList_tok_struct
_tmp71C;_tmp71C.tag=Cyc_DesignatorList_tok;_tmp71C.f1=({struct Cyc_List_List*
_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71D->tl=0;_tmp71D;});
_tmp71C;});_tmp71B;});break;case 231: _LL2E0: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_DesignatorList_tok_struct
_tmp71F;_tmp71F.tag=Cyc_DesignatorList_tok;_tmp71F.f1=({struct Cyc_List_List*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp720->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp720;});_tmp71F;});
_tmp71E;});break;case 232: _LL2E1: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Designator_tok_struct
_tmp722;_tmp722.tag=Cyc_Designator_tok;_tmp722.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp724;_tmp724.tag=0;_tmp724.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp724;});_tmp723;}));
_tmp722;});_tmp721;});break;case 233: _LL2E2: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Designator_tok_struct
_tmp726;_tmp726.tag=Cyc_Designator_tok;_tmp726.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_Absyn_FieldName_struct
_tmp728;_tmp728.tag=1;_tmp728.f1=({struct _tagged_arr*_tmp729=_cycalloc(sizeof(*
_tmp729));_tmp729[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp729;});_tmp728;});_tmp727;}));_tmp726;});_tmp725;});
break;case 234: _LL2E3: {struct _tuple15 _tmp72B;struct Cyc_Absyn_Tqual _tmp72C;struct
Cyc_List_List*_tmp72D;struct Cyc_List_List*_tmp72E;struct _tuple15*_tmp72A=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp72B=*_tmp72A;
_tmp72C=_tmp72B.f1;_tmp72D=_tmp72B.f2;_tmp72E=_tmp72B.f3;{void*_tmp72F=Cyc_Parse_speclist2typ(
_tmp72D,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp72E != 0)({void*_tmp730[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp731="ignoring attributes in type";
_tag_arr(_tmp731,sizeof(char),_get_zero_arr_size(_tmp731,28));}),_tag_arr(
_tmp730,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_ParamDecl_tok_struct
_tmp733;_tmp733.tag=Cyc_ParamDecl_tok;_tmp733.f1=({struct _tuple2*_tmp734=
_cycalloc(sizeof(*_tmp734));_tmp734->f1=0;_tmp734->f2=_tmp72C;_tmp734->f3=
_tmp72F;_tmp734;});_tmp733;});_tmp732;});break;}}case 235: _LL2E4: {struct _tuple15
_tmp736;struct Cyc_Absyn_Tqual _tmp737;struct Cyc_List_List*_tmp738;struct Cyc_List_List*
_tmp739;struct _tuple15*_tmp735=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp736=*_tmp735;_tmp737=
_tmp736.f1;_tmp738=_tmp736.f2;_tmp739=_tmp736.f3;{void*_tmp73A=Cyc_Parse_speclist2typ(
_tmp738,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp73B=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp73C=Cyc_Parse_apply_tms(_tmp737,_tmp73A,_tmp739,_tmp73B);if(_tmp73C.f3 != 0)({
void*_tmp73D[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp73E="bad type params, ignoring";_tag_arr(_tmp73E,sizeof(char),
_get_zero_arr_size(_tmp73E,26));}),_tag_arr(_tmp73D,sizeof(void*),0));});if(
_tmp73C.f4 != 0)({void*_tmp73F[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp740="bad specifiers, ignoring";_tag_arr(_tmp740,sizeof(char),
_get_zero_arr_size(_tmp740,25));}),_tag_arr(_tmp73F,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp741=_cycalloc(sizeof(*_tmp741));
_tmp741[0]=({struct Cyc_ParamDecl_tok_struct _tmp742;_tmp742.tag=Cyc_ParamDecl_tok;
_tmp742.f1=({struct _tuple2*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->f1=0;
_tmp743->f2=_tmp73C.f1;_tmp743->f3=_tmp73C.f2;_tmp743;});_tmp742;});_tmp741;});
break;}}case 236: _LL2E5: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp744=
_cycalloc(sizeof(*_tmp744));_tmp744[0]=({struct Cyc_Type_tok_struct _tmp745;
_tmp745.tag=Cyc_Type_tok;_tmp745.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp745;});_tmp744;});
break;case 237: _LL2E6: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp746=_cycalloc(
sizeof(*_tmp746));_tmp746[0]=({struct Cyc_Type_tok_struct _tmp747;_tmp747.tag=Cyc_Type_tok;
_tmp747.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp748=_cycalloc(
sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Absyn_JoinEff_struct _tmp749;_tmp749.tag=
20;_tmp749.f1=0;_tmp749;});_tmp748;}));_tmp747;});_tmp746;});break;case 238:
_LL2E7: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74A=_cycalloc(sizeof(*
_tmp74A));_tmp74A[0]=({struct Cyc_Type_tok_struct _tmp74B;_tmp74B.tag=Cyc_Type_tok;
_tmp74B.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp74C=_cycalloc(
sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp74D;_tmp74D.tag=
20;_tmp74D.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp74D;});_tmp74C;}));_tmp74B;});_tmp74A;});break;case 239:
_LL2E8: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74E=_cycalloc(sizeof(*
_tmp74E));_tmp74E[0]=({struct Cyc_Type_tok_struct _tmp74F;_tmp74F.tag=Cyc_Type_tok;
_tmp74F.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp750=_cycalloc(
sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp751;_tmp751.tag=
21;_tmp751.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp751;});_tmp750;}));_tmp74F;});_tmp74E;});break;case
240: _LL2E9: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp752=_cycalloc(sizeof(*
_tmp752));_tmp752[0]=({struct Cyc_Type_tok_struct _tmp753;_tmp753.tag=Cyc_Type_tok;
_tmp753.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp754=_cycalloc(
sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Absyn_JoinEff_struct _tmp755;_tmp755.tag=
20;_tmp755.f1=({struct Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp756->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp756;});_tmp755;});_tmp754;}));_tmp753;});_tmp752;});
break;case 241: _LL2EA: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp757=
_cycalloc(sizeof(*_tmp757));_tmp757[0]=({struct Cyc_TypeList_tok_struct _tmp758;
_tmp758.tag=Cyc_TypeList_tok;_tmp758.f1=({struct Cyc_List_List*_tmp759=_cycalloc(
sizeof(*_tmp759));_tmp759->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp759->tl=0;_tmp759;});
_tmp758;});_tmp757;});break;case 242: _LL2EB: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A[0]=({struct Cyc_TypeList_tok_struct
_tmp75B;_tmp75B.tag=Cyc_TypeList_tok;_tmp75B.f1=({struct Cyc_List_List*_tmp75C=
_cycalloc(sizeof(*_tmp75C));_tmp75C->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75C->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75C;});_tmp75B;});
_tmp75A;});break;case 243: _LL2EC: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp75E;_tmp75E.tag=Cyc_AbstractDeclarator_tok;_tmp75E.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75F;});_tmp75E;});
_tmp75D;});break;case 244: _LL2ED: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 245: _LL2EE: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp761;_tmp761.tag=Cyc_AbstractDeclarator_tok;_tmp761.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp762;});
_tmp761;});_tmp760;});break;case 246: _LL2EF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 247: _LL2F0:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp763=_cycalloc(sizeof(*
_tmp763));_tmp763[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp764;_tmp764.tag=
Cyc_AbstractDeclarator_tok;_tmp764.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->tms=({struct Cyc_List_List*_tmp766=
_cycalloc(sizeof(*_tmp766));_tmp766->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp768;_tmp768.tag=0;_tmp768.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp768.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp768;});
_tmp767;}));_tmp766->tl=0;_tmp766;});_tmp765;});_tmp764;});_tmp763;});break;case
248: _LL2F1: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp769=
_cycalloc(sizeof(*_tmp769));_tmp769[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp76A;_tmp76A.tag=Cyc_AbstractDeclarator_tok;_tmp76A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B->tms=({struct Cyc_List_List*_tmp76C=
_cycalloc(sizeof(*_tmp76C));_tmp76C->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp76E;_tmp76E.tag=0;_tmp76E.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp76E;});
_tmp76D;}));_tmp76C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp76C;});_tmp76B;});
_tmp76A;});_tmp769;});break;case 249: _LL2F2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp770;_tmp770.tag=Cyc_AbstractDeclarator_tok;_tmp770.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->tms=({struct Cyc_List_List*_tmp772=
_cycalloc(sizeof(*_tmp772));_tmp772->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp774;_tmp774.tag=1;_tmp774.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp774.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp774.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp774;});
_tmp773;}));_tmp772->tl=0;_tmp772;});_tmp771;});_tmp770;});_tmp76F;});break;case
250: _LL2F3: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp775=
_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp776;_tmp776.tag=Cyc_AbstractDeclarator_tok;_tmp776.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->tms=({struct Cyc_List_List*_tmp778=
_cycalloc(sizeof(*_tmp778));_tmp778->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp77A;_tmp77A.tag=1;_tmp77A.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77A.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77A.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77A;});
_tmp779;}));_tmp778->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp778;});_tmp777;});
_tmp776;});_tmp775;});break;case 251: _LL2F4: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp77C;_tmp77C.tag=Cyc_AbstractDeclarator_tok;_tmp77C.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->tms=({struct Cyc_List_List*_tmp77E=
_cycalloc(sizeof(*_tmp77E));_tmp77E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp780;_tmp780.tag=3;_tmp780.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp782;_tmp782.tag=1;_tmp782.f1=0;_tmp782.f2=0;_tmp782.f3=0;_tmp782.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp782.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp782;});_tmp781;}));
_tmp780;});_tmp77F;}));_tmp77E->tl=0;_tmp77E;});_tmp77D;});_tmp77C;});_tmp77B;});
break;case 252: _LL2F5: {struct _tuple14 _tmp784;struct Cyc_List_List*_tmp785;int
_tmp786;struct Cyc_Absyn_VarargInfo*_tmp787;struct Cyc_Core_Opt*_tmp788;struct Cyc_List_List*
_tmp789;struct _tuple14*_tmp783=Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp784=*_tmp783;_tmp785=_tmp784.f1;_tmp786=_tmp784.f2;
_tmp787=_tmp784.f3;_tmp788=_tmp784.f4;_tmp789=_tmp784.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp78A=_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp78B;_tmp78B.tag=Cyc_AbstractDeclarator_tok;
_tmp78B.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp78C=_cycalloc(sizeof(*
_tmp78C));_tmp78C->tms=({struct Cyc_List_List*_tmp78D=_cycalloc(sizeof(*_tmp78D));
_tmp78D->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp78E=
_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp78F;_tmp78F.tag=3;_tmp78F.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp791;_tmp791.tag=1;_tmp791.f1=_tmp785;_tmp791.f2=_tmp786;_tmp791.f3=_tmp787;
_tmp791.f4=_tmp788;_tmp791.f5=_tmp789;_tmp791;});_tmp790;}));_tmp78F;});_tmp78E;}));
_tmp78D->tl=0;_tmp78D;});_tmp78C;});_tmp78B;});_tmp78A;});break;}case 253: _LL2F6:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp792=_cycalloc(sizeof(*
_tmp792));_tmp792[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp793;_tmp793.tag=
Cyc_AbstractDeclarator_tok;_tmp793.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794->tms=({struct Cyc_List_List*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp797;_tmp797.tag=3;_tmp797.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp799;_tmp799.tag=1;_tmp799.f1=0;_tmp799.f2=0;_tmp799.f3=0;_tmp799.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp799.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp799;});_tmp798;}));
_tmp797;});_tmp796;}));_tmp795->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp795;});_tmp794;});
_tmp793;});_tmp792;});break;case 254: _LL2F7: {struct _tuple14 _tmp79B;struct Cyc_List_List*
_tmp79C;int _tmp79D;struct Cyc_Absyn_VarargInfo*_tmp79E;struct Cyc_Core_Opt*_tmp79F;
struct Cyc_List_List*_tmp7A0;struct _tuple14*_tmp79A=Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79B=*_tmp79A;_tmp79C=
_tmp79B.f1;_tmp79D=_tmp79B.f2;_tmp79E=_tmp79B.f3;_tmp79F=_tmp79B.f4;_tmp7A0=
_tmp79B.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7A2;_tmp7A2.tag=Cyc_AbstractDeclarator_tok;_tmp7A2.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3->tms=({struct Cyc_List_List*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp7A6;_tmp7A6.tag=3;_tmp7A6.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp7A8;_tmp7A8.tag=1;_tmp7A8.f1=_tmp79C;_tmp7A8.f2=_tmp79D;_tmp7A8.f3=_tmp79E;
_tmp7A8.f4=_tmp79F;_tmp7A8.f5=_tmp7A0;_tmp7A8;});_tmp7A7;}));_tmp7A6;});_tmp7A5;}));
_tmp7A4->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7A4;});_tmp7A3;});
_tmp7A2;});_tmp7A1;});break;}case 255: _LL2F8: {struct Cyc_List_List*_tmp7A9=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp7AB;_tmp7AB.tag=Cyc_AbstractDeclarator_tok;
_tmp7AB.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp7AC=_cycalloc(sizeof(*
_tmp7AC));_tmp7AC->tms=({struct Cyc_List_List*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));
_tmp7AD->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp7AE=
_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp7AF;_tmp7AF.tag=4;_tmp7AF.f1=_tmp7A9;_tmp7AF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7AF.f3=0;
_tmp7AF;});_tmp7AE;}));_tmp7AD->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7AD;});_tmp7AC;});
_tmp7AB;});_tmp7AA;});break;}case 256: _LL2F9: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7B1;_tmp7B1.tag=Cyc_AbstractDeclarator_tok;_tmp7B1.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2->tms=({struct Cyc_List_List*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp7B5;_tmp7B5.tag=5;_tmp7B5.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B5.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7B5;});_tmp7B4;}));
_tmp7B3->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp7B3;});_tmp7B2;});
_tmp7B1;});_tmp7B0;});break;case 257: _LL2FA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 258: _LL2FB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 259: _LL2FC:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 260:
_LL2FD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
261: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 262: _LL2FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 263: _LL300: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7B6="`H";
_tag_arr(_tmp7B6,sizeof(char),_get_zero_arr_size(_tmp7B6,3));}))== 0)Cyc_Parse_err(({
const char*_tmp7B7="bad occurrence of heap region `H";_tag_arr(_tmp7B7,sizeof(
char),_get_zero_arr_size(_tmp7B7,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2->name=({
struct _tagged_arr*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7C5;});_tmp7C2->identity=
0;_tmp7C2->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7C3=_cycalloc(
sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7C4;_tmp7C4.tag=0;
_tmp7C4.f1=(void*)((void*)3);_tmp7C4;});_tmp7C3;}));_tmp7C2;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0[0]=({
struct Cyc_Absyn_VarType_struct _tmp7C1;_tmp7C1.tag=1;_tmp7C1.f1=tv;_tmp7C1;});
_tmp7C0;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7B8=_cycalloc(sizeof(*
_tmp7B8));_tmp7B8[0]=({struct Cyc_Stmt_tok_struct _tmp7B9;_tmp7B9.tag=Cyc_Stmt_tok;
_tmp7B9.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7BA=
_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=({struct Cyc_Absyn_Region_s_struct _tmp7BB;
_tmp7BB.tag=15;_tmp7BB.f1=tv;_tmp7BB.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC->f1=(void*)0;_tmp7BC->f2=({struct
_tagged_arr*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7BD;});_tmp7BC;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));
_tmp7BE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7BF;_tmp7BF.tag=15;_tmp7BF.f1=(
void*)t;_tmp7BF;});_tmp7BE;}),0);_tmp7BB.f3=0;_tmp7BB.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BB;});_tmp7BA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B9;});
_tmp7B8;});break;}case 264: _LL301: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C6="H";
_tag_arr(_tmp7C6,sizeof(char),_get_zero_arr_size(_tmp7C6,2));}))== 0)Cyc_Parse_err(({
const char*_tmp7C7="bad occurrence of heap region `H";_tag_arr(_tmp7C7,sizeof(
char),_get_zero_arr_size(_tmp7C7,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2->name=({
struct _tagged_arr*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp7D8;_tmp7D8.tag=0;_tmp7D8.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7D6[1]={&
_tmp7D8};Cyc_aprintf(({const char*_tmp7D7="`%s";_tag_arr(_tmp7D7,sizeof(char),
_get_zero_arr_size(_tmp7D7,4));}),_tag_arr(_tmp7D6,sizeof(void*),1));}});_tmp7D5;});
_tmp7D2->identity=0;_tmp7D2->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7D4;_tmp7D4.tag=0;_tmp7D4.f1=(void*)((void*)3);_tmp7D4;});_tmp7D3;}));
_tmp7D2;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7D0=_cycalloc(
sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Absyn_VarType_struct _tmp7D1;_tmp7D1.tag=
1;_tmp7D1.f1=tv;_tmp7D1;});_tmp7D0;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Stmt_tok_struct
_tmp7C9;_tmp7C9.tag=Cyc_Stmt_tok;_tmp7C9.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7CB;_tmp7CB.tag=15;_tmp7CB.f1=tv;_tmp7CB.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));
_tmp7CC->f1=(void*)0;_tmp7CC->f2=({struct _tagged_arr*_tmp7CD=_cycalloc(sizeof(*
_tmp7CD));_tmp7CD[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7CD;});_tmp7CC;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7CF;_tmp7CF.tag=15;_tmp7CF.f1=(void*)t;_tmp7CF;});_tmp7CE;}),0);_tmp7CB.f3=0;
_tmp7CB.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7CB;});_tmp7CA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C9;});
_tmp7C8;});break;}case 265: _LL302: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp7D9="resetable";
_tag_arr(_tmp7D9,sizeof(char),_get_zero_arr_size(_tmp7D9,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7DA="expecting [resetable]";_tag_arr(_tmp7DA,sizeof(char),
_get_zero_arr_size(_tmp7DA,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp7DB="`H";_tag_arr(_tmp7DB,sizeof(char),
_get_zero_arr_size(_tmp7DB,3));}))== 0)Cyc_Parse_err(({const char*_tmp7DC="bad occurrence of heap region `H";
_tag_arr(_tmp7DC,sizeof(char),_get_zero_arr_size(_tmp7DC,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7->name=({
struct _tagged_arr*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7EA;});_tmp7E7->identity=
0;_tmp7E7->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7E8=_cycalloc(
sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7E9;_tmp7E9.tag=0;
_tmp7E9.f1=(void*)((void*)3);_tmp7E9;});_tmp7E8;}));_tmp7E7;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({
struct Cyc_Absyn_VarType_struct _tmp7E6;_tmp7E6.tag=1;_tmp7E6.f1=tv;_tmp7E6;});
_tmp7E5;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7DD=_cycalloc(sizeof(*
_tmp7DD));_tmp7DD[0]=({struct Cyc_Stmt_tok_struct _tmp7DE;_tmp7DE.tag=Cyc_Stmt_tok;
_tmp7DE.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7DF=
_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=({struct Cyc_Absyn_Region_s_struct _tmp7E0;
_tmp7E0.tag=15;_tmp7E0.f1=tv;_tmp7E0.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1->f1=(void*)0;_tmp7E1->f2=({struct
_tagged_arr*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7E2;});_tmp7E1;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));
_tmp7E3[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7E4;_tmp7E4.tag=15;_tmp7E4.f1=(
void*)t;_tmp7E4;});_tmp7E3;}),0);_tmp7E0.f3=1;_tmp7E0.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E0;});_tmp7DF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DE;});
_tmp7DD;});break;}case 266: _LL303: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7EB="resetable";
_tag_arr(_tmp7EB,sizeof(char),_get_zero_arr_size(_tmp7EB,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7EC="expecting `resetable'";_tag_arr(_tmp7EC,sizeof(char),
_get_zero_arr_size(_tmp7EC,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp7ED="H";_tag_arr(_tmp7ED,sizeof(char),
_get_zero_arr_size(_tmp7ED,2));}))== 0)Cyc_Parse_err(({const char*_tmp7EE="bad occurrence of heap region `H";
_tag_arr(_tmp7EE,sizeof(char),_get_zero_arr_size(_tmp7EE,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9->name=({
struct _tagged_arr*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp7FF;_tmp7FF.tag=0;_tmp7FF.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7FD[1]={&
_tmp7FF};Cyc_aprintf(({const char*_tmp7FE="`%s";_tag_arr(_tmp7FE,sizeof(char),
_get_zero_arr_size(_tmp7FE,4));}),_tag_arr(_tmp7FD,sizeof(void*),1));}});_tmp7FC;});
_tmp7F9->identity=0;_tmp7F9->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7FB;_tmp7FB.tag=0;_tmp7FB.f1=(void*)((void*)3);_tmp7FB;});_tmp7FA;}));
_tmp7F9;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7F7=_cycalloc(
sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_Absyn_VarType_struct _tmp7F8;_tmp7F8.tag=
1;_tmp7F8.f1=tv;_tmp7F8;});_tmp7F7;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Stmt_tok_struct
_tmp7F0;_tmp7F0.tag=Cyc_Stmt_tok;_tmp7F0.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7F2;_tmp7F2.tag=15;_tmp7F2.f1=tv;_tmp7F2.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));
_tmp7F3->f1=(void*)0;_tmp7F3->f2=({struct _tagged_arr*_tmp7F4=_cycalloc(sizeof(*
_tmp7F4));_tmp7F4[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7F4;});_tmp7F3;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7F6;_tmp7F6.tag=15;_tmp7F6.f1=(void*)t;_tmp7F6;});_tmp7F5;}),0);_tmp7F2.f3=1;
_tmp7F2.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F2;});_tmp7F1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F0;});
_tmp7EF;});break;}case 267: _LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=({struct Cyc_Stmt_tok_struct
_tmp801;_tmp801.tag=Cyc_Stmt_tok;_tmp801.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp803;_tmp803.tag=16;_tmp803.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp803;});_tmp802;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp801;});_tmp800;});break;case 268: _LL305: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({
struct Cyc_Stmt_tok_struct _tmp805;_tmp805.tag=Cyc_Stmt_tok;_tmp805.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp806=_cycalloc(sizeof(*_tmp806));
_tmp806[0]=({struct Cyc_Absyn_Label_s_struct _tmp807;_tmp807.tag=12;_tmp807.f1=({
struct _tagged_arr*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp808;});_tmp807.f2=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp807;});_tmp806;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp805;});
_tmp804;});break;case 269: _LL306: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp809=
_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Stmt_tok_struct _tmp80A;
_tmp80A.tag=Cyc_Stmt_tok;_tmp80A.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80A;});
_tmp809;});break;case 270: _LL307: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80B=
_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Stmt_tok_struct _tmp80C;
_tmp80C.tag=Cyc_Stmt_tok;_tmp80C.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80C;});
_tmp80B;});break;case 271: _LL308: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp80D=
_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=({struct Cyc_Stmt_tok_struct _tmp80E;
_tmp80E.tag=Cyc_Stmt_tok;_tmp80E.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80E;});
_tmp80D;});break;case 272: _LL309: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 273: _LL30A: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({struct Cyc_Stmt_tok_struct
_tmp810;_tmp810.tag=Cyc_Stmt_tok;_tmp810.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp810;});_tmp80F;});break;case 274: _LL30B: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({struct Cyc_Stmt_tok_struct
_tmp812;_tmp812.tag=Cyc_Stmt_tok;_tmp812.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp812;});_tmp811;});
break;case 275: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 276: _LL30D: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[0]=({struct Cyc_Stmt_tok_struct
_tmp814;_tmp814.tag=Cyc_Stmt_tok;_tmp814.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp814;});
_tmp813;});break;case 277: _LL30E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp815=
_cycalloc(sizeof(*_tmp815));_tmp815[0]=({struct Cyc_Stmt_tok_struct _tmp816;
_tmp816.tag=Cyc_Stmt_tok;_tmp816.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp818;_tmp818.tag=1;_tmp818.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp818;});_tmp817;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp816;});_tmp815;});break;
case 278: _LL30F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp819=_cycalloc(
sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Stmt_tok_struct _tmp81A;_tmp81A.tag=Cyc_Stmt_tok;
_tmp81A.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp81C;_tmp81C.tag=1;_tmp81C.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp81C;});_tmp81B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp81A;});_tmp819;});
break;case 279: _LL310: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp81D=_cycalloc(
sizeof(*_tmp81D));_tmp81D[0]=({struct Cyc_Stmt_tok_struct _tmp81E;_tmp81E.tag=Cyc_Stmt_tok;
_tmp81E.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81E;});
_tmp81D;});break;case 280: _LL311: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp81F=
_cycalloc(sizeof(*_tmp81F));_tmp81F[0]=({struct Cyc_Stmt_tok_struct _tmp820;
_tmp820.tag=Cyc_Stmt_tok;_tmp820.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp820;});
_tmp81F;});break;case 281: _LL312: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp821=
_cycalloc(sizeof(*_tmp821));_tmp821[0]=({struct Cyc_Stmt_tok_struct _tmp822;
_tmp822.tag=Cyc_Stmt_tok;_tmp822.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp822;});
_tmp821;});break;case 282: _LL313: {struct Cyc_Absyn_Exp*_tmp823=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp826=_cycalloc(sizeof(*_tmp826));
_tmp826[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp827;_tmp827.tag=2;_tmp827.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp827;});_tmp826;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[0]=({
struct Cyc_Stmt_tok_struct _tmp825;_tmp825.tag=Cyc_Stmt_tok;_tmp825.f1=Cyc_Absyn_switch_stmt(
_tmp823,Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp825;});
_tmp824;});break;}case 283: _LL314: {struct Cyc_Absyn_Exp*_tmp828=Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp829=
_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Stmt_tok_struct _tmp82A;
_tmp82A.tag=Cyc_Stmt_tok;_tmp82A.f1=Cyc_Absyn_switch_stmt(_tmp828,Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82A;});
_tmp829;});break;}case 284: _LL315: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_Stmt_tok_struct
_tmp82C;_tmp82C.tag=Cyc_Stmt_tok;_tmp82C.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82C;});
_tmp82B;});break;case 285: _LL316: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp82E;_tmp82E.tag=Cyc_SwitchClauseList_tok;_tmp82E.f1=0;_tmp82E;});_tmp82D;});
break;case 286: _LL317: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp830;_tmp830.tag=Cyc_SwitchClauseList_tok;_tmp830.f1=({struct Cyc_List_List*
_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->hd=({struct Cyc_Absyn_Switch_clause*
_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp832->pat_vars=0;_tmp832->where_clause=0;
_tmp832->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp832->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp832;});
_tmp831->tl=0;_tmp831;});_tmp830;});_tmp82F;});break;case 287: _LL318: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp833=_cycalloc(sizeof(*_tmp833));
_tmp833[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp834;_tmp834.tag=Cyc_SwitchClauseList_tok;
_tmp834.f1=({struct Cyc_List_List*_tmp835=_cycalloc(sizeof(*_tmp835));_tmp835->hd=({
struct Cyc_Absyn_Switch_clause*_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp836->pat_vars=0;_tmp836->where_clause=0;_tmp836->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp836->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp836;});
_tmp835->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp835;});_tmp834;});_tmp833;});break;case 288: _LL319:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp837=_cycalloc(sizeof(*
_tmp837));_tmp837[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp838;_tmp838.tag=
Cyc_SwitchClauseList_tok;_tmp838.f1=({struct Cyc_List_List*_tmp839=_cycalloc(
sizeof(*_tmp839));_tmp839->hd=({struct Cyc_Absyn_Switch_clause*_tmp83A=_cycalloc(
sizeof(*_tmp83A));_tmp83A->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp83A->pat_vars=0;
_tmp83A->where_clause=0;_tmp83A->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp83A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp83A;});
_tmp839->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp839;});_tmp838;});_tmp837;});break;case 289: _LL31A:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp83B=_cycalloc(sizeof(*
_tmp83B));_tmp83B[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp83C;_tmp83C.tag=
Cyc_SwitchClauseList_tok;_tmp83C.f1=({struct Cyc_List_List*_tmp83D=_cycalloc(
sizeof(*_tmp83D));_tmp83D->hd=({struct Cyc_Absyn_Switch_clause*_tmp83E=_cycalloc(
sizeof(*_tmp83E));_tmp83E->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp83E->pat_vars=0;
_tmp83E->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp83E->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp83E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp83E;});
_tmp83D->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp83D;});_tmp83C;});_tmp83B;});break;case 290: _LL31B:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp83F=_cycalloc(sizeof(*
_tmp83F));_tmp83F[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp840;_tmp840.tag=
Cyc_SwitchClauseList_tok;_tmp840.f1=({struct Cyc_List_List*_tmp841=_cycalloc(
sizeof(*_tmp841));_tmp841->hd=({struct Cyc_Absyn_Switch_clause*_tmp842=_cycalloc(
sizeof(*_tmp842));_tmp842->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp842->pat_vars=0;
_tmp842->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp842->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp842->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp842;});
_tmp841->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp841;});_tmp840;});_tmp83F;});break;case 291: _LL31C:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp843=_cycalloc(sizeof(*_tmp843));
_tmp843[0]=({struct Cyc_Stmt_tok_struct _tmp844;_tmp844.tag=Cyc_Stmt_tok;_tmp844.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp844;});_tmp843;});break;case 292: _LL31D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845[0]=({
struct Cyc_Stmt_tok_struct _tmp846;_tmp846.tag=Cyc_Stmt_tok;_tmp846.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp846;});_tmp845;});break;case 293: _LL31E: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847[0]=({
struct Cyc_Stmt_tok_struct _tmp848;_tmp848.tag=Cyc_Stmt_tok;_tmp848.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
_tmp847;});break;case 294: _LL31F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp849=
_cycalloc(sizeof(*_tmp849));_tmp849[0]=({struct Cyc_Stmt_tok_struct _tmp84A;
_tmp84A.tag=Cyc_Stmt_tok;_tmp84A.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
_tmp849;});break;case 295: _LL320: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp84B=
_cycalloc(sizeof(*_tmp84B));_tmp84B[0]=({struct Cyc_Stmt_tok_struct _tmp84C;
_tmp84C.tag=Cyc_Stmt_tok;_tmp84C.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
_tmp84B;});break;case 296: _LL321: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp84D=
_cycalloc(sizeof(*_tmp84D));_tmp84D[0]=({struct Cyc_Stmt_tok_struct _tmp84E;
_tmp84E.tag=Cyc_Stmt_tok;_tmp84E.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp84E;});
_tmp84D;});break;case 297: _LL322: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp84F=
_cycalloc(sizeof(*_tmp84F));_tmp84F[0]=({struct Cyc_Stmt_tok_struct _tmp850;
_tmp850.tag=Cyc_Stmt_tok;_tmp850.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
_tmp84F;});break;case 298: _LL323: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp851=
_cycalloc(sizeof(*_tmp851));_tmp851[0]=({struct Cyc_Stmt_tok_struct _tmp852;
_tmp852.tag=Cyc_Stmt_tok;_tmp852.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
_tmp851;});break;case 299: _LL324: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp853=
_cycalloc(sizeof(*_tmp853));_tmp853[0]=({struct Cyc_Stmt_tok_struct _tmp854;
_tmp854.tag=Cyc_Stmt_tok;_tmp854.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
_tmp853;});break;case 300: _LL325: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp855=
_cycalloc(sizeof(*_tmp855));_tmp855[0]=({struct Cyc_Stmt_tok_struct _tmp856;
_tmp856.tag=Cyc_Stmt_tok;_tmp856.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp856;});
_tmp855;});break;case 301: _LL326: {struct Cyc_List_List*_tmp857=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp858=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp859=
_cycalloc(sizeof(*_tmp859));_tmp859[0]=({struct Cyc_Stmt_tok_struct _tmp85A;
_tmp85A.tag=Cyc_Stmt_tok;_tmp85A.f1=Cyc_Parse_flatten_declarations(_tmp857,
_tmp858);_tmp85A;});_tmp859;});break;}case 302: _LL327: {struct Cyc_List_List*
_tmp85B=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp85C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({
struct Cyc_Stmt_tok_struct _tmp85E;_tmp85E.tag=Cyc_Stmt_tok;_tmp85E.f1=Cyc_Parse_flatten_declarations(
_tmp85B,_tmp85C);_tmp85E;});_tmp85D;});break;}case 303: _LL328: {struct Cyc_List_List*
_tmp85F=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp860=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=({
struct Cyc_Stmt_tok_struct _tmp862;_tmp862.tag=Cyc_Stmt_tok;_tmp862.f1=Cyc_Parse_flatten_declarations(
_tmp85F,_tmp860);_tmp862;});_tmp861;});break;}case 304: _LL329: {struct Cyc_List_List*
_tmp863=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp864=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865[0]=({
struct Cyc_Stmt_tok_struct _tmp866;_tmp866.tag=Cyc_Stmt_tok;_tmp866.f1=Cyc_Parse_flatten_declarations(
_tmp863,_tmp864);_tmp866;});_tmp865;});break;}case 305: _LL32A: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867[0]=({
struct Cyc_Stmt_tok_struct _tmp868;_tmp868.tag=Cyc_Stmt_tok;_tmp868.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp869;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp868;});
_tmp867;});break;case 306: _LL32B: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp86A=
_cycalloc(sizeof(*_tmp86A));_tmp86A[0]=({struct Cyc_Stmt_tok_struct _tmp86B;
_tmp86B.tag=Cyc_Stmt_tok;_tmp86B.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp86B;});
_tmp86A;});break;case 307: _LL32C: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp86C=
_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({struct Cyc_Stmt_tok_struct _tmp86D;
_tmp86D.tag=Cyc_Stmt_tok;_tmp86D.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp86D;});
_tmp86C;});break;case 308: _LL32D: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp86E=
_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Stmt_tok_struct _tmp86F;
_tmp86F.tag=Cyc_Stmt_tok;_tmp86F.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp86F;});
_tmp86E;});break;case 309: _LL32E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp870=
_cycalloc(sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Stmt_tok_struct _tmp871;
_tmp871.tag=Cyc_Stmt_tok;_tmp871.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp871;});_tmp870;});break;case 310: _LL32F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({
struct Cyc_Stmt_tok_struct _tmp873;_tmp873.tag=Cyc_Stmt_tok;_tmp873.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp873;});_tmp872;});break;case 311: _LL330: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874[0]=({
struct Cyc_Stmt_tok_struct _tmp875;_tmp875.tag=Cyc_Stmt_tok;_tmp875.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp875;});_tmp874;});break;case 312: _LL331: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876[0]=({
struct Cyc_Stmt_tok_struct _tmp877;_tmp877.tag=Cyc_Stmt_tok;_tmp877.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp877;});_tmp876;});break;case 313: _LL332: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 314: _LL333:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 315:
_LL334: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp878=_cycalloc(sizeof(*
_tmp878));_tmp878[0]=({struct Cyc_Pattern_tok_struct _tmp879;_tmp879.tag=Cyc_Pattern_tok;
_tmp879.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp879;});
_tmp878;});break;case 316: _LL335: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 317: _LL336: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87A=_cycalloc(sizeof(*_tmp87A));_tmp87A[0]=({struct Cyc_Pattern_tok_struct
_tmp87B;_tmp87B.tag=Cyc_Pattern_tok;_tmp87B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87B;});
_tmp87A;});break;case 318: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 319: _LL338: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Pattern_tok_struct
_tmp87D;_tmp87D.tag=Cyc_Pattern_tok;_tmp87D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87D;});
_tmp87C;});break;case 320: _LL339: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 321: _LL33A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E[0]=({struct Cyc_Pattern_tok_struct
_tmp87F;_tmp87F.tag=Cyc_Pattern_tok;_tmp87F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp87F;});
_tmp87E;});break;case 322: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 323: _LL33C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880[0]=({struct Cyc_Pattern_tok_struct
_tmp881;_tmp881.tag=Cyc_Pattern_tok;_tmp881.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp881;});
_tmp880;});break;case 324: _LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL33E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Pattern_tok_struct
_tmp883;_tmp883.tag=Cyc_Pattern_tok;_tmp883.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp883;});
_tmp882;});break;case 326: _LL33F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL340: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp884=_cycalloc(sizeof(*_tmp884));_tmp884[0]=({struct Cyc_Pattern_tok_struct
_tmp885;_tmp885.tag=Cyc_Pattern_tok;_tmp885.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp885;});
_tmp884;});break;case 328: _LL341: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp886=_cycalloc(sizeof(*_tmp886));_tmp886[0]=({struct Cyc_Pattern_tok_struct
_tmp887;_tmp887.tag=Cyc_Pattern_tok;_tmp887.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp887;});
_tmp886;});break;case 329: _LL342: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 330: _LL343: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888[0]=({struct Cyc_Pattern_tok_struct
_tmp889;_tmp889.tag=Cyc_Pattern_tok;_tmp889.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp889;});
_tmp888;});break;case 331: _LL344: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88A=_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=({struct Cyc_Pattern_tok_struct
_tmp88B;_tmp88B.tag=Cyc_Pattern_tok;_tmp88B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88B;});
_tmp88A;});break;case 332: _LL345: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Pattern_tok_struct
_tmp88D;_tmp88D.tag=Cyc_Pattern_tok;_tmp88D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88D;});
_tmp88C;});break;case 333: _LL346: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=({struct Cyc_Pattern_tok_struct
_tmp88F;_tmp88F.tag=Cyc_Pattern_tok;_tmp88F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88F;});
_tmp88E;});break;case 334: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL348: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890[0]=({struct Cyc_Pattern_tok_struct
_tmp891;_tmp891.tag=Cyc_Pattern_tok;_tmp891.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp891;});
_tmp890;});break;case 336: _LL349: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Pattern_tok_struct
_tmp893;_tmp893.tag=Cyc_Pattern_tok;_tmp893.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp893;});
_tmp892;});break;case 337: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL34B: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894[0]=({struct Cyc_Pattern_tok_struct
_tmp895;_tmp895.tag=Cyc_Pattern_tok;_tmp895.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp895;});
_tmp894;});break;case 339: _LL34C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Pattern_tok_struct
_tmp897;_tmp897.tag=Cyc_Pattern_tok;_tmp897.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp897;});
_tmp896;});break;case 340: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL34E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp898=_cycalloc(sizeof(*_tmp898));_tmp898[0]=({struct Cyc_Pattern_tok_struct
_tmp899;_tmp899.tag=Cyc_Pattern_tok;_tmp899.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp899;});
_tmp898;});break;case 342: _LL34F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Pattern_tok_struct
_tmp89B;_tmp89B.tag=Cyc_Pattern_tok;_tmp89B.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89B;});
_tmp89A;});break;case 343: _LL350: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=({struct Cyc_Pattern_tok_struct
_tmp89D;_tmp89D.tag=Cyc_Pattern_tok;_tmp89D.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89D;});
_tmp89C;});break;case 344: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL352: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Pattern_tok_struct
_tmp89F;_tmp89F.tag=Cyc_Pattern_tok;_tmp89F.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)])).f3,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89F;});
_tmp89E;});break;case 346: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 347: _LL354: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({struct Cyc_Pattern_tok_struct
_tmp8A1;_tmp8A1.tag=Cyc_Pattern_tok;_tmp8A1.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp8A1;});_tmp8A0;});
break;case 348: _LL355: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8A2=
_cycalloc(sizeof(*_tmp8A2));_tmp8A2[0]=({struct Cyc_Pattern_tok_struct _tmp8A3;
_tmp8A3.tag=Cyc_Pattern_tok;_tmp8A3.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A3;});
_tmp8A2;});break;case 349: _LL356: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A4=_cycalloc(sizeof(*_tmp8A4));_tmp8A4[0]=({struct Cyc_Pattern_tok_struct
_tmp8A5;_tmp8A5.tag=Cyc_Pattern_tok;_tmp8A5.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8A5;});_tmp8A4;});break;case 350: _LL357: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6[
0]=({struct Cyc_Pattern_tok_struct _tmp8A7;_tmp8A7.tag=Cyc_Pattern_tok;_tmp8A7.f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A7;});
_tmp8A6;});break;case 351: _LL358: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A8=_cycalloc(sizeof(*_tmp8A8));_tmp8A8[0]=({struct Cyc_Pattern_tok_struct
_tmp8A9;_tmp8A9.tag=Cyc_Pattern_tok;_tmp8A9.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp8AA=_cycalloc(sizeof(*
_tmp8AA));_tmp8AA[0]=({struct Cyc_Absyn_StructField_struct _tmp8AB;_tmp8AB.tag=0;
_tmp8AB.f1=({struct _tagged_arr*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8AC;});_tmp8AB;});_tmp8AA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A9;});
_tmp8A8;});break;case 352: _LL359: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Pattern_tok_struct
_tmp8AE;_tmp8AE.tag=Cyc_Pattern_tok;_tmp8AE.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp8AF=_cycalloc_atomic(
sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp8B0;_tmp8B0.tag=
1;_tmp8B0.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8B0;});_tmp8AF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8AE;});_tmp8AD;});break;case 353: _LL35A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 354: _LL35B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 355:
_LL35C: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8B1=_cycalloc(sizeof(*
_tmp8B1));_tmp8B1[0]=({struct Cyc_Pattern_tok_struct _tmp8B2;_tmp8B2.tag=Cyc_Pattern_tok;
_tmp8B2.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
_tmp8B1;});break;case 356: _LL35D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3[0]=({struct Cyc_Pattern_tok_struct
_tmp8B4;_tmp8B4.tag=Cyc_Pattern_tok;_tmp8B4.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8B4;});_tmp8B3;});
break;case 357: _LL35E: {struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp8B5=(void*)e->r;
void*_tmp8B6;void*_tmp8B7;char _tmp8B8;void*_tmp8B9;void*_tmp8BA;short _tmp8BB;
void*_tmp8BC;void*_tmp8BD;int _tmp8BE;void*_tmp8BF;struct _tagged_arr _tmp8C0;void*
_tmp8C1;void*_tmp8C2;void*_tmp8C3;_LL361: if(*((int*)_tmp8B5)!= 0)goto _LL363;
_tmp8B6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8B5)->f1;if(_tmp8B6 <= (void*)
1  || *((int*)_tmp8B6)!= 0)goto _LL363;_tmp8B7=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp8B6)->f1;_tmp8B8=((struct Cyc_Absyn_Char_c_struct*)_tmp8B6)->f2;_LL362: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8C4=_cycalloc(sizeof(*_tmp8C4));_tmp8C4[
0]=({struct Cyc_Pattern_tok_struct _tmp8C5;_tmp8C5.tag=Cyc_Pattern_tok;_tmp8C5.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*_tmp8C6=
_cycalloc_atomic(sizeof(*_tmp8C6));_tmp8C6[0]=({struct Cyc_Absyn_Char_p_struct
_tmp8C7;_tmp8C7.tag=8;_tmp8C7.f1=_tmp8B8;_tmp8C7;});_tmp8C6;}),e->loc);_tmp8C5;});
_tmp8C4;});goto _LL360;_LL363: if(*((int*)_tmp8B5)!= 0)goto _LL365;_tmp8B9=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8B5)->f1;if(_tmp8B9 <= (void*)1  || *((int*)
_tmp8B9)!= 1)goto _LL365;_tmp8BA=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp8B9)->f1;
_tmp8BB=((struct Cyc_Absyn_Short_c_struct*)_tmp8B9)->f2;_LL364: yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8[0]=({
struct Cyc_Pattern_tok_struct _tmp8C9;_tmp8C9.tag=Cyc_Pattern_tok;_tmp8C9.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));_tmp8CA[
0]=({struct Cyc_Absyn_Int_p_struct _tmp8CB;_tmp8CB.tag=7;_tmp8CB.f1=(void*)_tmp8BA;
_tmp8CB.f2=(int)_tmp8BB;_tmp8CB;});_tmp8CA;}),e->loc);_tmp8C9;});_tmp8C8;});goto
_LL360;_LL365: if(*((int*)_tmp8B5)!= 0)goto _LL367;_tmp8BC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B5)->f1;if(_tmp8BC <= (void*)1  || *((int*)_tmp8BC)!= 2)goto _LL367;_tmp8BD=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp8BC)->f1;_tmp8BE=((struct Cyc_Absyn_Int_c_struct*)
_tmp8BC)->f2;_LL366: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8CC=
_cycalloc(sizeof(*_tmp8CC));_tmp8CC[0]=({struct Cyc_Pattern_tok_struct _tmp8CD;
_tmp8CD.tag=Cyc_Pattern_tok;_tmp8CD.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp8CE=_cycalloc(sizeof(*_tmp8CE));_tmp8CE[0]=({struct Cyc_Absyn_Int_p_struct
_tmp8CF;_tmp8CF.tag=7;_tmp8CF.f1=(void*)_tmp8BD;_tmp8CF.f2=_tmp8BE;_tmp8CF;});
_tmp8CE;}),e->loc);_tmp8CD;});_tmp8CC;});goto _LL360;_LL367: if(*((int*)_tmp8B5)!= 
0)goto _LL369;_tmp8BF=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8B5)->f1;if(
_tmp8BF <= (void*)1  || *((int*)_tmp8BF)!= 4)goto _LL369;_tmp8C0=((struct Cyc_Absyn_Float_c_struct*)
_tmp8BF)->f1;_LL368: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8D0=
_cycalloc(sizeof(*_tmp8D0));_tmp8D0[0]=({struct Cyc_Pattern_tok_struct _tmp8D1;
_tmp8D1.tag=Cyc_Pattern_tok;_tmp8D1.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*
_tmp8D2=_cycalloc(sizeof(*_tmp8D2));_tmp8D2[0]=({struct Cyc_Absyn_Float_p_struct
_tmp8D3;_tmp8D3.tag=9;_tmp8D3.f1=_tmp8C0;_tmp8D3;});_tmp8D2;}),e->loc);_tmp8D1;});
_tmp8D0;});goto _LL360;_LL369: if(*((int*)_tmp8B5)!= 0)goto _LL36B;_tmp8C1=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8B5)->f1;if((int)_tmp8C1 != 0)goto _LL36B;
_LL36A: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8D4=_cycalloc(sizeof(*
_tmp8D4));_tmp8D4[0]=({struct Cyc_Pattern_tok_struct _tmp8D5;_tmp8D5.tag=Cyc_Pattern_tok;
_tmp8D5.f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp8D5;});_tmp8D4;});goto _LL360;
_LL36B: if(*((int*)_tmp8B5)!= 0)goto _LL36D;_tmp8C2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B5)->f1;if(_tmp8C2 <= (void*)1  || *((int*)_tmp8C2)!= 5)goto _LL36D;_LL36C: Cyc_Parse_err(({
const char*_tmp8D6="strings cannot occur within patterns";_tag_arr(_tmp8D6,
sizeof(char),_get_zero_arr_size(_tmp8D6,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL360;
_LL36D: if(*((int*)_tmp8B5)!= 0)goto _LL36F;_tmp8C3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B5)->f1;if(_tmp8C3 <= (void*)1  || *((int*)_tmp8C3)!= 3)goto _LL36F;_LL36E: Cyc_Parse_unimp(({
const char*_tmp8D7="long long's in patterns";_tag_arr(_tmp8D7,sizeof(char),
_get_zero_arr_size(_tmp8D7,24));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL360;
_LL36F:;_LL370: Cyc_Parse_err(({const char*_tmp8D8="bad constant in case";_tag_arr(
_tmp8D8,sizeof(char),_get_zero_arr_size(_tmp8D8,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL360:;}break;}
case 358: _LL35F: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8D9=_cycalloc(
sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_Pattern_tok_struct _tmp8DA;_tmp8DA.tag=
Cyc_Pattern_tok;_tmp8DA.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp8DB=_cycalloc(sizeof(*_tmp8DB));_tmp8DB[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp8DC;_tmp8DC.tag=12;_tmp8DC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8DC;});_tmp8DB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8DA;});
_tmp8D9;});break;case 359: _LL371: {struct Cyc_List_List*_tmp8DE;int _tmp8DF;struct
_tuple17 _tmp8DD=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8DE=_tmp8DD.f1;_tmp8DF=_tmp8DD.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8E0=_cycalloc(sizeof(*_tmp8E0));_tmp8E0[0]=({struct
Cyc_Pattern_tok_struct _tmp8E1;_tmp8E1.tag=Cyc_Pattern_tok;_tmp8E1.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));
_tmp8E2[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp8E3;_tmp8E3.tag=3;_tmp8E3.f1=
_tmp8DE;_tmp8E3.f2=_tmp8DF;_tmp8E3;});_tmp8E2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E1;});
_tmp8E0;});break;}case 360: _LL372: {struct Cyc_List_List*_tmp8E5;int _tmp8E6;struct
_tuple17 _tmp8E4=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8E5=_tmp8E4.f1;_tmp8E6=_tmp8E4.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({struct
Cyc_Pattern_tok_struct _tmp8E8;_tmp8E8.tag=Cyc_Pattern_tok;_tmp8E8.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp8E9=_cycalloc(sizeof(*_tmp8E9));
_tmp8E9[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp8EA;_tmp8EA.tag=13;_tmp8EA.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp8EA.f2=_tmp8E5;_tmp8EA.f3=_tmp8E6;_tmp8EA;});_tmp8E9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E8;});
_tmp8E7;});break;}case 361: _LL373: {struct Cyc_List_List*_tmp8EC;int _tmp8ED;struct
_tuple17 _tmp8EB=*Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8EC=_tmp8EB.f1;_tmp8ED=_tmp8EB.f2;{struct Cyc_List_List*
_tmp8EE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8EF=_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct
Cyc_Pattern_tok_struct _tmp8F0;_tmp8F0.tag=Cyc_Pattern_tok;_tmp8F0.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp8F1=_cycalloc(sizeof(*_tmp8F1));
_tmp8F1[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp8F2;_tmp8F2.tag=5;_tmp8F2.f1=({
struct Cyc_Absyn_AggrInfo _tmp8F3;_tmp8F3.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8F4=_cycalloc(sizeof(*_tmp8F4));_tmp8F4[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp8F5;_tmp8F5.tag=0;_tmp8F5.f1=(void*)((void*)0);_tmp8F5.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8F5;});_tmp8F4;}));
_tmp8F3.targs=0;_tmp8F3;});_tmp8F2.f2=_tmp8EE;_tmp8F2.f3=_tmp8EC;_tmp8F2.f4=
_tmp8ED;_tmp8F2;});_tmp8F1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F0;});
_tmp8EF;});break;}}case 362: _LL374: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8F6=_cycalloc(sizeof(*_tmp8F6));_tmp8F6[0]=({struct Cyc_Pattern_tok_struct
_tmp8F7;_tmp8F7.tag=Cyc_Pattern_tok;_tmp8F7.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp8F8=_cycalloc(sizeof(*_tmp8F8));_tmp8F8[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp8F9;_tmp8F9.tag=4;_tmp8F9.f1=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8F9;});_tmp8F8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8F7;});_tmp8F6;});break;case 363: _LL375: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA[
0]=({struct Cyc_Pattern_tok_struct _tmp8FB;_tmp8FB.tag=Cyc_Pattern_tok;_tmp8FB.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp8FC=_cycalloc(
sizeof(*_tmp8FC));_tmp8FC[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp8FD;_tmp8FD.tag=
4;_tmp8FD.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp8FE=
_cycalloc(sizeof(*_tmp8FE));_tmp8FE[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp8FF;
_tmp8FF.tag=4;_tmp8FF.f1=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8FF;});_tmp8FE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FD;});
_tmp8FC;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8FB;});_tmp8FA;});break;case 364: _LL376: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900[
0]=({struct Cyc_Pattern_tok_struct _tmp901;_tmp901.tag=Cyc_Pattern_tok;_tmp901.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp902=_cycalloc(
sizeof(*_tmp902));_tmp902[0]=({struct Cyc_Absyn_Reference_p_struct _tmp903;_tmp903.tag=
1;_tmp903.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp904=_cycalloc(sizeof(*
_tmp904));_tmp904->f1=(void*)0;_tmp904->f2=({struct _tagged_arr*_tmp905=_cycalloc(
sizeof(*_tmp905));_tmp905[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp905;});_tmp904;}),(void*)
0,0);_tmp903;});_tmp902;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp901;});
_tmp900;});break;case 365: _LL377: {void*_tmp906=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp90F=_cycalloc(sizeof(*_tmp90F));_tmp90F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp910;_tmp910.tag=0;_tmp910.f1=(void*)((void*)5);_tmp910;});_tmp90F;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907[
0]=({struct Cyc_Pattern_tok_struct _tmp908;_tmp908.tag=Cyc_Pattern_tok;_tmp908.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp909=_cycalloc(
sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp90A;_tmp90A.tag=
2;_tmp90A.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp906);
_tmp90A.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp90B=_cycalloc(sizeof(*
_tmp90B));_tmp90B->f1=(void*)0;_tmp90B->f2=({struct _tagged_arr*_tmp90C=_cycalloc(
sizeof(*_tmp90C));_tmp90C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp90C;});_tmp90B;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp90D=_cycalloc(sizeof(*_tmp90D));
_tmp90D[0]=({struct Cyc_Absyn_TagType_struct _tmp90E;_tmp90E.tag=17;_tmp90E.f1=(
void*)_tmp906;_tmp90E;});_tmp90D;}),0);_tmp90A;});_tmp909;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp908;});
_tmp907;});break;}case 366: _LL378: {struct Cyc_Absyn_Tvar*_tmp911=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp91C=_cycalloc(sizeof(*_tmp91C));_tmp91C[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp91D;_tmp91D.tag=0;_tmp91D.f1=(void*)((void*)
5);_tmp91D;});_tmp91C;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp912=
_cycalloc(sizeof(*_tmp912));_tmp912[0]=({struct Cyc_Pattern_tok_struct _tmp913;
_tmp913.tag=Cyc_Pattern_tok;_tmp913.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp915;_tmp915.tag=2;_tmp915.f1=_tmp911;_tmp915.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp916=_cycalloc(sizeof(*_tmp916));_tmp916->f1=(void*)0;_tmp916->f2=({
struct _tagged_arr*_tmp917=_cycalloc(sizeof(*_tmp917));_tmp917[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp917;});_tmp916;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp918=_cycalloc(sizeof(*_tmp918));
_tmp918[0]=({struct Cyc_Absyn_TagType_struct _tmp919;_tmp919.tag=17;_tmp919.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp91A=_cycalloc(sizeof(*_tmp91A));
_tmp91A[0]=({struct Cyc_Absyn_VarType_struct _tmp91B;_tmp91B.tag=1;_tmp91B.f1=
_tmp911;_tmp91B;});_tmp91A;}));_tmp919;});_tmp918;}),0);_tmp915;});_tmp914;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp913;});_tmp912;});break;}case 367: _LL379: yyval=(
void*)({struct Cyc_YY3_struct*_tmp91E=_cycalloc(sizeof(*_tmp91E));_tmp91E[0]=({
struct Cyc_YY3_struct _tmp91F;_tmp91F.tag=Cyc_YY3;_tmp91F.f1=({struct _tuple17*
_tmp920=_cycalloc(sizeof(*_tmp920));_tmp920->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp920->f2=0;_tmp920;});
_tmp91F;});_tmp91E;});break;case 368: _LL37A: yyval=(void*)({struct Cyc_YY3_struct*
_tmp921=_cycalloc(sizeof(*_tmp921));_tmp921[0]=({struct Cyc_YY3_struct _tmp922;
_tmp922.tag=Cyc_YY3;_tmp922.f1=({struct _tuple17*_tmp923=_cycalloc(sizeof(*
_tmp923));_tmp923->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]));_tmp923->f2=1;_tmp923;});_tmp922;});_tmp921;});break;case 369: _LL37B:
yyval=(void*)({struct Cyc_YY3_struct*_tmp924=_cycalloc(sizeof(*_tmp924));_tmp924[
0]=({struct Cyc_YY3_struct _tmp925;_tmp925.tag=Cyc_YY3;_tmp925.f1=({struct _tuple17*
_tmp926=_cycalloc(sizeof(*_tmp926));_tmp926->f1=0;_tmp926->f2=1;_tmp926;});
_tmp925;});_tmp924;});break;case 370: _LL37C: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp927=_cycalloc(sizeof(*_tmp927));_tmp927[0]=({struct Cyc_PatternList_tok_struct
_tmp928;_tmp928.tag=Cyc_PatternList_tok;_tmp928.f1=({struct Cyc_List_List*_tmp929=
_cycalloc(sizeof(*_tmp929));_tmp929->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp929->tl=0;_tmp929;});
_tmp928;});_tmp927;});break;case 371: _LL37D: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp92A=_cycalloc(sizeof(*_tmp92A));_tmp92A[0]=({struct Cyc_PatternList_tok_struct
_tmp92B;_tmp92B.tag=Cyc_PatternList_tok;_tmp92B.f1=({struct Cyc_List_List*_tmp92C=
_cycalloc(sizeof(*_tmp92C));_tmp92C->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp92C->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp92C;});_tmp92B;});
_tmp92A;});break;case 372: _LL37E: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D[0]=({struct Cyc_FieldPattern_tok_struct
_tmp92E;_tmp92E.tag=Cyc_FieldPattern_tok;_tmp92E.f1=({struct _tuple11*_tmp92F=
_cycalloc(sizeof(*_tmp92F));_tmp92F->f1=0;_tmp92F->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp92F;});_tmp92E;});
_tmp92D;});break;case 373: _LL37F: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp930=_cycalloc(sizeof(*_tmp930));_tmp930[0]=({struct Cyc_FieldPattern_tok_struct
_tmp931;_tmp931.tag=Cyc_FieldPattern_tok;_tmp931.f1=({struct _tuple11*_tmp932=
_cycalloc(sizeof(*_tmp932));_tmp932->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp932->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp932;});_tmp931;});
_tmp930;});break;case 374: _LL380: yyval=(void*)({struct Cyc_YY4_struct*_tmp933=
_cycalloc(sizeof(*_tmp933));_tmp933[0]=({struct Cyc_YY4_struct _tmp934;_tmp934.tag=
Cyc_YY4;_tmp934.f1=({struct _tuple17*_tmp935=_cycalloc(sizeof(*_tmp935));_tmp935->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp935->f2=0;_tmp935;});
_tmp934;});_tmp933;});break;case 375: _LL381: yyval=(void*)({struct Cyc_YY4_struct*
_tmp936=_cycalloc(sizeof(*_tmp936));_tmp936[0]=({struct Cyc_YY4_struct _tmp937;
_tmp937.tag=Cyc_YY4;_tmp937.f1=({struct _tuple17*_tmp938=_cycalloc(sizeof(*
_tmp938));_tmp938->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp938->f2=1;_tmp938;});_tmp937;});_tmp936;});break;case 376:
_LL382: yyval=(void*)({struct Cyc_YY4_struct*_tmp939=_cycalloc(sizeof(*_tmp939));
_tmp939[0]=({struct Cyc_YY4_struct _tmp93A;_tmp93A.tag=Cyc_YY4;_tmp93A.f1=({struct
_tuple17*_tmp93B=_cycalloc(sizeof(*_tmp93B));_tmp93B->f1=0;_tmp93B->f2=1;_tmp93B;});
_tmp93A;});_tmp939;});break;case 377: _LL383: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp93C=_cycalloc(sizeof(*_tmp93C));_tmp93C[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp93D;_tmp93D.tag=Cyc_FieldPatternList_tok;_tmp93D.f1=({struct Cyc_List_List*
_tmp93E=_cycalloc(sizeof(*_tmp93E));_tmp93E->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp93E->tl=0;_tmp93E;});
_tmp93D;});_tmp93C;});break;case 378: _LL384: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp93F=_cycalloc(sizeof(*_tmp93F));_tmp93F[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp940;_tmp940.tag=Cyc_FieldPatternList_tok;_tmp940.f1=({struct Cyc_List_List*
_tmp941=_cycalloc(sizeof(*_tmp941));_tmp941->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp941->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp941;});_tmp940;});
_tmp93F;});break;case 379: _LL385: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 380: _LL386: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp942=_cycalloc(sizeof(*_tmp942));_tmp942[0]=({struct Cyc_Exp_tok_struct _tmp943;
_tmp943.tag=Cyc_Exp_tok;_tmp943.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp943;});
_tmp942;});break;case 381: _LL387: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 382: _LL388: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp944=_cycalloc(sizeof(*_tmp944));_tmp944[0]=({struct Cyc_Exp_tok_struct _tmp945;
_tmp945.tag=Cyc_Exp_tok;_tmp945.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp945;});
_tmp944;});break;case 383: _LL389: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp946=_cycalloc(sizeof(*_tmp946));_tmp946[0]=({struct Cyc_Primopopt_tok_struct
_tmp947;_tmp947.tag=Cyc_Primopopt_tok;_tmp947.f1=0;_tmp947;});_tmp946;});break;
case 384: _LL38A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp948=_cycalloc(
sizeof(*_tmp948));_tmp948[0]=({struct Cyc_Primopopt_tok_struct _tmp949;_tmp949.tag=
Cyc_Primopopt_tok;_tmp949.f1=({struct Cyc_Core_Opt*_tmp94A=_cycalloc(sizeof(*
_tmp94A));_tmp94A->v=(void*)((void*)1);_tmp94A;});_tmp949;});_tmp948;});break;
case 385: _LL38B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp94B=_cycalloc(
sizeof(*_tmp94B));_tmp94B[0]=({struct Cyc_Primopopt_tok_struct _tmp94C;_tmp94C.tag=
Cyc_Primopopt_tok;_tmp94C.f1=({struct Cyc_Core_Opt*_tmp94D=_cycalloc(sizeof(*
_tmp94D));_tmp94D->v=(void*)((void*)3);_tmp94D;});_tmp94C;});_tmp94B;});break;
case 386: _LL38C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp94E=_cycalloc(
sizeof(*_tmp94E));_tmp94E[0]=({struct Cyc_Primopopt_tok_struct _tmp94F;_tmp94F.tag=
Cyc_Primopopt_tok;_tmp94F.f1=({struct Cyc_Core_Opt*_tmp950=_cycalloc(sizeof(*
_tmp950));_tmp950->v=(void*)((void*)4);_tmp950;});_tmp94F;});_tmp94E;});break;
case 387: _LL38D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp951=_cycalloc(
sizeof(*_tmp951));_tmp951[0]=({struct Cyc_Primopopt_tok_struct _tmp952;_tmp952.tag=
Cyc_Primopopt_tok;_tmp952.f1=({struct Cyc_Core_Opt*_tmp953=_cycalloc(sizeof(*
_tmp953));_tmp953->v=(void*)((void*)0);_tmp953;});_tmp952;});_tmp951;});break;
case 388: _LL38E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp954=_cycalloc(
sizeof(*_tmp954));_tmp954[0]=({struct Cyc_Primopopt_tok_struct _tmp955;_tmp955.tag=
Cyc_Primopopt_tok;_tmp955.f1=({struct Cyc_Core_Opt*_tmp956=_cycalloc(sizeof(*
_tmp956));_tmp956->v=(void*)((void*)2);_tmp956;});_tmp955;});_tmp954;});break;
case 389: _LL38F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp957=_cycalloc(
sizeof(*_tmp957));_tmp957[0]=({struct Cyc_Primopopt_tok_struct _tmp958;_tmp958.tag=
Cyc_Primopopt_tok;_tmp958.f1=({struct Cyc_Core_Opt*_tmp959=_cycalloc(sizeof(*
_tmp959));_tmp959->v=(void*)((void*)16);_tmp959;});_tmp958;});_tmp957;});break;
case 390: _LL390: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp95A=_cycalloc(
sizeof(*_tmp95A));_tmp95A[0]=({struct Cyc_Primopopt_tok_struct _tmp95B;_tmp95B.tag=
Cyc_Primopopt_tok;_tmp95B.f1=({struct Cyc_Core_Opt*_tmp95C=_cycalloc(sizeof(*
_tmp95C));_tmp95C->v=(void*)((void*)17);_tmp95C;});_tmp95B;});_tmp95A;});break;
case 391: _LL391: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp95D=_cycalloc(
sizeof(*_tmp95D));_tmp95D[0]=({struct Cyc_Primopopt_tok_struct _tmp95E;_tmp95E.tag=
Cyc_Primopopt_tok;_tmp95E.f1=({struct Cyc_Core_Opt*_tmp95F=_cycalloc(sizeof(*
_tmp95F));_tmp95F->v=(void*)((void*)13);_tmp95F;});_tmp95E;});_tmp95D;});break;
case 392: _LL392: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp960=_cycalloc(
sizeof(*_tmp960));_tmp960[0]=({struct Cyc_Primopopt_tok_struct _tmp961;_tmp961.tag=
Cyc_Primopopt_tok;_tmp961.f1=({struct Cyc_Core_Opt*_tmp962=_cycalloc(sizeof(*
_tmp962));_tmp962->v=(void*)((void*)15);_tmp962;});_tmp961;});_tmp960;});break;
case 393: _LL393: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp963=_cycalloc(
sizeof(*_tmp963));_tmp963[0]=({struct Cyc_Primopopt_tok_struct _tmp964;_tmp964.tag=
Cyc_Primopopt_tok;_tmp964.f1=({struct Cyc_Core_Opt*_tmp965=_cycalloc(sizeof(*
_tmp965));_tmp965->v=(void*)((void*)14);_tmp965;});_tmp964;});_tmp963;});break;
case 394: _LL394: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 395: _LL395: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp966=_cycalloc(
sizeof(*_tmp966));_tmp966[0]=({struct Cyc_Exp_tok_struct _tmp967;_tmp967.tag=Cyc_Exp_tok;
_tmp967.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp967;});
_tmp966;});break;case 396: _LL396: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp968=
_cycalloc(sizeof(*_tmp968));_tmp968[0]=({struct Cyc_Exp_tok_struct _tmp969;_tmp969.tag=
Cyc_Exp_tok;_tmp969.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp969;});
_tmp968;});break;case 397: _LL397: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96A=
_cycalloc(sizeof(*_tmp96A));_tmp96A[0]=({struct Cyc_Exp_tok_struct _tmp96B;_tmp96B.tag=
Cyc_Exp_tok;_tmp96B.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96B;});
_tmp96A;});break;case 398: _LL398: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96C=
_cycalloc(sizeof(*_tmp96C));_tmp96C[0]=({struct Cyc_Exp_tok_struct _tmp96D;_tmp96D.tag=
Cyc_Exp_tok;_tmp96D.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96D;});
_tmp96C;});break;case 399: _LL399: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96E=
_cycalloc(sizeof(*_tmp96E));_tmp96E[0]=({struct Cyc_Exp_tok_struct _tmp96F;_tmp96F.tag=
Cyc_Exp_tok;_tmp96F.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96F;});
_tmp96E;});break;case 400: _LL39A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp970=
_cycalloc(sizeof(*_tmp970));_tmp970[0]=({struct Cyc_Exp_tok_struct _tmp971;_tmp971.tag=
Cyc_Exp_tok;_tmp971.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp971;});
_tmp970;});break;case 401: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 402: _LL39C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 403: _LL39D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp972=_cycalloc(sizeof(*_tmp972));_tmp972[0]=({struct Cyc_Exp_tok_struct _tmp973;
_tmp973.tag=Cyc_Exp_tok;_tmp973.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp973;});
_tmp972;});break;case 404: _LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL39F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp974=_cycalloc(sizeof(*_tmp974));_tmp974[0]=({struct Cyc_Exp_tok_struct _tmp975;
_tmp975.tag=Cyc_Exp_tok;_tmp975.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp975;});
_tmp974;});break;case 406: _LL3A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 407: _LL3A1: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp976=_cycalloc(sizeof(*_tmp976));_tmp976[0]=({struct Cyc_Exp_tok_struct _tmp977;
_tmp977.tag=Cyc_Exp_tok;_tmp977.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp977;});
_tmp976;});break;case 408: _LL3A2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 409: _LL3A3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp978=_cycalloc(sizeof(*_tmp978));_tmp978[0]=({struct Cyc_Exp_tok_struct _tmp979;
_tmp979.tag=Cyc_Exp_tok;_tmp979.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp979;});
_tmp978;});break;case 410: _LL3A4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 411: _LL3A5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97A=_cycalloc(sizeof(*_tmp97A));_tmp97A[0]=({struct Cyc_Exp_tok_struct _tmp97B;
_tmp97B.tag=Cyc_Exp_tok;_tmp97B.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97B;});
_tmp97A;});break;case 412: _LL3A6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3A7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97C=_cycalloc(sizeof(*_tmp97C));_tmp97C[0]=({struct Cyc_Exp_tok_struct _tmp97D;
_tmp97D.tag=Cyc_Exp_tok;_tmp97D.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97D;});
_tmp97C;});break;case 414: _LL3A8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp97E=
_cycalloc(sizeof(*_tmp97E));_tmp97E[0]=({struct Cyc_Exp_tok_struct _tmp97F;_tmp97F.tag=
Cyc_Exp_tok;_tmp97F.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97F;});
_tmp97E;});break;case 415: _LL3A9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3AA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp980=_cycalloc(sizeof(*_tmp980));_tmp980[0]=({struct Cyc_Exp_tok_struct _tmp981;
_tmp981.tag=Cyc_Exp_tok;_tmp981.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp981;});
_tmp980;});break;case 417: _LL3AB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp982=
_cycalloc(sizeof(*_tmp982));_tmp982[0]=({struct Cyc_Exp_tok_struct _tmp983;_tmp983.tag=
Cyc_Exp_tok;_tmp983.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp983;});
_tmp982;});break;case 418: _LL3AC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp984=
_cycalloc(sizeof(*_tmp984));_tmp984[0]=({struct Cyc_Exp_tok_struct _tmp985;_tmp985.tag=
Cyc_Exp_tok;_tmp985.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp985;});
_tmp984;});break;case 419: _LL3AD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp986=
_cycalloc(sizeof(*_tmp986));_tmp986[0]=({struct Cyc_Exp_tok_struct _tmp987;_tmp987.tag=
Cyc_Exp_tok;_tmp987.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp987;});
_tmp986;});break;case 420: _LL3AE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3AF: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp988=_cycalloc(sizeof(*_tmp988));_tmp988[0]=({struct Cyc_Exp_tok_struct _tmp989;
_tmp989.tag=Cyc_Exp_tok;_tmp989.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp989;});
_tmp988;});break;case 422: _LL3B0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98A=
_cycalloc(sizeof(*_tmp98A));_tmp98A[0]=({struct Cyc_Exp_tok_struct _tmp98B;_tmp98B.tag=
Cyc_Exp_tok;_tmp98B.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98B;});
_tmp98A;});break;case 423: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B2: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp98C=_cycalloc(sizeof(*_tmp98C));_tmp98C[0]=({struct Cyc_Exp_tok_struct _tmp98D;
_tmp98D.tag=Cyc_Exp_tok;_tmp98D.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98D;});
_tmp98C;});break;case 425: _LL3B3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98E=
_cycalloc(sizeof(*_tmp98E));_tmp98E[0]=({struct Cyc_Exp_tok_struct _tmp98F;_tmp98F.tag=
Cyc_Exp_tok;_tmp98F.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98F;});
_tmp98E;});break;case 426: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3B5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp990=_cycalloc(sizeof(*_tmp990));_tmp990[0]=({struct Cyc_Exp_tok_struct _tmp991;
_tmp991.tag=Cyc_Exp_tok;_tmp991.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp991;});
_tmp990;});break;case 428: _LL3B6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp992=
_cycalloc(sizeof(*_tmp992));_tmp992[0]=({struct Cyc_Exp_tok_struct _tmp993;_tmp993.tag=
Cyc_Exp_tok;_tmp993.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp993;});
_tmp992;});break;case 429: _LL3B7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp994=
_cycalloc(sizeof(*_tmp994));_tmp994[0]=({struct Cyc_Exp_tok_struct _tmp995;_tmp995.tag=
Cyc_Exp_tok;_tmp995.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp995;});
_tmp994;});break;case 430: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 431: _LL3B9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp996=_cycalloc(sizeof(*_tmp996));_tmp996[0]=({struct Cyc_Exp_tok_struct _tmp997;
_tmp997.tag=Cyc_Exp_tok;_tmp997.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp997;});
_tmp996;});break;case 432: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 433: _LL3BB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp998=_cycalloc(sizeof(*_tmp998));_tmp998[0]=({struct Cyc_Exp_tok_struct _tmp999;
_tmp999.tag=Cyc_Exp_tok;_tmp999.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp999;});
_tmp998;});break;case 434: _LL3BC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99A=
_cycalloc(sizeof(*_tmp99A));_tmp99A[0]=({struct Cyc_Exp_tok_struct _tmp99B;_tmp99B.tag=
Cyc_Exp_tok;_tmp99B.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99B;});
_tmp99A;});break;case 435: _LL3BD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99C=
_cycalloc(sizeof(*_tmp99C));_tmp99C[0]=({struct Cyc_Exp_tok_struct _tmp99D;_tmp99D.tag=
Cyc_Exp_tok;_tmp99D.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99D;});
_tmp99C;});break;case 436: _LL3BE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99E=
_cycalloc(sizeof(*_tmp99E));_tmp99E[0]=({struct Cyc_Exp_tok_struct _tmp99F;_tmp99F.tag=
Cyc_Exp_tok;_tmp99F.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99F;});
_tmp99E;});break;case 437: _LL3BF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3C0: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9A0=_cycalloc(sizeof(*_tmp9A0));_tmp9A0[0]=({struct Cyc_Exp_tok_struct _tmp9A1;
_tmp9A1.tag=Cyc_Exp_tok;_tmp9A1.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A1;});
_tmp9A0;});break;case 439: _LL3C1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A2=
_cycalloc(sizeof(*_tmp9A2));_tmp9A2[0]=({struct Cyc_Exp_tok_struct _tmp9A3;_tmp9A3.tag=
Cyc_Exp_tok;_tmp9A3.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A3;});
_tmp9A2;});break;case 440: _LL3C2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A4=
_cycalloc(sizeof(*_tmp9A4));_tmp9A4[0]=({struct Cyc_Exp_tok_struct _tmp9A5;_tmp9A5.tag=
Cyc_Exp_tok;_tmp9A5.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A5;});
_tmp9A4;});break;case 441: _LL3C3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A6=
_cycalloc(sizeof(*_tmp9A6));_tmp9A6[0]=({struct Cyc_Exp_tok_struct _tmp9A7;_tmp9A7.tag=
Cyc_Exp_tok;_tmp9A7.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp9A8=_cycalloc(sizeof(*_tmp9A8));_tmp9A8[0]=({struct Cyc_Absyn_StructField_struct
_tmp9A9;_tmp9A9.tag=0;_tmp9A9.f1=({struct _tagged_arr*_tmp9AA=_cycalloc(sizeof(*
_tmp9AA));_tmp9AA[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp9AA;});_tmp9A9;});_tmp9A8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A7;});
_tmp9A6;});break;case 442: _LL3C4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AB=
_cycalloc(sizeof(*_tmp9AB));_tmp9AB[0]=({struct Cyc_Exp_tok_struct _tmp9AC;_tmp9AC.tag=
Cyc_Exp_tok;_tmp9AC.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp9AD=_cycalloc_atomic(sizeof(*_tmp9AD));_tmp9AD[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp9AE;_tmp9AE.tag=1;_tmp9AE.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp9AE;});_tmp9AD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9AC;});_tmp9AB;});break;case 443: _LL3C5: {struct
Cyc_Position_Segment*_tmp9AF=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp9B0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp9AF,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B1=_cycalloc(
sizeof(*_tmp9B1));_tmp9B1[0]=({struct Cyc_Exp_tok_struct _tmp9B2;_tmp9B2.tag=Cyc_Exp_tok;
_tmp9B2.f1=Cyc_Absyn_gentyp_exp(_tmp9B0,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B2;});
_tmp9B1;});break;}case 444: _LL3C6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B3=
_cycalloc(sizeof(*_tmp9B3));_tmp9B3[0]=({struct Cyc_Exp_tok_struct _tmp9B4;_tmp9B4.tag=
Cyc_Exp_tok;_tmp9B4.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9B5=_cycalloc(sizeof(*_tmp9B5));_tmp9B5[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9B6;_tmp9B6.tag=35;_tmp9B6.f1=({struct Cyc_Absyn_MallocInfo _tmp9B7;_tmp9B7.is_calloc=
0;_tmp9B7.rgn=0;_tmp9B7.elt_type=0;_tmp9B7.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9B7.fat_result=0;
_tmp9B7;});_tmp9B6;});_tmp9B5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B4;});
_tmp9B3;});break;case 445: _LL3C7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B8=
_cycalloc(sizeof(*_tmp9B8));_tmp9B8[0]=({struct Cyc_Exp_tok_struct _tmp9B9;_tmp9B9.tag=
Cyc_Exp_tok;_tmp9B9.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9BA=_cycalloc(sizeof(*_tmp9BA));_tmp9BA[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9BB;_tmp9BB.tag=35;_tmp9BB.f1=({struct Cyc_Absyn_MallocInfo _tmp9BC;_tmp9BC.is_calloc=
0;_tmp9BC.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp9BC.elt_type=0;
_tmp9BC.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp9BC.fat_result=0;_tmp9BC;});_tmp9BB;});_tmp9BA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B9;});
_tmp9B8;});break;case 446: _LL3C8: {void*_tmp9BE;struct _tuple2 _tmp9BD=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9BE=_tmp9BD.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BF=_cycalloc(sizeof(*_tmp9BF));
_tmp9BF[0]=({struct Cyc_Exp_tok_struct _tmp9C0;_tmp9C0.tag=Cyc_Exp_tok;_tmp9C0.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9C1=_cycalloc(
sizeof(*_tmp9C1));_tmp9C1[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9C2;_tmp9C2.tag=
35;_tmp9C2.f1=({struct Cyc_Absyn_MallocInfo _tmp9C3;_tmp9C3.is_calloc=1;_tmp9C3.rgn=
0;_tmp9C3.elt_type=({void**_tmp9C4=_cycalloc(sizeof(*_tmp9C4));_tmp9C4[0]=
_tmp9BE;_tmp9C4;});_tmp9C3.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9C3.fat_result=0;
_tmp9C3;});_tmp9C2;});_tmp9C1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C0;});
_tmp9BF;});break;}case 447: _LL3C9: {void*_tmp9C6;struct _tuple2 _tmp9C5=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9C6=_tmp9C5.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C7=_cycalloc(sizeof(*_tmp9C7));
_tmp9C7[0]=({struct Cyc_Exp_tok_struct _tmp9C8;_tmp9C8.tag=Cyc_Exp_tok;_tmp9C8.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9C9=_cycalloc(
sizeof(*_tmp9C9));_tmp9C9[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9CA;_tmp9CA.tag=
35;_tmp9CA.f1=({struct Cyc_Absyn_MallocInfo _tmp9CB;_tmp9CB.is_calloc=1;_tmp9CB.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp9CB.elt_type=({void**_tmp9CC=_cycalloc(sizeof(*_tmp9CC));
_tmp9CC[0]=_tmp9C6;_tmp9CC;});_tmp9CB.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9CB.fat_result=0;
_tmp9CB;});_tmp9CA;});_tmp9C9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C8;});
_tmp9C7;});break;}case 448: _LL3CA: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9CD=_cycalloc(sizeof(*_tmp9CD));_tmp9CD[0]=({struct Cyc_Primop_tok_struct
_tmp9CE;_tmp9CE.tag=Cyc_Primop_tok;_tmp9CE.f1=(void*)((void*)12);_tmp9CE;});
_tmp9CD;});break;case 449: _LL3CB: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9CF=_cycalloc(sizeof(*_tmp9CF));_tmp9CF[0]=({struct Cyc_Primop_tok_struct
_tmp9D0;_tmp9D0.tag=Cyc_Primop_tok;_tmp9D0.f1=(void*)((void*)11);_tmp9D0;});
_tmp9CF;});break;case 450: _LL3CC: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9D1=_cycalloc(sizeof(*_tmp9D1));_tmp9D1[0]=({struct Cyc_Primop_tok_struct
_tmp9D2;_tmp9D2.tag=Cyc_Primop_tok;_tmp9D2.f1=(void*)((void*)2);_tmp9D2;});
_tmp9D1;});break;case 451: _LL3CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 452: _LL3CE: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9D3=_cycalloc(sizeof(*_tmp9D3));_tmp9D3[0]=({struct Cyc_Exp_tok_struct _tmp9D4;
_tmp9D4.tag=Cyc_Exp_tok;_tmp9D4.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D4;});
_tmp9D3;});break;case 453: _LL3CF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D5=
_cycalloc(sizeof(*_tmp9D5));_tmp9D5[0]=({struct Cyc_Exp_tok_struct _tmp9D6;_tmp9D6.tag=
Cyc_Exp_tok;_tmp9D6.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D6;});
_tmp9D5;});break;case 454: _LL3D0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D7=
_cycalloc(sizeof(*_tmp9D7));_tmp9D7[0]=({struct Cyc_Exp_tok_struct _tmp9D8;_tmp9D8.tag=
Cyc_Exp_tok;_tmp9D8.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D8;});
_tmp9D7;});break;case 455: _LL3D1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D9=
_cycalloc(sizeof(*_tmp9D9));_tmp9D9[0]=({struct Cyc_Exp_tok_struct _tmp9DA;_tmp9DA.tag=
Cyc_Exp_tok;_tmp9DA.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9DB=_cycalloc(sizeof(*_tmp9DB));_tmp9DB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9DB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DA;});
_tmp9D9;});break;case 456: _LL3D2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DC=
_cycalloc(sizeof(*_tmp9DC));_tmp9DC[0]=({struct Cyc_Exp_tok_struct _tmp9DD;_tmp9DD.tag=
Cyc_Exp_tok;_tmp9DD.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9DE=_cycalloc(sizeof(*_tmp9DE));_tmp9DE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9DE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DD;});
_tmp9DC;});break;case 457: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DF=
_cycalloc(sizeof(*_tmp9DF));_tmp9DF[0]=({struct Cyc_Exp_tok_struct _tmp9E0;_tmp9E0.tag=
Cyc_Exp_tok;_tmp9E0.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E0;});
_tmp9DF;});break;case 458: _LL3D4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E1=
_cycalloc(sizeof(*_tmp9E1));_tmp9E1[0]=({struct Cyc_Exp_tok_struct _tmp9E2;_tmp9E2.tag=
Cyc_Exp_tok;_tmp9E2.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E2;});
_tmp9E1;});break;case 459: _LL3D5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E3=
_cycalloc(sizeof(*_tmp9E3));_tmp9E3[0]=({struct Cyc_Exp_tok_struct _tmp9E4;_tmp9E4.tag=
Cyc_Exp_tok;_tmp9E4.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp9E5=_cycalloc(sizeof(*_tmp9E5));_tmp9E5[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp9E6;_tmp9E6.tag=27;_tmp9E6.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp9E6.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp9E6;});_tmp9E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9E4;});_tmp9E3;});break;case 460: _LL3D6: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp9E7=_cycalloc(sizeof(*_tmp9E7));_tmp9E7[0]=({
struct Cyc_Exp_tok_struct _tmp9E8;_tmp9E8.tag=Cyc_Exp_tok;_tmp9E8.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp9E9=_cycalloc(sizeof(*_tmp9E9));
_tmp9E9[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp9EA;_tmp9EA.tag=27;_tmp9EA.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp9EA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp9EA;});_tmp9E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E8;});
_tmp9E7;});break;case 461: _LL3D7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9EB=
_cycalloc(sizeof(*_tmp9EB));_tmp9EB[0]=({struct Cyc_Exp_tok_struct _tmp9EC;_tmp9EC.tag=
Cyc_Exp_tok;_tmp9EC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp9ED=_cycalloc(sizeof(*_tmp9ED));_tmp9ED[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp9EE;_tmp9EE.tag=2;_tmp9EE.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9EE;});_tmp9ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9EC;});
_tmp9EB;});break;case 462: _LL3D8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 463: _LL3D9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9EF=_cycalloc(sizeof(*_tmp9EF));_tmp9EF[0]=({struct Cyc_Exp_tok_struct _tmp9F0;
_tmp9F0.tag=Cyc_Exp_tok;_tmp9F0.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F0;});
_tmp9EF;});break;case 464: _LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 465: _LL3DB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9F1=_cycalloc(sizeof(*_tmp9F1));_tmp9F1[0]=({struct Cyc_Exp_tok_struct _tmp9F2;
_tmp9F2.tag=Cyc_Exp_tok;_tmp9F2.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F2;});
_tmp9F1;});break;case 466: _LL3DC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F3=
_cycalloc(sizeof(*_tmp9F3));_tmp9F3[0]=({struct Cyc_Exp_tok_struct _tmp9F4;_tmp9F4.tag=
Cyc_Exp_tok;_tmp9F4.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F4;});
_tmp9F3;});break;case 467: _LL3DD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F5=
_cycalloc(sizeof(*_tmp9F5));_tmp9F5[0]=({struct Cyc_Exp_tok_struct _tmp9F6;_tmp9F6.tag=
Cyc_Exp_tok;_tmp9F6.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F6;});
_tmp9F5;});break;case 468: _LL3DE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F7=
_cycalloc(sizeof(*_tmp9F7));_tmp9F7[0]=({struct Cyc_Exp_tok_struct _tmp9F8;_tmp9F8.tag=
Cyc_Exp_tok;_tmp9F8.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp9F9=_cycalloc(sizeof(*_tmp9F9));_tmp9F9[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp9FA;_tmp9FA.tag=30;_tmp9FA.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp9FA.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9FA.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp9FA.f4=0;
_tmp9FA;});_tmp9F9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F8;});
_tmp9F7;});break;case 469: _LL3DF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FB=
_cycalloc(sizeof(*_tmp9FB));_tmp9FB[0]=({struct Cyc_Exp_tok_struct _tmp9FC;_tmp9FC.tag=
Cyc_Exp_tok;_tmp9FC.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FC;});
_tmp9FB;});break;case 470: _LL3E0: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp9FD=_cycalloc(sizeof(*_tmp9FD));_tmp9FD[0]=({struct Cyc_ExpList_tok_struct
_tmp9FE;_tmp9FE.tag=Cyc_ExpList_tok;_tmp9FE.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp9FE;});_tmp9FD;});
break;case 471: _LL3E1: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp9FF=
_cycalloc(sizeof(*_tmp9FF));_tmp9FF[0]=({struct Cyc_ExpList_tok_struct _tmpA00;
_tmpA00.tag=Cyc_ExpList_tok;_tmpA00.f1=({struct Cyc_List_List*_tmpA01=_cycalloc(
sizeof(*_tmpA01));_tmpA01->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA01->tl=0;_tmpA01;});
_tmpA00;});_tmp9FF;});break;case 472: _LL3E2: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmpA02=_cycalloc(sizeof(*_tmpA02));_tmpA02[0]=({struct Cyc_ExpList_tok_struct
_tmpA03;_tmpA03.tag=Cyc_ExpList_tok;_tmpA03.f1=({struct Cyc_List_List*_tmpA04=
_cycalloc(sizeof(*_tmpA04));_tmpA04->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA04->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA04;});_tmpA03;});
_tmpA02;});break;case 473: _LL3E3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA05=
_cycalloc(sizeof(*_tmpA05));_tmpA05[0]=({struct Cyc_Exp_tok_struct _tmpA06;_tmpA06.tag=
Cyc_Exp_tok;_tmpA06.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA06;});
_tmpA05;});break;case 474: _LL3E4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA07=
_cycalloc(sizeof(*_tmpA07));_tmpA07[0]=({struct Cyc_Exp_tok_struct _tmpA08;_tmpA08.tag=
Cyc_Exp_tok;_tmpA08.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA08;});
_tmpA07;});break;case 475: _LL3E5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA09=
_cycalloc(sizeof(*_tmpA09));_tmpA09[0]=({struct Cyc_Exp_tok_struct _tmpA0A;_tmpA0A.tag=
Cyc_Exp_tok;_tmpA0A.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0A;});
_tmpA09;});break;case 476: _LL3E6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA0B=
_cycalloc(sizeof(*_tmpA0B));_tmpA0B[0]=({struct Cyc_Exp_tok_struct _tmpA0C;_tmpA0C.tag=
Cyc_Exp_tok;_tmpA0C.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0C;});
_tmpA0B;});break;case 477: _LL3E7: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmpA0D=_cycalloc(sizeof(*_tmpA0D));_tmpA0D[0]=({struct Cyc_QualId_tok_struct
_tmpA0E;_tmpA0E.tag=Cyc_QualId_tok;_tmpA0E.f1=({struct _tuple1*_tmpA0F=_cycalloc(
sizeof(*_tmpA0F));_tmpA0F->f1=Cyc_Absyn_rel_ns_null;_tmpA0F->f2=({struct
_tagged_arr*_tmpA10=_cycalloc(sizeof(*_tmpA10));_tmpA10[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA10;});_tmpA0F;});
_tmpA0E;});_tmpA0D;});break;case 478: _LL3E8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 479: _LL3E9: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmpA11=_cycalloc(sizeof(*_tmpA11));_tmpA11[
0]=({struct Cyc_QualId_tok_struct _tmpA12;_tmpA12.tag=Cyc_QualId_tok;_tmpA12.f1=({
struct _tuple1*_tmpA13=_cycalloc(sizeof(*_tmpA13));_tmpA13->f1=Cyc_Absyn_rel_ns_null;
_tmpA13->f2=({struct _tagged_arr*_tmpA14=_cycalloc(sizeof(*_tmpA14));_tmpA14[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmpA14;});_tmpA13;});_tmpA12;});_tmpA11;});break;case 480: _LL3EA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 481: _LL3EB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 482: _LL3EC:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 483:
_LL3ED: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
484: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 485: _LL3EF: break;case 486: _LL3F0:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL3F1: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(487,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(139,yyn - 129)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5863) && Cyc_yycheck[_check_known_subscript_notnull(5864,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5864,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(139,yyn - 129)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(970,yystate)];if(yyn > - 32768  && yyn < 5863){int sze=
0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 268 / sizeof(
char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5864,x + yyn)]== x)(sze
+=Cyc_strlen((struct _tagged_arr)Cyc_yytname[_check_known_subscript_notnull(268,x)])
+ 15,count ++);}msg=({unsigned int _tmpA15=(unsigned int)(sze + 15);char*_tmpA16=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpA15 + 1));struct _tagged_arr
_tmpA18=_tag_arr(_tmpA16,sizeof(char),_tmpA15 + 1);{unsigned int _tmpA17=_tmpA15;
unsigned int i;for(i=0;i < _tmpA17;i ++){_tmpA16[i]='\000';}_tmpA16[_tmpA17]=(char)
0;}_tmpA18;});Cyc_strcpy(msg,({const char*_tmpA19="parse error";_tag_arr(_tmpA19,
sizeof(char),_get_zero_arr_size(_tmpA19,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 268 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5864,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmpA1A=", expecting `";
_tag_arr(_tmpA1A,sizeof(char),_get_zero_arr_size(_tmpA1A,14));}):(struct
_tagged_arr)({const char*_tmpA1B=" or `";_tag_arr(_tmpA1B,sizeof(char),
_get_zero_arr_size(_tmpA1B,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(268,x)]);Cyc_strcat(msg,({const char*_tmpA1C="'";
_tag_arr(_tmpA1C,sizeof(char),_get_zero_arr_size(_tmpA1C,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmpA1D="parse error";
_tag_arr(_tmpA1D,sizeof(char),_get_zero_arr_size(_tmpA1D,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(970,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 5863) || Cyc_yycheck[_check_known_subscript_notnull(
5864,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5864,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 969)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmpA27=v;struct _tuple13*_tmpA28;
struct _tuple13 _tmpA29;int _tmpA2A;char _tmpA2B;short _tmpA2C;struct _tagged_arr
_tmpA2D;struct Cyc_Core_Opt*_tmpA2E;struct Cyc_Core_Opt*_tmpA2F;struct Cyc_Core_Opt
_tmpA30;struct _tagged_arr*_tmpA31;struct _tuple1*_tmpA32;struct _tuple1 _tmpA33;
void*_tmpA34;struct _tagged_arr*_tmpA35;_LL3F4: if(*((void**)_tmpA27)!= Cyc_Int_tok)
goto _LL3F6;_tmpA28=((struct Cyc_Int_tok_struct*)_tmpA27)->f1;_tmpA29=*_tmpA28;
_tmpA2A=_tmpA29.f2;_LL3F5:({struct Cyc_Int_pa_struct _tmpA38;_tmpA38.tag=1;_tmpA38.f1=(
unsigned int)_tmpA2A;{void*_tmpA36[1]={& _tmpA38};Cyc_fprintf(Cyc_stderr,({const
char*_tmpA37="%d";_tag_arr(_tmpA37,sizeof(char),_get_zero_arr_size(_tmpA37,3));}),
_tag_arr(_tmpA36,sizeof(void*),1));}});goto _LL3F3;_LL3F6: if(*((void**)_tmpA27)!= 
Cyc_Char_tok)goto _LL3F8;_tmpA2B=((struct Cyc_Char_tok_struct*)_tmpA27)->f1;_LL3F7:({
struct Cyc_Int_pa_struct _tmpA3B;_tmpA3B.tag=1;_tmpA3B.f1=(unsigned int)((int)
_tmpA2B);{void*_tmpA39[1]={& _tmpA3B};Cyc_fprintf(Cyc_stderr,({const char*_tmpA3A="%c";
_tag_arr(_tmpA3A,sizeof(char),_get_zero_arr_size(_tmpA3A,3));}),_tag_arr(_tmpA39,
sizeof(void*),1));}});goto _LL3F3;_LL3F8: if(*((void**)_tmpA27)!= Cyc_Short_tok)
goto _LL3FA;_tmpA2C=((struct Cyc_Short_tok_struct*)_tmpA27)->f1;_LL3F9:({struct Cyc_Int_pa_struct
_tmpA3E;_tmpA3E.tag=1;_tmpA3E.f1=(unsigned int)((int)_tmpA2C);{void*_tmpA3C[1]={&
_tmpA3E};Cyc_fprintf(Cyc_stderr,({const char*_tmpA3D="%ds";_tag_arr(_tmpA3D,
sizeof(char),_get_zero_arr_size(_tmpA3D,4));}),_tag_arr(_tmpA3C,sizeof(void*),1));}});
goto _LL3F3;_LL3FA: if(*((void**)_tmpA27)!= Cyc_String_tok)goto _LL3FC;_tmpA2D=((
struct Cyc_String_tok_struct*)_tmpA27)->f1;_LL3FB:({struct Cyc_String_pa_struct
_tmpA41;_tmpA41.tag=0;_tmpA41.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpA2D);{
void*_tmpA3F[1]={& _tmpA41};Cyc_fprintf(Cyc_stderr,({const char*_tmpA40="\"%s\"";
_tag_arr(_tmpA40,sizeof(char),_get_zero_arr_size(_tmpA40,5));}),_tag_arr(_tmpA3F,
sizeof(void*),1));}});goto _LL3F3;_LL3FC: if(*((void**)_tmpA27)!= Cyc_Stringopt_tok)
goto _LL3FE;_tmpA2E=((struct Cyc_Stringopt_tok_struct*)_tmpA27)->f1;if(_tmpA2E != 0)
goto _LL3FE;_LL3FD:({void*_tmpA42[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpA43="NULL";_tag_arr(_tmpA43,sizeof(char),_get_zero_arr_size(_tmpA43,5));}),
_tag_arr(_tmpA42,sizeof(void*),0));});goto _LL3F3;_LL3FE: if(*((void**)_tmpA27)!= 
Cyc_Stringopt_tok)goto _LL400;_tmpA2F=((struct Cyc_Stringopt_tok_struct*)_tmpA27)->f1;
if(_tmpA2F == 0)goto _LL400;_tmpA30=*_tmpA2F;_tmpA31=(struct _tagged_arr*)_tmpA30.v;
_LL3FF:({struct Cyc_String_pa_struct _tmpA46;_tmpA46.tag=0;_tmpA46.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmpA31);{void*_tmpA44[1]={& _tmpA46};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA45="\"%s\"";_tag_arr(_tmpA45,sizeof(char),
_get_zero_arr_size(_tmpA45,5));}),_tag_arr(_tmpA44,sizeof(void*),1));}});goto
_LL3F3;_LL400: if(*((void**)_tmpA27)!= Cyc_QualId_tok)goto _LL402;_tmpA32=((struct
Cyc_QualId_tok_struct*)_tmpA27)->f1;_tmpA33=*_tmpA32;_tmpA34=_tmpA33.f1;_tmpA35=
_tmpA33.f2;_LL401: {struct Cyc_List_List*_tmpA47=0;{void*_tmpA48=_tmpA34;struct
Cyc_List_List*_tmpA49;struct Cyc_List_List*_tmpA4A;_LL405: if(_tmpA48 <= (void*)1
 || *((int*)_tmpA48)!= 0)goto _LL407;_tmpA49=((struct Cyc_Absyn_Rel_n_struct*)
_tmpA48)->f1;_LL406: _tmpA47=_tmpA49;goto _LL404;_LL407: if(_tmpA48 <= (void*)1  || *((
int*)_tmpA48)!= 1)goto _LL409;_tmpA4A=((struct Cyc_Absyn_Abs_n_struct*)_tmpA48)->f1;
_LL408: _tmpA47=_tmpA4A;goto _LL404;_LL409: if((int)_tmpA48 != 0)goto _LL404;_LL40A:
goto _LL404;_LL404:;}for(0;_tmpA47 != 0;_tmpA47=_tmpA47->tl){({struct Cyc_String_pa_struct
_tmpA4D;_tmpA4D.tag=0;_tmpA4D.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmpA47->hd));{void*_tmpA4B[1]={& _tmpA4D};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA4C="%s::";_tag_arr(_tmpA4C,sizeof(char),_get_zero_arr_size(
_tmpA4C,5));}),_tag_arr(_tmpA4B,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmpA50;_tmpA50.tag=0;_tmpA50.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmpA35);{
void*_tmpA4E[1]={& _tmpA50};Cyc_fprintf(Cyc_stderr,({const char*_tmpA4F="%s::";
_tag_arr(_tmpA4F,sizeof(char),_get_zero_arr_size(_tmpA4F,5));}),_tag_arr(_tmpA4E,
sizeof(void*),1));}});goto _LL3F3;}_LL402:;_LL403:({void*_tmpA51[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA52="?";_tag_arr(_tmpA52,sizeof(char),
_get_zero_arr_size(_tmpA52,2));}),_tag_arr(_tmpA51,sizeof(void*),0));});goto
_LL3F3;_LL3F3:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmpA53=_cycalloc(
sizeof(*_tmpA53));_tmpA53->v=Cyc_Lexing_from_file(f);_tmpA53;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
