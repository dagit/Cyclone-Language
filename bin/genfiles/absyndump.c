#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

/* Regions */
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
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

/* Built-in Exceptions */
struct Cyc_Null_Exception_struct { char *tag; };
struct Cyc_Array_bounds_struct { char *tag; };
struct Cyc_Match_Exception_struct { char *tag; };
struct Cyc_Bad_alloc_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
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
static _INLINE char *
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
static _INLINE unsigned
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

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_short(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_int(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_float(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_double(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char(char *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short(short *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int(int *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float(float *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double(double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble(long double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar(void **orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
#endif


/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })
  */
static _INLINE void 
_zero_arr_inplace_plus_char(char **x, int orig_i) {
  *x = _zero_arr_plus_char(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short **x, int orig_i) {
  *x = _zero_arr_plus_short(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int **x, int orig_i) {
  *x = _zero_arr_plus_int(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float **x, int orig_i) {
  *x = _zero_arr_plus_float(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double **x, int orig_i) {
  *x = _zero_arr_plus_double(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double **x, int orig_i) {
  *x = _zero_arr_plus_longdouble(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void ***x, int orig_i) {
  *x = _zero_arr_plus_voidstar(*x,1,orig_i);
}

/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })*/
  
static _INLINE char *
_zero_arr_inplace_plus_post_char(char **x, int orig_i){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short **x, int orig_i){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int **x, int orig_i){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float **x, int orig_i){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double **x, int orig_i){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double **x, int orig_i){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void ***x, int orig_i){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}



/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
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

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,struct
_dyneither_ptr src,int src_offset,int max_count);struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz);inline static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,
unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*
ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base)
 || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*
loc);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,
Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind
 = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind
 = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;
enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned
 = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA
 = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{int tag;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{
int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{
int tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};extern
struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*);struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct
_tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple10{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple10*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);struct _tuple12{unsigned int f1;int f2;};struct _tuple12 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x);void Cyc_Absyndump_ignore(void*x){return;}static
unsigned int Cyc_Absyndump_pos=0;static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before();
int Cyc_Absyndump_need_space_before(){switch(Cyc_Absyndump_prev_char){case '{':
_LL0: goto _LL1;case '}': _LL1: goto _LL2;case '(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;
case '[': _LL4: goto _LL5;case ']': _LL5: goto _LL6;case ';': _LL6: goto _LL7;case ',': _LL7:
goto _LL8;case '=': _LL8: goto _LL9;case '?': _LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ':
_LLB: goto _LLC;case '\n': _LLC: goto _LLD;case '*': _LLD: return 0;default: _LLE: return 1;}}
void Cyc_Absyndump_dump(struct _dyneither_ptr s);void Cyc_Absyndump_dump(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s);void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){int sz=(
int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos +=sz;if(sz >= 1){Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((const char*)
_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c);void Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc);void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dyneither_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);};}void Cyc_Absyndump_dump_str(
struct _dyneither_ptr*s);void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){Cyc_Absyndump_dump(*
s);}void Cyc_Absyndump_dump_semi();void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((
int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,
struct _dyneither_ptr sep);void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f((void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep);
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)
l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep);void Cyc_Absyndump_group(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(f,l,
sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(void*,
void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep);void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep);void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(
l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct
_tuple0*v);void Cyc_Absyndump_dumpqvar(struct _tuple0*v){struct Cyc_List_List*_tmp1=
0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace _tmp2=(*v).f1;int _tmp3;
struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;_LL11: if((_tmp2.Loc_n).tag != 
3)goto _LL13;_tmp3=(int)(_tmp2.Loc_n).val;_LL12: _tmp4=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag
!= 1)goto _LL15;_tmp4=(struct Cyc_List_List*)(_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;
goto _LL10;_LL15: if((_tmp2.Abs_n).tag != 2)goto _LL10;_tmp5=(struct Cyc_List_List*)(
_tmp2.Abs_n).val;_LL16: if(Cyc_Absyndump_qvar_to_Cids  && Cyc_Absyndump_add_cyc_prefix)
prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;_tmp1=_tmp5;goto _LL10;
_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp3A0;Cyc_Absyndump_dump_nospace(((
_tmp3A0="::",_tag_dyneither(_tmp3A0,sizeof(char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp3A1;Cyc_Absyndump_dump_nospace(((_tmp3A1="::",_tag_dyneither(_tmp3A1,sizeof(
char),3))));}Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmp1->hd));}
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp3A2;Cyc_Absyndump_dump_nospace(((
_tmp3A2="_",_tag_dyneither(_tmp3A2,sizeof(char),2))));}else{const char*_tmp3A3;
Cyc_Absyndump_dump_nospace(((_tmp3A3="::",_tag_dyneither(_tmp3A3,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*
v).f2);else{Cyc_Absyndump_dump_str((*v).f2);}}}void Cyc_Absyndump_dumptq(struct
Cyc_Absyn_Tqual tq);void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp3A4;Cyc_Absyndump_dump(((_tmp3A4="restrict",_tag_dyneither(_tmp3A4,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp3A5;Cyc_Absyndump_dump(((
_tmp3A5="volatile",_tag_dyneither(_tmp3A5,sizeof(char),9))));}if(tq.print_const){
const char*_tmp3A6;Cyc_Absyndump_dump(((_tmp3A6="const",_tag_dyneither(_tmp3A6,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc);void Cyc_Absyndump_dumpscope(
enum Cyc_Absyn_Scope sc){switch(sc){case Cyc_Absyn_Static: _LL17:{const char*_tmp3A7;
Cyc_Absyndump_dump(((_tmp3A7="static",_tag_dyneither(_tmp3A7,sizeof(char),7))));}
return;case Cyc_Absyn_Public: _LL18: return;case Cyc_Absyn_Extern: _LL19:{const char*
_tmp3A8;Cyc_Absyndump_dump(((_tmp3A8="extern",_tag_dyneither(_tmp3A8,sizeof(char),
7))));}return;case Cyc_Absyn_ExternC: _LL1A:{const char*_tmp3A9;Cyc_Absyndump_dump(((
_tmp3A9="extern \"C\"",_tag_dyneither(_tmp3A9,sizeof(char),11))));}return;case
Cyc_Absyn_Abstract: _LL1B:{const char*_tmp3AA;Cyc_Absyndump_dump(((_tmp3AA="abstract",
_tag_dyneither(_tmp3AA,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1C:{
const char*_tmp3AB;Cyc_Absyndump_dump(((_tmp3AB="register",_tag_dyneither(_tmp3AB,
sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka);
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp13;
enum Cyc_Absyn_KindQual _tmp14;enum Cyc_Absyn_AliasQual _tmp15;struct Cyc_Absyn_Kind*
_tmp12=ka;_tmp13=*_tmp12;_tmp14=_tmp13.kind;_tmp15=_tmp13.aliasqual;switch(
_tmp14){case Cyc_Absyn_AnyKind: _LL1E: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL20:{
const char*_tmp3AC;Cyc_Absyndump_dump(((_tmp3AC="A",_tag_dyneither(_tmp3AC,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL21:{const char*_tmp3AD;Cyc_Absyndump_dump(((
_tmp3AD="UA",_tag_dyneither(_tmp3AD,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL22:{const char*_tmp3AE;Cyc_Absyndump_dump(((_tmp3AE="TA",_tag_dyneither(
_tmp3AE,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL1F: switch(_tmp15){
case Cyc_Absyn_Aliasable: _LL25:{const char*_tmp3AF;Cyc_Absyndump_dump(((_tmp3AF="M",
_tag_dyneither(_tmp3AF,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL26:{
const char*_tmp3B0;Cyc_Absyndump_dump(((_tmp3B0="UM",_tag_dyneither(_tmp3B0,
sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL27:{const char*_tmp3B1;Cyc_Absyndump_dump(((
_tmp3B1="TM",_tag_dyneither(_tmp3B1,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind:
_LL24: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL2A:{const char*_tmp3B2;Cyc_Absyndump_dump(((
_tmp3B2="B",_tag_dyneither(_tmp3B2,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL2B:{const char*_tmp3B3;Cyc_Absyndump_dump(((_tmp3B3="UB",_tag_dyneither(
_tmp3B3,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2C:{const char*_tmp3B4;
Cyc_Absyndump_dump(((_tmp3B4="TB",_tag_dyneither(_tmp3B4,sizeof(char),3))));}
return;}case Cyc_Absyn_RgnKind: _LL29: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL2F:{
const char*_tmp3B5;Cyc_Absyndump_dump(((_tmp3B5="R",_tag_dyneither(_tmp3B5,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL30:{const char*_tmp3B6;Cyc_Absyndump_dump(((
_tmp3B6="UR",_tag_dyneither(_tmp3B6,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL31:{const char*_tmp3B7;Cyc_Absyndump_dump(((_tmp3B7="TR",_tag_dyneither(
_tmp3B7,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL2E:{const char*
_tmp3B8;Cyc_Absyndump_dump(((_tmp3B8="E",_tag_dyneither(_tmp3B8,sizeof(char),2))));}
return;case Cyc_Absyn_IntKind: _LL33:{const char*_tmp3B9;Cyc_Absyndump_dump(((
_tmp3B9="I",_tag_dyneither(_tmp3B9,sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(
enum Cyc_Absyn_AggrKind k);void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LL35:{const char*_tmp3BA;Cyc_Absyndump_dump(((
_tmp3BA="struct",_tag_dyneither(_tmp3BA,sizeof(char),7))));}return;case Cyc_Absyn_UnionA:
_LL36:{const char*_tmp3BB;Cyc_Absyndump_dump(((_tmp3BB="union",_tag_dyneither(
_tmp3BB,sizeof(char),6))));}return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts);void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){const char*_tmp3BE;const
char*_tmp3BD;const char*_tmp3BC;((void(*)(void(*f)(void*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptyp,ts,((_tmp3BC="<",_tag_dyneither(_tmp3BC,sizeof(char),2))),((
_tmp3BD=">",_tag_dyneither(_tmp3BD,sizeof(char),2))),((_tmp3BE=",",
_tag_dyneither(_tmp3BE,sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*
tv);void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv);void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp29=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp2B;struct Cyc_Absyn_Kind _tmp2C;enum Cyc_Absyn_KindQual
_tmp2D;enum Cyc_Absyn_AliasQual _tmp2E;struct Cyc_Absyn_Kind*_tmp32;_LL39: {struct
Cyc_Absyn_Eq_kb_struct*_tmp2A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp29;if(_tmp2A->tag
!= 0)goto _LL3B;else{_tmp2B=_tmp2A->f1;_tmp2C=*_tmp2B;_tmp2D=_tmp2C.kind;if(
_tmp2D != Cyc_Absyn_BoxKind)goto _LL3B;_tmp2E=_tmp2C.aliasqual;if(_tmp2E != Cyc_Absyn_Aliasable)
goto _LL3B;}}_LL3A: goto _LL3C;_LL3B: {struct Cyc_Absyn_Less_kb_struct*_tmp2F=(
struct Cyc_Absyn_Less_kb_struct*)_tmp29;if(_tmp2F->tag != 2)goto _LL3D;}_LL3C: goto
_LL3E;_LL3D: {struct Cyc_Absyn_Unknown_kb_struct*_tmp30=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp29;if(_tmp30->tag != 1)goto _LL3F;}_LL3E:{const char*_tmp3BF;Cyc_Absyndump_dump(((
_tmp3BF="::?",_tag_dyneither(_tmp3BF,sizeof(char),4))));}goto _LL38;_LL3F: {
struct Cyc_Absyn_Eq_kb_struct*_tmp31=(struct Cyc_Absyn_Eq_kb_struct*)_tmp29;if(
_tmp31->tag != 0)goto _LL38;else{_tmp32=_tmp31->f1;}}_LL40:{const char*_tmp3C0;Cyc_Absyndump_dump(((
_tmp3C0="::",_tag_dyneither(_tmp3C0,sizeof(char),3))));}Cyc_Absyndump_dumpkind(
_tmp32);goto _LL38;_LL38:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs);
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){const char*_tmp3C3;const char*
_tmp3C2;const char*_tmp3C1;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,((_tmp3C1="<",_tag_dyneither(_tmp3C1,sizeof(char),2))),((
_tmp3C2=">",_tag_dyneither(_tmp3C2,sizeof(char),2))),((_tmp3C3=",",
_tag_dyneither(_tmp3C3,sizeof(char),2))));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs);void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*
tvs){const char*_tmp3C6;const char*_tmp3C5;const char*_tmp3C4;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,((_tmp3C4="<",_tag_dyneither(_tmp3C4,sizeof(char),2))),((_tmp3C5=">",
_tag_dyneither(_tmp3C5,sizeof(char),2))),((_tmp3C6=",",_tag_dyneither(_tmp3C6,
sizeof(char),2))));}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple13*pr);void Cyc_Absyndump_dumparg(struct _tuple13*pr){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(
void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(struct Cyc_List_List*
ts);void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){const char*_tmp3C9;const
char*_tmp3C8;const char*_tmp3C7;((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,((_tmp3C7="(",_tag_dyneither(_tmp3C7,sizeof(char),2))),((
_tmp3C8=")",_tag_dyneither(_tmp3C8,sizeof(char),2))),((_tmp3C9=",",
_tag_dyneither(_tmp3C9,sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts);void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmp3E=(void*)atts->hd;_LL42: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp3F=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp3E;
if(_tmp3F->tag != 1)goto _LL44;}_LL43:{const char*_tmp3CA;Cyc_Absyndump_dump(((
_tmp3CA="_stdcall",_tag_dyneither(_tmp3CA,sizeof(char),9))));}return;_LL44: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp40=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp3E;if(_tmp40->tag != 2)goto _LL46;}_LL45:{const char*_tmp3CB;Cyc_Absyndump_dump(((
_tmp3CB="_cdecl",_tag_dyneither(_tmp3CB,sizeof(char),7))));}return;_LL46: {
struct Cyc_Absyn_Fastcall_att_struct*_tmp41=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp3E;if(_tmp41->tag != 3)goto _LL48;}_LL47:{const char*_tmp3CC;Cyc_Absyndump_dump(((
_tmp3CC="_fastcall",_tag_dyneither(_tmp3CC,sizeof(char),10))));}return;_LL48:;
_LL49: goto _LL41;_LL41:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts);void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{
struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp45=(
void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4B: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp46=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp45;if(_tmp46->tag != 1)goto _LL4D;}
_LL4C: goto _LL4E;_LL4D: {struct Cyc_Absyn_Cdecl_att_struct*_tmp47=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp45;if(_tmp47->tag != 2)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp48=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp45;if(_tmp48->tag != 3)goto _LL51;}
_LL50: goto _LL4A;_LL51:;_LL52: hasatt=1;goto _LL4A;_LL4A:;}}if(!hasatt)return;{
const char*_tmp3CD;Cyc_Absyndump_dump(((_tmp3CD="__declspec(",_tag_dyneither(
_tmp3CD,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*_tmp4A=(void*)
atts->hd;_LL54: {struct Cyc_Absyn_Stdcall_att_struct*_tmp4B=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp4A;if(_tmp4B->tag != 1)goto _LL56;}_LL55: goto _LL57;_LL56: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp4C=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp4A;if(_tmp4C->tag != 2)goto _LL58;}
_LL57: goto _LL59;_LL58: {struct Cyc_Absyn_Fastcall_att_struct*_tmp4D=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp4A;if(_tmp4D->tag != 3)goto _LL5A;}_LL59: goto _LL53;_LL5A:;_LL5B: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL53;_LL53:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts);void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL5C:{const char*_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE=" __attribute__((",
_tag_dyneither(_tmp3CE,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3CF;
Cyc_Absyndump_dump(((_tmp3CF=",",_tag_dyneither(_tmp3CF,sizeof(char),2))));}}{
const char*_tmp3D0;Cyc_Absyndump_dump(((_tmp3D0=")) ",_tag_dyneither(_tmp3D0,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL5D: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms);int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp51=(void*)tms->hd;_LL60: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp52=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp51;if(_tmp52->tag != 2)goto _LL62;}_LL61: return 1;_LL62:;_LL63: return 0;_LL5F:;};}
static void Cyc_Absyndump_dumprgn(void*t);static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp53=Cyc_Tcutil_compress(t);_LL65: {struct Cyc_Absyn_HeapRgn_struct*_tmp54=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp53;if(_tmp54->tag != 21)goto _LL67;}_LL66:{
const char*_tmp3D1;Cyc_Absyndump_dump(((_tmp3D1="`H",_tag_dyneither(_tmp3D1,
sizeof(char),3))));}goto _LL64;_LL67:;_LL68: Cyc_Absyndump_dumpntyp(t);goto _LL64;
_LL64:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple14 Cyc_Absyndump_effects_split(void*t);static struct _tuple14 Cyc_Absyndump_effects_split(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp56=
Cyc_Tcutil_compress(t);void*_tmp58;struct Cyc_List_List*_tmp5A;_LL6A: {struct Cyc_Absyn_AccessEff_struct*
_tmp57=(struct Cyc_Absyn_AccessEff_struct*)_tmp56;if(_tmp57->tag != 23)goto _LL6C;
else{_tmp58=(void*)_tmp57->f1;}}_LL6B:{struct Cyc_List_List*_tmp3D2;rgions=((
_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->hd=_tmp58,((_tmp3D2->tl=rgions,
_tmp3D2))))));}goto _LL69;_LL6C: {struct Cyc_Absyn_JoinEff_struct*_tmp59=(struct
Cyc_Absyn_JoinEff_struct*)_tmp56;if(_tmp59->tag != 24)goto _LL6E;else{_tmp5A=
_tmp59->f1;}}_LL6D: for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){struct Cyc_List_List*
_tmp5D;struct Cyc_List_List*_tmp5E;struct _tuple14 _tmp5C=Cyc_Absyndump_effects_split((
void*)_tmp5A->hd);_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5D,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp5E,effects);}goto _LL69;_LL6E:;_LL6F:{struct Cyc_List_List*
_tmp3D3;effects=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->hd=t,((_tmp3D3->tl=
effects,_tmp3D3))))));}goto _LL69;_LL69:;}{struct _tuple14 _tmp3D4;return(_tmp3D4.f1=
rgions,((_tmp3D4.f2=effects,_tmp3D4)));};}static void Cyc_Absyndump_dumpeff(void*t);
static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp62;struct Cyc_List_List*
_tmp63;struct _tuple14 _tmp61=Cyc_Absyndump_effects_split(t);_tmp62=_tmp61.f1;
_tmp63=_tmp61.f2;_tmp62=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp62);_tmp63=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp63);for(0;_tmp63 != 0;_tmp63=_tmp63->tl){Cyc_Absyndump_dumpntyp((void*)_tmp63->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp62 != 0;_tmp62=_tmp62->tl){Cyc_Absyndump_dumprgn((void*)_tmp62->hd);if(_tmp62->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t);void Cyc_Absyndump_dumpntyp(void*t){void*_tmp64=t;
struct Cyc_Absyn_Tvar*_tmp6A;struct Cyc_Core_Opt*_tmp6C;struct Cyc_Core_Opt*_tmp6D;
int _tmp6E;struct Cyc_Core_Opt*_tmp70;struct Cyc_Core_Opt*_tmp71;struct Cyc_Core_Opt
_tmp72;void*_tmp73;int _tmp74;struct Cyc_Absyn_DatatypeInfo _tmp76;union Cyc_Absyn_DatatypeInfoU
_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_Absyn_DatatypeFieldInfo _tmp7A;union
Cyc_Absyn_DatatypeFieldInfoU _tmp7B;struct Cyc_List_List*_tmp7C;enum Cyc_Absyn_Sign
_tmp7E;enum Cyc_Absyn_Size_of _tmp7F;enum Cyc_Absyn_Sign _tmp81;enum Cyc_Absyn_Size_of
_tmp82;enum Cyc_Absyn_Sign _tmp84;enum Cyc_Absyn_Size_of _tmp85;enum Cyc_Absyn_Sign
_tmp87;enum Cyc_Absyn_Size_of _tmp88;enum Cyc_Absyn_Sign _tmp8A;enum Cyc_Absyn_Size_of
_tmp8B;enum Cyc_Absyn_Sign _tmp8D;enum Cyc_Absyn_Size_of _tmp8E;enum Cyc_Absyn_Sign
_tmp90;enum Cyc_Absyn_Size_of _tmp91;enum Cyc_Absyn_Sign _tmp93;enum Cyc_Absyn_Size_of
_tmp94;enum Cyc_Absyn_Sign _tmp96;enum Cyc_Absyn_Size_of _tmp97;enum Cyc_Absyn_Sign
_tmp99;enum Cyc_Absyn_Size_of _tmp9A;enum Cyc_Absyn_Sign _tmp9C;enum Cyc_Absyn_Size_of
_tmp9D;enum Cyc_Absyn_Sign _tmp9F;enum Cyc_Absyn_Size_of _tmpA0;enum Cyc_Absyn_Sign
_tmpA2;enum Cyc_Absyn_Size_of _tmpA3;enum Cyc_Absyn_Sign _tmpA5;enum Cyc_Absyn_Size_of
_tmpA6;enum Cyc_Absyn_Sign _tmpA8;enum Cyc_Absyn_Size_of _tmpA9;int _tmpAC;struct
Cyc_List_List*_tmpAE;struct Cyc_Absyn_AggrInfo _tmpB0;union Cyc_Absyn_AggrInfoU
_tmpB1;struct Cyc_List_List*_tmpB2;enum Cyc_Absyn_AggrKind _tmpB4;struct Cyc_List_List*
_tmpB5;struct _tuple0*_tmpB7;struct Cyc_List_List*_tmpB9;struct _tuple0*_tmpBB;
struct Cyc_List_List*_tmpBC;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpC0;void*_tmpC2;
void*_tmpC3;void*_tmpC5;_LL71: {struct Cyc_Absyn_ArrayType_struct*_tmp65=(struct
Cyc_Absyn_ArrayType_struct*)_tmp64;if(_tmp65->tag != 9)goto _LL73;}_LL72: goto _LL74;
_LL73: {struct Cyc_Absyn_FnType_struct*_tmp66=(struct Cyc_Absyn_FnType_struct*)
_tmp64;if(_tmp66->tag != 10)goto _LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_PointerType_struct*
_tmp67=(struct Cyc_Absyn_PointerType_struct*)_tmp64;if(_tmp67->tag != 5)goto _LL77;}
_LL76: return;_LL77: {struct Cyc_Absyn_VoidType_struct*_tmp68=(struct Cyc_Absyn_VoidType_struct*)
_tmp64;if(_tmp68->tag != 0)goto _LL79;}_LL78:{const char*_tmp3D5;Cyc_Absyndump_dump(((
_tmp3D5="void",_tag_dyneither(_tmp3D5,sizeof(char),5))));}return;_LL79: {struct
Cyc_Absyn_VarType_struct*_tmp69=(struct Cyc_Absyn_VarType_struct*)_tmp64;if(
_tmp69->tag != 2)goto _LL7B;else{_tmp6A=_tmp69->f1;}}_LL7A: Cyc_Absyndump_dump_str(
_tmp6A->name);return;_LL7B: {struct Cyc_Absyn_Evar_struct*_tmp6B=(struct Cyc_Absyn_Evar_struct*)
_tmp64;if(_tmp6B->tag != 1)goto _LL7D;else{_tmp6C=_tmp6B->f1;_tmp6D=_tmp6B->f2;if(
_tmp6D != 0)goto _LL7D;_tmp6E=_tmp6B->f3;}}_LL7C:{const char*_tmp3D6;Cyc_Absyndump_dump(((
_tmp3D6="%",_tag_dyneither(_tmp3D6,sizeof(char),2))));}if(_tmp6C == 0){const char*
_tmp3D7;Cyc_Absyndump_dump(((_tmp3D7="?",_tag_dyneither(_tmp3D7,sizeof(char),2))));}
else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp6C->v);}{const char*
_tmp3DB;void*_tmp3DA[1];struct Cyc_Int_pa_struct _tmp3D9;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp3D9.tag=1,((_tmp3D9.f1=(unsigned long)_tmp6E,((
_tmp3DA[0]=& _tmp3D9,Cyc_aprintf(((_tmp3DB="(%d)",_tag_dyneither(_tmp3DB,sizeof(
char),5))),_tag_dyneither(_tmp3DA,sizeof(void*),1)))))))));}return;_LL7D: {
struct Cyc_Absyn_Evar_struct*_tmp6F=(struct Cyc_Absyn_Evar_struct*)_tmp64;if(
_tmp6F->tag != 1)goto _LL7F;else{_tmp70=_tmp6F->f1;_tmp71=_tmp6F->f2;if(_tmp71 == 0)
goto _LL7F;_tmp72=*_tmp71;_tmp73=(void*)_tmp72.v;_tmp74=_tmp6F->f3;}}_LL7E: Cyc_Absyndump_dumpntyp(
_tmp73);return;_LL7F: {struct Cyc_Absyn_DatatypeType_struct*_tmp75=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp64;if(_tmp75->tag != 3)goto _LL81;else{_tmp76=_tmp75->f1;_tmp77=_tmp76.datatype_info;
_tmp78=_tmp76.targs;}}_LL80:{union Cyc_Absyn_DatatypeInfoU _tmpD1=_tmp77;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpD2;struct _tuple0*_tmpD3;int _tmpD4;struct Cyc_Absyn_Datatypedecl**_tmpD5;
struct Cyc_Absyn_Datatypedecl*_tmpD6;struct Cyc_Absyn_Datatypedecl _tmpD7;struct
_tuple0*_tmpD8;int _tmpD9;_LLC4: if((_tmpD1.UnknownDatatype).tag != 1)goto _LLC6;
_tmpD2=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD1.UnknownDatatype).val;_tmpD3=
_tmpD2.name;_tmpD4=_tmpD2.is_extensible;_LLC5: _tmpD8=_tmpD3;_tmpD9=_tmpD4;goto
_LLC7;_LLC6: if((_tmpD1.KnownDatatype).tag != 2)goto _LLC3;_tmpD5=(struct Cyc_Absyn_Datatypedecl**)(
_tmpD1.KnownDatatype).val;_tmpD6=*_tmpD5;_tmpD7=*_tmpD6;_tmpD8=_tmpD7.name;
_tmpD9=_tmpD7.is_extensible;_LLC7: if(_tmpD9){const char*_tmp3DC;Cyc_Absyndump_dump(((
_tmp3DC="@extensible ",_tag_dyneither(_tmp3DC,sizeof(char),13))));}{const char*
_tmp3DD;Cyc_Absyndump_dump(((_tmp3DD="datatype ",_tag_dyneither(_tmp3DD,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpD8);Cyc_Absyndump_dumptps(_tmp78);goto
_LLC3;_LLC3:;}goto _LL70;_LL81: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp79=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp64;if(_tmp79->tag != 4)goto _LL83;
else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.field_info;_tmp7C=_tmp7A.targs;}}_LL82:{
union Cyc_Absyn_DatatypeFieldInfoU _tmpDC=_tmp7B;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpDD;struct _tuple0*_tmpDE;struct _tuple0*_tmpDF;int _tmpE0;struct _tuple1 _tmpE1;
struct Cyc_Absyn_Datatypedecl*_tmpE2;struct Cyc_Absyn_Datatypedecl _tmpE3;struct
_tuple0*_tmpE4;int _tmpE5;struct Cyc_Absyn_Datatypefield*_tmpE6;struct Cyc_Absyn_Datatypefield
_tmpE7;struct _tuple0*_tmpE8;_LLC9: if((_tmpDC.UnknownDatatypefield).tag != 1)goto
_LLCB;_tmpDD=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDC.UnknownDatatypefield).val;
_tmpDE=_tmpDD.datatype_name;_tmpDF=_tmpDD.field_name;_tmpE0=_tmpDD.is_extensible;
_LLCA: _tmpE4=_tmpDE;_tmpE5=_tmpE0;_tmpE8=_tmpDF;goto _LLCC;_LLCB: if((_tmpDC.KnownDatatypefield).tag
!= 2)goto _LLC8;_tmpE1=(struct _tuple1)(_tmpDC.KnownDatatypefield).val;_tmpE2=
_tmpE1.f1;_tmpE3=*_tmpE2;_tmpE4=_tmpE3.name;_tmpE5=_tmpE3.is_extensible;_tmpE6=
_tmpE1.f2;_tmpE7=*_tmpE6;_tmpE8=_tmpE7.name;_LLCC: if(_tmpE5){const char*_tmp3DE;
Cyc_Absyndump_dump(((_tmp3DE="@extensible ",_tag_dyneither(_tmp3DE,sizeof(char),
13))));}{const char*_tmp3DF;Cyc_Absyndump_dump(((_tmp3DF="datatype ",
_tag_dyneither(_tmp3DF,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpE4);{
const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0=".",_tag_dyneither(_tmp3E0,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpE8);Cyc_Absyndump_dumptps(_tmp7C);
goto _LLC8;_LLC8:;}goto _LL70;_LL83: {struct Cyc_Absyn_IntType_struct*_tmp7D=(
struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp7D->tag != 6)goto _LL85;else{_tmp7E=
_tmp7D->f1;if(_tmp7E != Cyc_Absyn_None)goto _LL85;_tmp7F=_tmp7D->f2;if(_tmp7F != 
Cyc_Absyn_Int_sz)goto _LL85;}}_LL84: goto _LL86;_LL85: {struct Cyc_Absyn_IntType_struct*
_tmp80=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp80->tag != 6)goto _LL87;
else{_tmp81=_tmp80->f1;if(_tmp81 != Cyc_Absyn_Signed)goto _LL87;_tmp82=_tmp80->f2;
if(_tmp82 != Cyc_Absyn_Int_sz)goto _LL87;}}_LL86:{const char*_tmp3E1;Cyc_Absyndump_dump(((
_tmp3E1="int",_tag_dyneither(_tmp3E1,sizeof(char),4))));}return;_LL87: {struct
Cyc_Absyn_IntType_struct*_tmp83=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(
_tmp83->tag != 6)goto _LL89;else{_tmp84=_tmp83->f1;if(_tmp84 != Cyc_Absyn_None)goto
_LL89;_tmp85=_tmp83->f2;if(_tmp85 != Cyc_Absyn_Long_sz)goto _LL89;}}_LL88: goto
_LL8A;_LL89: {struct Cyc_Absyn_IntType_struct*_tmp86=(struct Cyc_Absyn_IntType_struct*)
_tmp64;if(_tmp86->tag != 6)goto _LL8B;else{_tmp87=_tmp86->f1;if(_tmp87 != Cyc_Absyn_Signed)
goto _LL8B;_tmp88=_tmp86->f2;if(_tmp88 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A:{
const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="long",_tag_dyneither(_tmp3E2,
sizeof(char),5))));}return;_LL8B: {struct Cyc_Absyn_IntType_struct*_tmp89=(struct
Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp89->tag != 6)goto _LL8D;else{_tmp8A=_tmp89->f1;
if(_tmp8A != Cyc_Absyn_None)goto _LL8D;_tmp8B=_tmp89->f2;if(_tmp8B != Cyc_Absyn_Char_sz)
goto _LL8D;}}_LL8C:{const char*_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3="char",
_tag_dyneither(_tmp3E3,sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_struct*
_tmp8C=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp8C->tag != 6)goto _LL8F;
else{_tmp8D=_tmp8C->f1;if(_tmp8D != Cyc_Absyn_Signed)goto _LL8F;_tmp8E=_tmp8C->f2;
if(_tmp8E != Cyc_Absyn_Char_sz)goto _LL8F;}}_LL8E:{const char*_tmp3E4;Cyc_Absyndump_dump(((
_tmp3E4="signed char",_tag_dyneither(_tmp3E4,sizeof(char),12))));}return;_LL8F: {
struct Cyc_Absyn_IntType_struct*_tmp8F=(struct Cyc_Absyn_IntType_struct*)_tmp64;
if(_tmp8F->tag != 6)goto _LL91;else{_tmp90=_tmp8F->f1;if(_tmp90 != Cyc_Absyn_Unsigned)
goto _LL91;_tmp91=_tmp8F->f2;if(_tmp91 != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:{
const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5="unsigned char",_tag_dyneither(
_tmp3E5,sizeof(char),14))));}return;_LL91: {struct Cyc_Absyn_IntType_struct*
_tmp92=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp92->tag != 6)goto _LL93;
else{_tmp93=_tmp92->f1;if(_tmp93 != Cyc_Absyn_None)goto _LL93;_tmp94=_tmp92->f2;
if(_tmp94 != Cyc_Absyn_Short_sz)goto _LL93;}}_LL92: goto _LL94;_LL93: {struct Cyc_Absyn_IntType_struct*
_tmp95=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp95->tag != 6)goto _LL95;
else{_tmp96=_tmp95->f1;if(_tmp96 != Cyc_Absyn_Signed)goto _LL95;_tmp97=_tmp95->f2;
if(_tmp97 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94:{const char*_tmp3E6;Cyc_Absyndump_dump(((
_tmp3E6="short",_tag_dyneither(_tmp3E6,sizeof(char),6))));}return;_LL95: {struct
Cyc_Absyn_IntType_struct*_tmp98=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(
_tmp98->tag != 6)goto _LL97;else{_tmp99=_tmp98->f1;if(_tmp99 != Cyc_Absyn_Unsigned)
goto _LL97;_tmp9A=_tmp98->f2;if(_tmp9A != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:{
const char*_tmp3E7;Cyc_Absyndump_dump(((_tmp3E7="unsigned short",_tag_dyneither(
_tmp3E7,sizeof(char),15))));}return;_LL97: {struct Cyc_Absyn_IntType_struct*
_tmp9B=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmp9B->tag != 6)goto _LL99;
else{_tmp9C=_tmp9B->f1;if(_tmp9C != Cyc_Absyn_Unsigned)goto _LL99;_tmp9D=_tmp9B->f2;
if(_tmp9D != Cyc_Absyn_Int_sz)goto _LL99;}}_LL98:{const char*_tmp3E8;Cyc_Absyndump_dump(((
_tmp3E8="unsigned int",_tag_dyneither(_tmp3E8,sizeof(char),13))));}return;_LL99: {
struct Cyc_Absyn_IntType_struct*_tmp9E=(struct Cyc_Absyn_IntType_struct*)_tmp64;
if(_tmp9E->tag != 6)goto _LL9B;else{_tmp9F=_tmp9E->f1;if(_tmp9F != Cyc_Absyn_Unsigned)
goto _LL9B;_tmpA0=_tmp9E->f2;if(_tmpA0 != Cyc_Absyn_Long_sz)goto _LL9B;}}_LL9A:{
const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="unsigned long",_tag_dyneither(
_tmp3E9,sizeof(char),14))));}return;_LL9B: {struct Cyc_Absyn_IntType_struct*
_tmpA1=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmpA1->tag != 6)goto _LL9D;
else{_tmpA2=_tmpA1->f1;if(_tmpA2 != Cyc_Absyn_None)goto _LL9D;_tmpA3=_tmpA1->f2;
if(_tmpA3 != Cyc_Absyn_LongLong_sz)goto _LL9D;}}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmpA4=(struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmpA4->tag != 6)goto _LL9F;
else{_tmpA5=_tmpA4->f1;if(_tmpA5 != Cyc_Absyn_Signed)goto _LL9F;_tmpA6=_tmpA4->f2;
if(_tmpA6 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLCD:{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="long long",
_tag_dyneither(_tmp3EA,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLCE:{
const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB="__int64",_tag_dyneither(_tmp3EB,
sizeof(char),8))));}return;}_LL9F: {struct Cyc_Absyn_IntType_struct*_tmpA7=(
struct Cyc_Absyn_IntType_struct*)_tmp64;if(_tmpA7->tag != 6)goto _LLA1;else{_tmpA8=
_tmpA7->f1;if(_tmpA8 != Cyc_Absyn_Unsigned)goto _LLA1;_tmpA9=_tmpA7->f2;if(_tmpA9
!= Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Vc_c: _LLD0:{const char*_tmp3EC;Cyc_Absyndump_dump(((_tmp3EC="unsigned __int64",
_tag_dyneither(_tmp3EC,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD1:{
const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="unsigned long long",
_tag_dyneither(_tmp3ED,sizeof(char),19))));}return;}_LLA1: {struct Cyc_Absyn_FloatType_struct*
_tmpAA=(struct Cyc_Absyn_FloatType_struct*)_tmp64;if(_tmpAA->tag != 7)goto _LLA3;}
_LLA2:{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="float",_tag_dyneither(
_tmp3EE,sizeof(char),6))));}return;_LLA3: {struct Cyc_Absyn_DoubleType_struct*
_tmpAB=(struct Cyc_Absyn_DoubleType_struct*)_tmp64;if(_tmpAB->tag != 8)goto _LLA5;
else{_tmpAC=_tmpAB->f1;}}_LLA4: if(_tmpAC){const char*_tmp3EF;Cyc_Absyndump_dump(((
_tmp3EF="long double",_tag_dyneither(_tmp3EF,sizeof(char),12))));}else{const char*
_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="double",_tag_dyneither(_tmp3F0,sizeof(char),
7))));}return;_LLA5: {struct Cyc_Absyn_TupleType_struct*_tmpAD=(struct Cyc_Absyn_TupleType_struct*)
_tmp64;if(_tmpAD->tag != 11)goto _LLA7;else{_tmpAE=_tmpAD->f1;}}_LLA6: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmpAE);return;_LLA7: {struct Cyc_Absyn_AggrType_struct*
_tmpAF=(struct Cyc_Absyn_AggrType_struct*)_tmp64;if(_tmpAF->tag != 12)goto _LLA9;
else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.aggr_info;_tmpB2=_tmpB0.targs;}}_LLA8: {
enum Cyc_Absyn_AggrKind _tmpFD;struct _tuple0*_tmpFE;struct _tuple9 _tmpFC=Cyc_Absyn_aggr_kinded_name(
_tmpB1);_tmpFD=_tmpFC.f1;_tmpFE=_tmpFC.f2;Cyc_Absyndump_dumpaggr_kind(_tmpFD);
Cyc_Absyndump_dumpqvar(_tmpFE);Cyc_Absyndump_dumptps(_tmpB2);return;}_LLA9: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpB3=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64;if(_tmpB3->tag != 13)goto _LLAB;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}
_LLAA: Cyc_Absyndump_dumpaggr_kind(_tmpB4);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpB5);Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_struct*
_tmpB6=(struct Cyc_Absyn_EnumType_struct*)_tmp64;if(_tmpB6->tag != 14)goto _LLAD;
else{_tmpB7=_tmpB6->f1;}}_LLAC:{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="enum ",
_tag_dyneither(_tmp3F1,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpB7);
return;_LLAD: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB8=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64;if(_tmpB8->tag != 15)goto _LLAF;else{_tmpB9=_tmpB8->f1;}}_LLAE:{const char*
_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="enum {",_tag_dyneither(_tmp3F2,sizeof(char),
7))));}Cyc_Absyndump_dumpenumfields(_tmpB9);{const char*_tmp3F3;Cyc_Absyndump_dump(((
_tmp3F3="}",_tag_dyneither(_tmp3F3,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_struct*
_tmpBA=(struct Cyc_Absyn_TypedefType_struct*)_tmp64;if(_tmpBA->tag != 18)goto _LLB1;
else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}_LLB0:(Cyc_Absyndump_dumpqvar(_tmpBB),
Cyc_Absyndump_dumptps(_tmpBC));return;_LLB1: {struct Cyc_Absyn_ValueofType_struct*
_tmpBD=(struct Cyc_Absyn_ValueofType_struct*)_tmp64;if(_tmpBD->tag != 19)goto _LLB3;
else{_tmpBE=_tmpBD->f1;}}_LLB2:{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="valueof_t(",
_tag_dyneither(_tmp3F4,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpBE);{const
char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5=")",_tag_dyneither(_tmp3F5,sizeof(char),
2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*_tmpBF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64;if(_tmpBF->tag != 16)goto _LLB5;else{_tmpC0=(void*)_tmpBF->f1;}}_LLB4:{
const char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="region_t<",_tag_dyneither(
_tmp3F6,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpC0);{const char*_tmp3F7;
Cyc_Absyndump_dump(((_tmp3F7=">",_tag_dyneither(_tmp3F7,sizeof(char),2))));}
return;_LLB5: {struct Cyc_Absyn_DynRgnType_struct*_tmpC1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp64;if(_tmpC1->tag != 17)goto _LLB7;else{_tmpC2=(void*)_tmpC1->f1;_tmpC3=(void*)
_tmpC1->f2;}}_LLB6:{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="dynregion_t<",
_tag_dyneither(_tmp3F8,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpC2);{const
char*_tmp3F9;Cyc_Absyndump_dump(((_tmp3F9=",",_tag_dyneither(_tmp3F9,sizeof(char),
2))));}Cyc_Absyndump_dumprgn(_tmpC3);{const char*_tmp3FA;Cyc_Absyndump_dump(((
_tmp3FA=">",_tag_dyneither(_tmp3FA,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_struct*
_tmpC4=(struct Cyc_Absyn_TagType_struct*)_tmp64;if(_tmpC4->tag != 20)goto _LLB9;
else{_tmpC5=(void*)_tmpC4->f1;}}_LLB8:{const char*_tmp3FB;Cyc_Absyndump_dump(((
_tmp3FB="tag_t<",_tag_dyneither(_tmp3FB,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpC5);{const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC=">",_tag_dyneither(
_tmp3FC,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp64;if(_tmpC6->tag != 22)goto _LLBB;}
_LLBA:{const char*_tmp3FD;Cyc_Absyndump_dump(((_tmp3FD="`U",_tag_dyneither(
_tmp3FD,sizeof(char),3))));}goto _LL70;_LLBB: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp64;if(_tmpC7->tag != 21)goto _LLBD;}
_LLBC: goto _LLBE;_LLBD: {struct Cyc_Absyn_AccessEff_struct*_tmpC8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp64;if(_tmpC8->tag != 23)goto _LLBF;}_LLBE: goto _LLC0;_LLBF: {struct Cyc_Absyn_RgnsEff_struct*
_tmpC9=(struct Cyc_Absyn_RgnsEff_struct*)_tmp64;if(_tmpC9->tag != 25)goto _LLC1;}
_LLC0: goto _LLC2;_LLC1: {struct Cyc_Absyn_JoinEff_struct*_tmpCA=(struct Cyc_Absyn_JoinEff_struct*)
_tmp64;if(_tmpCA->tag != 24)goto _LL70;}_LLC2: return;_LL70:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo);void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple7*t);void Cyc_Absyndump_dumpfunarg(struct _tuple7*t){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple15{void*f1;void*f2;
};void Cyc_Absyndump_dump_rgncmp(struct _tuple15*cmp);void Cyc_Absyndump_dump_rgncmp(
struct _tuple15*cmp){struct _tuple15 _tmp10D;void*_tmp10E;void*_tmp10F;struct
_tuple15*_tmp10C=cmp;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;Cyc_Absyndump_dumpeff(
_tmp10E);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp10F);}void
Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){const char*_tmp3FE;((void(*)(void(*f)(struct _tuple15*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,((_tmp3FE=",",_tag_dyneither(_tmp3FE,sizeof(char),2))));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs){const char*_tmp3FF;Cyc_Absyndump_dump(((_tmp3FF="...",
_tag_dyneither(_tmp3FF,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple7*_tmp400;struct _tuple7*_tmp112=(_tmp400=_cycalloc(sizeof(*_tmp400)),((
_tmp400->f1=cyc_varargs->name,((_tmp400->f2=cyc_varargs->tq,((_tmp400->f3=
cyc_varargs->type,_tmp400)))))));{const char*_tmp401;Cyc_Absyndump_dump(((_tmp401="...",
_tag_dyneither(_tmp401,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp402;Cyc_Absyndump_dump(((_tmp402=" inject ",_tag_dyneither(_tmp402,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp112);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t);void Cyc_Absyndump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d);void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp116=d;struct Cyc_Absyn_Exp*
_tmp118;struct _dyneither_ptr*_tmp11A;_LLD4: {struct Cyc_Absyn_ArrayElement_struct*
_tmp117=(struct Cyc_Absyn_ArrayElement_struct*)_tmp116;if(_tmp117->tag != 0)goto
_LLD6;else{_tmp118=_tmp117->f1;}}_LLD5:{const char*_tmp403;Cyc_Absyndump_dump(((
_tmp403=".[",_tag_dyneither(_tmp403,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp118);Cyc_Absyndump_dump_char((int)']');goto _LLD3;_LLD6: {struct Cyc_Absyn_FieldName_struct*
_tmp119=(struct Cyc_Absyn_FieldName_struct*)_tmp116;if(_tmp119->tag != 1)goto _LLD3;
else{_tmp11A=_tmp119->f1;}}_LLD7: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp11A);goto _LLD3;_LLD3:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de);void Cyc_Absyndump_dumpde(struct
_tuple16*de){{const char*_tmp406;const char*_tmp405;const char*_tmp404;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
de).f1,((_tmp404="",_tag_dyneither(_tmp404,sizeof(char),1))),((_tmp405="=",
_tag_dyneither(_tmp405,sizeof(char),2))),((_tmp406="=",_tag_dyneither(_tmp406,
sizeof(char),2))));}Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es);void Cyc_Absyndump_dumpexps_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp409;const char*_tmp408;const char*_tmp407;((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp407="",_tag_dyneither(_tmp407,sizeof(
char),1))),((_tmp408="",_tag_dyneither(_tmp408,sizeof(char),1))),((_tmp409=",",
_tag_dyneither(_tmp409,sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int
inprec,struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){const char*_tmp40A;Cyc_Absyndump_dump_nospace(((
_tmp40A="(",_tag_dyneither(_tmp40A,sizeof(char),2))));}{void*_tmp123=e->r;union
Cyc_Absyn_Cnst _tmp125;struct _tuple3 _tmp126;enum Cyc_Absyn_Sign _tmp127;char
_tmp128;union Cyc_Absyn_Cnst _tmp12A;struct _tuple4 _tmp12B;enum Cyc_Absyn_Sign
_tmp12C;short _tmp12D;union Cyc_Absyn_Cnst _tmp12F;struct _tuple5 _tmp130;enum Cyc_Absyn_Sign
_tmp131;int _tmp132;union Cyc_Absyn_Cnst _tmp134;struct _tuple5 _tmp135;enum Cyc_Absyn_Sign
_tmp136;int _tmp137;union Cyc_Absyn_Cnst _tmp139;struct _tuple5 _tmp13A;enum Cyc_Absyn_Sign
_tmp13B;int _tmp13C;union Cyc_Absyn_Cnst _tmp13E;struct _tuple6 _tmp13F;enum Cyc_Absyn_Sign
_tmp140;long long _tmp141;union Cyc_Absyn_Cnst _tmp143;struct _dyneither_ptr _tmp144;
union Cyc_Absyn_Cnst _tmp146;int _tmp147;union Cyc_Absyn_Cnst _tmp149;struct
_dyneither_ptr _tmp14A;struct _tuple0*_tmp14C;struct _tuple0*_tmp14E;enum Cyc_Absyn_Primop
_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Core_Opt*
_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp157;enum Cyc_Absyn_Incrementor
_tmp158;struct Cyc_Absyn_Exp*_tmp15A;enum Cyc_Absyn_Incrementor _tmp15B;struct Cyc_Absyn_Exp*
_tmp15D;enum Cyc_Absyn_Incrementor _tmp15E;struct Cyc_Absyn_Exp*_tmp160;enum Cyc_Absyn_Incrementor
_tmp161;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*
_tmp165;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*
_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Exp*
_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*
_tmp178;struct Cyc_Absyn_Exp*_tmp17A;void*_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;
struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*
_tmp182;void*_tmp184;struct Cyc_Absyn_Exp*_tmp186;void*_tmp188;struct Cyc_Absyn_Exp*
_tmp18A;struct _dyneither_ptr*_tmp18B;void*_tmp18D;void*_tmp18E;struct
_dyneither_ptr*_tmp190;void*_tmp192;void*_tmp193;unsigned int _tmp195;struct Cyc_Absyn_Exp*
_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct _dyneither_ptr*_tmp19A;struct Cyc_Absyn_Exp*
_tmp19C;struct _dyneither_ptr*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*
_tmp1A0;struct Cyc_List_List*_tmp1A2;struct _tuple7*_tmp1A4;struct Cyc_List_List*
_tmp1A5;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_Vardecl*_tmp1A9;struct Cyc_Absyn_Exp*
_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct _tuple0*_tmp1AD;struct Cyc_List_List*
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct Cyc_List_List*
_tmp1B3;struct Cyc_Absyn_Datatypefield*_tmp1B4;struct _tuple0*_tmp1B6;struct
_tuple0*_tmp1B8;struct Cyc_Absyn_MallocInfo _tmp1BA;int _tmp1BB;struct Cyc_Absyn_Exp*
_tmp1BC;void**_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1C0;
struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Core_Opt*_tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_Absyn_Stmt*_tmp1C6;_LLD9: {struct Cyc_Absyn_Const_e_struct*
_tmp124=(struct Cyc_Absyn_Const_e_struct*)_tmp123;if(_tmp124->tag != 0)goto _LLDB;
else{_tmp125=_tmp124->f1;if((_tmp125.Char_c).tag != 2)goto _LLDB;_tmp126=(struct
_tuple3)(_tmp125.Char_c).val;_tmp127=_tmp126.f1;_tmp128=_tmp126.f2;}}_LLDA: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp128));Cyc_Absyndump_dump_char((
int)'\'');goto _LLD8;_LLDB: {struct Cyc_Absyn_Const_e_struct*_tmp129=(struct Cyc_Absyn_Const_e_struct*)
_tmp123;if(_tmp129->tag != 0)goto _LLDD;else{_tmp12A=_tmp129->f1;if((_tmp12A.Short_c).tag
!= 3)goto _LLDD;_tmp12B=(struct _tuple4)(_tmp12A.Short_c).val;_tmp12C=_tmp12B.f1;
_tmp12D=_tmp12B.f2;}}_LLDC:{const char*_tmp40E;void*_tmp40D[1];struct Cyc_Int_pa_struct
_tmp40C;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp40C.tag=1,((_tmp40C.f1=(
unsigned long)((int)_tmp12D),((_tmp40D[0]=& _tmp40C,Cyc_aprintf(((_tmp40E="%d",
_tag_dyneither(_tmp40E,sizeof(char),3))),_tag_dyneither(_tmp40D,sizeof(void*),1)))))))));}
goto _LLD8;_LLDD: {struct Cyc_Absyn_Const_e_struct*_tmp12E=(struct Cyc_Absyn_Const_e_struct*)
_tmp123;if(_tmp12E->tag != 0)goto _LLDF;else{_tmp12F=_tmp12E->f1;if((_tmp12F.Int_c).tag
!= 4)goto _LLDF;_tmp130=(struct _tuple5)(_tmp12F.Int_c).val;_tmp131=_tmp130.f1;if(
_tmp131 != Cyc_Absyn_None)goto _LLDF;_tmp132=_tmp130.f2;}}_LLDE: _tmp137=_tmp132;
goto _LLE0;_LLDF: {struct Cyc_Absyn_Const_e_struct*_tmp133=(struct Cyc_Absyn_Const_e_struct*)
_tmp123;if(_tmp133->tag != 0)goto _LLE1;else{_tmp134=_tmp133->f1;if((_tmp134.Int_c).tag
!= 4)goto _LLE1;_tmp135=(struct _tuple5)(_tmp134.Int_c).val;_tmp136=_tmp135.f1;if(
_tmp136 != Cyc_Absyn_Signed)goto _LLE1;_tmp137=_tmp135.f2;}}_LLE0:{const char*
_tmp412;void*_tmp411[1];struct Cyc_Int_pa_struct _tmp410;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp410.tag=1,((_tmp410.f1=(unsigned long)_tmp137,((
_tmp411[0]=& _tmp410,Cyc_aprintf(((_tmp412="%d",_tag_dyneither(_tmp412,sizeof(
char),3))),_tag_dyneither(_tmp411,sizeof(void*),1)))))))));}goto _LLD8;_LLE1: {
struct Cyc_Absyn_Const_e_struct*_tmp138=(struct Cyc_Absyn_Const_e_struct*)_tmp123;
if(_tmp138->tag != 0)goto _LLE3;else{_tmp139=_tmp138->f1;if((_tmp139.Int_c).tag != 
4)goto _LLE3;_tmp13A=(struct _tuple5)(_tmp139.Int_c).val;_tmp13B=_tmp13A.f1;if(
_tmp13B != Cyc_Absyn_Unsigned)goto _LLE3;_tmp13C=_tmp13A.f2;}}_LLE2:{const char*
_tmp416;void*_tmp415[1];struct Cyc_Int_pa_struct _tmp414;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp414.tag=1,((_tmp414.f1=(unsigned int)_tmp13C,((
_tmp415[0]=& _tmp414,Cyc_aprintf(((_tmp416="%u",_tag_dyneither(_tmp416,sizeof(
char),3))),_tag_dyneither(_tmp415,sizeof(void*),1)))))))));}goto _LLD8;_LLE3: {
struct Cyc_Absyn_Const_e_struct*_tmp13D=(struct Cyc_Absyn_Const_e_struct*)_tmp123;
if(_tmp13D->tag != 0)goto _LLE5;else{_tmp13E=_tmp13D->f1;if((_tmp13E.LongLong_c).tag
!= 5)goto _LLE5;_tmp13F=(struct _tuple6)(_tmp13E.LongLong_c).val;_tmp140=_tmp13F.f1;
_tmp141=_tmp13F.f2;}}_LLE4:{const char*_tmp417;Cyc_Absyndump_dump(((_tmp417="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp417,sizeof(char),27))));}goto _LLD8;_LLE5: {struct Cyc_Absyn_Const_e_struct*
_tmp142=(struct Cyc_Absyn_Const_e_struct*)_tmp123;if(_tmp142->tag != 0)goto _LLE7;
else{_tmp143=_tmp142->f1;if((_tmp143.Float_c).tag != 6)goto _LLE7;_tmp144=(struct
_dyneither_ptr)(_tmp143.Float_c).val;}}_LLE6: Cyc_Absyndump_dump(_tmp144);goto
_LLD8;_LLE7: {struct Cyc_Absyn_Const_e_struct*_tmp145=(struct Cyc_Absyn_Const_e_struct*)
_tmp123;if(_tmp145->tag != 0)goto _LLE9;else{_tmp146=_tmp145->f1;if((_tmp146.Null_c).tag
!= 1)goto _LLE9;_tmp147=(int)(_tmp146.Null_c).val;}}_LLE8:{const char*_tmp418;Cyc_Absyndump_dump(((
_tmp418="NULL",_tag_dyneither(_tmp418,sizeof(char),5))));}goto _LLD8;_LLE9: {
struct Cyc_Absyn_Const_e_struct*_tmp148=(struct Cyc_Absyn_Const_e_struct*)_tmp123;
if(_tmp148->tag != 0)goto _LLEB;else{_tmp149=_tmp148->f1;if((_tmp149.String_c).tag
!= 7)goto _LLEB;_tmp14A=(struct _dyneither_ptr)(_tmp149.String_c).val;}}_LLEA: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14A));Cyc_Absyndump_dump_char((
int)'"');goto _LLD8;_LLEB: {struct Cyc_Absyn_UnknownId_e_struct*_tmp14B=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp123;if(_tmp14B->tag != 2)goto _LLED;else{_tmp14C=_tmp14B->f1;}}_LLEC: _tmp14E=
_tmp14C;goto _LLEE;_LLED: {struct Cyc_Absyn_Var_e_struct*_tmp14D=(struct Cyc_Absyn_Var_e_struct*)
_tmp123;if(_tmp14D->tag != 1)goto _LLEF;else{_tmp14E=_tmp14D->f1;}}_LLEE: Cyc_Absyndump_dumpqvar(
_tmp14E);goto _LLD8;_LLEF: {struct Cyc_Absyn_Primop_e_struct*_tmp14F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp123;if(_tmp14F->tag != 3)goto _LLF1;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;}}
_LLF0: {struct _dyneither_ptr _tmp1D2=Cyc_Absynpp_prim2str(_tmp150);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp151)){case 1: _LL141: if(_tmp150 == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp419;Cyc_Absyndump_dump(((
_tmp419="numelts(",_tag_dyneither(_tmp419,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151))->hd);{const
char*_tmp41A;Cyc_Absyndump_dump(((_tmp41A=")",_tag_dyneither(_tmp41A,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1D2);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151))->hd);}break;
case 2: _LL142: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp151))->hd);Cyc_Absyndump_dump(_tmp1D2);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp151->tl))->hd);break;default: _LL143: {struct Cyc_Core_Failure_struct
_tmp420;const char*_tmp41F;struct Cyc_Core_Failure_struct*_tmp41E;(int)_throw((
void*)((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=((_tmp420.tag=Cyc_Core_Failure,((
_tmp420.f1=((_tmp41F="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp41F,sizeof(char),47))),_tmp420)))),_tmp41E)))));}}goto _LLD8;}
_LLF1: {struct Cyc_Absyn_AssignOp_e_struct*_tmp152=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp123;if(_tmp152->tag != 4)goto _LLF3;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;
_tmp155=_tmp152->f3;}}_LLF2: Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);if(
_tmp154 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp154->v));{const char*_tmp421;Cyc_Absyndump_dump_nospace(((_tmp421="=",
_tag_dyneither(_tmp421,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp155);goto _LLD8;_LLF3: {struct Cyc_Absyn_Increment_e_struct*_tmp156=(struct Cyc_Absyn_Increment_e_struct*)
_tmp123;if(_tmp156->tag != 5)goto _LLF5;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;
if(_tmp158 != Cyc_Absyn_PreInc)goto _LLF5;}}_LLF4:{const char*_tmp422;Cyc_Absyndump_dump(((
_tmp422="++",_tag_dyneither(_tmp422,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp157);goto _LLD8;_LLF5: {struct Cyc_Absyn_Increment_e_struct*_tmp159=(
struct Cyc_Absyn_Increment_e_struct*)_tmp123;if(_tmp159->tag != 5)goto _LLF7;else{
_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;if(_tmp15B != Cyc_Absyn_PreDec)goto _LLF7;}}
_LLF6:{const char*_tmp423;Cyc_Absyndump_dump(((_tmp423="--",_tag_dyneither(
_tmp423,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);goto _LLD8;
_LLF7: {struct Cyc_Absyn_Increment_e_struct*_tmp15C=(struct Cyc_Absyn_Increment_e_struct*)
_tmp123;if(_tmp15C->tag != 5)goto _LLF9;else{_tmp15D=_tmp15C->f1;_tmp15E=_tmp15C->f2;
if(_tmp15E != Cyc_Absyn_PostInc)goto _LLF9;}}_LLF8: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15D);{const char*_tmp424;Cyc_Absyndump_dump(((_tmp424="++",
_tag_dyneither(_tmp424,sizeof(char),3))));}goto _LLD8;_LLF9: {struct Cyc_Absyn_Increment_e_struct*
_tmp15F=(struct Cyc_Absyn_Increment_e_struct*)_tmp123;if(_tmp15F->tag != 5)goto
_LLFB;else{_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;if(_tmp161 != Cyc_Absyn_PostDec)
goto _LLFB;}}_LLFA: Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);{const char*_tmp425;
Cyc_Absyndump_dump(((_tmp425="--",_tag_dyneither(_tmp425,sizeof(char),3))));}
goto _LLD8;_LLFB: {struct Cyc_Absyn_Conditional_e_struct*_tmp162=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp123;if(_tmp162->tag != 6)goto _LLFD;else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;
_tmp165=_tmp162->f3;}}_LLFC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp163);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp164);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp165);goto _LLD8;_LLFD: {struct Cyc_Absyn_And_e_struct*
_tmp166=(struct Cyc_Absyn_And_e_struct*)_tmp123;if(_tmp166->tag != 7)goto _LLFF;
else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLFE: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp167);{const char*_tmp426;Cyc_Absyndump_dump(((_tmp426=" && ",
_tag_dyneither(_tmp426,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp168);goto _LLD8;_LLFF: {struct Cyc_Absyn_Or_e_struct*_tmp169=(struct Cyc_Absyn_Or_e_struct*)
_tmp123;if(_tmp169->tag != 8)goto _LL101;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}
_LL100: Cyc_Absyndump_dumpexp_prec(myprec,_tmp16A);{const char*_tmp427;Cyc_Absyndump_dump(((
_tmp427=" || ",_tag_dyneither(_tmp427,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16B);goto _LLD8;_LL101: {struct Cyc_Absyn_SeqExp_e_struct*_tmp16C=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp123;if(_tmp16C->tag != 9)goto _LL103;else{
_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LL102: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp16D);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16E);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL103: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp16F=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp123;if(_tmp16F->tag != 10)goto
_LL105;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}_LL104: _tmp173=_tmp170;
_tmp174=_tmp171;goto _LL106;_LL105: {struct Cyc_Absyn_FnCall_e_struct*_tmp172=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp123;if(_tmp172->tag != 11)goto _LL107;else{
_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LL106: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp173);{const char*_tmp428;Cyc_Absyndump_dump_nospace(((_tmp428="(",
_tag_dyneither(_tmp428,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp174);{const char*_tmp429;Cyc_Absyndump_dump_nospace(((_tmp429=")",
_tag_dyneither(_tmp429,sizeof(char),2))));}goto _LLD8;_LL107: {struct Cyc_Absyn_Throw_e_struct*
_tmp175=(struct Cyc_Absyn_Throw_e_struct*)_tmp123;if(_tmp175->tag != 12)goto _LL109;
else{_tmp176=_tmp175->f1;}}_LL108:{const char*_tmp42A;Cyc_Absyndump_dump(((
_tmp42A="throw",_tag_dyneither(_tmp42A,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp176);goto _LLD8;_LL109: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp177=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp123;if(_tmp177->tag != 13)
goto _LL10B;else{_tmp178=_tmp177->f1;}}_LL10A: _tmp17A=_tmp178;goto _LL10C;_LL10B: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp179=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp123;if(_tmp179->tag != 14)goto _LL10D;else{_tmp17A=_tmp179->f1;}}_LL10C: Cyc_Absyndump_dumpexp_prec(
inprec,_tmp17A);goto _LLD8;_LL10D: {struct Cyc_Absyn_Cast_e_struct*_tmp17B=(struct
Cyc_Absyn_Cast_e_struct*)_tmp123;if(_tmp17B->tag != 15)goto _LL10F;else{_tmp17C=(
void*)_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LL10E: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp(_tmp17C);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17D);goto _LLD8;_LL10F: {struct Cyc_Absyn_Address_e_struct*_tmp17E=(
struct Cyc_Absyn_Address_e_struct*)_tmp123;if(_tmp17E->tag != 16)goto _LL111;else{
_tmp17F=_tmp17E->f1;}}_LL110: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17F);goto _LLD8;_LL111: {struct Cyc_Absyn_New_e_struct*_tmp180=(struct
Cyc_Absyn_New_e_struct*)_tmp123;if(_tmp180->tag != 17)goto _LL113;else{_tmp181=
_tmp180->f1;_tmp182=_tmp180->f2;}}_LL112:{const char*_tmp42B;Cyc_Absyndump_dump(((
_tmp42B="new ",_tag_dyneither(_tmp42B,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp182);goto _LLD8;_LL113: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp183=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp123;if(_tmp183->tag != 18)goto _LL115;
else{_tmp184=(void*)_tmp183->f1;}}_LL114:{const char*_tmp42C;Cyc_Absyndump_dump(((
_tmp42C="sizeof(",_tag_dyneither(_tmp42C,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp184);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL115: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp185=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp123;if(_tmp185->tag != 19)goto
_LL117;else{_tmp186=_tmp185->f1;}}_LL116:{const char*_tmp42D;Cyc_Absyndump_dump(((
_tmp42D="sizeof(",_tag_dyneither(_tmp42D,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp186);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL117: {struct Cyc_Absyn_Valueof_e_struct*
_tmp187=(struct Cyc_Absyn_Valueof_e_struct*)_tmp123;if(_tmp187->tag != 39)goto
_LL119;else{_tmp188=(void*)_tmp187->f1;}}_LL118:{const char*_tmp42E;Cyc_Absyndump_dump(((
_tmp42E="valueof(",_tag_dyneither(_tmp42E,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp188);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL119: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp189=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp123;if(_tmp189->tag != 38)goto
_LL11B;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LL11A:{const char*_tmp42F;
Cyc_Absyndump_dump(((_tmp42F="tagcheck(",_tag_dyneither(_tmp42F,sizeof(char),10))));}
Cyc_Absyndump_dumpexp(_tmp18A);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp18B);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL11B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp18C=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp123;if(_tmp18C->tag != 20)goto
_LL11D;else{_tmp18D=(void*)_tmp18C->f1;_tmp18E=(void*)_tmp18C->f2;{struct Cyc_Absyn_StructField_struct*
_tmp18F=(struct Cyc_Absyn_StructField_struct*)_tmp18E;if(_tmp18F->tag != 0)goto
_LL11D;else{_tmp190=_tmp18F->f1;}};}}_LL11C:{const char*_tmp430;Cyc_Absyndump_dump(((
_tmp430="offsetof(",_tag_dyneither(_tmp430,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp18D);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp190);
Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL11D: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp191=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp123;if(_tmp191->tag != 20)goto
_LL11F;else{_tmp192=(void*)_tmp191->f1;_tmp193=(void*)_tmp191->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp194=(struct Cyc_Absyn_TupleIndex_struct*)_tmp193;if(_tmp194->tag != 1)goto
_LL11F;else{_tmp195=_tmp194->f1;}};}}_LL11E:{const char*_tmp431;Cyc_Absyndump_dump(((
_tmp431="offsetof(",_tag_dyneither(_tmp431,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp192);Cyc_Absyndump_dump_char((int)',');{const char*_tmp435;void*_tmp434[1];
struct Cyc_Int_pa_struct _tmp433;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp433.tag=1,((_tmp433.f1=(unsigned long)((int)_tmp195),((_tmp434[0]=& _tmp433,
Cyc_aprintf(((_tmp435="%d",_tag_dyneither(_tmp435,sizeof(char),3))),
_tag_dyneither(_tmp434,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLD8;_LL11F: {struct Cyc_Absyn_Deref_e_struct*_tmp196=(struct Cyc_Absyn_Deref_e_struct*)
_tmp123;if(_tmp196->tag != 21)goto _LL121;else{_tmp197=_tmp196->f1;}}_LL120: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp197);goto _LLD8;_LL121: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp198=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp123;if(_tmp198->tag != 22)goto
_LL123;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL122: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp199);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp19A);goto _LLD8;_LL123: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp19B=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp123;if(_tmp19B->tag != 23)goto _LL125;else{
_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL124: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp19C);{const char*_tmp436;Cyc_Absyndump_dump_nospace(((_tmp436="->",
_tag_dyneither(_tmp436,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp19D);
goto _LLD8;_LL125: {struct Cyc_Absyn_Subscript_e_struct*_tmp19E=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp123;if(_tmp19E->tag != 24)goto _LL127;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;}}
_LL126: Cyc_Absyndump_dumpexp_prec(myprec,_tmp19F);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp1A0);Cyc_Absyndump_dump_char((int)']');goto _LLD8;
_LL127: {struct Cyc_Absyn_Tuple_e_struct*_tmp1A1=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp123;if(_tmp1A1->tag != 25)goto _LL129;else{_tmp1A2=_tmp1A1->f1;}}_LL128:{const
char*_tmp437;Cyc_Absyndump_dump(((_tmp437="$(",_tag_dyneither(_tmp437,sizeof(
char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp1A2);Cyc_Absyndump_dump_char((
int)')');goto _LLD8;_LL129: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1A3=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp123;if(_tmp1A3->tag != 26)goto _LL12B;else{
_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;}}_LL12A: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp1A4).f3);Cyc_Absyndump_dump_char((int)')');{const
char*_tmp43A;const char*_tmp439;const char*_tmp438;((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A5,((
_tmp438="{",_tag_dyneither(_tmp438,sizeof(char),2))),((_tmp439="}",
_tag_dyneither(_tmp439,sizeof(char),2))),((_tmp43A=",",_tag_dyneither(_tmp43A,
sizeof(char),2))));}goto _LLD8;_LL12B: {struct Cyc_Absyn_Array_e_struct*_tmp1A6=(
struct Cyc_Absyn_Array_e_struct*)_tmp123;if(_tmp1A6->tag != 27)goto _LL12D;else{
_tmp1A7=_tmp1A6->f1;}}_LL12C:{const char*_tmp43D;const char*_tmp43C;const char*
_tmp43B;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1A7,((_tmp43B="{",_tag_dyneither(_tmp43B,sizeof(char),2))),((
_tmp43C="}",_tag_dyneither(_tmp43C,sizeof(char),2))),((_tmp43D=",",
_tag_dyneither(_tmp43D,sizeof(char),2))));}goto _LLD8;_LL12D: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1A8=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp123;if(_tmp1A8->tag != 28)
goto _LL12F;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;_tmp1AB=_tmp1A8->f3;}}
_LL12E:{const char*_tmp43E;Cyc_Absyndump_dump(((_tmp43E="new {for",_tag_dyneither(
_tmp43E,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1A9->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp1AA);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp1AB);Cyc_Absyndump_dump_char((int)'}');goto _LLD8;_LL12F: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1AC=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp123;if(_tmp1AC->tag != 29)goto
_LL131;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;_tmp1AF=_tmp1AC->f3;}}_LL130:
Cyc_Absyndump_dumpqvar(_tmp1AD);Cyc_Absyndump_dump_char((int)'{');if(_tmp1AE != 0)
Cyc_Absyndump_dumptps(_tmp1AE);{const char*_tmp441;const char*_tmp440;const char*
_tmp43F;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1AF,((_tmp43F="",_tag_dyneither(_tmp43F,sizeof(char),1))),((
_tmp440="}",_tag_dyneither(_tmp440,sizeof(char),2))),((_tmp441=",",
_tag_dyneither(_tmp441,sizeof(char),2))));}goto _LLD8;_LL131: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1B0=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp123;if(_tmp1B0->tag != 30)goto
_LL133;else{_tmp1B1=_tmp1B0->f2;}}_LL132:{const char*_tmp444;const char*_tmp443;
const char*_tmp442;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1B1,((_tmp442="{",_tag_dyneither(_tmp442,sizeof(char),2))),((
_tmp443="}",_tag_dyneither(_tmp443,sizeof(char),2))),((_tmp444=",",
_tag_dyneither(_tmp444,sizeof(char),2))));}goto _LLD8;_LL133: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1B2=(struct Cyc_Absyn_Datatype_e_struct*)_tmp123;if(_tmp1B2->tag != 31)goto
_LL135;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f3;}}_LL134: Cyc_Absyndump_dumpqvar(
_tmp1B4->name);if(_tmp1B3 != 0){const char*_tmp447;const char*_tmp446;const char*
_tmp445;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1B3,((_tmp445="(",_tag_dyneither(_tmp445,sizeof(char),
2))),((_tmp446=")",_tag_dyneither(_tmp446,sizeof(char),2))),((_tmp447=",",
_tag_dyneither(_tmp447,sizeof(char),2))));}goto _LLD8;_LL135: {struct Cyc_Absyn_Enum_e_struct*
_tmp1B5=(struct Cyc_Absyn_Enum_e_struct*)_tmp123;if(_tmp1B5->tag != 32)goto _LL137;
else{_tmp1B6=_tmp1B5->f1;}}_LL136: Cyc_Absyndump_dumpqvar(_tmp1B6);goto _LLD8;
_LL137: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1B7=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp123;if(_tmp1B7->tag != 33)goto _LL139;else{_tmp1B8=_tmp1B7->f1;}}_LL138: Cyc_Absyndump_dumpqvar(
_tmp1B8);goto _LLD8;_LL139: {struct Cyc_Absyn_Malloc_e_struct*_tmp1B9=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp123;if(_tmp1B9->tag != 34)goto _LL13B;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1BA.is_calloc;
_tmp1BC=_tmp1BA.rgn;_tmp1BD=_tmp1BA.elt_type;_tmp1BE=_tmp1BA.num_elts;}}_LL13A:
if(_tmp1BB){if(_tmp1BC != 0){{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448="rcalloc(",
_tag_dyneither(_tmp448,sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1BC);{const char*_tmp449;Cyc_Absyndump_dump(((_tmp449=",",_tag_dyneither(
_tmp449,sizeof(char),2))));};}else{const char*_tmp44A;Cyc_Absyndump_dump(((
_tmp44A="calloc",_tag_dyneither(_tmp44A,sizeof(char),7))));}Cyc_Absyndump_dumpexp(
_tmp1BE);{const char*_tmp44B;Cyc_Absyndump_dump(((_tmp44B=",",_tag_dyneither(
_tmp44B,sizeof(char),2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp1BD)),0));{const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C=")",
_tag_dyneither(_tmp44C,sizeof(char),2))));};}else{if(_tmp1BC != 0){{const char*
_tmp44D;Cyc_Absyndump_dump(((_tmp44D="rmalloc(",_tag_dyneither(_tmp44D,sizeof(
char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1BC);{const char*
_tmp44E;Cyc_Absyndump_dump(((_tmp44E=",",_tag_dyneither(_tmp44E,sizeof(char),2))));};}
else{const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F="malloc(",_tag_dyneither(
_tmp44F,sizeof(char),8))));}if(_tmp1BD != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1BD,0),_tmp1BE,0));else{Cyc_Absyndump_dumpexp(
_tmp1BE);}{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450=")",_tag_dyneither(
_tmp450,sizeof(char),2))));};}goto _LLD8;_LL13B: {struct Cyc_Absyn_Swap_e_struct*
_tmp1BF=(struct Cyc_Absyn_Swap_e_struct*)_tmp123;if(_tmp1BF->tag != 35)goto _LL13D;
else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;}}_LL13C:{const char*_tmp451;Cyc_Absyndump_dump(((
_tmp451="cycswap(",_tag_dyneither(_tmp451,sizeof(char),9))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1C0);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1C1);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL13D: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1C2=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp123;if(_tmp1C2->tag != 36)
goto _LL13F;else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL13E:{const char*
_tmp454;const char*_tmp453;const char*_tmp452;((void(*)(void(*f)(struct _tuple16*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1C4,((_tmp452="{",
_tag_dyneither(_tmp452,sizeof(char),2))),((_tmp453="}",_tag_dyneither(_tmp453,
sizeof(char),2))),((_tmp454=",",_tag_dyneither(_tmp454,sizeof(char),2))));}goto
_LLD8;_LL13F: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1C5=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp123;if(_tmp1C5->tag != 37)goto _LLD8;else{_tmp1C6=_tmp1C5->f1;}}_LL140:{const
char*_tmp455;Cyc_Absyndump_dump_nospace(((_tmp455="({",_tag_dyneither(_tmp455,
sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1C6,1);{const char*_tmp456;Cyc_Absyndump_dump_nospace(((
_tmp456="})",_tag_dyneither(_tmp456,sizeof(char),3))));}goto _LLD8;_LLD8:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(
0,e);}void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs);void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp20D=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp20D->where_clause == 0  && (
_tmp20D->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp457;Cyc_Absyndump_dump(((
_tmp457="default:",_tag_dyneither(_tmp457,sizeof(char),9))));}else{{const char*
_tmp458;Cyc_Absyndump_dump(((_tmp458="case",_tag_dyneither(_tmp458,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp20D->pattern);if(_tmp20D->where_clause != 0){{
const char*_tmp459;Cyc_Absyndump_dump(((_tmp459="&&",_tag_dyneither(_tmp459,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp20D->where_clause));}{const char*_tmp45A;Cyc_Absyndump_dump_nospace(((_tmp45A=":",
_tag_dyneither(_tmp45A,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp20D->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt);void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp212=s->r;struct Cyc_Absyn_Exp*_tmp215;
struct Cyc_Absyn_Stmt*_tmp217;struct Cyc_Absyn_Stmt*_tmp218;struct Cyc_Absyn_Exp*
_tmp21A;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Stmt*
_tmp21F;struct Cyc_Absyn_Stmt*_tmp220;struct _tuple8 _tmp222;struct Cyc_Absyn_Exp*
_tmp223;struct Cyc_Absyn_Stmt*_tmp224;struct _dyneither_ptr*_tmp228;struct Cyc_Absyn_Exp*
_tmp22A;struct _tuple8 _tmp22B;struct Cyc_Absyn_Exp*_tmp22C;struct _tuple8 _tmp22D;
struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Stmt*_tmp22F;struct Cyc_Absyn_Exp*
_tmp231;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Decl*_tmp234;struct Cyc_Absyn_Stmt*
_tmp235;struct _dyneither_ptr*_tmp237;struct Cyc_Absyn_Stmt*_tmp238;struct Cyc_Absyn_Stmt*
_tmp23A;struct _tuple8 _tmp23B;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_List_List*
_tmp23E;struct Cyc_List_List*_tmp240;struct Cyc_Absyn_Stmt*_tmp242;struct Cyc_List_List*
_tmp243;struct Cyc_Absyn_Exp*_tmp245;_LL146: {struct Cyc_Absyn_Skip_s_struct*
_tmp213=(struct Cyc_Absyn_Skip_s_struct*)_tmp212;if(_tmp213->tag != 0)goto _LL148;}
_LL147: Cyc_Absyndump_dump_semi();goto _LL145;_LL148: {struct Cyc_Absyn_Exp_s_struct*
_tmp214=(struct Cyc_Absyn_Exp_s_struct*)_tmp212;if(_tmp214->tag != 1)goto _LL14A;
else{_tmp215=_tmp214->f1;}}_LL149: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(
_tmp215);Cyc_Absyndump_dump_semi();goto _LL145;_LL14A: {struct Cyc_Absyn_Seq_s_struct*
_tmp216=(struct Cyc_Absyn_Seq_s_struct*)_tmp212;if(_tmp216->tag != 2)goto _LL14C;
else{_tmp217=_tmp216->f1;_tmp218=_tmp216->f2;}}_LL14B: if(Cyc_Absynpp_is_declaration(
_tmp217)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp217,0);
Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp217,0);}if(
Cyc_Absynpp_is_declaration(_tmp218)){if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp218,expstmt);Cyc_Absyndump_dump_char((
int)'}');if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dumpstmt(_tmp218,expstmt);}goto _LL145;_LL14C: {struct Cyc_Absyn_Return_s_struct*
_tmp219=(struct Cyc_Absyn_Return_s_struct*)_tmp212;if(_tmp219->tag != 3)goto _LL14E;
else{_tmp21A=_tmp219->f1;if(_tmp21A != 0)goto _LL14E;}}_LL14D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp45B;Cyc_Absyndump_dump(((_tmp45B="return;",_tag_dyneither(
_tmp45B,sizeof(char),8))));}goto _LL145;_LL14E: {struct Cyc_Absyn_Return_s_struct*
_tmp21B=(struct Cyc_Absyn_Return_s_struct*)_tmp212;if(_tmp21B->tag != 3)goto _LL150;
else{_tmp21C=_tmp21B->f1;}}_LL14F: Cyc_Absyndump_dumploc(s->loc);{const char*
_tmp45C;Cyc_Absyndump_dump(((_tmp45C="return",_tag_dyneither(_tmp45C,sizeof(char),
7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp21C));Cyc_Absyndump_dump_semi();
goto _LL145;_LL150: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp21D=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp212;if(_tmp21D->tag != 4)goto _LL152;else{_tmp21E=_tmp21D->f1;_tmp21F=_tmp21D->f2;
_tmp220=_tmp21D->f3;}}_LL151: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp45D;
Cyc_Absyndump_dump(((_tmp45D="if(",_tag_dyneither(_tmp45D,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp21E);{void*_tmp249=_tmp21F->r;_LL16D: {struct Cyc_Absyn_Seq_s_struct*
_tmp24A=(struct Cyc_Absyn_Seq_s_struct*)_tmp249;if(_tmp24A->tag != 2)goto _LL16F;}
_LL16E: goto _LL170;_LL16F: {struct Cyc_Absyn_Decl_s_struct*_tmp24B=(struct Cyc_Absyn_Decl_s_struct*)
_tmp249;if(_tmp24B->tag != 12)goto _LL171;}_LL170: goto _LL172;_LL171: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp24C=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp249;if(_tmp24C->tag != 4)goto
_LL173;}_LL172: goto _LL174;_LL173: {struct Cyc_Absyn_Label_s_struct*_tmp24D=(
struct Cyc_Absyn_Label_s_struct*)_tmp249;if(_tmp24D->tag != 13)goto _LL175;}_LL174:{
const char*_tmp45E;Cyc_Absyndump_dump_nospace(((_tmp45E="){",_tag_dyneither(
_tmp45E,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp21F,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL16C;_LL175:;_LL176: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp21F,0);_LL16C:;}{void*_tmp24F=_tmp220->r;_LL178: {struct Cyc_Absyn_Skip_s_struct*
_tmp250=(struct Cyc_Absyn_Skip_s_struct*)_tmp24F;if(_tmp250->tag != 0)goto _LL17A;}
_LL179: goto _LL177;_LL17A:;_LL17B:{const char*_tmp45F;Cyc_Absyndump_dump(((_tmp45F="else{",
_tag_dyneither(_tmp45F,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp220,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL177;_LL177:;}goto _LL145;_LL152: {struct Cyc_Absyn_While_s_struct*
_tmp221=(struct Cyc_Absyn_While_s_struct*)_tmp212;if(_tmp221->tag != 5)goto _LL154;
else{_tmp222=_tmp221->f1;_tmp223=_tmp222.f1;_tmp224=_tmp221->f2;}}_LL153: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp460;Cyc_Absyndump_dump(((_tmp460="while(",_tag_dyneither(
_tmp460,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp223);{const char*_tmp461;
Cyc_Absyndump_dump_nospace(((_tmp461="){",_tag_dyneither(_tmp461,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp224,0);Cyc_Absyndump_dump_char((int)'}');goto _LL145;
_LL154: {struct Cyc_Absyn_Break_s_struct*_tmp225=(struct Cyc_Absyn_Break_s_struct*)
_tmp212;if(_tmp225->tag != 6)goto _LL156;}_LL155: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp462;Cyc_Absyndump_dump(((_tmp462="break;",_tag_dyneither(_tmp462,
sizeof(char),7))));}goto _LL145;_LL156: {struct Cyc_Absyn_Continue_s_struct*
_tmp226=(struct Cyc_Absyn_Continue_s_struct*)_tmp212;if(_tmp226->tag != 7)goto
_LL158;}_LL157: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp463;Cyc_Absyndump_dump(((
_tmp463="continue;",_tag_dyneither(_tmp463,sizeof(char),10))));}goto _LL145;
_LL158: {struct Cyc_Absyn_Goto_s_struct*_tmp227=(struct Cyc_Absyn_Goto_s_struct*)
_tmp212;if(_tmp227->tag != 8)goto _LL15A;else{_tmp228=_tmp227->f1;}}_LL159: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp464;Cyc_Absyndump_dump(((_tmp464="goto",_tag_dyneither(
_tmp464,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp228);Cyc_Absyndump_dump_semi();
goto _LL145;_LL15A: {struct Cyc_Absyn_For_s_struct*_tmp229=(struct Cyc_Absyn_For_s_struct*)
_tmp212;if(_tmp229->tag != 9)goto _LL15C;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;
_tmp22C=_tmp22B.f1;_tmp22D=_tmp229->f3;_tmp22E=_tmp22D.f1;_tmp22F=_tmp229->f4;}}
_LL15B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp465;Cyc_Absyndump_dump(((
_tmp465="for(",_tag_dyneither(_tmp465,sizeof(char),5))));}Cyc_Absyndump_dumpexp(
_tmp22A);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp22C);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp22E);{const char*_tmp466;Cyc_Absyndump_dump_nospace(((
_tmp466="){",_tag_dyneither(_tmp466,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp22F,0);Cyc_Absyndump_dump_char((int)'}');goto _LL145;_LL15C: {struct Cyc_Absyn_Switch_s_struct*
_tmp230=(struct Cyc_Absyn_Switch_s_struct*)_tmp212;if(_tmp230->tag != 10)goto
_LL15E;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp467;Cyc_Absyndump_dump(((_tmp467="switch(",_tag_dyneither(
_tmp467,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp231);{const char*_tmp468;
Cyc_Absyndump_dump_nospace(((_tmp468="){",_tag_dyneither(_tmp468,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp232);Cyc_Absyndump_dump_char((int)'}');goto
_LL145;_LL15E: {struct Cyc_Absyn_Decl_s_struct*_tmp233=(struct Cyc_Absyn_Decl_s_struct*)
_tmp212;if(_tmp233->tag != 12)goto _LL160;else{_tmp234=_tmp233->f1;_tmp235=_tmp233->f2;}}
_LL15F: Cyc_Absyndump_dumpdecl(_tmp234);Cyc_Absyndump_dumpstmt(_tmp235,expstmt);
goto _LL145;_LL160: {struct Cyc_Absyn_Label_s_struct*_tmp236=(struct Cyc_Absyn_Label_s_struct*)
_tmp212;if(_tmp236->tag != 13)goto _LL162;else{_tmp237=_tmp236->f1;_tmp238=_tmp236->f2;}}
_LL161: if(Cyc_Absynpp_is_declaration(_tmp238)){Cyc_Absyndump_dump_str(_tmp237);
if(expstmt){const char*_tmp469;Cyc_Absyndump_dump_nospace(((_tmp469=": ({",
_tag_dyneither(_tmp469,sizeof(char),5))));}else{const char*_tmp46A;Cyc_Absyndump_dump_nospace(((
_tmp46A=": {",_tag_dyneither(_tmp46A,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp238,expstmt);if(expstmt){const char*_tmp46B;Cyc_Absyndump_dump_nospace(((
_tmp46B="});}",_tag_dyneither(_tmp46B,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp237);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp238,expstmt);}goto _LL145;_LL162: {struct Cyc_Absyn_Do_s_struct*
_tmp239=(struct Cyc_Absyn_Do_s_struct*)_tmp212;if(_tmp239->tag != 14)goto _LL164;
else{_tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;_tmp23C=_tmp23B.f1;}}_LL163: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C="do{",_tag_dyneither(
_tmp46C,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp23A,0);{const char*_tmp46D;
Cyc_Absyndump_dump_nospace(((_tmp46D="}while(",_tag_dyneither(_tmp46D,sizeof(
char),8))));}Cyc_Absyndump_dumpexp(_tmp23C);{const char*_tmp46E;Cyc_Absyndump_dump_nospace(((
_tmp46E=");",_tag_dyneither(_tmp46E,sizeof(char),3))));}goto _LL145;_LL164: {
struct Cyc_Absyn_Fallthru_s_struct*_tmp23D=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp212;if(_tmp23D->tag != 11)goto _LL166;else{_tmp23E=_tmp23D->f1;if(_tmp23E != 0)
goto _LL166;}}_LL165:{const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F="fallthru;",
_tag_dyneither(_tmp46F,sizeof(char),10))));}goto _LL145;_LL166: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp23F=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp212;if(_tmp23F->tag != 11)goto
_LL168;else{_tmp240=_tmp23F->f1;}}_LL167:{const char*_tmp470;Cyc_Absyndump_dump(((
_tmp470="fallthru(",_tag_dyneither(_tmp470,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp240);{const char*_tmp471;Cyc_Absyndump_dump_nospace(((_tmp471=");",
_tag_dyneither(_tmp471,sizeof(char),3))));}goto _LL145;_LL168: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp241=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp212;if(_tmp241->tag != 15)goto
_LL16A;else{_tmp242=_tmp241->f1;_tmp243=_tmp241->f2;}}_LL169: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp472;Cyc_Absyndump_dump(((_tmp472="try",_tag_dyneither(
_tmp472,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp242,0);{const char*_tmp473;
Cyc_Absyndump_dump(((_tmp473="catch{",_tag_dyneither(_tmp473,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp243);Cyc_Absyndump_dump_char((int)'}');goto
_LL145;_LL16A: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp244=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp212;if(_tmp244->tag != 16)goto _LL145;else{_tmp245=_tmp244->f1;}}_LL16B: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp474;Cyc_Absyndump_dump(((_tmp474="reset_region(",
_tag_dyneither(_tmp474,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp245);{
const char*_tmp475;Cyc_Absyndump_dump(((_tmp475=");",_tag_dyneither(_tmp475,
sizeof(char),3))));}goto _LL145;_LL145:;}struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple17*dp);void Cyc_Absyndump_dumpdp(
struct _tuple17*dp){{const char*_tmp478;const char*_tmp477;const char*_tmp476;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
dp).f1,((_tmp476="",_tag_dyneither(_tmp476,sizeof(char),1))),((_tmp477="=",
_tag_dyneither(_tmp477,sizeof(char),2))),((_tmp478="=",_tag_dyneither(_tmp478,
sizeof(char),2))));}Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*
_tmp26B=p->r;enum Cyc_Absyn_Sign _tmp26F;int _tmp270;enum Cyc_Absyn_Sign _tmp272;
int _tmp273;enum Cyc_Absyn_Sign _tmp275;int _tmp276;char _tmp278;struct
_dyneither_ptr _tmp27A;struct Cyc_Absyn_Vardecl*_tmp27C;struct Cyc_Absyn_Pat*
_tmp27D;struct Cyc_Absyn_Pat _tmp27E;void*_tmp27F;struct Cyc_Absyn_Vardecl*_tmp282;
struct Cyc_Absyn_Pat*_tmp283;struct Cyc_List_List*_tmp285;int _tmp286;struct Cyc_Absyn_Pat*
_tmp288;struct Cyc_Absyn_Vardecl*_tmp28A;struct Cyc_Absyn_Pat*_tmp28B;struct Cyc_Absyn_Pat
_tmp28C;void*_tmp28D;struct Cyc_Absyn_Vardecl*_tmp290;struct Cyc_Absyn_Pat*_tmp291;
struct Cyc_Absyn_Tvar*_tmp293;struct Cyc_Absyn_Vardecl*_tmp294;struct _tuple0*
_tmp296;struct _tuple0*_tmp298;struct Cyc_List_List*_tmp299;int _tmp29A;struct Cyc_Absyn_AggrInfo*
_tmp29C;struct Cyc_Absyn_AggrInfo _tmp29D;union Cyc_Absyn_AggrInfoU _tmp29E;struct
Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp2A0;int _tmp2A1;struct Cyc_Absyn_AggrInfo*
_tmp2A3;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*_tmp2A5;int _tmp2A6;
struct Cyc_Absyn_Datatypefield*_tmp2A8;struct Cyc_List_List*_tmp2A9;int _tmp2AA;
struct Cyc_Absyn_Enumfield*_tmp2AC;struct Cyc_Absyn_Enumfield*_tmp2AE;struct Cyc_Absyn_Exp*
_tmp2B0;_LL17D: {struct Cyc_Absyn_Wild_p_struct*_tmp26C=(struct Cyc_Absyn_Wild_p_struct*)
_tmp26B;if(_tmp26C->tag != 0)goto _LL17F;}_LL17E: Cyc_Absyndump_dump_char((int)'_');
goto _LL17C;_LL17F: {struct Cyc_Absyn_Null_p_struct*_tmp26D=(struct Cyc_Absyn_Null_p_struct*)
_tmp26B;if(_tmp26D->tag != 8)goto _LL181;}_LL180:{const char*_tmp479;Cyc_Absyndump_dump(((
_tmp479="NULL",_tag_dyneither(_tmp479,sizeof(char),5))));}goto _LL17C;_LL181: {
struct Cyc_Absyn_Int_p_struct*_tmp26E=(struct Cyc_Absyn_Int_p_struct*)_tmp26B;if(
_tmp26E->tag != 9)goto _LL183;else{_tmp26F=_tmp26E->f1;if(_tmp26F != Cyc_Absyn_None)
goto _LL183;_tmp270=_tmp26E->f2;}}_LL182: _tmp273=_tmp270;goto _LL184;_LL183: {
struct Cyc_Absyn_Int_p_struct*_tmp271=(struct Cyc_Absyn_Int_p_struct*)_tmp26B;if(
_tmp271->tag != 9)goto _LL185;else{_tmp272=_tmp271->f1;if(_tmp272 != Cyc_Absyn_Signed)
goto _LL185;_tmp273=_tmp271->f2;}}_LL184:{const char*_tmp47D;void*_tmp47C[1];
struct Cyc_Int_pa_struct _tmp47B;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp47B.tag=1,((_tmp47B.f1=(unsigned long)_tmp273,((_tmp47C[0]=& _tmp47B,Cyc_aprintf(((
_tmp47D="%d",_tag_dyneither(_tmp47D,sizeof(char),3))),_tag_dyneither(_tmp47C,
sizeof(void*),1)))))))));}goto _LL17C;_LL185: {struct Cyc_Absyn_Int_p_struct*
_tmp274=(struct Cyc_Absyn_Int_p_struct*)_tmp26B;if(_tmp274->tag != 9)goto _LL187;
else{_tmp275=_tmp274->f1;if(_tmp275 != Cyc_Absyn_Unsigned)goto _LL187;_tmp276=
_tmp274->f2;}}_LL186:{const char*_tmp481;void*_tmp480[1];struct Cyc_Int_pa_struct
_tmp47F;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp47F.tag=1,((_tmp47F.f1=(
unsigned int)_tmp276,((_tmp480[0]=& _tmp47F,Cyc_aprintf(((_tmp481="%u",
_tag_dyneither(_tmp481,sizeof(char),3))),_tag_dyneither(_tmp480,sizeof(void*),1)))))))));}
goto _LL17C;_LL187: {struct Cyc_Absyn_Char_p_struct*_tmp277=(struct Cyc_Absyn_Char_p_struct*)
_tmp26B;if(_tmp277->tag != 10)goto _LL189;else{_tmp278=_tmp277->f1;}}_LL188:{const
char*_tmp482;Cyc_Absyndump_dump(((_tmp482="'",_tag_dyneither(_tmp482,sizeof(char),
2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp278));{const char*
_tmp483;Cyc_Absyndump_dump_nospace(((_tmp483="'",_tag_dyneither(_tmp483,sizeof(
char),2))));}goto _LL17C;_LL189: {struct Cyc_Absyn_Float_p_struct*_tmp279=(struct
Cyc_Absyn_Float_p_struct*)_tmp26B;if(_tmp279->tag != 11)goto _LL18B;else{_tmp27A=
_tmp279->f1;}}_LL18A: Cyc_Absyndump_dump(_tmp27A);goto _LL17C;_LL18B: {struct Cyc_Absyn_Var_p_struct*
_tmp27B=(struct Cyc_Absyn_Var_p_struct*)_tmp26B;if(_tmp27B->tag != 1)goto _LL18D;
else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;_tmp27E=*_tmp27D;_tmp27F=_tmp27E.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp280=(struct Cyc_Absyn_Wild_p_struct*)_tmp27F;
if(_tmp280->tag != 0)goto _LL18D;};}}_LL18C: Cyc_Absyndump_dumpqvar(_tmp27C->name);
goto _LL17C;_LL18D: {struct Cyc_Absyn_Var_p_struct*_tmp281=(struct Cyc_Absyn_Var_p_struct*)
_tmp26B;if(_tmp281->tag != 1)goto _LL18F;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}
_LL18E: Cyc_Absyndump_dumpqvar(_tmp282->name);{const char*_tmp484;Cyc_Absyndump_dump(((
_tmp484=" as ",_tag_dyneither(_tmp484,sizeof(char),5))));}Cyc_Absyndump_dumppat(
_tmp283);goto _LL17C;_LL18F: {struct Cyc_Absyn_Tuple_p_struct*_tmp284=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp26B;if(_tmp284->tag != 4)goto _LL191;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}
_LL190: {const char*_tmp486;const char*_tmp485;struct _dyneither_ptr term=_tmp286?(
_tmp486=", ...)",_tag_dyneither(_tmp486,sizeof(char),7)):((_tmp485=")",
_tag_dyneither(_tmp485,sizeof(char),2)));{const char*_tmp488;const char*_tmp487;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp285,((_tmp487="$(",_tag_dyneither(_tmp487,sizeof(char),
3))),term,((_tmp488=",",_tag_dyneither(_tmp488,sizeof(char),2))));}goto _LL17C;}
_LL191: {struct Cyc_Absyn_Pointer_p_struct*_tmp287=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp26B;if(_tmp287->tag != 5)goto _LL193;else{_tmp288=_tmp287->f1;}}_LL192:{const
char*_tmp489;Cyc_Absyndump_dump(((_tmp489="&",_tag_dyneither(_tmp489,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp288);goto _LL17C;_LL193: {struct Cyc_Absyn_Reference_p_struct*
_tmp289=(struct Cyc_Absyn_Reference_p_struct*)_tmp26B;if(_tmp289->tag != 2)goto
_LL195;else{_tmp28A=_tmp289->f1;_tmp28B=_tmp289->f2;_tmp28C=*_tmp28B;_tmp28D=
_tmp28C.r;{struct Cyc_Absyn_Wild_p_struct*_tmp28E=(struct Cyc_Absyn_Wild_p_struct*)
_tmp28D;if(_tmp28E->tag != 0)goto _LL195;};}}_LL194:{const char*_tmp48A;Cyc_Absyndump_dump(((
_tmp48A="*",_tag_dyneither(_tmp48A,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp28A->name);goto _LL17C;_LL195: {struct Cyc_Absyn_Reference_p_struct*_tmp28F=(
struct Cyc_Absyn_Reference_p_struct*)_tmp26B;if(_tmp28F->tag != 2)goto _LL197;else{
_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL196:{const char*_tmp48B;Cyc_Absyndump_dump(((
_tmp48B="*",_tag_dyneither(_tmp48B,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp290->name);{const char*_tmp48C;Cyc_Absyndump_dump(((_tmp48C=" as ",
_tag_dyneither(_tmp48C,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp291);goto
_LL17C;_LL197: {struct Cyc_Absyn_TagInt_p_struct*_tmp292=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp26B;if(_tmp292->tag != 3)goto _LL199;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}
_LL198: Cyc_Absyndump_dumpqvar(_tmp294->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp293);Cyc_Absyndump_dump_char((int)'>');goto _LL17C;
_LL199: {struct Cyc_Absyn_UnknownId_p_struct*_tmp295=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp26B;if(_tmp295->tag != 14)goto _LL19B;else{_tmp296=_tmp295->f1;}}_LL19A: Cyc_Absyndump_dumpqvar(
_tmp296);goto _LL17C;_LL19B: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp297=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp26B;if(_tmp297->tag != 15)goto _LL19D;
else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;_tmp29A=_tmp297->f3;}}_LL19C: {
const char*_tmp48E;const char*_tmp48D;struct _dyneither_ptr term=_tmp29A?(_tmp48E=", ...)",
_tag_dyneither(_tmp48E,sizeof(char),7)):((_tmp48D=")",_tag_dyneither(_tmp48D,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp298);{const char*_tmp490;const char*
_tmp48F;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp299,((_tmp48F="(",_tag_dyneither(_tmp48F,sizeof(char),
2))),term,((_tmp490=",",_tag_dyneither(_tmp490,sizeof(char),2))));}goto _LL17C;}
_LL19D: {struct Cyc_Absyn_Aggr_p_struct*_tmp29B=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp26B;if(_tmp29B->tag != 6)goto _LL19F;else{_tmp29C=_tmp29B->f1;if(_tmp29C == 0)
goto _LL19F;_tmp29D=*_tmp29C;_tmp29E=_tmp29D.aggr_info;_tmp29F=_tmp29B->f2;
_tmp2A0=_tmp29B->f3;_tmp2A1=_tmp29B->f4;}}_LL19E: {struct _tuple0*_tmp2C8;struct
_tuple9 _tmp2C7=Cyc_Absyn_aggr_kinded_name(_tmp29E);_tmp2C8=_tmp2C7.f2;{const char*
_tmp492;const char*_tmp491;struct _dyneither_ptr term=_tmp2A1?(_tmp492=", ...)",
_tag_dyneither(_tmp492,sizeof(char),7)):((_tmp491=")",_tag_dyneither(_tmp491,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2C8);Cyc_Absyndump_dump_char((int)'{');{
const char*_tmp495;const char*_tmp494;const char*_tmp493;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp29F,((_tmp493="[",_tag_dyneither(_tmp493,sizeof(char),2))),((_tmp494="]",
_tag_dyneither(_tmp494,sizeof(char),2))),((_tmp495=",",_tag_dyneither(_tmp495,
sizeof(char),2))));}{const char*_tmp497;const char*_tmp496;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2A0,((_tmp496="",_tag_dyneither(_tmp496,sizeof(char),1))),term,((_tmp497=",",
_tag_dyneither(_tmp497,sizeof(char),2))));}goto _LL17C;};}_LL19F: {struct Cyc_Absyn_Aggr_p_struct*
_tmp2A2=(struct Cyc_Absyn_Aggr_p_struct*)_tmp26B;if(_tmp2A2->tag != 6)goto _LL1A1;
else{_tmp2A3=_tmp2A2->f1;if(_tmp2A3 != 0)goto _LL1A1;_tmp2A4=_tmp2A2->f2;_tmp2A5=
_tmp2A2->f3;_tmp2A6=_tmp2A2->f4;}}_LL1A0: {const char*_tmp499;const char*_tmp498;
struct _dyneither_ptr term=_tmp2A6?(_tmp499=", ...)",_tag_dyneither(_tmp499,
sizeof(char),7)):((_tmp498=")",_tag_dyneither(_tmp498,sizeof(char),2)));Cyc_Absyndump_dump_char((
int)'{');{const char*_tmp49C;const char*_tmp49B;const char*_tmp49A;((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2A4,((_tmp49A="[",_tag_dyneither(_tmp49A,sizeof(char),2))),((_tmp49B="]",
_tag_dyneither(_tmp49B,sizeof(char),2))),((_tmp49C=",",_tag_dyneither(_tmp49C,
sizeof(char),2))));}{const char*_tmp49E;const char*_tmp49D;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2A5,((_tmp49D="",_tag_dyneither(_tmp49D,sizeof(char),1))),term,((_tmp49E=",",
_tag_dyneither(_tmp49E,sizeof(char),2))));}goto _LL17C;}_LL1A1: {struct Cyc_Absyn_Datatype_p_struct*
_tmp2A7=(struct Cyc_Absyn_Datatype_p_struct*)_tmp26B;if(_tmp2A7->tag != 7)goto
_LL1A3;else{_tmp2A8=_tmp2A7->f2;_tmp2A9=_tmp2A7->f3;_tmp2AA=_tmp2A7->f4;}}_LL1A2: {
const char*_tmp4A0;const char*_tmp49F;struct _dyneither_ptr term=_tmp2AA?(_tmp4A0=", ...)",
_tag_dyneither(_tmp4A0,sizeof(char),7)):((_tmp49F=")",_tag_dyneither(_tmp49F,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2A8->name);if(_tmp2A9 != 0){const
char*_tmp4A2;const char*_tmp4A1;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2A9,((_tmp4A1="(",
_tag_dyneither(_tmp4A1,sizeof(char),2))),term,((_tmp4A2=",",_tag_dyneither(
_tmp4A2,sizeof(char),2))));}goto _LL17C;}_LL1A3: {struct Cyc_Absyn_Enum_p_struct*
_tmp2AB=(struct Cyc_Absyn_Enum_p_struct*)_tmp26B;if(_tmp2AB->tag != 12)goto _LL1A5;
else{_tmp2AC=_tmp2AB->f2;}}_LL1A4: _tmp2AE=_tmp2AC;goto _LL1A6;_LL1A5: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2AD=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp26B;if(_tmp2AD->tag != 13)goto
_LL1A7;else{_tmp2AE=_tmp2AD->f2;}}_LL1A6: Cyc_Absyndump_dumpqvar(_tmp2AE->name);
goto _LL17C;_LL1A7: {struct Cyc_Absyn_Exp_p_struct*_tmp2AF=(struct Cyc_Absyn_Exp_p_struct*)
_tmp26B;if(_tmp2AF->tag != 16)goto _LL17C;else{_tmp2B0=_tmp2AF->f1;}}_LL1A8: Cyc_Absyndump_dumpexp(
_tmp2B0);goto _LL17C;_LL17C:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef);void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumpdatatypefields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*
fields){const char*_tmp4A3;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp4A3=",",_tag_dyneither(_tmp4A3,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef);void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{const char*_tmp4A4;Cyc_Absyndump_dump(((
_tmp4A4=" = ",_tag_dyneither(_tmp4A4,sizeof(char),4))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){const char*_tmp4A5;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct
Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,((_tmp4A5=",",_tag_dyneither(_tmp4A5,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp2DF;
struct _dyneither_ptr*_tmp2E0;struct Cyc_Absyn_Tqual _tmp2E1;void*_tmp2E2;struct Cyc_Absyn_Exp*
_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_Absyn_Aggrfield*_tmp2DE=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp2DF=*_tmp2DE;_tmp2E0=_tmp2DF.name;_tmp2E1=_tmp2DF.tq;_tmp2E2=
_tmp2DF.type;_tmp2E3=_tmp2DF.width;_tmp2E4=_tmp2DF.attributes;switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1A9:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2E1,
_tmp2E2,Cyc_Absyndump_dump_str,_tmp2E0);Cyc_Absyndump_dumpatts(_tmp2E4);break;
case Cyc_Cyclone_Vc_c: _LL1AA: Cyc_Absyndump_dumpatts(_tmp2E4);((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2E1,_tmp2E2,Cyc_Absyndump_dump_str,_tmp2E0);break;}if(_tmp2E3 != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2E3);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td);void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd);
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple18{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr);
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr){{void*_tmp2E5=(*pr).f1;
_LL1AD: {struct Cyc_Absyn_Unused_att_struct*_tmp2E6=(struct Cyc_Absyn_Unused_att_struct*)
_tmp2E5;if(_tmp2E6->tag != 11)goto _LL1AF;}_LL1AE: goto _LL1AC;_LL1AF: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2E7=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2E5;if(_tmp2E7->tag != 1)goto
_LL1B1;}_LL1B0:{const char*_tmp4A6;Cyc_Absyndump_dump(((_tmp4A6="_stdcall",
_tag_dyneither(_tmp4A6,sizeof(char),9))));}goto _LL1AC;_LL1B1: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp2E8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp2E5;if(_tmp2E8->tag != 2)goto
_LL1B3;}_LL1B2:{const char*_tmp4A7;Cyc_Absyndump_dump(((_tmp4A7="_cdecl",
_tag_dyneither(_tmp4A7,sizeof(char),7))));}goto _LL1AC;_LL1B3: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp2E9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp2E5;if(_tmp2E9->tag != 3)goto
_LL1B5;}_LL1B4:{const char*_tmp4A8;Cyc_Absyndump_dump(((_tmp4A8="_fastcall",
_tag_dyneither(_tmp4A8,sizeof(char),10))));}goto _LL1AC;_LL1B5:;_LL1B6: goto _LL1AC;
_LL1AC:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_Absyndump_dump_callconv_fdqvar(struct
Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds);static void
Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc);void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp2EE;enum Cyc_Absyn_Scope _tmp2EF;struct _tuple0*
_tmp2F0;struct Cyc_Absyn_Tqual _tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;
struct Cyc_List_List*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2ED=vd;_tmp2EE=*_tmp2ED;
_tmp2EF=_tmp2EE.sc;_tmp2F0=_tmp2EE.name;_tmp2F1=_tmp2EE.tq;_tmp2F2=_tmp2EE.type;
_tmp2F3=_tmp2EE.initializer;_tmp2F4=_tmp2EE.attributes;Cyc_Absyndump_dumploc(loc);
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B7: if(_tmp2EF == Cyc_Absyn_Extern
 && Cyc_Absyndump_qvar_to_Cids){void*_tmp2F5=Cyc_Tcutil_compress(_tmp2F2);_LL1BA: {
struct Cyc_Absyn_FnType_struct*_tmp2F6=(struct Cyc_Absyn_FnType_struct*)_tmp2F5;
if(_tmp2F6->tag != 10)goto _LL1BC;}_LL1BB: goto _LL1B9;_LL1BC:;_LL1BD: Cyc_Absyndump_dumpscope(
_tmp2EF);_LL1B9:;}else{Cyc_Absyndump_dumpscope(_tmp2EF);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2F1,
_tmp2F2,Cyc_Absyndump_dumpqvar,_tmp2F0);Cyc_Absyndump_dumpatts(_tmp2F4);break;
case Cyc_Cyclone_Vc_c: _LL1B8: Cyc_Absyndump_dumpatts(_tmp2F4);Cyc_Absyndump_dumpscope(
_tmp2EF);{struct _RegionHandle _tmp2F7=_new_region("temp");struct _RegionHandle*
temp=& _tmp2F7;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2FA;
struct Cyc_List_List*_tmp2FB;struct _tuple11 _tmp2F8=Cyc_Absynpp_to_tms(temp,
_tmp2F1,_tmp2F2);_tmp2F9=_tmp2F8.f1;_tmp2FA=_tmp2F8.f2;_tmp2FB=_tmp2F8.f3;{void*
call_conv=(void*)& Cyc_Absyn_Unused_att_val;{struct Cyc_List_List*tms2=_tmp2FB;
for(0;tms2 != 0;tms2=tms2->tl){void*_tmp2FC=(void*)tms2->hd;struct Cyc_List_List*
_tmp2FE;_LL1C0: {struct Cyc_Absyn_Attributes_mod_struct*_tmp2FD=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp2FC;if(_tmp2FD->tag != 5)goto _LL1C2;else{_tmp2FE=_tmp2FD->f2;}}_LL1C1: for(0;
_tmp2FE != 0;_tmp2FE=_tmp2FE->tl){void*_tmp2FF=(void*)_tmp2FE->hd;_LL1C5: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp300=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp2FF;if(_tmp300->tag != 1)goto _LL1C7;}_LL1C6: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;
goto _LL1C4;_LL1C7: {struct Cyc_Absyn_Cdecl_att_struct*_tmp301=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp2FF;if(_tmp301->tag != 2)goto _LL1C9;}_LL1C8: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1C4;_LL1C9: {struct Cyc_Absyn_Fastcall_att_struct*_tmp302=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2FF;if(_tmp302->tag != 3)goto _LL1CB;}_LL1CA: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1C4;_LL1CB:;_LL1CC: goto _LL1C4;_LL1C4:;}goto _LL1BF;_LL1C2:;_LL1C3: goto
_LL1BF;_LL1BF:;}}Cyc_Absyndump_dumptq(_tmp2F9);Cyc_Absyndump_dumpntyp(_tmp2FA);{
struct _tuple18 _tmp4A9;struct _tuple18 _tmp303=(_tmp4A9.f1=call_conv,((_tmp4A9.f2=
_tmp2F0,_tmp4A9)));((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),
struct _tuple18*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp2FB),Cyc_Absyndump_dump_callconv_qvar,& _tmp303);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp2F3 != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2F3);}Cyc_Absyndump_dump_semi();}
struct _tuple19{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d);void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*
_tmp305=d->r;struct Cyc_Absyn_Vardecl*_tmp307;struct Cyc_Absyn_Fndecl*_tmp309;
struct Cyc_Absyn_Aggrdecl*_tmp30B;struct Cyc_Absyn_Datatypedecl*_tmp30D;struct Cyc_Absyn_Datatypedecl
_tmp30E;enum Cyc_Absyn_Scope _tmp30F;struct _tuple0*_tmp310;struct Cyc_List_List*
_tmp311;struct Cyc_Core_Opt*_tmp312;int _tmp313;struct Cyc_Absyn_Enumdecl*_tmp315;
struct Cyc_Absyn_Enumdecl _tmp316;enum Cyc_Absyn_Scope _tmp317;struct _tuple0*
_tmp318;struct Cyc_Core_Opt*_tmp319;struct Cyc_Absyn_Pat*_tmp31B;struct Cyc_Absyn_Exp*
_tmp31C;struct Cyc_List_List*_tmp31E;struct Cyc_Absyn_Tvar*_tmp320;struct Cyc_Absyn_Vardecl*
_tmp321;int _tmp322;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_Absyn_Exp*_tmp325;
struct Cyc_Absyn_Tvar*_tmp326;struct Cyc_Absyn_Vardecl*_tmp327;struct Cyc_Absyn_Typedefdecl*
_tmp329;struct _dyneither_ptr*_tmp32B;struct Cyc_List_List*_tmp32C;struct _tuple0*
_tmp32E;struct Cyc_List_List*_tmp32F;struct Cyc_List_List*_tmp331;struct Cyc_List_List*
_tmp333;struct Cyc_List_List*_tmp334;_LL1CE: {struct Cyc_Absyn_Var_d_struct*
_tmp306=(struct Cyc_Absyn_Var_d_struct*)_tmp305;if(_tmp306->tag != 0)goto _LL1D0;
else{_tmp307=_tmp306->f1;}}_LL1CF: Cyc_Absyndump_dumpvardecl(_tmp307,d->loc);goto
_LL1CD;_LL1D0: {struct Cyc_Absyn_Fn_d_struct*_tmp308=(struct Cyc_Absyn_Fn_d_struct*)
_tmp305;if(_tmp308->tag != 1)goto _LL1D2;else{_tmp309=_tmp308->f1;}}_LL1D1: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1EE: Cyc_Absyndump_dumpatts(
_tmp309->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1EF: break;}if(_tmp309->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp337=Cyc_Cyclone_c_compiler;_LL1F2: if(_tmp337 != 
Cyc_Cyclone_Vc_c)goto _LL1F4;_LL1F3:{const char*_tmp4AA;Cyc_Absyndump_dump(((
_tmp4AA="__inline",_tag_dyneither(_tmp4AA,sizeof(char),9))));}goto _LL1F1;_LL1F4:;
_LL1F5:{const char*_tmp4AB;Cyc_Absyndump_dump(((_tmp4AB="inline",_tag_dyneither(
_tmp4AB,sizeof(char),7))));}goto _LL1F1;_LL1F1:;}Cyc_Absyndump_dumpscope(_tmp309->sc);{
struct Cyc_Absyn_FnType_struct _tmp4B1;struct Cyc_Absyn_FnInfo _tmp4B0;struct Cyc_Absyn_FnType_struct*
_tmp4AF;void*t=(void*)((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=((
_tmp4B1.tag=10,((_tmp4B1.f1=((_tmp4B0.tvars=_tmp309->tvs,((_tmp4B0.effect=
_tmp309->effect,((_tmp4B0.ret_typ=_tmp309->ret_type,((_tmp4B0.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp309->args),((_tmp4B0.c_varargs=_tmp309->c_varargs,((_tmp4B0.cyc_varargs=
_tmp309->cyc_varargs,((_tmp4B0.rgn_po=_tmp309->rgn_po,((_tmp4B0.attributes=0,
_tmp4B0)))))))))))))))),_tmp4B1)))),_tmp4AF))));switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1F6:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_dump_atts_qvar,_tmp309);break;case Cyc_Cyclone_Vc_c: _LL1F7:((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp309);break;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp309->body,
0);Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;};_LL1D2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp30A=(struct Cyc_Absyn_Aggr_d_struct*)_tmp305;if(_tmp30A->tag != 6)goto _LL1D4;
else{_tmp30B=_tmp30A->f1;}}_LL1D3: Cyc_Absyndump_dumpscope(_tmp30B->sc);Cyc_Absyndump_dumpaggr_kind(
_tmp30B->kind);Cyc_Absyndump_dumpqvar(_tmp30B->name);Cyc_Absyndump_dumptvars(
_tmp30B->tvs);if(_tmp30B->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30B->impl))->exist_vars
!= 0){const char*_tmp4B4;const char*_tmp4B3;const char*_tmp4B2;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30B->impl))->exist_vars,((_tmp4B2="<",
_tag_dyneither(_tmp4B2,sizeof(char),2))),((_tmp4B3=">",_tag_dyneither(_tmp4B3,
sizeof(char),2))),((_tmp4B4=",",_tag_dyneither(_tmp4B4,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30B->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp30B->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp30B->impl))->fields);{const char*_tmp4B5;Cyc_Absyndump_dump(((
_tmp4B5="}",_tag_dyneither(_tmp4B5,sizeof(char),2))));}Cyc_Absyndump_dumpatts(
_tmp30B->attributes);{const char*_tmp4B6;Cyc_Absyndump_dump(((_tmp4B6=";",
_tag_dyneither(_tmp4B6,sizeof(char),2))));};}goto _LL1CD;_LL1D4: {struct Cyc_Absyn_Datatype_d_struct*
_tmp30C=(struct Cyc_Absyn_Datatype_d_struct*)_tmp305;if(_tmp30C->tag != 7)goto
_LL1D6;else{_tmp30D=_tmp30C->f1;_tmp30E=*_tmp30D;_tmp30F=_tmp30E.sc;_tmp310=
_tmp30E.name;_tmp311=_tmp30E.tvs;_tmp312=_tmp30E.fields;_tmp313=_tmp30E.is_extensible;}}
_LL1D5: Cyc_Absyndump_dumpscope(_tmp30F);if(_tmp313){const char*_tmp4B7;Cyc_Absyndump_dump(((
_tmp4B7="@extensible ",_tag_dyneither(_tmp4B7,sizeof(char),13))));}{const char*
_tmp4B8;Cyc_Absyndump_dump(((_tmp4B8="datatype ",_tag_dyneither(_tmp4B8,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmp310);Cyc_Absyndump_dumptvars(_tmp311);
if(_tmp312 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp312->v);{const char*
_tmp4B9;Cyc_Absyndump_dump_nospace(((_tmp4B9="};",_tag_dyneither(_tmp4B9,sizeof(
char),3))));};}goto _LL1CD;_LL1D6: {struct Cyc_Absyn_Enum_d_struct*_tmp314=(struct
Cyc_Absyn_Enum_d_struct*)_tmp305;if(_tmp314->tag != 8)goto _LL1D8;else{_tmp315=
_tmp314->f1;_tmp316=*_tmp315;_tmp317=_tmp316.sc;_tmp318=_tmp316.name;_tmp319=
_tmp316.fields;}}_LL1D7: Cyc_Absyndump_dumpscope(_tmp317);{const char*_tmp4BA;Cyc_Absyndump_dump(((
_tmp4BA="enum ",_tag_dyneither(_tmp4BA,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp318);if(_tmp319 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp319->v);{const
char*_tmp4BB;Cyc_Absyndump_dump_nospace(((_tmp4BB="};",_tag_dyneither(_tmp4BB,
sizeof(char),3))));};}return;_LL1D8: {struct Cyc_Absyn_Let_d_struct*_tmp31A=(
struct Cyc_Absyn_Let_d_struct*)_tmp305;if(_tmp31A->tag != 2)goto _LL1DA;else{
_tmp31B=_tmp31A->f1;_tmp31C=_tmp31A->f3;}}_LL1D9:{const char*_tmp4BC;Cyc_Absyndump_dump(((
_tmp4BC="let",_tag_dyneither(_tmp4BC,sizeof(char),4))));}Cyc_Absyndump_dumppat(
_tmp31B);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp31C);Cyc_Absyndump_dump_semi();
goto _LL1CD;_LL1DA: {struct Cyc_Absyn_Letv_d_struct*_tmp31D=(struct Cyc_Absyn_Letv_d_struct*)
_tmp305;if(_tmp31D->tag != 3)goto _LL1DC;else{_tmp31E=_tmp31D->f1;}}_LL1DB:{const
char*_tmp4BD;Cyc_Absyndump_dump(((_tmp4BD="let ",_tag_dyneither(_tmp4BD,sizeof(
char),5))));}Cyc_Absyndump_dumpids(_tmp31E);Cyc_Absyndump_dump_semi();goto _LL1CD;
_LL1DC: {struct Cyc_Absyn_Region_d_struct*_tmp31F=(struct Cyc_Absyn_Region_d_struct*)
_tmp305;if(_tmp31F->tag != 4)goto _LL1DE;else{_tmp320=_tmp31F->f1;_tmp321=_tmp31F->f2;
_tmp322=_tmp31F->f3;_tmp323=_tmp31F->f4;}}_LL1DD:{const char*_tmp4BE;Cyc_Absyndump_dump(((
_tmp4BE="region",_tag_dyneither(_tmp4BE,sizeof(char),7))));}{const char*_tmp4BF;
Cyc_Absyndump_dump(((_tmp4BF="<",_tag_dyneither(_tmp4BF,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp320);{const char*_tmp4C0;Cyc_Absyndump_dump(((_tmp4C0="> ",_tag_dyneither(
_tmp4C0,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp321->name);if(_tmp322){
const char*_tmp4C1;Cyc_Absyndump_dump(((_tmp4C1="@resetable",_tag_dyneither(
_tmp4C1,sizeof(char),11))));}if(_tmp323 != 0){{const char*_tmp4C2;Cyc_Absyndump_dump(((
_tmp4C2=" = open(",_tag_dyneither(_tmp4C2,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp323);{const char*_tmp4C3;Cyc_Absyndump_dump(((_tmp4C3=")",
_tag_dyneither(_tmp4C3,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1CD;_LL1DE: {struct Cyc_Absyn_Alias_d_struct*_tmp324=(struct Cyc_Absyn_Alias_d_struct*)
_tmp305;if(_tmp324->tag != 5)goto _LL1E0;else{_tmp325=_tmp324->f1;_tmp326=_tmp324->f2;
_tmp327=_tmp324->f3;}}_LL1DF:{const char*_tmp4C4;Cyc_Absyndump_dump(((_tmp4C4="alias ",
_tag_dyneither(_tmp4C4,sizeof(char),7))));}{const char*_tmp4C5;Cyc_Absyndump_dump(((
_tmp4C5="<",_tag_dyneither(_tmp4C5,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp326);{const char*_tmp4C6;Cyc_Absyndump_dump(((_tmp4C6=">",_tag_dyneither(
_tmp4C6,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp327->name);{const char*
_tmp4C7;Cyc_Absyndump_dump(((_tmp4C7=" = ",_tag_dyneither(_tmp4C7,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp325);Cyc_Absyndump_dump_semi();goto _LL1CD;_LL1E0: {
struct Cyc_Absyn_Typedef_d_struct*_tmp328=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp305;if(_tmp328->tag != 9)goto _LL1E2;else{_tmp329=_tmp328->f1;}}_LL1E1: if(!Cyc_Absyndump_expand_typedefs
 || _tmp329->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp329->defn))->v)){{const char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="typedef",
_tag_dyneither(_tmp4C8,sizeof(char),8))));}{void*t;if(_tmp329->defn == 0)t=Cyc_Absyn_new_evar(
_tmp329->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp329->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp329->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp329);Cyc_Absyndump_dumpatts(_tmp329->atts);Cyc_Absyndump_dump_semi();};}goto
_LL1CD;_LL1E2: {struct Cyc_Absyn_Namespace_d_struct*_tmp32A=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp305;if(_tmp32A->tag != 10)goto _LL1E4;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;}}
_LL1E3:{const char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="namespace",
_tag_dyneither(_tmp4C9,sizeof(char),10))));}Cyc_Absyndump_dump_str(_tmp32B);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp32C != 0;_tmp32C=_tmp32C->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp32C->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E4: {struct Cyc_Absyn_Using_d_struct*_tmp32D=(struct Cyc_Absyn_Using_d_struct*)
_tmp305;if(_tmp32D->tag != 11)goto _LL1E6;else{_tmp32E=_tmp32D->f1;_tmp32F=_tmp32D->f2;}}
_LL1E5:{const char*_tmp4CA;Cyc_Absyndump_dump(((_tmp4CA="using",_tag_dyneither(
_tmp4CA,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp32E);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp32F != 0;_tmp32F=_tmp32F->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp32F->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E6: {struct Cyc_Absyn_ExternC_d_struct*_tmp330=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp305;if(_tmp330->tag != 12)goto _LL1E8;else{_tmp331=_tmp330->f1;}}_LL1E7:{const
char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB="extern \"C\" {",_tag_dyneither(
_tmp4CB,sizeof(char),13))));}for(0;_tmp331 != 0;_tmp331=_tmp331->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp331->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp332=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp305;if(_tmp332->tag != 13)goto _LL1EA;else{_tmp333=_tmp332->f1;_tmp334=_tmp332->f2;}}
_LL1E9:{const char*_tmp4CC;Cyc_Absyndump_dump(((_tmp4CC="extern \"C include\" {",
_tag_dyneither(_tmp4CC,sizeof(char),21))));}for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp333->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp334 != 0){{const char*_tmp4CD;Cyc_Absyndump_dump(((_tmp4CD=" export {",
_tag_dyneither(_tmp4CD,sizeof(char),10))));}for(0;_tmp334 != 0;_tmp334=_tmp334->tl){
struct _tuple0*_tmp35A;struct _tuple19 _tmp359=*((struct _tuple19*)_tmp334->hd);
_tmp35A=_tmp359.f2;Cyc_Absyndump_dumpqvar(_tmp35A);if(_tmp334->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4CE;Cyc_Absyndump_dump(((_tmp4CE="}",_tag_dyneither(
_tmp4CE,sizeof(char),2))));};}goto _LL1CD;_LL1EA: {struct Cyc_Absyn_Porton_d_struct*
_tmp335=(struct Cyc_Absyn_Porton_d_struct*)_tmp305;if(_tmp335->tag != 14)goto
_LL1EC;}_LL1EB:{const char*_tmp4CF;Cyc_Absyndump_dump(((_tmp4CF=" __cyclone_port_on__; ",
_tag_dyneither(_tmp4CF,sizeof(char),23))));}goto _LL1CD;_LL1EC: {struct Cyc_Absyn_Portoff_d_struct*
_tmp336=(struct Cyc_Absyn_Portoff_d_struct*)_tmp305;if(_tmp336->tag != 15)goto
_LL1CD;}_LL1ED:{const char*_tmp4D0;Cyc_Absyndump_dump(((_tmp4D0=" __cyclone_port_off__; ",
_tag_dyneither(_tmp4D0,sizeof(char),24))));}goto _LL1CD;_LL1CD:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e);static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a);void Cyc_Absyndump_dumptms(struct
Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*_tmp35E=(
void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp360;void*_tmp361;union Cyc_Absyn_Constraint*
_tmp362;union Cyc_Absyn_Constraint*_tmp363;union Cyc_Absyn_Constraint*_tmp364;
struct Cyc_Absyn_Tqual _tmp365;_LL1FA: {struct Cyc_Absyn_Pointer_mod_struct*_tmp35F=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp35E;if(_tmp35F->tag != 2)goto _LL1FC;else{
_tmp360=_tmp35F->f1;_tmp361=_tmp360.rgn;_tmp362=_tmp360.nullable;_tmp363=_tmp360.bounds;
_tmp364=_tmp360.zero_term;_tmp365=_tmp35F->f2;}}_LL1FB:{void*_tmp366=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp363);struct Cyc_Absyn_Exp*_tmp369;_LL1FF: {struct Cyc_Absyn_DynEither_b_struct*
_tmp367=(struct Cyc_Absyn_DynEither_b_struct*)_tmp366;if(_tmp367->tag != 0)goto
_LL201;}_LL200: Cyc_Absyndump_dump_char((int)'?');goto _LL1FE;_LL201: {struct Cyc_Absyn_Upper_b_struct*
_tmp368=(struct Cyc_Absyn_Upper_b_struct*)_tmp366;if(_tmp368->tag != 1)goto _LL1FE;
else{_tmp369=_tmp368->f1;}}_LL202: Cyc_Absyndump_dump_char((int)(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp362)?'*':'@'));Cyc_Absyndump_dump_upperbound(
_tmp369);goto _LL1FE;_LL1FE:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp364)){const char*_tmp4D1;Cyc_Absyndump_dump(((_tmp4D1="@zeroterm",
_tag_dyneither(_tmp4D1,sizeof(char),10))));}{void*_tmp36B=Cyc_Tcutil_compress(
_tmp361);struct Cyc_Absyn_Tvar*_tmp36F;struct Cyc_Core_Opt*_tmp371;_LL204: {struct
Cyc_Absyn_HeapRgn_struct*_tmp36C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp36B;if(
_tmp36C->tag != 21)goto _LL206;}_LL205: goto _LL203;_LL206: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp36D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp36B;if(_tmp36D->tag != 22)goto
_LL208;}_LL207:{const char*_tmp4D2;Cyc_Absyndump_dump(((_tmp4D2="`U",
_tag_dyneither(_tmp4D2,sizeof(char),3))));}goto _LL203;_LL208: {struct Cyc_Absyn_VarType_struct*
_tmp36E=(struct Cyc_Absyn_VarType_struct*)_tmp36B;if(_tmp36E->tag != 2)goto _LL20A;
else{_tmp36F=_tmp36E->f1;}}_LL209: Cyc_Absyndump_dump_str(_tmp36F->name);goto
_LL203;_LL20A: {struct Cyc_Absyn_Evar_struct*_tmp370=(struct Cyc_Absyn_Evar_struct*)
_tmp36B;if(_tmp370->tag != 1)goto _LL20C;else{_tmp371=_tmp370->f2;if(_tmp371 != 0)
goto _LL20C;}}_LL20B: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp361));goto
_LL203;_LL20C:;_LL20D: {const char*_tmp4D5;void*_tmp4D4;(_tmp4D4=0,Cyc_Tcutil_impos(((
_tmp4D5="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4D5,sizeof(
char),37))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}_LL203:;}Cyc_Absyndump_dumptq(
_tmp365);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1FC:;_LL1FD: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp375=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL20F: {struct Cyc_Absyn_Pointer_mod_struct*_tmp376=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp375;if(_tmp376->tag != 2)goto _LL211;}
_LL210: next_is_pointer=1;goto _LL20E;_LL211:;_LL212: goto _LL20E;_LL20E:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp377=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp379;struct Cyc_Absyn_Exp*_tmp37B;union Cyc_Absyn_Constraint*
_tmp37C;void*_tmp37E;struct Cyc_List_List*_tmp380;int _tmp381;struct Cyc_Absyn_VarargInfo*
_tmp382;struct Cyc_Core_Opt*_tmp383;struct Cyc_List_List*_tmp384;void*_tmp386;
struct Cyc_List_List*_tmp388;struct Cyc_Position_Segment*_tmp389;struct Cyc_List_List*
_tmp38B;struct Cyc_Position_Segment*_tmp38C;int _tmp38D;struct Cyc_List_List*
_tmp38F;_LL214: {struct Cyc_Absyn_Carray_mod_struct*_tmp378=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp377;if(_tmp378->tag != 0)goto _LL216;else{_tmp379=_tmp378->f1;}}_LL215:{const
char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6="[]",_tag_dyneither(_tmp4D6,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp379)){const char*_tmp4D7;Cyc_Absyndump_dump(((_tmp4D7="@zeroterm",
_tag_dyneither(_tmp4D7,sizeof(char),10))));}goto _LL213;_LL216: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp37A=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp377;if(_tmp37A->tag != 1)goto
_LL218;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37A->f2;}}_LL217: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp37B);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp37C)){const
char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="@zeroterm",_tag_dyneither(_tmp4D8,
sizeof(char),10))));}goto _LL213;_LL218: {struct Cyc_Absyn_Function_mod_struct*
_tmp37D=(struct Cyc_Absyn_Function_mod_struct*)_tmp377;if(_tmp37D->tag != 3)goto
_LL21A;else{_tmp37E=(void*)_tmp37D->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp37F=(
struct Cyc_Absyn_WithTypes_struct*)_tmp37E;if(_tmp37F->tag != 1)goto _LL21A;else{
_tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_tmp382=_tmp37F->f3;_tmp383=_tmp37F->f4;
_tmp384=_tmp37F->f5;}};}}_LL219: Cyc_Absyndump_dumpfunargs(_tmp380,_tmp381,
_tmp382,_tmp383,_tmp384);goto _LL213;_LL21A: {struct Cyc_Absyn_Function_mod_struct*
_tmp385=(struct Cyc_Absyn_Function_mod_struct*)_tmp377;if(_tmp385->tag != 3)goto
_LL21C;else{_tmp386=(void*)_tmp385->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp387=(
struct Cyc_Absyn_NoTypes_struct*)_tmp386;if(_tmp387->tag != 0)goto _LL21C;else{
_tmp388=_tmp387->f1;_tmp389=_tmp387->f2;}};}}_LL21B:{const char*_tmp4DB;const char*
_tmp4DA;const char*_tmp4D9;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp388,((_tmp4D9="(",_tag_dyneither(_tmp4D9,sizeof(char),
2))),((_tmp4DA=")",_tag_dyneither(_tmp4DA,sizeof(char),2))),((_tmp4DB=",",
_tag_dyneither(_tmp4DB,sizeof(char),2))));}goto _LL213;_LL21C: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp38A=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp377;if(_tmp38A->tag != 4)goto
_LL21E;else{_tmp38B=_tmp38A->f1;_tmp38C=_tmp38A->f2;_tmp38D=_tmp38A->f3;}}_LL21D:
if(_tmp38D)Cyc_Absyndump_dumpkindedtvars(_tmp38B);else{Cyc_Absyndump_dumptvars(
_tmp38B);}goto _LL213;_LL21E: {struct Cyc_Absyn_Attributes_mod_struct*_tmp38E=(
struct Cyc_Absyn_Attributes_mod_struct*)_tmp377;if(_tmp38E->tag != 5)goto _LL220;
else{_tmp38F=_tmp38E->f2;}}_LL21F: Cyc_Absyndump_dumpatts(_tmp38F);goto _LL213;
_LL220: {struct Cyc_Absyn_Pointer_mod_struct*_tmp390=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp377;if(_tmp390->tag != 2)goto _LL213;}_LL221: {const char*_tmp4DE;void*_tmp4DD;(
_tmp4DD=0,Cyc_Tcutil_impos(((_tmp4DE="dumptms",_tag_dyneither(_tmp4DE,sizeof(
char),8))),_tag_dyneither(_tmp4DD,sizeof(void*),0)));}_LL213:;}return;}_LL1F9:;};}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct _RegionHandle _tmp399=_new_region("temp");struct _RegionHandle*temp=& _tmp399;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp39B;void*_tmp39C;struct Cyc_List_List*
_tmp39D;struct _tuple11 _tmp39A=Cyc_Absynpp_to_tms(temp,tq,t);_tmp39B=_tmp39A.f1;
_tmp39C=_tmp39A.f2;_tmp39D=_tmp39A.f3;Cyc_Absyndump_dumptq(_tmp39B);Cyc_Absyndump_dumpntyp(
_tmp39C);Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp39D),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp4E1;void*_tmp4E0;(_tmp4E0=0,Cyc_fprintf(f,((_tmp4E1="\n",
_tag_dyneither(_tmp4E1,sizeof(char),2))),_tag_dyneither(_tmp4E0,sizeof(void*),0)));};}
