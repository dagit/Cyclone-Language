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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
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
hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);void*Cyc_List_hd(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int
Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(
void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
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
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_nontagged_union_type(void*);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);int
Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple10{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple10 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,
void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct
Cyc_CfFlowInfo_IsZero_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*
f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct
Cyc_Absyn_Exp*f1;int f2;void*f3;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*
unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict
mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(struct
Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,
struct _tuple12 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x);union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){union
Cyc_CfFlowInfo_AbsLVal _tmp455;return((_tmp455.PlaceL).val=x,(((_tmp455.PlaceL).tag=
1,_tmp455)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal _tmp456;return((_tmp456.UnknownL).val=
0,(((_tmp456.UnknownL).tag=2,_tmp456)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp457;return((_tmp457.BottomFL).val=0,(((_tmp457.BottomFL).tag=1,_tmp457)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){struct _tuple11 _tmp45A;union Cyc_CfFlowInfo_FlowInfo
_tmp459;return((_tmp459.ReachableFL).val=((_tmp45A.f1=fd,((_tmp45A.f2=r,_tmp45A)))),(((
_tmp459.ReachableFL).tag=2,_tmp459)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp491;struct Cyc_CfFlowInfo_MallocPt_struct _tmp490;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp48F;struct Cyc_CfFlowInfo_Esc_struct*
_tmp48E;struct Cyc_CfFlowInfo_Esc_struct _tmp48D;struct Cyc_CfFlowInfo_Esc_struct*
_tmp48C;struct Cyc_CfFlowInfo_Esc_struct _tmp48B;struct Cyc_CfFlowInfo_Esc_struct*
_tmp48A;struct Cyc_CfFlowInfo_Esc_struct _tmp489;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp488;struct Cyc_CfFlowInfo_UnknownR_struct _tmp487;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp486;struct Cyc_CfFlowInfo_UnknownR_struct _tmp485;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp484;struct Cyc_CfFlowInfo_UnknownR_struct _tmp483;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp482;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp481;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp480;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp47F;struct Cyc_CfFlowInfo_Zero_struct*
_tmp47E;struct Cyc_CfFlowInfo_Zero_struct _tmp47D;struct Cyc_CfFlowInfo_FlowEnv*
_tmp47C;return(_tmp47C=_region_malloc(r,sizeof(*_tmp47C)),((_tmp47C->r=r,((
_tmp47C->zero=(void*)((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E[0]=((
_tmp47D.tag=0,_tmp47D)),_tmp47E)))),((_tmp47C->notzeroall=(void*)((_tmp480=
_region_malloc(r,sizeof(*_tmp480)),((_tmp480[0]=((_tmp47F.tag=1,_tmp47F)),
_tmp480)))),((_tmp47C->notzerothis=(void*)((_tmp482=_region_malloc(r,sizeof(*
_tmp482)),((_tmp482[0]=((_tmp481.tag=2,_tmp481)),_tmp482)))),((_tmp47C->unknown_none=(
void*)((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484[0]=((_tmp483.tag=3,((
_tmp483.f1=Cyc_CfFlowInfo_NoneIL,_tmp483)))),_tmp484)))),((_tmp47C->unknown_this=(
void*)((_tmp486=_region_malloc(r,sizeof(*_tmp486)),((_tmp486[0]=((_tmp485.tag=3,((
_tmp485.f1=Cyc_CfFlowInfo_ThisIL,_tmp485)))),_tmp486)))),((_tmp47C->unknown_all=(
void*)((_tmp488=_region_malloc(r,sizeof(*_tmp488)),((_tmp488[0]=((_tmp487.tag=3,((
_tmp487.f1=Cyc_CfFlowInfo_AllIL,_tmp487)))),_tmp488)))),((_tmp47C->esc_none=(
void*)((_tmp48A=_region_malloc(r,sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp489.tag=4,((
_tmp489.f1=Cyc_CfFlowInfo_NoneIL,_tmp489)))),_tmp48A)))),((_tmp47C->esc_this=(
void*)((_tmp48C=_region_malloc(r,sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48B.tag=4,((
_tmp48B.f1=Cyc_CfFlowInfo_ThisIL,_tmp48B)))),_tmp48C)))),((_tmp47C->esc_all=(
void*)((_tmp48E=_region_malloc(r,sizeof(*_tmp48E)),((_tmp48E[0]=((_tmp48D.tag=4,((
_tmp48D.f1=Cyc_CfFlowInfo_AllIL,_tmp48D)))),_tmp48E)))),((_tmp47C->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp47C->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp47C->dummy_place=((_tmp491=
_region_malloc(r,sizeof(*_tmp491)),((_tmp491->root=(void*)((_tmp48F=
_region_malloc(r,sizeof(*_tmp48F)),((_tmp48F[0]=((_tmp490.tag=1,((_tmp490.f1=&
dummy_exp,((_tmp490.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp490)))))),
_tmp48F)))),((_tmp491->fields=0,_tmp491)))))),_tmp47C)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc);int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,
void*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp1F;struct Cyc_List_List*_tmp20;struct
Cyc_CfFlowInfo_Place _tmp1E=*place;_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*
_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp21;if(_tmp22->tag != 0)goto _LL3;
else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)return Cyc_Absynpp_qvar2string(_tmp23->name);
else{const char*_tmp495;void*_tmp494[1];struct Cyc_String_pa_struct _tmp493;return(
struct _dyneither_ptr)((_tmp493.tag=0,((_tmp493.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23->name)),((_tmp494[0]=& _tmp493,Cyc_aprintf(((
_tmp495="reachable from %s",_tag_dyneither(_tmp495,sizeof(char),18))),
_tag_dyneither(_tmp494,sizeof(void*),1))))))));}_LL3:;_LL4: {const char*_tmp498;
void*_tmp497;(_tmp497=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp498="error locations not for VarRoots",_tag_dyneither(
_tmp498,sizeof(char),33))),_tag_dyneither(_tmp497,sizeof(void*),0)));}_LL0:;};}
static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*env,void*
place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2);static
struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){return loc1;}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp29=0;struct Cyc_Dict_Dict
_tmp2A=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(int*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),int*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
int*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct
Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,s1,s2);
return _tmp2A;}struct _tuple13{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))
_tmp2D=1;if(!_tmp2D){{struct _RegionHandle _tmp2E=_new_region("temp");struct
_RegionHandle*temp=& _tmp2E;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple13 _tmp2F=*((struct _tuple13*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple13*))Cyc_Iter_next)(iter,& _tmp2F)){void*_tmp30=_tmp2F.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp30)){int _tmp31=0;
_npop_handler(1);return _tmp31;}}{int _tmp32=1;_npop_handler(1);return _tmp32;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;void*
_tmp34=_tmp2C;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp34);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,
struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct
_dyneither_ptr*_tmp35=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmp35,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4A5;const char*
_tmp4A4;void*_tmp4A3[1];struct Cyc_String_pa_struct _tmp4A2;struct Cyc_Core_Impossible_struct*
_tmp4A1;(int)_throw((void*)((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1[0]=((
_tmp4A5.tag=Cyc_Core_Impossible,((_tmp4A5.f1=(struct _dyneither_ptr)((_tmp4A2.tag=
0,((_tmp4A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4A3[0]=&
_tmp4A2,Cyc_aprintf(((_tmp4A4="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4A4,sizeof(char),30))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))))),
_tmp4A5)))),_tmp4A1)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3D;union Cyc_Absyn_AggrInfoU
_tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct Cyc_Absyn_AggrType_struct*_tmp3C=(
struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(_tmp3C->tag != 12)goto _LLD;else{_tmp3D=
_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {struct Cyc_List_List*_tmp41=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp3E))->impl))->fields;
_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp3F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;else{
_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);_LLF:;
_LL10: {struct Cyc_Core_Impossible_struct _tmp4B2;const char*_tmp4B1;void*_tmp4B0[1];
struct Cyc_String_pa_struct _tmp4AF;struct Cyc_Core_Impossible_struct*_tmp4AE;(int)
_throw((void*)((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE[0]=((_tmp4B2.tag=
Cyc_Core_Impossible,((_tmp4B2.f1=(struct _dyneither_ptr)((_tmp4AF.tag=0,((_tmp4AF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp4B0[
0]=& _tmp4AF,Cyc_aprintf(((_tmp4B1="get_field_index failed: %s",_tag_dyneither(
_tmp4B1,sizeof(char),27))),_tag_dyneither(_tmp4B0,sizeof(void*),1)))))))),
_tmp4B2)))),_tmp4AE)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp4B3;struct _tuple0 _tmp48=(_tmp4B3.f1=r1,((_tmp4B3.f2=r2,_tmp4B3)));
void*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4B;void*_tmp4C;struct Cyc_Absyn_Vardecl*
_tmp4E;void*_tmp4F;void*_tmp51;void*_tmp53;struct Cyc_Absyn_Exp*_tmp55;void*
_tmp56;struct Cyc_Absyn_Exp*_tmp58;void*_tmp59;void*_tmp5B;void*_tmp5D;int _tmp5F;
void*_tmp60;int _tmp62;_LL12: _tmp49=_tmp48.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4A=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp49;if(_tmp4A->tag != 0)goto _LL14;
else{_tmp4B=_tmp4A->f1;}};_tmp4C=_tmp48.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4C;if(_tmp4D->tag != 0)goto _LL14;
else{_tmp4E=_tmp4D->f1;}};_LL13: return(int)_tmp4B - (int)_tmp4E;_LL14: _tmp4F=
_tmp48.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp50=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4F;if(_tmp50->tag != 0)goto _LL16;};_LL15: return - 1;_LL16: _tmp51=_tmp48.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp52=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp51;if(_tmp52->tag != 0)goto _LL18;};_LL17: return 1;_LL18: _tmp53=_tmp48.f1;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp54=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp53;if(_tmp54->tag != 1)goto _LL1A;else{_tmp55=_tmp54->f1;}};_tmp56=_tmp48.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp57=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1A;else{_tmp58=_tmp57->f1;}};_LL19: return(int)
_tmp55 - (int)_tmp58;_LL1A: _tmp59=_tmp48.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp5A=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp59;if(_tmp5A->tag != 1)goto
_LL1C;};_LL1B: return - 1;_LL1C: _tmp5B=_tmp48.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp5C=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp5B;if(_tmp5C->tag != 1)goto
_LL1E;};_LL1D: return 1;_LL1E: _tmp5D=_tmp48.f1;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5E=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp5D;if(_tmp5E->tag != 2)goto
_LL11;else{_tmp5F=_tmp5E->f1;}};_tmp60=_tmp48.f2;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp61=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp60;if(_tmp61->tag != 2)goto
_LL11;else{_tmp62=_tmp61->f1;}};_LL1F: return _tmp5F - _tmp62;_LL11:;};}static int
Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2);
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*
list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{void*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_CfFlowInfo_Place
_tmp63=*p1;_tmp64=_tmp63.root;_tmp65=_tmp63.fields;{void*_tmp67;struct Cyc_List_List*
_tmp68;struct Cyc_CfFlowInfo_Place _tmp66=*p2;_tmp67=_tmp66.root;_tmp68=_tmp66.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp64,_tmp67);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp65,_tmp68);};};};}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p);static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp6A;struct Cyc_List_List*
_tmp6B;struct Cyc_CfFlowInfo_Place _tmp69=*p;_tmp6A=_tmp69.root;_tmp6B=_tmp69.fields;{
void*_tmp6C=_tmp6A;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Exp*_tmp70;int
_tmp72;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp6D=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp6C;if(_tmp6D->tag != 0)goto _LL23;else{_tmp6E=_tmp6D->f1;}}_LL22:{struct
_dyneither_ptr*_tmp4C0;const char*_tmp4BF;void*_tmp4BE[1];struct Cyc_String_pa_struct
_tmp4BD;struct Cyc_List_List*_tmp4BC;sl=((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((
_tmp4BC->hd=((_tmp4C0=_cycalloc(sizeof(*_tmp4C0)),((_tmp4C0[0]=((_tmp4BD.tag=0,((
_tmp4BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6E->name).f2),((
_tmp4BE[0]=& _tmp4BD,Cyc_aprintf(((_tmp4BF="%s",_tag_dyneither(_tmp4BF,sizeof(
char),3))),_tag_dyneither(_tmp4BE,sizeof(void*),1)))))))),_tmp4C0)))),((_tmp4BC->tl=
sl,_tmp4BC))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{
_tmp70=_tmp6F->f1;}}_LL24:{struct _dyneither_ptr*_tmp4CD;const char*_tmp4CC;void*
_tmp4CB[1];struct Cyc_Int_pa_struct _tmp4CA;struct Cyc_List_List*_tmp4C9;sl=((
_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9->hd=((_tmp4CD=_cycalloc(sizeof(*
_tmp4CD)),((_tmp4CD[0]=((_tmp4CA.tag=1,((_tmp4CA.f1=(unsigned long)((int)_tmp70),((
_tmp4CB[0]=& _tmp4CA,Cyc_aprintf(((_tmp4CC="mpt%d",_tag_dyneither(_tmp4CC,sizeof(
char),6))),_tag_dyneither(_tmp4CB,sizeof(void*),1)))))))),_tmp4CD)))),((_tmp4C9->tl=
sl,_tmp4C9))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{
_tmp72=_tmp71->f1;}}_LL26:{struct _dyneither_ptr*_tmp4DA;const char*_tmp4D9;void*
_tmp4D8[1];struct Cyc_Int_pa_struct _tmp4D7;struct Cyc_List_List*_tmp4D6;sl=((
_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6->hd=((_tmp4DA=_cycalloc(sizeof(*
_tmp4DA)),((_tmp4DA[0]=((_tmp4D7.tag=1,((_tmp4D7.f1=(unsigned long)_tmp72,((
_tmp4D8[0]=& _tmp4D7,Cyc_aprintf(((_tmp4D9="param%d",_tag_dyneither(_tmp4D9,
sizeof(char),8))),_tag_dyneither(_tmp4D8,sizeof(void*),1)))))))),_tmp4DA)))),((
_tmp4D6->tl=sl,_tmp4D6))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;_tmp6B=_tmp6B->tl){
struct _dyneither_ptr*_tmp4E7;const char*_tmp4E6;void*_tmp4E5[1];struct Cyc_Int_pa_struct
_tmp4E4;struct Cyc_List_List*_tmp4E3;sl=((_tmp4E3=_cycalloc(sizeof(*_tmp4E3)),((
_tmp4E3->hd=((_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7[0]=((_tmp4E4.tag=1,((
_tmp4E4.f1=(unsigned long)((int)_tmp6B->hd),((_tmp4E5[0]=& _tmp4E4,Cyc_aprintf(((
_tmp4E6="%d",_tag_dyneither(_tmp4E6,sizeof(char),3))),_tag_dyneither(_tmp4E5,
sizeof(void*),1)))))))),_tmp4E7)))),((_tmp4E3->tl=sl,_tmp4E3))))));}{struct Cyc_String_pa_struct
_tmp4F2;const char*_tmp4F1;void*_tmp4F0[1];const char*_tmp4EF;struct _dyneither_ptr*
_tmp4EE;struct _dyneither_ptr*_tmp87=(_tmp4EE=_cycalloc(sizeof(*_tmp4EE)),((
_tmp4EE[0]=((_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((_tmp4F1="",
_tag_dyneither(_tmp4F1,sizeof(char),1))),((_tmp4F0[0]=& _tmp4F2,Cyc_aprintf(((
_tmp4EF="%s",_tag_dyneither(_tmp4EF,sizeof(char),3))),_tag_dyneither(_tmp4F0,
sizeof(void*),1)))))))),_tmp4EE)));for(0;sl != 0;sl=sl->tl){const char*_tmp4F7;
void*_tmp4F6[2];struct Cyc_String_pa_struct _tmp4F5;struct Cyc_String_pa_struct
_tmp4F4;*_tmp87=((_tmp4F4.tag=0,((_tmp4F4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp87),((_tmp4F5.tag=0,((_tmp4F5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp4F6[0]=& _tmp4F5,((
_tmp4F6[1]=& _tmp4F4,Cyc_aprintf(((_tmp4F7="%s.%s",_tag_dyneither(_tmp4F7,sizeof(
char),6))),_tag_dyneither(_tmp4F6,sizeof(void*),2))))))))))))));}return _tmp87;};}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp4FD(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4FC,unsigned int*
_tmp4FB,void***_tmp4F9){for(*_tmp4FC=0;*_tmp4FC < *_tmp4FB;(*_tmp4FC)++){(*
_tmp4F9)[*_tmp4FC]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp4FC;unsigned int _tmp4FB;struct _dyneither_ptr _tmp4FA;void**
_tmp4F9;unsigned int _tmp4F8;struct _dyneither_ptr d=(_tmp4F8=sz,((_tmp4F9=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F8)),((_tmp4FA=
_tag_dyneither(_tmp4F9,sizeof(void*),_tmp4F8),((((_tmp4FB=_tmp4F8,_tmp4FD(& fenv,&
_tmp4FC,& _tmp4FB,& _tmp4F9))),_tmp4FA)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp92;struct _dyneither_ptr*_tmp93;void*_tmp94;struct
Cyc_Absyn_Aggrfield*_tmp91=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp92=*_tmp91;_tmp93=_tmp92.name;_tmp94=_tmp92.type;if(
_get_dyneither_size(*_tmp93,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmp94,leafval);}}return d;}struct _tuple14{struct _RegionHandle*f1;struct
Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
struct _tuple14 _tmp9A;struct _RegionHandle*_tmp9B;struct Cyc_List_List*_tmp9C;
struct _tuple14*_tmp99=env;_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;{void*
_tmp9D=Cyc_Tcutil_rsubstitute(_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*
_tmp4FE;return(_tmp4FE=_region_malloc(_tmp9B,sizeof(*_tmp4FE)),((_tmp4FE->name=f->name,((
_tmp4FE->tq=f->tq,((_tmp4FE->type=_tmp9D,((_tmp4FE->width=f->width,((_tmp4FE->attributes=
f->attributes,_tmp4FE)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,void*leafval);static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,void*leafval){struct _RegionHandle _tmp9F=_new_region("temp");
struct _RegionHandle*temp=& _tmp9F;_push_region(temp);{struct Cyc_List_List*inst=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14
_tmp4FF;struct _tuple14 _tmpA0=(_tmp4FF.f1=temp,((_tmp4FF.f2=inst,_tmp4FF)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,&
_tmpA0,fs);struct _dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,
subs_fs,leafval);_npop_handler(0);return _tmpA1;};_pop_region(temp);}struct
_tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);static
void _tmp505(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp504,unsigned int*
_tmp503,void***_tmp501){for(*_tmp504=0;*_tmp504 < *_tmp503;(*_tmp504)++){(*
_tmp501)[*_tmp504]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(
t))return fenv->unknown_all;{void*_tmpA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpA5;union Cyc_Absyn_DatatypeFieldInfoU _tmpA6;struct _tuple2 _tmpA7;struct Cyc_Absyn_Datatypefield*
_tmpA8;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_AggrInfo _tmpAC;union Cyc_Absyn_AggrInfoU
_tmpAD;struct Cyc_List_List*_tmpAE;enum Cyc_Absyn_AggrKind _tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_ArrayInfo _tmpB3;void*_tmpB4;union Cyc_Absyn_Constraint*
_tmpB5;void*_tmpB7;struct Cyc_Absyn_PtrInfo _tmpB9;struct Cyc_Absyn_PtrAtts _tmpBA;
union Cyc_Absyn_Constraint*_tmpBB;_LL28: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA4=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA3;if(_tmpA4->tag != 4)goto
_LL2A;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.field_info;if((_tmpA6.KnownDatatypefield).tag
!= 2)goto _LL2A;_tmpA7=(struct _tuple2)(_tmpA6.KnownDatatypefield).val;_tmpA8=
_tmpA7.f2;}}_LL29: if(_tmpA8->typs == 0)return leafval;_tmpAA=_tmpA8->typs;goto
_LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpA9=(struct Cyc_Absyn_TupleType_struct*)
_tmpA3;if(_tmpA9->tag != 11)goto _LL2C;else{_tmpAA=_tmpA9->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpAA);unsigned int _tmp504;unsigned int _tmp503;struct _dyneither_ptr _tmp502;void**
_tmp501;unsigned int _tmp500;struct _dyneither_ptr d=(_tmp500=sz,((_tmp501=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp500)),((_tmp502=
_tag_dyneither(_tmp501,sizeof(void*),_tmp500),((((_tmp503=_tmp500,_tmp505(& fenv,&
_tmp504,& _tmp503,& _tmp501))),_tmp502)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpAA))->hd)).f2,
leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp508;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp507;return(void*)((_tmp507=
_region_malloc(fenv->r,sizeof(*_tmp507)),((_tmp507[0]=((_tmp508.tag=6,((_tmp508.f1=
0,((_tmp508.f2=d,_tmp508)))))),_tmp507))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC2=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC2->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC3=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC2->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC2->kind;_tmpB1=
_tmpC3;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp50B;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp50A;return(void*)((_tmp50A=_region_malloc(fenv->r,sizeof(*_tmp50A)),((
_tmp50A[0]=((_tmp50B.tag=6,((_tmp50B.f1=_tmpC2->kind == Cyc_Absyn_UnionA,((
_tmp50B.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC2->tvs,
_tmpAE,_tmpC3,leafval),_tmp50B)))))),_tmp50A))));};};}_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpA3;if(_tmpAF->tag != 13)goto
_LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct
_tmp50E;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp50D;return(void*)((_tmp50D=
_region_malloc(fenv->r,sizeof(*_tmp50D)),((_tmp50D[0]=((_tmp50E.tag=6,((_tmp50E.f1=
_tmpB0 == Cyc_Absyn_UnionA,((_tmp50E.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmpB1,leafval),_tmp50E)))))),_tmp50D))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpB2=(struct Cyc_Absyn_ArrayType_struct*)_tmpA3;if(_tmpB2->tag != 9)goto _LL32;
else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB3.elt_type;_tmpB5=_tmpB3.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB5))goto
_LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpB4)?fenv->unknown_all:
leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpB6=(struct Cyc_Absyn_TagType_struct*)
_tmpA3;if(_tmpB6->tag != 20)goto _LL34;else{_tmpB7=(void*)_tmpB6->f1;}}_LL33:
return leafval;_LL34:{struct Cyc_Absyn_PointerType_struct*_tmpB8=(struct Cyc_Absyn_PointerType_struct*)
_tmpA3;if(_tmpB8->tag != 5)goto _LL36;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.ptr_atts;
_tmpBB=_tmpBA.nullable;}}if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpBB)))goto _LL36;_LL35:{void*_tmpC8=leafval;enum Cyc_CfFlowInfo_InitLevel
_tmpCA;enum Cyc_CfFlowInfo_InitLevel _tmpCC;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpC9=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpC8;if(_tmpC9->tag != 3)goto
_LL3B;else{_tmpCA=_tmpC9->f1;if(_tmpCA != Cyc_CfFlowInfo_ThisIL)goto _LL3B;}}_LL3A:
return fenv->notzerothis;_LL3B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpCB=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpC8;if(_tmpCB->tag != 3)goto _LL3D;else{
_tmpCC=_tmpCB->f1;if(_tmpCC != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C: return fenv->notzeroall;
_LL3D:;_LL3E: goto _LL38;_LL38:;}goto _LL27;_LL36:;_LL37: goto _LL27;_LL27:;}return
Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,
int env_iteration,void*r);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*
e,int env_iteration,void*r){void*_tmpCD=r;struct Cyc_Absyn_Exp*_tmpCF;int _tmpD0;
void*_tmpD1;struct _dyneither_ptr _tmpD3;_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmpCE=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmpCD;if(_tmpCE->tag != 7)goto
_LL42;else{_tmpCF=_tmpCE->f1;_tmpD0=_tmpCE->f2;_tmpD1=(void*)_tmpCE->f3;}}_LL41:
return !(_tmpCF == e  && _tmpD0 == env_iteration);_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpD2=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpCD;if(_tmpD2->tag != 6)goto
_LL44;else{_tmpD3=_tmpD2->f2;}}_LL43: {unsigned int _tmpD4=_get_dyneither_size(
_tmpD3,sizeof(void*));{int i=0;for(0;i < _tmpD4;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(
e,env_iteration,*((void**)_check_dyneither_subscript(_tmpD3,sizeof(void*),i))))
return 1;}}return 0;}_LL44:;_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp518(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp517,unsigned int*_tmp516,void***_tmp514){for(*_tmp517=0;*
_tmp517 < *_tmp516;(*_tmp517)++){(*_tmp514)[*_tmp517]=(*fenv)->unknown_all;}}
static void _tmp521(struct _dyneither_ptr*_tmpEE,unsigned int*_tmp520,unsigned int*
_tmp51F,void***_tmp51D){for(*_tmp520=0;*_tmp520 < *_tmp51F;(*_tmp520)++){(*
_tmp51D)[*_tmp520]=((void**)(*_tmpEE).curr)[(int)*_tmp520];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp511;struct Cyc_CfFlowInfo_Consumed_struct*_tmp510;return(void*)((_tmp510=
_region_malloc(fenv->r,sizeof(*_tmp510)),((_tmp510[0]=((_tmp511.tag=7,((_tmp511.f1=
consumer,((_tmp511.f2=iteration,((_tmp511.f3=(void*)r,_tmp511)))))))),_tmp510))));}{
struct _tuple0 _tmp512;struct _tuple0 _tmpD8=(_tmp512.f1=Cyc_Tcutil_compress(t),((
_tmp512.f2=r,_tmp512)));void*_tmpD9;struct Cyc_List_List*_tmpDB;void*_tmpDC;
struct _dyneither_ptr _tmpDE;void*_tmpDF;struct Cyc_Absyn_AggrInfo _tmpE1;union Cyc_Absyn_AggrInfoU
_tmpE2;void*_tmpE3;int _tmpE5;struct _dyneither_ptr _tmpE6;void*_tmpE7;enum Cyc_Absyn_AggrKind
_tmpE9;struct Cyc_List_List*_tmpEA;void*_tmpEB;int _tmpED;struct _dyneither_ptr
_tmpEE;_LL47: _tmpD9=_tmpD8.f1;{struct Cyc_Absyn_TupleType_struct*_tmpDA=(struct
Cyc_Absyn_TupleType_struct*)_tmpD9;if(_tmpDA->tag != 11)goto _LL49;else{_tmpDB=
_tmpDA->f1;}};_tmpDC=_tmpD8.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmpDD=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDC;if(_tmpDD->tag != 6)goto _LL49;else{
_tmpDE=_tmpDD->f2;}};_LL48: {unsigned int _tmpEF=_get_dyneither_size(_tmpDE,
sizeof(void*));unsigned int _tmp517;unsigned int _tmp516;struct _dyneither_ptr
_tmp515;void**_tmp514;unsigned int _tmp513;struct _dyneither_ptr d2=(_tmp513=_tmpEF,((
_tmp514=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp513)),((
_tmp515=_tag_dyneither(_tmp514,sizeof(void*),_tmp513),((((_tmp516=_tmp513,
_tmp518(& fenv,& _tmp517,& _tmp516,& _tmp514))),_tmp515)))))));{int i=0;for(0;i < 
_tmpEF;++ i){*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpDB))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpDE,sizeof(void*),i)));
_tmpDB=_tmpDB->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp51B;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp51A;return(void*)((_tmp51A=_region_malloc(fenv->r,sizeof(*_tmp51A)),((
_tmp51A[0]=((_tmp51B.tag=6,((_tmp51B.f1=0,((_tmp51B.f2=d2,_tmp51B)))))),_tmp51A))));};}
_LL49: _tmpDF=_tmpD8.f1;{struct Cyc_Absyn_AggrType_struct*_tmpE0=(struct Cyc_Absyn_AggrType_struct*)
_tmpDF;if(_tmpE0->tag != 12)goto _LL4B;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE1.aggr_info;}};
_tmpE3=_tmpD8.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmpE4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpE3;if(_tmpE4->tag != 6)goto _LL4B;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;}};
_LL4A: {struct Cyc_Absyn_Aggrdecl*_tmpF6=Cyc_Absyn_get_known_aggrdecl(_tmpE2);if(
_tmpF6->impl == 0)return r;_tmpE9=_tmpF6->kind;_tmpEA=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpF6->impl))->fields;_tmpED=_tmpE5;_tmpEE=_tmpE6;goto _LL4C;}_LL4B:
_tmpE7=_tmpD8.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmpE8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE7;if(_tmpE8->tag != 13)goto _LL4D;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}};
_tmpEB=_tmpD8.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmpEC=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpEB;if(_tmpEC->tag != 6)goto _LL4D;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}};
_LL4C: {unsigned int _tmp520;unsigned int _tmp51F;struct _dyneither_ptr _tmp51E;void**
_tmp51D;unsigned int _tmp51C;struct _dyneither_ptr d2=(_tmp51C=_get_dyneither_size(
_tmpEE,sizeof(void*)),((_tmp51D=(void**)_region_malloc(fenv->r,_check_times(
sizeof(void*),_tmp51C)),((_tmp51E=_tag_dyneither(_tmp51D,sizeof(void*),_tmp51C),((((
_tmp51F=_tmp51C,_tmp521(& _tmpEE,& _tmp520,& _tmp51F,& _tmp51D))),_tmp51E)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpEA);{int i=0;for(0;i < sz;(i ++,_tmpEA=_tmpEA->tl)){struct Cyc_Absyn_Aggrfield
_tmpF8;struct _dyneither_ptr*_tmpF9;void*_tmpFA;struct Cyc_Absyn_Aggrfield*_tmpF7=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpEA))->hd;
_tmpF8=*_tmpF7;_tmpF9=_tmpF8.name;_tmpFA=_tmpF8.type;if(_get_dyneither_size(*
_tmpF9,sizeof(char))!= 1)*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=
Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpFA,consumer,iteration,*((void**)
_check_dyneither_subscript(_tmpEE,sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp524;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp523;return(void*)((_tmp523=
_region_malloc(fenv->r,sizeof(*_tmp523)),((_tmp523[0]=((_tmp524.tag=6,((_tmp524.f1=
_tmpED,((_tmp524.f2=_tmpEE,_tmp524)))))),_tmp523))));};}_LL4D:;_LL4E: return r;
_LL46:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmp101=_new_region("r");struct
_RegionHandle*r=& _tmp101;_push_region(r);{struct _tuple16 _tmp525;struct _tuple16
_tmp102=(_tmp525.f1=place,((_tmp525.f2=0,_tmp525)));struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next)(iter,&
_tmp102)){struct Cyc_CfFlowInfo_Place*_tmp103=_tmp102.f1;void*_tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_CfFlowInfo_Place _tmp104=*place;_tmp105=_tmp104.root;_tmp106=
_tmp104.fields;{void*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_CfFlowInfo_Place
_tmp107=*_tmp103;_tmp108=_tmp107.root;_tmp109=_tmp107.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmp105,_tmp108)!= 0)continue;for(0;_tmp106 != 0  && _tmp109 != 0;(_tmp106=_tmp106->tl,
_tmp109=_tmp109->tl)){if((int)_tmp106->hd != (int)_tmp109->hd)break;}if(_tmp106 == 
0){int _tmp10A=1;_npop_handler(0);return _tmp10A;}};}{int _tmp10B=0;_npop_handler(0);
return _tmp10B;};};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place);static void
Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*
place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,
pile->places,place)){struct Cyc_List_List*_tmp526;pile->places=((_tmp526=
_region_malloc(pile->rgn,sizeof(*_tmp526)),((_tmp526->hd=place,((_tmp526->tl=
pile->places,_tmp526))))));}}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r);static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r){void*_tmp10E=r;void*_tmp110;struct Cyc_CfFlowInfo_Place*_tmp112;int
_tmp114;struct _dyneither_ptr _tmp115;_LL50: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp10F=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp10E;if(_tmp10F->tag != 7)goto
_LL52;else{_tmp110=(void*)_tmp10F->f3;}}_LL51: Cyc_CfFlowInfo_add_places(pile,
_tmp110);return;_LL52: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp111=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp10E;if(_tmp111->tag != 5)goto _LL54;else{_tmp112=_tmp111->f1;}}_LL53: Cyc_CfFlowInfo_add_place(
pile,_tmp112);return;_LL54: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp113=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10E;if(_tmp113->tag != 6)goto _LL56;
else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL55:{int i=0;for(0;i < 
_get_dyneither_size(_tmp115,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(pile,((
void**)_tmp115.curr)[i]);}}return;_LL56:;_LL57: return;_LL4F:;}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp52C(struct
_RegionHandle**r,void**new_val,unsigned int*_tmp52B,unsigned int*_tmp52A,void***
_tmp528,struct _dyneither_ptr*_tmp119){for(*_tmp52B=0;*_tmp52B < *_tmp52A;(*
_tmp52B)++){(*_tmp528)[*_tmp52B]=Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((
void**)(*_tmp119).curr)[(int)*_tmp52B]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp116=old_val;int _tmp118;
struct _dyneither_ptr _tmp119;_LL59: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp117=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp116;if(_tmp117->tag != 6)goto
_LL5B;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL5A: {unsigned int _tmp52B;
unsigned int _tmp52A;struct _dyneither_ptr _tmp529;void**_tmp528;unsigned int
_tmp527;struct _dyneither_ptr d2=(_tmp527=_get_dyneither_size(_tmp119,sizeof(void*)),((
_tmp528=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp527)),((_tmp529=
_tag_dyneither(_tmp528,sizeof(void*),_tmp527),((((_tmp52A=_tmp527,_tmp52C(& r,&
new_val,& _tmp52B,& _tmp52A,& _tmp528,& _tmp119))),_tmp529)))))));struct Cyc_CfFlowInfo_Aggregate_struct
_tmp52F;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp52E;return(void*)((_tmp52E=
_region_malloc(r,sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp52F.tag=6,((_tmp52F.f1=
_tmp118,((_tmp52F.f2=d2,_tmp52F)))))),_tmp52E))));}_LL5B:;_LL5C: return new_val;
_LL58:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp535(struct
_dyneither_ptr*d,unsigned int*_tmp534,unsigned int*_tmp533,void***_tmp531){for(*
_tmp534=0;*_tmp534 < *_tmp533;(*_tmp534)++){(*_tmp531)[*_tmp534]=((void**)(*d).curr)[(
int)*_tmp534];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp120=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp120 == rval)return d;{
unsigned int _tmp534;unsigned int _tmp533;struct _dyneither_ptr _tmp532;void**
_tmp531;unsigned int _tmp530;struct _dyneither_ptr res=(_tmp530=_get_dyneither_size(
d,sizeof(void*)),((_tmp531=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp530)),((_tmp532=_tag_dyneither(_tmp531,sizeof(void*),_tmp530),((((_tmp533=
_tmp530,_tmp535(& d,& _tmp534,& _tmp533,& _tmp531))),_tmp532)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple17 _tmp536;struct _tuple17 _tmp126=(_tmp536.f1=fs,((
_tmp536.f2=old_val,_tmp536)));struct Cyc_List_List*_tmp127;struct Cyc_List_List
_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;void*_tmp12B;int _tmp12D;struct
_dyneither_ptr _tmp12E;_LL5E: _tmp127=_tmp126.f1;if(_tmp127 == 0)goto _LL60;_tmp128=*
_tmp127;_tmp129=(int)_tmp128.hd;_tmp12A=_tmp128.tl;_tmp12B=_tmp126.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12C=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12B;if(_tmp12C->tag != 6)goto
_LL60;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}};_LL5F: {void*_tmp12F=Cyc_CfFlowInfo_insert_place_outer(
r,_tmp12A,*((void**)_check_dyneither_subscript(_tmp12E,sizeof(void*),_tmp129)),
new_val);struct _dyneither_ptr _tmp130=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp12E,
_tmp129,_tmp12F);if((void**)_tmp130.curr == (void**)_tmp12E.curr)return old_val;
else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp539;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp538;return(void*)((_tmp538=_region_malloc(r,sizeof(*_tmp538)),((_tmp538[0]=((
_tmp539.tag=6,((_tmp539.f1=_tmp12D,((_tmp539.f2=_tmp130,_tmp539)))))),_tmp538))));}}
_LL60:;_LL61: {struct Cyc_Core_Impossible_struct _tmp53F;const char*_tmp53E;struct
Cyc_Core_Impossible_struct*_tmp53D;(int)_throw((void*)((_tmp53D=_cycalloc(
sizeof(*_tmp53D)),((_tmp53D[0]=((_tmp53F.tag=Cyc_Core_Impossible,((_tmp53F.f1=((
_tmp53E="bad insert place",_tag_dyneither(_tmp53E,sizeof(char),17))),_tmp53F)))),
_tmp53D)))));}_LL5D:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp136=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp136,0);{void*oldval;void*
newval;{struct _handler_cons _tmp137;_push_handler(& _tmp137);{int _tmp139=0;if(
setjmp(_tmp137.handler))_tmp139=1;if(!_tmp139){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp136);;_pop_handler();}else{void*_tmp138=(void*)_exn_thrown;void*_tmp13B=
_tmp138;_LL63: {struct Cyc_Dict_Absent_struct*_tmp13C=(struct Cyc_Dict_Absent_struct*)
_tmp13B;if(_tmp13C->tag != Cyc_Dict_Absent)goto _LL65;}_LL64: continue;_LL65:;_LL66:(
void)_throw(_tmp13B);_LL62:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL67: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL68: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL69: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp13E;struct Cyc_List_List*
_tmp13F;struct Cyc_CfFlowInfo_Place _tmp13D=*_tmp136;_tmp13E=_tmp13D.root;_tmp13F=
_tmp13D.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp13E,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp13F,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp13E),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r);static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*
r){void*_tmp140=r;enum Cyc_CfFlowInfo_InitLevel _tmp142;enum Cyc_CfFlowInfo_InitLevel
_tmp144;void*_tmp149;_LL6C: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp141=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp140;if(_tmp141->tag != 3)goto _LL6E;
else{_tmp142=_tmp141->f1;}}_LL6D: return _tmp142;_LL6E: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp143=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp140;if(_tmp143->tag != 4)goto _LL70;
else{_tmp144=_tmp143->f1;}}_LL6F: return _tmp144;_LL70: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp145=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp140;if(_tmp145->tag != 0)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp146=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp140;if(_tmp146->tag != 1)goto _LL74;}_LL73:
return Cyc_CfFlowInfo_AllIL;_LL74: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp147=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp140;if(_tmp147->tag != 2)
goto _LL76;}_LL75: return Cyc_CfFlowInfo_ThisIL;_LL76: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp148=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp140;if(_tmp148->tag != 7)goto
_LL78;else{_tmp149=(void*)_tmp148->f3;}}_LL77: return Cyc_CfFlowInfo_NoneIL;_LL78:;
_LL79: {struct Cyc_Core_Impossible_struct _tmp545;const char*_tmp544;struct Cyc_Core_Impossible_struct*
_tmp543;(int)_throw((void*)((_tmp543=_cycalloc(sizeof(*_tmp543)),((_tmp543[0]=((
_tmp545.tag=Cyc_Core_Impossible,((_tmp545.f1=((_tmp544="initlevel_approx",
_tag_dyneither(_tmp544,sizeof(char),17))),_tmp545)))),_tmp543)))));}_LL6B:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc);static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum 
Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)
return Cyc_CfFlowInfo_NoneIL;{void*_tmp14D=r;int _tmp14F;struct _dyneither_ptr
_tmp150;int _tmp152;struct _dyneither_ptr _tmp153;struct Cyc_CfFlowInfo_Place*
_tmp155;_LL7B: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp14E=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp14D;if(_tmp14E->tag != 6)goto _LL7D;else{_tmp14F=_tmp14E->f1;if(_tmp14F != 1)
goto _LL7D;_tmp150=_tmp14E->f2;}}_LL7C: {unsigned int _tmp156=_get_dyneither_size(
_tmp150,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;{int i=0;for(0;i < _tmp156;
++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(
_tmp150,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){this_ans=
Cyc_CfFlowInfo_AllIL;break;}}}goto _LL7A;}_LL7D: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp151=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14D;if(_tmp151->tag != 6)goto
_LL7F;else{_tmp152=_tmp151->f1;if(_tmp152 != 0)goto _LL7F;_tmp153=_tmp151->f2;}}
_LL7E: {unsigned int _tmp157=_get_dyneither_size(_tmp153,sizeof(void*));this_ans=
Cyc_CfFlowInfo_AllIL;{int i=0;for(0;i < _tmp157;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp153,sizeof(void*),i)),this_ans);}}
goto _LL7A;}_LL7F: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp154=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp14D;if(_tmp154->tag != 5)goto _LL81;else{_tmp155=_tmp154->f1;}}_LL80: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp155))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp546;env->seen=((
_tmp546=_region_malloc(env->e,sizeof(*_tmp546)),((_tmp546->hd=_tmp155,((_tmp546->tl=
env->seen,_tmp546))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp155),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL7A;_LL81:;_LL82: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL7A:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct
_RegionHandle*_tmp159=env->r;struct Cyc_CfFlowInfo_InitlevelEnv _tmp547;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp15A=(_tmp547.e=_tmp159,((_tmp547.d=d,((_tmp547.seen=0,_tmp547)))));return Cyc_CfFlowInfo_initlevel_rec(&
_tmp15A,r,Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp15D;struct Cyc_List_List*
_tmp15E;struct Cyc_CfFlowInfo_Place _tmp15C=*place;_tmp15D=_tmp15C.root;_tmp15E=
_tmp15C.fields;{void*_tmp15F=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp15D);for(0;_tmp15E != 0;_tmp15E=_tmp15E->tl){struct _tuple18 _tmp548;struct
_tuple18 _tmp161=(_tmp548.f1=_tmp15F,((_tmp548.f2=(int)_tmp15E->hd,_tmp548)));
void*_tmp162;int _tmp164;struct _dyneither_ptr _tmp165;int _tmp166;_LL84: _tmp162=
_tmp161.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp163=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp162;if(_tmp163->tag != 6)goto _LL86;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}};
_tmp166=_tmp161.f2;_LL85: _tmp15F=*((void**)_check_dyneither_subscript(_tmp165,
sizeof(void*),_tmp166));goto _LL83;_LL86:;_LL87: {struct Cyc_Core_Impossible_struct
_tmp54E;const char*_tmp54D;struct Cyc_Core_Impossible_struct*_tmp54C;(int)_throw((
void*)((_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C[0]=((_tmp54E.tag=Cyc_Core_Impossible,((
_tmp54E.f1=((_tmp54D="bad lookup_place",_tag_dyneither(_tmp54D,sizeof(char),17))),
_tmp54E)))),_tmp54C)))));}_LL83:;}return _tmp15F;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp16A=
rval;void*_tmp16D;int _tmp16F;struct _dyneither_ptr _tmp170;_LL89: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp16B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp16A;if(_tmp16B->tag != 4)goto _LL8B;}
_LL8A: return 0;_LL8B: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp16C=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp16A;if(_tmp16C->tag != 7)goto _LL8D;else{_tmp16D=(void*)_tmp16C->f3;}}_LL8C:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL8D: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp16E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16A;if(_tmp16E->tag != 6)goto
_LL8F;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;}}_LL8E: {unsigned int sz=
_get_dyneither_size(_tmp170,sizeof(void*));{int i=0;for(0;i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((
void**)_tmp170.curr)[i]))return 0;}}return 1;}_LL8F:;_LL90: return 1;_LL88:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp171=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp54F;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp54F=_region_malloc(_tmp171,sizeof(*_tmp54F)),((_tmp54F->rgn=_tmp171,((
_tmp54F->places=0,_tmp54F)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp559(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp558,unsigned int*_tmp557,void***_tmp555,struct
_dyneither_ptr*_tmp183,struct _dyneither_ptr*_tmp17F){for(*_tmp558=0;*_tmp558 < *
_tmp557;(*_tmp558)++){(*_tmp555)[*_tmp558]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp17F).curr)[(int)*_tmp558],*((void**)
_check_dyneither_subscript(*_tmp183,sizeof(void*),(int)*_tmp558)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp550;struct _tuple0 _tmp174=(_tmp550.f1=oldval,((
_tmp550.f2=newval,_tmp550)));void*_tmp175;void*_tmp177;struct Cyc_CfFlowInfo_Place*
_tmp179;void*_tmp17A;void*_tmp17C;int _tmp17E;struct _dyneither_ptr _tmp17F;void*
_tmp180;int _tmp182;struct _dyneither_ptr _tmp183;void*_tmp184;enum Cyc_CfFlowInfo_InitLevel
_tmp186;_LL92: _tmp175=_tmp174.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp176=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp175;if(_tmp176->tag != 4)goto _LL94;};_tmp177=
_tmp174.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp178=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp177;if(_tmp178->tag != 5)goto _LL94;else{_tmp179=_tmp178->f1;}};_LL93: Cyc_CfFlowInfo_add_place(
env->pile,_tmp179);goto _LL95;_LL94: _tmp17A=_tmp174.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp17B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp17A;if(_tmp17B->tag != 4)goto _LL96;};
_LL95: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp553;void*_tmp552;(_tmp552=0,Cyc_Tcutil_terr(env->loc,((_tmp553="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp553,sizeof(char),67))),_tag_dyneither(_tmp552,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL96: _tmp17C=_tmp174.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp17D=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp17C;if(_tmp17D->tag != 6)goto
_LL98;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}};_tmp180=_tmp174.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp181=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp180;if(_tmp181->tag != 6)goto _LL98;else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;}};
_LL97: {unsigned int _tmp558;unsigned int _tmp557;struct _dyneither_ptr _tmp556;void**
_tmp555;unsigned int _tmp554;struct _dyneither_ptr new_d=(_tmp554=
_get_dyneither_size(_tmp17F,sizeof(void*)),((_tmp555=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp554)),((_tmp556=_tag_dyneither(_tmp555,sizeof(
void*),_tmp554),((((_tmp557=_tmp554,_tmp559(& env,& _tmp558,& _tmp557,& _tmp555,&
_tmp183,& _tmp17F))),_tmp556)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp17F,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp17F.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp17F,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp183,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp55C;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp55B;return(void*)((_tmp55B=_region_malloc((env->fenv)->r,sizeof(*_tmp55B)),((
_tmp55B[0]=((_tmp55C.tag=6,((_tmp55C.f1=_tmp17E,((_tmp55C.f2=new_d,_tmp55C)))))),
_tmp55B))));};}_LL98: _tmp184=_tmp174.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp185=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp184;if(_tmp185->tag != 4)goto _LL9A;else{
_tmp186=_tmp185->f1;}};_LL99: switch(_tmp186){case Cyc_CfFlowInfo_NoneIL: _LL9C:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL9D: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL9E: return(env->fenv)->unknown_all;}_LL9A:;_LL9B:
return newval;_LL91:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval);static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmp55D;struct _tuple17 _tmp190=(_tmp55D.f1=fs,((_tmp55D.f2=oldval,
_tmp55D)));struct Cyc_List_List*_tmp191;struct Cyc_List_List _tmp192;int _tmp193;
struct Cyc_List_List*_tmp194;void*_tmp195;int _tmp197;struct _dyneither_ptr _tmp198;
_LLA1: _tmp191=_tmp190.f1;if(_tmp191 == 0)goto _LLA3;_tmp192=*_tmp191;_tmp193=(int)
_tmp192.hd;_tmp194=_tmp192.tl;_tmp195=_tmp190.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp196=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp195;if(_tmp196->tag != 6)goto
_LLA3;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}};_LLA2: {void*_tmp199=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp194,*((void**)_check_dyneither_subscript(_tmp198,sizeof(void*),_tmp193)),
newval);struct _dyneither_ptr _tmp19A=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp198,_tmp193,_tmp199);if((void**)_tmp19A.curr == (void**)_tmp198.curr)return
oldval;else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp560;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp55F;return(void*)((_tmp55F=_region_malloc((env->fenv)->r,sizeof(*_tmp55F)),((
_tmp55F[0]=((_tmp560.tag=6,((_tmp560.f1=_tmp197,((_tmp560.f2=_tmp19A,_tmp560)))))),
_tmp55F))));}}_LLA3:;_LLA4: {struct Cyc_Core_Impossible_struct _tmp566;const char*
_tmp565;struct Cyc_Core_Impossible_struct*_tmp564;(int)_throw((void*)((_tmp564=
_cycalloc(sizeof(*_tmp564)),((_tmp564[0]=((_tmp566.tag=Cyc_Core_Impossible,((
_tmp566.f1=((_tmp565="bad assign place",_tag_dyneither(_tmp565,sizeof(char),17))),
_tmp566)))),_tmp564)))));}_LLA0:;};}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp1A0=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp1A2;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_CfFlowInfo_Place*
_tmp1A1=place;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.root;_tmp1A4=_tmp1A2.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp569;struct Cyc_CfFlowInfo_AssignEnv _tmp568;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp568.fenv=fenv,((_tmp568.pile=((_tmp569=
_region_malloc(_tmp1A0,sizeof(*_tmp569)),((_tmp569->rgn=_tmp1A0,((_tmp569->places=
0,_tmp569)))))),((_tmp568.d=d,((_tmp568.loc=loc,_tmp568)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp1A4,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1A3),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1A3,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp579(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp578,unsigned int*_tmp577,void***_tmp575,struct
_dyneither_ptr*_tmp1ED,struct _dyneither_ptr*_tmp1E9){for(*_tmp578=0;*_tmp578 < *
_tmp577;(*_tmp578)++){(*_tmp575)[*_tmp578]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1E9).curr)[(
int)*_tmp578],*((void**)_check_dyneither_subscript(*_tmp1ED,sizeof(void*),(int)*
_tmp578)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1A7=(
env->fenv)->r;{struct _tuple0 _tmp56A;struct _tuple0 _tmp1A9=(_tmp56A.f1=r1,((
_tmp56A.f2=r2,_tmp56A)));void*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;int _tmp1AD;
void*_tmp1AE;void*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B1;int _tmp1B2;void*_tmp1B3;
void*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;int _tmp1B7;void*_tmp1B8;void*_tmp1B9;
struct Cyc_Absyn_Exp*_tmp1BB;int _tmp1BC;void*_tmp1BD;void*_tmp1BE;struct Cyc_CfFlowInfo_Place*
_tmp1C0;void*_tmp1C1;struct Cyc_CfFlowInfo_Place*_tmp1C3;void*_tmp1C4;struct Cyc_CfFlowInfo_Place*
_tmp1C6;void*_tmp1C7;void*_tmp1C9;void*_tmp1CB;struct Cyc_CfFlowInfo_Place*
_tmp1CD;void*_tmp1CE;struct Cyc_CfFlowInfo_Place*_tmp1D0;void*_tmp1D1;void*
_tmp1D3;void*_tmp1D5;struct Cyc_CfFlowInfo_Place*_tmp1D7;void*_tmp1D8;struct Cyc_CfFlowInfo_Place*
_tmp1DA;void*_tmp1DB;struct Cyc_CfFlowInfo_Place*_tmp1DD;void*_tmp1DE;void*
_tmp1E0;void*_tmp1E2;void*_tmp1E4;void*_tmp1E6;int _tmp1E8;struct _dyneither_ptr
_tmp1E9;void*_tmp1EA;int _tmp1EC;struct _dyneither_ptr _tmp1ED;_LLA6: _tmp1AA=
_tmp1A9.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1AB=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1AA;if(_tmp1AB->tag != 7)goto _LLA8;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;
_tmp1AE=(void*)_tmp1AB->f3;}};_tmp1AF=_tmp1A9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1B0=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1AF;if(_tmp1B0->tag != 7)goto
_LLA8;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;_tmp1B3=(void*)_tmp1B0->f3;}};
_LLA7: if(_tmp1AC == _tmp1B1  && _tmp1AD == _tmp1B2){struct Cyc_CfFlowInfo_Consumed_struct
_tmp56D;struct Cyc_CfFlowInfo_Consumed_struct*_tmp56C;return(void*)((_tmp56C=
_region_malloc(_tmp1A7,sizeof(*_tmp56C)),((_tmp56C[0]=((_tmp56D.tag=7,((_tmp56D.f1=
_tmp1AC,((_tmp56D.f2=_tmp1AD,((_tmp56D.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1AE,_tmp1B3),_tmp56D)))))))),_tmp56C))));}{void*_tmp1F0=_tmp1AE;struct Cyc_CfFlowInfo_Place*
_tmp1F2;_LLC3: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1F1=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1F0;if(_tmp1F1->tag != 5)goto _LLC5;else{_tmp1F2=_tmp1F1->f1;}}_LLC4: Cyc_CfFlowInfo_add_place(
env->pile,_tmp1F2);goto _LLC2;_LLC5:;_LLC6: goto _LLC2;_LLC2:;}{void*_tmp1F3=
_tmp1B3;struct Cyc_CfFlowInfo_Place*_tmp1F5;_LLC8: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F3;if(_tmp1F4->tag != 5)goto
_LLCA;else{_tmp1F5=_tmp1F4->f1;}}_LLC9: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1F5);goto _LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}goto _LLA5;_LLA8: _tmp1B4=
_tmp1A9.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1B5=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1B4;if(_tmp1B5->tag != 7)goto _LLAA;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;
_tmp1B8=(void*)_tmp1B5->f3;}};_LLA9: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp570;struct Cyc_CfFlowInfo_Consumed_struct*_tmp56F;return(void*)((_tmp56F=
_region_malloc(_tmp1A7,sizeof(*_tmp56F)),((_tmp56F[0]=((_tmp570.tag=7,((_tmp570.f1=
_tmp1B6,((_tmp570.f2=_tmp1B7,((_tmp570.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1B8,r2),_tmp570)))))))),_tmp56F))));}_LLAA: _tmp1B9=_tmp1A9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1BA=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1B9;if(_tmp1BA->tag != 7)goto
_LLAC;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;_tmp1BD=(void*)_tmp1BA->f3;}};
_LLAB: {struct Cyc_CfFlowInfo_Consumed_struct _tmp573;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp572;return(void*)((_tmp572=_region_malloc(_tmp1A7,sizeof(*_tmp572)),((
_tmp572[0]=((_tmp573.tag=7,((_tmp573.f1=_tmp1BB,((_tmp573.f2=_tmp1BC,((_tmp573.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1BD),_tmp573)))))))),_tmp572))));}
_LLAC: _tmp1BE=_tmp1A9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1BF=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1BE;if(_tmp1BF->tag != 5)goto _LLAE;else{
_tmp1C0=_tmp1BF->f1;}};_tmp1C1=_tmp1A9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1C2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1C1;if(_tmp1C2->tag != 5)goto
_LLAE;else{_tmp1C3=_tmp1C2->f1;}};_LLAD: if(Cyc_CfFlowInfo_place_cmp(_tmp1C0,
_tmp1C3)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1C0);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1C3);goto _LLA5;_LLAE: _tmp1C4=_tmp1A9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1C5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1C4;if(_tmp1C5->tag != 5)goto
_LLB0;else{_tmp1C6=_tmp1C5->f1;}};_tmp1C7=_tmp1A9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1C8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1C7;if(_tmp1C8->tag != 2)
goto _LLB0;};_LLAF: _tmp1CD=_tmp1C6;goto _LLB1;_LLB0: _tmp1C9=_tmp1A9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1CA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1C9;if(_tmp1CA->tag != 2)
goto _LLB2;};_tmp1CB=_tmp1A9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1CC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1CB;if(_tmp1CC->tag != 5)goto _LLB2;
else{_tmp1CD=_tmp1CC->f1;}};_LLB1: Cyc_CfFlowInfo_add_place(env->pile,_tmp1CD);
return(env->fenv)->notzerothis;_LLB2: _tmp1CE=_tmp1A9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1CF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1CE;if(_tmp1CF->tag != 5)goto
_LLB4;else{_tmp1D0=_tmp1CF->f1;}};_tmp1D1=_tmp1A9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1D2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1D1;if(_tmp1D2->tag != 1)
goto _LLB4;};_LLB3: Cyc_CfFlowInfo_add_place(env->pile,_tmp1D0);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLCC: return(env->fenv)->notzeroall;
default: _LLCD: return(env->fenv)->notzerothis;}_LLB4: _tmp1D3=_tmp1A9.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1D4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1D3;if(_tmp1D4->tag != 1)goto _LLB6;};_tmp1D5=_tmp1A9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1D6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D5;if(_tmp1D6->tag != 5)goto
_LLB6;else{_tmp1D7=_tmp1D6->f1;}};_LLB5: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1D7);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLCF: return(env->fenv)->notzeroall;default: _LLD0: return(env->fenv)->notzerothis;}
_LLB6: _tmp1D8=_tmp1A9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1D9=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D8;if(_tmp1D9->tag != 5)goto _LLB8;else{
_tmp1DA=_tmp1D9->f1;}};_LLB7: Cyc_CfFlowInfo_add_place(env->pile,_tmp1DA);goto
_LLA5;_LLB8: _tmp1DB=_tmp1A9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1DC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DB;if(_tmp1DC->tag != 5)goto _LLBA;
else{_tmp1DD=_tmp1DC->f1;}};_LLB9: Cyc_CfFlowInfo_add_place(env->pile,_tmp1DD);
goto _LLA5;_LLBA: _tmp1DE=_tmp1A9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1DF=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1DE;if(_tmp1DF->tag != 1)
goto _LLBC;};_tmp1E0=_tmp1A9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp1E1=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1E0;if(_tmp1E1->tag != 2)goto _LLBC;};
_LLBB: goto _LLBD;_LLBC: _tmp1E2=_tmp1A9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1E3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1E2;if(_tmp1E3->tag != 2)
goto _LLBE;};_tmp1E4=_tmp1A9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1E5=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1E4;if(_tmp1E5->tag != 1)goto _LLBE;};
_LLBD: return(env->fenv)->notzerothis;_LLBE: _tmp1E6=_tmp1A9.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1E7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1E6;if(_tmp1E7->tag != 6)goto
_LLC0;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}};_tmp1EA=_tmp1A9.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1EB=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1EA;if(_tmp1EB->tag != 6)goto _LLC0;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;}};
_LLBF: {unsigned int _tmp578;unsigned int _tmp577;struct _dyneither_ptr _tmp576;void**
_tmp575;unsigned int _tmp574;struct _dyneither_ptr new_d=(_tmp574=
_get_dyneither_size(_tmp1E9,sizeof(void*)),((_tmp575=(void**)_region_malloc(
_tmp1A7,_check_times(sizeof(void*),_tmp574)),((_tmp576=_tag_dyneither(_tmp575,
sizeof(void*),_tmp574),((((_tmp577=_tmp574,_tmp579(& env,& _tmp578,& _tmp577,&
_tmp575,& _tmp1ED,& _tmp1E9))),_tmp576)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1E9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1E9.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1E9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1ED,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp57C;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp57B;return(void*)((_tmp57B=_region_malloc(_tmp1A7,sizeof(*_tmp57B)),((
_tmp57B[0]=((_tmp57C.tag=6,((_tmp57C.f1=_tmp1E8,((_tmp57C.f2=new_d,_tmp57C)))))),
_tmp57B))));};}_LLC0:;_LLC1: goto _LLA5;_LLA5:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp57D;struct
_tuple0 _tmp201=(_tmp57D.f1=r1,((_tmp57D.f2=r2,_tmp57D)));void*_tmp202;void*
_tmp204;_LLD3: _tmp202=_tmp201.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp203=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp202;if(_tmp203->tag != 4)goto _LLD5;};_LLD4:
goto _LLD6;_LLD5: _tmp204=_tmp201.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp205=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp204;if(_tmp205->tag != 4)goto _LLD7;};_LLD6: {
struct _tuple19 _tmp57E;struct _tuple19 _tmp207=(_tmp57E.f1=il1,((_tmp57E.f2=il2,
_tmp57E)));enum Cyc_CfFlowInfo_InitLevel _tmp208;enum Cyc_CfFlowInfo_InitLevel
_tmp209;enum Cyc_CfFlowInfo_InitLevel _tmp20A;enum Cyc_CfFlowInfo_InitLevel
_tmp20B;_LLDA: _tmp208=_tmp207.f2;if(_tmp208 != Cyc_CfFlowInfo_NoneIL)goto _LLDC;
_LLDB: goto _LLDD;_LLDC: _tmp209=_tmp207.f1;if(_tmp209 != Cyc_CfFlowInfo_NoneIL)goto
_LLDE;_LLDD: return(env->fenv)->esc_none;_LLDE: _tmp20A=_tmp207.f2;if(_tmp20A != 
Cyc_CfFlowInfo_ThisIL)goto _LLE0;_LLDF: goto _LLE1;_LLE0: _tmp20B=_tmp207.f1;if(
_tmp20B != Cyc_CfFlowInfo_ThisIL)goto _LLE2;_LLE1: return(env->fenv)->esc_this;
_LLE2:;_LLE3: return(env->fenv)->esc_all;_LLD9:;}_LLD7:;_LLD8: {struct _tuple19
_tmp57F;struct _tuple19 _tmp20D=(_tmp57F.f1=il1,((_tmp57F.f2=il2,_tmp57F)));enum 
Cyc_CfFlowInfo_InitLevel _tmp20E;enum Cyc_CfFlowInfo_InitLevel _tmp20F;enum Cyc_CfFlowInfo_InitLevel
_tmp210;enum Cyc_CfFlowInfo_InitLevel _tmp211;_LLE5: _tmp20E=_tmp20D.f2;if(_tmp20E
!= Cyc_CfFlowInfo_NoneIL)goto _LLE7;_LLE6: goto _LLE8;_LLE7: _tmp20F=_tmp20D.f1;if(
_tmp20F != Cyc_CfFlowInfo_NoneIL)goto _LLE9;_LLE8: return(env->fenv)->unknown_none;
_LLE9: _tmp210=_tmp20D.f2;if(_tmp210 != Cyc_CfFlowInfo_ThisIL)goto _LLEB;_LLEA: goto
_LLEC;_LLEB: _tmp211=_tmp20D.f1;if(_tmp211 != Cyc_CfFlowInfo_ThisIL)goto _LLED;
_LLEC: return(env->fenv)->unknown_this;_LLED:;_LLEE: return(env->fenv)->unknown_all;
_LLE4:;}_LLD2:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp580;return((_tmp580.EqualConst).val=i,(((_tmp580.EqualConst).tag=
1,_tmp580)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple10 _tmp583;union Cyc_CfFlowInfo_RelnOp _tmp582;return((
_tmp582.LessVar).val=((_tmp583.f1=v,((_tmp583.f2=t,_tmp583)))),(((_tmp582.LessVar).tag=
2,_tmp582)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp584;return((_tmp584.LessNumelts).val=v,(((
_tmp584.LessNumelts).tag=3,_tmp584)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp585;return((_tmp585.LessConst).val=i,(((_tmp585.LessConst).tag=
4,_tmp585)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp
_tmp586;return((_tmp586.LessEqConst).val=i,(((_tmp586.LessEqConst).tag=6,_tmp586)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp587;return((_tmp587.LessEqNumelts).val=v,(((
_tmp587.LessEqNumelts).tag=5,_tmp587)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp588;struct _tuple20 _tmp21A=(
_tmp588.f1=r1,((_tmp588.f2=r2,_tmp588)));union Cyc_CfFlowInfo_RelnOp _tmp21B;
unsigned int _tmp21C;union Cyc_CfFlowInfo_RelnOp _tmp21D;unsigned int _tmp21E;union
Cyc_CfFlowInfo_RelnOp _tmp21F;struct _tuple10 _tmp220;struct Cyc_Absyn_Vardecl*
_tmp221;union Cyc_CfFlowInfo_RelnOp _tmp222;struct _tuple10 _tmp223;struct Cyc_Absyn_Vardecl*
_tmp224;union Cyc_CfFlowInfo_RelnOp _tmp225;struct Cyc_Absyn_Vardecl*_tmp226;union
Cyc_CfFlowInfo_RelnOp _tmp227;struct Cyc_Absyn_Vardecl*_tmp228;union Cyc_CfFlowInfo_RelnOp
_tmp229;unsigned int _tmp22A;union Cyc_CfFlowInfo_RelnOp _tmp22B;unsigned int
_tmp22C;union Cyc_CfFlowInfo_RelnOp _tmp22D;unsigned int _tmp22E;union Cyc_CfFlowInfo_RelnOp
_tmp22F;unsigned int _tmp230;union Cyc_CfFlowInfo_RelnOp _tmp231;struct Cyc_Absyn_Vardecl*
_tmp232;union Cyc_CfFlowInfo_RelnOp _tmp233;struct Cyc_Absyn_Vardecl*_tmp234;_LLF0:
_tmp21B=_tmp21A.f1;if((_tmp21B.EqualConst).tag != 1)goto _LLF2;_tmp21C=(
unsigned int)(_tmp21B.EqualConst).val;_tmp21D=_tmp21A.f2;if((_tmp21D.EqualConst).tag
!= 1)goto _LLF2;_tmp21E=(unsigned int)(_tmp21D.EqualConst).val;_LLF1: return
_tmp21C == _tmp21E;_LLF2: _tmp21F=_tmp21A.f1;if((_tmp21F.LessVar).tag != 2)goto
_LLF4;_tmp220=(struct _tuple10)(_tmp21F.LessVar).val;_tmp221=_tmp220.f1;_tmp222=
_tmp21A.f2;if((_tmp222.LessVar).tag != 2)goto _LLF4;_tmp223=(struct _tuple10)(
_tmp222.LessVar).val;_tmp224=_tmp223.f1;_LLF3: return _tmp221 == _tmp224;_LLF4:
_tmp225=_tmp21A.f1;if((_tmp225.LessNumelts).tag != 3)goto _LLF6;_tmp226=(struct Cyc_Absyn_Vardecl*)(
_tmp225.LessNumelts).val;_tmp227=_tmp21A.f2;if((_tmp227.LessNumelts).tag != 3)
goto _LLF6;_tmp228=(struct Cyc_Absyn_Vardecl*)(_tmp227.LessNumelts).val;_LLF5:
return _tmp226 == _tmp228;_LLF6: _tmp229=_tmp21A.f1;if((_tmp229.LessConst).tag != 4)
goto _LLF8;_tmp22A=(unsigned int)(_tmp229.LessConst).val;_tmp22B=_tmp21A.f2;if((
_tmp22B.LessConst).tag != 4)goto _LLF8;_tmp22C=(unsigned int)(_tmp22B.LessConst).val;
_LLF7: return _tmp22A == _tmp22C;_LLF8: _tmp22D=_tmp21A.f1;if((_tmp22D.LessEqConst).tag
!= 6)goto _LLFA;_tmp22E=(unsigned int)(_tmp22D.LessEqConst).val;_tmp22F=_tmp21A.f2;
if((_tmp22F.LessEqConst).tag != 6)goto _LLFA;_tmp230=(unsigned int)(_tmp22F.LessEqConst).val;
_LLF9: return _tmp22E == _tmp230;_LLFA: _tmp231=_tmp21A.f1;if((_tmp231.LessEqNumelts).tag
!= 5)goto _LLFC;_tmp232=(struct Cyc_Absyn_Vardecl*)(_tmp231.LessEqNumelts).val;
_tmp233=_tmp21A.f2;if((_tmp233.LessEqNumelts).tag != 5)goto _LLFC;_tmp234=(struct
Cyc_Absyn_Vardecl*)(_tmp233.LessEqNumelts).val;_LLFB: return _tmp232 == _tmp234;
_LLFC:;_LLFD: return 0;_LLEF:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp235=r1s;for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
struct Cyc_CfFlowInfo_Reln*_tmp236=(struct Cyc_CfFlowInfo_Reln*)_tmp235->hd;int
found=0;{struct Cyc_List_List*_tmp237=r2s;for(0;_tmp237 != 0;_tmp237=_tmp237->tl){
struct Cyc_CfFlowInfo_Reln*_tmp238=(struct Cyc_CfFlowInfo_Reln*)_tmp237->hd;if(
_tmp236 == _tmp238  || _tmp236->vd == _tmp238->vd  && Cyc_CfFlowInfo_same_relop(
_tmp236->rop,_tmp238->rop)){{struct Cyc_List_List*_tmp589;res=((_tmp589=
_region_malloc(r,sizeof(*_tmp589)),((_tmp589->hd=_tmp236,((_tmp589->tl=res,
_tmp589))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;};}
struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp58A;struct _tuple21 _tmp23B=(_tmp58A.f1=f1,((_tmp58A.f2=f2,
_tmp58A)));union Cyc_CfFlowInfo_FlowInfo _tmp23C;int _tmp23D;union Cyc_CfFlowInfo_FlowInfo
_tmp23E;int _tmp23F;union Cyc_CfFlowInfo_FlowInfo _tmp240;struct _tuple11 _tmp241;
struct Cyc_Dict_Dict _tmp242;struct Cyc_List_List*_tmp243;union Cyc_CfFlowInfo_FlowInfo
_tmp244;struct _tuple11 _tmp245;struct Cyc_Dict_Dict _tmp246;struct Cyc_List_List*
_tmp247;_LLFF: _tmp23C=_tmp23B.f1;if((_tmp23C.BottomFL).tag != 1)goto _LL101;
_tmp23D=(int)(_tmp23C.BottomFL).val;_LL100: return f2;_LL101: _tmp23E=_tmp23B.f2;
if((_tmp23E.BottomFL).tag != 1)goto _LL103;_tmp23F=(int)(_tmp23E.BottomFL).val;
_LL102: return f1;_LL103: _tmp240=_tmp23B.f1;if((_tmp240.ReachableFL).tag != 2)goto
_LLFE;_tmp241=(struct _tuple11)(_tmp240.ReachableFL).val;_tmp242=_tmp241.f1;
_tmp243=_tmp241.f2;_tmp244=_tmp23B.f2;if((_tmp244.ReachableFL).tag != 2)goto _LLFE;
_tmp245=(struct _tuple11)(_tmp244.ReachableFL).val;_tmp246=_tmp245.f1;_tmp247=
_tmp245.f2;_LL104: if(_tmp242.t == _tmp246.t  && _tmp243 == _tmp247)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp248=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp58D;struct Cyc_CfFlowInfo_JoinEnv
_tmp58C;struct Cyc_CfFlowInfo_JoinEnv _tmp249=(_tmp58C.fenv=fenv,((_tmp58C.pile=((
_tmp58D=_region_malloc(_tmp248,sizeof(*_tmp58D)),((_tmp58D->rgn=_tmp248,((
_tmp58D->places=0,_tmp58D)))))),((_tmp58C.d1=_tmp242,((_tmp58C.d2=_tmp246,
_tmp58C)))))));struct Cyc_Dict_Dict _tmp24A=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp249,_tmp242,_tmp246);struct Cyc_List_List*_tmp24B=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp243,_tmp247);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp249.pile,all_changed,_tmp24A),_tmp24B);};_LLFE:;}struct _tuple22{union
Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo
f3;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2);struct
_tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){union Cyc_CfFlowInfo_FlowInfo
_tmp24F;void*_tmp250;struct _tuple12 _tmp24E=pr1;_tmp24F=_tmp24E.f1;_tmp250=
_tmp24E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp252;void*_tmp253;struct _tuple12
_tmp251=pr2;_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;{union Cyc_CfFlowInfo_FlowInfo
outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp24F,_tmp252);struct
_tuple22 _tmp58E;struct _tuple22 _tmp255=(_tmp58E.f1=_tmp24F,((_tmp58E.f2=_tmp252,((
_tmp58E.f3=outflow,_tmp58E)))));union Cyc_CfFlowInfo_FlowInfo _tmp256;int _tmp257;
union Cyc_CfFlowInfo_FlowInfo _tmp258;int _tmp259;union Cyc_CfFlowInfo_FlowInfo
_tmp25A;struct _tuple11 _tmp25B;struct Cyc_Dict_Dict _tmp25C;union Cyc_CfFlowInfo_FlowInfo
_tmp25D;struct _tuple11 _tmp25E;struct Cyc_Dict_Dict _tmp25F;union Cyc_CfFlowInfo_FlowInfo
_tmp260;struct _tuple11 _tmp261;struct Cyc_Dict_Dict _tmp262;struct Cyc_List_List*
_tmp263;_LL106: _tmp256=_tmp255.f1;if((_tmp256.BottomFL).tag != 1)goto _LL108;
_tmp257=(int)(_tmp256.BottomFL).val;_LL107: {struct _tuple12 _tmp58F;return(
_tmp58F.f1=outflow,((_tmp58F.f2=_tmp253,_tmp58F)));}_LL108: _tmp258=_tmp255.f2;
if((_tmp258.BottomFL).tag != 1)goto _LL10A;_tmp259=(int)(_tmp258.BottomFL).val;
_LL109: {struct _tuple12 _tmp590;return(_tmp590.f1=outflow,((_tmp590.f2=_tmp250,
_tmp590)));}_LL10A: _tmp25A=_tmp255.f1;if((_tmp25A.ReachableFL).tag != 2)goto
_LL10C;_tmp25B=(struct _tuple11)(_tmp25A.ReachableFL).val;_tmp25C=_tmp25B.f1;
_tmp25D=_tmp255.f2;if((_tmp25D.ReachableFL).tag != 2)goto _LL10C;_tmp25E=(struct
_tuple11)(_tmp25D.ReachableFL).val;_tmp25F=_tmp25E.f1;_tmp260=_tmp255.f3;if((
_tmp260.ReachableFL).tag != 2)goto _LL10C;_tmp261=(struct _tuple11)(_tmp260.ReachableFL).val;
_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;_LL10B: if(((int(*)(int ignore,void*r1,void*
r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp250,_tmp253)){struct _tuple12
_tmp591;return(_tmp591.f1=outflow,((_tmp591.f2=_tmp253,_tmp591)));}if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp253,
_tmp250)){struct _tuple12 _tmp592;return(_tmp592.f1=outflow,((_tmp592.f2=_tmp250,
_tmp592)));}{struct _RegionHandle*_tmp268=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp595;struct Cyc_CfFlowInfo_JoinEnv _tmp594;struct Cyc_CfFlowInfo_JoinEnv _tmp269=(
_tmp594.fenv=fenv,((_tmp594.pile=((_tmp595=_region_malloc(_tmp268,sizeof(*
_tmp595)),((_tmp595->rgn=_tmp268,((_tmp595->places=0,_tmp595)))))),((_tmp594.d1=
_tmp25C,((_tmp594.d2=_tmp25F,_tmp594)))))));void*_tmp26A=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp269,0,_tmp250,
_tmp253);struct _tuple12 _tmp596;return(_tmp596.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp269.pile,all_changed,_tmp262),_tmp263),((_tmp596.f2=_tmp26A,_tmp596)));};
_LL10C:;_LL10D: {struct Cyc_Core_Impossible_struct _tmp59C;const char*_tmp59B;
struct Cyc_Core_Impossible_struct*_tmp59A;(int)_throw((void*)((_tmp59A=_cycalloc(
sizeof(*_tmp59A)),((_tmp59A[0]=((_tmp59C.tag=Cyc_Core_Impossible,((_tmp59C.f1=((
_tmp59B="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp59B,sizeof(
char),37))),_tmp59C)))),_tmp59A)))));}_LL105:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5A3(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5A2,unsigned int*_tmp5A1,void***_tmp59F,struct
_dyneither_ptr*_tmp27E,struct _dyneither_ptr*_tmp27A){for(*_tmp5A2=0;*_tmp5A2 < *
_tmp5A1;(*_tmp5A2)++){(*_tmp59F)[*_tmp5A2]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5A2,((void**)(*_tmp27A).curr)[(int)*_tmp5A2],*((void**)
_check_dyneither_subscript(*_tmp27E,sizeof(void*),(int)*_tmp5A2)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp59D;
struct _tuple0 _tmp272=(_tmp59D.f1=r1,((_tmp59D.f2=r2,_tmp59D)));void*_tmp273;void*
_tmp275;void*_tmp277;int _tmp279;struct _dyneither_ptr _tmp27A;void*_tmp27B;int
_tmp27D;struct _dyneither_ptr _tmp27E;_LL10F: _tmp273=_tmp272.f1;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp274=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp273;if(_tmp274->tag != 7)goto
_LL111;};_LL110: goto _LL112;_LL111: _tmp275=_tmp272.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp276=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp275;if(_tmp276->tag != 7)goto
_LL113;};_LL112: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL113: _tmp277=
_tmp272.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp278=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp277;if(_tmp278->tag != 6)goto _LL115;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}};
_tmp27B=_tmp272.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp27C=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp27B;if(_tmp27C->tag != 6)goto _LL115;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}};
_LL114: {unsigned int _tmp5A2;unsigned int _tmp5A1;struct _dyneither_ptr _tmp5A0;
void**_tmp59F;unsigned int _tmp59E;struct _dyneither_ptr new_d=(_tmp59E=
_get_dyneither_size(_tmp27A,sizeof(void*)),((_tmp59F=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp59E)),((_tmp5A0=
_tag_dyneither(_tmp59F,sizeof(void*),_tmp59E),((((_tmp5A1=_tmp59E,_tmp5A3(& env,&
_tmp5A2,& _tmp5A1,& _tmp59F,& _tmp27E,& _tmp27A))),_tmp5A0)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp27A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp27A.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp27A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp27E,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5A6;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A5;return(void*)((_tmp5A5=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp5A5)),((_tmp5A5[0]=((_tmp5A6.tag=6,((_tmp5A6.f1=_tmp279,((_tmp5A6.f2=new_d,
_tmp5A6)))))),_tmp5A5))));};}_LL115:;_LL116: {struct Cyc_Core_Impossible_struct
_tmp5AC;const char*_tmp5AB;struct Cyc_Core_Impossible_struct*_tmp5AA;(int)_throw((
void*)((_tmp5AA=_cycalloc(sizeof(*_tmp5AA)),((_tmp5AA[0]=((_tmp5AC.tag=Cyc_Core_Impossible,((
_tmp5AC.f1=((_tmp5AB="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5AB,
sizeof(char),33))),_tmp5AC)))),_tmp5AA)))));}_LL10E:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp288=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp289=env->changed;{struct Cyc_List_List*_tmp5AD;*env->last_field_cell=((
_tmp5AD=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5AD)),((_tmp5AD->hd=(
void*)key,((_tmp5AD->tl=0,_tmp5AD))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp28B=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp288;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp289;return _tmp28B;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp5AE;*env->curr_place=((_tmp5AE.root=root,((_tmp5AE.fields=
0,_tmp5AE))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple21 _tmp5AF;struct _tuple21
_tmp28E=(_tmp5AF.f1=f1,((_tmp5AF.f2=f2,_tmp5AF)));union Cyc_CfFlowInfo_FlowInfo
_tmp28F;int _tmp290;union Cyc_CfFlowInfo_FlowInfo _tmp291;int _tmp292;union Cyc_CfFlowInfo_FlowInfo
_tmp293;struct _tuple11 _tmp294;struct Cyc_Dict_Dict _tmp295;struct Cyc_List_List*
_tmp296;union Cyc_CfFlowInfo_FlowInfo _tmp297;struct _tuple11 _tmp298;struct Cyc_Dict_Dict
_tmp299;struct Cyc_List_List*_tmp29A;_LL118: _tmp28F=_tmp28E.f1;if((_tmp28F.BottomFL).tag
!= 1)goto _LL11A;_tmp290=(int)(_tmp28F.BottomFL).val;_LL119: goto _LL11B;_LL11A:
_tmp291=_tmp28E.f2;if((_tmp291.BottomFL).tag != 1)goto _LL11C;_tmp292=(int)(
_tmp291.BottomFL).val;_LL11B: return Cyc_CfFlowInfo_BottomFL();_LL11C: _tmp293=
_tmp28E.f1;if((_tmp293.ReachableFL).tag != 2)goto _LL117;_tmp294=(struct _tuple11)(
_tmp293.ReachableFL).val;_tmp295=_tmp294.f1;_tmp296=_tmp294.f2;_tmp297=_tmp28E.f2;
if((_tmp297.ReachableFL).tag != 2)goto _LL117;_tmp298=(struct _tuple11)(_tmp297.ReachableFL).val;
_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;_LL11D: if(_tmp295.t == _tmp299.t  && _tmp296
== _tmp29A)return f1;{struct _RegionHandle*_tmp29B=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp29C=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp5B5;struct Cyc_CfFlowInfo_EscPile*
_tmp5B4;struct Cyc_CfFlowInfo_AfterEnv _tmp5B3;struct Cyc_CfFlowInfo_AfterEnv
_tmp29D=(_tmp5B3.joinenv=((_tmp5B5.fenv=fenv,((_tmp5B5.pile=((_tmp5B4=
_region_malloc(_tmp29B,sizeof(*_tmp5B4)),((_tmp5B4->rgn=_tmp29B,((_tmp5B4->places=
0,_tmp5B4)))))),((_tmp5B5.d1=_tmp295,((_tmp5B5.d2=_tmp299,_tmp5B5)))))))),((
_tmp5B3.chg1=chg1,((_tmp5B3.chg2=chg2,((_tmp5B3.curr_place=_tmp29C,((_tmp5B3.last_field_cell=(
struct Cyc_List_List**)& _tmp29C->fields,((_tmp5B3.changed=Cyc_CfFlowInfo_Neither,
_tmp5B3)))))))))));struct Cyc_Dict_Dict _tmp29E=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp29D,_tmp295,_tmp299);struct Cyc_List_List*_tmp29F=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp296,_tmp29A);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp29D.joinenv).pile,all_changed,_tmp29E),_tmp29F);};_LL117:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp5B6;struct
_tuple0 _tmp2A4=(_tmp5B6.f1=r1,((_tmp5B6.f2=r2,_tmp5B6)));void*_tmp2A5;struct Cyc_CfFlowInfo_Place*
_tmp2A7;void*_tmp2A8;struct Cyc_CfFlowInfo_Place*_tmp2AA;void*_tmp2AB;void*
_tmp2AD;void*_tmp2AF;int _tmp2B1;struct _dyneither_ptr _tmp2B2;void*_tmp2B3;int
_tmp2B5;struct _dyneither_ptr _tmp2B6;void*_tmp2B7;void*_tmp2B9;void*_tmp2BB;void*
_tmp2BD;void*_tmp2BF;void*_tmp2C1;void*_tmp2C3;void*_tmp2C5;_LL11F: _tmp2A5=
_tmp2A4.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2A6=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A5;if(_tmp2A6->tag != 5)goto _LL121;else{_tmp2A7=_tmp2A6->f1;}};_tmp2A8=
_tmp2A4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2A9=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A8;if(_tmp2A9->tag != 5)goto _LL121;else{_tmp2AA=_tmp2A9->f1;}};_LL120: return
Cyc_CfFlowInfo_place_cmp(_tmp2A7,_tmp2AA)== 0;_LL121: _tmp2AB=_tmp2A4.f1;{struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp2AC=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2AB;if(_tmp2AC->tag != 5)goto _LL123;};_LL122: goto _LL124;_LL123: _tmp2AD=
_tmp2A4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2AE=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2AD;if(_tmp2AE->tag != 5)goto _LL125;};_LL124: return 0;_LL125: _tmp2AF=_tmp2A4.f1;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2B0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2AF;if(_tmp2B0->tag != 6)goto _LL127;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;}};
_tmp2B3=_tmp2A4.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2B3;if(_tmp2B4->tag != 6)goto _LL127;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}};
_LL126: if((void**)_tmp2B2.curr == (void**)_tmp2B6.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2B2,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2B2.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2B6,sizeof(void*),i))))return 0;}}return 1;
_LL127: _tmp2B7=_tmp2A4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2B8=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2B7;if(_tmp2B8->tag != 1)goto _LL129;};
_tmp2B9=_tmp2A4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2BA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2B9;if(_tmp2BA->tag != 2)goto _LL129;};_LL128: return 1;_LL129: _tmp2BB=_tmp2A4.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2BC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2BB;if(_tmp2BC->tag != 2)goto _LL12B;};_LL12A: return 0;_LL12B: _tmp2BD=_tmp2A4.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2BE=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2BD;if(_tmp2BE->tag != 0)goto _LL12D;};_LL12C: goto _LL12E;_LL12D: _tmp2BF=
_tmp2A4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2C0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2BF;if(_tmp2C0->tag != 1)goto _LL12F;};_LL12E: return 0;_LL12F: _tmp2C1=_tmp2A4.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2C2=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2C1;if(_tmp2C2->tag != 4)goto _LL131;};_tmp2C3=_tmp2A4.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2C4=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2C3;if(_tmp2C4->tag != 4)goto _LL131;};
_LL130: goto _LL11E;_LL131: _tmp2C5=_tmp2A4.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2C6=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2C5;if(_tmp2C6->tag != 4)goto _LL133;};
_LL132: return 0;_LL133:;_LL134: goto _LL11E;_LL11E:;}{struct _tuple19 _tmp5B7;struct
_tuple19 _tmp2C8=(_tmp5B7.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5B7.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp5B7)));enum Cyc_CfFlowInfo_InitLevel _tmp2C9;enum Cyc_CfFlowInfo_InitLevel
_tmp2CA;enum Cyc_CfFlowInfo_InitLevel _tmp2CB;enum Cyc_CfFlowInfo_InitLevel
_tmp2CC;enum Cyc_CfFlowInfo_InitLevel _tmp2CD;enum Cyc_CfFlowInfo_InitLevel
_tmp2CE;_LL136: _tmp2C9=_tmp2C8.f1;if(_tmp2C9 != Cyc_CfFlowInfo_AllIL)goto _LL138;
_tmp2CA=_tmp2C8.f2;if(_tmp2CA != Cyc_CfFlowInfo_AllIL)goto _LL138;_LL137: return 1;
_LL138: _tmp2CB=_tmp2C8.f2;if(_tmp2CB != Cyc_CfFlowInfo_NoneIL)goto _LL13A;_LL139:
return 1;_LL13A: _tmp2CC=_tmp2C8.f1;if(_tmp2CC != Cyc_CfFlowInfo_NoneIL)goto _LL13C;
_LL13B: return 0;_LL13C: _tmp2CD=_tmp2C8.f2;if(_tmp2CD != Cyc_CfFlowInfo_ThisIL)goto
_LL13E;_LL13D: return 1;_LL13E: _tmp2CE=_tmp2C8.f1;if(_tmp2CE != Cyc_CfFlowInfo_ThisIL)
goto _LL135;_LL13F: return 0;_LL135:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2CF=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp2D0=r2s;for(0;_tmp2D0 != 0;_tmp2D0=
_tmp2D0->tl){struct Cyc_CfFlowInfo_Reln*_tmp2D1=(struct Cyc_CfFlowInfo_Reln*)
_tmp2D0->hd;if(_tmp2CF == _tmp2D1  || _tmp2CF->vd == _tmp2D1->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2CF->rop,_tmp2D1->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp5B8;struct _tuple21 _tmp2D3=(_tmp5B8.f1=f1,((_tmp5B8.f2=f2,_tmp5B8)));union Cyc_CfFlowInfo_FlowInfo
_tmp2D4;int _tmp2D5;union Cyc_CfFlowInfo_FlowInfo _tmp2D6;int _tmp2D7;union Cyc_CfFlowInfo_FlowInfo
_tmp2D8;struct _tuple11 _tmp2D9;struct Cyc_Dict_Dict _tmp2DA;struct Cyc_List_List*
_tmp2DB;union Cyc_CfFlowInfo_FlowInfo _tmp2DC;struct _tuple11 _tmp2DD;struct Cyc_Dict_Dict
_tmp2DE;struct Cyc_List_List*_tmp2DF;_LL141: _tmp2D4=_tmp2D3.f1;if((_tmp2D4.BottomFL).tag
!= 1)goto _LL143;_tmp2D5=(int)(_tmp2D4.BottomFL).val;_LL142: return 1;_LL143:
_tmp2D6=_tmp2D3.f2;if((_tmp2D6.BottomFL).tag != 1)goto _LL145;_tmp2D7=(int)(
_tmp2D6.BottomFL).val;_LL144: return 0;_LL145: _tmp2D8=_tmp2D3.f1;if((_tmp2D8.ReachableFL).tag
!= 2)goto _LL140;_tmp2D9=(struct _tuple11)(_tmp2D8.ReachableFL).val;_tmp2DA=
_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;_tmp2DC=_tmp2D3.f2;if((_tmp2DC.ReachableFL).tag != 
2)goto _LL140;_tmp2DD=(struct _tuple11)(_tmp2DC.ReachableFL).val;_tmp2DE=_tmp2DD.f1;
_tmp2DF=_tmp2DD.f2;_LL146: if(_tmp2DA.t == _tmp2DE.t  && _tmp2DB == _tmp2DF)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2DA,_tmp2DE) && Cyc_CfFlowInfo_relns_approx(_tmp2DB,_tmp2DF);_LL140:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2E0=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2E0->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp2E1=_tmp2E0->rop;struct _tuple10 _tmp2E2;struct Cyc_Absyn_Vardecl*
_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E4;struct Cyc_Absyn_Vardecl*_tmp2E5;_LL148:
if((_tmp2E1.LessVar).tag != 2)goto _LL14A;_tmp2E2=(struct _tuple10)(_tmp2E1.LessVar).val;
_tmp2E3=_tmp2E2.f1;_LL149: _tmp2E4=_tmp2E3;goto _LL14B;_LL14A: if((_tmp2E1.LessNumelts).tag
!= 3)goto _LL14C;_tmp2E4=(struct Cyc_Absyn_Vardecl*)(_tmp2E1.LessNumelts).val;
_LL14B: _tmp2E5=_tmp2E4;goto _LL14D;_LL14C: if((_tmp2E1.LessEqNumelts).tag != 5)goto
_LL14E;_tmp2E5=(struct Cyc_Absyn_Vardecl*)(_tmp2E1.LessEqNumelts).val;_LL14D: if(v
== _tmp2E5)found=1;goto _LL147;_LL14E:;_LL14F: goto _LL147;_LL147:;};}if(!found)
return rs;{struct Cyc_List_List*_tmp2E6=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2E7=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2E7->vd != v){{union Cyc_CfFlowInfo_RelnOp
_tmp2E8=_tmp2E7->rop;struct _tuple10 _tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EA;
struct Cyc_Absyn_Vardecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EC;_LL151: if((
_tmp2E8.LessVar).tag != 2)goto _LL153;_tmp2E9=(struct _tuple10)(_tmp2E8.LessVar).val;
_tmp2EA=_tmp2E9.f1;_LL152: _tmp2EB=_tmp2EA;goto _LL154;_LL153: if((_tmp2E8.LessNumelts).tag
!= 3)goto _LL155;_tmp2EB=(struct Cyc_Absyn_Vardecl*)(_tmp2E8.LessNumelts).val;
_LL154: _tmp2EC=_tmp2EB;goto _LL156;_LL155: if((_tmp2E8.LessEqNumelts).tag != 5)goto
_LL157;_tmp2EC=(struct Cyc_Absyn_Vardecl*)(_tmp2E8.LessEqNumelts).val;_LL156: if(v
== _tmp2EC)continue;goto _LL150;_LL157:;_LL158: goto _LL150;_LL150:;}{struct Cyc_List_List*
_tmp5B9;_tmp2E6=((_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9)),((_tmp5B9->hd=
_tmp2E7,((_tmp5B9->tl=_tmp2E6,_tmp5B9))))));};}}return _tmp2E6;};}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct
_RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp2EE=e->r;
void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F2;void*_tmp2F4;struct Cyc_Absyn_Vardecl*
_tmp2F6;void*_tmp2F8;struct Cyc_Absyn_Vardecl*_tmp2FA;void*_tmp2FC;struct Cyc_Absyn_Vardecl*
_tmp2FE;_LL15A: {struct Cyc_Absyn_Var_e_struct*_tmp2EF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2EE;if(_tmp2EF->tag != 1)goto _LL15C;else{_tmp2F0=(void*)_tmp2EF->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2F1=(struct Cyc_Absyn_Global_b_struct*)_tmp2F0;if(
_tmp2F1->tag != 1)goto _LL15C;else{_tmp2F2=_tmp2F1->f1;}};}}_LL15B: _tmp2F6=_tmp2F2;
goto _LL15D;_LL15C: {struct Cyc_Absyn_Var_e_struct*_tmp2F3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2EE;if(_tmp2F3->tag != 1)goto _LL15E;else{_tmp2F4=(void*)_tmp2F3->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2F5=(struct Cyc_Absyn_Param_b_struct*)_tmp2F4;if(
_tmp2F5->tag != 3)goto _LL15E;else{_tmp2F6=_tmp2F5->f1;}};}}_LL15D: _tmp2FA=_tmp2F6;
goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_struct*_tmp2F7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2EE;if(_tmp2F7->tag != 1)goto _LL160;else{_tmp2F8=(void*)_tmp2F7->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2F9=(struct Cyc_Absyn_Local_b_struct*)_tmp2F8;if(
_tmp2F9->tag != 4)goto _LL160;else{_tmp2FA=_tmp2F9->f1;}};}}_LL15F: _tmp2FE=_tmp2FA;
goto _LL161;_LL160: {struct Cyc_Absyn_Var_e_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2EE;if(_tmp2FB->tag != 1)goto _LL162;else{_tmp2FC=(void*)_tmp2FB->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2FD=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FC;if(_tmp2FD->tag
!= 5)goto _LL162;else{_tmp2FE=_tmp2FD->f1;}};}}_LL161: if(!_tmp2FE->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2FE);goto _LL159;_LL162:;_LL163: goto _LL159;
_LL159:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*
_tmp5BA;return(_tmp5BA=_region_malloc(r2,sizeof(*_tmp5BA)),((_tmp5BA[0]=*r,
_tmp5BA)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,
struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp300=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp301=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp300->vd != _tmp301->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp300->rop,_tmp301->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp302=e->r;struct Cyc_Absyn_MallocInfo _tmp304;struct Cyc_Absyn_Exp*
_tmp305;int _tmp306;_LL165: {struct Cyc_Absyn_Malloc_e_struct*_tmp303=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp302;if(_tmp303->tag != 34)goto _LL167;else{_tmp304=_tmp303->f1;_tmp305=_tmp304.num_elts;
_tmp306=_tmp304.fat_result;if(_tmp306 != 1)goto _LL167;}}_LL166: malloc_loop: {void*
_tmp307=_tmp305->r;struct Cyc_Absyn_Exp*_tmp309;void*_tmp30B;struct Cyc_Absyn_Vardecl*
_tmp30D;void*_tmp30F;struct Cyc_Absyn_Vardecl*_tmp311;void*_tmp313;struct Cyc_Absyn_Vardecl*
_tmp315;void*_tmp317;struct Cyc_Absyn_Vardecl*_tmp319;_LL16A: {struct Cyc_Absyn_Cast_e_struct*
_tmp308=(struct Cyc_Absyn_Cast_e_struct*)_tmp307;if(_tmp308->tag != 15)goto _LL16C;
else{_tmp309=_tmp308->f2;}}_LL16B: _tmp305=_tmp309;goto malloc_loop;_LL16C: {
struct Cyc_Absyn_Var_e_struct*_tmp30A=(struct Cyc_Absyn_Var_e_struct*)_tmp307;if(
_tmp30A->tag != 1)goto _LL16E;else{_tmp30B=(void*)_tmp30A->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp30C=(struct Cyc_Absyn_Pat_b_struct*)_tmp30B;if(_tmp30C->tag != 5)goto _LL16E;
else{_tmp30D=_tmp30C->f1;}};}}_LL16D: _tmp311=_tmp30D;goto _LL16F;_LL16E: {struct
Cyc_Absyn_Var_e_struct*_tmp30E=(struct Cyc_Absyn_Var_e_struct*)_tmp307;if(_tmp30E->tag
!= 1)goto _LL170;else{_tmp30F=(void*)_tmp30E->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp310=(struct Cyc_Absyn_Local_b_struct*)_tmp30F;if(_tmp310->tag != 4)goto _LL170;
else{_tmp311=_tmp310->f1;}};}}_LL16F: _tmp315=_tmp311;goto _LL171;_LL170: {struct
Cyc_Absyn_Var_e_struct*_tmp312=(struct Cyc_Absyn_Var_e_struct*)_tmp307;if(_tmp312->tag
!= 1)goto _LL172;else{_tmp313=(void*)_tmp312->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp314=(struct Cyc_Absyn_Param_b_struct*)_tmp313;if(_tmp314->tag != 3)goto _LL172;
else{_tmp315=_tmp314->f1;}};}}_LL171: _tmp319=_tmp315;goto _LL173;_LL172: {struct
Cyc_Absyn_Var_e_struct*_tmp316=(struct Cyc_Absyn_Var_e_struct*)_tmp307;if(_tmp316->tag
!= 1)goto _LL174;else{_tmp317=(void*)_tmp316->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp318=(struct Cyc_Absyn_Global_b_struct*)_tmp317;if(_tmp318->tag != 1)goto _LL174;
else{_tmp319=_tmp318->f1;}};}}_LL173: if(_tmp319->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp5BD;struct Cyc_List_List*_tmp5BC;return(_tmp5BC=_region_malloc(rgn,sizeof(*
_tmp5BC)),((_tmp5BC->hd=((_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD)),((_tmp5BD->vd=
_tmp319,((_tmp5BD->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp5BD)))))),((_tmp5BC->tl=
r,_tmp5BC)))));};_LL174:;_LL175: return r;_LL169:;}_LL167:;_LL168: goto _LL164;
_LL164:;}{void*_tmp31C=Cyc_Tcutil_compress(v->type);_LL177: {struct Cyc_Absyn_IntType_struct*
_tmp31D=(struct Cyc_Absyn_IntType_struct*)_tmp31C;if(_tmp31D->tag != 6)goto _LL179;}
_LL178: goto _LL176;_LL179:;_LL17A: return r;_LL176:;}loop: {void*_tmp31E=e->r;
struct Cyc_Absyn_Exp*_tmp320;union Cyc_Absyn_Cnst _tmp322;struct _tuple6 _tmp323;int
_tmp324;enum Cyc_Absyn_Primop _tmp326;struct Cyc_List_List*_tmp327;struct Cyc_List_List
_tmp328;struct Cyc_List_List*_tmp329;struct Cyc_List_List _tmp32A;struct Cyc_Absyn_Exp*
_tmp32B;enum Cyc_Absyn_Primop _tmp32D;struct Cyc_List_List*_tmp32E;struct Cyc_List_List
_tmp32F;struct Cyc_Absyn_Exp*_tmp330;_LL17C: {struct Cyc_Absyn_Cast_e_struct*
_tmp31F=(struct Cyc_Absyn_Cast_e_struct*)_tmp31E;if(_tmp31F->tag != 15)goto _LL17E;
else{_tmp320=_tmp31F->f2;}}_LL17D: e=_tmp320;goto loop;_LL17E: {struct Cyc_Absyn_Const_e_struct*
_tmp321=(struct Cyc_Absyn_Const_e_struct*)_tmp31E;if(_tmp321->tag != 0)goto _LL180;
else{_tmp322=_tmp321->f1;if((_tmp322.Int_c).tag != 4)goto _LL180;_tmp323=(struct
_tuple6)(_tmp322.Int_c).val;_tmp324=_tmp323.f2;}}_LL17F: {struct Cyc_CfFlowInfo_Reln*
_tmp5C0;struct Cyc_List_List*_tmp5BF;return(_tmp5BF=_region_malloc(rgn,sizeof(*
_tmp5BF)),((_tmp5BF->hd=((_tmp5C0=_region_malloc(rgn,sizeof(*_tmp5C0)),((_tmp5C0->vd=
v,((_tmp5C0->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp324),_tmp5C0)))))),((
_tmp5BF->tl=r,_tmp5BF)))));}_LL180: {struct Cyc_Absyn_Primop_e_struct*_tmp325=(
struct Cyc_Absyn_Primop_e_struct*)_tmp31E;if(_tmp325->tag != 3)goto _LL182;else{
_tmp326=_tmp325->f1;if(_tmp326 != Cyc_Absyn_Mod)goto _LL182;_tmp327=_tmp325->f2;
if(_tmp327 == 0)goto _LL182;_tmp328=*_tmp327;_tmp329=_tmp328.tl;if(_tmp329 == 0)
goto _LL182;_tmp32A=*_tmp329;_tmp32B=(struct Cyc_Absyn_Exp*)_tmp32A.hd;}}_LL181:{
void*_tmp333=_tmp32B->r;enum Cyc_Absyn_Primop _tmp335;struct Cyc_List_List*_tmp336;
struct Cyc_List_List _tmp337;struct Cyc_Absyn_Exp*_tmp338;_LL187: {struct Cyc_Absyn_Primop_e_struct*
_tmp334=(struct Cyc_Absyn_Primop_e_struct*)_tmp333;if(_tmp334->tag != 3)goto _LL189;
else{_tmp335=_tmp334->f1;_tmp336=_tmp334->f2;if(_tmp336 == 0)goto _LL189;_tmp337=*
_tmp336;_tmp338=(struct Cyc_Absyn_Exp*)_tmp337.hd;}}_LL188:{void*_tmp339=_tmp338->r;
void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*
_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*_tmp345;void*_tmp347;struct Cyc_Absyn_Vardecl*
_tmp349;_LL18C: {struct Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)
_tmp339;if(_tmp33A->tag != 1)goto _LL18E;else{_tmp33B=(void*)_tmp33A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp33C=(struct Cyc_Absyn_Global_b_struct*)_tmp33B;if(
_tmp33C->tag != 1)goto _LL18E;else{_tmp33D=_tmp33C->f1;}};}}_LL18D: _tmp341=_tmp33D;
goto _LL18F;_LL18E: {struct Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)
_tmp339;if(_tmp33E->tag != 1)goto _LL190;else{_tmp33F=(void*)_tmp33E->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp340=(struct Cyc_Absyn_Local_b_struct*)_tmp33F;if(
_tmp340->tag != 4)goto _LL190;else{_tmp341=_tmp340->f1;}};}}_LL18F: _tmp345=_tmp341;
goto _LL191;_LL190: {struct Cyc_Absyn_Var_e_struct*_tmp342=(struct Cyc_Absyn_Var_e_struct*)
_tmp339;if(_tmp342->tag != 1)goto _LL192;else{_tmp343=(void*)_tmp342->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp344=(struct Cyc_Absyn_Param_b_struct*)_tmp343;if(
_tmp344->tag != 3)goto _LL192;else{_tmp345=_tmp344->f1;}};}}_LL191: _tmp349=_tmp345;
goto _LL193;_LL192: {struct Cyc_Absyn_Var_e_struct*_tmp346=(struct Cyc_Absyn_Var_e_struct*)
_tmp339;if(_tmp346->tag != 1)goto _LL194;else{_tmp347=(void*)_tmp346->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp348=(struct Cyc_Absyn_Pat_b_struct*)_tmp347;if(_tmp348->tag
!= 5)goto _LL194;else{_tmp349=_tmp348->f1;}};}}_LL193: if(_tmp349->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5C3;struct Cyc_List_List*_tmp5C2;return(_tmp5C2=
_region_malloc(rgn,sizeof(*_tmp5C2)),((_tmp5C2->hd=((_tmp5C3=_region_malloc(rgn,
sizeof(*_tmp5C3)),((_tmp5C3->vd=v,((_tmp5C3->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp349),_tmp5C3)))))),((_tmp5C2->tl=r,_tmp5C2)))));};_LL194:;_LL195: goto _LL18B;
_LL18B:;}goto _LL186;_LL189:;_LL18A: goto _LL186;_LL186:;}goto _LL17B;_LL182: {
struct Cyc_Absyn_Primop_e_struct*_tmp32C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp31E;if(_tmp32C->tag != 3)goto _LL184;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;
if(_tmp32E == 0)goto _LL184;_tmp32F=*_tmp32E;_tmp330=(struct Cyc_Absyn_Exp*)_tmp32F.hd;}}
_LL183:{void*_tmp34C=_tmp330->r;void*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp350;
void*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;void*_tmp356;struct Cyc_Absyn_Vardecl*
_tmp358;void*_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35C;_LL197: {struct Cyc_Absyn_Var_e_struct*
_tmp34D=(struct Cyc_Absyn_Var_e_struct*)_tmp34C;if(_tmp34D->tag != 1)goto _LL199;
else{_tmp34E=(void*)_tmp34D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp34F=(struct
Cyc_Absyn_Global_b_struct*)_tmp34E;if(_tmp34F->tag != 1)goto _LL199;else{_tmp350=
_tmp34F->f1;}};}}_LL198: _tmp354=_tmp350;goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*
_tmp351=(struct Cyc_Absyn_Var_e_struct*)_tmp34C;if(_tmp351->tag != 1)goto _LL19B;
else{_tmp352=(void*)_tmp351->f2;{struct Cyc_Absyn_Local_b_struct*_tmp353=(struct
Cyc_Absyn_Local_b_struct*)_tmp352;if(_tmp353->tag != 4)goto _LL19B;else{_tmp354=
_tmp353->f1;}};}}_LL19A: _tmp358=_tmp354;goto _LL19C;_LL19B: {struct Cyc_Absyn_Var_e_struct*
_tmp355=(struct Cyc_Absyn_Var_e_struct*)_tmp34C;if(_tmp355->tag != 1)goto _LL19D;
else{_tmp356=(void*)_tmp355->f2;{struct Cyc_Absyn_Param_b_struct*_tmp357=(struct
Cyc_Absyn_Param_b_struct*)_tmp356;if(_tmp357->tag != 3)goto _LL19D;else{_tmp358=
_tmp357->f1;}};}}_LL19C: _tmp35C=_tmp358;goto _LL19E;_LL19D: {struct Cyc_Absyn_Var_e_struct*
_tmp359=(struct Cyc_Absyn_Var_e_struct*)_tmp34C;if(_tmp359->tag != 1)goto _LL19F;
else{_tmp35A=(void*)_tmp359->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp35B=(struct Cyc_Absyn_Pat_b_struct*)
_tmp35A;if(_tmp35B->tag != 5)goto _LL19F;else{_tmp35C=_tmp35B->f1;}};}}_LL19E: if(
_tmp35C->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp5C6;struct Cyc_List_List*
_tmp5C5;return(_tmp5C5=_region_malloc(rgn,sizeof(*_tmp5C5)),((_tmp5C5->hd=((
_tmp5C6=_region_malloc(rgn,sizeof(*_tmp5C6)),((_tmp5C6->vd=v,((_tmp5C6->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp35C),_tmp5C6)))))),((_tmp5C5->tl=r,_tmp5C5)))));};_LL19F:;_LL1A0: goto _LL196;
_LL196:;}goto _LL17B;_LL184:;_LL185: goto _LL17B;_LL17B:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp35F=e1->r;void*_tmp361;struct Cyc_Absyn_Vardecl*_tmp363;void*
_tmp365;struct Cyc_Absyn_Vardecl*_tmp367;void*_tmp369;struct Cyc_Absyn_Vardecl*
_tmp36B;void*_tmp36D;struct Cyc_Absyn_Vardecl*_tmp36F;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp360=(struct Cyc_Absyn_Var_e_struct*)_tmp35F;if(_tmp360->tag != 1)goto _LL1A4;
else{_tmp361=(void*)_tmp360->f2;{struct Cyc_Absyn_Global_b_struct*_tmp362=(struct
Cyc_Absyn_Global_b_struct*)_tmp361;if(_tmp362->tag != 1)goto _LL1A4;else{_tmp363=
_tmp362->f1;}};}}_LL1A3: _tmp367=_tmp363;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp364=(struct Cyc_Absyn_Var_e_struct*)_tmp35F;if(_tmp364->tag != 1)goto _LL1A6;
else{_tmp365=(void*)_tmp364->f2;{struct Cyc_Absyn_Param_b_struct*_tmp366=(struct
Cyc_Absyn_Param_b_struct*)_tmp365;if(_tmp366->tag != 3)goto _LL1A6;else{_tmp367=
_tmp366->f1;}};}}_LL1A5: _tmp36B=_tmp367;goto _LL1A7;_LL1A6: {struct Cyc_Absyn_Var_e_struct*
_tmp368=(struct Cyc_Absyn_Var_e_struct*)_tmp35F;if(_tmp368->tag != 1)goto _LL1A8;
else{_tmp369=(void*)_tmp368->f2;{struct Cyc_Absyn_Local_b_struct*_tmp36A=(struct
Cyc_Absyn_Local_b_struct*)_tmp369;if(_tmp36A->tag != 4)goto _LL1A8;else{_tmp36B=
_tmp36A->f1;}};}}_LL1A7: _tmp36F=_tmp36B;goto _LL1A9;_LL1A8: {struct Cyc_Absyn_Var_e_struct*
_tmp36C=(struct Cyc_Absyn_Var_e_struct*)_tmp35F;if(_tmp36C->tag != 1)goto _LL1AA;
else{_tmp36D=(void*)_tmp36C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp36E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp36D;if(_tmp36E->tag != 5)goto _LL1AA;else{_tmp36F=_tmp36E->f1;}};}}_LL1A9: if(!
_tmp36F->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp36F,e2);goto
_LL1A1;_LL1AA:;_LL1AB: goto _LL1A1;_LL1A1:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp5CA;void*_tmp5C9[1];struct Cyc_String_pa_struct _tmp5C8;(_tmp5C8.tag=
0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp5C9[0]=& _tmp5C8,Cyc_fprintf(Cyc_stderr,((_tmp5CA="%s",
_tag_dyneither(_tmp5CA,sizeof(char),3))),_tag_dyneither(_tmp5C9,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp373=r->rop;unsigned int _tmp374;struct _tuple10
_tmp375;struct Cyc_Absyn_Vardecl*_tmp376;struct Cyc_Absyn_Vardecl*_tmp377;
unsigned int _tmp378;unsigned int _tmp379;struct Cyc_Absyn_Vardecl*_tmp37A;_LL1AD:
if((_tmp373.EqualConst).tag != 1)goto _LL1AF;_tmp374=(unsigned int)(_tmp373.EqualConst).val;
_LL1AE:{const char*_tmp5CE;void*_tmp5CD[1];struct Cyc_Int_pa_struct _tmp5CC;(
_tmp5CC.tag=1,((_tmp5CC.f1=(unsigned long)((int)_tmp374),((_tmp5CD[0]=& _tmp5CC,
Cyc_fprintf(Cyc_stderr,((_tmp5CE="==%d",_tag_dyneither(_tmp5CE,sizeof(char),5))),
_tag_dyneither(_tmp5CD,sizeof(void*),1)))))));}goto _LL1AC;_LL1AF: if((_tmp373.LessVar).tag
!= 2)goto _LL1B1;_tmp375=(struct _tuple10)(_tmp373.LessVar).val;_tmp376=_tmp375.f1;
_LL1B0:{const char*_tmp5D2;void*_tmp5D1[1];struct Cyc_String_pa_struct _tmp5D0;(
_tmp5D0.tag=0,((_tmp5D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp376->name)),((_tmp5D1[0]=& _tmp5D0,Cyc_fprintf(Cyc_stderr,((_tmp5D2="<%s",
_tag_dyneither(_tmp5D2,sizeof(char),4))),_tag_dyneither(_tmp5D1,sizeof(void*),1)))))));}
goto _LL1AC;_LL1B1: if((_tmp373.LessNumelts).tag != 3)goto _LL1B3;_tmp377=(struct Cyc_Absyn_Vardecl*)(
_tmp373.LessNumelts).val;_LL1B2:{const char*_tmp5D6;void*_tmp5D5[1];struct Cyc_String_pa_struct
_tmp5D4;(_tmp5D4.tag=0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp377->name)),((_tmp5D5[0]=& _tmp5D4,Cyc_fprintf(Cyc_stderr,((
_tmp5D6="<numelts(%s)",_tag_dyneither(_tmp5D6,sizeof(char),13))),_tag_dyneither(
_tmp5D5,sizeof(void*),1)))))));}goto _LL1AC;_LL1B3: if((_tmp373.LessConst).tag != 4)
goto _LL1B5;_tmp378=(unsigned int)(_tmp373.LessConst).val;_LL1B4:{const char*
_tmp5DA;void*_tmp5D9[1];struct Cyc_Int_pa_struct _tmp5D8;(_tmp5D8.tag=1,((_tmp5D8.f1=(
unsigned long)((int)_tmp378),((_tmp5D9[0]=& _tmp5D8,Cyc_fprintf(Cyc_stderr,((
_tmp5DA="<%d",_tag_dyneither(_tmp5DA,sizeof(char),4))),_tag_dyneither(_tmp5D9,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B5: if((_tmp373.LessEqConst).tag != 6)goto
_LL1B7;_tmp379=(unsigned int)(_tmp373.LessEqConst).val;_LL1B6:{const char*_tmp5DE;
void*_tmp5DD[1];struct Cyc_Int_pa_struct _tmp5DC;(_tmp5DC.tag=1,((_tmp5DC.f1=(
unsigned long)((int)_tmp379),((_tmp5DD[0]=& _tmp5DC,Cyc_fprintf(Cyc_stderr,((
_tmp5DE="<=%d",_tag_dyneither(_tmp5DE,sizeof(char),5))),_tag_dyneither(_tmp5DD,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B7: if((_tmp373.LessEqNumelts).tag != 5)
goto _LL1AC;_tmp37A=(struct Cyc_Absyn_Vardecl*)(_tmp373.LessEqNumelts).val;_LL1B8:{
const char*_tmp5E2;void*_tmp5E1[1];struct Cyc_String_pa_struct _tmp5E0;(_tmp5E0.tag=
0,((_tmp5E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp37A->name)),((_tmp5E1[0]=& _tmp5E0,Cyc_fprintf(Cyc_stderr,((_tmp5E2="<=numelts(%s)",
_tag_dyneither(_tmp5E2,sizeof(char),14))),_tag_dyneither(_tmp5E1,sizeof(void*),1)))))));}
goto _LL1AC;_LL1AC:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp5E5;void*_tmp5E4;(
_tmp5E4=0,Cyc_fprintf(Cyc_stderr,((_tmp5E5=",",_tag_dyneither(_tmp5E5,sizeof(
char),2))),_tag_dyneither(_tmp5E4,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1B9:{const char*_tmp5E8;void*_tmp5E7;(
_tmp5E7=0,Cyc_fprintf(Cyc_stderr,((_tmp5E8="uninitialized",_tag_dyneither(
_tmp5E8,sizeof(char),14))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1BA:{const char*_tmp5EB;void*_tmp5EA;(_tmp5EA=0,Cyc_fprintf(
Cyc_stderr,((_tmp5EB="this-initialized",_tag_dyneither(_tmp5EB,sizeof(char),17))),
_tag_dyneither(_tmp5EA,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1BB:{
const char*_tmp5EE;void*_tmp5ED;(_tmp5ED=0,Cyc_fprintf(Cyc_stderr,((_tmp5EE="all-initialized",
_tag_dyneither(_tmp5EE,sizeof(char),16))),_tag_dyneither(_tmp5ED,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp395=root;struct Cyc_Absyn_Vardecl*_tmp397;struct Cyc_Absyn_Exp*
_tmp399;void*_tmp39A;_LL1BE: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp396=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp395;if(_tmp396->tag != 0)goto _LL1C0;
else{_tmp397=_tmp396->f1;}}_LL1BF:{const char*_tmp5F2;void*_tmp5F1[1];struct Cyc_String_pa_struct
_tmp5F0;(_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp397->name)),((_tmp5F1[0]=& _tmp5F0,Cyc_fprintf(Cyc_stderr,((
_tmp5F2="Root(%s)",_tag_dyneither(_tmp5F2,sizeof(char),9))),_tag_dyneither(
_tmp5F1,sizeof(void*),1)))))));}goto _LL1BD;_LL1C0: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp398=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp395;if(_tmp398->tag != 1)goto
_LL1C2;else{_tmp399=_tmp398->f1;_tmp39A=(void*)_tmp398->f2;}}_LL1C1:{const char*
_tmp5F7;void*_tmp5F6[2];struct Cyc_String_pa_struct _tmp5F5;struct Cyc_String_pa_struct
_tmp5F4;(_tmp5F4.tag=0,((_tmp5F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp39A)),((_tmp5F5.tag=0,((_tmp5F5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp399)),((_tmp5F6[
0]=& _tmp5F5,((_tmp5F6[1]=& _tmp5F4,Cyc_fprintf(Cyc_stderr,((_tmp5F7="MallocPt(%s,%s)",
_tag_dyneither(_tmp5F7,sizeof(char),16))),_tag_dyneither(_tmp5F6,sizeof(void*),2)))))))))))));}
goto _LL1BD;_LL1C2: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp39B=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp395;if(_tmp39B->tag != 2)goto _LL1BD;}_LL1C3:{const char*_tmp5FA;void*_tmp5F9;(
_tmp5F9=0,Cyc_fprintf(Cyc_stderr,((_tmp5FA="InitParam(_,_)",_tag_dyneither(
_tmp5FA,sizeof(char),15))),_tag_dyneither(_tmp5F9,sizeof(void*),0)));}goto _LL1BD;
_LL1BD:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*
_tmp3A5=p->fields;while(_tmp3A5 != 0){{const char*_tmp5FE;void*_tmp5FD[1];struct
Cyc_Int_pa_struct _tmp5FC;(_tmp5FC.tag=1,((_tmp5FC.f1=(unsigned long)((int)
_tmp3A5->hd),((_tmp5FD[0]=& _tmp5FC,Cyc_fprintf(Cyc_stderr,((_tmp5FE=".%d",
_tag_dyneither(_tmp5FE,sizeof(char),4))),_tag_dyneither(_tmp5FD,sizeof(void*),1)))))));}
_tmp3A5=_tmp3A5->tl;}{const char*_tmp601;void*_tmp600;(_tmp600=0,Cyc_fprintf(Cyc_stderr,((
_tmp601=" ",_tag_dyneither(_tmp601,sizeof(char),2))),_tag_dyneither(_tmp600,
sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*
pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp3AB=1;while(x != 0){if(!_tmp3AB){{const char*_tmp604;void*_tmp603;(_tmp603=
0,Cyc_fprintf(Cyc_stderr,((_tmp604=", ",_tag_dyneither(_tmp604,sizeof(char),3))),
_tag_dyneither(_tmp603,sizeof(void*),0)));}_tmp3AB=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp607;void*_tmp606;(_tmp606=0,Cyc_fprintf(Cyc_stderr,((_tmp607="\n",
_tag_dyneither(_tmp607,sizeof(char),2))),_tag_dyneither(_tmp606,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3B0=_new_region("r");
struct _RegionHandle*r=& _tmp3B0;_push_region(r);{struct _handler_cons _tmp3B1;
_push_handler(& _tmp3B1);{int _tmp3B3=0;if(setjmp(_tmp3B1.handler))_tmp3B3=1;if(!
_tmp3B3){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3B4=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp60A;void*_tmp609;(_tmp609=0,Cyc_fprintf(Cyc_stderr,((
_tmp60A="{ ",_tag_dyneither(_tmp60A,sizeof(char),3))),_tag_dyneither(_tmp609,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3B4)){void*_tmp3B7=_tmp3B4.f1;if(!first){{const char*_tmp60D;void*
_tmp60C;(_tmp60C=0,Cyc_fprintf(Cyc_stderr,((_tmp60D=", ",_tag_dyneither(_tmp60D,
sizeof(char),3))),_tag_dyneither(_tmp60C,sizeof(void*),0)));}first=0;}pr(_tmp3B7);}{
const char*_tmp610;void*_tmp60F;(_tmp60F=0,Cyc_fprintf(Cyc_stderr,((_tmp610="}\n",
_tag_dyneither(_tmp610,sizeof(char),3))),_tag_dyneither(_tmp60F,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3B2=(void*)_exn_thrown;void*_tmp3BD=_tmp3B2;_LL1C5:;
_LL1C6:{const char*_tmp613;void*_tmp612;(_tmp612=0,Cyc_fprintf(Cyc_stderr,((
_tmp613="{ }\n",_tag_dyneither(_tmp613,sizeof(char),5))),_tag_dyneither(_tmp612,
sizeof(void*),0)));}_npop_handler(0);return;_LL1C7:;_LL1C8:(void)_throw(_tmp3BD);
_LL1C4:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp3C0=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3C5;enum Cyc_CfFlowInfo_InitLevel
_tmp3C7;struct Cyc_CfFlowInfo_Place*_tmp3C9;int _tmp3CB;struct _dyneither_ptr
_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CE;int _tmp3CF;void*_tmp3D0;_LL1CA: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3C1=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3C0;if(_tmp3C1->tag != 0)goto
_LL1CC;}_LL1CB:{const char*_tmp616;void*_tmp615;(_tmp615=0,Cyc_fprintf(Cyc_stderr,((
_tmp616="Zero",_tag_dyneither(_tmp616,sizeof(char),5))),_tag_dyneither(_tmp615,
sizeof(void*),0)));}goto _LL1C9;_LL1CC: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3C2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3C0;if(_tmp3C2->tag != 1)
goto _LL1CE;}_LL1CD:{const char*_tmp619;void*_tmp618;(_tmp618=0,Cyc_fprintf(Cyc_stderr,((
_tmp619="NotZeroAll",_tag_dyneither(_tmp619,sizeof(char),11))),_tag_dyneither(
_tmp618,sizeof(void*),0)));}goto _LL1C9;_LL1CE: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3C3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3C0;if(_tmp3C3->tag != 2)
goto _LL1D0;}_LL1CF:{const char*_tmp61C;void*_tmp61B;(_tmp61B=0,Cyc_fprintf(Cyc_stderr,((
_tmp61C="NotZeroThis",_tag_dyneither(_tmp61C,sizeof(char),12))),_tag_dyneither(
_tmp61B,sizeof(void*),0)));}goto _LL1C9;_LL1D0: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3C4=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3C0;if(_tmp3C4->tag != 3)goto
_LL1D2;else{_tmp3C5=_tmp3C4->f1;}}_LL1D1:{const char*_tmp61F;void*_tmp61E;(
_tmp61E=0,Cyc_fprintf(Cyc_stderr,((_tmp61F="Unknown(",_tag_dyneither(_tmp61F,
sizeof(char),9))),_tag_dyneither(_tmp61E,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3C5);{const char*_tmp622;void*_tmp621;(_tmp621=0,Cyc_fprintf(Cyc_stderr,((
_tmp622=")",_tag_dyneither(_tmp622,sizeof(char),2))),_tag_dyneither(_tmp621,
sizeof(void*),0)));}goto _LL1C9;_LL1D2: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3C6=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3C0;if(_tmp3C6->tag != 4)goto _LL1D4;else{
_tmp3C7=_tmp3C6->f1;}}_LL1D3:{const char*_tmp625;void*_tmp624;(_tmp624=0,Cyc_fprintf(
Cyc_stderr,((_tmp625="Esc(",_tag_dyneither(_tmp625,sizeof(char),5))),
_tag_dyneither(_tmp624,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3C7);{const char*_tmp628;void*_tmp627;(_tmp627=0,Cyc_fprintf(Cyc_stderr,((
_tmp628=")",_tag_dyneither(_tmp628,sizeof(char),2))),_tag_dyneither(_tmp627,
sizeof(void*),0)));}goto _LL1C9;_LL1D4: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3C8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3C0;if(_tmp3C8->tag != 5)goto
_LL1D6;else{_tmp3C9=_tmp3C8->f1;}}_LL1D5:{const char*_tmp62B;void*_tmp62A;(
_tmp62A=0,Cyc_fprintf(Cyc_stderr,((_tmp62B="AddrOf(",_tag_dyneither(_tmp62B,
sizeof(char),8))),_tag_dyneither(_tmp62A,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3C9);{const char*_tmp62E;void*_tmp62D;(_tmp62D=0,Cyc_fprintf(Cyc_stderr,((
_tmp62E=")",_tag_dyneither(_tmp62E,sizeof(char),2))),_tag_dyneither(_tmp62D,
sizeof(void*),0)));}goto _LL1C9;_LL1D6: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3CA=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3C0;if(_tmp3CA->tag != 6)goto
_LL1D8;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL1D7: if(_tmp3CB){const
char*_tmp631;void*_tmp630;(_tmp630=0,Cyc_fprintf(Cyc_stderr,((_tmp631="AggrUnion{",
_tag_dyneither(_tmp631,sizeof(char),11))),_tag_dyneither(_tmp630,sizeof(void*),0)));}
else{const char*_tmp634;void*_tmp633;(_tmp633=0,Cyc_fprintf(Cyc_stderr,((_tmp634="AggrStruct{",
_tag_dyneither(_tmp634,sizeof(char),12))),_tag_dyneither(_tmp633,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp3CC,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3CC.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3CC,sizeof(void*))){
const char*_tmp637;void*_tmp636;(_tmp636=0,Cyc_fprintf(Cyc_stderr,((_tmp637=",",
_tag_dyneither(_tmp637,sizeof(char),2))),_tag_dyneither(_tmp636,sizeof(void*),0)));}}}{
const char*_tmp63A;void*_tmp639;(_tmp639=0,Cyc_fprintf(Cyc_stderr,((_tmp63A="}",
_tag_dyneither(_tmp63A,sizeof(char),2))),_tag_dyneither(_tmp639,sizeof(void*),0)));}
goto _LL1C9;_LL1D8: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3CD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3C0;if(_tmp3CD->tag != 7)goto _LL1C9;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;
_tmp3D0=(void*)_tmp3CD->f3;}}_LL1D9:{const char*_tmp63F;void*_tmp63E[2];struct Cyc_String_pa_struct
_tmp63D;struct Cyc_Int_pa_struct _tmp63C;(_tmp63C.tag=1,((_tmp63C.f1=(
unsigned long)_tmp3CF,((_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3CE)),((_tmp63E[0]=& _tmp63D,((
_tmp63E[1]=& _tmp63C,Cyc_fprintf(Cyc_stderr,((_tmp63F="[Consumed(%s,%d,",
_tag_dyneither(_tmp63F,sizeof(char),17))),_tag_dyneither(_tmp63E,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3D0);{const char*_tmp642;void*_tmp641;(_tmp641=0,
Cyc_fprintf(Cyc_stderr,((_tmp642="]",_tag_dyneither(_tmp642,sizeof(char),2))),
_tag_dyneither(_tmp641,sizeof(void*),0)));};_LL1C9:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval){{const char*_tmp645;void*_tmp644;(_tmp644=0,Cyc_fprintf(Cyc_stderr,((
_tmp645="    ",_tag_dyneither(_tmp645,sizeof(char),5))),_tag_dyneither(_tmp644,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp648;void*
_tmp647;(_tmp647=0,Cyc_fprintf(Cyc_stderr,((_tmp648=" --> ",_tag_dyneither(
_tmp648,sizeof(char),6))),_tag_dyneither(_tmp647,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_fprintf(Cyc_stderr,((
_tmp64B="\n",_tag_dyneither(_tmp64B,sizeof(char),2))),_tag_dyneither(_tmp64A,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((void(*)(void(*f)(void*,
void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp3F7=f;int _tmp3F8;
struct _tuple11 _tmp3F9;struct Cyc_Dict_Dict _tmp3FA;_LL1DB: if((_tmp3F7.BottomFL).tag
!= 1)goto _LL1DD;_tmp3F8=(int)(_tmp3F7.BottomFL).val;_LL1DC:{const char*_tmp64E;
void*_tmp64D;(_tmp64D=0,Cyc_fprintf(Cyc_stderr,((_tmp64E="  BottomFL\n",
_tag_dyneither(_tmp64E,sizeof(char),12))),_tag_dyneither(_tmp64D,sizeof(void*),0)));}
goto _LL1DA;_LL1DD: if((_tmp3F7.ReachableFL).tag != 2)goto _LL1DA;_tmp3F9=(struct
_tuple11)(_tmp3F7.ReachableFL).val;_tmp3FA=_tmp3F9.f1;_LL1DE:{const char*_tmp651;
void*_tmp650;(_tmp650=0,Cyc_fprintf(Cyc_stderr,((_tmp651="  ReachableFL:\n",
_tag_dyneither(_tmp651,sizeof(char),16))),_tag_dyneither(_tmp650,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp3FA);{const char*_tmp654;void*_tmp653;(_tmp653=
0,Cyc_fprintf(Cyc_stderr,((_tmp654="\n",_tag_dyneither(_tmp654,sizeof(char),2))),
_tag_dyneither(_tmp653,sizeof(void*),0)));}goto _LL1DA;_LL1DA:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp401=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp40D;struct Cyc_Absyn_DatatypeInfo
_tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_AggrInfo
_tmp414;struct Cyc_List_List*_tmp415;struct Cyc_Absyn_DatatypeFieldInfo _tmp417;
struct Cyc_List_List*_tmp418;struct Cyc_List_List*_tmp41A;struct Cyc_Absyn_PtrInfo
_tmp41C;void*_tmp41D;struct Cyc_Absyn_PtrAtts _tmp41E;void*_tmp41F;struct Cyc_List_List*
_tmp422;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_ArrayInfo _tmp426;void*
_tmp427;void*_tmp429;void*_tmp42B;void*_tmp42D;void*_tmp42E;void*_tmp430;_LL1E0: {
struct Cyc_Absyn_VoidType_struct*_tmp402=(struct Cyc_Absyn_VoidType_struct*)
_tmp401;if(_tmp402->tag != 0)goto _LL1E2;}_LL1E1: goto _LL1E3;_LL1E2: {struct Cyc_Absyn_IntType_struct*
_tmp403=(struct Cyc_Absyn_IntType_struct*)_tmp401;if(_tmp403->tag != 6)goto _LL1E4;}
_LL1E3: goto _LL1E5;_LL1E4: {struct Cyc_Absyn_FloatType_struct*_tmp404=(struct Cyc_Absyn_FloatType_struct*)
_tmp401;if(_tmp404->tag != 7)goto _LL1E6;}_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_DoubleType_struct*
_tmp405=(struct Cyc_Absyn_DoubleType_struct*)_tmp401;if(_tmp405->tag != 8)goto
_LL1E8;}_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_EnumType_struct*_tmp406=(
struct Cyc_Absyn_EnumType_struct*)_tmp401;if(_tmp406->tag != 14)goto _LL1EA;}_LL1E9:
goto _LL1EB;_LL1EA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp407=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp401;if(_tmp407->tag != 15)goto _LL1EC;}_LL1EB: goto _LL1ED;_LL1EC: {struct Cyc_Absyn_TagType_struct*
_tmp408=(struct Cyc_Absyn_TagType_struct*)_tmp401;if(_tmp408->tag != 20)goto _LL1EE;}
_LL1ED: goto _LL1EF;_LL1EE: {struct Cyc_Absyn_HeapRgn_struct*_tmp409=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp401;if(_tmp409->tag != 21)goto _LL1F0;}_LL1EF: goto _LL1F1;_LL1F0: {struct Cyc_Absyn_Evar_struct*
_tmp40A=(struct Cyc_Absyn_Evar_struct*)_tmp401;if(_tmp40A->tag != 1)goto _LL1F2;}
_LL1F1: return 0;_LL1F2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp40B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp401;if(_tmp40B->tag != 22)goto _LL1F4;}_LL1F3: {void*_tmp432=rgn;_LL215: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp433=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp432;if(_tmp433->tag != 0)goto _LL217;}_LL216: return 1;_LL217:;_LL218: return 0;
_LL214:;}_LL1F4: {struct Cyc_Absyn_VarType_struct*_tmp40C=(struct Cyc_Absyn_VarType_struct*)
_tmp401;if(_tmp40C->tag != 2)goto _LL1F6;else{_tmp40D=_tmp40C->f1;}}_LL1F5: {void*
_tmp434=rgn;struct Cyc_Absyn_Tvar*_tmp437;_LL21A: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp435=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp434;if(_tmp435->tag != 0)
goto _LL21C;}_LL21B: return 0;_LL21C: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp436=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp434;if(_tmp436->tag != 1)goto _LL219;
else{_tmp437=_tmp436->f1;}}_LL21D: return Cyc_Absyn_tvar_cmp(_tmp40D,_tmp437)== 0;
_LL219:;}_LL1F6: {struct Cyc_Absyn_DatatypeType_struct*_tmp40E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp401;if(_tmp40E->tag != 3)goto _LL1F8;else{_tmp40F=_tmp40E->f1;_tmp410=_tmp40F.targs;}}
_LL1F7: _tmp412=_tmp410;goto _LL1F9;_LL1F8: {struct Cyc_Absyn_TypedefType_struct*
_tmp411=(struct Cyc_Absyn_TypedefType_struct*)_tmp401;if(_tmp411->tag != 18)goto
_LL1FA;else{_tmp412=_tmp411->f2;}}_LL1F9: _tmp415=_tmp412;goto _LL1FB;_LL1FA: {
struct Cyc_Absyn_AggrType_struct*_tmp413=(struct Cyc_Absyn_AggrType_struct*)
_tmp401;if(_tmp413->tag != 12)goto _LL1FC;else{_tmp414=_tmp413->f1;_tmp415=_tmp414.targs;}}
_LL1FB: _tmp418=_tmp415;goto _LL1FD;_LL1FC: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp416=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp401;if(_tmp416->tag != 4)
goto _LL1FE;else{_tmp417=_tmp416->f1;_tmp418=_tmp417.targs;}}_LL1FD: _tmp41A=
_tmp418;goto _LL1FF;_LL1FE: {struct Cyc_Absyn_JoinEff_struct*_tmp419=(struct Cyc_Absyn_JoinEff_struct*)
_tmp401;if(_tmp419->tag != 24)goto _LL200;else{_tmp41A=_tmp419->f1;}}_LL1FF: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp41A);_LL200: {struct Cyc_Absyn_PointerType_struct*
_tmp41B=(struct Cyc_Absyn_PointerType_struct*)_tmp401;if(_tmp41B->tag != 5)goto
_LL202;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41C.elt_typ;_tmp41E=_tmp41C.ptr_atts;
_tmp41F=_tmp41E.rgn;}}_LL201: return Cyc_CfFlowInfo_contains_region(rgn,_tmp41F)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp41D);_LL202: {struct Cyc_Absyn_FnType_struct*
_tmp420=(struct Cyc_Absyn_FnType_struct*)_tmp401;if(_tmp420->tag != 10)goto _LL204;}
_LL203: return 0;_LL204: {struct Cyc_Absyn_TupleType_struct*_tmp421=(struct Cyc_Absyn_TupleType_struct*)
_tmp401;if(_tmp421->tag != 11)goto _LL206;else{_tmp422=_tmp421->f1;}}_LL205: for(0;
_tmp422 != 0;_tmp422=_tmp422->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp422->hd)).f2))return 1;}return 0;_LL206: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp423=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp401;if(_tmp423->tag != 13)goto
_LL208;else{_tmp424=_tmp423->f2;}}_LL207: for(0;_tmp424 != 0;_tmp424=_tmp424->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp424->hd)->type))
return 1;}return 0;_LL208: {struct Cyc_Absyn_ArrayType_struct*_tmp425=(struct Cyc_Absyn_ArrayType_struct*)
_tmp401;if(_tmp425->tag != 9)goto _LL20A;else{_tmp426=_tmp425->f1;_tmp427=_tmp426.elt_type;}}
_LL209: _tmp429=_tmp427;goto _LL20B;_LL20A: {struct Cyc_Absyn_AccessEff_struct*
_tmp428=(struct Cyc_Absyn_AccessEff_struct*)_tmp401;if(_tmp428->tag != 23)goto
_LL20C;else{_tmp429=(void*)_tmp428->f1;}}_LL20B: _tmp42B=_tmp429;goto _LL20D;
_LL20C: {struct Cyc_Absyn_RgnsEff_struct*_tmp42A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp401;if(_tmp42A->tag != 25)goto _LL20E;else{_tmp42B=(void*)_tmp42A->f1;}}_LL20D:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp42B);_LL20E: {struct Cyc_Absyn_DynRgnType_struct*
_tmp42C=(struct Cyc_Absyn_DynRgnType_struct*)_tmp401;if(_tmp42C->tag != 17)goto
_LL210;else{_tmp42D=(void*)_tmp42C->f1;_tmp42E=(void*)_tmp42C->f2;}}_LL20F:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp42D) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp42E);_LL210: {struct Cyc_Absyn_RgnHandleType_struct*_tmp42F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp401;if(_tmp42F->tag != 16)goto _LL212;else{_tmp430=(void*)_tmp42F->f1;}}_LL211:
return 0;_LL212: {struct Cyc_Absyn_ValueofType_struct*_tmp431=(struct Cyc_Absyn_ValueofType_struct*)
_tmp401;if(_tmp431->tag != 19)goto _LL1DF;}_LL213: return 0;_LL1DF:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp439;struct Cyc_CfFlowInfo_FlowEnv*
_tmp43A;struct Cyc_Dict_Dict _tmp43B;struct Cyc_Dict_Dict*_tmp43C;void*_tmp43D;
struct _tuple23*_tmp438=env;_tmp439=*_tmp438;_tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;
_tmp43C=(struct Cyc_Dict_Dict*)&(*_tmp438).f2;_tmp43D=_tmp439.f3;{void*_tmp43E=
root;struct Cyc_Absyn_Vardecl*_tmp440;void*_tmp442;_LL21F: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp43F=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp43E;if(_tmp43F->tag != 0)goto
_LL221;else{_tmp440=_tmp43F->f1;}}_LL220: if(Cyc_CfFlowInfo_contains_region(
_tmp43D,_tmp440->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp43A,_tmp440->type,
_tmp43A->unknown_none);*_tmp43C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp43C,root,rval);goto _LL21E;_LL221: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp441=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp43E;if(_tmp441->tag != 1)goto _LL223;else{_tmp442=(void*)_tmp441->f2;}}_LL222:
if(!Cyc_CfFlowInfo_contains_region(_tmp43D,_tmp442))*_tmp43C=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp43C,root,rval);goto
_LL21E;_LL223: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp443=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp43E;if(_tmp443->tag != 2)goto _LL21E;}_LL224: goto _LL21E;_LL21E:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp444=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp446;_LL226: {struct Cyc_Absyn_VarType_struct*
_tmp445=(struct Cyc_Absyn_VarType_struct*)_tmp444;if(_tmp445->tag != 2)goto _LL228;
else{_tmp446=_tmp445->f1;}}_LL227: {struct Cyc_CfFlowInfo_Region_k_struct _tmp655;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp655.tag=1,((_tmp655.f1=_tmp446,
_tmp655)));void*v2=(void*)& v;struct _tuple23 _tmp656;struct _tuple23 env=(_tmp656.f1=
fenv,((_tmp656.f2=fenv->mt_flowdict,((_tmp656.f3=v2,_tmp656)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL228: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp447=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp444;if(_tmp447->tag != 22)goto
_LL22A;}_LL229: {struct _tuple23 _tmp657;struct _tuple23 env=(_tmp657.f1=fenv,((
_tmp657.f2=fenv->mt_flowdict,((_tmp657.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp657)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL22A:;_LL22B: {struct Cyc_Core_Impossible_struct _tmp65D;const char*
_tmp65C;struct Cyc_Core_Impossible_struct*_tmp65B;(int)_throw((void*)((_tmp65B=
_cycalloc(sizeof(*_tmp65B)),((_tmp65B[0]=((_tmp65D.tag=Cyc_Core_Impossible,((
_tmp65D.f1=((_tmp65C="kill_flowdict_region",_tag_dyneither(_tmp65C,sizeof(char),
21))),_tmp65D)))),_tmp65B)))));}_LL225:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp44E=
f;int _tmp44F;struct _tuple11 _tmp450;struct Cyc_Dict_Dict _tmp451;struct Cyc_List_List*
_tmp452;_LL22D: if((_tmp44E.BottomFL).tag != 1)goto _LL22F;_tmp44F=(int)(_tmp44E.BottomFL).val;
_LL22E: return f;_LL22F: if((_tmp44E.ReachableFL).tag != 2)goto _LL22C;_tmp450=(
struct _tuple11)(_tmp44E.ReachableFL).val;_tmp451=_tmp450.f1;_tmp452=_tmp450.f2;
_LL230: {struct Cyc_Dict_Dict _tmp453=Cyc_CfFlowInfo_kill_flowdict_region(fenv,
_tmp451,rgn);struct Cyc_List_List*_tmp454=Cyc_CfFlowInfo_kill_relns_region(
_tmp452,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp453,_tmp454);}_LL22C:;}
