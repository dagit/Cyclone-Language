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
char*tag;};static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz);inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
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
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern
 = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{
Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{
Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind
 = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual
kind;enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 
0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
v){struct Cyc_List_List*_tmp5EA;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5EA=_cycalloc(sizeof(*_tmp5EA)),((_tmp5EA->hd=v,((_tmp5EA->tl=0,_tmp5EA)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5EB;return(_tmp5EB="\\a",
_tag_dyneither(_tmp5EB,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5EC;
return(_tmp5EC="\\b",_tag_dyneither(_tmp5EC,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5ED;return(_tmp5ED="\\f",_tag_dyneither(_tmp5ED,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5EE;return(_tmp5EE="\\n",_tag_dyneither(_tmp5EE,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5EF;return(_tmp5EF="\\r",
_tag_dyneither(_tmp5EF,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5F0;
return(_tmp5F0="\\t",_tag_dyneither(_tmp5F0,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5F1;return(_tmp5F1="\\v",_tag_dyneither(_tmp5F1,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5F2;return(_tmp5F2="\\\\",_tag_dyneither(_tmp5F2,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5F3;return(_tmp5F3="\"",
_tag_dyneither(_tmp5F3,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5F4;
return(_tmp5F4="\\'",_tag_dyneither(_tmp5F4,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5F7;
char _tmp5F6;struct _dyneither_ptr _tmp5F5;(_tmp5F5=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5F6=*((char*)_check_dyneither_subscript(_tmp5F5,sizeof(
char),0)),((_tmp5F7=c,((_get_dyneither_size(_tmp5F5,sizeof(char))== 1  && (
_tmp5F6 == '\000'  && _tmp5F7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F5.curr)=
_tmp5F7)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5FA;char _tmp5F9;struct
_dyneither_ptr _tmp5F8;(_tmp5F8=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F9=*((char*)_check_dyneither_subscript(_tmp5F8,sizeof(char),0)),((_tmp5FA='\\',((
_get_dyneither_size(_tmp5F8,sizeof(char))== 1  && (_tmp5F9 == '\000'  && _tmp5FA != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F8.curr)=_tmp5FA)))))));}{char _tmp5FD;char
_tmp5FC;struct _dyneither_ptr _tmp5FB;(_tmp5FB=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5FC=*((char*)_check_dyneither_subscript(_tmp5FB,sizeof(char),0)),((
_tmp5FD=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5FB,
sizeof(char))== 1  && (_tmp5FC == '\000'  && _tmp5FD != '\000')?_throw_arraybounds():
1,*((char*)_tmp5FB.curr)=_tmp5FD)))))));}{char _tmp600;char _tmp5FF;struct
_dyneither_ptr _tmp5FE;(_tmp5FE=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5FF=*((char*)_check_dyneither_subscript(_tmp5FE,sizeof(char),0)),((_tmp600=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5FE,sizeof(char))== 1  && (
_tmp5FF == '\000'  && _tmp600 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FE.curr)=
_tmp600)))))));}{char _tmp603;char _tmp602;struct _dyneither_ptr _tmp601;(_tmp601=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp602=*((char*)
_check_dyneither_subscript(_tmp601,sizeof(char),0)),((_tmp603=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp601,sizeof(char))== 1  && (_tmp602 == '\000'  && _tmp603 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp601.curr)=_tmp603)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp606;char _tmp605;struct
_dyneither_ptr _tmp604;(_tmp604=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp605=*((
char*)_check_dyneither_subscript(_tmp604,sizeof(char),0)),((_tmp606='\\',((
_get_dyneither_size(_tmp604,sizeof(char))== 1  && (_tmp605 == '\000'  && _tmp606 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp604.curr)=_tmp606)))))));}{char _tmp609;char
_tmp608;struct _dyneither_ptr _tmp607;(_tmp607=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp608=*((char*)_check_dyneither_subscript(_tmp607,sizeof(char),0)),((
_tmp609='a',((_get_dyneither_size(_tmp607,sizeof(char))== 1  && (_tmp608 == '\000'
 && _tmp609 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp607.curr)=_tmp609)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp60C;char _tmp60B;struct
_dyneither_ptr _tmp60A;(_tmp60A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60B=*((
char*)_check_dyneither_subscript(_tmp60A,sizeof(char),0)),((_tmp60C='\\',((
_get_dyneither_size(_tmp60A,sizeof(char))== 1  && (_tmp60B == '\000'  && _tmp60C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60A.curr)=_tmp60C)))))));}{char _tmp60F;char
_tmp60E;struct _dyneither_ptr _tmp60D;(_tmp60D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60E=*((char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0)),((
_tmp60F='b',((_get_dyneither_size(_tmp60D,sizeof(char))== 1  && (_tmp60E == '\000'
 && _tmp60F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60D.curr)=_tmp60F)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp612;char _tmp611;struct
_dyneither_ptr _tmp610;(_tmp610=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp611=*((
char*)_check_dyneither_subscript(_tmp610,sizeof(char),0)),((_tmp612='\\',((
_get_dyneither_size(_tmp610,sizeof(char))== 1  && (_tmp611 == '\000'  && _tmp612 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp610.curr)=_tmp612)))))));}{char _tmp615;char
_tmp614;struct _dyneither_ptr _tmp613;(_tmp613=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp614=*((char*)_check_dyneither_subscript(_tmp613,sizeof(char),0)),((
_tmp615='f',((_get_dyneither_size(_tmp613,sizeof(char))== 1  && (_tmp614 == '\000'
 && _tmp615 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp613.curr)=_tmp615)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp618;char _tmp617;struct
_dyneither_ptr _tmp616;(_tmp616=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp617=*((
char*)_check_dyneither_subscript(_tmp616,sizeof(char),0)),((_tmp618='\\',((
_get_dyneither_size(_tmp616,sizeof(char))== 1  && (_tmp617 == '\000'  && _tmp618 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp616.curr)=_tmp618)))))));}{char _tmp61B;char
_tmp61A;struct _dyneither_ptr _tmp619;(_tmp619=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61A=*((char*)_check_dyneither_subscript(_tmp619,sizeof(char),0)),((
_tmp61B='n',((_get_dyneither_size(_tmp619,sizeof(char))== 1  && (_tmp61A == '\000'
 && _tmp61B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp619.curr)=_tmp61B)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp61E;char _tmp61D;struct
_dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61D=*((
char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E='\\',((
_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}{char _tmp621;char
_tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((
_tmp621='r',((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'
 && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp624;char _tmp623;struct
_dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp623=*((
char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624='\\',((
_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}{char _tmp627;char
_tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((
_tmp627='t',((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'
 && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp62A;char _tmp629;struct
_dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp629=*((
char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A='\\',((
_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && _tmp62A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}{char _tmp62D;char
_tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62C=*((char*)_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((
_tmp62D='v',((_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'
 && _tmp62D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp630;char _tmp62F;struct
_dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62F=*((
char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((_tmp630='\\',((
_get_dyneither_size(_tmp62E,sizeof(char))== 1  && (_tmp62F == '\000'  && _tmp630 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62E.curr)=_tmp630)))))));}{char _tmp633;char
_tmp632;struct _dyneither_ptr _tmp631;(_tmp631=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp632=*((char*)_check_dyneither_subscript(_tmp631,sizeof(char),0)),((
_tmp633='\\',((_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'
 && _tmp633 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp636;char _tmp635;struct
_dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp635=*((
char*)_check_dyneither_subscript(_tmp634,sizeof(char),0)),((_tmp636='\\',((
_get_dyneither_size(_tmp634,sizeof(char))== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp634.curr)=_tmp636)))))));}{char _tmp639;char
_tmp638;struct _dyneither_ptr _tmp637;(_tmp637=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp638=*((char*)_check_dyneither_subscript(_tmp637,sizeof(char),0)),((
_tmp639='"',((_get_dyneither_size(_tmp637,sizeof(char))== 1  && (_tmp638 == '\000'
 && _tmp639 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp637.curr)=_tmp639)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp63C;char _tmp63B;
struct _dyneither_ptr _tmp63A;(_tmp63A=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp63B=*((char*)_check_dyneither_subscript(_tmp63A,sizeof(char),0)),((_tmp63C=
_tmp1E,((_get_dyneither_size(_tmp63A,sizeof(char))== 1  && (_tmp63B == '\000'  && 
_tmp63C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63A.curr)=_tmp63C)))))));}
else{{char _tmp63F;char _tmp63E;struct _dyneither_ptr _tmp63D;(_tmp63D=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63E=*((char*)
_check_dyneither_subscript(_tmp63D,sizeof(char),0)),((_tmp63F='\\',((
_get_dyneither_size(_tmp63D,sizeof(char))== 1  && (_tmp63E == '\000'  && _tmp63F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63D.curr)=_tmp63F)))))));}{char _tmp642;char
_tmp641;struct _dyneither_ptr _tmp640;(_tmp640=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp641=*((char*)_check_dyneither_subscript(_tmp640,sizeof(char),0)),((
_tmp642=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp640,sizeof(char))
== 1  && (_tmp641 == '\000'  && _tmp642 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp640.curr)=_tmp642)))))));}{char _tmp645;char _tmp644;struct _dyneither_ptr
_tmp643;(_tmp643=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp644=*((char*)
_check_dyneither_subscript(_tmp643,sizeof(char),0)),((_tmp645=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp643,sizeof(char))== 1  && (_tmp644 == '\000'
 && _tmp645 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp643.curr)=_tmp645)))))));}{
char _tmp648;char _tmp647;struct _dyneither_ptr _tmp646;(_tmp646=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp647=*((char*)_check_dyneither_subscript(_tmp646,
sizeof(char),0)),((_tmp648=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp646,sizeof(char))== 1  && (_tmp647 == '\000'  && _tmp648 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp646.curr)=_tmp648)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp649;l=((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649->hd=
Cyc_Absynpp_restrict_sp,((_tmp649->tl=l,_tmp649))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp64A;l=((_tmp64A=_cycalloc(sizeof(*_tmp64A)),((_tmp64A->hd=Cyc_Absynpp_volatile_sp,((
_tmp64A->tl=l,_tmp64A))))));}if(tq.print_const){struct Cyc_List_List*_tmp64B;l=((
_tmp64B=_cycalloc(sizeof(*_tmp64B)),((_tmp64B->hd=Cyc_Absynpp_const_sp,((_tmp64B->tl=
l,_tmp64B))))));}{const char*_tmp64E;const char*_tmp64D;const char*_tmp64C;return
Cyc_PP_egroup(((_tmp64C="",_tag_dyneither(_tmp64C,sizeof(char),1))),((_tmp64D=" ",
_tag_dyneither(_tmp64D,sizeof(char),2))),((_tmp64E=" ",_tag_dyneither(_tmp64E,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*
_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(
_tmp6F){case Cyc_Absyn_AnyKind: _LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp64F;return(_tmp64F="A",_tag_dyneither(_tmp64F,sizeof(char),2));}
case Cyc_Absyn_Unique: _LL39: {const char*_tmp650;return(_tmp650="UA",
_tag_dyneither(_tmp650,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {const char*
_tmp651;return(_tmp651="TA",_tag_dyneither(_tmp651,sizeof(char),3));}}case Cyc_Absyn_MemKind:
_LL37: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3D: {const char*_tmp652;return(
_tmp652="M",_tag_dyneither(_tmp652,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp653;return(_tmp653="UM",_tag_dyneither(_tmp653,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3F: {const char*_tmp654;return(_tmp654="TM",_tag_dyneither(
_tmp654,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL42: {const char*_tmp655;return(_tmp655="B",_tag_dyneither(_tmp655,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL43: {const char*_tmp656;return(_tmp656="UB",
_tag_dyneither(_tmp656,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {const char*
_tmp657;return(_tmp657="TB",_tag_dyneither(_tmp657,sizeof(char),3));}}case Cyc_Absyn_RgnKind:
_LL41: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL47: {const char*_tmp658;return(
_tmp658="R",_tag_dyneither(_tmp658,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp659;return(_tmp659="UR",_tag_dyneither(_tmp659,sizeof(char),3));}
case Cyc_Absyn_Top: _LL49: {const char*_tmp65A;return(_tmp65A="TR",_tag_dyneither(
_tmp65A,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {const char*_tmp65B;
return(_tmp65B="E",_tag_dyneither(_tmp65B,sizeof(char),2));}case Cyc_Absyn_IntKind:
_LL4B: {const char*_tmp65C;return(_tmp65C="I",_tag_dyneither(_tmp65C,sizeof(char),
2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k);struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp65D;
return(_tmp65D="{?}",_tag_dyneither(_tmp65D,sizeof(char),4));}else{const char*
_tmp65E;return(_tmp65E="?",_tag_dyneither(_tmp65E,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp662;
void*_tmp661[1];struct Cyc_String_pa_struct _tmp660;return(struct _dyneither_ptr)((
_tmp660.tag=0,((_tmp660.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp661[0]=& _tmp660,Cyc_aprintf(((_tmp662="<=%s",_tag_dyneither(
_tmp662,sizeof(char),5))),_tag_dyneither(_tmp661,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp8A=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8C;struct
Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(
Cyc_Absynpp_kind2string(_tmp8C));_LL57: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}
_LL58: if(Cyc_PP_tex_output){const char*_tmp663;return Cyc_PP_text_width(((_tmp663="{?}",
_tag_dyneither(_tmp663,sizeof(char),4))),1);}else{const char*_tmp664;return Cyc_PP_text(((
_tmp664="?",_tag_dyneither(_tmp664,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp667;const char*_tmp666;
const char*_tmp665;return Cyc_PP_egroup(((_tmp665="<",_tag_dyneither(_tmp665,
sizeof(char),2))),((_tmp666=">",_tag_dyneither(_tmp666,sizeof(char),2))),((
_tmp667=",",_tag_dyneither(_tmp667,sizeof(char),2))),((struct Cyc_List_List*(*)(
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
_tmp9F=_tmp9E->f1;}}_LL63: {const char*_tmp66A;struct Cyc_PP_Doc*_tmp669[3];return(
_tmp669[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp669[1]=Cyc_PP_text(((_tmp66A="::",
_tag_dyneither(_tmp66A,sizeof(char),3)))),((_tmp669[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp669,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp66D;const char*
_tmp66C;const char*_tmp66B;return Cyc_PP_egroup(((_tmp66B="<",_tag_dyneither(
_tmp66B,sizeof(char),2))),((_tmp66C=">",_tag_dyneither(_tmp66C,sizeof(char),2))),((
_tmp66D=",",_tag_dyneither(_tmp66D,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp670;const char*_tmp66F;const char*_tmp66E;return Cyc_PP_egroup(((_tmp66E="<",
_tag_dyneither(_tmp66E,sizeof(char),2))),((_tmp66F=">",_tag_dyneither(_tmp66F,
sizeof(char),2))),((_tmp670=",",_tag_dyneither(_tmp670,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple15*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp673;const char*_tmp672;const char*_tmp671;return Cyc_PP_group(((
_tmp671="(",_tag_dyneither(_tmp671,sizeof(char),2))),((_tmp672=")",
_tag_dyneither(_tmp672,sizeof(char),2))),((_tmp673=",",_tag_dyneither(_tmp673,
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
_LL6F: {const char*_tmp674;return Cyc_PP_text(((_tmp674=" _stdcall ",
_tag_dyneither(_tmp674,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}
_LL71: {const char*_tmp675;return Cyc_PP_text(((_tmp675=" _cdecl ",_tag_dyneither(
_tmp675,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*_tmpB2=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp676;return Cyc_PP_text(((_tmp676=" _fastcall ",_tag_dyneither(
_tmp676,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}return Cyc_PP_nil_doc();}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*
atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmpB6=(void*)atts2->hd;_LL77: {
struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78: goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}
_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto
_LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp681;const char*
_tmp680;const char*_tmp67F;const char*_tmp67E;const char*_tmp67D;struct Cyc_PP_Doc*
_tmp67C[3];return(_tmp67C[2]=Cyc_PP_text(((_tmp67D=")",_tag_dyneither(_tmp67D,
sizeof(char),2)))),((_tmp67C[1]=Cyc_PP_group(((_tmp680="",_tag_dyneither(_tmp680,
sizeof(char),1))),((_tmp67F="",_tag_dyneither(_tmp67F,sizeof(char),1))),((
_tmp67E=" ",_tag_dyneither(_tmp67E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp67C[0]=Cyc_PP_text(((_tmp681=" __declspec(",_tag_dyneither(_tmp681,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp67C,sizeof(struct Cyc_PP_Doc*),
3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c:
_LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp68A;const char*_tmp689;const char*_tmp688;const char*_tmp687;struct
Cyc_PP_Doc*_tmp686[2];return(_tmp686[1]=Cyc_PP_group(((_tmp689="((",
_tag_dyneither(_tmp689,sizeof(char),3))),((_tmp688="))",_tag_dyneither(_tmp688,
sizeof(char),3))),((_tmp687=",",_tag_dyneither(_tmp687,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp686[0]=Cyc_PP_text(((_tmp68A=" __attribute__",
_tag_dyneither(_tmp68A,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp686,
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
_tmp68B;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp68B="{?}",_tag_dyneither(_tmp68B,sizeof(char),4))),1);}else{const char*
_tmp68C;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68C="?",
_tag_dyneither(_tmp68C,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp68D;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp68D="{\\lb}",
_tag_dyneither(_tmp68D,sizeof(char),6))),1);}else{const char*_tmp68E;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68E="{",_tag_dyneither(_tmp68E,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp68F;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp68F="{\\rb}",_tag_dyneither(_tmp68F,sizeof(char),6))),1);}else{const char*
_tmp690;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp690="}",
_tag_dyneither(_tmp690,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp691;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp691="\\$",
_tag_dyneither(_tmp691,sizeof(char),3))),1);}else{const char*_tmp692;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp692="$",_tag_dyneither(_tmp692,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp693[3];return(_tmp693[2]=Cyc_Absynpp_rb(),((_tmp693[1]=
Cyc_PP_seq(sep,ss),((_tmp693[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp693,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*
_tmpD7;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp696;void*_tmp695;(
_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="Carray_mod ",_tag_dyneither(_tmp696,
sizeof(char),12))),_tag_dyneither(_tmp695,sizeof(void*),0)));}goto _LL8C;_LL8F: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp699;void*_tmp698;(
_tmp698=0,Cyc_fprintf(Cyc_stderr,((_tmp699="ConstArray_mod ",_tag_dyneither(
_tmp699,sizeof(char),16))),_tag_dyneither(_tmp698,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp69C;void*_tmp69B;(_tmp69B=0,Cyc_fprintf(
Cyc_stderr,((_tmp69C="Function_mod(",_tag_dyneither(_tmp69C,sizeof(char),14))),
_tag_dyneither(_tmp69B,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple7*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,((_tmp69F="?",
_tag_dyneither(_tmp69F,sizeof(char),2))),_tag_dyneither(_tmp69E,sizeof(void*),0)));}
else{void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6A3;void*_tmp6A2;(_tmp6A2=0,Cyc_fprintf(Cyc_stderr,((_tmp6A3=",",
_tag_dyneither(_tmp6A3,sizeof(char),2))),_tag_dyneither(_tmp6A2,sizeof(void*),0)));}}{
const char*_tmp6A6;void*_tmp6A5;(_tmp6A5=0,Cyc_fprintf(Cyc_stderr,((_tmp6A6=") ",
_tag_dyneither(_tmp6A6,sizeof(char),3))),_tag_dyneither(_tmp6A5,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6A9;void*_tmp6A8;(
_tmp6A8=0,Cyc_fprintf(Cyc_stderr,((_tmp6A9="Function_mod()",_tag_dyneither(
_tmp6A9,sizeof(char),15))),_tag_dyneither(_tmp6A8,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6AC;void*_tmp6AB;(
_tmp6AB=0,Cyc_fprintf(Cyc_stderr,((_tmp6AC="Attributes_mod ",_tag_dyneither(
_tmp6AC,sizeof(char),16))),_tag_dyneither(_tmp6AB,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6AF;void*_tmp6AE;(
_tmp6AE=0,Cyc_fprintf(Cyc_stderr,((_tmp6AF="TypeParams_mod ",_tag_dyneither(
_tmp6AF,sizeof(char),16))),_tag_dyneither(_tmp6AE,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6B2;void*_tmp6B1;(
_tmp6B1=0,Cyc_fprintf(Cyc_stderr,((_tmp6B2="Pointer_mod ",_tag_dyneither(_tmp6B2,
sizeof(char),13))),_tag_dyneither(_tmp6B1,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6B5;void*_tmp6B4;(_tmp6B4=0,Cyc_fprintf(Cyc_stderr,((_tmp6B5="\n",
_tag_dyneither(_tmp6B5,sizeof(char),2))),_tag_dyneither(_tmp6B4,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6BA;const char*_tmp6B9;struct Cyc_PP_Doc*
_tmp6B8[3];struct Cyc_PP_Doc*p_rest=(_tmp6B8[2]=Cyc_PP_text(((_tmp6B9=")",
_tag_dyneither(_tmp6B9,sizeof(char),2)))),((_tmp6B8[1]=rest,((_tmp6B8[0]=Cyc_PP_text(((
_tmp6BA="(",_tag_dyneither(_tmp6BA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6B8,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6BF;const char*_tmp6BE;struct Cyc_PP_Doc*_tmp6BD[2];return(_tmp6BD[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6BE="[]@zeroterm",_tag_dyneither(_tmp6BE,sizeof(char),12)))): Cyc_PP_text(((
_tmp6BF="[]",_tag_dyneither(_tmp6BF,sizeof(char),3)))),((_tmp6BD[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6C6;const char*_tmp6C5;const char*_tmp6C4;
struct Cyc_PP_Doc*_tmp6C3[4];return(_tmp6C3[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6C4="]@zeroterm",
_tag_dyneither(_tmp6C4,sizeof(char),11)))): Cyc_PP_text(((_tmp6C5="]",
_tag_dyneither(_tmp6C5,sizeof(char),2)))),((_tmp6C3[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6C3[1]=Cyc_PP_text(((_tmp6C6="[",_tag_dyneither(_tmp6C6,sizeof(
char),2)))),((_tmp6C3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C3,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6C7[2];return(_tmp6C7[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6C7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C7,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6CE;const char*
_tmp6CD;const char*_tmp6CC;struct Cyc_PP_Doc*_tmp6CB[2];return(_tmp6CB[1]=Cyc_PP_group(((
_tmp6CE="(",_tag_dyneither(_tmp6CE,sizeof(char),2))),((_tmp6CD=")",
_tag_dyneither(_tmp6CD,sizeof(char),2))),((_tmp6CC=",",_tag_dyneither(_tmp6CC,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6CB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CB,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6CF[2];return(_tmp6CF[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6CF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CF,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6D0[2];return(_tmp6D0[1]=rest,((_tmp6D0[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6D0,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6D1[2];return(_tmp6D1[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6D1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D1,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6D2[2];return(_tmp6D2[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6D2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D2,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6D4;const char*_tmp6D3;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6D4="*",_tag_dyneither(_tmp6D4,sizeof(char),2)):((_tmp6D3="@",_tag_dyneither(
_tmp6D3,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple10
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp6D5[4];ptr=((_tmp6D5[
3]=Cyc_Absynpp_rb(),((_tmp6D5[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6D5[1]=Cyc_Absynpp_lb(),((
_tmp6D5[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6D5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp6D8;struct Cyc_PP_Doc*_tmp6D7[2];ptr=((_tmp6D7[1]=Cyc_PP_text(((_tmp6D8="@zeroterm ",
_tag_dyneither(_tmp6D8,sizeof(char),11)))),((_tmp6D7[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6D7,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp6DB;struct Cyc_PP_Doc*_tmp6DA[2];ptr=((_tmp6DA[1]=Cyc_PP_text(((
_tmp6DB="@nozeroterm ",_tag_dyneither(_tmp6DB,sizeof(char),13)))),((_tmp6DA[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6DA,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp6DE;struct Cyc_PP_Doc*_tmp6DD[
3];ptr=((_tmp6DD[2]=Cyc_PP_text(((_tmp6DE=" ",_tag_dyneither(_tmp6DE,sizeof(char),
2)))),((_tmp6DD[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp6DD[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp6DF[2];ptr=((_tmp6DF[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp6DF[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6DF,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6E0[2];return(_tmp6E0[1]=rest,((_tmp6E0[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6E0,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp13B=Cyc_Tcutil_compress(
t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp13B;if(_tmp13C->tag != 21)goto _LLBF;}_LLBE: {const char*_tmp6E1;return Cyc_PP_text(((
_tmp6E1="`H",_tag_dyneither(_tmp6E1,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp13D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 22)goto
_LLC1;}_LLC0: {const char*_tmp6E2;return Cyc_PP_text(((_tmp6E2="`U",_tag_dyneither(
_tmp6E2,sizeof(char),3))));}_LLC1:;_LLC2: return Cyc_Absynpp_ntyp2doc(t);_LLBC:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp140=Cyc_Tcutil_compress(t);void*
_tmp142;struct Cyc_List_List*_tmp144;_LLC4: {struct Cyc_Absyn_AccessEff_struct*
_tmp141=(struct Cyc_Absyn_AccessEff_struct*)_tmp140;if(_tmp141->tag != 23)goto
_LLC6;else{_tmp142=(void*)_tmp141->f1;}}_LLC5:{struct Cyc_List_List*_tmp6E3;*
rgions=((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3->hd=Cyc_Absynpp_rgn2doc(
_tmp142),((_tmp6E3->tl=*rgions,_tmp6E3))))));}goto _LLC3;_LLC6: {struct Cyc_Absyn_JoinEff_struct*
_tmp143=(struct Cyc_Absyn_JoinEff_struct*)_tmp140;if(_tmp143->tag != 24)goto _LLC8;
else{_tmp144=_tmp143->f1;}}_LLC7: for(0;_tmp144 != 0;_tmp144=_tmp144->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp144->hd);}goto _LLC3;_LLC8:;_LLC9:{struct Cyc_List_List*
_tmp6E4;*effects=((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6E4->tl=*effects,_tmp6E4))))));}goto _LLC3;_LLC3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6E7;const char*_tmp6E6;const
char*_tmp6E5;return Cyc_PP_group(((_tmp6E5="",_tag_dyneither(_tmp6E5,sizeof(char),
1))),((_tmp6E6="",_tag_dyneither(_tmp6E6,sizeof(char),1))),((_tmp6E7="+",
_tag_dyneither(_tmp6E7,sizeof(char),2))),effects);}else{const char*_tmp6E8;struct
Cyc_PP_Doc*_tmp14A=Cyc_Absynpp_group_braces(((_tmp6E8=",",_tag_dyneither(_tmp6E8,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6EC;const char*_tmp6EB;const
char*_tmp6EA;const char*_tmp6E9;return Cyc_PP_group(((_tmp6E9="",_tag_dyneither(
_tmp6E9,sizeof(char),1))),((_tmp6EA="",_tag_dyneither(_tmp6EA,sizeof(char),1))),((
_tmp6EB="+",_tag_dyneither(_tmp6EB,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC->hd=_tmp14A,((_tmp6EC->tl=0,
_tmp6EC))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLCA: {const char*_tmp6ED;return Cyc_PP_text(((_tmp6ED="struct ",
_tag_dyneither(_tmp6ED,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCB: {const
char*_tmp6EE;return Cyc_PP_text(((_tmp6EE="union ",_tag_dyneither(_tmp6EE,sizeof(
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
_LLCF: {const char*_tmp6EF;return Cyc_PP_text(((_tmp6EF="[[[array]]]",
_tag_dyneither(_tmp6EF,sizeof(char),12))));}_LLD0: {struct Cyc_Absyn_FnType_struct*
_tmp154=(struct Cyc_Absyn_FnType_struct*)_tmp152;if(_tmp154->tag != 10)goto _LLD2;}
_LLD1: return Cyc_PP_nil_doc();_LLD2: {struct Cyc_Absyn_PointerType_struct*_tmp155=(
struct Cyc_Absyn_PointerType_struct*)_tmp152;if(_tmp155->tag != 5)goto _LLD4;}_LLD3:
return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_VoidType_struct*_tmp156=(struct
Cyc_Absyn_VoidType_struct*)_tmp152;if(_tmp156->tag != 0)goto _LLD6;}_LLD5:{const
char*_tmp6F0;s=Cyc_PP_text(((_tmp6F0="void",_tag_dyneither(_tmp6F0,sizeof(char),
5))));}goto _LLCD;_LLD6: {struct Cyc_Absyn_Evar_struct*_tmp157=(struct Cyc_Absyn_Evar_struct*)
_tmp152;if(_tmp157->tag != 1)goto _LLD8;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LLD7: if(_tmp159 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp159->v);else{const char*_tmp700;const char*_tmp6FF;const char*_tmp6FE;
struct Cyc_Int_pa_struct _tmp6FD;void*_tmp6FC[1];const char*_tmp6FB;const char*
_tmp6FA;struct Cyc_PP_Doc*_tmp6F9[6];s=((_tmp6F9[5]=_tmp158 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp158->v),((_tmp6F9[4]=Cyc_PP_text(((
_tmp6FA=")::",_tag_dyneither(_tmp6FA,sizeof(char),4)))),((_tmp6F9[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp15B == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp15B->v),((_tmp6F9[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6FD.tag=1,((
_tmp6FD.f1=(unsigned long)_tmp15A,((_tmp6FC[0]=& _tmp6FD,Cyc_aprintf(((_tmp6FB="%d",
_tag_dyneither(_tmp6FB,sizeof(char),3))),_tag_dyneither(_tmp6FC,sizeof(void*),1))))))))),((
_tmp6F9[1]=Cyc_PP_text(((_tmp6FE="(",_tag_dyneither(_tmp6FE,sizeof(char),2)))),((
_tmp6F9[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6FF="\\%",
_tag_dyneither(_tmp6FF,sizeof(char),3)))): Cyc_PP_text(((_tmp700="%",
_tag_dyneither(_tmp700,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6F9,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: {struct Cyc_Absyn_VarType_struct*
_tmp15C=(struct Cyc_Absyn_VarType_struct*)_tmp152;if(_tmp15C->tag != 2)goto _LLDA;
else{_tmp15D=_tmp15C->f1;}}_LLD9: s=Cyc_PP_textptr(_tmp15D->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp703;struct Cyc_PP_Doc*_tmp702[3];s=((_tmp702[2]=Cyc_Absynpp_kindbound2doc(
_tmp15D->kind),((_tmp702[1]=Cyc_PP_text(((_tmp703="::",_tag_dyneither(_tmp703,
sizeof(char),3)))),((_tmp702[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp702,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15D)){const char*_tmp708;const char*_tmp707;struct Cyc_PP_Doc*_tmp706[3];s=((
_tmp706[2]=Cyc_PP_text(((_tmp707=" */",_tag_dyneither(_tmp707,sizeof(char),4)))),((
_tmp706[1]=s,((_tmp706[0]=Cyc_PP_text(((_tmp708="_ /* ",_tag_dyneither(_tmp708,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp706,sizeof(struct Cyc_PP_Doc*),3))))))));}
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
_LL106: {const char*_tmp709;struct Cyc_PP_Doc*_tmp1A4=Cyc_PP_text(((_tmp709="datatype ",
_tag_dyneither(_tmp709,sizeof(char),10))));const char*_tmp70A;struct Cyc_PP_Doc*
_tmp1A5=_tmp1A3?Cyc_PP_text(((_tmp70A="@extensible ",_tag_dyneither(_tmp70A,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp70B[4];s=((_tmp70B[3]=
Cyc_Absynpp_tps2doc(_tmp161),((_tmp70B[2]=Cyc_Absynpp_qvar2doc(_tmp1A2),((
_tmp70B[1]=_tmp1A4,((_tmp70B[0]=_tmp1A5,Cyc_PP_cat(_tag_dyneither(_tmp70B,
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
_tmp70D;const char*_tmp70C;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B2?(_tmp70D="@extensible datatype ",
_tag_dyneither(_tmp70D,sizeof(char),22)):((_tmp70C="datatype ",_tag_dyneither(
_tmp70C,sizeof(char),10))));{const char*_tmp710;struct Cyc_PP_Doc*_tmp70F[4];s=((
_tmp70F[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp70F[2]=Cyc_PP_text(((_tmp710=".",
_tag_dyneither(_tmp710,sizeof(char),2)))),((_tmp70F[1]=Cyc_Absynpp_qvar2doc(
_tmp1B1),((_tmp70F[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL107;}_LL107:;}goto _LLCD;_LLDE: {struct Cyc_Absyn_IntType_struct*
_tmp166=(struct Cyc_Absyn_IntType_struct*)_tmp152;if(_tmp166->tag != 6)goto _LLE0;
else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp167){case Cyc_Absyn_None: _LL10C: goto _LL10D;case
Cyc_Absyn_Signed: _LL10D:{const char*_tmp711;sns=((_tmp711="",_tag_dyneither(
_tmp711,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL10E:{const char*
_tmp712;sns=((_tmp712="unsigned ",_tag_dyneither(_tmp712,sizeof(char),10)));}
break;}switch(_tmp168){case Cyc_Absyn_Char_sz: _LL110: switch(_tmp167){case Cyc_Absyn_None:
_LL112:{const char*_tmp713;sns=((_tmp713="",_tag_dyneither(_tmp713,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL113:{const char*_tmp714;sns=((_tmp714="signed ",
_tag_dyneither(_tmp714,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL114:{
const char*_tmp715;sns=((_tmp715="unsigned ",_tag_dyneither(_tmp715,sizeof(char),
10)));}break;}{const char*_tmp716;ts=((_tmp716="char",_tag_dyneither(_tmp716,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL111:{const char*_tmp717;ts=((
_tmp717="short",_tag_dyneither(_tmp717,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL116:{const char*_tmp718;ts=((_tmp718="int",_tag_dyneither(_tmp718,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL117:{const char*_tmp719;ts=((_tmp719="long",
_tag_dyneither(_tmp719,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL118:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL11A:{const char*_tmp71A;
ts=((_tmp71A="long long",_tag_dyneither(_tmp71A,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL11B:{const char*_tmp71B;ts=((_tmp71B="__int64",_tag_dyneither(
_tmp71B,sizeof(char),8)));}break;}break;}{const char*_tmp720;void*_tmp71F[2];
struct Cyc_String_pa_struct _tmp71E;struct Cyc_String_pa_struct _tmp71D;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp71D.tag=0,((_tmp71D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp71E.tag=0,((_tmp71E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp71F[0]=& _tmp71E,((_tmp71F[1]=& _tmp71D,Cyc_aprintf(((
_tmp720="%s%s",_tag_dyneither(_tmp720,sizeof(char),5))),_tag_dyneither(_tmp71F,
sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: {struct Cyc_Absyn_FloatType_struct*
_tmp169=(struct Cyc_Absyn_FloatType_struct*)_tmp152;if(_tmp169->tag != 7)goto _LLE2;}
_LLE1:{const char*_tmp721;s=Cyc_PP_text(((_tmp721="float",_tag_dyneither(_tmp721,
sizeof(char),6))));}goto _LLCD;_LLE2: {struct Cyc_Absyn_DoubleType_struct*_tmp16A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp152;if(_tmp16A->tag != 8)goto _LLE4;else{
_tmp16B=_tmp16A->f1;}}_LLE3: if(_tmp16B){const char*_tmp722;s=Cyc_PP_text(((
_tmp722="long double",_tag_dyneither(_tmp722,sizeof(char),12))));}else{const char*
_tmp723;s=Cyc_PP_text(((_tmp723="double",_tag_dyneither(_tmp723,sizeof(char),7))));}
goto _LLCD;_LLE4: {struct Cyc_Absyn_TupleType_struct*_tmp16C=(struct Cyc_Absyn_TupleType_struct*)
_tmp152;if(_tmp16C->tag != 11)goto _LLE6;else{_tmp16D=_tmp16C->f1;}}_LLE5:{struct
Cyc_PP_Doc*_tmp724[2];s=((_tmp724[1]=Cyc_Absynpp_args2doc(_tmp16D),((_tmp724[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp724,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCD;_LLE6: {struct Cyc_Absyn_AggrType_struct*_tmp16E=(struct Cyc_Absyn_AggrType_struct*)
_tmp152;if(_tmp16E->tag != 12)goto _LLE8;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.aggr_info;
_tmp171=_tmp16F.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*
_tmp1D0;struct _tuple9 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;
_tmp1D0=_tmp1CE.f2;{struct Cyc_PP_Doc*_tmp725[3];s=((_tmp725[2]=Cyc_Absynpp_tps2doc(
_tmp171),((_tmp725[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp725[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp152;if(_tmp172->tag != 13)goto _LLEA;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp726[4];s=((_tmp726[3]=Cyc_Absynpp_rb(),((_tmp726[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp726[1]=Cyc_Absynpp_lb(),((
_tmp726[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp726,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp175=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp152;if(_tmp175->tag != 15)goto
_LLEC;else{_tmp176=_tmp175->f1;}}_LLEB:{const char*_tmp729;struct Cyc_PP_Doc*
_tmp728[4];s=((_tmp728[3]=Cyc_Absynpp_rb(),((_tmp728[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp176)),((_tmp728[1]=Cyc_Absynpp_lb(),((_tmp728[0]=Cyc_PP_text(((_tmp729="enum ",
_tag_dyneither(_tmp729,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp728,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp177=(struct Cyc_Absyn_EnumType_struct*)_tmp152;if(_tmp177->tag != 14)goto _LLEE;
else{_tmp178=_tmp177->f1;}}_LLED:{const char*_tmp72C;struct Cyc_PP_Doc*_tmp72B[2];
s=((_tmp72B[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp72B[0]=Cyc_PP_text(((_tmp72C="enum ",
_tag_dyneither(_tmp72C,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp72B,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp179=(struct Cyc_Absyn_ValueofType_struct*)_tmp152;if(_tmp179->tag != 19)goto
_LLF0;else{_tmp17A=_tmp179->f1;}}_LLEF:{const char*_tmp731;const char*_tmp730;
struct Cyc_PP_Doc*_tmp72F[3];s=((_tmp72F[2]=Cyc_PP_text(((_tmp730=")",
_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72F[1]=Cyc_Absynpp_exp2doc(
_tmp17A),((_tmp72F[0]=Cyc_PP_text(((_tmp731="valueof_t(",_tag_dyneither(_tmp731,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp72F,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp17B=(struct
Cyc_Absyn_TypedefType_struct*)_tmp152;if(_tmp17B->tag != 18)goto _LLF2;else{
_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp732[2];s=((_tmp732[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp732[0]=Cyc_Absynpp_qvar2doc(
_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp732,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCD;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp152;if(_tmp17F->tag != 16)goto _LLF4;else{_tmp180=(void*)_tmp17F->f1;}}_LLF3:{
const char*_tmp737;const char*_tmp736;struct Cyc_PP_Doc*_tmp735[3];s=((_tmp735[2]=
Cyc_PP_text(((_tmp736=">",_tag_dyneither(_tmp736,sizeof(char),2)))),((_tmp735[1]=
Cyc_Absynpp_rgn2doc(_tmp180),((_tmp735[0]=Cyc_PP_text(((_tmp737="region_t<",
_tag_dyneither(_tmp737,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp735,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp181=(struct Cyc_Absyn_DynRgnType_struct*)_tmp152;if(_tmp181->tag != 17)goto
_LLF6;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF5:{const
char*_tmp73E;const char*_tmp73D;const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[5];s=((
_tmp73B[4]=Cyc_PP_text(((_tmp73C=">",_tag_dyneither(_tmp73C,sizeof(char),2)))),((
_tmp73B[3]=Cyc_Absynpp_rgn2doc(_tmp183),((_tmp73B[2]=Cyc_PP_text(((_tmp73D=",",
_tag_dyneither(_tmp73D,sizeof(char),2)))),((_tmp73B[1]=Cyc_Absynpp_rgn2doc(
_tmp182),((_tmp73B[0]=Cyc_PP_text(((_tmp73E="dynregion_t<",_tag_dyneither(
_tmp73E,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp184=(struct
Cyc_Absyn_TagType_struct*)_tmp152;if(_tmp184->tag != 20)goto _LLF8;else{_tmp185=(
void*)_tmp184->f1;}}_LLF7:{const char*_tmp743;const char*_tmp742;struct Cyc_PP_Doc*
_tmp741[3];s=((_tmp741[2]=Cyc_PP_text(((_tmp742=">",_tag_dyneither(_tmp742,
sizeof(char),2)))),((_tmp741[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp741[0]=Cyc_PP_text(((
_tmp743="tag_t<",_tag_dyneither(_tmp743,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp741,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp152;if(_tmp186->tag != 22)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp187=(struct Cyc_Absyn_HeapRgn_struct*)_tmp152;if(_tmp187->tag != 21)goto _LLFC;}
_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp188=(struct Cyc_Absyn_RgnsEff_struct*)_tmp152;if(_tmp188->tag != 25)goto _LLFE;
else{_tmp189=(void*)_tmp188->f1;}}_LLFD:{const char*_tmp748;const char*_tmp747;
struct Cyc_PP_Doc*_tmp746[3];s=((_tmp746[2]=Cyc_PP_text(((_tmp747=")",
_tag_dyneither(_tmp747,sizeof(char),2)))),((_tmp746[1]=Cyc_Absynpp_typ2doc(
_tmp189),((_tmp746[0]=Cyc_PP_text(((_tmp748="regions(",_tag_dyneither(_tmp748,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp746,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: {struct Cyc_Absyn_AccessEff_struct*_tmp18A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp152;if(_tmp18A->tag != 23)goto _LL100;}_LLFF: goto _LL101;_LL100: {struct Cyc_Absyn_JoinEff_struct*
_tmp18B=(struct Cyc_Absyn_JoinEff_struct*)_tmp152;if(_tmp18B->tag != 24)goto _LLCD;}
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple16*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16 _tmp1E9;void*_tmp1EA;void*_tmp1EB;struct _tuple16*_tmp1E8=cmp;
_tmp1E9=*_tmp1E8;_tmp1EA=_tmp1E9.f1;_tmp1EB=_tmp1E9.f2;{const char*_tmp74B;struct
Cyc_PP_Doc*_tmp74A[3];return(_tmp74A[2]=Cyc_Absynpp_rgn2doc(_tmp1EB),((_tmp74A[1]=
Cyc_PP_text(((_tmp74B=" > ",_tag_dyneither(_tmp74B,sizeof(char),4)))),((_tmp74A[
0]=Cyc_Absynpp_rgn2doc(_tmp1EA),Cyc_PP_cat(_tag_dyneither(_tmp74A,sizeof(struct
Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp74E;const char*_tmp74D;const char*_tmp74C;return Cyc_PP_group(((_tmp74C="",
_tag_dyneither(_tmp74C,sizeof(char),1))),((_tmp74D="",_tag_dyneither(_tmp74D,
sizeof(char),1))),((_tmp74E=",",_tag_dyneither(_tmp74E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp74F;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp74F=_cycalloc(
sizeof(*_tmp74F)),((_tmp74F->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp74F))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp752;struct Cyc_List_List*
_tmp751;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp751=_cycalloc(sizeof(*_tmp751)),((_tmp751->hd=
Cyc_PP_text(((_tmp752="...",_tag_dyneither(_tmp752,sizeof(char),4)))),((_tmp751->tl=
0,_tmp751)))))));}else{if(cyc_varargs != 0){const char*_tmp75B;const char*_tmp75A;
const char*_tmp759;struct _tuple7*_tmp758;struct Cyc_PP_Doc*_tmp757[3];struct Cyc_PP_Doc*
_tmp1F5=(_tmp757[2]=Cyc_Absynpp_funarg2doc(((_tmp758=_cycalloc(sizeof(*_tmp758)),((
_tmp758->f1=cyc_varargs->name,((_tmp758->f2=cyc_varargs->tq,((_tmp758->f3=
cyc_varargs->type,_tmp758))))))))),((_tmp757[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp759=" inject ",_tag_dyneither(_tmp759,sizeof(char),9)))): Cyc_PP_text(((
_tmp75A=" ",_tag_dyneither(_tmp75A,sizeof(char),2)))),((_tmp757[0]=Cyc_PP_text(((
_tmp75B="...",_tag_dyneither(_tmp75B,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp757,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp75C;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp75C=_cycalloc(sizeof(*_tmp75C)),((_tmp75C->hd=
_tmp1F5,((_tmp75C->tl=0,_tmp75C)))))));}}{const char*_tmp75F;const char*_tmp75E;
const char*_tmp75D;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp75D="",
_tag_dyneither(_tmp75D,sizeof(char),1))),((_tmp75E="",_tag_dyneither(_tmp75E,
sizeof(char),1))),((_tmp75F=",",_tag_dyneither(_tmp75F,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp762;struct Cyc_PP_Doc*
_tmp761[3];_tmp1FC=((_tmp761[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp761[
1]=Cyc_PP_text(((_tmp762=";",_tag_dyneither(_tmp762,sizeof(char),2)))),((_tmp761[
0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp761,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp765;struct Cyc_PP_Doc*_tmp764[3];_tmp1FC=((_tmp764[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp764[1]=Cyc_PP_text(((_tmp765=":",
_tag_dyneither(_tmp765,sizeof(char),2)))),((_tmp764[0]=_tmp1FC,Cyc_PP_cat(
_tag_dyneither(_tmp764,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp76A;
const char*_tmp769;struct Cyc_PP_Doc*_tmp768[3];return(_tmp768[2]=Cyc_PP_text(((
_tmp769=")",_tag_dyneither(_tmp769,sizeof(char),2)))),((_tmp768[1]=_tmp1FC,((
_tmp768[0]=Cyc_PP_text(((_tmp76A="(",_tag_dyneither(_tmp76A,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp768,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){struct Cyc_Core_Opt*_tmp76D;struct _tuple7*_tmp76C;return(
_tmp76C=_cycalloc(sizeof(*_tmp76C)),((_tmp76C->f1=((_tmp76D=_cycalloc(sizeof(*
_tmp76D)),((_tmp76D->v=(*arg).f1,_tmp76D)))),((_tmp76C->f2=(*arg).f2,((_tmp76C->f3=(*
arg).f3,_tmp76C)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp209=0;int match;{union Cyc_Absyn_Nmspace _tmp20A=(*q).f1;int _tmp20B;struct Cyc_List_List*
_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20E;_LL11E: if((
_tmp20A.Loc_n).tag != 4)goto _LL120;_tmp20B=(int)(_tmp20A.Loc_n).val;_LL11F:
_tmp20C=0;goto _LL121;_LL120: if((_tmp20A.Rel_n).tag != 1)goto _LL122;_tmp20C=(
struct Cyc_List_List*)(_tmp20A.Rel_n).val;_LL121: match=0;_tmp209=_tmp20C;goto
_LL11D;_LL122: if((_tmp20A.C_n).tag != 3)goto _LL124;_tmp20D=(struct Cyc_List_List*)(
_tmp20A.C_n).val;_LL123: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20D,Cyc_Absynpp_curr_namespace);
goto _LL11D;_LL124: if((_tmp20A.Abs_n).tag != 2)goto _LL11D;_tmp20E=(struct Cyc_List_List*)(
_tmp20A.Abs_n).val;_LL125: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20E,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp76E;_tmp209=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp76E=_cycalloc(sizeof(*_tmp76E)),((_tmp76E->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp76E->tl=_tmp20E,_tmp76E))))): _tmp20E;}goto _LL11D;_LL11D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp770;struct Cyc_List_List*_tmp76F;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp209,((_tmp76F=_cycalloc(sizeof(*_tmp76F)),((_tmp76F->hd=(*q).f2,((_tmp76F->tl=
0,_tmp76F))))))),((_tmp770="_",_tag_dyneither(_tmp770,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp772;struct Cyc_List_List*_tmp771;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp209,((_tmp771=_cycalloc(sizeof(*
_tmp771)),((_tmp771->hd=(*q).f2,((_tmp771->tl=0,_tmp771))))))),((_tmp772="::",
_tag_dyneither(_tmp772,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp214=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp774;const char*_tmp773;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp773="\\textbf{",
_tag_dyneither(_tmp773,sizeof(char),9))),(struct _dyneither_ptr)_tmp214),((
_tmp774="}",_tag_dyneither(_tmp774,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp214));}else{return Cyc_PP_text(_tmp214);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp217=(*v).f1;int
_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*
_tmp21B;_LL127: if((_tmp217.Loc_n).tag != 4)goto _LL129;_tmp218=(int)(_tmp217.Loc_n).val;
_LL128: goto _LL12A;_LL129: if((_tmp217.Rel_n).tag != 1)goto _LL12B;_tmp219=(struct
Cyc_List_List*)(_tmp217.Rel_n).val;if(_tmp219 != 0)goto _LL12B;_LL12A: return*(*v).f2;
_LL12B: if((_tmp217.C_n).tag != 3)goto _LL12D;_tmp21A=(struct Cyc_List_List*)(
_tmp217.C_n).val;_LL12C: _tmp21B=_tmp21A;goto _LL12E;_LL12D: if((_tmp217.Abs_n).tag
!= 2)goto _LL12F;_tmp21B=(struct Cyc_List_List*)(_tmp217.Abs_n).val;_LL12E: if(((
int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp21B,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL130;}_LL12F:;_LL130: {const char*_tmp775;return(
struct _dyneither_ptr)Cyc_strconcat(((_tmp775="/* bad namespace : */ ",
_tag_dyneither(_tmp775,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL126:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp21D=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp777;const char*_tmp776;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp776="\\textbf{",
_tag_dyneither(_tmp776,sizeof(char),9))),(struct _dyneither_ptr)_tmp21D),((
_tmp777="}",_tag_dyneither(_tmp777,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp21D));}else{return Cyc_PP_text(_tmp21D);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp220=e->r;enum Cyc_Absyn_Primop
_tmp225;struct Cyc_Absyn_Exp*_tmp230;struct Cyc_Absyn_Exp*_tmp232;_LL132: {struct
Cyc_Absyn_Const_e_struct*_tmp221=(struct Cyc_Absyn_Const_e_struct*)_tmp220;if(
_tmp221->tag != 0)goto _LL134;}_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*
_tmp222=(struct Cyc_Absyn_Var_e_struct*)_tmp220;if(_tmp222->tag != 1)goto _LL136;}
_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_UnknownId_e_struct*_tmp223=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp220;if(_tmp223->tag != 2)goto _LL138;}_LL137: return 10000;_LL138: {struct Cyc_Absyn_Primop_e_struct*
_tmp224=(struct Cyc_Absyn_Primop_e_struct*)_tmp220;if(_tmp224->tag != 3)goto _LL13A;
else{_tmp225=_tmp224->f1;}}_LL139: switch(_tmp225){case Cyc_Absyn_Plus: _LL182:
return 100;case Cyc_Absyn_Times: _LL183: return 110;case Cyc_Absyn_Minus: _LL184: return
100;case Cyc_Absyn_Div: _LL185: goto _LL186;case Cyc_Absyn_Mod: _LL186: return 110;case
Cyc_Absyn_Eq: _LL187: goto _LL188;case Cyc_Absyn_Neq: _LL188: return 70;case Cyc_Absyn_Gt:
_LL189: goto _LL18A;case Cyc_Absyn_Lt: _LL18A: goto _LL18B;case Cyc_Absyn_Gte: _LL18B:
goto _LL18C;case Cyc_Absyn_Lte: _LL18C: return 80;case Cyc_Absyn_Not: _LL18D: goto _LL18E;
case Cyc_Absyn_Bitnot: _LL18E: return 130;case Cyc_Absyn_Bitand: _LL18F: return 60;case
Cyc_Absyn_Bitor: _LL190: return 40;case Cyc_Absyn_Bitxor: _LL191: return 50;case Cyc_Absyn_Bitlshift:
_LL192: return 90;case Cyc_Absyn_Bitlrshift: _LL193: return 80;case Cyc_Absyn_Bitarshift:
_LL194: return 80;case Cyc_Absyn_Numelts: _LL195: return 140;}_LL13A: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp226=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp220;if(_tmp226->tag != 4)goto
_LL13C;}_LL13B: return 20;_LL13C: {struct Cyc_Absyn_Increment_e_struct*_tmp227=(
struct Cyc_Absyn_Increment_e_struct*)_tmp220;if(_tmp227->tag != 5)goto _LL13E;}
_LL13D: return 130;_LL13E: {struct Cyc_Absyn_Conditional_e_struct*_tmp228=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp220;if(_tmp228->tag != 6)goto _LL140;}_LL13F:
return 30;_LL140: {struct Cyc_Absyn_And_e_struct*_tmp229=(struct Cyc_Absyn_And_e_struct*)
_tmp220;if(_tmp229->tag != 7)goto _LL142;}_LL141: return 35;_LL142: {struct Cyc_Absyn_Or_e_struct*
_tmp22A=(struct Cyc_Absyn_Or_e_struct*)_tmp220;if(_tmp22A->tag != 8)goto _LL144;}
_LL143: return 30;_LL144: {struct Cyc_Absyn_SeqExp_e_struct*_tmp22B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp220;if(_tmp22B->tag != 9)goto _LL146;}_LL145: return 10;_LL146: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp22C=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp220;if(_tmp22C->tag != 10)goto
_LL148;}_LL147: goto _LL149;_LL148: {struct Cyc_Absyn_FnCall_e_struct*_tmp22D=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp220;if(_tmp22D->tag != 11)goto _LL14A;}_LL149:
return 140;_LL14A: {struct Cyc_Absyn_Throw_e_struct*_tmp22E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp220;if(_tmp22E->tag != 12)goto _LL14C;}_LL14B: return 130;_LL14C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp22F=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp220;if(_tmp22F->tag != 13)
goto _LL14E;else{_tmp230=_tmp22F->f1;}}_LL14D: return Cyc_Absynpp_exp_prec(_tmp230);
_LL14E: {struct Cyc_Absyn_Instantiate_e_struct*_tmp231=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp220;if(_tmp231->tag != 14)goto _LL150;else{_tmp232=_tmp231->f1;}}_LL14F: return
Cyc_Absynpp_exp_prec(_tmp232);_LL150: {struct Cyc_Absyn_Cast_e_struct*_tmp233=(
struct Cyc_Absyn_Cast_e_struct*)_tmp220;if(_tmp233->tag != 15)goto _LL152;}_LL151:
return 120;_LL152: {struct Cyc_Absyn_New_e_struct*_tmp234=(struct Cyc_Absyn_New_e_struct*)
_tmp220;if(_tmp234->tag != 17)goto _LL154;}_LL153: return 15;_LL154: {struct Cyc_Absyn_Address_e_struct*
_tmp235=(struct Cyc_Absyn_Address_e_struct*)_tmp220;if(_tmp235->tag != 16)goto
_LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp236=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp220;if(_tmp236->tag != 18)goto _LL158;}
_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp237=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp220;if(_tmp237->tag != 19)goto _LL15A;}_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Valueof_e_struct*
_tmp238=(struct Cyc_Absyn_Valueof_e_struct*)_tmp220;if(_tmp238->tag != 39)goto
_LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp239=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp220;if(_tmp239->tag != 38)goto _LL15E;}
_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_Offsetof_e_struct*_tmp23A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp220;if(_tmp23A->tag != 20)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Deref_e_struct*
_tmp23B=(struct Cyc_Absyn_Deref_e_struct*)_tmp220;if(_tmp23B->tag != 21)goto _LL162;}
_LL161: return 130;_LL162: {struct Cyc_Absyn_AggrMember_e_struct*_tmp23C=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp220;if(_tmp23C->tag != 22)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp23D=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp220;if(_tmp23D->tag != 23)goto
_LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Subscript_e_struct*_tmp23E=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp220;if(_tmp23E->tag != 24)goto _LL168;}
_LL167: return 140;_LL168: {struct Cyc_Absyn_Tuple_e_struct*_tmp23F=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp220;if(_tmp23F->tag != 25)goto _LL16A;}_LL169: return 150;_LL16A: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp240=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp220;if(_tmp240->tag != 26)goto
_LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Array_e_struct*_tmp241=(
struct Cyc_Absyn_Array_e_struct*)_tmp220;if(_tmp241->tag != 27)goto _LL16E;}_LL16D:
goto _LL16F;_LL16E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp242=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp220;if(_tmp242->tag != 28)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp243=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp220;if(_tmp243->tag != 29)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp244=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp220;if(_tmp244->tag != 30)goto _LL174;}
_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_Datatype_e_struct*_tmp245=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp220;if(_tmp245->tag != 31)goto _LL176;}_LL175: goto _LL177;_LL176: {struct Cyc_Absyn_Enum_e_struct*
_tmp246=(struct Cyc_Absyn_Enum_e_struct*)_tmp220;if(_tmp246->tag != 32)goto _LL178;}
_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp247=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp220;if(_tmp247->tag != 33)goto _LL17A;}_LL179: goto _LL17B;_LL17A: {struct Cyc_Absyn_Malloc_e_struct*
_tmp248=(struct Cyc_Absyn_Malloc_e_struct*)_tmp220;if(_tmp248->tag != 34)goto
_LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_Swap_e_struct*_tmp249=(struct
Cyc_Absyn_Swap_e_struct*)_tmp220;if(_tmp249->tag != 35)goto _LL17E;}_LL17D: goto
_LL17F;_LL17E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp24A=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp220;if(_tmp24A->tag != 36)goto _LL180;}_LL17F: return 140;_LL180: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp24B=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp220;if(_tmp24B->tag != 37)goto
_LL131;}_LL181: return 10000;_LL131:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp24C=e->r;union Cyc_Absyn_Cnst _tmp24E;struct _tuple0*_tmp250;struct
_tuple0*_tmp252;enum Cyc_Absyn_Primop _tmp254;struct Cyc_List_List*_tmp255;struct
Cyc_Absyn_Exp*_tmp257;struct Cyc_Core_Opt*_tmp258;struct Cyc_Absyn_Exp*_tmp259;
struct Cyc_Absyn_Exp*_tmp25B;enum Cyc_Absyn_Incrementor _tmp25C;struct Cyc_Absyn_Exp*
_tmp25E;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*
_tmp262;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*
_tmp266;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*
_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_List_List*
_tmp26F;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*
_tmp275;void*_tmp277;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp27A;
struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27D;void*_tmp27F;struct Cyc_Absyn_Exp*
_tmp281;void*_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct _dyneither_ptr*_tmp286;
void*_tmp288;void*_tmp289;struct _dyneither_ptr*_tmp28B;void*_tmp28D;void*_tmp28E;
unsigned int _tmp290;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp294;
struct _dyneither_ptr*_tmp295;struct Cyc_Absyn_Exp*_tmp297;struct _dyneither_ptr*
_tmp298;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_List_List*
_tmp29D;struct _tuple7*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*
_tmp2A2;struct Cyc_Absyn_Vardecl*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*
_tmp2A6;struct _tuple0*_tmp2A8;struct Cyc_List_List*_tmp2A9;struct Cyc_List_List*
_tmp2AA;struct Cyc_List_List*_tmp2AC;struct Cyc_List_List*_tmp2AE;struct Cyc_Absyn_Datatypefield*
_tmp2AF;struct _tuple0*_tmp2B1;struct _tuple0*_tmp2B3;struct Cyc_Absyn_MallocInfo
_tmp2B5;int _tmp2B6;struct Cyc_Absyn_Exp*_tmp2B7;void**_tmp2B8;struct Cyc_Absyn_Exp*
_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Core_Opt*
_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Absyn_Stmt*_tmp2C1;_LL198: {struct
Cyc_Absyn_Const_e_struct*_tmp24D=(struct Cyc_Absyn_Const_e_struct*)_tmp24C;if(
_tmp24D->tag != 0)goto _LL19A;else{_tmp24E=_tmp24D->f1;}}_LL199: s=Cyc_Absynpp_cnst2doc(
_tmp24E);goto _LL197;_LL19A: {struct Cyc_Absyn_Var_e_struct*_tmp24F=(struct Cyc_Absyn_Var_e_struct*)
_tmp24C;if(_tmp24F->tag != 1)goto _LL19C;else{_tmp250=_tmp24F->f1;}}_LL19B: _tmp252=
_tmp250;goto _LL19D;_LL19C: {struct Cyc_Absyn_UnknownId_e_struct*_tmp251=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp24C;if(_tmp251->tag != 2)goto _LL19E;else{
_tmp252=_tmp251->f1;}}_LL19D: s=Cyc_Absynpp_qvar2doc(_tmp252);goto _LL197;_LL19E: {
struct Cyc_Absyn_Primop_e_struct*_tmp253=(struct Cyc_Absyn_Primop_e_struct*)
_tmp24C;if(_tmp253->tag != 3)goto _LL1A0;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}
_LL19F: s=Cyc_Absynpp_primapp2doc(myprec,_tmp254,_tmp255);goto _LL197;_LL1A0: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp256=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp24C;if(_tmp256->tag != 4)goto _LL1A2;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;
_tmp259=_tmp256->f3;}}_LL1A1:{const char*_tmp77C;const char*_tmp77B;struct Cyc_PP_Doc*
_tmp77A[5];s=((_tmp77A[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp259),((_tmp77A[3]=
Cyc_PP_text(((_tmp77B="= ",_tag_dyneither(_tmp77B,sizeof(char),3)))),((_tmp77A[2]=
_tmp258 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp258->v),((
_tmp77A[1]=Cyc_PP_text(((_tmp77C=" ",_tag_dyneither(_tmp77C,sizeof(char),2)))),((
_tmp77A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp257),Cyc_PP_cat(_tag_dyneither(
_tmp77A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL197;_LL1A2: {struct Cyc_Absyn_Increment_e_struct*
_tmp25A=(struct Cyc_Absyn_Increment_e_struct*)_tmp24C;if(_tmp25A->tag != 5)goto
_LL1A4;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;}}_LL1A3: {struct Cyc_PP_Doc*
_tmp2C5=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B);switch(_tmp25C){case Cyc_Absyn_PreInc:
_LL1EA:{const char*_tmp77F;struct Cyc_PP_Doc*_tmp77E[2];s=((_tmp77E[1]=_tmp2C5,((
_tmp77E[0]=Cyc_PP_text(((_tmp77F="++",_tag_dyneither(_tmp77F,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp77E,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1EB:{const char*_tmp782;struct Cyc_PP_Doc*_tmp781[2];s=((
_tmp781[1]=_tmp2C5,((_tmp781[0]=Cyc_PP_text(((_tmp782="--",_tag_dyneither(
_tmp782,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1EC:{const char*_tmp785;struct Cyc_PP_Doc*
_tmp784[2];s=((_tmp784[1]=Cyc_PP_text(((_tmp785="++",_tag_dyneither(_tmp785,
sizeof(char),3)))),((_tmp784[0]=_tmp2C5,Cyc_PP_cat(_tag_dyneither(_tmp784,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1ED:{const char*
_tmp788;struct Cyc_PP_Doc*_tmp787[2];s=((_tmp787[1]=Cyc_PP_text(((_tmp788="--",
_tag_dyneither(_tmp788,sizeof(char),3)))),((_tmp787[0]=_tmp2C5,Cyc_PP_cat(
_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL197;}
_LL1A4: {struct Cyc_Absyn_Conditional_e_struct*_tmp25D=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp24C;if(_tmp25D->tag != 6)goto _LL1A6;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;
_tmp260=_tmp25D->f3;}}_LL1A5:{const char*_tmp78D;const char*_tmp78C;struct Cyc_PP_Doc*
_tmp78B[5];s=((_tmp78B[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),((_tmp78B[3]=
Cyc_PP_text(((_tmp78C=" : ",_tag_dyneither(_tmp78C,sizeof(char),4)))),((_tmp78B[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp25F),((_tmp78B[1]=Cyc_PP_text(((_tmp78D=" ? ",
_tag_dyneither(_tmp78D,sizeof(char),4)))),((_tmp78B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25E),Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL197;_LL1A6: {struct Cyc_Absyn_And_e_struct*_tmp261=(struct Cyc_Absyn_And_e_struct*)
_tmp24C;if(_tmp261->tag != 7)goto _LL1A8;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;}}
_LL1A7:{const char*_tmp790;struct Cyc_PP_Doc*_tmp78F[3];s=((_tmp78F[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp263),((_tmp78F[1]=Cyc_PP_text(((_tmp790=" && ",_tag_dyneither(_tmp790,
sizeof(char),5)))),((_tmp78F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp262),Cyc_PP_cat(
_tag_dyneither(_tmp78F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1A8: {
struct Cyc_Absyn_Or_e_struct*_tmp264=(struct Cyc_Absyn_Or_e_struct*)_tmp24C;if(
_tmp264->tag != 8)goto _LL1AA;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;}}
_LL1A9:{const char*_tmp793;struct Cyc_PP_Doc*_tmp792[3];s=((_tmp792[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp266),((_tmp792[1]=Cyc_PP_text(((_tmp793=" || ",_tag_dyneither(_tmp793,
sizeof(char),5)))),((_tmp792[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp265),Cyc_PP_cat(
_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1AA: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp267=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp24C;if(_tmp267->tag != 9)goto _LL1AC;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}
_LL1AB:{const char*_tmp79A;const char*_tmp799;const char*_tmp798;struct Cyc_PP_Doc*
_tmp797[5];s=((_tmp797[4]=Cyc_PP_text(((_tmp798=")",_tag_dyneither(_tmp798,
sizeof(char),2)))),((_tmp797[3]=Cyc_Absynpp_exp2doc(_tmp269),((_tmp797[2]=Cyc_PP_text(((
_tmp799=", ",_tag_dyneither(_tmp799,sizeof(char),3)))),((_tmp797[1]=Cyc_Absynpp_exp2doc(
_tmp268),((_tmp797[0]=Cyc_PP_text(((_tmp79A="(",_tag_dyneither(_tmp79A,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp797,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL197;_LL1AC: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp26A=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp24C;if(_tmp26A->tag != 10)goto _LL1AE;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26A->f2;}}
_LL1AD:{const char*_tmp79F;const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[4];s=((
_tmp79D[3]=Cyc_PP_text(((_tmp79E=")",_tag_dyneither(_tmp79E,sizeof(char),2)))),((
_tmp79D[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26C),((_tmp79D[1]=Cyc_PP_text(((
_tmp79F="(",_tag_dyneither(_tmp79F,sizeof(char),2)))),((_tmp79D[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26B),Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL197;_LL1AE: {struct Cyc_Absyn_FnCall_e_struct*_tmp26D=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp24C;if(_tmp26D->tag != 11)goto _LL1B0;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}
_LL1AF:{const char*_tmp7A4;const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[4];s=((
_tmp7A2[3]=Cyc_PP_text(((_tmp7A3=")",_tag_dyneither(_tmp7A3,sizeof(char),2)))),((
_tmp7A2[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26F),((_tmp7A2[1]=Cyc_PP_text(((
_tmp7A4="(",_tag_dyneither(_tmp7A4,sizeof(char),2)))),((_tmp7A2[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26E),Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL197;_LL1B0: {struct Cyc_Absyn_Throw_e_struct*_tmp270=(struct Cyc_Absyn_Throw_e_struct*)
_tmp24C;if(_tmp270->tag != 12)goto _LL1B2;else{_tmp271=_tmp270->f1;}}_LL1B1:{const
char*_tmp7A7;struct Cyc_PP_Doc*_tmp7A6[2];s=((_tmp7A6[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp271),((_tmp7A6[0]=Cyc_PP_text(((_tmp7A7="throw ",_tag_dyneither(
_tmp7A7,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7A6,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL197;_LL1B2: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp272=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp24C;if(_tmp272->tag != 13)goto _LL1B4;
else{_tmp273=_tmp272->f1;}}_LL1B3: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp273);
goto _LL197;_LL1B4: {struct Cyc_Absyn_Instantiate_e_struct*_tmp274=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp24C;if(_tmp274->tag != 14)goto _LL1B6;else{_tmp275=_tmp274->f1;}}_LL1B5: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp275);goto _LL197;_LL1B6: {struct Cyc_Absyn_Cast_e_struct*_tmp276=(
struct Cyc_Absyn_Cast_e_struct*)_tmp24C;if(_tmp276->tag != 15)goto _LL1B8;else{
_tmp277=(void*)_tmp276->f1;_tmp278=_tmp276->f2;}}_LL1B7:{const char*_tmp7AC;const
char*_tmp7AB;struct Cyc_PP_Doc*_tmp7AA[4];s=((_tmp7AA[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp278),((_tmp7AA[2]=Cyc_PP_text(((_tmp7AB=")",_tag_dyneither(_tmp7AB,
sizeof(char),2)))),((_tmp7AA[1]=Cyc_Absynpp_typ2doc(_tmp277),((_tmp7AA[0]=Cyc_PP_text(((
_tmp7AC="(",_tag_dyneither(_tmp7AC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7AA,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL197;_LL1B8: {struct Cyc_Absyn_Address_e_struct*
_tmp279=(struct Cyc_Absyn_Address_e_struct*)_tmp24C;if(_tmp279->tag != 16)goto
_LL1BA;else{_tmp27A=_tmp279->f1;}}_LL1B9:{const char*_tmp7AF;struct Cyc_PP_Doc*
_tmp7AE[2];s=((_tmp7AE[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27A),((_tmp7AE[0]=
Cyc_PP_text(((_tmp7AF="&",_tag_dyneither(_tmp7AF,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7AE,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL197;_LL1BA: {
struct Cyc_Absyn_New_e_struct*_tmp27B=(struct Cyc_Absyn_New_e_struct*)_tmp24C;if(
_tmp27B->tag != 17)goto _LL1BC;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;}}
_LL1BB: if(_tmp27C == 0){const char*_tmp7B2;struct Cyc_PP_Doc*_tmp7B1[2];s=((_tmp7B1[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27D),((_tmp7B1[0]=Cyc_PP_text(((_tmp7B2="new ",
_tag_dyneither(_tmp7B2,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7B1,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7B7;const char*_tmp7B6;
struct Cyc_PP_Doc*_tmp7B5[4];s=((_tmp7B5[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp27D),((_tmp7B5[2]=Cyc_PP_text(((_tmp7B6=") ",_tag_dyneither(_tmp7B6,sizeof(
char),3)))),((_tmp7B5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp27C),((
_tmp7B5[0]=Cyc_PP_text(((_tmp7B7="rnew(",_tag_dyneither(_tmp7B7,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7B5,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL197;_LL1BC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp27E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp24C;if(_tmp27E->tag != 18)goto _LL1BE;else{_tmp27F=(void*)_tmp27E->f1;}}_LL1BD:{
const char*_tmp7BC;const char*_tmp7BB;struct Cyc_PP_Doc*_tmp7BA[3];s=((_tmp7BA[2]=
Cyc_PP_text(((_tmp7BB=")",_tag_dyneither(_tmp7BB,sizeof(char),2)))),((_tmp7BA[1]=
Cyc_Absynpp_typ2doc(_tmp27F),((_tmp7BA[0]=Cyc_PP_text(((_tmp7BC="sizeof(",
_tag_dyneither(_tmp7BC,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7BA,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1BE: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp280=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp24C;if(_tmp280->tag != 19)goto
_LL1C0;else{_tmp281=_tmp280->f1;}}_LL1BF:{const char*_tmp7C1;const char*_tmp7C0;
struct Cyc_PP_Doc*_tmp7BF[3];s=((_tmp7BF[2]=Cyc_PP_text(((_tmp7C0=")",
_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BF[1]=Cyc_Absynpp_exp2doc(
_tmp281),((_tmp7BF[0]=Cyc_PP_text(((_tmp7C1="sizeof(",_tag_dyneither(_tmp7C1,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7BF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL197;_LL1C0: {struct Cyc_Absyn_Valueof_e_struct*_tmp282=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp24C;if(_tmp282->tag != 39)goto _LL1C2;else{_tmp283=(void*)_tmp282->f1;}}_LL1C1:{
const char*_tmp7C6;const char*_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[3];s=((_tmp7C4[2]=
Cyc_PP_text(((_tmp7C5=")",_tag_dyneither(_tmp7C5,sizeof(char),2)))),((_tmp7C4[1]=
Cyc_Absynpp_typ2doc(_tmp283),((_tmp7C4[0]=Cyc_PP_text(((_tmp7C6="valueof(",
_tag_dyneither(_tmp7C6,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7C4,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1C2: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp284=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp24C;if(_tmp284->tag != 38)goto
_LL1C4;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}_LL1C3:{const char*_tmp7CD;
const char*_tmp7CC;const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[5];s=((_tmp7CA[4]=
Cyc_PP_text(((_tmp7CB=")",_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7CA[3]=
Cyc_PP_textptr(_tmp286),((_tmp7CA[2]=Cyc_PP_text(((_tmp7CC=".",_tag_dyneither(
_tmp7CC,sizeof(char),2)))),((_tmp7CA[1]=Cyc_Absynpp_exp2doc(_tmp285),((_tmp7CA[0]=
Cyc_PP_text(((_tmp7CD="tagcheck(",_tag_dyneither(_tmp7CD,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL197;
_LL1C4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp287=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp24C;if(_tmp287->tag != 20)goto _LL1C6;else{_tmp288=(void*)_tmp287->f1;_tmp289=(
void*)_tmp287->f2;{struct Cyc_Absyn_StructField_struct*_tmp28A=(struct Cyc_Absyn_StructField_struct*)
_tmp289;if(_tmp28A->tag != 0)goto _LL1C6;else{_tmp28B=_tmp28A->f1;}};}}_LL1C5:{
const char*_tmp7D4;const char*_tmp7D3;const char*_tmp7D2;struct Cyc_PP_Doc*_tmp7D1[5];
s=((_tmp7D1[4]=Cyc_PP_text(((_tmp7D2=")",_tag_dyneither(_tmp7D2,sizeof(char),2)))),((
_tmp7D1[3]=Cyc_PP_textptr(_tmp28B),((_tmp7D1[2]=Cyc_PP_text(((_tmp7D3=",",
_tag_dyneither(_tmp7D3,sizeof(char),2)))),((_tmp7D1[1]=Cyc_Absynpp_typ2doc(
_tmp288),((_tmp7D1[0]=Cyc_PP_text(((_tmp7D4="offsetof(",_tag_dyneither(_tmp7D4,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7D1,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL197;_LL1C6: {struct Cyc_Absyn_Offsetof_e_struct*_tmp28C=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp24C;if(_tmp28C->tag != 20)goto _LL1C8;else{
_tmp28D=(void*)_tmp28C->f1;_tmp28E=(void*)_tmp28C->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp28F=(struct Cyc_Absyn_TupleIndex_struct*)_tmp28E;if(_tmp28F->tag != 1)goto
_LL1C8;else{_tmp290=_tmp28F->f1;}};}}_LL1C7:{const char*_tmp7E2;const char*_tmp7E1;
struct Cyc_Int_pa_struct _tmp7E0;void*_tmp7DF[1];const char*_tmp7DE;const char*
_tmp7DD;struct Cyc_PP_Doc*_tmp7DC[5];s=((_tmp7DC[4]=Cyc_PP_text(((_tmp7DD=")",
_tag_dyneither(_tmp7DD,sizeof(char),2)))),((_tmp7DC[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7E0.tag=1,((_tmp7E0.f1=(unsigned long)((int)_tmp290),((
_tmp7DF[0]=& _tmp7E0,Cyc_aprintf(((_tmp7DE="%d",_tag_dyneither(_tmp7DE,sizeof(
char),3))),_tag_dyneither(_tmp7DF,sizeof(void*),1))))))))),((_tmp7DC[2]=Cyc_PP_text(((
_tmp7E1=",",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7DC[1]=Cyc_Absynpp_typ2doc(
_tmp28D),((_tmp7DC[0]=Cyc_PP_text(((_tmp7E2="offsetof(",_tag_dyneither(_tmp7E2,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7DC,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL197;_LL1C8: {struct Cyc_Absyn_Deref_e_struct*_tmp291=(
struct Cyc_Absyn_Deref_e_struct*)_tmp24C;if(_tmp291->tag != 21)goto _LL1CA;else{
_tmp292=_tmp291->f1;}}_LL1C9:{const char*_tmp7E5;struct Cyc_PP_Doc*_tmp7E4[2];s=((
_tmp7E4[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),((_tmp7E4[0]=Cyc_PP_text(((
_tmp7E5="*",_tag_dyneither(_tmp7E5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7E4,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL197;_LL1CA: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp293=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp24C;if(_tmp293->tag != 22)goto
_LL1CC;else{_tmp294=_tmp293->f1;_tmp295=_tmp293->f2;}}_LL1CB:{const char*_tmp7E8;
struct Cyc_PP_Doc*_tmp7E7[3];s=((_tmp7E7[2]=Cyc_PP_textptr(_tmp295),((_tmp7E7[1]=
Cyc_PP_text(((_tmp7E8=".",_tag_dyneither(_tmp7E8,sizeof(char),2)))),((_tmp7E7[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp294),Cyc_PP_cat(_tag_dyneither(_tmp7E7,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1CC: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp296=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp24C;if(_tmp296->tag != 23)goto
_LL1CE;else{_tmp297=_tmp296->f1;_tmp298=_tmp296->f2;}}_LL1CD:{const char*_tmp7EB;
struct Cyc_PP_Doc*_tmp7EA[3];s=((_tmp7EA[2]=Cyc_PP_textptr(_tmp298),((_tmp7EA[1]=
Cyc_PP_text(((_tmp7EB="->",_tag_dyneither(_tmp7EB,sizeof(char),3)))),((_tmp7EA[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp297),Cyc_PP_cat(_tag_dyneither(_tmp7EA,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL197;_LL1CE: {struct Cyc_Absyn_Subscript_e_struct*
_tmp299=(struct Cyc_Absyn_Subscript_e_struct*)_tmp24C;if(_tmp299->tag != 24)goto
_LL1D0;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;}}_LL1CF:{const char*_tmp7F0;
const char*_tmp7EF;struct Cyc_PP_Doc*_tmp7EE[4];s=((_tmp7EE[3]=Cyc_PP_text(((
_tmp7EF="]",_tag_dyneither(_tmp7EF,sizeof(char),2)))),((_tmp7EE[2]=Cyc_Absynpp_exp2doc(
_tmp29B),((_tmp7EE[1]=Cyc_PP_text(((_tmp7F0="[",_tag_dyneither(_tmp7F0,sizeof(
char),2)))),((_tmp7EE[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A),Cyc_PP_cat(
_tag_dyneither(_tmp7EE,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL197;_LL1D0: {
struct Cyc_Absyn_Tuple_e_struct*_tmp29C=(struct Cyc_Absyn_Tuple_e_struct*)_tmp24C;
if(_tmp29C->tag != 25)goto _LL1D2;else{_tmp29D=_tmp29C->f1;}}_LL1D1:{const char*
_tmp7F5;const char*_tmp7F4;struct Cyc_PP_Doc*_tmp7F3[4];s=((_tmp7F3[3]=Cyc_PP_text(((
_tmp7F4=")",_tag_dyneither(_tmp7F4,sizeof(char),2)))),((_tmp7F3[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp29D),((_tmp7F3[1]=Cyc_PP_text(((_tmp7F5="(",_tag_dyneither(_tmp7F5,
sizeof(char),2)))),((_tmp7F3[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7F3,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL197;_LL1D2: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp29E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp24C;if(_tmp29E->tag != 26)goto
_LL1D4;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;}}_LL1D3:{const char*_tmp7FC;
const char*_tmp7FB;const char*_tmp7FA;struct Cyc_PP_Doc*_tmp7F9[4];s=((_tmp7F9[3]=
Cyc_Absynpp_group_braces(((_tmp7FA=",",_tag_dyneither(_tmp7FA,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A0)),((_tmp7F9[2]=Cyc_PP_text(((_tmp7FB=")",
_tag_dyneither(_tmp7FB,sizeof(char),2)))),((_tmp7F9[1]=Cyc_Absynpp_typ2doc((*
_tmp29F).f3),((_tmp7F9[0]=Cyc_PP_text(((_tmp7FC="(",_tag_dyneither(_tmp7FC,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7F9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL197;_LL1D4: {struct Cyc_Absyn_Array_e_struct*_tmp2A1=(struct Cyc_Absyn_Array_e_struct*)
_tmp24C;if(_tmp2A1->tag != 27)goto _LL1D6;else{_tmp2A2=_tmp2A1->f1;}}_LL1D5:{const
char*_tmp7FD;s=Cyc_Absynpp_group_braces(((_tmp7FD=",",_tag_dyneither(_tmp7FD,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A2));}goto _LL197;
_LL1D6: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2A3=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp24C;if(_tmp2A3->tag != 28)goto _LL1D8;else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A3->f2;
_tmp2A6=_tmp2A3->f3;}}_LL1D7:{const char*_tmp804;const char*_tmp803;const char*
_tmp802;struct Cyc_PP_Doc*_tmp801[8];s=((_tmp801[7]=Cyc_Absynpp_rb(),((_tmp801[6]=
Cyc_Absynpp_exp2doc(_tmp2A6),((_tmp801[5]=Cyc_PP_text(((_tmp802=" : ",
_tag_dyneither(_tmp802,sizeof(char),4)))),((_tmp801[4]=Cyc_Absynpp_exp2doc(
_tmp2A5),((_tmp801[3]=Cyc_PP_text(((_tmp803=" < ",_tag_dyneither(_tmp803,sizeof(
char),4)))),((_tmp801[2]=Cyc_PP_text(*(*_tmp2A4->name).f2),((_tmp801[1]=Cyc_PP_text(((
_tmp804="for ",_tag_dyneither(_tmp804,sizeof(char),5)))),((_tmp801[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp801,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL197;_LL1D8: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2A7=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp24C;if(_tmp2A7->tag != 29)goto _LL1DA;else{_tmp2A8=_tmp2A7->f1;_tmp2A9=_tmp2A7->f2;
_tmp2AA=_tmp2A7->f3;}}_LL1D9: {struct Cyc_List_List*_tmp318=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2AA);{const char*_tmp809;struct Cyc_List_List*_tmp808;struct Cyc_PP_Doc*_tmp807[
2];s=((_tmp807[1]=Cyc_Absynpp_group_braces(((_tmp809=",",_tag_dyneither(_tmp809,
sizeof(char),2))),_tmp2A9 != 0?(_tmp808=_cycalloc(sizeof(*_tmp808)),((_tmp808->hd=
Cyc_Absynpp_tps2doc(_tmp2A9),((_tmp808->tl=_tmp318,_tmp808))))): _tmp318),((
_tmp807[0]=Cyc_Absynpp_qvar2doc(_tmp2A8),Cyc_PP_cat(_tag_dyneither(_tmp807,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL197;}_LL1DA: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2AB=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp24C;if(_tmp2AB->tag != 30)goto
_LL1DC;else{_tmp2AC=_tmp2AB->f2;}}_LL1DB:{const char*_tmp80A;s=Cyc_Absynpp_group_braces(((
_tmp80A=",",_tag_dyneither(_tmp80A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2AC));}goto _LL197;_LL1DC: {struct Cyc_Absyn_Datatype_e_struct*_tmp2AD=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp24C;if(_tmp2AD->tag != 31)goto _LL1DE;else{
_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AD->f3;}}_LL1DD: if(_tmp2AE == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2AF->name);else{const char*_tmp811;const char*_tmp810;const char*_tmp80F;struct
Cyc_PP_Doc*_tmp80E[2];s=((_tmp80E[1]=Cyc_PP_egroup(((_tmp811="(",_tag_dyneither(
_tmp811,sizeof(char),2))),((_tmp810=")",_tag_dyneither(_tmp810,sizeof(char),2))),((
_tmp80F=",",_tag_dyneither(_tmp80F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2AE)),((_tmp80E[0]=Cyc_Absynpp_qvar2doc(_tmp2AF->name),
Cyc_PP_cat(_tag_dyneither(_tmp80E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL197;
_LL1DE: {struct Cyc_Absyn_Enum_e_struct*_tmp2B0=(struct Cyc_Absyn_Enum_e_struct*)
_tmp24C;if(_tmp2B0->tag != 32)goto _LL1E0;else{_tmp2B1=_tmp2B0->f1;}}_LL1DF: s=Cyc_Absynpp_qvar2doc(
_tmp2B1);goto _LL197;_LL1E0: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2B2=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp24C;if(_tmp2B2->tag != 33)goto _LL1E2;else{
_tmp2B3=_tmp2B2->f1;}}_LL1E1: s=Cyc_Absynpp_qvar2doc(_tmp2B3);goto _LL197;_LL1E2: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2B4=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp24C;if(_tmp2B4->tag != 34)goto _LL1E4;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B5.is_calloc;
_tmp2B7=_tmp2B5.rgn;_tmp2B8=_tmp2B5.elt_type;_tmp2B9=_tmp2B5.num_elts;}}_LL1E3:
if(_tmp2B6){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2B8)),0);if(_tmp2B7 == 0){const char*_tmp818;const char*_tmp817;const char*
_tmp816;struct Cyc_PP_Doc*_tmp815[5];s=((_tmp815[4]=Cyc_PP_text(((_tmp816=")",
_tag_dyneither(_tmp816,sizeof(char),2)))),((_tmp815[3]=Cyc_Absynpp_exp2doc(st),((
_tmp815[2]=Cyc_PP_text(((_tmp817=",",_tag_dyneither(_tmp817,sizeof(char),2)))),((
_tmp815[1]=Cyc_Absynpp_exp2doc(_tmp2B9),((_tmp815[0]=Cyc_PP_text(((_tmp818="calloc(",
_tag_dyneither(_tmp818,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp815,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp821;const char*
_tmp820;const char*_tmp81F;const char*_tmp81E;struct Cyc_PP_Doc*_tmp81D[7];s=((
_tmp81D[6]=Cyc_PP_text(((_tmp81E=")",_tag_dyneither(_tmp81E,sizeof(char),2)))),((
_tmp81D[5]=Cyc_Absynpp_exp2doc(st),((_tmp81D[4]=Cyc_PP_text(((_tmp81F=",",
_tag_dyneither(_tmp81F,sizeof(char),2)))),((_tmp81D[3]=Cyc_Absynpp_exp2doc(
_tmp2B9),((_tmp81D[2]=Cyc_PP_text(((_tmp820=",",_tag_dyneither(_tmp820,sizeof(
char),2)))),((_tmp81D[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2B7),((
_tmp81D[0]=Cyc_PP_text(((_tmp821="rcalloc(",_tag_dyneither(_tmp821,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp81D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2B8 == 0)new_e=_tmp2B9;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2B8,0),_tmp2B9,0);}if(_tmp2B7 == 0){const char*
_tmp826;const char*_tmp825;struct Cyc_PP_Doc*_tmp824[3];s=((_tmp824[2]=Cyc_PP_text(((
_tmp825=")",_tag_dyneither(_tmp825,sizeof(char),2)))),((_tmp824[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp824[0]=Cyc_PP_text(((_tmp826="malloc(",_tag_dyneither(_tmp826,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp824,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp82D;const char*_tmp82C;const char*_tmp82B;struct Cyc_PP_Doc*
_tmp82A[5];s=((_tmp82A[4]=Cyc_PP_text(((_tmp82B=")",_tag_dyneither(_tmp82B,
sizeof(char),2)))),((_tmp82A[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp82A[2]=Cyc_PP_text(((
_tmp82C=",",_tag_dyneither(_tmp82C,sizeof(char),2)))),((_tmp82A[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2B7),((_tmp82A[0]=Cyc_PP_text(((_tmp82D="rmalloc(",
_tag_dyneither(_tmp82D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL197;_LL1E4: {struct Cyc_Absyn_Swap_e_struct*
_tmp2BA=(struct Cyc_Absyn_Swap_e_struct*)_tmp24C;if(_tmp2BA->tag != 35)goto _LL1E6;
else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;}}_LL1E5:{const char*_tmp834;const
char*_tmp833;const char*_tmp832;struct Cyc_PP_Doc*_tmp831[5];s=((_tmp831[4]=Cyc_PP_text(((
_tmp832=")",_tag_dyneither(_tmp832,sizeof(char),2)))),((_tmp831[3]=Cyc_Absynpp_exp2doc(
_tmp2BC),((_tmp831[2]=Cyc_PP_text(((_tmp833=",",_tag_dyneither(_tmp833,sizeof(
char),2)))),((_tmp831[1]=Cyc_Absynpp_exp2doc(_tmp2BB),((_tmp831[0]=Cyc_PP_text(((
_tmp834="cycswap(",_tag_dyneither(_tmp834,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp831,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL197;
_LL1E6: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2BD=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp24C;if(_tmp2BD->tag != 36)goto _LL1E8;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BD->f2;}}
_LL1E7:{const char*_tmp835;s=Cyc_Absynpp_group_braces(((_tmp835=",",
_tag_dyneither(_tmp835,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2BF));}goto _LL197;_LL1E8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2C0=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp24C;if(_tmp2C0->tag != 37)goto _LL197;else{_tmp2C1=
_tmp2C0->f1;}}_LL1E9:{const char*_tmp83A;const char*_tmp839;struct Cyc_PP_Doc*
_tmp838[7];s=((_tmp838[6]=Cyc_PP_text(((_tmp839=")",_tag_dyneither(_tmp839,
sizeof(char),2)))),((_tmp838[5]=Cyc_Absynpp_rb(),((_tmp838[4]=Cyc_PP_blank_doc(),((
_tmp838[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2C1,1)),((_tmp838[2]=Cyc_PP_blank_doc(),((
_tmp838[1]=Cyc_Absynpp_lb(),((_tmp838[0]=Cyc_PP_text(((_tmp83A="(",
_tag_dyneither(_tmp83A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp838,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL197;_LL197:;}if(inprec >= 
myprec){const char*_tmp83F;const char*_tmp83E;struct Cyc_PP_Doc*_tmp83D[3];s=((
_tmp83D[2]=Cyc_PP_text(((_tmp83E=")",_tag_dyneither(_tmp83E,sizeof(char),2)))),((
_tmp83D[1]=s,((_tmp83D[0]=Cyc_PP_text(((_tmp83F="(",_tag_dyneither(_tmp83F,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp83D,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp33C=d;struct Cyc_Absyn_Exp*_tmp33E;
struct _dyneither_ptr*_tmp340;_LL1F0: {struct Cyc_Absyn_ArrayElement_struct*
_tmp33D=(struct Cyc_Absyn_ArrayElement_struct*)_tmp33C;if(_tmp33D->tag != 0)goto
_LL1F2;else{_tmp33E=_tmp33D->f1;}}_LL1F1: {const char*_tmp844;const char*_tmp843;
struct Cyc_PP_Doc*_tmp842[3];return(_tmp842[2]=Cyc_PP_text(((_tmp843="]",
_tag_dyneither(_tmp843,sizeof(char),2)))),((_tmp842[1]=Cyc_Absynpp_exp2doc(
_tmp33E),((_tmp842[0]=Cyc_PP_text(((_tmp844=".[",_tag_dyneither(_tmp844,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp842,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1F2: {struct Cyc_Absyn_FieldName_struct*_tmp33F=(struct Cyc_Absyn_FieldName_struct*)
_tmp33C;if(_tmp33F->tag != 1)goto _LL1EF;else{_tmp340=_tmp33F->f1;}}_LL1F3: {const
char*_tmp847;struct Cyc_PP_Doc*_tmp846[2];return(_tmp846[1]=Cyc_PP_textptr(
_tmp340),((_tmp846[0]=Cyc_PP_text(((_tmp847=".",_tag_dyneither(_tmp847,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp846,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1EF:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp84E;const char*_tmp84D;const char*_tmp84C;struct Cyc_PP_Doc*
_tmp84B[2];return(_tmp84B[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp84B[0]=Cyc_PP_egroup(((
_tmp84E="",_tag_dyneither(_tmp84E,sizeof(char),1))),((_tmp84D="=",_tag_dyneither(
_tmp84D,sizeof(char),2))),((_tmp84C="=",_tag_dyneither(_tmp84C,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp84B,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp851;const char*_tmp850;const char*_tmp84F;
return Cyc_PP_group(((_tmp84F="",_tag_dyneither(_tmp84F,sizeof(char),1))),((
_tmp850="",_tag_dyneither(_tmp850,sizeof(char),1))),((_tmp851=",",_tag_dyneither(
_tmp851,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp34D=
c;struct _tuple3 _tmp34E;enum Cyc_Absyn_Sign _tmp34F;char _tmp350;struct _tuple4
_tmp351;enum Cyc_Absyn_Sign _tmp352;short _tmp353;struct _tuple5 _tmp354;enum Cyc_Absyn_Sign
_tmp355;int _tmp356;struct _tuple5 _tmp357;enum Cyc_Absyn_Sign _tmp358;int _tmp359;
struct _tuple5 _tmp35A;enum Cyc_Absyn_Sign _tmp35B;int _tmp35C;struct _tuple6 _tmp35D;
enum Cyc_Absyn_Sign _tmp35E;long long _tmp35F;struct _dyneither_ptr _tmp360;int
_tmp361;struct _dyneither_ptr _tmp362;_LL1F5: if((_tmp34D.Char_c).tag != 2)goto
_LL1F7;_tmp34E=(struct _tuple3)(_tmp34D.Char_c).val;_tmp34F=_tmp34E.f1;_tmp350=
_tmp34E.f2;_LL1F6: {const char*_tmp855;void*_tmp854[1];struct Cyc_String_pa_struct
_tmp853;return Cyc_PP_text((struct _dyneither_ptr)((_tmp853.tag=0,((_tmp853.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp350)),((
_tmp854[0]=& _tmp853,Cyc_aprintf(((_tmp855="'%s'",_tag_dyneither(_tmp855,sizeof(
char),5))),_tag_dyneither(_tmp854,sizeof(void*),1)))))))));}_LL1F7: if((_tmp34D.Short_c).tag
!= 3)goto _LL1F9;_tmp351=(struct _tuple4)(_tmp34D.Short_c).val;_tmp352=_tmp351.f1;
_tmp353=_tmp351.f2;_LL1F8: {const char*_tmp859;void*_tmp858[1];struct Cyc_Int_pa_struct
_tmp857;return Cyc_PP_text((struct _dyneither_ptr)((_tmp857.tag=1,((_tmp857.f1=(
unsigned long)((int)_tmp353),((_tmp858[0]=& _tmp857,Cyc_aprintf(((_tmp859="%d",
_tag_dyneither(_tmp859,sizeof(char),3))),_tag_dyneither(_tmp858,sizeof(void*),1)))))))));}
_LL1F9: if((_tmp34D.Int_c).tag != 4)goto _LL1FB;_tmp354=(struct _tuple5)(_tmp34D.Int_c).val;
_tmp355=_tmp354.f1;if(_tmp355 != Cyc_Absyn_None)goto _LL1FB;_tmp356=_tmp354.f2;
_LL1FA: _tmp359=_tmp356;goto _LL1FC;_LL1FB: if((_tmp34D.Int_c).tag != 4)goto _LL1FD;
_tmp357=(struct _tuple5)(_tmp34D.Int_c).val;_tmp358=_tmp357.f1;if(_tmp358 != Cyc_Absyn_Signed)
goto _LL1FD;_tmp359=_tmp357.f2;_LL1FC: {const char*_tmp85D;void*_tmp85C[1];struct
Cyc_Int_pa_struct _tmp85B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp85B.tag=1,((
_tmp85B.f1=(unsigned long)_tmp359,((_tmp85C[0]=& _tmp85B,Cyc_aprintf(((_tmp85D="%d",
_tag_dyneither(_tmp85D,sizeof(char),3))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))))));}
_LL1FD: if((_tmp34D.Int_c).tag != 4)goto _LL1FF;_tmp35A=(struct _tuple5)(_tmp34D.Int_c).val;
_tmp35B=_tmp35A.f1;if(_tmp35B != Cyc_Absyn_Unsigned)goto _LL1FF;_tmp35C=_tmp35A.f2;
_LL1FE: {const char*_tmp861;void*_tmp860[1];struct Cyc_Int_pa_struct _tmp85F;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp85F.tag=1,((_tmp85F.f1=(unsigned int)
_tmp35C,((_tmp860[0]=& _tmp85F,Cyc_aprintf(((_tmp861="%u",_tag_dyneither(_tmp861,
sizeof(char),3))),_tag_dyneither(_tmp860,sizeof(void*),1)))))))));}_LL1FF: if((
_tmp34D.LongLong_c).tag != 5)goto _LL201;_tmp35D=(struct _tuple6)(_tmp34D.LongLong_c).val;
_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;_LL200: {const char*_tmp862;return Cyc_PP_text(((
_tmp862="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp862,sizeof(char),27))));}
_LL201: if((_tmp34D.Float_c).tag != 6)goto _LL203;_tmp360=(struct _dyneither_ptr)(
_tmp34D.Float_c).val;_LL202: return Cyc_PP_text(_tmp360);_LL203: if((_tmp34D.Null_c).tag
!= 1)goto _LL205;_tmp361=(int)(_tmp34D.Null_c).val;_LL204: {const char*_tmp863;
return Cyc_PP_text(((_tmp863="NULL",_tag_dyneither(_tmp863,sizeof(char),5))));}
_LL205: if((_tmp34D.String_c).tag != 7)goto _LL1F4;_tmp362=(struct _dyneither_ptr)(
_tmp34D.String_c).val;_LL206: {const char*_tmp868;const char*_tmp867;struct Cyc_PP_Doc*
_tmp866[3];return(_tmp866[2]=Cyc_PP_text(((_tmp867="\"",_tag_dyneither(_tmp867,
sizeof(char),2)))),((_tmp866[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp362)),((
_tmp866[0]=Cyc_PP_text(((_tmp868="\"",_tag_dyneither(_tmp868,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp866,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F4:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp875;const char*_tmp874;void*_tmp873[1];struct Cyc_String_pa_struct _tmp872;
struct Cyc_Core_Failure_struct*_tmp871;(int)_throw((void*)((_tmp871=_cycalloc(
sizeof(*_tmp871)),((_tmp871[0]=((_tmp875.tag=Cyc_Core_Failure,((_tmp875.f1=(
struct _dyneither_ptr)((_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp873[0]=& _tmp872,Cyc_aprintf(((
_tmp874="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp874,
sizeof(char),47))),_tag_dyneither(_tmp873,sizeof(void*),1)))))))),_tmp875)))),
_tmp871)))));}{const char*_tmp87A;const char*_tmp879;struct Cyc_PP_Doc*_tmp878[3];
return(_tmp878[2]=Cyc_PP_text(((_tmp879=")",_tag_dyneither(_tmp879,sizeof(char),
2)))),((_tmp878[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp878[0]=
Cyc_PP_text(((_tmp87A="numelts(",_tag_dyneither(_tmp87A,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp878,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp887;const char*_tmp886;void*_tmp885[1];
struct Cyc_String_pa_struct _tmp884;struct Cyc_Core_Failure_struct*_tmp883;(int)
_throw((void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp887.tag=
Cyc_Core_Failure,((_tmp887.f1=(struct _dyneither_ptr)((_tmp884.tag=0,((_tmp884.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp885[0]=& _tmp884,Cyc_aprintf(((_tmp886="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp886,sizeof(char),38))),_tag_dyneither(_tmp885,sizeof(void*),1)))))))),
_tmp887)))),_tmp883)))));}else{if(ds->tl == 0){const char*_tmp88A;struct Cyc_PP_Doc*
_tmp889[3];return(_tmp889[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp889[1]=Cyc_PP_text(((
_tmp88A=" ",_tag_dyneither(_tmp88A,sizeof(char),2)))),((_tmp889[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp889,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp897;const char*
_tmp896;void*_tmp895[1];struct Cyc_String_pa_struct _tmp894;struct Cyc_Core_Failure_struct*
_tmp893;(int)_throw((void*)((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893[0]=((
_tmp897.tag=Cyc_Core_Failure,((_tmp897.f1=(struct _dyneither_ptr)((_tmp894.tag=0,((
_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp895[0]=& _tmp894,Cyc_aprintf(((_tmp896="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp896,sizeof(char),47))),_tag_dyneither(_tmp895,sizeof(void*),1)))))))),
_tmp897)))),_tmp893)))));}else{const char*_tmp89C;const char*_tmp89B;struct Cyc_PP_Doc*
_tmp89A[5];return(_tmp89A[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp89A[3]=Cyc_PP_text(((_tmp89B=" ",_tag_dyneither(
_tmp89B,sizeof(char),2)))),((_tmp89A[2]=ps,((_tmp89A[1]=Cyc_PP_text(((_tmp89C=" ",
_tag_dyneither(_tmp89C,sizeof(char),2)))),((_tmp89A[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp89A,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL207: {const char*_tmp89D;return(_tmp89D="+",_tag_dyneither(_tmp89D,sizeof(char),
2));}case Cyc_Absyn_Times: _LL208: {const char*_tmp89E;return(_tmp89E="*",
_tag_dyneither(_tmp89E,sizeof(char),2));}case Cyc_Absyn_Minus: _LL209: {const char*
_tmp89F;return(_tmp89F="-",_tag_dyneither(_tmp89F,sizeof(char),2));}case Cyc_Absyn_Div:
_LL20A: {const char*_tmp8A0;return(_tmp8A0="/",_tag_dyneither(_tmp8A0,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL20B: {const char*_tmp8A2;const char*_tmp8A1;return Cyc_Absynpp_print_for_cycdoc?(
_tmp8A2="\\%",_tag_dyneither(_tmp8A2,sizeof(char),3)):((_tmp8A1="%",
_tag_dyneither(_tmp8A1,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL20C: {const char*
_tmp8A3;return(_tmp8A3="==",_tag_dyneither(_tmp8A3,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL20D: {const char*_tmp8A4;return(_tmp8A4="!=",_tag_dyneither(_tmp8A4,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL20E: {const char*_tmp8A5;return(_tmp8A5=">",
_tag_dyneither(_tmp8A5,sizeof(char),2));}case Cyc_Absyn_Lt: _LL20F: {const char*
_tmp8A6;return(_tmp8A6="<",_tag_dyneither(_tmp8A6,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL210: {const char*_tmp8A7;return(_tmp8A7=">=",_tag_dyneither(_tmp8A7,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL211: {const char*_tmp8A8;return(_tmp8A8="<=",
_tag_dyneither(_tmp8A8,sizeof(char),3));}case Cyc_Absyn_Not: _LL212: {const char*
_tmp8A9;return(_tmp8A9="!",_tag_dyneither(_tmp8A9,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL213: {const char*_tmp8AA;return(_tmp8AA="~",_tag_dyneither(_tmp8AA,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL214: {const char*_tmp8AB;return(_tmp8AB="&",
_tag_dyneither(_tmp8AB,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL215: {const char*
_tmp8AC;return(_tmp8AC="|",_tag_dyneither(_tmp8AC,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL216: {const char*_tmp8AD;return(_tmp8AD="^",_tag_dyneither(_tmp8AD,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL217: {const char*_tmp8AE;return(_tmp8AE="<<",
_tag_dyneither(_tmp8AE,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL218: {const
char*_tmp8AF;return(_tmp8AF=">>",_tag_dyneither(_tmp8AF,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL219: {const char*_tmp8B0;return(_tmp8B0=">>>",
_tag_dyneither(_tmp8B0,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL21A: {const
char*_tmp8B1;return(_tmp8B1="numelts",_tag_dyneither(_tmp8B1,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp3A0=s->r;_LL21D: {struct Cyc_Absyn_Decl_s_struct*
_tmp3A1=(struct Cyc_Absyn_Decl_s_struct*)_tmp3A0;if(_tmp3A1->tag != 12)goto _LL21F;}
_LL21E: return 1;_LL21F:;_LL220: return 0;_LL21C:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3A2=st->r;struct Cyc_Absyn_Exp*
_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Exp*
_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Stmt*_tmp3AD;struct Cyc_Absyn_Stmt*
_tmp3AE;struct _tuple8 _tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Stmt*
_tmp3B2;struct _dyneither_ptr*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B8;struct _tuple8
_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct _tuple8 _tmp3BB;struct Cyc_Absyn_Exp*
_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_List_List*
_tmp3C0;struct Cyc_List_List*_tmp3C2;struct Cyc_List_List*_tmp3C4;struct Cyc_Absyn_Decl*
_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C7;struct _dyneither_ptr*_tmp3C9;struct Cyc_Absyn_Stmt*
_tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CC;struct _tuple8 _tmp3CD;struct Cyc_Absyn_Exp*
_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D0;struct Cyc_List_List*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D3;_LL222: {struct Cyc_Absyn_Skip_s_struct*_tmp3A3=(struct Cyc_Absyn_Skip_s_struct*)
_tmp3A2;if(_tmp3A3->tag != 0)goto _LL224;}_LL223:{const char*_tmp8B2;s=Cyc_PP_text(((
_tmp8B2=";",_tag_dyneither(_tmp8B2,sizeof(char),2))));}goto _LL221;_LL224: {
struct Cyc_Absyn_Exp_s_struct*_tmp3A4=(struct Cyc_Absyn_Exp_s_struct*)_tmp3A2;if(
_tmp3A4->tag != 1)goto _LL226;else{_tmp3A5=_tmp3A4->f1;}}_LL225:{const char*_tmp8B5;
struct Cyc_PP_Doc*_tmp8B4[2];s=((_tmp8B4[1]=Cyc_PP_text(((_tmp8B5=";",
_tag_dyneither(_tmp8B5,sizeof(char),2)))),((_tmp8B4[0]=Cyc_Absynpp_exp2doc(
_tmp3A5),Cyc_PP_cat(_tag_dyneither(_tmp8B4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL221;_LL226: {struct Cyc_Absyn_Seq_s_struct*_tmp3A6=(struct Cyc_Absyn_Seq_s_struct*)
_tmp3A2;if(_tmp3A6->tag != 2)goto _LL228;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;}}
_LL227: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp3A7)){
struct Cyc_PP_Doc*_tmp8C0[5];struct Cyc_PP_Doc*_tmp8BF[7];const char*_tmp8BE;const
char*_tmp8BD;struct Cyc_PP_Doc*_tmp8BC[7];s=((_tmp8BC[6]=Cyc_Absynpp_is_declaration(
_tmp3A8)?stmtexp?(_tmp8BF[6]=Cyc_PP_line_doc(),((_tmp8BF[5]=Cyc_PP_text(((
_tmp8BE=");",_tag_dyneither(_tmp8BE,sizeof(char),3)))),((_tmp8BF[4]=Cyc_Absynpp_rb(),((
_tmp8BF[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp)),((_tmp8BF[2]=Cyc_PP_blank_doc(),((
_tmp8BF[1]=Cyc_Absynpp_lb(),((_tmp8BF[0]=Cyc_PP_text(((_tmp8BD="(",
_tag_dyneither(_tmp8BD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BF,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8C0[4]=Cyc_PP_line_doc(),((
_tmp8C0[3]=Cyc_Absynpp_rb(),((_tmp8C0[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A8,stmtexp)),((_tmp8C0[1]=Cyc_PP_blank_doc(),((_tmp8C0[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8C0,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp3A8,stmtexp),((_tmp8BC[5]=Cyc_PP_line_doc(),((_tmp8BC[4]=Cyc_Absynpp_rb(),((
_tmp8BC[3]=Cyc_PP_line_doc(),((_tmp8BC[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A7,0)),((_tmp8BC[1]=Cyc_PP_blank_doc(),((_tmp8BC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp3A8)){struct Cyc_PP_Doc*_tmp8CB[4];struct Cyc_PP_Doc*_tmp8CA[6];const char*
_tmp8C9;const char*_tmp8C8;struct Cyc_PP_Doc*_tmp8C7[4];s=((_tmp8C7[3]=Cyc_PP_line_doc(),((
_tmp8C7[2]=stmtexp?(_tmp8CA[5]=Cyc_PP_text(((_tmp8C9=");",_tag_dyneither(_tmp8C9,
sizeof(char),3)))),((_tmp8CA[4]=Cyc_Absynpp_rb(),((_tmp8CA[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A8,stmtexp)),((_tmp8CA[2]=Cyc_PP_blank_doc(),((_tmp8CA[1]=Cyc_Absynpp_lb(),((
_tmp8CA[0]=Cyc_PP_text(((_tmp8C8="(",_tag_dyneither(_tmp8C8,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8CA,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8CB[3]=Cyc_Absynpp_rb(),((_tmp8CB[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A8,stmtexp)),((_tmp8CB[1]=Cyc_PP_blank_doc(),((_tmp8CB[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8CB,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8C7[1]=Cyc_PP_line_doc(),((_tmp8C7[0]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(
_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8CC[3];s=((_tmp8CC[2]=Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp),((_tmp8CC[1]=Cyc_PP_line_doc(),((
_tmp8CC[0]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(_tag_dyneither(_tmp8CC,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8CD[3];s=((
_tmp8CD[2]=Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp),((_tmp8CD[1]=Cyc_PP_line_doc(),((
_tmp8CD[0]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(_tag_dyneither(_tmp8CD,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL221;_LL228: {struct Cyc_Absyn_Return_s_struct*
_tmp3A9=(struct Cyc_Absyn_Return_s_struct*)_tmp3A2;if(_tmp3A9->tag != 3)goto _LL22A;
else{_tmp3AA=_tmp3A9->f1;}}_LL229: if(_tmp3AA == 0){const char*_tmp8CE;s=Cyc_PP_text(((
_tmp8CE="return;",_tag_dyneither(_tmp8CE,sizeof(char),8))));}else{const char*
_tmp8D3;const char*_tmp8D2;struct Cyc_PP_Doc*_tmp8D1[3];s=((_tmp8D1[2]=Cyc_PP_text(((
_tmp8D2=";",_tag_dyneither(_tmp8D2,sizeof(char),2)))),((_tmp8D1[1]=_tmp3AA == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3AA),((_tmp8D1[0]=
Cyc_PP_text(((_tmp8D3="return ",_tag_dyneither(_tmp8D3,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8D1,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL221;_LL22A: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp3AB=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3A2;if(_tmp3AB->tag != 4)goto _LL22C;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;
_tmp3AE=_tmp3AB->f3;}}_LL22B: {int print_else;{void*_tmp3E7=_tmp3AE->r;_LL247: {
struct Cyc_Absyn_Skip_s_struct*_tmp3E8=(struct Cyc_Absyn_Skip_s_struct*)_tmp3E7;
if(_tmp3E8->tag != 0)goto _LL249;}_LL248: print_else=0;goto _LL246;_LL249:;_LL24A:
print_else=1;goto _LL246;_LL246:;}{const char*_tmp8E2;const char*_tmp8E1;struct Cyc_PP_Doc*
_tmp8E0[2];struct Cyc_PP_Doc*_tmp8DF[6];struct Cyc_PP_Doc*_tmp8DE[2];const char*
_tmp8DD;struct Cyc_PP_Doc*_tmp8DC[8];s=((_tmp8DC[7]=print_else?(_tmp8DF[5]=Cyc_Absynpp_rb(),((
_tmp8DF[4]=Cyc_PP_line_doc(),((_tmp8DF[3]=Cyc_PP_nest(2,((_tmp8DE[1]=Cyc_Absynpp_stmt2doc(
_tmp3AE,0),((_tmp8DE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DE,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DF[2]=Cyc_Absynpp_lb(),((_tmp8DF[1]=
Cyc_PP_text(((_tmp8DD="else ",_tag_dyneither(_tmp8DD,sizeof(char),6)))),((
_tmp8DF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DF,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8DC[6]=Cyc_Absynpp_rb(),((_tmp8DC[5]=Cyc_PP_line_doc(),((
_tmp8DC[4]=Cyc_PP_nest(2,((_tmp8E0[1]=Cyc_Absynpp_stmt2doc(_tmp3AD,0),((_tmp8E0[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E0,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8DC[3]=Cyc_Absynpp_lb(),((_tmp8DC[2]=Cyc_PP_text(((_tmp8E1=") ",
_tag_dyneither(_tmp8E1,sizeof(char),3)))),((_tmp8DC[1]=Cyc_Absynpp_exp2doc(
_tmp3AC),((_tmp8DC[0]=Cyc_PP_text(((_tmp8E2="if (",_tag_dyneither(_tmp8E2,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8DC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL221;}_LL22C: {struct Cyc_Absyn_While_s_struct*_tmp3AF=(struct Cyc_Absyn_While_s_struct*)
_tmp3A2;if(_tmp3AF->tag != 5)goto _LL22E;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3B0.f1;
_tmp3B2=_tmp3AF->f2;}}_LL22D:{const char*_tmp8E9;const char*_tmp8E8;struct Cyc_PP_Doc*
_tmp8E7[2];struct Cyc_PP_Doc*_tmp8E6[7];s=((_tmp8E6[6]=Cyc_Absynpp_rb(),((_tmp8E6[
5]=Cyc_PP_line_doc(),((_tmp8E6[4]=Cyc_PP_nest(2,((_tmp8E7[1]=Cyc_Absynpp_stmt2doc(
_tmp3B2,0),((_tmp8E7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E7,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E6[3]=Cyc_Absynpp_lb(),((_tmp8E6[2]=
Cyc_PP_text(((_tmp8E8=") ",_tag_dyneither(_tmp8E8,sizeof(char),3)))),((_tmp8E6[1]=
Cyc_Absynpp_exp2doc(_tmp3B1),((_tmp8E6[0]=Cyc_PP_text(((_tmp8E9="while (",
_tag_dyneither(_tmp8E9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8E6,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL221;_LL22E: {struct Cyc_Absyn_Break_s_struct*
_tmp3B3=(struct Cyc_Absyn_Break_s_struct*)_tmp3A2;if(_tmp3B3->tag != 6)goto _LL230;}
_LL22F:{const char*_tmp8EA;s=Cyc_PP_text(((_tmp8EA="break;",_tag_dyneither(
_tmp8EA,sizeof(char),7))));}goto _LL221;_LL230: {struct Cyc_Absyn_Continue_s_struct*
_tmp3B4=(struct Cyc_Absyn_Continue_s_struct*)_tmp3A2;if(_tmp3B4->tag != 7)goto
_LL232;}_LL231:{const char*_tmp8EB;s=Cyc_PP_text(((_tmp8EB="continue;",
_tag_dyneither(_tmp8EB,sizeof(char),10))));}goto _LL221;_LL232: {struct Cyc_Absyn_Goto_s_struct*
_tmp3B5=(struct Cyc_Absyn_Goto_s_struct*)_tmp3A2;if(_tmp3B5->tag != 8)goto _LL234;
else{_tmp3B6=_tmp3B5->f1;}}_LL233:{const char*_tmp8EF;void*_tmp8EE[1];struct Cyc_String_pa_struct
_tmp8ED;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8ED.tag=0,((_tmp8ED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3B6),((_tmp8EE[0]=& _tmp8ED,Cyc_aprintf(((
_tmp8EF="goto %s;",_tag_dyneither(_tmp8EF,sizeof(char),9))),_tag_dyneither(
_tmp8EE,sizeof(void*),1)))))))));}goto _LL221;_LL234: {struct Cyc_Absyn_For_s_struct*
_tmp3B7=(struct Cyc_Absyn_For_s_struct*)_tmp3A2;if(_tmp3B7->tag != 9)goto _LL236;
else{_tmp3B8=_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B7->f3;
_tmp3BC=_tmp3BB.f1;_tmp3BD=_tmp3B7->f4;}}_LL235:{const char*_tmp8FA;const char*
_tmp8F9;const char*_tmp8F8;const char*_tmp8F7;struct Cyc_PP_Doc*_tmp8F6[2];struct
Cyc_PP_Doc*_tmp8F5[11];s=((_tmp8F5[10]=Cyc_Absynpp_rb(),((_tmp8F5[9]=Cyc_PP_line_doc(),((
_tmp8F5[8]=Cyc_PP_nest(2,((_tmp8F6[1]=Cyc_Absynpp_stmt2doc(_tmp3BD,0),((_tmp8F6[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8F5[7]=Cyc_Absynpp_lb(),((_tmp8F5[6]=Cyc_PP_text(((_tmp8F7=") ",
_tag_dyneither(_tmp8F7,sizeof(char),3)))),((_tmp8F5[5]=Cyc_Absynpp_exp2doc(
_tmp3BC),((_tmp8F5[4]=Cyc_PP_text(((_tmp8F8="; ",_tag_dyneither(_tmp8F8,sizeof(
char),3)))),((_tmp8F5[3]=Cyc_Absynpp_exp2doc(_tmp3BA),((_tmp8F5[2]=Cyc_PP_text(((
_tmp8F9="; ",_tag_dyneither(_tmp8F9,sizeof(char),3)))),((_tmp8F5[1]=Cyc_Absynpp_exp2doc(
_tmp3B8),((_tmp8F5[0]=Cyc_PP_text(((_tmp8FA="for(",_tag_dyneither(_tmp8FA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL221;_LL236: {struct Cyc_Absyn_Switch_s_struct*
_tmp3BE=(struct Cyc_Absyn_Switch_s_struct*)_tmp3A2;if(_tmp3BE->tag != 10)goto
_LL238;else{_tmp3BF=_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;}}_LL237:{const char*_tmp8FF;
const char*_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[8];s=((_tmp8FD[7]=Cyc_Absynpp_rb(),((
_tmp8FD[6]=Cyc_PP_line_doc(),((_tmp8FD[5]=Cyc_Absynpp_switchclauses2doc(_tmp3C0),((
_tmp8FD[4]=Cyc_PP_line_doc(),((_tmp8FD[3]=Cyc_Absynpp_lb(),((_tmp8FD[2]=Cyc_PP_text(((
_tmp8FE=") ",_tag_dyneither(_tmp8FE,sizeof(char),3)))),((_tmp8FD[1]=Cyc_Absynpp_exp2doc(
_tmp3BF),((_tmp8FD[0]=Cyc_PP_text(((_tmp8FF="switch (",_tag_dyneither(_tmp8FF,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8FD,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL221;_LL238: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3C1=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3A2;if(_tmp3C1->tag != 11)goto _LL23A;else{_tmp3C2=_tmp3C1->f1;if(_tmp3C2 != 0)
goto _LL23A;}}_LL239:{const char*_tmp900;s=Cyc_PP_text(((_tmp900="fallthru;",
_tag_dyneither(_tmp900,sizeof(char),10))));}goto _LL221;_LL23A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3C3=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3A2;if(_tmp3C3->tag != 11)goto
_LL23C;else{_tmp3C4=_tmp3C3->f1;}}_LL23B:{const char*_tmp905;const char*_tmp904;
struct Cyc_PP_Doc*_tmp903[3];s=((_tmp903[2]=Cyc_PP_text(((_tmp904=");",
_tag_dyneither(_tmp904,sizeof(char),3)))),((_tmp903[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3C4),((_tmp903[0]=Cyc_PP_text(((_tmp905="fallthru(",_tag_dyneither(
_tmp905,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp903,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL221;_LL23C: {struct Cyc_Absyn_Decl_s_struct*_tmp3C5=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3A2;if(_tmp3C5->tag != 12)goto _LL23E;else{_tmp3C6=_tmp3C5->f1;_tmp3C7=_tmp3C5->f2;}}
_LL23D:{struct Cyc_PP_Doc*_tmp906[3];s=((_tmp906[2]=Cyc_Absynpp_stmt2doc(_tmp3C7,
stmtexp),((_tmp906[1]=Cyc_PP_line_doc(),((_tmp906[0]=Cyc_Absynpp_decl2doc(
_tmp3C6),Cyc_PP_cat(_tag_dyneither(_tmp906,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL221;_LL23E: {struct Cyc_Absyn_Label_s_struct*_tmp3C8=(struct Cyc_Absyn_Label_s_struct*)
_tmp3A2;if(_tmp3C8->tag != 13)goto _LL240;else{_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;}}
_LL23F: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3CA)){if(
stmtexp){const char*_tmp90B;const char*_tmp90A;struct Cyc_PP_Doc*_tmp909[8];s=((
_tmp909[7]=Cyc_PP_text(((_tmp90A=");",_tag_dyneither(_tmp90A,sizeof(char),3)))),((
_tmp909[6]=Cyc_Absynpp_rb(),((_tmp909[5]=Cyc_PP_line_doc(),((_tmp909[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3CA,1)),((_tmp909[3]=Cyc_PP_line_doc(),((_tmp909[2]=
Cyc_Absynpp_lb(),((_tmp909[1]=Cyc_PP_text(((_tmp90B=": (",_tag_dyneither(_tmp90B,
sizeof(char),4)))),((_tmp909[0]=Cyc_PP_textptr(_tmp3C9),Cyc_PP_cat(
_tag_dyneither(_tmp909,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp90E;struct Cyc_PP_Doc*_tmp90D[7];s=((_tmp90D[6]=Cyc_Absynpp_rb(),((
_tmp90D[5]=Cyc_PP_line_doc(),((_tmp90D[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3CA,0)),((_tmp90D[3]=Cyc_PP_line_doc(),((_tmp90D[2]=Cyc_Absynpp_lb(),((
_tmp90D[1]=Cyc_PP_text(((_tmp90E=": ",_tag_dyneither(_tmp90E,sizeof(char),3)))),((
_tmp90D[0]=Cyc_PP_textptr(_tmp3C9),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp911;struct Cyc_PP_Doc*
_tmp910[3];s=((_tmp910[2]=Cyc_Absynpp_stmt2doc(_tmp3CA,stmtexp),((_tmp910[1]=Cyc_PP_text(((
_tmp911=": ",_tag_dyneither(_tmp911,sizeof(char),3)))),((_tmp910[0]=Cyc_PP_textptr(
_tmp3C9),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL221;_LL240: {struct Cyc_Absyn_Do_s_struct*_tmp3CB=(struct Cyc_Absyn_Do_s_struct*)
_tmp3A2;if(_tmp3CB->tag != 14)goto _LL242;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;
_tmp3CE=_tmp3CD.f1;}}_LL241:{const char*_tmp918;const char*_tmp917;const char*
_tmp916;struct Cyc_PP_Doc*_tmp915[9];s=((_tmp915[8]=Cyc_PP_text(((_tmp916=");",
_tag_dyneither(_tmp916,sizeof(char),3)))),((_tmp915[7]=Cyc_Absynpp_exp2doc(
_tmp3CE),((_tmp915[6]=Cyc_PP_text(((_tmp917=" while (",_tag_dyneither(_tmp917,
sizeof(char),9)))),((_tmp915[5]=Cyc_Absynpp_rb(),((_tmp915[4]=Cyc_PP_line_doc(),((
_tmp915[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CC,0)),((_tmp915[2]=Cyc_PP_line_doc(),((
_tmp915[1]=Cyc_Absynpp_lb(),((_tmp915[0]=Cyc_PP_text(((_tmp918="do ",
_tag_dyneither(_tmp918,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp915,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL221;_LL242: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3CF=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3A2;if(_tmp3CF->tag != 15)goto
_LL244;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;}}_LL243:{const char*_tmp91D;
const char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[12];s=((_tmp91B[11]=Cyc_Absynpp_rb(),((
_tmp91B[10]=Cyc_PP_line_doc(),((_tmp91B[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3D1)),((_tmp91B[8]=Cyc_PP_line_doc(),((_tmp91B[7]=Cyc_Absynpp_lb(),((_tmp91B[
6]=Cyc_PP_text(((_tmp91C=" catch ",_tag_dyneither(_tmp91C,sizeof(char),8)))),((
_tmp91B[5]=Cyc_Absynpp_rb(),((_tmp91B[4]=Cyc_PP_line_doc(),((_tmp91B[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3D0,0)),((_tmp91B[2]=Cyc_PP_line_doc(),((_tmp91B[1]=
Cyc_Absynpp_lb(),((_tmp91B[0]=Cyc_PP_text(((_tmp91D="try ",_tag_dyneither(
_tmp91D,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL221;_LL244: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3D2=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3A2;if(_tmp3D2->tag != 16)goto
_LL221;else{_tmp3D3=_tmp3D2->f1;}}_LL245:{const char*_tmp922;const char*_tmp921;
struct Cyc_PP_Doc*_tmp920[3];s=((_tmp920[2]=Cyc_PP_text(((_tmp921=");",
_tag_dyneither(_tmp921,sizeof(char),3)))),((_tmp920[1]=Cyc_Absynpp_exp2doc(
_tmp3D3),((_tmp920[0]=Cyc_PP_text(((_tmp922="reset_region(",_tag_dyneither(
_tmp922,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp920,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL221;_LL221:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp418=p->r;enum Cyc_Absyn_Sign _tmp41C;int _tmp41D;
char _tmp41F;struct _dyneither_ptr _tmp421;struct Cyc_Absyn_Vardecl*_tmp423;struct
Cyc_Absyn_Pat*_tmp424;struct Cyc_Absyn_Pat _tmp425;void*_tmp426;struct Cyc_Absyn_Vardecl*
_tmp429;struct Cyc_Absyn_Pat*_tmp42A;struct Cyc_Absyn_Tvar*_tmp42C;struct Cyc_Absyn_Vardecl*
_tmp42D;struct Cyc_List_List*_tmp42F;int _tmp430;struct Cyc_Absyn_Pat*_tmp432;
struct Cyc_Absyn_Vardecl*_tmp434;struct Cyc_Absyn_Pat*_tmp435;struct Cyc_Absyn_Pat
_tmp436;void*_tmp437;struct Cyc_Absyn_Vardecl*_tmp43A;struct Cyc_Absyn_Pat*_tmp43B;
struct _tuple0*_tmp43D;struct _tuple0*_tmp43F;struct Cyc_List_List*_tmp440;int
_tmp441;struct Cyc_Absyn_AggrInfo*_tmp443;struct Cyc_Absyn_AggrInfo _tmp444;union
Cyc_Absyn_AggrInfoU _tmp445;struct Cyc_List_List*_tmp446;struct Cyc_List_List*
_tmp447;int _tmp448;struct Cyc_Absyn_AggrInfo*_tmp44A;struct Cyc_List_List*_tmp44B;
struct Cyc_List_List*_tmp44C;int _tmp44D;struct Cyc_Absyn_Enumfield*_tmp44F;struct
Cyc_Absyn_Enumfield*_tmp451;struct Cyc_Absyn_Datatypefield*_tmp453;struct Cyc_List_List*
_tmp454;struct Cyc_Absyn_Datatypefield*_tmp456;struct Cyc_List_List*_tmp457;int
_tmp458;struct Cyc_Absyn_Exp*_tmp45A;_LL24C: {struct Cyc_Absyn_Wild_p_struct*
_tmp419=(struct Cyc_Absyn_Wild_p_struct*)_tmp418;if(_tmp419->tag != 0)goto _LL24E;}
_LL24D:{const char*_tmp923;s=Cyc_PP_text(((_tmp923="_",_tag_dyneither(_tmp923,
sizeof(char),2))));}goto _LL24B;_LL24E: {struct Cyc_Absyn_Null_p_struct*_tmp41A=(
struct Cyc_Absyn_Null_p_struct*)_tmp418;if(_tmp41A->tag != 8)goto _LL250;}_LL24F:{
const char*_tmp924;s=Cyc_PP_text(((_tmp924="NULL",_tag_dyneither(_tmp924,sizeof(
char),5))));}goto _LL24B;_LL250: {struct Cyc_Absyn_Int_p_struct*_tmp41B=(struct Cyc_Absyn_Int_p_struct*)
_tmp418;if(_tmp41B->tag != 9)goto _LL252;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;}}
_LL251: if(_tmp41C != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp928;
void*_tmp927[1];struct Cyc_Int_pa_struct _tmp926;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp926.tag=1,((_tmp926.f1=(unsigned long)_tmp41D,((_tmp927[0]=&
_tmp926,Cyc_aprintf(((_tmp928="%d",_tag_dyneither(_tmp928,sizeof(char),3))),
_tag_dyneither(_tmp927,sizeof(void*),1)))))))));}else{const char*_tmp92C;void*
_tmp92B[1];struct Cyc_Int_pa_struct _tmp92A;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp92A.tag=1,((_tmp92A.f1=(unsigned int)_tmp41D,((_tmp92B[0]=& _tmp92A,Cyc_aprintf(((
_tmp92C="%u",_tag_dyneither(_tmp92C,sizeof(char),3))),_tag_dyneither(_tmp92B,
sizeof(void*),1)))))))));}goto _LL24B;_LL252: {struct Cyc_Absyn_Char_p_struct*
_tmp41E=(struct Cyc_Absyn_Char_p_struct*)_tmp418;if(_tmp41E->tag != 10)goto _LL254;
else{_tmp41F=_tmp41E->f1;}}_LL253:{const char*_tmp930;void*_tmp92F[1];struct Cyc_String_pa_struct
_tmp92E;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp92E.tag=0,((_tmp92E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp41F)),((
_tmp92F[0]=& _tmp92E,Cyc_aprintf(((_tmp930="'%s'",_tag_dyneither(_tmp930,sizeof(
char),5))),_tag_dyneither(_tmp92F,sizeof(void*),1)))))))));}goto _LL24B;_LL254: {
struct Cyc_Absyn_Float_p_struct*_tmp420=(struct Cyc_Absyn_Float_p_struct*)_tmp418;
if(_tmp420->tag != 11)goto _LL256;else{_tmp421=_tmp420->f1;}}_LL255: s=Cyc_PP_text(
_tmp421);goto _LL24B;_LL256: {struct Cyc_Absyn_Var_p_struct*_tmp422=(struct Cyc_Absyn_Var_p_struct*)
_tmp418;if(_tmp422->tag != 1)goto _LL258;else{_tmp423=_tmp422->f1;_tmp424=_tmp422->f2;
_tmp425=*_tmp424;_tmp426=_tmp425.r;{struct Cyc_Absyn_Wild_p_struct*_tmp427=(
struct Cyc_Absyn_Wild_p_struct*)_tmp426;if(_tmp427->tag != 0)goto _LL258;};}}_LL257:
s=Cyc_Absynpp_qvar2doc(_tmp423->name);goto _LL24B;_LL258: {struct Cyc_Absyn_Var_p_struct*
_tmp428=(struct Cyc_Absyn_Var_p_struct*)_tmp418;if(_tmp428->tag != 1)goto _LL25A;
else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL259:{const char*_tmp933;struct
Cyc_PP_Doc*_tmp932[3];s=((_tmp932[2]=Cyc_Absynpp_pat2doc(_tmp42A),((_tmp932[1]=
Cyc_PP_text(((_tmp933=" as ",_tag_dyneither(_tmp933,sizeof(char),5)))),((_tmp932[
0]=Cyc_Absynpp_qvar2doc(_tmp429->name),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL24B;_LL25A: {struct Cyc_Absyn_TagInt_p_struct*
_tmp42B=(struct Cyc_Absyn_TagInt_p_struct*)_tmp418;if(_tmp42B->tag != 3)goto _LL25C;
else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;}}_LL25B:{const char*_tmp938;const
char*_tmp937;struct Cyc_PP_Doc*_tmp936[4];s=((_tmp936[3]=Cyc_PP_text(((_tmp937=">",
_tag_dyneither(_tmp937,sizeof(char),2)))),((_tmp936[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp42C)),((_tmp936[1]=Cyc_PP_text(((_tmp938="<",_tag_dyneither(_tmp938,sizeof(
char),2)))),((_tmp936[0]=Cyc_Absynpp_qvar2doc(_tmp42D->name),Cyc_PP_cat(
_tag_dyneither(_tmp936,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24B;_LL25C: {
struct Cyc_Absyn_Tuple_p_struct*_tmp42E=(struct Cyc_Absyn_Tuple_p_struct*)_tmp418;
if(_tmp42E->tag != 4)goto _LL25E;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;}}
_LL25D:{const char*_tmp941;const char*_tmp940;const char*_tmp93F;const char*_tmp93E;
struct Cyc_PP_Doc*_tmp93D[4];s=((_tmp93D[3]=_tmp430?Cyc_PP_text(((_tmp93E=", ...)",
_tag_dyneither(_tmp93E,sizeof(char),7)))): Cyc_PP_text(((_tmp93F=")",
_tag_dyneither(_tmp93F,sizeof(char),2)))),((_tmp93D[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp940=",",_tag_dyneither(_tmp940,
sizeof(char),2))),_tmp42F),((_tmp93D[1]=Cyc_PP_text(((_tmp941="(",_tag_dyneither(
_tmp941,sizeof(char),2)))),((_tmp93D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp93D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24B;_LL25E: {
struct Cyc_Absyn_Pointer_p_struct*_tmp431=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp418;if(_tmp431->tag != 5)goto _LL260;else{_tmp432=_tmp431->f1;}}_LL25F:{const
char*_tmp944;struct Cyc_PP_Doc*_tmp943[2];s=((_tmp943[1]=Cyc_Absynpp_pat2doc(
_tmp432),((_tmp943[0]=Cyc_PP_text(((_tmp944="&",_tag_dyneither(_tmp944,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL24B;_LL260: {struct Cyc_Absyn_Reference_p_struct*_tmp433=(struct Cyc_Absyn_Reference_p_struct*)
_tmp418;if(_tmp433->tag != 2)goto _LL262;else{_tmp434=_tmp433->f1;_tmp435=_tmp433->f2;
_tmp436=*_tmp435;_tmp437=_tmp436.r;{struct Cyc_Absyn_Wild_p_struct*_tmp438=(
struct Cyc_Absyn_Wild_p_struct*)_tmp437;if(_tmp438->tag != 0)goto _LL262;};}}_LL261:{
const char*_tmp947;struct Cyc_PP_Doc*_tmp946[2];s=((_tmp946[1]=Cyc_Absynpp_qvar2doc(
_tmp434->name),((_tmp946[0]=Cyc_PP_text(((_tmp947="*",_tag_dyneither(_tmp947,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp946,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL24B;_LL262: {struct Cyc_Absyn_Reference_p_struct*_tmp439=(struct Cyc_Absyn_Reference_p_struct*)
_tmp418;if(_tmp439->tag != 2)goto _LL264;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}
_LL263:{const char*_tmp94C;const char*_tmp94B;struct Cyc_PP_Doc*_tmp94A[4];s=((
_tmp94A[3]=Cyc_Absynpp_pat2doc(_tmp43B),((_tmp94A[2]=Cyc_PP_text(((_tmp94B=" as ",
_tag_dyneither(_tmp94B,sizeof(char),5)))),((_tmp94A[1]=Cyc_Absynpp_qvar2doc(
_tmp43A->name),((_tmp94A[0]=Cyc_PP_text(((_tmp94C="*",_tag_dyneither(_tmp94C,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp94A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL24B;_LL264: {struct Cyc_Absyn_UnknownId_p_struct*_tmp43C=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp418;if(_tmp43C->tag != 14)goto _LL266;else{_tmp43D=_tmp43C->f1;}}_LL265: s=Cyc_Absynpp_qvar2doc(
_tmp43D);goto _LL24B;_LL266: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp43E=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp418;if(_tmp43E->tag != 15)goto _LL268;
else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;_tmp441=_tmp43E->f3;}}_LL267: {
const char*_tmp94E;const char*_tmp94D;struct _dyneither_ptr term=_tmp441?(_tmp94E=", ...)",
_tag_dyneither(_tmp94E,sizeof(char),7)):((_tmp94D=")",_tag_dyneither(_tmp94D,
sizeof(char),2)));{const char*_tmp953;const char*_tmp952;struct Cyc_PP_Doc*_tmp951[
2];s=((_tmp951[1]=Cyc_PP_group(((_tmp953="(",_tag_dyneither(_tmp953,sizeof(char),
2))),term,((_tmp952=",",_tag_dyneither(_tmp952,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp440)),((_tmp951[0]=Cyc_Absynpp_qvar2doc(_tmp43F),Cyc_PP_cat(
_tag_dyneither(_tmp951,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL24B;}_LL268: {
struct Cyc_Absyn_Aggr_p_struct*_tmp442=(struct Cyc_Absyn_Aggr_p_struct*)_tmp418;
if(_tmp442->tag != 6)goto _LL26A;else{_tmp443=_tmp442->f1;if(_tmp443 == 0)goto
_LL26A;_tmp444=*_tmp443;_tmp445=_tmp444.aggr_info;_tmp446=_tmp442->f2;_tmp447=
_tmp442->f3;_tmp448=_tmp442->f4;}}_LL269: {const char*_tmp955;const char*_tmp954;
struct _dyneither_ptr term=_tmp448?(_tmp955=", ...}",_tag_dyneither(_tmp955,
sizeof(char),7)):((_tmp954="}",_tag_dyneither(_tmp954,sizeof(char),2)));struct
_tuple0*_tmp47D;struct _tuple9 _tmp47C=Cyc_Absyn_aggr_kinded_name(_tmp445);_tmp47D=
_tmp47C.f2;{const char*_tmp960;const char*_tmp95F;const char*_tmp95E;const char*
_tmp95D;const char*_tmp95C;struct Cyc_PP_Doc*_tmp95B[4];s=((_tmp95B[3]=Cyc_PP_group(((
_tmp95D="",_tag_dyneither(_tmp95D,sizeof(char),1))),term,((_tmp95C=",",
_tag_dyneither(_tmp95C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp447)),((_tmp95B[2]=Cyc_PP_egroup(((_tmp960="[",_tag_dyneither(_tmp960,
sizeof(char),2))),((_tmp95F="]",_tag_dyneither(_tmp95F,sizeof(char),2))),((
_tmp95E=",",_tag_dyneither(_tmp95E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp446))),((_tmp95B[1]=
Cyc_Absynpp_lb(),((_tmp95B[0]=Cyc_Absynpp_qvar2doc(_tmp47D),Cyc_PP_cat(
_tag_dyneither(_tmp95B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24B;}_LL26A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp449=(struct Cyc_Absyn_Aggr_p_struct*)_tmp418;
if(_tmp449->tag != 6)goto _LL26C;else{_tmp44A=_tmp449->f1;if(_tmp44A != 0)goto
_LL26C;_tmp44B=_tmp449->f2;_tmp44C=_tmp449->f3;_tmp44D=_tmp449->f4;}}_LL26B: {
const char*_tmp962;const char*_tmp961;struct _dyneither_ptr term=_tmp44D?(_tmp962=", ...}",
_tag_dyneither(_tmp962,sizeof(char),7)):((_tmp961="}",_tag_dyneither(_tmp961,
sizeof(char),2)));{const char*_tmp96D;const char*_tmp96C;const char*_tmp96B;const
char*_tmp96A;const char*_tmp969;struct Cyc_PP_Doc*_tmp968[3];s=((_tmp968[2]=Cyc_PP_group(((
_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),term,((_tmp969=",",
_tag_dyneither(_tmp969,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp44C)),((_tmp968[1]=Cyc_PP_egroup(((_tmp96D="[",_tag_dyneither(_tmp96D,
sizeof(char),2))),((_tmp96C="]",_tag_dyneither(_tmp96C,sizeof(char),2))),((
_tmp96B=",",_tag_dyneither(_tmp96B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp44B))),((_tmp968[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp968,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL24B;}_LL26C: {struct Cyc_Absyn_Enum_p_struct*_tmp44E=(struct Cyc_Absyn_Enum_p_struct*)
_tmp418;if(_tmp44E->tag != 12)goto _LL26E;else{_tmp44F=_tmp44E->f2;}}_LL26D:
_tmp451=_tmp44F;goto _LL26F;_LL26E: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp450=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp418;if(_tmp450->tag != 13)goto _LL270;else{
_tmp451=_tmp450->f2;}}_LL26F: s=Cyc_Absynpp_qvar2doc(_tmp451->name);goto _LL24B;
_LL270: {struct Cyc_Absyn_Datatype_p_struct*_tmp452=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp418;if(_tmp452->tag != 7)goto _LL272;else{_tmp453=_tmp452->f2;_tmp454=_tmp452->f3;
if(_tmp454 != 0)goto _LL272;}}_LL271: s=Cyc_Absynpp_qvar2doc(_tmp453->name);goto
_LL24B;_LL272: {struct Cyc_Absyn_Datatype_p_struct*_tmp455=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp418;if(_tmp455->tag != 7)goto _LL274;else{_tmp456=_tmp455->f2;_tmp457=_tmp455->f3;
_tmp458=_tmp455->f4;}}_LL273: {const char*_tmp96F;const char*_tmp96E;struct
_dyneither_ptr term=_tmp458?(_tmp96F=", ...)",_tag_dyneither(_tmp96F,sizeof(char),
7)):((_tmp96E=")",_tag_dyneither(_tmp96E,sizeof(char),2)));{const char*_tmp974;
const char*_tmp973;struct Cyc_PP_Doc*_tmp972[2];s=((_tmp972[1]=Cyc_PP_egroup(((
_tmp974="(",_tag_dyneither(_tmp974,sizeof(char),2))),term,((_tmp973=",",
_tag_dyneither(_tmp973,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp457)),((_tmp972[0]=Cyc_Absynpp_qvar2doc(_tmp456->name),Cyc_PP_cat(
_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL24B;}_LL274: {
struct Cyc_Absyn_Exp_p_struct*_tmp459=(struct Cyc_Absyn_Exp_p_struct*)_tmp418;if(
_tmp459->tag != 16)goto _LL24B;else{_tmp45A=_tmp459->f1;}}_LL275: s=Cyc_Absynpp_exp2doc(
_tmp45A);goto _LL24B;_LL24B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){const char*
_tmp97B;const char*_tmp97A;const char*_tmp979;struct Cyc_PP_Doc*_tmp978[2];return(
_tmp978[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp978[0]=Cyc_PP_egroup(((_tmp97B="",
_tag_dyneither(_tmp97B,sizeof(char),1))),((_tmp97A="=",_tag_dyneither(_tmp97A,
sizeof(char),2))),((_tmp979="=",_tag_dyneither(_tmp979,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp978,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp980;struct Cyc_PP_Doc*_tmp97F[2];struct Cyc_PP_Doc*
_tmp97E[2];return(_tmp97E[1]=Cyc_PP_nest(2,((_tmp97F[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp97F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97F,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp97E[0]=Cyc_PP_text(((_tmp980="default: ",
_tag_dyneither(_tmp980,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp97E,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp987;
const char*_tmp986;struct Cyc_PP_Doc*_tmp985[2];struct Cyc_PP_Doc*_tmp984[4];return(
_tmp984[3]=Cyc_PP_nest(2,((_tmp985[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp985[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp985,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp984[2]=Cyc_PP_text(((_tmp986=": ",_tag_dyneither(_tmp986,sizeof(
char),3)))),((_tmp984[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp984[0]=Cyc_PP_text(((
_tmp987="case ",_tag_dyneither(_tmp987,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp984,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_PP_Doc*_tmp98D[2];struct
Cyc_PP_Doc*_tmp98C[6];return(_tmp98C[5]=Cyc_PP_nest(2,((_tmp98D[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp98D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp98D,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp98C[4]=Cyc_PP_text(((_tmp98E=": ",
_tag_dyneither(_tmp98E,sizeof(char),3)))),((_tmp98C[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp98C[2]=Cyc_PP_text(((
_tmp98F=" && ",_tag_dyneither(_tmp98F,sizeof(char),5)))),((_tmp98C[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp98C[0]=Cyc_PP_text(((_tmp990="case ",_tag_dyneither(_tmp990,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp98C,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp991;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp991="",_tag_dyneither(_tmp991,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp994;struct Cyc_PP_Doc*_tmp993[3];return(_tmp993[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp993[1]=Cyc_PP_text(((_tmp994=" = ",
_tag_dyneither(_tmp994,sizeof(char),4)))),((_tmp993[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp993,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp995;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp995=",",_tag_dyneither(_tmp995,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp996;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp996=",",_tag_dyneither(_tmp996,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp4A9;enum Cyc_Absyn_Scope _tmp4AA;struct _tuple0*_tmp4AB;struct Cyc_Absyn_Tqual
_tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_List_List*_tmp4AF;
struct Cyc_Absyn_Vardecl*_tmp4A8=vd;_tmp4A9=*_tmp4A8;_tmp4AA=_tmp4A9.sc;_tmp4AB=
_tmp4A9.name;_tmp4AC=_tmp4A9.tq;_tmp4AD=_tmp4A9.type;_tmp4AE=_tmp4A9.initializer;
_tmp4AF=_tmp4A9.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4AB);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4AF);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL276:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL277:{void*_tmp4B0=Cyc_Tcutil_compress(
_tmp4AD);struct Cyc_Absyn_FnInfo _tmp4B2;struct Cyc_List_List*_tmp4B3;_LL27A: {
struct Cyc_Absyn_FnType_struct*_tmp4B1=(struct Cyc_Absyn_FnType_struct*)_tmp4B0;
if(_tmp4B1->tag != 10)goto _LL27C;else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B2.attributes;}}
_LL27B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4B3);goto _LL279;_LL27C:;_LL27D:
beforenamedoc=Cyc_PP_nil_doc();goto _LL279;_LL279:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL27E: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL27F: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9A3;struct Cyc_PP_Doc*_tmp9A2[2];struct Cyc_PP_Doc*_tmp9A1[2];const char*
_tmp9A0;const char*_tmp99F;struct Cyc_PP_Doc*_tmp99E[5];s=((_tmp99E[4]=Cyc_PP_text(((
_tmp99F=";",_tag_dyneither(_tmp99F,sizeof(char),2)))),((_tmp99E[3]=_tmp4AE == 0?
Cyc_PP_nil_doc():((_tmp9A1[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4AE),((
_tmp9A1[0]=Cyc_PP_text(((_tmp9A0=" = ",_tag_dyneither(_tmp9A0,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp99E[2]=
Cyc_Absynpp_tqtd2doc(_tmp4AC,_tmp4AD,((_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((
_tmp9A3->v=((_tmp9A2[1]=sn,((_tmp9A2[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9A2,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9A3))))),((_tmp99E[1]=Cyc_Absynpp_scope2doc(
_tmp4AA),((_tmp99E[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4BB;
struct _tuple17 _tmp4BA=*x;_tmp4BB=_tmp4BA.f2;return Cyc_Absynpp_qvar2doc(_tmp4BB);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4BC=d->r;
struct Cyc_Absyn_Fndecl*_tmp4BE;struct Cyc_Absyn_Aggrdecl*_tmp4C0;struct Cyc_Absyn_Vardecl*
_tmp4C2;struct Cyc_Absyn_Tvar*_tmp4C4;struct Cyc_Absyn_Vardecl*_tmp4C5;int _tmp4C6;
struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Tvar*
_tmp4CA;struct Cyc_Absyn_Vardecl*_tmp4CB;struct Cyc_Absyn_Datatypedecl*_tmp4CD;
struct Cyc_Absyn_Datatypedecl _tmp4CE;enum Cyc_Absyn_Scope _tmp4CF;struct _tuple0*
_tmp4D0;struct Cyc_List_List*_tmp4D1;struct Cyc_Core_Opt*_tmp4D2;int _tmp4D3;struct
Cyc_Absyn_Pat*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_List_List*_tmp4D8;
struct Cyc_Absyn_Enumdecl*_tmp4DA;struct Cyc_Absyn_Enumdecl _tmp4DB;enum Cyc_Absyn_Scope
_tmp4DC;struct _tuple0*_tmp4DD;struct Cyc_Core_Opt*_tmp4DE;struct Cyc_Absyn_Typedefdecl*
_tmp4E0;struct _dyneither_ptr*_tmp4E2;struct Cyc_List_List*_tmp4E3;struct _tuple0*
_tmp4E5;struct Cyc_List_List*_tmp4E6;struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*
_tmp4EA;struct Cyc_List_List*_tmp4EB;_LL282: {struct Cyc_Absyn_Fn_d_struct*_tmp4BD=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4BC;if(_tmp4BD->tag != 1)goto _LL284;else{_tmp4BE=
_tmp4BD->f1;}}_LL283: {struct Cyc_Absyn_FnType_struct _tmp9A9;struct Cyc_Absyn_FnInfo
_tmp9A8;struct Cyc_Absyn_FnType_struct*_tmp9A7;void*t=(void*)((_tmp9A7=_cycalloc(
sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A9.tag=10,((_tmp9A9.f1=((_tmp9A8.tvars=
_tmp4BE->tvs,((_tmp9A8.effect=_tmp4BE->effect,((_tmp9A8.ret_typ=_tmp4BE->ret_type,((
_tmp9A8.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4BE->args),((
_tmp9A8.c_varargs=_tmp4BE->c_varargs,((_tmp9A8.cyc_varargs=_tmp4BE->cyc_varargs,((
_tmp9A8.rgn_po=_tmp4BE->rgn_po,((_tmp9A8.attributes=0,_tmp9A8)))))))))))))))),
_tmp9A9)))),_tmp9A7))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4BE->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4BE->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2A2:{const char*_tmp9AA;inlinedoc=Cyc_PP_text(((_tmp9AA="inline ",
_tag_dyneither(_tmp9AA,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2A3:{
const char*_tmp9AB;inlinedoc=Cyc_PP_text(((_tmp9AB="__inline ",_tag_dyneither(
_tmp9AB,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4BE->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2A5: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2A6: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4BE->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4BE->name);struct Cyc_PP_Doc*_tmp9AE[2];struct Cyc_Core_Opt*_tmp9AD;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9AD=_cycalloc(
sizeof(*_tmp9AD)),((_tmp9AD->v=((_tmp9AE[1]=namedoc,((_tmp9AE[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp9AE,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9AD)))));
struct Cyc_PP_Doc*_tmp9B1[2];struct Cyc_PP_Doc*_tmp9B0[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9B0[4]=Cyc_Absynpp_rb(),((_tmp9B0[3]=Cyc_PP_line_doc(),((_tmp9B0[2]=Cyc_PP_nest(
2,((_tmp9B1[1]=Cyc_Absynpp_stmt2doc(_tmp4BE->body,0),((_tmp9B1[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B0[1]=
Cyc_Absynpp_lb(),((_tmp9B0[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9B0,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9B2[4];s=((
_tmp9B2[3]=bodydoc,((_tmp9B2[2]=tqtddoc,((_tmp9B2[1]=scopedoc,((_tmp9B2[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9B2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2A8:{struct Cyc_PP_Doc*
_tmp9B3[2];s=((_tmp9B3[1]=s,((_tmp9B3[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9B3,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2A9: break;}goto
_LL281;};};}_LL284: {struct Cyc_Absyn_Aggr_d_struct*_tmp4BF=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4BC;if(_tmp4BF->tag != 6)goto _LL286;else{_tmp4C0=_tmp4BF->f1;}}_LL285: if(
_tmp4C0->impl == 0){const char*_tmp9B6;struct Cyc_PP_Doc*_tmp9B5[5];s=((_tmp9B5[4]=
Cyc_PP_text(((_tmp9B6=";",_tag_dyneither(_tmp9B6,sizeof(char),2)))),((_tmp9B5[3]=
Cyc_Absynpp_ktvars2doc(_tmp4C0->tvs),((_tmp9B5[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4C0->name),((_tmp9B5[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4C0->kind),((_tmp9B5[0]=
Cyc_Absynpp_scope2doc(_tmp4C0->sc),Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9C0[2];const char*
_tmp9BF;struct Cyc_PP_Doc*_tmp9BE[2];const char*_tmp9BD;struct Cyc_PP_Doc*_tmp9BC[
13];s=((_tmp9BC[12]=Cyc_PP_text(((_tmp9BD=";",_tag_dyneither(_tmp9BD,sizeof(char),
2)))),((_tmp9BC[11]=Cyc_Absynpp_atts2doc(_tmp4C0->attributes),((_tmp9BC[10]=Cyc_Absynpp_rb(),((
_tmp9BC[9]=Cyc_PP_line_doc(),((_tmp9BC[8]=Cyc_PP_nest(2,((_tmp9BE[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->fields),((_tmp9BE[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9BC[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9C0[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4C0->impl))->rgn_po),((_tmp9C0[0]=Cyc_PP_text(((_tmp9BF=":",
_tag_dyneither(_tmp9BF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9C0,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9BC[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->exist_vars),((_tmp9BC[5]=
Cyc_Absynpp_lb(),((_tmp9BC[4]=Cyc_PP_blank_doc(),((_tmp9BC[3]=Cyc_Absynpp_ktvars2doc(
_tmp4C0->tvs),((_tmp9BC[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4C0->name),((_tmp9BC[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4C0->kind),((_tmp9BC[0]=Cyc_Absynpp_scope2doc(
_tmp4C0->sc),Cyc_PP_cat(_tag_dyneither(_tmp9BC,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL281;_LL286: {struct Cyc_Absyn_Var_d_struct*_tmp4C1=(struct Cyc_Absyn_Var_d_struct*)
_tmp4BC;if(_tmp4C1->tag != 0)goto _LL288;else{_tmp4C2=_tmp4C1->f1;}}_LL287: s=Cyc_Absynpp_vardecl2doc(
_tmp4C2);goto _LL281;_LL288: {struct Cyc_Absyn_Region_d_struct*_tmp4C3=(struct Cyc_Absyn_Region_d_struct*)
_tmp4BC;if(_tmp4C3->tag != 4)goto _LL28A;else{_tmp4C4=_tmp4C3->f1;_tmp4C5=_tmp4C3->f2;
_tmp4C6=_tmp4C3->f3;_tmp4C7=_tmp4C3->f4;}}_LL289:{const char*_tmp9D3;const char*
_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_PP_Doc*_tmp9CF[3];const
char*_tmp9CE;const char*_tmp9CD;const char*_tmp9CC;struct Cyc_PP_Doc*_tmp9CB[8];s=((
_tmp9CB[7]=Cyc_PP_text(((_tmp9CC=";",_tag_dyneither(_tmp9CC,sizeof(char),2)))),((
_tmp9CB[6]=_tmp4C7 != 0?(_tmp9CF[2]=Cyc_PP_text(((_tmp9CE=")",_tag_dyneither(
_tmp9CE,sizeof(char),2)))),((_tmp9CF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4C7),((_tmp9CF[0]=Cyc_PP_text(((_tmp9CD=" = open(",_tag_dyneither(_tmp9CD,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9CF,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9CB[5]=_tmp4C6?Cyc_PP_text(((_tmp9D0=" @resetable",
_tag_dyneither(_tmp9D0,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9CB[4]=Cyc_Absynpp_qvar2doc(
_tmp4C5->name),((_tmp9CB[3]=Cyc_PP_text(((_tmp9D1=">",_tag_dyneither(_tmp9D1,
sizeof(char),2)))),((_tmp9CB[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C4)),((
_tmp9CB[1]=Cyc_PP_text(((_tmp9D2="<",_tag_dyneither(_tmp9D2,sizeof(char),2)))),((
_tmp9CB[0]=Cyc_PP_text(((_tmp9D3="region",_tag_dyneither(_tmp9D3,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9CB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL281;_LL28A: {struct Cyc_Absyn_Alias_d_struct*_tmp4C8=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4BC;if(_tmp4C8->tag != 5)goto _LL28C;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C8->f2;
_tmp4CB=_tmp4C8->f3;}}_LL28B:{const char*_tmp9DE;const char*_tmp9DD;const char*
_tmp9DC;const char*_tmp9DB;const char*_tmp9DA;struct Cyc_PP_Doc*_tmp9D9[8];s=((
_tmp9D9[7]=Cyc_PP_text(((_tmp9DA=";",_tag_dyneither(_tmp9DA,sizeof(char),2)))),((
_tmp9D9[6]=Cyc_Absynpp_exp2doc(_tmp4C9),((_tmp9D9[5]=Cyc_PP_text(((_tmp9DB=" = ",
_tag_dyneither(_tmp9DB,sizeof(char),4)))),((_tmp9D9[4]=Cyc_Absynpp_qvar2doc(
_tmp4CB->name),((_tmp9D9[3]=Cyc_PP_text(((_tmp9DC="> ",_tag_dyneither(_tmp9DC,
sizeof(char),3)))),((_tmp9D9[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CA)),((
_tmp9D9[1]=Cyc_PP_text(((_tmp9DD=" <",_tag_dyneither(_tmp9DD,sizeof(char),3)))),((
_tmp9D9[0]=Cyc_PP_text(((_tmp9DE="alias",_tag_dyneither(_tmp9DE,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL281;_LL28C: {struct Cyc_Absyn_Datatype_d_struct*_tmp4CC=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4BC;if(_tmp4CC->tag != 7)goto _LL28E;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=*_tmp4CD;
_tmp4CF=_tmp4CE.sc;_tmp4D0=_tmp4CE.name;_tmp4D1=_tmp4CE.tvs;_tmp4D2=_tmp4CE.fields;
_tmp4D3=_tmp4CE.is_extensible;}}_LL28D: if(_tmp4D2 == 0){const char*_tmp9E5;const
char*_tmp9E4;const char*_tmp9E3;struct Cyc_PP_Doc*_tmp9E2[6];s=((_tmp9E2[5]=Cyc_PP_text(((
_tmp9E3=";",_tag_dyneither(_tmp9E3,sizeof(char),2)))),((_tmp9E2[4]=Cyc_Absynpp_ktvars2doc(
_tmp4D1),((_tmp9E2[3]=_tmp4D3?Cyc_Absynpp_qvar2bolddoc(_tmp4D0): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4D0),((_tmp9E2[2]=Cyc_PP_text(((_tmp9E4="datatype ",_tag_dyneither(_tmp9E4,
sizeof(char),10)))),((_tmp9E2[1]=_tmp4D3?Cyc_PP_text(((_tmp9E5="@extensible",
_tag_dyneither(_tmp9E5,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9E2[0]=Cyc_Absynpp_scope2doc(
_tmp4CF),Cyc_PP_cat(_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9EE;const char*_tmp9ED;struct Cyc_PP_Doc*_tmp9EC[2];const char*
_tmp9EB;struct Cyc_PP_Doc*_tmp9EA[11];s=((_tmp9EA[10]=Cyc_PP_text(((_tmp9EB=";",
_tag_dyneither(_tmp9EB,sizeof(char),2)))),((_tmp9EA[9]=Cyc_Absynpp_rb(),((
_tmp9EA[8]=Cyc_PP_line_doc(),((_tmp9EA[7]=Cyc_PP_nest(2,((_tmp9EC[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4D2->v),((_tmp9EC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EA[6]=Cyc_Absynpp_lb(),((
_tmp9EA[5]=Cyc_PP_blank_doc(),((_tmp9EA[4]=Cyc_Absynpp_ktvars2doc(_tmp4D1),((
_tmp9EA[3]=_tmp4D3?Cyc_Absynpp_qvar2bolddoc(_tmp4D0): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4D0),((_tmp9EA[2]=Cyc_PP_text(((_tmp9ED="datatype ",_tag_dyneither(_tmp9ED,
sizeof(char),10)))),((_tmp9EA[1]=_tmp4D3?Cyc_PP_text(((_tmp9EE="@extensible ",
_tag_dyneither(_tmp9EE,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9EA[0]=Cyc_Absynpp_scope2doc(
_tmp4CF),Cyc_PP_cat(_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL281;_LL28E: {struct Cyc_Absyn_Let_d_struct*_tmp4D4=(struct Cyc_Absyn_Let_d_struct*)
_tmp4BC;if(_tmp4D4->tag != 2)goto _LL290;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f3;}}
_LL28F:{const char*_tmp9F5;const char*_tmp9F4;const char*_tmp9F3;struct Cyc_PP_Doc*
_tmp9F2[5];s=((_tmp9F2[4]=Cyc_PP_text(((_tmp9F3=";",_tag_dyneither(_tmp9F3,
sizeof(char),2)))),((_tmp9F2[3]=Cyc_Absynpp_exp2doc(_tmp4D6),((_tmp9F2[2]=Cyc_PP_text(((
_tmp9F4=" = ",_tag_dyneither(_tmp9F4,sizeof(char),4)))),((_tmp9F2[1]=Cyc_Absynpp_pat2doc(
_tmp4D5),((_tmp9F2[0]=Cyc_PP_text(((_tmp9F5="let ",_tag_dyneither(_tmp9F5,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL281;_LL290: {struct Cyc_Absyn_Letv_d_struct*_tmp4D7=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4BC;if(_tmp4D7->tag != 3)goto _LL292;else{_tmp4D8=_tmp4D7->f1;}}_LL291:{const
char*_tmp9FA;const char*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[3];s=((_tmp9F8[2]=Cyc_PP_text(((
_tmp9F9=";",_tag_dyneither(_tmp9F9,sizeof(char),2)))),((_tmp9F8[1]=Cyc_Absynpp_ids2doc(
_tmp4D8),((_tmp9F8[0]=Cyc_PP_text(((_tmp9FA="let ",_tag_dyneither(_tmp9FA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL281;_LL292: {struct Cyc_Absyn_Enum_d_struct*_tmp4D9=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4BC;if(_tmp4D9->tag != 8)goto _LL294;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=*_tmp4DA;
_tmp4DC=_tmp4DB.sc;_tmp4DD=_tmp4DB.name;_tmp4DE=_tmp4DB.fields;}}_LL293: if(
_tmp4DE == 0){const char*_tmp9FF;const char*_tmp9FE;struct Cyc_PP_Doc*_tmp9FD[4];s=((
_tmp9FD[3]=Cyc_PP_text(((_tmp9FE=";",_tag_dyneither(_tmp9FE,sizeof(char),2)))),((
_tmp9FD[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4DD),((_tmp9FD[1]=Cyc_PP_text(((
_tmp9FF="enum ",_tag_dyneither(_tmp9FF,sizeof(char),6)))),((_tmp9FD[0]=Cyc_Absynpp_scope2doc(
_tmp4DC),Cyc_PP_cat(_tag_dyneither(_tmp9FD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmpA06;struct Cyc_PP_Doc*_tmpA05[2];const char*_tmpA04;struct Cyc_PP_Doc*
_tmpA03[9];s=((_tmpA03[8]=Cyc_PP_text(((_tmpA04=";",_tag_dyneither(_tmpA04,
sizeof(char),2)))),((_tmpA03[7]=Cyc_Absynpp_rb(),((_tmpA03[6]=Cyc_PP_line_doc(),((
_tmpA03[5]=Cyc_PP_nest(2,((_tmpA05[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4DE->v),((_tmpA05[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA05,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA03[4]=Cyc_Absynpp_lb(),((_tmpA03[3]=
Cyc_PP_blank_doc(),((_tmpA03[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4DD),((_tmpA03[1]=
Cyc_PP_text(((_tmpA06="enum ",_tag_dyneither(_tmpA06,sizeof(char),6)))),((
_tmpA03[0]=Cyc_Absynpp_scope2doc(_tmp4DC),Cyc_PP_cat(_tag_dyneither(_tmpA03,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL281;_LL294: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4DF=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4BC;if(_tmp4DF->tag != 9)goto
_LL296;else{_tmp4E0=_tmp4DF->f1;}}_LL295: {void*t;if(_tmp4E0->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4E0->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4E0->kind,0);}{const char*_tmpA10;struct Cyc_Core_Opt*_tmpA0F;struct Cyc_PP_Doc*
_tmpA0E[2];const char*_tmpA0D;struct Cyc_PP_Doc*_tmpA0C[4];s=((_tmpA0C[3]=Cyc_PP_text(((
_tmpA0D=";",_tag_dyneither(_tmpA0D,sizeof(char),2)))),((_tmpA0C[2]=Cyc_Absynpp_atts2doc(
_tmp4E0->atts),((_tmpA0C[1]=Cyc_Absynpp_tqtd2doc(_tmp4E0->tq,t,((_tmpA0F=
_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F->v=((_tmpA0E[1]=Cyc_Absynpp_tvars2doc(
_tmp4E0->tvs),((_tmpA0E[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4E0->name),Cyc_PP_cat(
_tag_dyneither(_tmpA0E,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA0F))))),((_tmpA0C[
0]=Cyc_PP_text(((_tmpA10="typedef ",_tag_dyneither(_tmpA10,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL281;}_LL296: {struct Cyc_Absyn_Namespace_d_struct*_tmp4E1=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4BC;if(_tmp4E1->tag != 10)goto _LL298;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;}}
_LL297: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4E2);{
const char*_tmpA15;const char*_tmpA14;struct Cyc_PP_Doc*_tmpA13[8];s=((_tmpA13[7]=
Cyc_Absynpp_rb(),((_tmpA13[6]=Cyc_PP_line_doc(),((_tmpA13[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA14="",_tag_dyneither(_tmpA14,
sizeof(char),1))),_tmp4E3),((_tmpA13[4]=Cyc_PP_line_doc(),((_tmpA13[3]=Cyc_Absynpp_lb(),((
_tmpA13[2]=Cyc_PP_blank_doc(),((_tmpA13[1]=Cyc_PP_textptr(_tmp4E2),((_tmpA13[0]=
Cyc_PP_text(((_tmpA15="namespace ",_tag_dyneither(_tmpA15,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA13,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL281;
_LL298: {struct Cyc_Absyn_Using_d_struct*_tmp4E4=(struct Cyc_Absyn_Using_d_struct*)
_tmp4BC;if(_tmp4E4->tag != 11)goto _LL29A;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E4->f2;}}
_LL299: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA1A;const char*_tmpA19;
struct Cyc_PP_Doc*_tmpA18[8];s=((_tmpA18[7]=Cyc_Absynpp_rb(),((_tmpA18[6]=Cyc_PP_line_doc(),((
_tmpA18[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA19="",_tag_dyneither(_tmpA19,sizeof(char),1))),_tmp4E6),((_tmpA18[4]=Cyc_PP_line_doc(),((
_tmpA18[3]=Cyc_Absynpp_lb(),((_tmpA18[2]=Cyc_PP_blank_doc(),((_tmpA18[1]=Cyc_Absynpp_qvar2doc(
_tmp4E5),((_tmpA18[0]=Cyc_PP_text(((_tmpA1A="using ",_tag_dyneither(_tmpA1A,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA25;const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;
const char*_tmpA21;struct Cyc_PP_Doc*_tmpA20[11];s=((_tmpA20[10]=Cyc_PP_text(((
_tmpA21=" */",_tag_dyneither(_tmpA21,sizeof(char),4)))),((_tmpA20[9]=Cyc_Absynpp_rb(),((
_tmpA20[8]=Cyc_PP_text(((_tmpA22="/* ",_tag_dyneither(_tmpA22,sizeof(char),4)))),((
_tmpA20[7]=Cyc_PP_line_doc(),((_tmpA20[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA23="",_tag_dyneither(_tmpA23,sizeof(char),1))),
_tmp4E6),((_tmpA20[5]=Cyc_PP_line_doc(),((_tmpA20[4]=Cyc_PP_text(((_tmpA24=" */",
_tag_dyneither(_tmpA24,sizeof(char),4)))),((_tmpA20[3]=Cyc_Absynpp_lb(),((
_tmpA20[2]=Cyc_PP_blank_doc(),((_tmpA20[1]=Cyc_Absynpp_qvar2doc(_tmp4E5),((
_tmpA20[0]=Cyc_PP_text(((_tmpA25="/* using ",_tag_dyneither(_tmpA25,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA20,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL281;_LL29A: {struct Cyc_Absyn_ExternC_d_struct*_tmp4E7=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4BC;if(_tmp4E7->tag != 12)goto _LL29C;else{_tmp4E8=_tmp4E7->f1;}}_LL29B: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA2A;const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[6];s=((_tmpA28[5]=
Cyc_Absynpp_rb(),((_tmpA28[4]=Cyc_PP_line_doc(),((_tmpA28[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA29="",_tag_dyneither(_tmpA29,
sizeof(char),1))),_tmp4E8),((_tmpA28[2]=Cyc_PP_line_doc(),((_tmpA28[1]=Cyc_Absynpp_lb(),((
_tmpA28[0]=Cyc_PP_text(((_tmpA2A="extern \"C\" ",_tag_dyneither(_tmpA2A,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA35;const char*_tmpA34;const char*_tmpA33;const char*_tmpA32;
const char*_tmpA31;struct Cyc_PP_Doc*_tmpA30[9];s=((_tmpA30[8]=Cyc_PP_text(((
_tmpA31=" */",_tag_dyneither(_tmpA31,sizeof(char),4)))),((_tmpA30[7]=Cyc_Absynpp_rb(),((
_tmpA30[6]=Cyc_PP_text(((_tmpA32="/* ",_tag_dyneither(_tmpA32,sizeof(char),4)))),((
_tmpA30[5]=Cyc_PP_line_doc(),((_tmpA30[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA33="",_tag_dyneither(_tmpA33,sizeof(char),1))),
_tmp4E8),((_tmpA30[3]=Cyc_PP_line_doc(),((_tmpA30[2]=Cyc_PP_text(((_tmpA34=" */",
_tag_dyneither(_tmpA34,sizeof(char),4)))),((_tmpA30[1]=Cyc_Absynpp_lb(),((
_tmpA30[0]=Cyc_PP_text(((_tmpA35="/* extern \"C\" ",_tag_dyneither(_tmpA35,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA30,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL281;_LL29C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4E9=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4BC;if(_tmp4E9->tag != 13)
goto _LL29E;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;}}_LL29D: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4EB != 0){const char*_tmpA3A;const char*_tmpA39;
struct Cyc_PP_Doc*_tmpA38[7];exs_doc=((_tmpA38[6]=Cyc_Absynpp_rb(),((_tmpA38[5]=
Cyc_PP_line_doc(),((_tmpA38[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA39=",",_tag_dyneither(_tmpA39,sizeof(char),2))),
_tmp4EB),((_tmpA38[3]=Cyc_PP_line_doc(),((_tmpA38[2]=Cyc_Absynpp_lb(),((_tmpA38[
1]=Cyc_PP_text(((_tmpA3A=" export ",_tag_dyneither(_tmpA3A,sizeof(char),9)))),((
_tmpA38[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA38,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA3F;const char*
_tmpA3E;struct Cyc_PP_Doc*_tmpA3D[6];s=((_tmpA3D[5]=exs_doc,((_tmpA3D[4]=Cyc_PP_line_doc(),((
_tmpA3D[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA3E="",_tag_dyneither(_tmpA3E,sizeof(char),1))),_tmp4EA),((_tmpA3D[2]=Cyc_PP_line_doc(),((
_tmpA3D[1]=Cyc_Absynpp_lb(),((_tmpA3D[0]=Cyc_PP_text(((_tmpA3F="extern \"C include\" ",
_tag_dyneither(_tmpA3F,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA3D,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA4A;const char*
_tmpA49;const char*_tmpA48;const char*_tmpA47;const char*_tmpA46;struct Cyc_PP_Doc*
_tmpA45[9];s=((_tmpA45[8]=Cyc_PP_text(((_tmpA46=" */",_tag_dyneither(_tmpA46,
sizeof(char),4)))),((_tmpA45[7]=Cyc_Absynpp_rb(),((_tmpA45[6]=Cyc_PP_text(((
_tmpA47="/* ",_tag_dyneither(_tmpA47,sizeof(char),4)))),((_tmpA45[5]=Cyc_PP_line_doc(),((
_tmpA45[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA48="",_tag_dyneither(_tmpA48,sizeof(char),1))),_tmp4EA),((_tmpA45[3]=Cyc_PP_line_doc(),((
_tmpA45[2]=Cyc_PP_text(((_tmpA49=" */",_tag_dyneither(_tmpA49,sizeof(char),4)))),((
_tmpA45[1]=Cyc_Absynpp_lb(),((_tmpA45[0]=Cyc_PP_text(((_tmpA4A="/* extern \"C include\" ",
_tag_dyneither(_tmpA4A,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA45,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL281;_LL29E: {struct Cyc_Absyn_Porton_d_struct*
_tmp4EC=(struct Cyc_Absyn_Porton_d_struct*)_tmp4BC;if(_tmp4EC->tag != 14)goto
_LL2A0;}_LL29F:{const char*_tmpA4D;struct Cyc_PP_Doc*_tmpA4C[2];s=((_tmpA4C[1]=Cyc_Absynpp_lb(),((
_tmpA4C[0]=Cyc_PP_text(((_tmpA4D="__cyclone_port_on__;",_tag_dyneither(_tmpA4D,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA4C,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL281;_LL2A0: {struct Cyc_Absyn_Portoff_d_struct*_tmp4ED=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4BC;if(_tmp4ED->tag != 15)goto _LL281;}_LL2A1:{const char*_tmpA50;struct Cyc_PP_Doc*
_tmpA4F[2];s=((_tmpA4F[1]=Cyc_Absynpp_lb(),((_tmpA4F[0]=Cyc_PP_text(((_tmpA50="__cyclone_port_off__;",
_tag_dyneither(_tmpA50,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA4F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL281;_LL281:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2AB: {const char*_tmpA51;return Cyc_PP_text(((
_tmpA51="static ",_tag_dyneither(_tmpA51,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL2AC: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2AD: {const char*_tmpA52;
return Cyc_PP_text(((_tmpA52="extern ",_tag_dyneither(_tmpA52,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL2AE: {const char*_tmpA53;return Cyc_PP_text(((_tmpA53="extern \"C\" ",
_tag_dyneither(_tmpA53,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2AF: {
const char*_tmpA54;return Cyc_PP_text(((_tmpA54="abstract ",_tag_dyneither(_tmpA54,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2B0: {const char*_tmpA55;return Cyc_PP_text(((
_tmpA55="register ",_tag_dyneither(_tmpA55,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp555=t;struct
Cyc_Absyn_Tvar*_tmp557;struct Cyc_List_List*_tmp559;_LL2B3: {struct Cyc_Absyn_VarType_struct*
_tmp556=(struct Cyc_Absyn_VarType_struct*)_tmp555;if(_tmp556->tag != 2)goto _LL2B5;
else{_tmp557=_tmp556->f1;}}_LL2B4: return Cyc_Tcutil_is_temp_tvar(_tmp557);_LL2B5: {
struct Cyc_Absyn_JoinEff_struct*_tmp558=(struct Cyc_Absyn_JoinEff_struct*)_tmp555;
if(_tmp558->tag != 24)goto _LL2B7;else{_tmp559=_tmp558->f1;}}_LL2B6: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp559);_LL2B7:;_LL2B8: return 0;_LL2B2:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp55A=t;void**_tmp55E;void*
_tmp55F;_LL2BA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp55B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp55A;if(_tmp55B->tag != 13)goto _LL2BC;}_LL2BB: return 1;_LL2BC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp55C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp55A;if(_tmp55C->tag != 15)goto
_LL2BE;}_LL2BD: return 1;_LL2BE: {struct Cyc_Absyn_TypedefType_struct*_tmp55D=(
struct Cyc_Absyn_TypedefType_struct*)_tmp55A;if(_tmp55D->tag != 18)goto _LL2C0;
else{_tmp55E=_tmp55D->f4;if(_tmp55E == 0)goto _LL2C0;_tmp55F=*_tmp55E;}}_LL2BF:
return Cyc_Absynpp_is_anon_aggrtype(_tmp55F);_LL2C0:;_LL2C1: return 0;_LL2B9:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple16 _tmpA56;struct _tuple16 _tmp561=(_tmpA56.f1=(void*)tms->hd,((
_tmpA56.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA56)));
void*_tmp562;void*_tmp564;_LL2C3: _tmp562=_tmp561.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp563=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp562;if(_tmp563->tag != 2)goto
_LL2C5;};_tmp564=_tmp561.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp565=(struct
Cyc_Absyn_Function_mod_struct*)_tmp564;if(_tmp565->tag != 3)goto _LL2C5;};_LL2C4: {
struct Cyc_List_List*_tmpA59;struct Cyc_List_List*_tmpA58;return(_tmpA58=
_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58->hd=(void*)tms->hd,((_tmpA58->tl=((
_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA59->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA59)))))),_tmpA58)))));}
_LL2C5:;_LL2C6: {struct Cyc_List_List*_tmpA5A;return(_tmpA5A=_region_malloc(r,
sizeof(*_tmpA5A)),((_tmpA5A->hd=atts,((_tmpA5A->tl=tms,_tmpA5A)))));}_LL2C2:;}
else{struct Cyc_List_List*_tmpA5B;return(_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((
_tmpA5B->hd=atts,((_tmpA5B->tl=tms,_tmpA5B)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp56A=t;struct Cyc_Absyn_ArrayInfo
_tmp56C;void*_tmp56D;struct Cyc_Absyn_Tqual _tmp56E;struct Cyc_Absyn_Exp*_tmp56F;
union Cyc_Absyn_Constraint*_tmp570;struct Cyc_Position_Segment*_tmp571;struct Cyc_Absyn_PtrInfo
_tmp573;void*_tmp574;struct Cyc_Absyn_Tqual _tmp575;struct Cyc_Absyn_PtrAtts _tmp576;
struct Cyc_Absyn_FnInfo _tmp578;struct Cyc_List_List*_tmp579;struct Cyc_Core_Opt*
_tmp57A;void*_tmp57B;struct Cyc_List_List*_tmp57C;int _tmp57D;struct Cyc_Absyn_VarargInfo*
_tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_List_List*_tmp580;struct Cyc_Core_Opt*
_tmp582;struct Cyc_Core_Opt*_tmp583;int _tmp584;struct _tuple0*_tmp586;struct Cyc_List_List*
_tmp587;void**_tmp588;_LL2C8: {struct Cyc_Absyn_ArrayType_struct*_tmp56B=(struct
Cyc_Absyn_ArrayType_struct*)_tmp56A;if(_tmp56B->tag != 9)goto _LL2CA;else{_tmp56C=
_tmp56B->f1;_tmp56D=_tmp56C.elt_type;_tmp56E=_tmp56C.tq;_tmp56F=_tmp56C.num_elts;
_tmp570=_tmp56C.zero_term;_tmp571=_tmp56C.zt_loc;}}_LL2C9: {struct Cyc_Absyn_Tqual
_tmp58A;void*_tmp58B;struct Cyc_List_List*_tmp58C;struct _tuple12 _tmp589=Cyc_Absynpp_to_tms(
r,_tmp56E,_tmp56D);_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;{
void*tm;if(_tmp56F == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA5E;struct Cyc_Absyn_Carray_mod_struct*
_tmpA5D;tm=(void*)((_tmpA5D=_region_malloc(r,sizeof(*_tmpA5D)),((_tmpA5D[0]=((
_tmpA5E.tag=0,((_tmpA5E.f1=_tmp570,((_tmpA5E.f2=_tmp571,_tmpA5E)))))),_tmpA5D))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA61;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA60;tm=(void*)((_tmpA60=_region_malloc(r,sizeof(*_tmpA60)),((_tmpA60[0]=((
_tmpA61.tag=1,((_tmpA61.f1=(struct Cyc_Absyn_Exp*)_tmp56F,((_tmpA61.f2=_tmp570,((
_tmpA61.f3=_tmp571,_tmpA61)))))))),_tmpA60))));}{struct Cyc_List_List*_tmpA64;
struct _tuple12 _tmpA63;return(_tmpA63.f1=_tmp58A,((_tmpA63.f2=_tmp58B,((_tmpA63.f3=((
_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64->hd=tm,((_tmpA64->tl=
_tmp58C,_tmpA64)))))),_tmpA63)))));};};}_LL2CA: {struct Cyc_Absyn_PointerType_struct*
_tmp572=(struct Cyc_Absyn_PointerType_struct*)_tmp56A;if(_tmp572->tag != 5)goto
_LL2CC;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.elt_typ;_tmp575=_tmp573.elt_tq;
_tmp576=_tmp573.ptr_atts;}}_LL2CB: {struct Cyc_Absyn_Tqual _tmp594;void*_tmp595;
struct Cyc_List_List*_tmp596;struct _tuple12 _tmp593=Cyc_Absynpp_to_tms(r,_tmp575,
_tmp574);_tmp594=_tmp593.f1;_tmp595=_tmp593.f2;_tmp596=_tmp593.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA6A;struct Cyc_Absyn_Pointer_mod_struct _tmpA69;struct Cyc_List_List*_tmpA68;
_tmp596=((_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->hd=(void*)((
_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA69.tag=2,((
_tmpA69.f1=_tmp576,((_tmpA69.f2=tq,_tmpA69)))))),_tmpA6A)))),((_tmpA68->tl=
_tmp596,_tmpA68))))));}{struct _tuple12 _tmpA6B;return(_tmpA6B.f1=_tmp594,((
_tmpA6B.f2=_tmp595,((_tmpA6B.f3=_tmp596,_tmpA6B)))));};}_LL2CC: {struct Cyc_Absyn_FnType_struct*
_tmp577=(struct Cyc_Absyn_FnType_struct*)_tmp56A;if(_tmp577->tag != 10)goto _LL2CE;
else{_tmp578=_tmp577->f1;_tmp579=_tmp578.tvars;_tmp57A=_tmp578.effect;_tmp57B=
_tmp578.ret_typ;_tmp57C=_tmp578.args;_tmp57D=_tmp578.c_varargs;_tmp57E=_tmp578.cyc_varargs;
_tmp57F=_tmp578.rgn_po;_tmp580=_tmp578.attributes;}}_LL2CD: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp57A == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp57A->v)){
_tmp57A=0;_tmp579=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp579);}{struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;struct Cyc_List_List*_tmp59E;
struct _tuple12 _tmp59B=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp57B);
_tmp59C=_tmp59B.f1;_tmp59D=_tmp59B.f2;_tmp59E=_tmp59B.f3;{struct Cyc_List_List*
tms=_tmp59E;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2D4: if(
_tmp580 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA6E;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA6D;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA6D=_region_malloc(r,
sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=5,((_tmpA6E.f1=0,((_tmpA6E.f2=
_tmp580,_tmpA6E)))))),_tmpA6D)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA7D;struct Cyc_Absyn_WithTypes_struct*_tmpA7C;struct Cyc_Absyn_WithTypes_struct
_tmpA7B;struct Cyc_Absyn_Function_mod_struct _tmpA7A;struct Cyc_List_List*_tmpA79;
tms=((_tmpA79=_region_malloc(r,sizeof(*_tmpA79)),((_tmpA79->hd=(void*)((_tmpA7D=
_region_malloc(r,sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7A.tag=3,((_tmpA7A.f1=(
void*)((void*)((_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C[0]=((
_tmpA7B.tag=1,((_tmpA7B.f1=_tmp57C,((_tmpA7B.f2=_tmp57D,((_tmpA7B.f3=_tmp57E,((
_tmpA7B.f4=_tmp57A,((_tmpA7B.f5=_tmp57F,_tmpA7B)))))))))))),_tmpA7C))))),_tmpA7A)))),
_tmpA7D)))),((_tmpA79->tl=tms,_tmpA79))))));}break;case Cyc_Cyclone_Vc_c: _LL2D5:{
struct Cyc_Absyn_Function_mod_struct*_tmpA8C;struct Cyc_Absyn_WithTypes_struct*
_tmpA8B;struct Cyc_Absyn_WithTypes_struct _tmpA8A;struct Cyc_Absyn_Function_mod_struct
_tmpA89;struct Cyc_List_List*_tmpA88;tms=((_tmpA88=_region_malloc(r,sizeof(*
_tmpA88)),((_tmpA88->hd=(void*)((_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((
_tmpA8C[0]=((_tmpA89.tag=3,((_tmpA89.f1=(void*)((void*)((_tmpA8B=_region_malloc(
r,sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8A.tag=1,((_tmpA8A.f1=_tmp57C,((_tmpA8A.f2=
_tmp57D,((_tmpA8A.f3=_tmp57E,((_tmpA8A.f4=_tmp57A,((_tmpA8A.f5=_tmp57F,_tmpA8A)))))))))))),
_tmpA8B))))),_tmpA89)))),_tmpA8C)))),((_tmpA88->tl=tms,_tmpA88))))));}for(0;
_tmp580 != 0;_tmp580=_tmp580->tl){void*_tmp5AB=(void*)_tmp580->hd;_LL2D8: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5AC=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5AB;if(_tmp5AC->tag != 1)goto _LL2DA;}_LL2D9: goto _LL2DB;_LL2DA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5AD=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5AB;if(_tmp5AD->tag != 2)goto
_LL2DC;}_LL2DB: goto _LL2DD;_LL2DC: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5AE=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5AB;if(_tmp5AE->tag != 3)goto _LL2DE;}
_LL2DD:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA96;struct Cyc_List_List*
_tmpA95;struct Cyc_Absyn_Attributes_mod_struct _tmpA94;struct Cyc_List_List*_tmpA93;
tms=((_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93->hd=(void*)((_tmpA96=
_region_malloc(r,sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA94.tag=5,((_tmpA94.f1=0,((
_tmpA94.f2=((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->hd=(void*)_tmp580->hd,((
_tmpA95->tl=0,_tmpA95)))))),_tmpA94)))))),_tmpA96)))),((_tmpA93->tl=tms,_tmpA93))))));}
goto AfterAtts;_LL2DE:;_LL2DF: goto _LL2D7;_LL2D7:;}break;}AfterAtts: if(_tmp579 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA9C;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA9B;struct Cyc_List_List*_tmpA9A;tms=((_tmpA9A=_region_malloc(r,sizeof(*
_tmpA9A)),((_tmpA9A->hd=(void*)((_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((
_tmpA9C[0]=((_tmpA9B.tag=4,((_tmpA9B.f1=_tmp579,((_tmpA9B.f2=0,((_tmpA9B.f3=1,
_tmpA9B)))))))),_tmpA9C)))),((_tmpA9A->tl=tms,_tmpA9A))))));}{struct _tuple12
_tmpA9D;return(_tmpA9D.f1=_tmp59C,((_tmpA9D.f2=_tmp59D,((_tmpA9D.f3=tms,_tmpA9D)))));};};};
_LL2CE: {struct Cyc_Absyn_Evar_struct*_tmp581=(struct Cyc_Absyn_Evar_struct*)
_tmp56A;if(_tmp581->tag != 1)goto _LL2D0;else{_tmp582=_tmp581->f1;_tmp583=_tmp581->f2;
_tmp584=_tmp581->f3;}}_LL2CF: if(_tmp583 == 0){struct _tuple12 _tmpA9E;return(
_tmpA9E.f1=tq,((_tmpA9E.f2=t,((_tmpA9E.f3=0,_tmpA9E)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp583->v);}_LL2D0: {struct Cyc_Absyn_TypedefType_struct*_tmp585=(
struct Cyc_Absyn_TypedefType_struct*)_tmp56A;if(_tmp585->tag != 18)goto _LL2D2;
else{_tmp586=_tmp585->f1;_tmp587=_tmp585->f2;_tmp588=_tmp585->f4;}}_LL2D1: if((
_tmp588 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp588)){struct _tuple12 _tmpA9F;return(_tmpA9F.f1=tq,((_tmpA9F.f2=t,((_tmpA9F.f3=
0,_tmpA9F)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp588);}_LL2D2:;_LL2D3: {
struct _tuple12 _tmpAA0;return(_tmpAA0.f1=tq,((_tmpAA0.f2=t,((_tmpAA0.f3=0,_tmpAA0)))));}
_LL2C7:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5BA=t;struct Cyc_Core_Opt*_tmp5BC;struct Cyc_Core_Opt _tmp5BD;void*
_tmp5BE;struct Cyc_Absyn_PtrInfo _tmp5C0;void*_tmp5C1;_LL2E1: {struct Cyc_Absyn_Evar_struct*
_tmp5BB=(struct Cyc_Absyn_Evar_struct*)_tmp5BA;if(_tmp5BB->tag != 1)goto _LL2E3;
else{_tmp5BC=_tmp5BB->f2;if(_tmp5BC == 0)goto _LL2E3;_tmp5BD=*_tmp5BC;_tmp5BE=(
void*)_tmp5BD.v;}}_LL2E2: return Cyc_Absynpp_is_char_ptr(_tmp5BE);_LL2E3: {struct
Cyc_Absyn_PointerType_struct*_tmp5BF=(struct Cyc_Absyn_PointerType_struct*)
_tmp5BA;if(_tmp5BF->tag != 5)goto _LL2E5;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=_tmp5C0.elt_typ;}}
_LL2E4: L: {void*_tmp5C2=_tmp5C1;struct Cyc_Core_Opt*_tmp5C4;struct Cyc_Core_Opt
_tmp5C5;void*_tmp5C6;void**_tmp5C8;void*_tmp5C9;enum Cyc_Absyn_Size_of _tmp5CB;
_LL2E8: {struct Cyc_Absyn_Evar_struct*_tmp5C3=(struct Cyc_Absyn_Evar_struct*)
_tmp5C2;if(_tmp5C3->tag != 1)goto _LL2EA;else{_tmp5C4=_tmp5C3->f2;if(_tmp5C4 == 0)
goto _LL2EA;_tmp5C5=*_tmp5C4;_tmp5C6=(void*)_tmp5C5.v;}}_LL2E9: _tmp5C1=_tmp5C6;
goto L;_LL2EA: {struct Cyc_Absyn_TypedefType_struct*_tmp5C7=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5C2;if(_tmp5C7->tag != 18)goto _LL2EC;else{_tmp5C8=_tmp5C7->f4;if(_tmp5C8 == 0)
goto _LL2EC;_tmp5C9=*_tmp5C8;}}_LL2EB: _tmp5C1=_tmp5C9;goto L;_LL2EC: {struct Cyc_Absyn_IntType_struct*
_tmp5CA=(struct Cyc_Absyn_IntType_struct*)_tmp5C2;if(_tmp5CA->tag != 6)goto _LL2EE;
else{_tmp5CB=_tmp5CA->f2;if(_tmp5CB != Cyc_Absyn_Char_sz)goto _LL2EE;}}_LL2ED:
return 1;_LL2EE:;_LL2EF: return 0;_LL2E7:;}_LL2E5:;_LL2E6: return 0;_LL2E0:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5CC=_new_region("temp");struct
_RegionHandle*temp=& _tmp5CC;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5CE;
void*_tmp5CF;struct Cyc_List_List*_tmp5D0;struct _tuple12 _tmp5CD=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5CE=_tmp5CD.f1;_tmp5CF=_tmp5CD.f2;_tmp5D0=_tmp5CD.f3;_tmp5D0=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5D0);if(
_tmp5D0 == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpAA1[2];struct Cyc_PP_Doc*_tmp5D2=(
_tmpAA1[1]=Cyc_Absynpp_ntyp2doc(_tmp5CF),((_tmpAA1[0]=Cyc_Absynpp_tqual2doc(
_tmp5CE),Cyc_PP_cat(_tag_dyneither(_tmpAA1,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5D2;}else{const char*_tmpAA4;struct Cyc_PP_Doc*_tmpAA3[
4];struct Cyc_PP_Doc*_tmp5D5=(_tmpAA3[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5D0),((_tmpAA3[2]=
Cyc_PP_text(((_tmpAA4=" ",_tag_dyneither(_tmpAA4,sizeof(char),2)))),((_tmpAA3[1]=
Cyc_Absynpp_ntyp2doc(_tmp5CF),((_tmpAA3[0]=Cyc_Absynpp_tqual2doc(_tmp5CE),Cyc_PP_cat(
_tag_dyneither(_tmpAA3,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5D5;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2F0: if(f->width != 0){struct Cyc_Core_Opt*_tmpAAB;const char*_tmpAAA;const char*
_tmpAA9;struct Cyc_PP_Doc*_tmpAA8[5];return(_tmpAA8[4]=Cyc_PP_text(((_tmpAA9=";",
_tag_dyneither(_tmpAA9,sizeof(char),2)))),((_tmpAA8[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpAA8[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpAA8[1]=Cyc_PP_text(((_tmpAAA=":",_tag_dyneither(_tmpAAA,sizeof(char),2)))),((
_tmpAA8[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAAB=_cycalloc(sizeof(*
_tmpAAB)),((_tmpAAB->v=Cyc_PP_textptr(f->name),_tmpAAB))))),Cyc_PP_cat(
_tag_dyneither(_tmpAA8,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAB0;const char*_tmpAAF;struct Cyc_PP_Doc*_tmpAAE[3];return(_tmpAAE[2]=Cyc_PP_text(((
_tmpAAF=";",_tag_dyneither(_tmpAAF,sizeof(char),2)))),((_tmpAAE[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAAE[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAB0=
_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->v=Cyc_PP_textptr(f->name),_tmpAB0))))),
Cyc_PP_cat(_tag_dyneither(_tmpAAE,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2F1: if(f->width != 0){struct Cyc_Core_Opt*_tmpAB7;const char*_tmpAB6;const char*
_tmpAB5;struct Cyc_PP_Doc*_tmpAB4[5];return(_tmpAB4[4]=Cyc_PP_text(((_tmpAB5=";",
_tag_dyneither(_tmpAB5,sizeof(char),2)))),((_tmpAB4[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAB4[2]=Cyc_PP_text(((_tmpAB6=":",
_tag_dyneither(_tmpAB6,sizeof(char),2)))),((_tmpAB4[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->v=Cyc_PP_textptr(f->name),
_tmpAB7))))),((_tmpAB4[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAB4,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpABC;const char*_tmpABB;struct Cyc_PP_Doc*_tmpABA[3];return(_tmpABA[2]=Cyc_PP_text(((
_tmpABB=";",_tag_dyneither(_tmpABB,sizeof(char),2)))),((_tmpABA[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->v=Cyc_PP_textptr(
f->name),_tmpABC))))),((_tmpABA[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpABA,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpABD;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpABD="",
_tag_dyneither(_tmpABD,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpABE[3];return(_tmpABE[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpABE[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpABE[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpABE,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpABF;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpABF=",",
_tag_dyneither(_tmpABF,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAC2;
void*_tmpAC1;(_tmpAC1=0,Cyc_fprintf(f,((_tmpAC2="\n",_tag_dyneither(_tmpAC2,
sizeof(char),2))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAC3;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAC3="",_tag_dyneither(
_tmpAC3,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
