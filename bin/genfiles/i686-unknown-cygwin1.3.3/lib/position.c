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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;
};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int
tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};struct
Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _tagged_arr,int);void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places);extern char Cyc_Position_Exit[
9];void Cyc_Position_reset_position(struct _tagged_arr);void Cyc_Position_set_position_file(
struct _tagged_arr);struct _tagged_arr Cyc_Position_get_position_file();struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Position_string_of_loc(int);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _tagged_arr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _tagged_arr);struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];extern int Cyc_Position_use_gcc_style_location;extern int Cyc_Position_print_context;
extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);int Cyc_Position_error_p();struct _tagged_arr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);unsigned int Cyc_strlen(struct _tagged_arr s);int
Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_strncpy(
struct _tagged_arr,struct _tagged_arr,unsigned int);struct _tagged_arr Cyc_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Typerep_Int_struct{int tag;int
f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*
f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int
f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple1{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple2{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_Position_use_gcc_style_location=1;char Cyc_Position_Exit[9]="\000\000\000\000Exit\000";
static char _tmp0[1]="";static struct _tagged_arr Cyc_Position_source={_tmp0,_tmp0,
_tmp0 + 1};struct Cyc_Position_Segment{int start;int end;};struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int start,int end){return({struct Cyc_Position_Segment*
_tmp1=_cycalloc_atomic(sizeof(*_tmp1));_tmp1->start=start;_tmp1->end=end;_tmp1;});}
struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*
s1,struct Cyc_Position_Segment*s2){if(s1 == 0)return s2;if(s2 == 0)return s1;return({
struct Cyc_Position_Segment*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2->start=
s1->start;_tmp2->end=s2->end;_tmp2;});}struct _tagged_arr Cyc_Position_string_of_loc(
int loc){struct Cyc_Lineno_Pos*pos=Cyc_Lineno_pos_of_abs(Cyc_Position_source,loc);
if(Cyc_Position_use_gcc_style_location)return({struct Cyc_Int_pa_struct _tmp6;
_tmp6.tag=1;_tmp6.f1=(unsigned int)pos->line_no;{struct Cyc_String_pa_struct _tmp5;
_tmp5.tag=0;_tmp5.f1=(struct _tagged_arr)((struct _tagged_arr)pos->logical_file);{
void*_tmp3[2]={& _tmp5,& _tmp6};Cyc_aprintf(({const char*_tmp4="%s:%d";_tag_arr(
_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,6));}),_tag_arr(_tmp3,sizeof(void*),
2));}}});else{return({struct Cyc_Int_pa_struct _tmpB;_tmpB.tag=1;_tmpB.f1=(
unsigned int)pos->col;{struct Cyc_Int_pa_struct _tmpA;_tmpA.tag=1;_tmpA.f1=(
unsigned int)pos->line_no;{struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(
struct _tagged_arr)((struct _tagged_arr)pos->logical_file);{void*_tmp7[3]={& _tmp9,&
_tmpA,& _tmpB};Cyc_aprintf(({const char*_tmp8="%s:(%d:%d)";_tag_arr(_tmp8,sizeof(
char),_get_zero_arr_size(_tmp8,11));}),_tag_arr(_tmp7,sizeof(void*),3));}}}});}}
static struct _tagged_arr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*pos_s,
struct Cyc_Lineno_Pos*pos_e){if(Cyc_Position_use_gcc_style_location)return({
struct Cyc_Int_pa_struct _tmpF;_tmpF.tag=1;_tmpF.f1=(unsigned int)pos_s->line_no;{
struct Cyc_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(struct _tagged_arr)((struct
_tagged_arr)pos_s->logical_file);{void*_tmpC[2]={& _tmpE,& _tmpF};Cyc_aprintf(({
const char*_tmpD="%s:%d";_tag_arr(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,6));}),
_tag_arr(_tmpC,sizeof(void*),2));}}});else{if(Cyc_strcmp((struct _tagged_arr)
pos_s->logical_file,(struct _tagged_arr)pos_e->logical_file)== 0)return({struct
Cyc_Int_pa_struct _tmp16;_tmp16.tag=1;_tmp16.f1=(unsigned int)pos_e->col;{struct
Cyc_Int_pa_struct _tmp15;_tmp15.tag=1;_tmp15.f1=(unsigned int)pos_e->line_no;{
struct Cyc_Int_pa_struct _tmp14;_tmp14.tag=1;_tmp14.f1=(unsigned int)pos_s->col;{
struct Cyc_Int_pa_struct _tmp13;_tmp13.tag=1;_tmp13.f1=(unsigned int)pos_s->line_no;{
struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(struct _tagged_arr)((
struct _tagged_arr)pos_s->logical_file);{void*_tmp10[5]={& _tmp12,& _tmp13,& _tmp14,&
_tmp15,& _tmp16};Cyc_aprintf(({const char*_tmp11="%s(%d:%d-%d:%d)";_tag_arr(_tmp11,
sizeof(char),_get_zero_arr_size(_tmp11,16));}),_tag_arr(_tmp10,sizeof(void*),5));}}}}}});
else{return({struct Cyc_Int_pa_struct _tmp1E;_tmp1E.tag=1;_tmp1E.f1=(unsigned int)
pos_e->col;{struct Cyc_Int_pa_struct _tmp1D;_tmp1D.tag=1;_tmp1D.f1=(unsigned int)
pos_e->line_no;{struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct
_tagged_arr)((struct _tagged_arr)pos_e->logical_file);{struct Cyc_Int_pa_struct
_tmp1B;_tmp1B.tag=1;_tmp1B.f1=(unsigned int)pos_s->col;{struct Cyc_Int_pa_struct
_tmp1A;_tmp1A.tag=1;_tmp1A.f1=(unsigned int)pos_s->line_no;{struct Cyc_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)((struct _tagged_arr)pos_s->logical_file);{
void*_tmp17[6]={& _tmp19,& _tmp1A,& _tmp1B,& _tmp1C,& _tmp1D,& _tmp1E};Cyc_aprintf(({
const char*_tmp18="%s(%d:%d)-%s(%d:%d)";_tag_arr(_tmp18,sizeof(char),
_get_zero_arr_size(_tmp18,20));}),_tag_arr(_tmp17,sizeof(void*),6));}}}}}}});}}}
struct _tagged_arr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*s){
if(s == 0)return({struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Position_source);{void*_tmp1F[1]={& _tmp21};
Cyc_aprintf(({const char*_tmp20="%s";_tag_arr(_tmp20,sizeof(char),
_get_zero_arr_size(_tmp20,3));}),_tag_arr(_tmp1F,sizeof(void*),1));}});{struct
Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->start);struct
Cyc_Lineno_Pos*pos_e=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->end);return Cyc_Position_string_of_pos_pr(
pos_s,pos_e);}}static struct Cyc_Lineno_Pos*Cyc_Position_new_pos(){return({struct
Cyc_Lineno_Pos*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->logical_file=({const
char*_tmp23="";_tag_arr(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,1));});
_tmp22->line=Cyc_Core_new_string(0);_tmp22->line_no=0;_tmp22->col=0;_tmp22;});}
struct _tuple4{int f1;struct Cyc_Lineno_Pos*f2;};struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*segs){struct Cyc_List_List*places=0;{struct Cyc_List_List*
_tmp24=segs;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){if((struct Cyc_Position_Segment*)
_tmp24->hd == 0)continue;places=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*
_tmp25));_tmp25->hd=({struct _tuple4*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->f1=((
struct Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)_tmp24->hd))->end;
_tmp28->f2=Cyc_Position_new_pos();_tmp28;});_tmp25->tl=({struct Cyc_List_List*
_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->hd=({struct _tuple4*_tmp27=_cycalloc(
sizeof(*_tmp27));_tmp27->f1=((struct Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)
_tmp24->hd))->start;_tmp27->f2=Cyc_Position_new_pos();_tmp27;});_tmp26->tl=
places;_tmp26;});_tmp25;});}}Cyc_Lineno_poss_of_abss(Cyc_Position_source,places);{
struct Cyc_List_List*ans=0;places=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(places);for(0;segs != 0;segs=segs->tl){if((struct Cyc_Position_Segment*)
segs->hd == 0)ans=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=({
struct _tagged_arr*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_source);{
void*_tmp2B[1]={& _tmp2D};Cyc_aprintf(({const char*_tmp2C="%s(unknown)";_tag_arr(
_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,12));}),_tag_arr(_tmp2B,sizeof(
void*),1));}});_tmp2A;});_tmp29->tl=ans;_tmp29;});else{ans=({struct Cyc_List_List*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->hd=({struct _tagged_arr*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=Cyc_Position_string_of_pos_pr((*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(places))->hd)).f2,(*((struct _tuple4*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(places))->tl))->hd)).f2);
_tmp2F;});_tmp2E->tl=ans;_tmp2E;});places=((struct Cyc_List_List*)_check_null(
places->tl))->tl;}}return ans;}}struct Cyc_Position_Error;struct Cyc_Position_Error*
Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*l,struct _tagged_arr desc){
return({struct Cyc_Position_Error*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->source=
Cyc_Position_source;_tmp30->seg=l;_tmp30->kind=(void*)((void*)0);_tmp30->desc=
desc;_tmp30;});}struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*
l,struct _tagged_arr desc){return({struct Cyc_Position_Error*_tmp31=_cycalloc(
sizeof(*_tmp31));_tmp31->source=Cyc_Position_source;_tmp31->seg=l;_tmp31->kind=(
void*)((void*)1);_tmp31->desc=desc;_tmp31;});}struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*l,struct _tagged_arr desc){return({struct Cyc_Position_Error*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->source=Cyc_Position_source;_tmp32->seg=
l;_tmp32->kind=(void*)((void*)2);_tmp32->desc=desc;_tmp32;});}char Cyc_Position_Nocontext[
14]="\000\000\000\000Nocontext\000";static struct _tagged_arr Cyc_Position_trunc(
int n,struct _tagged_arr s){int len=(int)Cyc_strlen((struct _tagged_arr)s);if(len < n)
return s;{int len_one=(n - 3)/ 2;int len_two=(n - 3)- len_one;struct _tagged_arr ans=
Cyc_Core_new_string((unsigned int)(n + 1));Cyc_strncpy(ans,(struct _tagged_arr)s,(
unsigned int)len_one);Cyc_strncpy(_tagged_arr_plus(ans,sizeof(char),len_one),({
const char*_tmp33="...";_tag_arr(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,4));}),
3);Cyc_strncpy(_tagged_arr_plus(_tagged_arr_plus(ans,sizeof(char),len_one),
sizeof(char),3),(struct _tagged_arr)_tagged_arr_plus(_tagged_arr_plus(s,sizeof(
char),len),sizeof(char),- len_two),(unsigned int)len_two);return ans;}}static int
Cyc_Position_line_length=76;struct _tuple5{struct _tagged_arr f1;int f2;int f3;};
static struct _tuple5*Cyc_Position_get_context(struct Cyc_Position_Segment*seg){if(
seg == 0)(int)_throw((void*)Cyc_Position_Nocontext);{struct Cyc_Lineno_Pos*pos_s;
struct Cyc_Lineno_Pos*pos_e;{struct _handler_cons _tmp34;_push_handler(& _tmp34);{
int _tmp36=0;if(setjmp(_tmp34.handler))_tmp36=1;if(!_tmp36){pos_s=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,seg->start);pos_e=Cyc_Lineno_pos_of_abs(Cyc_Position_source,
seg->end);;_pop_handler();}else{void*_tmp35=(void*)_exn_thrown;void*_tmp38=
_tmp35;_LL1:;_LL2:(int)_throw((void*)Cyc_Position_Nocontext);_LL3:;_LL4:(void)
_throw(_tmp38);_LL0:;}}}{struct Cyc_Lineno_Pos _tmp3A;struct _tagged_arr _tmp3B;int
_tmp3C;int _tmp3D;struct Cyc_Lineno_Pos*_tmp39=pos_s;_tmp3A=*_tmp39;_tmp3B=_tmp3A.line;
_tmp3C=_tmp3A.line_no;_tmp3D=_tmp3A.col;{struct Cyc_Lineno_Pos _tmp3F;struct
_tagged_arr _tmp40;int _tmp41;int _tmp42;struct Cyc_Lineno_Pos*_tmp3E=pos_e;_tmp3F=*
_tmp3E;_tmp40=_tmp3F.line;_tmp41=_tmp3F.line_no;_tmp42=_tmp3F.col;if(_tmp3C == 
_tmp41){int n=Cyc_Position_line_length / 3;struct _tagged_arr sec_one=Cyc_Position_trunc(
n,Cyc_substring((struct _tagged_arr)_tmp3B,0,(unsigned int)_tmp3D));struct
_tagged_arr sec_two=Cyc_Position_trunc(n,Cyc_substring((struct _tagged_arr)_tmp3B,
_tmp3D,(unsigned int)(_tmp42 - _tmp3D)));struct _tagged_arr sec_three=Cyc_Position_trunc(
n,Cyc_substring((struct _tagged_arr)_tmp3B,_tmp3D,Cyc_strlen((struct _tagged_arr)
_tmp3B)- _tmp42));return({struct _tuple5*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=({
struct Cyc_String_pa_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(struct _tagged_arr)((
struct _tagged_arr)sec_three);{struct Cyc_String_pa_struct _tmp47;_tmp47.tag=0;
_tmp47.f1=(struct _tagged_arr)((struct _tagged_arr)sec_two);{struct Cyc_String_pa_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=(struct _tagged_arr)((struct _tagged_arr)sec_one);{
void*_tmp44[3]={& _tmp46,& _tmp47,& _tmp48};Cyc_aprintf(({const char*_tmp45="%s%s%s";
_tag_arr(_tmp45,sizeof(char),_get_zero_arr_size(_tmp45,7));}),_tag_arr(_tmp44,
sizeof(void*),3));}}}});_tmp43->f2=(int)Cyc_strlen((struct _tagged_arr)sec_one);
_tmp43->f3=(int)(Cyc_strlen((struct _tagged_arr)sec_one)+ Cyc_strlen((struct
_tagged_arr)sec_two));_tmp43;});}else{int n=(Cyc_Position_line_length - 3)/ 4;
struct _tagged_arr sec_one=Cyc_Position_trunc(n,Cyc_substring((struct _tagged_arr)
_tmp3B,0,(unsigned int)_tmp3D));struct _tagged_arr sec_two=Cyc_Position_trunc(n,
Cyc_substring((struct _tagged_arr)_tmp3B,_tmp3D,Cyc_strlen((struct _tagged_arr)
_tmp3B)- _tmp3D));struct _tagged_arr sec_three=Cyc_Position_trunc(n,Cyc_substring((
struct _tagged_arr)_tmp40,0,(unsigned int)_tmp42));struct _tagged_arr sec_four=Cyc_Position_trunc(
n,Cyc_substring((struct _tagged_arr)_tmp40,_tmp42,Cyc_strlen((struct _tagged_arr)
_tmp40)- _tmp42));return({struct _tuple5*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->f1=({
struct Cyc_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)((
struct _tagged_arr)sec_four);{struct Cyc_String_pa_struct _tmp4E;_tmp4E.tag=0;
_tmp4E.f1=(struct _tagged_arr)((struct _tagged_arr)sec_three);{struct Cyc_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)((struct _tagged_arr)sec_two);{
struct Cyc_String_pa_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _tagged_arr)((
struct _tagged_arr)sec_one);{void*_tmp4A[4]={& _tmp4C,& _tmp4D,& _tmp4E,& _tmp4F};Cyc_aprintf(({
const char*_tmp4B="%s%s.\\.%s%s";_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(
_tmp4B,12));}),_tag_arr(_tmp4A,sizeof(void*),4));}}}}});_tmp49->f2=(int)Cyc_strlen((
struct _tagged_arr)sec_one);_tmp49->f3=(int)(((Cyc_strlen((struct _tagged_arr)
sec_one)+ Cyc_strlen((struct _tagged_arr)sec_two))+ 3)+ Cyc_strlen((struct
_tagged_arr)sec_three));_tmp49;});}}}}}static int Cyc_Position_error_b=0;int Cyc_Position_error_p(){
return Cyc_Position_error_b;}char Cyc_Position_Error[10]="\000\000\000\000Error\000";
struct Cyc_Position_Error_struct{char*tag;struct Cyc_Position_Error*f1;};int Cyc_Position_print_context=
0;int Cyc_Position_first_error=1;int Cyc_Position_num_errors=0;int Cyc_Position_max_errors=
10;void Cyc_Position_post_error(struct Cyc_Position_Error*e){Cyc_Position_error_b=
1;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stdout);if(Cyc_Position_first_error){({
void*_tmp50[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp51="\n";_tag_arr(_tmp51,
sizeof(char),_get_zero_arr_size(_tmp51,2));}),_tag_arr(_tmp50,sizeof(void*),0));});
Cyc_Position_first_error=0;}if(Cyc_Position_num_errors <= Cyc_Position_max_errors){({
struct Cyc_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)((
struct _tagged_arr)e->desc);{struct Cyc_String_pa_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(e->seg));{
void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_fprintf(Cyc_stderr,({const char*_tmp53="%s: %s\n";
_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,8));}),_tag_arr(_tmp52,
sizeof(void*),2));}}});if(Cyc_Position_print_context){struct _handler_cons _tmp56;
_push_handler(& _tmp56);{int _tmp58=0;if(setjmp(_tmp56.handler))_tmp58=1;if(!
_tmp58){{struct _tuple5*x=Cyc_Position_get_context(e->seg);struct _tagged_arr
marker_str=({unsigned int _tmp63=(unsigned int)((*x).f3 + 1);char*_tmp64=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp63 + 1));struct _tagged_arr _tmp66=
_tag_arr(_tmp64,sizeof(char),_tmp63 + 1);{unsigned int _tmp65=_tmp63;unsigned int i;
for(i=0;i < _tmp65;i ++){_tmp64[i]='\000';}_tmp64[_tmp65]=(char)0;}_tmp66;});int i=
- 1;while(++ i < (*x).f2){({struct _tagged_arr _tmp59=_tagged_arr_plus(marker_str,
sizeof(char),i);char _tmp5A=*((char*)_check_unknown_subscript(_tmp59,sizeof(char),
0));char _tmp5B=' ';if(_get_arr_size(_tmp59,sizeof(char))== 1?_tmp5A == '\000'?
_tmp5B != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});}while(
++ i < (*x).f3){({struct _tagged_arr _tmp5C=_tagged_arr_plus(marker_str,sizeof(char),
i);char _tmp5D=*((char*)_check_unknown_subscript(_tmp5C,sizeof(char),0));char
_tmp5E='^';if(_get_arr_size(_tmp5C,sizeof(char))== 1?_tmp5D == '\000'?_tmp5E != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});}({struct Cyc_String_pa_struct
_tmp62;_tmp62.tag=0;_tmp62.f1=(struct _tagged_arr)((struct _tagged_arr)marker_str);{
struct Cyc_String_pa_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(struct _tagged_arr)((
struct _tagged_arr)(*x).f1);{void*_tmp5F[2]={& _tmp61,& _tmp62};Cyc_fprintf(Cyc_stderr,({
const char*_tmp60="  %s\n  %s\n";_tag_arr(_tmp60,sizeof(char),_get_zero_arr_size(
_tmp60,11));}),_tag_arr(_tmp5F,sizeof(void*),2));}}});};_pop_handler();}else{
void*_tmp57=(void*)_exn_thrown;void*_tmp68=_tmp57;_LL6: if(_tmp68 != Cyc_Position_Nocontext)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp68);_LL5:;}}}}if(Cyc_Position_num_errors
== Cyc_Position_max_errors)({void*_tmp69[0]={};Cyc_fprintf(Cyc_stderr,({const
char*_tmp6A="Too many error messages!\n";_tag_arr(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,26));}),_tag_arr(_tmp69,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);Cyc_Position_num_errors ++;}void Cyc_Position_reset_position(
struct _tagged_arr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}void Cyc_Position_set_position_file(
struct _tagged_arr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}struct
_tagged_arr Cyc_Position_get_position_file(){return Cyc_Position_source;}struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*s){struct
Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,((struct Cyc_Position_Segment*)
_check_null(s))->start);if((struct Cyc_Lineno_Pos*)pos_s != 0)return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct
_tagged_arr)((struct _tagged_arr)pos_s->logical_file);{struct Cyc_Int_pa_struct
_tmp6D;_tmp6D.tag=1;_tmp6D.f1=(unsigned int)pos_s->line_no;{void*_tmp6B[2]={&
_tmp6D,& _tmp6E};Cyc_aprintf(({const char*_tmp6C="\n#line %d \"%s\"\n";_tag_arr(
_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,16));}),_tag_arr(_tmp6B,sizeof(
void*),2));}}});else{return(struct _tagged_arr)_tag_arr(0,0,0);}}extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_Int_struct Cyc__genrep_0={
0,1,32};static char _tmp70[8]="Segment";static struct _tagged_arr Cyc__genname_4={
_tmp70,_tmp70,_tmp70 + 8};static char _tmp71[6]="start";static struct _tuple0 Cyc__gentuple_1={
offsetof(struct Cyc_Position_Segment,start),{_tmp71,_tmp71,_tmp71 + 6},(void*)& Cyc__genrep_0};
static char _tmp72[4]="end";static struct _tuple0 Cyc__gentuple_2={offsetof(struct Cyc_Position_Segment,end),{
_tmp72,_tmp72,_tmp72 + 4},(void*)& Cyc__genrep_0};static struct _tuple0*Cyc__genarr_3[
2]={& Cyc__gentuple_1,& Cyc__gentuple_2};struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep={
3,(struct _tagged_arr*)& Cyc__genname_4,sizeof(struct Cyc_Position_Segment),{(void*)((
struct _tuple0**)Cyc__genarr_3),(void*)((struct _tuple0**)Cyc__genarr_3),(void*)((
struct _tuple0**)Cyc__genarr_3 + 2)}};void*Cyc_segment_rep=(void*)& Cyc_struct_Position_Segment_rep;
