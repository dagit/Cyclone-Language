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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_printf(struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;
void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct _tagged_arr Cyc_Bitvec_new_empty(int);struct _tagged_arr Cyc_Bitvec_new_full(
int);struct _tagged_arr Cyc_Bitvec_new_copy(struct _tagged_arr);struct _tagged_arr
Cyc_Bitvec_from_list(struct Cyc_Dict_Dict*d,int(*f)(void*),int sz,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Bitvec_to_sorted_list(struct _tagged_arr bvec,int sz);void
Cyc_Bitvec_print_bvec(struct _tagged_arr bvec);int Cyc_Bitvec_get(struct _tagged_arr,
int);void Cyc_Bitvec_set(struct _tagged_arr,int);void Cyc_Bitvec_clear(struct
_tagged_arr,int);int Cyc_Bitvec_get_and_set(struct _tagged_arr,int);void Cyc_Bitvec_clear_all(
struct _tagged_arr);void Cyc_Bitvec_set_all(struct _tagged_arr);int Cyc_Bitvec_all_set(
struct _tagged_arr bvec,int sz);void Cyc_Bitvec_union_two(struct _tagged_arr dest,
struct _tagged_arr src1,struct _tagged_arr src2);void Cyc_Bitvec_intersect_two(struct
_tagged_arr dest,struct _tagged_arr src1,struct _tagged_arr src2);void Cyc_Bitvec_diff_two(
struct _tagged_arr dest,struct _tagged_arr src1,struct _tagged_arr src2);int Cyc_Bitvec_compare_two(
struct _tagged_arr src1,struct _tagged_arr src2);void*Cyc___assert_fail(struct
_tagged_arr assertion,struct _tagged_arr file,unsigned int line);int Cyc_Bitvec_get(
struct _tagged_arr bvec,int pos){int word=pos >> 5;int offset=pos & 31;return(*((int*)
_check_unknown_subscript(bvec,sizeof(int),word))>> offset & 1)== 1;}void Cyc_Bitvec_set(
struct _tagged_arr bvec,int pos){int word=pos >> 5;int offset=pos & 31;*((int*)
_check_unknown_subscript(bvec,sizeof(int),word))=*((int*)
_check_unknown_subscript(bvec,sizeof(int),word))| 1 << offset;}void Cyc_Bitvec_clear(
struct _tagged_arr bvec,int pos){int word=pos >> 5;int offset=pos & 31;*((int*)
_check_unknown_subscript(bvec,sizeof(int),word))=*((int*)
_check_unknown_subscript(bvec,sizeof(int),word))& ~(1 << offset);}int Cyc_Bitvec_get_and_set(
struct _tagged_arr bvec,int pos){int word=pos >> 5;int offset=pos & 31;int slot=*((int*)
_check_unknown_subscript(bvec,sizeof(int),word));int ans=(slot >> offset & 1)== 1;
if(!ans)((int*)bvec.curr)[word]=slot | 1 << offset;return ans;}void Cyc_Bitvec_union_two(
struct _tagged_arr dest,struct _tagged_arr src1,struct _tagged_arr src2){unsigned int
len=_get_arr_size(dest,sizeof(int));(len <= _get_arr_size(src1,sizeof(int))?len <= 
_get_arr_size(src2,sizeof(int)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp0="len <= src1.size && len <= src2.size";
_tag_arr(_tmp0,sizeof(char),_get_zero_arr_size(_tmp0,37));}),({const char*_tmp1="bitvec.cyc";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,11));}),71);{int i=0;for(0;i
< len;++ i){((int*)dest.curr)[i]=((int*)src1.curr)[i]| ((int*)src2.curr)[i];}}}
void Cyc_Bitvec_intersect_two(struct _tagged_arr dest,struct _tagged_arr src1,struct
_tagged_arr src2){unsigned int len=_get_arr_size(dest,sizeof(int));(len <= 
_get_arr_size(src1,sizeof(int))?len <= _get_arr_size(src2,sizeof(int)): 0)?0:((int(*)(
struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp2="len <= src1.size && len <= src2.size";_tag_arr(_tmp2,sizeof(
char),_get_zero_arr_size(_tmp2,37));}),({const char*_tmp3="bitvec.cyc";_tag_arr(
_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,11));}),78);{int i=0;for(0;i < len;++ i){((
int*)dest.curr)[i]=((int*)src1.curr)[i]& ((int*)src2.curr)[i];}}}void Cyc_Bitvec_diff_two(
struct _tagged_arr dest,struct _tagged_arr src1,struct _tagged_arr src2){unsigned int
len=_get_arr_size(dest,sizeof(int));(len <= _get_arr_size(src1,sizeof(int))?len <= 
_get_arr_size(src2,sizeof(int)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp4="len <= src1.size && len <= src2.size";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,37));}),({const char*_tmp5="bitvec.cyc";
_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,11));}),85);{int i=0;for(0;i
< len;++ i){((int*)dest.curr)[i]=((int*)src1.curr)[i]& ~((int*)src2.curr)[i];}}}
int Cyc_Bitvec_compare_two(struct _tagged_arr src1,struct _tagged_arr src2){
unsigned int len=_get_arr_size(src1,sizeof(int));len <= _get_arr_size(src2,sizeof(
int))?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int
line))Cyc___assert_fail)(({const char*_tmp6="len <= src2.size";_tag_arr(_tmp6,
sizeof(char),_get_zero_arr_size(_tmp6,17));}),({const char*_tmp7="bitvec.cyc";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,11));}),92);{int i=0;for(0;i
< len;++ i){if(((int*)src1.curr)[i]!= ((int*)src2.curr)[i])return 0;}}return 1;}
struct _tagged_arr Cyc_Bitvec_new_empty(int sz){struct _tagged_arr ans=({unsigned int
_tmp8=(unsigned int)(sz / 32 + 1);int*_tmp9=(int*)_cycalloc_atomic(_check_times(
sizeof(int),_tmp8));struct _tagged_arr _tmpB=_tag_arr(_tmp9,sizeof(int),_tmp8);{
unsigned int _tmpA=_tmp8;unsigned int i;for(i=0;i < _tmpA;i ++){_tmp9[i]=0;}}_tmpB;});
return ans;}struct _tagged_arr Cyc_Bitvec_new_full(int sz){struct _tagged_arr ans=({
unsigned int _tmpC=(unsigned int)(sz / 32 + 1);int*_tmpD=(int*)_cycalloc_atomic(
_check_times(sizeof(int),_tmpC));struct _tagged_arr _tmpF=_tag_arr(_tmpD,sizeof(
int),_tmpC);{unsigned int _tmpE=_tmpC;unsigned int i;for(i=0;i < _tmpE;i ++){_tmpD[i]=
-1;}}_tmpF;});return ans;}struct _tagged_arr Cyc_Bitvec_new_copy(struct _tagged_arr
old){struct _tagged_arr copy=Cyc_Bitvec_new_empty((int)_get_arr_size(old,sizeof(
int)));Cyc_Bitvec_union_two(copy,copy,old);return copy;}struct _tagged_arr Cyc_Bitvec_from_list(
struct Cyc_Dict_Dict*d,int(*f)(void*),int sz,struct Cyc_List_List*l){struct
_tagged_arr ans=({unsigned int _tmp10=(unsigned int)(sz % 32 + 1);int*_tmp11=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp10));struct _tagged_arr _tmp13=
_tag_arr(_tmp11,sizeof(int),_tmp10);{unsigned int _tmp12=_tmp10;unsigned int i;
for(i=0;i < _tmp12;i ++){_tmp11[i]=0;}}_tmp13;});for(0;l != 0;l=l->tl){Cyc_Bitvec_set(
ans,f(Cyc_Dict_lookup(d,(void*)l->hd)));}return ans;}struct Cyc_List_List*Cyc_Bitvec_to_sorted_list(
struct _tagged_arr bvec,int sz){struct Cyc_List_List*ans=0;{int pos=sz - 1;for(0;pos >= 
0;0){int word=pos >> 5;int bits=*((int*)_check_unknown_subscript(bvec,sizeof(int),
word));int offset=pos & 31;for(0;offset >= 0;(-- offset,-- pos)){if((bits >> offset & 1)
== 1)ans=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(struct Cyc_List_List)* 1);
_tmp14[0]=({struct Cyc_List_List _tmp15;_tmp15.hd=(void*)pos;_tmp15.tl=ans;_tmp15;});
_tmp14;});}}}return ans;}void Cyc_Bitvec_clear_all(struct _tagged_arr bvec){
unsigned int len=_get_arr_size(bvec,sizeof(int));int i=0;for(0;i < len;++ i){((int*)
bvec.curr)[i]=0;}}void Cyc_Bitvec_set_all(struct _tagged_arr bvec){unsigned int len=
_get_arr_size(bvec,sizeof(int));int i=0;for(0;i < len;++ i){((int*)bvec.curr)[i]=-1;}}
int Cyc_Bitvec_all_set(struct _tagged_arr bvec,int sz){int words=sz >> 5;words < 
_get_arr_size(bvec,sizeof(int))?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp16="words < bvec.size";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,18));}),({const char*
_tmp17="bitvec.cyc";_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,11));}),
149);{int i=0;for(0;i < words;i ++){if(*((int*)_check_unknown_subscript(bvec,
sizeof(int),i))!= -1)return 0;}}{int i=words * 32;for(0;i < sz;i ++){if(!Cyc_Bitvec_get(
bvec,i))return 0;}}return 1;}void Cyc_Bitvec_print_bvec(struct _tagged_arr bvec){{int
i=0;for(0;i < 32 * _get_arr_size(bvec,sizeof(int));++ i){({struct Cyc_Int_pa_struct
_tmp1A;_tmp1A.tag=1;_tmp1A.f1=(unsigned int)(Cyc_Bitvec_get(bvec,i)?1: 0);{void*
_tmp18[1]={& _tmp1A};Cyc_printf(({const char*_tmp19="%d";_tag_arr(_tmp19,sizeof(
char),_get_zero_arr_size(_tmp19,3));}),_tag_arr(_tmp18,sizeof(void*),1));}});}}({
void*_tmp1B[0]={};Cyc_printf(({const char*_tmp1C="\n";_tag_arr(_tmp1C,sizeof(char),
_get_zero_arr_size(_tmp1C,2));}),_tag_arr(_tmp1B,sizeof(void*),0));});}
