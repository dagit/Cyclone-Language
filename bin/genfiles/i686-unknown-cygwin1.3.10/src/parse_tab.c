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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
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
_tmp20;_LL1: if(_tmp1A <= (void*)4  || *((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct
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
return(void*)2;else{if(Cyc_zstrcmp((struct _tagged_arr)s,({const char*_tmp2B="`U";
_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,3));}))== 0)return(void*)
3;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp2C=_cycalloc(sizeof(*
_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_VarType_struct _tmp2D;_tmp2D.tag=1;_tmp2D.f1=({
struct Cyc_Absyn_Tvar*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=({struct
_tagged_arr*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=s;_tmp2F;});_tmp2E->identity=
0;_tmp2E->kind=(void*)k;_tmp2E;});_tmp2D;});_tmp2C;});}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp30=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp31;void*_tmp32;_LL10: if(*((int*)_tmp30)!= 0)goto _LL12;
_tmp31=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp30)->f1;_LL11: k=(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(void*)_tmp31;_tmp34;});
_tmp33;});goto _LLF;_LL12: if(*((int*)_tmp30)!= 1)goto _LL14;_LL13: k=(void*)({
struct Cyc_Absyn_Unknown_kb_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({
struct Cyc_Absyn_Unknown_kb_struct _tmp36;_tmp36.tag=1;_tmp36.f1=0;_tmp36;});
_tmp35;});goto _LLF;_LL14: if(*((int*)_tmp30)!= 2)goto _LLF;_tmp32=(void*)((struct
Cyc_Absyn_Less_kb_struct*)_tmp30)->f2;_LL15: k=(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp38;_tmp38.tag=2;_tmp38.f1=0;_tmp38.f2=(void*)_tmp32;_tmp38;});_tmp37;});goto
_LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));
_tmp39->name=t->name;_tmp39->identity=0;_tmp39->kind=(void*)k;_tmp39;});}static
struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){
void*_tmp3A=t;struct Cyc_Absyn_Tvar*_tmp3B;_LL17: if(_tmp3A <= (void*)4  || *((int*)
_tmp3A)!= 1)goto _LL19;_tmp3B=((struct Cyc_Absyn_VarType_struct*)_tmp3A)->f1;_LL18:
return _tmp3B;_LL19:;_LL1A:({void*_tmp3C[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp3D="expecting a list of type variables, not types";_tag_arr(_tmp3D,sizeof(
char),_get_zero_arr_size(_tmp3D,46));}),_tag_arr(_tmp3C,sizeof(void*),0));});
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({
struct Cyc_Absyn_VarType_struct _tmp3F;_tmp3F.tag=1;_tmp3F.f1=pr;_tmp3F;});_tmp3E;});}
static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*_tmp40=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp41;struct Cyc_Absyn_Tvar _tmp42;void*_tmp43;void**
_tmp44;_LL1C: if(_tmp40 <= (void*)4  || *((int*)_tmp40)!= 1)goto _LL1E;_tmp41=((
struct Cyc_Absyn_VarType_struct*)_tmp40)->f1;_tmp42=*_tmp41;_tmp43=(void*)_tmp42.kind;
_tmp44=(void**)&(*((struct Cyc_Absyn_VarType_struct*)_tmp40)->f1).kind;_LL1D: {
void*_tmp45=Cyc_Absyn_compress_kb(*_tmp44);_LL21: if(*((int*)_tmp45)!= 1)goto
_LL23;_LL22:*_tmp44=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp46=_cycalloc(
sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp47;_tmp47.tag=0;
_tmp47.f1=(void*)k;_tmp47;});_tmp46;});return;_LL23:;_LL24: return;_LL20:;}_LL1E:;
_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp48=(void*)tms->hd;void*_tmp49;
_LL26: if(*((int*)_tmp48)!= 3)goto _LL28;_tmp49=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp48)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4A=_tmp49;struct Cyc_List_List*_tmp4B;_LL2B: if(*((int*)_tmp4A)!= 1)
goto _LL2D;_LL2C:({void*_tmp4C[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4D="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,93));}),_tag_arr(_tmp4C,
sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp4A)!= 0)goto _LL2A;_tmp4B=((
struct Cyc_Absyn_NoTypes_struct*)_tmp4A)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tds))({void*_tmp4E[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp4F="wrong number of parameter declarations in old-style function declaration";
_tag_arr(_tmp4F,sizeof(char),_get_zero_arr_size(_tmp4F,73));}),_tag_arr(_tmp4E,
sizeof(void*),0));});{struct Cyc_List_List*_tmp50=0;for(0;_tmp4B != 0;_tmp4B=
_tmp4B->tl){struct Cyc_List_List*_tmp51=tds;for(0;_tmp51 != 0;_tmp51=_tmp51->tl){
struct Cyc_Absyn_Decl*_tmp52=(struct Cyc_Absyn_Decl*)_tmp51->hd;void*_tmp53=(void*)
_tmp52->r;struct Cyc_Absyn_Vardecl*_tmp54;_LL30: if(_tmp53 <= (void*)2  || *((int*)
_tmp53)!= 0)goto _LL32;_tmp54=((struct Cyc_Absyn_Var_d_struct*)_tmp53)->f1;_LL31:
if(Cyc_zstrptrcmp((*_tmp54->name).f2,(struct _tagged_arr*)_tmp4B->hd)!= 0)
continue;if(_tmp54->initializer != 0)({void*_tmp55[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp52->loc,({
const char*_tmp56="initializer found in parameter declaration";_tag_arr(_tmp56,
sizeof(char),_get_zero_arr_size(_tmp56,43));}),_tag_arr(_tmp55,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp54->name))({void*_tmp57[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmp52->loc,({const char*_tmp58="namespaces forbidden in parameter declarations";
_tag_arr(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,47));}),_tag_arr(_tmp57,
sizeof(void*),0));});_tmp50=({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*
_tmp59));_tmp59->hd=({struct _tuple2*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=({
struct Cyc_Core_Opt*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->v=(*_tmp54->name).f2;
_tmp5B;});_tmp5A->f2=_tmp54->tq;_tmp5A->f3=(void*)_tmp54->type;_tmp5A;});_tmp59->tl=
_tmp50;_tmp59;});goto L;_LL32:;_LL33:({void*_tmp5C[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp52->loc,({
const char*_tmp5D="nonvariable declaration in parameter type";_tag_arr(_tmp5D,
sizeof(char),_get_zero_arr_size(_tmp5D,42));}),_tag_arr(_tmp5C,sizeof(void*),0));});
_LL2F:;}L: if(_tmp51 == 0)({struct Cyc_String_pa_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp4B->hd));{void*
_tmp5E[1]={& _tmp60};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp5F="%s is not given a type";
_tag_arr(_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,23));}),_tag_arr(_tmp5E,
sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*
_tmp61));_tmp61->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp62=
_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Function_mod_struct _tmp63;
_tmp63.tag=3;_tmp63.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp64=
_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_WithTypes_struct _tmp65;
_tmp65.tag=1;_tmp65.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp50);_tmp65.f2=0;_tmp65.f3=0;_tmp65.f4=0;_tmp65.f5=0;_tmp65;});_tmp64;}));
_tmp63;});_tmp62;}));_tmp61->tl=0;_tmp61;});}_LL2A:;}goto _LL29;_LL28:;_LL29:
return({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(void*)((
void*)tms->hd);_tmp66->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp66;});
_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp67=
_cycalloc(sizeof(*_tmp67));_tmp67->id=d->id;_tmp67->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp67;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp68=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp68 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp68 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp69="bad storage class on function";
_tag_arr(_tmp69,sizeof(char),_get_zero_arr_size(_tmp69,30));}),loc);goto _LL34;
_LL34:;}}{void*_tmp6B;struct Cyc_Core_Opt*_tmp6C;struct _tuple5 _tmp6A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;{struct Cyc_Absyn_Tqual _tmp6E;void*
_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp71;struct _tuple6 _tmp6D=
Cyc_Parse_apply_tms(tq,_tmp6B,atts,d->tms);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_tmp70=_tmp6D.f3;_tmp71=_tmp6D.f4;if(_tmp6C != 0)({void*_tmp72[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp73="nested type declaration within function prototype";
_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,50));}),_tag_arr(_tmp72,
sizeof(void*),0));});if(_tmp70 != 0)({void*_tmp74[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp75="bad type params, ignoring";_tag_arr(_tmp75,sizeof(char),
_get_zero_arr_size(_tmp75,26));}),_tag_arr(_tmp74,sizeof(void*),0));});{void*
_tmp76=_tmp6F;struct Cyc_Absyn_FnInfo _tmp77;struct Cyc_List_List*_tmp78;struct Cyc_Core_Opt*
_tmp79;void*_tmp7A;struct Cyc_List_List*_tmp7B;int _tmp7C;struct Cyc_Absyn_VarargInfo*
_tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_List_List*_tmp7F;_LL3C: if(_tmp76 <= (
void*)4  || *((int*)_tmp76)!= 8)goto _LL3E;_tmp77=((struct Cyc_Absyn_FnType_struct*)
_tmp76)->f1;_tmp78=_tmp77.tvars;_tmp79=_tmp77.effect;_tmp7A=(void*)_tmp77.ret_typ;
_tmp7B=_tmp77.args;_tmp7C=_tmp77.c_varargs;_tmp7D=_tmp77.cyc_varargs;_tmp7E=
_tmp77.rgn_po;_tmp7F=_tmp77.attributes;_LL3D: {struct Cyc_List_List*_tmp80=0;{
struct Cyc_List_List*_tmp81=_tmp7B;for(0;_tmp81 != 0;_tmp81=_tmp81->tl){struct
_tuple2 _tmp83;struct Cyc_Core_Opt*_tmp84;struct Cyc_Absyn_Tqual _tmp85;void*_tmp86;
struct _tuple2*_tmp82=(struct _tuple2*)_tmp81->hd;_tmp83=*_tmp82;_tmp84=_tmp83.f1;
_tmp85=_tmp83.f2;_tmp86=_tmp83.f3;if(_tmp84 == 0){Cyc_Parse_err(({const char*
_tmp87="missing argument variable in function prototype";_tag_arr(_tmp87,sizeof(
char),_get_zero_arr_size(_tmp87,48));}),loc);_tmp80=({struct Cyc_List_List*_tmp88=
_cycalloc(sizeof(*_tmp88));_tmp88->hd=({struct _tuple10*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",
sizeof(char),2);_tmp89->f2=_tmp85;_tmp89->f3=_tmp86;_tmp89;});_tmp88->tl=_tmp80;
_tmp88;});}else{_tmp80=({struct Cyc_List_List*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->hd=({struct _tuple10*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->f1=(struct
_tagged_arr*)_tmp84->v;_tmp8C->f2=_tmp85;_tmp8C->f3=_tmp86;_tmp8C;});_tmp8B->tl=
_tmp80;_tmp8B;});}}}return({struct Cyc_Absyn_Fndecl*_tmp8D=_cycalloc(sizeof(*
_tmp8D));_tmp8D->sc=(void*)sc;_tmp8D->name=d->id;_tmp8D->tvs=_tmp78;_tmp8D->is_inline=
is_inline;_tmp8D->effect=_tmp79;_tmp8D->ret_type=(void*)_tmp7A;_tmp8D->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp80);_tmp8D->c_varargs=
_tmp7C;_tmp8D->cyc_varargs=_tmp7D;_tmp8D->rgn_po=_tmp7E;_tmp8D->body=body;_tmp8D->cached_typ=
0;_tmp8D->param_vardecls=0;_tmp8D->attributes=Cyc_List_append(_tmp7F,_tmp71);
_tmp8D;});}_LL3E:;_LL3F:({void*_tmp8E[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*
_tmp8F="declarator is not a function prototype";_tag_arr(_tmp8F,sizeof(char),
_get_zero_arr_size(_tmp8F,39));}),_tag_arr(_tmp8E,sizeof(void*),0));});_LL3B:;}}}}}
static char _tmp90[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp90,_tmp90,_tmp90 + 52};static char
_tmp91[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp91,_tmp91,_tmp91 + 63};static char
_tmp92[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp92,_tmp92,_tmp92 + 50};static char _tmp93[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp93,_tmp93,_tmp93 + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp94=(void*)ts->hd;void*_tmp95;
struct Cyc_Position_Segment*_tmp96;struct Cyc_Position_Segment*_tmp97;struct Cyc_Position_Segment*
_tmp98;struct Cyc_Position_Segment*_tmp99;struct Cyc_Position_Segment*_tmp9A;
struct Cyc_Absyn_Decl*_tmp9B;_LL41: if(*((int*)_tmp94)!= 4)goto _LL43;_tmp95=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp94)->f1;_tmp96=((struct Cyc_Parse_Type_spec_struct*)
_tmp94)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp96);last_loc=
_tmp96;seen_type=1;t=_tmp95;goto _LL40;_LL43: if(*((int*)_tmp94)!= 0)goto _LL45;
_tmp97=((struct Cyc_Parse_Signed_spec_struct*)_tmp94)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp97);if(seen_type)Cyc_Parse_err(({const char*_tmp9C="signed qualifier must come before type";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,39));}),_tmp97);last_loc=
_tmp97;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp94)!= 1)goto _LL47;
_tmp98=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp94)->f1;_LL46: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type)Cyc_Parse_err(({const char*
_tmp9D="signed qualifier must come before type";_tag_arr(_tmp9D,sizeof(char),
_get_zero_arr_size(_tmp9D,39));}),_tmp98);last_loc=_tmp98;seen_sign=1;sgn=(void*)
1;goto _LL40;_LL47: if(*((int*)_tmp94)!= 2)goto _LL49;_tmp99=((struct Cyc_Parse_Short_spec_struct*)
_tmp94)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp9E="integral size may appear only once within a type specifier";
_tag_arr(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,59));}),_tmp99);if(
seen_type)Cyc_Parse_err(({const char*_tmp9F="short modifier must come before base type";
_tag_arr(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,42));}),_tmp99);last_loc=
_tmp99;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp94)!= 3)goto _LL4B;
_tmp9A=((struct Cyc_Parse_Long_spec_struct*)_tmp94)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({
const char*_tmpA0="long modifier must come before base type";_tag_arr(_tmpA0,
sizeof(char),_get_zero_arr_size(_tmpA0,41));}),_tmp9A);if(seen_size){void*_tmpA1=
sz;_LL4E: if((int)_tmpA1 != 2)goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51:
Cyc_Parse_err(({const char*_tmpA2="extra long in type specifier";_tag_arr(_tmpA2,
sizeof(char),_get_zero_arr_size(_tmpA2,29));}),_tmp9A);goto _LL4D;_LL4D:;}
last_loc=_tmp9A;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp94)!= 5)goto _LL40;
_tmp9B=((struct Cyc_Parse_Decl_spec_struct*)_tmp94)->f1;_LL4C: last_loc=_tmp9B->loc;
if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpA3=(void*)_tmp9B->r;struct
Cyc_Absyn_Aggrdecl*_tmpA4;struct Cyc_Absyn_Tuniondecl*_tmpA5;struct Cyc_Absyn_Enumdecl*
_tmpA6;_LL53: if(_tmpA3 <= (void*)2  || *((int*)_tmpA3)!= 4)goto _LL55;_tmpA4=((
struct Cyc_Absyn_Aggr_d_struct*)_tmpA3)->f1;_LL54: {struct Cyc_List_List*_tmpA7=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA4->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpA8=_cycalloc(
sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_AggrType_struct _tmpA9;_tmpA9.tag=10;
_tmpA9.f1=({struct Cyc_Absyn_AggrInfo _tmpAA;_tmpAA.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmpAC;_tmpAC.tag=0;_tmpAC.f1=(void*)((void*)
_tmpA4->kind);_tmpAC.f2=_tmpA4->name;_tmpAC;});_tmpAB;}));_tmpAA.targs=_tmpA7;
_tmpAA;});_tmpA9;});_tmpA8;});if(_tmpA4->impl != 0)declopt=({struct Cyc_Core_Opt*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->v=_tmp9B;_tmpAD;});goto _LL52;}_LL55:
if(_tmpA3 <= (void*)2  || *((int*)_tmpA3)!= 5)goto _LL57;_tmpA5=((struct Cyc_Absyn_Tunion_d_struct*)
_tmpA3)->f1;_LL56: {struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA5->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_TunionType_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=({struct Cyc_Absyn_TunionInfo
_tmpB1;_tmpB1.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB3;_tmpB3.tag=1;_tmpB3.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=_tmpA5;_tmpB4;});_tmpB3;});_tmpB2;}));_tmpB1.targs=
_tmpAE;_tmpB1.rgn=(void*)((void*)2);_tmpB1;});_tmpB0;});_tmpAF;});if(_tmpA5->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=
_tmp9B;_tmpB5;});goto _LL52;}_LL57: if(_tmpA3 <= (void*)2  || *((int*)_tmpA3)!= 6)
goto _LL59;_tmpA6=((struct Cyc_Absyn_Enum_d_struct*)_tmpA3)->f1;_LL58: t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({
struct Cyc_Absyn_EnumType_struct _tmpB7;_tmpB7.tag=12;_tmpB7.f1=_tmpA6->name;
_tmpB7.f2=0;_tmpB7;});_tmpB6;});declopt=({struct Cyc_Core_Opt*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8->v=_tmp9B;_tmpB8;});goto _LL52;_LL59:;_LL5A:({void*_tmpB9[
0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp9B->loc,({const char*_tmpBA="bad declaration within type specifier";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,38));}),_tag_arr(_tmpB9,
sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9B->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpBB[0]={};
Cyc_Tcutil_warn(last_loc,({const char*_tmpBC="missing type within specifier";
_tag_arr(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,30));}),_tag_arr(_tmpBB,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_IntType_struct _tmpBE;_tmpBE.tag=5;
_tmpBE.f1=(void*)sgn;_tmpBE.f2=(void*)sz;_tmpBE;});_tmpBD;});}else{if(seen_sign){
void*_tmpBF=t;void*_tmpC0;_LL5C: if(_tmpBF <= (void*)4  || *((int*)_tmpBF)!= 5)goto
_LL5E;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f2;_LL5D: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({
struct Cyc_Absyn_IntType_struct _tmpC2;_tmpC2.tag=5;_tmpC2.f1=(void*)sgn;_tmpC2.f2=(
void*)_tmpC0;_tmpC2;});_tmpC1;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const
char*_tmpC3="sign specification on non-integral type";_tag_arr(_tmpC3,sizeof(
char),_get_zero_arr_size(_tmpC3,40));}),last_loc);goto _LL5B;_LL5B:;}if(seen_size){
void*_tmpC4=t;void*_tmpC5;_LL61: if(_tmpC4 <= (void*)4  || *((int*)_tmpC4)!= 5)goto
_LL63;_tmpC5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC4)->f1;_LL62: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_Absyn_IntType_struct _tmpC7;_tmpC7.tag=5;_tmpC7.f1=(void*)_tmpC5;_tmpC7.f2=(
void*)sz;_tmpC7;});_tmpC6;});goto _LL60;_LL63: if(_tmpC4 <= (void*)4  || *((int*)
_tmpC4)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpC8=
_cycalloc_atomic(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC9;_tmpC9.tag=6;_tmpC9.f1=1;_tmpC9;});_tmpC8;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCA="size qualifier on non-integral type";_tag_arr(_tmpCA,sizeof(
char),_get_zero_arr_size(_tmpCA,36));}),last_loc);goto _LL60;_LL60:;}}return({
struct _tuple5 _tmpCB;_tmpCB.f1=t;_tmpCB.f2=declopt;_tmpCB;});}static struct Cyc_List_List*
Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCC=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCD=_tmpCC->id;
struct Cyc_Absyn_Tqual _tmpCF;void*_tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*
_tmpD2;struct _tuple6 _tmpCE=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCC->tms);
_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;_tmpD1=_tmpCE.f3;_tmpD2=_tmpCE.f4;return({
struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=({struct _tuple7*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=_tmpCD;_tmpD4->f2=_tmpCF;_tmpD4->f3=
_tmpD0;_tmpD4->f4=_tmpD1;_tmpD4->f5=_tmpD2;_tmpD4;});_tmpD3->tl=Cyc_Parse_apply_tmss(
_tmpCF,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD3;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD5;_tmpD5.f1=
tq;_tmpD5.f2=t;_tmpD5.f3=0;_tmpD5.f4=atts;_tmpD5;});{void*_tmpD6=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD7;struct Cyc_Position_Segment*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Absyn_Conref*_tmpDA;struct Cyc_Position_Segment*_tmpDB;void*
_tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_Position_Segment*_tmpDE;struct Cyc_Absyn_PtrAtts
_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;struct Cyc_Position_Segment*_tmpE1;struct Cyc_List_List*
_tmpE2;_LL68: if(*((int*)_tmpD6)!= 0)goto _LL6A;_tmpD7=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpD6)->f1;_tmpD8=((struct Cyc_Absyn_Carray_mod_struct*)_tmpD6)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD7,
_tmpD8),atts,tms->tl);_LL6A: if(*((int*)_tmpD6)!= 1)goto _LL6C;_tmpD9=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD6)->f1;_tmpDA=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD6)->f2;_tmpDB=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD6)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpD9,
_tmpDA,_tmpDB),atts,tms->tl);_LL6C: if(*((int*)_tmpD6)!= 3)goto _LL6E;_tmpDC=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpD6)->f1;_LL6D: {void*_tmpE3=_tmpDC;
struct Cyc_List_List*_tmpE4;int _tmpE5;struct Cyc_Absyn_VarargInfo*_tmpE6;struct Cyc_Core_Opt*
_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_Position_Segment*_tmpE9;_LL75: if(*((
int*)_tmpE3)!= 1)goto _LL77;_tmpE4=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f1;
_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f2;_tmpE6=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE3)->f3;_tmpE7=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f4;_tmpE8=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)((
void*)as->hd);_tmpEA->tl=fn_atts;_tmpEA;});else{new_atts=({struct Cyc_List_List*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)((void*)as->hd);_tmpEB->tl=
new_atts;_tmpEB;});}}}if(tms->tl != 0){void*_tmpEC=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpED;_LL7A: if(*((int*)_tmpEC)!= 
4)goto _LL7C;_tmpED=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpEC)->f1;_LL7B:
typvars=_tmpED;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE5  && _tmpE6 == 0) && _tmpE4 != 0) && _tmpE4->tl == 0) && (*((struct _tuple2*)
_tmpE4->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE4->hd)).f3 == (void*)0)_tmpE4=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE4);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE7,t,_tmpE4,_tmpE5,_tmpE6,_tmpE8,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE3)!= 0)goto _LL74;_tmpE9=((struct Cyc_Absyn_NoTypes_struct*)_tmpE3)->f2;_LL78:({
void*_tmpEE[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(_tmpE9,({const char*_tmpEF="function declaration without parameter types";
_tag_arr(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,45));}),_tag_arr(_tmpEE,
sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD6)!= 4)goto _LL70;_tmpDD=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD6)->f1;_tmpDE=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD6)->f2;_LL6F: if(tms->tl == 0)return({struct _tuple6 _tmpF0;_tmpF0.f1=tq;_tmpF0.f2=
t;_tmpF0.f3=_tmpDD;_tmpF0.f4=atts;_tmpF0;});({void*_tmpF1[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
_tmpDE,({const char*_tmpF2="type parameters must appear before function arguments in declarator";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,68));}),_tag_arr(_tmpF1,
sizeof(void*),0));});_LL70: if(*((int*)_tmpD6)!= 2)goto _LL72;_tmpDF=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD6)->f1;_tmpE0=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD6)->f2;_LL71:
return Cyc_Parse_apply_tms(_tmpE0,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_PointerType_struct
_tmpF4;_tmpF4.tag=4;_tmpF4.f1=({struct Cyc_Absyn_PtrInfo _tmpF5;_tmpF5.elt_typ=(
void*)t;_tmpF5.elt_tq=tq;_tmpF5.ptr_atts=_tmpDF;_tmpF5;});_tmpF4;});_tmpF3;}),
atts,tms->tl);_LL72: if(*((int*)_tmpD6)!= 5)goto _LL67;_tmpE1=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD6)->f1;_tmpE2=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD6)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpE2),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpF7;struct Cyc_Core_Opt*_tmpF8;struct _tuple5 _tmpF6=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;if(_tmpF8 != 0)({void*_tmpF9[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmpFA="ignoring nested type declaration(s) in specifier list";
_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,54));}),_tag_arr(_tmpF9,
sizeof(void*),0));});return _tmpF7;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t){struct _tuple7 _tmpFC;struct
_tuple1*_tmpFD;struct Cyc_Absyn_Tqual _tmpFE;void*_tmpFF;struct Cyc_List_List*
_tmp100;struct Cyc_List_List*_tmp101;struct _tuple7*_tmpFB=t;_tmpFC=*_tmpFB;_tmpFD=
_tmpFC.f1;_tmpFE=_tmpFC.f2;_tmpFF=_tmpFC.f3;_tmp100=_tmpFC.f4;_tmp101=_tmpFC.f5;
Cyc_Lex_register_typedef(_tmpFD);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmp102=_tmpFF;struct Cyc_Core_Opt*_tmp103;_LL7F: if(_tmp102 <= (void*)4
 || *((int*)_tmp102)!= 0)goto _LL81;_tmp103=((struct Cyc_Absyn_Evar_struct*)
_tmp102)->f1;_LL80: type=0;if(_tmp103 == 0)kind=({struct Cyc_Core_Opt*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104->v=(void*)((void*)2);_tmp104;});else{kind=
_tmp103;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105->v=(void*)_tmpFF;_tmp105;});goto _LL7E;_LL7E:;}
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp107;
_tmp107.tag=7;_tmp107.f1=({struct Cyc_Absyn_Typedefdecl*_tmp108=_cycalloc(sizeof(*
_tmp108));_tmp108->name=_tmpFD;_tmp108->tvs=_tmp100;_tmp108->kind=kind;_tmp108->defn=
type;_tmp108->atts=_tmp101;_tmp108->tq=_tmpFE;_tmp108;});_tmp107;});_tmp106;}),
loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,
struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10A;_tmp10A.tag=11;_tmp10A.f1=d;_tmp10A.f2=s;_tmp10A;});_tmp109;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10C;struct Cyc_Absyn_Tqual _tmp10D;struct
Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Parse_Declaration_spec*
_tmp10B=ds;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.tq;_tmp10E=_tmp10C.type_specs;
_tmp10F=_tmp10C.attributes;if(_tmp10D.loc == 0)_tmp10D.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp110="inline is allowed only on function definitions";
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
_tmp11D;_LL93: if(_tmp11A <= (void*)2  || *((int*)_tmp11A)!= 6)goto _LL95;_tmp11B=((
struct Cyc_Absyn_Enum_d_struct*)_tmp11A)->f1;_LL94:(void*)(_tmp11B->sc=(void*)s);
if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp11E="bad attributes on enum";
_tag_arr(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,23));}),loc);goto _LL92;
_LL95: if(_tmp11A <= (void*)2  || *((int*)_tmp11A)!= 4)goto _LL97;_tmp11C=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp11A)->f1;_LL96:(void*)(_tmp11C->sc=(void*)s);
_tmp11C->attributes=_tmp10F;goto _LL92;_LL97: if(_tmp11A <= (void*)2  || *((int*)
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
_tmp132;_LL9C: if(_tmp122 <= (void*)4  || *((int*)_tmp122)!= 10)goto _LL9E;_tmp123=((
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
void*)4  || *((int*)_tmp122)!= 2)goto _LLA0;_tmp128=((struct Cyc_Absyn_TunionType_struct*)
_tmp122)->f1;_tmp129=(void*)_tmp128.tunion_info;if(*((int*)_tmp129)!= 1)goto
_LLA0;_tmp12A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp129)->f1;_LL9F: if(
_tmp10F != 0)Cyc_Parse_err(({const char*_tmp13A="bad attributes on tunion";
_tag_arr(_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,25));}),loc);return({
struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp13D;_tmp13D.tag=5;_tmp13D.f1=*
_tmp12A;_tmp13D;});_tmp13C;}),loc);_tmp13B->tl=0;_tmp13B;});_LLA0: if(_tmp122 <= (
void*)4  || *((int*)_tmp122)!= 2)goto _LLA2;_tmp12B=((struct Cyc_Absyn_TunionType_struct*)
_tmp122)->f1;_tmp12C=(void*)_tmp12B.tunion_info;if(*((int*)_tmp12C)!= 0)goto
_LLA2;_tmp12D=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12C)->f1;_tmp12E=
_tmp12D.name;_tmp12F=_tmp12D.is_xtunion;_tmp130=_tmp12B.targs;_LLA1: {struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp130);struct Cyc_Absyn_Decl*_tmp13F=Cyc_Absyn_tunion_decl(s,_tmp12E,
_tmp13E,0,_tmp12F,loc);if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on tunion";
_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,25));}),loc);return({
struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=_tmp13F;
_tmp141->tl=0;_tmp141;});}_LLA2: if(_tmp122 <= (void*)4  || *((int*)_tmp122)!= 12)
goto _LLA4;_tmp131=((struct Cyc_Absyn_EnumType_struct*)_tmp122)->f1;_LLA3: {struct
Cyc_Absyn_Enumdecl*_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->sc=(void*)s;_tmp148->name=_tmp131;_tmp148->fields=0;_tmp148;});
if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp143="bad attributes on enum";
_tag_arr(_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,23));}),loc);return({
struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=({struct Cyc_Absyn_Decl*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp147;_tmp147.tag=6;_tmp147.f1=_tmp142;_tmp147;});_tmp146;}));_tmp145->loc=loc;
_tmp145;});_tmp144->tl=0;_tmp144;});}_LLA4: if(_tmp122 <= (void*)4  || *((int*)
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
_LLA9: if(_tmp154 <= (void*)2  || *((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);
_tmp155->attributes=_tmp10F;_tmp10F=0;goto _LLA8;_LLAB: if(_tmp154 <= (void*)2  || *((
int*)_tmp154)!= 5)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)_tmp154)->f1;
_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(_tmp154 <= (void*)2  || *((
int*)_tmp154)!= 6)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp158="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,70));}),loc);goto _LLA8;
_LLA8:;}decls=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=
d;_tmp159->tl=decls;_tmp159;});}return decls;}}else{if(_tmp116.f2 != 0)Cyc_Parse_unimp(({
const char*_tmp15A="nested type declaration within declarator";_tag_arr(_tmp15A,
sizeof(char),_get_zero_arr_size(_tmp15A,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp15B=_tmp152;for(0;_tmp15B != 0;(_tmp15B=_tmp15B->tl,
_tmp115=_tmp115->tl)){struct _tuple7 _tmp15D;struct _tuple1*_tmp15E;struct Cyc_Absyn_Tqual
_tmp15F;void*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;
struct _tuple7*_tmp15C=(struct _tuple7*)_tmp15B->hd;_tmp15D=*_tmp15C;_tmp15E=
_tmp15D.f1;_tmp15F=_tmp15D.f2;_tmp160=_tmp15D.f3;_tmp161=_tmp15D.f4;_tmp162=
_tmp15D.f5;if(_tmp161 != 0)({void*_tmp163[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp164="bad type params, ignoring";_tag_arr(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,26));}),_tag_arr(_tmp163,sizeof(void*),0));});if(
_tmp115 == 0)({void*_tmp165[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,({const char*_tmp166="unexpected NULL in parse!";
_tag_arr(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,26));}),_tag_arr(
_tmp165,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp167=(struct Cyc_Absyn_Exp*)
_tmp115->hd;struct Cyc_Absyn_Vardecl*_tmp168=Cyc_Absyn_new_vardecl(_tmp15E,
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
_tmp16E,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp171=(void*)p->r;struct _tuple1*_tmp172;struct Cyc_Absyn_Vardecl*
_tmp173;struct Cyc_Absyn_Pat*_tmp174;void*_tmp175;int _tmp176;char _tmp177;struct
_tagged_arr _tmp178;struct _tuple1*_tmp179;struct Cyc_List_List*_tmp17A;int _tmp17B;
struct Cyc_Absyn_Exp*_tmp17C;_LLBA: if(_tmp171 <= (void*)2  || *((int*)_tmp171)!= 12)
goto _LLBC;_tmp172=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp171)->f1;_LLBB:
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp17D=
_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp17E;_tmp17E.tag=2;_tmp17E.f1=_tmp172;_tmp17E;});_tmp17D;}),p->loc);_LLBC: if(
_tmp171 <= (void*)2  || *((int*)_tmp171)!= 1)goto _LLBE;_tmp173=((struct Cyc_Absyn_Reference_p_struct*)
_tmp171)->f1;_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp180;_tmp180.tag=2;_tmp180.f1=_tmp173->name;
_tmp180;});_tmp17F;}),p->loc),p->loc);_LLBE: if(_tmp171 <= (void*)2  || *((int*)
_tmp171)!= 4)goto _LLC0;_tmp174=((struct Cyc_Absyn_Pointer_p_struct*)_tmp171)->f1;
_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp174),p->loc);_LLC0: if((
int)_tmp171 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(p->loc);_LLC2: if(
_tmp171 <= (void*)2  || *((int*)_tmp171)!= 7)goto _LLC4;_tmp175=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp171)->f1;_tmp176=((struct Cyc_Absyn_Int_p_struct*)_tmp171)->f2;_LLC3: return
Cyc_Absyn_int_exp(_tmp175,_tmp176,p->loc);_LLC4: if(_tmp171 <= (void*)2  || *((int*)
_tmp171)!= 8)goto _LLC6;_tmp177=((struct Cyc_Absyn_Char_p_struct*)_tmp171)->f1;
_LLC5: return Cyc_Absyn_char_exp(_tmp177,p->loc);_LLC6: if(_tmp171 <= (void*)2  || *((
int*)_tmp171)!= 9)goto _LLC8;_tmp178=((struct Cyc_Absyn_Float_p_struct*)_tmp171)->f1;
_LLC7: return Cyc_Absyn_float_exp(_tmp178,p->loc);_LLC8: if(_tmp171 <= (void*)2  || *((
int*)_tmp171)!= 13)goto _LLCA;_tmp179=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp171)->f1;_tmp17A=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp171)->f2;
_tmp17B=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp171)->f3;if(_tmp17B != 0)goto
_LLCA;_LLC9: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp182;_tmp182.tag=2;_tmp182.f1=_tmp179;_tmp182;});_tmp181;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp17A);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(_tmp171 <= (void*)2  || *((int*)_tmp171)!= 14)goto _LLCC;
_tmp17C=((struct Cyc_Absyn_Exp_p_struct*)_tmp171)->f1;_LLCB: return _tmp17C;_LLCC:;
_LLCD: Cyc_Parse_err(({const char*_tmp183="cannot mix patterns and expressions in case";
_tag_arr(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,44));}),p->loc);return
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
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";static char _tmp185[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp185,
_tmp185,_tmp185 + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp186=yy1;struct _tuple13*_tmp187;_LLCF: if(*((void**)_tmp186)!= Cyc_Int_tok)
goto _LLD1;_tmp187=((struct Cyc_Int_tok_struct*)_tmp186)->f1;_LLD0: yyzzz=_tmp187;
goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLCE:;}return
yyzzz;}static char _tmp189[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp189,_tmp189,_tmp189 + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp18A=yy1;char _tmp18B;_LLD4: if(*((void**)_tmp18A)!= Cyc_Char_tok)
goto _LLD6;_tmp18B=((struct Cyc_Char_tok_struct*)_tmp18A)->f1;_LLD5: yyzzz=_tmp18B;
goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return
yyzzz;}static char _tmp18D[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp18D,_tmp18D,_tmp18D + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp18E=yy1;struct _tagged_arr _tmp18F;
_LLD9: if(*((void**)_tmp18E)!= Cyc_String_tok)goto _LLDB;_tmp18F=((struct Cyc_String_tok_struct*)
_tmp18E)->f1;_LLDA: yyzzz=_tmp18F;goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLD8:;}return yyzzz;}static char _tmp191[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp191,
_tmp191,_tmp191 + 54}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*
yyzzz;{void*_tmp192=yy1;struct _tuple16*_tmp193;_LLDE: if(*((void**)_tmp192)!= Cyc_YY1)
goto _LLE0;_tmp193=((struct Cyc_YY1_struct*)_tmp192)->f1;_LLDF: yyzzz=_tmp193;goto
_LLDD;_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}
static char _tmp195[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp195,_tmp195,_tmp195 + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp196=yy1;struct Cyc_Absyn_Conref*
_tmp197;_LLE3: if(*((void**)_tmp196)!= Cyc_YY2)goto _LLE5;_tmp197=((struct Cyc_YY2_struct*)
_tmp196)->f1;_LLE4: yyzzz=_tmp197;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLE2:;}return yyzzz;}static char _tmp199[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp199,_tmp199,_tmp199 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp19A=yy1;struct
Cyc_Absyn_Exp*_tmp19B;_LLE8: if(*((void**)_tmp19A)!= Cyc_Exp_tok)goto _LLEA;
_tmp19B=((struct Cyc_Exp_tok_struct*)_tmp19A)->f1;_LLE9: yyzzz=_tmp19B;goto _LLE7;
_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLE7:;}return yyzzz;}static
char _tmp19D[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp19D,_tmp19D,_tmp19D + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19E=yy1;struct Cyc_List_List*
_tmp19F;_LLED: if(*((void**)_tmp19E)!= Cyc_ExpList_tok)goto _LLEF;_tmp19F=((struct
Cyc_ExpList_tok_struct*)_tmp19E)->f1;_LLEE: yyzzz=_tmp19F;goto _LLEC;_LLEF:;_LLF0:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLEC:;}return yyzzz;}static char
_tmp1A1[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp1A1,_tmp1A1,_tmp1A1 + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;_LLF2: if(*((void**)_tmp1A2)
!= Cyc_InitializerList_tok)goto _LLF4;_tmp1A3=((struct Cyc_InitializerList_tok_struct*)
_tmp1A2)->f1;_LLF3: yyzzz=_tmp1A3;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLF1:;}return yyzzz;}static char _tmp1A5[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp1A5,_tmp1A5,_tmp1A5 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp1A6=yy1;void*_tmp1A7;_LLF7: if(*((void**)_tmp1A6)
!= Cyc_Primop_tok)goto _LLF9;_tmp1A7=(void*)((struct Cyc_Primop_tok_struct*)
_tmp1A6)->f1;_LLF8: yyzzz=_tmp1A7;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLF6:;}return yyzzz;}static char _tmp1A9[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp1A9,_tmp1A9,_tmp1A9 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp1AA=yy1;struct Cyc_Core_Opt*_tmp1AB;_LLFC: if(*((void**)_tmp1AA)!= Cyc_Primopopt_tok)
goto _LLFE;_tmp1AB=((struct Cyc_Primopopt_tok_struct*)_tmp1AA)->f1;_LLFD: yyzzz=
_tmp1AB;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLFB:;}return yyzzz;}static char _tmp1AD[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1AD,_tmp1AD,_tmp1AD + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp1AE=yy1;
struct _tuple1*_tmp1AF;_LL101: if(*((void**)_tmp1AE)!= Cyc_QualId_tok)goto _LL103;
_tmp1AF=((struct Cyc_QualId_tok_struct*)_tmp1AE)->f1;_LL102: yyzzz=_tmp1AF;goto
_LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL100:;}return
yyzzz;}static char _tmp1B1[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp1B1,_tmp1B1,_tmp1B1 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp1B2=yy1;struct Cyc_Absyn_Stmt*
_tmp1B3;_LL106: if(*((void**)_tmp1B2)!= Cyc_Stmt_tok)goto _LL108;_tmp1B3=((struct
Cyc_Stmt_tok_struct*)_tmp1B2)->f1;_LL107: yyzzz=_tmp1B3;goto _LL105;_LL108:;_LL109:(
int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL105:;}return yyzzz;}static char _tmp1B5[
24]="list_t<switch_clause_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{_tmp1B5,_tmp1B5,_tmp1B5 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B6=yy1;struct Cyc_List_List*
_tmp1B7;_LL10B: if(*((void**)_tmp1B6)!= Cyc_SwitchClauseList_tok)goto _LL10D;
_tmp1B7=((struct Cyc_SwitchClauseList_tok_struct*)_tmp1B6)->f1;_LL10C: yyzzz=
_tmp1B7;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL10A:;}return yyzzz;}static char _tmp1B9[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1B9,_tmp1B9,_tmp1B9 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1BA=yy1;
struct Cyc_Absyn_Pat*_tmp1BB;_LL110: if(*((void**)_tmp1BA)!= Cyc_Pattern_tok)goto
_LL112;_tmp1BB=((struct Cyc_Pattern_tok_struct*)_tmp1BA)->f1;_LL111: yyzzz=_tmp1BB;
goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL10F:;}
return yyzzz;}static char _tmp1BD[23]="$(list_t<pat_t>,bool)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1BD,_tmp1BD,_tmp1BD + 23}};struct _tuple17*
Cyc_yyget_YY3(void*yy1){struct _tuple17*yyzzz;{void*_tmp1BE=yy1;struct _tuple17*
_tmp1BF;_LL115: if(*((void**)_tmp1BE)!= Cyc_YY3)goto _LL117;_tmp1BF=((struct Cyc_YY3_struct*)
_tmp1BE)->f1;_LL116: yyzzz=_tmp1BF;goto _LL114;_LL117:;_LL118:(int)_throw((void*)&
Cyc_yyfail_YY3);_LL114:;}return yyzzz;}static char _tmp1C1[14]="list_t<pat_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{
_tmp1C1,_tmp1C1,_tmp1C1 + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1C2=yy1;struct Cyc_List_List*_tmp1C3;
_LL11A: if(*((void**)_tmp1C2)!= Cyc_PatternList_tok)goto _LL11C;_tmp1C3=((struct
Cyc_PatternList_tok_struct*)_tmp1C2)->f1;_LL11B: yyzzz=_tmp1C3;goto _LL119;_LL11C:;
_LL11D:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);_LL119:;}return yyzzz;}
static char _tmp1C5[31]="$(list_t<designator_t>,pat_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 31}};
struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){struct _tuple11*yyzzz;{void*
_tmp1C6=yy1;struct _tuple11*_tmp1C7;_LL11F: if(*((void**)_tmp1C6)!= Cyc_FieldPattern_tok)
goto _LL121;_tmp1C7=((struct Cyc_FieldPattern_tok_struct*)_tmp1C6)->f1;_LL120:
yyzzz=_tmp1C7;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);
_LL11E:;}return yyzzz;}static char _tmp1C9[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1C9,_tmp1C9,_tmp1C9 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CA=yy1;struct Cyc_List_List*
_tmp1CB;_LL124: if(*((void**)_tmp1CA)!= Cyc_FieldPatternList_tok)goto _LL126;
_tmp1CB=((struct Cyc_FieldPatternList_tok_struct*)_tmp1CA)->f1;_LL125: yyzzz=
_tmp1CB;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL123:;}return yyzzz;}static char _tmp1CD[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1CD,
_tmp1CD,_tmp1CD + 48}};struct _tuple17*Cyc_yyget_YY4(void*yy1){struct _tuple17*
yyzzz;{void*_tmp1CE=yy1;struct _tuple17*_tmp1CF;_LL129: if(*((void**)_tmp1CE)!= 
Cyc_YY4)goto _LL12B;_tmp1CF=((struct Cyc_YY4_struct*)_tmp1CE)->f1;_LL12A: yyzzz=
_tmp1CF;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY4);_LL128:;}
return yyzzz;}static char _tmp1D1[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1D2=yy1;
struct Cyc_Absyn_Fndecl*_tmp1D3;_LL12E: if(*((void**)_tmp1D2)!= Cyc_FnDecl_tok)
goto _LL130;_tmp1D3=((struct Cyc_FnDecl_tok_struct*)_tmp1D2)->f1;_LL12F: yyzzz=
_tmp1D3;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL12D:;}return yyzzz;}static char _tmp1D5[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp1D5,_tmp1D5,_tmp1D5 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1D6=yy1;struct Cyc_List_List*_tmp1D7;_LL133: if(*((void**)_tmp1D6)!= Cyc_DeclList_tok)
goto _LL135;_tmp1D7=((struct Cyc_DeclList_tok_struct*)_tmp1D6)->f1;_LL134: yyzzz=
_tmp1D7;goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL132:;}return yyzzz;}static char _tmp1D9[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1D9,_tmp1D9,_tmp1D9 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp1DA=yy1;struct Cyc_Parse_Declaration_spec*_tmp1DB;_LL138: if(*((
void**)_tmp1DA)!= Cyc_DeclSpec_tok)goto _LL13A;_tmp1DB=((struct Cyc_DeclSpec_tok_struct*)
_tmp1DA)->f1;_LL139: yyzzz=_tmp1DB;goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL137:;}return yyzzz;}static char _tmp1DD[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1DD,_tmp1DD,_tmp1DD + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp1DE=yy1;struct _tuple12*_tmp1DF;_LL13D: if(*((void**)
_tmp1DE)!= Cyc_InitDecl_tok)goto _LL13F;_tmp1DF=((struct Cyc_InitDecl_tok_struct*)
_tmp1DE)->f1;_LL13E: yyzzz=_tmp1DF;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL13C:;}return yyzzz;}static char _tmp1E1[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1E1,_tmp1E1,_tmp1E1 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E2=yy1;struct Cyc_List_List*
_tmp1E3;_LL142: if(*((void**)_tmp1E2)!= Cyc_InitDeclList_tok)goto _LL144;_tmp1E3=((
struct Cyc_InitDeclList_tok_struct*)_tmp1E2)->f1;_LL143: yyzzz=_tmp1E3;goto _LL141;
_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL141:;}return
yyzzz;}static char _tmp1E5[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1E5,_tmp1E5,_tmp1E5 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1E6=yy1;void*
_tmp1E7;_LL147: if(*((void**)_tmp1E6)!= Cyc_StorageClass_tok)goto _LL149;_tmp1E7=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1E6)->f1;_LL148: yyzzz=_tmp1E7;
goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL146:;}return yyzzz;}static char _tmp1E9[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1EA=yy1;void*
_tmp1EB;_LL14C: if(*((void**)_tmp1EA)!= Cyc_TypeSpecifier_tok)goto _LL14E;_tmp1EB=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1EA)->f1;_LL14D: yyzzz=_tmp1EB;
goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL14B:;}return yyzzz;}static char _tmp1ED[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1EE=yy1;void*
_tmp1EF;_LL151: if(*((void**)_tmp1EE)!= Cyc_StructOrUnion_tok)goto _LL153;_tmp1EF=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1EE)->f1;_LL152: yyzzz=_tmp1EF;
goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL150:;}return yyzzz;}static char _tmp1F1[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1F1,_tmp1F1,_tmp1F1 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1F2=yy1;struct Cyc_Absyn_Tqual _tmp1F3;_LL156: if(*((void**)_tmp1F2)!= Cyc_TypeQual_tok)
goto _LL158;_tmp1F3=((struct Cyc_TypeQual_tok_struct*)_tmp1F2)->f1;_LL157: yyzzz=
_tmp1F3;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL155:;}return yyzzz;}static char _tmp1F5[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1F5,_tmp1F5,_tmp1F5 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F6=yy1;struct Cyc_List_List*
_tmp1F7;_LL15B: if(*((void**)_tmp1F6)!= Cyc_AggrFieldDeclList_tok)goto _LL15D;
_tmp1F7=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1F6)->f1;_LL15C: yyzzz=
_tmp1F7;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL15A:;}return yyzzz;}static char _tmp1F9[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1F9,_tmp1F9,_tmp1F9 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FA=yy1;struct Cyc_List_List*
_tmp1FB;_LL160: if(*((void**)_tmp1FA)!= Cyc_AggrFieldDeclListList_tok)goto _LL162;
_tmp1FB=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1FA)->f1;_LL161: yyzzz=
_tmp1FB;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL15F:;}return yyzzz;}static char _tmp1FD[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1FD,_tmp1FD,_tmp1FD + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FE=yy1;struct Cyc_List_List*
_tmp1FF;_LL165: if(*((void**)_tmp1FE)!= Cyc_TypeModifierList_tok)goto _LL167;
_tmp1FF=((struct Cyc_TypeModifierList_tok_struct*)_tmp1FE)->f1;_LL166: yyzzz=
_tmp1FF;goto _LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL164:;}return yyzzz;}static char _tmp201[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp202=yy1;struct Cyc_Parse_Declarator*_tmp203;_LL16A: if(*((void**)
_tmp202)!= Cyc_Declarator_tok)goto _LL16C;_tmp203=((struct Cyc_Declarator_tok_struct*)
_tmp202)->f1;_LL16B: yyzzz=_tmp203;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL169:;}return yyzzz;}static char _tmp205[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp205,_tmp205,_tmp205 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp206=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp207;_LL16F: if(*((void**)_tmp206)!= Cyc_AbstractDeclarator_tok)goto _LL171;
_tmp207=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp206)->f1;_LL170: yyzzz=
_tmp207;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL16E:;}return yyzzz;}static char _tmp209[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp209,_tmp209,_tmp209 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp20A=yy1;int _tmp20B;_LL174: if(*((void**)_tmp20A)!= 
Cyc_Bool_tok)goto _LL176;_tmp20B=((struct Cyc_Bool_tok_struct*)_tmp20A)->f1;_LL175:
yyzzz=_tmp20B;goto _LL173;_LL176:;_LL177:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL173:;}return yyzzz;}static char _tmp20D[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp20E=yy1;void*_tmp20F;_LL179: if(*((void**)_tmp20E)
!= Cyc_Scope_tok)goto _LL17B;_tmp20F=(void*)((struct Cyc_Scope_tok_struct*)_tmp20E)->f1;
_LL17A: yyzzz=_tmp20F;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL178:;}return yyzzz;}static char _tmp211[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp212=yy1;struct Cyc_Absyn_Tunionfield*_tmp213;_LL17E: if(*((void**)
_tmp212)!= Cyc_TunionField_tok)goto _LL180;_tmp213=((struct Cyc_TunionField_tok_struct*)
_tmp212)->f1;_LL17F: yyzzz=_tmp213;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL17D:;}return yyzzz;}static char _tmp215[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp215,_tmp215,_tmp215 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp216=yy1;struct Cyc_List_List*
_tmp217;_LL183: if(*((void**)_tmp216)!= Cyc_TunionFieldList_tok)goto _LL185;
_tmp217=((struct Cyc_TunionFieldList_tok_struct*)_tmp216)->f1;_LL184: yyzzz=
_tmp217;goto _LL182;_LL185:;_LL186:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL182:;}return yyzzz;}static char _tmp219[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp219,_tmp219,_tmp219 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp21A=yy1;struct _tuple15*_tmp21B;_LL188: if(*((void**)
_tmp21A)!= Cyc_QualSpecList_tok)goto _LL18A;_tmp21B=((struct Cyc_QualSpecList_tok_struct*)
_tmp21A)->f1;_LL189: yyzzz=_tmp21B;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL187:;}return yyzzz;}static char _tmp21D[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp21D,_tmp21D,_tmp21D + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp21E=yy1;struct Cyc_List_List*_tmp21F;_LL18D:
if(*((void**)_tmp21E)!= Cyc_IdList_tok)goto _LL18F;_tmp21F=((struct Cyc_IdList_tok_struct*)
_tmp21E)->f1;_LL18E: yyzzz=_tmp21F;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL18C:;}return yyzzz;}static char _tmp221[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp221,_tmp221,_tmp221 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp222=yy1;struct _tuple2*_tmp223;_LL192: if(*((void**)
_tmp222)!= Cyc_ParamDecl_tok)goto _LL194;_tmp223=((struct Cyc_ParamDecl_tok_struct*)
_tmp222)->f1;_LL193: yyzzz=_tmp223;goto _LL191;_LL194:;_LL195:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL191:;}return yyzzz;}static char _tmp225[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp225,_tmp225,_tmp225 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp226=yy1;struct Cyc_List_List*
_tmp227;_LL197: if(*((void**)_tmp226)!= Cyc_ParamDeclList_tok)goto _LL199;_tmp227=((
struct Cyc_ParamDeclList_tok_struct*)_tmp226)->f1;_LL198: yyzzz=_tmp227;goto _LL196;
_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL196:;}return
yyzzz;}static char _tmp229[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp229,
_tmp229,_tmp229 + 107}};struct _tuple14*Cyc_yyget_YY5(void*yy1){struct _tuple14*
yyzzz;{void*_tmp22A=yy1;struct _tuple14*_tmp22B;_LL19C: if(*((void**)_tmp22A)!= 
Cyc_YY5)goto _LL19E;_tmp22B=((struct Cyc_YY5_struct*)_tmp22A)->f1;_LL19D: yyzzz=
_tmp22B;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY5);_LL19B:;}
return yyzzz;}static char _tmp22D[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp22D,_tmp22D,_tmp22D + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp22E=yy1;struct Cyc_List_List*_tmp22F;_LL1A1: if(*((void**)_tmp22E)!= Cyc_TypeList_tok)
goto _LL1A3;_tmp22F=((struct Cyc_TypeList_tok_struct*)_tmp22E)->f1;_LL1A2: yyzzz=
_tmp22F;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL1A0:;}return yyzzz;}static char _tmp231[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp231,
_tmp231,_tmp231 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp232=yy1;struct Cyc_List_List*_tmp233;_LL1A6:
if(*((void**)_tmp232)!= Cyc_DesignatorList_tok)goto _LL1A8;_tmp233=((struct Cyc_DesignatorList_tok_struct*)
_tmp232)->f1;_LL1A7: yyzzz=_tmp233;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL1A5:;}return yyzzz;}static char _tmp235[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp235,_tmp235,_tmp235 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp236=yy1;void*_tmp237;_LL1AB: if(*((void**)_tmp236)!= Cyc_Designator_tok)
goto _LL1AD;_tmp237=(void*)((struct Cyc_Designator_tok_struct*)_tmp236)->f1;_LL1AC:
yyzzz=_tmp237;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL1AA:;}return yyzzz;}static char _tmp239[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp239,_tmp239,_tmp239 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp23A=yy1;void*_tmp23B;_LL1B0: if(*((void**)_tmp23A)
!= Cyc_Kind_tok)goto _LL1B2;_tmp23B=(void*)((struct Cyc_Kind_tok_struct*)_tmp23A)->f1;
_LL1B1: yyzzz=_tmp23B;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL1AF:;}return yyzzz;}static char _tmp23D[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp23E=yy1;void*_tmp23F;_LL1B5: if(*((void**)_tmp23E)
!= Cyc_Type_tok)goto _LL1B7;_tmp23F=(void*)((struct Cyc_Type_tok_struct*)_tmp23E)->f1;
_LL1B6: yyzzz=_tmp23F;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL1B4:;}return yyzzz;}static char _tmp241[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp241,
_tmp241,_tmp241 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp242=yy1;struct Cyc_List_List*_tmp243;_LL1BA:
if(*((void**)_tmp242)!= Cyc_AttributeList_tok)goto _LL1BC;_tmp243=((struct Cyc_AttributeList_tok_struct*)
_tmp242)->f1;_LL1BB: yyzzz=_tmp243;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1B9:;}return yyzzz;}static char _tmp245[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp245,_tmp245,_tmp245 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp246=yy1;void*_tmp247;_LL1BF: if(*((void**)_tmp246)!= Cyc_Attribute_tok)
goto _LL1C1;_tmp247=(void*)((struct Cyc_Attribute_tok_struct*)_tmp246)->f1;_LL1C0:
yyzzz=_tmp247;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1BE:;}return yyzzz;}static char _tmp249[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp249,_tmp249,_tmp249 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp24A=yy1;struct Cyc_Absyn_Enumfield*_tmp24B;_LL1C4: if(*((void**)
_tmp24A)!= Cyc_Enumfield_tok)goto _LL1C6;_tmp24B=((struct Cyc_Enumfield_tok_struct*)
_tmp24A)->f1;_LL1C5: yyzzz=_tmp24B;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1C3:;}return yyzzz;}static char _tmp24D[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp24D,_tmp24D,_tmp24D + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp24E=yy1;struct Cyc_List_List*
_tmp24F;_LL1C9: if(*((void**)_tmp24E)!= Cyc_EnumfieldList_tok)goto _LL1CB;_tmp24F=((
struct Cyc_EnumfieldList_tok_struct*)_tmp24E)->f1;_LL1CA: yyzzz=_tmp24F;goto _LL1C8;
_LL1CB:;_LL1CC:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1C8:;}return
yyzzz;}static char _tmp251[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp252=yy1;struct Cyc_Core_Opt*_tmp253;_LL1CE: if(*((void**)_tmp252)!= Cyc_TypeOpt_tok)
goto _LL1D0;_tmp253=((struct Cyc_TypeOpt_tok_struct*)_tmp252)->f1;_LL1CF: yyzzz=
_tmp253;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1CD:;}return yyzzz;}static char _tmp255[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp255,
_tmp255,_tmp255 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp256=yy1;struct Cyc_List_List*_tmp257;_LL1D3:
if(*((void**)_tmp256)!= Cyc_Rgnorder_tok)goto _LL1D5;_tmp257=((struct Cyc_Rgnorder_tok_struct*)
_tmp256)->f1;_LL1D4: yyzzz=_tmp257;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1D2:;}return yyzzz;}static char _tmp259[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp259,
_tmp259,_tmp259 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY6(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp25A=yy1;struct Cyc_Absyn_Conref*_tmp25B;_LL1D8: if(*((void**)
_tmp25A)!= Cyc_YY6)goto _LL1DA;_tmp25B=((struct Cyc_YY6_struct*)_tmp25A)->f1;
_LL1D9: yyzzz=_tmp25B;goto _LL1D7;_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY6);
_LL1D7:;}return yyzzz;}static char _tmp25D[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp25D,
_tmp25D,_tmp25D + 40}};struct Cyc_List_List*Cyc_yyget_YY7(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp25E=yy1;struct Cyc_List_List*_tmp25F;_LL1DD: if(*((void**)_tmp25E)
!= Cyc_YY7)goto _LL1DF;_tmp25F=((struct Cyc_YY7_struct*)_tmp25E)->f1;_LL1DE: yyzzz=
_tmp25F;goto _LL1DC;_LL1DF:;_LL1E0:(int)_throw((void*)& Cyc_yyfail_YY7);_LL1DC:;}
return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp260;_tmp260.timestamp=0;_tmp260.first_line=0;_tmp260.first_column=0;_tmp260.last_line=
0;_tmp260.last_column=0;_tmp260;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
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
99,100,101};static char _tmp261[2]="$";static char _tmp262[6]="error";static char
_tmp263[12]="$undefined.";static char _tmp264[5]="AUTO";static char _tmp265[9]="REGISTER";
static char _tmp266[7]="STATIC";static char _tmp267[7]="EXTERN";static char _tmp268[8]="TYPEDEF";
static char _tmp269[5]="VOID";static char _tmp26A[5]="CHAR";static char _tmp26B[6]="SHORT";
static char _tmp26C[4]="INT";static char _tmp26D[5]="LONG";static char _tmp26E[6]="FLOAT";
static char _tmp26F[7]="DOUBLE";static char _tmp270[7]="SIGNED";static char _tmp271[9]="UNSIGNED";
static char _tmp272[6]="CONST";static char _tmp273[9]="VOLATILE";static char _tmp274[9]="RESTRICT";
static char _tmp275[7]="STRUCT";static char _tmp276[6]="UNION";static char _tmp277[5]="CASE";
static char _tmp278[8]="DEFAULT";static char _tmp279[7]="INLINE";static char _tmp27A[7]="SIZEOF";
static char _tmp27B[9]="OFFSETOF";static char _tmp27C[3]="IF";static char _tmp27D[5]="ELSE";
static char _tmp27E[7]="SWITCH";static char _tmp27F[6]="WHILE";static char _tmp280[3]="DO";
static char _tmp281[4]="FOR";static char _tmp282[5]="GOTO";static char _tmp283[9]="CONTINUE";
static char _tmp284[6]="BREAK";static char _tmp285[7]="RETURN";static char _tmp286[5]="ENUM";
static char _tmp287[8]="NULL_kw";static char _tmp288[4]="LET";static char _tmp289[6]="THROW";
static char _tmp28A[4]="TRY";static char _tmp28B[6]="CATCH";static char _tmp28C[7]="EXPORT";
static char _tmp28D[4]="NEW";static char _tmp28E[9]="ABSTRACT";static char _tmp28F[9]="FALLTHRU";
static char _tmp290[6]="USING";static char _tmp291[10]="NAMESPACE";static char _tmp292[
7]="TUNION";static char _tmp293[8]="XTUNION";static char _tmp294[7]="MALLOC";static
char _tmp295[8]="RMALLOC";static char _tmp296[7]="CALLOC";static char _tmp297[8]="RCALLOC";
static char _tmp298[9]="REGION_T";static char _tmp299[9]="SIZEOF_T";static char
_tmp29A[6]="TAG_T";static char _tmp29B[7]="REGION";static char _tmp29C[5]="RNEW";
static char _tmp29D[8]="REGIONS";static char _tmp29E[13]="RESET_REGION";static char
_tmp29F[4]="GEN";static char _tmp2A0[14]="NOZEROTERM_kw";static char _tmp2A1[12]="ZEROTERM_kw";
static char _tmp2A2[7]="PORTON";static char _tmp2A3[8]="PORTOFF";static char _tmp2A4[7]="PTR_OP";
static char _tmp2A5[7]="INC_OP";static char _tmp2A6[7]="DEC_OP";static char _tmp2A7[8]="LEFT_OP";
static char _tmp2A8[9]="RIGHT_OP";static char _tmp2A9[6]="LE_OP";static char _tmp2AA[6]="GE_OP";
static char _tmp2AB[6]="EQ_OP";static char _tmp2AC[6]="NE_OP";static char _tmp2AD[7]="AND_OP";
static char _tmp2AE[6]="OR_OP";static char _tmp2AF[11]="MUL_ASSIGN";static char
_tmp2B0[11]="DIV_ASSIGN";static char _tmp2B1[11]="MOD_ASSIGN";static char _tmp2B2[11]="ADD_ASSIGN";
static char _tmp2B3[11]="SUB_ASSIGN";static char _tmp2B4[12]="LEFT_ASSIGN";static
char _tmp2B5[13]="RIGHT_ASSIGN";static char _tmp2B6[11]="AND_ASSIGN";static char
_tmp2B7[11]="XOR_ASSIGN";static char _tmp2B8[10]="OR_ASSIGN";static char _tmp2B9[9]="ELLIPSIS";
static char _tmp2BA[11]="LEFT_RIGHT";static char _tmp2BB[12]="COLON_COLON";static
char _tmp2BC[11]="IDENTIFIER";static char _tmp2BD[17]="INTEGER_CONSTANT";static char
_tmp2BE[7]="STRING";static char _tmp2BF[19]="CHARACTER_CONSTANT";static char _tmp2C0[
18]="FLOATING_CONSTANT";static char _tmp2C1[9]="TYPE_VAR";static char _tmp2C2[13]="TYPEDEF_NAME";
static char _tmp2C3[16]="QUAL_IDENTIFIER";static char _tmp2C4[18]="QUAL_TYPEDEF_NAME";
static char _tmp2C5[13]="TYPE_INTEGER";static char _tmp2C6[10]="ATTRIBUTE";static
char _tmp2C7[4]="';'";static char _tmp2C8[4]="'{'";static char _tmp2C9[4]="'}'";
static char _tmp2CA[4]="','";static char _tmp2CB[4]="'='";static char _tmp2CC[4]="'('";
static char _tmp2CD[4]="')'";static char _tmp2CE[4]="'_'";static char _tmp2CF[4]="'$'";
static char _tmp2D0[4]="'<'";static char _tmp2D1[4]="':'";static char _tmp2D2[4]="'.'";
static char _tmp2D3[4]="'['";static char _tmp2D4[4]="']'";static char _tmp2D5[4]="'*'";
static char _tmp2D6[4]="'@'";static char _tmp2D7[4]="'?'";static char _tmp2D8[4]="'>'";
static char _tmp2D9[4]="'+'";static char _tmp2DA[4]="'|'";static char _tmp2DB[4]="'^'";
static char _tmp2DC[4]="'&'";static char _tmp2DD[4]="'-'";static char _tmp2DE[4]="'/'";
static char _tmp2DF[4]="'%'";static char _tmp2E0[4]="'~'";static char _tmp2E1[4]="'!'";
static char _tmp2E2[5]="prog";static char _tmp2E3[17]="translation_unit";static char
_tmp2E4[12]="export_list";static char _tmp2E5[19]="export_list_values";static char
_tmp2E6[21]="external_declaration";static char _tmp2E7[20]="function_definition";
static char _tmp2E8[21]="function_definition2";static char _tmp2E9[13]="using_action";
static char _tmp2EA[15]="unusing_action";static char _tmp2EB[17]="namespace_action";
static char _tmp2EC[19]="unnamespace_action";static char _tmp2ED[12]="declaration";
static char _tmp2EE[17]="declaration_list";static char _tmp2EF[23]="declaration_specifiers";
static char _tmp2F0[24]="storage_class_specifier";static char _tmp2F1[15]="attributes_opt";
static char _tmp2F2[11]="attributes";static char _tmp2F3[15]="attribute_list";static
char _tmp2F4[10]="attribute";static char _tmp2F5[15]="type_specifier";static char
_tmp2F6[25]="type_specifier_notypedef";static char _tmp2F7[5]="kind";static char
_tmp2F8[15]="type_qualifier";static char _tmp2F9[15]="enum_specifier";static char
_tmp2FA[11]="enum_field";static char _tmp2FB[22]="enum_declaration_list";static
char _tmp2FC[26]="struct_or_union_specifier";static char _tmp2FD[16]="type_params_opt";
static char _tmp2FE[16]="struct_or_union";static char _tmp2FF[24]="struct_declaration_list";
static char _tmp300[25]="struct_declaration_list0";static char _tmp301[21]="init_declarator_list";
static char _tmp302[22]="init_declarator_list0";static char _tmp303[16]="init_declarator";
static char _tmp304[19]="struct_declaration";static char _tmp305[25]="specifier_qualifier_list";
static char _tmp306[35]="notypedef_specifier_qualifier_list";static char _tmp307[23]="struct_declarator_list";
static char _tmp308[24]="struct_declarator_list0";static char _tmp309[18]="struct_declarator";
static char _tmp30A[17]="tunion_specifier";static char _tmp30B[18]="tunion_or_xtunion";
static char _tmp30C[17]="tunionfield_list";static char _tmp30D[18]="tunionfield_scope";
static char _tmp30E[12]="tunionfield";static char _tmp30F[11]="declarator";static
char _tmp310[23]="declarator_withtypedef";static char _tmp311[18]="direct_declarator";
static char _tmp312[30]="direct_declarator_withtypedef";static char _tmp313[8]="pointer";
static char _tmp314[12]="one_pointer";static char _tmp315[23]="pointer_null_and_bound";
static char _tmp316[14]="pointer_bound";static char _tmp317[18]="zeroterm_qual_opt";
static char _tmp318[8]="rgn_opt";static char _tmp319[11]="tqual_list";static char
_tmp31A[20]="parameter_type_list";static char _tmp31B[9]="type_var";static char
_tmp31C[16]="optional_effect";static char _tmp31D[19]="optional_rgn_order";static
char _tmp31E[10]="rgn_order";static char _tmp31F[16]="optional_inject";static char
_tmp320[11]="effect_set";static char _tmp321[14]="atomic_effect";static char _tmp322[
11]="region_set";static char _tmp323[15]="parameter_list";static char _tmp324[22]="parameter_declaration";
static char _tmp325[16]="identifier_list";static char _tmp326[17]="identifier_list0";
static char _tmp327[12]="initializer";static char _tmp328[18]="array_initializer";
static char _tmp329[17]="initializer_list";static char _tmp32A[12]="designation";
static char _tmp32B[16]="designator_list";static char _tmp32C[11]="designator";
static char _tmp32D[10]="type_name";static char _tmp32E[14]="any_type_name";static
char _tmp32F[15]="type_name_list";static char _tmp330[20]="abstract_declarator";
static char _tmp331[27]="direct_abstract_declarator";static char _tmp332[10]="statement";
static char _tmp333[18]="labeled_statement";static char _tmp334[21]="expression_statement";
static char _tmp335[19]="compound_statement";static char _tmp336[16]="block_item_list";
static char _tmp337[20]="selection_statement";static char _tmp338[15]="switch_clauses";
static char _tmp339[20]="iteration_statement";static char _tmp33A[15]="jump_statement";
static char _tmp33B[12]="exp_pattern";static char _tmp33C[20]="conditional_pattern";
static char _tmp33D[19]="logical_or_pattern";static char _tmp33E[20]="logical_and_pattern";
static char _tmp33F[21]="inclusive_or_pattern";static char _tmp340[21]="exclusive_or_pattern";
static char _tmp341[12]="and_pattern";static char _tmp342[17]="equality_pattern";
static char _tmp343[19]="relational_pattern";static char _tmp344[14]="shift_pattern";
static char _tmp345[17]="additive_pattern";static char _tmp346[23]="multiplicative_pattern";
static char _tmp347[13]="cast_pattern";static char _tmp348[14]="unary_pattern";
static char _tmp349[16]="postfix_pattern";static char _tmp34A[16]="primary_pattern";
static char _tmp34B[8]="pattern";static char _tmp34C[19]="tuple_pattern_list";static
char _tmp34D[20]="tuple_pattern_list0";static char _tmp34E[14]="field_pattern";
static char _tmp34F[19]="field_pattern_list";static char _tmp350[20]="field_pattern_list0";
static char _tmp351[11]="expression";static char _tmp352[22]="assignment_expression";
static char _tmp353[20]="assignment_operator";static char _tmp354[23]="conditional_expression";
static char _tmp355[20]="constant_expression";static char _tmp356[22]="logical_or_expression";
static char _tmp357[23]="logical_and_expression";static char _tmp358[24]="inclusive_or_expression";
static char _tmp359[24]="exclusive_or_expression";static char _tmp35A[15]="and_expression";
static char _tmp35B[20]="equality_expression";static char _tmp35C[22]="relational_expression";
static char _tmp35D[17]="shift_expression";static char _tmp35E[20]="additive_expression";
static char _tmp35F[26]="multiplicative_expression";static char _tmp360[16]="cast_expression";
static char _tmp361[17]="unary_expression";static char _tmp362[15]="unary_operator";
static char _tmp363[19]="postfix_expression";static char _tmp364[19]="primary_expression";
static char _tmp365[25]="argument_expression_list";static char _tmp366[26]="argument_expression_list0";
static char _tmp367[9]="constant";static char _tmp368[20]="qual_opt_identifier";
static char _tmp369[17]="qual_opt_typedef";static char _tmp36A[18]="struct_union_name";
static char _tmp36B[11]="field_name";static char _tmp36C[12]="right_angle";static
struct _tagged_arr Cyc_yytname[268]={{_tmp261,_tmp261,_tmp261 + 2},{_tmp262,_tmp262,
_tmp262 + 6},{_tmp263,_tmp263,_tmp263 + 12},{_tmp264,_tmp264,_tmp264 + 5},{_tmp265,
_tmp265,_tmp265 + 9},{_tmp266,_tmp266,_tmp266 + 7},{_tmp267,_tmp267,_tmp267 + 7},{
_tmp268,_tmp268,_tmp268 + 8},{_tmp269,_tmp269,_tmp269 + 5},{_tmp26A,_tmp26A,
_tmp26A + 5},{_tmp26B,_tmp26B,_tmp26B + 6},{_tmp26C,_tmp26C,_tmp26C + 4},{_tmp26D,
_tmp26D,_tmp26D + 5},{_tmp26E,_tmp26E,_tmp26E + 6},{_tmp26F,_tmp26F,_tmp26F + 7},{
_tmp270,_tmp270,_tmp270 + 7},{_tmp271,_tmp271,_tmp271 + 9},{_tmp272,_tmp272,
_tmp272 + 6},{_tmp273,_tmp273,_tmp273 + 9},{_tmp274,_tmp274,_tmp274 + 9},{_tmp275,
_tmp275,_tmp275 + 7},{_tmp276,_tmp276,_tmp276 + 6},{_tmp277,_tmp277,_tmp277 + 5},{
_tmp278,_tmp278,_tmp278 + 8},{_tmp279,_tmp279,_tmp279 + 7},{_tmp27A,_tmp27A,
_tmp27A + 7},{_tmp27B,_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C,_tmp27C + 3},{_tmp27D,
_tmp27D,_tmp27D + 5},{_tmp27E,_tmp27E,_tmp27E + 7},{_tmp27F,_tmp27F,_tmp27F + 6},{
_tmp280,_tmp280,_tmp280 + 3},{_tmp281,_tmp281,_tmp281 + 4},{_tmp282,_tmp282,
_tmp282 + 5},{_tmp283,_tmp283,_tmp283 + 9},{_tmp284,_tmp284,_tmp284 + 6},{_tmp285,
_tmp285,_tmp285 + 7},{_tmp286,_tmp286,_tmp286 + 5},{_tmp287,_tmp287,_tmp287 + 8},{
_tmp288,_tmp288,_tmp288 + 4},{_tmp289,_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A,
_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 6},{_tmp28C,_tmp28C,_tmp28C + 7},{_tmp28D,
_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E,_tmp28E + 9},{_tmp28F,_tmp28F,_tmp28F + 9},{
_tmp290,_tmp290,_tmp290 + 6},{_tmp291,_tmp291,_tmp291 + 10},{_tmp292,_tmp292,
_tmp292 + 7},{_tmp293,_tmp293,_tmp293 + 8},{_tmp294,_tmp294,_tmp294 + 7},{_tmp295,
_tmp295,_tmp295 + 8},{_tmp296,_tmp296,_tmp296 + 7},{_tmp297,_tmp297,_tmp297 + 8},{
_tmp298,_tmp298,_tmp298 + 9},{_tmp299,_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A,
_tmp29A + 6},{_tmp29B,_tmp29B,_tmp29B + 7},{_tmp29C,_tmp29C,_tmp29C + 5},{_tmp29D,
_tmp29D,_tmp29D + 8},{_tmp29E,_tmp29E,_tmp29E + 13},{_tmp29F,_tmp29F,_tmp29F + 4},{
_tmp2A0,_tmp2A0,_tmp2A0 + 14},{_tmp2A1,_tmp2A1,_tmp2A1 + 12},{_tmp2A2,_tmp2A2,
_tmp2A2 + 7},{_tmp2A3,_tmp2A3,_tmp2A3 + 8},{_tmp2A4,_tmp2A4,_tmp2A4 + 7},{_tmp2A5,
_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 7},{_tmp2A7,_tmp2A7,_tmp2A7 + 8},{
_tmp2A8,_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9,_tmp2A9 + 6},{_tmp2AA,_tmp2AA,
_tmp2AA + 6},{_tmp2AB,_tmp2AB,_tmp2AB + 6},{_tmp2AC,_tmp2AC,_tmp2AC + 6},{_tmp2AD,
_tmp2AD,_tmp2AD + 7},{_tmp2AE,_tmp2AE,_tmp2AE + 6},{_tmp2AF,_tmp2AF,_tmp2AF + 11},{
_tmp2B0,_tmp2B0,_tmp2B0 + 11},{_tmp2B1,_tmp2B1,_tmp2B1 + 11},{_tmp2B2,_tmp2B2,
_tmp2B2 + 11},{_tmp2B3,_tmp2B3,_tmp2B3 + 11},{_tmp2B4,_tmp2B4,_tmp2B4 + 12},{
_tmp2B5,_tmp2B5,_tmp2B5 + 13},{_tmp2B6,_tmp2B6,_tmp2B6 + 11},{_tmp2B7,_tmp2B7,
_tmp2B7 + 11},{_tmp2B8,_tmp2B8,_tmp2B8 + 10},{_tmp2B9,_tmp2B9,_tmp2B9 + 9},{_tmp2BA,
_tmp2BA,_tmp2BA + 11},{_tmp2BB,_tmp2BB,_tmp2BB + 12},{_tmp2BC,_tmp2BC,_tmp2BC + 11},{
_tmp2BD,_tmp2BD,_tmp2BD + 17},{_tmp2BE,_tmp2BE,_tmp2BE + 7},{_tmp2BF,_tmp2BF,
_tmp2BF + 19},{_tmp2C0,_tmp2C0,_tmp2C0 + 18},{_tmp2C1,_tmp2C1,_tmp2C1 + 9},{_tmp2C2,
_tmp2C2,_tmp2C2 + 13},{_tmp2C3,_tmp2C3,_tmp2C3 + 16},{_tmp2C4,_tmp2C4,_tmp2C4 + 18},{
_tmp2C5,_tmp2C5,_tmp2C5 + 13},{_tmp2C6,_tmp2C6,_tmp2C6 + 10},{_tmp2C7,_tmp2C7,
_tmp2C7 + 4},{_tmp2C8,_tmp2C8,_tmp2C8 + 4},{_tmp2C9,_tmp2C9,_tmp2C9 + 4},{_tmp2CA,
_tmp2CA,_tmp2CA + 4},{_tmp2CB,_tmp2CB,_tmp2CB + 4},{_tmp2CC,_tmp2CC,_tmp2CC + 4},{
_tmp2CD,_tmp2CD,_tmp2CD + 4},{_tmp2CE,_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF,
_tmp2CF + 4},{_tmp2D0,_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 4},{_tmp2D2,
_tmp2D2,_tmp2D2 + 4},{_tmp2D3,_tmp2D3,_tmp2D3 + 4},{_tmp2D4,_tmp2D4,_tmp2D4 + 4},{
_tmp2D5,_tmp2D5,_tmp2D5 + 4},{_tmp2D6,_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7,
_tmp2D7 + 4},{_tmp2D8,_tmp2D8,_tmp2D8 + 4},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{_tmp2DA,
_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB,_tmp2DB + 4},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{
_tmp2DD,_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,
_tmp2DF + 4},{_tmp2E0,_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,
_tmp2E2,_tmp2E2 + 5},{_tmp2E3,_tmp2E3,_tmp2E3 + 17},{_tmp2E4,_tmp2E4,_tmp2E4 + 12},{
_tmp2E5,_tmp2E5,_tmp2E5 + 19},{_tmp2E6,_tmp2E6,_tmp2E6 + 21},{_tmp2E7,_tmp2E7,
_tmp2E7 + 20},{_tmp2E8,_tmp2E8,_tmp2E8 + 21},{_tmp2E9,_tmp2E9,_tmp2E9 + 13},{
_tmp2EA,_tmp2EA,_tmp2EA + 15},{_tmp2EB,_tmp2EB,_tmp2EB + 17},{_tmp2EC,_tmp2EC,
_tmp2EC + 19},{_tmp2ED,_tmp2ED,_tmp2ED + 12},{_tmp2EE,_tmp2EE,_tmp2EE + 17},{
_tmp2EF,_tmp2EF,_tmp2EF + 23},{_tmp2F0,_tmp2F0,_tmp2F0 + 24},{_tmp2F1,_tmp2F1,
_tmp2F1 + 15},{_tmp2F2,_tmp2F2,_tmp2F2 + 11},{_tmp2F3,_tmp2F3,_tmp2F3 + 15},{
_tmp2F4,_tmp2F4,_tmp2F4 + 10},{_tmp2F5,_tmp2F5,_tmp2F5 + 15},{_tmp2F6,_tmp2F6,
_tmp2F6 + 25},{_tmp2F7,_tmp2F7,_tmp2F7 + 5},{_tmp2F8,_tmp2F8,_tmp2F8 + 15},{_tmp2F9,
_tmp2F9,_tmp2F9 + 15},{_tmp2FA,_tmp2FA,_tmp2FA + 11},{_tmp2FB,_tmp2FB,_tmp2FB + 22},{
_tmp2FC,_tmp2FC,_tmp2FC + 26},{_tmp2FD,_tmp2FD,_tmp2FD + 16},{_tmp2FE,_tmp2FE,
_tmp2FE + 16},{_tmp2FF,_tmp2FF,_tmp2FF + 24},{_tmp300,_tmp300,_tmp300 + 25},{
_tmp301,_tmp301,_tmp301 + 21},{_tmp302,_tmp302,_tmp302 + 22},{_tmp303,_tmp303,
_tmp303 + 16},{_tmp304,_tmp304,_tmp304 + 19},{_tmp305,_tmp305,_tmp305 + 25},{
_tmp306,_tmp306,_tmp306 + 35},{_tmp307,_tmp307,_tmp307 + 23},{_tmp308,_tmp308,
_tmp308 + 24},{_tmp309,_tmp309,_tmp309 + 18},{_tmp30A,_tmp30A,_tmp30A + 17},{
_tmp30B,_tmp30B,_tmp30B + 18},{_tmp30C,_tmp30C,_tmp30C + 17},{_tmp30D,_tmp30D,
_tmp30D + 18},{_tmp30E,_tmp30E,_tmp30E + 12},{_tmp30F,_tmp30F,_tmp30F + 11},{
_tmp310,_tmp310,_tmp310 + 23},{_tmp311,_tmp311,_tmp311 + 18},{_tmp312,_tmp312,
_tmp312 + 30},{_tmp313,_tmp313,_tmp313 + 8},{_tmp314,_tmp314,_tmp314 + 12},{_tmp315,
_tmp315,_tmp315 + 23},{_tmp316,_tmp316,_tmp316 + 14},{_tmp317,_tmp317,_tmp317 + 18},{
_tmp318,_tmp318,_tmp318 + 8},{_tmp319,_tmp319,_tmp319 + 11},{_tmp31A,_tmp31A,
_tmp31A + 20},{_tmp31B,_tmp31B,_tmp31B + 9},{_tmp31C,_tmp31C,_tmp31C + 16},{_tmp31D,
_tmp31D,_tmp31D + 19},{_tmp31E,_tmp31E,_tmp31E + 10},{_tmp31F,_tmp31F,_tmp31F + 16},{
_tmp320,_tmp320,_tmp320 + 11},{_tmp321,_tmp321,_tmp321 + 14},{_tmp322,_tmp322,
_tmp322 + 11},{_tmp323,_tmp323,_tmp323 + 15},{_tmp324,_tmp324,_tmp324 + 22},{
_tmp325,_tmp325,_tmp325 + 16},{_tmp326,_tmp326,_tmp326 + 17},{_tmp327,_tmp327,
_tmp327 + 12},{_tmp328,_tmp328,_tmp328 + 18},{_tmp329,_tmp329,_tmp329 + 17},{
_tmp32A,_tmp32A,_tmp32A + 12},{_tmp32B,_tmp32B,_tmp32B + 16},{_tmp32C,_tmp32C,
_tmp32C + 11},{_tmp32D,_tmp32D,_tmp32D + 10},{_tmp32E,_tmp32E,_tmp32E + 14},{
_tmp32F,_tmp32F,_tmp32F + 15},{_tmp330,_tmp330,_tmp330 + 20},{_tmp331,_tmp331,
_tmp331 + 27},{_tmp332,_tmp332,_tmp332 + 10},{_tmp333,_tmp333,_tmp333 + 18},{
_tmp334,_tmp334,_tmp334 + 21},{_tmp335,_tmp335,_tmp335 + 19},{_tmp336,_tmp336,
_tmp336 + 16},{_tmp337,_tmp337,_tmp337 + 20},{_tmp338,_tmp338,_tmp338 + 15},{
_tmp339,_tmp339,_tmp339 + 20},{_tmp33A,_tmp33A,_tmp33A + 15},{_tmp33B,_tmp33B,
_tmp33B + 12},{_tmp33C,_tmp33C,_tmp33C + 20},{_tmp33D,_tmp33D,_tmp33D + 19},{
_tmp33E,_tmp33E,_tmp33E + 20},{_tmp33F,_tmp33F,_tmp33F + 21},{_tmp340,_tmp340,
_tmp340 + 21},{_tmp341,_tmp341,_tmp341 + 12},{_tmp342,_tmp342,_tmp342 + 17},{
_tmp343,_tmp343,_tmp343 + 19},{_tmp344,_tmp344,_tmp344 + 14},{_tmp345,_tmp345,
_tmp345 + 17},{_tmp346,_tmp346,_tmp346 + 23},{_tmp347,_tmp347,_tmp347 + 13},{
_tmp348,_tmp348,_tmp348 + 14},{_tmp349,_tmp349,_tmp349 + 16},{_tmp34A,_tmp34A,
_tmp34A + 16},{_tmp34B,_tmp34B,_tmp34B + 8},{_tmp34C,_tmp34C,_tmp34C + 19},{_tmp34D,
_tmp34D,_tmp34D + 20},{_tmp34E,_tmp34E,_tmp34E + 14},{_tmp34F,_tmp34F,_tmp34F + 19},{
_tmp350,_tmp350,_tmp350 + 20},{_tmp351,_tmp351,_tmp351 + 11},{_tmp352,_tmp352,
_tmp352 + 22},{_tmp353,_tmp353,_tmp353 + 20},{_tmp354,_tmp354,_tmp354 + 23},{
_tmp355,_tmp355,_tmp355 + 20},{_tmp356,_tmp356,_tmp356 + 22},{_tmp357,_tmp357,
_tmp357 + 23},{_tmp358,_tmp358,_tmp358 + 24},{_tmp359,_tmp359,_tmp359 + 24},{
_tmp35A,_tmp35A,_tmp35A + 15},{_tmp35B,_tmp35B,_tmp35B + 20},{_tmp35C,_tmp35C,
_tmp35C + 22},{_tmp35D,_tmp35D,_tmp35D + 17},{_tmp35E,_tmp35E,_tmp35E + 20},{
_tmp35F,_tmp35F,_tmp35F + 26},{_tmp360,_tmp360,_tmp360 + 16},{_tmp361,_tmp361,
_tmp361 + 17},{_tmp362,_tmp362,_tmp362 + 15},{_tmp363,_tmp363,_tmp363 + 19},{
_tmp364,_tmp364,_tmp364 + 19},{_tmp365,_tmp365,_tmp365 + 25},{_tmp366,_tmp366,
_tmp366 + 26},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 20},{_tmp369,
_tmp369,_tmp369 + 17},{_tmp36A,_tmp36A,_tmp36A + 18},{_tmp36B,_tmp36B,_tmp36B + 11},{
_tmp36C,_tmp36C,_tmp36C + 12}};static short Cyc_yyr1[487]={0,129,130,130,130,130,
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
char _tmp43C[8]="stdcall";static char _tmp43D[6]="cdecl";static char _tmp43E[9]="fastcall";
static char _tmp43F[9]="noreturn";static char _tmp440[6]="const";static char _tmp441[8]="aligned";
static char _tmp442[7]="packed";static char _tmp443[7]="shared";static char _tmp444[7]="unused";
static char _tmp445[5]="weak";static char _tmp446[10]="dllimport";static char _tmp447[
10]="dllexport";static char _tmp448[23]="no_instrument_function";static char _tmp449[
12]="constructor";static char _tmp44A[11]="destructor";static char _tmp44B[22]="no_check_memory_usage";
static char _tmp44C[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmpA2B=10000;short*_tmpA2C=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmpA2B));{unsigned int _tmpA2D=
_tmpA2B;unsigned int i;for(i=0;i < _tmpA2D;i ++){_tmpA2C[i]=(short)0;}}_tmpA2C;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmpA28=10000;void**_tmpA29=(
void**)_cycalloc(_check_times(sizeof(void*),_tmpA28));{unsigned int _tmpA2A=
_tmpA28;unsigned int i;for(i=0;i < _tmpA2A;i ++){_tmpA29[i]=Cyc_yylval;}}_tmpA29;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmpA25=
10000;struct Cyc_Yyltype*_tmpA26=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmpA25));{unsigned int _tmpA27=_tmpA25;
unsigned int i;for(i=0;i < _tmpA27;i ++){_tmpA26[i]=Cyc_yynewloc();}}_tmpA26;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp36D="parser stack overflow";_tag_arr(_tmp36D,sizeof(char),_get_zero_arr_size(
_tmp36D,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
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
_handler_cons _tmp36E;_push_handler(& _tmp36E);{int _tmp370=0;if(setjmp(_tmp36E.handler))
_tmp370=1;if(!_tmp370){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp36F=(void*)_exn_thrown;void*_tmp372=_tmp36F;_LL1E5: if(*((void**)_tmp372)
!= Cyc_Core_Failure)goto _LL1E7;_LL1E6: x=0;goto _LL1E4;_LL1E7:;_LL1E8:(void)_throw(
_tmp372);_LL1E4:;}}}{struct _handler_cons _tmp373;_push_handler(& _tmp373);{int
_tmp375=0;if(setjmp(_tmp373.handler))_tmp375=1;if(!_tmp375){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp374=(void*)_exn_thrown;void*_tmp377=_tmp374;_LL1EA: if(*((void**)_tmp377)
!= Cyc_Core_Failure)goto _LL1EC;_LL1EB: y=0;goto _LL1E9;_LL1EC:;_LL1ED:(void)_throw(
_tmp377);_LL1E9:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp378=
_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_DeclList_tok_struct _tmp379;
_tmp379.tag=Cyc_DeclList_tok;_tmp379.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp379;});_tmp378;});break;}
case 3: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp37A=_cycalloc(
sizeof(*_tmp37A));_tmp37A[0]=({struct Cyc_DeclList_tok_struct _tmp37B;_tmp37B.tag=
Cyc_DeclList_tok;_tmp37B.f1=({struct Cyc_List_List*_tmp37C=_cycalloc(sizeof(*
_tmp37C));_tmp37C->hd=({struct Cyc_Absyn_Decl*_tmp37D=_cycalloc(sizeof(*_tmp37D));
_tmp37D->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp37E=_cycalloc(
sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_Absyn_Using_d_struct _tmp37F;_tmp37F.tag=
9;_tmp37F.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp37F.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp37F;});_tmp37E;}));
_tmp37D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp37D;});_tmp37C->tl=0;_tmp37C;});_tmp37B;});
_tmp37A;});Cyc_Lex_leave_using();break;case 4: _LL1EE: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380[0]=({struct Cyc_DeclList_tok_struct
_tmp381;_tmp381.tag=Cyc_DeclList_tok;_tmp381.f1=({struct Cyc_List_List*_tmp382=
_cycalloc(sizeof(*_tmp382));_tmp382->hd=({struct Cyc_Absyn_Decl*_tmp383=_cycalloc(
sizeof(*_tmp383));_tmp383->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384[0]=({struct Cyc_Absyn_Using_d_struct
_tmp385;_tmp385.tag=9;_tmp385.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp385.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp385;});_tmp384;}));
_tmp383->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp383;});_tmp382->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp382;});_tmp381;});
_tmp380;});break;case 5: _LL1EF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_DeclList_tok_struct
_tmp387;_tmp387.tag=Cyc_DeclList_tok;_tmp387.f1=({struct Cyc_List_List*_tmp388=
_cycalloc(sizeof(*_tmp388));_tmp388->hd=({struct Cyc_Absyn_Decl*_tmp389=_cycalloc(
sizeof(*_tmp389));_tmp389->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp38B;_tmp38B.tag=8;_tmp38B.f1=({struct _tagged_arr*_tmp38C=_cycalloc(sizeof(*
_tmp38C));_tmp38C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp38C;});_tmp38B.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp38B;});_tmp38A;}));
_tmp389->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp389;});_tmp388->tl=0;_tmp388;});_tmp387;});
_tmp386;});Cyc_Lex_leave_namespace();break;case 6: _LL1F0: {struct _tagged_arr
nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp38D;
_push_handler(& _tmp38D);{int _tmp38F=0;if(setjmp(_tmp38D.handler))_tmp38F=1;if(!
_tmp38F){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp38E=(void*)_exn_thrown;void*
_tmp391=_tmp38E;_LL1F3: if(*((void**)_tmp391)!= Cyc_Core_Failure)goto _LL1F5;
_LL1F4: nspace=({const char*_tmp392="";_tag_arr(_tmp392,sizeof(char),
_get_zero_arr_size(_tmp392,1));});goto _LL1F2;_LL1F5:;_LL1F6:(void)_throw(_tmp391);
_LL1F2:;}}}{struct _handler_cons _tmp393;_push_handler(& _tmp393);{int _tmp395=0;if(
setjmp(_tmp393.handler))_tmp395=1;if(!_tmp395){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp394=(void*)_exn_thrown;void*_tmp397=_tmp394;_LL1F8: if(*((void**)_tmp397)
!= Cyc_Core_Failure)goto _LL1FA;_LL1F9: x=0;goto _LL1F7;_LL1FA:;_LL1FB:(void)_throw(
_tmp397);_LL1F7:;}}}{struct _handler_cons _tmp398;_push_handler(& _tmp398);{int
_tmp39A=0;if(setjmp(_tmp398.handler))_tmp39A=1;if(!_tmp39A){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp399=(void*)_exn_thrown;void*_tmp39C=_tmp399;_LL1FD: if(*((void**)_tmp39C)
!= Cyc_Core_Failure)goto _LL1FF;_LL1FE: y=0;goto _LL1FC;_LL1FF:;_LL200:(void)_throw(
_tmp39C);_LL1FC:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp39D=
_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_DeclList_tok_struct _tmp39E;
_tmp39E.tag=Cyc_DeclList_tok;_tmp39E.f1=({struct Cyc_List_List*_tmp39F=_cycalloc(
sizeof(*_tmp39F));_tmp39F->hd=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*
_tmp3A0));_tmp3A0->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A1=
_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A2;_tmp3A2.tag=8;_tmp3A2.f1=({struct _tagged_arr*_tmp3A3=_cycalloc(sizeof(*
_tmp3A3));_tmp3A3[0]=nspace;_tmp3A3;});_tmp3A2.f2=x;_tmp3A2;});_tmp3A1;}));
_tmp3A0->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3A0;});_tmp39F->tl=y;_tmp39F;});_tmp39E;});
_tmp39D;});break;}case 7: _LL1F1: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3A4="C";
_tag_arr(_tmp3A4,sizeof(char),_get_zero_arr_size(_tmp3A4,2));}))== 0)yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5[0]=({
struct Cyc_DeclList_tok_struct _tmp3A6;_tmp3A6.tag=Cyc_DeclList_tok;_tmp3A6.f1=({
struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->hd=({struct Cyc_Absyn_Decl*
_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3AA;_tmp3AA.tag=10;_tmp3AA.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3AA;});_tmp3A9;}));
_tmp3A8->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3A8;});_tmp3A7->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A7;});_tmp3A6;});
_tmp3A5;});else{if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3AB="C include";
_tag_arr(_tmp3AB,sizeof(char),_get_zero_arr_size(_tmp3AB,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3AC="expecting \"C\" or \"C include\"";_tag_arr(_tmp3AC,sizeof(
char),_get_zero_arr_size(_tmp3AC,29));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({
struct Cyc_DeclList_tok_struct _tmp3AE;_tmp3AE.tag=Cyc_DeclList_tok;_tmp3AE.f1=({
struct Cyc_List_List*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->hd=({struct Cyc_Absyn_Decl*
_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3B2;_tmp3B2.tag=11;_tmp3B2.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B2.f2=0;_tmp3B2;});
_tmp3B1;}));_tmp3B0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B0;});
_tmp3AF->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3AF;});_tmp3AE;});_tmp3AD;});}break;case 8: _LL201: if(Cyc_strcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3B3="C include";_tag_arr(_tmp3B3,sizeof(char),
_get_zero_arr_size(_tmp3B3,10));}))!= 0)Cyc_Parse_err(({const char*_tmp3B4="expecting \"C include\"";
_tag_arr(_tmp3B4,sizeof(char),_get_zero_arr_size(_tmp3B4,22));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));
_tmp3B5[0]=({struct Cyc_DeclList_tok_struct _tmp3B6;_tmp3B6.tag=Cyc_DeclList_tok;
_tmp3B6.f1=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->hd=({
struct Cyc_Absyn_Decl*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->r=(void*)((
void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3B9=_cycalloc(sizeof(*
_tmp3B9));_tmp3B9[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3BA;_tmp3BA.tag=
11;_tmp3BA.f1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3BA.f2=exs;_tmp3BA;});_tmp3B9;}));_tmp3B8->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B8;});
_tmp3B7->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B7;});_tmp3B6;});_tmp3B5;});break;}case 9: _LL202: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({
struct Cyc_DeclList_tok_struct _tmp3BC;_tmp3BC.tag=Cyc_DeclList_tok;_tmp3BC.f1=({
struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=({struct Cyc_Absyn_Decl*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->r=(void*)((void*)0);_tmp3BE->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3BE;});_tmp3BD->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BD;});_tmp3BC;});
_tmp3BB;});break;case 10: _LL203: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_DeclList_tok_struct
_tmp3C0;_tmp3C0.tag=Cyc_DeclList_tok;_tmp3C0.f1=({struct Cyc_List_List*_tmp3C1=
_cycalloc(sizeof(*_tmp3C1));_tmp3C1->hd=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(
sizeof(*_tmp3C2));_tmp3C2->r=(void*)((void*)1);_tmp3C2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C2;});
_tmp3C1->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C1;});_tmp3C0;});_tmp3BF;});break;case 11: _LL204: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({
struct Cyc_DeclList_tok_struct _tmp3C4;_tmp3C4.tag=Cyc_DeclList_tok;_tmp3C4.f1=0;
_tmp3C4;});_tmp3C3;});break;case 12: _LL205: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 13: _LL206: yyval=(
void*)({struct Cyc_YY7_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5[0]=({
struct Cyc_YY7_struct _tmp3C6;_tmp3C6.tag=Cyc_YY7;_tmp3C6.f1=0;_tmp3C6;});_tmp3C5;});
break;case 14: _LL207: yyval=(void*)({struct Cyc_YY7_struct*_tmp3C7=_cycalloc(
sizeof(*_tmp3C7));_tmp3C7[0]=({struct Cyc_YY7_struct _tmp3C8;_tmp3C8.tag=Cyc_YY7;
_tmp3C8.f1=({struct Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->hd=({
struct _tuple18*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CA->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3CA->f3=0;_tmp3CA;});_tmp3C9->tl=0;_tmp3C9;});_tmp3C8;});_tmp3C7;});break;
case 15: _LL208: yyval=(void*)({struct Cyc_YY7_struct*_tmp3CB=_cycalloc(sizeof(*
_tmp3CB));_tmp3CB[0]=({struct Cyc_YY7_struct _tmp3CC;_tmp3CC.tag=Cyc_YY7;_tmp3CC.f1=({
struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->hd=({struct
_tuple18*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3CE->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3CE->f3=0;_tmp3CE;});_tmp3CD->tl=Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CD;});_tmp3CC;});
_tmp3CB;});break;case 16: _LL209: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_DeclList_tok_struct
_tmp3D0;_tmp3D0.tag=Cyc_DeclList_tok;_tmp3D0.f1=({struct Cyc_List_List*_tmp3D1=
_cycalloc(sizeof(*_tmp3D1));_tmp3D1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3D3;_tmp3D3.tag=1;_tmp3D3.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D3;});_tmp3D2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D1->tl=0;
_tmp3D1;});_tmp3D0;});_tmp3CF;});break;case 17: _LL20A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL20B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));
_tmp3D4[0]=({struct Cyc_DeclList_tok_struct _tmp3D5;_tmp3D5.tag=Cyc_DeclList_tok;
_tmp3D5.f1=0;_tmp3D5;});_tmp3D4;});break;case 19: _LL20C: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D7;_tmp3D7.tag=Cyc_FnDecl_tok;_tmp3D7.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D7;});
_tmp3D6;});break;case 20: _LL20D: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_FnDecl_tok_struct
_tmp3D9;_tmp3D9.tag=Cyc_FnDecl_tok;_tmp3D9.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3DA;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D9;});
_tmp3D8;});break;case 21: _LL20E: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DC;_tmp3DC.tag=Cyc_FnDecl_tok;_tmp3DC.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DC;});
_tmp3DB;});break;case 22: _LL20F: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DE;_tmp3DE.tag=Cyc_FnDecl_tok;_tmp3DE.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3DF;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DE;});
_tmp3DD;});break;case 23: _LL210: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E1;_tmp3E1.tag=Cyc_FnDecl_tok;_tmp3E1.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E2;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E1;});
_tmp3E0;});break;case 24: _LL211: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E4;_tmp3E4.tag=Cyc_FnDecl_tok;_tmp3E4.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E5;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E4;});
_tmp3E3;});break;case 25: _LL212: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL213: Cyc_Lex_leave_using();
break;case 27: _LL214: Cyc_Lex_enter_namespace(({struct _tagged_arr*_tmp3E6=
_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E6;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL215: Cyc_Lex_leave_namespace();
break;case 29: _LL216: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3E7=
_cycalloc(sizeof(*_tmp3E7));_tmp3E7[0]=({struct Cyc_DeclList_tok_struct _tmp3E8;
_tmp3E8.tag=Cyc_DeclList_tok;_tmp3E8.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3E8;});
_tmp3E7;});break;case 30: _LL217: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=({struct Cyc_DeclList_tok_struct
_tmp3EA;_tmp3EA.tag=Cyc_DeclList_tok;_tmp3EA.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EA;});
_tmp3E9;});break;case 31: _LL218: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({struct Cyc_DeclList_tok_struct
_tmp3EC;_tmp3EC.tag=Cyc_DeclList_tok;_tmp3EC.f1=({struct Cyc_List_List*_tmp3ED=
_cycalloc(sizeof(*_tmp3ED));_tmp3ED->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3ED->tl=0;
_tmp3ED;});_tmp3EC;});_tmp3EB;});break;case 32: _LL219: {struct Cyc_List_List*
_tmp3EE=0;{struct Cyc_List_List*_tmp3EF=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3EF != 0;
_tmp3EF=_tmp3EF->tl){struct _tagged_arr*_tmp3F0=(struct _tagged_arr*)_tmp3EF->hd;
struct _tuple1*_tmp3F1=({struct _tuple1*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));
_tmp3F4->f1=Cyc_Absyn_rel_ns_null;_tmp3F4->f2=_tmp3F0;_tmp3F4;});struct Cyc_Absyn_Vardecl*
_tmp3F2=Cyc_Absyn_new_vardecl(_tmp3F1,Cyc_Absyn_wildtyp(0),0);_tmp3EE=({struct
Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->hd=_tmp3F2;_tmp3F3->tl=
_tmp3EE;_tmp3F3;});}}_tmp3EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3EE);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3F5=
_cycalloc(sizeof(*_tmp3F5));_tmp3F5[0]=({struct Cyc_DeclList_tok_struct _tmp3F6;
_tmp3F6.tag=Cyc_DeclList_tok;_tmp3F6.f1=({struct Cyc_List_List*_tmp3F7=_cycalloc(
sizeof(*_tmp3F7));_tmp3F7->hd=Cyc_Absyn_letv_decl(_tmp3EE,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F7->tl=0;
_tmp3F7;});_tmp3F6;});_tmp3F5;});break;}case 33: _LL21A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL21B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));
_tmp3F8[0]=({struct Cyc_DeclList_tok_struct _tmp3F9;_tmp3F9.tag=Cyc_DeclList_tok;
_tmp3F9.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3F9;});_tmp3F8;});
break;case 35: _LL21C: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA[0]=({struct Cyc_DeclSpec_tok_struct _tmp3FB;
_tmp3FB.tag=Cyc_DeclSpec_tok;_tmp3FB.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->sc=({struct Cyc_Core_Opt*_tmp3FD=
_cycalloc(sizeof(*_tmp3FD));_tmp3FD->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FD;});_tmp3FC->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FC->type_specs=0;_tmp3FC->is_inline=0;_tmp3FC->attributes=
0;_tmp3FC;});_tmp3FB;});_tmp3FA;});break;case 36: _LL21D: if((Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*
_tmp3FE[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp3FF="Only one storage class is allowed in a declaration";_tag_arr(_tmp3FF,
sizeof(char),_get_zero_arr_size(_tmp3FF,51));}),_tag_arr(_tmp3FE,sizeof(void*),0));});
yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp400=_cycalloc(sizeof(*_tmp400));
_tmp400[0]=({struct Cyc_DeclSpec_tok_struct _tmp401;_tmp401.tag=Cyc_DeclSpec_tok;
_tmp401.f1=({struct Cyc_Parse_Declaration_spec*_tmp402=_cycalloc(sizeof(*_tmp402));
_tmp402->sc=({struct Cyc_Core_Opt*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp403;});_tmp402->tq=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp402->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp402->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp402->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp402;});
_tmp401;});_tmp400;});break;case 37: _LL21E: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404[0]=({struct Cyc_DeclSpec_tok_struct
_tmp405;_tmp405.tag=Cyc_DeclSpec_tok;_tmp405.f1=({struct Cyc_Parse_Declaration_spec*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->sc=0;_tmp406->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp406->type_specs=({struct Cyc_List_List*_tmp407=
_cycalloc(sizeof(*_tmp407));_tmp407->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp407->tl=0;_tmp407;});
_tmp406->is_inline=0;_tmp406->attributes=0;_tmp406;});_tmp405;});_tmp404;});
break;case 38: _LL21F: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp408=
_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_DeclSpec_tok_struct _tmp409;
_tmp409.tag=Cyc_DeclSpec_tok;_tmp409.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40A->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40A->type_specs=({
struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40B->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40B;});
_tmp40A->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp40A->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40A;});
_tmp409;});_tmp408;});break;case 39: _LL220: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_DeclSpec_tok_struct
_tmp40D;_tmp40D.tag=Cyc_DeclSpec_tok;_tmp40D.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->sc=0;_tmp40E->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40E->type_specs=0;
_tmp40E->is_inline=0;_tmp40E->attributes=0;_tmp40E;});_tmp40D;});_tmp40C;});
break;case 40: _LL221: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp40F=
_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_DeclSpec_tok_struct _tmp410;
_tmp410.tag=Cyc_DeclSpec_tok;_tmp410.f1=({struct Cyc_Parse_Declaration_spec*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp411->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp411->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp411->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp411->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp411;});_tmp410;});_tmp40F;});break;case 41: _LL222: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_DeclSpec_tok_struct
_tmp413;_tmp413.tag=Cyc_DeclSpec_tok;_tmp413.f1=({struct Cyc_Parse_Declaration_spec*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->sc=0;_tmp414->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp414->type_specs=0;_tmp414->is_inline=1;_tmp414->attributes=
0;_tmp414;});_tmp413;});_tmp412;});break;case 42: _LL223: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_DeclSpec_tok_struct
_tmp416;_tmp416.tag=Cyc_DeclSpec_tok;_tmp416.f1=({struct Cyc_Parse_Declaration_spec*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp417->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp417->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp417->is_inline=1;_tmp417->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp417;});
_tmp416;});_tmp415;});break;case 43: _LL224: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_DeclSpec_tok_struct
_tmp419;_tmp419.tag=Cyc_DeclSpec_tok;_tmp419.f1=({struct Cyc_Parse_Declaration_spec*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->sc=0;_tmp41A->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp41A->type_specs=0;_tmp41A->is_inline=0;_tmp41A->attributes=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp41A;});_tmp419;});_tmp418;});break;case 44: _LL225: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({
struct Cyc_DeclSpec_tok_struct _tmp41C;_tmp41C.tag=Cyc_DeclSpec_tok;_tmp41C.f1=({
struct Cyc_Parse_Declaration_spec*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;
_tmp41D->tq=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq;_tmp41D->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp41D->is_inline=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;
_tmp41D->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp41D;});
_tmp41C;});_tmp41B;});break;case 45: _LL226: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E[0]=({struct Cyc_StorageClass_tok_struct
_tmp41F;_tmp41F.tag=Cyc_StorageClass_tok;_tmp41F.f1=(void*)((void*)4);_tmp41F;});
_tmp41E;});break;case 46: _LL227: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420[0]=({struct Cyc_StorageClass_tok_struct
_tmp421;_tmp421.tag=Cyc_StorageClass_tok;_tmp421.f1=(void*)((void*)5);_tmp421;});
_tmp420;});break;case 47: _LL228: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_StorageClass_tok_struct
_tmp423;_tmp423.tag=Cyc_StorageClass_tok;_tmp423.f1=(void*)((void*)3);_tmp423;});
_tmp422;});break;case 48: _LL229: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424[0]=({struct Cyc_StorageClass_tok_struct
_tmp425;_tmp425.tag=Cyc_StorageClass_tok;_tmp425.f1=(void*)((void*)1);_tmp425;});
_tmp424;});break;case 49: _LL22A: if(Cyc_strcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp426="C";
_tag_arr(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp427="only extern or extern \"C\" is allowed";_tag_arr(_tmp427,
sizeof(char),_get_zero_arr_size(_tmp427,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428[0]=({
struct Cyc_StorageClass_tok_struct _tmp429;_tmp429.tag=Cyc_StorageClass_tok;
_tmp429.f1=(void*)((void*)2);_tmp429;});_tmp428;});break;case 50: _LL22B: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));
_tmp42A[0]=({struct Cyc_StorageClass_tok_struct _tmp42B;_tmp42B.tag=Cyc_StorageClass_tok;
_tmp42B.f1=(void*)((void*)0);_tmp42B;});_tmp42A;});break;case 51: _LL22C: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C[0]=({struct Cyc_StorageClass_tok_struct _tmp42D;_tmp42D.tag=Cyc_StorageClass_tok;
_tmp42D.f1=(void*)((void*)6);_tmp42D;});_tmp42C;});break;case 52: _LL22D: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));
_tmp42E[0]=({struct Cyc_AttributeList_tok_struct _tmp42F;_tmp42F.tag=Cyc_AttributeList_tok;
_tmp42F.f1=0;_tmp42F;});_tmp42E;});break;case 53: _LL22E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54: _LL22F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 55: _LL230:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp430=_cycalloc(sizeof(*
_tmp430));_tmp430[0]=({struct Cyc_AttributeList_tok_struct _tmp431;_tmp431.tag=Cyc_AttributeList_tok;
_tmp431.f1=({struct Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp432->tl=0;_tmp432;});_tmp431;});_tmp430;});break;case 56:
_LL231: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp433=_cycalloc(
sizeof(*_tmp433));_tmp433[0]=({struct Cyc_AttributeList_tok_struct _tmp434;_tmp434.tag=
Cyc_AttributeList_tok;_tmp434.f1=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*
_tmp435));_tmp435->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp435->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp435;});_tmp434;});
_tmp433;});break;case 57: _LL232: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple19 att_map[17]={{{_tmp43C,_tmp43C,_tmp43C + 8},(
void*)0},{{_tmp43D,_tmp43D,_tmp43D + 6},(void*)1},{{_tmp43E,_tmp43E,_tmp43E + 9},(
void*)2},{{_tmp43F,_tmp43F,_tmp43F + 9},(void*)3},{{_tmp440,_tmp440,_tmp440 + 6},(
void*)4},{{_tmp441,_tmp441,_tmp441 + 8},(void*)& att_aligned},{{_tmp442,_tmp442,
_tmp442 + 7},(void*)5},{{_tmp443,_tmp443,_tmp443 + 7},(void*)7},{{_tmp444,_tmp444,
_tmp444 + 7},(void*)8},{{_tmp445,_tmp445,_tmp445 + 5},(void*)9},{{_tmp446,_tmp446,
_tmp446 + 10},(void*)10},{{_tmp447,_tmp447,_tmp447 + 10},(void*)11},{{_tmp448,
_tmp448,_tmp448 + 23},(void*)12},{{_tmp449,_tmp449,_tmp449 + 12},(void*)13},{{
_tmp44A,_tmp44A,_tmp44A + 11},(void*)14},{{_tmp44B,_tmp44B,_tmp44B + 22},(void*)15},{{
_tmp44C,_tmp44C,_tmp44C + 5},(void*)16}};struct _tagged_arr _tmp436=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_arr_size(
_tmp436,sizeof(char))> 4  && *((const char*)_check_unknown_subscript(_tmp436,
sizeof(char),0))== '_') && *((const char*)_check_unknown_subscript(_tmp436,
sizeof(char),1))== '_') && *((const char*)_check_unknown_subscript(_tmp436,
sizeof(char),(int)(_get_arr_size(_tmp436,sizeof(char))- 2)))== '_') && *((const
char*)_check_unknown_subscript(_tmp436,sizeof(char),(int)(_get_arr_size(_tmp436,
sizeof(char))- 3)))== '_')_tmp436=(struct _tagged_arr)Cyc_substring((struct
_tagged_arr)_tmp436,2,_get_arr_size(_tmp436,sizeof(char))- 5);{int i=0;for(0;i < 
17;++ i){if(Cyc_strcmp((struct _tagged_arr)_tmp436,(struct _tagged_arr)(att_map[i]).f1)
== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp437=_cycalloc(sizeof(*
_tmp437));_tmp437[0]=({struct Cyc_Attribute_tok_struct _tmp438;_tmp438.tag=Cyc_Attribute_tok;
_tmp438.f1=(void*)(att_map[i]).f2;_tmp438;});_tmp437;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp439="unrecognized attribute";_tag_arr(_tmp439,sizeof(char),
_get_zero_arr_size(_tmp439,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({
struct Cyc_Attribute_tok_struct _tmp43B;_tmp43B.tag=Cyc_Attribute_tok;_tmp43B.f1=(
void*)((void*)1);_tmp43B;});_tmp43A;});}break;}}case 58: _LL233: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E[0]=({
struct Cyc_Attribute_tok_struct _tmp44F;_tmp44F.tag=Cyc_Attribute_tok;_tmp44F.f1=(
void*)((void*)4);_tmp44F;});_tmp44E;});break;case 59: _LL234: {struct _tagged_arr
_tmp450=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp452;int _tmp453;struct _tuple13*_tmp451=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp452=*_tmp451;
_tmp453=_tmp452.f2;{void*a;if(Cyc_zstrcmp((struct _tagged_arr)_tmp450,({const char*
_tmp454="regparm";_tag_arr(_tmp454,sizeof(char),_get_zero_arr_size(_tmp454,8));}))
== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp450,({const char*_tmp455="__regparm__";
_tag_arr(_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,12));}))== 0){if(
_tmp453 < 0  || _tmp453 > 3)Cyc_Parse_err(({const char*_tmp456="regparm requires value between 0 and 3";
_tag_arr(_tmp456,sizeof(char),_get_zero_arr_size(_tmp456,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp457=_cycalloc_atomic(sizeof(*_tmp457));
_tmp457[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp458;_tmp458.tag=0;_tmp458.f1=
_tmp453;_tmp458;});_tmp457;});}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp450,({
const char*_tmp459="aligned";_tag_arr(_tmp459,sizeof(char),_get_zero_arr_size(
_tmp459,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp450,({const char*
_tmp45A="__aligned__";_tag_arr(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,
12));}))== 0){if(_tmp453 < 0)Cyc_Parse_err(({const char*_tmp45B="aligned requires positive power of two";
_tag_arr(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp453;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp45C="aligned requires positive power of two";
_tag_arr(_tmp45C,sizeof(char),_get_zero_arr_size(_tmp45C,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp45D=_cycalloc_atomic(sizeof(*_tmp45D));
_tmp45D[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp45E;_tmp45E.tag=1;_tmp45E.f1=
_tmp453;_tmp45E;});_tmp45D;});}}else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp450,({
const char*_tmp45F="initializes";_tag_arr(_tmp45F,sizeof(char),_get_zero_arr_size(
_tmp45F,12));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)_tmp450,({const char*
_tmp460="__initializes__";_tag_arr(_tmp460,sizeof(char),_get_zero_arr_size(
_tmp460,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*_tmp461=
_cycalloc_atomic(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp462;_tmp462.tag=4;_tmp462.f1=_tmp453;_tmp462;});_tmp461;});else{Cyc_Parse_err(({
const char*_tmp463="unrecognized attribute";_tag_arr(_tmp463,sizeof(char),
_get_zero_arr_size(_tmp463,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp464=_cycalloc(sizeof(*_tmp464));
_tmp464[0]=({struct Cyc_Attribute_tok_struct _tmp465;_tmp465.tag=Cyc_Attribute_tok;
_tmp465.f1=(void*)a;_tmp465;});_tmp464;});break;}}case 60: _LL235: {struct
_tagged_arr _tmp466=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp467=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp466,({const char*_tmp468="section";_tag_arr(_tmp468,sizeof(
char),_get_zero_arr_size(_tmp468,8));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)
_tmp466,({const char*_tmp469="__section__";_tag_arr(_tmp469,sizeof(char),
_get_zero_arr_size(_tmp469,12));}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A[0]=({struct Cyc_Absyn_Section_att_struct
_tmp46B;_tmp46B.tag=2;_tmp46B.f1=_tmp467;_tmp46B;});_tmp46A;});else{Cyc_Parse_err(({
const char*_tmp46C="unrecognized attribute";_tag_arr(_tmp46C,sizeof(char),
_get_zero_arr_size(_tmp46C,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_Attribute_tok_struct _tmp46E;_tmp46E.tag=Cyc_Attribute_tok;
_tmp46E.f1=(void*)a;_tmp46E;});_tmp46D;});break;}case 61: _LL236: {struct
_tagged_arr _tmp46F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp470=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _tagged_arr)_tmp46F,({const char*_tmp471="__mode__";_tag_arr(_tmp471,
sizeof(char),_get_zero_arr_size(_tmp471,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp473;_tmp473.tag=5;_tmp473.f1=_tmp470;_tmp473;});_tmp472;});else{Cyc_Parse_err(({
const char*_tmp474="unrecognized attribute";_tag_arr(_tmp474,sizeof(char),
_get_zero_arr_size(_tmp474,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp475=_cycalloc(sizeof(*_tmp475));
_tmp475[0]=({struct Cyc_Attribute_tok_struct _tmp476;_tmp476.tag=Cyc_Attribute_tok;
_tmp476.f1=(void*)a;_tmp476;});_tmp475;});break;}case 62: _LL237: {struct
_tagged_arr _tmp477=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp478=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp47A;struct
_tuple13 _tmp479=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp47A=_tmp479.f2;{int _tmp47C;struct _tuple13 _tmp47B=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47C=_tmp47B.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _tagged_arr)_tmp477,({const char*_tmp47D="format";
_tag_arr(_tmp47D,sizeof(char),_get_zero_arr_size(_tmp47D,7));}))== 0  || Cyc_zstrcmp((
struct _tagged_arr)_tmp477,({const char*_tmp47E="__format__";_tag_arr(_tmp47E,
sizeof(char),_get_zero_arr_size(_tmp47E,11));}))== 0){if(Cyc_zstrcmp((struct
_tagged_arr)_tmp478,({const char*_tmp47F="printf";_tag_arr(_tmp47F,sizeof(char),
_get_zero_arr_size(_tmp47F,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480[0]=({struct Cyc_Absyn_Format_att_struct
_tmp481;_tmp481.tag=3;_tmp481.f1=(void*)((void*)0);_tmp481.f2=_tmp47A;_tmp481.f3=
_tmp47C;_tmp481;});_tmp480;});else{if(Cyc_zstrcmp((struct _tagged_arr)_tmp478,({
const char*_tmp482="scanf";_tag_arr(_tmp482,sizeof(char),_get_zero_arr_size(
_tmp482,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp483=
_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_Format_att_struct
_tmp484;_tmp484.tag=3;_tmp484.f1=(void*)((void*)1);_tmp484.f2=_tmp47A;_tmp484.f3=
_tmp47C;_tmp484;});_tmp483;});else{Cyc_Parse_err(({const char*_tmp485="unrecognized format type";
_tag_arr(_tmp485,sizeof(char),_get_zero_arr_size(_tmp485,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp486="unrecognized attribute";_tag_arr(_tmp486,sizeof(char),
_get_zero_arr_size(_tmp486,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=({
struct Cyc_Attribute_tok_struct _tmp488;_tmp488.tag=Cyc_Attribute_tok;_tmp488.f1=(
void*)a;_tmp488;});_tmp487;});break;}}}case 63: _LL238: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL239: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp489=_cycalloc(sizeof(*_tmp489));
_tmp489[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp48A;_tmp48A.tag=Cyc_TypeSpecifier_tok;
_tmp48A.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp48C;_tmp48C.tag=16;_tmp48C.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48C.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp48C.f3=0;_tmp48C.f4=
0;_tmp48C;});_tmp48B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48A;});
_tmp489;});break;case 65: _LL23A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48E;_tmp48E.tag=Cyc_TypeSpecifier_tok;_tmp48E.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48E;});_tmp48D;});break;case 66: _LL23B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp490;_tmp490.tag=Cyc_TypeSpecifier_tok;
_tmp490.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp490;});
_tmp48F;});break;case 67: _LL23C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp492;_tmp492.tag=Cyc_TypeSpecifier_tok;_tmp492.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp493=_cycalloc(sizeof(*_tmp493));
_tmp493->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp493;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp492;});
_tmp491;});break;case 68: _LL23D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp495;_tmp495.tag=Cyc_TypeSpecifier_tok;_tmp495.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp495;});
_tmp494;});break;case 69: _LL23E: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp497;_tmp497.tag=Cyc_TypeSpecifier_tok;_tmp497.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Parse_Short_spec_struct
_tmp499;_tmp499.tag=2;_tmp499.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp499;});
_tmp498;}));_tmp497;});_tmp496;});break;case 70: _LL23F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49B;_tmp49B.tag=Cyc_TypeSpecifier_tok;_tmp49B.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49B;});
_tmp49A;});break;case 71: _LL240: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49D;_tmp49D.tag=Cyc_TypeSpecifier_tok;_tmp49D.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Parse_Long_spec_struct
_tmp49F;_tmp49F.tag=3;_tmp49F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49F;});
_tmp49E;}));_tmp49D;});_tmp49C;});break;case 72: _LL241: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A1;_tmp4A1.tag=Cyc_TypeSpecifier_tok;_tmp4A1.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A1;});
_tmp4A0;});break;case 73: _LL242: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_TypeSpecifier_tok;_tmp4A3.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A3;});
_tmp4A2;});break;case 74: _LL243: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A5;_tmp4A5.tag=Cyc_TypeSpecifier_tok;_tmp4A5.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp4A7;_tmp4A7.tag=0;_tmp4A7.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A7;});
_tmp4A6;}));_tmp4A5;});_tmp4A4;});break;case 75: _LL244: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A9;_tmp4A9.tag=Cyc_TypeSpecifier_tok;_tmp4A9.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4AB;_tmp4AB.tag=1;_tmp4AB.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AB;});
_tmp4AA;}));_tmp4A9;});_tmp4A8;});break;case 76: _LL245: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL246: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL247:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL248: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AC=_cycalloc(
sizeof(*_tmp4AC));_tmp4AC[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4AD;_tmp4AD.tag=
Cyc_TypeSpecifier_tok;_tmp4AD.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AD;});
_tmp4AC;});break;case 80: _LL249: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4AF;_tmp4AF.tag=Cyc_TypeSpecifier_tok;_tmp4AF.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));
_tmp4B0[0]=({struct Cyc_Absyn_TupleType_struct _tmp4B1;_tmp4B1.tag=9;_tmp4B1.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4B1;});_tmp4B0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AF;});_tmp4AE;});break;case 81: _LL24A: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));
_tmp4B2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B3;_tmp4B3.tag=Cyc_TypeSpecifier_tok;
_tmp4B3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4B5;_tmp4B5.tag=15;_tmp4B5.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B5;});_tmp4B4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B3;});_tmp4B2;});break;case 82: _LL24B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));
_tmp4B6[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B7;_tmp4B7.tag=Cyc_TypeSpecifier_tok;
_tmp4B7.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp4B9;_tmp4B9.tag=14;_tmp4B9.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B9;});_tmp4B8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B7;});_tmp4B6;});break;case 83: _LL24C: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BB;_tmp4BB.tag=Cyc_TypeSpecifier_tok;
_tmp4BB.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Absyn_TagType_struct
_tmp4BD;_tmp4BD.tag=17;_tmp4BD.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BD;});_tmp4BC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4BB;});_tmp4BA;});break;case 84: _LL24D: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));
_tmp4BE[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BF;_tmp4BF.tag=Cyc_TypeSpecifier_tok;
_tmp4BF.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_Absyn_TagType_struct
_tmp4C1;_tmp4C1.tag=17;_tmp4C1.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->v=(void*)((void*)5);_tmp4C2;}),0);
_tmp4C1;});_tmp4C0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BF;});
_tmp4BE;});break;case 85: _LL24E: {struct _tuple13 _tmp4C4;int _tmp4C5;struct _tuple13*
_tmp4C3=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C4.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4C7;_tmp4C7.tag=Cyc_TypeSpecifier_tok;_tmp4C7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp4C8=_cycalloc_atomic(sizeof(*_tmp4C8));
_tmp4C8[0]=({struct Cyc_Absyn_TypeInt_struct _tmp4C9;_tmp4C9.tag=18;_tmp4C9.f1=
_tmp4C5;_tmp4C9;});_tmp4C8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C7;});
_tmp4C6;});break;}case 86: _LL24F: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp4CA=
_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_Kind_tok_struct _tmp4CB;
_tmp4CB.tag=Cyc_Kind_tok;_tmp4CB.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CB;});
_tmp4CA;});break;case 87: _LL250: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC[0]=({struct Cyc_TypeQual_tok_struct
_tmp4CD;_tmp4CD.tag=Cyc_TypeQual_tok;_tmp4CD.f1=({struct Cyc_Absyn_Tqual _tmp4CE;
_tmp4CE.print_const=1;_tmp4CE.q_volatile=0;_tmp4CE.q_restrict=0;_tmp4CE.real_const=
1;_tmp4CE.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4CE;});_tmp4CD;});_tmp4CC;});break;case 88: _LL251:
yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));
_tmp4CF[0]=({struct Cyc_TypeQual_tok_struct _tmp4D0;_tmp4D0.tag=Cyc_TypeQual_tok;
_tmp4D0.f1=({struct Cyc_Absyn_Tqual _tmp4D1;_tmp4D1.print_const=0;_tmp4D1.q_volatile=
1;_tmp4D1.q_restrict=0;_tmp4D1.real_const=0;_tmp4D1.loc=0;_tmp4D1;});_tmp4D0;});
_tmp4CF;});break;case 89: _LL252: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_TypeQual_tok_struct
_tmp4D3;_tmp4D3.tag=Cyc_TypeQual_tok;_tmp4D3.f1=({struct Cyc_Absyn_Tqual _tmp4D4;
_tmp4D4.print_const=0;_tmp4D4.q_volatile=0;_tmp4D4.q_restrict=1;_tmp4D4.real_const=
0;_tmp4D4.loc=0;_tmp4D4;});_tmp4D3;});_tmp4D2;});break;case 90: _LL253: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4D6;_tmp4D6.tag=Cyc_TypeSpecifier_tok;
_tmp4D6.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4D7=_cycalloc(
sizeof(*_tmp4D7));_tmp4D7[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4D8;_tmp4D8.tag=
5;_tmp4D8.f1=({struct Cyc_Absyn_Decl*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));
_tmp4DA[0]=({struct Cyc_Absyn_Enum_d_struct _tmp4DB;_tmp4DB.tag=6;_tmp4DB.f1=({
struct Cyc_Absyn_Enumdecl*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->sc=(void*)((
void*)2);_tmp4DC->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4DC->fields=({struct Cyc_Core_Opt*_tmp4DD=_cycalloc(
sizeof(*_tmp4DD));_tmp4DD->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DD;});_tmp4DC;});
_tmp4DB;});_tmp4DA;}));_tmp4D9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4D9;});
_tmp4D8;});_tmp4D7;}));_tmp4D6;});_tmp4D5;});break;case 91: _LL254: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4DF;_tmp4DF.tag=Cyc_TypeSpecifier_tok;
_tmp4DF.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({struct Cyc_Absyn_EnumType_struct
_tmp4E1;_tmp4E1.tag=12;_tmp4E1.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E1.f2=0;_tmp4E1;});
_tmp4E0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4DF;});_tmp4DE;});break;case 92: _LL255: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));
_tmp4E2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4E3;_tmp4E3.tag=Cyc_TypeSpecifier_tok;
_tmp4E3.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4E4=_cycalloc(
sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_Parse_Type_spec_struct _tmp4E5;_tmp4E5.tag=
4;_tmp4E5.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp4E6=
_cycalloc(sizeof(*_tmp4E6));_tmp4E6[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4E7;_tmp4E7.tag=13;_tmp4E7.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E7;});_tmp4E6;}));
_tmp4E5.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4E5;});_tmp4E4;}));_tmp4E3;});_tmp4E2;});break;
case 93: _LL256: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4E8=_cycalloc(
sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_Enumfield_tok_struct _tmp4E9;_tmp4E9.tag=
Cyc_Enumfield_tok;_tmp4E9.f1=({struct Cyc_Absyn_Enumfield*_tmp4EA=_cycalloc(
sizeof(*_tmp4EA));_tmp4EA->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EA->tag=0;_tmp4EA->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4EA;});_tmp4E9;});_tmp4E8;});break;case 94: _LL257:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));
_tmp4EB[0]=({struct Cyc_Enumfield_tok_struct _tmp4EC;_tmp4EC.tag=Cyc_Enumfield_tok;
_tmp4EC.f1=({struct Cyc_Absyn_Enumfield*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));
_tmp4ED->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4ED->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4ED->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4ED;});
_tmp4EC;});_tmp4EB;});break;case 95: _LL258: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4EF;_tmp4EF.tag=Cyc_EnumfieldList_tok;_tmp4EF.f1=({struct Cyc_List_List*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F0->tl=0;_tmp4F0;});
_tmp4EF;});_tmp4EE;});break;case 96: _LL259: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F2;_tmp4F2.tag=Cyc_EnumfieldList_tok;_tmp4F2.f1=({struct Cyc_List_List*
_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F3->tl=0;_tmp4F3;});
_tmp4F2;});_tmp4F1;});break;case 97: _LL25A: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F5;_tmp4F5.tag=Cyc_EnumfieldList_tok;_tmp4F5.f1=({struct Cyc_List_List*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F6->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F6;});_tmp4F5;});
_tmp4F4;});break;case 98: _LL25B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4F8;_tmp4F8.tag=Cyc_TypeSpecifier_tok;_tmp4F8.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));
_tmp4F9[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4FA;_tmp4FA.tag=11;_tmp4FA.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4FA.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FA;});_tmp4F9;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F8;});_tmp4F7;});break;case 99: _LL25C: {struct
Cyc_List_List*_tmp4FB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4FC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4FE;_tmp4FE.tag=Cyc_TypeSpecifier_tok;
_tmp4FE.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4FF=_cycalloc(
sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_Parse_Decl_spec_struct _tmp500;_tmp500.tag=
5;_tmp500.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4FB,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4FC,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp500;});
_tmp4FF;}));_tmp4FE;});_tmp4FD;});break;}case 100: _LL25D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp502;_tmp502.tag=Cyc_TypeSpecifier_tok;_tmp502.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503[0]=({struct Cyc_Absyn_AggrType_struct _tmp504;_tmp504.tag=10;_tmp504.f1=({
struct Cyc_Absyn_AggrInfo _tmp505;_tmp505.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp507;_tmp507.tag=0;_tmp507.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp507.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp507;});_tmp506;}));
_tmp505.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp505;});_tmp504;});_tmp503;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp502;});
_tmp501;});break;case 101: _LL25E: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_TypeList_tok_struct
_tmp509;_tmp509.tag=Cyc_TypeList_tok;_tmp509.f1=0;_tmp509;});_tmp508;});break;
case 102: _LL25F: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp50A=_cycalloc(
sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_TypeList_tok_struct _tmp50B;_tmp50B.tag=
Cyc_TypeList_tok;_tmp50B.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp50B;});_tmp50A;});
break;case 103: _LL260: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp50C=
_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp50D;_tmp50D.tag=Cyc_StructOrUnion_tok;_tmp50D.f1=(void*)((void*)0);_tmp50D;});
_tmp50C;});break;case 104: _LL261: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp50F;_tmp50F.tag=Cyc_StructOrUnion_tok;_tmp50F.f1=(void*)((void*)1);_tmp50F;});
_tmp50E;});break;case 105: _LL262: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp511;_tmp511.tag=Cyc_AggrFieldDeclList_tok;_tmp511.f1=0;_tmp511;});_tmp510;});
break;case 106: _LL263: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp513;_tmp513.tag=Cyc_AggrFieldDeclList_tok;_tmp513.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp513;});_tmp512;});
break;case 107: _LL264: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp515;_tmp515.tag=Cyc_AggrFieldDeclListList_tok;_tmp515.f1=({struct Cyc_List_List*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516->tl=0;_tmp516;});
_tmp515;});_tmp514;});break;case 108: _LL265: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp518;_tmp518.tag=Cyc_AggrFieldDeclListList_tok;_tmp518.f1=({struct Cyc_List_List*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp519;});_tmp518;});
_tmp517;});break;case 109: _LL266: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51B;_tmp51B.tag=Cyc_InitDeclList_tok;_tmp51B.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp51B;});_tmp51A;});
break;case 110: _LL267: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp51C=
_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51D;_tmp51D.tag=Cyc_InitDeclList_tok;_tmp51D.f1=({struct Cyc_List_List*
_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51E->tl=0;_tmp51E;});
_tmp51D;});_tmp51C;});break;case 111: _LL268: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_InitDeclList_tok_struct
_tmp520;_tmp520.tag=Cyc_InitDeclList_tok;_tmp520.f1=({struct Cyc_List_List*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp521;});_tmp520;});
_tmp51F;});break;case 112: _LL269: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_InitDecl_tok_struct
_tmp523;_tmp523.tag=Cyc_InitDecl_tok;_tmp523.f1=({struct _tuple12*_tmp524=
_cycalloc(sizeof(*_tmp524));_tmp524->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp524->f2=0;_tmp524;});
_tmp523;});_tmp522;});break;case 113: _LL26A: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525[0]=({struct Cyc_InitDecl_tok_struct
_tmp526;_tmp526.tag=Cyc_InitDecl_tok;_tmp526.f1=({struct _tuple12*_tmp527=
_cycalloc(sizeof(*_tmp527));_tmp527->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp527->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp527;});_tmp526;});_tmp525;});break;case 114: _LL26B: {struct _tuple15 _tmp529;
struct Cyc_Absyn_Tqual _tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_List_List*
_tmp52C;struct _tuple15*_tmp528=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp529=*_tmp528;_tmp52A=
_tmp529.f1;_tmp52B=_tmp529.f2;_tmp52C=_tmp529.f3;if(_tmp52A.loc == 0)_tmp52A.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp52E;struct Cyc_List_List*
_tmp52F;struct _tuple0 _tmp52D=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp52E=_tmp52D.f1;_tmp52F=_tmp52D.f2;{void*_tmp530=Cyc_Parse_speclist2typ(
_tmp52B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp531=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp52A,_tmp530,_tmp52E,_tmp52C),_tmp52F);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp533;_tmp533.tag=Cyc_AggrFieldDeclList_tok;_tmp533.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp531);_tmp533;});_tmp532;});break;}}}case 115:
_LL26C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp534=_cycalloc(
sizeof(*_tmp534));_tmp534[0]=({struct Cyc_QualSpecList_tok_struct _tmp535;_tmp535.tag=
Cyc_QualSpecList_tok;_tmp535.f1=({struct _tuple15*_tmp536=_cycalloc(sizeof(*
_tmp536));_tmp536->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp536->f2=({
struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp537->tl=0;_tmp537;});
_tmp536->f3=0;_tmp536;});_tmp535;});_tmp534;});break;case 116: _LL26D: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538[0]=({
struct Cyc_QualSpecList_tok_struct _tmp539;_tmp539.tag=Cyc_QualSpecList_tok;
_tmp539.f1=({struct _tuple15*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp53A->f2=({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*
_tmp53B));_tmp53B->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53B->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53B;});_tmp53A->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp53A;});_tmp539;});_tmp538;});break;case 117: _LL26E: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));
_tmp53C[0]=({struct Cyc_QualSpecList_tok_struct _tmp53D;_tmp53D.tag=Cyc_QualSpecList_tok;
_tmp53D.f1=({struct _tuple15*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53E->f2=0;_tmp53E->f3=
0;_tmp53E;});_tmp53D;});_tmp53C;});break;case 118: _LL26F: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F[0]=({struct Cyc_QualSpecList_tok_struct
_tmp540;_tmp540.tag=Cyc_QualSpecList_tok;_tmp540.f1=({struct _tuple15*_tmp541=
_cycalloc(sizeof(*_tmp541));_tmp541->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp541->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp541->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp541;});_tmp540;});
_tmp53F;});break;case 119: _LL270: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542[0]=({struct Cyc_QualSpecList_tok_struct
_tmp543;_tmp543.tag=Cyc_QualSpecList_tok;_tmp543.f1=({struct _tuple15*_tmp544=
_cycalloc(sizeof(*_tmp544));_tmp544->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp544->f2=0;
_tmp544->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp544;});_tmp543;});_tmp542;});break;case 120: _LL271:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp545=_cycalloc(sizeof(*
_tmp545));_tmp545[0]=({struct Cyc_QualSpecList_tok_struct _tmp546;_tmp546.tag=Cyc_QualSpecList_tok;
_tmp546.f1=({struct _tuple15*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp547->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp547->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp547;});_tmp546;});
_tmp545;});break;case 121: _LL272: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548[0]=({struct Cyc_QualSpecList_tok_struct
_tmp549;_tmp549.tag=Cyc_QualSpecList_tok;_tmp549.f1=({struct _tuple15*_tmp54A=
_cycalloc(sizeof(*_tmp54A));_tmp54A->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp54A->f2=({
struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54B->tl=0;_tmp54B;});
_tmp54A->f3=0;_tmp54A;});_tmp549;});_tmp548;});break;case 122: _LL273: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({
struct Cyc_QualSpecList_tok_struct _tmp54D;_tmp54D.tag=Cyc_QualSpecList_tok;
_tmp54D.f1=({struct _tuple15*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp54E->f2=({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*
_tmp54F));_tmp54F->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54F->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp54F;});_tmp54E->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp54E;});_tmp54D;});_tmp54C;});break;case 123: _LL274: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp550=_cycalloc(sizeof(*_tmp550));
_tmp550[0]=({struct Cyc_QualSpecList_tok_struct _tmp551;_tmp551.tag=Cyc_QualSpecList_tok;
_tmp551.f1=({struct _tuple15*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp552->f2=0;_tmp552->f3=
0;_tmp552;});_tmp551;});_tmp550;});break;case 124: _LL275: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_QualSpecList_tok_struct
_tmp554;_tmp554.tag=Cyc_QualSpecList_tok;_tmp554.f1=({struct _tuple15*_tmp555=
_cycalloc(sizeof(*_tmp555));_tmp555->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp555->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp555->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp555;});_tmp554;});
_tmp553;});break;case 125: _LL276: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556[0]=({struct Cyc_QualSpecList_tok_struct
_tmp557;_tmp557.tag=Cyc_QualSpecList_tok;_tmp557.f1=({struct _tuple15*_tmp558=
_cycalloc(sizeof(*_tmp558));_tmp558->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp558->f2=0;
_tmp558->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp558;});_tmp557;});_tmp556;});break;case 126: _LL277:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp559=_cycalloc(sizeof(*
_tmp559));_tmp559[0]=({struct Cyc_QualSpecList_tok_struct _tmp55A;_tmp55A.tag=Cyc_QualSpecList_tok;
_tmp55A.f1=({struct _tuple15*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp55B->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp55B->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp55B;});_tmp55A;});
_tmp559;});break;case 127: _LL278: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_InitDeclList_tok_struct
_tmp55D;_tmp55D.tag=Cyc_InitDeclList_tok;_tmp55D.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp55D;});_tmp55C;});
break;case 128: _LL279: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp55E=
_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_InitDeclList_tok_struct
_tmp55F;_tmp55F.tag=Cyc_InitDeclList_tok;_tmp55F.f1=({struct Cyc_List_List*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp560->tl=0;_tmp560;});
_tmp55F;});_tmp55E;});break;case 129: _LL27A: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_InitDeclList_tok_struct
_tmp562;_tmp562.tag=Cyc_InitDeclList_tok;_tmp562.f1=({struct Cyc_List_List*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp563->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp563;});_tmp562;});
_tmp561;});break;case 130: _LL27B: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_InitDecl_tok_struct
_tmp565;_tmp565.tag=Cyc_InitDecl_tok;_tmp565.f1=({struct _tuple12*_tmp566=
_cycalloc(sizeof(*_tmp566));_tmp566->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp566->f2=0;_tmp566;});
_tmp565;});_tmp564;});break;case 131: _LL27C: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567[0]=({struct Cyc_InitDecl_tok_struct
_tmp568;_tmp568.tag=Cyc_InitDecl_tok;_tmp568.f1=({struct _tuple12*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569->f1=({struct Cyc_Parse_Declarator*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A->id=({struct _tuple1*_tmp56B=_cycalloc(
sizeof(*_tmp56B));_tmp56B->f1=Cyc_Absyn_rel_ns_null;_tmp56B->f2=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmp56B;});_tmp56A->tms=
0;_tmp56A;});_tmp569->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp569;});_tmp568;});
_tmp567;});break;case 132: _LL27D: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_InitDecl_tok_struct
_tmp56E;_tmp56E.tag=Cyc_InitDecl_tok;_tmp56E.f1=({struct _tuple12*_tmp56F=
_cycalloc(sizeof(*_tmp56F));_tmp56F->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp56F;});_tmp56E;});_tmp56D;});break;case 133: _LL27E: {struct Cyc_List_List*
_tmp570=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp572;_tmp572.tag=Cyc_TypeSpecifier_tok;
_tmp572.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp573=_cycalloc(
sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Parse_Decl_spec_struct _tmp574;_tmp574.tag=
5;_tmp574.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp570,({struct Cyc_Core_Opt*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp575;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp574;});
_tmp573;}));_tmp572;});_tmp571;});break;}case 134: _LL27F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp577;_tmp577.tag=Cyc_TypeSpecifier_tok;_tmp577.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp578=_cycalloc(sizeof(*_tmp578));
_tmp578[0]=({struct Cyc_Absyn_TunionType_struct _tmp579;_tmp579.tag=2;_tmp579.f1=({
struct Cyc_Absyn_TunionInfo _tmp57A;_tmp57A.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp57C;_tmp57C.tag=0;_tmp57C.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp57D;
_tmp57D.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp57D.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57D;});_tmp57C;});
_tmp57B;}));_tmp57A.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57A.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp57A;});_tmp579;});
_tmp578;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp577;});_tmp576;});break;case 135: _LL280: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));
_tmp57E[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp57F;_tmp57F.tag=Cyc_TypeSpecifier_tok;
_tmp57F.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp581;_tmp581.tag=3;_tmp581.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp582;
_tmp582.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp584;_tmp584.tag=0;_tmp584.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp585;
_tmp585.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp585.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp585.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp585;});_tmp584;});
_tmp583;}));_tmp582.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp582;});_tmp581;});
_tmp580;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp57F;});_tmp57E;});break;case 136: _LL281: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp586=_cycalloc_atomic(sizeof(*_tmp586));
_tmp586[0]=({struct Cyc_Bool_tok_struct _tmp587;_tmp587.tag=Cyc_Bool_tok;_tmp587.f1=
0;_tmp587;});_tmp586;});break;case 137: _LL282: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp588=_cycalloc_atomic(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_Bool_tok_struct
_tmp589;_tmp589.tag=Cyc_Bool_tok;_tmp589.f1=1;_tmp589;});_tmp588;});break;case
138: _LL283: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp58A=_cycalloc(
sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_TunionFieldList_tok_struct _tmp58B;
_tmp58B.tag=Cyc_TunionFieldList_tok;_tmp58B.f1=({struct Cyc_List_List*_tmp58C=
_cycalloc(sizeof(*_tmp58C));_tmp58C->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58C->tl=0;_tmp58C;});
_tmp58B;});_tmp58A;});break;case 139: _LL284: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp58E;_tmp58E.tag=Cyc_TunionFieldList_tok;_tmp58E.f1=({struct Cyc_List_List*
_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58F->tl=0;_tmp58F;});
_tmp58E;});_tmp58D;});break;case 140: _LL285: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp591;_tmp591.tag=Cyc_TunionFieldList_tok;_tmp591.f1=({struct Cyc_List_List*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp592->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp592;});_tmp591;});
_tmp590;});break;case 141: _LL286: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp594;_tmp594.tag=Cyc_TunionFieldList_tok;_tmp594.f1=({struct Cyc_List_List*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp595->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp595;});_tmp594;});
_tmp593;});break;case 142: _LL287: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596[0]=({struct Cyc_Scope_tok_struct
_tmp597;_tmp597.tag=Cyc_Scope_tok;_tmp597.f1=(void*)((void*)2);_tmp597;});
_tmp596;});break;case 143: _LL288: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_Scope_tok_struct
_tmp599;_tmp599.tag=Cyc_Scope_tok;_tmp599.f1=(void*)((void*)3);_tmp599;});
_tmp598;});break;case 144: _LL289: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Scope_tok_struct
_tmp59B;_tmp59B.tag=Cyc_Scope_tok;_tmp59B.f1=(void*)((void*)0);_tmp59B;});
_tmp59A;});break;case 145: _LL28A: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_TunionField_tok_struct
_tmp59D;_tmp59D.tag=Cyc_TunionField_tok;_tmp59D.f1=({struct Cyc_Absyn_Tunionfield*
_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59E->typs=0;_tmp59E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp59E->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59E;});_tmp59D;});
_tmp59C;});break;case 146: _LL28B: {struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0[0]=({
struct Cyc_TunionField_tok_struct _tmp5A1;_tmp5A1.tag=Cyc_TunionField_tok;_tmp5A1.f1=({
struct Cyc_Absyn_Tunionfield*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5A2->typs=_tmp59F;_tmp5A2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A2->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp5A2;});_tmp5A1;});_tmp5A0;});break;}case 147: _LL28C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 148: _LL28D: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3[0]=({struct Cyc_Declarator_tok_struct _tmp5A4;_tmp5A4.tag=Cyc_Declarator_tok;
_tmp5A4.f1=({struct Cyc_Parse_Declarator*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5A5->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5A5;});
_tmp5A4;});_tmp5A3;});break;case 149: _LL28E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 150: _LL28F: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
_tmp5A6[0]=({struct Cyc_Declarator_tok_struct _tmp5A7;_tmp5A7.tag=Cyc_Declarator_tok;
_tmp5A7.f1=({struct Cyc_Parse_Declarator*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));
_tmp5A8->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5A8->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5A8;});
_tmp5A7;});_tmp5A6;});break;case 151: _LL290: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9[0]=({struct Cyc_Declarator_tok_struct
_tmp5AA;_tmp5AA.tag=Cyc_Declarator_tok;_tmp5AA.f1=({struct Cyc_Parse_Declarator*
_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AB->tms=0;_tmp5AB;});
_tmp5AA;});_tmp5A9;});break;case 152: _LL291: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 153: _LL292: {
struct Cyc_Parse_Declarator*_tmp5AC=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AC->tms=({struct Cyc_List_List*
_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5AF;_tmp5AF.tag=5;_tmp5AF.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5AF.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5AF;});_tmp5AE;}));_tmp5AD->tl=_tmp5AC->tms;_tmp5AD;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 154:
_LL293: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5B0=_cycalloc(sizeof(*
_tmp5B0));_tmp5B0[0]=({struct Cyc_Declarator_tok_struct _tmp5B1;_tmp5B1.tag=Cyc_Declarator_tok;
_tmp5B1.f1=({struct Cyc_Parse_Declarator*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));
_tmp5B2->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B2->tms=({struct Cyc_List_List*_tmp5B3=_cycalloc(
sizeof(*_tmp5B3));_tmp5B3->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5B5;_tmp5B5.tag=0;_tmp5B5.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B5;});
_tmp5B4;}));_tmp5B3->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B3;});_tmp5B2;});
_tmp5B1;});_tmp5B0;});break;case 155: _LL294: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=({struct Cyc_Declarator_tok_struct
_tmp5B7;_tmp5B7.tag=Cyc_Declarator_tok;_tmp5B7.f1=({struct Cyc_Parse_Declarator*
_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5B8->tms=({
struct Cyc_List_List*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));
_tmp5BA[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5BB;_tmp5BB.tag=1;_tmp5BB.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5BB.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5BB.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5BB;});_tmp5BA;}));_tmp5B9->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5B9;});
_tmp5B8;});_tmp5B7;});_tmp5B6;});break;case 156: _LL295: {struct _tuple14 _tmp5BD;
struct Cyc_List_List*_tmp5BE;int _tmp5BF;struct Cyc_Absyn_VarargInfo*_tmp5C0;struct
Cyc_Core_Opt*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct _tuple14*_tmp5BC=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BD=*_tmp5BC;
_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;_tmp5C0=_tmp5BD.f3;_tmp5C1=_tmp5BD.f4;
_tmp5C2=_tmp5BD.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5C3=
_cycalloc(sizeof(*_tmp5C3));_tmp5C3[0]=({struct Cyc_Declarator_tok_struct _tmp5C4;
_tmp5C4.tag=Cyc_Declarator_tok;_tmp5C4.f1=({struct Cyc_Parse_Declarator*_tmp5C5=
_cycalloc(sizeof(*_tmp5C5));_tmp5C5->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5C5->tms=({
struct Cyc_List_List*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
_tmp5C7[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5C8;_tmp5C8.tag=3;_tmp5C8.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5C9=_cycalloc(sizeof(*
_tmp5C9));_tmp5C9[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5CA;_tmp5CA.tag=1;
_tmp5CA.f1=_tmp5BE;_tmp5CA.f2=_tmp5BF;_tmp5CA.f3=_tmp5C0;_tmp5CA.f4=_tmp5C1;
_tmp5CA.f5=_tmp5C2;_tmp5CA;});_tmp5C9;}));_tmp5C8;});_tmp5C7;}));_tmp5C6->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp5C6;});_tmp5C5;});_tmp5C4;});_tmp5C3;});break;}case 157: _LL296:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));
_tmp5CB[0]=({struct Cyc_Declarator_tok_struct _tmp5CC;_tmp5CC.tag=Cyc_Declarator_tok;
_tmp5CC.f1=({struct Cyc_Parse_Declarator*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));
_tmp5CD->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5CD->tms=({struct Cyc_List_List*_tmp5CE=_cycalloc(
sizeof(*_tmp5CE));_tmp5CE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5D0;_tmp5D0.tag=3;_tmp5D0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5D2;_tmp5D2.tag=1;_tmp5D2.f1=0;_tmp5D2.f2=0;_tmp5D2.f3=0;_tmp5D2.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D2.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D2;});_tmp5D1;}));
_tmp5D0;});_tmp5CF;}));_tmp5CE->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5CE;});_tmp5CD;});
_tmp5CC;});_tmp5CB;});break;case 158: _LL297: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_Declarator_tok_struct
_tmp5D4;_tmp5D4.tag=Cyc_Declarator_tok;_tmp5D4.f1=({struct Cyc_Parse_Declarator*
_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5D5->tms=({
struct Cyc_List_List*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));
_tmp5D7[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D8;_tmp5D8.tag=3;_tmp5D8.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));
_tmp5D9[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5DA;_tmp5DA.tag=0;_tmp5DA.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DA.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5DA;});
_tmp5D9;}));_tmp5D8;});_tmp5D7;}));_tmp5D6->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5D6;});_tmp5D5;});
_tmp5D4;});_tmp5D3;});break;case 159: _LL298: {struct Cyc_List_List*_tmp5DB=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC[0]=({
struct Cyc_Declarator_tok_struct _tmp5DD;_tmp5DD.tag=Cyc_Declarator_tok;_tmp5DD.f1=({
struct Cyc_Parse_Declarator*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5DE->tms=({
struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));
_tmp5E0[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp5E1;_tmp5E1.tag=4;_tmp5E1.f1=
_tmp5DB;_tmp5E1.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E1.f3=0;
_tmp5E1;});_tmp5E0;}));_tmp5DF->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5DF;});_tmp5DE;});
_tmp5DD;});_tmp5DC;});break;}case 160: _LL299: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2[0]=({struct Cyc_Declarator_tok_struct
_tmp5E3;_tmp5E3.tag=Cyc_Declarator_tok;_tmp5E3.f1=({struct Cyc_Parse_Declarator*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5E4->tms=({
struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));
_tmp5E6[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5E7;_tmp5E7.tag=5;_tmp5E7.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5E7.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E7;});_tmp5E6;}));
_tmp5E5->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5E5;});_tmp5E4;});_tmp5E3;});_tmp5E2;});break;case
161: _LL29A: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5E8=_cycalloc(
sizeof(*_tmp5E8));_tmp5E8[0]=({struct Cyc_Declarator_tok_struct _tmp5E9;_tmp5E9.tag=
Cyc_Declarator_tok;_tmp5E9.f1=({struct Cyc_Parse_Declarator*_tmp5EA=_cycalloc(
sizeof(*_tmp5EA));_tmp5EA->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EA->tms=0;_tmp5EA;});
_tmp5E9;});_tmp5E8;});break;case 162: _LL29B: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Declarator_tok_struct
_tmp5EC;_tmp5EC.tag=Cyc_Declarator_tok;_tmp5EC.f1=({struct Cyc_Parse_Declarator*
_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5ED->tms=0;_tmp5ED;});
_tmp5EC;});_tmp5EB;});break;case 163: _LL29C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 164: _LL29D: {
struct Cyc_Parse_Declarator*_tmp5EE=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EE->tms=({struct Cyc_List_List*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5F1;_tmp5F1.tag=5;_tmp5F1.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5F1.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5F1;});_tmp5F0;}));_tmp5EF->tl=_tmp5EE->tms;_tmp5EF;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165:
_LL29E: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5F2=_cycalloc(sizeof(*
_tmp5F2));_tmp5F2[0]=({struct Cyc_Declarator_tok_struct _tmp5F3;_tmp5F3.tag=Cyc_Declarator_tok;
_tmp5F3.f1=({struct Cyc_Parse_Declarator*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));
_tmp5F4->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5F4->tms=({struct Cyc_List_List*_tmp5F5=_cycalloc(
sizeof(*_tmp5F5));_tmp5F5->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5F7;_tmp5F7.tag=0;_tmp5F7.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5F7;});
_tmp5F6;}));_tmp5F5->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5F5;});_tmp5F4;});
_tmp5F3;});_tmp5F2;});break;case 166: _LL29F: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Declarator_tok_struct
_tmp5F9;_tmp5F9.tag=Cyc_Declarator_tok;_tmp5F9.f1=({struct Cyc_Parse_Declarator*
_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5FA->tms=({
struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));
_tmp5FC[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5FD;_tmp5FD.tag=1;_tmp5FD.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5FD.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5FD.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5FD;});_tmp5FC;}));_tmp5FB->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5FB;});
_tmp5FA;});_tmp5F9;});_tmp5F8;});break;case 167: _LL2A0: {struct _tuple14 _tmp5FF;
struct Cyc_List_List*_tmp600;int _tmp601;struct Cyc_Absyn_VarargInfo*_tmp602;struct
Cyc_Core_Opt*_tmp603;struct Cyc_List_List*_tmp604;struct _tuple14*_tmp5FE=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FF=*_tmp5FE;
_tmp600=_tmp5FF.f1;_tmp601=_tmp5FF.f2;_tmp602=_tmp5FF.f3;_tmp603=_tmp5FF.f4;
_tmp604=_tmp5FF.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp605=
_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Declarator_tok_struct _tmp606;
_tmp606.tag=Cyc_Declarator_tok;_tmp606.f1=({struct Cyc_Parse_Declarator*_tmp607=
_cycalloc(sizeof(*_tmp607));_tmp607->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp607->tms=({
struct Cyc_List_List*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp609=_cycalloc(sizeof(*_tmp609));
_tmp609[0]=({struct Cyc_Absyn_Function_mod_struct _tmp60A;_tmp60A.tag=3;_tmp60A.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp60B=_cycalloc(sizeof(*
_tmp60B));_tmp60B[0]=({struct Cyc_Absyn_WithTypes_struct _tmp60C;_tmp60C.tag=1;
_tmp60C.f1=_tmp600;_tmp60C.f2=_tmp601;_tmp60C.f3=_tmp602;_tmp60C.f4=_tmp603;
_tmp60C.f5=_tmp604;_tmp60C;});_tmp60B;}));_tmp60A;});_tmp609;}));_tmp608->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp608;});_tmp607;});_tmp606;});_tmp605;});break;}case 168: _LL2A1:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));
_tmp60D[0]=({struct Cyc_Declarator_tok_struct _tmp60E;_tmp60E.tag=Cyc_Declarator_tok;
_tmp60E.f1=({struct Cyc_Parse_Declarator*_tmp60F=_cycalloc(sizeof(*_tmp60F));
_tmp60F->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp60F->tms=({struct Cyc_List_List*_tmp610=_cycalloc(
sizeof(*_tmp610));_tmp610->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp612;_tmp612.tag=3;_tmp612.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp614;_tmp614.tag=1;_tmp614.f1=0;_tmp614.f2=0;_tmp614.f3=0;_tmp614.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp614.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp614;});_tmp613;}));
_tmp612;});_tmp611;}));_tmp610->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp610;});_tmp60F;});
_tmp60E;});_tmp60D;});break;case 169: _LL2A2: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({struct Cyc_Declarator_tok_struct
_tmp616;_tmp616.tag=Cyc_Declarator_tok;_tmp616.f1=({struct Cyc_Parse_Declarator*
_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp617->tms=({
struct Cyc_List_List*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp619=_cycalloc(sizeof(*_tmp619));
_tmp619[0]=({struct Cyc_Absyn_Function_mod_struct _tmp61A;_tmp61A.tag=3;_tmp61A.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));
_tmp61B[0]=({struct Cyc_Absyn_NoTypes_struct _tmp61C;_tmp61C.tag=0;_tmp61C.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp61C;});
_tmp61B;}));_tmp61A;});_tmp619;}));_tmp618->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp618;});_tmp617;});
_tmp616;});_tmp615;});break;case 170: _LL2A3: {struct Cyc_List_List*_tmp61D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({
struct Cyc_Declarator_tok_struct _tmp61F;_tmp61F.tag=Cyc_Declarator_tok;_tmp61F.f1=({
struct Cyc_Parse_Declarator*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp620->tms=({
struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp622=_cycalloc(sizeof(*_tmp622));
_tmp622[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp623;_tmp623.tag=4;_tmp623.f1=
_tmp61D;_tmp623.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp623.f3=0;
_tmp623;});_tmp622;}));_tmp621->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp621;});_tmp620;});
_tmp61F;});_tmp61E;});break;}case 171: _LL2A4: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Declarator_tok_struct
_tmp625;_tmp625.tag=Cyc_Declarator_tok;_tmp625.f1=({struct Cyc_Parse_Declarator*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp626->tms=({
struct Cyc_List_List*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp628=_cycalloc(sizeof(*_tmp628));
_tmp628[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp629;_tmp629.tag=5;_tmp629.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp629.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp629;});_tmp628;}));
_tmp627->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp627;});_tmp626;});_tmp625;});_tmp624;});break;case
172: _LL2A5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 173: _LL2A6: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp62A=
_cycalloc(sizeof(*_tmp62A));_tmp62A[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp62B;_tmp62B.tag=Cyc_TypeModifierList_tok;_tmp62B.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp62B;});_tmp62A;});
break;case 174: _LL2A7: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp62E;_tmp62E.tag=5;_tmp62E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp62E.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp62E;});_tmp62D;}));_tmp62C->tl=ans;_tmp62C;});{struct Cyc_Absyn_PtrLoc*
_tmp62F=({struct Cyc_Absyn_PtrLoc*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->ptr_loc=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;
_tmp636->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp636->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp636;});ans=({struct Cyc_List_List*_tmp630=
_cycalloc(sizeof(*_tmp630));_tmp630->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp632;_tmp632.tag=2;_tmp632.f1=({struct Cyc_Absyn_PtrAtts _tmp633;_tmp633.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp633.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp633.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp633.zero_term=
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp633.ptrloc=_tmp62F;_tmp633;});_tmp632.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp632;});_tmp631;}));
_tmp630->tl=ans;_tmp630;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp635;_tmp635.tag=Cyc_TypeModifierList_tok;_tmp635.f1=ans;_tmp635;});_tmp634;});
break;}}case 175: _LL2A8: yyval=(void*)({struct Cyc_YY1_struct*_tmp637=_cycalloc(
sizeof(*_tmp637));_tmp637[0]=({struct Cyc_YY1_struct _tmp638;_tmp638.tag=Cyc_YY1;
_tmp638.f1=({struct _tuple16*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp639->f2=
Cyc_Absyn_true_conref;_tmp639->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp639;});_tmp638;});
_tmp637;});break;case 176: _LL2A9: yyval=(void*)({struct Cyc_YY1_struct*_tmp63A=
_cycalloc(sizeof(*_tmp63A));_tmp63A[0]=({struct Cyc_YY1_struct _tmp63B;_tmp63B.tag=
Cyc_YY1;_tmp63B.f1=({struct _tuple16*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp63C->f2=Cyc_Absyn_false_conref;_tmp63C->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63C;});_tmp63B;});
_tmp63A;});break;case 177: _LL2AA: yyval=(void*)({struct Cyc_YY1_struct*_tmp63D=
_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_YY1_struct _tmp63E;_tmp63E.tag=
Cyc_YY1;_tmp63E.f1=({struct _tuple16*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp63F->f2=Cyc_Absyn_true_conref;_tmp63F->f3=Cyc_Absyn_bounds_unknown_conref;
_tmp63F;});_tmp63E;});_tmp63D;});break;case 178: _LL2AB: yyval=(void*)({struct Cyc_YY2_struct*
_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640[0]=({struct Cyc_YY2_struct _tmp641;
_tmp641.tag=Cyc_YY2;_tmp641.f1=Cyc_Absyn_bounds_one_conref;_tmp641;});_tmp640;});
break;case 179: _LL2AC: yyval=(void*)({struct Cyc_YY2_struct*_tmp642=_cycalloc(
sizeof(*_tmp642));_tmp642[0]=({struct Cyc_YY2_struct _tmp643;_tmp643.tag=Cyc_YY2;
_tmp643.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp644=
_cycalloc(sizeof(*_tmp644));_tmp644[0]=({struct Cyc_Absyn_Upper_b_struct _tmp645;
_tmp645.tag=0;_tmp645.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp645;});_tmp644;}));_tmp643;});_tmp642;});break;case
180: _LL2AD: yyval=(void*)({struct Cyc_YY2_struct*_tmp646=_cycalloc(sizeof(*_tmp646));
_tmp646[0]=({struct Cyc_YY2_struct _tmp647;_tmp647.tag=Cyc_YY2;_tmp647.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp648=_cycalloc(sizeof(*_tmp648));
_tmp648[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp649;_tmp649.tag=1;_tmp649.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp649;});_tmp648;}));_tmp647;});_tmp646;});break;case 181: _LL2AE: yyval=(
void*)({struct Cyc_YY6_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A[0]=({
struct Cyc_YY6_struct _tmp64B;_tmp64B.tag=Cyc_YY6;_tmp64B.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp64B;});_tmp64A;});break;case 182: _LL2AF: yyval=(void*)({
struct Cyc_YY6_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_YY6_struct
_tmp64D;_tmp64D.tag=Cyc_YY6;_tmp64D.f1=Cyc_Absyn_true_conref;_tmp64D;});_tmp64C;});
break;case 183: _LL2B0: yyval=(void*)({struct Cyc_YY6_struct*_tmp64E=_cycalloc(
sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_YY6_struct _tmp64F;_tmp64F.tag=Cyc_YY6;
_tmp64F.f1=Cyc_Absyn_false_conref;_tmp64F;});_tmp64E;});break;case 184: _LL2B1:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp650=_cycalloc(sizeof(*_tmp650));
_tmp650[0]=({struct Cyc_Type_tok_struct _tmp651;_tmp651.tag=Cyc_Type_tok;_tmp651.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp652=_cycalloc(sizeof(*_tmp652));
_tmp652->v=(void*)((void*)3);_tmp652;}),0);_tmp651;});_tmp650;});break;case 185:
_LL2B2: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 186: _LL2B3: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653[0]=({
struct Cyc_Type_tok_struct _tmp654;_tmp654.tag=Cyc_Type_tok;_tmp654.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)((void*)
3);_tmp655;}),0);_tmp654;});_tmp653;});break;case 187: _LL2B4: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656[0]=({
struct Cyc_TypeQual_tok_struct _tmp657;_tmp657.tag=Cyc_TypeQual_tok;_tmp657.f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp657;});_tmp656;});break;case 188: _LL2B5: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp658=_cycalloc(sizeof(*_tmp658));
_tmp658[0]=({struct Cyc_TypeQual_tok_struct _tmp659;_tmp659.tag=Cyc_TypeQual_tok;
_tmp659.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp659;});_tmp658;});
break;case 189: _LL2B6: yyval=(void*)({struct Cyc_YY5_struct*_tmp65A=_cycalloc(
sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_YY5_struct _tmp65B;_tmp65B.tag=Cyc_YY5;
_tmp65B.f1=({struct _tuple14*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp65C->f2=0;
_tmp65C->f3=0;_tmp65C->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65C->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65C;});_tmp65B;});
_tmp65A;});break;case 190: _LL2B7: yyval=(void*)({struct Cyc_YY5_struct*_tmp65D=
_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_YY5_struct _tmp65E;_tmp65E.tag=
Cyc_YY5;_tmp65E.f1=({struct _tuple14*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp65F->f2=1;
_tmp65F->f3=0;_tmp65F->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65F;});_tmp65E;});
_tmp65D;});break;case 191: _LL2B8: {struct _tuple2 _tmp661;struct Cyc_Core_Opt*
_tmp662;struct Cyc_Absyn_Tqual _tmp663;void*_tmp664;struct _tuple2*_tmp660=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp661=*_tmp660;
_tmp662=_tmp661.f1;_tmp663=_tmp661.f2;_tmp664=_tmp661.f3;{struct Cyc_Absyn_VarargInfo*
_tmp665=({struct Cyc_Absyn_VarargInfo*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->name=
_tmp662;_tmp669->tq=_tmp663;_tmp669->type=(void*)_tmp664;_tmp669->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp669;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666[0]=({
struct Cyc_YY5_struct _tmp667;_tmp667.tag=Cyc_YY5;_tmp667.f1=({struct _tuple14*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->f1=0;_tmp668->f2=0;_tmp668->f3=
_tmp665;_tmp668->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp668->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp668;});_tmp667;});
_tmp666;});break;}}case 192: _LL2B9: {struct _tuple2 _tmp66B;struct Cyc_Core_Opt*
_tmp66C;struct Cyc_Absyn_Tqual _tmp66D;void*_tmp66E;struct _tuple2*_tmp66A=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66B=*_tmp66A;
_tmp66C=_tmp66B.f1;_tmp66D=_tmp66B.f2;_tmp66E=_tmp66B.f3;{struct Cyc_Absyn_VarargInfo*
_tmp66F=({struct Cyc_Absyn_VarargInfo*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->name=
_tmp66C;_tmp673->tq=_tmp66D;_tmp673->type=(void*)_tmp66E;_tmp673->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp673;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({
struct Cyc_YY5_struct _tmp671;_tmp671.tag=Cyc_YY5;_tmp671.f1=({struct _tuple14*
_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp672->f2=0;_tmp672->f3=
_tmp66F;_tmp672->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp672->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp672;});_tmp671;});
_tmp670;});break;}}case 193: _LL2BA: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674[0]=({struct Cyc_Type_tok_struct
_tmp675;_tmp675.tag=Cyc_Type_tok;_tmp675.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp677;_tmp677.tag=1;_tmp677.f1=0;_tmp677;});_tmp676;}));_tmp675;});_tmp674;});
break;case 194: _LL2BB: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp678=_cycalloc(
sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Type_tok_struct _tmp679;_tmp679.tag=Cyc_Type_tok;
_tmp679.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp67B;_tmp67B.tag=0;_tmp67B.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67B;});_tmp67A;}));
_tmp679;});_tmp678;});break;case 195: _LL2BC: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_TypeOpt_tok_struct
_tmp67D;_tmp67D.tag=Cyc_TypeOpt_tok;_tmp67D.f1=0;_tmp67D;});_tmp67C;});break;
case 196: _LL2BD: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp67E=_cycalloc(
sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_TypeOpt_tok_struct _tmp67F;_tmp67F.tag=
Cyc_TypeOpt_tok;_tmp67F.f1=({struct Cyc_Core_Opt*_tmp680=_cycalloc(sizeof(*
_tmp680));_tmp680->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp681=
_cycalloc(sizeof(*_tmp681));_tmp681[0]=({struct Cyc_Absyn_JoinEff_struct _tmp682;
_tmp682.tag=20;_tmp682.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp682;});_tmp681;}));
_tmp680;});_tmp67F;});_tmp67E;});break;case 197: _LL2BE: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Rgnorder_tok_struct
_tmp684;_tmp684.tag=Cyc_Rgnorder_tok;_tmp684.f1=0;_tmp684;});_tmp683;});break;
case 198: _LL2BF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 199: _LL2C0: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp685=
_cycalloc(sizeof(*_tmp685));_tmp685[0]=({struct Cyc_Rgnorder_tok_struct _tmp686;
_tmp686.tag=Cyc_Rgnorder_tok;_tmp686.f1=({struct Cyc_List_List*_tmp687=_cycalloc(
sizeof(*_tmp687));_tmp687->hd=({struct _tuple4*_tmp688=_cycalloc(sizeof(*_tmp688));
_tmp688->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68B=_cycalloc(sizeof(*
_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68C;_tmp68C.tag=20;
_tmp68C.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp68C;});_tmp68B;});_tmp688->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp68A;_tmp68A.tag=0;_tmp68A.f1=(void*)((void*)3);_tmp68A;});_tmp689;}));
_tmp688;});_tmp687->tl=0;_tmp687;});_tmp686;});_tmp685;});break;case 200: _LL2C1:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));
_tmp68D[0]=({struct Cyc_Rgnorder_tok_struct _tmp68E;_tmp68E.tag=Cyc_Rgnorder_tok;
_tmp68E.f1=({struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->hd=({
struct _tuple4*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp694;_tmp694.tag=20;_tmp694.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp694;});_tmp693;});
_tmp690->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp692;_tmp692.tag=0;_tmp692.f1=(void*)((void*)3);_tmp692;});_tmp691;}));
_tmp690;});_tmp68F->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68F;});_tmp68E;});
_tmp68D;});break;case 201: _LL2C2: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp695=
_cycalloc_atomic(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Bool_tok_struct _tmp696;
_tmp696.tag=Cyc_Bool_tok;_tmp696.f1=0;_tmp696;});_tmp695;});break;case 202: _LL2C3:
if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp697="inject";
_tag_arr(_tmp697,sizeof(char),_get_zero_arr_size(_tmp697,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp698="missing type in function declaration";_tag_arr(_tmp698,
sizeof(char),_get_zero_arr_size(_tmp698,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp699=_cycalloc_atomic(sizeof(*_tmp699));_tmp699[0]=({
struct Cyc_Bool_tok_struct _tmp69A;_tmp69A.tag=Cyc_Bool_tok;_tmp69A.f1=1;_tmp69A;});
_tmp699;});break;case 203: _LL2C4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 204: _LL2C5: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_TypeList_tok_struct
_tmp69C;_tmp69C.tag=Cyc_TypeList_tok;_tmp69C.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp69C;});_tmp69B;});
break;case 205: _LL2C6: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_TypeList_tok_struct _tmp69E;
_tmp69E.tag=Cyc_TypeList_tok;_tmp69E.f1=0;_tmp69E;});_tmp69D;});break;case 206:
_LL2C7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 207: _LL2C8: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp69F=_cycalloc(
sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_TypeList_tok_struct _tmp6A0;_tmp6A0.tag=
Cyc_TypeList_tok;_tmp6A0.f1=({struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*
_tmp6A1));_tmp6A1->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A2=
_cycalloc(sizeof(*_tmp6A2));_tmp6A2[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A3;
_tmp6A3.tag=21;_tmp6A3.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A3;});_tmp6A2;}));
_tmp6A1->tl=0;_tmp6A1;});_tmp6A0;});_tmp69F;});break;case 208: _LL2C9: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A4=_cycalloc(sizeof(*
_tmp6A4));_tmp6A4[0]=({struct Cyc_TypeList_tok_struct _tmp6A5;_tmp6A5.tag=Cyc_TypeList_tok;
_tmp6A5.f1=({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6A6->tl=0;_tmp6A6;});_tmp6A5;});_tmp6A4;});break;case 209: _LL2CA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A7=_cycalloc(sizeof(*
_tmp6A7));_tmp6A7[0]=({struct Cyc_TypeList_tok_struct _tmp6A8;_tmp6A8.tag=Cyc_TypeList_tok;
_tmp6A8.f1=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AA=_cycalloc(sizeof(*
_tmp6AA));_tmp6AA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AB;_tmp6AB.tag=19;
_tmp6AB.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6AB;});_tmp6AA;}));_tmp6A9->tl=0;_tmp6A9;});_tmp6A8;});
_tmp6A7;});break;case 210: _LL2CB: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));
_tmp6AC[0]=({struct Cyc_TypeList_tok_struct _tmp6AD;_tmp6AD.tag=Cyc_TypeList_tok;
_tmp6AD.f1=({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AF=_cycalloc(sizeof(*
_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6B0;_tmp6B0.tag=19;
_tmp6B0.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp6B0;});_tmp6AF;}));_tmp6AE->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AE;});_tmp6AD;});
_tmp6AC;});break;case 211: _LL2CC: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6B2;_tmp6B2.tag=Cyc_ParamDeclList_tok;_tmp6B2.f1=({struct Cyc_List_List*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3->tl=0;_tmp6B3;});
_tmp6B2;});_tmp6B1;});break;case 212: _LL2CD: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6B5;_tmp6B5.tag=Cyc_ParamDeclList_tok;_tmp6B5.f1=({struct Cyc_List_List*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B6->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B6;});_tmp6B5;});
_tmp6B4;});break;case 213: _LL2CE: {struct _tuple15 _tmp6B8;struct Cyc_Absyn_Tqual
_tmp6B9;struct Cyc_List_List*_tmp6BA;struct Cyc_List_List*_tmp6BB;struct _tuple15*
_tmp6B7=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6B8=*_tmp6B7;_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;
_tmp6BB=_tmp6B8.f3;if(_tmp6B9.loc == 0)_tmp6B9.loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp6BD;struct _tuple1*_tmp6BE;struct Cyc_List_List*_tmp6BF;struct Cyc_Parse_Declarator*
_tmp6BC=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6BD=*_tmp6BC;_tmp6BE=_tmp6BD.id;_tmp6BF=_tmp6BD.tms;{void*
_tmp6C0=Cyc_Parse_speclist2typ(_tmp6BA,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp6C2;void*_tmp6C3;struct Cyc_List_List*_tmp6C4;struct Cyc_List_List*_tmp6C5;
struct _tuple6 _tmp6C1=Cyc_Parse_apply_tms(_tmp6B9,_tmp6C0,_tmp6BB,_tmp6BF);
_tmp6C2=_tmp6C1.f1;_tmp6C3=_tmp6C1.f2;_tmp6C4=_tmp6C1.f3;_tmp6C5=_tmp6C1.f4;if(
_tmp6C4 != 0)Cyc_Parse_err(({const char*_tmp6C6="parameter with bad type params";
_tag_arr(_tmp6C6,sizeof(char),_get_zero_arr_size(_tmp6C6,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6BE))Cyc_Parse_err(({const char*_tmp6C7="parameter cannot be qualified with a namespace";
_tag_arr(_tmp6C7,sizeof(char),_get_zero_arr_size(_tmp6C7,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6C8=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6CE=_cycalloc(sizeof(*
_tmp6CE));_tmp6CE->v=(*_tmp6BE).f2;_tmp6CE;});if(_tmp6C5 != 0)({void*_tmp6C9[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6CA="extra attributes on parameter, ignoring";
_tag_arr(_tmp6CA,sizeof(char),_get_zero_arr_size(_tmp6CA,40));}),_tag_arr(
_tmp6C9,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6CC;_tmp6CC.tag=Cyc_ParamDecl_tok;_tmp6CC.f1=({struct _tuple2*_tmp6CD=
_cycalloc(sizeof(*_tmp6CD));_tmp6CD->f1=_tmp6C8;_tmp6CD->f2=_tmp6C2;_tmp6CD->f3=
_tmp6C3;_tmp6CD;});_tmp6CC;});_tmp6CB;});break;}}}}case 214: _LL2CF: {struct
_tuple15 _tmp6D0;struct Cyc_Absyn_Tqual _tmp6D1;struct Cyc_List_List*_tmp6D2;struct
Cyc_List_List*_tmp6D3;struct _tuple15*_tmp6CF=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D0=*_tmp6CF;_tmp6D1=
_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;_tmp6D3=_tmp6D0.f3;if(_tmp6D1.loc == 0)_tmp6D1.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6D4=Cyc_Parse_speclist2typ(_tmp6D2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6D3 != 0)({
void*_tmp6D5[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6D6="bad attributes on parameter, ignoring";_tag_arr(_tmp6D6,sizeof(char),
_get_zero_arr_size(_tmp6D6,38));}),_tag_arr(_tmp6D5,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));
_tmp6D7[0]=({struct Cyc_ParamDecl_tok_struct _tmp6D8;_tmp6D8.tag=Cyc_ParamDecl_tok;
_tmp6D8.f1=({struct _tuple2*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->f1=0;
_tmp6D9->f2=_tmp6D1;_tmp6D9->f3=_tmp6D4;_tmp6D9;});_tmp6D8;});_tmp6D7;});break;}}
case 215: _LL2D0: {struct _tuple15 _tmp6DB;struct Cyc_Absyn_Tqual _tmp6DC;struct Cyc_List_List*
_tmp6DD;struct Cyc_List_List*_tmp6DE;struct _tuple15*_tmp6DA=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DB=*_tmp6DA;
_tmp6DC=_tmp6DB.f1;_tmp6DD=_tmp6DB.f2;_tmp6DE=_tmp6DB.f3;if(_tmp6DC.loc == 0)
_tmp6DC.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6DF=Cyc_Parse_speclist2typ(_tmp6DD,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6E0=(Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual _tmp6E2;void*_tmp6E3;struct Cyc_List_List*
_tmp6E4;struct Cyc_List_List*_tmp6E5;struct _tuple6 _tmp6E1=Cyc_Parse_apply_tms(
_tmp6DC,_tmp6DF,_tmp6DE,_tmp6E0);_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=
_tmp6E1.f3;_tmp6E5=_tmp6E1.f4;if(_tmp6E4 != 0)({void*_tmp6E6[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6E7="bad type parameters on formal argument, ignoring";
_tag_arr(_tmp6E7,sizeof(char),_get_zero_arr_size(_tmp6E7,49));}),_tag_arr(
_tmp6E6,sizeof(void*),0));});if(_tmp6E5 != 0)({void*_tmp6E8[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6E9="bad attributes on parameter, ignoring";
_tag_arr(_tmp6E9,sizeof(char),_get_zero_arr_size(_tmp6E9,38));}),_tag_arr(
_tmp6E8,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6EB;_tmp6EB.tag=Cyc_ParamDecl_tok;_tmp6EB.f1=({struct _tuple2*_tmp6EC=
_cycalloc(sizeof(*_tmp6EC));_tmp6EC->f1=0;_tmp6EC->f2=_tmp6E2;_tmp6EC->f3=
_tmp6E3;_tmp6EC;});_tmp6EB;});_tmp6EA;});break;}}case 216: _LL2D1: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({
struct Cyc_IdList_tok_struct _tmp6EE;_tmp6EE.tag=Cyc_IdList_tok;_tmp6EE.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp6EE;});_tmp6ED;});
break;case 217: _LL2D2: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_IdList_tok_struct _tmp6F0;
_tmp6F0.tag=Cyc_IdList_tok;_tmp6F0.f1=({struct Cyc_List_List*_tmp6F1=_cycalloc(
sizeof(*_tmp6F1));_tmp6F1->hd=({struct _tagged_arr*_tmp6F2=_cycalloc(sizeof(*
_tmp6F2));_tmp6F2[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6F2;});_tmp6F1->tl=0;_tmp6F1;});_tmp6F0;});_tmp6EF;});
break;case 218: _LL2D3: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_IdList_tok_struct _tmp6F4;
_tmp6F4.tag=Cyc_IdList_tok;_tmp6F4.f1=({struct Cyc_List_List*_tmp6F5=_cycalloc(
sizeof(*_tmp6F5));_tmp6F5->hd=({struct _tagged_arr*_tmp6F6=_cycalloc(sizeof(*
_tmp6F6));_tmp6F6[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6F6;});_tmp6F5->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F5;});_tmp6F4;});
_tmp6F3;});break;case 219: _LL2D4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 220: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 221: _LL2D6: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Exp_tok_struct _tmp6F8;
_tmp6F8.tag=Cyc_Exp_tok;_tmp6F8.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6FA;_tmp6FA.tag=36;_tmp6FA.f1=0;_tmp6FA.f2=0;_tmp6FA;});_tmp6F9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F8;});
_tmp6F7;});break;case 222: _LL2D7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Exp_tok_struct _tmp6FC;_tmp6FC.tag=
Cyc_Exp_tok;_tmp6FC.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6FE;_tmp6FE.tag=36;_tmp6FE.f1=0;_tmp6FE.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6FE;});_tmp6FD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp6FC;});_tmp6FB;});break;case 223: _LL2D8: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({
struct Cyc_Exp_tok_struct _tmp700;_tmp700.tag=Cyc_Exp_tok;_tmp700.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp701=_cycalloc(sizeof(*_tmp701));
_tmp701[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp702;_tmp702.tag=36;
_tmp702.f1=0;_tmp702.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp702;});_tmp701;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp700;});
_tmp6FF;});break;case 224: _LL2D9: {struct Cyc_Absyn_Vardecl*_tmp703=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708->f1=(void*)0;_tmp708->f2=({
struct _tagged_arr*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp709;});_tmp708;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp703->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp704=_cycalloc(sizeof(*_tmp704));
_tmp704[0]=({struct Cyc_Exp_tok_struct _tmp705;_tmp705.tag=Cyc_Exp_tok;_tmp705.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp706=
_cycalloc(sizeof(*_tmp706));_tmp706[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp707;_tmp707.tag=29;_tmp707.f1=_tmp703;_tmp707.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp707.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp707.f4=0;
_tmp707;});_tmp706;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp705;});
_tmp704;});break;}case 225: _LL2DA: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_InitializerList_tok_struct
_tmp70B;_tmp70B.tag=Cyc_InitializerList_tok;_tmp70B.f1=({struct Cyc_List_List*
_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=({struct _tuple20*_tmp70D=
_cycalloc(sizeof(*_tmp70D));_tmp70D->f1=0;_tmp70D->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70D;});_tmp70C->tl=0;
_tmp70C;});_tmp70B;});_tmp70A;});break;case 226: _LL2DB: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_InitializerList_tok_struct
_tmp70F;_tmp70F.tag=Cyc_InitializerList_tok;_tmp70F.f1=({struct Cyc_List_List*
_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->hd=({struct _tuple20*_tmp711=
_cycalloc(sizeof(*_tmp711));_tmp711->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp711->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp711;});_tmp710->tl=
0;_tmp710;});_tmp70F;});_tmp70E;});break;case 227: _LL2DC: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({struct Cyc_InitializerList_tok_struct
_tmp713;_tmp713.tag=Cyc_InitializerList_tok;_tmp713.f1=({struct Cyc_List_List*
_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714->hd=({struct _tuple20*_tmp715=
_cycalloc(sizeof(*_tmp715));_tmp715->f1=0;_tmp715->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp715;});_tmp714->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp714;});_tmp713;});
_tmp712;});break;case 228: _LL2DD: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_InitializerList_tok_struct
_tmp717;_tmp717.tag=Cyc_InitializerList_tok;_tmp717.f1=({struct Cyc_List_List*
_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718->hd=({struct _tuple20*_tmp719=
_cycalloc(sizeof(*_tmp719));_tmp719->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp719->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp719;});_tmp718->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp718;});_tmp717;});_tmp716;});break;case 229: _LL2DE: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp71A=_cycalloc(sizeof(*_tmp71A));
_tmp71A[0]=({struct Cyc_DesignatorList_tok_struct _tmp71B;_tmp71B.tag=Cyc_DesignatorList_tok;
_tmp71B.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp71B;});_tmp71A;});
break;case 230: _LL2DF: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp71C=
_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_DesignatorList_tok_struct
_tmp71D;_tmp71D.tag=Cyc_DesignatorList_tok;_tmp71D.f1=({struct Cyc_List_List*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71E->tl=0;_tmp71E;});
_tmp71D;});_tmp71C;});break;case 231: _LL2E0: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F[0]=({struct Cyc_DesignatorList_tok_struct
_tmp720;_tmp720.tag=Cyc_DesignatorList_tok;_tmp720.f1=({struct Cyc_List_List*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp721->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp721;});_tmp720;});
_tmp71F;});break;case 232: _LL2E1: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Designator_tok_struct
_tmp723;_tmp723.tag=Cyc_Designator_tok;_tmp723.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp725;_tmp725.tag=0;_tmp725.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp725;});_tmp724;}));
_tmp723;});_tmp722;});break;case 233: _LL2E2: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726[0]=({struct Cyc_Designator_tok_struct
_tmp727;_tmp727.tag=Cyc_Designator_tok;_tmp727.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_FieldName_struct
_tmp729;_tmp729.tag=1;_tmp729.f1=({struct _tagged_arr*_tmp72A=_cycalloc(sizeof(*
_tmp72A));_tmp72A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp72A;});_tmp729;});_tmp728;}));_tmp727;});_tmp726;});
break;case 234: _LL2E3: {struct _tuple15 _tmp72C;struct Cyc_Absyn_Tqual _tmp72D;struct
Cyc_List_List*_tmp72E;struct Cyc_List_List*_tmp72F;struct _tuple15*_tmp72B=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp72C=*_tmp72B;
_tmp72D=_tmp72C.f1;_tmp72E=_tmp72C.f2;_tmp72F=_tmp72C.f3;{void*_tmp730=Cyc_Parse_speclist2typ(
_tmp72E,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp72F != 0)({void*_tmp731[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp732="ignoring attributes in type";
_tag_arr(_tmp732,sizeof(char),_get_zero_arr_size(_tmp732,28));}),_tag_arr(
_tmp731,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[0]=({struct Cyc_ParamDecl_tok_struct
_tmp734;_tmp734.tag=Cyc_ParamDecl_tok;_tmp734.f1=({struct _tuple2*_tmp735=
_cycalloc(sizeof(*_tmp735));_tmp735->f1=0;_tmp735->f2=_tmp72D;_tmp735->f3=
_tmp730;_tmp735;});_tmp734;});_tmp733;});break;}}case 235: _LL2E4: {struct _tuple15
_tmp737;struct Cyc_Absyn_Tqual _tmp738;struct Cyc_List_List*_tmp739;struct Cyc_List_List*
_tmp73A;struct _tuple15*_tmp736=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp737=*_tmp736;_tmp738=
_tmp737.f1;_tmp739=_tmp737.f2;_tmp73A=_tmp737.f3;{void*_tmp73B=Cyc_Parse_speclist2typ(
_tmp739,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp73C=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp73D=Cyc_Parse_apply_tms(_tmp738,_tmp73B,_tmp73A,_tmp73C);if(_tmp73D.f3 != 0)({
void*_tmp73E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp73F="bad type params, ignoring";_tag_arr(_tmp73F,sizeof(char),
_get_zero_arr_size(_tmp73F,26));}),_tag_arr(_tmp73E,sizeof(void*),0));});if(
_tmp73D.f4 != 0)({void*_tmp740[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp741="bad specifiers, ignoring";_tag_arr(_tmp741,sizeof(char),
_get_zero_arr_size(_tmp741,25));}),_tag_arr(_tmp740,sizeof(void*),0));});yyval=(
void*)({struct Cyc_ParamDecl_tok_struct*_tmp742=_cycalloc(sizeof(*_tmp742));
_tmp742[0]=({struct Cyc_ParamDecl_tok_struct _tmp743;_tmp743.tag=Cyc_ParamDecl_tok;
_tmp743.f1=({struct _tuple2*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744->f1=0;
_tmp744->f2=_tmp73D.f1;_tmp744->f3=_tmp73D.f2;_tmp744;});_tmp743;});_tmp742;});
break;}}case 236: _LL2E5: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Type_tok_struct _tmp746;
_tmp746.tag=Cyc_Type_tok;_tmp746.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp746;});_tmp745;});
break;case 237: _LL2E6: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp747=_cycalloc(
sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Type_tok_struct _tmp748;_tmp748.tag=Cyc_Type_tok;
_tmp748.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp749=_cycalloc(
sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Absyn_JoinEff_struct _tmp74A;_tmp74A.tag=
20;_tmp74A.f1=0;_tmp74A;});_tmp749;}));_tmp748;});_tmp747;});break;case 238:
_LL2E7: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74B=_cycalloc(sizeof(*
_tmp74B));_tmp74B[0]=({struct Cyc_Type_tok_struct _tmp74C;_tmp74C.tag=Cyc_Type_tok;
_tmp74C.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp74D=_cycalloc(
sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp74E;_tmp74E.tag=
20;_tmp74E.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp74E;});_tmp74D;}));_tmp74C;});_tmp74B;});break;case 239:
_LL2E8: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74F=_cycalloc(sizeof(*
_tmp74F));_tmp74F[0]=({struct Cyc_Type_tok_struct _tmp750;_tmp750.tag=Cyc_Type_tok;
_tmp750.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp751=_cycalloc(
sizeof(*_tmp751));_tmp751[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp752;_tmp752.tag=
21;_tmp752.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp752;});_tmp751;}));_tmp750;});_tmp74F;});break;case
240: _LL2E9: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp753=_cycalloc(sizeof(*
_tmp753));_tmp753[0]=({struct Cyc_Type_tok_struct _tmp754;_tmp754.tag=Cyc_Type_tok;
_tmp754.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp755=_cycalloc(
sizeof(*_tmp755));_tmp755[0]=({struct Cyc_Absyn_JoinEff_struct _tmp756;_tmp756.tag=
20;_tmp756.f1=({struct Cyc_List_List*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp757->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp757;});_tmp756;});_tmp755;}));_tmp754;});_tmp753;});
break;case 241: _LL2EA: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp758=
_cycalloc(sizeof(*_tmp758));_tmp758[0]=({struct Cyc_TypeList_tok_struct _tmp759;
_tmp759.tag=Cyc_TypeList_tok;_tmp759.f1=({struct Cyc_List_List*_tmp75A=_cycalloc(
sizeof(*_tmp75A));_tmp75A->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75A->tl=0;_tmp75A;});
_tmp759;});_tmp758;});break;case 242: _LL2EB: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=({struct Cyc_TypeList_tok_struct
_tmp75C;_tmp75C.tag=Cyc_TypeList_tok;_tmp75C.f1=({struct Cyc_List_List*_tmp75D=
_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75D->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75D;});_tmp75C;});
_tmp75B;});break;case 243: _LL2EC: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp75F;_tmp75F.tag=Cyc_AbstractDeclarator_tok;_tmp75F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp760;});_tmp75F;});
_tmp75E;});break;case 244: _LL2ED: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 245: _LL2EE: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp762;_tmp762.tag=Cyc_AbstractDeclarator_tok;_tmp762.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp763;});
_tmp762;});_tmp761;});break;case 246: _LL2EF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 247: _LL2F0:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp764=_cycalloc(sizeof(*
_tmp764));_tmp764[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp765;_tmp765.tag=
Cyc_AbstractDeclarator_tok;_tmp765.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->tms=({struct Cyc_List_List*_tmp767=
_cycalloc(sizeof(*_tmp767));_tmp767->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp769;_tmp769.tag=0;_tmp769.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp769.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp769;});
_tmp768;}));_tmp767->tl=0;_tmp767;});_tmp766;});_tmp765;});_tmp764;});break;case
248: _LL2F1: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp76B;_tmp76B.tag=Cyc_AbstractDeclarator_tok;_tmp76B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->tms=({struct Cyc_List_List*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp76F;_tmp76F.tag=0;_tmp76F.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp76F;});
_tmp76E;}));_tmp76D->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp76D;});_tmp76C;});
_tmp76B;});_tmp76A;});break;case 249: _LL2F2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp771;_tmp771.tag=Cyc_AbstractDeclarator_tok;_tmp771.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772->tms=({struct Cyc_List_List*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp775;_tmp775.tag=1;_tmp775.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp775.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp775.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp775;});
_tmp774;}));_tmp773->tl=0;_tmp773;});_tmp772;});_tmp771;});_tmp770;});break;case
250: _LL2F3: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp776=
_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp777;_tmp777.tag=Cyc_AbstractDeclarator_tok;_tmp777.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778->tms=({struct Cyc_List_List*_tmp779=
_cycalloc(sizeof(*_tmp779));_tmp779->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp77B;_tmp77B.tag=1;_tmp77B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77B.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77B.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77B;});
_tmp77A;}));_tmp779->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp779;});_tmp778;});
_tmp777;});_tmp776;});break;case 251: _LL2F4: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp77D;_tmp77D.tag=Cyc_AbstractDeclarator_tok;_tmp77D.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->tms=({struct Cyc_List_List*_tmp77F=
_cycalloc(sizeof(*_tmp77F));_tmp77F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp781;_tmp781.tag=3;_tmp781.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp783;_tmp783.tag=1;_tmp783.f1=0;_tmp783.f2=0;_tmp783.f3=0;_tmp783.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp783.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp783;});_tmp782;}));
_tmp781;});_tmp780;}));_tmp77F->tl=0;_tmp77F;});_tmp77E;});_tmp77D;});_tmp77C;});
break;case 252: _LL2F5: {struct _tuple14 _tmp785;struct Cyc_List_List*_tmp786;int
_tmp787;struct Cyc_Absyn_VarargInfo*_tmp788;struct Cyc_Core_Opt*_tmp789;struct Cyc_List_List*
_tmp78A;struct _tuple14*_tmp784=Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp785=*_tmp784;_tmp786=_tmp785.f1;_tmp787=_tmp785.f2;
_tmp788=_tmp785.f3;_tmp789=_tmp785.f4;_tmp78A=_tmp785.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp78C;_tmp78C.tag=Cyc_AbstractDeclarator_tok;
_tmp78C.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp78D=_cycalloc(sizeof(*
_tmp78D));_tmp78D->tms=({struct Cyc_List_List*_tmp78E=_cycalloc(sizeof(*_tmp78E));
_tmp78E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp78F=
_cycalloc(sizeof(*_tmp78F));_tmp78F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp790;_tmp790.tag=3;_tmp790.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp792;_tmp792.tag=1;_tmp792.f1=_tmp786;_tmp792.f2=_tmp787;_tmp792.f3=_tmp788;
_tmp792.f4=_tmp789;_tmp792.f5=_tmp78A;_tmp792;});_tmp791;}));_tmp790;});_tmp78F;}));
_tmp78E->tl=0;_tmp78E;});_tmp78D;});_tmp78C;});_tmp78B;});break;}case 253: _LL2F6:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp793=_cycalloc(sizeof(*
_tmp793));_tmp793[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp794;_tmp794.tag=
Cyc_AbstractDeclarator_tok;_tmp794.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795->tms=({struct Cyc_List_List*_tmp796=
_cycalloc(sizeof(*_tmp796));_tmp796->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp798;_tmp798.tag=3;_tmp798.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp79A;_tmp79A.tag=1;_tmp79A.f1=0;_tmp79A.f2=0;_tmp79A.f3=0;_tmp79A.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp79A.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79A;});_tmp799;}));
_tmp798;});_tmp797;}));_tmp796->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp796;});_tmp795;});
_tmp794;});_tmp793;});break;case 254: _LL2F7: {struct _tuple14 _tmp79C;struct Cyc_List_List*
_tmp79D;int _tmp79E;struct Cyc_Absyn_VarargInfo*_tmp79F;struct Cyc_Core_Opt*_tmp7A0;
struct Cyc_List_List*_tmp7A1;struct _tuple14*_tmp79B=Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79C=*_tmp79B;_tmp79D=
_tmp79C.f1;_tmp79E=_tmp79C.f2;_tmp79F=_tmp79C.f3;_tmp7A0=_tmp79C.f4;_tmp7A1=
_tmp79C.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7A3;_tmp7A3.tag=Cyc_AbstractDeclarator_tok;_tmp7A3.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4->tms=({struct Cyc_List_List*_tmp7A5=
_cycalloc(sizeof(*_tmp7A5));_tmp7A5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp7A7;_tmp7A7.tag=3;_tmp7A7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp7A9;_tmp7A9.tag=1;_tmp7A9.f1=_tmp79D;_tmp7A9.f2=_tmp79E;_tmp7A9.f3=_tmp79F;
_tmp7A9.f4=_tmp7A0;_tmp7A9.f5=_tmp7A1;_tmp7A9;});_tmp7A8;}));_tmp7A7;});_tmp7A6;}));
_tmp7A5->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7A5;});_tmp7A4;});
_tmp7A3;});_tmp7A2;});break;}case 255: _LL2F8: {struct Cyc_List_List*_tmp7AA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp7AC;_tmp7AC.tag=Cyc_AbstractDeclarator_tok;
_tmp7AC.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp7AD=_cycalloc(sizeof(*
_tmp7AD));_tmp7AD->tms=({struct Cyc_List_List*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp7B0;_tmp7B0.tag=4;_tmp7B0.f1=_tmp7AA;_tmp7B0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B0.f3=0;
_tmp7B0;});_tmp7AF;}));_tmp7AE->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7AE;});_tmp7AD;});
_tmp7AC;});_tmp7AB;});break;}case 256: _LL2F9: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7B2;_tmp7B2.tag=Cyc_AbstractDeclarator_tok;_tmp7B2.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3->tms=({struct Cyc_List_List*_tmp7B4=
_cycalloc(sizeof(*_tmp7B4));_tmp7B4->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp7B6;_tmp7B6.tag=5;_tmp7B6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B6.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7B6;});_tmp7B5;}));
_tmp7B4->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp7B4;});_tmp7B3;});
_tmp7B2;});_tmp7B1;});break;case 257: _LL2FA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 258: _LL2FB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 259: _LL2FC:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 260:
_LL2FD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
261: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 262: _LL2FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 263: _LL300: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7B7="`H";
_tag_arr(_tmp7B7,sizeof(char),_get_zero_arr_size(_tmp7B7,3));}))== 0  || Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp7B8="`U";_tag_arr(_tmp7B8,sizeof(char),
_get_zero_arr_size(_tmp7B8,3));}))== 0)Cyc_Parse_err((struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp7BB;_tmp7BB.tag=0;_tmp7BB.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));{void*_tmp7B9[1]={& _tmp7BB};Cyc_aprintf(({const char*_tmp7BA="bad occurrence of heap region %s";
_tag_arr(_tmp7BA,sizeof(char),_get_zero_arr_size(_tmp7BA,33));}),_tag_arr(
_tmp7B9,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6->name=({
struct _tagged_arr*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7C9;});_tmp7C6->identity=
0;_tmp7C6->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7C7=_cycalloc(
sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7C8;_tmp7C8.tag=0;
_tmp7C8.f1=(void*)((void*)3);_tmp7C8;});_tmp7C7;}));_tmp7C6;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));_tmp7C4[0]=({
struct Cyc_Absyn_VarType_struct _tmp7C5;_tmp7C5.tag=1;_tmp7C5.f1=tv;_tmp7C5;});
_tmp7C4;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7BC=_cycalloc(sizeof(*
_tmp7BC));_tmp7BC[0]=({struct Cyc_Stmt_tok_struct _tmp7BD;_tmp7BD.tag=Cyc_Stmt_tok;
_tmp7BD.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7BE=
_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_Region_s_struct _tmp7BF;
_tmp7BF.tag=15;_tmp7BF.f1=tv;_tmp7BF.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0->f1=(void*)0;_tmp7C0->f2=({struct
_tagged_arr*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C1;});_tmp7C0;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));
_tmp7C2[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7C3;_tmp7C3.tag=15;_tmp7C3.f1=(
void*)t;_tmp7C3;});_tmp7C2;}),0);_tmp7BF.f3=0;_tmp7BF.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BF;});_tmp7BE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BD;});
_tmp7BC;});break;}case 264: _LL301: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7CA="H";
_tag_arr(_tmp7CA,sizeof(char),_get_zero_arr_size(_tmp7CA,2));}))== 0)Cyc_Parse_err(({
const char*_tmp7CB="bad occurrence of heap region `H";_tag_arr(_tmp7CB,sizeof(
char),_get_zero_arr_size(_tmp7CB,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));_tmp7D6->name=({
struct _tagged_arr*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp7DC;_tmp7DC.tag=0;_tmp7DC.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7DA[1]={&
_tmp7DC};Cyc_aprintf(({const char*_tmp7DB="`%s";_tag_arr(_tmp7DB,sizeof(char),
_get_zero_arr_size(_tmp7DB,4));}),_tag_arr(_tmp7DA,sizeof(void*),1));}});_tmp7D9;});
_tmp7D6->identity=0;_tmp7D6->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7D8;_tmp7D8.tag=0;_tmp7D8.f1=(void*)((void*)3);_tmp7D8;});_tmp7D7;}));
_tmp7D6;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7D4=_cycalloc(
sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Absyn_VarType_struct _tmp7D5;_tmp7D5.tag=
1;_tmp7D5.f1=tv;_tmp7D5;});_tmp7D4;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({struct Cyc_Stmt_tok_struct
_tmp7CD;_tmp7CD.tag=Cyc_Stmt_tok;_tmp7CD.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7CF;_tmp7CF.tag=15;_tmp7CF.f1=tv;_tmp7CF.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));
_tmp7D0->f1=(void*)0;_tmp7D0->f2=({struct _tagged_arr*_tmp7D1=_cycalloc(sizeof(*
_tmp7D1));_tmp7D1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7D1;});_tmp7D0;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7D3;_tmp7D3.tag=15;_tmp7D3.f1=(void*)t;_tmp7D3;});_tmp7D2;}),0);_tmp7CF.f3=0;
_tmp7CF.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7CF;});_tmp7CE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CD;});
_tmp7CC;});break;}case 265: _LL302: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp7DD="resetable";
_tag_arr(_tmp7DD,sizeof(char),_get_zero_arr_size(_tmp7DD,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7DE="expecting [resetable]";_tag_arr(_tmp7DE,sizeof(char),
_get_zero_arr_size(_tmp7DE,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp7DF="`H";_tag_arr(_tmp7DF,sizeof(char),
_get_zero_arr_size(_tmp7DF,3));}))== 0  || Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7E0="`U";
_tag_arr(_tmp7E0,sizeof(char),_get_zero_arr_size(_tmp7E0,3));})))Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp7E3;_tmp7E3.tag=0;_tmp7E3.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp7E1[1]={&
_tmp7E3};Cyc_aprintf(({const char*_tmp7E2="bad occurrence of heap region %s";
_tag_arr(_tmp7E2,sizeof(char),_get_zero_arr_size(_tmp7E2,33));}),_tag_arr(
_tmp7E1,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE->name=({
struct _tagged_arr*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7F1;});_tmp7EE->identity=
0;_tmp7EE->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7EF=_cycalloc(
sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7F0;_tmp7F0.tag=0;
_tmp7F0.f1=(void*)((void*)3);_tmp7F0;});_tmp7EF;}));_tmp7EE;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({
struct Cyc_Absyn_VarType_struct _tmp7ED;_tmp7ED.tag=1;_tmp7ED.f1=tv;_tmp7ED;});
_tmp7EC;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7E4=_cycalloc(sizeof(*
_tmp7E4));_tmp7E4[0]=({struct Cyc_Stmt_tok_struct _tmp7E5;_tmp7E5.tag=Cyc_Stmt_tok;
_tmp7E5.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7E6=
_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Region_s_struct _tmp7E7;
_tmp7E7.tag=15;_tmp7E7.f1=tv;_tmp7E7.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8->f1=(void*)0;_tmp7E8->f2=({struct
_tagged_arr*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7E9;});_tmp7E8;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));
_tmp7EA[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7EB;_tmp7EB.tag=15;_tmp7EB.f1=(
void*)t;_tmp7EB;});_tmp7EA;}),0);_tmp7E7.f3=1;_tmp7E7.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
_tmp7E4;});break;}case 266: _LL303: if(Cyc_zstrcmp((struct _tagged_arr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7F2="resetable";
_tag_arr(_tmp7F2,sizeof(char),_get_zero_arr_size(_tmp7F2,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7F3="expecting `resetable'";_tag_arr(_tmp7F3,sizeof(char),
_get_zero_arr_size(_tmp7F3,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp7F4="H";_tag_arr(_tmp7F4,sizeof(char),
_get_zero_arr_size(_tmp7F4,2));}))== 0)Cyc_Parse_err(({const char*_tmp7F5="bad occurrence of heap region `H";
_tag_arr(_tmp7F5,sizeof(char),_get_zero_arr_size(_tmp7F5,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800->name=({
struct _tagged_arr*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803[0]=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp806;_tmp806.tag=0;_tmp806.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp804[1]={&
_tmp806};Cyc_aprintf(({const char*_tmp805="`%s";_tag_arr(_tmp805,sizeof(char),
_get_zero_arr_size(_tmp805,4));}),_tag_arr(_tmp804,sizeof(void*),1));}});_tmp803;});
_tmp800->identity=0;_tmp800->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp802;_tmp802.tag=0;_tmp802.f1=(void*)((void*)3);_tmp802;});_tmp801;}));
_tmp800;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7FE=_cycalloc(
sizeof(*_tmp7FE));_tmp7FE[0]=({struct Cyc_Absyn_VarType_struct _tmp7FF;_tmp7FF.tag=
1;_tmp7FF.f1=tv;_tmp7FF;});_tmp7FE;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Stmt_tok_struct
_tmp7F7;_tmp7F7.tag=Cyc_Stmt_tok;_tmp7F7.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7F9;_tmp7F9.tag=15;_tmp7F9.f1=tv;_tmp7F9.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));
_tmp7FA->f1=(void*)0;_tmp7FA->f2=({struct _tagged_arr*_tmp7FB=_cycalloc(sizeof(*
_tmp7FB));_tmp7FB[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7FB;});_tmp7FA;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp7FD;_tmp7FD.tag=15;_tmp7FD.f1=(void*)t;_tmp7FD;});_tmp7FC;}),0);_tmp7F9.f3=1;
_tmp7F9.f4=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F9;});_tmp7F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F7;});
_tmp7F6;});break;}case 267: _LL304: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Stmt_tok_struct
_tmp808;_tmp808.tag=Cyc_Stmt_tok;_tmp808.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp80A;_tmp80A.tag=16;_tmp80A.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp80A;});_tmp809;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp808;});_tmp807;});break;case 268: _LL305: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({
struct Cyc_Stmt_tok_struct _tmp80C;_tmp80C.tag=Cyc_Stmt_tok;_tmp80C.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));
_tmp80D[0]=({struct Cyc_Absyn_Label_s_struct _tmp80E;_tmp80E.tag=12;_tmp80E.f1=({
struct _tagged_arr*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp80F;});_tmp80E.f2=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp80E;});_tmp80D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80C;});
_tmp80B;});break;case 269: _LL306: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp810=
_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Stmt_tok_struct _tmp811;
_tmp811.tag=Cyc_Stmt_tok;_tmp811.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp811;});
_tmp810;});break;case 270: _LL307: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp812=
_cycalloc(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Stmt_tok_struct _tmp813;
_tmp813.tag=Cyc_Stmt_tok;_tmp813.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp813;});
_tmp812;});break;case 271: _LL308: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Stmt_tok_struct _tmp815;
_tmp815.tag=Cyc_Stmt_tok;_tmp815.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp815;});
_tmp814;});break;case 272: _LL309: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 273: _LL30A: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Stmt_tok_struct
_tmp817;_tmp817.tag=Cyc_Stmt_tok;_tmp817.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp817;});_tmp816;});break;case 274: _LL30B: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Stmt_tok_struct
_tmp819;_tmp819.tag=Cyc_Stmt_tok;_tmp819.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp819;});_tmp818;});
break;case 275: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 276: _LL30D: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Stmt_tok_struct
_tmp81B;_tmp81B.tag=Cyc_Stmt_tok;_tmp81B.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81B;});
_tmp81A;});break;case 277: _LL30E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp81C=
_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Stmt_tok_struct _tmp81D;
_tmp81D.tag=Cyc_Stmt_tok;_tmp81D.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp81F;_tmp81F.tag=1;_tmp81F.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81F;});_tmp81E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp81D;});_tmp81C;});break;
case 278: _LL30F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp820=_cycalloc(
sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Stmt_tok_struct _tmp821;_tmp821.tag=Cyc_Stmt_tok;
_tmp821.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp823;_tmp823.tag=1;_tmp823.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp823;});_tmp822;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp821;});_tmp820;});
break;case 279: _LL310: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp824=_cycalloc(
sizeof(*_tmp824));_tmp824[0]=({struct Cyc_Stmt_tok_struct _tmp825;_tmp825.tag=Cyc_Stmt_tok;
_tmp825.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp825;});
_tmp824;});break;case 280: _LL311: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Stmt_tok_struct _tmp827;
_tmp827.tag=Cyc_Stmt_tok;_tmp827.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp827;});
_tmp826;});break;case 281: _LL312: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp828=
_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Stmt_tok_struct _tmp829;
_tmp829.tag=Cyc_Stmt_tok;_tmp829.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp829;});
_tmp828;});break;case 282: _LL313: {struct Cyc_Absyn_Exp*_tmp82A=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp82D=_cycalloc(sizeof(*_tmp82D));
_tmp82D[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp82E;_tmp82E.tag=2;_tmp82E.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp82E;});_tmp82D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({
struct Cyc_Stmt_tok_struct _tmp82C;_tmp82C.tag=Cyc_Stmt_tok;_tmp82C.f1=Cyc_Absyn_switch_stmt(
_tmp82A,Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82C;});
_tmp82B;});break;}case 283: _LL314: {struct Cyc_Absyn_Exp*_tmp82F=Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp830=
_cycalloc(sizeof(*_tmp830));_tmp830[0]=({struct Cyc_Stmt_tok_struct _tmp831;
_tmp831.tag=Cyc_Stmt_tok;_tmp831.f1=Cyc_Absyn_switch_stmt(_tmp82F,Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp831;});
_tmp830;});break;}case 284: _LL315: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Stmt_tok_struct
_tmp833;_tmp833.tag=Cyc_Stmt_tok;_tmp833.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp833;});
_tmp832;});break;case 285: _LL316: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp835;_tmp835.tag=Cyc_SwitchClauseList_tok;_tmp835.f1=0;_tmp835;});_tmp834;});
break;case 286: _LL317: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp837;_tmp837.tag=Cyc_SwitchClauseList_tok;_tmp837.f1=({struct Cyc_List_List*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838->hd=({struct Cyc_Absyn_Switch_clause*
_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp839->pat_vars=0;_tmp839->where_clause=0;
_tmp839->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp839->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp839;});
_tmp838->tl=0;_tmp838;});_tmp837;});_tmp836;});break;case 287: _LL318: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp83A=_cycalloc(sizeof(*_tmp83A));
_tmp83A[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp83B;_tmp83B.tag=Cyc_SwitchClauseList_tok;
_tmp83B.f1=({struct Cyc_List_List*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C->hd=({
struct Cyc_Absyn_Switch_clause*_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp83D->pat_vars=0;_tmp83D->where_clause=0;_tmp83D->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp83D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp83D;});
_tmp83C->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp83C;});_tmp83B;});_tmp83A;});break;case 288: _LL319:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp83E=_cycalloc(sizeof(*
_tmp83E));_tmp83E[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp83F;_tmp83F.tag=
Cyc_SwitchClauseList_tok;_tmp83F.f1=({struct Cyc_List_List*_tmp840=_cycalloc(
sizeof(*_tmp840));_tmp840->hd=({struct Cyc_Absyn_Switch_clause*_tmp841=_cycalloc(
sizeof(*_tmp841));_tmp841->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp841->pat_vars=0;
_tmp841->where_clause=0;_tmp841->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp841->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp841;});
_tmp840->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp840;});_tmp83F;});_tmp83E;});break;case 289: _LL31A:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp842=_cycalloc(sizeof(*
_tmp842));_tmp842[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp843;_tmp843.tag=
Cyc_SwitchClauseList_tok;_tmp843.f1=({struct Cyc_List_List*_tmp844=_cycalloc(
sizeof(*_tmp844));_tmp844->hd=({struct Cyc_Absyn_Switch_clause*_tmp845=_cycalloc(
sizeof(*_tmp845));_tmp845->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp845->pat_vars=0;
_tmp845->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp845->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp845->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp845;});
_tmp844->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp844;});_tmp843;});_tmp842;});break;case 290: _LL31B:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp846=_cycalloc(sizeof(*
_tmp846));_tmp846[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp847;_tmp847.tag=
Cyc_SwitchClauseList_tok;_tmp847.f1=({struct Cyc_List_List*_tmp848=_cycalloc(
sizeof(*_tmp848));_tmp848->hd=({struct Cyc_Absyn_Switch_clause*_tmp849=_cycalloc(
sizeof(*_tmp849));_tmp849->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp849->pat_vars=0;
_tmp849->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp849->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp849->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp849;});
_tmp848->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp848;});_tmp847;});_tmp846;});break;case 291: _LL31C:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));
_tmp84A[0]=({struct Cyc_Stmt_tok_struct _tmp84B;_tmp84B.tag=Cyc_Stmt_tok;_tmp84B.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp84B;});_tmp84A;});break;case 292: _LL31D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({
struct Cyc_Stmt_tok_struct _tmp84D;_tmp84D.tag=Cyc_Stmt_tok;_tmp84D.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp84D;});_tmp84C;});break;case 293: _LL31E: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({
struct Cyc_Stmt_tok_struct _tmp84F;_tmp84F.tag=Cyc_Stmt_tok;_tmp84F.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
_tmp84E;});break;case 294: _LL31F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp850=
_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_Stmt_tok_struct _tmp851;
_tmp851.tag=Cyc_Stmt_tok;_tmp851.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
_tmp850;});break;case 295: _LL320: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp852=
_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Stmt_tok_struct _tmp853;
_tmp853.tag=Cyc_Stmt_tok;_tmp853.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
_tmp852;});break;case 296: _LL321: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp854=
_cycalloc(sizeof(*_tmp854));_tmp854[0]=({struct Cyc_Stmt_tok_struct _tmp855;
_tmp855.tag=Cyc_Stmt_tok;_tmp855.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp855;});
_tmp854;});break;case 297: _LL322: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp856=
_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_Stmt_tok_struct _tmp857;
_tmp857.tag=Cyc_Stmt_tok;_tmp857.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
_tmp856;});break;case 298: _LL323: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp858=
_cycalloc(sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Stmt_tok_struct _tmp859;
_tmp859.tag=Cyc_Stmt_tok;_tmp859.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
_tmp858;});break;case 299: _LL324: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85A=
_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Stmt_tok_struct _tmp85B;
_tmp85B.tag=Cyc_Stmt_tok;_tmp85B.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
_tmp85A;});break;case 300: _LL325: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85C=
_cycalloc(sizeof(*_tmp85C));_tmp85C[0]=({struct Cyc_Stmt_tok_struct _tmp85D;
_tmp85D.tag=Cyc_Stmt_tok;_tmp85D.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
_tmp85C;});break;case 301: _LL326: {struct Cyc_List_List*_tmp85E=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp85F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Stmt_tok_struct _tmp861;
_tmp861.tag=Cyc_Stmt_tok;_tmp861.f1=Cyc_Parse_flatten_declarations(_tmp85E,
_tmp85F);_tmp861;});_tmp860;});break;}case 302: _LL327: {struct Cyc_List_List*
_tmp862=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp863=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864[0]=({
struct Cyc_Stmt_tok_struct _tmp865;_tmp865.tag=Cyc_Stmt_tok;_tmp865.f1=Cyc_Parse_flatten_declarations(
_tmp862,_tmp863);_tmp865;});_tmp864;});break;}case 303: _LL328: {struct Cyc_List_List*
_tmp866=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp867=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868[0]=({
struct Cyc_Stmt_tok_struct _tmp869;_tmp869.tag=Cyc_Stmt_tok;_tmp869.f1=Cyc_Parse_flatten_declarations(
_tmp866,_tmp867);_tmp869;});_tmp868;});break;}case 304: _LL329: {struct Cyc_List_List*
_tmp86A=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp86B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({
struct Cyc_Stmt_tok_struct _tmp86D;_tmp86D.tag=Cyc_Stmt_tok;_tmp86D.f1=Cyc_Parse_flatten_declarations(
_tmp86A,_tmp86B);_tmp86D;});_tmp86C;});break;}case 305: _LL32A: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({
struct Cyc_Stmt_tok_struct _tmp86F;_tmp86F.tag=Cyc_Stmt_tok;_tmp86F.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp870;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp86F;});
_tmp86E;});break;case 306: _LL32B: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp871=
_cycalloc(sizeof(*_tmp871));_tmp871[0]=({struct Cyc_Stmt_tok_struct _tmp872;
_tmp872.tag=Cyc_Stmt_tok;_tmp872.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp872;});
_tmp871;});break;case 307: _LL32C: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp873=
_cycalloc(sizeof(*_tmp873));_tmp873[0]=({struct Cyc_Stmt_tok_struct _tmp874;
_tmp874.tag=Cyc_Stmt_tok;_tmp874.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp874;});
_tmp873;});break;case 308: _LL32D: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp875=
_cycalloc(sizeof(*_tmp875));_tmp875[0]=({struct Cyc_Stmt_tok_struct _tmp876;
_tmp876.tag=Cyc_Stmt_tok;_tmp876.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp876;});
_tmp875;});break;case 309: _LL32E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp877=
_cycalloc(sizeof(*_tmp877));_tmp877[0]=({struct Cyc_Stmt_tok_struct _tmp878;
_tmp878.tag=Cyc_Stmt_tok;_tmp878.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp878;});_tmp877;});break;case 310: _LL32F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({
struct Cyc_Stmt_tok_struct _tmp87A;_tmp87A.tag=Cyc_Stmt_tok;_tmp87A.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp87A;});_tmp879;});break;case 311: _LL330: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp87B=_cycalloc(sizeof(*_tmp87B));_tmp87B[0]=({
struct Cyc_Stmt_tok_struct _tmp87C;_tmp87C.tag=Cyc_Stmt_tok;_tmp87C.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp87C;});_tmp87B;});break;case 312: _LL331: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=({
struct Cyc_Stmt_tok_struct _tmp87E;_tmp87E.tag=Cyc_Stmt_tok;_tmp87E.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp87E;});_tmp87D;});break;case 313: _LL332: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 314: _LL333:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 315:
_LL334: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp87F=_cycalloc(sizeof(*
_tmp87F));_tmp87F[0]=({struct Cyc_Pattern_tok_struct _tmp880;_tmp880.tag=Cyc_Pattern_tok;
_tmp880.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp880;});
_tmp87F;});break;case 316: _LL335: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 317: _LL336: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct Cyc_Pattern_tok_struct
_tmp882;_tmp882.tag=Cyc_Pattern_tok;_tmp882.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp882;});
_tmp881;});break;case 318: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 319: _LL338: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[0]=({struct Cyc_Pattern_tok_struct
_tmp884;_tmp884.tag=Cyc_Pattern_tok;_tmp884.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp884;});
_tmp883;});break;case 320: _LL339: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 321: _LL33A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885[0]=({struct Cyc_Pattern_tok_struct
_tmp886;_tmp886.tag=Cyc_Pattern_tok;_tmp886.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp886;});
_tmp885;});break;case 322: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 323: _LL33C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Pattern_tok_struct
_tmp888;_tmp888.tag=Cyc_Pattern_tok;_tmp888.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp888;});
_tmp887;});break;case 324: _LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL33E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({struct Cyc_Pattern_tok_struct
_tmp88A;_tmp88A.tag=Cyc_Pattern_tok;_tmp88A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88A;});
_tmp889;});break;case 326: _LL33F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL340: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Pattern_tok_struct
_tmp88C;_tmp88C.tag=Cyc_Pattern_tok;_tmp88C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88C;});
_tmp88B;});break;case 328: _LL341: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Pattern_tok_struct
_tmp88E;_tmp88E.tag=Cyc_Pattern_tok;_tmp88E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88E;});
_tmp88D;});break;case 329: _LL342: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 330: _LL343: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Pattern_tok_struct
_tmp890;_tmp890.tag=Cyc_Pattern_tok;_tmp890.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp890;});
_tmp88F;});break;case 331: _LL344: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Pattern_tok_struct
_tmp892;_tmp892.tag=Cyc_Pattern_tok;_tmp892.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp892;});
_tmp891;});break;case 332: _LL345: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Pattern_tok_struct
_tmp894;_tmp894.tag=Cyc_Pattern_tok;_tmp894.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp894;});
_tmp893;});break;case 333: _LL346: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Pattern_tok_struct
_tmp896;_tmp896.tag=Cyc_Pattern_tok;_tmp896.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp896;});
_tmp895;});break;case 334: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL348: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Pattern_tok_struct
_tmp898;_tmp898.tag=Cyc_Pattern_tok;_tmp898.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp898;});
_tmp897;});break;case 336: _LL349: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=({struct Cyc_Pattern_tok_struct
_tmp89A;_tmp89A.tag=Cyc_Pattern_tok;_tmp89A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89A;});
_tmp899;});break;case 337: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL34B: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Pattern_tok_struct
_tmp89C;_tmp89C.tag=Cyc_Pattern_tok;_tmp89C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89C;});
_tmp89B;});break;case 339: _LL34C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=({struct Cyc_Pattern_tok_struct
_tmp89E;_tmp89E.tag=Cyc_Pattern_tok;_tmp89E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89E;});
_tmp89D;});break;case 340: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL34E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Pattern_tok_struct
_tmp8A0;_tmp8A0.tag=Cyc_Pattern_tok;_tmp8A0.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A0;});
_tmp89F;});break;case 342: _LL34F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct Cyc_Pattern_tok_struct
_tmp8A2;_tmp8A2.tag=Cyc_Pattern_tok;_tmp8A2.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A2;});
_tmp8A1;});break;case 343: _LL350: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Pattern_tok_struct
_tmp8A4;_tmp8A4.tag=Cyc_Pattern_tok;_tmp8A4.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A4;});
_tmp8A3;});break;case 344: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL352: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Pattern_tok_struct
_tmp8A6;_tmp8A6.tag=Cyc_Pattern_tok;_tmp8A6.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)])).f3,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A6;});
_tmp8A5;});break;case 346: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 347: _LL354: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Pattern_tok_struct
_tmp8A8;_tmp8A8.tag=Cyc_Pattern_tok;_tmp8A8.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp8A8;});_tmp8A7;});
break;case 348: _LL355: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Pattern_tok_struct _tmp8AA;
_tmp8AA.tag=Cyc_Pattern_tok;_tmp8AA.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8AA;});
_tmp8A9;});break;case 349: _LL356: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Pattern_tok_struct
_tmp8AC;_tmp8AC.tag=Cyc_Pattern_tok;_tmp8AC.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8AC;});_tmp8AB;});break;case 350: _LL357: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[
0]=({struct Cyc_Pattern_tok_struct _tmp8AE;_tmp8AE.tag=Cyc_Pattern_tok;_tmp8AE.f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8AE;});
_tmp8AD;});break;case 351: _LL358: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Pattern_tok_struct
_tmp8B0;_tmp8B0.tag=Cyc_Pattern_tok;_tmp8B0.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp8B1=_cycalloc(sizeof(*
_tmp8B1));_tmp8B1[0]=({struct Cyc_Absyn_StructField_struct _tmp8B2;_tmp8B2.tag=0;
_tmp8B2.f1=({struct _tagged_arr*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8B3;});_tmp8B2;});_tmp8B1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8B0;});
_tmp8AF;});break;case 352: _LL359: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4[0]=({struct Cyc_Pattern_tok_struct
_tmp8B5;_tmp8B5.tag=Cyc_Pattern_tok;_tmp8B5.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp8B6=_cycalloc_atomic(
sizeof(*_tmp8B6));_tmp8B6[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp8B7;_tmp8B7.tag=
1;_tmp8B7.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8B7;});_tmp8B6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8B5;});_tmp8B4;});break;case 353: _LL35A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 354: _LL35B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 355:
_LL35C: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8B8=_cycalloc(sizeof(*
_tmp8B8));_tmp8B8[0]=({struct Cyc_Pattern_tok_struct _tmp8B9;_tmp8B9.tag=Cyc_Pattern_tok;
_tmp8B9.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B9;});
_tmp8B8;});break;case 356: _LL35D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8BA=_cycalloc(sizeof(*_tmp8BA));_tmp8BA[0]=({struct Cyc_Pattern_tok_struct
_tmp8BB;_tmp8BB.tag=Cyc_Pattern_tok;_tmp8BB.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8BB;});_tmp8BA;});
break;case 357: _LL35E: {struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp8BC=(void*)e->r;
void*_tmp8BD;void*_tmp8BE;char _tmp8BF;void*_tmp8C0;void*_tmp8C1;short _tmp8C2;
void*_tmp8C3;void*_tmp8C4;int _tmp8C5;void*_tmp8C6;struct _tagged_arr _tmp8C7;void*
_tmp8C8;void*_tmp8C9;void*_tmp8CA;_LL361: if(*((int*)_tmp8BC)!= 0)goto _LL363;
_tmp8BD=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8BC)->f1;if(_tmp8BD <= (void*)
1  || *((int*)_tmp8BD)!= 0)goto _LL363;_tmp8BE=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp8BD)->f1;_tmp8BF=((struct Cyc_Absyn_Char_c_struct*)_tmp8BD)->f2;_LL362: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[
0]=({struct Cyc_Pattern_tok_struct _tmp8CC;_tmp8CC.tag=Cyc_Pattern_tok;_tmp8CC.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*_tmp8CD=
_cycalloc_atomic(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Absyn_Char_p_struct
_tmp8CE;_tmp8CE.tag=8;_tmp8CE.f1=_tmp8BF;_tmp8CE;});_tmp8CD;}),e->loc);_tmp8CC;});
_tmp8CB;});goto _LL360;_LL363: if(*((int*)_tmp8BC)!= 0)goto _LL365;_tmp8C0=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8BC)->f1;if(_tmp8C0 <= (void*)1  || *((int*)
_tmp8C0)!= 1)goto _LL365;_tmp8C1=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp8C0)->f1;
_tmp8C2=((struct Cyc_Absyn_Short_c_struct*)_tmp8C0)->f2;_LL364: yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({
struct Cyc_Pattern_tok_struct _tmp8D0;_tmp8D0.tag=Cyc_Pattern_tok;_tmp8D0.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1[
0]=({struct Cyc_Absyn_Int_p_struct _tmp8D2;_tmp8D2.tag=7;_tmp8D2.f1=(void*)_tmp8C1;
_tmp8D2.f2=(int)_tmp8C2;_tmp8D2;});_tmp8D1;}),e->loc);_tmp8D0;});_tmp8CF;});goto
_LL360;_LL365: if(*((int*)_tmp8BC)!= 0)goto _LL367;_tmp8C3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8BC)->f1;if(_tmp8C3 <= (void*)1  || *((int*)_tmp8C3)!= 2)goto _LL367;_tmp8C4=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp8C3)->f1;_tmp8C5=((struct Cyc_Absyn_Int_c_struct*)
_tmp8C3)->f2;_LL366: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8D3=
_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_Pattern_tok_struct _tmp8D4;
_tmp8D4.tag=Cyc_Pattern_tok;_tmp8D4.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5[0]=({struct Cyc_Absyn_Int_p_struct
_tmp8D6;_tmp8D6.tag=7;_tmp8D6.f1=(void*)_tmp8C4;_tmp8D6.f2=_tmp8C5;_tmp8D6;});
_tmp8D5;}),e->loc);_tmp8D4;});_tmp8D3;});goto _LL360;_LL367: if(*((int*)_tmp8BC)!= 
0)goto _LL369;_tmp8C6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8BC)->f1;if(
_tmp8C6 <= (void*)1  || *((int*)_tmp8C6)!= 4)goto _LL369;_tmp8C7=((struct Cyc_Absyn_Float_c_struct*)
_tmp8C6)->f1;_LL368: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8D7=
_cycalloc(sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_Pattern_tok_struct _tmp8D8;
_tmp8D8.tag=Cyc_Pattern_tok;_tmp8D8.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*
_tmp8D9=_cycalloc(sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_Absyn_Float_p_struct
_tmp8DA;_tmp8DA.tag=9;_tmp8DA.f1=_tmp8C7;_tmp8DA;});_tmp8D9;}),e->loc);_tmp8D8;});
_tmp8D7;});goto _LL360;_LL369: if(*((int*)_tmp8BC)!= 0)goto _LL36B;_tmp8C8=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8BC)->f1;if((int)_tmp8C8 != 0)goto _LL36B;
_LL36A: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8DB=_cycalloc(sizeof(*
_tmp8DB));_tmp8DB[0]=({struct Cyc_Pattern_tok_struct _tmp8DC;_tmp8DC.tag=Cyc_Pattern_tok;
_tmp8DC.f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp8DC;});_tmp8DB;});goto _LL360;
_LL36B: if(*((int*)_tmp8BC)!= 0)goto _LL36D;_tmp8C9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8BC)->f1;if(_tmp8C9 <= (void*)1  || *((int*)_tmp8C9)!= 5)goto _LL36D;_LL36C: Cyc_Parse_err(({
const char*_tmp8DD="strings cannot occur within patterns";_tag_arr(_tmp8DD,
sizeof(char),_get_zero_arr_size(_tmp8DD,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL360;
_LL36D: if(*((int*)_tmp8BC)!= 0)goto _LL36F;_tmp8CA=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8BC)->f1;if(_tmp8CA <= (void*)1  || *((int*)_tmp8CA)!= 3)goto _LL36F;_LL36E: Cyc_Parse_unimp(({
const char*_tmp8DE="long long's in patterns";_tag_arr(_tmp8DE,sizeof(char),
_get_zero_arr_size(_tmp8DE,24));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL360;
_LL36F:;_LL370: Cyc_Parse_err(({const char*_tmp8DF="bad constant in case";_tag_arr(
_tmp8DF,sizeof(char),_get_zero_arr_size(_tmp8DF,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL360:;}break;}
case 358: _LL35F: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8E0=_cycalloc(
sizeof(*_tmp8E0));_tmp8E0[0]=({struct Cyc_Pattern_tok_struct _tmp8E1;_tmp8E1.tag=
Cyc_Pattern_tok;_tmp8E1.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp8E2=_cycalloc(sizeof(*_tmp8E2));_tmp8E2[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp8E3;_tmp8E3.tag=12;_tmp8E3.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8E3;});_tmp8E2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E1;});
_tmp8E0;});break;case 359: _LL371: {struct Cyc_List_List*_tmp8E5;int _tmp8E6;struct
_tuple17 _tmp8E4=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8E5=_tmp8E4.f1;_tmp8E6=_tmp8E4.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({struct
Cyc_Pattern_tok_struct _tmp8E8;_tmp8E8.tag=Cyc_Pattern_tok;_tmp8E8.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp8E9=_cycalloc(sizeof(*_tmp8E9));
_tmp8E9[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp8EA;_tmp8EA.tag=3;_tmp8EA.f1=
_tmp8E5;_tmp8EA.f2=_tmp8E6;_tmp8EA;});_tmp8E9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E8;});
_tmp8E7;});break;}case 360: _LL372: {struct Cyc_List_List*_tmp8EC;int _tmp8ED;struct
_tuple17 _tmp8EB=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8EC=_tmp8EB.f1;_tmp8ED=_tmp8EB.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE[0]=({struct
Cyc_Pattern_tok_struct _tmp8EF;_tmp8EF.tag=Cyc_Pattern_tok;_tmp8EF.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp8F0=_cycalloc(sizeof(*_tmp8F0));
_tmp8F0[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp8F1;_tmp8F1.tag=13;_tmp8F1.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp8F1.f2=_tmp8EC;_tmp8F1.f3=_tmp8ED;_tmp8F1;});_tmp8F0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8EF;});
_tmp8EE;});break;}case 361: _LL373: {struct Cyc_List_List*_tmp8F3;int _tmp8F4;struct
_tuple17 _tmp8F2=*Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8F3=_tmp8F2.f1;_tmp8F4=_tmp8F2.f2;{struct Cyc_List_List*
_tmp8F5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8F6=_cycalloc(sizeof(*_tmp8F6));_tmp8F6[0]=({struct
Cyc_Pattern_tok_struct _tmp8F7;_tmp8F7.tag=Cyc_Pattern_tok;_tmp8F7.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp8F8=_cycalloc(sizeof(*_tmp8F8));
_tmp8F8[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp8F9;_tmp8F9.tag=5;_tmp8F9.f1=({
struct Cyc_Absyn_AggrInfo _tmp8FA;_tmp8FA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp8FC;_tmp8FC.tag=0;_tmp8FC.f1=(void*)((void*)0);_tmp8FC.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8FC;});_tmp8FB;}));
_tmp8FA.targs=0;_tmp8FA;});_tmp8F9.f2=_tmp8F5;_tmp8F9.f3=_tmp8F3;_tmp8F9.f4=
_tmp8F4;_tmp8F9;});_tmp8F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F7;});
_tmp8F6;});break;}}case 362: _LL374: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8FD=_cycalloc(sizeof(*_tmp8FD));_tmp8FD[0]=({struct Cyc_Pattern_tok_struct
_tmp8FE;_tmp8FE.tag=Cyc_Pattern_tok;_tmp8FE.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp900;_tmp900.tag=4;_tmp900.f1=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp900;});_tmp8FF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8FE;});_tmp8FD;});break;case 363: _LL375: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp901=_cycalloc(sizeof(*_tmp901));_tmp901[
0]=({struct Cyc_Pattern_tok_struct _tmp902;_tmp902.tag=Cyc_Pattern_tok;_tmp902.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp903=_cycalloc(
sizeof(*_tmp903));_tmp903[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp904;_tmp904.tag=
4;_tmp904.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp905=
_cycalloc(sizeof(*_tmp905));_tmp905[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp906;
_tmp906.tag=4;_tmp906.f1=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp906;});_tmp905;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp904;});
_tmp903;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp902;});_tmp901;});break;case 364: _LL376: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907[
0]=({struct Cyc_Pattern_tok_struct _tmp908;_tmp908.tag=Cyc_Pattern_tok;_tmp908.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp909=_cycalloc(
sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Absyn_Reference_p_struct _tmp90A;_tmp90A.tag=
1;_tmp90A.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp90B=_cycalloc(sizeof(*
_tmp90B));_tmp90B->f1=(void*)0;_tmp90B->f2=({struct _tagged_arr*_tmp90C=_cycalloc(
sizeof(*_tmp90C));_tmp90C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90C;});_tmp90B;}),(void*)
0,0);_tmp90A;});_tmp909;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp908;});
_tmp907;});break;case 365: _LL377: {void*_tmp90D=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp916=_cycalloc(sizeof(*_tmp916));_tmp916[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp917;_tmp917.tag=0;_tmp917.f1=(void*)((void*)5);_tmp917;});_tmp916;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[
0]=({struct Cyc_Pattern_tok_struct _tmp90F;_tmp90F.tag=Cyc_Pattern_tok;_tmp90F.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp910=_cycalloc(
sizeof(*_tmp910));_tmp910[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp911;_tmp911.tag=
2;_tmp911.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp90D);
_tmp911.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp912=_cycalloc(sizeof(*
_tmp912));_tmp912->f1=(void*)0;_tmp912->f2=({struct _tagged_arr*_tmp913=_cycalloc(
sizeof(*_tmp913));_tmp913[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp913;});_tmp912;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp914=_cycalloc(sizeof(*_tmp914));
_tmp914[0]=({struct Cyc_Absyn_TagType_struct _tmp915;_tmp915.tag=17;_tmp915.f1=(
void*)_tmp90D;_tmp915;});_tmp914;}),0);_tmp911;});_tmp910;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp90F;});
_tmp90E;});break;}case 366: _LL378: {struct Cyc_Absyn_Tvar*_tmp918=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp923=_cycalloc(sizeof(*_tmp923));_tmp923[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp924;_tmp924.tag=0;_tmp924.f1=(void*)((void*)
5);_tmp924;});_tmp923;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp919=
_cycalloc(sizeof(*_tmp919));_tmp919[0]=({struct Cyc_Pattern_tok_struct _tmp91A;
_tmp91A.tag=Cyc_Pattern_tok;_tmp91A.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp91B=_cycalloc(sizeof(*_tmp91B));_tmp91B[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp91C;_tmp91C.tag=2;_tmp91C.f1=_tmp918;_tmp91C.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp91D=_cycalloc(sizeof(*_tmp91D));_tmp91D->f1=(void*)0;_tmp91D->f2=({
struct _tagged_arr*_tmp91E=_cycalloc(sizeof(*_tmp91E));_tmp91E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp91E;});_tmp91D;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp91F=_cycalloc(sizeof(*_tmp91F));
_tmp91F[0]=({struct Cyc_Absyn_TagType_struct _tmp920;_tmp920.tag=17;_tmp920.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp921=_cycalloc(sizeof(*_tmp921));
_tmp921[0]=({struct Cyc_Absyn_VarType_struct _tmp922;_tmp922.tag=1;_tmp922.f1=
_tmp918;_tmp922;});_tmp921;}));_tmp920;});_tmp91F;}),0);_tmp91C;});_tmp91B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp91A;});_tmp919;});break;}case 367: _LL379: yyval=(
void*)({struct Cyc_YY3_struct*_tmp925=_cycalloc(sizeof(*_tmp925));_tmp925[0]=({
struct Cyc_YY3_struct _tmp926;_tmp926.tag=Cyc_YY3;_tmp926.f1=({struct _tuple17*
_tmp927=_cycalloc(sizeof(*_tmp927));_tmp927->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp927->f2=0;_tmp927;});
_tmp926;});_tmp925;});break;case 368: _LL37A: yyval=(void*)({struct Cyc_YY3_struct*
_tmp928=_cycalloc(sizeof(*_tmp928));_tmp928[0]=({struct Cyc_YY3_struct _tmp929;
_tmp929.tag=Cyc_YY3;_tmp929.f1=({struct _tuple17*_tmp92A=_cycalloc(sizeof(*
_tmp92A));_tmp92A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]));_tmp92A->f2=1;_tmp92A;});_tmp929;});_tmp928;});break;case 369: _LL37B:
yyval=(void*)({struct Cyc_YY3_struct*_tmp92B=_cycalloc(sizeof(*_tmp92B));_tmp92B[
0]=({struct Cyc_YY3_struct _tmp92C;_tmp92C.tag=Cyc_YY3;_tmp92C.f1=({struct _tuple17*
_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D->f1=0;_tmp92D->f2=1;_tmp92D;});
_tmp92C;});_tmp92B;});break;case 370: _LL37C: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp92E=_cycalloc(sizeof(*_tmp92E));_tmp92E[0]=({struct Cyc_PatternList_tok_struct
_tmp92F;_tmp92F.tag=Cyc_PatternList_tok;_tmp92F.f1=({struct Cyc_List_List*_tmp930=
_cycalloc(sizeof(*_tmp930));_tmp930->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp930->tl=0;_tmp930;});
_tmp92F;});_tmp92E;});break;case 371: _LL37D: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp931=_cycalloc(sizeof(*_tmp931));_tmp931[0]=({struct Cyc_PatternList_tok_struct
_tmp932;_tmp932.tag=Cyc_PatternList_tok;_tmp932.f1=({struct Cyc_List_List*_tmp933=
_cycalloc(sizeof(*_tmp933));_tmp933->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp933->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp933;});_tmp932;});
_tmp931;});break;case 372: _LL37E: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934[0]=({struct Cyc_FieldPattern_tok_struct
_tmp935;_tmp935.tag=Cyc_FieldPattern_tok;_tmp935.f1=({struct _tuple11*_tmp936=
_cycalloc(sizeof(*_tmp936));_tmp936->f1=0;_tmp936->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp936;});_tmp935;});
_tmp934;});break;case 373: _LL37F: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp937=_cycalloc(sizeof(*_tmp937));_tmp937[0]=({struct Cyc_FieldPattern_tok_struct
_tmp938;_tmp938.tag=Cyc_FieldPattern_tok;_tmp938.f1=({struct _tuple11*_tmp939=
_cycalloc(sizeof(*_tmp939));_tmp939->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp939->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp939;});_tmp938;});
_tmp937;});break;case 374: _LL380: yyval=(void*)({struct Cyc_YY4_struct*_tmp93A=
_cycalloc(sizeof(*_tmp93A));_tmp93A[0]=({struct Cyc_YY4_struct _tmp93B;_tmp93B.tag=
Cyc_YY4;_tmp93B.f1=({struct _tuple17*_tmp93C=_cycalloc(sizeof(*_tmp93C));_tmp93C->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp93C->f2=0;_tmp93C;});
_tmp93B;});_tmp93A;});break;case 375: _LL381: yyval=(void*)({struct Cyc_YY4_struct*
_tmp93D=_cycalloc(sizeof(*_tmp93D));_tmp93D[0]=({struct Cyc_YY4_struct _tmp93E;
_tmp93E.tag=Cyc_YY4;_tmp93E.f1=({struct _tuple17*_tmp93F=_cycalloc(sizeof(*
_tmp93F));_tmp93F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp93F->f2=1;_tmp93F;});_tmp93E;});_tmp93D;});break;case 376:
_LL382: yyval=(void*)({struct Cyc_YY4_struct*_tmp940=_cycalloc(sizeof(*_tmp940));
_tmp940[0]=({struct Cyc_YY4_struct _tmp941;_tmp941.tag=Cyc_YY4;_tmp941.f1=({struct
_tuple17*_tmp942=_cycalloc(sizeof(*_tmp942));_tmp942->f1=0;_tmp942->f2=1;_tmp942;});
_tmp941;});_tmp940;});break;case 377: _LL383: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp943=_cycalloc(sizeof(*_tmp943));_tmp943[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp944;_tmp944.tag=Cyc_FieldPatternList_tok;_tmp944.f1=({struct Cyc_List_List*
_tmp945=_cycalloc(sizeof(*_tmp945));_tmp945->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp945->tl=0;_tmp945;});
_tmp944;});_tmp943;});break;case 378: _LL384: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp946=_cycalloc(sizeof(*_tmp946));_tmp946[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp947;_tmp947.tag=Cyc_FieldPatternList_tok;_tmp947.f1=({struct Cyc_List_List*
_tmp948=_cycalloc(sizeof(*_tmp948));_tmp948->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp948->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp948;});_tmp947;});
_tmp946;});break;case 379: _LL385: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 380: _LL386: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp949=_cycalloc(sizeof(*_tmp949));_tmp949[0]=({struct Cyc_Exp_tok_struct _tmp94A;
_tmp94A.tag=Cyc_Exp_tok;_tmp94A.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94A;});
_tmp949;});break;case 381: _LL387: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 382: _LL388: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp94B=_cycalloc(sizeof(*_tmp94B));_tmp94B[0]=({struct Cyc_Exp_tok_struct _tmp94C;
_tmp94C.tag=Cyc_Exp_tok;_tmp94C.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp94C;});
_tmp94B;});break;case 383: _LL389: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp94D=_cycalloc(sizeof(*_tmp94D));_tmp94D[0]=({struct Cyc_Primopopt_tok_struct
_tmp94E;_tmp94E.tag=Cyc_Primopopt_tok;_tmp94E.f1=0;_tmp94E;});_tmp94D;});break;
case 384: _LL38A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp94F=_cycalloc(
sizeof(*_tmp94F));_tmp94F[0]=({struct Cyc_Primopopt_tok_struct _tmp950;_tmp950.tag=
Cyc_Primopopt_tok;_tmp950.f1=({struct Cyc_Core_Opt*_tmp951=_cycalloc(sizeof(*
_tmp951));_tmp951->v=(void*)((void*)1);_tmp951;});_tmp950;});_tmp94F;});break;
case 385: _LL38B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp952=_cycalloc(
sizeof(*_tmp952));_tmp952[0]=({struct Cyc_Primopopt_tok_struct _tmp953;_tmp953.tag=
Cyc_Primopopt_tok;_tmp953.f1=({struct Cyc_Core_Opt*_tmp954=_cycalloc(sizeof(*
_tmp954));_tmp954->v=(void*)((void*)3);_tmp954;});_tmp953;});_tmp952;});break;
case 386: _LL38C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp955=_cycalloc(
sizeof(*_tmp955));_tmp955[0]=({struct Cyc_Primopopt_tok_struct _tmp956;_tmp956.tag=
Cyc_Primopopt_tok;_tmp956.f1=({struct Cyc_Core_Opt*_tmp957=_cycalloc(sizeof(*
_tmp957));_tmp957->v=(void*)((void*)4);_tmp957;});_tmp956;});_tmp955;});break;
case 387: _LL38D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp958=_cycalloc(
sizeof(*_tmp958));_tmp958[0]=({struct Cyc_Primopopt_tok_struct _tmp959;_tmp959.tag=
Cyc_Primopopt_tok;_tmp959.f1=({struct Cyc_Core_Opt*_tmp95A=_cycalloc(sizeof(*
_tmp95A));_tmp95A->v=(void*)((void*)0);_tmp95A;});_tmp959;});_tmp958;});break;
case 388: _LL38E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp95B=_cycalloc(
sizeof(*_tmp95B));_tmp95B[0]=({struct Cyc_Primopopt_tok_struct _tmp95C;_tmp95C.tag=
Cyc_Primopopt_tok;_tmp95C.f1=({struct Cyc_Core_Opt*_tmp95D=_cycalloc(sizeof(*
_tmp95D));_tmp95D->v=(void*)((void*)2);_tmp95D;});_tmp95C;});_tmp95B;});break;
case 389: _LL38F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp95E=_cycalloc(
sizeof(*_tmp95E));_tmp95E[0]=({struct Cyc_Primopopt_tok_struct _tmp95F;_tmp95F.tag=
Cyc_Primopopt_tok;_tmp95F.f1=({struct Cyc_Core_Opt*_tmp960=_cycalloc(sizeof(*
_tmp960));_tmp960->v=(void*)((void*)16);_tmp960;});_tmp95F;});_tmp95E;});break;
case 390: _LL390: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp961=_cycalloc(
sizeof(*_tmp961));_tmp961[0]=({struct Cyc_Primopopt_tok_struct _tmp962;_tmp962.tag=
Cyc_Primopopt_tok;_tmp962.f1=({struct Cyc_Core_Opt*_tmp963=_cycalloc(sizeof(*
_tmp963));_tmp963->v=(void*)((void*)17);_tmp963;});_tmp962;});_tmp961;});break;
case 391: _LL391: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp964=_cycalloc(
sizeof(*_tmp964));_tmp964[0]=({struct Cyc_Primopopt_tok_struct _tmp965;_tmp965.tag=
Cyc_Primopopt_tok;_tmp965.f1=({struct Cyc_Core_Opt*_tmp966=_cycalloc(sizeof(*
_tmp966));_tmp966->v=(void*)((void*)13);_tmp966;});_tmp965;});_tmp964;});break;
case 392: _LL392: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp967=_cycalloc(
sizeof(*_tmp967));_tmp967[0]=({struct Cyc_Primopopt_tok_struct _tmp968;_tmp968.tag=
Cyc_Primopopt_tok;_tmp968.f1=({struct Cyc_Core_Opt*_tmp969=_cycalloc(sizeof(*
_tmp969));_tmp969->v=(void*)((void*)15);_tmp969;});_tmp968;});_tmp967;});break;
case 393: _LL393: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp96A=_cycalloc(
sizeof(*_tmp96A));_tmp96A[0]=({struct Cyc_Primopopt_tok_struct _tmp96B;_tmp96B.tag=
Cyc_Primopopt_tok;_tmp96B.f1=({struct Cyc_Core_Opt*_tmp96C=_cycalloc(sizeof(*
_tmp96C));_tmp96C->v=(void*)((void*)14);_tmp96C;});_tmp96B;});_tmp96A;});break;
case 394: _LL394: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 395: _LL395: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96D=_cycalloc(
sizeof(*_tmp96D));_tmp96D[0]=({struct Cyc_Exp_tok_struct _tmp96E;_tmp96E.tag=Cyc_Exp_tok;
_tmp96E.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp96E;});
_tmp96D;});break;case 396: _LL396: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp96F=
_cycalloc(sizeof(*_tmp96F));_tmp96F[0]=({struct Cyc_Exp_tok_struct _tmp970;_tmp970.tag=
Cyc_Exp_tok;_tmp970.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp970;});
_tmp96F;});break;case 397: _LL397: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp971=
_cycalloc(sizeof(*_tmp971));_tmp971[0]=({struct Cyc_Exp_tok_struct _tmp972;_tmp972.tag=
Cyc_Exp_tok;_tmp972.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp972;});
_tmp971;});break;case 398: _LL398: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp973=
_cycalloc(sizeof(*_tmp973));_tmp973[0]=({struct Cyc_Exp_tok_struct _tmp974;_tmp974.tag=
Cyc_Exp_tok;_tmp974.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp974;});
_tmp973;});break;case 399: _LL399: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp975=
_cycalloc(sizeof(*_tmp975));_tmp975[0]=({struct Cyc_Exp_tok_struct _tmp976;_tmp976.tag=
Cyc_Exp_tok;_tmp976.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp976;});
_tmp975;});break;case 400: _LL39A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp977=
_cycalloc(sizeof(*_tmp977));_tmp977[0]=({struct Cyc_Exp_tok_struct _tmp978;_tmp978.tag=
Cyc_Exp_tok;_tmp978.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp978;});
_tmp977;});break;case 401: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 402: _LL39C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 403: _LL39D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp979=_cycalloc(sizeof(*_tmp979));_tmp979[0]=({struct Cyc_Exp_tok_struct _tmp97A;
_tmp97A.tag=Cyc_Exp_tok;_tmp97A.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97A;});
_tmp979;});break;case 404: _LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL39F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97B=_cycalloc(sizeof(*_tmp97B));_tmp97B[0]=({struct Cyc_Exp_tok_struct _tmp97C;
_tmp97C.tag=Cyc_Exp_tok;_tmp97C.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97C;});
_tmp97B;});break;case 406: _LL3A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 407: _LL3A1: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97D=_cycalloc(sizeof(*_tmp97D));_tmp97D[0]=({struct Cyc_Exp_tok_struct _tmp97E;
_tmp97E.tag=Cyc_Exp_tok;_tmp97E.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp97E;});
_tmp97D;});break;case 408: _LL3A2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 409: _LL3A3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp97F=_cycalloc(sizeof(*_tmp97F));_tmp97F[0]=({struct Cyc_Exp_tok_struct _tmp980;
_tmp980.tag=Cyc_Exp_tok;_tmp980.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp980;});
_tmp97F;});break;case 410: _LL3A4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 411: _LL3A5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp981=_cycalloc(sizeof(*_tmp981));_tmp981[0]=({struct Cyc_Exp_tok_struct _tmp982;
_tmp982.tag=Cyc_Exp_tok;_tmp982.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp982;});
_tmp981;});break;case 412: _LL3A6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3A7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp983=_cycalloc(sizeof(*_tmp983));_tmp983[0]=({struct Cyc_Exp_tok_struct _tmp984;
_tmp984.tag=Cyc_Exp_tok;_tmp984.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp984;});
_tmp983;});break;case 414: _LL3A8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp985=
_cycalloc(sizeof(*_tmp985));_tmp985[0]=({struct Cyc_Exp_tok_struct _tmp986;_tmp986.tag=
Cyc_Exp_tok;_tmp986.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp986;});
_tmp985;});break;case 415: _LL3A9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3AA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp987=_cycalloc(sizeof(*_tmp987));_tmp987[0]=({struct Cyc_Exp_tok_struct _tmp988;
_tmp988.tag=Cyc_Exp_tok;_tmp988.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp988;});
_tmp987;});break;case 417: _LL3AB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp989=
_cycalloc(sizeof(*_tmp989));_tmp989[0]=({struct Cyc_Exp_tok_struct _tmp98A;_tmp98A.tag=
Cyc_Exp_tok;_tmp98A.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98A;});
_tmp989;});break;case 418: _LL3AC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98B=
_cycalloc(sizeof(*_tmp98B));_tmp98B[0]=({struct Cyc_Exp_tok_struct _tmp98C;_tmp98C.tag=
Cyc_Exp_tok;_tmp98C.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98C;});
_tmp98B;});break;case 419: _LL3AD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98D=
_cycalloc(sizeof(*_tmp98D));_tmp98D[0]=({struct Cyc_Exp_tok_struct _tmp98E;_tmp98E.tag=
Cyc_Exp_tok;_tmp98E.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98E;});
_tmp98D;});break;case 420: _LL3AE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3AF: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp98F=_cycalloc(sizeof(*_tmp98F));_tmp98F[0]=({struct Cyc_Exp_tok_struct _tmp990;
_tmp990.tag=Cyc_Exp_tok;_tmp990.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp990;});
_tmp98F;});break;case 422: _LL3B0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp991=
_cycalloc(sizeof(*_tmp991));_tmp991[0]=({struct Cyc_Exp_tok_struct _tmp992;_tmp992.tag=
Cyc_Exp_tok;_tmp992.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp992;});
_tmp991;});break;case 423: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B2: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp993=_cycalloc(sizeof(*_tmp993));_tmp993[0]=({struct Cyc_Exp_tok_struct _tmp994;
_tmp994.tag=Cyc_Exp_tok;_tmp994.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp994;});
_tmp993;});break;case 425: _LL3B3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp995=
_cycalloc(sizeof(*_tmp995));_tmp995[0]=({struct Cyc_Exp_tok_struct _tmp996;_tmp996.tag=
Cyc_Exp_tok;_tmp996.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp996;});
_tmp995;});break;case 426: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3B5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp997=_cycalloc(sizeof(*_tmp997));_tmp997[0]=({struct Cyc_Exp_tok_struct _tmp998;
_tmp998.tag=Cyc_Exp_tok;_tmp998.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp998;});
_tmp997;});break;case 428: _LL3B6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp999=
_cycalloc(sizeof(*_tmp999));_tmp999[0]=({struct Cyc_Exp_tok_struct _tmp99A;_tmp99A.tag=
Cyc_Exp_tok;_tmp99A.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99A;});
_tmp999;});break;case 429: _LL3B7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99B=
_cycalloc(sizeof(*_tmp99B));_tmp99B[0]=({struct Cyc_Exp_tok_struct _tmp99C;_tmp99C.tag=
Cyc_Exp_tok;_tmp99C.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99C;});
_tmp99B;});break;case 430: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 431: _LL3B9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp99D=_cycalloc(sizeof(*_tmp99D));_tmp99D[0]=({struct Cyc_Exp_tok_struct _tmp99E;
_tmp99E.tag=Cyc_Exp_tok;_tmp99E.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99E;});
_tmp99D;});break;case 432: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 433: _LL3BB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp99F=_cycalloc(sizeof(*_tmp99F));_tmp99F[0]=({struct Cyc_Exp_tok_struct _tmp9A0;
_tmp9A0.tag=Cyc_Exp_tok;_tmp9A0.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A0;});
_tmp99F;});break;case 434: _LL3BC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A1=
_cycalloc(sizeof(*_tmp9A1));_tmp9A1[0]=({struct Cyc_Exp_tok_struct _tmp9A2;_tmp9A2.tag=
Cyc_Exp_tok;_tmp9A2.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A2;});
_tmp9A1;});break;case 435: _LL3BD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A3=
_cycalloc(sizeof(*_tmp9A3));_tmp9A3[0]=({struct Cyc_Exp_tok_struct _tmp9A4;_tmp9A4.tag=
Cyc_Exp_tok;_tmp9A4.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A4;});
_tmp9A3;});break;case 436: _LL3BE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A5=
_cycalloc(sizeof(*_tmp9A5));_tmp9A5[0]=({struct Cyc_Exp_tok_struct _tmp9A6;_tmp9A6.tag=
Cyc_Exp_tok;_tmp9A6.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A6;});
_tmp9A5;});break;case 437: _LL3BF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3C0: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9A7=_cycalloc(sizeof(*_tmp9A7));_tmp9A7[0]=({struct Cyc_Exp_tok_struct _tmp9A8;
_tmp9A8.tag=Cyc_Exp_tok;_tmp9A8.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A8;});
_tmp9A7;});break;case 439: _LL3C1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A9=
_cycalloc(sizeof(*_tmp9A9));_tmp9A9[0]=({struct Cyc_Exp_tok_struct _tmp9AA;_tmp9AA.tag=
Cyc_Exp_tok;_tmp9AA.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AA;});
_tmp9A9;});break;case 440: _LL3C2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AB=
_cycalloc(sizeof(*_tmp9AB));_tmp9AB[0]=({struct Cyc_Exp_tok_struct _tmp9AC;_tmp9AC.tag=
Cyc_Exp_tok;_tmp9AC.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AC;});
_tmp9AB;});break;case 441: _LL3C3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AD=
_cycalloc(sizeof(*_tmp9AD));_tmp9AD[0]=({struct Cyc_Exp_tok_struct _tmp9AE;_tmp9AE.tag=
Cyc_Exp_tok;_tmp9AE.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp9AF=_cycalloc(sizeof(*_tmp9AF));_tmp9AF[0]=({struct Cyc_Absyn_StructField_struct
_tmp9B0;_tmp9B0.tag=0;_tmp9B0.f1=({struct _tagged_arr*_tmp9B1=_cycalloc(sizeof(*
_tmp9B1));_tmp9B1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp9B1;});_tmp9B0;});_tmp9AF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AE;});
_tmp9AD;});break;case 442: _LL3C4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B2=
_cycalloc(sizeof(*_tmp9B2));_tmp9B2[0]=({struct Cyc_Exp_tok_struct _tmp9B3;_tmp9B3.tag=
Cyc_Exp_tok;_tmp9B3.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp9B4=_cycalloc_atomic(sizeof(*_tmp9B4));_tmp9B4[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp9B5;_tmp9B5.tag=1;_tmp9B5.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp9B5;});_tmp9B4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9B3;});_tmp9B2;});break;case 443: _LL3C5: {struct
Cyc_Position_Segment*_tmp9B6=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp9B7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp9B6,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B8=_cycalloc(
sizeof(*_tmp9B8));_tmp9B8[0]=({struct Cyc_Exp_tok_struct _tmp9B9;_tmp9B9.tag=Cyc_Exp_tok;
_tmp9B9.f1=Cyc_Absyn_gentyp_exp(_tmp9B7,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B9;});
_tmp9B8;});break;}case 444: _LL3C6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BA=
_cycalloc(sizeof(*_tmp9BA));_tmp9BA[0]=({struct Cyc_Exp_tok_struct _tmp9BB;_tmp9BB.tag=
Cyc_Exp_tok;_tmp9BB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9BC=_cycalloc(sizeof(*_tmp9BC));_tmp9BC[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9BD;_tmp9BD.tag=35;_tmp9BD.f1=({struct Cyc_Absyn_MallocInfo _tmp9BE;_tmp9BE.is_calloc=
0;_tmp9BE.rgn=0;_tmp9BE.elt_type=0;_tmp9BE.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9BE.fat_result=0;
_tmp9BE;});_tmp9BD;});_tmp9BC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BB;});
_tmp9BA;});break;case 445: _LL3C7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BF=
_cycalloc(sizeof(*_tmp9BF));_tmp9BF[0]=({struct Cyc_Exp_tok_struct _tmp9C0;_tmp9C0.tag=
Cyc_Exp_tok;_tmp9C0.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9C1=_cycalloc(sizeof(*_tmp9C1));_tmp9C1[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9C2;_tmp9C2.tag=35;_tmp9C2.f1=({struct Cyc_Absyn_MallocInfo _tmp9C3;_tmp9C3.is_calloc=
0;_tmp9C3.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp9C3.elt_type=0;
_tmp9C3.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp9C3.fat_result=0;_tmp9C3;});_tmp9C2;});_tmp9C1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C0;});
_tmp9BF;});break;case 446: _LL3C8: {void*_tmp9C5;struct _tuple2 _tmp9C4=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9C5=_tmp9C4.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C6=_cycalloc(sizeof(*_tmp9C6));
_tmp9C6[0]=({struct Cyc_Exp_tok_struct _tmp9C7;_tmp9C7.tag=Cyc_Exp_tok;_tmp9C7.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9C8=_cycalloc(
sizeof(*_tmp9C8));_tmp9C8[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9C9;_tmp9C9.tag=
35;_tmp9C9.f1=({struct Cyc_Absyn_MallocInfo _tmp9CA;_tmp9CA.is_calloc=1;_tmp9CA.rgn=
0;_tmp9CA.elt_type=({void**_tmp9CB=_cycalloc(sizeof(*_tmp9CB));_tmp9CB[0]=
_tmp9C5;_tmp9CB;});_tmp9CA.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9CA.fat_result=0;
_tmp9CA;});_tmp9C9;});_tmp9C8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C7;});
_tmp9C6;});break;}case 447: _LL3C9: {void*_tmp9CD;struct _tuple2 _tmp9CC=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9CD=_tmp9CC.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9CE=_cycalloc(sizeof(*_tmp9CE));
_tmp9CE[0]=({struct Cyc_Exp_tok_struct _tmp9CF;_tmp9CF.tag=Cyc_Exp_tok;_tmp9CF.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9D0=_cycalloc(
sizeof(*_tmp9D0));_tmp9D0[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9D1;_tmp9D1.tag=
35;_tmp9D1.f1=({struct Cyc_Absyn_MallocInfo _tmp9D2;_tmp9D2.is_calloc=1;_tmp9D2.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp9D2.elt_type=({void**_tmp9D3=_cycalloc(sizeof(*_tmp9D3));
_tmp9D3[0]=_tmp9CD;_tmp9D3;});_tmp9D2.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9D2.fat_result=0;
_tmp9D2;});_tmp9D1;});_tmp9D0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9CF;});
_tmp9CE;});break;}case 448: _LL3CA: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9D4=_cycalloc(sizeof(*_tmp9D4));_tmp9D4[0]=({struct Cyc_Primop_tok_struct
_tmp9D5;_tmp9D5.tag=Cyc_Primop_tok;_tmp9D5.f1=(void*)((void*)12);_tmp9D5;});
_tmp9D4;});break;case 449: _LL3CB: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9D6=_cycalloc(sizeof(*_tmp9D6));_tmp9D6[0]=({struct Cyc_Primop_tok_struct
_tmp9D7;_tmp9D7.tag=Cyc_Primop_tok;_tmp9D7.f1=(void*)((void*)11);_tmp9D7;});
_tmp9D6;});break;case 450: _LL3CC: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9D8=_cycalloc(sizeof(*_tmp9D8));_tmp9D8[0]=({struct Cyc_Primop_tok_struct
_tmp9D9;_tmp9D9.tag=Cyc_Primop_tok;_tmp9D9.f1=(void*)((void*)2);_tmp9D9;});
_tmp9D8;});break;case 451: _LL3CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 452: _LL3CE: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9DA=_cycalloc(sizeof(*_tmp9DA));_tmp9DA[0]=({struct Cyc_Exp_tok_struct _tmp9DB;
_tmp9DB.tag=Cyc_Exp_tok;_tmp9DB.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DB;});
_tmp9DA;});break;case 453: _LL3CF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DC=
_cycalloc(sizeof(*_tmp9DC));_tmp9DC[0]=({struct Cyc_Exp_tok_struct _tmp9DD;_tmp9DD.tag=
Cyc_Exp_tok;_tmp9DD.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DD;});
_tmp9DC;});break;case 454: _LL3D0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DE=
_cycalloc(sizeof(*_tmp9DE));_tmp9DE[0]=({struct Cyc_Exp_tok_struct _tmp9DF;_tmp9DF.tag=
Cyc_Exp_tok;_tmp9DF.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DF;});
_tmp9DE;});break;case 455: _LL3D1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E0=
_cycalloc(sizeof(*_tmp9E0));_tmp9E0[0]=({struct Cyc_Exp_tok_struct _tmp9E1;_tmp9E1.tag=
Cyc_Exp_tok;_tmp9E1.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9E2=_cycalloc(sizeof(*_tmp9E2));_tmp9E2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9E2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E1;});
_tmp9E0;});break;case 456: _LL3D2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E3=
_cycalloc(sizeof(*_tmp9E3));_tmp9E3[0]=({struct Cyc_Exp_tok_struct _tmp9E4;_tmp9E4.tag=
Cyc_Exp_tok;_tmp9E4.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _tagged_arr*
_tmp9E5=_cycalloc(sizeof(*_tmp9E5));_tmp9E5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9E5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E4;});
_tmp9E3;});break;case 457: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E6=
_cycalloc(sizeof(*_tmp9E6));_tmp9E6[0]=({struct Cyc_Exp_tok_struct _tmp9E7;_tmp9E7.tag=
Cyc_Exp_tok;_tmp9E7.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E7;});
_tmp9E6;});break;case 458: _LL3D4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E8=
_cycalloc(sizeof(*_tmp9E8));_tmp9E8[0]=({struct Cyc_Exp_tok_struct _tmp9E9;_tmp9E9.tag=
Cyc_Exp_tok;_tmp9E9.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E9;});
_tmp9E8;});break;case 459: _LL3D5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9EA=
_cycalloc(sizeof(*_tmp9EA));_tmp9EA[0]=({struct Cyc_Exp_tok_struct _tmp9EB;_tmp9EB.tag=
Cyc_Exp_tok;_tmp9EB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp9EC=_cycalloc(sizeof(*_tmp9EC));_tmp9EC[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp9ED;_tmp9ED.tag=27;_tmp9ED.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp9ED.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp9ED;});_tmp9EC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9EB;});_tmp9EA;});break;case 460: _LL3D6: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp9EE=_cycalloc(sizeof(*_tmp9EE));_tmp9EE[0]=({
struct Cyc_Exp_tok_struct _tmp9EF;_tmp9EF.tag=Cyc_Exp_tok;_tmp9EF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp9F0=_cycalloc(sizeof(*_tmp9F0));
_tmp9F0[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp9F1;_tmp9F1.tag=27;_tmp9F1.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp9F1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp9F1;});_tmp9F0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9EF;});
_tmp9EE;});break;case 461: _LL3D7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F2=
_cycalloc(sizeof(*_tmp9F2));_tmp9F2[0]=({struct Cyc_Exp_tok_struct _tmp9F3;_tmp9F3.tag=
Cyc_Exp_tok;_tmp9F3.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp9F4=_cycalloc(sizeof(*_tmp9F4));_tmp9F4[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp9F5;_tmp9F5.tag=2;_tmp9F5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9F5;});_tmp9F4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F3;});
_tmp9F2;});break;case 462: _LL3D8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 463: _LL3D9: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9F6=_cycalloc(sizeof(*_tmp9F6));_tmp9F6[0]=({struct Cyc_Exp_tok_struct _tmp9F7;
_tmp9F7.tag=Cyc_Exp_tok;_tmp9F7.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F7;});
_tmp9F6;});break;case 464: _LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 465: _LL3DB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9F8=_cycalloc(sizeof(*_tmp9F8));_tmp9F8[0]=({struct Cyc_Exp_tok_struct _tmp9F9;
_tmp9F9.tag=Cyc_Exp_tok;_tmp9F9.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F9;});
_tmp9F8;});break;case 466: _LL3DC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FA=
_cycalloc(sizeof(*_tmp9FA));_tmp9FA[0]=({struct Cyc_Exp_tok_struct _tmp9FB;_tmp9FB.tag=
Cyc_Exp_tok;_tmp9FB.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FB;});
_tmp9FA;});break;case 467: _LL3DD: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FC=
_cycalloc(sizeof(*_tmp9FC));_tmp9FC[0]=({struct Cyc_Exp_tok_struct _tmp9FD;_tmp9FD.tag=
Cyc_Exp_tok;_tmp9FD.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FD;});
_tmp9FC;});break;case 468: _LL3DE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FE=
_cycalloc(sizeof(*_tmp9FE));_tmp9FE[0]=({struct Cyc_Exp_tok_struct _tmp9FF;_tmp9FF.tag=
Cyc_Exp_tok;_tmp9FF.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmpA00=_cycalloc(sizeof(*_tmpA00));_tmpA00[0]=({struct Cyc_Absyn_Struct_e_struct
_tmpA01;_tmpA01.tag=30;_tmpA01.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmpA01.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA01.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmpA01.f4=0;
_tmpA01;});_tmpA00;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FF;});
_tmp9FE;});break;case 469: _LL3DF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA02=
_cycalloc(sizeof(*_tmpA02));_tmpA02[0]=({struct Cyc_Exp_tok_struct _tmpA03;_tmpA03.tag=
Cyc_Exp_tok;_tmpA03.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA03;});
_tmpA02;});break;case 470: _LL3E0: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmpA04=_cycalloc(sizeof(*_tmpA04));_tmpA04[0]=({struct Cyc_ExpList_tok_struct
_tmpA05;_tmpA05.tag=Cyc_ExpList_tok;_tmpA05.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmpA05;});_tmpA04;});
break;case 471: _LL3E1: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmpA06=
_cycalloc(sizeof(*_tmpA06));_tmpA06[0]=({struct Cyc_ExpList_tok_struct _tmpA07;
_tmpA07.tag=Cyc_ExpList_tok;_tmpA07.f1=({struct Cyc_List_List*_tmpA08=_cycalloc(
sizeof(*_tmpA08));_tmpA08->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA08->tl=0;_tmpA08;});
_tmpA07;});_tmpA06;});break;case 472: _LL3E2: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmpA09=_cycalloc(sizeof(*_tmpA09));_tmpA09[0]=({struct Cyc_ExpList_tok_struct
_tmpA0A;_tmpA0A.tag=Cyc_ExpList_tok;_tmpA0A.f1=({struct Cyc_List_List*_tmpA0B=
_cycalloc(sizeof(*_tmpA0B));_tmpA0B->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA0B->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA0B;});_tmpA0A;});
_tmpA09;});break;case 473: _LL3E3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA0C=
_cycalloc(sizeof(*_tmpA0C));_tmpA0C[0]=({struct Cyc_Exp_tok_struct _tmpA0D;_tmpA0D.tag=
Cyc_Exp_tok;_tmpA0D.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0D;});
_tmpA0C;});break;case 474: _LL3E4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA0E=
_cycalloc(sizeof(*_tmpA0E));_tmpA0E[0]=({struct Cyc_Exp_tok_struct _tmpA0F;_tmpA0F.tag=
Cyc_Exp_tok;_tmpA0F.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0F;});
_tmpA0E;});break;case 475: _LL3E5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA10=
_cycalloc(sizeof(*_tmpA10));_tmpA10[0]=({struct Cyc_Exp_tok_struct _tmpA11;_tmpA11.tag=
Cyc_Exp_tok;_tmpA11.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA11;});
_tmpA10;});break;case 476: _LL3E6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA12=
_cycalloc(sizeof(*_tmpA12));_tmpA12[0]=({struct Cyc_Exp_tok_struct _tmpA13;_tmpA13.tag=
Cyc_Exp_tok;_tmpA13.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA13;});
_tmpA12;});break;case 477: _LL3E7: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmpA14=_cycalloc(sizeof(*_tmpA14));_tmpA14[0]=({struct Cyc_QualId_tok_struct
_tmpA15;_tmpA15.tag=Cyc_QualId_tok;_tmpA15.f1=({struct _tuple1*_tmpA16=_cycalloc(
sizeof(*_tmpA16));_tmpA16->f1=Cyc_Absyn_rel_ns_null;_tmpA16->f2=({struct
_tagged_arr*_tmpA17=_cycalloc(sizeof(*_tmpA17));_tmpA17[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA17;});_tmpA16;});
_tmpA15;});_tmpA14;});break;case 478: _LL3E8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 479: _LL3E9: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmpA18=_cycalloc(sizeof(*_tmpA18));_tmpA18[
0]=({struct Cyc_QualId_tok_struct _tmpA19;_tmpA19.tag=Cyc_QualId_tok;_tmpA19.f1=({
struct _tuple1*_tmpA1A=_cycalloc(sizeof(*_tmpA1A));_tmpA1A->f1=Cyc_Absyn_rel_ns_null;
_tmpA1A->f2=({struct _tagged_arr*_tmpA1B=_cycalloc(sizeof(*_tmpA1B));_tmpA1B[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmpA1B;});_tmpA1A;});_tmpA19;});_tmpA18;});break;case 480: _LL3EA: yyval=yyvs[
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
+ 15,count ++);}msg=({unsigned int _tmpA1C=(unsigned int)(sze + 15);char*_tmpA1D=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpA1C + 1));struct _tagged_arr
_tmpA1F=_tag_arr(_tmpA1D,sizeof(char),_tmpA1C + 1);{unsigned int _tmpA1E=_tmpA1C;
unsigned int i;for(i=0;i < _tmpA1E;i ++){_tmpA1D[i]='\000';}_tmpA1D[_tmpA1E]=(char)
0;}_tmpA1F;});Cyc_strcpy(msg,({const char*_tmpA20="parse error";_tag_arr(_tmpA20,
sizeof(char),_get_zero_arr_size(_tmpA20,12));}));if(count < 5){count=0;for(x=yyn < 
0?- yyn: 0;x < 268 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5864,x + yyn)]== x){Cyc_strcat(msg,(struct
_tagged_arr)(count == 0?(struct _tagged_arr)({const char*_tmpA21=", expecting `";
_tag_arr(_tmpA21,sizeof(char),_get_zero_arr_size(_tmpA21,14));}):(struct
_tagged_arr)({const char*_tmpA22=" or `";_tag_arr(_tmpA22,sizeof(char),
_get_zero_arr_size(_tmpA22,6));})));Cyc_strcat(msg,(struct _tagged_arr)Cyc_yytname[
_check_known_subscript_notnull(268,x)]);Cyc_strcat(msg,({const char*_tmpA23="'";
_tag_arr(_tmpA23,sizeof(char),_get_zero_arr_size(_tmpA23,2));}));count ++;}}}Cyc_yyerror((
struct _tagged_arr)msg);}else{Cyc_yyerror(({const char*_tmpA24="parse error";
_tag_arr(_tmpA24,sizeof(char),_get_zero_arr_size(_tmpA24,12));}));}}goto
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
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmpA2E=v;struct _tuple13*_tmpA2F;
struct _tuple13 _tmpA30;int _tmpA31;char _tmpA32;short _tmpA33;struct _tagged_arr
_tmpA34;struct Cyc_Core_Opt*_tmpA35;struct Cyc_Core_Opt*_tmpA36;struct Cyc_Core_Opt
_tmpA37;struct _tagged_arr*_tmpA38;struct _tuple1*_tmpA39;struct _tuple1 _tmpA3A;
void*_tmpA3B;struct _tagged_arr*_tmpA3C;_LL3F4: if(*((void**)_tmpA2E)!= Cyc_Int_tok)
goto _LL3F6;_tmpA2F=((struct Cyc_Int_tok_struct*)_tmpA2E)->f1;_tmpA30=*_tmpA2F;
_tmpA31=_tmpA30.f2;_LL3F5:({struct Cyc_Int_pa_struct _tmpA3F;_tmpA3F.tag=1;_tmpA3F.f1=(
unsigned int)_tmpA31;{void*_tmpA3D[1]={& _tmpA3F};Cyc_fprintf(Cyc_stderr,({const
char*_tmpA3E="%d";_tag_arr(_tmpA3E,sizeof(char),_get_zero_arr_size(_tmpA3E,3));}),
_tag_arr(_tmpA3D,sizeof(void*),1));}});goto _LL3F3;_LL3F6: if(*((void**)_tmpA2E)!= 
Cyc_Char_tok)goto _LL3F8;_tmpA32=((struct Cyc_Char_tok_struct*)_tmpA2E)->f1;_LL3F7:({
struct Cyc_Int_pa_struct _tmpA42;_tmpA42.tag=1;_tmpA42.f1=(unsigned int)((int)
_tmpA32);{void*_tmpA40[1]={& _tmpA42};Cyc_fprintf(Cyc_stderr,({const char*_tmpA41="%c";
_tag_arr(_tmpA41,sizeof(char),_get_zero_arr_size(_tmpA41,3));}),_tag_arr(_tmpA40,
sizeof(void*),1));}});goto _LL3F3;_LL3F8: if(*((void**)_tmpA2E)!= Cyc_Short_tok)
goto _LL3FA;_tmpA33=((struct Cyc_Short_tok_struct*)_tmpA2E)->f1;_LL3F9:({struct Cyc_Int_pa_struct
_tmpA45;_tmpA45.tag=1;_tmpA45.f1=(unsigned int)((int)_tmpA33);{void*_tmpA43[1]={&
_tmpA45};Cyc_fprintf(Cyc_stderr,({const char*_tmpA44="%ds";_tag_arr(_tmpA44,
sizeof(char),_get_zero_arr_size(_tmpA44,4));}),_tag_arr(_tmpA43,sizeof(void*),1));}});
goto _LL3F3;_LL3FA: if(*((void**)_tmpA2E)!= Cyc_String_tok)goto _LL3FC;_tmpA34=((
struct Cyc_String_tok_struct*)_tmpA2E)->f1;_LL3FB:({struct Cyc_String_pa_struct
_tmpA48;_tmpA48.tag=0;_tmpA48.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpA34);{
void*_tmpA46[1]={& _tmpA48};Cyc_fprintf(Cyc_stderr,({const char*_tmpA47="\"%s\"";
_tag_arr(_tmpA47,sizeof(char),_get_zero_arr_size(_tmpA47,5));}),_tag_arr(_tmpA46,
sizeof(void*),1));}});goto _LL3F3;_LL3FC: if(*((void**)_tmpA2E)!= Cyc_Stringopt_tok)
goto _LL3FE;_tmpA35=((struct Cyc_Stringopt_tok_struct*)_tmpA2E)->f1;if(_tmpA35 != 0)
goto _LL3FE;_LL3FD:({void*_tmpA49[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpA4A="NULL";_tag_arr(_tmpA4A,sizeof(char),_get_zero_arr_size(_tmpA4A,5));}),
_tag_arr(_tmpA49,sizeof(void*),0));});goto _LL3F3;_LL3FE: if(*((void**)_tmpA2E)!= 
Cyc_Stringopt_tok)goto _LL400;_tmpA36=((struct Cyc_Stringopt_tok_struct*)_tmpA2E)->f1;
if(_tmpA36 == 0)goto _LL400;_tmpA37=*_tmpA36;_tmpA38=(struct _tagged_arr*)_tmpA37.v;
_LL3FF:({struct Cyc_String_pa_struct _tmpA4D;_tmpA4D.tag=0;_tmpA4D.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmpA38);{void*_tmpA4B[1]={& _tmpA4D};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA4C="\"%s\"";_tag_arr(_tmpA4C,sizeof(char),
_get_zero_arr_size(_tmpA4C,5));}),_tag_arr(_tmpA4B,sizeof(void*),1));}});goto
_LL3F3;_LL400: if(*((void**)_tmpA2E)!= Cyc_QualId_tok)goto _LL402;_tmpA39=((struct
Cyc_QualId_tok_struct*)_tmpA2E)->f1;_tmpA3A=*_tmpA39;_tmpA3B=_tmpA3A.f1;_tmpA3C=
_tmpA3A.f2;_LL401: {struct Cyc_List_List*_tmpA4E=0;{void*_tmpA4F=_tmpA3B;struct
Cyc_List_List*_tmpA50;struct Cyc_List_List*_tmpA51;_LL405: if(_tmpA4F <= (void*)1
 || *((int*)_tmpA4F)!= 0)goto _LL407;_tmpA50=((struct Cyc_Absyn_Rel_n_struct*)
_tmpA4F)->f1;_LL406: _tmpA4E=_tmpA50;goto _LL404;_LL407: if(_tmpA4F <= (void*)1  || *((
int*)_tmpA4F)!= 1)goto _LL409;_tmpA51=((struct Cyc_Absyn_Abs_n_struct*)_tmpA4F)->f1;
_LL408: _tmpA4E=_tmpA51;goto _LL404;_LL409: if((int)_tmpA4F != 0)goto _LL404;_LL40A:
goto _LL404;_LL404:;}for(0;_tmpA4E != 0;_tmpA4E=_tmpA4E->tl){({struct Cyc_String_pa_struct
_tmpA54;_tmpA54.tag=0;_tmpA54.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmpA4E->hd));{void*_tmpA52[1]={& _tmpA54};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA53="%s::";_tag_arr(_tmpA53,sizeof(char),_get_zero_arr_size(
_tmpA53,5));}),_tag_arr(_tmpA52,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmpA57;_tmpA57.tag=0;_tmpA57.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmpA3C);{
void*_tmpA55[1]={& _tmpA57};Cyc_fprintf(Cyc_stderr,({const char*_tmpA56="%s::";
_tag_arr(_tmpA56,sizeof(char),_get_zero_arr_size(_tmpA56,5));}),_tag_arr(_tmpA55,
sizeof(void*),1));}});goto _LL3F3;}_LL402:;_LL403:({void*_tmpA58[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA59="?";_tag_arr(_tmpA59,sizeof(char),
_get_zero_arr_size(_tmpA59,2));}),_tag_arr(_tmpA58,sizeof(void*),0));});goto
_LL3F3;_LL3F3:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*_tmpA5A=_cycalloc(
sizeof(*_tmpA5A));_tmpA5A->v=Cyc_Lexing_from_file(f);_tmpA5A;});Cyc_yyparse();
return Cyc_Parse_parse_result;}
