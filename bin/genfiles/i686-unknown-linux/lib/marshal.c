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
 void exit(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;
};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{char*tag;
struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct _tagged_arr Cstring_to_string(char*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[11];struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int f1;
struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple1{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple2{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct _tuple4{int f1;void*
f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct _tagged_arr l);
struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,struct _tagged_arr l);
struct _tagged_arr Cyc_Typerep_get_tagname2(unsigned int tag,struct _tagged_arr l);
struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,struct
_tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);int Cyc_fputc(int,
struct Cyc___cycFILE*);unsigned int Cyc_fread(struct _tagged_arr,unsigned int,
unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};int Cyc_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int isprint(int);struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);void Cyc_Marshal_fprint_type(void*rep,
struct Cyc___cycFILE*fp,void*val);void Cyc_Marshal_print_type(void*rep,void*val);
void Cyc_Marshal_write_type(void*rep,struct Cyc___cycFILE*fp,void*val);struct
_tuple6{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple7{struct _tagged_arr f1;int f2;}
;struct _tuple6 Cyc_Marshal_empty_addr_index();struct _tuple6 Cyc_Marshal_write_type_base(
void*rep,struct _tuple6 env,struct Cyc___cycFILE*fp,void*val);void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*rep,struct Cyc___cycFILE*fp);void*Cyc_Marshal_read_type(
void*rep,struct Cyc___cycFILE*fp);void*Cyc_Marshal_rcopy_type(struct _RegionHandle*,
void*rep,void*val);void*Cyc_Marshal_copy_type(void*rep,void*val);int Cyc_Marshal_leq_type(
void*rep,void*x,void*y);int Cyc_Marshal_eq_type(void*rep,void*x,void*y);int Cyc_Marshal_cmp_type(
void*rep,void*x,void*y);void*unsafe_cast(void*x);static int Cyc_Marshal_uint_cmp(
unsigned int a,unsigned int b){if(a == b)return 0;else{if(a < b)return 1;else{return - 1;}}}
static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){return((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((struct Cyc_Set_Set*(*)(int(*
cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),0);}struct
_tuple8{int f1;struct Cyc_Set_Set*f2;};static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){return({struct _tuple8*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0->f1=((int(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_member)(
set,val);_tmp0->f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))
Cyc_Set_insert)(set,val);_tmp0;});}inline static int Cyc_Marshal_index_member(
struct _tuple6 idx,unsigned int m){int _tmp2;struct _tuple6 _tmp1=idx;_tmp2=_tmp1.f2;
return m < _tmp2;}struct _tuple6 Cyc_Marshal_empty_addr_index(){return({struct
_tuple6 _tmp3;_tmp3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp3.f2=
1;_tmp3;});}static unsigned int Cyc_Marshal_index_lookup(struct _tuple6 index,
unsigned int val){struct Cyc_Dict_Dict*_tmp5;int _tmp6;struct _tuple6 _tmp4=index;
_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;{struct Cyc_Core_Opt*_tmp7=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp5,val);if(_tmp7 != 
0)return(unsigned int)((int)_tmp7->v);else{return(unsigned int)_tmp6;}}}static
struct _tuple6 Cyc_Marshal_index_insert(struct _tuple6 index,unsigned int val){struct
Cyc_Dict_Dict*_tmp9;int _tmpA;struct _tuple6 _tmp8=index;_tmp9=_tmp8.f1;_tmpA=_tmp8.f2;
return({struct _tuple6 _tmpB;_tmpB.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,unsigned int k,int v))Cyc_Dict_insert)(_tmp9,val,_tmpA);_tmpB.f2=_tmpA + 1;_tmpB;});}
static struct _tuple7 Cyc_Marshal_empty_addr_table(){return({struct _tuple7 _tmpC;
_tmpC.f1=_tag_arr(({unsigned int _tmpD=(unsigned int)1;unsigned int*_tmpE=(
unsigned int*)_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmpD));{
unsigned int _tmpF=_tmpD;unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=0;}}_tmpE;}),
sizeof(unsigned int),(unsigned int)1);_tmpC.f2=1;_tmpC;});}inline static int Cyc_Marshal_table_member(
struct _tuple7 table,unsigned int m){int _tmp11;struct _tuple7 _tmp10=table;_tmp11=
_tmp10.f2;return m < _tmp11;}static struct _tuple7 Cyc_Marshal_table_insert(struct
_tuple7 tbl,unsigned int i){struct _tagged_arr _tmp13;int _tmp14;struct _tuple7 _tmp12=
tbl;_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;if(_get_arr_size(_tmp13,sizeof(
unsigned int))== _tmp14)_tmp13=({unsigned int _tmp15=(unsigned int)(2 * _tmp14);
unsigned int*_tmp16=(unsigned int*)_cycalloc_atomic(_check_times(sizeof(
unsigned int),_tmp15));struct _tagged_arr _tmp18=_tag_arr(_tmp16,sizeof(
unsigned int),_tmp15);{unsigned int _tmp17=_tmp15;unsigned int j;for(j=0;j < _tmp17;
j ++){_tmp16[j]=j < _tmp14?*((unsigned int*)_check_unknown_subscript(_tmp13,
sizeof(unsigned int),(int)j)): 0;}}_tmp18;});*((unsigned int*)
_check_unknown_subscript(_tmp13,sizeof(unsigned int),_tmp14))=i;return({struct
_tuple7 _tmp19;_tmp19.f1=_tmp13;_tmp19.f2=_tmp14 + 1;_tmp19;});}static unsigned int
Cyc_Marshal_table_lookup(struct _tuple7 tbl,int i){return*((unsigned int*)
_check_unknown_subscript(tbl.f1,sizeof(unsigned int),i));}static void Cyc_Marshal_fprint_ref(
struct Cyc___cycFILE*fp,unsigned int ref){if(ref == 0)({void*_tmp1A[0]={};Cyc_fprintf(
fp,({const char*_tmp1B="NULL";_tag_arr(_tmp1B,sizeof(char),_get_zero_arr_size(
_tmp1B,5));}),_tag_arr(_tmp1A,sizeof(void*),0));});else{({struct Cyc_Int_pa_struct
_tmp1E;_tmp1E.tag=1;_tmp1E.f1=ref;{void*_tmp1C[1]={& _tmp1E};Cyc_fprintf(fp,({
const char*_tmp1D="(#%x)";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,
6));}),_tag_arr(_tmp1C,sizeof(void*),1));}});}}struct _tuple9{unsigned int f1;
unsigned int f2;unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(
void*rep,struct Cyc_Set_Set*env,struct Cyc___cycFILE*fp,void*val){{void*_tmp1F=rep;
int _tmp20;unsigned int _tmp21;unsigned int _tmp22;void*_tmp23;void*_tmp24;struct
_tagged_arr*_tmp25;struct _tagged_arr _tmp26;struct _tagged_arr _tmp27;struct
_tagged_arr _tmp28;struct _tagged_arr _tmp29;struct _tagged_arr _tmp2A;struct
_tagged_arr _tmp2B;struct _tagged_arr _tmp2C;struct _tagged_arr _tmp2D;struct
_tagged_arr _tmp2E;struct _tagged_arr _tmp2F;struct _tagged_arr*_tmp30;struct
_tagged_arr _tmp31;int _tmp32;struct _tagged_arr _tmp33;_LL1: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 0)goto _LL3;_tmp20=((struct Cyc_Typerep_Int_struct*)_tmp1F)->f1;
_tmp21=((struct Cyc_Typerep_Int_struct*)_tmp1F)->f2;_LL2: switch(_tmp21){case 8:
_LL19: {char*x=((char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*x))({struct
Cyc_Int_pa_struct _tmp36;_tmp36.tag=1;_tmp36.f1=(unsigned int)((int)*x);{void*
_tmp34[1]={& _tmp36};Cyc_fprintf(fp,({const char*_tmp35="'%c'";_tag_arr(_tmp35,
sizeof(char),_get_zero_arr_size(_tmp35,5));}),_tag_arr(_tmp34,sizeof(void*),1));}});
else{({struct Cyc_Int_pa_struct _tmp39;_tmp39.tag=1;_tmp39.f1=(unsigned int)((int)*
x);{void*_tmp37[1]={& _tmp39};Cyc_fprintf(fp,({const char*_tmp38="'\\%d'";_tag_arr(
_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,6));}),_tag_arr(_tmp37,sizeof(void*),
1));}});}break;}case 16: _LL1A: {short*x=((short*(*)(void*x))unsafe_cast)(val);({
struct Cyc_Int_pa_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=(unsigned int)((int)*x);{
void*_tmp3A[1]={& _tmp3D};Cyc_fprintf(fp,_tmp20?({const char*_tmp3B="%d";_tag_arr(
_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,3));}):({const char*_tmp3C="%u";
_tag_arr(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,3));}),_tag_arr(_tmp3A,
sizeof(void*),1));}});break;}case 32: _LL1B: {int*x=((int*(*)(void*x))unsafe_cast)(
val);({struct Cyc_Int_pa_struct _tmp41;_tmp41.tag=1;_tmp41.f1=(unsigned int)*x;{
void*_tmp3E[1]={& _tmp41};Cyc_fprintf(fp,_tmp20?({const char*_tmp3F="%d";_tag_arr(
_tmp3F,sizeof(char),_get_zero_arr_size(_tmp3F,3));}):({const char*_tmp40="%u";
_tag_arr(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,3));}),_tag_arr(_tmp3E,
sizeof(void*),1));}});break;}default: _LL1C:({struct Cyc_Int_pa_struct _tmp44;
_tmp44.tag=1;_tmp44.f1=(unsigned int)((int)_tmp21);{void*_tmp42[1]={& _tmp44};Cyc_fprintf(
Cyc_stderr,({const char*_tmp43="fprint_type_base: illegal int size %d bits\n";
_tag_arr(_tmp43,sizeof(char),_get_zero_arr_size(_tmp43,44));}),_tag_arr(_tmp42,
sizeof(void*),1));}});exit(1);}goto _LL0;_LL3: if((int)_tmp1F != 0)goto _LL5;_LL4: {
float*x=((float*(*)(void*x))unsafe_cast)(val);({struct Cyc_Double_pa_struct _tmp47;
_tmp47.tag=2;_tmp47.f1=(double)*x;{void*_tmp45[1]={& _tmp47};Cyc_fprintf(fp,({
const char*_tmp46="%f";_tag_arr(_tmp46,sizeof(char),_get_zero_arr_size(_tmp46,3));}),
_tag_arr(_tmp45,sizeof(void*),1));}});goto _LL0;}_LL5: if((int)_tmp1F != 1)goto _LL7;
_LL6: {double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Double_pa_struct
_tmp4A;_tmp4A.tag=2;_tmp4A.f1=*x;{void*_tmp48[1]={& _tmp4A};Cyc_fprintf(fp,({
const char*_tmp49="%f";_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,3));}),
_tag_arr(_tmp48,sizeof(void*),1));}});goto _LL0;}_LL7: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 1)goto _LL9;_tmp22=((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F)->f1;
_tmp23=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F)->f2;_LL8: {int*x=((int*(*)(
void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));struct _tuple8 _tmp4C;int
_tmp4D;struct Cyc_Set_Set*_tmp4E;struct _tuple8*_tmp4B=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp4C=*_tmp4B;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;env=_tmp4E;
if(!_tmp4D){({struct Cyc_Int_pa_struct _tmp51;_tmp51.tag=1;_tmp51.f1=(unsigned int)
y;{void*_tmp4F[1]={& _tmp51};Cyc_fprintf(fp,({const char*_tmp50="{(@%x) ";_tag_arr(
_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,8));}),_tag_arr(_tmp4F,sizeof(void*),
1));}});if(y != 0){int typ_szb=(int)Cyc_Typerep_size_type(_tmp23);int i=0;for(0;i < 
_tmp22;i ++){env=Cyc_Marshal_fprint_type_base(_tmp23,env,fp,((void*(*)(int x))
unsafe_cast)(y));if(i != _tmp22 - 1)({void*_tmp52[0]={};Cyc_fprintf(fp,({const char*
_tmp53=", ";_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,3));}),
_tag_arr(_tmp52,sizeof(void*),0));});y +=typ_szb;}}else{({void*_tmp54[0]={};Cyc_fprintf(
fp,({const char*_tmp55="NULL";_tag_arr(_tmp55,sizeof(char),_get_zero_arr_size(
_tmp55,5));}),_tag_arr(_tmp54,sizeof(void*),0));});}({void*_tmp56[0]={};Cyc_fprintf(
fp,({const char*_tmp57=" }";_tag_arr(_tmp57,sizeof(char),_get_zero_arr_size(
_tmp57,3));}),_tag_arr(_tmp56,sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(
fp,(unsigned int)y);}goto _LL0;}_LL9: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 2)
goto _LLB;_tmp24=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1F)->f1;_LLA: {
struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmp24);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple8 _tmp59;int _tmp5A;struct Cyc_Set_Set*
_tmp5B;struct _tuple8*_tmp58=Cyc_Marshal_member_insert(env,base);_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;env=_tmp5B;if(!_tmp5A){({struct Cyc_Int_pa_struct
_tmp5E;_tmp5E.tag=1;_tmp5E.f1=base;{void*_tmp5C[1]={& _tmp5E};Cyc_fprintf(fp,({
const char*_tmp5D="[(@%x) ";_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(
_tmp5D,8));}),_tag_arr(_tmp5C,sizeof(void*),1));}});{unsigned int i=base;for(0;i < 
last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(_tmp24,env,fp,((void*(*)(
unsigned int x))unsafe_cast)(i));if(i + typ_szb < last_plus_one)({void*_tmp5F[0]={};
Cyc_fprintf(fp,({const char*_tmp60=", ";_tag_arr(_tmp60,sizeof(char),
_get_zero_arr_size(_tmp60,3));}),_tag_arr(_tmp5F,sizeof(void*),0));});}}({void*
_tmp61[0]={};Cyc_fprintf(fp,({const char*_tmp62=" ]";_tag_arr(_tmp62,sizeof(char),
_get_zero_arr_size(_tmp62,3));}),_tag_arr(_tmp61,sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(
fp,base);}goto _LL0;}_LLB: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 3)goto _LLD;
_tmp25=((struct Cyc_Typerep_Struct_struct*)_tmp1F)->f1;_tmp26=((struct Cyc_Typerep_Struct_struct*)
_tmp1F)->f3;_LLC: if(_tmp25 == 0)({void*_tmp63[0]={};Cyc_fprintf(fp,({const char*
_tmp64="{ ";_tag_arr(_tmp64,sizeof(char),_get_zero_arr_size(_tmp64,3));}),
_tag_arr(_tmp63,sizeof(void*),0));});else{({struct Cyc_String_pa_struct _tmp67;
_tmp67.tag=0;_tmp67.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp25);{void*
_tmp65[1]={& _tmp67};Cyc_fprintf(fp,({const char*_tmp66="%s{ ";_tag_arr(_tmp66,
sizeof(char),_get_zero_arr_size(_tmp66,5));}),_tag_arr(_tmp65,sizeof(void*),1));}});}{
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp26,
sizeof(struct _tuple0*));i ++){unsigned int _tmp69;struct _tagged_arr _tmp6A;void*
_tmp6B;struct _tuple0 _tmp68=*((struct _tuple0**)_tmp26.curr)[i];_tmp69=_tmp68.f1;
_tmp6A=_tmp68.f2;_tmp6B=_tmp68.f3;({struct Cyc_String_pa_struct _tmp6E;_tmp6E.tag=
0;_tmp6E.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp6A);{void*_tmp6C[1]={&
_tmp6E};Cyc_fprintf(fp,({const char*_tmp6D="%s=";_tag_arr(_tmp6D,sizeof(char),
_get_zero_arr_size(_tmp6D,4));}),_tag_arr(_tmp6C,sizeof(void*),1));}});{int x_ofs=(
int)(x + _tmp69);env=Cyc_Marshal_fprint_type_base(_tmp6B,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp26,sizeof(struct _tuple0*))- 1)({
void*_tmp6F[0]={};Cyc_fprintf(fp,({const char*_tmp70=", ";_tag_arr(_tmp70,sizeof(
char),_get_zero_arr_size(_tmp70,3));}),_tag_arr(_tmp6F,sizeof(void*),0));});}}}({
void*_tmp71[0]={};Cyc_fprintf(fp,({const char*_tmp72=" }";_tag_arr(_tmp72,sizeof(
char),_get_zero_arr_size(_tmp72,3));}),_tag_arr(_tmp71,sizeof(void*),0));});goto
_LL0;}_LLD: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 4)goto _LLF;_tmp27=((struct
Cyc_Typerep_Tuple_struct*)_tmp1F)->f2;_LLE:({void*_tmp73[0]={};Cyc_fprintf(fp,({
const char*_tmp74="$( ";_tag_arr(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,4));}),
_tag_arr(_tmp73,sizeof(void*),0));});{int x=((int(*)(void*x))unsafe_cast)(val);{
int i=0;for(0;i < _get_arr_size(_tmp27,sizeof(struct _tuple1*));i ++){unsigned int
_tmp76;void*_tmp77;struct _tuple1 _tmp75=*((struct _tuple1**)_tmp27.curr)[i];_tmp76=
_tmp75.f1;_tmp77=_tmp75.f2;{int x_ofs=(int)(x + _tmp76);env=Cyc_Marshal_fprint_type_base(
_tmp77,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp27,
sizeof(struct _tuple1*))- 1)({void*_tmp78[0]={};Cyc_fprintf(fp,({const char*_tmp79=", ";
_tag_arr(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,3));}),_tag_arr(_tmp78,
sizeof(void*),0));});}}}({void*_tmp7A[0]={};Cyc_fprintf(fp,({const char*_tmp7B=" )";
_tag_arr(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,3));}),_tag_arr(_tmp7A,
sizeof(void*),0));});goto _LL0;}_LLF: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 6)
goto _LL11;_tmp28=((struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f1;_tmp29=((
struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f2;_tmp2A=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1F)->f4;_LL10:({struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp29);{struct Cyc_String_pa_struct _tmp7E;
_tmp7E.tag=0;_tmp7E.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp28);{void*
_tmp7C[2]={& _tmp7E,& _tmp7F};Cyc_fprintf(fp,({const char*_tmp7D="%s.%s(";_tag_arr(
_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,7));}),_tag_arr(_tmp7C,sizeof(void*),
2));}}});{int x=((int(*)(void*x))unsafe_cast)(val);{int i=1;for(0;i < _get_arr_size(
_tmp2A,sizeof(struct _tuple1*));i ++){unsigned int _tmp81;void*_tmp82;struct _tuple1
_tmp80=*((struct _tuple1**)_tmp2A.curr)[i];_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{int
x_ofs=(int)(x + _tmp81);env=Cyc_Marshal_fprint_type_base(_tmp82,env,fp,((void*(*)(
int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp2A,sizeof(struct _tuple1*))- 
1)({void*_tmp83[0]={};Cyc_fprintf(fp,({const char*_tmp84=", ";_tag_arr(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,3));}),_tag_arr(_tmp83,sizeof(void*),0));});}}}({
void*_tmp85[0]={};Cyc_fprintf(fp,({const char*_tmp86=" )";_tag_arr(_tmp86,sizeof(
char),_get_zero_arr_size(_tmp86,3));}),_tag_arr(_tmp85,sizeof(void*),0));});goto
_LL0;}_LL11: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 5)goto _LL13;_tmp2B=((struct
Cyc_Typerep_TUnion_struct*)_tmp1F)->f1;_tmp2C=((struct Cyc_Typerep_TUnion_struct*)
_tmp1F)->f2;_tmp2D=((struct Cyc_Typerep_TUnion_struct*)_tmp1F)->f3;_LL12: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)({
struct Cyc_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Typerep_get_tagname(*tagp,_tmp2C));{void*_tmp87[1]={&
_tmp89};Cyc_fprintf(fp,({const char*_tmp88="%s";_tag_arr(_tmp88,sizeof(char),
_get_zero_arr_size(_tmp88,3));}),_tag_arr(_tmp87,sizeof(void*),1));}});else{
struct _tuple8 _tmp8B;int _tmp8C;struct Cyc_Set_Set*_tmp8D;struct _tuple8*_tmp8A=Cyc_Marshal_member_insert(
env,*tagp);_tmp8B=*_tmp8A;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;env=_tmp8D;if(!
_tmp8C){({struct Cyc_Int_pa_struct _tmp90;_tmp90.tag=1;_tmp90.f1=*tagp;{void*
_tmp8E[1]={& _tmp90};Cyc_fprintf(fp,({const char*_tmp8F="(@%x)";_tag_arr(_tmp8F,
sizeof(char),_get_zero_arr_size(_tmp8F,6));}),_tag_arr(_tmp8E,sizeof(void*),1));}});{
unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;int _tmp92;void*_tmp93;struct _tuple4 _tmp91=Cyc_Typerep_get_unionbranch(
tag,_tmp2D);_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{void*xoff=((void*(*)(unsigned int
x))unsafe_cast)(*tagp + _tmp92);({struct Cyc_String_pa_struct _tmp96;_tmp96.tag=0;
_tmp96.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Typerep_get_tagname2(tag,
_tmp2D));{void*_tmp94[1]={& _tmp96};Cyc_fprintf(fp,({const char*_tmp95="&%s[";
_tag_arr(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,5));}),_tag_arr(_tmp94,
sizeof(void*),1));}});env=Cyc_Marshal_fprint_type_base(_tmp93,env,fp,xoff);({
void*_tmp97[0]={};Cyc_fprintf(fp,({const char*_tmp98="]";_tag_arr(_tmp98,sizeof(
char),_get_zero_arr_size(_tmp98,2));}),_tag_arr(_tmp97,sizeof(void*),0));});}}}
else{Cyc_Marshal_fprint_ref(fp,*tagp);}}goto _LL0;}_LL13: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 7)goto _LL15;_tmp2E=((struct Cyc_Typerep_XTUnion_struct*)_tmp1F)->f1;
_tmp2F=((struct Cyc_Typerep_XTUnion_struct*)_tmp1F)->f2;_LL14: {unsigned int*
xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);struct _tuple8 _tmp9A;int
_tmp9B;struct Cyc_Set_Set*_tmp9C;struct _tuple8*_tmp99=Cyc_Marshal_member_insert(
env,*xtunionp);_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;env=_tmp9C;if(!
_tmp9B){({struct Cyc_Int_pa_struct _tmp9F;_tmp9F.tag=1;_tmp9F.f1=*xtunionp;{void*
_tmp9D[1]={& _tmp9F};Cyc_fprintf(fp,({const char*_tmp9E="(@%x)";_tag_arr(_tmp9E,
sizeof(char),_get_zero_arr_size(_tmp9E,6));}),_tag_arr(_tmp9D,sizeof(void*),1));}});{
unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);
if(*xtstructp != 0){char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_String_pa_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct
_tagged_arr)((struct _tagged_arr)xtname);{void*_tmpA0[1]={& _tmpA2};Cyc_fprintf(fp,({
const char*_tmpA1="&%s[( ";_tag_arr(_tmpA1,sizeof(char),_get_zero_arr_size(_tmpA1,
7));}),_tag_arr(_tmpA0,sizeof(void*),1));}});{int _tmpA4;void*_tmpA5;struct
_tuple4 _tmpA3=Cyc_Typerep_get_xtunionbranch(xtname,_tmp2F);_tmpA4=_tmpA3.f1;
_tmpA5=_tmpA3.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*xtunionp + 
_tmpA4);env=Cyc_Marshal_fprint_type_base(_tmpA5,env,fp,xoff);({void*_tmpA6[0]={};
Cyc_fprintf(fp,({const char*_tmpA7=" )]";_tag_arr(_tmpA7,sizeof(char),
_get_zero_arr_size(_tmpA7,4));}),_tag_arr(_tmpA6,sizeof(void*),0));});}}}else{
char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct
_tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _tagged_arr)((struct _tagged_arr)xtname);{
void*_tmpA8[1]={& _tmpAA};Cyc_fprintf(fp,({const char*_tmpA9="%s";_tag_arr(_tmpA9,
sizeof(char),_get_zero_arr_size(_tmpA9,3));}),_tag_arr(_tmpA8,sizeof(void*),1));}});}}}
else{Cyc_Marshal_fprint_ref(fp,*xtunionp);}goto _LL0;}_LL15: if(_tmp1F <= (void*)2?
1:*((int*)_tmp1F)!= 8)goto _LL17;_tmp30=((struct Cyc_Typerep_Union_struct*)_tmp1F)->f1;
_tmp31=((struct Cyc_Typerep_Union_struct*)_tmp1F)->f3;_LL16: if(_tmp30 == 0)({void*
_tmpAB[0]={};Cyc_fprintf(fp,({const char*_tmpAC="{";_tag_arr(_tmpAC,sizeof(char),
_get_zero_arr_size(_tmpAC,2));}),_tag_arr(_tmpAB,sizeof(void*),0));});else{({
struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp30);{void*_tmpAD[1]={& _tmpAF};Cyc_fprintf(fp,({const char*
_tmpAE="%s{";_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,4));}),
_tag_arr(_tmpAD,sizeof(void*),1));}});}{int i=0;for(0;i < _get_arr_size(_tmp31,
sizeof(struct _tuple3*));i ++){struct _tagged_arr _tmpB1;void*_tmpB2;struct _tuple3
_tmpB0=*((struct _tuple3**)_tmp31.curr)[i];_tmpB1=_tmpB0.f1;_tmpB2=_tmpB0.f2;({
struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmpB1);{void*_tmpB3[1]={& _tmpB5};Cyc_fprintf(fp,({const char*
_tmpB4="%s=";_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,4));}),
_tag_arr(_tmpB3,sizeof(void*),1));}});Cyc_Marshal_fprint_type_base(_tmpB2,env,fp,
val);if(i < _get_arr_size(_tmp31,sizeof(struct _tuple3*))- 1)({void*_tmpB6[0]={};
Cyc_fprintf(fp,({const char*_tmpB7=" | ";_tag_arr(_tmpB7,sizeof(char),
_get_zero_arr_size(_tmpB7,4));}),_tag_arr(_tmpB6,sizeof(void*),0));});}}({void*
_tmpB8[0]={};Cyc_fprintf(fp,({const char*_tmpB9="}";_tag_arr(_tmpB9,sizeof(char),
_get_zero_arr_size(_tmpB9,2));}),_tag_arr(_tmpB8,sizeof(void*),0));});goto _LL0;
_LL17: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 9)goto _LL0;_tmp32=((struct Cyc_Typerep_Enum_struct*)
_tmp1F)->f2;_tmp33=((struct Cyc_Typerep_Enum_struct*)_tmp1F)->f3;_LL18: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);({struct Cyc_String_pa_struct
_tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Typerep_get_tagname(*
tagp,_tmp33));{void*_tmpBA[1]={& _tmpBC};Cyc_fprintf(fp,({const char*_tmpBB="%s";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,3));}),_tag_arr(_tmpBA,
sizeof(void*),1));}});goto _LL0;}_LL0:;}return env;}void Cyc_Marshal_fprint_type(
void*ts,struct Cyc___cycFILE*fp,void*val){struct _handler_cons _tmpBD;_push_handler(&
_tmpBD);{int _tmpBF=0;if(setjmp(_tmpBD.handler))_tmpBF=1;if(!_tmpBF){Cyc_Marshal_fprint_type_base(
ts,Cyc_Marshal_empty_addr_set(),fp,val);;_pop_handler();}else{void*_tmpBE=(void*)
_exn_thrown;void*_tmpC1=_tmpBE;struct _tagged_arr _tmpC2;_LL1F: if(*((void**)_tmpC1)
!= Cyc_Core_Failure)goto _LL21;_tmpC2=((struct Cyc_Core_Failure_struct*)_tmpC1)->f1;
_LL20:({struct Cyc_String_pa_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct
_tagged_arr)((struct _tagged_arr)_tmpC2);{void*_tmpC3[1]={& _tmpC5};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC4="Core::Failure(%s)\n";_tag_arr(_tmpC4,sizeof(char),
_get_zero_arr_size(_tmpC4,19));}),_tag_arr(_tmpC3,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));
_tmpC6[0]=({struct Cyc_Core_Failure_struct _tmpC7;_tmpC7.tag=Cyc_Core_Failure;
_tmpC7.f1=_tmpC2;_tmpC7;});_tmpC6;}));_LL21:;_LL22:(void)_throw(_tmpC1);_LL1E:;}}}
void Cyc_Marshal_print_type(void*ts,void*val){Cyc_Marshal_fprint_type(ts,Cyc_stdout,
val);}void Cyc_Marshal_cycputchar(char x,struct Cyc___cycFILE*fp){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Core_Failure_struct _tmpC9;_tmpC9.tag=Cyc_Core_Failure;_tmpC9.f1=({
const char*_tmpCA="Write failure";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(
_tmpCA,14));});_tmpC9;});_tmpC8;}));}void Cyc_Marshal_cycputshort(short x,struct
Cyc___cycFILE*fp){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpCB=
_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Failure_struct _tmpCC;
_tmpCC.tag=Cyc_Core_Failure;_tmpCC.f1=({const char*_tmpCD="Write failure";
_tag_arr(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,14));});_tmpCC;});_tmpCB;}));}
void Cyc_Marshal_cycputint(int x,struct Cyc___cycFILE*fp){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({
struct Cyc_Core_Failure_struct _tmpCF;_tmpCF.tag=Cyc_Core_Failure;_tmpCF.f1=({
const char*_tmpD0="Write failure";_tag_arr(_tmpD0,sizeof(char),_get_zero_arr_size(
_tmpD0,14));});_tmpCF;});_tmpCE;}));}void Cyc_Marshal_cycputfloat(float x,struct
Cyc___cycFILE*fp){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD1=
_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Core_Failure_struct _tmpD2;
_tmpD2.tag=Cyc_Core_Failure;_tmpD2.f1=({const char*_tmpD3="Write failure";
_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,14));});_tmpD2;});_tmpD1;}));}
void Cyc_Marshal_cycputdouble(double x,struct Cyc___cycFILE*fp){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({
struct Cyc_Core_Failure_struct _tmpD5;_tmpD5.tag=Cyc_Core_Failure;_tmpD5.f1=({
const char*_tmpD6="Write failure";_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(
_tmpD6,14));});_tmpD5;});_tmpD4;}));}void Cyc_Marshal_cycputvarint(unsigned int i,
struct Cyc___cycFILE*fp){int width=0;unsigned int mask=-256;while((int)mask){if((
int)(i & mask))width ++;mask <<=8;}Cyc_fputc((int)(width << 6 | i & 63),fp);i >>=6;{int
j=0;for(0;j < width;j ++){Cyc_fputc((int)(i & 255),fp);i >>=8;}}}char Cyc_Marshal_cycgetchar(
struct Cyc___cycFILE*fp){struct _RegionHandle _tmpD7=_new_region("r");struct
_RegionHandle*r=& _tmpD7;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpDC=sizeof(char);char*_tmpDD=(char*)_region_malloc(r,_check_times(
sizeof(char),_tmpDC));{unsigned int _tmpDE=_tmpDC;unsigned int i;for(i=0;i < _tmpDE;
i ++){_tmpDD[i]='\000';}}_tmpDD;}),sizeof(char),sizeof(char));if(Cyc_fread(x,
sizeof(char),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD8=
_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Core_Failure_struct _tmpD9;
_tmpD9.tag=Cyc_Core_Failure;_tmpD9.f1=({const char*_tmpDA="Read failure";_tag_arr(
_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,13));});_tmpD9;});_tmpD8;}));{char*
xx=((char*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(char),
1)));char _tmpDB=*xx;_npop_handler(0);return _tmpDB;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc___cycFILE*fp){struct _RegionHandle _tmpDF=_new_region("r");struct
_RegionHandle*r=& _tmpDF;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpE4=sizeof(short);char*_tmpE5=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmpE4));{unsigned int _tmpE6=_tmpE4;unsigned int i;for(
i=0;i < _tmpE6;i ++){_tmpE5[i]='\000';}}_tmpE5;}),sizeof(char),sizeof(short));if(
Cyc_fread(x,sizeof(short),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Core_Failure_struct
_tmpE1;_tmpE1.tag=Cyc_Core_Failure;_tmpE1.f1=({const char*_tmpE2="Read failure";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,13));});_tmpE1;});_tmpE0;}));{
short*xx=((short*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,
sizeof(char),1)));short _tmpE3=*xx;_npop_handler(0);return _tmpE3;}};_pop_region(r);}
int Cyc_Marshal_cycgetint(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpE7=
_new_region("r");struct _RegionHandle*r=& _tmpE7;_push_region(r);{struct
_tagged_arr x=_tag_arr(({unsigned int _tmpEC=sizeof(int);char*_tmpED=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmpEC));{unsigned int _tmpEE=_tmpEC;
unsigned int i;for(i=0;i < _tmpEE;i ++){_tmpED[i]='\000';}}_tmpED;}),sizeof(char),
sizeof(int));if(Cyc_fread(x,sizeof(int),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Core_Failure_struct
_tmpE9;_tmpE9.tag=Cyc_Core_Failure;_tmpE9.f1=({const char*_tmpEA="Read failure";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,13));});_tmpE9;});_tmpE8;}));{
int*xx=((int*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(
char),1)));int _tmpEB=*xx;_npop_handler(0);return _tmpEB;}};_pop_region(r);}float
Cyc_Marshal_cycgetfloat(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpEF=
_new_region("r");struct _RegionHandle*r=& _tmpEF;_push_region(r);{struct
_tagged_arr x=_tag_arr(({unsigned int _tmpF4=sizeof(float);char*_tmpF5=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmpF4));{unsigned int _tmpF6=_tmpF4;
unsigned int i;for(i=0;i < _tmpF6;i ++){_tmpF5[i]='\000';}}_tmpF5;}),sizeof(char),
sizeof(float));if(Cyc_fread(x,sizeof(float),1,fp)< 1)(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Core_Failure_struct
_tmpF1;_tmpF1.tag=Cyc_Core_Failure;_tmpF1.f1=({const char*_tmpF2="Read failure";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,13));});_tmpF1;});_tmpF0;}));{
float*xx=((float*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,
sizeof(char),1)));float _tmpF3=*xx;_npop_handler(0);return _tmpF3;}};_pop_region(r);}
double Cyc_Marshal_cycgetdouble(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpF7=
_new_region("r");struct _RegionHandle*r=& _tmpF7;_push_region(r);{struct
_tagged_arr x=_tag_arr(({unsigned int _tmpFC=sizeof(double);char*_tmpFD=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmpFC));{unsigned int _tmpFE=_tmpFC;
unsigned int i;for(i=0;i < _tmpFE;i ++){_tmpFD[i]='\000';}}_tmpFD;}),sizeof(char),
sizeof(double));if(Cyc_fread(x,sizeof(double),1,fp)< 1)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({
struct Cyc_Core_Failure_struct _tmpF9;_tmpF9.tag=Cyc_Core_Failure;_tmpF9.f1=({
const char*_tmpFA="Read failure";_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(
_tmpFA,13));});_tmpF9;});_tmpF8;}));{double*xx=((double*(*)(char*x))unsafe_cast)((
char*)_check_null(_untag_arr(x,sizeof(char),1)));double _tmpFB=*xx;_npop_handler(
0);return _tmpFB;}};_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(struct
Cyc___cycFILE*fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(j & 192)
>> 6;int pos=6;i +=j & 63;{int k=0;for(0;k < width;k ++){j=(int)Cyc_Marshal_cycgetchar(
fp);i |=j << pos;pos +=8;}}return(unsigned int)i;}struct _tuple6 Cyc_Marshal_write_type_base(
void*rep,struct _tuple6 env,struct Cyc___cycFILE*fp,void*val){{void*_tmpFF=rep;
unsigned int _tmp100;unsigned int _tmp101;void*_tmp102;void*_tmp103;struct
_tagged_arr _tmp104;struct _tagged_arr _tmp105;struct _tagged_arr _tmp106;struct
_tagged_arr _tmp107;struct _tagged_arr _tmp108;struct _tagged_arr _tmp109;int _tmp10A;
int _tmp10B;_LL24: if(_tmpFF <= (void*)2?1:*((int*)_tmpFF)!= 0)goto _LL26;_tmp100=((
struct Cyc_Typerep_Int_struct*)_tmpFF)->f2;_LL25: switch(_tmp100){case 8: _LL3C: {
char*x=((char*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}
case 16: _LL3D: {short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*
x,fp);break;}case 32: _LL3E: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL3F:({struct Cyc_Int_pa_struct _tmp10E;_tmp10E.tag=1;
_tmp10E.f1=(unsigned int)((int)_tmp100);{void*_tmp10C[1]={& _tmp10E};Cyc_fprintf(
Cyc_stderr,({const char*_tmp10D="write_type_base: illegal int size %d bits\n";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,43));}),_tag_arr(
_tmp10C,sizeof(void*),1));}});exit(1);}goto _LL23;_LL26: if((int)_tmpFF != 0)goto
_LL28;_LL27: {float*x=((float*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*
x,fp);goto _LL23;}_LL28: if((int)_tmpFF != 1)goto _LL2A;_LL29: {double*x=((double*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*x,fp);goto _LL23;}_LL2A: if(
_tmpFF <= (void*)2?1:*((int*)_tmpFF)!= 1)goto _LL2C;_tmp101=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpFF)->f1;_tmp102=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpFF)->f2;_LL2B: {
int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));
unsigned int _tmp10F=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint(
_tmp10F,fp);if(!Cyc_Marshal_index_member(env,_tmp10F)){env=Cyc_Marshal_index_insert(
env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(_tmp102);int i=0;for(
0;i < _tmp101;i ++){env=Cyc_Marshal_write_type_base(_tmp102,env,fp,((void*(*)(int x))
unsafe_cast)(y));y +=typ_szb;}}}goto _LL23;}_LL2C: if(_tmpFF <= (void*)2?1:*((int*)
_tmpFF)!= 2)goto _LL2E;_tmp103=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpFF)->f1;
_LL2D: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmp103);unsigned int base=(*x).f1;
unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr
- base)/ typ_szb;unsigned int sz=(last_plus_one - base)/ typ_szb;unsigned int
_tmp110=Cyc_Marshal_index_lookup(env,base);Cyc_Marshal_cycputvarint(_tmp110,fp);
Cyc_Marshal_cycputvarint(pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(!Cyc_Marshal_index_member(
env,_tmp110)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;for(0;i
< last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmp103,env,fp,((
void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL23;}_LL2E: if(_tmpFF <= (void*)
2?1:*((int*)_tmpFF)!= 3)goto _LL30;_tmp104=((struct Cyc_Typerep_Struct_struct*)
_tmpFF)->f3;_LL2F: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp104,sizeof(struct _tuple0*));i ++){unsigned int _tmp112;void*
_tmp113;struct _tuple0 _tmp111=*((struct _tuple0**)_tmp104.curr)[i];_tmp112=_tmp111.f1;
_tmp113=_tmp111.f3;{int x_ofs=(int)(x + _tmp112);env=Cyc_Marshal_write_type_base(
_tmp113,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL30: if(
_tmpFF <= (void*)2?1:*((int*)_tmpFF)!= 4)goto _LL32;_tmp105=((struct Cyc_Typerep_Tuple_struct*)
_tmpFF)->f2;_LL31: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp105,sizeof(struct _tuple1*));i ++){unsigned int _tmp115;void*
_tmp116;struct _tuple1 _tmp114=*((struct _tuple1**)_tmp105.curr)[i];_tmp115=_tmp114.f1;
_tmp116=_tmp114.f2;{int x_ofs=(int)(x + _tmp115);env=Cyc_Marshal_write_type_base(
_tmp116,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL32: if(
_tmpFF <= (void*)2?1:*((int*)_tmpFF)!= 6)goto _LL34;_tmp106=((struct Cyc_Typerep_TUnionField_struct*)
_tmpFF)->f4;_LL33: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp106,sizeof(struct _tuple1*));i ++){unsigned int _tmp118;void*
_tmp119;struct _tuple1 _tmp117=*((struct _tuple1**)_tmp106.curr)[i];_tmp118=_tmp117.f1;
_tmp119=_tmp117.f2;{int x_ofs=(int)(x + _tmp118);env=Cyc_Marshal_write_type_base(
_tmp119,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL34: if(
_tmpFF <= (void*)2?1:*((int*)_tmpFF)!= 5)goto _LL36;_tmp107=((struct Cyc_Typerep_TUnion_struct*)
_tmpFF)->f2;_tmp108=((struct Cyc_Typerep_TUnion_struct*)_tmpFF)->f3;_LL35: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)
Cyc_Marshal_cycputvarint(*tagp,fp);else{unsigned int _tmp11A=Cyc_Marshal_index_lookup(
env,*tagp);Cyc_Marshal_cycputvarint(_tmp11A + 1024,fp);if(!Cyc_Marshal_index_member(
env,_tmp11A)){env=Cyc_Marshal_index_insert(env,*tagp);{unsigned int*x=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;Cyc_Marshal_cycputvarint(
tag,fp);{int _tmp11C;void*_tmp11D;struct _tuple4 _tmp11B=Cyc_Typerep_get_unionbranch(
tag,_tmp108);_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*tagp + _tmp11C);env=Cyc_Marshal_write_type_base(
_tmp11D,env,fp,xoff);}}}}}goto _LL23;}_LL36: if(_tmpFF <= (void*)2?1:*((int*)_tmpFF)
!= 7)goto _LL38;_tmp109=((struct Cyc_Typerep_XTUnion_struct*)_tmpFF)->f2;_LL37: {
unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int
_tmp11E=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint(_tmp11E,
fp);if(!Cyc_Marshal_index_member(env,_tmp11E)){env=Cyc_Marshal_index_insert(env,*
xtunionp);{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*
xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',fp);{char*xtnamec=((
char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct _tagged_arr xtname=(
struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_String_pa_struct _tmp121;
_tmp121.tag=0;_tmp121.f1=(struct _tagged_arr)((struct _tagged_arr)xtname);{void*
_tmp11F[1]={& _tmp121};Cyc_fprintf(fp,({const char*_tmp120="%s";_tag_arr(_tmp120,
sizeof(char),_get_zero_arr_size(_tmp120,3));}),_tag_arr(_tmp11F,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);{int _tmp123;void*_tmp124;struct _tuple4 _tmp122=
Cyc_Typerep_get_xtunionbranch(xtname,_tmp109);_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;{
void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*xtunionp + _tmp123);env=Cyc_Marshal_write_type_base(
_tmp124,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',fp);{char*xtnamec=((
char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct _tagged_arr xtname=(
struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_String_pa_struct _tmp127;
_tmp127.tag=0;_tmp127.f1=(struct _tagged_arr)((struct _tagged_arr)xtname);{void*
_tmp125[1]={& _tmp127};Cyc_fprintf(fp,({const char*_tmp126="%s";_tag_arr(_tmp126,
sizeof(char),_get_zero_arr_size(_tmp126,3));}),_tag_arr(_tmp125,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);}}}}goto _LL23;}_LL38: if(_tmpFF <= (void*)2?1:*((
int*)_tmpFF)!= 8)goto _LL3A;_tmp10A=((struct Cyc_Typerep_Union_struct*)_tmpFF)->f2;
_LL39: env=Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Tuple_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Typerep_Tuple_struct
_tmp129;_tmp129.tag=4;_tmp129.f1=(unsigned int)_tmp10A;_tmp129.f2=({unsigned int
_tmp12A=(unsigned int)_tmp10A;struct _tuple1**_tmp12B=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp12A));struct _tagged_arr _tmp130=_tag_arr(
_tmp12B,sizeof(struct _tuple1*),_tmp12A);{unsigned int _tmp12C=_tmp12A;
unsigned int i;for(i=0;i < _tmp12C;i ++){_tmp12B[i]=({struct _tuple1*_tmp12D=
_cycalloc(sizeof(*_tmp12D));_tmp12D->f1=i;_tmp12D->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp12E=_cycalloc_atomic(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Typerep_Int_struct
_tmp12F;_tmp12F.tag=0;_tmp12F.f1=0;_tmp12F.f2=8;_tmp12F;});_tmp12E;});_tmp12D;});}}
_tmp130;});_tmp129;});_tmp128;}),env,fp,val);goto _LL23;_LL3A: if(_tmpFF <= (void*)
2?1:*((int*)_tmpFF)!= 9)goto _LL23;_tmp10B=((struct Cyc_Typerep_Enum_struct*)
_tmpFF)->f2;_LL3B: env=Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Int_struct*
_tmp131=_cycalloc_atomic(sizeof(*_tmp131));_tmp131[0]=({struct Cyc_Typerep_Int_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=0;_tmp132.f2=(unsigned int)(_tmp10B << 3);
_tmp132;});_tmp131;}),env,fp,val);_LL23:;}return env;}void Cyc_Marshal_write_type(
void*ts,struct Cyc___cycFILE*fp,void*val){struct _handler_cons _tmp133;
_push_handler(& _tmp133);{int _tmp135=0;if(setjmp(_tmp133.handler))_tmp135=1;if(!
_tmp135){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),fp,val);;
_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp137=_tmp134;struct
_tagged_arr _tmp138;_LL42: if(*((void**)_tmp137)!= Cyc_Core_Failure)goto _LL44;
_tmp138=((struct Cyc_Core_Failure_struct*)_tmp137)->f1;_LL43:({struct Cyc_String_pa_struct
_tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp138);{
void*_tmp139[1]={& _tmp13B};Cyc_fprintf(Cyc_stderr,({const char*_tmp13A="Core::Failure(%s)\n";
_tag_arr(_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,19));}),_tag_arr(
_tmp139,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=({struct Cyc_Core_Failure_struct
_tmp13D;_tmp13D.tag=Cyc_Core_Failure;_tmp13D.f1=_tmp138;_tmp13D;});_tmp13C;}));
_LL44:;_LL45:(void)_throw(_tmp137);_LL41:;}}}void Cyc_Marshal_fscanntstring(
struct Cyc___cycFILE*fp,struct _tagged_arr s){int c;int i=0;while((c=Cyc_fgetc(fp))!= '\000'){
if(c == - 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp13E=_cycalloc(
sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Core_Failure_struct _tmp13F;_tmp13F.tag=
Cyc_Core_Failure;_tmp13F.f1=({const char*_tmp140="Read error";_tag_arr(_tmp140,
sizeof(char),_get_zero_arr_size(_tmp140,11));});_tmp13F;});_tmp13E;}));({struct
_tagged_arr _tmp141=_tagged_arr_plus(s,sizeof(char),i);char _tmp142=*((char*)
_check_unknown_subscript(_tmp141,sizeof(char),0));char _tmp143=(char)c;if(
_get_arr_size(_tmp141,sizeof(char))== 1?_tmp142 == '\000'?_tmp143 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp141.curr)=_tmp143;});i ++;}({struct _tagged_arr
_tmp144=_tagged_arr_plus(s,sizeof(char),i);char _tmp145=*((char*)
_check_unknown_subscript(_tmp144,sizeof(char),0));char _tmp146='\000';if(
_get_arr_size(_tmp144,sizeof(char))== 1?_tmp145 == '\000'?_tmp146 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp144.curr)=_tmp146;});}struct _tuple7 Cyc_Marshal_rread_type_base(
struct _RegionHandle*r,void*rep,struct _tuple7 env,struct Cyc___cycFILE*fp,void*
result){{void*_tmp147=rep;unsigned int _tmp148;unsigned int _tmp149;void*_tmp14A;
void*_tmp14B;struct _tagged_arr _tmp14C;struct _tagged_arr _tmp14D;struct _tagged_arr
_tmp14E;struct _tagged_arr _tmp14F;struct _tagged_arr _tmp150;struct _tagged_arr
_tmp151;int _tmp152;int _tmp153;_LL47: if(_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 0)
goto _LL49;_tmp148=((struct Cyc_Typerep_Int_struct*)_tmp147)->f2;_LL48: switch(
_tmp148){case 8: _LL5F: {char*x=((char*(*)(void*x))unsafe_cast)(result);({struct
_tagged_arr _tmp155=({char*_tmp154=x;_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,1));});char _tmp156=*((char*)_check_unknown_subscript(
_tmp155,sizeof(char),0));char _tmp157=Cyc_Marshal_cycgetchar(fp);if(_get_arr_size(
_tmp155,sizeof(char))== 1?_tmp156 == '\000'?_tmp157 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp155.curr)=_tmp157;});break;}case 16: _LL60: {
short*x=((short*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(fp);
break;}case 32: _LL61: {int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(
fp);break;}default: _LL62:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Core_Failure_struct
_tmp159;_tmp159.tag=Cyc_Core_Failure;_tmp159.f1=({const char*_tmp15A="read_type_base: illegal int size bits";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,38));});_tmp159;});
_tmp158;}));}goto _LL46;_LL49: if((int)_tmp147 != 0)goto _LL4B;_LL4A: {float*y=((
float*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LL46;}
_LL4B: if((int)_tmp147 != 1)goto _LL4D;_LL4C: {double*y=((double*(*)(void*x))
unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(fp);goto _LL46;}_LL4D: if(_tmp147
<= (void*)2?1:*((int*)_tmp147)!= 1)goto _LL4F;_tmp149=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp147)->f1;_tmp14A=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp147)->f2;
_LL4E: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp14A);struct
_tagged_arr buf=({unsigned int _tmp15B=_tmp149 * typ_szb;char*_tmp15C=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp15B + 1));struct _tagged_arr _tmp15E=
_tag_arr(_tmp15C,sizeof(char),_tmp15B + 1);{unsigned int _tmp15D=_tmp15B;
unsigned int i;for(i=0;i < _tmp15D;i ++){_tmp15C[i]='\000';}_tmp15C[_tmp15D]=(char)
0;}_tmp15E;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(
result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int
i=0;for(0;i < _tmp149;i ++){env=Cyc_Marshal_rread_type_base(r,_tmp14A,env,fp,((
void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((unsigned int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}
_LL4F: if(_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 2)goto _LL51;_tmp14B=(void*)((
struct Cyc_Typerep_FatPtr_struct*)_tmp147)->f1;_LL50: {int mem=(int)Cyc_Marshal_cycgetvarint(
fp);int pos=(int)Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(
fp);int typ_szb=(int)Cyc_Typerep_size_type(_tmp14B);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){struct _tagged_arr buf=({unsigned int _tmp15F=(unsigned int)(
sz * typ_szb);char*_tmp160=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp15F + 1));struct _tagged_arr _tmp162=_tag_arr(_tmp160,sizeof(char),_tmp15F + 1);{
unsigned int _tmp161=_tmp15F;unsigned int i;for(i=0;i < _tmp161;i ++){_tmp160[i]='\000';}
_tmp160[_tmp161]=(char)0;}_tmp162;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));struct _tuple9*x=((struct _tuple9*(*)(
void*x))unsafe_cast)(result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=
bufptr + sz * typ_szb;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;
int i=0;for(0;i < sz;i ++){env=Cyc_Marshal_rread_type_base(r,_tmp14B,env,fp,((void*(*)(
int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct _tuple9*x=((struct _tuple9*(*)(
void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*
x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL46;}_LL51: if(_tmp147 <= (
void*)2?1:*((int*)_tmp147)!= 3)goto _LL53;_tmp14C=((struct Cyc_Typerep_Struct_struct*)
_tmp147)->f3;_LL52: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp14C,sizeof(struct _tuple0*));i ++){unsigned int
_tmp164;void*_tmp165;struct _tuple0 _tmp163=*((struct _tuple0**)_tmp14C.curr)[i];
_tmp164=_tmp163.f1;_tmp165=_tmp163.f3;{int x_ofs=(int)(x + _tmp164);env=Cyc_Marshal_rread_type_base(
r,_tmp165,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL53: if(
_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 4)goto _LL55;_tmp14D=((struct Cyc_Typerep_Tuple_struct*)
_tmp147)->f2;_LL54: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp14D,sizeof(struct _tuple1*));i ++){unsigned int
_tmp167;void*_tmp168;struct _tuple1 _tmp166=*((struct _tuple1**)_tmp14D.curr)[i];
_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{int x_ofs=(int)(x + _tmp167);env=Cyc_Marshal_rread_type_base(
r,_tmp168,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL55: if(
_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 6)goto _LL57;_tmp14E=((struct Cyc_Typerep_TUnionField_struct*)
_tmp147)->f4;_LL56: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp14E,sizeof(struct _tuple1*));i ++){unsigned int
_tmp16A;void*_tmp16B;struct _tuple1 _tmp169=*((struct _tuple1**)_tmp14E.curr)[i];
_tmp16A=_tmp169.f1;_tmp16B=_tmp169.f2;{int x_ofs=(int)(x + _tmp16A);env=Cyc_Marshal_rread_type_base(
r,_tmp16B,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL57: if(
_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 5)goto _LL59;_tmp14F=((struct Cyc_Typerep_TUnion_struct*)
_tmp147)->f2;_tmp150=((struct Cyc_Typerep_TUnion_struct*)_tmp147)->f3;_LL58: {int
tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){int tag=tagmem;
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)
tag;}else{int mem=tagmem - 1024;if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){
int tag=(int)Cyc_Marshal_cycgetvarint(fp);int _tmp16D;void*_tmp16E;struct _tuple4
_tmp16C=Cyc_Typerep_get_unionbranch((unsigned int)tag,_tmp150);_tmp16D=_tmp16C.f1;
_tmp16E=_tmp16C.f2;{struct _tagged_arr buf=({unsigned int _tmp16F=_tmp16D + Cyc_Typerep_size_type(
_tmp16E);char*_tmp170=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp16F + 
1));struct _tagged_arr _tmp172=_tag_arr(_tmp170,sizeof(char),_tmp16F + 1);{
unsigned int _tmp171=_tmp16F;unsigned int i;for(i=0;i < _tmp171;i ++){_tmp170[i]='\000';}
_tmp170[_tmp171]=(char)0;}_tmp172;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,
bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)tag;env=Cyc_Marshal_rread_type_base(r,_tmp16E,env,
fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp16D));}}}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}}goto _LL46;}_LL59: if(_tmp147 <= (void*)2?1:*((int*)_tmp147)!= 7)goto
_LL5B;_tmp151=((struct Cyc_Typerep_XTUnion_struct*)_tmp147)->f2;_LL5A: {int mem=(
int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(env,(unsigned int)
mem)){char nonnull=(char)Cyc_Marshal_cycgetchar(fp);struct _tagged_arr s=({char*
_tmp17D=({unsigned int _tmp17A=(unsigned int)100;char*_tmp17B=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp17A + 1));{unsigned int _tmp17C=
_tmp17A;unsigned int i;for(i=0;i < _tmp17C;i ++){_tmp17B[i]='\000';}_tmp17B[_tmp17C]=(
char)0;}_tmp17B;});_tag_arr(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,(
unsigned int)100 + 1));});struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(char),
4);Cyc_Marshal_fscanntstring(fp,xtname);if((int)nonnull){int _tmp174;void*_tmp175;
struct _tuple4 _tmp173=Cyc_Typerep_get_xtunionbranch((struct _tagged_arr)xtname,
_tmp151);_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;{struct _tagged_arr buf=({
unsigned int _tmp176=_tmp174 + Cyc_Typerep_size_type(_tmp175);char*_tmp177=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp176 + 1));struct _tagged_arr _tmp179=
_tag_arr(_tmp177,sizeof(char),_tmp176 + 1);{unsigned int _tmp178=_tmp176;
unsigned int i;for(i=0;i < _tmp178;i ++){_tmp177[i]='\000';}_tmp177[_tmp178]=(char)
0;}_tmp179;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*
x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(unsigned int)((
char*)_check_null(_untag_arr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_rread_type_base(
r,_tmp175,env,fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp174));}}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)((char*)_check_null(_untag_arr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(
env,*x);}}else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}_LL5B: if(_tmp147 <= (void*)2?1:*((
int*)_tmp147)!= 8)goto _LL5D;_tmp152=((struct Cyc_Typerep_Union_struct*)_tmp147)->f2;
_LL5C: env=Cyc_Marshal_rread_type_base(r,(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E[0]=({struct Cyc_Typerep_Tuple_struct
_tmp17F;_tmp17F.tag=4;_tmp17F.f1=(unsigned int)_tmp152;_tmp17F.f2=({unsigned int
_tmp180=(unsigned int)_tmp152;struct _tuple1**_tmp181=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp180));struct _tagged_arr _tmp186=_tag_arr(
_tmp181,sizeof(struct _tuple1*),_tmp180);{unsigned int _tmp182=_tmp180;
unsigned int i;for(i=0;i < _tmp182;i ++){_tmp181[i]=({struct _tuple1*_tmp183=
_cycalloc(sizeof(*_tmp183));_tmp183->f1=i;_tmp183->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp184=_cycalloc_atomic(sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Typerep_Int_struct
_tmp185;_tmp185.tag=0;_tmp185.f1=0;_tmp185.f2=8;_tmp185;});_tmp184;});_tmp183;});}}
_tmp186;});_tmp17F;});_tmp17E;}),env,fp,result);goto _LL46;_LL5D: if(_tmp147 <= (
void*)2?1:*((int*)_tmp147)!= 9)goto _LL46;_tmp153=((struct Cyc_Typerep_Enum_struct*)
_tmp147)->f2;_LL5E: env=Cyc_Marshal_rread_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp187=_cycalloc_atomic(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Typerep_Int_struct
_tmp188;_tmp188.tag=0;_tmp188.f1=0;_tmp188.f2=(unsigned int)(_tmp153 << 3);
_tmp188;});_tmp187;}),env,fp,result);_LL46:;}return env;}void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*ts,struct Cyc___cycFILE*fp){struct _handler_cons _tmp189;
_push_handler(& _tmp189);{int _tmp18B=0;if(setjmp(_tmp189.handler))_tmp18B=1;if(!
_tmp18B){{struct _tagged_arr buf=({unsigned int _tmp18D=Cyc_Typerep_size_type(ts);
char*_tmp18E=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp18D + 1));
struct _tagged_arr _tmp190=_tag_arr(_tmp18E,sizeof(char),_tmp18D + 1);{unsigned int
_tmp18F=_tmp18D;unsigned int i;for(i=0;i < _tmp18F;i ++){_tmp18E[i]='\000';}_tmp18E[
_tmp18F]=(char)0;}_tmp190;});void*result=((void*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rread_type_base(r,ts,
Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp18C=result;_npop_handler(0);
return _tmp18C;}};_pop_handler();}else{void*_tmp18A=(void*)_exn_thrown;void*
_tmp192=_tmp18A;struct _tagged_arr _tmp193;_LL65: if(*((void**)_tmp192)!= Cyc_Core_Failure)
goto _LL67;_tmp193=((struct Cyc_Core_Failure_struct*)_tmp192)->f1;_LL66:({struct
Cyc_String_pa_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _tagged_arr)((struct
_tagged_arr)_tmp193);{void*_tmp194[1]={& _tmp196};Cyc_fprintf(Cyc_stderr,({const
char*_tmp195="Core::Failure(%s)\n";_tag_arr(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,19));}),_tag_arr(_tmp194,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp197=_cycalloc(sizeof(*_tmp197));
_tmp197[0]=({struct Cyc_Core_Failure_struct _tmp198;_tmp198.tag=Cyc_Core_Failure;
_tmp198.f1=_tmp193;_tmp198;});_tmp197;}));_LL67:;_LL68:(void)_throw(_tmp192);
_LL64:;}}}void*Cyc_Marshal_read_type(void*ts,struct Cyc___cycFILE*fp){return Cyc_Marshal_rread_type(
Cyc_Core_heap_region,ts,fp);}static struct Cyc_Dict_Dict*Cyc_Marshal_empty_addr_map(){
return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,unsigned int k,unsigned int
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(unsigned int,
unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);}struct Cyc_Dict_Dict*
Cyc_Marshal_rcopy_type_base(struct _RegionHandle*r,void*rep,struct Cyc_Dict_Dict*
env,void*val,void*result){{void*_tmp199=rep;unsigned int _tmp19A;unsigned int
_tmp19B;unsigned int _tmp19C;unsigned int _tmp19D;unsigned int _tmp19E;void*_tmp19F;
void*_tmp1A0;struct _tagged_arr _tmp1A1;struct _tagged_arr _tmp1A2;struct _tagged_arr
_tmp1A3;struct _tagged_arr _tmp1A4;struct _tagged_arr _tmp1A5;struct _tagged_arr
_tmp1A6;int _tmp1A7;int _tmp1A8;_LL6A: if(_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 0)
goto _LL6C;_tmp19A=((struct Cyc_Typerep_Int_struct*)_tmp199)->f2;if(_tmp19A != 8)
goto _LL6C;_LL6B: {char*val0=((char*(*)(void*x))unsafe_cast)(val);char*result0=((
char*(*)(void*x))unsafe_cast)(result);({struct _tagged_arr _tmp1AA=({char*_tmp1A9=
result0;_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,1));});char
_tmp1AB=*((char*)_check_unknown_subscript(_tmp1AA,sizeof(char),0));char _tmp1AC=*
val0;if(_get_arr_size(_tmp1AA,sizeof(char))== 1?_tmp1AB == '\000'?_tmp1AC != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp1AA.curr)=_tmp1AC;});goto _LL69;}_LL6C: if(
_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 0)goto _LL6E;_tmp19B=((struct Cyc_Typerep_Int_struct*)
_tmp199)->f2;if(_tmp19B != 16)goto _LL6E;_LL6D: {short*val0=((short*(*)(void*x))
unsafe_cast)(val);short*result0=((short*(*)(void*x))unsafe_cast)(result);*
result0=*val0;goto _LL69;}_LL6E: if(_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 0)goto
_LL70;_tmp19C=((struct Cyc_Typerep_Int_struct*)_tmp199)->f2;if(_tmp19C != 32)goto
_LL70;_LL6F: {int*val0=((int*(*)(void*x))unsafe_cast)(val);int*result0=((int*(*)(
void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL70: if(_tmp199 <= (void*)
2?1:*((int*)_tmp199)!= 0)goto _LL72;_tmp19D=((struct Cyc_Typerep_Int_struct*)
_tmp199)->f2;if(_tmp19D != 64)goto _LL72;_LL71: {long long*val0=((long long*(*)(
void*x))unsafe_cast)(val);long long*result0=((long long*(*)(void*x))unsafe_cast)(
result);*result0=*val0;goto _LL69;}_LL72: if((int)_tmp199 != 0)goto _LL74;_LL73: {
float*val0=((float*(*)(void*x))unsafe_cast)(val);float*result0=((float*(*)(void*
x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL74: if((int)_tmp199 != 1)goto
_LL76;_LL75: {double*val0=((double*(*)(void*x))unsafe_cast)(val);double*result0=((
double*(*)(void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL76: if(
_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 1)goto _LL78;_tmp19E=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp199)->f1;_tmp19F=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp199)->f2;
_LL77: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);int addr=(
int)*val0;if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,(
unsigned int)addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int
k))Cyc_Dict_lookup)(env,(unsigned int)addr);else{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp19F);struct _tagged_arr buf=({unsigned int _tmp1AD=_tmp19E * typ_szb;char*
_tmp1AE=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp1AD + 1));struct
_tagged_arr _tmp1B0=_tag_arr(_tmp1AE,sizeof(char),_tmp1AD + 1);{unsigned int
_tmp1AF=_tmp1AD;unsigned int i;for(i=0;i < _tmp1AF;i ++){_tmp1AE[i]='\000';}_tmp1AE[
_tmp1AF]=(char)0;}_tmp1B0;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));*result0=bufptr;env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,(
unsigned int)addr,bufptr);{int x=addr;int y=(int)bufptr;int i=0;for(0;i < _tmp19E;i
++){env=Cyc_Marshal_rcopy_type_base(r,_tmp19F,env,((void*(*)(int x))unsafe_cast)(
x),((void*(*)(int x))unsafe_cast)(y));x +=typ_szb;y +=typ_szb;}}}goto _LL69;}_LL78:
if(_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 2)goto _LL7A;_tmp1A0=(void*)((struct
Cyc_Typerep_FatPtr_struct*)_tmp199)->f1;_LL79: {struct _tuple9*val0=((struct
_tuple9*(*)(void*x))unsafe_cast)(val);struct _tuple9*result0=((struct _tuple9*(*)(
void*x))unsafe_cast)(result);unsigned int _tmp1B2;unsigned int _tmp1B3;
unsigned int _tmp1B4;struct _tuple9 _tmp1B1=*val0;_tmp1B2=_tmp1B1.f1;_tmp1B3=
_tmp1B1.f2;_tmp1B4=_tmp1B1.f3;{int typ_szb=(int)Cyc_Typerep_size_type(_tmp1A0);
int size=(int)((_tmp1B4 - _tmp1B2)/ typ_szb);int pos=(int)((_tmp1B3 - _tmp1B2)/ 
typ_szb);if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,
_tmp1B2)){unsigned int _tmp1B5=((unsigned int(*)(struct Cyc_Dict_Dict*d,
unsigned int k))Cyc_Dict_lookup)(env,_tmp1B2);*result0=({struct _tuple9 _tmp1B6;
_tmp1B6.f1=_tmp1B5;_tmp1B6.f2=_tmp1B5 + pos * typ_szb;_tmp1B6.f3=_tmp1B5 + size * 
typ_szb;_tmp1B6;});}else{struct _tagged_arr buf=({unsigned int _tmp1B8=_tmp1B4 - 
_tmp1B2;char*_tmp1B9=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp1B8 + 
1));struct _tagged_arr _tmp1BB=_tag_arr(_tmp1B9,sizeof(char),_tmp1B8 + 1);{
unsigned int _tmp1BA=_tmp1B8;unsigned int i;for(i=0;i < _tmp1BA;i ++){_tmp1B9[i]='\000';}
_tmp1B9[_tmp1BA]=(char)0;}_tmp1BB;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));*result0=({struct _tuple9 _tmp1B7;
_tmp1B7.f1=bufptr;_tmp1B7.f2=bufptr + pos * typ_szb;_tmp1B7.f3=bufptr + size * 
typ_szb;_tmp1B7;});env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,_tmp1B2,bufptr);{int y=(int)
_tmp1B2;int z=(int)bufptr;int i=0;for(0;i < size;i ++){env=Cyc_Marshal_rcopy_type_base(
r,_tmp1A0,env,((void*(*)(int x))unsafe_cast)(y),((void*(*)(int x))unsafe_cast)(z));
y +=typ_szb;z +=typ_szb;}}}goto _LL69;}}_LL7A: if(_tmp199 <= (void*)2?1:*((int*)
_tmp199)!= 3)goto _LL7C;_tmp1A1=((struct Cyc_Typerep_Struct_struct*)_tmp199)->f3;
_LL7B: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp1A1,sizeof(struct _tuple0*));
i ++){unsigned int _tmp1BD;void*_tmp1BE;struct _tuple0 _tmp1BC=*((struct _tuple0**)
_tmp1A1.curr)[i];_tmp1BD=_tmp1BC.f1;_tmp1BE=_tmp1BC.f3;{int x_ofs=(int)(x + 
_tmp1BD);int y_ofs=(int)(y + _tmp1BD);env=Cyc_Marshal_rcopy_type_base(r,_tmp1BE,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}
goto _LL69;}_LL7C: if(_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 4)goto _LL7E;_tmp1A2=((
struct Cyc_Typerep_Tuple_struct*)_tmp199)->f2;_LL7D: {int x=((int(*)(void*x))
unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmp1A2,sizeof(struct _tuple1*));i ++){unsigned int _tmp1C0;void*
_tmp1C1;struct _tuple1 _tmp1BF=*((struct _tuple1**)_tmp1A2.curr)[i];_tmp1C0=_tmp1BF.f1;
_tmp1C1=_tmp1BF.f2;{int x_ofs=(int)(x + _tmp1C0);int y_ofs=(int)(y + _tmp1C0);env=
Cyc_Marshal_rcopy_type_base(r,_tmp1C1,env,((void*(*)(int x))unsafe_cast)(x_ofs),((
void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7E: if(_tmp199 <= (void*)2?1:*((
int*)_tmp199)!= 6)goto _LL80;_tmp1A3=((struct Cyc_Typerep_TUnionField_struct*)
_tmp199)->f4;_LL7F: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*
x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp1A3,sizeof(struct
_tuple1*));i ++){unsigned int _tmp1C3;void*_tmp1C4;struct _tuple1 _tmp1C2=*((struct
_tuple1**)_tmp1A3.curr)[i];_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;{int x_ofs=(int)(
x + _tmp1C3);int y_ofs=(int)(y + _tmp1C3);env=Cyc_Marshal_rcopy_type_base(r,_tmp1C4,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}
goto _LL69;}_LL80: if(_tmp199 <= (void*)2?1:*((int*)_tmp199)!= 5)goto _LL82;_tmp1A4=((
struct Cyc_Typerep_TUnion_struct*)_tmp199)->f2;_tmp1A5=((struct Cyc_Typerep_TUnion_struct*)
_tmp199)->f3;_LL81: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(
val);unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);if(*
val0 < 1024)*result0=*val0;else{unsigned int addr=*val0;if(((int(*)(struct Cyc_Dict_Dict*
d,unsigned int k))Cyc_Dict_member)(env,addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict*
d,unsigned int k))Cyc_Dict_lookup)(env,addr);else{unsigned int*tufp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*val0);unsigned int tag=*tufp;int
_tmp1C6;void*_tmp1C7;struct _tuple4 _tmp1C5=Cyc_Typerep_get_unionbranch(tag,
_tmp1A5);_tmp1C6=_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;{struct _tagged_arr buf=({
unsigned int _tmp1C8=_tmp1C6 + Cyc_Typerep_size_type(_tmp1C7);char*_tmp1C9=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp1C8 + 1));struct _tagged_arr _tmp1CB=
_tag_arr(_tmp1C9,sizeof(char),_tmp1C8 + 1);{unsigned int _tmp1CA=_tmp1C8;
unsigned int i;for(i=0;i < _tmp1CA;i ++){_tmp1C9[i]='\000';}_tmp1C9[_tmp1CA]=(char)
0;}_tmp1CB;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,bufptr);*result0=bufptr;{
unsigned int*newtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*
newtagp=tag;env=Cyc_Marshal_rcopy_type_base(r,_tmp1C7,env,((void*(*)(
unsigned int x))unsafe_cast)(addr + _tmp1C6),((void*(*)(unsigned int x))unsafe_cast)(
bufptr + _tmp1C6));}}}}goto _LL69;}_LL82: if(_tmp199 <= (void*)2?1:*((int*)_tmp199)
!= 7)goto _LL84;_tmp1A6=((struct Cyc_Typerep_XTUnion_struct*)_tmp199)->f2;_LL83: {
unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int*
result0=((unsigned int*(*)(void*x))unsafe_cast)(result);unsigned int addr=*val0;
if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,addr))*
result0=((unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(
env,addr);else{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*val0);if(*xtstructp != 0){unsigned int xtnamep=*xtstructp;char*
xtnamec=((char*(*)(unsigned int x))unsafe_cast)(xtnamep + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);int _tmp1CD;void*_tmp1CE;
struct _tuple4 _tmp1CC=Cyc_Typerep_get_xtunionbranch(xtname,_tmp1A6);_tmp1CD=
_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;{struct _tagged_arr buf=({unsigned int _tmp1CF=
_tmp1CD + Cyc_Typerep_size_type(_tmp1CE);char*_tmp1D0=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmp1CF + 1));struct _tagged_arr _tmp1D2=_tag_arr(_tmp1D0,
sizeof(char),_tmp1CF + 1);{unsigned int _tmp1D1=_tmp1CF;unsigned int i;for(i=0;i < 
_tmp1D1;i ++){_tmp1D0[i]='\000';}_tmp1D0[_tmp1D1]=(char)0;}_tmp1D2;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1 + 1)));*result0=bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))
unsafe_cast)(bufptr);*tagptr=xtnamep;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*val0 + _tmp1CD);env=Cyc_Marshal_rcopy_type_base(r,_tmp1CE,env,xoff,((
void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp1CD));env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,
bufptr);}}}}else{*result0=*val0;}}goto _LL69;}_LL84: if(_tmp199 <= (void*)2?1:*((
int*)_tmp199)!= 8)goto _LL86;_tmp1A7=((struct Cyc_Typerep_Union_struct*)_tmp199)->f2;
_LL85: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Typerep_Tuple_struct
_tmp1D4;_tmp1D4.tag=4;_tmp1D4.f1=(unsigned int)_tmp1A7;_tmp1D4.f2=({unsigned int
_tmp1D5=(unsigned int)_tmp1A7;struct _tuple1**_tmp1D6=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp1D5));struct _tagged_arr _tmp1DB=_tag_arr(
_tmp1D6,sizeof(struct _tuple1*),_tmp1D5);{unsigned int _tmp1D7=_tmp1D5;
unsigned int i;for(i=0;i < _tmp1D7;i ++){_tmp1D6[i]=({struct _tuple1*_tmp1D8=
_cycalloc(sizeof(*_tmp1D8));_tmp1D8->f1=i;_tmp1D8->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp1D9=_cycalloc_atomic(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Typerep_Int_struct
_tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=0;_tmp1DA.f2=8;_tmp1DA;});_tmp1D9;});_tmp1D8;});}}
_tmp1DB;});_tmp1D4;});_tmp1D3;}),env,val,result);goto _LL69;_LL86: if(_tmp199 <= (
void*)2?1:*((int*)_tmp199)!= 9)goto _LL88;_tmp1A8=((struct Cyc_Typerep_Enum_struct*)
_tmp199)->f2;_LL87: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp1DC=_cycalloc_atomic(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Typerep_Int_struct
_tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=0;_tmp1DD.f2=(unsigned int)(_tmp1A8 << 3);
_tmp1DD;});_tmp1DC;}),env,val,result);goto _LL69;_LL88:;_LL89:(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=({
struct Cyc_Core_Failure_struct _tmp1DF;_tmp1DF.tag=Cyc_Core_Failure;_tmp1DF.f1=({
const char*_tmp1E0="Unhandled case in copy_type_base";_tag_arr(_tmp1E0,sizeof(
char),_get_zero_arr_size(_tmp1E0,33));});_tmp1DF;});_tmp1DE;}));_LL69:;}return
env;}void*Cyc_Marshal_rcopy_type(struct _RegionHandle*r,void*ts,void*val){struct
_handler_cons _tmp1E1;_push_handler(& _tmp1E1);{int _tmp1E3=0;if(setjmp(_tmp1E1.handler))
_tmp1E3=1;if(!_tmp1E3){{struct _tagged_arr buf=({unsigned int _tmp1E5=Cyc_Typerep_size_type(
ts);char*_tmp1E6=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp1E5 + 1));
struct _tagged_arr _tmp1E8=_tag_arr(_tmp1E6,sizeof(char),_tmp1E5 + 1);{unsigned int
_tmp1E7=_tmp1E5;unsigned int i;for(i=0;i < _tmp1E7;i ++){_tmp1E6[i]='\000';}_tmp1E6[
_tmp1E7]=(char)0;}_tmp1E8;});void*result=((void*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rcopy_type_base(r,ts,
Cyc_Marshal_empty_addr_map(),val,result);{void*_tmp1E4=result;_npop_handler(0);
return _tmp1E4;}};_pop_handler();}else{void*_tmp1E2=(void*)_exn_thrown;void*
_tmp1EA=_tmp1E2;struct _tagged_arr _tmp1EB;_LL8B: if(*((void**)_tmp1EA)!= Cyc_Core_Failure)
goto _LL8D;_tmp1EB=((struct Cyc_Core_Failure_struct*)_tmp1EA)->f1;_LL8C:({struct
Cyc_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct _tagged_arr)((struct
_tagged_arr)_tmp1EB);{void*_tmp1EC[1]={& _tmp1EE};Cyc_fprintf(Cyc_stderr,({const
char*_tmp1ED="Core::Failure(%s)\n";_tag_arr(_tmp1ED,sizeof(char),
_get_zero_arr_size(_tmp1ED,19));}),_tag_arr(_tmp1EC,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));
_tmp1EF[0]=({struct Cyc_Core_Failure_struct _tmp1F0;_tmp1F0.tag=Cyc_Core_Failure;
_tmp1F0.f1=_tmp1EB;_tmp1F0;});_tmp1EF;}));_LL8D:;_LL8E:(void)_throw(_tmp1EA);
_LL8A:;}}}void*Cyc_Marshal_copy_type(void*ts,void*val){return Cyc_Marshal_rcopy_type(
Cyc_Core_heap_region,ts,val);}void*Cyc_Marshal_order_lex(void*ord1,void*ord2){
if(ord1 != (void*)1)return ord1;else{return ord2;}}struct _tuple10{unsigned int f1;
unsigned int f2;};static int Cyc_Marshal_addrp_cmp(struct _tuple10*addrp1,struct
_tuple10*addrp2){if((*addrp1).f1 < (*addrp2).f1)return 1;else{if((*addrp1).f1 == (*
addrp2).f1)return Cyc_Marshal_uint_cmp((*addrp1).f2,(*addrp2).f2);else{return - 1;}}}
struct Cyc_Dict_Dict*Cyc_Marshal_empty_eq_dict(){return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple10*,struct _tuple10*)))Cyc_Dict_empty)(Cyc_Marshal_addrp_cmp),({
struct _tuple10*_tmp1F1=_cycalloc_atomic(sizeof(*_tmp1F1));_tmp1F1->f1=0;_tmp1F1->f2=
0;_tmp1F1;}),(void*)1);}struct _tuple11{void*f1;struct Cyc_Dict_Dict*f2;};struct
_tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11 ordenv){void*
_tmp1F3;struct Cyc_Dict_Dict*_tmp1F4;struct _tuple11 _tmp1F2=ordenv;_tmp1F3=_tmp1F2.f1;
_tmp1F4=_tmp1F2.f2;_tmp1F4=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple10*k,void*v))Cyc_Dict_insert)(_tmp1F4,pair,_tmp1F3);return({struct
_tuple11 _tmp1F5;_tmp1F5.f1=_tmp1F3;_tmp1F5.f2=_tmp1F4;_tmp1F5;});}struct _tuple11
Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict*env,void*x,void*y){if(x == y)
return({struct _tuple11 _tmp1F6;_tmp1F6.f1=(void*)1;_tmp1F6.f2=env;_tmp1F6;});{
void*_tmp1F7=rep;int _tmp1F8;unsigned int _tmp1F9;int _tmp1FA;unsigned int _tmp1FB;
int _tmp1FC;unsigned int _tmp1FD;int _tmp1FE;unsigned int _tmp1FF;int _tmp200;
unsigned int _tmp201;int _tmp202;unsigned int _tmp203;int _tmp204;unsigned int
_tmp205;int _tmp206;unsigned int _tmp207;unsigned int _tmp208;void*_tmp209;void*
_tmp20A;struct _tagged_arr _tmp20B;struct _tagged_arr _tmp20C;struct _tagged_arr
_tmp20D;struct _tagged_arr _tmp20E;struct _tagged_arr _tmp20F;struct _tagged_arr
_tmp210;int _tmp211;struct _tagged_arr _tmp212;int _tmp213;_LL90: if(_tmp1F7 <= (void*)
2?1:*((int*)_tmp1F7)!= 0)goto _LL92;_tmp1F8=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp1F8 != 0)goto _LL92;_tmp1F9=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp1F9 != 8)goto _LL92;_LL91: {unsigned char*x0=((unsigned char*(*)(
void*x))unsafe_cast)(x);unsigned char*y0=((unsigned char*(*)(void*x))unsafe_cast)(
y);return({struct _tuple11 _tmp214;_tmp214.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)
2:(void*)0);_tmp214.f2=env;_tmp214;});}_LL92: if(_tmp1F7 <= (void*)2?1:*((int*)
_tmp1F7)!= 0)goto _LL94;_tmp1FA=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(
_tmp1FA != 1)goto _LL94;_tmp1FB=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(
_tmp1FB != 8)goto _LL94;_LL93: {signed char*x0=((signed char*(*)(void*x))
unsafe_cast)(x);signed char*y0=((signed char*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp215;_tmp215.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp215.f2=env;_tmp215;});}_LL94: if(_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 0)
goto _LL96;_tmp1FC=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp1FC != 1)
goto _LL96;_tmp1FD=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp1FD != 16)
goto _LL96;_LL95: {short*x0=((short*(*)(void*x))unsafe_cast)(x);short*y0=((short*(*)(
void*x))unsafe_cast)(y);return({struct _tuple11 _tmp216;_tmp216.f1=*x0 == *y0?(void*)
1:(*x0 <= *y0?(void*)2:(void*)0);_tmp216.f2=env;_tmp216;});}_LL96: if(_tmp1F7 <= (
void*)2?1:*((int*)_tmp1F7)!= 0)goto _LL98;_tmp1FE=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp1FE != 0)goto _LL98;_tmp1FF=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp1FF != 16)goto _LL98;_LL97: {unsigned short*x0=((
unsigned short*(*)(void*x))unsafe_cast)(x);unsigned short*y0=((unsigned short*(*)(
void*x))unsafe_cast)(y);return({struct _tuple11 _tmp217;_tmp217.f1=*x0 == *y0?(void*)
1:(*x0 <= *y0?(void*)2:(void*)0);_tmp217.f2=env;_tmp217;});}_LL98: if(_tmp1F7 <= (
void*)2?1:*((int*)_tmp1F7)!= 0)goto _LL9A;_tmp200=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp200 != 1)goto _LL9A;_tmp201=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp201 != 32)goto _LL9A;_LL99: {int*x0=((int*(*)(void*x))
unsafe_cast)(x);int*y0=((int*(*)(void*x))unsafe_cast)(y);return({struct _tuple11
_tmp218;_tmp218.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp218.f2=
env;_tmp218;});}_LL9A: if(_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 0)goto _LL9C;
_tmp202=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp202 != 0)goto _LL9C;
_tmp203=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp203 != 32)goto _LL9C;
_LL9B: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*
y0=((unsigned int*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp219;
_tmp219.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp219.f2=env;
_tmp219;});}_LL9C: if(_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 0)goto _LL9E;_tmp204=((
struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp204 != 1)goto _LL9E;_tmp205=((
struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp205 != 64)goto _LL9E;_LL9D: {
long long*x0=((long long*(*)(void*x))unsafe_cast)(x);long long*y0=((long long*(*)(
void*x))unsafe_cast)(y);return({struct _tuple11 _tmp21A;_tmp21A.f1=*x0 == *y0?(void*)
1:(*x0 <= *y0?(void*)2:(void*)0);_tmp21A.f2=env;_tmp21A;});}_LL9E: if(_tmp1F7 <= (
void*)2?1:*((int*)_tmp1F7)!= 0)goto _LLA0;_tmp206=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp206 != 0)goto _LLA0;_tmp207=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp207 != 64)goto _LLA0;_LL9F: {unsigned long long*x0=((
unsigned long long*(*)(void*x))unsafe_cast)(x);unsigned long long*y0=((
unsigned long long*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp21B;
_tmp21B.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp21B.f2=env;
_tmp21B;});}_LLA0: if((int)_tmp1F7 != 0)goto _LLA2;_LLA1: {float*x0=((float*(*)(
void*x))unsafe_cast)(x);float*y0=((float*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp21C;_tmp21C.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp21C.f2=env;_tmp21C;});}_LLA2: if((int)_tmp1F7 != 1)goto _LLA4;_LLA3: {double*x0=((
double*(*)(void*x))unsafe_cast)(x);double*y0=((double*(*)(void*x))unsafe_cast)(y);
return({struct _tuple11 _tmp21D;_tmp21D.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(
void*)0);_tmp21D.f2=env;_tmp21D;});}_LLA4: if(_tmp1F7 <= (void*)2?1:*((int*)
_tmp1F7)!= 1)goto _LLA6;_tmp208=((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F7)->f1;
_tmp209=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F7)->f2;_LLA5: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);unsigned int addrx=*x0;unsigned int addry=*
y0;struct _tuple10*_tmp21E=({struct _tuple10*_tmp226=_cycalloc_atomic(sizeof(*
_tmp226));_tmp226->f1=addrx;_tmp226->f2=addry;_tmp226;});if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp21E))return({struct _tuple11 _tmp21F;
_tmp21F.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_lookup)(
env,_tmp21E);_tmp21F.f2=env;_tmp21F;});else{if(addrx == 0)return({struct _tuple11
_tmp220;_tmp220.f1=(void*)2;_tmp220.f2=env;_tmp220;});else{if(addry == 0)return({
struct _tuple11 _tmp221;_tmp221.f1=(void*)0;_tmp221.f2=env;_tmp221;});else{env=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(
env,_tmp21E,(void*)1);{int typ_szb=(int)Cyc_Typerep_size_type(_tmp209);int xa=(int)
addrx;int ya=(int)addry;void*ord=(void*)1;{int i=0;for(0;i < _tmp208;i ++){void*
_tmp223;struct Cyc_Dict_Dict*_tmp224;struct _tuple11 _tmp222=Cyc_Marshal_leq_eq_type(
_tmp209,env,((void*(*)(int x))unsafe_cast)(xa),((void*(*)(int x))unsafe_cast)(ya));
_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;ord=Cyc_Marshal_order_lex(ord,_tmp223);env=
_tmp224;xa +=typ_szb;ya +=typ_szb;}}return Cyc_Marshal_return_update(_tmp21E,({
struct _tuple11 _tmp225;_tmp225.f1=ord;_tmp225.f2=env;_tmp225;}));}}}}}_LLA6: if(
_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 2)goto _LLA8;_tmp20A=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp1F7)->f1;_LLA7: {struct _tuple9*x0=((struct _tuple9*(*)(void*x))unsafe_cast)(x);
struct _tuple9*y0=((struct _tuple9*(*)(void*x))unsafe_cast)(y);unsigned int _tmp228;
unsigned int _tmp229;unsigned int _tmp22A;struct _tuple9 _tmp227=*x0;_tmp228=_tmp227.f1;
_tmp229=_tmp227.f2;_tmp22A=_tmp227.f3;{unsigned int _tmp22C;unsigned int _tmp22D;
unsigned int _tmp22E;struct _tuple9 _tmp22B=*y0;_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;
_tmp22E=_tmp22B.f3;{int typ_szb=(int)Cyc_Typerep_size_type(_tmp20A);int xpos=(int)((
_tmp229 - _tmp228)/ typ_szb);int ypos=(int)((_tmp22D - _tmp22C)/ typ_szb);if(xpos > 
ypos)return({struct _tuple11 _tmp22F;_tmp22F.f1=(void*)0;_tmp22F.f2=env;_tmp22F;});{
int xsize=(int)((_tmp22A - _tmp228)/ typ_szb);int ysize=(int)((_tmp22E - _tmp22C)/ 
typ_szb);if(xsize > ysize)return({struct _tuple11 _tmp230;_tmp230.f1=(void*)0;
_tmp230.f2=env;_tmp230;});{struct _tuple10*_tmp231=({struct _tuple10*_tmp239=
_cycalloc_atomic(sizeof(*_tmp239));_tmp239->f1=_tmp228;_tmp239->f2=_tmp22C;
_tmp239;});if(((int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp231))return({struct _tuple11 _tmp232;_tmp232.f1=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp231);_tmp232.f2=env;_tmp232;});
else{if(_tmp228 == 0)return({struct _tuple11 _tmp233;_tmp233.f1=(void*)2;_tmp233.f2=
env;_tmp233;});else{if(_tmp22C == 0)return({struct _tuple11 _tmp234;_tmp234.f1=(
void*)0;_tmp234.f2=env;_tmp234;});else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp231,(void*)1);{int xb=(int)
_tmp228;int yb=(int)_tmp22C;void*ord=(void*)1;{int i=0;for(0;i < xsize;i ++){void*
_tmp236;struct Cyc_Dict_Dict*_tmp237;struct _tuple11 _tmp235=Cyc_Marshal_leq_eq_type(
_tmp20A,env,((void*(*)(int x))unsafe_cast)(xb),((void*(*)(int x))unsafe_cast)(yb));
_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;ord=Cyc_Marshal_order_lex(ord,_tmp236);env=
_tmp237;xb +=typ_szb;yb +=typ_szb;}}if(ord == (void*)1?xsize < ysize: 0)ord=(void*)2;
return Cyc_Marshal_return_update(_tmp231,({struct _tuple11 _tmp238;_tmp238.f1=ord;
_tmp238.f2=env;_tmp238;}));}}}}}}}}}_LLA8: if(_tmp1F7 <= (void*)2?1:*((int*)
_tmp1F7)!= 4)goto _LLAA;_tmp20B=((struct Cyc_Typerep_Tuple_struct*)_tmp1F7)->f2;
_LLA9: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*x))
unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_arr_size(_tmp20B,sizeof(
struct _tuple1*));i ++){unsigned int _tmp23B;void*_tmp23C;struct _tuple1 _tmp23A=*((
struct _tuple1**)_tmp20B.curr)[i];_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;{int x_ofs=(
int)(x0 + _tmp23B);int y_ofs=(int)(y0 + _tmp23B);void*_tmp23E;struct Cyc_Dict_Dict*
_tmp23F;struct _tuple11 _tmp23D=Cyc_Marshal_leq_eq_type(_tmp23C,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp23E=_tmp23D.f1;
_tmp23F=_tmp23D.f2;ord=Cyc_Marshal_order_lex(ord,_tmp23E);env=_tmp23F;}}}return({
struct _tuple11 _tmp240;_tmp240.f1=ord;_tmp240.f2=env;_tmp240;});}_LLAA: if(_tmp1F7
<= (void*)2?1:*((int*)_tmp1F7)!= 3)goto _LLAC;_tmp20C=((struct Cyc_Typerep_Struct_struct*)
_tmp1F7)->f3;_LLAB: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_arr_size(_tmp20C,
sizeof(struct _tuple0*));i ++){unsigned int _tmp242;void*_tmp243;struct _tuple0
_tmp241=*((struct _tuple0**)_tmp20C.curr)[i];_tmp242=_tmp241.f1;_tmp243=_tmp241.f3;{
int x_ofs=(int)(x0 + _tmp242);int y_ofs=(int)(y0 + _tmp242);void*_tmp245;struct Cyc_Dict_Dict*
_tmp246;struct _tuple11 _tmp244=Cyc_Marshal_leq_eq_type(_tmp243,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp245=_tmp244.f1;
_tmp246=_tmp244.f2;ord=Cyc_Marshal_order_lex(ord,_tmp245);env=_tmp246;}}}return({
struct _tuple11 _tmp247;_tmp247.f1=ord;_tmp247.f2=env;_tmp247;});}_LLAC: if(_tmp1F7
<= (void*)2?1:*((int*)_tmp1F7)!= 6)goto _LLAE;_tmp20D=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1F7)->f4;_LLAD: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=1;for(0;i < _get_arr_size(_tmp20D,
sizeof(struct _tuple1*));i ++){unsigned int _tmp249;void*_tmp24A;struct _tuple1
_tmp248=*((struct _tuple1**)_tmp20D.curr)[i];_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{
int x_ofs=(int)(x0 + _tmp249);int y_ofs=(int)(y0 + _tmp249);void*_tmp24C;struct Cyc_Dict_Dict*
_tmp24D;struct _tuple11 _tmp24B=Cyc_Marshal_leq_eq_type(_tmp24A,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp24C=_tmp24B.f1;
_tmp24D=_tmp24B.f2;ord=Cyc_Marshal_order_lex(ord,_tmp24C);env=_tmp24D;}}}return({
struct _tuple11 _tmp24E;_tmp24E.f1=ord;_tmp24E.f2=env;_tmp24E;});}_LLAE: if(_tmp1F7
<= (void*)2?1:*((int*)_tmp1F7)!= 5)goto _LLB0;_tmp20E=((struct Cyc_Typerep_TUnion_struct*)
_tmp1F7)->f2;_tmp20F=((struct Cyc_Typerep_TUnion_struct*)_tmp1F7)->f3;_LLAF: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);if(*x0 < 1024?*y0 < 1024: 0)return({struct
_tuple11 _tmp24F;_tmp24F.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp24F.f2=env;_tmp24F;});else{if(*x0 < 1024)return({struct _tuple11 _tmp250;
_tmp250.f1=(void*)2;_tmp250.f2=env;_tmp250;});else{if(*y0 < 1024)return({struct
_tuple11 _tmp251;_tmp251.f1=(void*)0;_tmp251.f2=env;_tmp251;});else{struct
_tuple10*_tmp252=({struct _tuple10*_tmp258=_cycalloc_atomic(sizeof(*_tmp258));
_tmp258->f1=*x0;_tmp258->f2=*y0;_tmp258;});if(((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple10*k))Cyc_Dict_member)(env,_tmp252))return({struct _tuple11 _tmp253;
_tmp253.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_lookup)(
env,_tmp252);_tmp253.f2=env;_tmp253;});else{env=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp252,(void*)1);{
unsigned int*xtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);
unsigned int*ytagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*y0);if(*xtagp
!= *ytagp)return Cyc_Marshal_return_update(_tmp252,({struct _tuple11 _tmp254;
_tmp254.f1=*xtagp == *ytagp?(void*)1:(*xtagp <= *ytagp?(void*)2:(void*)0);_tmp254.f2=
env;_tmp254;}));else{int _tmp256;void*_tmp257;struct _tuple4 _tmp255=Cyc_Typerep_get_unionbranch(*
xtagp,_tmp20F);_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp256);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp256);return Cyc_Marshal_return_update(_tmp252,Cyc_Marshal_leq_eq_type(
_tmp257,env,xoff,yoff));}}}}}}}}_LLB0: if(_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 
7)goto _LLB2;_tmp210=((struct Cyc_Typerep_XTUnion_struct*)_tmp1F7)->f2;_LLB1: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);struct _tuple10*_tmp259=({struct _tuple10*
_tmp266=_cycalloc_atomic(sizeof(*_tmp266));_tmp266->f1=*x0;_tmp266->f2=*y0;
_tmp266;});if(((int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp259))return({struct _tuple11 _tmp25A;_tmp25A.f1=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp259);_tmp25A.f2=env;_tmp25A;});
else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))
Cyc_Dict_insert)(env,({struct _tuple10*_tmp25B=_cycalloc_atomic(sizeof(*_tmp25B));
_tmp25B->f1=*x0;_tmp25B->f2=*y0;_tmp25B;}),(void*)1);{unsigned int*xp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);unsigned int*yp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*y0);if(*xp == 0?*yp == 0: 0){char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*x0 + 4);char*ynamec=((char*(*)(unsigned int x))
unsafe_cast)(*y0 + 4);struct _tagged_arr xname=(struct _tagged_arr)Cstring_to_string(
xnamec);struct _tagged_arr yname=(struct _tagged_arr)Cstring_to_string(ynamec);int
cmp=Cyc_strcmp((struct _tagged_arr)xname,(struct _tagged_arr)yname);if(cmp < 0)
return Cyc_Marshal_return_update(_tmp259,({struct _tuple11 _tmp25C;_tmp25C.f1=(void*)
0;_tmp25C.f2=env;_tmp25C;}));else{if(cmp == 0)return Cyc_Marshal_return_update(
_tmp259,({struct _tuple11 _tmp25D;_tmp25D.f1=(void*)1;_tmp25D.f2=env;_tmp25D;}));
else{return Cyc_Marshal_return_update(_tmp259,({struct _tuple11 _tmp25E;_tmp25E.f1=(
void*)2;_tmp25E.f2=env;_tmp25E;}));}}}else{if(*xp == 0)return Cyc_Marshal_return_update(
_tmp259,({struct _tuple11 _tmp25F;_tmp25F.f1=(void*)2;_tmp25F.f2=env;_tmp25F;}));
else{if(*yp == 0)return Cyc_Marshal_return_update(_tmp259,({struct _tuple11 _tmp260;
_tmp260.f1=(void*)0;_tmp260.f2=env;_tmp260;}));else{char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*xp + 4);char*ynamec=((char*(*)(unsigned int x))
unsafe_cast)(*yp + 4);struct _tagged_arr xname=(struct _tagged_arr)Cstring_to_string(
xnamec);struct _tagged_arr yname=(struct _tagged_arr)Cstring_to_string(ynamec);int
cmp=Cyc_strcmp((struct _tagged_arr)xname,(struct _tagged_arr)yname);if(cmp < 0)
return Cyc_Marshal_return_update(_tmp259,({struct _tuple11 _tmp261;_tmp261.f1=(void*)
0;_tmp261.f2=env;_tmp261;}));else{if(cmp == 0){int _tmp263;void*_tmp264;struct
_tuple4 _tmp262=Cyc_Typerep_get_xtunionbranch(xname,_tmp210);_tmp263=_tmp262.f1;
_tmp264=_tmp262.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*x0 + 
_tmp263);void*yoff=((void*(*)(unsigned int x))unsafe_cast)(*y0 + _tmp263);return
Cyc_Marshal_return_update(_tmp259,Cyc_Marshal_leq_eq_type(_tmp264,env,xoff,yoff));}}
else{return Cyc_Marshal_return_update(_tmp259,({struct _tuple11 _tmp265;_tmp265.f1=(
void*)2;_tmp265.f2=env;_tmp265;}));}}}}}}}}_LLB2: if(_tmp1F7 <= (void*)2?1:*((int*)
_tmp1F7)!= 8)goto _LLB4;_tmp211=((struct Cyc_Typerep_Union_struct*)_tmp1F7)->f2;
_tmp212=((struct Cyc_Typerep_Union_struct*)_tmp1F7)->f3;_LLB3: return Cyc_Marshal_leq_eq_type((
void*)({struct Cyc_Typerep_Tuple_struct*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_Typerep_Tuple_struct _tmp268;_tmp268.tag=4;_tmp268.f1=(
unsigned int)_tmp211;_tmp268.f2=({unsigned int _tmp269=(unsigned int)_tmp211;
struct _tuple1**_tmp26A=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp269));struct _tagged_arr _tmp26F=_tag_arr(_tmp26A,sizeof(struct
_tuple1*),_tmp269);{unsigned int _tmp26B=_tmp269;unsigned int i;for(i=0;i < _tmp26B;
i ++){_tmp26A[i]=({struct _tuple1*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->f1=
i;_tmp26C->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp26D=_cycalloc_atomic(
sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Typerep_Int_struct _tmp26E;_tmp26E.tag=0;
_tmp26E.f1=0;_tmp26E.f2=8;_tmp26E;});_tmp26D;});_tmp26C;});}}_tmp26F;});_tmp268;});
_tmp267;}),env,x,y);_LLB4: if(_tmp1F7 <= (void*)2?1:*((int*)_tmp1F7)!= 9)goto _LLB6;
_tmp213=((struct Cyc_Typerep_Enum_struct*)_tmp1F7)->f2;_LLB5: return Cyc_Marshal_leq_eq_type((
void*)({struct Cyc_Typerep_Int_struct*_tmp270=_cycalloc_atomic(sizeof(*_tmp270));
_tmp270[0]=({struct Cyc_Typerep_Int_struct _tmp271;_tmp271.tag=0;_tmp271.f1=0;
_tmp271.f2=(unsigned int)(_tmp213 << 3);_tmp271;});_tmp270;}),env,x,y);_LLB6:;
_LLB7:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp272=_cycalloc(
sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Core_Failure_struct _tmp273;_tmp273.tag=
Cyc_Core_Failure;_tmp273.f1=({const char*_tmp274="leq_eq_type case unmatched";
_tag_arr(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,27));});_tmp273;});
_tmp272;}));_LL8F:;}}int Cyc_Marshal_eq_type(void*ts,void*x,void*y){struct
_handler_cons _tmp275;_push_handler(& _tmp275);{int _tmp277=0;if(setjmp(_tmp275.handler))
_tmp277=1;if(!_tmp277){{void*_tmp279;struct _tuple11 _tmp278=Cyc_Marshal_leq_eq_type(
ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp279=_tmp278.f1;{int _tmp27A=_tmp279 == (
void*)1;_npop_handler(0);return _tmp27A;}};_pop_handler();}else{void*_tmp276=(
void*)_exn_thrown;void*_tmp27C=_tmp276;struct _tagged_arr _tmp27D;_LLB9: if(*((void**)
_tmp27C)!= Cyc_Core_Failure)goto _LLBB;_tmp27D=((struct Cyc_Core_Failure_struct*)
_tmp27C)->f1;_LLBA:({struct Cyc_String_pa_struct _tmp280;_tmp280.tag=0;_tmp280.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp27D);{void*_tmp27E[1]={& _tmp280};Cyc_fprintf(
Cyc_stderr,({const char*_tmp27F="Core::Failure(%s)\n";_tag_arr(_tmp27F,sizeof(
char),_get_zero_arr_size(_tmp27F,19));}),_tag_arr(_tmp27E,sizeof(void*),1));}});(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp281=_cycalloc(sizeof(*
_tmp281));_tmp281[0]=({struct Cyc_Core_Failure_struct _tmp282;_tmp282.tag=Cyc_Core_Failure;
_tmp282.f1=_tmp27D;_tmp282;});_tmp281;}));_LLBB:;_LLBC:(void)_throw(_tmp27C);
_LLB8:;}}}int Cyc_Marshal_leq_type(void*ts,void*x,void*y){struct _handler_cons
_tmp283;_push_handler(& _tmp283);{int _tmp285=0;if(setjmp(_tmp283.handler))_tmp285=
1;if(!_tmp285){{void*_tmp287;struct _tuple11 _tmp286=Cyc_Marshal_leq_eq_type(ts,
Cyc_Marshal_empty_eq_dict(),x,y);_tmp287=_tmp286.f1;{int _tmp288=_tmp287 != (void*)
0;_npop_handler(0);return _tmp288;}};_pop_handler();}else{void*_tmp284=(void*)
_exn_thrown;void*_tmp28A=_tmp284;struct _tagged_arr _tmp28B;_LLBE: if(*((void**)
_tmp28A)!= Cyc_Core_Failure)goto _LLC0;_tmp28B=((struct Cyc_Core_Failure_struct*)
_tmp28A)->f1;_LLBF:({struct Cyc_String_pa_struct _tmp28E;_tmp28E.tag=0;_tmp28E.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp28B);{void*_tmp28C[1]={& _tmp28E};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28D="Core::Failure(%s)\n";_tag_arr(_tmp28D,sizeof(
char),_get_zero_arr_size(_tmp28D,19));}),_tag_arr(_tmp28C,sizeof(void*),1));}});(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp28F=_cycalloc(sizeof(*
_tmp28F));_tmp28F[0]=({struct Cyc_Core_Failure_struct _tmp290;_tmp290.tag=Cyc_Core_Failure;
_tmp290.f1=_tmp28B;_tmp290;});_tmp28F;}));_LLC0:;_LLC1:(void)_throw(_tmp28A);
_LLBD:;}}}int Cyc_Marshal_cmp_type(void*ts,void*x,void*y){struct _handler_cons
_tmp291;_push_handler(& _tmp291);{int _tmp293=0;if(setjmp(_tmp291.handler))_tmp293=
1;if(!_tmp293){{void*_tmp295;struct _tuple11 _tmp294=Cyc_Marshal_leq_eq_type(ts,
Cyc_Marshal_empty_eq_dict(),x,y);_tmp295=_tmp294.f1;{void*_tmp296=_tmp295;_LLC3:
if((int)_tmp296 != 0)goto _LLC5;_LLC4: {int _tmp297=- 1;_npop_handler(0);return
_tmp297;}_LLC5: if((int)_tmp296 != 1)goto _LLC7;_LLC6: {int _tmp298=0;_npop_handler(
0);return _tmp298;}_LLC7: if((int)_tmp296 != 2)goto _LLC2;_LLC8: {int _tmp299=1;
_npop_handler(0);return _tmp299;}_LLC2:;}};_pop_handler();}else{void*_tmp292=(
void*)_exn_thrown;void*_tmp29B=_tmp292;struct _tagged_arr _tmp29C;_LLCA: if(*((void**)
_tmp29B)!= Cyc_Core_Failure)goto _LLCC;_tmp29C=((struct Cyc_Core_Failure_struct*)
_tmp29B)->f1;_LLCB:({struct Cyc_String_pa_struct _tmp29F;_tmp29F.tag=0;_tmp29F.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmp29C);{void*_tmp29D[1]={& _tmp29F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp29E="Core::Failure(%s)\n";_tag_arr(_tmp29E,sizeof(
char),_get_zero_arr_size(_tmp29E,19));}),_tag_arr(_tmp29D,sizeof(void*),1));}});(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2A0=_cycalloc(sizeof(*
_tmp2A0));_tmp2A0[0]=({struct Cyc_Core_Failure_struct _tmp2A1;_tmp2A1.tag=Cyc_Core_Failure;
_tmp2A1.f1=_tmp29C;_tmp2A1;});_tmp2A0;}));_LLCC:;_LLCD:(void)_throw(_tmp29B);
_LLC9:;}}}
