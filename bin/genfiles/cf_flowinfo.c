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
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
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
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int
no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(struct
Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,
void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
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
Cyc_CfFlowInfo_AbsLVal _tmp46A;return((_tmp46A.PlaceL).val=x,(((_tmp46A.PlaceL).tag=
1,_tmp46A)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal _tmp46B;return((_tmp46B.UnknownL).val=
0,(((_tmp46B.UnknownL).tag=2,_tmp46B)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp46C;return((_tmp46C.BottomFL).val=0,(((_tmp46C.BottomFL).tag=1,_tmp46C)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){struct _tuple11 _tmp46F;union Cyc_CfFlowInfo_FlowInfo
_tmp46E;return((_tmp46E.ReachableFL).val=((_tmp46F.f1=fd,((_tmp46F.f2=r,_tmp46F)))),(((
_tmp46E.ReachableFL).tag=2,_tmp46E)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4A6;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4A5;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4A4;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4A3;struct Cyc_CfFlowInfo_Esc_struct _tmp4A2;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4A1;struct Cyc_CfFlowInfo_Esc_struct _tmp4A0;struct Cyc_CfFlowInfo_Esc_struct*
_tmp49F;struct Cyc_CfFlowInfo_Esc_struct _tmp49E;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp49D;struct Cyc_CfFlowInfo_UnknownR_struct _tmp49C;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp49B;struct Cyc_CfFlowInfo_UnknownR_struct _tmp49A;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp499;struct Cyc_CfFlowInfo_UnknownR_struct _tmp498;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp497;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp496;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp495;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp494;struct Cyc_CfFlowInfo_Zero_struct*
_tmp493;struct Cyc_CfFlowInfo_Zero_struct _tmp492;struct Cyc_CfFlowInfo_FlowEnv*
_tmp491;return(_tmp491=_region_malloc(r,sizeof(*_tmp491)),((_tmp491->r=r,((
_tmp491->zero=(void*)((_tmp493=_region_malloc(r,sizeof(*_tmp493)),((_tmp493[0]=((
_tmp492.tag=0,_tmp492)),_tmp493)))),((_tmp491->notzeroall=(void*)((_tmp495=
_region_malloc(r,sizeof(*_tmp495)),((_tmp495[0]=((_tmp494.tag=1,_tmp494)),
_tmp495)))),((_tmp491->notzerothis=(void*)((_tmp497=_region_malloc(r,sizeof(*
_tmp497)),((_tmp497[0]=((_tmp496.tag=2,_tmp496)),_tmp497)))),((_tmp491->unknown_none=(
void*)((_tmp499=_region_malloc(r,sizeof(*_tmp499)),((_tmp499[0]=((_tmp498.tag=3,((
_tmp498.f1=Cyc_CfFlowInfo_NoneIL,_tmp498)))),_tmp499)))),((_tmp491->unknown_this=(
void*)((_tmp49B=_region_malloc(r,sizeof(*_tmp49B)),((_tmp49B[0]=((_tmp49A.tag=3,((
_tmp49A.f1=Cyc_CfFlowInfo_ThisIL,_tmp49A)))),_tmp49B)))),((_tmp491->unknown_all=(
void*)((_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp49C.tag=3,((
_tmp49C.f1=Cyc_CfFlowInfo_AllIL,_tmp49C)))),_tmp49D)))),((_tmp491->esc_none=(
void*)((_tmp49F=_region_malloc(r,sizeof(*_tmp49F)),((_tmp49F[0]=((_tmp49E.tag=4,((
_tmp49E.f1=Cyc_CfFlowInfo_NoneIL,_tmp49E)))),_tmp49F)))),((_tmp491->esc_this=(
void*)((_tmp4A1=_region_malloc(r,sizeof(*_tmp4A1)),((_tmp4A1[0]=((_tmp4A0.tag=4,((
_tmp4A0.f1=Cyc_CfFlowInfo_ThisIL,_tmp4A0)))),_tmp4A1)))),((_tmp491->esc_all=(
void*)((_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((_tmp4A2.tag=4,((
_tmp4A2.f1=Cyc_CfFlowInfo_AllIL,_tmp4A2)))),_tmp4A3)))),((_tmp491->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp491->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp491->dummy_place=((_tmp4A6=
_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6->root=(void*)((_tmp4A4=
_region_malloc(r,sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A5.tag=1,((_tmp4A5.f1=&
dummy_exp,((_tmp4A5.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4A5)))))),
_tmp4A4)))),((_tmp4A6->fields=0,_tmp4A6)))))),_tmp491)))))))))))))))))))))))))));}
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
else{const char*_tmp4AA;void*_tmp4A9[1];struct Cyc_String_pa_struct _tmp4A8;return(
struct _dyneither_ptr)((_tmp4A8.tag=0,((_tmp4A8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23->name)),((_tmp4A9[0]=& _tmp4A8,Cyc_aprintf(((
_tmp4AA="reachable from %s",_tag_dyneither(_tmp4AA,sizeof(char),18))),
_tag_dyneither(_tmp4A9,sizeof(void*),1))))))));}_LL3:;_LL4: {const char*_tmp4AD;
void*_tmp4AC;(_tmp4AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp4AD="error locations not for VarRoots",_tag_dyneither(
_tmp4AD,sizeof(char),33))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}_LL0:;};}
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
_tmp35,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4BA;const char*
_tmp4B9;void*_tmp4B8[1];struct Cyc_String_pa_struct _tmp4B7;struct Cyc_Core_Impossible_struct*
_tmp4B6;(int)_throw((void*)((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6[0]=((
_tmp4BA.tag=Cyc_Core_Impossible,((_tmp4BA.f1=(struct _dyneither_ptr)((_tmp4B7.tag=
0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4B8[0]=&
_tmp4B7,Cyc_aprintf(((_tmp4B9="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4B9,sizeof(char),30))),_tag_dyneither(_tmp4B8,sizeof(void*),1)))))))),
_tmp4BA)))),_tmp4B6)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3D;union Cyc_Absyn_AggrInfoU
_tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct Cyc_Absyn_AggrType_struct*_tmp3C=(
struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(_tmp3C->tag != 12)goto _LLD;else{_tmp3D=
_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {struct Cyc_List_List*_tmp41=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp3E))->impl))->fields;
_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp3F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;else{
_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);_LLF:;
_LL10: {struct Cyc_Core_Impossible_struct _tmp4C7;const char*_tmp4C6;void*_tmp4C5[1];
struct Cyc_String_pa_struct _tmp4C4;struct Cyc_Core_Impossible_struct*_tmp4C3;(int)
_throw((void*)((_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C7.tag=
Cyc_Core_Impossible,((_tmp4C7.f1=(struct _dyneither_ptr)((_tmp4C4.tag=0,((_tmp4C4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp4C5[
0]=& _tmp4C4,Cyc_aprintf(((_tmp4C6="get_field_index failed: %s",_tag_dyneither(
_tmp4C6,sizeof(char),27))),_tag_dyneither(_tmp4C5,sizeof(void*),1)))))))),
_tmp4C7)))),_tmp4C3)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp4C8;struct _tuple0 _tmp48=(_tmp4C8.f1=r1,((_tmp4C8.f2=r2,_tmp4C8)));
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
_dyneither_ptr*_tmp4D5;const char*_tmp4D4;void*_tmp4D3[1];struct Cyc_String_pa_struct
_tmp4D2;struct Cyc_List_List*_tmp4D1;sl=((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((
_tmp4D1->hd=((_tmp4D5=_cycalloc(sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D2.tag=0,((
_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6E->name).f2),((
_tmp4D3[0]=& _tmp4D2,Cyc_aprintf(((_tmp4D4="%s",_tag_dyneither(_tmp4D4,sizeof(
char),3))),_tag_dyneither(_tmp4D3,sizeof(void*),1)))))))),_tmp4D5)))),((_tmp4D1->tl=
sl,_tmp4D1))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{
_tmp70=_tmp6F->f1;}}_LL24:{struct _dyneither_ptr*_tmp4E2;const char*_tmp4E1;void*
_tmp4E0[1];struct Cyc_Int_pa_struct _tmp4DF;struct Cyc_List_List*_tmp4DE;sl=((
_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE->hd=((_tmp4E2=_cycalloc(sizeof(*
_tmp4E2)),((_tmp4E2[0]=((_tmp4DF.tag=1,((_tmp4DF.f1=(unsigned long)((int)_tmp70),((
_tmp4E0[0]=& _tmp4DF,Cyc_aprintf(((_tmp4E1="mpt%d",_tag_dyneither(_tmp4E1,sizeof(
char),6))),_tag_dyneither(_tmp4E0,sizeof(void*),1)))))))),_tmp4E2)))),((_tmp4DE->tl=
sl,_tmp4DE))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{
_tmp72=_tmp71->f1;}}_LL26:{struct _dyneither_ptr*_tmp4EF;const char*_tmp4EE;void*
_tmp4ED[1];struct Cyc_Int_pa_struct _tmp4EC;struct Cyc_List_List*_tmp4EB;sl=((
_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB->hd=((_tmp4EF=_cycalloc(sizeof(*
_tmp4EF)),((_tmp4EF[0]=((_tmp4EC.tag=1,((_tmp4EC.f1=(unsigned long)_tmp72,((
_tmp4ED[0]=& _tmp4EC,Cyc_aprintf(((_tmp4EE="param%d",_tag_dyneither(_tmp4EE,
sizeof(char),8))),_tag_dyneither(_tmp4ED,sizeof(void*),1)))))))),_tmp4EF)))),((
_tmp4EB->tl=sl,_tmp4EB))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;_tmp6B=_tmp6B->tl){
struct _dyneither_ptr*_tmp4FC;const char*_tmp4FB;void*_tmp4FA[1];struct Cyc_Int_pa_struct
_tmp4F9;struct Cyc_List_List*_tmp4F8;sl=((_tmp4F8=_cycalloc(sizeof(*_tmp4F8)),((
_tmp4F8->hd=((_tmp4FC=_cycalloc(sizeof(*_tmp4FC)),((_tmp4FC[0]=((_tmp4F9.tag=1,((
_tmp4F9.f1=(unsigned long)((int)_tmp6B->hd),((_tmp4FA[0]=& _tmp4F9,Cyc_aprintf(((
_tmp4FB="%d",_tag_dyneither(_tmp4FB,sizeof(char),3))),_tag_dyneither(_tmp4FA,
sizeof(void*),1)))))))),_tmp4FC)))),((_tmp4F8->tl=sl,_tmp4F8))))));}{struct Cyc_String_pa_struct
_tmp507;const char*_tmp506;void*_tmp505[1];const char*_tmp504;struct _dyneither_ptr*
_tmp503;struct _dyneither_ptr*_tmp87=(_tmp503=_cycalloc(sizeof(*_tmp503)),((
_tmp503[0]=((_tmp507.tag=0,((_tmp507.f1=(struct _dyneither_ptr)((_tmp506="",
_tag_dyneither(_tmp506,sizeof(char),1))),((_tmp505[0]=& _tmp507,Cyc_aprintf(((
_tmp504="%s",_tag_dyneither(_tmp504,sizeof(char),3))),_tag_dyneither(_tmp505,
sizeof(void*),1)))))))),_tmp503)));for(0;sl != 0;sl=sl->tl){const char*_tmp50C;
void*_tmp50B[2];struct Cyc_String_pa_struct _tmp50A;struct Cyc_String_pa_struct
_tmp509;*_tmp87=((_tmp509.tag=0,((_tmp509.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp87),((_tmp50A.tag=0,((_tmp50A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp50B[0]=& _tmp50A,((
_tmp50B[1]=& _tmp509,Cyc_aprintf(((_tmp50C="%s.%s",_tag_dyneither(_tmp50C,sizeof(
char),6))),_tag_dyneither(_tmp50B,sizeof(void*),2))))))))))))));}return _tmp87;};}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval);static void _tmp512(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp511,unsigned int*_tmp510,void***_tmp50E){for(*_tmp511=0;*_tmp511 < *_tmp510;(*
_tmp511)++){(*_tmp50E)[*_tmp511]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp511;unsigned int _tmp510;struct _dyneither_ptr _tmp50F;void**
_tmp50E;unsigned int _tmp50D;struct _dyneither_ptr d=(_tmp50D=sz,((_tmp50E=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp50D)),((_tmp50F=
_tag_dyneither(_tmp50E,sizeof(void*),_tmp50D),((((_tmp510=_tmp50D,_tmp512(& fenv,&
_tmp511,& _tmp510,& _tmp50E))),_tmp50F)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp92;struct _dyneither_ptr*_tmp93;void*_tmp94;struct
Cyc_Absyn_Aggrfield*_tmp91=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp92=*_tmp91;_tmp93=_tmp92.name;_tmp94=_tmp92.type;if(
_get_dyneither_size(*_tmp93,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,_tmp94,leafval);}}return d;}struct _tuple14{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
struct _tuple14 _tmp9A;struct _RegionHandle*_tmp9B;struct Cyc_List_List*_tmp9C;
struct _tuple14*_tmp99=env;_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;{void*
_tmp9D=Cyc_Tcutil_rsubstitute(_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*
_tmp513;return(_tmp513=_region_malloc(_tmp9B,sizeof(*_tmp513)),((_tmp513->name=f->name,((
_tmp513->tq=f->tq,((_tmp513->type=_tmp9D,((_tmp513->width=f->width,((_tmp513->attributes=
f->attributes,_tmp513)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static struct
_dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,
int no_init_bits_only,void*leafval){struct _RegionHandle _tmp9F=_new_region("temp");
struct _RegionHandle*temp=& _tmp9F;_push_region(temp);{struct Cyc_List_List*inst=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14
_tmp514;struct _tuple14 _tmpA0=(_tmp514.f1=temp,((_tmp514.f2=inst,_tmp514)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,&
_tmpA0,fs);struct _dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,
subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA1;};_pop_region(
temp);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp51A(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp519,unsigned int*_tmp518,void***_tmp516){for(*_tmp519=0;*_tmp519 < *_tmp518;(*
_tmp519)++){(*_tmp516)[*_tmp519]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))return fenv->unknown_all;{
void*_tmpA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo _tmpA5;union
Cyc_Absyn_DatatypeFieldInfoU _tmpA6;struct _tuple2 _tmpA7;struct Cyc_Absyn_Datatypefield*
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
_tmpAA);unsigned int _tmp519;unsigned int _tmp518;struct _dyneither_ptr _tmp517;void**
_tmp516;unsigned int _tmp515;struct _dyneither_ptr d=(_tmp515=sz,((_tmp516=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp515)),((_tmp517=
_tag_dyneither(_tmp516,sizeof(void*),_tmp515),((((_tmp518=_tmp515,_tmp51A(& fenv,&
_tmp519,& _tmp518,& _tmp516))),_tmp517)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(
_tmpAA))->hd)).f2,leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp520;struct Cyc_CfFlowInfo_UnionRInfo _tmp51F;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp51E;return(void*)((_tmp51E=_region_malloc(fenv->r,sizeof(*_tmp51E)),((
_tmp51E[0]=((_tmp520.tag=6,((_tmp520.f1=((_tmp51F.is_union=0,((_tmp51F.fieldnum=
- 1,_tmp51F)))),((_tmp520.f2=d,_tmp520)))))),_tmp51E))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC3->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC3->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC3->kind;_tmpB1=
_tmpC4;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp526;struct Cyc_CfFlowInfo_UnionRInfo
_tmp525;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp524;return(void*)((_tmp524=
_region_malloc(fenv->r,sizeof(*_tmp524)),((_tmp524[0]=((_tmp526.tag=6,((_tmp526.f1=((
_tmp525.is_union=_tmpC3->kind == Cyc_Absyn_UnionA,((_tmp525.fieldnum=- 1,_tmp525)))),((
_tmp526.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC3->tvs,
_tmpAE,_tmpC4,_tmpC3->kind == Cyc_Absyn_UnionA,leafval),_tmp526)))))),_tmp524))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA3;if(_tmpAF->tag != 13)goto _LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp52C;struct Cyc_CfFlowInfo_UnionRInfo
_tmp52B;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp52A;return(void*)((_tmp52A=
_region_malloc(fenv->r,sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52C.tag=6,((_tmp52C.f1=((
_tmp52B.is_union=_tmpB0 == Cyc_Absyn_UnionA,((_tmp52B.fieldnum=- 1,_tmp52B)))),((
_tmp52C.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB1,_tmpB0 == Cyc_Absyn_UnionA,
leafval),_tmp52C)))))),_tmp52A))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpB2=(struct Cyc_Absyn_ArrayType_struct*)_tmpA3;if(_tmpB2->tag != 9)goto _LL32;
else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB3.elt_type;_tmpB5=_tmpB3.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB5))goto
_LL32;_LL31: return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(
_tmpB4)?fenv->unknown_all: leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpB6=(
struct Cyc_Absyn_TagType_struct*)_tmpA3;if(_tmpB6->tag != 20)goto _LL34;else{_tmpB7=(
void*)_tmpB6->f1;}}_LL33: return leafval;_LL34:{struct Cyc_Absyn_PointerType_struct*
_tmpB8=(struct Cyc_Absyn_PointerType_struct*)_tmpA3;if(_tmpB8->tag != 5)goto _LL36;
else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.ptr_atts;_tmpBB=_tmpBA.nullable;}}if(!(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBB)))goto
_LL36;_LL35:{void*_tmpCB=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpCD;enum Cyc_CfFlowInfo_InitLevel
_tmpCF;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpCB;if(_tmpCC->tag != 3)goto _LL3B;else{_tmpCD=_tmpCC->f1;if(_tmpCD != Cyc_CfFlowInfo_ThisIL)
goto _LL3B;}}_LL3A: return fenv->notzerothis;_LL3B: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpCE=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpCB;if(_tmpCE->tag != 3)goto
_LL3D;else{_tmpCF=_tmpCE->f1;if(_tmpCF != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C:
return fenv->notzeroall;_LL3D:;_LL3E: goto _LL38;_LL38:;}goto _LL27;_LL36:;_LL37:
goto _LL27;_LL27:;}return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all:
leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}int
Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*
r){void*_tmpD0=r;struct Cyc_Absyn_Exp*_tmpD2;int _tmpD3;void*_tmpD4;struct
_dyneither_ptr _tmpD6;_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*_tmpD1=(struct
Cyc_CfFlowInfo_Consumed_struct*)_tmpD0;if(_tmpD1->tag != 7)goto _LL42;else{_tmpD2=
_tmpD1->f1;_tmpD3=_tmpD1->f2;_tmpD4=(void*)_tmpD1->f3;}}_LL41: return !(_tmpD2 == e
 && _tmpD3 == env_iteration);_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD5=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpD0;if(_tmpD5->tag != 6)goto _LL44;else{
_tmpD6=_tmpD5->f2;}}_LL43: {unsigned int _tmpD7=_get_dyneither_size(_tmpD6,
sizeof(void*));{int i=0;for(0;i < _tmpD7;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(
e,env_iteration,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i))))
return 1;}}return 0;}_LL44:;_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp536(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp535,unsigned int*_tmp534,void***_tmp532){for(*_tmp535=0;*
_tmp535 < *_tmp534;(*_tmp535)++){(*_tmp532)[*_tmp535]=(*fenv)->unknown_all;}}
static void _tmp53F(struct _dyneither_ptr*_tmpF2,unsigned int*_tmp53E,unsigned int*
_tmp53D,void***_tmp53B){for(*_tmp53E=0;*_tmp53E < *_tmp53D;(*_tmp53E)++){(*
_tmp53B)[*_tmp53E]=((void**)(*_tmpF2).curr)[(int)*_tmp53E];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp52F;struct Cyc_CfFlowInfo_Consumed_struct*_tmp52E;return(void*)((_tmp52E=
_region_malloc(fenv->r,sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp52F.tag=7,((_tmp52F.f1=
consumer,((_tmp52F.f2=iteration,((_tmp52F.f3=(void*)r,_tmp52F)))))))),_tmp52E))));}{
struct _tuple0 _tmp530;struct _tuple0 _tmpDB=(_tmp530.f1=Cyc_Tcutil_compress(t),((
_tmp530.f2=r,_tmp530)));void*_tmpDC;struct Cyc_List_List*_tmpDE;void*_tmpDF;
struct Cyc_CfFlowInfo_UnionRInfo _tmpE1;struct _dyneither_ptr _tmpE2;void*_tmpE3;
struct Cyc_Absyn_AggrInfo _tmpE5;union Cyc_Absyn_AggrInfoU _tmpE6;void*_tmpE7;struct
Cyc_CfFlowInfo_UnionRInfo _tmpE9;struct _dyneither_ptr _tmpEA;void*_tmpEB;enum Cyc_Absyn_AggrKind
_tmpED;struct Cyc_List_List*_tmpEE;void*_tmpEF;struct Cyc_CfFlowInfo_UnionRInfo
_tmpF1;struct _dyneither_ptr _tmpF2;_LL47: _tmpDC=_tmpDB.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpDD=(struct Cyc_Absyn_TupleType_struct*)_tmpDC;if(_tmpDD->tag != 11)goto _LL49;
else{_tmpDE=_tmpDD->f1;}};_tmpDF=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDF;if(_tmpE0->tag != 6)goto
_LL49;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;}};_LL48: {unsigned int _tmpF3=
_get_dyneither_size(_tmpE2,sizeof(void*));unsigned int _tmp535;unsigned int
_tmp534;struct _dyneither_ptr _tmp533;void**_tmp532;unsigned int _tmp531;struct
_dyneither_ptr d2=(_tmp531=_tmpF3,((_tmp532=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp531)),((_tmp533=_tag_dyneither(_tmp532,sizeof(
void*),_tmp531),((((_tmp534=_tmp531,_tmp536(& fenv,& _tmp535,& _tmp534,& _tmp532))),
_tmp533)))))));{int i=0;for(0;i < _tmpF3;++ i){*((void**)_check_dyneither_subscript(
d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)((
struct Cyc_List_List*)_check_null(_tmpDE))->hd)).f2,consumer,iteration,*((void**)
_check_dyneither_subscript(_tmpE2,sizeof(void*),i)));_tmpDE=_tmpDE->tl;}}{struct
Cyc_CfFlowInfo_Aggregate_struct _tmp539;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp538;return(void*)((_tmp538=_region_malloc(fenv->r,sizeof(*_tmp538)),((
_tmp538[0]=((_tmp539.tag=6,((_tmp539.f1=_tmpE1,((_tmp539.f2=d2,_tmp539)))))),
_tmp538))));};}_LL49: _tmpE3=_tmpDB.f1;{struct Cyc_Absyn_AggrType_struct*_tmpE4=(
struct Cyc_Absyn_AggrType_struct*)_tmpE3;if(_tmpE4->tag != 12)goto _LL4B;else{
_tmpE5=_tmpE4->f1;_tmpE6=_tmpE5.aggr_info;}};_tmpE7=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE8=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpE7;if(_tmpE8->tag != 6)goto
_LL4B;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}};_LL4A: {struct Cyc_Absyn_Aggrdecl*
_tmpFA=Cyc_Absyn_get_known_aggrdecl(_tmpE6);if(_tmpFA->impl == 0)return r;_tmpED=
_tmpFA->kind;_tmpEE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFA->impl))->fields;
_tmpF1=_tmpE9;_tmpF2=_tmpEA;goto _LL4C;}_LL4B: _tmpEB=_tmpDB.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmpEC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEB;if(_tmpEC->tag != 13)goto
_LL4D;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}};_tmpEF=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEF;if(_tmpF0->tag != 6)goto
_LL4D;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}};_LL4C: {unsigned int _tmp53E;
unsigned int _tmp53D;struct _dyneither_ptr _tmp53C;void**_tmp53B;unsigned int
_tmp53A;struct _dyneither_ptr d2=(_tmp53A=_get_dyneither_size(_tmpF2,sizeof(void*)),((
_tmp53B=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp53A)),((
_tmp53C=_tag_dyneither(_tmp53B,sizeof(void*),_tmp53A),((((_tmp53D=_tmp53A,
_tmp53F(& _tmpF2,& _tmp53E,& _tmp53D,& _tmp53B))),_tmp53C)))))));unsigned int sz=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpEE);{int i=0;
for(0;i < sz;(i ++,_tmpEE=_tmpEE->tl)){struct Cyc_Absyn_Aggrfield _tmpFC;struct
_dyneither_ptr*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Aggrfield*_tmpFB=(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_List_List*)_check_null(_tmpEE))->hd;_tmpFC=*_tmpFB;_tmpFD=_tmpFC.name;
_tmpFE=_tmpFC.type;if(_get_dyneither_size(*_tmpFD,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmpFE,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF2,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp542;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp541;return(void*)((_tmp541=_region_malloc(fenv->r,sizeof(*_tmp541)),((
_tmp541[0]=((_tmp542.tag=6,((_tmp542.f1=_tmpF1,((_tmp542.f2=_tmpF2,_tmp542)))))),
_tmp541))));};}_LL4D:;_LL4E: return r;_LL46:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmp105=_new_region("r");struct _RegionHandle*r=& _tmp105;_push_region(r);{struct
_tuple16 _tmp543;struct _tuple16 _tmp106=(_tmp543.f1=place,((_tmp543.f2=0,_tmp543)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple16*))Cyc_Iter_next)(iter,& _tmp106)){struct Cyc_CfFlowInfo_Place*
_tmp107=_tmp106.f1;void*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_CfFlowInfo_Place
_tmp108=*place;_tmp109=_tmp108.root;_tmp10A=_tmp108.fields;{void*_tmp10C;struct
Cyc_List_List*_tmp10D;struct Cyc_CfFlowInfo_Place _tmp10B=*_tmp107;_tmp10C=_tmp10B.root;
_tmp10D=_tmp10B.fields;if(Cyc_CfFlowInfo_root_cmp(_tmp109,_tmp10C)!= 0)continue;
for(0;_tmp10A != 0  && _tmp10D != 0;(_tmp10A=_tmp10A->tl,_tmp10D=_tmp10D->tl)){if((
int)_tmp10A->hd != (int)_tmp10D->hd)break;}if(_tmp10A == 0){int _tmp10E=1;
_npop_handler(0);return _tmp10E;}};}{int _tmp10F=0;_npop_handler(0);return _tmp10F;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp544;pile->places=((_tmp544=_region_malloc(pile->rgn,sizeof(*_tmp544)),((
_tmp544->hd=place,((_tmp544->tl=pile->places,_tmp544))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r);static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp112=r;void*_tmp114;struct Cyc_CfFlowInfo_Place*
_tmp116;struct Cyc_CfFlowInfo_UnionRInfo _tmp118;struct _dyneither_ptr _tmp119;_LL50: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp113=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp112;if(_tmp113->tag != 7)goto _LL52;else{_tmp114=(void*)_tmp113->f3;}}_LL51:
Cyc_CfFlowInfo_add_places(pile,_tmp114);return;_LL52: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp115=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp112;if(_tmp115->tag != 5)goto
_LL54;else{_tmp116=_tmp115->f1;}}_LL53: Cyc_CfFlowInfo_add_place(pile,_tmp116);
return;_LL54: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp117=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp112;if(_tmp117->tag != 6)goto _LL56;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}
_LL55:{int i=0;for(0;i < _get_dyneither_size(_tmp119,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp119.curr)[i]);}}return;_LL56:;_LL57: return;_LL4F:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp54A(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp549,unsigned int*_tmp548,void***_tmp546,struct _dyneither_ptr*_tmp11E){for(*
_tmp549=0;*_tmp549 < *_tmp548;(*_tmp549)++){(*_tmp546)[*_tmp549]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp11E).curr)[(int)*_tmp549]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp11A=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp11C;int _tmp11D;struct _dyneither_ptr _tmp11E;_LL59: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp11B=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11A;if(_tmp11B->tag != 6)goto
_LL5B;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11C.is_union;_tmp11E=_tmp11B->f2;}}
_LL5A: {unsigned int _tmp549;unsigned int _tmp548;struct _dyneither_ptr _tmp547;void**
_tmp546;unsigned int _tmp545;struct _dyneither_ptr d2=(_tmp545=_get_dyneither_size(
_tmp11E,sizeof(void*)),((_tmp546=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp545)),((_tmp547=_tag_dyneither(_tmp546,sizeof(void*),_tmp545),((((
_tmp548=_tmp545,_tmp54A(& r,& new_val,& _tmp549,& _tmp548,& _tmp546,& _tmp11E))),
_tmp547)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp550;struct Cyc_CfFlowInfo_UnionRInfo
_tmp54F;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp54E;return(void*)((_tmp54E=
_region_malloc(r,sizeof(*_tmp54E)),((_tmp54E[0]=((_tmp550.tag=6,((_tmp550.f1=((
_tmp54F.is_union=_tmp11D,((_tmp54F.fieldnum=- 1,_tmp54F)))),((_tmp550.f2=d2,
_tmp550)))))),_tmp54E))));}_LL5B:;_LL5C: return new_val;_LL58:;}static struct
_dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct
_dyneither_ptr d,int n,void*rval);static void _tmp556(struct _dyneither_ptr*d,
unsigned int*_tmp555,unsigned int*_tmp554,void***_tmp552){for(*_tmp555=0;*
_tmp555 < *_tmp554;(*_tmp555)++){(*_tmp552)[*_tmp555]=((void**)(*d).curr)[(int)*
_tmp555];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp126=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp126 == rval)return d;{
unsigned int _tmp555;unsigned int _tmp554;struct _dyneither_ptr _tmp553;void**
_tmp552;unsigned int _tmp551;struct _dyneither_ptr res=(_tmp551=_get_dyneither_size(
d,sizeof(void*)),((_tmp552=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp551)),((_tmp553=_tag_dyneither(_tmp552,sizeof(void*),_tmp551),((((_tmp554=
_tmp551,_tmp556(& d,& _tmp555,& _tmp554,& _tmp552))),_tmp553)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple17 _tmp557;struct _tuple17 _tmp12C=(_tmp557.f1=fs,((
_tmp557.f2=old_val,_tmp557)));struct Cyc_List_List*_tmp12D;struct Cyc_List_List
_tmp12E;int _tmp12F;struct Cyc_List_List*_tmp130;void*_tmp131;struct Cyc_CfFlowInfo_UnionRInfo
_tmp133;int _tmp134;struct _dyneither_ptr _tmp135;_LL5E: _tmp12D=_tmp12C.f1;if(
_tmp12D == 0)goto _LL60;_tmp12E=*_tmp12D;_tmp12F=(int)_tmp12E.hd;_tmp130=_tmp12E.tl;
_tmp131=_tmp12C.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp132=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp131;if(_tmp132->tag != 6)goto _LL60;else{_tmp133=_tmp132->f1;_tmp134=_tmp133.is_union;
_tmp135=_tmp132->f2;}};_LL5F: {void*_tmp136=Cyc_CfFlowInfo_insert_place_outer(r,
_tmp130,*((void**)_check_dyneither_subscript(_tmp135,sizeof(void*),_tmp12F)),
new_val);struct _dyneither_ptr _tmp137=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp135,
_tmp12F,_tmp136);if((void**)_tmp137.curr == (void**)_tmp135.curr)return old_val;
else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp55D;struct Cyc_CfFlowInfo_UnionRInfo
_tmp55C;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp55B;return(void*)((_tmp55B=
_region_malloc(r,sizeof(*_tmp55B)),((_tmp55B[0]=((_tmp55D.tag=6,((_tmp55D.f1=((
_tmp55C.is_union=_tmp134,((_tmp55C.fieldnum=- 1,_tmp55C)))),((_tmp55D.f2=_tmp137,
_tmp55D)))))),_tmp55B))));}}_LL60:;_LL61: {struct Cyc_Core_Impossible_struct
_tmp563;const char*_tmp562;struct Cyc_Core_Impossible_struct*_tmp561;(int)_throw((
void*)((_tmp561=_cycalloc(sizeof(*_tmp561)),((_tmp561[0]=((_tmp563.tag=Cyc_Core_Impossible,((
_tmp563.f1=((_tmp562="bad insert place",_tag_dyneither(_tmp562,sizeof(char),17))),
_tmp563)))),_tmp561)))));}_LL5D:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp13E=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp13E,0);{void*oldval;void*
newval;{struct _handler_cons _tmp13F;_push_handler(& _tmp13F);{int _tmp141=0;if(
setjmp(_tmp13F.handler))_tmp141=1;if(!_tmp141){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp13E);;_pop_handler();}else{void*_tmp140=(void*)_exn_thrown;void*_tmp143=
_tmp140;_LL63: {struct Cyc_Dict_Absent_struct*_tmp144=(struct Cyc_Dict_Absent_struct*)
_tmp143;if(_tmp144->tag != Cyc_Dict_Absent)goto _LL65;}_LL64: continue;_LL65:;_LL66:(
void)_throw(_tmp143);_LL62:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL67: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL68: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL69: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp146;struct Cyc_List_List*
_tmp147;struct Cyc_CfFlowInfo_Place _tmp145=*_tmp13E;_tmp146=_tmp145.root;_tmp147=
_tmp145.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp146,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp147,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp146),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r);static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*
r){void*_tmp148=r;enum Cyc_CfFlowInfo_InitLevel _tmp14A;enum Cyc_CfFlowInfo_InitLevel
_tmp14C;void*_tmp151;_LL6C: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp149=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp148;if(_tmp149->tag != 3)goto _LL6E;
else{_tmp14A=_tmp149->f1;}}_LL6D: return _tmp14A;_LL6E: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp14B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp148;if(_tmp14B->tag != 4)goto _LL70;
else{_tmp14C=_tmp14B->f1;}}_LL6F: return _tmp14C;_LL70: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp14D=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp148;if(_tmp14D->tag != 0)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp14E=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp148;if(_tmp14E->tag != 1)goto _LL74;}_LL73:
return Cyc_CfFlowInfo_AllIL;_LL74: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp14F=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp148;if(_tmp14F->tag != 2)
goto _LL76;}_LL75: return Cyc_CfFlowInfo_ThisIL;_LL76: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp150=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp148;if(_tmp150->tag != 7)goto
_LL78;else{_tmp151=(void*)_tmp150->f3;}}_LL77: return Cyc_CfFlowInfo_NoneIL;_LL78:;
_LL79: {struct Cyc_Core_Impossible_struct _tmp569;const char*_tmp568;struct Cyc_Core_Impossible_struct*
_tmp567;(int)_throw((void*)((_tmp567=_cycalloc(sizeof(*_tmp567)),((_tmp567[0]=((
_tmp569.tag=Cyc_Core_Impossible,((_tmp569.f1=((_tmp568="initlevel_approx",
_tag_dyneither(_tmp568,sizeof(char),17))),_tmp569)))),_tmp567)))));}_LL6B:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc);static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum 
Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)
return Cyc_CfFlowInfo_NoneIL;{void*_tmp155=r;struct Cyc_CfFlowInfo_UnionRInfo
_tmp157;int _tmp158;int _tmp159;struct _dyneither_ptr _tmp15A;struct Cyc_CfFlowInfo_UnionRInfo
_tmp15C;int _tmp15D;struct _dyneither_ptr _tmp15E;struct Cyc_CfFlowInfo_Place*
_tmp160;_LL7B: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp156=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp155;if(_tmp156->tag != 6)goto _LL7D;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.is_union;
if(_tmp158 != 1)goto _LL7D;_tmp159=_tmp157.fieldnum;_tmp15A=_tmp156->f2;}}_LL7C: {
unsigned int _tmp161=_get_dyneither_size(_tmp15A,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp159 == - 1){int i=0;for(0;i < _tmp161;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp15A,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp15A,sizeof(void*),_tmp159)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL7A;}_LL7D: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp15B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp155;if(_tmp15B->tag != 6)goto _LL7F;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15C.is_union;
if(_tmp15D != 0)goto _LL7F;_tmp15E=_tmp15B->f2;}}_LL7E: {unsigned int _tmp162=
_get_dyneither_size(_tmp15E,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp162;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp15E,sizeof(void*),i)),this_ans);}}goto _LL7A;}
_LL7F: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp15F=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp155;if(_tmp15F->tag != 5)goto _LL81;else{_tmp160=_tmp15F->f1;}}_LL80: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp160))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp56A;env->seen=((
_tmp56A=_region_malloc(env->e,sizeof(*_tmp56A)),((_tmp56A->hd=_tmp160,((_tmp56A->tl=
env->seen,_tmp56A))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp160),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL7A;_LL81:;_LL82: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL7A:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct
_RegionHandle*_tmp164=env->r;struct Cyc_CfFlowInfo_InitlevelEnv _tmp56B;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp165=(_tmp56B.e=_tmp164,((_tmp56B.d=d,((_tmp56B.seen=0,_tmp56B)))));return Cyc_CfFlowInfo_initlevel_rec(&
_tmp165,r,Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp168;struct Cyc_List_List*
_tmp169;struct Cyc_CfFlowInfo_Place _tmp167=*place;_tmp168=_tmp167.root;_tmp169=
_tmp167.fields;{void*_tmp16A=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp168);for(0;_tmp169 != 0;_tmp169=_tmp169->tl){struct _tuple18 _tmp56C;struct
_tuple18 _tmp16C=(_tmp56C.f1=_tmp16A,((_tmp56C.f2=(int)_tmp169->hd,_tmp56C)));
void*_tmp16D;struct Cyc_CfFlowInfo_UnionRInfo _tmp16F;struct _dyneither_ptr _tmp170;
int _tmp171;_LL84: _tmp16D=_tmp16C.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp16E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16D;if(_tmp16E->tag != 6)goto
_LL86;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;}};_tmp171=_tmp16C.f2;_LL85:
_tmp16A=*((void**)_check_dyneither_subscript(_tmp170,sizeof(void*),_tmp171));
goto _LL83;_LL86:;_LL87: {struct Cyc_Core_Impossible_struct _tmp572;const char*
_tmp571;struct Cyc_Core_Impossible_struct*_tmp570;(int)_throw((void*)((_tmp570=
_cycalloc(sizeof(*_tmp570)),((_tmp570[0]=((_tmp572.tag=Cyc_Core_Impossible,((
_tmp572.f1=((_tmp571="bad lookup_place",_tag_dyneither(_tmp571,sizeof(char),17))),
_tmp572)))),_tmp570)))));}_LL83:;}return _tmp16A;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp175=
rval;void*_tmp178;struct Cyc_CfFlowInfo_UnionRInfo _tmp17A;int _tmp17B;int _tmp17C;
struct _dyneither_ptr _tmp17D;_LL89: {struct Cyc_CfFlowInfo_Esc_struct*_tmp176=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp175;if(_tmp176->tag != 4)goto _LL8B;}_LL8A:
return 0;_LL8B: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp177=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp175;if(_tmp177->tag != 7)goto _LL8D;else{_tmp178=(void*)_tmp177->f3;}}_LL8C:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL8D: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp179=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp175;if(_tmp179->tag != 6)goto
_LL8F;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.is_union;_tmp17C=_tmp17A.fieldnum;
_tmp17D=_tmp179->f2;}}_LL8E: if(_tmp17B  && _tmp17C != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp17D,sizeof(void*),_tmp17C)));else{
unsigned int sz=_get_dyneither_size(_tmp17D,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp17D.curr)[i]))return 0;}}
return 1;}_LL8F:;_LL90: return 1;_LL88:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,void*r){struct _RegionHandle*_tmp17E=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp573;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp573=
_region_malloc(_tmp17E,sizeof(*_tmp573)),((_tmp573->rgn=_tmp17E,((_tmp573->places=
0,_tmp573)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp57D(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp57C,unsigned int*_tmp57B,void***_tmp579,struct
_dyneither_ptr*_tmp190,struct _dyneither_ptr*_tmp18C){for(*_tmp57C=0;*_tmp57C < *
_tmp57B;(*_tmp57C)++){(*_tmp579)[*_tmp57C]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp18C).curr)[(int)*_tmp57C],*((void**)
_check_dyneither_subscript(*_tmp190,sizeof(void*),(int)*_tmp57C)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp574;struct _tuple0 _tmp181=(_tmp574.f1=oldval,((
_tmp574.f2=newval,_tmp574)));void*_tmp182;void*_tmp184;struct Cyc_CfFlowInfo_Place*
_tmp186;void*_tmp187;void*_tmp189;struct Cyc_CfFlowInfo_UnionRInfo _tmp18B;struct
_dyneither_ptr _tmp18C;void*_tmp18D;struct Cyc_CfFlowInfo_UnionRInfo _tmp18F;struct
_dyneither_ptr _tmp190;void*_tmp191;enum Cyc_CfFlowInfo_InitLevel _tmp193;_LL92:
_tmp182=_tmp181.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp183=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp182;if(_tmp183->tag != 4)goto _LL94;};_tmp184=_tmp181.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp185=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp184;if(_tmp185->tag != 5)goto
_LL94;else{_tmp186=_tmp185->f1;}};_LL93: Cyc_CfFlowInfo_add_place(env->pile,
_tmp186);goto _LL95;_LL94: _tmp187=_tmp181.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp188=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp187;if(_tmp188->tag != 4)goto _LL96;};
_LL95: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp577;void*_tmp576;(_tmp576=0,Cyc_Tcutil_terr(env->loc,((_tmp577="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp577,sizeof(char),67))),_tag_dyneither(_tmp576,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL96: _tmp189=_tmp181.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp18A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp189;if(_tmp18A->tag != 6)goto
_LL98;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}};_tmp18D=_tmp181.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp18E=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp18D;if(_tmp18E->tag != 6)goto _LL98;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}};
_LL97: {unsigned int _tmp57C;unsigned int _tmp57B;struct _dyneither_ptr _tmp57A;void**
_tmp579;unsigned int _tmp578;struct _dyneither_ptr new_d=(_tmp578=
_get_dyneither_size(_tmp18C,sizeof(void*)),((_tmp579=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp578)),((_tmp57A=_tag_dyneither(_tmp579,sizeof(
void*),_tmp578),((((_tmp57B=_tmp578,_tmp57D(& env,& _tmp57C,& _tmp57B,& _tmp579,&
_tmp190,& _tmp18C))),_tmp57A)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp18C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp18C.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp18C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp190,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp580;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp57F;return(void*)((_tmp57F=_region_malloc((env->fenv)->r,sizeof(*_tmp57F)),((
_tmp57F[0]=((_tmp580.tag=6,((_tmp580.f1=_tmp18B,((_tmp580.f2=new_d,_tmp580)))))),
_tmp57F))));};}_LL98: _tmp191=_tmp181.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp192=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp191;if(_tmp192->tag != 4)goto _LL9A;else{
_tmp193=_tmp192->f1;}};_LL99: switch(_tmp193){case Cyc_CfFlowInfo_NoneIL: _LL9C:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL9D: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL9E: return(env->fenv)->unknown_all;}_LL9A:;_LL9B:
return newval;_LL91:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval);static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmp581;struct _tuple17 _tmp19D=(_tmp581.f1=fs,((_tmp581.f2=oldval,
_tmp581)));struct Cyc_List_List*_tmp19E;struct Cyc_List_List _tmp19F;int _tmp1A0;
struct Cyc_List_List*_tmp1A1;void*_tmp1A2;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A4;
int _tmp1A5;int _tmp1A6;struct _dyneither_ptr _tmp1A7;_LLA1: _tmp19E=_tmp19D.f1;if(
_tmp19E == 0)goto _LLA3;_tmp19F=*_tmp19E;_tmp1A0=(int)_tmp19F.hd;_tmp1A1=_tmp19F.tl;
_tmp1A2=_tmp19D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1A3=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1A2;if(_tmp1A3->tag != 6)goto _LLA3;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.is_union;
_tmp1A6=_tmp1A4.fieldnum;_tmp1A7=_tmp1A3->f2;}};_LLA2: {void*_tmp1A8=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1A1,*((void**)_check_dyneither_subscript(_tmp1A7,sizeof(void*),_tmp1A0)),
newval);struct _dyneither_ptr _tmp1A9=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp1A7,_tmp1A0,_tmp1A8);if((void**)_tmp1A9.curr == (void**)_tmp1A7.curr  && (!
_tmp1A5  || _tmp1A6 == _tmp1A0))return oldval;{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp587;struct Cyc_CfFlowInfo_UnionRInfo _tmp586;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp585;return(void*)((_tmp585=_region_malloc((env->fenv)->r,sizeof(*_tmp585)),((
_tmp585[0]=((_tmp587.tag=6,((_tmp587.f1=((_tmp586.is_union=_tmp1A5,((_tmp586.fieldnum=
_tmp1A0,_tmp586)))),((_tmp587.f2=_tmp1A9,_tmp587)))))),_tmp585))));};}_LLA3:;
_LLA4: {struct Cyc_Core_Impossible_struct _tmp58D;const char*_tmp58C;struct Cyc_Core_Impossible_struct*
_tmp58B;(int)_throw((void*)((_tmp58B=_cycalloc(sizeof(*_tmp58B)),((_tmp58B[0]=((
_tmp58D.tag=Cyc_Core_Impossible,((_tmp58D.f1=((_tmp58C="bad assign place",
_tag_dyneither(_tmp58C,sizeof(char),17))),_tmp58D)))),_tmp58B)))));}_LLA0:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{struct _RegionHandle*
_tmp1B0=fenv->r;struct Cyc_CfFlowInfo_Place _tmp1B2;void*_tmp1B3;struct Cyc_List_List*
_tmp1B4;struct Cyc_CfFlowInfo_Place*_tmp1B1=place;_tmp1B2=*_tmp1B1;_tmp1B3=
_tmp1B2.root;_tmp1B4=_tmp1B2.fields;{struct Cyc_CfFlowInfo_EscPile*_tmp590;struct
Cyc_CfFlowInfo_AssignEnv _tmp58F;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp58F.fenv=
fenv,((_tmp58F.pile=((_tmp590=_region_malloc(_tmp1B0,sizeof(*_tmp590)),((_tmp590->rgn=
_tmp1B0,((_tmp590->places=0,_tmp590)))))),((_tmp58F.d=d,((_tmp58F.loc=loc,
_tmp58F)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1B4,((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1B3),r);return Cyc_CfFlowInfo_escape_these(
fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
void*v))Cyc_Dict_insert)(d,_tmp1B3,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither
 = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict
chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5A0(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp59F,unsigned int*_tmp59E,void***_tmp59C,struct
_dyneither_ptr*_tmp1FD,struct _dyneither_ptr*_tmp1F9){for(*_tmp59F=0;*_tmp59F < *
_tmp59E;(*_tmp59F)++){(*_tmp59C)[*_tmp59F]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1F9).curr)[(
int)*_tmp59F],*((void**)_check_dyneither_subscript(*_tmp1FD,sizeof(void*),(int)*
_tmp59F)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1B7=(
env->fenv)->r;{struct _tuple0 _tmp591;struct _tuple0 _tmp1B9=(_tmp591.f1=r1,((
_tmp591.f2=r2,_tmp591)));void*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;int _tmp1BD;
void*_tmp1BE;void*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C2;void*_tmp1C3;
void*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;int _tmp1C7;void*_tmp1C8;void*_tmp1C9;
struct Cyc_Absyn_Exp*_tmp1CB;int _tmp1CC;void*_tmp1CD;void*_tmp1CE;struct Cyc_CfFlowInfo_Place*
_tmp1D0;void*_tmp1D1;struct Cyc_CfFlowInfo_Place*_tmp1D3;void*_tmp1D4;struct Cyc_CfFlowInfo_Place*
_tmp1D6;void*_tmp1D7;void*_tmp1D9;void*_tmp1DB;struct Cyc_CfFlowInfo_Place*
_tmp1DD;void*_tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1E0;void*_tmp1E1;void*
_tmp1E3;void*_tmp1E5;struct Cyc_CfFlowInfo_Place*_tmp1E7;void*_tmp1E8;struct Cyc_CfFlowInfo_Place*
_tmp1EA;void*_tmp1EB;struct Cyc_CfFlowInfo_Place*_tmp1ED;void*_tmp1EE;void*
_tmp1F0;void*_tmp1F2;void*_tmp1F4;void*_tmp1F6;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1F8;struct _dyneither_ptr _tmp1F9;void*_tmp1FA;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1FC;struct _dyneither_ptr _tmp1FD;_LLA6: _tmp1BA=_tmp1B9.f1;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1BB=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1BA;if(_tmp1BB->tag != 7)goto
_LLA8;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;_tmp1BE=(void*)_tmp1BB->f3;}};
_tmp1BF=_tmp1B9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1C0=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1BF;if(_tmp1C0->tag != 7)goto _LLA8;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;
_tmp1C3=(void*)_tmp1C0->f3;}};_LLA7: if(_tmp1BC == _tmp1C1  && _tmp1BD == _tmp1C2){
struct Cyc_CfFlowInfo_Consumed_struct _tmp594;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp593;return(void*)((_tmp593=_region_malloc(_tmp1B7,sizeof(*_tmp593)),((
_tmp593[0]=((_tmp594.tag=7,((_tmp594.f1=_tmp1BC,((_tmp594.f2=_tmp1BD,((_tmp594.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,_tmp1BE,_tmp1C3),_tmp594)))))))),_tmp593))));}{
void*_tmp200=_tmp1BE;struct Cyc_CfFlowInfo_Place*_tmp202;_LLC3: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp201=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp200;if(_tmp201->tag != 5)goto
_LLC5;else{_tmp202=_tmp201->f1;}}_LLC4: Cyc_CfFlowInfo_add_place(env->pile,
_tmp202);goto _LLC2;_LLC5:;_LLC6: goto _LLC2;_LLC2:;}{void*_tmp203=_tmp1C3;struct
Cyc_CfFlowInfo_Place*_tmp205;_LLC8: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp204=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp203;if(_tmp204->tag != 5)goto
_LLCA;else{_tmp205=_tmp204->f1;}}_LLC9: Cyc_CfFlowInfo_add_place(env->pile,
_tmp205);goto _LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}goto _LLA5;_LLA8: _tmp1C4=
_tmp1B9.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1C5=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1C4;if(_tmp1C5->tag != 7)goto _LLAA;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;
_tmp1C8=(void*)_tmp1C5->f3;}};_LLA9: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp597;struct Cyc_CfFlowInfo_Consumed_struct*_tmp596;return(void*)((_tmp596=
_region_malloc(_tmp1B7,sizeof(*_tmp596)),((_tmp596[0]=((_tmp597.tag=7,((_tmp597.f1=
_tmp1C6,((_tmp597.f2=_tmp1C7,((_tmp597.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1C8,r2),_tmp597)))))))),_tmp596))));}_LLAA: _tmp1C9=_tmp1B9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1CA=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1C9;if(_tmp1CA->tag != 7)goto
_LLAC;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;_tmp1CD=(void*)_tmp1CA->f3;}};
_LLAB: {struct Cyc_CfFlowInfo_Consumed_struct _tmp59A;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp599;return(void*)((_tmp599=_region_malloc(_tmp1B7,sizeof(*_tmp599)),((
_tmp599[0]=((_tmp59A.tag=7,((_tmp59A.f1=_tmp1CB,((_tmp59A.f2=_tmp1CC,((_tmp59A.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1CD),_tmp59A)))))))),_tmp599))));}
_LLAC: _tmp1CE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1CF=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1CE;if(_tmp1CF->tag != 5)goto _LLAE;else{
_tmp1D0=_tmp1CF->f1;}};_tmp1D1=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1D2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D1;if(_tmp1D2->tag != 5)goto
_LLAE;else{_tmp1D3=_tmp1D2->f1;}};_LLAD: if(Cyc_CfFlowInfo_place_cmp(_tmp1D0,
_tmp1D3)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1D0);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1D3);goto _LLA5;_LLAE: _tmp1D4=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1D5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D4;if(_tmp1D5->tag != 5)goto
_LLB0;else{_tmp1D6=_tmp1D5->f1;}};_tmp1D7=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1D8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1D7;if(_tmp1D8->tag != 2)
goto _LLB0;};_LLAF: _tmp1DD=_tmp1D6;goto _LLB1;_LLB0: _tmp1D9=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1DA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1D9;if(_tmp1DA->tag != 2)
goto _LLB2;};_tmp1DB=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1DC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DB;if(_tmp1DC->tag != 5)goto _LLB2;
else{_tmp1DD=_tmp1DC->f1;}};_LLB1: Cyc_CfFlowInfo_add_place(env->pile,_tmp1DD);
return(env->fenv)->notzerothis;_LLB2: _tmp1DE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1DF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DE;if(_tmp1DF->tag != 5)goto
_LLB4;else{_tmp1E0=_tmp1DF->f1;}};_tmp1E1=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1E2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1E1;if(_tmp1E2->tag != 1)
goto _LLB4;};_LLB3: Cyc_CfFlowInfo_add_place(env->pile,_tmp1E0);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLCC: return(env->fenv)->notzeroall;
default: _LLCD: return(env->fenv)->notzerothis;}_LLB4: _tmp1E3=_tmp1B9.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1E4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1E3;if(_tmp1E4->tag != 1)goto _LLB6;};_tmp1E5=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E5;if(_tmp1E6->tag != 5)goto
_LLB6;else{_tmp1E7=_tmp1E6->f1;}};_LLB5: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1E7);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLCF: return(env->fenv)->notzeroall;default: _LLD0: return(env->fenv)->notzerothis;}
_LLB6: _tmp1E8=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1E9=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E8;if(_tmp1E9->tag != 5)goto _LLB8;else{
_tmp1EA=_tmp1E9->f1;}};_LLB7: Cyc_CfFlowInfo_add_place(env->pile,_tmp1EA);goto
_LLA5;_LLB8: _tmp1EB=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1EC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EB;if(_tmp1EC->tag != 5)goto _LLBA;
else{_tmp1ED=_tmp1EC->f1;}};_LLB9: Cyc_CfFlowInfo_add_place(env->pile,_tmp1ED);
goto _LLA5;_LLBA: _tmp1EE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1EF=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1EE;if(_tmp1EF->tag != 1)
goto _LLBC;};_tmp1F0=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp1F1=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLBC;};
_LLBB: goto _LLBD;_LLBC: _tmp1F2=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1F3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F2;if(_tmp1F3->tag != 2)
goto _LLBE;};_tmp1F4=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1F5=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1F4;if(_tmp1F5->tag != 1)goto _LLBE;};
_LLBD: return(env->fenv)->notzerothis;_LLBE: _tmp1F6=_tmp1B9.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1F7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F6;if(_tmp1F7->tag != 6)goto
_LLC0;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;}};_tmp1FA=_tmp1B9.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1FB=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1FA;if(_tmp1FB->tag != 6)goto _LLC0;else{_tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;}};
_LLBF: {unsigned int _tmp59F;unsigned int _tmp59E;struct _dyneither_ptr _tmp59D;void**
_tmp59C;unsigned int _tmp59B;struct _dyneither_ptr new_d=(_tmp59B=
_get_dyneither_size(_tmp1F9,sizeof(void*)),((_tmp59C=(void**)_region_malloc(
_tmp1B7,_check_times(sizeof(void*),_tmp59B)),((_tmp59D=_tag_dyneither(_tmp59C,
sizeof(void*),_tmp59B),((((_tmp59E=_tmp59B,_tmp5A0(& env,& _tmp59F,& _tmp59E,&
_tmp59C,& _tmp1FD,& _tmp1F9))),_tmp59D)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1F9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1F9.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1F9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1FD,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5A3;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A2;return(void*)((_tmp5A2=_region_malloc(_tmp1B7,sizeof(*_tmp5A2)),((
_tmp5A2[0]=((_tmp5A3.tag=6,((_tmp5A3.f1=_tmp1F8,((_tmp5A3.f2=new_d,_tmp5A3)))))),
_tmp5A2))));};}_LLC0:;_LLC1: goto _LLA5;_LLA5:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5A4;struct
_tuple0 _tmp211=(_tmp5A4.f1=r1,((_tmp5A4.f2=r2,_tmp5A4)));void*_tmp212;void*
_tmp214;_LLD3: _tmp212=_tmp211.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp213=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp212;if(_tmp213->tag != 4)goto _LLD5;};_LLD4:
goto _LLD6;_LLD5: _tmp214=_tmp211.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp215=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp214;if(_tmp215->tag != 4)goto _LLD7;};_LLD6: {
struct _tuple19 _tmp5A5;struct _tuple19 _tmp217=(_tmp5A5.f1=il1,((_tmp5A5.f2=il2,
_tmp5A5)));enum Cyc_CfFlowInfo_InitLevel _tmp218;enum Cyc_CfFlowInfo_InitLevel
_tmp219;enum Cyc_CfFlowInfo_InitLevel _tmp21A;enum Cyc_CfFlowInfo_InitLevel
_tmp21B;_LLDA: _tmp218=_tmp217.f2;if(_tmp218 != Cyc_CfFlowInfo_NoneIL)goto _LLDC;
_LLDB: goto _LLDD;_LLDC: _tmp219=_tmp217.f1;if(_tmp219 != Cyc_CfFlowInfo_NoneIL)goto
_LLDE;_LLDD: return(env->fenv)->esc_none;_LLDE: _tmp21A=_tmp217.f2;if(_tmp21A != 
Cyc_CfFlowInfo_ThisIL)goto _LLE0;_LLDF: goto _LLE1;_LLE0: _tmp21B=_tmp217.f1;if(
_tmp21B != Cyc_CfFlowInfo_ThisIL)goto _LLE2;_LLE1: return(env->fenv)->esc_this;
_LLE2:;_LLE3: return(env->fenv)->esc_all;_LLD9:;}_LLD7:;_LLD8: {struct _tuple19
_tmp5A6;struct _tuple19 _tmp21D=(_tmp5A6.f1=il1,((_tmp5A6.f2=il2,_tmp5A6)));enum 
Cyc_CfFlowInfo_InitLevel _tmp21E;enum Cyc_CfFlowInfo_InitLevel _tmp21F;enum Cyc_CfFlowInfo_InitLevel
_tmp220;enum Cyc_CfFlowInfo_InitLevel _tmp221;_LLE5: _tmp21E=_tmp21D.f2;if(_tmp21E
!= Cyc_CfFlowInfo_NoneIL)goto _LLE7;_LLE6: goto _LLE8;_LLE7: _tmp21F=_tmp21D.f1;if(
_tmp21F != Cyc_CfFlowInfo_NoneIL)goto _LLE9;_LLE8: return(env->fenv)->unknown_none;
_LLE9: _tmp220=_tmp21D.f2;if(_tmp220 != Cyc_CfFlowInfo_ThisIL)goto _LLEB;_LLEA: goto
_LLEC;_LLEB: _tmp221=_tmp21D.f1;if(_tmp221 != Cyc_CfFlowInfo_ThisIL)goto _LLED;
_LLEC: return(env->fenv)->unknown_this;_LLED:;_LLEE: return(env->fenv)->unknown_all;
_LLE4:;}_LLD2:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5A7;return((_tmp5A7.EqualConst).val=i,(((_tmp5A7.EqualConst).tag=
1,_tmp5A7)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple10 _tmp5AA;union Cyc_CfFlowInfo_RelnOp _tmp5A9;return((
_tmp5A9.LessVar).val=((_tmp5AA.f1=v,((_tmp5AA.f2=t,_tmp5AA)))),(((_tmp5A9.LessVar).tag=
2,_tmp5A9)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp5AB;return((_tmp5AB.LessNumelts).val=v,(((
_tmp5AB.LessNumelts).tag=3,_tmp5AB)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5AC;return((_tmp5AC.LessConst).val=i,(((_tmp5AC.LessConst).tag=
4,_tmp5AC)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp
_tmp5AD;return((_tmp5AD.LessEqConst).val=i,(((_tmp5AD.LessEqConst).tag=6,_tmp5AD)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp5AE;return((_tmp5AE.LessEqNumelts).val=v,(((
_tmp5AE.LessEqNumelts).tag=5,_tmp5AE)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp5AF;struct _tuple20 _tmp22A=(
_tmp5AF.f1=r1,((_tmp5AF.f2=r2,_tmp5AF)));union Cyc_CfFlowInfo_RelnOp _tmp22B;
unsigned int _tmp22C;union Cyc_CfFlowInfo_RelnOp _tmp22D;unsigned int _tmp22E;union
Cyc_CfFlowInfo_RelnOp _tmp22F;struct _tuple10 _tmp230;struct Cyc_Absyn_Vardecl*
_tmp231;union Cyc_CfFlowInfo_RelnOp _tmp232;struct _tuple10 _tmp233;struct Cyc_Absyn_Vardecl*
_tmp234;union Cyc_CfFlowInfo_RelnOp _tmp235;struct Cyc_Absyn_Vardecl*_tmp236;union
Cyc_CfFlowInfo_RelnOp _tmp237;struct Cyc_Absyn_Vardecl*_tmp238;union Cyc_CfFlowInfo_RelnOp
_tmp239;unsigned int _tmp23A;union Cyc_CfFlowInfo_RelnOp _tmp23B;unsigned int
_tmp23C;union Cyc_CfFlowInfo_RelnOp _tmp23D;unsigned int _tmp23E;union Cyc_CfFlowInfo_RelnOp
_tmp23F;unsigned int _tmp240;union Cyc_CfFlowInfo_RelnOp _tmp241;struct Cyc_Absyn_Vardecl*
_tmp242;union Cyc_CfFlowInfo_RelnOp _tmp243;struct Cyc_Absyn_Vardecl*_tmp244;_LLF0:
_tmp22B=_tmp22A.f1;if((_tmp22B.EqualConst).tag != 1)goto _LLF2;_tmp22C=(
unsigned int)(_tmp22B.EqualConst).val;_tmp22D=_tmp22A.f2;if((_tmp22D.EqualConst).tag
!= 1)goto _LLF2;_tmp22E=(unsigned int)(_tmp22D.EqualConst).val;_LLF1: return
_tmp22C == _tmp22E;_LLF2: _tmp22F=_tmp22A.f1;if((_tmp22F.LessVar).tag != 2)goto
_LLF4;_tmp230=(struct _tuple10)(_tmp22F.LessVar).val;_tmp231=_tmp230.f1;_tmp232=
_tmp22A.f2;if((_tmp232.LessVar).tag != 2)goto _LLF4;_tmp233=(struct _tuple10)(
_tmp232.LessVar).val;_tmp234=_tmp233.f1;_LLF3: return _tmp231 == _tmp234;_LLF4:
_tmp235=_tmp22A.f1;if((_tmp235.LessNumelts).tag != 3)goto _LLF6;_tmp236=(struct Cyc_Absyn_Vardecl*)(
_tmp235.LessNumelts).val;_tmp237=_tmp22A.f2;if((_tmp237.LessNumelts).tag != 3)
goto _LLF6;_tmp238=(struct Cyc_Absyn_Vardecl*)(_tmp237.LessNumelts).val;_LLF5:
return _tmp236 == _tmp238;_LLF6: _tmp239=_tmp22A.f1;if((_tmp239.LessConst).tag != 4)
goto _LLF8;_tmp23A=(unsigned int)(_tmp239.LessConst).val;_tmp23B=_tmp22A.f2;if((
_tmp23B.LessConst).tag != 4)goto _LLF8;_tmp23C=(unsigned int)(_tmp23B.LessConst).val;
_LLF7: return _tmp23A == _tmp23C;_LLF8: _tmp23D=_tmp22A.f1;if((_tmp23D.LessEqConst).tag
!= 6)goto _LLFA;_tmp23E=(unsigned int)(_tmp23D.LessEqConst).val;_tmp23F=_tmp22A.f2;
if((_tmp23F.LessEqConst).tag != 6)goto _LLFA;_tmp240=(unsigned int)(_tmp23F.LessEqConst).val;
_LLF9: return _tmp23E == _tmp240;_LLFA: _tmp241=_tmp22A.f1;if((_tmp241.LessEqNumelts).tag
!= 5)goto _LLFC;_tmp242=(struct Cyc_Absyn_Vardecl*)(_tmp241.LessEqNumelts).val;
_tmp243=_tmp22A.f2;if((_tmp243.LessEqNumelts).tag != 5)goto _LLFC;_tmp244=(struct
Cyc_Absyn_Vardecl*)(_tmp243.LessEqNumelts).val;_LLFB: return _tmp242 == _tmp244;
_LLFC:;_LLFD: return 0;_LLEF:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp245=r1s;for(0;_tmp245 != 0;_tmp245=_tmp245->tl){
struct Cyc_CfFlowInfo_Reln*_tmp246=(struct Cyc_CfFlowInfo_Reln*)_tmp245->hd;int
found=0;{struct Cyc_List_List*_tmp247=r2s;for(0;_tmp247 != 0;_tmp247=_tmp247->tl){
struct Cyc_CfFlowInfo_Reln*_tmp248=(struct Cyc_CfFlowInfo_Reln*)_tmp247->hd;if(
_tmp246 == _tmp248  || _tmp246->vd == _tmp248->vd  && Cyc_CfFlowInfo_same_relop(
_tmp246->rop,_tmp248->rop)){{struct Cyc_List_List*_tmp5B0;res=((_tmp5B0=
_region_malloc(r,sizeof(*_tmp5B0)),((_tmp5B0->hd=_tmp246,((_tmp5B0->tl=res,
_tmp5B0))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;};}
struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp5B1;struct _tuple21 _tmp24B=(_tmp5B1.f1=f1,((_tmp5B1.f2=f2,
_tmp5B1)));union Cyc_CfFlowInfo_FlowInfo _tmp24C;int _tmp24D;union Cyc_CfFlowInfo_FlowInfo
_tmp24E;int _tmp24F;union Cyc_CfFlowInfo_FlowInfo _tmp250;struct _tuple11 _tmp251;
struct Cyc_Dict_Dict _tmp252;struct Cyc_List_List*_tmp253;union Cyc_CfFlowInfo_FlowInfo
_tmp254;struct _tuple11 _tmp255;struct Cyc_Dict_Dict _tmp256;struct Cyc_List_List*
_tmp257;_LLFF: _tmp24C=_tmp24B.f1;if((_tmp24C.BottomFL).tag != 1)goto _LL101;
_tmp24D=(int)(_tmp24C.BottomFL).val;_LL100: return f2;_LL101: _tmp24E=_tmp24B.f2;
if((_tmp24E.BottomFL).tag != 1)goto _LL103;_tmp24F=(int)(_tmp24E.BottomFL).val;
_LL102: return f1;_LL103: _tmp250=_tmp24B.f1;if((_tmp250.ReachableFL).tag != 2)goto
_LLFE;_tmp251=(struct _tuple11)(_tmp250.ReachableFL).val;_tmp252=_tmp251.f1;
_tmp253=_tmp251.f2;_tmp254=_tmp24B.f2;if((_tmp254.ReachableFL).tag != 2)goto _LLFE;
_tmp255=(struct _tuple11)(_tmp254.ReachableFL).val;_tmp256=_tmp255.f1;_tmp257=
_tmp255.f2;_LL104: if(_tmp252.t == _tmp256.t  && _tmp253 == _tmp257)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp258=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5B4;struct Cyc_CfFlowInfo_JoinEnv
_tmp5B3;struct Cyc_CfFlowInfo_JoinEnv _tmp259=(_tmp5B3.fenv=fenv,((_tmp5B3.pile=((
_tmp5B4=_region_malloc(_tmp258,sizeof(*_tmp5B4)),((_tmp5B4->rgn=_tmp258,((
_tmp5B4->places=0,_tmp5B4)))))),((_tmp5B3.d1=_tmp252,((_tmp5B3.d2=_tmp256,
_tmp5B3)))))));struct Cyc_Dict_Dict _tmp25A=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp259,_tmp252,_tmp256);struct Cyc_List_List*_tmp25B=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp253,_tmp257);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp259.pile,all_changed,_tmp25A),_tmp25B);};_LLFE:;}struct _tuple22{union
Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo
f3;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2);struct
_tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){union Cyc_CfFlowInfo_FlowInfo
_tmp25F;void*_tmp260;struct _tuple12 _tmp25E=pr1;_tmp25F=_tmp25E.f1;_tmp260=
_tmp25E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp262;void*_tmp263;struct _tuple12
_tmp261=pr2;_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;{union Cyc_CfFlowInfo_FlowInfo
outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp25F,_tmp262);struct
_tuple22 _tmp5B5;struct _tuple22 _tmp265=(_tmp5B5.f1=_tmp25F,((_tmp5B5.f2=_tmp262,((
_tmp5B5.f3=outflow,_tmp5B5)))));union Cyc_CfFlowInfo_FlowInfo _tmp266;int _tmp267;
union Cyc_CfFlowInfo_FlowInfo _tmp268;int _tmp269;union Cyc_CfFlowInfo_FlowInfo
_tmp26A;struct _tuple11 _tmp26B;struct Cyc_Dict_Dict _tmp26C;union Cyc_CfFlowInfo_FlowInfo
_tmp26D;struct _tuple11 _tmp26E;struct Cyc_Dict_Dict _tmp26F;union Cyc_CfFlowInfo_FlowInfo
_tmp270;struct _tuple11 _tmp271;struct Cyc_Dict_Dict _tmp272;struct Cyc_List_List*
_tmp273;_LL106: _tmp266=_tmp265.f1;if((_tmp266.BottomFL).tag != 1)goto _LL108;
_tmp267=(int)(_tmp266.BottomFL).val;_LL107: {struct _tuple12 _tmp5B6;return(
_tmp5B6.f1=outflow,((_tmp5B6.f2=_tmp263,_tmp5B6)));}_LL108: _tmp268=_tmp265.f2;
if((_tmp268.BottomFL).tag != 1)goto _LL10A;_tmp269=(int)(_tmp268.BottomFL).val;
_LL109: {struct _tuple12 _tmp5B7;return(_tmp5B7.f1=outflow,((_tmp5B7.f2=_tmp260,
_tmp5B7)));}_LL10A: _tmp26A=_tmp265.f1;if((_tmp26A.ReachableFL).tag != 2)goto
_LL10C;_tmp26B=(struct _tuple11)(_tmp26A.ReachableFL).val;_tmp26C=_tmp26B.f1;
_tmp26D=_tmp265.f2;if((_tmp26D.ReachableFL).tag != 2)goto _LL10C;_tmp26E=(struct
_tuple11)(_tmp26D.ReachableFL).val;_tmp26F=_tmp26E.f1;_tmp270=_tmp265.f3;if((
_tmp270.ReachableFL).tag != 2)goto _LL10C;_tmp271=(struct _tuple11)(_tmp270.ReachableFL).val;
_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;_LL10B: if(((int(*)(int ignore,void*r1,void*
r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp260,_tmp263)){struct _tuple12
_tmp5B8;return(_tmp5B8.f1=outflow,((_tmp5B8.f2=_tmp263,_tmp5B8)));}if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp263,
_tmp260)){struct _tuple12 _tmp5B9;return(_tmp5B9.f1=outflow,((_tmp5B9.f2=_tmp260,
_tmp5B9)));}{struct _RegionHandle*_tmp278=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5BC;struct Cyc_CfFlowInfo_JoinEnv _tmp5BB;struct Cyc_CfFlowInfo_JoinEnv _tmp279=(
_tmp5BB.fenv=fenv,((_tmp5BB.pile=((_tmp5BC=_region_malloc(_tmp278,sizeof(*
_tmp5BC)),((_tmp5BC->rgn=_tmp278,((_tmp5BC->places=0,_tmp5BC)))))),((_tmp5BB.d1=
_tmp26C,((_tmp5BB.d2=_tmp26F,_tmp5BB)))))));void*_tmp27A=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp279,0,_tmp260,
_tmp263);struct _tuple12 _tmp5BD;return(_tmp5BD.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp279.pile,all_changed,_tmp272),_tmp273),((_tmp5BD.f2=_tmp27A,_tmp5BD)));};
_LL10C:;_LL10D: {struct Cyc_Core_Impossible_struct _tmp5C3;const char*_tmp5C2;
struct Cyc_Core_Impossible_struct*_tmp5C1;(int)_throw((void*)((_tmp5C1=_cycalloc(
sizeof(*_tmp5C1)),((_tmp5C1[0]=((_tmp5C3.tag=Cyc_Core_Impossible,((_tmp5C3.f1=((
_tmp5C2="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp5C2,sizeof(
char),37))),_tmp5C3)))),_tmp5C1)))));}_LL105:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5CA(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5C9,unsigned int*_tmp5C8,void***_tmp5C6,struct
_dyneither_ptr*_tmp28E,struct _dyneither_ptr*_tmp28A){for(*_tmp5C9=0;*_tmp5C9 < *
_tmp5C8;(*_tmp5C9)++){(*_tmp5C6)[*_tmp5C9]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5C9,((void**)(*_tmp28A).curr)[(int)*_tmp5C9],*((void**)
_check_dyneither_subscript(*_tmp28E,sizeof(void*),(int)*_tmp5C9)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5C4;
struct _tuple0 _tmp282=(_tmp5C4.f1=r1,((_tmp5C4.f2=r2,_tmp5C4)));void*_tmp283;void*
_tmp285;void*_tmp287;struct Cyc_CfFlowInfo_UnionRInfo _tmp289;struct _dyneither_ptr
_tmp28A;void*_tmp28B;struct Cyc_CfFlowInfo_UnionRInfo _tmp28D;struct _dyneither_ptr
_tmp28E;_LL10F: _tmp283=_tmp282.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp284=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp283;if(_tmp284->tag != 7)goto _LL111;};
_LL110: goto _LL112;_LL111: _tmp285=_tmp282.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp286=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp285;if(_tmp286->tag != 7)goto
_LL113;};_LL112: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL113: _tmp287=
_tmp282.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp288=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp287;if(_tmp288->tag != 6)goto _LL115;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}};
_tmp28B=_tmp282.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp28C=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp28B;if(_tmp28C->tag != 6)goto _LL115;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;}};
_LL114: {unsigned int _tmp5C9;unsigned int _tmp5C8;struct _dyneither_ptr _tmp5C7;
void**_tmp5C6;unsigned int _tmp5C5;struct _dyneither_ptr new_d=(_tmp5C5=
_get_dyneither_size(_tmp28A,sizeof(void*)),((_tmp5C6=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp5C5)),((_tmp5C7=
_tag_dyneither(_tmp5C6,sizeof(void*),_tmp5C5),((((_tmp5C8=_tmp5C5,_tmp5CA(& env,&
_tmp5C9,& _tmp5C8,& _tmp5C6,& _tmp28E,& _tmp28A))),_tmp5C7)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp28A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp28A.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp28A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp28E,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5CD;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5CC;return(void*)((_tmp5CC=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp5CC)),((_tmp5CC[0]=((_tmp5CD.tag=6,((_tmp5CD.f1=_tmp289,((_tmp5CD.f2=new_d,
_tmp5CD)))))),_tmp5CC))));};}_LL115:;_LL116: {struct Cyc_Core_Impossible_struct
_tmp5D3;const char*_tmp5D2;struct Cyc_Core_Impossible_struct*_tmp5D1;(int)_throw((
void*)((_tmp5D1=_cycalloc(sizeof(*_tmp5D1)),((_tmp5D1[0]=((_tmp5D3.tag=Cyc_Core_Impossible,((
_tmp5D3.f1=((_tmp5D2="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5D2,
sizeof(char),33))),_tmp5D3)))),_tmp5D1)))));}_LL10E:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp298=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp299=env->changed;{struct Cyc_List_List*_tmp5D4;*env->last_field_cell=((
_tmp5D4=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5D4)),((_tmp5D4->hd=(
void*)key,((_tmp5D4->tl=0,_tmp5D4))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp29B=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp298;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp299;return _tmp29B;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp5D5;*env->curr_place=((_tmp5D5.root=root,((_tmp5D5.fields=
0,_tmp5D5))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple21 _tmp5D6;struct _tuple21
_tmp29E=(_tmp5D6.f1=f1,((_tmp5D6.f2=f2,_tmp5D6)));union Cyc_CfFlowInfo_FlowInfo
_tmp29F;int _tmp2A0;union Cyc_CfFlowInfo_FlowInfo _tmp2A1;int _tmp2A2;union Cyc_CfFlowInfo_FlowInfo
_tmp2A3;struct _tuple11 _tmp2A4;struct Cyc_Dict_Dict _tmp2A5;struct Cyc_List_List*
_tmp2A6;union Cyc_CfFlowInfo_FlowInfo _tmp2A7;struct _tuple11 _tmp2A8;struct Cyc_Dict_Dict
_tmp2A9;struct Cyc_List_List*_tmp2AA;_LL118: _tmp29F=_tmp29E.f1;if((_tmp29F.BottomFL).tag
!= 1)goto _LL11A;_tmp2A0=(int)(_tmp29F.BottomFL).val;_LL119: goto _LL11B;_LL11A:
_tmp2A1=_tmp29E.f2;if((_tmp2A1.BottomFL).tag != 1)goto _LL11C;_tmp2A2=(int)(
_tmp2A1.BottomFL).val;_LL11B: return Cyc_CfFlowInfo_BottomFL();_LL11C: _tmp2A3=
_tmp29E.f1;if((_tmp2A3.ReachableFL).tag != 2)goto _LL117;_tmp2A4=(struct _tuple11)(
_tmp2A3.ReachableFL).val;_tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_tmp2A7=_tmp29E.f2;
if((_tmp2A7.ReachableFL).tag != 2)goto _LL117;_tmp2A8=(struct _tuple11)(_tmp2A7.ReachableFL).val;
_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_LL11D: if(_tmp2A5.t == _tmp2A9.t  && _tmp2A6
== _tmp2AA)return f1;{struct _RegionHandle*_tmp2AB=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp2AC=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp5DC;struct Cyc_CfFlowInfo_EscPile*
_tmp5DB;struct Cyc_CfFlowInfo_AfterEnv _tmp5DA;struct Cyc_CfFlowInfo_AfterEnv
_tmp2AD=(_tmp5DA.joinenv=((_tmp5DC.fenv=fenv,((_tmp5DC.pile=((_tmp5DB=
_region_malloc(_tmp2AB,sizeof(*_tmp5DB)),((_tmp5DB->rgn=_tmp2AB,((_tmp5DB->places=
0,_tmp5DB)))))),((_tmp5DC.d1=_tmp2A5,((_tmp5DC.d2=_tmp2A9,_tmp5DC)))))))),((
_tmp5DA.chg1=chg1,((_tmp5DA.chg2=chg2,((_tmp5DA.curr_place=_tmp2AC,((_tmp5DA.last_field_cell=(
struct Cyc_List_List**)& _tmp2AC->fields,((_tmp5DA.changed=Cyc_CfFlowInfo_Neither,
_tmp5DA)))))))))));struct Cyc_Dict_Dict _tmp2AE=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp2AD,_tmp2A5,_tmp2A9);struct Cyc_List_List*_tmp2AF=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp2A6,_tmp2AA);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp2AD.joinenv).pile,all_changed,_tmp2AE),_tmp2AF);};_LL117:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp5DD;struct
_tuple0 _tmp2B4=(_tmp5DD.f1=r1,((_tmp5DD.f2=r2,_tmp5DD)));void*_tmp2B5;struct Cyc_CfFlowInfo_Place*
_tmp2B7;void*_tmp2B8;struct Cyc_CfFlowInfo_Place*_tmp2BA;void*_tmp2BB;void*
_tmp2BD;void*_tmp2BF;struct Cyc_CfFlowInfo_UnionRInfo _tmp2C1;struct _dyneither_ptr
_tmp2C2;void*_tmp2C3;struct Cyc_CfFlowInfo_UnionRInfo _tmp2C5;struct _dyneither_ptr
_tmp2C6;void*_tmp2C7;void*_tmp2C9;void*_tmp2CB;void*_tmp2CD;void*_tmp2CF;void*
_tmp2D1;void*_tmp2D3;void*_tmp2D5;_LL11F: _tmp2B5=_tmp2B4.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2B6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2B5;if(_tmp2B6->tag != 5)goto
_LL121;else{_tmp2B7=_tmp2B6->f1;}};_tmp2B8=_tmp2B4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2B9=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2B8;if(_tmp2B9->tag != 5)goto
_LL121;else{_tmp2BA=_tmp2B9->f1;}};_LL120: return Cyc_CfFlowInfo_place_cmp(_tmp2B7,
_tmp2BA)== 0;_LL121: _tmp2BB=_tmp2B4.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2BC=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2BB;if(_tmp2BC->tag != 5)goto
_LL123;};_LL122: goto _LL124;_LL123: _tmp2BD=_tmp2B4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2BE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2BD;if(_tmp2BE->tag != 5)goto
_LL125;};_LL124: return 0;_LL125: _tmp2BF=_tmp2B4.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2C0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2BF;if(_tmp2C0->tag != 6)goto
_LL127;else{_tmp2C1=_tmp2C0->f1;_tmp2C2=_tmp2C0->f2;}};_tmp2C3=_tmp2B4.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2C4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2C3;if(_tmp2C4->tag != 6)goto _LL127;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;}};
_LL126: if((void**)_tmp2C2.curr == (void**)_tmp2C6.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2C2,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2C2.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2C6,sizeof(void*),i))))return 0;}}return 1;
_LL127: _tmp2C7=_tmp2B4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2C8=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2C7;if(_tmp2C8->tag != 1)goto _LL129;};
_tmp2C9=_tmp2B4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2CA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2C9;if(_tmp2CA->tag != 2)goto _LL129;};_LL128: return 1;_LL129: _tmp2CB=_tmp2B4.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2CC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2CB;if(_tmp2CC->tag != 2)goto _LL12B;};_LL12A: return 0;_LL12B: _tmp2CD=_tmp2B4.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2CE=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2CD;if(_tmp2CE->tag != 0)goto _LL12D;};_LL12C: goto _LL12E;_LL12D: _tmp2CF=
_tmp2B4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2D0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2CF;if(_tmp2D0->tag != 1)goto _LL12F;};_LL12E: return 0;_LL12F: _tmp2D1=_tmp2B4.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2D2=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2D1;if(_tmp2D2->tag != 4)goto _LL131;};_tmp2D3=_tmp2B4.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2D4=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2D3;if(_tmp2D4->tag != 4)goto _LL131;};
_LL130: goto _LL11E;_LL131: _tmp2D5=_tmp2B4.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2D6=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2D5;if(_tmp2D6->tag != 4)goto _LL133;};
_LL132: return 0;_LL133:;_LL134: goto _LL11E;_LL11E:;}{struct _tuple19 _tmp5DE;struct
_tuple19 _tmp2D8=(_tmp5DE.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5DE.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp5DE)));enum Cyc_CfFlowInfo_InitLevel _tmp2D9;enum Cyc_CfFlowInfo_InitLevel
_tmp2DA;enum Cyc_CfFlowInfo_InitLevel _tmp2DB;enum Cyc_CfFlowInfo_InitLevel
_tmp2DC;enum Cyc_CfFlowInfo_InitLevel _tmp2DD;enum Cyc_CfFlowInfo_InitLevel
_tmp2DE;_LL136: _tmp2D9=_tmp2D8.f1;if(_tmp2D9 != Cyc_CfFlowInfo_AllIL)goto _LL138;
_tmp2DA=_tmp2D8.f2;if(_tmp2DA != Cyc_CfFlowInfo_AllIL)goto _LL138;_LL137: return 1;
_LL138: _tmp2DB=_tmp2D8.f2;if(_tmp2DB != Cyc_CfFlowInfo_NoneIL)goto _LL13A;_LL139:
return 1;_LL13A: _tmp2DC=_tmp2D8.f1;if(_tmp2DC != Cyc_CfFlowInfo_NoneIL)goto _LL13C;
_LL13B: return 0;_LL13C: _tmp2DD=_tmp2D8.f2;if(_tmp2DD != Cyc_CfFlowInfo_ThisIL)goto
_LL13E;_LL13D: return 1;_LL13E: _tmp2DE=_tmp2D8.f1;if(_tmp2DE != Cyc_CfFlowInfo_ThisIL)
goto _LL135;_LL13F: return 0;_LL135:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2DF=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp2E0=r2s;for(0;_tmp2E0 != 0;_tmp2E0=
_tmp2E0->tl){struct Cyc_CfFlowInfo_Reln*_tmp2E1=(struct Cyc_CfFlowInfo_Reln*)
_tmp2E0->hd;if(_tmp2DF == _tmp2E1  || _tmp2DF->vd == _tmp2E1->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2DF->rop,_tmp2E1->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp5DF;struct _tuple21 _tmp2E3=(_tmp5DF.f1=f1,((_tmp5DF.f2=f2,_tmp5DF)));union Cyc_CfFlowInfo_FlowInfo
_tmp2E4;int _tmp2E5;union Cyc_CfFlowInfo_FlowInfo _tmp2E6;int _tmp2E7;union Cyc_CfFlowInfo_FlowInfo
_tmp2E8;struct _tuple11 _tmp2E9;struct Cyc_Dict_Dict _tmp2EA;struct Cyc_List_List*
_tmp2EB;union Cyc_CfFlowInfo_FlowInfo _tmp2EC;struct _tuple11 _tmp2ED;struct Cyc_Dict_Dict
_tmp2EE;struct Cyc_List_List*_tmp2EF;_LL141: _tmp2E4=_tmp2E3.f1;if((_tmp2E4.BottomFL).tag
!= 1)goto _LL143;_tmp2E5=(int)(_tmp2E4.BottomFL).val;_LL142: return 1;_LL143:
_tmp2E6=_tmp2E3.f2;if((_tmp2E6.BottomFL).tag != 1)goto _LL145;_tmp2E7=(int)(
_tmp2E6.BottomFL).val;_LL144: return 0;_LL145: _tmp2E8=_tmp2E3.f1;if((_tmp2E8.ReachableFL).tag
!= 2)goto _LL140;_tmp2E9=(struct _tuple11)(_tmp2E8.ReachableFL).val;_tmp2EA=
_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;_tmp2EC=_tmp2E3.f2;if((_tmp2EC.ReachableFL).tag != 
2)goto _LL140;_tmp2ED=(struct _tuple11)(_tmp2EC.ReachableFL).val;_tmp2EE=_tmp2ED.f1;
_tmp2EF=_tmp2ED.f2;_LL146: if(_tmp2EA.t == _tmp2EE.t  && _tmp2EB == _tmp2EF)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2EA,_tmp2EE) && Cyc_CfFlowInfo_relns_approx(_tmp2EB,_tmp2EF);_LL140:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2F0=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2F0->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp2F1=_tmp2F0->rop;struct _tuple10 _tmp2F2;struct Cyc_Absyn_Vardecl*
_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;_LL148:
if((_tmp2F1.LessVar).tag != 2)goto _LL14A;_tmp2F2=(struct _tuple10)(_tmp2F1.LessVar).val;
_tmp2F3=_tmp2F2.f1;_LL149: _tmp2F4=_tmp2F3;goto _LL14B;_LL14A: if((_tmp2F1.LessNumelts).tag
!= 3)goto _LL14C;_tmp2F4=(struct Cyc_Absyn_Vardecl*)(_tmp2F1.LessNumelts).val;
_LL14B: _tmp2F5=_tmp2F4;goto _LL14D;_LL14C: if((_tmp2F1.LessEqNumelts).tag != 5)goto
_LL14E;_tmp2F5=(struct Cyc_Absyn_Vardecl*)(_tmp2F1.LessEqNumelts).val;_LL14D: if(v
== _tmp2F5)found=1;goto _LL147;_LL14E:;_LL14F: goto _LL147;_LL147:;};}if(!found)
return rs;{struct Cyc_List_List*_tmp2F6=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2F7=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2F7->vd != v){{union Cyc_CfFlowInfo_RelnOp
_tmp2F8=_tmp2F7->rop;struct _tuple10 _tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FA;
struct Cyc_Absyn_Vardecl*_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FC;_LL151: if((
_tmp2F8.LessVar).tag != 2)goto _LL153;_tmp2F9=(struct _tuple10)(_tmp2F8.LessVar).val;
_tmp2FA=_tmp2F9.f1;_LL152: _tmp2FB=_tmp2FA;goto _LL154;_LL153: if((_tmp2F8.LessNumelts).tag
!= 3)goto _LL155;_tmp2FB=(struct Cyc_Absyn_Vardecl*)(_tmp2F8.LessNumelts).val;
_LL154: _tmp2FC=_tmp2FB;goto _LL156;_LL155: if((_tmp2F8.LessEqNumelts).tag != 5)goto
_LL157;_tmp2FC=(struct Cyc_Absyn_Vardecl*)(_tmp2F8.LessEqNumelts).val;_LL156: if(v
== _tmp2FC)continue;goto _LL150;_LL157:;_LL158: goto _LL150;_LL150:;}{struct Cyc_List_List*
_tmp5E0;_tmp2F6=((_tmp5E0=_region_malloc(rgn,sizeof(*_tmp5E0)),((_tmp5E0->hd=
_tmp2F7,((_tmp5E0->tl=_tmp2F6,_tmp5E0))))));};}}return _tmp2F6;};}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct
_RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp2FE=e->r;
void*_tmp300;struct Cyc_Absyn_Vardecl*_tmp302;void*_tmp304;struct Cyc_Absyn_Vardecl*
_tmp306;void*_tmp308;struct Cyc_Absyn_Vardecl*_tmp30A;void*_tmp30C;struct Cyc_Absyn_Vardecl*
_tmp30E;_LL15A: {struct Cyc_Absyn_Var_e_struct*_tmp2FF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FE;if(_tmp2FF->tag != 1)goto _LL15C;else{_tmp300=(void*)_tmp2FF->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp301=(struct Cyc_Absyn_Global_b_struct*)_tmp300;if(
_tmp301->tag != 1)goto _LL15C;else{_tmp302=_tmp301->f1;}};}}_LL15B: _tmp306=_tmp302;
goto _LL15D;_LL15C: {struct Cyc_Absyn_Var_e_struct*_tmp303=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FE;if(_tmp303->tag != 1)goto _LL15E;else{_tmp304=(void*)_tmp303->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp305=(struct Cyc_Absyn_Param_b_struct*)_tmp304;if(
_tmp305->tag != 3)goto _LL15E;else{_tmp306=_tmp305->f1;}};}}_LL15D: _tmp30A=_tmp306;
goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_struct*_tmp307=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FE;if(_tmp307->tag != 1)goto _LL160;else{_tmp308=(void*)_tmp307->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp309=(struct Cyc_Absyn_Local_b_struct*)_tmp308;if(
_tmp309->tag != 4)goto _LL160;else{_tmp30A=_tmp309->f1;}};}}_LL15F: _tmp30E=_tmp30A;
goto _LL161;_LL160: {struct Cyc_Absyn_Var_e_struct*_tmp30B=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FE;if(_tmp30B->tag != 1)goto _LL162;else{_tmp30C=(void*)_tmp30B->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp30D=(struct Cyc_Absyn_Pat_b_struct*)_tmp30C;if(_tmp30D->tag
!= 5)goto _LL162;else{_tmp30E=_tmp30D->f1;}};}}_LL161: if(!_tmp30E->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp30E);goto _LL159;_LL162:;_LL163: goto _LL159;
_LL159:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*
_tmp5E1;return(_tmp5E1=_region_malloc(r2,sizeof(*_tmp5E1)),((_tmp5E1[0]=*r,
_tmp5E1)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,
struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp310=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp311=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp310->vd != _tmp311->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp310->rop,_tmp311->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp312=e->r;struct Cyc_Absyn_MallocInfo _tmp314;struct Cyc_Absyn_Exp*
_tmp315;int _tmp316;_LL165: {struct Cyc_Absyn_Malloc_e_struct*_tmp313=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp312;if(_tmp313->tag != 34)goto _LL167;else{_tmp314=_tmp313->f1;_tmp315=_tmp314.num_elts;
_tmp316=_tmp314.fat_result;if(_tmp316 != 1)goto _LL167;}}_LL166: malloc_loop: {void*
_tmp317=_tmp315->r;struct Cyc_Absyn_Exp*_tmp319;void*_tmp31B;struct Cyc_Absyn_Vardecl*
_tmp31D;void*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp321;void*_tmp323;struct Cyc_Absyn_Vardecl*
_tmp325;void*_tmp327;struct Cyc_Absyn_Vardecl*_tmp329;_LL16A: {struct Cyc_Absyn_Cast_e_struct*
_tmp318=(struct Cyc_Absyn_Cast_e_struct*)_tmp317;if(_tmp318->tag != 15)goto _LL16C;
else{_tmp319=_tmp318->f2;}}_LL16B: _tmp315=_tmp319;goto malloc_loop;_LL16C: {
struct Cyc_Absyn_Var_e_struct*_tmp31A=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(
_tmp31A->tag != 1)goto _LL16E;else{_tmp31B=(void*)_tmp31A->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp31C=(struct Cyc_Absyn_Pat_b_struct*)_tmp31B;if(_tmp31C->tag != 5)goto _LL16E;
else{_tmp31D=_tmp31C->f1;}};}}_LL16D: _tmp321=_tmp31D;goto _LL16F;_LL16E: {struct
Cyc_Absyn_Var_e_struct*_tmp31E=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp31E->tag
!= 1)goto _LL170;else{_tmp31F=(void*)_tmp31E->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp320=(struct Cyc_Absyn_Local_b_struct*)_tmp31F;if(_tmp320->tag != 4)goto _LL170;
else{_tmp321=_tmp320->f1;}};}}_LL16F: _tmp325=_tmp321;goto _LL171;_LL170: {struct
Cyc_Absyn_Var_e_struct*_tmp322=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp322->tag
!= 1)goto _LL172;else{_tmp323=(void*)_tmp322->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp324=(struct Cyc_Absyn_Param_b_struct*)_tmp323;if(_tmp324->tag != 3)goto _LL172;
else{_tmp325=_tmp324->f1;}};}}_LL171: _tmp329=_tmp325;goto _LL173;_LL172: {struct
Cyc_Absyn_Var_e_struct*_tmp326=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp326->tag
!= 1)goto _LL174;else{_tmp327=(void*)_tmp326->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp328=(struct Cyc_Absyn_Global_b_struct*)_tmp327;if(_tmp328->tag != 1)goto _LL174;
else{_tmp329=_tmp328->f1;}};}}_LL173: if(_tmp329->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp5E4;struct Cyc_List_List*_tmp5E3;return(_tmp5E3=_region_malloc(rgn,sizeof(*
_tmp5E3)),((_tmp5E3->hd=((_tmp5E4=_region_malloc(rgn,sizeof(*_tmp5E4)),((_tmp5E4->vd=
_tmp329,((_tmp5E4->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp5E4)))))),((_tmp5E3->tl=
r,_tmp5E3)))));};_LL174:;_LL175: return r;_LL169:;}_LL167:;_LL168: goto _LL164;
_LL164:;}{void*_tmp32C=Cyc_Tcutil_compress(v->type);_LL177: {struct Cyc_Absyn_IntType_struct*
_tmp32D=(struct Cyc_Absyn_IntType_struct*)_tmp32C;if(_tmp32D->tag != 6)goto _LL179;}
_LL178: goto _LL176;_LL179:;_LL17A: return r;_LL176:;}loop: {void*_tmp32E=e->r;
struct Cyc_Absyn_Exp*_tmp330;union Cyc_Absyn_Cnst _tmp332;struct _tuple6 _tmp333;int
_tmp334;enum Cyc_Absyn_Primop _tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List
_tmp338;struct Cyc_List_List*_tmp339;struct Cyc_List_List _tmp33A;struct Cyc_Absyn_Exp*
_tmp33B;enum Cyc_Absyn_Primop _tmp33D;struct Cyc_List_List*_tmp33E;struct Cyc_List_List
_tmp33F;struct Cyc_Absyn_Exp*_tmp340;_LL17C: {struct Cyc_Absyn_Cast_e_struct*
_tmp32F=(struct Cyc_Absyn_Cast_e_struct*)_tmp32E;if(_tmp32F->tag != 15)goto _LL17E;
else{_tmp330=_tmp32F->f2;}}_LL17D: e=_tmp330;goto loop;_LL17E: {struct Cyc_Absyn_Const_e_struct*
_tmp331=(struct Cyc_Absyn_Const_e_struct*)_tmp32E;if(_tmp331->tag != 0)goto _LL180;
else{_tmp332=_tmp331->f1;if((_tmp332.Int_c).tag != 4)goto _LL180;_tmp333=(struct
_tuple6)(_tmp332.Int_c).val;_tmp334=_tmp333.f2;}}_LL17F: {struct Cyc_CfFlowInfo_Reln*
_tmp5E7;struct Cyc_List_List*_tmp5E6;return(_tmp5E6=_region_malloc(rgn,sizeof(*
_tmp5E6)),((_tmp5E6->hd=((_tmp5E7=_region_malloc(rgn,sizeof(*_tmp5E7)),((_tmp5E7->vd=
v,((_tmp5E7->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp334),_tmp5E7)))))),((
_tmp5E6->tl=r,_tmp5E6)))));}_LL180: {struct Cyc_Absyn_Primop_e_struct*_tmp335=(
struct Cyc_Absyn_Primop_e_struct*)_tmp32E;if(_tmp335->tag != 3)goto _LL182;else{
_tmp336=_tmp335->f1;if(_tmp336 != Cyc_Absyn_Mod)goto _LL182;_tmp337=_tmp335->f2;
if(_tmp337 == 0)goto _LL182;_tmp338=*_tmp337;_tmp339=_tmp338.tl;if(_tmp339 == 0)
goto _LL182;_tmp33A=*_tmp339;_tmp33B=(struct Cyc_Absyn_Exp*)_tmp33A.hd;}}_LL181:{
void*_tmp343=_tmp33B->r;enum Cyc_Absyn_Primop _tmp345;struct Cyc_List_List*_tmp346;
struct Cyc_List_List _tmp347;struct Cyc_Absyn_Exp*_tmp348;_LL187: {struct Cyc_Absyn_Primop_e_struct*
_tmp344=(struct Cyc_Absyn_Primop_e_struct*)_tmp343;if(_tmp344->tag != 3)goto _LL189;
else{_tmp345=_tmp344->f1;_tmp346=_tmp344->f2;if(_tmp346 == 0)goto _LL189;_tmp347=*
_tmp346;_tmp348=(struct Cyc_Absyn_Exp*)_tmp347.hd;}}_LL188:{void*_tmp349=_tmp348->r;
void*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34D;void*_tmp34F;struct Cyc_Absyn_Vardecl*
_tmp351;void*_tmp353;struct Cyc_Absyn_Vardecl*_tmp355;void*_tmp357;struct Cyc_Absyn_Vardecl*
_tmp359;_LL18C: {struct Cyc_Absyn_Var_e_struct*_tmp34A=(struct Cyc_Absyn_Var_e_struct*)
_tmp349;if(_tmp34A->tag != 1)goto _LL18E;else{_tmp34B=(void*)_tmp34A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp34C=(struct Cyc_Absyn_Global_b_struct*)_tmp34B;if(
_tmp34C->tag != 1)goto _LL18E;else{_tmp34D=_tmp34C->f1;}};}}_LL18D: _tmp351=_tmp34D;
goto _LL18F;_LL18E: {struct Cyc_Absyn_Var_e_struct*_tmp34E=(struct Cyc_Absyn_Var_e_struct*)
_tmp349;if(_tmp34E->tag != 1)goto _LL190;else{_tmp34F=(void*)_tmp34E->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp350=(struct Cyc_Absyn_Local_b_struct*)_tmp34F;if(
_tmp350->tag != 4)goto _LL190;else{_tmp351=_tmp350->f1;}};}}_LL18F: _tmp355=_tmp351;
goto _LL191;_LL190: {struct Cyc_Absyn_Var_e_struct*_tmp352=(struct Cyc_Absyn_Var_e_struct*)
_tmp349;if(_tmp352->tag != 1)goto _LL192;else{_tmp353=(void*)_tmp352->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp354=(struct Cyc_Absyn_Param_b_struct*)_tmp353;if(
_tmp354->tag != 3)goto _LL192;else{_tmp355=_tmp354->f1;}};}}_LL191: _tmp359=_tmp355;
goto _LL193;_LL192: {struct Cyc_Absyn_Var_e_struct*_tmp356=(struct Cyc_Absyn_Var_e_struct*)
_tmp349;if(_tmp356->tag != 1)goto _LL194;else{_tmp357=(void*)_tmp356->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp358=(struct Cyc_Absyn_Pat_b_struct*)_tmp357;if(_tmp358->tag
!= 5)goto _LL194;else{_tmp359=_tmp358->f1;}};}}_LL193: if(_tmp359->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5EA;struct Cyc_List_List*_tmp5E9;return(_tmp5E9=
_region_malloc(rgn,sizeof(*_tmp5E9)),((_tmp5E9->hd=((_tmp5EA=_region_malloc(rgn,
sizeof(*_tmp5EA)),((_tmp5EA->vd=v,((_tmp5EA->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp359),_tmp5EA)))))),((_tmp5E9->tl=r,_tmp5E9)))));};_LL194:;_LL195: goto _LL18B;
_LL18B:;}goto _LL186;_LL189:;_LL18A: goto _LL186;_LL186:;}goto _LL17B;_LL182: {
struct Cyc_Absyn_Primop_e_struct*_tmp33C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp32E;if(_tmp33C->tag != 3)goto _LL184;else{_tmp33D=_tmp33C->f1;_tmp33E=_tmp33C->f2;
if(_tmp33E == 0)goto _LL184;_tmp33F=*_tmp33E;_tmp340=(struct Cyc_Absyn_Exp*)_tmp33F.hd;}}
_LL183:{void*_tmp35C=_tmp340->r;void*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp360;
void*_tmp362;struct Cyc_Absyn_Vardecl*_tmp364;void*_tmp366;struct Cyc_Absyn_Vardecl*
_tmp368;void*_tmp36A;struct Cyc_Absyn_Vardecl*_tmp36C;_LL197: {struct Cyc_Absyn_Var_e_struct*
_tmp35D=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp35D->tag != 1)goto _LL199;
else{_tmp35E=(void*)_tmp35D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp35F=(struct
Cyc_Absyn_Global_b_struct*)_tmp35E;if(_tmp35F->tag != 1)goto _LL199;else{_tmp360=
_tmp35F->f1;}};}}_LL198: _tmp364=_tmp360;goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*
_tmp361=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp361->tag != 1)goto _LL19B;
else{_tmp362=(void*)_tmp361->f2;{struct Cyc_Absyn_Local_b_struct*_tmp363=(struct
Cyc_Absyn_Local_b_struct*)_tmp362;if(_tmp363->tag != 4)goto _LL19B;else{_tmp364=
_tmp363->f1;}};}}_LL19A: _tmp368=_tmp364;goto _LL19C;_LL19B: {struct Cyc_Absyn_Var_e_struct*
_tmp365=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp365->tag != 1)goto _LL19D;
else{_tmp366=(void*)_tmp365->f2;{struct Cyc_Absyn_Param_b_struct*_tmp367=(struct
Cyc_Absyn_Param_b_struct*)_tmp366;if(_tmp367->tag != 3)goto _LL19D;else{_tmp368=
_tmp367->f1;}};}}_LL19C: _tmp36C=_tmp368;goto _LL19E;_LL19D: {struct Cyc_Absyn_Var_e_struct*
_tmp369=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp369->tag != 1)goto _LL19F;
else{_tmp36A=(void*)_tmp369->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp36B=(struct Cyc_Absyn_Pat_b_struct*)
_tmp36A;if(_tmp36B->tag != 5)goto _LL19F;else{_tmp36C=_tmp36B->f1;}};}}_LL19E: if(
_tmp36C->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp5ED;struct Cyc_List_List*
_tmp5EC;return(_tmp5EC=_region_malloc(rgn,sizeof(*_tmp5EC)),((_tmp5EC->hd=((
_tmp5ED=_region_malloc(rgn,sizeof(*_tmp5ED)),((_tmp5ED->vd=v,((_tmp5ED->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp36C),_tmp5ED)))))),((_tmp5EC->tl=r,_tmp5EC)))));};_LL19F:;_LL1A0: goto _LL196;
_LL196:;}goto _LL17B;_LL184:;_LL185: goto _LL17B;_LL17B:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp36F=e1->r;void*_tmp371;struct Cyc_Absyn_Vardecl*_tmp373;void*
_tmp375;struct Cyc_Absyn_Vardecl*_tmp377;void*_tmp379;struct Cyc_Absyn_Vardecl*
_tmp37B;void*_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37F;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp370=(struct Cyc_Absyn_Var_e_struct*)_tmp36F;if(_tmp370->tag != 1)goto _LL1A4;
else{_tmp371=(void*)_tmp370->f2;{struct Cyc_Absyn_Global_b_struct*_tmp372=(struct
Cyc_Absyn_Global_b_struct*)_tmp371;if(_tmp372->tag != 1)goto _LL1A4;else{_tmp373=
_tmp372->f1;}};}}_LL1A3: _tmp377=_tmp373;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp374=(struct Cyc_Absyn_Var_e_struct*)_tmp36F;if(_tmp374->tag != 1)goto _LL1A6;
else{_tmp375=(void*)_tmp374->f2;{struct Cyc_Absyn_Param_b_struct*_tmp376=(struct
Cyc_Absyn_Param_b_struct*)_tmp375;if(_tmp376->tag != 3)goto _LL1A6;else{_tmp377=
_tmp376->f1;}};}}_LL1A5: _tmp37B=_tmp377;goto _LL1A7;_LL1A6: {struct Cyc_Absyn_Var_e_struct*
_tmp378=(struct Cyc_Absyn_Var_e_struct*)_tmp36F;if(_tmp378->tag != 1)goto _LL1A8;
else{_tmp379=(void*)_tmp378->f2;{struct Cyc_Absyn_Local_b_struct*_tmp37A=(struct
Cyc_Absyn_Local_b_struct*)_tmp379;if(_tmp37A->tag != 4)goto _LL1A8;else{_tmp37B=
_tmp37A->f1;}};}}_LL1A7: _tmp37F=_tmp37B;goto _LL1A9;_LL1A8: {struct Cyc_Absyn_Var_e_struct*
_tmp37C=(struct Cyc_Absyn_Var_e_struct*)_tmp36F;if(_tmp37C->tag != 1)goto _LL1AA;
else{_tmp37D=(void*)_tmp37C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp37E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp37D;if(_tmp37E->tag != 5)goto _LL1AA;else{_tmp37F=_tmp37E->f1;}};}}_LL1A9: if(!
_tmp37F->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp37F,e2);goto
_LL1A1;_LL1AA:;_LL1AB: goto _LL1A1;_LL1A1:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp5F1;void*_tmp5F0[1];struct Cyc_String_pa_struct _tmp5EF;(_tmp5EF.tag=
0,((_tmp5EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp5F0[0]=& _tmp5EF,Cyc_fprintf(Cyc_stderr,((_tmp5F1="%s",
_tag_dyneither(_tmp5F1,sizeof(char),3))),_tag_dyneither(_tmp5F0,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp383=r->rop;unsigned int _tmp384;struct _tuple10
_tmp385;struct Cyc_Absyn_Vardecl*_tmp386;struct Cyc_Absyn_Vardecl*_tmp387;
unsigned int _tmp388;unsigned int _tmp389;struct Cyc_Absyn_Vardecl*_tmp38A;_LL1AD:
if((_tmp383.EqualConst).tag != 1)goto _LL1AF;_tmp384=(unsigned int)(_tmp383.EqualConst).val;
_LL1AE:{const char*_tmp5F5;void*_tmp5F4[1];struct Cyc_Int_pa_struct _tmp5F3;(
_tmp5F3.tag=1,((_tmp5F3.f1=(unsigned long)((int)_tmp384),((_tmp5F4[0]=& _tmp5F3,
Cyc_fprintf(Cyc_stderr,((_tmp5F5="==%d",_tag_dyneither(_tmp5F5,sizeof(char),5))),
_tag_dyneither(_tmp5F4,sizeof(void*),1)))))));}goto _LL1AC;_LL1AF: if((_tmp383.LessVar).tag
!= 2)goto _LL1B1;_tmp385=(struct _tuple10)(_tmp383.LessVar).val;_tmp386=_tmp385.f1;
_LL1B0:{const char*_tmp5F9;void*_tmp5F8[1];struct Cyc_String_pa_struct _tmp5F7;(
_tmp5F7.tag=0,((_tmp5F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp386->name)),((_tmp5F8[0]=& _tmp5F7,Cyc_fprintf(Cyc_stderr,((_tmp5F9="<%s",
_tag_dyneither(_tmp5F9,sizeof(char),4))),_tag_dyneither(_tmp5F8,sizeof(void*),1)))))));}
goto _LL1AC;_LL1B1: if((_tmp383.LessNumelts).tag != 3)goto _LL1B3;_tmp387=(struct Cyc_Absyn_Vardecl*)(
_tmp383.LessNumelts).val;_LL1B2:{const char*_tmp5FD;void*_tmp5FC[1];struct Cyc_String_pa_struct
_tmp5FB;(_tmp5FB.tag=0,((_tmp5FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp387->name)),((_tmp5FC[0]=& _tmp5FB,Cyc_fprintf(Cyc_stderr,((
_tmp5FD="<numelts(%s)",_tag_dyneither(_tmp5FD,sizeof(char),13))),_tag_dyneither(
_tmp5FC,sizeof(void*),1)))))));}goto _LL1AC;_LL1B3: if((_tmp383.LessConst).tag != 4)
goto _LL1B5;_tmp388=(unsigned int)(_tmp383.LessConst).val;_LL1B4:{const char*
_tmp601;void*_tmp600[1];struct Cyc_Int_pa_struct _tmp5FF;(_tmp5FF.tag=1,((_tmp5FF.f1=(
unsigned long)((int)_tmp388),((_tmp600[0]=& _tmp5FF,Cyc_fprintf(Cyc_stderr,((
_tmp601="<%d",_tag_dyneither(_tmp601,sizeof(char),4))),_tag_dyneither(_tmp600,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B5: if((_tmp383.LessEqConst).tag != 6)goto
_LL1B7;_tmp389=(unsigned int)(_tmp383.LessEqConst).val;_LL1B6:{const char*_tmp605;
void*_tmp604[1];struct Cyc_Int_pa_struct _tmp603;(_tmp603.tag=1,((_tmp603.f1=(
unsigned long)((int)_tmp389),((_tmp604[0]=& _tmp603,Cyc_fprintf(Cyc_stderr,((
_tmp605="<=%d",_tag_dyneither(_tmp605,sizeof(char),5))),_tag_dyneither(_tmp604,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B7: if((_tmp383.LessEqNumelts).tag != 5)
goto _LL1AC;_tmp38A=(struct Cyc_Absyn_Vardecl*)(_tmp383.LessEqNumelts).val;_LL1B8:{
const char*_tmp609;void*_tmp608[1];struct Cyc_String_pa_struct _tmp607;(_tmp607.tag=
0,((_tmp607.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp38A->name)),((_tmp608[0]=& _tmp607,Cyc_fprintf(Cyc_stderr,((_tmp609="<=numelts(%s)",
_tag_dyneither(_tmp609,sizeof(char),14))),_tag_dyneither(_tmp608,sizeof(void*),1)))))));}
goto _LL1AC;_LL1AC:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp60C;void*_tmp60B;(
_tmp60B=0,Cyc_fprintf(Cyc_stderr,((_tmp60C=",",_tag_dyneither(_tmp60C,sizeof(
char),2))),_tag_dyneither(_tmp60B,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1B9:{const char*_tmp60F;void*_tmp60E;(
_tmp60E=0,Cyc_fprintf(Cyc_stderr,((_tmp60F="uninitialized",_tag_dyneither(
_tmp60F,sizeof(char),14))),_tag_dyneither(_tmp60E,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1BA:{const char*_tmp612;void*_tmp611;(_tmp611=0,Cyc_fprintf(
Cyc_stderr,((_tmp612="this-initialized",_tag_dyneither(_tmp612,sizeof(char),17))),
_tag_dyneither(_tmp611,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1BB:{
const char*_tmp615;void*_tmp614;(_tmp614=0,Cyc_fprintf(Cyc_stderr,((_tmp615="all-initialized",
_tag_dyneither(_tmp615,sizeof(char),16))),_tag_dyneither(_tmp614,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp3A5=root;struct Cyc_Absyn_Vardecl*_tmp3A7;struct Cyc_Absyn_Exp*
_tmp3A9;void*_tmp3AA;_LL1BE: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp3A6=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3A5;if(_tmp3A6->tag != 0)goto _LL1C0;
else{_tmp3A7=_tmp3A6->f1;}}_LL1BF:{const char*_tmp619;void*_tmp618[1];struct Cyc_String_pa_struct
_tmp617;(_tmp617.tag=0,((_tmp617.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3A7->name)),((_tmp618[0]=& _tmp617,Cyc_fprintf(Cyc_stderr,((
_tmp619="Root(%s)",_tag_dyneither(_tmp619,sizeof(char),9))),_tag_dyneither(
_tmp618,sizeof(void*),1)))))));}goto _LL1BD;_LL1C0: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp3A8=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp3A5;if(_tmp3A8->tag != 1)goto
_LL1C2;else{_tmp3A9=_tmp3A8->f1;_tmp3AA=(void*)_tmp3A8->f2;}}_LL1C1:{const char*
_tmp61E;void*_tmp61D[2];struct Cyc_String_pa_struct _tmp61C;struct Cyc_String_pa_struct
_tmp61B;(_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp3AA)),((_tmp61C.tag=0,((_tmp61C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3A9)),((_tmp61D[
0]=& _tmp61C,((_tmp61D[1]=& _tmp61B,Cyc_fprintf(Cyc_stderr,((_tmp61E="MallocPt(%s,%s)",
_tag_dyneither(_tmp61E,sizeof(char),16))),_tag_dyneither(_tmp61D,sizeof(void*),2)))))))))))));}
goto _LL1BD;_LL1C2: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp3AB=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3A5;if(_tmp3AB->tag != 2)goto _LL1BD;}_LL1C3:{const char*_tmp621;void*_tmp620;(
_tmp620=0,Cyc_fprintf(Cyc_stderr,((_tmp621="InitParam(_,_)",_tag_dyneither(
_tmp621,sizeof(char),15))),_tag_dyneither(_tmp620,sizeof(void*),0)));}goto _LL1BD;
_LL1BD:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*
_tmp3B5=p->fields;while(_tmp3B5 != 0){{const char*_tmp625;void*_tmp624[1];struct
Cyc_Int_pa_struct _tmp623;(_tmp623.tag=1,((_tmp623.f1=(unsigned long)((int)
_tmp3B5->hd),((_tmp624[0]=& _tmp623,Cyc_fprintf(Cyc_stderr,((_tmp625=".%d",
_tag_dyneither(_tmp625,sizeof(char),4))),_tag_dyneither(_tmp624,sizeof(void*),1)))))));}
_tmp3B5=_tmp3B5->tl;}{const char*_tmp628;void*_tmp627;(_tmp627=0,Cyc_fprintf(Cyc_stderr,((
_tmp628=" ",_tag_dyneither(_tmp628,sizeof(char),2))),_tag_dyneither(_tmp627,
sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*
pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp3BB=1;while(x != 0){if(!_tmp3BB){{const char*_tmp62B;void*_tmp62A;(_tmp62A=
0,Cyc_fprintf(Cyc_stderr,((_tmp62B=", ",_tag_dyneither(_tmp62B,sizeof(char),3))),
_tag_dyneither(_tmp62A,sizeof(void*),0)));}_tmp3BB=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp62E;void*_tmp62D;(_tmp62D=0,Cyc_fprintf(Cyc_stderr,((_tmp62E="\n",
_tag_dyneither(_tmp62E,sizeof(char),2))),_tag_dyneither(_tmp62D,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3C0=_new_region("r");
struct _RegionHandle*r=& _tmp3C0;_push_region(r);{struct _handler_cons _tmp3C1;
_push_handler(& _tmp3C1);{int _tmp3C3=0;if(setjmp(_tmp3C1.handler))_tmp3C3=1;if(!
_tmp3C3){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3C4=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp631;void*_tmp630;(_tmp630=0,Cyc_fprintf(Cyc_stderr,((
_tmp631="{ ",_tag_dyneither(_tmp631,sizeof(char),3))),_tag_dyneither(_tmp630,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3C4)){void*_tmp3C7=_tmp3C4.f1;if(!first){{const char*_tmp634;void*
_tmp633;(_tmp633=0,Cyc_fprintf(Cyc_stderr,((_tmp634=", ",_tag_dyneither(_tmp634,
sizeof(char),3))),_tag_dyneither(_tmp633,sizeof(void*),0)));}first=0;}pr(_tmp3C7);}{
const char*_tmp637;void*_tmp636;(_tmp636=0,Cyc_fprintf(Cyc_stderr,((_tmp637="}\n",
_tag_dyneither(_tmp637,sizeof(char),3))),_tag_dyneither(_tmp636,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3C2=(void*)_exn_thrown;void*_tmp3CD=_tmp3C2;_LL1C5:;
_LL1C6:{const char*_tmp63A;void*_tmp639;(_tmp639=0,Cyc_fprintf(Cyc_stderr,((
_tmp63A="{ }\n",_tag_dyneither(_tmp63A,sizeof(char),5))),_tag_dyneither(_tmp639,
sizeof(void*),0)));}_npop_handler(0);return;_LL1C7:;_LL1C8:(void)_throw(_tmp3CD);
_LL1C4:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp3D0=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3D5;enum Cyc_CfFlowInfo_InitLevel
_tmp3D7;struct Cyc_CfFlowInfo_Place*_tmp3D9;struct Cyc_CfFlowInfo_UnionRInfo
_tmp3DB;int _tmp3DC;int _tmp3DD;struct _dyneither_ptr _tmp3DE;struct Cyc_Absyn_Exp*
_tmp3E0;int _tmp3E1;void*_tmp3E2;_LL1CA: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3D1=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3D0;if(_tmp3D1->tag != 0)goto
_LL1CC;}_LL1CB:{const char*_tmp63D;void*_tmp63C;(_tmp63C=0,Cyc_fprintf(Cyc_stderr,((
_tmp63D="Zero",_tag_dyneither(_tmp63D,sizeof(char),5))),_tag_dyneither(_tmp63C,
sizeof(void*),0)));}goto _LL1C9;_LL1CC: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3D2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3D0;if(_tmp3D2->tag != 1)
goto _LL1CE;}_LL1CD:{const char*_tmp640;void*_tmp63F;(_tmp63F=0,Cyc_fprintf(Cyc_stderr,((
_tmp640="NotZeroAll",_tag_dyneither(_tmp640,sizeof(char),11))),_tag_dyneither(
_tmp63F,sizeof(void*),0)));}goto _LL1C9;_LL1CE: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3D3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3D0;if(_tmp3D3->tag != 2)
goto _LL1D0;}_LL1CF:{const char*_tmp643;void*_tmp642;(_tmp642=0,Cyc_fprintf(Cyc_stderr,((
_tmp643="NotZeroThis",_tag_dyneither(_tmp643,sizeof(char),12))),_tag_dyneither(
_tmp642,sizeof(void*),0)));}goto _LL1C9;_LL1D0: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3D4=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3D0;if(_tmp3D4->tag != 3)goto
_LL1D2;else{_tmp3D5=_tmp3D4->f1;}}_LL1D1:{const char*_tmp646;void*_tmp645;(
_tmp645=0,Cyc_fprintf(Cyc_stderr,((_tmp646="Unknown(",_tag_dyneither(_tmp646,
sizeof(char),9))),_tag_dyneither(_tmp645,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3D5);{const char*_tmp649;void*_tmp648;(_tmp648=0,Cyc_fprintf(Cyc_stderr,((
_tmp649=")",_tag_dyneither(_tmp649,sizeof(char),2))),_tag_dyneither(_tmp648,
sizeof(void*),0)));}goto _LL1C9;_LL1D2: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3D6=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3D0;if(_tmp3D6->tag != 4)goto _LL1D4;else{
_tmp3D7=_tmp3D6->f1;}}_LL1D3:{const char*_tmp64C;void*_tmp64B;(_tmp64B=0,Cyc_fprintf(
Cyc_stderr,((_tmp64C="Esc(",_tag_dyneither(_tmp64C,sizeof(char),5))),
_tag_dyneither(_tmp64B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3D7);{const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_fprintf(Cyc_stderr,((
_tmp64F=")",_tag_dyneither(_tmp64F,sizeof(char),2))),_tag_dyneither(_tmp64E,
sizeof(void*),0)));}goto _LL1C9;_LL1D4: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3D8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3D0;if(_tmp3D8->tag != 5)goto
_LL1D6;else{_tmp3D9=_tmp3D8->f1;}}_LL1D5:{const char*_tmp652;void*_tmp651;(
_tmp651=0,Cyc_fprintf(Cyc_stderr,((_tmp652="AddrOf(",_tag_dyneither(_tmp652,
sizeof(char),8))),_tag_dyneither(_tmp651,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3D9);{const char*_tmp655;void*_tmp654;(_tmp654=0,Cyc_fprintf(Cyc_stderr,((
_tmp655=")",_tag_dyneither(_tmp655,sizeof(char),2))),_tag_dyneither(_tmp654,
sizeof(void*),0)));}goto _LL1C9;_LL1D6: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3DA=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3D0;if(_tmp3DA->tag != 6)goto
_LL1D8;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DB.is_union;_tmp3DD=_tmp3DB.fieldnum;
_tmp3DE=_tmp3DA->f2;}}_LL1D7: if(_tmp3DC){{const char*_tmp658;void*_tmp657;(
_tmp657=0,Cyc_fprintf(Cyc_stderr,((_tmp658="AggrUnion{",_tag_dyneither(_tmp658,
sizeof(char),11))),_tag_dyneither(_tmp657,sizeof(void*),0)));}if(_tmp3DD != - 1){
const char*_tmp65C;void*_tmp65B[1];struct Cyc_Int_pa_struct _tmp65A;(_tmp65A.tag=1,((
_tmp65A.f1=(unsigned long)_tmp3DD,((_tmp65B[0]=& _tmp65A,Cyc_fprintf(Cyc_stderr,((
_tmp65C="tag == %d;",_tag_dyneither(_tmp65C,sizeof(char),11))),_tag_dyneither(
_tmp65B,sizeof(void*),1)))))));}}else{const char*_tmp65F;void*_tmp65E;(_tmp65E=0,
Cyc_fprintf(Cyc_stderr,((_tmp65F="AggrStruct{",_tag_dyneither(_tmp65F,sizeof(
char),12))),_tag_dyneither(_tmp65E,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp3DE,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3DE.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3DE,sizeof(void*))){
const char*_tmp662;void*_tmp661;(_tmp661=0,Cyc_fprintf(Cyc_stderr,((_tmp662=",",
_tag_dyneither(_tmp662,sizeof(char),2))),_tag_dyneither(_tmp661,sizeof(void*),0)));}}}{
const char*_tmp665;void*_tmp664;(_tmp664=0,Cyc_fprintf(Cyc_stderr,((_tmp665="}",
_tag_dyneither(_tmp665,sizeof(char),2))),_tag_dyneither(_tmp664,sizeof(void*),0)));}
goto _LL1C9;_LL1D8: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3DF=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3D0;if(_tmp3DF->tag != 7)goto _LL1C9;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;
_tmp3E2=(void*)_tmp3DF->f3;}}_LL1D9:{const char*_tmp66A;void*_tmp669[2];struct Cyc_String_pa_struct
_tmp668;struct Cyc_Int_pa_struct _tmp667;(_tmp667.tag=1,((_tmp667.f1=(
unsigned long)_tmp3E1,((_tmp668.tag=0,((_tmp668.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3E0)),((_tmp669[0]=& _tmp668,((
_tmp669[1]=& _tmp667,Cyc_fprintf(Cyc_stderr,((_tmp66A="[Consumed(%s,%d,",
_tag_dyneither(_tmp66A,sizeof(char),17))),_tag_dyneither(_tmp669,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3E2);{const char*_tmp66D;void*_tmp66C;(_tmp66C=0,
Cyc_fprintf(Cyc_stderr,((_tmp66D="]",_tag_dyneither(_tmp66D,sizeof(char),2))),
_tag_dyneither(_tmp66C,sizeof(void*),0)));};_LL1C9:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval){{const char*_tmp670;void*_tmp66F;(_tmp66F=0,Cyc_fprintf(Cyc_stderr,((
_tmp670="    ",_tag_dyneither(_tmp670,sizeof(char),5))),_tag_dyneither(_tmp66F,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp673;void*
_tmp672;(_tmp672=0,Cyc_fprintf(Cyc_stderr,((_tmp673=" --> ",_tag_dyneither(
_tmp673,sizeof(char),6))),_tag_dyneither(_tmp672,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp676;void*_tmp675;(_tmp675=0,Cyc_fprintf(Cyc_stderr,((
_tmp676="\n",_tag_dyneither(_tmp676,sizeof(char),2))),_tag_dyneither(_tmp675,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((void(*)(void(*f)(void*,
void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp40C=f;int _tmp40D;
struct _tuple11 _tmp40E;struct Cyc_Dict_Dict _tmp40F;_LL1DB: if((_tmp40C.BottomFL).tag
!= 1)goto _LL1DD;_tmp40D=(int)(_tmp40C.BottomFL).val;_LL1DC:{const char*_tmp679;
void*_tmp678;(_tmp678=0,Cyc_fprintf(Cyc_stderr,((_tmp679="  BottomFL\n",
_tag_dyneither(_tmp679,sizeof(char),12))),_tag_dyneither(_tmp678,sizeof(void*),0)));}
goto _LL1DA;_LL1DD: if((_tmp40C.ReachableFL).tag != 2)goto _LL1DA;_tmp40E=(struct
_tuple11)(_tmp40C.ReachableFL).val;_tmp40F=_tmp40E.f1;_LL1DE:{const char*_tmp67C;
void*_tmp67B;(_tmp67B=0,Cyc_fprintf(Cyc_stderr,((_tmp67C="  ReachableFL:\n",
_tag_dyneither(_tmp67C,sizeof(char),16))),_tag_dyneither(_tmp67B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp40F);{const char*_tmp67F;void*_tmp67E;(_tmp67E=
0,Cyc_fprintf(Cyc_stderr,((_tmp67F="\n",_tag_dyneither(_tmp67F,sizeof(char),2))),
_tag_dyneither(_tmp67E,sizeof(void*),0)));}goto _LL1DA;_LL1DA:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp416=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp422;struct Cyc_Absyn_DatatypeInfo
_tmp424;struct Cyc_List_List*_tmp425;struct Cyc_List_List*_tmp427;struct Cyc_Absyn_AggrInfo
_tmp429;struct Cyc_List_List*_tmp42A;struct Cyc_Absyn_DatatypeFieldInfo _tmp42C;
struct Cyc_List_List*_tmp42D;struct Cyc_List_List*_tmp42F;struct Cyc_Absyn_PtrInfo
_tmp431;void*_tmp432;struct Cyc_Absyn_PtrAtts _tmp433;void*_tmp434;struct Cyc_List_List*
_tmp437;struct Cyc_List_List*_tmp439;struct Cyc_Absyn_ArrayInfo _tmp43B;void*
_tmp43C;void*_tmp43E;void*_tmp440;void*_tmp442;void*_tmp443;void*_tmp445;_LL1E0: {
struct Cyc_Absyn_VoidType_struct*_tmp417=(struct Cyc_Absyn_VoidType_struct*)
_tmp416;if(_tmp417->tag != 0)goto _LL1E2;}_LL1E1: goto _LL1E3;_LL1E2: {struct Cyc_Absyn_IntType_struct*
_tmp418=(struct Cyc_Absyn_IntType_struct*)_tmp416;if(_tmp418->tag != 6)goto _LL1E4;}
_LL1E3: goto _LL1E5;_LL1E4: {struct Cyc_Absyn_FloatType_struct*_tmp419=(struct Cyc_Absyn_FloatType_struct*)
_tmp416;if(_tmp419->tag != 7)goto _LL1E6;}_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_DoubleType_struct*
_tmp41A=(struct Cyc_Absyn_DoubleType_struct*)_tmp416;if(_tmp41A->tag != 8)goto
_LL1E8;}_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_EnumType_struct*_tmp41B=(
struct Cyc_Absyn_EnumType_struct*)_tmp416;if(_tmp41B->tag != 14)goto _LL1EA;}_LL1E9:
goto _LL1EB;_LL1EA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp41C=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp416;if(_tmp41C->tag != 15)goto _LL1EC;}_LL1EB: goto _LL1ED;_LL1EC: {struct Cyc_Absyn_TagType_struct*
_tmp41D=(struct Cyc_Absyn_TagType_struct*)_tmp416;if(_tmp41D->tag != 20)goto _LL1EE;}
_LL1ED: goto _LL1EF;_LL1EE: {struct Cyc_Absyn_HeapRgn_struct*_tmp41E=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp416;if(_tmp41E->tag != 21)goto _LL1F0;}_LL1EF: goto _LL1F1;_LL1F0: {struct Cyc_Absyn_Evar_struct*
_tmp41F=(struct Cyc_Absyn_Evar_struct*)_tmp416;if(_tmp41F->tag != 1)goto _LL1F2;}
_LL1F1: return 0;_LL1F2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp420=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp416;if(_tmp420->tag != 22)goto _LL1F4;}_LL1F3: {void*_tmp447=rgn;_LL215: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp448=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp447;if(_tmp448->tag != 0)goto _LL217;}_LL216: return 1;_LL217:;_LL218: return 0;
_LL214:;}_LL1F4: {struct Cyc_Absyn_VarType_struct*_tmp421=(struct Cyc_Absyn_VarType_struct*)
_tmp416;if(_tmp421->tag != 2)goto _LL1F6;else{_tmp422=_tmp421->f1;}}_LL1F5: {void*
_tmp449=rgn;struct Cyc_Absyn_Tvar*_tmp44C;_LL21A: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp44A=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp449;if(_tmp44A->tag != 0)
goto _LL21C;}_LL21B: return 0;_LL21C: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp44B=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp449;if(_tmp44B->tag != 1)goto _LL219;
else{_tmp44C=_tmp44B->f1;}}_LL21D: return Cyc_Absyn_tvar_cmp(_tmp422,_tmp44C)== 0;
_LL219:;}_LL1F6: {struct Cyc_Absyn_DatatypeType_struct*_tmp423=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp416;if(_tmp423->tag != 3)goto _LL1F8;else{_tmp424=_tmp423->f1;_tmp425=_tmp424.targs;}}
_LL1F7: _tmp427=_tmp425;goto _LL1F9;_LL1F8: {struct Cyc_Absyn_TypedefType_struct*
_tmp426=(struct Cyc_Absyn_TypedefType_struct*)_tmp416;if(_tmp426->tag != 18)goto
_LL1FA;else{_tmp427=_tmp426->f2;}}_LL1F9: _tmp42A=_tmp427;goto _LL1FB;_LL1FA: {
struct Cyc_Absyn_AggrType_struct*_tmp428=(struct Cyc_Absyn_AggrType_struct*)
_tmp416;if(_tmp428->tag != 12)goto _LL1FC;else{_tmp429=_tmp428->f1;_tmp42A=_tmp429.targs;}}
_LL1FB: _tmp42D=_tmp42A;goto _LL1FD;_LL1FC: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp42B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp416;if(_tmp42B->tag != 4)
goto _LL1FE;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42C.targs;}}_LL1FD: _tmp42F=
_tmp42D;goto _LL1FF;_LL1FE: {struct Cyc_Absyn_JoinEff_struct*_tmp42E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp416;if(_tmp42E->tag != 24)goto _LL200;else{_tmp42F=_tmp42E->f1;}}_LL1FF: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp42F);_LL200: {struct Cyc_Absyn_PointerType_struct*
_tmp430=(struct Cyc_Absyn_PointerType_struct*)_tmp416;if(_tmp430->tag != 5)goto
_LL202;else{_tmp431=_tmp430->f1;_tmp432=_tmp431.elt_typ;_tmp433=_tmp431.ptr_atts;
_tmp434=_tmp433.rgn;}}_LL201: return Cyc_CfFlowInfo_contains_region(rgn,_tmp434)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp432);_LL202: {struct Cyc_Absyn_FnType_struct*
_tmp435=(struct Cyc_Absyn_FnType_struct*)_tmp416;if(_tmp435->tag != 10)goto _LL204;}
_LL203: return 0;_LL204: {struct Cyc_Absyn_TupleType_struct*_tmp436=(struct Cyc_Absyn_TupleType_struct*)
_tmp416;if(_tmp436->tag != 11)goto _LL206;else{_tmp437=_tmp436->f1;}}_LL205: for(0;
_tmp437 != 0;_tmp437=_tmp437->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp437->hd)).f2))return 1;}return 0;_LL206: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp438=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp416;if(_tmp438->tag != 13)goto
_LL208;else{_tmp439=_tmp438->f2;}}_LL207: for(0;_tmp439 != 0;_tmp439=_tmp439->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp439->hd)->type))
return 1;}return 0;_LL208: {struct Cyc_Absyn_ArrayType_struct*_tmp43A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp416;if(_tmp43A->tag != 9)goto _LL20A;else{_tmp43B=_tmp43A->f1;_tmp43C=_tmp43B.elt_type;}}
_LL209: _tmp43E=_tmp43C;goto _LL20B;_LL20A: {struct Cyc_Absyn_AccessEff_struct*
_tmp43D=(struct Cyc_Absyn_AccessEff_struct*)_tmp416;if(_tmp43D->tag != 23)goto
_LL20C;else{_tmp43E=(void*)_tmp43D->f1;}}_LL20B: _tmp440=_tmp43E;goto _LL20D;
_LL20C: {struct Cyc_Absyn_RgnsEff_struct*_tmp43F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp416;if(_tmp43F->tag != 25)goto _LL20E;else{_tmp440=(void*)_tmp43F->f1;}}_LL20D:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp440);_LL20E: {struct Cyc_Absyn_DynRgnType_struct*
_tmp441=(struct Cyc_Absyn_DynRgnType_struct*)_tmp416;if(_tmp441->tag != 17)goto
_LL210;else{_tmp442=(void*)_tmp441->f1;_tmp443=(void*)_tmp441->f2;}}_LL20F:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp442) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp443);_LL210: {struct Cyc_Absyn_RgnHandleType_struct*_tmp444=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp416;if(_tmp444->tag != 16)goto _LL212;else{_tmp445=(void*)_tmp444->f1;}}_LL211:
return 0;_LL212: {struct Cyc_Absyn_ValueofType_struct*_tmp446=(struct Cyc_Absyn_ValueofType_struct*)
_tmp416;if(_tmp446->tag != 19)goto _LL1DF;}_LL213: return 0;_LL1DF:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp44E;struct Cyc_CfFlowInfo_FlowEnv*
_tmp44F;struct Cyc_Dict_Dict _tmp450;struct Cyc_Dict_Dict*_tmp451;void*_tmp452;
struct _tuple23*_tmp44D=env;_tmp44E=*_tmp44D;_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;
_tmp451=(struct Cyc_Dict_Dict*)&(*_tmp44D).f2;_tmp452=_tmp44E.f3;{void*_tmp453=
root;struct Cyc_Absyn_Vardecl*_tmp455;void*_tmp457;_LL21F: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp454=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp453;if(_tmp454->tag != 0)goto
_LL221;else{_tmp455=_tmp454->f1;}}_LL220: if(Cyc_CfFlowInfo_contains_region(
_tmp452,_tmp455->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp44F,_tmp455->type,
0,_tmp44F->unknown_none);*_tmp451=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp451,root,rval);goto _LL21E;_LL221: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp456=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp453;if(_tmp456->tag != 1)goto _LL223;else{_tmp457=(void*)_tmp456->f2;}}_LL222:
if(!Cyc_CfFlowInfo_contains_region(_tmp452,_tmp457))*_tmp451=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp451,root,rval);goto
_LL21E;_LL223: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp458=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp453;if(_tmp458->tag != 2)goto _LL21E;}_LL224: goto _LL21E;_LL21E:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp459=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp45B;_LL226: {struct Cyc_Absyn_VarType_struct*
_tmp45A=(struct Cyc_Absyn_VarType_struct*)_tmp459;if(_tmp45A->tag != 2)goto _LL228;
else{_tmp45B=_tmp45A->f1;}}_LL227: {struct Cyc_CfFlowInfo_Region_k_struct _tmp680;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp680.tag=1,((_tmp680.f1=_tmp45B,
_tmp680)));void*v2=(void*)& v;struct _tuple23 _tmp681;struct _tuple23 env=(_tmp681.f1=
fenv,((_tmp681.f2=fenv->mt_flowdict,((_tmp681.f3=v2,_tmp681)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL228: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp45C=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp459;if(_tmp45C->tag != 22)goto
_LL22A;}_LL229: {struct _tuple23 _tmp682;struct _tuple23 env=(_tmp682.f1=fenv,((
_tmp682.f2=fenv->mt_flowdict,((_tmp682.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp682)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL22A:;_LL22B: {struct Cyc_Core_Impossible_struct _tmp688;const char*
_tmp687;struct Cyc_Core_Impossible_struct*_tmp686;(int)_throw((void*)((_tmp686=
_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((_tmp688.tag=Cyc_Core_Impossible,((
_tmp688.f1=((_tmp687="kill_flowdict_region",_tag_dyneither(_tmp687,sizeof(char),
21))),_tmp688)))),_tmp686)))));}_LL225:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp463=
f;int _tmp464;struct _tuple11 _tmp465;struct Cyc_Dict_Dict _tmp466;struct Cyc_List_List*
_tmp467;_LL22D: if((_tmp463.BottomFL).tag != 1)goto _LL22F;_tmp464=(int)(_tmp463.BottomFL).val;
_LL22E: return f;_LL22F: if((_tmp463.ReachableFL).tag != 2)goto _LL22C;_tmp465=(
struct _tuple11)(_tmp463.ReachableFL).val;_tmp466=_tmp465.f1;_tmp467=_tmp465.f2;
_LL230: {struct Cyc_Dict_Dict _tmp468=Cyc_CfFlowInfo_kill_flowdict_region(fenv,
_tmp466,rgn);struct Cyc_List_List*_tmp469=Cyc_CfFlowInfo_kill_relns_region(
_tmp467,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp469);}_LL22C:;}
