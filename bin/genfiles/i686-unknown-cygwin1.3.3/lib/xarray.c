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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Xarray_Xarray{
struct _tagged_arr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,
int,void*);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty();struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int,void*);
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int Cyc_Xarray_add_ind(struct
Cyc_Xarray_Xarray*,void*);struct _tagged_arr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*);
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _tagged_arr arr);struct Cyc_Xarray_Xarray*
Cyc_Xarray_append(struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_app(
void*(*f)(void*),struct Cyc_Xarray_Xarray*);void Cyc_Xarray_app_c(void*(*f)(void*,
void*),void*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_iter(void(*f)(void*),
struct Cyc_Xarray_Xarray*);void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*,
struct Cyc_Xarray_Xarray*);struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),
struct Cyc_Xarray_Xarray*);struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(
void*,void*),void*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*
xarr);void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num);void Cyc_Xarray_remove(
struct Cyc_Xarray_Xarray*xarr,int i);int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*
xarr){return xarr->num_elmts;}void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,
int i){if(i < 0?1: i >= xarr->num_elmts)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp1;_tmp1.tag=Cyc_Core_Invalid_argument;_tmp1.f1=({const char*_tmp2="Xarray::get: bad index";
_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,23));});_tmp1;});_tmp0;}));
return*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),i));}void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*xarr,int i,void*a){if(i < 0?1: i >= xarr->num_elmts)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp3=_cycalloc(sizeof(*
_tmp3));_tmp3[0]=({struct Cyc_Core_Invalid_argument_struct _tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;
_tmp4.f1=({const char*_tmp5="Xarray::set: bad index";_tag_arr(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,23));});_tmp4;});_tmp3;}));*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i))=a;}struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty(){struct _tagged_arr x=_tag_arr(({void**_tmp7=_cycalloc(
sizeof(void*)* 0);_tmp7;}),sizeof(void*),0);return({struct Cyc_Xarray_Xarray*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->elmts=x;_tmp6->num_elmts=0;_tmp6;});}
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){if(len < 0)(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp8=_cycalloc(sizeof(*_tmp8));
_tmp8[0]=({struct Cyc_Core_Invalid_argument_struct _tmp9;_tmp9.tag=Cyc_Core_Invalid_argument;
_tmp9.f1=({const char*_tmpA="xarrays must have a non-negative size buffer";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,45));});_tmp9;});_tmp8;}));
return({struct Cyc_Xarray_Xarray*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->elmts=({
unsigned int _tmpC=(unsigned int)len;void**_tmpD=(void**)_cycalloc(_check_times(
sizeof(void*),_tmpC));struct _tagged_arr _tmpF=_tag_arr(_tmpD,sizeof(void*),_tmpC);{
unsigned int _tmpE=_tmpC;unsigned int i;for(i=0;i < _tmpE;i ++){_tmpD[i]=a;}}_tmpF;});
_tmpB->num_elmts=0;_tmpB;});}struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,
void*a){if(len < 1)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp11;_tmp11.tag=Cyc_Core_Invalid_argument;_tmp11.f1=({const char*_tmp12="singleton xarray must have size >=1";
_tag_arr(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,36));});_tmp11;});_tmp10;}));{
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_create(len,a);x->num_elmts=1;return x;}}void
Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){if(xarr->num_elmts == 
_get_arr_size(xarr->elmts,sizeof(void*))){if(xarr->num_elmts == 0)xarr->elmts=
_tag_arr(({unsigned int _tmp13=(unsigned int)10;void**_tmp14=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp13));{unsigned int _tmp15=_tmp13;unsigned int i;
for(i=0;i < _tmp15;i ++){_tmp14[i]=a;}}_tmp14;}),sizeof(void*),(unsigned int)10);
else{struct _tagged_arr newarr=({unsigned int _tmp16=(unsigned int)(xarr->num_elmts
* 2);void**_tmp17=(void**)_cycalloc(_check_times(sizeof(void*),_tmp16));struct
_tagged_arr _tmp19=_tag_arr(_tmp17,sizeof(void*),_tmp16);{unsigned int _tmp18=
_tmp16;unsigned int i;for(i=0;i < _tmp18;i ++){_tmp17[i]=*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),0));}}_tmp19;});{int i=1;for(0;
i < xarr->num_elmts;++ i){*((void**)_check_unknown_subscript(newarr,sizeof(void*),
i))=*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),i));}}xarr->elmts=
newarr;}}*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts
++))=a;}int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){Cyc_Xarray_add(
xarr,a);return xarr->num_elmts - 1;}struct _tagged_arr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*
xarr){if(xarr->num_elmts == 0)return _tag_arr(({void**_tmp1A=_cycalloc(sizeof(void*)
* 0);_tmp1A;}),sizeof(void*),0);{struct _tagged_arr ans=({unsigned int _tmp1B=(
unsigned int)xarr->num_elmts;void**_tmp1C=(void**)_cycalloc(_check_times(sizeof(
void*),_tmp1B));struct _tagged_arr _tmp1E=_tag_arr(_tmp1C,sizeof(void*),_tmp1B);{
unsigned int _tmp1D=_tmp1B;unsigned int i;for(i=0;i < _tmp1D;i ++){_tmp1C[i]=*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),(int)i));}}_tmp1E;});return
ans;}}struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _tagged_arr arr){if(
_get_arr_size(arr,sizeof(void*))== 0)return Cyc_Xarray_create_empty();{struct Cyc_Xarray_Xarray*
ans=({struct Cyc_Xarray_Xarray*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->elmts=({
unsigned int _tmp20=_get_arr_size(arr,sizeof(void*));void**_tmp21=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp20));struct _tagged_arr _tmp23=_tag_arr(
_tmp21,sizeof(void*),_tmp20);{unsigned int _tmp22=_tmp20;unsigned int i;for(i=0;i < 
_tmp22;i ++){_tmp21[i]=((void**)arr.curr)[(int)i];}}_tmp23;});_tmp1F->num_elmts=(
int)_get_arr_size(arr,sizeof(void*));_tmp1F;});return ans;}}struct Cyc_Xarray_Xarray*
Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
int newsz=(int)(_get_arr_size(xarr1->elmts,sizeof(void*))+ _get_arr_size(xarr2->elmts,
sizeof(void*)));if(newsz == 0)return Cyc_Xarray_create_empty();{void*init=
_get_arr_size(xarr1->elmts,sizeof(void*))== 0?*((void**)_check_unknown_subscript(
xarr2->elmts,sizeof(void*),0)):*((void**)_check_unknown_subscript(xarr1->elmts,
sizeof(void*),0));struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp24=
_cycalloc(sizeof(*_tmp24));_tmp24->elmts=({unsigned int _tmp25=(unsigned int)
newsz;void**_tmp26=(void**)_cycalloc(_check_times(sizeof(void*),_tmp25));struct
_tagged_arr _tmp28=_tag_arr(_tmp26,sizeof(void*),_tmp25);{unsigned int _tmp27=
_tmp25;unsigned int i;for(i=0;i < _tmp27;i ++){_tmp26[i]=init;}}_tmp28;});_tmp24->num_elmts=
0;_tmp24;});{int i=0;for(0;i < xarr1->num_elmts;++ i){Cyc_Xarray_add(ans,*((void**)
_check_unknown_subscript(xarr1->elmts,sizeof(void*),i)));}}{int i=0;for(0;i < 
xarr2->num_elmts;++ i){Cyc_Xarray_add(ans,*((void**)_check_unknown_subscript(
xarr2->elmts,sizeof(void*),i)));}}return ans;}}void Cyc_Xarray_app(void*(*f)(void*),
struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}void Cyc_Xarray_app_c(
void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < 
xarr->num_elmts;++ i){f(env,*((void**)_check_unknown_subscript(xarr->elmts,
sizeof(void*),i)));}}void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*
xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(*((void**)_check_unknown_subscript(
xarr->elmts,sizeof(void*),i)));}}void Cyc_Xarray_iter_c(void(*f)(void*,void*),
void*env,struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(
env,*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}struct
Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_create_empty();{struct Cyc_Xarray_Xarray*
ans=({struct Cyc_Xarray_Xarray*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->elmts=({
unsigned int _tmp2A=_get_arr_size(xarr->elmts,sizeof(void*));void**_tmp2B=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp2A));struct _tagged_arr _tmp2D=_tag_arr(
_tmp2B,sizeof(void*),_tmp2A);{unsigned int _tmp2C=_tmp2A;unsigned int i;for(i=0;i < 
_tmp2C;i ++){_tmp2B[i]=f(*((void**)_check_unknown_subscript(xarr->elmts,sizeof(
void*),0)));}}_tmp2D;});_tmp29->num_elmts=xarr->num_elmts;_tmp29;});{int i=1;for(
0;i < xarr->num_elmts;++ i){*((void**)_check_unknown_subscript(ans->elmts,sizeof(
void*),i))=f(*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}
return ans;}}struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_Xarray_Xarray*xarr){if(xarr->num_elmts == 0)return Cyc_Xarray_create_empty();{
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp2E=_cycalloc(sizeof(*
_tmp2E));_tmp2E->elmts=({unsigned int _tmp2F=_get_arr_size(xarr->elmts,sizeof(
void*));void**_tmp30=(void**)_cycalloc(_check_times(sizeof(void*),_tmp2F));
struct _tagged_arr _tmp32=_tag_arr(_tmp30,sizeof(void*),_tmp2F);{unsigned int
_tmp31=_tmp2F;unsigned int i;for(i=0;i < _tmp31;i ++){_tmp30[i]=f(env,*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),0)));}}_tmp32;});_tmp2E->num_elmts=
xarr->num_elmts;_tmp2E;});{int i=1;for(0;i < xarr->num_elmts;++ i){*((void**)
_check_unknown_subscript(ans->elmts,sizeof(void*),i))=f(env,*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}return ans;}}void Cyc_Xarray_reuse(
struct Cyc_Xarray_Xarray*xarr){xarr->num_elmts=0;}void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*
xarr,int num){if(num > Cyc_Xarray_length(xarr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp34;_tmp34.tag=Cyc_Core_Invalid_argument;_tmp34.f1=({const char*_tmp35="number deleted is greater than length of xarray";
_tag_arr(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,48));});_tmp34;});_tmp33;}));
xarr->num_elmts -=num;}void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0?1: i > xarr->num_elmts - 1)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp37;_tmp37.tag=Cyc_Core_Invalid_argument;_tmp37.f1=({const char*_tmp38="xarray index out of bounds";
_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,27));});_tmp37;});_tmp36;}));{
int j=i;for(0;j < xarr->num_elmts - 2;j ++){*((void**)_check_unknown_subscript(xarr->elmts,
sizeof(void*),j))=*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),j
+ 1));}}xarr->num_elmts --;}
