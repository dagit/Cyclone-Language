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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);struct
Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
 = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct
Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None
 = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
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
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;}
;struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
enum Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p);struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*
v){struct Cyc_List_List*_tmp5E8;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5E8=_cycalloc(sizeof(*_tmp5E8)),((_tmp5E8->hd=v,((_tmp5E8->tl=0,_tmp5E8)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5E9;return(_tmp5E9="\\a",
_tag_dyneither(_tmp5E9,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5EA;
return(_tmp5EA="\\b",_tag_dyneither(_tmp5EA,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5EB;return(_tmp5EB="\\f",_tag_dyneither(_tmp5EB,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5EC;return(_tmp5EC="\\n",_tag_dyneither(_tmp5EC,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5ED;return(_tmp5ED="\\r",
_tag_dyneither(_tmp5ED,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5EE;
return(_tmp5EE="\\t",_tag_dyneither(_tmp5EE,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5EF;return(_tmp5EF="\\v",_tag_dyneither(_tmp5EF,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5F0;return(_tmp5F0="\\\\",_tag_dyneither(_tmp5F0,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5F1;return(_tmp5F1="\"",
_tag_dyneither(_tmp5F1,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5F2;
return(_tmp5F2="\\'",_tag_dyneither(_tmp5F2,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5F5;
char _tmp5F4;struct _dyneither_ptr _tmp5F3;(_tmp5F3=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5F4=*((char*)_check_dyneither_subscript(_tmp5F3,sizeof(
char),0)),((_tmp5F5=c,((_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (
_tmp5F4 == '\000'  && _tmp5F5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=
_tmp5F5)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5F8;char _tmp5F7;struct
_dyneither_ptr _tmp5F6;(_tmp5F6=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F7=*((char*)_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((_tmp5F8='\\',((
_get_dyneither_size(_tmp5F6,sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}{char _tmp5FB;char
_tmp5FA;struct _dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5FA=*((char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((
_tmp5FB=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5F9,
sizeof(char))== 1  && (_tmp5FA == '\000'  && _tmp5FB != '\000')?_throw_arraybounds():
1,*((char*)_tmp5F9.curr)=_tmp5FB)))))));}{char _tmp5FE;char _tmp5FD;struct
_dyneither_ptr _tmp5FC;(_tmp5FC=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5FD=*((char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((_tmp5FE=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (
_tmp5FD == '\000'  && _tmp5FE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=
_tmp5FE)))))));}{char _tmp601;char _tmp600;struct _dyneither_ptr _tmp5FF;(_tmp5FF=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp600=*((char*)
_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((_tmp601=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'  && _tmp601 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s);
static int Cyc_Absynpp_special(struct _dyneither_ptr s){int sz=(int)(
_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((const
char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~') || 
c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s);struct _dyneither_ptr Cyc_Absynpp_string_escape(struct
_dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp604;char _tmp603;struct
_dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp603=*((
char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((_tmp604='\\',((
_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'  && _tmp604 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}{char _tmp607;char
_tmp606;struct _dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp606=*((char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((
_tmp607='a',((_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'
 && _tmp607 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp60A;char _tmp609;struct
_dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp609=*((
char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((_tmp60A='\\',((
_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'  && _tmp60A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}{char _tmp60D;char
_tmp60C;struct _dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60C=*((char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((
_tmp60D='b',((_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'
 && _tmp60D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp610;char _tmp60F;struct
_dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60F=*((
char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((_tmp610='\\',((
_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'  && _tmp610 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}{char _tmp613;char
_tmp612;struct _dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp612=*((char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((
_tmp613='f',((_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'
 && _tmp613 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp616;char _tmp615;struct
_dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp615=*((
char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((_tmp616='\\',((
_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'  && _tmp616 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}{char _tmp619;char
_tmp618;struct _dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp618=*((char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((
_tmp619='n',((_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'
 && _tmp619 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp61C;char _tmp61B;struct
_dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61B=*((
char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((_tmp61C='\\',((
_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'  && _tmp61C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}{char _tmp61F;char
_tmp61E;struct _dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61E=*((char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((
_tmp61F='r',((_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'
 && _tmp61F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp622;char _tmp621;struct
_dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp621=*((
char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622='\\',((
_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}{char _tmp625;char
_tmp624;struct _dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp624=*((char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((
_tmp625='t',((_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'
 && _tmp625 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp628;char _tmp627;struct
_dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp627=*((
char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((_tmp628='\\',((
_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'  && _tmp628 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}{char _tmp62B;char
_tmp62A;struct _dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62A=*((char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((
_tmp62B='v',((_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'
 && _tmp62B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp62E;char _tmp62D;struct
_dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62D=*((
char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((_tmp62E='\\',((
_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'  && _tmp62E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}{char _tmp631;char
_tmp630;struct _dyneither_ptr _tmp62F;(_tmp62F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp630=*((char*)_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((
_tmp631='\\',((_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'
 && _tmp631 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp634;char _tmp633;struct
_dyneither_ptr _tmp632;(_tmp632=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp633=*((
char*)_check_dyneither_subscript(_tmp632,sizeof(char),0)),((_tmp634='\\',((
_get_dyneither_size(_tmp632,sizeof(char))== 1  && (_tmp633 == '\000'  && _tmp634 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp632.curr)=_tmp634)))))));}{char _tmp637;char
_tmp636;struct _dyneither_ptr _tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp636=*((char*)_check_dyneither_subscript(_tmp635,sizeof(char),0)),((
_tmp637='"',((_get_dyneither_size(_tmp635,sizeof(char))== 1  && (_tmp636 == '\000'
 && _tmp637 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp635.curr)=_tmp637)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp63A;char _tmp639;
struct _dyneither_ptr _tmp638;(_tmp638=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp639=*((char*)_check_dyneither_subscript(_tmp638,sizeof(char),0)),((_tmp63A=
_tmp1E,((_get_dyneither_size(_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'  && 
_tmp63A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));}
else{{char _tmp63D;char _tmp63C;struct _dyneither_ptr _tmp63B;(_tmp63B=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63C=*((char*)
_check_dyneither_subscript(_tmp63B,sizeof(char),0)),((_tmp63D='\\',((
_get_dyneither_size(_tmp63B,sizeof(char))== 1  && (_tmp63C == '\000'  && _tmp63D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63B.curr)=_tmp63D)))))));}{char _tmp640;char
_tmp63F;struct _dyneither_ptr _tmp63E;(_tmp63E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63F=*((char*)_check_dyneither_subscript(_tmp63E,sizeof(char),0)),((
_tmp640=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp63E,sizeof(char))
== 1  && (_tmp63F == '\000'  && _tmp640 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp63E.curr)=_tmp640)))))));}{char _tmp643;char _tmp642;struct _dyneither_ptr
_tmp641;(_tmp641=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp642=*((char*)
_check_dyneither_subscript(_tmp641,sizeof(char),0)),((_tmp643=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp641,sizeof(char))== 1  && (_tmp642 == '\000'
 && _tmp643 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp641.curr)=_tmp643)))))));}{
char _tmp646;char _tmp645;struct _dyneither_ptr _tmp644;(_tmp644=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp645=*((char*)_check_dyneither_subscript(_tmp644,
sizeof(char),0)),((_tmp646=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp644,sizeof(char))== 1  && (_tmp645 == '\000'  && _tmp646 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp644.curr)=_tmp646)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp647;l=((_tmp647=_cycalloc(sizeof(*_tmp647)),((_tmp647->hd=
Cyc_Absynpp_restrict_sp,((_tmp647->tl=l,_tmp647))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp648;l=((_tmp648=_cycalloc(sizeof(*_tmp648)),((_tmp648->hd=Cyc_Absynpp_volatile_sp,((
_tmp648->tl=l,_tmp648))))));}if(tq.print_const){struct Cyc_List_List*_tmp649;l=((
_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649->hd=Cyc_Absynpp_const_sp,((_tmp649->tl=
l,_tmp649))))));}{const char*_tmp64C;const char*_tmp64B;const char*_tmp64A;return
Cyc_PP_egroup(((_tmp64A="",_tag_dyneither(_tmp64A,sizeof(char),1))),((_tmp64B=" ",
_tag_dyneither(_tmp64B,sizeof(char),2))),((_tmp64C=" ",_tag_dyneither(_tmp64C,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*
_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(
_tmp6F){case Cyc_Absyn_AnyKind: _LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp64D;return(_tmp64D="A",_tag_dyneither(_tmp64D,sizeof(char),2));}
case Cyc_Absyn_Unique: _LL39: {const char*_tmp64E;return(_tmp64E="UA",
_tag_dyneither(_tmp64E,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {const char*
_tmp64F;return(_tmp64F="TA",_tag_dyneither(_tmp64F,sizeof(char),3));}}case Cyc_Absyn_MemKind:
_LL37: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3D: {const char*_tmp650;return(
_tmp650="M",_tag_dyneither(_tmp650,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp651;return(_tmp651="UM",_tag_dyneither(_tmp651,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3F: {const char*_tmp652;return(_tmp652="TM",_tag_dyneither(
_tmp652,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL42: {const char*_tmp653;return(_tmp653="B",_tag_dyneither(_tmp653,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL43: {const char*_tmp654;return(_tmp654="UB",
_tag_dyneither(_tmp654,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {const char*
_tmp655;return(_tmp655="TB",_tag_dyneither(_tmp655,sizeof(char),3));}}case Cyc_Absyn_RgnKind:
_LL41: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL47: {const char*_tmp656;return(
_tmp656="R",_tag_dyneither(_tmp656,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp657;return(_tmp657="UR",_tag_dyneither(_tmp657,sizeof(char),3));}
case Cyc_Absyn_Top: _LL49: {const char*_tmp658;return(_tmp658="TR",_tag_dyneither(
_tmp658,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {const char*_tmp659;
return(_tmp659="E",_tag_dyneither(_tmp659,sizeof(char),2));}case Cyc_Absyn_IntKind:
_LL4B: {const char*_tmp65A;return(_tmp65A="I",_tag_dyneither(_tmp65A,sizeof(char),
2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k);struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp65B;
return(_tmp65B="{?}",_tag_dyneither(_tmp65B,sizeof(char),4));}else{const char*
_tmp65C;return(_tmp65C="?",_tag_dyneither(_tmp65C,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp660;
void*_tmp65F[1];struct Cyc_String_pa_struct _tmp65E;return(struct _dyneither_ptr)((
_tmp65E.tag=0,((_tmp65E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp65F[0]=& _tmp65E,Cyc_aprintf(((_tmp660="<=%s",_tag_dyneither(
_tmp660,sizeof(char),5))),_tag_dyneither(_tmp65F,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp8A=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8C;struct
Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(
Cyc_Absynpp_kind2string(_tmp8C));_LL57: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}
_LL58: if(Cyc_PP_tex_output){const char*_tmp661;return Cyc_PP_text_width(((_tmp661="{?}",
_tag_dyneither(_tmp661,sizeof(char),4))),1);}else{const char*_tmp662;return Cyc_PP_text(((
_tmp662="?",_tag_dyneither(_tmp662,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp665;const char*_tmp664;
const char*_tmp663;return Cyc_PP_egroup(((_tmp663="<",_tag_dyneither(_tmp663,
sizeof(char),2))),((_tmp664=">",_tag_dyneither(_tmp664,sizeof(char),2))),((
_tmp665=",",_tag_dyneither(_tmp665,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp95=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind _tmp99;enum Cyc_Absyn_KindQual
_tmp9A;enum Cyc_Absyn_AliasQual _tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*
_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_struct*_tmp96=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp95;if(_tmp96->tag != 1)goto _LL5E;}_LL5D: goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp97=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95;if(_tmp97->tag != 0)goto _LL60;else{
_tmp98=_tmp97->f1;_tmp99=*_tmp98;_tmp9A=_tmp99.kind;if(_tmp9A != Cyc_Absyn_BoxKind)
goto _LL60;_tmp9B=_tmp99.aliasqual;if(_tmp9B != Cyc_Absyn_Aliasable)goto _LL60;}}
_LL5F: return Cyc_PP_textptr(tv->name);_LL60: {struct Cyc_Absyn_Less_kb_struct*
_tmp9C=(struct Cyc_Absyn_Less_kb_struct*)_tmp95;if(_tmp9C->tag != 2)goto _LL62;
else{_tmp9D=_tmp9C->f2;}}_LL61: _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_struct*
_tmp9E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95;if(_tmp9E->tag != 0)goto _LL5B;else{
_tmp9F=_tmp9E->f1;}}_LL63: {const char*_tmp668;struct Cyc_PP_Doc*_tmp667[3];return(
_tmp667[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp667[1]=Cyc_PP_text(((_tmp668="::",
_tag_dyneither(_tmp668,sizeof(char),3)))),((_tmp667[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp667,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp66B;const char*
_tmp66A;const char*_tmp669;return Cyc_PP_egroup(((_tmp669="<",_tag_dyneither(
_tmp669,sizeof(char),2))),((_tmp66A=">",_tag_dyneither(_tmp66A,sizeof(char),2))),((
_tmp66B=",",_tag_dyneither(_tmp66B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp66E;const char*_tmp66D;const char*_tmp66C;return Cyc_PP_egroup(((_tmp66C="<",
_tag_dyneither(_tmp66C,sizeof(char),2))),((_tmp66D=">",_tag_dyneither(_tmp66D,
sizeof(char),2))),((_tmp66E=",",_tag_dyneither(_tmp66E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple15*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp671;const char*_tmp670;const char*_tmp66F;return Cyc_PP_group(((
_tmp66F="(",_tag_dyneither(_tmp66F,sizeof(char),2))),((_tmp670=")",
_tag_dyneither(_tmp670,sizeof(char),2))),((_tmp671=",",_tag_dyneither(_tmp671,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmpAB=att;_LL65: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAC=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66:
goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_struct*_tmpAD=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpAE=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}
_LL6A: return Cyc_PP_nil_doc();_LL6B:;_LL6C: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL64:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmpAF=(void*)atts->hd;_LL6E: {struct Cyc_Absyn_Stdcall_att_struct*
_tmpB0=(struct Cyc_Absyn_Stdcall_att_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}
_LL6F: {const char*_tmp672;return Cyc_PP_text(((_tmp672=" _stdcall ",
_tag_dyneither(_tmp672,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}
_LL71: {const char*_tmp673;return Cyc_PP_text(((_tmp673=" _cdecl ",_tag_dyneither(
_tmp673,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*_tmpB2=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp674;return Cyc_PP_text(((_tmp674=" _fastcall ",_tag_dyneither(
_tmp674,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}return Cyc_PP_nil_doc();}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*
atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmpB6=(void*)atts2->hd;_LL77: {
struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78: goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}
_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto
_LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp67F;const char*
_tmp67E;const char*_tmp67D;const char*_tmp67C;const char*_tmp67B;struct Cyc_PP_Doc*
_tmp67A[3];return(_tmp67A[2]=Cyc_PP_text(((_tmp67B=")",_tag_dyneither(_tmp67B,
sizeof(char),2)))),((_tmp67A[1]=Cyc_PP_group(((_tmp67E="",_tag_dyneither(_tmp67E,
sizeof(char),1))),((_tmp67D="",_tag_dyneither(_tmp67D,sizeof(char),1))),((
_tmp67C=" ",_tag_dyneither(_tmp67C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp67A[0]=Cyc_PP_text(((_tmp67F=" __declspec(",_tag_dyneither(_tmp67F,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp67A,sizeof(struct Cyc_PP_Doc*),
3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c:
_LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp688;const char*_tmp687;const char*_tmp686;const char*_tmp685;struct
Cyc_PP_Doc*_tmp684[2];return(_tmp684[1]=Cyc_PP_group(((_tmp687="((",
_tag_dyneither(_tmp687,sizeof(char),3))),((_tmp686="))",_tag_dyneither(_tmp686,
sizeof(char),3))),((_tmp685=",",_tag_dyneither(_tmp685,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp684[0]=Cyc_PP_text(((_tmp688=" __attribute__",
_tag_dyneither(_tmp688,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp684,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){if(tms == 0)return 0;{
void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*_tmpC6=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}_LL84:
return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question();static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){const char*
_tmp689;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp689="{?}",_tag_dyneither(_tmp689,sizeof(char),4))),1);}else{const char*
_tmp68A;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68A="?",
_tag_dyneither(_tmp68A,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp68B;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp68B="{\\lb}",
_tag_dyneither(_tmp68B,sizeof(char),6))),1);}else{const char*_tmp68C;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68C="{",_tag_dyneither(_tmp68C,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp68D;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp68D="{\\rb}",_tag_dyneither(_tmp68D,sizeof(char),6))),1);}else{const char*
_tmp68E;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68E="}",
_tag_dyneither(_tmp68E,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp68F;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp68F="\\$",
_tag_dyneither(_tmp68F,sizeof(char),3))),1);}else{const char*_tmp690;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp690="$",_tag_dyneither(_tmp690,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp691[3];return(_tmp691[2]=Cyc_Absynpp_rb(),((_tmp691[1]=
Cyc_PP_seq(sep,ss),((_tmp691[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp691,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*
_tmpD7;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp694;void*_tmp693;(
_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="Carray_mod ",_tag_dyneither(_tmp694,
sizeof(char),12))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL8C;_LL8F: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp697;void*_tmp696;(
_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="ConstArray_mod ",_tag_dyneither(
_tmp697,sizeof(char),16))),_tag_dyneither(_tmp696,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp69A;void*_tmp699;(_tmp699=0,Cyc_fprintf(
Cyc_stderr,((_tmp69A="Function_mod(",_tag_dyneither(_tmp69A,sizeof(char),14))),
_tag_dyneither(_tmp699,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple7*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp69D;void*_tmp69C;(_tmp69C=0,Cyc_fprintf(Cyc_stderr,((_tmp69D="?",
_tag_dyneither(_tmp69D,sizeof(char),2))),_tag_dyneither(_tmp69C,sizeof(void*),0)));}
else{void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp69E,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,((_tmp6A1=",",
_tag_dyneither(_tmp6A1,sizeof(char),2))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}}{
const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4=") ",
_tag_dyneither(_tmp6A4,sizeof(char),3))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6A7;void*_tmp6A6;(
_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="Function_mod()",_tag_dyneither(
_tmp6A7,sizeof(char),15))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6AA;void*_tmp6A9;(
_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="Attributes_mod ",_tag_dyneither(
_tmp6AA,sizeof(char),16))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6AD;void*_tmp6AC;(
_tmp6AC=0,Cyc_fprintf(Cyc_stderr,((_tmp6AD="TypeParams_mod ",_tag_dyneither(
_tmp6AD,sizeof(char),16))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6B0;void*_tmp6AF;(
_tmp6AF=0,Cyc_fprintf(Cyc_stderr,((_tmp6B0="Pointer_mod ",_tag_dyneither(_tmp6B0,
sizeof(char),13))),_tag_dyneither(_tmp6AF,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6B3;void*_tmp6B2;(_tmp6B2=0,Cyc_fprintf(Cyc_stderr,((_tmp6B3="\n",
_tag_dyneither(_tmp6B3,sizeof(char),2))),_tag_dyneither(_tmp6B2,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6B8;const char*_tmp6B7;struct Cyc_PP_Doc*
_tmp6B6[3];struct Cyc_PP_Doc*p_rest=(_tmp6B6[2]=Cyc_PP_text(((_tmp6B7=")",
_tag_dyneither(_tmp6B7,sizeof(char),2)))),((_tmp6B6[1]=rest,((_tmp6B6[0]=Cyc_PP_text(((
_tmp6B8="(",_tag_dyneither(_tmp6B8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6B6,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6BD;const char*_tmp6BC;struct Cyc_PP_Doc*_tmp6BB[2];return(_tmp6BB[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6BC="[]@zeroterm",_tag_dyneither(_tmp6BC,sizeof(char),12)))): Cyc_PP_text(((
_tmp6BD="[]",_tag_dyneither(_tmp6BD,sizeof(char),3)))),((_tmp6BB[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6C4;const char*_tmp6C3;const char*_tmp6C2;
struct Cyc_PP_Doc*_tmp6C1[4];return(_tmp6C1[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6C2="]@zeroterm",
_tag_dyneither(_tmp6C2,sizeof(char),11)))): Cyc_PP_text(((_tmp6C3="]",
_tag_dyneither(_tmp6C3,sizeof(char),2)))),((_tmp6C1[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6C1[1]=Cyc_PP_text(((_tmp6C4="[",_tag_dyneither(_tmp6C4,sizeof(
char),2)))),((_tmp6C1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C1,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6C5[2];return(_tmp6C5[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6C5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C5,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6CC;const char*
_tmp6CB;const char*_tmp6CA;struct Cyc_PP_Doc*_tmp6C9[2];return(_tmp6C9[1]=Cyc_PP_group(((
_tmp6CC="(",_tag_dyneither(_tmp6CC,sizeof(char),2))),((_tmp6CB=")",
_tag_dyneither(_tmp6CB,sizeof(char),2))),((_tmp6CA=",",_tag_dyneither(_tmp6CA,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6C9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C9,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6CD[2];return(_tmp6CD[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6CD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CD,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6CE[2];return(_tmp6CE[1]=rest,((_tmp6CE[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6CE,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6CF[2];return(_tmp6CF[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6CF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CF,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6D0[2];return(_tmp6D0[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6D0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D0,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6D2;const char*_tmp6D1;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6D2="*",_tag_dyneither(_tmp6D2,sizeof(char),2)):((_tmp6D1="@",_tag_dyneither(
_tmp6D1,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple10
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp6D3[4];ptr=((_tmp6D3[
3]=Cyc_Absynpp_rb(),((_tmp6D3[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6D3[1]=Cyc_Absynpp_lb(),((
_tmp6D3[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6D3,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp6D6;struct Cyc_PP_Doc*_tmp6D5[2];ptr=((_tmp6D5[1]=Cyc_PP_text(((_tmp6D6="@zeroterm ",
_tag_dyneither(_tmp6D6,sizeof(char),11)))),((_tmp6D5[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6D5,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp6D9;struct Cyc_PP_Doc*_tmp6D8[2];ptr=((_tmp6D8[1]=Cyc_PP_text(((
_tmp6D9="@nozeroterm ",_tag_dyneither(_tmp6D9,sizeof(char),13)))),((_tmp6D8[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6D8,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp6DC;struct Cyc_PP_Doc*_tmp6DB[
3];ptr=((_tmp6DB[2]=Cyc_PP_text(((_tmp6DC=" ",_tag_dyneither(_tmp6DC,sizeof(char),
2)))),((_tmp6DB[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp6DB[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6DB,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp6DD[2];ptr=((_tmp6DD[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp6DD[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6DE[2];return(_tmp6DE[1]=rest,((_tmp6DE[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6DE,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp13B=Cyc_Tcutil_compress(
t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp13B;if(_tmp13C->tag != 21)goto _LLBF;}_LLBE: {const char*_tmp6DF;return Cyc_PP_text(((
_tmp6DF="`H",_tag_dyneither(_tmp6DF,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp13D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 22)goto
_LLC1;}_LLC0: {const char*_tmp6E0;return Cyc_PP_text(((_tmp6E0="`U",_tag_dyneither(
_tmp6E0,sizeof(char),3))));}_LLC1:;_LLC2: return Cyc_Absynpp_ntyp2doc(t);_LLBC:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp140=Cyc_Tcutil_compress(t);void*
_tmp142;struct Cyc_List_List*_tmp144;_LLC4: {struct Cyc_Absyn_AccessEff_struct*
_tmp141=(struct Cyc_Absyn_AccessEff_struct*)_tmp140;if(_tmp141->tag != 23)goto
_LLC6;else{_tmp142=(void*)_tmp141->f1;}}_LLC5:{struct Cyc_List_List*_tmp6E1;*
rgions=((_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1->hd=Cyc_Absynpp_rgn2doc(
_tmp142),((_tmp6E1->tl=*rgions,_tmp6E1))))));}goto _LLC3;_LLC6: {struct Cyc_Absyn_JoinEff_struct*
_tmp143=(struct Cyc_Absyn_JoinEff_struct*)_tmp140;if(_tmp143->tag != 24)goto _LLC8;
else{_tmp144=_tmp143->f1;}}_LLC7: for(0;_tmp144 != 0;_tmp144=_tmp144->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp144->hd);}goto _LLC3;_LLC8:;_LLC9:{struct Cyc_List_List*
_tmp6E2;*effects=((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6E2->tl=*effects,_tmp6E2))))));}goto _LLC3;_LLC3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6E5;const char*_tmp6E4;const
char*_tmp6E3;return Cyc_PP_group(((_tmp6E3="",_tag_dyneither(_tmp6E3,sizeof(char),
1))),((_tmp6E4="",_tag_dyneither(_tmp6E4,sizeof(char),1))),((_tmp6E5="+",
_tag_dyneither(_tmp6E5,sizeof(char),2))),effects);}else{const char*_tmp6E6;struct
Cyc_PP_Doc*_tmp14A=Cyc_Absynpp_group_braces(((_tmp6E6=",",_tag_dyneither(_tmp6E6,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6EA;const char*_tmp6E9;const
char*_tmp6E8;const char*_tmp6E7;return Cyc_PP_group(((_tmp6E7="",_tag_dyneither(
_tmp6E7,sizeof(char),1))),((_tmp6E8="",_tag_dyneither(_tmp6E8,sizeof(char),1))),((
_tmp6E9="+",_tag_dyneither(_tmp6E9,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA->hd=_tmp14A,((_tmp6EA->tl=0,
_tmp6EA))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLCA: {const char*_tmp6EB;return Cyc_PP_text(((_tmp6EB="struct ",
_tag_dyneither(_tmp6EB,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCB: {const
char*_tmp6EC;return Cyc_PP_text(((_tmp6EC="union ",_tag_dyneither(_tmp6EC,sizeof(
char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp152=t;struct Cyc_Core_Opt*
_tmp158;struct Cyc_Core_Opt*_tmp159;int _tmp15A;struct Cyc_Core_Opt*_tmp15B;struct
Cyc_Absyn_Tvar*_tmp15D;struct Cyc_Absyn_DatatypeInfo _tmp15F;union Cyc_Absyn_DatatypeInfoU
_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp163;
union Cyc_Absyn_DatatypeFieldInfoU _tmp164;struct Cyc_List_List*_tmp165;enum Cyc_Absyn_Sign
_tmp167;enum Cyc_Absyn_Size_of _tmp168;int _tmp16B;struct Cyc_List_List*_tmp16D;
struct Cyc_Absyn_AggrInfo _tmp16F;union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*
_tmp171;enum Cyc_Absyn_AggrKind _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*
_tmp176;struct _tuple0*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;
struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;
void*_tmp182;void*_tmp183;void*_tmp185;void*_tmp189;_LLCE: {struct Cyc_Absyn_ArrayType_struct*
_tmp153=(struct Cyc_Absyn_ArrayType_struct*)_tmp152;if(_tmp153->tag != 9)goto _LLD0;}
_LLCF: {const char*_tmp6ED;return Cyc_PP_text(((_tmp6ED="[[[array]]]",
_tag_dyneither(_tmp6ED,sizeof(char),12))));}_LLD0: {struct Cyc_Absyn_FnType_struct*
_tmp154=(struct Cyc_Absyn_FnType_struct*)_tmp152;if(_tmp154->tag != 10)goto _LLD2;}
_LLD1: return Cyc_PP_nil_doc();_LLD2: {struct Cyc_Absyn_PointerType_struct*_tmp155=(
struct Cyc_Absyn_PointerType_struct*)_tmp152;if(_tmp155->tag != 5)goto _LLD4;}_LLD3:
return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_VoidType_struct*_tmp156=(struct
Cyc_Absyn_VoidType_struct*)_tmp152;if(_tmp156->tag != 0)goto _LLD6;}_LLD5:{const
char*_tmp6EE;s=Cyc_PP_text(((_tmp6EE="void",_tag_dyneither(_tmp6EE,sizeof(char),
5))));}goto _LLCD;_LLD6: {struct Cyc_Absyn_Evar_struct*_tmp157=(struct Cyc_Absyn_Evar_struct*)
_tmp152;if(_tmp157->tag != 1)goto _LLD8;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LLD7: if(_tmp159 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp159->v);else{const char*_tmp6FE;const char*_tmp6FD;const char*_tmp6FC;
struct Cyc_Int_pa_struct _tmp6FB;void*_tmp6FA[1];const char*_tmp6F9;const char*
_tmp6F8;struct Cyc_PP_Doc*_tmp6F7[6];s=((_tmp6F7[5]=_tmp158 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp158->v),((_tmp6F7[4]=Cyc_PP_text(((
_tmp6F8=")::",_tag_dyneither(_tmp6F8,sizeof(char),4)))),((_tmp6F7[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp15B == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp15B->v),((_tmp6F7[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6FB.tag=1,((
_tmp6FB.f1=(unsigned long)_tmp15A,((_tmp6FA[0]=& _tmp6FB,Cyc_aprintf(((_tmp6F9="%d",
_tag_dyneither(_tmp6F9,sizeof(char),3))),_tag_dyneither(_tmp6FA,sizeof(void*),1))))))))),((
_tmp6F7[1]=Cyc_PP_text(((_tmp6FC="(",_tag_dyneither(_tmp6FC,sizeof(char),2)))),((
_tmp6F7[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6FD="\\%",
_tag_dyneither(_tmp6FD,sizeof(char),3)))): Cyc_PP_text(((_tmp6FE="%",
_tag_dyneither(_tmp6FE,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6F7,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: {struct Cyc_Absyn_VarType_struct*
_tmp15C=(struct Cyc_Absyn_VarType_struct*)_tmp152;if(_tmp15C->tag != 2)goto _LLDA;
else{_tmp15D=_tmp15C->f1;}}_LLD9: s=Cyc_PP_textptr(_tmp15D->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp701;struct Cyc_PP_Doc*_tmp700[3];s=((_tmp700[2]=Cyc_Absynpp_kindbound2doc(
_tmp15D->kind),((_tmp700[1]=Cyc_PP_text(((_tmp701="::",_tag_dyneither(_tmp701,
sizeof(char),3)))),((_tmp700[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp700,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15D)){const char*_tmp706;const char*_tmp705;struct Cyc_PP_Doc*_tmp704[3];s=((
_tmp704[2]=Cyc_PP_text(((_tmp705=" */",_tag_dyneither(_tmp705,sizeof(char),4)))),((
_tmp704[1]=s,((_tmp704[0]=Cyc_PP_text(((_tmp706="_ /* ",_tag_dyneither(_tmp706,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: {struct Cyc_Absyn_DatatypeType_struct*_tmp15E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp152;if(_tmp15E->tag != 3)goto _LLDC;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.datatype_info;
_tmp161=_tmp15F.targs;}}_LLDB:{union Cyc_Absyn_DatatypeInfoU _tmp19B=_tmp160;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp19C;struct _tuple0*_tmp19D;int _tmp19E;
struct Cyc_Absyn_Datatypedecl**_tmp19F;struct Cyc_Absyn_Datatypedecl*_tmp1A0;
struct Cyc_Absyn_Datatypedecl _tmp1A1;struct _tuple0*_tmp1A2;int _tmp1A3;_LL103: if((
_tmp19B.UnknownDatatype).tag != 1)goto _LL105;_tmp19C=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp19B.UnknownDatatype).val;_tmp19D=_tmp19C.name;_tmp19E=_tmp19C.is_extensible;
_LL104: _tmp1A2=_tmp19D;_tmp1A3=_tmp19E;goto _LL106;_LL105: if((_tmp19B.KnownDatatype).tag
!= 2)goto _LL102;_tmp19F=(struct Cyc_Absyn_Datatypedecl**)(_tmp19B.KnownDatatype).val;
_tmp1A0=*_tmp19F;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.name;_tmp1A3=_tmp1A1.is_extensible;
_LL106: {const char*_tmp707;struct Cyc_PP_Doc*_tmp1A4=Cyc_PP_text(((_tmp707="datatype ",
_tag_dyneither(_tmp707,sizeof(char),10))));const char*_tmp708;struct Cyc_PP_Doc*
_tmp1A5=_tmp1A3?Cyc_PP_text(((_tmp708="@extensible ",_tag_dyneither(_tmp708,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp709[4];s=((_tmp709[3]=
Cyc_Absynpp_tps2doc(_tmp161),((_tmp709[2]=Cyc_Absynpp_qvar2doc(_tmp1A2),((
_tmp709[1]=_tmp1A4,((_tmp709[0]=_tmp1A5,Cyc_PP_cat(_tag_dyneither(_tmp709,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp152;if(_tmp162->tag != 4)goto _LLDE;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
_tmp165=_tmp163.targs;}}_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1A9=_tmp164;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1AA;struct _tuple0*_tmp1AB;struct
_tuple0*_tmp1AC;int _tmp1AD;struct _tuple1 _tmp1AE;struct Cyc_Absyn_Datatypedecl*
_tmp1AF;struct Cyc_Absyn_Datatypedecl _tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;
struct Cyc_Absyn_Datatypefield*_tmp1B3;struct Cyc_Absyn_Datatypefield _tmp1B4;
struct _tuple0*_tmp1B5;_LL108: if((_tmp1A9.UnknownDatatypefield).tag != 1)goto
_LL10A;_tmp1AA=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1A9.UnknownDatatypefield).val;
_tmp1AB=_tmp1AA.datatype_name;_tmp1AC=_tmp1AA.field_name;_tmp1AD=_tmp1AA.is_extensible;
_LL109: _tmp1B1=_tmp1AB;_tmp1B2=_tmp1AD;_tmp1B5=_tmp1AC;goto _LL10B;_LL10A: if((
_tmp1A9.KnownDatatypefield).tag != 2)goto _LL107;_tmp1AE=(struct _tuple1)(_tmp1A9.KnownDatatypefield).val;
_tmp1AF=_tmp1AE.f1;_tmp1B0=*_tmp1AF;_tmp1B1=_tmp1B0.name;_tmp1B2=_tmp1B0.is_extensible;
_tmp1B3=_tmp1AE.f2;_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.name;_LL10B: {const char*
_tmp70B;const char*_tmp70A;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B2?(_tmp70B="@extensible datatype ",
_tag_dyneither(_tmp70B,sizeof(char),22)):((_tmp70A="datatype ",_tag_dyneither(
_tmp70A,sizeof(char),10))));{const char*_tmp70E;struct Cyc_PP_Doc*_tmp70D[4];s=((
_tmp70D[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp70D[2]=Cyc_PP_text(((_tmp70E=".",
_tag_dyneither(_tmp70E,sizeof(char),2)))),((_tmp70D[1]=Cyc_Absynpp_qvar2doc(
_tmp1B1),((_tmp70D[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp70D,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL107;}_LL107:;}goto _LLCD;_LLDE: {struct Cyc_Absyn_IntType_struct*
_tmp166=(struct Cyc_Absyn_IntType_struct*)_tmp152;if(_tmp166->tag != 6)goto _LLE0;
else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp167){case Cyc_Absyn_None: _LL10C: goto _LL10D;case
Cyc_Absyn_Signed: _LL10D:{const char*_tmp70F;sns=((_tmp70F="",_tag_dyneither(
_tmp70F,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL10E:{const char*
_tmp710;sns=((_tmp710="unsigned ",_tag_dyneither(_tmp710,sizeof(char),10)));}
break;}switch(_tmp168){case Cyc_Absyn_Char_sz: _LL110: switch(_tmp167){case Cyc_Absyn_None:
_LL112:{const char*_tmp711;sns=((_tmp711="",_tag_dyneither(_tmp711,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL113:{const char*_tmp712;sns=((_tmp712="signed ",
_tag_dyneither(_tmp712,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL114:{
const char*_tmp713;sns=((_tmp713="unsigned ",_tag_dyneither(_tmp713,sizeof(char),
10)));}break;}{const char*_tmp714;ts=((_tmp714="char",_tag_dyneither(_tmp714,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL111:{const char*_tmp715;ts=((
_tmp715="short",_tag_dyneither(_tmp715,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL116:{const char*_tmp716;ts=((_tmp716="int",_tag_dyneither(_tmp716,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL117:{const char*_tmp717;ts=((_tmp717="long",
_tag_dyneither(_tmp717,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL118:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL11A:{const char*_tmp718;
ts=((_tmp718="long long",_tag_dyneither(_tmp718,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL11B:{const char*_tmp719;ts=((_tmp719="__int64",_tag_dyneither(
_tmp719,sizeof(char),8)));}break;}break;}{const char*_tmp71E;void*_tmp71D[2];
struct Cyc_String_pa_struct _tmp71C;struct Cyc_String_pa_struct _tmp71B;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp71B.tag=0,((_tmp71B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp71D[0]=& _tmp71C,((_tmp71D[1]=& _tmp71B,Cyc_aprintf(((
_tmp71E="%s%s",_tag_dyneither(_tmp71E,sizeof(char),5))),_tag_dyneither(_tmp71D,
sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: {struct Cyc_Absyn_FloatType_struct*
_tmp169=(struct Cyc_Absyn_FloatType_struct*)_tmp152;if(_tmp169->tag != 7)goto _LLE2;}
_LLE1:{const char*_tmp71F;s=Cyc_PP_text(((_tmp71F="float",_tag_dyneither(_tmp71F,
sizeof(char),6))));}goto _LLCD;_LLE2: {struct Cyc_Absyn_DoubleType_struct*_tmp16A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp152;if(_tmp16A->tag != 8)goto _LLE4;else{
_tmp16B=_tmp16A->f1;}}_LLE3: if(_tmp16B){const char*_tmp720;s=Cyc_PP_text(((
_tmp720="long double",_tag_dyneither(_tmp720,sizeof(char),12))));}else{const char*
_tmp721;s=Cyc_PP_text(((_tmp721="double",_tag_dyneither(_tmp721,sizeof(char),7))));}
goto _LLCD;_LLE4: {struct Cyc_Absyn_TupleType_struct*_tmp16C=(struct Cyc_Absyn_TupleType_struct*)
_tmp152;if(_tmp16C->tag != 11)goto _LLE6;else{_tmp16D=_tmp16C->f1;}}_LLE5:{struct
Cyc_PP_Doc*_tmp722[2];s=((_tmp722[1]=Cyc_Absynpp_args2doc(_tmp16D),((_tmp722[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp722,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCD;_LLE6: {struct Cyc_Absyn_AggrType_struct*_tmp16E=(struct Cyc_Absyn_AggrType_struct*)
_tmp152;if(_tmp16E->tag != 12)goto _LLE8;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.aggr_info;
_tmp171=_tmp16F.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*
_tmp1D0;struct _tuple9 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;
_tmp1D0=_tmp1CE.f2;{struct Cyc_PP_Doc*_tmp723[3];s=((_tmp723[2]=Cyc_Absynpp_tps2doc(
_tmp171),((_tmp723[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp723[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp723,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp152;if(_tmp172->tag != 13)goto _LLEA;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp724[4];s=((_tmp724[3]=Cyc_Absynpp_rb(),((_tmp724[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp724[1]=Cyc_Absynpp_lb(),((
_tmp724[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp724,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp175=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp152;if(_tmp175->tag != 15)goto
_LLEC;else{_tmp176=_tmp175->f1;}}_LLEB:{const char*_tmp727;struct Cyc_PP_Doc*
_tmp726[4];s=((_tmp726[3]=Cyc_Absynpp_rb(),((_tmp726[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp176)),((_tmp726[1]=Cyc_Absynpp_lb(),((_tmp726[0]=Cyc_PP_text(((_tmp727="enum ",
_tag_dyneither(_tmp727,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp726,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp177=(struct Cyc_Absyn_EnumType_struct*)_tmp152;if(_tmp177->tag != 14)goto _LLEE;
else{_tmp178=_tmp177->f1;}}_LLED:{const char*_tmp72A;struct Cyc_PP_Doc*_tmp729[2];
s=((_tmp729[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp729[0]=Cyc_PP_text(((_tmp72A="enum ",
_tag_dyneither(_tmp72A,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp729,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp179=(struct Cyc_Absyn_ValueofType_struct*)_tmp152;if(_tmp179->tag != 19)goto
_LLF0;else{_tmp17A=_tmp179->f1;}}_LLEF:{const char*_tmp72F;const char*_tmp72E;
struct Cyc_PP_Doc*_tmp72D[3];s=((_tmp72D[2]=Cyc_PP_text(((_tmp72E=")",
_tag_dyneither(_tmp72E,sizeof(char),2)))),((_tmp72D[1]=Cyc_Absynpp_exp2doc(
_tmp17A),((_tmp72D[0]=Cyc_PP_text(((_tmp72F="valueof_t(",_tag_dyneither(_tmp72F,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp17B=(struct
Cyc_Absyn_TypedefType_struct*)_tmp152;if(_tmp17B->tag != 18)goto _LLF2;else{
_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp730[2];s=((_tmp730[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp730[0]=Cyc_Absynpp_qvar2doc(
_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCD;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp152;if(_tmp17F->tag != 16)goto _LLF4;else{_tmp180=(void*)_tmp17F->f1;}}_LLF3:{
const char*_tmp735;const char*_tmp734;struct Cyc_PP_Doc*_tmp733[3];s=((_tmp733[2]=
Cyc_PP_text(((_tmp734=">",_tag_dyneither(_tmp734,sizeof(char),2)))),((_tmp733[1]=
Cyc_Absynpp_rgn2doc(_tmp180),((_tmp733[0]=Cyc_PP_text(((_tmp735="region_t<",
_tag_dyneither(_tmp735,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp733,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp181=(struct Cyc_Absyn_DynRgnType_struct*)_tmp152;if(_tmp181->tag != 17)goto
_LLF6;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF5:{const
char*_tmp73C;const char*_tmp73B;const char*_tmp73A;struct Cyc_PP_Doc*_tmp739[5];s=((
_tmp739[4]=Cyc_PP_text(((_tmp73A=">",_tag_dyneither(_tmp73A,sizeof(char),2)))),((
_tmp739[3]=Cyc_Absynpp_rgn2doc(_tmp183),((_tmp739[2]=Cyc_PP_text(((_tmp73B=",",
_tag_dyneither(_tmp73B,sizeof(char),2)))),((_tmp739[1]=Cyc_Absynpp_rgn2doc(
_tmp182),((_tmp739[0]=Cyc_PP_text(((_tmp73C="dynregion_t<",_tag_dyneither(
_tmp73C,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp739,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp184=(struct
Cyc_Absyn_TagType_struct*)_tmp152;if(_tmp184->tag != 20)goto _LLF8;else{_tmp185=(
void*)_tmp184->f1;}}_LLF7:{const char*_tmp741;const char*_tmp740;struct Cyc_PP_Doc*
_tmp73F[3];s=((_tmp73F[2]=Cyc_PP_text(((_tmp740=">",_tag_dyneither(_tmp740,
sizeof(char),2)))),((_tmp73F[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp73F[0]=Cyc_PP_text(((
_tmp741="tag_t<",_tag_dyneither(_tmp741,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp73F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp152;if(_tmp186->tag != 22)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp187=(struct Cyc_Absyn_HeapRgn_struct*)_tmp152;if(_tmp187->tag != 21)goto _LLFC;}
_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp188=(struct Cyc_Absyn_RgnsEff_struct*)_tmp152;if(_tmp188->tag != 25)goto _LLFE;
else{_tmp189=(void*)_tmp188->f1;}}_LLFD:{const char*_tmp746;const char*_tmp745;
struct Cyc_PP_Doc*_tmp744[3];s=((_tmp744[2]=Cyc_PP_text(((_tmp745=")",
_tag_dyneither(_tmp745,sizeof(char),2)))),((_tmp744[1]=Cyc_Absynpp_typ2doc(
_tmp189),((_tmp744[0]=Cyc_PP_text(((_tmp746="regions(",_tag_dyneither(_tmp746,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: {struct Cyc_Absyn_AccessEff_struct*_tmp18A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp152;if(_tmp18A->tag != 23)goto _LL100;}_LLFF: goto _LL101;_LL100: {struct Cyc_Absyn_JoinEff_struct*
_tmp18B=(struct Cyc_Absyn_JoinEff_struct*)_tmp152;if(_tmp18B->tag != 24)goto _LLCD;}
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple16*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16 _tmp1E9;void*_tmp1EA;void*_tmp1EB;struct _tuple16*_tmp1E8=cmp;
_tmp1E9=*_tmp1E8;_tmp1EA=_tmp1E9.f1;_tmp1EB=_tmp1E9.f2;{const char*_tmp749;struct
Cyc_PP_Doc*_tmp748[3];return(_tmp748[2]=Cyc_Absynpp_rgn2doc(_tmp1EB),((_tmp748[1]=
Cyc_PP_text(((_tmp749=" > ",_tag_dyneither(_tmp749,sizeof(char),4)))),((_tmp748[
0]=Cyc_Absynpp_rgn2doc(_tmp1EA),Cyc_PP_cat(_tag_dyneither(_tmp748,sizeof(struct
Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp74C;const char*_tmp74B;const char*_tmp74A;return Cyc_PP_group(((_tmp74A="",
_tag_dyneither(_tmp74A,sizeof(char),1))),((_tmp74B="",_tag_dyneither(_tmp74B,
sizeof(char),1))),((_tmp74C=",",_tag_dyneither(_tmp74C,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp74D;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp74D=_cycalloc(
sizeof(*_tmp74D)),((_tmp74D->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp74D))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp750;struct Cyc_List_List*
_tmp74F;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F->hd=
Cyc_PP_text(((_tmp750="...",_tag_dyneither(_tmp750,sizeof(char),4)))),((_tmp74F->tl=
0,_tmp74F)))))));}else{if(cyc_varargs != 0){const char*_tmp759;const char*_tmp758;
const char*_tmp757;struct _tuple7*_tmp756;struct Cyc_PP_Doc*_tmp755[3];struct Cyc_PP_Doc*
_tmp1F5=(_tmp755[2]=Cyc_Absynpp_funarg2doc(((_tmp756=_cycalloc(sizeof(*_tmp756)),((
_tmp756->f1=cyc_varargs->name,((_tmp756->f2=cyc_varargs->tq,((_tmp756->f3=
cyc_varargs->type,_tmp756))))))))),((_tmp755[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp757=" inject ",_tag_dyneither(_tmp757,sizeof(char),9)))): Cyc_PP_text(((
_tmp758=" ",_tag_dyneither(_tmp758,sizeof(char),2)))),((_tmp755[0]=Cyc_PP_text(((
_tmp759="...",_tag_dyneither(_tmp759,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp755,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp75A;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp75A=_cycalloc(sizeof(*_tmp75A)),((_tmp75A->hd=
_tmp1F5,((_tmp75A->tl=0,_tmp75A)))))));}}{const char*_tmp75D;const char*_tmp75C;
const char*_tmp75B;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp75B="",
_tag_dyneither(_tmp75B,sizeof(char),1))),((_tmp75C="",_tag_dyneither(_tmp75C,
sizeof(char),1))),((_tmp75D=",",_tag_dyneither(_tmp75D,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp760;struct Cyc_PP_Doc*
_tmp75F[3];_tmp1FC=((_tmp75F[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp75F[
1]=Cyc_PP_text(((_tmp760=";",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75F[
0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp763;struct Cyc_PP_Doc*_tmp762[3];_tmp1FC=((_tmp762[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp762[1]=Cyc_PP_text(((_tmp763=":",
_tag_dyneither(_tmp763,sizeof(char),2)))),((_tmp762[0]=_tmp1FC,Cyc_PP_cat(
_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp768;
const char*_tmp767;struct Cyc_PP_Doc*_tmp766[3];return(_tmp766[2]=Cyc_PP_text(((
_tmp767=")",_tag_dyneither(_tmp767,sizeof(char),2)))),((_tmp766[1]=_tmp1FC,((
_tmp766[0]=Cyc_PP_text(((_tmp768="(",_tag_dyneither(_tmp768,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){struct Cyc_Core_Opt*_tmp76B;struct _tuple7*_tmp76A;return(
_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A->f1=((_tmp76B=_cycalloc(sizeof(*
_tmp76B)),((_tmp76B->v=(*arg).f1,_tmp76B)))),((_tmp76A->f2=(*arg).f2,((_tmp76A->f3=(*
arg).f3,_tmp76A)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp209=0;int match;{union Cyc_Absyn_Nmspace _tmp20A=(*q).f1;int _tmp20B;struct Cyc_List_List*
_tmp20C;struct Cyc_List_List*_tmp20D;_LL11E: if((_tmp20A.Loc_n).tag != 3)goto _LL120;
_tmp20B=(int)(_tmp20A.Loc_n).val;_LL11F: _tmp20C=0;goto _LL121;_LL120: if((_tmp20A.Rel_n).tag
!= 1)goto _LL122;_tmp20C=(struct Cyc_List_List*)(_tmp20A.Rel_n).val;_LL121: match=0;
_tmp209=_tmp20C;goto _LL11D;_LL122: if((_tmp20A.Abs_n).tag != 2)goto _LL11D;_tmp20D=(
struct Cyc_List_List*)(_tmp20A.Abs_n).val;_LL123: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20D,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp76C;_tmp209=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp76C=_cycalloc(sizeof(*_tmp76C)),((_tmp76C->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp76C->tl=_tmp20D,_tmp76C))))): _tmp20D;}goto _LL11D;_LL11D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp76E;struct Cyc_List_List*_tmp76D;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp209,((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D->hd=(*q).f2,((_tmp76D->tl=
0,_tmp76D))))))),((_tmp76E="_",_tag_dyneither(_tmp76E,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp770;struct Cyc_List_List*_tmp76F;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp209,((_tmp76F=_cycalloc(sizeof(*
_tmp76F)),((_tmp76F->hd=(*q).f2,((_tmp76F->tl=0,_tmp76F))))))),((_tmp770="::",
_tag_dyneither(_tmp770,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp213=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp772;const char*_tmp771;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp771="\\textbf{",
_tag_dyneither(_tmp771,sizeof(char),9))),(struct _dyneither_ptr)_tmp213),((
_tmp772="}",_tag_dyneither(_tmp772,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp213));}else{return Cyc_PP_text(_tmp213);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp216=(*v).f1;int
_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;_LL125: if((
_tmp216.Loc_n).tag != 3)goto _LL127;_tmp217=(int)(_tmp216.Loc_n).val;_LL126: goto
_LL128;_LL127: if((_tmp216.Rel_n).tag != 1)goto _LL129;_tmp218=(struct Cyc_List_List*)(
_tmp216.Rel_n).val;if(_tmp218 != 0)goto _LL129;_LL128: return*(*v).f2;_LL129: if((
_tmp216.Abs_n).tag != 2)goto _LL12B;_tmp219=(struct Cyc_List_List*)(_tmp216.Abs_n).val;
_LL12A: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp219,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL12C;}_LL12B:;
_LL12C: {const char*_tmp773;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp773="/* bad namespace : */ ",
_tag_dyneither(_tmp773,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL124:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp21B=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp775;const char*_tmp774;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp774="\\textbf{",
_tag_dyneither(_tmp774,sizeof(char),9))),(struct _dyneither_ptr)_tmp21B),((
_tmp775="}",_tag_dyneither(_tmp775,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp21B));}else{return Cyc_PP_text(_tmp21B);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp21E=e->r;enum Cyc_Absyn_Primop
_tmp223;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp230;_LL12E: {struct
Cyc_Absyn_Const_e_struct*_tmp21F=(struct Cyc_Absyn_Const_e_struct*)_tmp21E;if(
_tmp21F->tag != 0)goto _LL130;}_LL12F: goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp220=(struct Cyc_Absyn_Var_e_struct*)_tmp21E;if(_tmp220->tag != 1)goto _LL132;}
_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnknownId_e_struct*_tmp221=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp21E;if(_tmp221->tag != 2)goto _LL134;}_LL133: return 10000;_LL134: {struct Cyc_Absyn_Primop_e_struct*
_tmp222=(struct Cyc_Absyn_Primop_e_struct*)_tmp21E;if(_tmp222->tag != 3)goto _LL136;
else{_tmp223=_tmp222->f1;}}_LL135: switch(_tmp223){case Cyc_Absyn_Plus: _LL17E:
return 100;case Cyc_Absyn_Times: _LL17F: return 110;case Cyc_Absyn_Minus: _LL180: return
100;case Cyc_Absyn_Div: _LL181: goto _LL182;case Cyc_Absyn_Mod: _LL182: return 110;case
Cyc_Absyn_Eq: _LL183: goto _LL184;case Cyc_Absyn_Neq: _LL184: return 70;case Cyc_Absyn_Gt:
_LL185: goto _LL186;case Cyc_Absyn_Lt: _LL186: goto _LL187;case Cyc_Absyn_Gte: _LL187:
goto _LL188;case Cyc_Absyn_Lte: _LL188: return 80;case Cyc_Absyn_Not: _LL189: goto _LL18A;
case Cyc_Absyn_Bitnot: _LL18A: return 130;case Cyc_Absyn_Bitand: _LL18B: return 60;case
Cyc_Absyn_Bitor: _LL18C: return 40;case Cyc_Absyn_Bitxor: _LL18D: return 50;case Cyc_Absyn_Bitlshift:
_LL18E: return 90;case Cyc_Absyn_Bitlrshift: _LL18F: return 80;case Cyc_Absyn_Bitarshift:
_LL190: return 80;case Cyc_Absyn_Numelts: _LL191: return 140;}_LL136: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp224=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp21E;if(_tmp224->tag != 4)goto
_LL138;}_LL137: return 20;_LL138: {struct Cyc_Absyn_Increment_e_struct*_tmp225=(
struct Cyc_Absyn_Increment_e_struct*)_tmp21E;if(_tmp225->tag != 5)goto _LL13A;}
_LL139: return 130;_LL13A: {struct Cyc_Absyn_Conditional_e_struct*_tmp226=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp21E;if(_tmp226->tag != 6)goto _LL13C;}_LL13B:
return 30;_LL13C: {struct Cyc_Absyn_And_e_struct*_tmp227=(struct Cyc_Absyn_And_e_struct*)
_tmp21E;if(_tmp227->tag != 7)goto _LL13E;}_LL13D: return 35;_LL13E: {struct Cyc_Absyn_Or_e_struct*
_tmp228=(struct Cyc_Absyn_Or_e_struct*)_tmp21E;if(_tmp228->tag != 8)goto _LL140;}
_LL13F: return 30;_LL140: {struct Cyc_Absyn_SeqExp_e_struct*_tmp229=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp21E;if(_tmp229->tag != 9)goto _LL142;}_LL141: return 10;_LL142: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp22A=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp21E;if(_tmp22A->tag != 10)goto
_LL144;}_LL143: goto _LL145;_LL144: {struct Cyc_Absyn_FnCall_e_struct*_tmp22B=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp21E;if(_tmp22B->tag != 11)goto _LL146;}_LL145:
return 140;_LL146: {struct Cyc_Absyn_Throw_e_struct*_tmp22C=(struct Cyc_Absyn_Throw_e_struct*)
_tmp21E;if(_tmp22C->tag != 12)goto _LL148;}_LL147: return 130;_LL148: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp22D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp21E;if(_tmp22D->tag != 13)
goto _LL14A;else{_tmp22E=_tmp22D->f1;}}_LL149: return Cyc_Absynpp_exp_prec(_tmp22E);
_LL14A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp22F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp21E;if(_tmp22F->tag != 14)goto _LL14C;else{_tmp230=_tmp22F->f1;}}_LL14B: return
Cyc_Absynpp_exp_prec(_tmp230);_LL14C: {struct Cyc_Absyn_Cast_e_struct*_tmp231=(
struct Cyc_Absyn_Cast_e_struct*)_tmp21E;if(_tmp231->tag != 15)goto _LL14E;}_LL14D:
return 120;_LL14E: {struct Cyc_Absyn_New_e_struct*_tmp232=(struct Cyc_Absyn_New_e_struct*)
_tmp21E;if(_tmp232->tag != 17)goto _LL150;}_LL14F: return 15;_LL150: {struct Cyc_Absyn_Address_e_struct*
_tmp233=(struct Cyc_Absyn_Address_e_struct*)_tmp21E;if(_tmp233->tag != 16)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp234=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp21E;if(_tmp234->tag != 18)goto _LL154;}
_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp235=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp21E;if(_tmp235->tag != 19)goto _LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Valueof_e_struct*
_tmp236=(struct Cyc_Absyn_Valueof_e_struct*)_tmp21E;if(_tmp236->tag != 39)goto
_LL158;}_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp237=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp21E;if(_tmp237->tag != 38)goto _LL15A;}
_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Offsetof_e_struct*_tmp238=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp21E;if(_tmp238->tag != 20)goto _LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Deref_e_struct*
_tmp239=(struct Cyc_Absyn_Deref_e_struct*)_tmp21E;if(_tmp239->tag != 21)goto _LL15E;}
_LL15D: return 130;_LL15E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp23A=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp21E;if(_tmp23A->tag != 22)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp23B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp21E;if(_tmp23B->tag != 23)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Subscript_e_struct*_tmp23C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp21E;if(_tmp23C->tag != 24)goto _LL164;}
_LL163: return 140;_LL164: {struct Cyc_Absyn_Tuple_e_struct*_tmp23D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp21E;if(_tmp23D->tag != 25)goto _LL166;}_LL165: return 150;_LL166: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp23E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp21E;if(_tmp23E->tag != 26)goto
_LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Array_e_struct*_tmp23F=(
struct Cyc_Absyn_Array_e_struct*)_tmp21E;if(_tmp23F->tag != 27)goto _LL16A;}_LL169:
goto _LL16B;_LL16A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp240=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp21E;if(_tmp240->tag != 28)goto _LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp241=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp21E;if(_tmp241->tag != 29)goto
_LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp242=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp21E;if(_tmp242->tag != 30)goto _LL170;}
_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_Datatype_e_struct*_tmp243=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp21E;if(_tmp243->tag != 31)goto _LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Enum_e_struct*
_tmp244=(struct Cyc_Absyn_Enum_e_struct*)_tmp21E;if(_tmp244->tag != 32)goto _LL174;}
_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp245=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp21E;if(_tmp245->tag != 33)goto _LL176;}_LL175: goto _LL177;_LL176: {struct Cyc_Absyn_Malloc_e_struct*
_tmp246=(struct Cyc_Absyn_Malloc_e_struct*)_tmp21E;if(_tmp246->tag != 34)goto
_LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Swap_e_struct*_tmp247=(struct
Cyc_Absyn_Swap_e_struct*)_tmp21E;if(_tmp247->tag != 35)goto _LL17A;}_LL179: goto
_LL17B;_LL17A: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp248=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp21E;if(_tmp248->tag != 36)goto _LL17C;}_LL17B: return 140;_LL17C: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp249=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp21E;if(_tmp249->tag != 37)goto
_LL12D;}_LL17D: return 10000;_LL12D:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp24A=e->r;union Cyc_Absyn_Cnst _tmp24C;struct _tuple0*_tmp24E;struct
_tuple0*_tmp250;enum Cyc_Absyn_Primop _tmp252;struct Cyc_List_List*_tmp253;struct
Cyc_Absyn_Exp*_tmp255;struct Cyc_Core_Opt*_tmp256;struct Cyc_Absyn_Exp*_tmp257;
struct Cyc_Absyn_Exp*_tmp259;enum Cyc_Absyn_Incrementor _tmp25A;struct Cyc_Absyn_Exp*
_tmp25C;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*
_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*
_tmp264;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*
_tmp269;struct Cyc_List_List*_tmp26A;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_List_List*
_tmp26D;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*
_tmp273;void*_tmp275;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp278;
struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp27B;void*_tmp27D;struct Cyc_Absyn_Exp*
_tmp27F;void*_tmp281;struct Cyc_Absyn_Exp*_tmp283;struct _dyneither_ptr*_tmp284;
void*_tmp286;void*_tmp287;struct _dyneither_ptr*_tmp289;void*_tmp28B;void*_tmp28C;
unsigned int _tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp292;
struct _dyneither_ptr*_tmp293;struct Cyc_Absyn_Exp*_tmp295;struct _dyneither_ptr*
_tmp296;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_List_List*
_tmp29B;struct _tuple7*_tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*
_tmp2A0;struct Cyc_Absyn_Vardecl*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*
_tmp2A4;struct _tuple0*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*
_tmp2A8;struct Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_Datatypefield*
_tmp2AD;struct _tuple0*_tmp2AF;struct _tuple0*_tmp2B1;struct Cyc_Absyn_MallocInfo
_tmp2B3;int _tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;void**_tmp2B6;struct Cyc_Absyn_Exp*
_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Core_Opt*
_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_Stmt*_tmp2BF;_LL194: {struct
Cyc_Absyn_Const_e_struct*_tmp24B=(struct Cyc_Absyn_Const_e_struct*)_tmp24A;if(
_tmp24B->tag != 0)goto _LL196;else{_tmp24C=_tmp24B->f1;}}_LL195: s=Cyc_Absynpp_cnst2doc(
_tmp24C);goto _LL193;_LL196: {struct Cyc_Absyn_Var_e_struct*_tmp24D=(struct Cyc_Absyn_Var_e_struct*)
_tmp24A;if(_tmp24D->tag != 1)goto _LL198;else{_tmp24E=_tmp24D->f1;}}_LL197: _tmp250=
_tmp24E;goto _LL199;_LL198: {struct Cyc_Absyn_UnknownId_e_struct*_tmp24F=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp24A;if(_tmp24F->tag != 2)goto _LL19A;else{
_tmp250=_tmp24F->f1;}}_LL199: s=Cyc_Absynpp_qvar2doc(_tmp250);goto _LL193;_LL19A: {
struct Cyc_Absyn_Primop_e_struct*_tmp251=(struct Cyc_Absyn_Primop_e_struct*)
_tmp24A;if(_tmp251->tag != 3)goto _LL19C;else{_tmp252=_tmp251->f1;_tmp253=_tmp251->f2;}}
_LL19B: s=Cyc_Absynpp_primapp2doc(myprec,_tmp252,_tmp253);goto _LL193;_LL19C: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp254=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp24A;if(_tmp254->tag != 4)goto _LL19E;else{_tmp255=_tmp254->f1;_tmp256=_tmp254->f2;
_tmp257=_tmp254->f3;}}_LL19D:{const char*_tmp77A;const char*_tmp779;struct Cyc_PP_Doc*
_tmp778[5];s=((_tmp778[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp257),((_tmp778[3]=
Cyc_PP_text(((_tmp779="= ",_tag_dyneither(_tmp779,sizeof(char),3)))),((_tmp778[2]=
_tmp256 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp256->v),((
_tmp778[1]=Cyc_PP_text(((_tmp77A=" ",_tag_dyneither(_tmp77A,sizeof(char),2)))),((
_tmp778[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp255),Cyc_PP_cat(_tag_dyneither(
_tmp778,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;_LL19E: {struct Cyc_Absyn_Increment_e_struct*
_tmp258=(struct Cyc_Absyn_Increment_e_struct*)_tmp24A;if(_tmp258->tag != 5)goto
_LL1A0;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}_LL19F: {struct Cyc_PP_Doc*
_tmp2C3=Cyc_Absynpp_exp2doc_prec(myprec,_tmp259);switch(_tmp25A){case Cyc_Absyn_PreInc:
_LL1E6:{const char*_tmp77D;struct Cyc_PP_Doc*_tmp77C[2];s=((_tmp77C[1]=_tmp2C3,((
_tmp77C[0]=Cyc_PP_text(((_tmp77D="++",_tag_dyneither(_tmp77D,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp77C,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1E7:{const char*_tmp780;struct Cyc_PP_Doc*_tmp77F[2];s=((
_tmp77F[1]=_tmp2C3,((_tmp77F[0]=Cyc_PP_text(((_tmp780="--",_tag_dyneither(
_tmp780,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp77F,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1E8:{const char*_tmp783;struct Cyc_PP_Doc*
_tmp782[2];s=((_tmp782[1]=Cyc_PP_text(((_tmp783="++",_tag_dyneither(_tmp783,
sizeof(char),3)))),((_tmp782[0]=_tmp2C3,Cyc_PP_cat(_tag_dyneither(_tmp782,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1E9:{const char*
_tmp786;struct Cyc_PP_Doc*_tmp785[2];s=((_tmp785[1]=Cyc_PP_text(((_tmp786="--",
_tag_dyneither(_tmp786,sizeof(char),3)))),((_tmp785[0]=_tmp2C3,Cyc_PP_cat(
_tag_dyneither(_tmp785,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL193;}
_LL1A0: {struct Cyc_Absyn_Conditional_e_struct*_tmp25B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp24A;if(_tmp25B->tag != 6)goto _LL1A2;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;
_tmp25E=_tmp25B->f3;}}_LL1A1:{const char*_tmp78B;const char*_tmp78A;struct Cyc_PP_Doc*
_tmp789[5];s=((_tmp789[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25E),((_tmp789[3]=
Cyc_PP_text(((_tmp78A=" : ",_tag_dyneither(_tmp78A,sizeof(char),4)))),((_tmp789[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp25D),((_tmp789[1]=Cyc_PP_text(((_tmp78B=" ? ",
_tag_dyneither(_tmp78B,sizeof(char),4)))),((_tmp789[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25C),Cyc_PP_cat(_tag_dyneither(_tmp789,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL193;_LL1A2: {struct Cyc_Absyn_And_e_struct*_tmp25F=(struct Cyc_Absyn_And_e_struct*)
_tmp24A;if(_tmp25F->tag != 7)goto _LL1A4;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}
_LL1A3:{const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[3];s=((_tmp78D[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp261),((_tmp78D[1]=Cyc_PP_text(((_tmp78E=" && ",_tag_dyneither(_tmp78E,
sizeof(char),5)))),((_tmp78D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),Cyc_PP_cat(
_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1A4: {
struct Cyc_Absyn_Or_e_struct*_tmp262=(struct Cyc_Absyn_Or_e_struct*)_tmp24A;if(
_tmp262->tag != 8)goto _LL1A6;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}
_LL1A5:{const char*_tmp791;struct Cyc_PP_Doc*_tmp790[3];s=((_tmp790[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp264),((_tmp790[1]=Cyc_PP_text(((_tmp791=" || ",_tag_dyneither(_tmp791,
sizeof(char),5)))),((_tmp790[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp263),Cyc_PP_cat(
_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1A6: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp265=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp24A;if(_tmp265->tag != 9)goto _LL1A8;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}
_LL1A7:{const char*_tmp798;const char*_tmp797;const char*_tmp796;struct Cyc_PP_Doc*
_tmp795[5];s=((_tmp795[4]=Cyc_PP_text(((_tmp796=")",_tag_dyneither(_tmp796,
sizeof(char),2)))),((_tmp795[3]=Cyc_Absynpp_exp2doc(_tmp267),((_tmp795[2]=Cyc_PP_text(((
_tmp797=", ",_tag_dyneither(_tmp797,sizeof(char),3)))),((_tmp795[1]=Cyc_Absynpp_exp2doc(
_tmp266),((_tmp795[0]=Cyc_PP_text(((_tmp798="(",_tag_dyneither(_tmp798,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp795,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL193;_LL1A8: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp268=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp24A;if(_tmp268->tag != 10)goto _LL1AA;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}
_LL1A9:{const char*_tmp79D;const char*_tmp79C;struct Cyc_PP_Doc*_tmp79B[4];s=((
_tmp79B[3]=Cyc_PP_text(((_tmp79C=")",_tag_dyneither(_tmp79C,sizeof(char),2)))),((
_tmp79B[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26A),((_tmp79B[1]=Cyc_PP_text(((
_tmp79D="(",_tag_dyneither(_tmp79D,sizeof(char),2)))),((_tmp79B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp269),Cyc_PP_cat(_tag_dyneither(_tmp79B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1AA: {struct Cyc_Absyn_FnCall_e_struct*_tmp26B=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp24A;if(_tmp26B->tag != 11)goto _LL1AC;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26B->f2;}}
_LL1AB:{const char*_tmp7A2;const char*_tmp7A1;struct Cyc_PP_Doc*_tmp7A0[4];s=((
_tmp7A0[3]=Cyc_PP_text(((_tmp7A1=")",_tag_dyneither(_tmp7A1,sizeof(char),2)))),((
_tmp7A0[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26D),((_tmp7A0[1]=Cyc_PP_text(((
_tmp7A2="(",_tag_dyneither(_tmp7A2,sizeof(char),2)))),((_tmp7A0[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26C),Cyc_PP_cat(_tag_dyneither(_tmp7A0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1AC: {struct Cyc_Absyn_Throw_e_struct*_tmp26E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp24A;if(_tmp26E->tag != 12)goto _LL1AE;else{_tmp26F=_tmp26E->f1;}}_LL1AD:{const
char*_tmp7A5;struct Cyc_PP_Doc*_tmp7A4[2];s=((_tmp7A4[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26F),((_tmp7A4[0]=Cyc_PP_text(((_tmp7A5="throw ",_tag_dyneither(
_tmp7A5,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7A4,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL193;_LL1AE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp270=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp24A;if(_tmp270->tag != 13)goto _LL1B0;
else{_tmp271=_tmp270->f1;}}_LL1AF: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp271);
goto _LL193;_LL1B0: {struct Cyc_Absyn_Instantiate_e_struct*_tmp272=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp24A;if(_tmp272->tag != 14)goto _LL1B2;else{_tmp273=_tmp272->f1;}}_LL1B1: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp273);goto _LL193;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp274=(
struct Cyc_Absyn_Cast_e_struct*)_tmp24A;if(_tmp274->tag != 15)goto _LL1B4;else{
_tmp275=(void*)_tmp274->f1;_tmp276=_tmp274->f2;}}_LL1B3:{const char*_tmp7AA;const
char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[4];s=((_tmp7A8[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp276),((_tmp7A8[2]=Cyc_PP_text(((_tmp7A9=")",_tag_dyneither(_tmp7A9,
sizeof(char),2)))),((_tmp7A8[1]=Cyc_Absynpp_typ2doc(_tmp275),((_tmp7A8[0]=Cyc_PP_text(((
_tmp7AA="(",_tag_dyneither(_tmp7AA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7A8,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1B4: {struct Cyc_Absyn_Address_e_struct*
_tmp277=(struct Cyc_Absyn_Address_e_struct*)_tmp24A;if(_tmp277->tag != 16)goto
_LL1B6;else{_tmp278=_tmp277->f1;}}_LL1B5:{const char*_tmp7AD;struct Cyc_PP_Doc*
_tmp7AC[2];s=((_tmp7AC[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp278),((_tmp7AC[0]=
Cyc_PP_text(((_tmp7AD="&",_tag_dyneither(_tmp7AD,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;_LL1B6: {
struct Cyc_Absyn_New_e_struct*_tmp279=(struct Cyc_Absyn_New_e_struct*)_tmp24A;if(
_tmp279->tag != 17)goto _LL1B8;else{_tmp27A=_tmp279->f1;_tmp27B=_tmp279->f2;}}
_LL1B7: if(_tmp27A == 0){const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[2];s=((_tmp7AF[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27B),((_tmp7AF[0]=Cyc_PP_text(((_tmp7B0="new ",
_tag_dyneither(_tmp7B0,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7AF,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7B5;const char*_tmp7B4;
struct Cyc_PP_Doc*_tmp7B3[4];s=((_tmp7B3[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp27B),((_tmp7B3[2]=Cyc_PP_text(((_tmp7B4=") ",_tag_dyneither(_tmp7B4,sizeof(
char),3)))),((_tmp7B3[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp27A),((
_tmp7B3[0]=Cyc_PP_text(((_tmp7B5="rnew(",_tag_dyneither(_tmp7B5,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL193;_LL1B8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp27C=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp24A;if(_tmp27C->tag != 18)goto _LL1BA;else{_tmp27D=(void*)_tmp27C->f1;}}_LL1B9:{
const char*_tmp7BA;const char*_tmp7B9;struct Cyc_PP_Doc*_tmp7B8[3];s=((_tmp7B8[2]=
Cyc_PP_text(((_tmp7B9=")",_tag_dyneither(_tmp7B9,sizeof(char),2)))),((_tmp7B8[1]=
Cyc_Absynpp_typ2doc(_tmp27D),((_tmp7B8[0]=Cyc_PP_text(((_tmp7BA="sizeof(",
_tag_dyneither(_tmp7BA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7B8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1BA: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp27E=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp24A;if(_tmp27E->tag != 19)goto
_LL1BC;else{_tmp27F=_tmp27E->f1;}}_LL1BB:{const char*_tmp7BF;const char*_tmp7BE;
struct Cyc_PP_Doc*_tmp7BD[3];s=((_tmp7BD[2]=Cyc_PP_text(((_tmp7BE=")",
_tag_dyneither(_tmp7BE,sizeof(char),2)))),((_tmp7BD[1]=Cyc_Absynpp_exp2doc(
_tmp27F),((_tmp7BD[0]=Cyc_PP_text(((_tmp7BF="sizeof(",_tag_dyneither(_tmp7BF,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7BD,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL193;_LL1BC: {struct Cyc_Absyn_Valueof_e_struct*_tmp280=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp24A;if(_tmp280->tag != 39)goto _LL1BE;else{_tmp281=(void*)_tmp280->f1;}}_LL1BD:{
const char*_tmp7C4;const char*_tmp7C3;struct Cyc_PP_Doc*_tmp7C2[3];s=((_tmp7C2[2]=
Cyc_PP_text(((_tmp7C3=")",_tag_dyneither(_tmp7C3,sizeof(char),2)))),((_tmp7C2[1]=
Cyc_Absynpp_typ2doc(_tmp281),((_tmp7C2[0]=Cyc_PP_text(((_tmp7C4="valueof(",
_tag_dyneither(_tmp7C4,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7C2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1BE: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp282=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp24A;if(_tmp282->tag != 38)goto
_LL1C0;else{_tmp283=_tmp282->f1;_tmp284=_tmp282->f2;}}_LL1BF:{const char*_tmp7CB;
const char*_tmp7CA;const char*_tmp7C9;struct Cyc_PP_Doc*_tmp7C8[5];s=((_tmp7C8[4]=
Cyc_PP_text(((_tmp7C9=")",_tag_dyneither(_tmp7C9,sizeof(char),2)))),((_tmp7C8[3]=
Cyc_PP_textptr(_tmp284),((_tmp7C8[2]=Cyc_PP_text(((_tmp7CA=".",_tag_dyneither(
_tmp7CA,sizeof(char),2)))),((_tmp7C8[1]=Cyc_Absynpp_exp2doc(_tmp283),((_tmp7C8[0]=
Cyc_PP_text(((_tmp7CB="tagcheck(",_tag_dyneither(_tmp7CB,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7C8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;
_LL1C0: {struct Cyc_Absyn_Offsetof_e_struct*_tmp285=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp24A;if(_tmp285->tag != 20)goto _LL1C2;else{_tmp286=(void*)_tmp285->f1;_tmp287=(
void*)_tmp285->f2;{struct Cyc_Absyn_StructField_struct*_tmp288=(struct Cyc_Absyn_StructField_struct*)
_tmp287;if(_tmp288->tag != 0)goto _LL1C2;else{_tmp289=_tmp288->f1;}};}}_LL1C1:{
const char*_tmp7D2;const char*_tmp7D1;const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[5];
s=((_tmp7CF[4]=Cyc_PP_text(((_tmp7D0=")",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((
_tmp7CF[3]=Cyc_PP_textptr(_tmp289),((_tmp7CF[2]=Cyc_PP_text(((_tmp7D1=",",
_tag_dyneither(_tmp7D1,sizeof(char),2)))),((_tmp7CF[1]=Cyc_Absynpp_typ2doc(
_tmp286),((_tmp7CF[0]=Cyc_PP_text(((_tmp7D2="offsetof(",_tag_dyneither(_tmp7D2,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL193;_LL1C2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp28A=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp24A;if(_tmp28A->tag != 20)goto _LL1C4;else{
_tmp28B=(void*)_tmp28A->f1;_tmp28C=(void*)_tmp28A->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp28D=(struct Cyc_Absyn_TupleIndex_struct*)_tmp28C;if(_tmp28D->tag != 1)goto
_LL1C4;else{_tmp28E=_tmp28D->f1;}};}}_LL1C3:{const char*_tmp7E0;const char*_tmp7DF;
struct Cyc_Int_pa_struct _tmp7DE;void*_tmp7DD[1];const char*_tmp7DC;const char*
_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[5];s=((_tmp7DA[4]=Cyc_PP_text(((_tmp7DB=")",
_tag_dyneither(_tmp7DB,sizeof(char),2)))),((_tmp7DA[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7DE.tag=1,((_tmp7DE.f1=(unsigned long)((int)_tmp28E),((
_tmp7DD[0]=& _tmp7DE,Cyc_aprintf(((_tmp7DC="%d",_tag_dyneither(_tmp7DC,sizeof(
char),3))),_tag_dyneither(_tmp7DD,sizeof(void*),1))))))))),((_tmp7DA[2]=Cyc_PP_text(((
_tmp7DF=",",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DA[1]=Cyc_Absynpp_typ2doc(
_tmp28B),((_tmp7DA[0]=Cyc_PP_text(((_tmp7E0="offsetof(",_tag_dyneither(_tmp7E0,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL193;_LL1C4: {struct Cyc_Absyn_Deref_e_struct*_tmp28F=(
struct Cyc_Absyn_Deref_e_struct*)_tmp24A;if(_tmp28F->tag != 21)goto _LL1C6;else{
_tmp290=_tmp28F->f1;}}_LL1C5:{const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[2];s=((
_tmp7E2[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),((_tmp7E2[0]=Cyc_PP_text(((
_tmp7E3="*",_tag_dyneither(_tmp7E3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7E2,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;_LL1C6: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp291=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp24A;if(_tmp291->tag != 22)goto
_LL1C8;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}}_LL1C7:{const char*_tmp7E6;
struct Cyc_PP_Doc*_tmp7E5[3];s=((_tmp7E5[2]=Cyc_PP_textptr(_tmp293),((_tmp7E5[1]=
Cyc_PP_text(((_tmp7E6=".",_tag_dyneither(_tmp7E6,sizeof(char),2)))),((_tmp7E5[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),Cyc_PP_cat(_tag_dyneither(_tmp7E5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1C8: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp294=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp24A;if(_tmp294->tag != 23)goto
_LL1CA;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1C9:{const char*_tmp7E9;
struct Cyc_PP_Doc*_tmp7E8[3];s=((_tmp7E8[2]=Cyc_PP_textptr(_tmp296),((_tmp7E8[1]=
Cyc_PP_text(((_tmp7E9="->",_tag_dyneither(_tmp7E9,sizeof(char),3)))),((_tmp7E8[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),Cyc_PP_cat(_tag_dyneither(_tmp7E8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1CA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp297=(struct Cyc_Absyn_Subscript_e_struct*)_tmp24A;if(_tmp297->tag != 24)goto
_LL1CC;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}_LL1CB:{const char*_tmp7EE;
const char*_tmp7ED;struct Cyc_PP_Doc*_tmp7EC[4];s=((_tmp7EC[3]=Cyc_PP_text(((
_tmp7ED="]",_tag_dyneither(_tmp7ED,sizeof(char),2)))),((_tmp7EC[2]=Cyc_Absynpp_exp2doc(
_tmp299),((_tmp7EC[1]=Cyc_PP_text(((_tmp7EE="[",_tag_dyneither(_tmp7EE,sizeof(
char),2)))),((_tmp7EC[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp298),Cyc_PP_cat(
_tag_dyneither(_tmp7EC,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1CC: {
struct Cyc_Absyn_Tuple_e_struct*_tmp29A=(struct Cyc_Absyn_Tuple_e_struct*)_tmp24A;
if(_tmp29A->tag != 25)goto _LL1CE;else{_tmp29B=_tmp29A->f1;}}_LL1CD:{const char*
_tmp7F3;const char*_tmp7F2;struct Cyc_PP_Doc*_tmp7F1[4];s=((_tmp7F1[3]=Cyc_PP_text(((
_tmp7F2=")",_tag_dyneither(_tmp7F2,sizeof(char),2)))),((_tmp7F1[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp29B),((_tmp7F1[1]=Cyc_PP_text(((_tmp7F3="(",_tag_dyneither(_tmp7F3,
sizeof(char),2)))),((_tmp7F1[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7F1,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1CE: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp29C=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp24A;if(_tmp29C->tag != 26)goto
_LL1D0;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;}}_LL1CF:{const char*_tmp7FA;
const char*_tmp7F9;const char*_tmp7F8;struct Cyc_PP_Doc*_tmp7F7[4];s=((_tmp7F7[3]=
Cyc_Absynpp_group_braces(((_tmp7F8=",",_tag_dyneither(_tmp7F8,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29E)),((_tmp7F7[2]=Cyc_PP_text(((_tmp7F9=")",
_tag_dyneither(_tmp7F9,sizeof(char),2)))),((_tmp7F7[1]=Cyc_Absynpp_typ2doc((*
_tmp29D).f3),((_tmp7F7[0]=Cyc_PP_text(((_tmp7FA="(",_tag_dyneither(_tmp7FA,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1D0: {struct Cyc_Absyn_Array_e_struct*_tmp29F=(struct Cyc_Absyn_Array_e_struct*)
_tmp24A;if(_tmp29F->tag != 27)goto _LL1D2;else{_tmp2A0=_tmp29F->f1;}}_LL1D1:{const
char*_tmp7FB;s=Cyc_Absynpp_group_braces(((_tmp7FB=",",_tag_dyneither(_tmp7FB,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A0));}goto _LL193;
_LL1D2: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2A1=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp24A;if(_tmp2A1->tag != 28)goto _LL1D4;else{_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A1->f2;
_tmp2A4=_tmp2A1->f3;}}_LL1D3:{const char*_tmp802;const char*_tmp801;const char*
_tmp800;struct Cyc_PP_Doc*_tmp7FF[8];s=((_tmp7FF[7]=Cyc_Absynpp_rb(),((_tmp7FF[6]=
Cyc_Absynpp_exp2doc(_tmp2A4),((_tmp7FF[5]=Cyc_PP_text(((_tmp800=" : ",
_tag_dyneither(_tmp800,sizeof(char),4)))),((_tmp7FF[4]=Cyc_Absynpp_exp2doc(
_tmp2A3),((_tmp7FF[3]=Cyc_PP_text(((_tmp801=" < ",_tag_dyneither(_tmp801,sizeof(
char),4)))),((_tmp7FF[2]=Cyc_PP_text(*(*_tmp2A2->name).f2),((_tmp7FF[1]=Cyc_PP_text(((
_tmp802="for ",_tag_dyneither(_tmp802,sizeof(char),5)))),((_tmp7FF[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7FF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL193;_LL1D4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2A5=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp24A;if(_tmp2A5->tag != 29)goto _LL1D6;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;
_tmp2A8=_tmp2A5->f3;}}_LL1D5: {struct Cyc_List_List*_tmp316=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2A8);{const char*_tmp807;struct Cyc_List_List*_tmp806;struct Cyc_PP_Doc*_tmp805[
2];s=((_tmp805[1]=Cyc_Absynpp_group_braces(((_tmp807=",",_tag_dyneither(_tmp807,
sizeof(char),2))),_tmp2A7 != 0?(_tmp806=_cycalloc(sizeof(*_tmp806)),((_tmp806->hd=
Cyc_Absynpp_tps2doc(_tmp2A7),((_tmp806->tl=_tmp316,_tmp806))))): _tmp316),((
_tmp805[0]=Cyc_Absynpp_qvar2doc(_tmp2A6),Cyc_PP_cat(_tag_dyneither(_tmp805,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;}_LL1D6: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2A9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp24A;if(_tmp2A9->tag != 30)goto
_LL1D8;else{_tmp2AA=_tmp2A9->f2;}}_LL1D7:{const char*_tmp808;s=Cyc_Absynpp_group_braces(((
_tmp808=",",_tag_dyneither(_tmp808,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2AA));}goto _LL193;_LL1D8: {struct Cyc_Absyn_Datatype_e_struct*_tmp2AB=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp24A;if(_tmp2AB->tag != 31)goto _LL1DA;else{
_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f3;}}_LL1D9: if(_tmp2AC == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2AD->name);else{const char*_tmp80F;const char*_tmp80E;const char*_tmp80D;struct
Cyc_PP_Doc*_tmp80C[2];s=((_tmp80C[1]=Cyc_PP_egroup(((_tmp80F="(",_tag_dyneither(
_tmp80F,sizeof(char),2))),((_tmp80E=")",_tag_dyneither(_tmp80E,sizeof(char),2))),((
_tmp80D=",",_tag_dyneither(_tmp80D,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2AC)),((_tmp80C[0]=Cyc_Absynpp_qvar2doc(_tmp2AD->name),
Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;
_LL1DA: {struct Cyc_Absyn_Enum_e_struct*_tmp2AE=(struct Cyc_Absyn_Enum_e_struct*)
_tmp24A;if(_tmp2AE->tag != 32)goto _LL1DC;else{_tmp2AF=_tmp2AE->f1;}}_LL1DB: s=Cyc_Absynpp_qvar2doc(
_tmp2AF);goto _LL193;_LL1DC: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2B0=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp24A;if(_tmp2B0->tag != 33)goto _LL1DE;else{
_tmp2B1=_tmp2B0->f1;}}_LL1DD: s=Cyc_Absynpp_qvar2doc(_tmp2B1);goto _LL193;_LL1DE: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2B2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp24A;if(_tmp2B2->tag != 34)goto _LL1E0;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B3.is_calloc;
_tmp2B5=_tmp2B3.rgn;_tmp2B6=_tmp2B3.elt_type;_tmp2B7=_tmp2B3.num_elts;}}_LL1DF:
if(_tmp2B4){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2B6)),0);if(_tmp2B5 == 0){const char*_tmp816;const char*_tmp815;const char*
_tmp814;struct Cyc_PP_Doc*_tmp813[5];s=((_tmp813[4]=Cyc_PP_text(((_tmp814=")",
_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[3]=Cyc_Absynpp_exp2doc(st),((
_tmp813[2]=Cyc_PP_text(((_tmp815=",",_tag_dyneither(_tmp815,sizeof(char),2)))),((
_tmp813[1]=Cyc_Absynpp_exp2doc(_tmp2B7),((_tmp813[0]=Cyc_PP_text(((_tmp816="calloc(",
_tag_dyneither(_tmp816,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp813,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp81F;const char*
_tmp81E;const char*_tmp81D;const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[7];s=((
_tmp81B[6]=Cyc_PP_text(((_tmp81C=")",_tag_dyneither(_tmp81C,sizeof(char),2)))),((
_tmp81B[5]=Cyc_Absynpp_exp2doc(st),((_tmp81B[4]=Cyc_PP_text(((_tmp81D=",",
_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81B[3]=Cyc_Absynpp_exp2doc(
_tmp2B7),((_tmp81B[2]=Cyc_PP_text(((_tmp81E=",",_tag_dyneither(_tmp81E,sizeof(
char),2)))),((_tmp81B[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2B5),((
_tmp81B[0]=Cyc_PP_text(((_tmp81F="rcalloc(",_tag_dyneither(_tmp81F,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2B6 == 0)new_e=_tmp2B7;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2B6,0),_tmp2B7,0);}if(_tmp2B5 == 0){const char*
_tmp824;const char*_tmp823;struct Cyc_PP_Doc*_tmp822[3];s=((_tmp822[2]=Cyc_PP_text(((
_tmp823=")",_tag_dyneither(_tmp823,sizeof(char),2)))),((_tmp822[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp822[0]=Cyc_PP_text(((_tmp824="malloc(",_tag_dyneither(_tmp824,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp822,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp82B;const char*_tmp82A;const char*_tmp829;struct Cyc_PP_Doc*
_tmp828[5];s=((_tmp828[4]=Cyc_PP_text(((_tmp829=")",_tag_dyneither(_tmp829,
sizeof(char),2)))),((_tmp828[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp828[2]=Cyc_PP_text(((
_tmp82A=",",_tag_dyneither(_tmp82A,sizeof(char),2)))),((_tmp828[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2B5),((_tmp828[0]=Cyc_PP_text(((_tmp82B="rmalloc(",
_tag_dyneither(_tmp82B,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp828,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL193;_LL1E0: {struct Cyc_Absyn_Swap_e_struct*
_tmp2B8=(struct Cyc_Absyn_Swap_e_struct*)_tmp24A;if(_tmp2B8->tag != 35)goto _LL1E2;
else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B8->f2;}}_LL1E1:{const char*_tmp832;const
char*_tmp831;const char*_tmp830;struct Cyc_PP_Doc*_tmp82F[5];s=((_tmp82F[4]=Cyc_PP_text(((
_tmp830=")",_tag_dyneither(_tmp830,sizeof(char),2)))),((_tmp82F[3]=Cyc_Absynpp_exp2doc(
_tmp2BA),((_tmp82F[2]=Cyc_PP_text(((_tmp831=",",_tag_dyneither(_tmp831,sizeof(
char),2)))),((_tmp82F[1]=Cyc_Absynpp_exp2doc(_tmp2B9),((_tmp82F[0]=Cyc_PP_text(((
_tmp832="cycswap(",_tag_dyneither(_tmp832,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;
_LL1E2: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2BB=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp24A;if(_tmp2BB->tag != 36)goto _LL1E4;else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;}}
_LL1E3:{const char*_tmp833;s=Cyc_Absynpp_group_braces(((_tmp833=",",
_tag_dyneither(_tmp833,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2BD));}goto _LL193;_LL1E4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2BE=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp24A;if(_tmp2BE->tag != 37)goto _LL193;else{_tmp2BF=
_tmp2BE->f1;}}_LL1E5:{const char*_tmp838;const char*_tmp837;struct Cyc_PP_Doc*
_tmp836[7];s=((_tmp836[6]=Cyc_PP_text(((_tmp837=")",_tag_dyneither(_tmp837,
sizeof(char),2)))),((_tmp836[5]=Cyc_Absynpp_rb(),((_tmp836[4]=Cyc_PP_blank_doc(),((
_tmp836[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2BF,1)),((_tmp836[2]=Cyc_PP_blank_doc(),((
_tmp836[1]=Cyc_Absynpp_lb(),((_tmp836[0]=Cyc_PP_text(((_tmp838="(",
_tag_dyneither(_tmp838,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp836,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL193;_LL193:;}if(inprec >= 
myprec){const char*_tmp83D;const char*_tmp83C;struct Cyc_PP_Doc*_tmp83B[3];s=((
_tmp83B[2]=Cyc_PP_text(((_tmp83C=")",_tag_dyneither(_tmp83C,sizeof(char),2)))),((
_tmp83B[1]=s,((_tmp83B[0]=Cyc_PP_text(((_tmp83D="(",_tag_dyneither(_tmp83D,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp83B,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp33A=d;struct Cyc_Absyn_Exp*_tmp33C;
struct _dyneither_ptr*_tmp33E;_LL1EC: {struct Cyc_Absyn_ArrayElement_struct*
_tmp33B=(struct Cyc_Absyn_ArrayElement_struct*)_tmp33A;if(_tmp33B->tag != 0)goto
_LL1EE;else{_tmp33C=_tmp33B->f1;}}_LL1ED: {const char*_tmp842;const char*_tmp841;
struct Cyc_PP_Doc*_tmp840[3];return(_tmp840[2]=Cyc_PP_text(((_tmp841="]",
_tag_dyneither(_tmp841,sizeof(char),2)))),((_tmp840[1]=Cyc_Absynpp_exp2doc(
_tmp33C),((_tmp840[0]=Cyc_PP_text(((_tmp842=".[",_tag_dyneither(_tmp842,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp840,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1EE: {struct Cyc_Absyn_FieldName_struct*_tmp33D=(struct Cyc_Absyn_FieldName_struct*)
_tmp33A;if(_tmp33D->tag != 1)goto _LL1EB;else{_tmp33E=_tmp33D->f1;}}_LL1EF: {const
char*_tmp845;struct Cyc_PP_Doc*_tmp844[2];return(_tmp844[1]=Cyc_PP_textptr(
_tmp33E),((_tmp844[0]=Cyc_PP_text(((_tmp845=".",_tag_dyneither(_tmp845,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp844,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1EB:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp84C;const char*_tmp84B;const char*_tmp84A;struct Cyc_PP_Doc*
_tmp849[2];return(_tmp849[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp849[0]=Cyc_PP_egroup(((
_tmp84C="",_tag_dyneither(_tmp84C,sizeof(char),1))),((_tmp84B="=",_tag_dyneither(
_tmp84B,sizeof(char),2))),((_tmp84A="=",_tag_dyneither(_tmp84A,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp849,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp84F;const char*_tmp84E;const char*_tmp84D;
return Cyc_PP_group(((_tmp84D="",_tag_dyneither(_tmp84D,sizeof(char),1))),((
_tmp84E="",_tag_dyneither(_tmp84E,sizeof(char),1))),((_tmp84F=",",_tag_dyneither(
_tmp84F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp34B=
c;struct _tuple3 _tmp34C;enum Cyc_Absyn_Sign _tmp34D;char _tmp34E;struct _tuple4
_tmp34F;enum Cyc_Absyn_Sign _tmp350;short _tmp351;struct _tuple5 _tmp352;enum Cyc_Absyn_Sign
_tmp353;int _tmp354;struct _tuple5 _tmp355;enum Cyc_Absyn_Sign _tmp356;int _tmp357;
struct _tuple5 _tmp358;enum Cyc_Absyn_Sign _tmp359;int _tmp35A;struct _tuple6 _tmp35B;
enum Cyc_Absyn_Sign _tmp35C;long long _tmp35D;struct _dyneither_ptr _tmp35E;int
_tmp35F;struct _dyneither_ptr _tmp360;_LL1F1: if((_tmp34B.Char_c).tag != 2)goto
_LL1F3;_tmp34C=(struct _tuple3)(_tmp34B.Char_c).val;_tmp34D=_tmp34C.f1;_tmp34E=
_tmp34C.f2;_LL1F2: {const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_struct
_tmp851;return Cyc_PP_text((struct _dyneither_ptr)((_tmp851.tag=0,((_tmp851.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34E)),((
_tmp852[0]=& _tmp851,Cyc_aprintf(((_tmp853="'%s'",_tag_dyneither(_tmp853,sizeof(
char),5))),_tag_dyneither(_tmp852,sizeof(void*),1)))))))));}_LL1F3: if((_tmp34B.Short_c).tag
!= 3)goto _LL1F5;_tmp34F=(struct _tuple4)(_tmp34B.Short_c).val;_tmp350=_tmp34F.f1;
_tmp351=_tmp34F.f2;_LL1F4: {const char*_tmp857;void*_tmp856[1];struct Cyc_Int_pa_struct
_tmp855;return Cyc_PP_text((struct _dyneither_ptr)((_tmp855.tag=1,((_tmp855.f1=(
unsigned long)((int)_tmp351),((_tmp856[0]=& _tmp855,Cyc_aprintf(((_tmp857="%d",
_tag_dyneither(_tmp857,sizeof(char),3))),_tag_dyneither(_tmp856,sizeof(void*),1)))))))));}
_LL1F5: if((_tmp34B.Int_c).tag != 4)goto _LL1F7;_tmp352=(struct _tuple5)(_tmp34B.Int_c).val;
_tmp353=_tmp352.f1;if(_tmp353 != Cyc_Absyn_None)goto _LL1F7;_tmp354=_tmp352.f2;
_LL1F6: _tmp357=_tmp354;goto _LL1F8;_LL1F7: if((_tmp34B.Int_c).tag != 4)goto _LL1F9;
_tmp355=(struct _tuple5)(_tmp34B.Int_c).val;_tmp356=_tmp355.f1;if(_tmp356 != Cyc_Absyn_Signed)
goto _LL1F9;_tmp357=_tmp355.f2;_LL1F8: {const char*_tmp85B;void*_tmp85A[1];struct
Cyc_Int_pa_struct _tmp859;return Cyc_PP_text((struct _dyneither_ptr)((_tmp859.tag=1,((
_tmp859.f1=(unsigned long)_tmp357,((_tmp85A[0]=& _tmp859,Cyc_aprintf(((_tmp85B="%d",
_tag_dyneither(_tmp85B,sizeof(char),3))),_tag_dyneither(_tmp85A,sizeof(void*),1)))))))));}
_LL1F9: if((_tmp34B.Int_c).tag != 4)goto _LL1FB;_tmp358=(struct _tuple5)(_tmp34B.Int_c).val;
_tmp359=_tmp358.f1;if(_tmp359 != Cyc_Absyn_Unsigned)goto _LL1FB;_tmp35A=_tmp358.f2;
_LL1FA: {const char*_tmp85F;void*_tmp85E[1];struct Cyc_Int_pa_struct _tmp85D;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp85D.tag=1,((_tmp85D.f1=(unsigned int)
_tmp35A,((_tmp85E[0]=& _tmp85D,Cyc_aprintf(((_tmp85F="%u",_tag_dyneither(_tmp85F,
sizeof(char),3))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))))));}_LL1FB: if((
_tmp34B.LongLong_c).tag != 5)goto _LL1FD;_tmp35B=(struct _tuple6)(_tmp34B.LongLong_c).val;
_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;_LL1FC: {const char*_tmp860;return Cyc_PP_text(((
_tmp860="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp860,sizeof(char),27))));}
_LL1FD: if((_tmp34B.Float_c).tag != 6)goto _LL1FF;_tmp35E=(struct _dyneither_ptr)(
_tmp34B.Float_c).val;_LL1FE: return Cyc_PP_text(_tmp35E);_LL1FF: if((_tmp34B.Null_c).tag
!= 1)goto _LL201;_tmp35F=(int)(_tmp34B.Null_c).val;_LL200: {const char*_tmp861;
return Cyc_PP_text(((_tmp861="NULL",_tag_dyneither(_tmp861,sizeof(char),5))));}
_LL201: if((_tmp34B.String_c).tag != 7)goto _LL1F0;_tmp360=(struct _dyneither_ptr)(
_tmp34B.String_c).val;_LL202: {const char*_tmp866;const char*_tmp865;struct Cyc_PP_Doc*
_tmp864[3];return(_tmp864[2]=Cyc_PP_text(((_tmp865="\"",_tag_dyneither(_tmp865,
sizeof(char),2)))),((_tmp864[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp360)),((
_tmp864[0]=Cyc_PP_text(((_tmp866="\"",_tag_dyneither(_tmp866,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp864,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F0:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp873;const char*_tmp872;void*_tmp871[1];struct Cyc_String_pa_struct _tmp870;
struct Cyc_Core_Failure_struct*_tmp86F;(int)_throw((void*)((_tmp86F=_cycalloc(
sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp873.tag=Cyc_Core_Failure,((_tmp873.f1=(
struct _dyneither_ptr)((_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp871[0]=& _tmp870,Cyc_aprintf(((
_tmp872="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp872,
sizeof(char),47))),_tag_dyneither(_tmp871,sizeof(void*),1)))))))),_tmp873)))),
_tmp86F)))));}{const char*_tmp878;const char*_tmp877;struct Cyc_PP_Doc*_tmp876[3];
return(_tmp876[2]=Cyc_PP_text(((_tmp877=")",_tag_dyneither(_tmp877,sizeof(char),
2)))),((_tmp876[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp876[0]=
Cyc_PP_text(((_tmp878="numelts(",_tag_dyneither(_tmp878,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp885;const char*_tmp884;void*_tmp883[1];
struct Cyc_String_pa_struct _tmp882;struct Cyc_Core_Failure_struct*_tmp881;(int)
_throw((void*)((_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881[0]=((_tmp885.tag=
Cyc_Core_Failure,((_tmp885.f1=(struct _dyneither_ptr)((_tmp882.tag=0,((_tmp882.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp883[0]=& _tmp882,Cyc_aprintf(((_tmp884="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp884,sizeof(char),38))),_tag_dyneither(_tmp883,sizeof(void*),1)))))))),
_tmp885)))),_tmp881)))));}else{if(ds->tl == 0){const char*_tmp888;struct Cyc_PP_Doc*
_tmp887[3];return(_tmp887[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp887[1]=Cyc_PP_text(((
_tmp888=" ",_tag_dyneither(_tmp888,sizeof(char),2)))),((_tmp887[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp887,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp895;const char*
_tmp894;void*_tmp893[1];struct Cyc_String_pa_struct _tmp892;struct Cyc_Core_Failure_struct*
_tmp891;(int)_throw((void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((
_tmp895.tag=Cyc_Core_Failure,((_tmp895.f1=(struct _dyneither_ptr)((_tmp892.tag=0,((
_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp893[0]=& _tmp892,Cyc_aprintf(((_tmp894="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp894,sizeof(char),47))),_tag_dyneither(_tmp893,sizeof(void*),1)))))))),
_tmp895)))),_tmp891)))));}else{const char*_tmp89A;const char*_tmp899;struct Cyc_PP_Doc*
_tmp898[5];return(_tmp898[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp898[3]=Cyc_PP_text(((_tmp899=" ",_tag_dyneither(
_tmp899,sizeof(char),2)))),((_tmp898[2]=ps,((_tmp898[1]=Cyc_PP_text(((_tmp89A=" ",
_tag_dyneither(_tmp89A,sizeof(char),2)))),((_tmp898[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp898,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL203: {const char*_tmp89B;return(_tmp89B="+",_tag_dyneither(_tmp89B,sizeof(char),
2));}case Cyc_Absyn_Times: _LL204: {const char*_tmp89C;return(_tmp89C="*",
_tag_dyneither(_tmp89C,sizeof(char),2));}case Cyc_Absyn_Minus: _LL205: {const char*
_tmp89D;return(_tmp89D="-",_tag_dyneither(_tmp89D,sizeof(char),2));}case Cyc_Absyn_Div:
_LL206: {const char*_tmp89E;return(_tmp89E="/",_tag_dyneither(_tmp89E,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL207: {const char*_tmp8A0;const char*_tmp89F;return Cyc_Absynpp_print_for_cycdoc?(
_tmp8A0="\\%",_tag_dyneither(_tmp8A0,sizeof(char),3)):((_tmp89F="%",
_tag_dyneither(_tmp89F,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL208: {const char*
_tmp8A1;return(_tmp8A1="==",_tag_dyneither(_tmp8A1,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL209: {const char*_tmp8A2;return(_tmp8A2="!=",_tag_dyneither(_tmp8A2,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL20A: {const char*_tmp8A3;return(_tmp8A3=">",
_tag_dyneither(_tmp8A3,sizeof(char),2));}case Cyc_Absyn_Lt: _LL20B: {const char*
_tmp8A4;return(_tmp8A4="<",_tag_dyneither(_tmp8A4,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL20C: {const char*_tmp8A5;return(_tmp8A5=">=",_tag_dyneither(_tmp8A5,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL20D: {const char*_tmp8A6;return(_tmp8A6="<=",
_tag_dyneither(_tmp8A6,sizeof(char),3));}case Cyc_Absyn_Not: _LL20E: {const char*
_tmp8A7;return(_tmp8A7="!",_tag_dyneither(_tmp8A7,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL20F: {const char*_tmp8A8;return(_tmp8A8="~",_tag_dyneither(_tmp8A8,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL210: {const char*_tmp8A9;return(_tmp8A9="&",
_tag_dyneither(_tmp8A9,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL211: {const char*
_tmp8AA;return(_tmp8AA="|",_tag_dyneither(_tmp8AA,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL212: {const char*_tmp8AB;return(_tmp8AB="^",_tag_dyneither(_tmp8AB,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL213: {const char*_tmp8AC;return(_tmp8AC="<<",
_tag_dyneither(_tmp8AC,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL214: {const
char*_tmp8AD;return(_tmp8AD=">>",_tag_dyneither(_tmp8AD,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL215: {const char*_tmp8AE;return(_tmp8AE=">>>",
_tag_dyneither(_tmp8AE,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL216: {const
char*_tmp8AF;return(_tmp8AF="numelts",_tag_dyneither(_tmp8AF,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp39E=s->r;_LL219: {struct Cyc_Absyn_Decl_s_struct*
_tmp39F=(struct Cyc_Absyn_Decl_s_struct*)_tmp39E;if(_tmp39F->tag != 12)goto _LL21B;}
_LL21A: return 1;_LL21B:;_LL21C: return 0;_LL218:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3A0=st->r;struct Cyc_Absyn_Exp*
_tmp3A3;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_Absyn_Exp*
_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_Absyn_Stmt*
_tmp3AC;struct _tuple8 _tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Stmt*
_tmp3B0;struct _dyneither_ptr*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B6;struct _tuple8
_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct _tuple8 _tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_Absyn_Stmt*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_List_List*
_tmp3BE;struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_Decl*
_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C5;struct _dyneither_ptr*_tmp3C7;struct Cyc_Absyn_Stmt*
_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3CA;struct _tuple8 _tmp3CB;struct Cyc_Absyn_Exp*
_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*
_tmp3D1;_LL21E: {struct Cyc_Absyn_Skip_s_struct*_tmp3A1=(struct Cyc_Absyn_Skip_s_struct*)
_tmp3A0;if(_tmp3A1->tag != 0)goto _LL220;}_LL21F:{const char*_tmp8B0;s=Cyc_PP_text(((
_tmp8B0=";",_tag_dyneither(_tmp8B0,sizeof(char),2))));}goto _LL21D;_LL220: {
struct Cyc_Absyn_Exp_s_struct*_tmp3A2=(struct Cyc_Absyn_Exp_s_struct*)_tmp3A0;if(
_tmp3A2->tag != 1)goto _LL222;else{_tmp3A3=_tmp3A2->f1;}}_LL221:{const char*_tmp8B3;
struct Cyc_PP_Doc*_tmp8B2[2];s=((_tmp8B2[1]=Cyc_PP_text(((_tmp8B3=";",
_tag_dyneither(_tmp8B3,sizeof(char),2)))),((_tmp8B2[0]=Cyc_Absynpp_exp2doc(
_tmp3A3),Cyc_PP_cat(_tag_dyneither(_tmp8B2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL21D;_LL222: {struct Cyc_Absyn_Seq_s_struct*_tmp3A4=(struct Cyc_Absyn_Seq_s_struct*)
_tmp3A0;if(_tmp3A4->tag != 2)goto _LL224;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;}}
_LL223: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp3A5)){
struct Cyc_PP_Doc*_tmp8BE[5];struct Cyc_PP_Doc*_tmp8BD[7];const char*_tmp8BC;const
char*_tmp8BB;struct Cyc_PP_Doc*_tmp8BA[7];s=((_tmp8BA[6]=Cyc_Absynpp_is_declaration(
_tmp3A6)?stmtexp?(_tmp8BD[6]=Cyc_PP_line_doc(),((_tmp8BD[5]=Cyc_PP_text(((
_tmp8BC=");",_tag_dyneither(_tmp8BC,sizeof(char),3)))),((_tmp8BD[4]=Cyc_Absynpp_rb(),((
_tmp8BD[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp)),((_tmp8BD[2]=Cyc_PP_blank_doc(),((
_tmp8BD[1]=Cyc_Absynpp_lb(),((_tmp8BD[0]=Cyc_PP_text(((_tmp8BB="(",
_tag_dyneither(_tmp8BB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BD,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8BE[4]=Cyc_PP_line_doc(),((
_tmp8BE[3]=Cyc_Absynpp_rb(),((_tmp8BE[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A6,stmtexp)),((_tmp8BE[1]=Cyc_PP_blank_doc(),((_tmp8BE[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp3A6,stmtexp),((_tmp8BA[5]=Cyc_PP_line_doc(),((_tmp8BA[4]=Cyc_Absynpp_rb(),((
_tmp8BA[3]=Cyc_PP_line_doc(),((_tmp8BA[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A5,0)),((_tmp8BA[1]=Cyc_PP_blank_doc(),((_tmp8BA[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp3A6)){struct Cyc_PP_Doc*_tmp8C9[4];struct Cyc_PP_Doc*_tmp8C8[6];const char*
_tmp8C7;const char*_tmp8C6;struct Cyc_PP_Doc*_tmp8C5[4];s=((_tmp8C5[3]=Cyc_PP_line_doc(),((
_tmp8C5[2]=stmtexp?(_tmp8C8[5]=Cyc_PP_text(((_tmp8C7=");",_tag_dyneither(_tmp8C7,
sizeof(char),3)))),((_tmp8C8[4]=Cyc_Absynpp_rb(),((_tmp8C8[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A6,stmtexp)),((_tmp8C8[2]=Cyc_PP_blank_doc(),((_tmp8C8[1]=Cyc_Absynpp_lb(),((
_tmp8C8[0]=Cyc_PP_text(((_tmp8C6="(",_tag_dyneither(_tmp8C6,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8C8,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8C9[3]=Cyc_Absynpp_rb(),((_tmp8C9[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A6,stmtexp)),((_tmp8C9[1]=Cyc_PP_blank_doc(),((_tmp8C9[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8C9,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8C5[1]=Cyc_PP_line_doc(),((_tmp8C5[0]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(
_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8CA[3];s=((_tmp8CA[2]=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp),((_tmp8CA[1]=Cyc_PP_line_doc(),((
_tmp8CA[0]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(_tag_dyneither(_tmp8CA,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8CB[3];s=((
_tmp8CB[2]=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp),((_tmp8CB[1]=Cyc_PP_line_doc(),((
_tmp8CB[0]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(_tag_dyneither(_tmp8CB,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL21D;_LL224: {struct Cyc_Absyn_Return_s_struct*
_tmp3A7=(struct Cyc_Absyn_Return_s_struct*)_tmp3A0;if(_tmp3A7->tag != 3)goto _LL226;
else{_tmp3A8=_tmp3A7->f1;}}_LL225: if(_tmp3A8 == 0){const char*_tmp8CC;s=Cyc_PP_text(((
_tmp8CC="return;",_tag_dyneither(_tmp8CC,sizeof(char),8))));}else{const char*
_tmp8D1;const char*_tmp8D0;struct Cyc_PP_Doc*_tmp8CF[3];s=((_tmp8CF[2]=Cyc_PP_text(((
_tmp8D0=";",_tag_dyneither(_tmp8D0,sizeof(char),2)))),((_tmp8CF[1]=_tmp3A8 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3A8),((_tmp8CF[0]=
Cyc_PP_text(((_tmp8D1="return ",_tag_dyneither(_tmp8D1,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL21D;_LL226: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp3A9=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3A0;if(_tmp3A9->tag != 4)goto _LL228;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;
_tmp3AC=_tmp3A9->f3;}}_LL227: {int print_else;{void*_tmp3E5=_tmp3AC->r;_LL243: {
struct Cyc_Absyn_Skip_s_struct*_tmp3E6=(struct Cyc_Absyn_Skip_s_struct*)_tmp3E5;
if(_tmp3E6->tag != 0)goto _LL245;}_LL244: print_else=0;goto _LL242;_LL245:;_LL246:
print_else=1;goto _LL242;_LL242:;}{const char*_tmp8E0;const char*_tmp8DF;struct Cyc_PP_Doc*
_tmp8DE[2];struct Cyc_PP_Doc*_tmp8DD[6];struct Cyc_PP_Doc*_tmp8DC[2];const char*
_tmp8DB;struct Cyc_PP_Doc*_tmp8DA[8];s=((_tmp8DA[7]=print_else?(_tmp8DD[5]=Cyc_Absynpp_rb(),((
_tmp8DD[4]=Cyc_PP_line_doc(),((_tmp8DD[3]=Cyc_PP_nest(2,((_tmp8DC[1]=Cyc_Absynpp_stmt2doc(
_tmp3AC,0),((_tmp8DC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DC,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DD[2]=Cyc_Absynpp_lb(),((_tmp8DD[1]=
Cyc_PP_text(((_tmp8DB="else ",_tag_dyneither(_tmp8DB,sizeof(char),6)))),((
_tmp8DD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DD,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8DA[6]=Cyc_Absynpp_rb(),((_tmp8DA[5]=Cyc_PP_line_doc(),((
_tmp8DA[4]=Cyc_PP_nest(2,((_tmp8DE[1]=Cyc_Absynpp_stmt2doc(_tmp3AB,0),((_tmp8DE[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DE,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8DA[3]=Cyc_Absynpp_lb(),((_tmp8DA[2]=Cyc_PP_text(((_tmp8DF=") ",
_tag_dyneither(_tmp8DF,sizeof(char),3)))),((_tmp8DA[1]=Cyc_Absynpp_exp2doc(
_tmp3AA),((_tmp8DA[0]=Cyc_PP_text(((_tmp8E0="if (",_tag_dyneither(_tmp8E0,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8DA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL21D;}_LL228: {struct Cyc_Absyn_While_s_struct*_tmp3AD=(struct Cyc_Absyn_While_s_struct*)
_tmp3A0;if(_tmp3AD->tag != 5)goto _LL22A;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AE.f1;
_tmp3B0=_tmp3AD->f2;}}_LL229:{const char*_tmp8E7;const char*_tmp8E6;struct Cyc_PP_Doc*
_tmp8E5[2];struct Cyc_PP_Doc*_tmp8E4[7];s=((_tmp8E4[6]=Cyc_Absynpp_rb(),((_tmp8E4[
5]=Cyc_PP_line_doc(),((_tmp8E4[4]=Cyc_PP_nest(2,((_tmp8E5[1]=Cyc_Absynpp_stmt2doc(
_tmp3B0,0),((_tmp8E5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E5,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E4[3]=Cyc_Absynpp_lb(),((_tmp8E4[2]=
Cyc_PP_text(((_tmp8E6=") ",_tag_dyneither(_tmp8E6,sizeof(char),3)))),((_tmp8E4[1]=
Cyc_Absynpp_exp2doc(_tmp3AF),((_tmp8E4[0]=Cyc_PP_text(((_tmp8E7="while (",
_tag_dyneither(_tmp8E7,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8E4,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL21D;_LL22A: {struct Cyc_Absyn_Break_s_struct*
_tmp3B1=(struct Cyc_Absyn_Break_s_struct*)_tmp3A0;if(_tmp3B1->tag != 6)goto _LL22C;}
_LL22B:{const char*_tmp8E8;s=Cyc_PP_text(((_tmp8E8="break;",_tag_dyneither(
_tmp8E8,sizeof(char),7))));}goto _LL21D;_LL22C: {struct Cyc_Absyn_Continue_s_struct*
_tmp3B2=(struct Cyc_Absyn_Continue_s_struct*)_tmp3A0;if(_tmp3B2->tag != 7)goto
_LL22E;}_LL22D:{const char*_tmp8E9;s=Cyc_PP_text(((_tmp8E9="continue;",
_tag_dyneither(_tmp8E9,sizeof(char),10))));}goto _LL21D;_LL22E: {struct Cyc_Absyn_Goto_s_struct*
_tmp3B3=(struct Cyc_Absyn_Goto_s_struct*)_tmp3A0;if(_tmp3B3->tag != 8)goto _LL230;
else{_tmp3B4=_tmp3B3->f1;}}_LL22F:{const char*_tmp8ED;void*_tmp8EC[1];struct Cyc_String_pa_struct
_tmp8EB;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8EB.tag=0,((_tmp8EB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3B4),((_tmp8EC[0]=& _tmp8EB,Cyc_aprintf(((
_tmp8ED="goto %s;",_tag_dyneither(_tmp8ED,sizeof(char),9))),_tag_dyneither(
_tmp8EC,sizeof(void*),1)))))))));}goto _LL21D;_LL230: {struct Cyc_Absyn_For_s_struct*
_tmp3B5=(struct Cyc_Absyn_For_s_struct*)_tmp3A0;if(_tmp3B5->tag != 9)goto _LL232;
else{_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;_tmp3B8=_tmp3B7.f1;_tmp3B9=_tmp3B5->f3;
_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B5->f4;}}_LL231:{const char*_tmp8F8;const char*
_tmp8F7;const char*_tmp8F6;const char*_tmp8F5;struct Cyc_PP_Doc*_tmp8F4[2];struct
Cyc_PP_Doc*_tmp8F3[11];s=((_tmp8F3[10]=Cyc_Absynpp_rb(),((_tmp8F3[9]=Cyc_PP_line_doc(),((
_tmp8F3[8]=Cyc_PP_nest(2,((_tmp8F4[1]=Cyc_Absynpp_stmt2doc(_tmp3BB,0),((_tmp8F4[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8F3[7]=Cyc_Absynpp_lb(),((_tmp8F3[6]=Cyc_PP_text(((_tmp8F5=") ",
_tag_dyneither(_tmp8F5,sizeof(char),3)))),((_tmp8F3[5]=Cyc_Absynpp_exp2doc(
_tmp3BA),((_tmp8F3[4]=Cyc_PP_text(((_tmp8F6="; ",_tag_dyneither(_tmp8F6,sizeof(
char),3)))),((_tmp8F3[3]=Cyc_Absynpp_exp2doc(_tmp3B8),((_tmp8F3[2]=Cyc_PP_text(((
_tmp8F7="; ",_tag_dyneither(_tmp8F7,sizeof(char),3)))),((_tmp8F3[1]=Cyc_Absynpp_exp2doc(
_tmp3B6),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F8="for(",_tag_dyneither(_tmp8F8,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL21D;_LL232: {struct Cyc_Absyn_Switch_s_struct*
_tmp3BC=(struct Cyc_Absyn_Switch_s_struct*)_tmp3A0;if(_tmp3BC->tag != 10)goto
_LL234;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL233:{const char*_tmp8FD;
const char*_tmp8FC;struct Cyc_PP_Doc*_tmp8FB[8];s=((_tmp8FB[7]=Cyc_Absynpp_rb(),((
_tmp8FB[6]=Cyc_PP_line_doc(),((_tmp8FB[5]=Cyc_Absynpp_switchclauses2doc(_tmp3BE),((
_tmp8FB[4]=Cyc_PP_line_doc(),((_tmp8FB[3]=Cyc_Absynpp_lb(),((_tmp8FB[2]=Cyc_PP_text(((
_tmp8FC=") ",_tag_dyneither(_tmp8FC,sizeof(char),3)))),((_tmp8FB[1]=Cyc_Absynpp_exp2doc(
_tmp3BD),((_tmp8FB[0]=Cyc_PP_text(((_tmp8FD="switch (",_tag_dyneither(_tmp8FD,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8FB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL21D;_LL234: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3BF=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3A0;if(_tmp3BF->tag != 11)goto _LL236;else{_tmp3C0=_tmp3BF->f1;if(_tmp3C0 != 0)
goto _LL236;}}_LL235:{const char*_tmp8FE;s=Cyc_PP_text(((_tmp8FE="fallthru;",
_tag_dyneither(_tmp8FE,sizeof(char),10))));}goto _LL21D;_LL236: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3C1=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3A0;if(_tmp3C1->tag != 11)goto
_LL238;else{_tmp3C2=_tmp3C1->f1;}}_LL237:{const char*_tmp903;const char*_tmp902;
struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=Cyc_PP_text(((_tmp902=");",
_tag_dyneither(_tmp902,sizeof(char),3)))),((_tmp901[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3C2),((_tmp901[0]=Cyc_PP_text(((_tmp903="fallthru(",_tag_dyneither(
_tmp903,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL21D;_LL238: {struct Cyc_Absyn_Decl_s_struct*_tmp3C3=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3A0;if(_tmp3C3->tag != 12)goto _LL23A;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}
_LL239:{struct Cyc_PP_Doc*_tmp904[3];s=((_tmp904[2]=Cyc_Absynpp_stmt2doc(_tmp3C5,
stmtexp),((_tmp904[1]=Cyc_PP_line_doc(),((_tmp904[0]=Cyc_Absynpp_decl2doc(
_tmp3C4),Cyc_PP_cat(_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL21D;_LL23A: {struct Cyc_Absyn_Label_s_struct*_tmp3C6=(struct Cyc_Absyn_Label_s_struct*)
_tmp3A0;if(_tmp3C6->tag != 13)goto _LL23C;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}}
_LL23B: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C8)){if(
stmtexp){const char*_tmp909;const char*_tmp908;struct Cyc_PP_Doc*_tmp907[8];s=((
_tmp907[7]=Cyc_PP_text(((_tmp908=");",_tag_dyneither(_tmp908,sizeof(char),3)))),((
_tmp907[6]=Cyc_Absynpp_rb(),((_tmp907[5]=Cyc_PP_line_doc(),((_tmp907[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C8,1)),((_tmp907[3]=Cyc_PP_line_doc(),((_tmp907[2]=
Cyc_Absynpp_lb(),((_tmp907[1]=Cyc_PP_text(((_tmp909=": (",_tag_dyneither(_tmp909,
sizeof(char),4)))),((_tmp907[0]=Cyc_PP_textptr(_tmp3C7),Cyc_PP_cat(
_tag_dyneither(_tmp907,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp90C;struct Cyc_PP_Doc*_tmp90B[7];s=((_tmp90B[6]=Cyc_Absynpp_rb(),((
_tmp90B[5]=Cyc_PP_line_doc(),((_tmp90B[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C8,0)),((_tmp90B[3]=Cyc_PP_line_doc(),((_tmp90B[2]=Cyc_Absynpp_lb(),((
_tmp90B[1]=Cyc_PP_text(((_tmp90C=": ",_tag_dyneither(_tmp90C,sizeof(char),3)))),((
_tmp90B[0]=Cyc_PP_textptr(_tmp3C7),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp90F;struct Cyc_PP_Doc*
_tmp90E[3];s=((_tmp90E[2]=Cyc_Absynpp_stmt2doc(_tmp3C8,stmtexp),((_tmp90E[1]=Cyc_PP_text(((
_tmp90F=": ",_tag_dyneither(_tmp90F,sizeof(char),3)))),((_tmp90E[0]=Cyc_PP_textptr(
_tmp3C7),Cyc_PP_cat(_tag_dyneither(_tmp90E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL21D;_LL23C: {struct Cyc_Absyn_Do_s_struct*_tmp3C9=(struct Cyc_Absyn_Do_s_struct*)
_tmp3A0;if(_tmp3C9->tag != 14)goto _LL23E;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;
_tmp3CC=_tmp3CB.f1;}}_LL23D:{const char*_tmp916;const char*_tmp915;const char*
_tmp914;struct Cyc_PP_Doc*_tmp913[9];s=((_tmp913[8]=Cyc_PP_text(((_tmp914=");",
_tag_dyneither(_tmp914,sizeof(char),3)))),((_tmp913[7]=Cyc_Absynpp_exp2doc(
_tmp3CC),((_tmp913[6]=Cyc_PP_text(((_tmp915=" while (",_tag_dyneither(_tmp915,
sizeof(char),9)))),((_tmp913[5]=Cyc_Absynpp_rb(),((_tmp913[4]=Cyc_PP_line_doc(),((
_tmp913[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CA,0)),((_tmp913[2]=Cyc_PP_line_doc(),((
_tmp913[1]=Cyc_Absynpp_lb(),((_tmp913[0]=Cyc_PP_text(((_tmp916="do ",
_tag_dyneither(_tmp916,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp913,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL21D;_LL23E: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3CD=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3A0;if(_tmp3CD->tag != 15)goto
_LL240;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;}}_LL23F:{const char*_tmp91B;
const char*_tmp91A;struct Cyc_PP_Doc*_tmp919[12];s=((_tmp919[11]=Cyc_Absynpp_rb(),((
_tmp919[10]=Cyc_PP_line_doc(),((_tmp919[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3CF)),((_tmp919[8]=Cyc_PP_line_doc(),((_tmp919[7]=Cyc_Absynpp_lb(),((_tmp919[
6]=Cyc_PP_text(((_tmp91A=" catch ",_tag_dyneither(_tmp91A,sizeof(char),8)))),((
_tmp919[5]=Cyc_Absynpp_rb(),((_tmp919[4]=Cyc_PP_line_doc(),((_tmp919[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3CE,0)),((_tmp919[2]=Cyc_PP_line_doc(),((_tmp919[1]=
Cyc_Absynpp_lb(),((_tmp919[0]=Cyc_PP_text(((_tmp91B="try ",_tag_dyneither(
_tmp91B,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL21D;_LL240: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3D0=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3A0;if(_tmp3D0->tag != 16)goto
_LL21D;else{_tmp3D1=_tmp3D0->f1;}}_LL241:{const char*_tmp920;const char*_tmp91F;
struct Cyc_PP_Doc*_tmp91E[3];s=((_tmp91E[2]=Cyc_PP_text(((_tmp91F=");",
_tag_dyneither(_tmp91F,sizeof(char),3)))),((_tmp91E[1]=Cyc_Absynpp_exp2doc(
_tmp3D1),((_tmp91E[0]=Cyc_PP_text(((_tmp920="reset_region(",_tag_dyneither(
_tmp920,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL21D;_LL21D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp416=p->r;enum Cyc_Absyn_Sign _tmp41A;int _tmp41B;
char _tmp41D;struct _dyneither_ptr _tmp41F;struct Cyc_Absyn_Vardecl*_tmp421;struct
Cyc_Absyn_Pat*_tmp422;struct Cyc_Absyn_Pat _tmp423;void*_tmp424;struct Cyc_Absyn_Vardecl*
_tmp427;struct Cyc_Absyn_Pat*_tmp428;struct Cyc_Absyn_Tvar*_tmp42A;struct Cyc_Absyn_Vardecl*
_tmp42B;struct Cyc_List_List*_tmp42D;int _tmp42E;struct Cyc_Absyn_Pat*_tmp430;
struct Cyc_Absyn_Vardecl*_tmp432;struct Cyc_Absyn_Pat*_tmp433;struct Cyc_Absyn_Pat
_tmp434;void*_tmp435;struct Cyc_Absyn_Vardecl*_tmp438;struct Cyc_Absyn_Pat*_tmp439;
struct _tuple0*_tmp43B;struct _tuple0*_tmp43D;struct Cyc_List_List*_tmp43E;int
_tmp43F;struct Cyc_Absyn_AggrInfo*_tmp441;struct Cyc_Absyn_AggrInfo _tmp442;union
Cyc_Absyn_AggrInfoU _tmp443;struct Cyc_List_List*_tmp444;struct Cyc_List_List*
_tmp445;int _tmp446;struct Cyc_Absyn_AggrInfo*_tmp448;struct Cyc_List_List*_tmp449;
struct Cyc_List_List*_tmp44A;int _tmp44B;struct Cyc_Absyn_Enumfield*_tmp44D;struct
Cyc_Absyn_Enumfield*_tmp44F;struct Cyc_Absyn_Datatypefield*_tmp451;struct Cyc_List_List*
_tmp452;struct Cyc_Absyn_Datatypefield*_tmp454;struct Cyc_List_List*_tmp455;int
_tmp456;struct Cyc_Absyn_Exp*_tmp458;_LL248: {struct Cyc_Absyn_Wild_p_struct*
_tmp417=(struct Cyc_Absyn_Wild_p_struct*)_tmp416;if(_tmp417->tag != 0)goto _LL24A;}
_LL249:{const char*_tmp921;s=Cyc_PP_text(((_tmp921="_",_tag_dyneither(_tmp921,
sizeof(char),2))));}goto _LL247;_LL24A: {struct Cyc_Absyn_Null_p_struct*_tmp418=(
struct Cyc_Absyn_Null_p_struct*)_tmp416;if(_tmp418->tag != 8)goto _LL24C;}_LL24B:{
const char*_tmp922;s=Cyc_PP_text(((_tmp922="NULL",_tag_dyneither(_tmp922,sizeof(
char),5))));}goto _LL247;_LL24C: {struct Cyc_Absyn_Int_p_struct*_tmp419=(struct Cyc_Absyn_Int_p_struct*)
_tmp416;if(_tmp419->tag != 9)goto _LL24E;else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}}
_LL24D: if(_tmp41A != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp926;
void*_tmp925[1];struct Cyc_Int_pa_struct _tmp924;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp924.tag=1,((_tmp924.f1=(unsigned long)_tmp41B,((_tmp925[0]=&
_tmp924,Cyc_aprintf(((_tmp926="%d",_tag_dyneither(_tmp926,sizeof(char),3))),
_tag_dyneither(_tmp925,sizeof(void*),1)))))))));}else{const char*_tmp92A;void*
_tmp929[1];struct Cyc_Int_pa_struct _tmp928;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp928.tag=1,((_tmp928.f1=(unsigned int)_tmp41B,((_tmp929[0]=& _tmp928,Cyc_aprintf(((
_tmp92A="%u",_tag_dyneither(_tmp92A,sizeof(char),3))),_tag_dyneither(_tmp929,
sizeof(void*),1)))))))));}goto _LL247;_LL24E: {struct Cyc_Absyn_Char_p_struct*
_tmp41C=(struct Cyc_Absyn_Char_p_struct*)_tmp416;if(_tmp41C->tag != 10)goto _LL250;
else{_tmp41D=_tmp41C->f1;}}_LL24F:{const char*_tmp92E;void*_tmp92D[1];struct Cyc_String_pa_struct
_tmp92C;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp92C.tag=0,((_tmp92C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp41D)),((
_tmp92D[0]=& _tmp92C,Cyc_aprintf(((_tmp92E="'%s'",_tag_dyneither(_tmp92E,sizeof(
char),5))),_tag_dyneither(_tmp92D,sizeof(void*),1)))))))));}goto _LL247;_LL250: {
struct Cyc_Absyn_Float_p_struct*_tmp41E=(struct Cyc_Absyn_Float_p_struct*)_tmp416;
if(_tmp41E->tag != 11)goto _LL252;else{_tmp41F=_tmp41E->f1;}}_LL251: s=Cyc_PP_text(
_tmp41F);goto _LL247;_LL252: {struct Cyc_Absyn_Var_p_struct*_tmp420=(struct Cyc_Absyn_Var_p_struct*)
_tmp416;if(_tmp420->tag != 1)goto _LL254;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;
_tmp423=*_tmp422;_tmp424=_tmp423.r;{struct Cyc_Absyn_Wild_p_struct*_tmp425=(
struct Cyc_Absyn_Wild_p_struct*)_tmp424;if(_tmp425->tag != 0)goto _LL254;};}}_LL253:
s=Cyc_Absynpp_qvar2doc(_tmp421->name);goto _LL247;_LL254: {struct Cyc_Absyn_Var_p_struct*
_tmp426=(struct Cyc_Absyn_Var_p_struct*)_tmp416;if(_tmp426->tag != 1)goto _LL256;
else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}_LL255:{const char*_tmp931;struct
Cyc_PP_Doc*_tmp930[3];s=((_tmp930[2]=Cyc_Absynpp_pat2doc(_tmp428),((_tmp930[1]=
Cyc_PP_text(((_tmp931=" as ",_tag_dyneither(_tmp931,sizeof(char),5)))),((_tmp930[
0]=Cyc_Absynpp_qvar2doc(_tmp427->name),Cyc_PP_cat(_tag_dyneither(_tmp930,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL247;_LL256: {struct Cyc_Absyn_TagInt_p_struct*
_tmp429=(struct Cyc_Absyn_TagInt_p_struct*)_tmp416;if(_tmp429->tag != 3)goto _LL258;
else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}_LL257:{const char*_tmp936;const
char*_tmp935;struct Cyc_PP_Doc*_tmp934[4];s=((_tmp934[3]=Cyc_PP_text(((_tmp935=">",
_tag_dyneither(_tmp935,sizeof(char),2)))),((_tmp934[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp42A)),((_tmp934[1]=Cyc_PP_text(((_tmp936="<",_tag_dyneither(_tmp936,sizeof(
char),2)))),((_tmp934[0]=Cyc_Absynpp_qvar2doc(_tmp42B->name),Cyc_PP_cat(
_tag_dyneither(_tmp934,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;_LL258: {
struct Cyc_Absyn_Tuple_p_struct*_tmp42C=(struct Cyc_Absyn_Tuple_p_struct*)_tmp416;
if(_tmp42C->tag != 4)goto _LL25A;else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}}
_LL259:{const char*_tmp93F;const char*_tmp93E;const char*_tmp93D;const char*_tmp93C;
struct Cyc_PP_Doc*_tmp93B[4];s=((_tmp93B[3]=_tmp42E?Cyc_PP_text(((_tmp93C=", ...)",
_tag_dyneither(_tmp93C,sizeof(char),7)))): Cyc_PP_text(((_tmp93D=")",
_tag_dyneither(_tmp93D,sizeof(char),2)))),((_tmp93B[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp93E=",",_tag_dyneither(_tmp93E,
sizeof(char),2))),_tmp42D),((_tmp93B[1]=Cyc_PP_text(((_tmp93F="(",_tag_dyneither(
_tmp93F,sizeof(char),2)))),((_tmp93B[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp93B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;_LL25A: {
struct Cyc_Absyn_Pointer_p_struct*_tmp42F=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp416;if(_tmp42F->tag != 5)goto _LL25C;else{_tmp430=_tmp42F->f1;}}_LL25B:{const
char*_tmp942;struct Cyc_PP_Doc*_tmp941[2];s=((_tmp941[1]=Cyc_Absynpp_pat2doc(
_tmp430),((_tmp941[0]=Cyc_PP_text(((_tmp942="&",_tag_dyneither(_tmp942,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL247;_LL25C: {struct Cyc_Absyn_Reference_p_struct*_tmp431=(struct Cyc_Absyn_Reference_p_struct*)
_tmp416;if(_tmp431->tag != 2)goto _LL25E;else{_tmp432=_tmp431->f1;_tmp433=_tmp431->f2;
_tmp434=*_tmp433;_tmp435=_tmp434.r;{struct Cyc_Absyn_Wild_p_struct*_tmp436=(
struct Cyc_Absyn_Wild_p_struct*)_tmp435;if(_tmp436->tag != 0)goto _LL25E;};}}_LL25D:{
const char*_tmp945;struct Cyc_PP_Doc*_tmp944[2];s=((_tmp944[1]=Cyc_Absynpp_qvar2doc(
_tmp432->name),((_tmp944[0]=Cyc_PP_text(((_tmp945="*",_tag_dyneither(_tmp945,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp944,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL247;_LL25E: {struct Cyc_Absyn_Reference_p_struct*_tmp437=(struct Cyc_Absyn_Reference_p_struct*)
_tmp416;if(_tmp437->tag != 2)goto _LL260;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;}}
_LL25F:{const char*_tmp94A;const char*_tmp949;struct Cyc_PP_Doc*_tmp948[4];s=((
_tmp948[3]=Cyc_Absynpp_pat2doc(_tmp439),((_tmp948[2]=Cyc_PP_text(((_tmp949=" as ",
_tag_dyneither(_tmp949,sizeof(char),5)))),((_tmp948[1]=Cyc_Absynpp_qvar2doc(
_tmp438->name),((_tmp948[0]=Cyc_PP_text(((_tmp94A="*",_tag_dyneither(_tmp94A,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp948,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL247;_LL260: {struct Cyc_Absyn_UnknownId_p_struct*_tmp43A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp416;if(_tmp43A->tag != 14)goto _LL262;else{_tmp43B=_tmp43A->f1;}}_LL261: s=Cyc_Absynpp_qvar2doc(
_tmp43B);goto _LL247;_LL262: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp43C=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp416;if(_tmp43C->tag != 15)goto _LL264;
else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;_tmp43F=_tmp43C->f3;}}_LL263: {
const char*_tmp94C;const char*_tmp94B;struct _dyneither_ptr term=_tmp43F?(_tmp94C=", ...)",
_tag_dyneither(_tmp94C,sizeof(char),7)):((_tmp94B=")",_tag_dyneither(_tmp94B,
sizeof(char),2)));{const char*_tmp951;const char*_tmp950;struct Cyc_PP_Doc*_tmp94F[
2];s=((_tmp94F[1]=Cyc_PP_group(((_tmp951="(",_tag_dyneither(_tmp951,sizeof(char),
2))),term,((_tmp950=",",_tag_dyneither(_tmp950,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp43E)),((_tmp94F[0]=Cyc_Absynpp_qvar2doc(_tmp43D),Cyc_PP_cat(
_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL247;}_LL264: {
struct Cyc_Absyn_Aggr_p_struct*_tmp440=(struct Cyc_Absyn_Aggr_p_struct*)_tmp416;
if(_tmp440->tag != 6)goto _LL266;else{_tmp441=_tmp440->f1;if(_tmp441 == 0)goto
_LL266;_tmp442=*_tmp441;_tmp443=_tmp442.aggr_info;_tmp444=_tmp440->f2;_tmp445=
_tmp440->f3;_tmp446=_tmp440->f4;}}_LL265: {const char*_tmp953;const char*_tmp952;
struct _dyneither_ptr term=_tmp446?(_tmp953=", ...}",_tag_dyneither(_tmp953,
sizeof(char),7)):((_tmp952="}",_tag_dyneither(_tmp952,sizeof(char),2)));struct
_tuple0*_tmp47B;struct _tuple9 _tmp47A=Cyc_Absyn_aggr_kinded_name(_tmp443);_tmp47B=
_tmp47A.f2;{const char*_tmp95E;const char*_tmp95D;const char*_tmp95C;const char*
_tmp95B;const char*_tmp95A;struct Cyc_PP_Doc*_tmp959[4];s=((_tmp959[3]=Cyc_PP_group(((
_tmp95B="",_tag_dyneither(_tmp95B,sizeof(char),1))),term,((_tmp95A=",",
_tag_dyneither(_tmp95A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp445)),((_tmp959[2]=Cyc_PP_egroup(((_tmp95E="[",_tag_dyneither(_tmp95E,
sizeof(char),2))),((_tmp95D="]",_tag_dyneither(_tmp95D,sizeof(char),2))),((
_tmp95C=",",_tag_dyneither(_tmp95C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp444))),((_tmp959[1]=
Cyc_Absynpp_lb(),((_tmp959[0]=Cyc_Absynpp_qvar2doc(_tmp47B),Cyc_PP_cat(
_tag_dyneither(_tmp959,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;}_LL266: {
struct Cyc_Absyn_Aggr_p_struct*_tmp447=(struct Cyc_Absyn_Aggr_p_struct*)_tmp416;
if(_tmp447->tag != 6)goto _LL268;else{_tmp448=_tmp447->f1;if(_tmp448 != 0)goto
_LL268;_tmp449=_tmp447->f2;_tmp44A=_tmp447->f3;_tmp44B=_tmp447->f4;}}_LL267: {
const char*_tmp960;const char*_tmp95F;struct _dyneither_ptr term=_tmp44B?(_tmp960=", ...}",
_tag_dyneither(_tmp960,sizeof(char),7)):((_tmp95F="}",_tag_dyneither(_tmp95F,
sizeof(char),2)));{const char*_tmp96B;const char*_tmp96A;const char*_tmp969;const
char*_tmp968;const char*_tmp967;struct Cyc_PP_Doc*_tmp966[3];s=((_tmp966[2]=Cyc_PP_group(((
_tmp968="",_tag_dyneither(_tmp968,sizeof(char),1))),term,((_tmp967=",",
_tag_dyneither(_tmp967,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp44A)),((_tmp966[1]=Cyc_PP_egroup(((_tmp96B="[",_tag_dyneither(_tmp96B,
sizeof(char),2))),((_tmp96A="]",_tag_dyneither(_tmp96A,sizeof(char),2))),((
_tmp969=",",_tag_dyneither(_tmp969,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp449))),((_tmp966[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL247;}_LL268: {struct Cyc_Absyn_Enum_p_struct*_tmp44C=(struct Cyc_Absyn_Enum_p_struct*)
_tmp416;if(_tmp44C->tag != 12)goto _LL26A;else{_tmp44D=_tmp44C->f2;}}_LL269:
_tmp44F=_tmp44D;goto _LL26B;_LL26A: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp44E=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp416;if(_tmp44E->tag != 13)goto _LL26C;else{
_tmp44F=_tmp44E->f2;}}_LL26B: s=Cyc_Absynpp_qvar2doc(_tmp44F->name);goto _LL247;
_LL26C: {struct Cyc_Absyn_Datatype_p_struct*_tmp450=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp416;if(_tmp450->tag != 7)goto _LL26E;else{_tmp451=_tmp450->f2;_tmp452=_tmp450->f3;
if(_tmp452 != 0)goto _LL26E;}}_LL26D: s=Cyc_Absynpp_qvar2doc(_tmp451->name);goto
_LL247;_LL26E: {struct Cyc_Absyn_Datatype_p_struct*_tmp453=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp416;if(_tmp453->tag != 7)goto _LL270;else{_tmp454=_tmp453->f2;_tmp455=_tmp453->f3;
_tmp456=_tmp453->f4;}}_LL26F: {const char*_tmp96D;const char*_tmp96C;struct
_dyneither_ptr term=_tmp456?(_tmp96D=", ...)",_tag_dyneither(_tmp96D,sizeof(char),
7)):((_tmp96C=")",_tag_dyneither(_tmp96C,sizeof(char),2)));{const char*_tmp972;
const char*_tmp971;struct Cyc_PP_Doc*_tmp970[2];s=((_tmp970[1]=Cyc_PP_egroup(((
_tmp972="(",_tag_dyneither(_tmp972,sizeof(char),2))),term,((_tmp971=",",
_tag_dyneither(_tmp971,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp455)),((_tmp970[0]=Cyc_Absynpp_qvar2doc(_tmp454->name),Cyc_PP_cat(
_tag_dyneither(_tmp970,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL247;}_LL270: {
struct Cyc_Absyn_Exp_p_struct*_tmp457=(struct Cyc_Absyn_Exp_p_struct*)_tmp416;if(
_tmp457->tag != 16)goto _LL247;else{_tmp458=_tmp457->f1;}}_LL271: s=Cyc_Absynpp_exp2doc(
_tmp458);goto _LL247;_LL247:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){const char*
_tmp979;const char*_tmp978;const char*_tmp977;struct Cyc_PP_Doc*_tmp976[2];return(
_tmp976[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp976[0]=Cyc_PP_egroup(((_tmp979="",
_tag_dyneither(_tmp979,sizeof(char),1))),((_tmp978="=",_tag_dyneither(_tmp978,
sizeof(char),2))),((_tmp977="=",_tag_dyneither(_tmp977,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp976,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp97E;struct Cyc_PP_Doc*_tmp97D[2];struct Cyc_PP_Doc*
_tmp97C[2];return(_tmp97C[1]=Cyc_PP_nest(2,((_tmp97D[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp97D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97D,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp97C[0]=Cyc_PP_text(((_tmp97E="default: ",
_tag_dyneither(_tmp97E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp97C,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp985;
const char*_tmp984;struct Cyc_PP_Doc*_tmp983[2];struct Cyc_PP_Doc*_tmp982[4];return(
_tmp982[3]=Cyc_PP_nest(2,((_tmp983[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp983[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp982[2]=Cyc_PP_text(((_tmp984=": ",_tag_dyneither(_tmp984,sizeof(
char),3)))),((_tmp982[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp982[0]=Cyc_PP_text(((
_tmp985="case ",_tag_dyneither(_tmp985,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp982,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp98E;const char*_tmp98D;const char*_tmp98C;struct Cyc_PP_Doc*_tmp98B[2];struct
Cyc_PP_Doc*_tmp98A[6];return(_tmp98A[5]=Cyc_PP_nest(2,((_tmp98B[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp98B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp98B,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp98A[4]=Cyc_PP_text(((_tmp98C=": ",
_tag_dyneither(_tmp98C,sizeof(char),3)))),((_tmp98A[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp98A[2]=Cyc_PP_text(((
_tmp98D=" && ",_tag_dyneither(_tmp98D,sizeof(char),5)))),((_tmp98A[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp98A[0]=Cyc_PP_text(((_tmp98E="case ",_tag_dyneither(_tmp98E,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp98A,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp98F;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp98F="",_tag_dyneither(_tmp98F,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp992;struct Cyc_PP_Doc*_tmp991[3];return(_tmp991[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp991[1]=Cyc_PP_text(((_tmp992=" = ",
_tag_dyneither(_tmp992,sizeof(char),4)))),((_tmp991[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp993;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp993=",",_tag_dyneither(_tmp993,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp994;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp994=",",_tag_dyneither(_tmp994,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp4A7;enum Cyc_Absyn_Scope _tmp4A8;struct _tuple0*_tmp4A9;struct Cyc_Absyn_Tqual
_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;
struct Cyc_Absyn_Vardecl*_tmp4A6=vd;_tmp4A7=*_tmp4A6;_tmp4A8=_tmp4A7.sc;_tmp4A9=
_tmp4A7.name;_tmp4AA=_tmp4A7.tq;_tmp4AB=_tmp4A7.type;_tmp4AC=_tmp4A7.initializer;
_tmp4AD=_tmp4A7.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4A9);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4AD);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL272:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL273:{void*_tmp4AE=Cyc_Tcutil_compress(
_tmp4AB);struct Cyc_Absyn_FnInfo _tmp4B0;struct Cyc_List_List*_tmp4B1;_LL276: {
struct Cyc_Absyn_FnType_struct*_tmp4AF=(struct Cyc_Absyn_FnType_struct*)_tmp4AE;
if(_tmp4AF->tag != 10)goto _LL278;else{_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4B0.attributes;}}
_LL277: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4B1);goto _LL275;_LL278:;_LL279:
beforenamedoc=Cyc_PP_nil_doc();goto _LL275;_LL275:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL27A: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL27B: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9A1;struct Cyc_PP_Doc*_tmp9A0[2];struct Cyc_PP_Doc*_tmp99F[2];const char*
_tmp99E;const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[5];s=((_tmp99C[4]=Cyc_PP_text(((
_tmp99D=";",_tag_dyneither(_tmp99D,sizeof(char),2)))),((_tmp99C[3]=_tmp4AC == 0?
Cyc_PP_nil_doc():((_tmp99F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4AC),((
_tmp99F[0]=Cyc_PP_text(((_tmp99E=" = ",_tag_dyneither(_tmp99E,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp99F,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp99C[2]=
Cyc_Absynpp_tqtd2doc(_tmp4AA,_tmp4AB,((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((
_tmp9A1->v=((_tmp9A0[1]=sn,((_tmp9A0[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9A0,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9A1))))),((_tmp99C[1]=Cyc_Absynpp_scope2doc(
_tmp4A8),((_tmp99C[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4B9;
struct _tuple17 _tmp4B8=*x;_tmp4B9=_tmp4B8.f2;return Cyc_Absynpp_qvar2doc(_tmp4B9);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4BA=d->r;
struct Cyc_Absyn_Fndecl*_tmp4BC;struct Cyc_Absyn_Aggrdecl*_tmp4BE;struct Cyc_Absyn_Vardecl*
_tmp4C0;struct Cyc_Absyn_Tvar*_tmp4C2;struct Cyc_Absyn_Vardecl*_tmp4C3;int _tmp4C4;
struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Tvar*
_tmp4C8;struct Cyc_Absyn_Vardecl*_tmp4C9;struct Cyc_Absyn_Datatypedecl*_tmp4CB;
struct Cyc_Absyn_Datatypedecl _tmp4CC;enum Cyc_Absyn_Scope _tmp4CD;struct _tuple0*
_tmp4CE;struct Cyc_List_List*_tmp4CF;struct Cyc_Core_Opt*_tmp4D0;int _tmp4D1;struct
Cyc_Absyn_Pat*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_List_List*_tmp4D6;
struct Cyc_Absyn_Enumdecl*_tmp4D8;struct Cyc_Absyn_Enumdecl _tmp4D9;enum Cyc_Absyn_Scope
_tmp4DA;struct _tuple0*_tmp4DB;struct Cyc_Core_Opt*_tmp4DC;struct Cyc_Absyn_Typedefdecl*
_tmp4DE;struct _dyneither_ptr*_tmp4E0;struct Cyc_List_List*_tmp4E1;struct _tuple0*
_tmp4E3;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*_tmp4E6;struct Cyc_List_List*
_tmp4E8;struct Cyc_List_List*_tmp4E9;_LL27E: {struct Cyc_Absyn_Fn_d_struct*_tmp4BB=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4BA;if(_tmp4BB->tag != 1)goto _LL280;else{_tmp4BC=
_tmp4BB->f1;}}_LL27F: {struct Cyc_Absyn_FnType_struct _tmp9A7;struct Cyc_Absyn_FnInfo
_tmp9A6;struct Cyc_Absyn_FnType_struct*_tmp9A5;void*t=(void*)((_tmp9A5=_cycalloc(
sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A7.tag=10,((_tmp9A7.f1=((_tmp9A6.tvars=
_tmp4BC->tvs,((_tmp9A6.effect=_tmp4BC->effect,((_tmp9A6.ret_typ=_tmp4BC->ret_type,((
_tmp9A6.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4BC->args),((
_tmp9A6.c_varargs=_tmp4BC->c_varargs,((_tmp9A6.cyc_varargs=_tmp4BC->cyc_varargs,((
_tmp9A6.rgn_po=_tmp4BC->rgn_po,((_tmp9A6.attributes=0,_tmp9A6)))))))))))))))),
_tmp9A7)))),_tmp9A5))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4BC->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4BC->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL29E:{const char*_tmp9A8;inlinedoc=Cyc_PP_text(((_tmp9A8="inline ",
_tag_dyneither(_tmp9A8,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL29F:{
const char*_tmp9A9;inlinedoc=Cyc_PP_text(((_tmp9A9="__inline ",_tag_dyneither(
_tmp9A9,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4BC->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2A1: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2A2: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4BC->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4BC->name);struct Cyc_PP_Doc*_tmp9AC[2];struct Cyc_Core_Opt*_tmp9AB;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9AB=_cycalloc(
sizeof(*_tmp9AB)),((_tmp9AB->v=((_tmp9AC[1]=namedoc,((_tmp9AC[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9AB)))));
struct Cyc_PP_Doc*_tmp9AF[2];struct Cyc_PP_Doc*_tmp9AE[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9AE[4]=Cyc_Absynpp_rb(),((_tmp9AE[3]=Cyc_PP_line_doc(),((_tmp9AE[2]=Cyc_PP_nest(
2,((_tmp9AF[1]=Cyc_Absynpp_stmt2doc(_tmp4BC->body,0),((_tmp9AF[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9AF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9AE[1]=
Cyc_Absynpp_lb(),((_tmp9AE[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9AE,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9B0[4];s=((
_tmp9B0[3]=bodydoc,((_tmp9B0[2]=tqtddoc,((_tmp9B0[1]=scopedoc,((_tmp9B0[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9B0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2A4:{struct Cyc_PP_Doc*
_tmp9B1[2];s=((_tmp9B1[1]=s,((_tmp9B1[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2A5: break;}goto
_LL27D;};};}_LL280: {struct Cyc_Absyn_Aggr_d_struct*_tmp4BD=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4BA;if(_tmp4BD->tag != 6)goto _LL282;else{_tmp4BE=_tmp4BD->f1;}}_LL281: if(
_tmp4BE->impl == 0){const char*_tmp9B4;struct Cyc_PP_Doc*_tmp9B3[5];s=((_tmp9B3[4]=
Cyc_PP_text(((_tmp9B4=";",_tag_dyneither(_tmp9B4,sizeof(char),2)))),((_tmp9B3[3]=
Cyc_Absynpp_ktvars2doc(_tmp4BE->tvs),((_tmp9B3[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4BE->name),((_tmp9B3[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4BE->kind),((_tmp9B3[0]=
Cyc_Absynpp_scope2doc(_tmp4BE->sc),Cyc_PP_cat(_tag_dyneither(_tmp9B3,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9BE[2];const char*
_tmp9BD;struct Cyc_PP_Doc*_tmp9BC[2];const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[
13];s=((_tmp9BA[12]=Cyc_PP_text(((_tmp9BB=";",_tag_dyneither(_tmp9BB,sizeof(char),
2)))),((_tmp9BA[11]=Cyc_Absynpp_atts2doc(_tmp4BE->attributes),((_tmp9BA[10]=Cyc_Absynpp_rb(),((
_tmp9BA[9]=Cyc_PP_line_doc(),((_tmp9BA[8]=Cyc_PP_nest(2,((_tmp9BC[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BE->impl))->fields),((_tmp9BC[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BC,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9BA[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BE->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9BE[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4BE->impl))->rgn_po),((_tmp9BE[0]=Cyc_PP_text(((_tmp9BD=":",
_tag_dyneither(_tmp9BD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9BE,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9BA[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BE->impl))->exist_vars),((_tmp9BA[5]=
Cyc_Absynpp_lb(),((_tmp9BA[4]=Cyc_PP_blank_doc(),((_tmp9BA[3]=Cyc_Absynpp_ktvars2doc(
_tmp4BE->tvs),((_tmp9BA[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4BE->name),((_tmp9BA[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4BE->kind),((_tmp9BA[0]=Cyc_Absynpp_scope2doc(
_tmp4BE->sc),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL27D;_LL282: {struct Cyc_Absyn_Var_d_struct*_tmp4BF=(struct Cyc_Absyn_Var_d_struct*)
_tmp4BA;if(_tmp4BF->tag != 0)goto _LL284;else{_tmp4C0=_tmp4BF->f1;}}_LL283: s=Cyc_Absynpp_vardecl2doc(
_tmp4C0);goto _LL27D;_LL284: {struct Cyc_Absyn_Region_d_struct*_tmp4C1=(struct Cyc_Absyn_Region_d_struct*)
_tmp4BA;if(_tmp4C1->tag != 4)goto _LL286;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;
_tmp4C4=_tmp4C1->f3;_tmp4C5=_tmp4C1->f4;}}_LL285:{const char*_tmp9D1;const char*
_tmp9D0;const char*_tmp9CF;const char*_tmp9CE;struct Cyc_PP_Doc*_tmp9CD[3];const
char*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_PP_Doc*_tmp9C9[8];s=((
_tmp9C9[7]=Cyc_PP_text(((_tmp9CA=";",_tag_dyneither(_tmp9CA,sizeof(char),2)))),((
_tmp9C9[6]=_tmp4C5 != 0?(_tmp9CD[2]=Cyc_PP_text(((_tmp9CC=")",_tag_dyneither(
_tmp9CC,sizeof(char),2)))),((_tmp9CD[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4C5),((_tmp9CD[0]=Cyc_PP_text(((_tmp9CB=" = open(",_tag_dyneither(_tmp9CB,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9CD,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9C9[5]=_tmp4C4?Cyc_PP_text(((_tmp9CE=" @resetable",
_tag_dyneither(_tmp9CE,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9C9[4]=Cyc_Absynpp_qvar2doc(
_tmp4C3->name),((_tmp9C9[3]=Cyc_PP_text(((_tmp9CF=">",_tag_dyneither(_tmp9CF,
sizeof(char),2)))),((_tmp9C9[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C2)),((
_tmp9C9[1]=Cyc_PP_text(((_tmp9D0="<",_tag_dyneither(_tmp9D0,sizeof(char),2)))),((
_tmp9C9[0]=Cyc_PP_text(((_tmp9D1="region",_tag_dyneither(_tmp9D1,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL27D;_LL286: {struct Cyc_Absyn_Alias_d_struct*_tmp4C6=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4BA;if(_tmp4C6->tag != 5)goto _LL288;else{_tmp4C7=_tmp4C6->f1;_tmp4C8=_tmp4C6->f2;
_tmp4C9=_tmp4C6->f3;}}_LL287:{const char*_tmp9DC;const char*_tmp9DB;const char*
_tmp9DA;const char*_tmp9D9;const char*_tmp9D8;struct Cyc_PP_Doc*_tmp9D7[8];s=((
_tmp9D7[7]=Cyc_PP_text(((_tmp9D8=";",_tag_dyneither(_tmp9D8,sizeof(char),2)))),((
_tmp9D7[6]=Cyc_Absynpp_exp2doc(_tmp4C7),((_tmp9D7[5]=Cyc_PP_text(((_tmp9D9=" = ",
_tag_dyneither(_tmp9D9,sizeof(char),4)))),((_tmp9D7[4]=Cyc_Absynpp_qvar2doc(
_tmp4C9->name),((_tmp9D7[3]=Cyc_PP_text(((_tmp9DA="> ",_tag_dyneither(_tmp9DA,
sizeof(char),3)))),((_tmp9D7[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C8)),((
_tmp9D7[1]=Cyc_PP_text(((_tmp9DB=" <",_tag_dyneither(_tmp9DB,sizeof(char),3)))),((
_tmp9D7[0]=Cyc_PP_text(((_tmp9DC="alias",_tag_dyneither(_tmp9DC,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9D7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL27D;_LL288: {struct Cyc_Absyn_Datatype_d_struct*_tmp4CA=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4BA;if(_tmp4CA->tag != 7)goto _LL28A;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=*_tmp4CB;
_tmp4CD=_tmp4CC.sc;_tmp4CE=_tmp4CC.name;_tmp4CF=_tmp4CC.tvs;_tmp4D0=_tmp4CC.fields;
_tmp4D1=_tmp4CC.is_extensible;}}_LL289: if(_tmp4D0 == 0){const char*_tmp9E3;const
char*_tmp9E2;const char*_tmp9E1;struct Cyc_PP_Doc*_tmp9E0[6];s=((_tmp9E0[5]=Cyc_PP_text(((
_tmp9E1=";",_tag_dyneither(_tmp9E1,sizeof(char),2)))),((_tmp9E0[4]=Cyc_Absynpp_ktvars2doc(
_tmp4CF),((_tmp9E0[3]=_tmp4D1?Cyc_Absynpp_qvar2bolddoc(_tmp4CE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4CE),((_tmp9E0[2]=Cyc_PP_text(((_tmp9E2="datatype ",_tag_dyneither(_tmp9E2,
sizeof(char),10)))),((_tmp9E0[1]=_tmp4D1?Cyc_PP_text(((_tmp9E3="@extensible",
_tag_dyneither(_tmp9E3,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9E0[0]=Cyc_Absynpp_scope2doc(
_tmp4CD),Cyc_PP_cat(_tag_dyneither(_tmp9E0,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9EC;const char*_tmp9EB;struct Cyc_PP_Doc*_tmp9EA[2];const char*
_tmp9E9;struct Cyc_PP_Doc*_tmp9E8[11];s=((_tmp9E8[10]=Cyc_PP_text(((_tmp9E9=";",
_tag_dyneither(_tmp9E9,sizeof(char),2)))),((_tmp9E8[9]=Cyc_Absynpp_rb(),((
_tmp9E8[8]=Cyc_PP_line_doc(),((_tmp9E8[7]=Cyc_PP_nest(2,((_tmp9EA[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4D0->v),((_tmp9EA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E8[6]=Cyc_Absynpp_lb(),((
_tmp9E8[5]=Cyc_PP_blank_doc(),((_tmp9E8[4]=Cyc_Absynpp_ktvars2doc(_tmp4CF),((
_tmp9E8[3]=_tmp4D1?Cyc_Absynpp_qvar2bolddoc(_tmp4CE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4CE),((_tmp9E8[2]=Cyc_PP_text(((_tmp9EB="datatype ",_tag_dyneither(_tmp9EB,
sizeof(char),10)))),((_tmp9E8[1]=_tmp4D1?Cyc_PP_text(((_tmp9EC="@extensible ",
_tag_dyneither(_tmp9EC,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E8[0]=Cyc_Absynpp_scope2doc(
_tmp4CD),Cyc_PP_cat(_tag_dyneither(_tmp9E8,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL27D;_LL28A: {struct Cyc_Absyn_Let_d_struct*_tmp4D2=(struct Cyc_Absyn_Let_d_struct*)
_tmp4BA;if(_tmp4D2->tag != 2)goto _LL28C;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f3;}}
_LL28B:{const char*_tmp9F3;const char*_tmp9F2;const char*_tmp9F1;struct Cyc_PP_Doc*
_tmp9F0[5];s=((_tmp9F0[4]=Cyc_PP_text(((_tmp9F1=";",_tag_dyneither(_tmp9F1,
sizeof(char),2)))),((_tmp9F0[3]=Cyc_Absynpp_exp2doc(_tmp4D4),((_tmp9F0[2]=Cyc_PP_text(((
_tmp9F2=" = ",_tag_dyneither(_tmp9F2,sizeof(char),4)))),((_tmp9F0[1]=Cyc_Absynpp_pat2doc(
_tmp4D3),((_tmp9F0[0]=Cyc_PP_text(((_tmp9F3="let ",_tag_dyneither(_tmp9F3,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL27D;_LL28C: {struct Cyc_Absyn_Letv_d_struct*_tmp4D5=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4BA;if(_tmp4D5->tag != 3)goto _LL28E;else{_tmp4D6=_tmp4D5->f1;}}_LL28D:{const
char*_tmp9F8;const char*_tmp9F7;struct Cyc_PP_Doc*_tmp9F6[3];s=((_tmp9F6[2]=Cyc_PP_text(((
_tmp9F7=";",_tag_dyneither(_tmp9F7,sizeof(char),2)))),((_tmp9F6[1]=Cyc_Absynpp_ids2doc(
_tmp4D6),((_tmp9F6[0]=Cyc_PP_text(((_tmp9F8="let ",_tag_dyneither(_tmp9F8,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL27D;_LL28E: {struct Cyc_Absyn_Enum_d_struct*_tmp4D7=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4BA;if(_tmp4D7->tag != 8)goto _LL290;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=*_tmp4D8;
_tmp4DA=_tmp4D9.sc;_tmp4DB=_tmp4D9.name;_tmp4DC=_tmp4D9.fields;}}_LL28F: if(
_tmp4DC == 0){const char*_tmp9FD;const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[4];s=((
_tmp9FB[3]=Cyc_PP_text(((_tmp9FC=";",_tag_dyneither(_tmp9FC,sizeof(char),2)))),((
_tmp9FB[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4DB),((_tmp9FB[1]=Cyc_PP_text(((
_tmp9FD="enum ",_tag_dyneither(_tmp9FD,sizeof(char),6)))),((_tmp9FB[0]=Cyc_Absynpp_scope2doc(
_tmp4DA),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmpA04;struct Cyc_PP_Doc*_tmpA03[2];const char*_tmpA02;struct Cyc_PP_Doc*
_tmpA01[9];s=((_tmpA01[8]=Cyc_PP_text(((_tmpA02=";",_tag_dyneither(_tmpA02,
sizeof(char),2)))),((_tmpA01[7]=Cyc_Absynpp_rb(),((_tmpA01[6]=Cyc_PP_line_doc(),((
_tmpA01[5]=Cyc_PP_nest(2,((_tmpA03[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4DC->v),((_tmpA03[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA03,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA01[4]=Cyc_Absynpp_lb(),((_tmpA01[3]=
Cyc_PP_blank_doc(),((_tmpA01[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4DB),((_tmpA01[1]=
Cyc_PP_text(((_tmpA04="enum ",_tag_dyneither(_tmpA04,sizeof(char),6)))),((
_tmpA01[0]=Cyc_Absynpp_scope2doc(_tmp4DA),Cyc_PP_cat(_tag_dyneither(_tmpA01,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL27D;_LL290: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4DD=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4BA;if(_tmp4DD->tag != 9)goto
_LL292;else{_tmp4DE=_tmp4DD->f1;}}_LL291: {void*t;if(_tmp4DE->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4DE->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4DE->kind,0);}{const char*_tmpA0E;struct Cyc_Core_Opt*_tmpA0D;struct Cyc_PP_Doc*
_tmpA0C[2];const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[4];s=((_tmpA0A[3]=Cyc_PP_text(((
_tmpA0B=";",_tag_dyneither(_tmpA0B,sizeof(char),2)))),((_tmpA0A[2]=Cyc_Absynpp_atts2doc(
_tmp4DE->atts),((_tmpA0A[1]=Cyc_Absynpp_tqtd2doc(_tmp4DE->tq,t,((_tmpA0D=
_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->v=((_tmpA0C[1]=Cyc_Absynpp_tvars2doc(
_tmp4DE->tvs),((_tmpA0C[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4DE->name),Cyc_PP_cat(
_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA0D))))),((_tmpA0A[
0]=Cyc_PP_text(((_tmpA0E="typedef ",_tag_dyneither(_tmpA0E,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL27D;}_LL292: {struct Cyc_Absyn_Namespace_d_struct*_tmp4DF=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4BA;if(_tmp4DF->tag != 10)goto _LL294;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}
_LL293: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4E0);{
const char*_tmpA13;const char*_tmpA12;struct Cyc_PP_Doc*_tmpA11[8];s=((_tmpA11[7]=
Cyc_Absynpp_rb(),((_tmpA11[6]=Cyc_PP_line_doc(),((_tmpA11[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA12="",_tag_dyneither(_tmpA12,
sizeof(char),1))),_tmp4E1),((_tmpA11[4]=Cyc_PP_line_doc(),((_tmpA11[3]=Cyc_Absynpp_lb(),((
_tmpA11[2]=Cyc_PP_blank_doc(),((_tmpA11[1]=Cyc_PP_textptr(_tmp4E0),((_tmpA11[0]=
Cyc_PP_text(((_tmpA13="namespace ",_tag_dyneither(_tmpA13,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA11,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL27D;
_LL294: {struct Cyc_Absyn_Using_d_struct*_tmp4E2=(struct Cyc_Absyn_Using_d_struct*)
_tmp4BA;if(_tmp4E2->tag != 11)goto _LL296;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}}
_LL295: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA18;const char*_tmpA17;
struct Cyc_PP_Doc*_tmpA16[8];s=((_tmpA16[7]=Cyc_Absynpp_rb(),((_tmpA16[6]=Cyc_PP_line_doc(),((
_tmpA16[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA17="",_tag_dyneither(_tmpA17,sizeof(char),1))),_tmp4E4),((_tmpA16[4]=Cyc_PP_line_doc(),((
_tmpA16[3]=Cyc_Absynpp_lb(),((_tmpA16[2]=Cyc_PP_blank_doc(),((_tmpA16[1]=Cyc_Absynpp_qvar2doc(
_tmp4E3),((_tmpA16[0]=Cyc_PP_text(((_tmpA18="using ",_tag_dyneither(_tmpA18,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA16,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA23;const char*_tmpA22;const char*_tmpA21;const char*_tmpA20;
const char*_tmpA1F;struct Cyc_PP_Doc*_tmpA1E[11];s=((_tmpA1E[10]=Cyc_PP_text(((
_tmpA1F=" */",_tag_dyneither(_tmpA1F,sizeof(char),4)))),((_tmpA1E[9]=Cyc_Absynpp_rb(),((
_tmpA1E[8]=Cyc_PP_text(((_tmpA20="/* ",_tag_dyneither(_tmpA20,sizeof(char),4)))),((
_tmpA1E[7]=Cyc_PP_line_doc(),((_tmpA1E[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA21="",_tag_dyneither(_tmpA21,sizeof(char),1))),
_tmp4E4),((_tmpA1E[5]=Cyc_PP_line_doc(),((_tmpA1E[4]=Cyc_PP_text(((_tmpA22=" */",
_tag_dyneither(_tmpA22,sizeof(char),4)))),((_tmpA1E[3]=Cyc_Absynpp_lb(),((
_tmpA1E[2]=Cyc_PP_blank_doc(),((_tmpA1E[1]=Cyc_Absynpp_qvar2doc(_tmp4E3),((
_tmpA1E[0]=Cyc_PP_text(((_tmpA23="/* using ",_tag_dyneither(_tmpA23,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA1E,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL27D;_LL296: {struct Cyc_Absyn_ExternC_d_struct*_tmp4E5=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4BA;if(_tmp4E5->tag != 12)goto _LL298;else{_tmp4E6=_tmp4E5->f1;}}_LL297: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA28;const char*_tmpA27;struct Cyc_PP_Doc*_tmpA26[6];s=((_tmpA26[5]=
Cyc_Absynpp_rb(),((_tmpA26[4]=Cyc_PP_line_doc(),((_tmpA26[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA27="",_tag_dyneither(_tmpA27,
sizeof(char),1))),_tmp4E6),((_tmpA26[2]=Cyc_PP_line_doc(),((_tmpA26[1]=Cyc_Absynpp_lb(),((
_tmpA26[0]=Cyc_PP_text(((_tmpA28="extern \"C\" ",_tag_dyneither(_tmpA28,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA33;const char*_tmpA32;const char*_tmpA31;const char*_tmpA30;
const char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[9];s=((_tmpA2E[8]=Cyc_PP_text(((
_tmpA2F=" */",_tag_dyneither(_tmpA2F,sizeof(char),4)))),((_tmpA2E[7]=Cyc_Absynpp_rb(),((
_tmpA2E[6]=Cyc_PP_text(((_tmpA30="/* ",_tag_dyneither(_tmpA30,sizeof(char),4)))),((
_tmpA2E[5]=Cyc_PP_line_doc(),((_tmpA2E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA31="",_tag_dyneither(_tmpA31,sizeof(char),1))),
_tmp4E6),((_tmpA2E[3]=Cyc_PP_line_doc(),((_tmpA2E[2]=Cyc_PP_text(((_tmpA32=" */",
_tag_dyneither(_tmpA32,sizeof(char),4)))),((_tmpA2E[1]=Cyc_Absynpp_lb(),((
_tmpA2E[0]=Cyc_PP_text(((_tmpA33="/* extern \"C\" ",_tag_dyneither(_tmpA33,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL27D;_LL298: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4E7=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4BA;if(_tmp4E7->tag != 13)
goto _LL29A;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;}}_LL299: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4E9 != 0){const char*_tmpA38;const char*_tmpA37;
struct Cyc_PP_Doc*_tmpA36[7];exs_doc=((_tmpA36[6]=Cyc_Absynpp_rb(),((_tmpA36[5]=
Cyc_PP_line_doc(),((_tmpA36[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA37=",",_tag_dyneither(_tmpA37,sizeof(char),2))),
_tmp4E9),((_tmpA36[3]=Cyc_PP_line_doc(),((_tmpA36[2]=Cyc_Absynpp_lb(),((_tmpA36[
1]=Cyc_PP_text(((_tmpA38=" export ",_tag_dyneither(_tmpA38,sizeof(char),9)))),((
_tmpA36[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA36,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA3D;const char*
_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[6];s=((_tmpA3B[5]=exs_doc,((_tmpA3B[4]=Cyc_PP_line_doc(),((
_tmpA3B[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA3C="",_tag_dyneither(_tmpA3C,sizeof(char),1))),_tmp4E8),((_tmpA3B[2]=Cyc_PP_line_doc(),((
_tmpA3B[1]=Cyc_Absynpp_lb(),((_tmpA3B[0]=Cyc_PP_text(((_tmpA3D="extern \"C include\" ",
_tag_dyneither(_tmpA3D,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA48;const char*
_tmpA47;const char*_tmpA46;const char*_tmpA45;const char*_tmpA44;struct Cyc_PP_Doc*
_tmpA43[9];s=((_tmpA43[8]=Cyc_PP_text(((_tmpA44=" */",_tag_dyneither(_tmpA44,
sizeof(char),4)))),((_tmpA43[7]=Cyc_Absynpp_rb(),((_tmpA43[6]=Cyc_PP_text(((
_tmpA45="/* ",_tag_dyneither(_tmpA45,sizeof(char),4)))),((_tmpA43[5]=Cyc_PP_line_doc(),((
_tmpA43[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA46="",_tag_dyneither(_tmpA46,sizeof(char),1))),_tmp4E8),((_tmpA43[3]=Cyc_PP_line_doc(),((
_tmpA43[2]=Cyc_PP_text(((_tmpA47=" */",_tag_dyneither(_tmpA47,sizeof(char),4)))),((
_tmpA43[1]=Cyc_Absynpp_lb(),((_tmpA43[0]=Cyc_PP_text(((_tmpA48="/* extern \"C include\" ",
_tag_dyneither(_tmpA48,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA43,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL27D;_LL29A: {struct Cyc_Absyn_Porton_d_struct*
_tmp4EA=(struct Cyc_Absyn_Porton_d_struct*)_tmp4BA;if(_tmp4EA->tag != 14)goto
_LL29C;}_LL29B:{const char*_tmpA4B;struct Cyc_PP_Doc*_tmpA4A[2];s=((_tmpA4A[1]=Cyc_Absynpp_lb(),((
_tmpA4A[0]=Cyc_PP_text(((_tmpA4B="__cyclone_port_on__;",_tag_dyneither(_tmpA4B,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA4A,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL27D;_LL29C: {struct Cyc_Absyn_Portoff_d_struct*_tmp4EB=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4BA;if(_tmp4EB->tag != 15)goto _LL27D;}_LL29D:{const char*_tmpA4E;struct Cyc_PP_Doc*
_tmpA4D[2];s=((_tmpA4D[1]=Cyc_Absynpp_lb(),((_tmpA4D[0]=Cyc_PP_text(((_tmpA4E="__cyclone_port_off__;",
_tag_dyneither(_tmpA4E,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA4D,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL27D;_LL27D:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2A7: {const char*_tmpA4F;return Cyc_PP_text(((
_tmpA4F="static ",_tag_dyneither(_tmpA4F,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL2A8: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2A9: {const char*_tmpA50;
return Cyc_PP_text(((_tmpA50="extern ",_tag_dyneither(_tmpA50,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL2AA: {const char*_tmpA51;return Cyc_PP_text(((_tmpA51="extern \"C\" ",
_tag_dyneither(_tmpA51,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2AB: {
const char*_tmpA52;return Cyc_PP_text(((_tmpA52="abstract ",_tag_dyneither(_tmpA52,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2AC: {const char*_tmpA53;return Cyc_PP_text(((
_tmpA53="register ",_tag_dyneither(_tmpA53,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp553=t;struct
Cyc_Absyn_Tvar*_tmp555;struct Cyc_List_List*_tmp557;_LL2AF: {struct Cyc_Absyn_VarType_struct*
_tmp554=(struct Cyc_Absyn_VarType_struct*)_tmp553;if(_tmp554->tag != 2)goto _LL2B1;
else{_tmp555=_tmp554->f1;}}_LL2B0: return Cyc_Tcutil_is_temp_tvar(_tmp555);_LL2B1: {
struct Cyc_Absyn_JoinEff_struct*_tmp556=(struct Cyc_Absyn_JoinEff_struct*)_tmp553;
if(_tmp556->tag != 24)goto _LL2B3;else{_tmp557=_tmp556->f1;}}_LL2B2: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp557);_LL2B3:;_LL2B4: return 0;_LL2AE:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp558=t;void**_tmp55C;void*
_tmp55D;_LL2B6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp559=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp558;if(_tmp559->tag != 13)goto _LL2B8;}_LL2B7: return 1;_LL2B8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp55A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp558;if(_tmp55A->tag != 15)goto
_LL2BA;}_LL2B9: return 1;_LL2BA: {struct Cyc_Absyn_TypedefType_struct*_tmp55B=(
struct Cyc_Absyn_TypedefType_struct*)_tmp558;if(_tmp55B->tag != 18)goto _LL2BC;
else{_tmp55C=_tmp55B->f4;if(_tmp55C == 0)goto _LL2BC;_tmp55D=*_tmp55C;}}_LL2BB:
return Cyc_Absynpp_is_anon_aggrtype(_tmp55D);_LL2BC:;_LL2BD: return 0;_LL2B5:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple16 _tmpA54;struct _tuple16 _tmp55F=(_tmpA54.f1=(void*)tms->hd,((
_tmpA54.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA54)));
void*_tmp560;void*_tmp562;_LL2BF: _tmp560=_tmp55F.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp561=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp560;if(_tmp561->tag != 2)goto
_LL2C1;};_tmp562=_tmp55F.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp563=(struct
Cyc_Absyn_Function_mod_struct*)_tmp562;if(_tmp563->tag != 3)goto _LL2C1;};_LL2C0: {
struct Cyc_List_List*_tmpA57;struct Cyc_List_List*_tmpA56;return(_tmpA56=
_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56->hd=(void*)tms->hd,((_tmpA56->tl=((
_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA57->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA57)))))),_tmpA56)))));}
_LL2C1:;_LL2C2: {struct Cyc_List_List*_tmpA58;return(_tmpA58=_region_malloc(r,
sizeof(*_tmpA58)),((_tmpA58->hd=atts,((_tmpA58->tl=tms,_tmpA58)))));}_LL2BE:;}
else{struct Cyc_List_List*_tmpA59;return(_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((
_tmpA59->hd=atts,((_tmpA59->tl=tms,_tmpA59)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp568=t;struct Cyc_Absyn_ArrayInfo
_tmp56A;void*_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;struct Cyc_Absyn_Exp*_tmp56D;
union Cyc_Absyn_Constraint*_tmp56E;struct Cyc_Position_Segment*_tmp56F;struct Cyc_Absyn_PtrInfo
_tmp571;void*_tmp572;struct Cyc_Absyn_Tqual _tmp573;struct Cyc_Absyn_PtrAtts _tmp574;
struct Cyc_Absyn_FnInfo _tmp576;struct Cyc_List_List*_tmp577;struct Cyc_Core_Opt*
_tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;int _tmp57B;struct Cyc_Absyn_VarargInfo*
_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_List_List*_tmp57E;struct Cyc_Core_Opt*
_tmp580;struct Cyc_Core_Opt*_tmp581;int _tmp582;struct _tuple0*_tmp584;struct Cyc_List_List*
_tmp585;void**_tmp586;_LL2C4: {struct Cyc_Absyn_ArrayType_struct*_tmp569=(struct
Cyc_Absyn_ArrayType_struct*)_tmp568;if(_tmp569->tag != 9)goto _LL2C6;else{_tmp56A=
_tmp569->f1;_tmp56B=_tmp56A.elt_type;_tmp56C=_tmp56A.tq;_tmp56D=_tmp56A.num_elts;
_tmp56E=_tmp56A.zero_term;_tmp56F=_tmp56A.zt_loc;}}_LL2C5: {struct Cyc_Absyn_Tqual
_tmp588;void*_tmp589;struct Cyc_List_List*_tmp58A;struct _tuple12 _tmp587=Cyc_Absynpp_to_tms(
r,_tmp56C,_tmp56B);_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;_tmp58A=_tmp587.f3;{
void*tm;if(_tmp56D == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA5C;struct Cyc_Absyn_Carray_mod_struct*
_tmpA5B;tm=(void*)((_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((_tmpA5B[0]=((
_tmpA5C.tag=0,((_tmpA5C.f1=_tmp56E,((_tmpA5C.f2=_tmp56F,_tmpA5C)))))),_tmpA5B))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA5F;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA5E;tm=(void*)((_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E[0]=((
_tmpA5F.tag=1,((_tmpA5F.f1=(struct Cyc_Absyn_Exp*)_tmp56D,((_tmpA5F.f2=_tmp56E,((
_tmpA5F.f3=_tmp56F,_tmpA5F)))))))),_tmpA5E))));}{struct Cyc_List_List*_tmpA62;
struct _tuple12 _tmpA61;return(_tmpA61.f1=_tmp588,((_tmpA61.f2=_tmp589,((_tmpA61.f3=((
_tmpA62=_region_malloc(r,sizeof(*_tmpA62)),((_tmpA62->hd=tm,((_tmpA62->tl=
_tmp58A,_tmpA62)))))),_tmpA61)))));};};}_LL2C6: {struct Cyc_Absyn_PointerType_struct*
_tmp570=(struct Cyc_Absyn_PointerType_struct*)_tmp568;if(_tmp570->tag != 5)goto
_LL2C8;else{_tmp571=_tmp570->f1;_tmp572=_tmp571.elt_typ;_tmp573=_tmp571.elt_tq;
_tmp574=_tmp571.ptr_atts;}}_LL2C7: {struct Cyc_Absyn_Tqual _tmp592;void*_tmp593;
struct Cyc_List_List*_tmp594;struct _tuple12 _tmp591=Cyc_Absynpp_to_tms(r,_tmp573,
_tmp572);_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;_tmp594=_tmp591.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA68;struct Cyc_Absyn_Pointer_mod_struct _tmpA67;struct Cyc_List_List*_tmpA66;
_tmp594=((_tmpA66=_region_malloc(r,sizeof(*_tmpA66)),((_tmpA66->hd=(void*)((
_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA67.tag=2,((
_tmpA67.f1=_tmp574,((_tmpA67.f2=tq,_tmpA67)))))),_tmpA68)))),((_tmpA66->tl=
_tmp594,_tmpA66))))));}{struct _tuple12 _tmpA69;return(_tmpA69.f1=_tmp592,((
_tmpA69.f2=_tmp593,((_tmpA69.f3=_tmp594,_tmpA69)))));};}_LL2C8: {struct Cyc_Absyn_FnType_struct*
_tmp575=(struct Cyc_Absyn_FnType_struct*)_tmp568;if(_tmp575->tag != 10)goto _LL2CA;
else{_tmp576=_tmp575->f1;_tmp577=_tmp576.tvars;_tmp578=_tmp576.effect;_tmp579=
_tmp576.ret_typ;_tmp57A=_tmp576.args;_tmp57B=_tmp576.c_varargs;_tmp57C=_tmp576.cyc_varargs;
_tmp57D=_tmp576.rgn_po;_tmp57E=_tmp576.attributes;}}_LL2C9: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp578 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp578->v)){
_tmp578=0;_tmp577=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp577);}{struct Cyc_Absyn_Tqual _tmp59A;void*_tmp59B;struct Cyc_List_List*_tmp59C;
struct _tuple12 _tmp599=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp579);
_tmp59A=_tmp599.f1;_tmp59B=_tmp599.f2;_tmp59C=_tmp599.f3;{struct Cyc_List_List*
tms=_tmp59C;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2D0: if(
_tmp57E != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA6C;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA6B;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA6B=_region_malloc(r,
sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=5,((_tmpA6C.f1=0,((_tmpA6C.f2=
_tmp57E,_tmpA6C)))))),_tmpA6B)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA7B;struct Cyc_Absyn_WithTypes_struct*_tmpA7A;struct Cyc_Absyn_WithTypes_struct
_tmpA79;struct Cyc_Absyn_Function_mod_struct _tmpA78;struct Cyc_List_List*_tmpA77;
tms=((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77->hd=(void*)((_tmpA7B=
_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA78.tag=3,((_tmpA78.f1=(
void*)((void*)((_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A[0]=((
_tmpA79.tag=1,((_tmpA79.f1=_tmp57A,((_tmpA79.f2=_tmp57B,((_tmpA79.f3=_tmp57C,((
_tmpA79.f4=_tmp578,((_tmpA79.f5=_tmp57D,_tmpA79)))))))))))),_tmpA7A))))),_tmpA78)))),
_tmpA7B)))),((_tmpA77->tl=tms,_tmpA77))))));}break;case Cyc_Cyclone_Vc_c: _LL2D1:{
struct Cyc_Absyn_Function_mod_struct*_tmpA8A;struct Cyc_Absyn_WithTypes_struct*
_tmpA89;struct Cyc_Absyn_WithTypes_struct _tmpA88;struct Cyc_Absyn_Function_mod_struct
_tmpA87;struct Cyc_List_List*_tmpA86;tms=((_tmpA86=_region_malloc(r,sizeof(*
_tmpA86)),((_tmpA86->hd=(void*)((_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((
_tmpA8A[0]=((_tmpA87.tag=3,((_tmpA87.f1=(void*)((void*)((_tmpA89=_region_malloc(
r,sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA88.tag=1,((_tmpA88.f1=_tmp57A,((_tmpA88.f2=
_tmp57B,((_tmpA88.f3=_tmp57C,((_tmpA88.f4=_tmp578,((_tmpA88.f5=_tmp57D,_tmpA88)))))))))))),
_tmpA89))))),_tmpA87)))),_tmpA8A)))),((_tmpA86->tl=tms,_tmpA86))))));}for(0;
_tmp57E != 0;_tmp57E=_tmp57E->tl){void*_tmp5A9=(void*)_tmp57E->hd;_LL2D4: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5AA=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5A9;if(_tmp5AA->tag != 1)goto _LL2D6;}_LL2D5: goto _LL2D7;_LL2D6: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5AB=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5A9;if(_tmp5AB->tag != 2)goto
_LL2D8;}_LL2D7: goto _LL2D9;_LL2D8: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5AC=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5A9;if(_tmp5AC->tag != 3)goto _LL2DA;}
_LL2D9:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA94;struct Cyc_List_List*
_tmpA93;struct Cyc_Absyn_Attributes_mod_struct _tmpA92;struct Cyc_List_List*_tmpA91;
tms=((_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91->hd=(void*)((_tmpA94=
_region_malloc(r,sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA92.tag=5,((_tmpA92.f1=0,((
_tmpA92.f2=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->hd=(void*)_tmp57E->hd,((
_tmpA93->tl=0,_tmpA93)))))),_tmpA92)))))),_tmpA94)))),((_tmpA91->tl=tms,_tmpA91))))));}
goto AfterAtts;_LL2DA:;_LL2DB: goto _LL2D3;_LL2D3:;}break;}AfterAtts: if(_tmp577 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA9A;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA99;struct Cyc_List_List*_tmpA98;tms=((_tmpA98=_region_malloc(r,sizeof(*
_tmpA98)),((_tmpA98->hd=(void*)((_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((
_tmpA9A[0]=((_tmpA99.tag=4,((_tmpA99.f1=_tmp577,((_tmpA99.f2=0,((_tmpA99.f3=1,
_tmpA99)))))))),_tmpA9A)))),((_tmpA98->tl=tms,_tmpA98))))));}{struct _tuple12
_tmpA9B;return(_tmpA9B.f1=_tmp59A,((_tmpA9B.f2=_tmp59B,((_tmpA9B.f3=tms,_tmpA9B)))));};};};
_LL2CA: {struct Cyc_Absyn_Evar_struct*_tmp57F=(struct Cyc_Absyn_Evar_struct*)
_tmp568;if(_tmp57F->tag != 1)goto _LL2CC;else{_tmp580=_tmp57F->f1;_tmp581=_tmp57F->f2;
_tmp582=_tmp57F->f3;}}_LL2CB: if(_tmp581 == 0){struct _tuple12 _tmpA9C;return(
_tmpA9C.f1=tq,((_tmpA9C.f2=t,((_tmpA9C.f3=0,_tmpA9C)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp581->v);}_LL2CC: {struct Cyc_Absyn_TypedefType_struct*_tmp583=(
struct Cyc_Absyn_TypedefType_struct*)_tmp568;if(_tmp583->tag != 18)goto _LL2CE;
else{_tmp584=_tmp583->f1;_tmp585=_tmp583->f2;_tmp586=_tmp583->f4;}}_LL2CD: if((
_tmp586 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp586)){struct _tuple12 _tmpA9D;return(_tmpA9D.f1=tq,((_tmpA9D.f2=t,((_tmpA9D.f3=
0,_tmpA9D)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp586);}_LL2CE:;_LL2CF: {
struct _tuple12 _tmpA9E;return(_tmpA9E.f1=tq,((_tmpA9E.f2=t,((_tmpA9E.f3=0,_tmpA9E)))));}
_LL2C3:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5B8=t;struct Cyc_Core_Opt*_tmp5BA;struct Cyc_Core_Opt _tmp5BB;void*
_tmp5BC;struct Cyc_Absyn_PtrInfo _tmp5BE;void*_tmp5BF;_LL2DD: {struct Cyc_Absyn_Evar_struct*
_tmp5B9=(struct Cyc_Absyn_Evar_struct*)_tmp5B8;if(_tmp5B9->tag != 1)goto _LL2DF;
else{_tmp5BA=_tmp5B9->f2;if(_tmp5BA == 0)goto _LL2DF;_tmp5BB=*_tmp5BA;_tmp5BC=(
void*)_tmp5BB.v;}}_LL2DE: return Cyc_Absynpp_is_char_ptr(_tmp5BC);_LL2DF: {struct
Cyc_Absyn_PointerType_struct*_tmp5BD=(struct Cyc_Absyn_PointerType_struct*)
_tmp5B8;if(_tmp5BD->tag != 5)goto _LL2E1;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BE.elt_typ;}}
_LL2E0: L: {void*_tmp5C0=_tmp5BF;struct Cyc_Core_Opt*_tmp5C2;struct Cyc_Core_Opt
_tmp5C3;void*_tmp5C4;void**_tmp5C6;void*_tmp5C7;enum Cyc_Absyn_Size_of _tmp5C9;
_LL2E4: {struct Cyc_Absyn_Evar_struct*_tmp5C1=(struct Cyc_Absyn_Evar_struct*)
_tmp5C0;if(_tmp5C1->tag != 1)goto _LL2E6;else{_tmp5C2=_tmp5C1->f2;if(_tmp5C2 == 0)
goto _LL2E6;_tmp5C3=*_tmp5C2;_tmp5C4=(void*)_tmp5C3.v;}}_LL2E5: _tmp5BF=_tmp5C4;
goto L;_LL2E6: {struct Cyc_Absyn_TypedefType_struct*_tmp5C5=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5C0;if(_tmp5C5->tag != 18)goto _LL2E8;else{_tmp5C6=_tmp5C5->f4;if(_tmp5C6 == 0)
goto _LL2E8;_tmp5C7=*_tmp5C6;}}_LL2E7: _tmp5BF=_tmp5C7;goto L;_LL2E8: {struct Cyc_Absyn_IntType_struct*
_tmp5C8=(struct Cyc_Absyn_IntType_struct*)_tmp5C0;if(_tmp5C8->tag != 6)goto _LL2EA;
else{_tmp5C9=_tmp5C8->f2;if(_tmp5C9 != Cyc_Absyn_Char_sz)goto _LL2EA;}}_LL2E9:
return 1;_LL2EA:;_LL2EB: return 0;_LL2E3:;}_LL2E1:;_LL2E2: return 0;_LL2DC:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5CA=_new_region("temp");struct
_RegionHandle*temp=& _tmp5CA;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5CC;
void*_tmp5CD;struct Cyc_List_List*_tmp5CE;struct _tuple12 _tmp5CB=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;_tmp5CE=_tmp5CB.f3;_tmp5CE=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5CE);if(
_tmp5CE == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA9F[2];struct Cyc_PP_Doc*_tmp5D0=(
_tmpA9F[1]=Cyc_Absynpp_ntyp2doc(_tmp5CD),((_tmpA9F[0]=Cyc_Absynpp_tqual2doc(
_tmp5CC),Cyc_PP_cat(_tag_dyneither(_tmpA9F,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5D0;}else{const char*_tmpAA2;struct Cyc_PP_Doc*_tmpAA1[
4];struct Cyc_PP_Doc*_tmp5D3=(_tmpAA1[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5CE),((_tmpAA1[2]=
Cyc_PP_text(((_tmpAA2=" ",_tag_dyneither(_tmpAA2,sizeof(char),2)))),((_tmpAA1[1]=
Cyc_Absynpp_ntyp2doc(_tmp5CD),((_tmpAA1[0]=Cyc_Absynpp_tqual2doc(_tmp5CC),Cyc_PP_cat(
_tag_dyneither(_tmpAA1,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5D3;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2EC: if(f->width != 0){struct Cyc_Core_Opt*_tmpAA9;const char*_tmpAA8;const char*
_tmpAA7;struct Cyc_PP_Doc*_tmpAA6[5];return(_tmpAA6[4]=Cyc_PP_text(((_tmpAA7=";",
_tag_dyneither(_tmpAA7,sizeof(char),2)))),((_tmpAA6[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpAA6[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpAA6[1]=Cyc_PP_text(((_tmpAA8=":",_tag_dyneither(_tmpAA8,sizeof(char),2)))),((
_tmpAA6[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA9=_cycalloc(sizeof(*
_tmpAA9)),((_tmpAA9->v=Cyc_PP_textptr(f->name),_tmpAA9))))),Cyc_PP_cat(
_tag_dyneither(_tmpAA6,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAAE;const char*_tmpAAD;struct Cyc_PP_Doc*_tmpAAC[3];return(_tmpAAC[2]=Cyc_PP_text(((
_tmpAAD=";",_tag_dyneither(_tmpAAD,sizeof(char),2)))),((_tmpAAC[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAAC[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAAE=
_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->v=Cyc_PP_textptr(f->name),_tmpAAE))))),
Cyc_PP_cat(_tag_dyneither(_tmpAAC,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2ED: if(f->width != 0){struct Cyc_Core_Opt*_tmpAB5;const char*_tmpAB4;const char*
_tmpAB3;struct Cyc_PP_Doc*_tmpAB2[5];return(_tmpAB2[4]=Cyc_PP_text(((_tmpAB3=";",
_tag_dyneither(_tmpAB3,sizeof(char),2)))),((_tmpAB2[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAB2[2]=Cyc_PP_text(((_tmpAB4=":",
_tag_dyneither(_tmpAB4,sizeof(char),2)))),((_tmpAB2[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->v=Cyc_PP_textptr(f->name),
_tmpAB5))))),((_tmpAB2[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAB2,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpABA;const char*_tmpAB9;struct Cyc_PP_Doc*_tmpAB8[3];return(_tmpAB8[2]=Cyc_PP_text(((
_tmpAB9=";",_tag_dyneither(_tmpAB9,sizeof(char),2)))),((_tmpAB8[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->v=Cyc_PP_textptr(
f->name),_tmpABA))))),((_tmpAB8[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAB8,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpABB;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpABB="",
_tag_dyneither(_tmpABB,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpABC[3];return(_tmpABC[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpABC[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpABC[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpABC,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpABD;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpABD=",",
_tag_dyneither(_tmpABD,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAC0;
void*_tmpABF;(_tmpABF=0,Cyc_fprintf(f,((_tmpAC0="\n",_tag_dyneither(_tmpAC0,
sizeof(char),2))),_tag_dyneither(_tmpABF,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAC1;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAC1="",_tag_dyneither(
_tmpAC1,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2cstring(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);struct
_dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
