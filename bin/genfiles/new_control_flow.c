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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
void*Cyc_Core_snd(struct _tuple0*);int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int
sz);inline static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{unsigned int f1;int f2;};struct
_tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[
7];struct Cyc_CfFlowInfo_IsZero_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[
8];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
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
set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12
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
f);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,
void*t,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp74A;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp749;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp748;enclosee->annot=(void*)((_tmp748=_cycalloc(sizeof(*_tmp748)),((_tmp748[0]=((
_tmp74A.tag=Cyc_NewControlFlow_CFAnnot,((_tmp74A.f1=((_tmp749.encloser=encloser,((
_tmp749.visited=0,_tmp749)))),_tmp74A)))),_tmp748))));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_AbsLVal f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,
struct Cyc_Absyn_Exp*);static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple15{
union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct
_tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)
goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)&
_tmp4->f1;}}_LL2: return _tmp6;_LL3:;_LL4: {struct Cyc_Core_Impossible_struct
_tmp750;const char*_tmp74F;struct Cyc_Core_Impossible_struct*_tmp74E;(int)_throw((
void*)((_tmp74E=_cycalloc(sizeof(*_tmp74E)),((_tmp74E[0]=((_tmp750.tag=Cyc_Core_Impossible,((
_tmp750.f1=((_tmp74F="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp74F,sizeof(char),37))),_tmp750)))),_tmp74E)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp751;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp751=_region_malloc(env->r,sizeof(*_tmp751)),((_tmp751[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp751)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp752;return(_tmp752.f1=_tmpB,((_tmp752.f2=
_tmpC,_tmp752)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow);static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow);}struct _tuple17{struct Cyc_NewControlFlow_AnalEnv*f1;
struct Cyc_Position_Segment*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_unique_root(
struct _tuple17*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_unique_root(
struct _tuple17*ckenv,void*root,void*rval){struct _tuple17 _tmpF;struct Cyc_NewControlFlow_AnalEnv*
_tmp10;struct Cyc_Position_Segment*_tmp11;struct Cyc_Dict_Dict _tmp12;struct
_tuple17*_tmpE=ckenv;_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{
void*_tmp13=root;struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp14=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL8;
else{_tmp15=_tmp14->f1;}}_LL7: if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))
Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp10->fenv)->r,_tmp15->type)){{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp19;_LLB: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp16;if(_tmp17->tag != 7)goto _LLD;}_LLC: goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp18=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp16;if(_tmp18->tag != 3)goto _LLF;
else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE: goto
_LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp16;if(_tmp1A->tag != 0)goto _LL11;}_LL10: goto _LLA;_LL11:;_LL12:{const char*
_tmp756;void*_tmp755[1];struct Cyc_String_pa_struct _tmp754;(_tmp754.tag=0,((
_tmp754.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp755[0]=& _tmp754,Cyc_Tcutil_warn(_tmp11,((_tmp756="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp756,sizeof(char),60))),_tag_dyneither(_tmp755,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=
Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple15 _tmp757;struct _tuple15 _tmp22=(_tmp757.f1=flow,((_tmp757.f2=_tmp20,
_tmp757)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict
_tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict
_tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(
struct _tuple12)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((
_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;
_tmp28=_tmp27.f1;_LL15: {struct _tuple17 _tmp758;struct _tuple17 _tmp29=(_tmp758.f1=
env,((_tmp758.f2=s->loc,((_tmp758.f3=_tmp28,_tmp758)))));((void(*)(void(*f)(
struct _tuple17*,void*,void*),struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_NewControlFlow_check_unique_root,& _tmp29,_tmp25);goto _LL13;}_LL16:;_LL17:
goto _LL13;_LL13:;}if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){*
_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=1;}}
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp2B=inflow;int _tmp2C;struct _tuple12 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*
_tmp2F;_LL19: if((_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;
_LL1A: return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto
_LL18;_tmp2D=(struct _tuple12)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=
_tmp2D.f2;_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp75B;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp75A;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp75A=_region_malloc(fenv->r,sizeof(*_tmp75A)),((_tmp75A[0]=((_tmp75B.tag=
0,((_tmp75B.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp75B)))),_tmp75A)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);_LL18:;}
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp33=inflow;int _tmp34;struct _tuple12 _tmp35;struct
Cyc_Dict_Dict _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 
1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;_LL1F: return Cyc_CfFlowInfo_BottomFL();
_LL20: if((_tmp33.ReachableFL).tag != 2)goto _LL1D;_tmp35=(struct _tuple12)(_tmp33.ReachableFL).val;
_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_Tcutil_terr(
loc,((_tmp75E="expression may not be fully initialized",_tag_dyneither(_tmp75E,
sizeof(char),40))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp75F;struct
_tuple0 _tmp3D=(_tmp75F.f1=Cyc_Tcutil_compress(t),((_tmp75F.f2=r,_tmp75F)));void*
_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*_tmp43;void*_tmp45;
struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU _tmp48;
struct _tuple2 _tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;void*_tmp4B;struct
_dyneither_ptr _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp50;void*_tmp51;struct
_dyneither_ptr _tmp53;void*_tmp54;struct Cyc_Absyn_AggrInfo _tmp56;union Cyc_Absyn_AggrInfoU
_tmp57;struct Cyc_List_List*_tmp58;void*_tmp59;struct _dyneither_ptr _tmp5B;void*
_tmp5C;enum Cyc_Absyn_AggrKind _tmp5E;struct Cyc_List_List*_tmp5F;void*_tmp60;
struct _dyneither_ptr _tmp62;void*_tmp63;_LL23: _tmp3E=_tmp3D.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E;if(_tmp3F->tag != 3)goto
_LL25;else{_tmp40=_tmp3F->f1;if(_tmp40 != Cyc_CfFlowInfo_NoneIL)goto _LL25;}};
_LL24: return;_LL25: _tmp41=_tmp3D.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp42=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp41;if(_tmp42->tag != 0)goto _LL27;};_LL26:
return;_LL27: _tmp43=_tmp3D.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp44=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp43;if(_tmp44->tag != 7)goto _LL29;};
_LL28: return;_LL29: _tmp45=_tmp3D.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp46=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp45;if(_tmp46->tag != 4)goto
_LL2B;else{_tmp47=_tmp46->f1;_tmp48=_tmp47.field_info;if((_tmp48.KnownDatatypefield).tag
!= 2)goto _LL2B;_tmp49=(struct _tuple2)(_tmp48.KnownDatatypefield).val;_tmp4A=
_tmp49.f2;}};_tmp4B=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4C=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp4B;if(_tmp4C->tag != 6)goto _LL2B;else{
_tmp4D=_tmp4C->f2;}};_LL2A: if(_tmp4A->typs == 0)return;_tmp50=_tmp4A->typs;_tmp53=
_tmp4D;goto _LL2C;_LL2B: _tmp4E=_tmp3D.f1;{struct Cyc_Absyn_TupleType_struct*_tmp4F=(
struct Cyc_Absyn_TupleType_struct*)_tmp4E;if(_tmp4F->tag != 11)goto _LL2D;else{
_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp52=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp51;if(_tmp52->tag != 6)goto
_LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp50);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(
env,loc,(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((
void**)_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}return;}_LL2D:
_tmp54=_tmp3D.f1;{struct Cyc_Absyn_AggrType_struct*_tmp55=(struct Cyc_Absyn_AggrType_struct*)
_tmp54;if(_tmp55->tag != 12)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;
_tmp58=_tmp56.targs;}};_tmp59=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp59;if(_tmp5A->tag != 6)goto
_LL2F;else{_tmp5B=_tmp5A->f2;}};_LL2E: {struct Cyc_Absyn_Aggrdecl*_tmp65=Cyc_Absyn_get_known_aggrdecl(
_tmp57);if(_tmp65->impl == 0)return;{struct Cyc_List_List*_tmp66=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->fields;struct _RegionHandle _tmp67=_new_region("temp");
struct _RegionHandle*temp=& _tmp67;_push_region(temp);{struct Cyc_List_List*_tmp68=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp65->tvs,_tmp58);{int i=0;
for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){void*
t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type;
if(_tmp68 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp68,t);Cyc_NewControlFlow_check_nounique(
env,loc,t,((void**)_tmp5B.curr)[i]);}}_npop_handler(0);return;};_pop_region(temp);};}
_LL2F: _tmp5C=_tmp3D.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp5D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5C;if(_tmp5D->tag != 13)goto _LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};
_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp61=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp60;if(_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:{int i=0;for(
0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((
void**)_tmp62.curr)[i]);}}return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64=(struct Cyc_Absyn_PointerType_struct*)_tmp63;if(_tmp64->tag != 5)goto _LL33;};
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp762;void*
_tmp761;(_tmp761=0,Cyc_Tcutil_warn(loc,((_tmp762="argument may still contain unique pointers",
_tag_dyneither(_tmp762,sizeof(char),43))),_tag_dyneither(_tmp761,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp765;void*
_tmp764;(_tmp764=0,Cyc_Tcutil_terr(loc,((_tmp765="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp765,sizeof(char),47))),_tag_dyneither(_tmp764,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp768;void*_tmp767;(_tmp767=0,Cyc_Tcutil_warn(
loc,((_tmp768="argument may contain live unique pointers",_tag_dyneither(_tmp768,
sizeof(char),42))),_tag_dyneither(_tmp767,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple19 _tmp769;return(
_tmp769.f1=inflow,((_tmp769.f2=0,_tmp769)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp76C;struct _tuple19 _tmp76B;return(_tmp76B.f1=
_tmp7C,((_tmp76B.f2=((_tmp76C=_region_malloc(rgn,sizeof(*_tmp76C)),((_tmp76C->hd=
_tmp7D,((_tmp76C->tl=0,_tmp76C)))))),_tmp76B)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp76D;env->all_changed=((
_tmp76D=_region_malloc(env->r,sizeof(*_tmp76D)),((_tmp76D[0]=(env->fenv)->mt_place_set,
_tmp76D))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp76E;rvals=((_tmp76E=_region_malloc(rgn,sizeof(*_tmp76E)),((_tmp76E->hd=
_tmp83,((_tmp76E->tl=0,_tmp76E))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp76F;env->all_changed=((_tmp76F=_region_malloc(env->r,sizeof(*_tmp76F)),((
_tmp76F[0]=(env->fenv)->mt_place_set,_tmp76F))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp770;rvals=((_tmp770=_region_malloc(rgn,
sizeof(*_tmp770)),((_tmp770->hd=_tmp88,((_tmp770->tl=rvals,_tmp770))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp771;env->all_changed=((
_tmp771=_region_malloc(env->r,sizeof(*_tmp771)),((_tmp771[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp771))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp772;return(_tmp772.f1=outflow,((_tmp772.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp772)));};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);static struct
_tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct _RegionHandle*
_tmp8C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*_tmp8F;
struct _tuple19 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp8C,env,inflow,
es,0,0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp90=
_tmp8E;struct _tuple12 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((_tmp90.ReachableFL).tag
!= 2)goto _LL42;_tmp91=(struct _tuple12)(_tmp90.ReachableFL).val;_tmp92=_tmp91.f1;
_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp775="expression may not be initialized",
_tag_dyneither(_tmp775,sizeof(char),34))),_tag_dyneither(_tmp774,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple13 _tmp776;return(_tmp776.f1=_tmp8E,((
_tmp776.f2=(env->fenv)->unknown_all,_tmp776)));};}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel
il);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp97=outflow;int _tmp98;struct _tuple12 _tmp99;struct Cyc_Dict_Dict _tmp9A;struct
Cyc_List_List*_tmp9B;_LL45: if((_tmp97.BottomFL).tag != 1)goto _LL47;_tmp98=(int)(
_tmp97.BottomFL).val;_LL46: return outflow;_LL47: if((_tmp97.ReachableFL).tag != 2)
goto _LL44;_tmp99=(struct _tuple12)(_tmp97.ReachableFL).val;_tmp9A=_tmp99.f1;
_tmp9B=_tmp99.f2;_LL48: {union Cyc_CfFlowInfo_AbsLVal _tmp9C=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp9D;struct Cyc_CfFlowInfo_Place*_tmp9E;_LL4A: if((_tmp9C.UnknownL).tag
!= 2)goto _LL4C;_tmp9D=(int)(_tmp9C.UnknownL).val;_LL4B: return outflow;_LL4C: if((
_tmp9C.PlaceL).tag != 1)goto _LL49;_tmp9E=(struct Cyc_CfFlowInfo_Place*)(_tmp9C.PlaceL).val;
_LL4D: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmp9F=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp9A,env->all_changed,_tmp9E,nzval),_tmp9B);return _tmp9F;}
_LL49:;}_LL44:;}static struct _tuple15 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il);static struct _tuple15 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmpA0=outflow;int _tmpA1;struct _tuple12 _tmpA2;struct Cyc_Dict_Dict _tmpA3;struct
Cyc_List_List*_tmpA4;_LL4F: if((_tmpA0.BottomFL).tag != 1)goto _LL51;_tmpA1=(int)(
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp777;return(_tmp777.f1=outflow,((
_tmp777.f2=outflow,_tmp777)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp778;
return(_tmp778.f1=outflow,((_tmp778.f2=outflow,_tmp778)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp779;return(_tmp779.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp779.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp779)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r);static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmpAD=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAF;void*
_tmpB0;struct Cyc_Absyn_PtrAtts _tmpB1;union Cyc_Absyn_Constraint*_tmpB2;union Cyc_Absyn_Constraint*
_tmpB3;_LL59: {struct Cyc_Absyn_PointerType_struct*_tmpAE=(struct Cyc_Absyn_PointerType_struct*)
_tmpAD;if(_tmpAE->tag != 5)goto _LL5B;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAF.elt_typ;
_tmpB1=_tmpAF.ptr_atts;_tmpB2=_tmpB1.bounds;_tmpB3=_tmpB1.zero_term;}}_LL5A: {
union Cyc_CfFlowInfo_FlowInfo _tmpB4=f;int _tmpB5;struct _tuple12 _tmpB6;struct Cyc_Dict_Dict
_tmpB7;struct Cyc_List_List*_tmpB8;_LL5E: if((_tmpB4.BottomFL).tag != 1)goto _LL60;
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp77A;return(_tmp77A.f1=
f,((_tmp77A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp77A)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
_tuple12)(_tmpB4.ReachableFL).val;_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;_LL61: if(Cyc_Tcutil_is_bound_one(
_tmpB2)){void*_tmpBA=r;struct Cyc_CfFlowInfo_Place*_tmpBE;enum Cyc_CfFlowInfo_InitLevel
_tmpC1;_LL63: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpBB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpBA;if(_tmpBB->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpBC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpBA;if(_tmpBC->tag != 2)goto
_LL67;}_LL66:{void*_tmpC2=e->annot;struct Cyc_List_List*_tmpC4;_LL70: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpC3=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpC2;if(_tmpC3->tag != Cyc_CfFlowInfo_NotZero)
goto _LL72;else{_tmpC4=_tmpC3->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,
_tmpC4))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC5=e->r;_LL75: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC5;if(_tmpC6->tag != 24)goto _LL77;}
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp77D;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp77C;e->annot=(void*)((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C[0]=((
_tmp77D.tag=Cyc_CfFlowInfo_NotZero,((_tmp77D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp77D)))),_tmp77C))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp780;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp77F;e->annot=(void*)((_tmp77F=_cycalloc(sizeof(*_tmp77F)),((_tmp77F[0]=((
_tmp780.tag=Cyc_CfFlowInfo_NotZero,((_tmp780.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp780)))),_tmp77F))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp781;return(_tmp781.f1=f,((
_tmp781.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp781)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp782;return(_tmp782.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp782.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),_tmp782)));};
_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp785;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp784;e->annot=(void*)((_tmp784=
_cycalloc(sizeof(*_tmp784)),((_tmp784[0]=((_tmp785.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp785.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp785)))),
_tmp784))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp788;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp787;e->annot=(void*)((_tmp787=_cycalloc(sizeof(*_tmp787)),((_tmp787[0]=((
_tmp788.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp788.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp788)))),_tmp787))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp78B;void*_tmp78A;(
_tmp78A=0,Cyc_Tcutil_terr(e->loc,((_tmp78B="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp78B,sizeof(char),61))),_tag_dyneither(_tmp78A,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(
e->loc,((_tmp78E="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp78E,sizeof(char),46))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp78F;return(_tmp78F.f1=
f,((_tmp78F.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp78F)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp790;return(
_tmp790.f1=f,((_tmp790.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_none),
_tmp790)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp796;
const char*_tmp795;struct Cyc_Core_Impossible_struct*_tmp794;(int)_throw((void*)((
_tmp794=_cycalloc(sizeof(*_tmp794)),((_tmp794[0]=((_tmp796.tag=Cyc_Core_Impossible,((
_tmp796.f1=((_tmp795="right deref of non-pointer-type",_tag_dyneither(_tmp795,
sizeof(char),32))),_tmp796)))),_tmp794)))));}_LL58:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpE6=e1->r;void*
_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEA;void*_tmpEC;struct Cyc_Absyn_Vardecl*_tmpEE;
void*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpF2;void*_tmpF4;struct Cyc_Absyn_Vardecl*
_tmpF6;_LL97: {struct Cyc_Absyn_Var_e_struct*_tmpE7=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpE7->tag != 1)goto _LL99;else{_tmpE8=(void*)_tmpE7->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpE9=(struct Cyc_Absyn_Pat_b_struct*)_tmpE8;if(_tmpE9->tag != 5)goto _LL99;else{
_tmpEA=_tmpE9->f1;}};}}_LL98: _tmpEE=_tmpEA;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*
_tmpEB=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpEB->tag != 1)goto _LL9B;else{
_tmpEC=(void*)_tmpEB->f2;{struct Cyc_Absyn_Local_b_struct*_tmpED=(struct Cyc_Absyn_Local_b_struct*)
_tmpEC;if(_tmpED->tag != 4)goto _LL9B;else{_tmpEE=_tmpED->f1;}};}}_LL9A: _tmpF2=
_tmpEE;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*_tmpEF=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpEF->tag != 1)goto _LL9D;else{_tmpF0=(void*)_tmpEF->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpF1=(struct Cyc_Absyn_Param_b_struct*)_tmpF0;if(_tmpF1->tag != 3)goto _LL9D;
else{_tmpF2=_tmpF1->f1;}};}}_LL9C: _tmpF6=_tmpF2;goto _LL9E;_LL9D: {struct Cyc_Absyn_Var_e_struct*
_tmpF3=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpF3->tag != 1)goto _LL9F;else{
_tmpF4=(void*)_tmpF3->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF5=(struct Cyc_Absyn_Global_b_struct*)
_tmpF4;if(_tmpF5->tag != 1)goto _LL9F;else{_tmpF6=_tmpF5->f1;}};}}_LL9E: if(!_tmpF6->escapes){
void*_tmpF7=e2->r;void*_tmpF9;struct Cyc_Absyn_Vardecl*_tmpFB;void*_tmpFD;struct
Cyc_Absyn_Vardecl*_tmpFF;void*_tmp101;struct Cyc_Absyn_Vardecl*_tmp103;void*
_tmp105;struct Cyc_Absyn_Vardecl*_tmp107;_LLA2: {struct Cyc_Absyn_Var_e_struct*
_tmpF8=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmpF8->tag != 1)goto _LLA4;else{
_tmpF9=(void*)_tmpF8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpFA=(struct Cyc_Absyn_Pat_b_struct*)
_tmpF9;if(_tmpFA->tag != 5)goto _LLA4;else{_tmpFB=_tmpFA->f1;}};}}_LLA3: _tmpFF=
_tmpFB;goto _LLA5;_LLA4: {struct Cyc_Absyn_Var_e_struct*_tmpFC=(struct Cyc_Absyn_Var_e_struct*)
_tmpF7;if(_tmpFC->tag != 1)goto _LLA6;else{_tmpFD=(void*)_tmpFC->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpFE=(struct Cyc_Absyn_Local_b_struct*)_tmpFD;if(_tmpFE->tag != 4)goto _LLA6;
else{_tmpFF=_tmpFE->f1;}};}}_LLA5: _tmp103=_tmpFF;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_struct*
_tmp100=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp100->tag != 1)goto _LLA8;
else{_tmp101=(void*)_tmp100->f2;{struct Cyc_Absyn_Param_b_struct*_tmp102=(struct
Cyc_Absyn_Param_b_struct*)_tmp101;if(_tmp102->tag != 3)goto _LLA8;else{_tmp103=
_tmp102->f1;}};}}_LLA7: _tmp107=_tmp103;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_struct*
_tmp104=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp104->tag != 1)goto _LLAA;
else{_tmp105=(void*)_tmp104->f2;{struct Cyc_Absyn_Global_b_struct*_tmp106=(struct
Cyc_Absyn_Global_b_struct*)_tmp105;if(_tmp106->tag != 1)goto _LLAA;else{_tmp107=
_tmp106->f1;}};}}_LLA9: if(!_tmp107->escapes){int found=0;{struct Cyc_List_List*
_tmp108=relns;for(0;_tmp108 != 0;_tmp108=_tmp108->tl){struct Cyc_CfFlowInfo_Reln*
_tmp109=(struct Cyc_CfFlowInfo_Reln*)_tmp108->hd;if(_tmp109->vd == _tmp107){union
Cyc_CfFlowInfo_RelnOp _tmp10A=_tmp109->rop;struct Cyc_Absyn_Vardecl*_tmp10B;_LLAD:
if((_tmp10A.LessNumelts).tag != 3)goto _LLAF;_tmp10B=(struct Cyc_Absyn_Vardecl*)(
_tmp10A.LessNumelts).val;if(!(_tmp10B == _tmpF6))goto _LLAF;_LLAE: return relns;
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp799;
struct Cyc_List_List*_tmp798;return(_tmp798=_region_malloc(rgn,sizeof(*_tmp798)),((
_tmp798->hd=((_tmp799=_region_malloc(rgn,sizeof(*_tmp799)),((_tmp799->vd=_tmp107,((
_tmp799->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp799)))))),((_tmp798->tl=
relns,_tmp798)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp79A;return(_tmp79A.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp79A.f2=rval,_tmp79A)));}_LLB4: if((_tmp10E.ReachableFL).tag
!= 2)goto _LLB1;_tmp110=(struct _tuple12)(_tmp10E.ReachableFL).val;_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp114=lval;struct Cyc_CfFlowInfo_Place*
_tmp115;int _tmp116;_LLB7: if((_tmp114.PlaceL).tag != 1)goto _LLB9;_tmp115=(struct
Cyc_CfFlowInfo_Place*)(_tmp114.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp117=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp111,env->all_changed,_tmp115,rval);
_tmp112=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp112,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp117,_tmp112);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)((struct Cyc_Core_Opt*)_check_null(lexp->topt))->v)){void*
_tmp118=Cyc_CfFlowInfo_lookup_place(_tmp111,_tmp115);enum Cyc_CfFlowInfo_InitLevel
_tmp11A;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp119=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp118;if(_tmp119->tag != 3)goto _LLBE;else{_tmp11A=_tmp119->f1;if(_tmp11A != Cyc_CfFlowInfo_NoneIL)
goto _LLBE;}}_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp11B=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp118;if(_tmp11B->tag != 7)goto _LLC0;}
_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_struct*_tmp11C=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp118;if(_tmp11C->tag != 0)goto _LLC2;}_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*
_tmp79D;void*_tmp79C;(_tmp79C=0,Cyc_Tcutil_warn(lexp->loc,((_tmp79D="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp79D,sizeof(char),43))),_tag_dyneither(_tmp79C,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp79E;return(_tmp79E.f1=outflow,((_tmp79E.f2=rval,_tmp79E)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp79F;return(_tmp79F.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp79F.f2=rval,_tmp79F)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e);static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp121=e->r;void*_tmp123;void*_tmp126;void*_tmp129;
struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*
_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;_LLC5: {struct
Cyc_Absyn_Var_e_struct*_tmp122=(struct Cyc_Absyn_Var_e_struct*)_tmp121;if(_tmp122->tag
!= 1)goto _LLC7;else{_tmp123=(void*)_tmp122->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp124=(struct Cyc_Absyn_Param_b_struct*)_tmp123;if(_tmp124->tag != 3)goto _LLC7;};}}
_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp125=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp125->tag != 1)goto _LLC9;else{_tmp126=(void*)_tmp125->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp127=(struct Cyc_Absyn_Local_b_struct*)_tmp126;if(_tmp127->tag != 4)goto _LLC9;};}}
_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp128=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp128->tag != 1)goto _LLCB;else{_tmp129=(void*)_tmp128->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp12A=(struct Cyc_Absyn_Pat_b_struct*)_tmp129;if(_tmp12A->tag != 5)goto _LLCB;};}}
_LLCA: return 1;_LLCB: {struct Cyc_Absyn_Deref_e_struct*_tmp12B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp121;if(_tmp12B->tag != 21)goto _LLCD;else{_tmp12C=_tmp12B->f1;}}_LLCC: _tmp12E=
_tmp12C;goto _LLCE;_LLCD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp12D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp121;if(_tmp12D->tag != 22)goto _LLCF;else{_tmp12E=_tmp12D->f1;}}_LLCE: _tmp130=
_tmp12E;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp12F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp121;if(_tmp12F->tag != 23)goto _LLD1;else{_tmp130=_tmp12F->f1;}}_LLD0: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp130);_LLD1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp131=(struct Cyc_Absyn_Subscript_e_struct*)_tmp121;if(_tmp131->tag != 24)goto
_LLD3;else{_tmp132=_tmp131->f1;}}_LLD2: {void*_tmp135=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp132->topt))->v);_LLD8: {struct Cyc_Absyn_TupleType_struct*
_tmp136=(struct Cyc_Absyn_TupleType_struct*)_tmp135;if(_tmp136->tag != 11)goto
_LLDA;}_LLD9: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132);_LLDA:;
_LLDB: return 0;_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_struct*_tmp133=(struct Cyc_Absyn_Cast_e_struct*)
_tmp121;if(_tmp133->tag != 15)goto _LLD5;else{_tmp134=_tmp133->f2;}}_LLD4: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp134);_LLD5:;_LLD6: return 0;_LLC4:;}
static struct _tuple13 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s);
static struct _tuple13 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_CfFlowInfo_FlowEnv*_tmp137=env->fenv;while(1){union Cyc_CfFlowInfo_FlowInfo*
_tmp139;struct _tuple16 _tmp138=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp139=_tmp138.f2;inflow=*_tmp139;{void*_tmp13A=s->r;struct Cyc_Absyn_Stmt*
_tmp13C;struct Cyc_Absyn_Stmt*_tmp13D;struct Cyc_Absyn_Decl*_tmp13F;struct Cyc_Absyn_Decl
_tmp140;void*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Tvar*_tmp144;
struct Cyc_Absyn_Vardecl*_tmp145;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Absyn_Decl*
_tmp148;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;_LLDD: {struct
Cyc_Absyn_Seq_s_struct*_tmp13B=(struct Cyc_Absyn_Seq_s_struct*)_tmp13A;if(_tmp13B->tag
!= 2)goto _LLDF;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LLDE: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp13C);s=_tmp13D;goto _LLDC;_LLDF: {struct Cyc_Absyn_Decl_s_struct*
_tmp13E=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp13E->tag != 12)goto _LLE1;
else{_tmp13F=_tmp13E->f1;_tmp140=*_tmp13F;_tmp141=_tmp140.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp142=(struct Cyc_Absyn_Alias_d_struct*)_tmp141;if(_tmp142->tag != 5)goto _LLE1;
else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;_tmp145=_tmp142->f3;}};_tmp146=
_tmp13E->f2;}}_LLE0: {union Cyc_CfFlowInfo_FlowInfo _tmp14D;union Cyc_CfFlowInfo_AbsLVal
_tmp14E;struct _tuple14 _tmp14C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp143);
_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct _tuple14 _tmp7A0;struct _tuple14
_tmp150=(_tmp7A0.f1=_tmp14D,((_tmp7A0.f2=_tmp14E,_tmp7A0)));union Cyc_CfFlowInfo_FlowInfo
_tmp151;struct _tuple12 _tmp152;struct Cyc_Dict_Dict _tmp153;struct Cyc_List_List*
_tmp154;union Cyc_CfFlowInfo_AbsLVal _tmp155;struct Cyc_CfFlowInfo_Place*_tmp156;
union Cyc_CfFlowInfo_FlowInfo _tmp157;int _tmp158;_LLE8: _tmp151=_tmp150.f1;if((
_tmp151.ReachableFL).tag != 2)goto _LLEA;_tmp152=(struct _tuple12)(_tmp151.ReachableFL).val;
_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;_tmp155=_tmp150.f2;if((_tmp155.PlaceL).tag
!= 1)goto _LLEA;_tmp156=(struct Cyc_CfFlowInfo_Place*)(_tmp155.PlaceL).val;_LLE9: {
void*_tmp159=Cyc_CfFlowInfo_lookup_place(_tmp153,_tmp156);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp143->topt))->v;{void*_tmp15A=_tmp143->r;struct Cyc_Absyn_Exp*
_tmp15C;_LLEF: {struct Cyc_Absyn_Cast_e_struct*_tmp15B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp15A;if(_tmp15B->tag != 15)goto _LLF1;else{_tmp15C=_tmp15B->f2;}}_LLF0: t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15C->topt))->v;goto _LLEE;_LLF1:;_LLF2: goto
_LLEE;_LLEE:;}{void*_tmp15D=Cyc_CfFlowInfo_make_unique_consumed(_tmp137,t,
_tmp143,env->iteration_num,_tmp159);_tmp153=Cyc_CfFlowInfo_assign_place(_tmp137,
_tmp143->loc,_tmp153,env->all_changed,_tmp156,_tmp15D);_tmp14D=Cyc_CfFlowInfo_ReachableFL(
_tmp153,_tmp154);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp137,_tmp153,
_tmp159)){case Cyc_CfFlowInfo_AllIL: _LLF3: leaf=_tmp137->unknown_all;break;case Cyc_CfFlowInfo_NoneIL:
_LLF4: leaf=_tmp137->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLF5: leaf=
_tmp137->unknown_this;break;}{void*_tmp15E=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,
t,leaf);_tmp14D=Cyc_NewControlFlow_use_Rval(env,_tmp143->loc,_tmp14D,_tmp159);{
struct Cyc_List_List _tmp7A1;struct Cyc_List_List _tmp15F=(_tmp7A1.hd=_tmp145,((
_tmp7A1.tl=0,_tmp7A1)));_tmp14D=Cyc_NewControlFlow_add_vars(_tmp137,_tmp14D,(
struct Cyc_List_List*)& _tmp15F,_tmp137->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp161;void*_tmp162;struct _tuple13 _tmp160=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14D,_tmp146);_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp163=_tmp161;struct _tuple12 _tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*
_tmp166;_LLF8: if((_tmp163.ReachableFL).tag != 2)goto _LLFA;_tmp164=(struct _tuple12)(
_tmp163.ReachableFL).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LLF9: _tmp165=Cyc_CfFlowInfo_assign_place(
_tmp137,s->loc,_tmp165,env->all_changed,_tmp156,_tmp15E);{union Cyc_CfFlowInfo_FlowInfo
_tmp167=Cyc_CfFlowInfo_ReachableFL(_tmp165,_tmp166);struct _tuple13 _tmp7A2;return(
_tmp7A2.f1=_tmp167,((_tmp7A2.f2=_tmp162,_tmp7A2)));};_LLFA:;_LLFB: {struct
_tuple13 _tmp7A3;return(_tmp7A3.f1=_tmp161,((_tmp7A3.f2=_tmp162,_tmp7A3)));}_LLF7:;};};};};};};}
_LLEA: _tmp157=_tmp150.f1;if((_tmp157.BottomFL).tag != 1)goto _LLEC;_tmp158=(int)(
_tmp157.BottomFL).val;_LLEB: {struct _tuple13 _tmp7A4;return(_tmp7A4.f1=_tmp14D,((
_tmp7A4.f2=_tmp137->unknown_all,_tmp7A4)));}_LLEC:;_LLED:{const char*_tmp7A7;void*
_tmp7A6;(_tmp7A6=0,Cyc_Tcutil_terr(_tmp143->loc,((_tmp7A7="bad alias expression--no unique path found",
_tag_dyneither(_tmp7A7,sizeof(char),43))),_tag_dyneither(_tmp7A6,sizeof(void*),0)));}{
struct _tuple13 _tmp7A8;return(_tmp7A8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A8.f2=
_tmp137->unknown_all,_tmp7A8)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp147=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp147->tag != 12)goto _LLE3;
else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp148);s=_tmp149;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14A=(struct Cyc_Absyn_Exp_s_struct*)_tmp13A;if(_tmp14A->tag != 1)goto _LLE5;
else{_tmp14B=_tmp14A->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14B);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7AE;
const char*_tmp7AD;struct Cyc_Core_Impossible_struct*_tmp7AC;(int)_throw((void*)((
_tmp7AC=_cycalloc(sizeof(*_tmp7AC)),((_tmp7AC[0]=((_tmp7AE.tag=Cyc_Core_Impossible,((
_tmp7AE.f1=((_tmp7AD="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7AD,
sizeof(char),33))),_tmp7AE)))),_tmp7AC)))));}_LLDC:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp83D(unsigned int*_tmp83C,unsigned int*_tmp83B,void***_tmp839,
struct Cyc_List_List**_tmp34C){for(*_tmp83C=0;*_tmp83C < *_tmp83B;(*_tmp83C)++){
void*_tmp837;(*_tmp839)[*_tmp83C]=((_tmp837=(void*)((struct Cyc_List_List*)
_check_null(*_tmp34C))->hd,((*_tmp34C=(*_tmp34C)->tl,_tmp837))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp172=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp173=inflow;int _tmp174;struct _tuple12 _tmp175;struct Cyc_Dict_Dict _tmp176;
struct Cyc_List_List*_tmp177;_LLFD: if((_tmp173.BottomFL).tag != 1)goto _LLFF;
_tmp174=(int)(_tmp173.BottomFL).val;_LLFE: {struct _tuple13 _tmp7AF;return(_tmp7AF.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7AF.f2=_tmp172->unknown_all,_tmp7AF)));}_LLFF:
if((_tmp173.ReachableFL).tag != 2)goto _LLFC;_tmp175=(struct _tuple12)(_tmp173.ReachableFL).val;
_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL100: d=_tmp176;relns=_tmp177;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17A;union Cyc_CfFlowInfo_AbsLVal _tmp17B;struct _tuple14 _tmp179=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tuple14 _tmp7B0;
struct _tuple14 _tmp17D=(_tmp7B0.f1=_tmp17A,((_tmp7B0.f2=_tmp17B,_tmp7B0)));union
Cyc_CfFlowInfo_FlowInfo _tmp17E;struct _tuple12 _tmp17F;struct Cyc_Dict_Dict _tmp180;
struct Cyc_List_List*_tmp181;union Cyc_CfFlowInfo_AbsLVal _tmp182;struct Cyc_CfFlowInfo_Place*
_tmp183;_LL102: _tmp17E=_tmp17D.f1;if((_tmp17E.ReachableFL).tag != 2)goto _LL104;
_tmp17F=(struct _tuple12)(_tmp17E.ReachableFL).val;_tmp180=_tmp17F.f1;_tmp181=
_tmp17F.f2;_tmp182=_tmp17D.f2;if((_tmp182.PlaceL).tag != 1)goto _LL104;_tmp183=(
struct Cyc_CfFlowInfo_Place*)(_tmp182.PlaceL).val;_LL103: {void*_tmp184=Cyc_CfFlowInfo_lookup_place(
_tmp180,_tmp183);{void*_tmp185=_tmp184;struct Cyc_Absyn_Exp*_tmp187;int _tmp188;
void*_tmp189;_LL107: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp186=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp185;if(_tmp186->tag != 7)goto _LL109;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;
_tmp189=(void*)_tmp186->f3;}}_LL108: if(_tmp187 == e  && _tmp188 == env->iteration_num){
struct _tuple13 _tmp7B1;return(_tmp7B1.f1=_tmp17A,((_tmp7B1.f2=_tmp189,_tmp7B1)));}{
const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_Tcutil_terr(e->loc,((_tmp7B4="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7B4,sizeof(char),71))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}{
struct _tuple13 _tmp7B5;return(_tmp7B5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B5.f2=
_tmp172->unknown_all,_tmp7B5)));};_LL109:;_LL10A: goto _LL106;_LL106:;}{void*
_tmp18E=Cyc_CfFlowInfo_make_unique_consumed(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,e,env->iteration_num,_tmp184);struct Cyc_Dict_Dict
_tmp18F=Cyc_CfFlowInfo_assign_place(_tmp172,e->loc,_tmp180,env->all_changed,
_tmp183,_tmp18E);struct _tuple13 _tmp7B6;return(_tmp7B6.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp18F,_tmp181),((_tmp7B6.f2=_tmp184,_tmp7B6)));};}_LL104:;_LL105: goto _LL101;
_LL101:;};}{void*_tmp191=e->r;struct Cyc_Absyn_Exp*_tmp193;enum Cyc_Absyn_Coercion
_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*
_tmp19A;union Cyc_Absyn_Cnst _tmp19C;int _tmp19D;union Cyc_Absyn_Cnst _tmp19F;struct
_tuple6 _tmp1A0;int _tmp1A1;union Cyc_Absyn_Cnst _tmp1A3;struct _tuple6 _tmp1A4;void*
_tmp1A6;struct Cyc_List_List*_tmp1A9;void*_tmp1B1;void*_tmp1B4;struct Cyc_Absyn_Vardecl*
_tmp1B6;void*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1BA;void*_tmp1BC;struct Cyc_Absyn_Vardecl*
_tmp1BE;enum Cyc_Absyn_Primop _tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Exp*
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;struct Cyc_Core_Opt
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Core_Opt*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*
_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_List_List*
_tmp1D4;struct Cyc_Absyn_MallocInfo _tmp1D6;int _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;
void**_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DA;int _tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;
struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*
_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*
_tmp1E7;struct _dyneither_ptr*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1EA;struct
_dyneither_ptr*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1ED;struct _dyneither_ptr*_tmp1EE;
struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_List_List*_tmp1FD;struct Cyc_Absyn_Datatypedecl*_tmp1FE;struct
Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp202;struct Cyc_List_List*_tmp204;
struct Cyc_Absyn_Aggrdecl*_tmp205;struct Cyc_Absyn_Aggrdecl _tmp206;enum Cyc_Absyn_AggrKind
_tmp207;struct Cyc_Absyn_AggrdeclImpl*_tmp208;struct Cyc_Absyn_AggrdeclImpl _tmp209;
struct Cyc_List_List*_tmp20A;struct Cyc_List_List*_tmp20D;struct Cyc_Absyn_Vardecl*
_tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_Absyn_Exp*_tmp211;int _tmp212;
struct Cyc_Absyn_Stmt*_tmp214;void*_tmp216;_LL10C: {struct Cyc_Absyn_Cast_e_struct*
_tmp192=(struct Cyc_Absyn_Cast_e_struct*)_tmp191;if(_tmp192->tag != 15)goto _LL10E;
else{_tmp193=_tmp192->f2;_tmp194=_tmp192->f4;if(_tmp194 != Cyc_Absyn_NonNull_to_Null)
goto _LL10E;}}_LL10D: {union Cyc_CfFlowInfo_FlowInfo _tmp21E;void*_tmp21F;struct
_tuple13 _tmp21D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp193);
_tmp21E=_tmp21D.f1;_tmp21F=_tmp21D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp221;void*
_tmp222;struct _tuple13 _tmp220=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp21E,
_tmp193,_tmp21F);_tmp221=_tmp220.f1;_tmp222=_tmp220.f2;{struct _tuple13 _tmp7B7;
return(_tmp7B7.f1=_tmp221,((_tmp7B7.f2=_tmp21F,_tmp7B7)));};};}_LL10E: {struct
Cyc_Absyn_Cast_e_struct*_tmp195=(struct Cyc_Absyn_Cast_e_struct*)_tmp191;if(
_tmp195->tag != 15)goto _LL110;else{_tmp196=_tmp195->f2;}}_LL10F: _tmp198=_tmp196;
goto _LL111;_LL110: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp197=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp191;if(_tmp197->tag != 13)goto _LL112;else{_tmp198=_tmp197->f1;}}_LL111:
_tmp19A=_tmp198;goto _LL113;_LL112: {struct Cyc_Absyn_Instantiate_e_struct*_tmp199=(
struct Cyc_Absyn_Instantiate_e_struct*)_tmp191;if(_tmp199->tag != 14)goto _LL114;
else{_tmp19A=_tmp199->f1;}}_LL113: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp19A);_LL114: {struct Cyc_Absyn_Const_e_struct*_tmp19B=(
struct Cyc_Absyn_Const_e_struct*)_tmp191;if(_tmp19B->tag != 0)goto _LL116;else{
_tmp19C=_tmp19B->f1;if((_tmp19C.Null_c).tag != 1)goto _LL116;_tmp19D=(int)(_tmp19C.Null_c).val;}}
_LL115: goto _LL117;_LL116: {struct Cyc_Absyn_Const_e_struct*_tmp19E=(struct Cyc_Absyn_Const_e_struct*)
_tmp191;if(_tmp19E->tag != 0)goto _LL118;else{_tmp19F=_tmp19E->f1;if((_tmp19F.Int_c).tag
!= 4)goto _LL118;_tmp1A0=(struct _tuple6)(_tmp19F.Int_c).val;_tmp1A1=_tmp1A0.f2;
if(_tmp1A1 != 0)goto _LL118;}}_LL117: {struct _tuple13 _tmp7B8;return(_tmp7B8.f1=
inflow,((_tmp7B8.f2=_tmp172->zero,_tmp7B8)));}_LL118: {struct Cyc_Absyn_Const_e_struct*
_tmp1A2=(struct Cyc_Absyn_Const_e_struct*)_tmp191;if(_tmp1A2->tag != 0)goto _LL11A;
else{_tmp1A3=_tmp1A2->f1;if((_tmp1A3.Int_c).tag != 4)goto _LL11A;_tmp1A4=(struct
_tuple6)(_tmp1A3.Int_c).val;}}_LL119: goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*
_tmp1A5=(struct Cyc_Absyn_Var_e_struct*)_tmp191;if(_tmp1A5->tag != 1)goto _LL11C;
else{_tmp1A6=(void*)_tmp1A5->f2;{struct Cyc_Absyn_Funname_b_struct*_tmp1A7=(
struct Cyc_Absyn_Funname_b_struct*)_tmp1A6;if(_tmp1A7->tag != 2)goto _LL11C;};}}
_LL11B: {struct _tuple13 _tmp7B9;return(_tmp7B9.f1=inflow,((_tmp7B9.f2=_tmp172->notzeroall,
_tmp7B9)));}_LL11C: {struct Cyc_Absyn_Datatype_e_struct*_tmp1A8=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp191;if(_tmp1A8->tag != 31)goto _LL11E;else{_tmp1A9=_tmp1A8->f1;if(_tmp1A9 != 0)
goto _LL11E;}}_LL11D: goto _LL11F;_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp1AA=(
struct Cyc_Absyn_Const_e_struct*)_tmp191;if(_tmp1AA->tag != 0)goto _LL120;}_LL11F:
goto _LL121;_LL120: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1AB=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp191;if(_tmp1AB->tag != 19)goto _LL122;}_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp1AC=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp191;if(_tmp1AC->tag != 18)goto
_LL124;}_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1AD=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp191;if(_tmp1AD->tag != 20)goto _LL126;}
_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1AE=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp191;if(_tmp1AE->tag != 33)goto _LL128;}_LL127: goto _LL129;_LL128: {struct Cyc_Absyn_Enum_e_struct*
_tmp1AF=(struct Cyc_Absyn_Enum_e_struct*)_tmp191;if(_tmp1AF->tag != 32)goto _LL12A;}
_LL129: {struct _tuple13 _tmp7BA;return(_tmp7BA.f1=inflow,((_tmp7BA.f2=_tmp172->unknown_all,
_tmp7BA)));}_LL12A: {struct Cyc_Absyn_Var_e_struct*_tmp1B0=(struct Cyc_Absyn_Var_e_struct*)
_tmp191;if(_tmp1B0->tag != 1)goto _LL12C;else{_tmp1B1=(void*)_tmp1B0->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1B2=(struct Cyc_Absyn_Global_b_struct*)_tmp1B1;if(
_tmp1B2->tag != 1)goto _LL12C;};}}_LL12B: {struct _tuple13 _tmp7BB;return(_tmp7BB.f1=
inflow,((_tmp7BB.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp7BB)));}_LL12C: {struct Cyc_Absyn_Var_e_struct*
_tmp1B3=(struct Cyc_Absyn_Var_e_struct*)_tmp191;if(_tmp1B3->tag != 1)goto _LL12E;
else{_tmp1B4=(void*)_tmp1B3->f2;{struct Cyc_Absyn_Param_b_struct*_tmp1B5=(struct
Cyc_Absyn_Param_b_struct*)_tmp1B4;if(_tmp1B5->tag != 3)goto _LL12E;else{_tmp1B6=
_tmp1B5->f1;}};}}_LL12D: _tmp1BA=_tmp1B6;goto _LL12F;_LL12E: {struct Cyc_Absyn_Var_e_struct*
_tmp1B7=(struct Cyc_Absyn_Var_e_struct*)_tmp191;if(_tmp1B7->tag != 1)goto _LL130;
else{_tmp1B8=(void*)_tmp1B7->f2;{struct Cyc_Absyn_Local_b_struct*_tmp1B9=(struct
Cyc_Absyn_Local_b_struct*)_tmp1B8;if(_tmp1B9->tag != 4)goto _LL130;else{_tmp1BA=
_tmp1B9->f1;}};}}_LL12F: _tmp1BE=_tmp1BA;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp1BB=(struct Cyc_Absyn_Var_e_struct*)_tmp191;if(_tmp1BB->tag != 1)goto _LL132;
else{_tmp1BC=(void*)_tmp1BB->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp1BD=(struct Cyc_Absyn_Pat_b_struct*)
_tmp1BC;if(_tmp1BD->tag != 5)goto _LL132;else{_tmp1BE=_tmp1BD->f1;}};}}_LL131: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7C1;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7C0;struct _tuple13 _tmp7BF;return(_tmp7BF.f1=inflow,((_tmp7BF.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7C1=_region_malloc(
env->r,sizeof(*_tmp7C1)),((_tmp7C1[0]=((_tmp7C0.tag=0,((_tmp7C0.f1=_tmp1BE,
_tmp7C0)))),_tmp7C1))))),_tmp7BF)));}_LL132: {struct Cyc_Absyn_Primop_e_struct*
_tmp1BF=(struct Cyc_Absyn_Primop_e_struct*)_tmp191;if(_tmp1BF->tag != 3)goto _LL134;
else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;}}_LL133: {union Cyc_CfFlowInfo_FlowInfo
_tmp22C;void*_tmp22D;struct _tuple13 _tmp22B=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1C1);_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;{struct _tuple13 _tmp7C2;
return(_tmp7C2.f1=_tmp22C,((_tmp7C2.f2=_tmp22D,_tmp7C2)));};}_LL134: {struct Cyc_Absyn_Increment_e_struct*
_tmp1C2=(struct Cyc_Absyn_Increment_e_struct*)_tmp191;if(_tmp1C2->tag != 5)goto
_LL136;else{_tmp1C3=_tmp1C2->f1;}}_LL135: {struct Cyc_List_List _tmp7C3;struct Cyc_List_List
_tmp22F=(_tmp7C3.hd=_tmp1C3,((_tmp7C3.tl=0,_tmp7C3)));union Cyc_CfFlowInfo_FlowInfo
_tmp231;struct _tuple13 _tmp230=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp22F);_tmp231=_tmp230.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp233;struct _tuple14 _tmp232=Cyc_NewControlFlow_anal_Lexp(env,_tmp231,0,_tmp1C3);
_tmp233=_tmp232.f2;{struct _tuple20 _tmp7C4;struct _tuple20 _tmp235=(_tmp7C4.f1=
_tmp233,((_tmp7C4.f2=_tmp231,_tmp7C4)));union Cyc_CfFlowInfo_AbsLVal _tmp236;
struct Cyc_CfFlowInfo_Place*_tmp237;union Cyc_CfFlowInfo_FlowInfo _tmp238;struct
_tuple12 _tmp239;struct Cyc_Dict_Dict _tmp23A;struct Cyc_List_List*_tmp23B;_LL175:
_tmp236=_tmp235.f1;if((_tmp236.PlaceL).tag != 1)goto _LL177;_tmp237=(struct Cyc_CfFlowInfo_Place*)(
_tmp236.PlaceL).val;_tmp238=_tmp235.f2;if((_tmp238.ReachableFL).tag != 2)goto
_LL177;_tmp239=(struct _tuple12)(_tmp238.ReachableFL).val;_tmp23A=_tmp239.f1;
_tmp23B=_tmp239.f2;_LL176: _tmp23B=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp23B,_tmp1C3);_tmp231=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C3->loc,_tmp23A,env->all_changed,_tmp237,_tmp172->unknown_all),
_tmp23B);goto _LL174;_LL177:;_LL178: goto _LL174;_LL174:;}{struct _tuple13 _tmp7C5;
return(_tmp7C5.f1=_tmp231,((_tmp7C5.f2=_tmp172->unknown_all,_tmp7C5)));};};}
_LL136: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C4=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp191;if(_tmp1C4->tag != 4)goto _LL138;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;
if(_tmp1C6 == 0)goto _LL138;_tmp1C7=*_tmp1C6;_tmp1C8=_tmp1C4->f3;}}_LL137: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7C8;void*_tmp7C7;(_tmp7C7=0,Cyc_Tcutil_terr(
e->loc,((_tmp7C8="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7C8,sizeof(char),58))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));}{
struct _tuple13 _tmp7C9;return(_tmp7C9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C9.f2=
_tmp172->unknown_all,_tmp7C9)));};}{struct Cyc_List_List _tmp7CA;struct Cyc_List_List
_tmp241=(_tmp7CA.hd=_tmp1C8,((_tmp7CA.tl=0,_tmp7CA)));struct Cyc_List_List _tmp7CB;
struct Cyc_List_List _tmp242=(_tmp7CB.hd=_tmp1C5,((_tmp7CB.tl=(struct Cyc_List_List*)&
_tmp241,_tmp7CB)));union Cyc_CfFlowInfo_FlowInfo _tmp244;struct _tuple13 _tmp243=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp242);_tmp244=_tmp243.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp246;struct _tuple14 _tmp245=Cyc_NewControlFlow_anal_Lexp(env,_tmp244,0,_tmp1C5);
_tmp246=_tmp245.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp247=_tmp244;struct _tuple12
_tmp248;struct Cyc_Dict_Dict _tmp249;struct Cyc_List_List*_tmp24A;_LL17A: if((
_tmp247.ReachableFL).tag != 2)goto _LL17C;_tmp248=(struct _tuple12)(_tmp247.ReachableFL).val;
_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;_LL17B:{union Cyc_CfFlowInfo_AbsLVal _tmp24B=
_tmp246;struct Cyc_CfFlowInfo_Place*_tmp24C;int _tmp24D;_LL17F: if((_tmp24B.PlaceL).tag
!= 1)goto _LL181;_tmp24C=(struct Cyc_CfFlowInfo_Place*)(_tmp24B.PlaceL).val;_LL180:
_tmp24A=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,_tmp24A,_tmp1C5);_tmp249=Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C5->loc,_tmp249,env->all_changed,_tmp24C,_tmp172->unknown_all);
_tmp244=Cyc_CfFlowInfo_ReachableFL(_tmp249,_tmp24A);goto _LL17E;_LL181: if((
_tmp24B.UnknownL).tag != 2)goto _LL17E;_tmp24D=(int)(_tmp24B.UnknownL).val;_LL182:
goto _LL17E;_LL17E:;}goto _LL179;_LL17C:;_LL17D: goto _LL179;_LL179:;}{struct
_tuple13 _tmp7CC;return(_tmp7CC.f1=_tmp244,((_tmp7CC.f2=_tmp172->unknown_all,
_tmp7CC)));};};};_LL138: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C9=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp191;if(_tmp1C9->tag != 4)goto _LL13A;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;
if(_tmp1CB != 0)goto _LL13A;_tmp1CC=_tmp1C9->f3;}}_LL139: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_terr(e->loc,((_tmp7CF="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7CF,sizeof(char),58))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}{
struct _tuple13 _tmp7D0;return(_tmp7D0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D0.f2=
_tmp172->unknown_all,_tmp7D0)));};}{struct Cyc_Dict_Dict*_tmp254=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7D1;env->all_changed=((_tmp7D1=_region_malloc(
env->r,sizeof(*_tmp7D1)),((_tmp7D1[0]=_tmp172->mt_place_set,_tmp7D1))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp257;union Cyc_CfFlowInfo_AbsLVal _tmp258;struct _tuple14
_tmp256=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1CA);_tmp257=_tmp256.f1;
_tmp258=_tmp256.f2;{struct Cyc_Dict_Dict _tmp259=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7D2;env->all_changed=((
_tmp7D2=_region_malloc(env->r,sizeof(*_tmp7D2)),((_tmp7D2[0]=_tmp172->mt_place_set,
_tmp7D2))));}{union Cyc_CfFlowInfo_FlowInfo _tmp25C;void*_tmp25D;struct _tuple13
_tmp25B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CC);_tmp25C=_tmp25B.f1;
_tmp25D=_tmp25B.f2;{struct Cyc_Dict_Dict _tmp25E=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp25F=Cyc_CfFlowInfo_after_flow(
_tmp172,(struct Cyc_Dict_Dict*)& _tmp259,_tmp257,_tmp25C,_tmp259,_tmp25E);union Cyc_CfFlowInfo_FlowInfo
_tmp260=Cyc_CfFlowInfo_join_flow(_tmp172,_tmp254,inflow,_tmp25F);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp260,inflow)){if(_tmp254 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp254,Cyc_CfFlowInfo_union_place_set(
_tmp259,_tmp25E,0),0);}return Cyc_NewControlFlow_do_assign(_tmp172,env,_tmp25F,
_tmp1CA,_tmp258,_tmp1CC,_tmp25D,e->loc);}inflow=_tmp260;};};};};}};_LL13A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1CD=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp191;if(_tmp1CD->tag != 9)goto _LL13C;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}
_LL13B: {union Cyc_CfFlowInfo_FlowInfo _tmp262;void*_tmp263;struct _tuple13 _tmp261=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1CE);_tmp262=_tmp261.f1;_tmp263=
_tmp261.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp262,_tmp1CF);}
_LL13C: {struct Cyc_Absyn_Throw_e_struct*_tmp1D0=(struct Cyc_Absyn_Throw_e_struct*)
_tmp191;if(_tmp1D0->tag != 12)goto _LL13E;else{_tmp1D1=_tmp1D0->f1;}}_LL13D: {
union Cyc_CfFlowInfo_FlowInfo _tmp265;void*_tmp266;struct _tuple13 _tmp264=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1D1);_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1D1->loc,_tmp265,_tmp266);{struct _tuple13 _tmp7D3;return(_tmp7D3.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7D3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp7D3)));};}_LL13E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1D2=(struct Cyc_Absyn_FnCall_e_struct*)_tmp191;if(_tmp1D2->tag != 11)goto
_LL140;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}_LL13F: {struct
_RegionHandle*_tmp268=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp26B;struct Cyc_List_List*
_tmp26C;struct Cyc_List_List*_tmp7D4;struct _tuple19 _tmp26A=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp268,env,inflow,((_tmp7D4=_region_malloc(_tmp268,sizeof(*_tmp7D4)),((_tmp7D4->hd=
_tmp1D3,((_tmp7D4->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp268,_tmp1D4),_tmp7D4)))))),0,1);_tmp26B=_tmp26A.f1;
_tmp26C=_tmp26A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp26D=Cyc_NewControlFlow_use_Rval(
env,_tmp1D3->loc,_tmp26B,(void*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd);
_tmp26C=_tmp26C->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp26E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1D3->topt))->v);struct Cyc_Absyn_PtrInfo _tmp270;void*_tmp271;
_LL184: {struct Cyc_Absyn_PointerType_struct*_tmp26F=(struct Cyc_Absyn_PointerType_struct*)
_tmp26E;if(_tmp26F->tag != 5)goto _LL186;else{_tmp270=_tmp26F->f1;_tmp271=_tmp270.elt_typ;}}
_LL185:{void*_tmp272=Cyc_Tcutil_compress(_tmp271);struct Cyc_Absyn_FnInfo _tmp274;
struct Cyc_List_List*_tmp275;_LL189: {struct Cyc_Absyn_FnType_struct*_tmp273=(
struct Cyc_Absyn_FnType_struct*)_tmp272;if(_tmp273->tag != 10)goto _LL18B;else{
_tmp274=_tmp273->f1;_tmp275=_tmp274.attributes;}}_LL18A: for(0;_tmp275 != 0;
_tmp275=_tmp275->tl){void*_tmp276=(void*)_tmp275->hd;int _tmp278;int _tmp27A;
_LL18E: {struct Cyc_Absyn_Initializes_att_struct*_tmp277=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp276;if(_tmp277->tag != 20)goto _LL190;else{_tmp278=_tmp277->f1;}}_LL18F:{
struct Cyc_List_List*_tmp7D5;init_params=((_tmp7D5=_region_malloc(_tmp268,sizeof(*
_tmp7D5)),((_tmp7D5->hd=(void*)_tmp278,((_tmp7D5->tl=init_params,_tmp7D5))))));}
goto _LL18D;_LL190: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp279=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp276;if(_tmp279->tag != 21)goto _LL192;else{_tmp27A=_tmp279->f1;}}_LL191:{
struct Cyc_List_List*_tmp7D6;nolive_unique_params=((_tmp7D6=_region_malloc(
_tmp268,sizeof(*_tmp7D6)),((_tmp7D6->hd=(void*)_tmp27A,((_tmp7D6->tl=
nolive_unique_params,_tmp7D6))))));}goto _LL18D;_LL192:;_LL193: goto _LL18D;_LL18D:;}
goto _LL188;_LL18B:;_LL18C: {const char*_tmp7D9;void*_tmp7D8;(_tmp7D8=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7D9="anal_Rexp: bad function type",
_tag_dyneither(_tmp7D9,sizeof(char),29))),_tag_dyneither(_tmp7D8,sizeof(void*),0)));}
_LL188:;}goto _LL183;_LL186:;_LL187: {const char*_tmp7DC;void*_tmp7DB;(_tmp7DB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7DC="anal_Rexp: bad function type",_tag_dyneither(_tmp7DC,sizeof(char),29))),
_tag_dyneither(_tmp7DB,sizeof(void*),0)));}_LL183:;}{int i=1;for(0;_tmp26C != 0;(((
_tmp26C=_tmp26C->tl,_tmp1D4=((struct Cyc_List_List*)_check_null(_tmp1D4))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp26D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D4))->hd)->loc,_tmp26D,(void*)_tmp26C->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp26D=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D4))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D4))->hd)->topt))->v,_tmp26D,(void*)_tmp26C->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp281=_tmp26B;int _tmp282;struct _tuple12 _tmp283;
struct Cyc_Dict_Dict _tmp284;_LL195: if((_tmp281.BottomFL).tag != 1)goto _LL197;
_tmp282=(int)(_tmp281.BottomFL).val;_LL196: goto _LL194;_LL197: if((_tmp281.ReachableFL).tag
!= 2)goto _LL194;_tmp283=(struct _tuple12)(_tmp281.ReachableFL).val;_tmp284=
_tmp283.f1;_LL198: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp284,(void*)_tmp26C->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7DF;void*_tmp7DE;(_tmp7DE=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D4))->hd)->loc,((
_tmp7DF="expression may not be initialized",_tag_dyneither(_tmp7DF,sizeof(char),
34))),_tag_dyneither(_tmp7DE,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp287=_tmp26D;int _tmp288;struct _tuple12 _tmp289;struct Cyc_Dict_Dict _tmp28A;
struct Cyc_List_List*_tmp28B;_LL19A: if((_tmp287.BottomFL).tag != 1)goto _LL19C;
_tmp288=(int)(_tmp287.BottomFL).val;_LL19B: goto _LL199;_LL19C: if((_tmp287.ReachableFL).tag
!= 2)goto _LL199;_tmp289=(struct _tuple12)(_tmp287.ReachableFL).val;_tmp28A=
_tmp289.f1;_tmp28B=_tmp289.f2;_LL19D: {struct Cyc_Dict_Dict _tmp28C=Cyc_CfFlowInfo_escape_deref(
_tmp172,_tmp28A,env->all_changed,(void*)_tmp26C->hd);{void*_tmp28D=(void*)
_tmp26C->hd;struct Cyc_CfFlowInfo_Place*_tmp28F;_LL19F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp28E=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp28D;if(_tmp28E->tag != 5)goto
_LL1A1;else{_tmp28F=_tmp28E->f1;}}_LL1A0:{void*_tmp290=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D4))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp292;void*
_tmp293;_LL1A4: {struct Cyc_Absyn_PointerType_struct*_tmp291=(struct Cyc_Absyn_PointerType_struct*)
_tmp290;if(_tmp291->tag != 5)goto _LL1A6;else{_tmp292=_tmp291->f1;_tmp293=_tmp292.elt_typ;}}
_LL1A5: _tmp28C=Cyc_CfFlowInfo_assign_place(_tmp172,((struct Cyc_Absyn_Exp*)
_tmp1D4->hd)->loc,_tmp28C,env->all_changed,_tmp28F,Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,_tmp293,_tmp172->esc_all));goto _LL1A3;_LL1A6:;_LL1A7: {const char*_tmp7E2;
void*_tmp7E1;(_tmp7E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp7E2="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7E2,sizeof(char),39))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}
_LL1A3:;}goto _LL19E;_LL1A1:;_LL1A2: goto _LL19E;_LL19E:;}_tmp26D=Cyc_CfFlowInfo_ReachableFL(
_tmp28C,_tmp28B);goto _LL199;}_LL199:;}goto _LL194;_LL194:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D3->topt))->v)){struct _tuple13
_tmp7E3;return(_tmp7E3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E3.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp7E3)));}else{struct _tuple13 _tmp7E4;return(_tmp7E4.f1=_tmp26D,((_tmp7E4.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp172->unknown_all),_tmp7E4)));}};};}_LL140: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D5=(struct Cyc_Absyn_Malloc_e_struct*)_tmp191;if(_tmp1D5->tag != 34)goto
_LL142;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D6.is_calloc;_tmp1D8=_tmp1D6.rgn;
_tmp1D9=_tmp1D6.elt_type;_tmp1DA=_tmp1D6.num_elts;_tmp1DB=_tmp1D6.fat_result;}}
_LL141: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7E7;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7E6;void*root=(void*)((_tmp7E6=_region_malloc(_tmp172->r,sizeof(*_tmp7E6)),((
_tmp7E6[0]=((_tmp7E7.tag=1,((_tmp7E7.f1=_tmp1DA,((_tmp7E7.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7E7)))))),_tmp7E6))));struct
Cyc_CfFlowInfo_Place*_tmp7E8;struct Cyc_CfFlowInfo_Place*place=(_tmp7E8=
_region_malloc(_tmp172->r,sizeof(*_tmp7E8)),((_tmp7E8->root=root,((_tmp7E8->fields=
0,_tmp7E8)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7EB;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7EA;void*rval=(void*)((_tmp7EA=_region_malloc(_tmp172->r,sizeof(*_tmp7EA)),((
_tmp7EA[0]=((_tmp7EB.tag=5,((_tmp7EB.f1=place,_tmp7EB)))),_tmp7EA))));void*
place_val;if(_tmp1DB)place_val=_tmp172->notzeroall;else{if(_tmp1D7)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1D9)),_tmp172->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1D9)),_tmp172->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1D8 != 0){struct _RegionHandle*_tmp298=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_Absyn_Exp*_tmp7EC[2];struct
_tuple19 _tmp29A=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp298,env,inflow,((
_tmp7EC[1]=_tmp1DA,((_tmp7EC[0]=(struct Cyc_Absyn_Exp*)_tmp1D8,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp298,
_tag_dyneither(_tmp7EC,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp29B=_tmp29A.f1;
_tmp29C=_tmp29A.f2;outflow=_tmp29B;}else{union Cyc_CfFlowInfo_FlowInfo _tmp29E;
struct _tuple13 _tmp29D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DA);_tmp29E=
_tmp29D.f1;outflow=_tmp29E;}{union Cyc_CfFlowInfo_FlowInfo _tmp29F=outflow;int
_tmp2A0;struct _tuple12 _tmp2A1;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*
_tmp2A3;_LL1A9: if((_tmp29F.BottomFL).tag != 1)goto _LL1AB;_tmp2A0=(int)(_tmp29F.BottomFL).val;
_LL1AA: {struct _tuple13 _tmp7ED;return(_tmp7ED.f1=outflow,((_tmp7ED.f2=rval,
_tmp7ED)));}_LL1AB: if((_tmp29F.ReachableFL).tag != 2)goto _LL1A8;_tmp2A1=(struct
_tuple12)(_tmp29F.ReachableFL).val;_tmp2A2=_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;_LL1AC: {
struct _tuple13 _tmp7EE;return(_tmp7EE.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A2,root,place_val),
_tmp2A3),((_tmp7EE.f2=rval,_tmp7EE)));}_LL1A8:;};};}_LL142: {struct Cyc_Absyn_Swap_e_struct*
_tmp1DC=(struct Cyc_Absyn_Swap_e_struct*)_tmp191;if(_tmp1DC->tag != 35)goto _LL144;
else{_tmp1DD=_tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}}_LL143: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp2AB=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_Absyn_Exp*
_tmp7EF[2];struct _tuple19 _tmp2AD=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AB,
env,inflow,((_tmp7EF[1]=_tmp1DE,((_tmp7EF[0]=_tmp1DD,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AB,
_tag_dyneither(_tmp7EF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2AE=_tmp2AD.f1;
_tmp2AF=_tmp2AD.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF->tl))->hd;outflow=
_tmp2AE;}{union Cyc_CfFlowInfo_AbsLVal _tmp2B1;struct _tuple14 _tmp2B0=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1DD);_tmp2B1=_tmp2B0.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2B3;
struct _tuple14 _tmp2B2=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1DE);
_tmp2B3=_tmp2B2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B4=outflow;struct _tuple12
_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;struct Cyc_List_List*_tmp2B7;_LL1AE: if((
_tmp2B4.ReachableFL).tag != 2)goto _LL1B0;_tmp2B5=(struct _tuple12)(_tmp2B4.ReachableFL).val;
_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;_LL1AF:{union Cyc_CfFlowInfo_AbsLVal _tmp2B8=
_tmp2B1;struct Cyc_CfFlowInfo_Place*_tmp2B9;int _tmp2BA;_LL1B3: if((_tmp2B8.PlaceL).tag
!= 1)goto _LL1B5;_tmp2B9=(struct Cyc_CfFlowInfo_Place*)(_tmp2B8.PlaceL).val;_LL1B4:
_tmp2B6=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1DD->loc,_tmp2B6,env->all_changed,
_tmp2B9,right_rval);goto _LL1B2;_LL1B5: if((_tmp2B8.UnknownL).tag != 2)goto _LL1B2;
_tmp2BA=(int)(_tmp2B8.UnknownL).val;_LL1B6: goto _LL1B2;_LL1B2:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2BB=_tmp2B3;struct Cyc_CfFlowInfo_Place*_tmp2BC;int _tmp2BD;_LL1B8: if((_tmp2BB.PlaceL).tag
!= 1)goto _LL1BA;_tmp2BC=(struct Cyc_CfFlowInfo_Place*)(_tmp2BB.PlaceL).val;_LL1B9:
_tmp2B6=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1DE->loc,_tmp2B6,env->all_changed,
_tmp2BC,left_rval);goto _LL1B7;_LL1BA: if((_tmp2BB.UnknownL).tag != 2)goto _LL1B7;
_tmp2BD=(int)(_tmp2BB.UnknownL).val;_LL1BB: goto _LL1B7;_LL1B7:;}_tmp2B7=Cyc_CfFlowInfo_reln_kill_exp(
_tmp172->r,_tmp2B7,_tmp1DD);_tmp2B7=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp2B7,_tmp1DE);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B6,_tmp2B7);goto _LL1AD;
_LL1B0:;_LL1B1: goto _LL1AD;_LL1AD:;}{struct _tuple13 _tmp7F0;return(_tmp7F0.f1=
outflow,((_tmp7F0.f2=_tmp172->unknown_all,_tmp7F0)));};};};}_LL144: {struct Cyc_Absyn_New_e_struct*
_tmp1DF=(struct Cyc_Absyn_New_e_struct*)_tmp191;if(_tmp1DF->tag != 17)goto _LL146;
else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LL145: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp7F3;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp7F2;void*root=(void*)((_tmp7F2=
_region_malloc(_tmp172->r,sizeof(*_tmp7F2)),((_tmp7F2[0]=((_tmp7F3.tag=1,((
_tmp7F3.f1=_tmp1E1,((_tmp7F3.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp7F3)))))),_tmp7F2))));struct Cyc_CfFlowInfo_Place*_tmp7F4;
struct Cyc_CfFlowInfo_Place*place=(_tmp7F4=_region_malloc(_tmp172->r,sizeof(*
_tmp7F4)),((_tmp7F4->root=root,((_tmp7F4->fields=0,_tmp7F4)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp7F7;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7F6;void*rval=(void*)((_tmp7F6=
_region_malloc(_tmp172->r,sizeof(*_tmp7F6)),((_tmp7F6[0]=((_tmp7F7.tag=5,((
_tmp7F7.f1=place,_tmp7F7)))),_tmp7F6))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1E0 != 0){struct _RegionHandle*_tmp2BF=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Exp*_tmp7F8[2];struct
_tuple19 _tmp2C1=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2BF,env,inflow,((
_tmp7F8[1]=_tmp1E1,((_tmp7F8[0]=(struct Cyc_Absyn_Exp*)_tmp1E0,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2BF,
_tag_dyneither(_tmp7F8,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C2=_tmp2C1.f1;
_tmp2C3=_tmp2C1.f2;outflow=_tmp2C2;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C3))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2C5;void*_tmp2C6;struct _tuple13 _tmp2C4=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1E1);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;outflow=_tmp2C5;
place_val=_tmp2C6;}{union Cyc_CfFlowInfo_FlowInfo _tmp2C7=outflow;int _tmp2C8;
struct _tuple12 _tmp2C9;struct Cyc_Dict_Dict _tmp2CA;struct Cyc_List_List*_tmp2CB;
_LL1BD: if((_tmp2C7.BottomFL).tag != 1)goto _LL1BF;_tmp2C8=(int)(_tmp2C7.BottomFL).val;
_LL1BE: {struct _tuple13 _tmp7F9;return(_tmp7F9.f1=outflow,((_tmp7F9.f2=rval,
_tmp7F9)));}_LL1BF: if((_tmp2C7.ReachableFL).tag != 2)goto _LL1BC;_tmp2C9=(struct
_tuple12)(_tmp2C7.ReachableFL).val;_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;_LL1C0: {
struct _tuple13 _tmp7FA;return(_tmp7FA.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CA,root,place_val),
_tmp2CB),((_tmp7FA.f2=rval,_tmp7FA)));}_LL1BC:;};};}_LL146: {struct Cyc_Absyn_Address_e_struct*
_tmp1E2=(struct Cyc_Absyn_Address_e_struct*)_tmp191;if(_tmp1E2->tag != 16)goto
_LL148;else{_tmp1E3=_tmp1E2->f1;}}_LL147: {union Cyc_CfFlowInfo_FlowInfo _tmp2D4;
union Cyc_CfFlowInfo_AbsLVal _tmp2D5;struct _tuple14 _tmp2D3=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1E3);_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2D6=_tmp2D5;int _tmp2D7;struct Cyc_CfFlowInfo_Place*_tmp2D8;_LL1C2: if((_tmp2D6.UnknownL).tag
!= 2)goto _LL1C4;_tmp2D7=(int)(_tmp2D6.UnknownL).val;_LL1C3: {struct _tuple13
_tmp7FB;return(_tmp7FB.f1=_tmp2D4,((_tmp7FB.f2=_tmp172->notzeroall,_tmp7FB)));}
_LL1C4: if((_tmp2D6.PlaceL).tag != 1)goto _LL1C1;_tmp2D8=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D6.PlaceL).val;_LL1C5: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp801;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp800;struct _tuple13 _tmp7FF;return(
_tmp7FF.f1=_tmp2D4,((_tmp7FF.f2=(void*)((_tmp801=_region_malloc(env->r,sizeof(*
_tmp801)),((_tmp801[0]=((_tmp800.tag=5,((_tmp800.f1=_tmp2D8,_tmp800)))),_tmp801)))),
_tmp7FF)));}_LL1C1:;};}_LL148: {struct Cyc_Absyn_Deref_e_struct*_tmp1E4=(struct
Cyc_Absyn_Deref_e_struct*)_tmp191;if(_tmp1E4->tag != 21)goto _LL14A;else{_tmp1E5=
_tmp1E4->f1;}}_LL149: {union Cyc_CfFlowInfo_FlowInfo _tmp2DE;void*_tmp2DF;struct
_tuple13 _tmp2DD=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E5);_tmp2DE=
_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2DE,_tmp1E5,_tmp2DF);}_LL14A: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E6=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp191;if(_tmp1E6->tag != 22)goto _LL14C;
else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;}}_LL14B: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E1;void*_tmp2E2;struct _tuple13 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E7);_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E7->topt))->v)){struct _tuple13
_tmp802;return(_tmp802.f1=_tmp2E1,((_tmp802.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp802)));}{void*_tmp2E4=_tmp2E2;int _tmp2E6;struct _dyneither_ptr _tmp2E7;_LL1C7: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2E5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2E4;if(_tmp2E5->tag != 6)goto _LL1C9;else{_tmp2E6=_tmp2E5->f1;_tmp2E7=_tmp2E5->f2;}}
_LL1C8: {int _tmp2E8=Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1E7->topt))->v,_tmp1E8);struct _tuple13 _tmp803;return(_tmp803.f1=
_tmp2E1,((_tmp803.f2=*((void**)_check_dyneither_subscript(_tmp2E7,sizeof(void*),
_tmp2E8)),_tmp803)));}_LL1C9:;_LL1CA: {struct Cyc_Core_Impossible_struct _tmp810;
const char*_tmp80F;void*_tmp80E[1];struct Cyc_String_pa_struct _tmp80D;struct Cyc_Core_Impossible_struct*
_tmp80C;(int)_throw((void*)((_tmp80C=_cycalloc(sizeof(*_tmp80C)),((_tmp80C[0]=((
_tmp810.tag=Cyc_Core_Impossible,((_tmp810.f1=(struct _dyneither_ptr)((_tmp80D.tag=
0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp80E[0]=& _tmp80D,Cyc_aprintf(((_tmp80F="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp80F,sizeof(char),26))),_tag_dyneither(_tmp80E,sizeof(void*),1)))))))),
_tmp810)))),_tmp80C)))));}_LL1C6:;};}_LL14C: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1E9=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp191;if(_tmp1E9->tag != 38)goto
_LL14E;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=_tmp1E9->f2;}}_LL14D: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F0;void*_tmp2F1;struct _tuple13 _tmp2EF=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1EA);_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;{struct _tuple13 _tmp811;
return(_tmp811.f1=_tmp2F0,((_tmp811.f2=_tmp172->unknown_all,_tmp811)));};}_LL14E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp1EC=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp191;if(_tmp1EC->tag != 23)goto _LL150;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}
_LL14F: {union Cyc_CfFlowInfo_FlowInfo _tmp2F4;void*_tmp2F5;struct _tuple13 _tmp2F3=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1ED);_tmp2F4=_tmp2F3.f1;_tmp2F5=
_tmp2F3.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2F7;void*_tmp2F8;struct _tuple13
_tmp2F6=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F4,_tmp1ED,_tmp2F5);
_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;{void*_tmp2F9=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2FB;void*_tmp2FC;_LL1CC: {struct Cyc_Absyn_PointerType_struct*_tmp2FA=(struct
Cyc_Absyn_PointerType_struct*)_tmp2F9;if(_tmp2FA->tag != 5)goto _LL1CE;else{
_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FB.elt_typ;}}_LL1CD: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2FC)){struct _tuple13 _tmp812;return(_tmp812.f1=_tmp2F7,((_tmp812.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp812)));}{void*_tmp2FE=_tmp2F8;int _tmp300;struct _dyneither_ptr _tmp301;_LL1D1: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2FF=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2FE;if(_tmp2FF->tag != 6)goto _LL1D3;else{_tmp300=_tmp2FF->f1;_tmp301=_tmp2FF->f2;}}
_LL1D2: {int _tmp302=Cyc_CfFlowInfo_get_field_index(_tmp2FC,_tmp1EE);struct
_tuple13 _tmp813;return(_tmp813.f1=_tmp2F7,((_tmp813.f2=*((void**)
_check_dyneither_subscript(_tmp301,sizeof(void*),_tmp302)),_tmp813)));}_LL1D3:;
_LL1D4: {struct Cyc_Core_Impossible_struct _tmp819;const char*_tmp818;struct Cyc_Core_Impossible_struct*
_tmp817;(int)_throw((void*)((_tmp817=_cycalloc(sizeof(*_tmp817)),((_tmp817[0]=((
_tmp819.tag=Cyc_Core_Impossible,((_tmp819.f1=((_tmp818="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp818,sizeof(char),21))),_tmp819)))),_tmp817)))));}_LL1D0:;};
_LL1CE:;_LL1CF: {struct Cyc_Core_Impossible_struct _tmp81F;const char*_tmp81E;
struct Cyc_Core_Impossible_struct*_tmp81D;(int)_throw((void*)((_tmp81D=_cycalloc(
sizeof(*_tmp81D)),((_tmp81D[0]=((_tmp81F.tag=Cyc_Core_Impossible,((_tmp81F.f1=((
_tmp81E="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp81E,sizeof(char),25))),
_tmp81F)))),_tmp81D)))));}_LL1CB:;};};}_LL150: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1EF=(struct Cyc_Absyn_Conditional_e_struct*)_tmp191;if(_tmp1EF->tag != 6)goto
_LL152;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;_tmp1F2=_tmp1EF->f3;}}_LL151: {
union Cyc_CfFlowInfo_FlowInfo _tmp30B;union Cyc_CfFlowInfo_FlowInfo _tmp30C;struct
_tuple15 _tmp30A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F0);_tmp30B=_tmp30A.f1;
_tmp30C=_tmp30A.f2;{struct _tuple13 _tmp30D=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp30B,_tmp1F1);struct _tuple13 _tmp30E=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp30C,_tmp1F2);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,
env->all_changed,_tmp30D,_tmp30E);};}_LL152: {struct Cyc_Absyn_And_e_struct*
_tmp1F3=(struct Cyc_Absyn_And_e_struct*)_tmp191;if(_tmp1F3->tag != 7)goto _LL154;
else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F3->f2;}}_LL153: {union Cyc_CfFlowInfo_FlowInfo
_tmp310;union Cyc_CfFlowInfo_FlowInfo _tmp311;struct _tuple15 _tmp30F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F4);_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp313;void*_tmp314;struct _tuple13 _tmp312=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp310,_tmp1F5);_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;{struct _tuple13
_tmp820;struct _tuple13 _tmp315=(_tmp820.f1=_tmp313,((_tmp820.f2=_tmp314,_tmp820)));
struct _tuple13 _tmp821;struct _tuple13 _tmp316=(_tmp821.f1=_tmp311,((_tmp821.f2=
_tmp172->zero,_tmp821)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp315,_tmp316);};};}_LL154: {struct Cyc_Absyn_Or_e_struct*_tmp1F6=(struct Cyc_Absyn_Or_e_struct*)
_tmp191;if(_tmp1F6->tag != 8)goto _LL156;else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}
_LL155: {union Cyc_CfFlowInfo_FlowInfo _tmp31A;union Cyc_CfFlowInfo_FlowInfo _tmp31B;
struct _tuple15 _tmp319=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F7);_tmp31A=
_tmp319.f1;_tmp31B=_tmp319.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp31D;void*_tmp31E;
struct _tuple13 _tmp31C=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp31B,_tmp1F8);
_tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;{struct _tuple13 _tmp822;struct _tuple13
_tmp31F=(_tmp822.f1=_tmp31D,((_tmp822.f2=_tmp31E,_tmp822)));struct _tuple13
_tmp823;struct _tuple13 _tmp320=(_tmp823.f1=_tmp31A,((_tmp823.f2=_tmp172->notzeroall,
_tmp823)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp31F,_tmp320);};};}_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp1F9=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp191;if(_tmp1F9->tag != 24)goto _LL158;
else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}}_LL157: {struct _RegionHandle*
_tmp323=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp326;struct Cyc_List_List*_tmp327;
struct Cyc_Absyn_Exp*_tmp824[2];struct _tuple19 _tmp325=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp323,env,inflow,((_tmp824[1]=_tmp1FB,((_tmp824[0]=_tmp1FA,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp323,
_tag_dyneither(_tmp824,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp326=_tmp325.f1;
_tmp327=_tmp325.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp328=_tmp326;{union Cyc_CfFlowInfo_FlowInfo
_tmp329=_tmp326;struct _tuple12 _tmp32A;struct Cyc_Dict_Dict _tmp32B;struct Cyc_List_List*
_tmp32C;_LL1D6: if((_tmp329.ReachableFL).tag != 2)goto _LL1D8;_tmp32A=(struct
_tuple12)(_tmp329.ReachableFL).val;_tmp32B=_tmp32A.f1;_tmp32C=_tmp32A.f2;_LL1D7:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp32B,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp327))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp827;void*_tmp826;(_tmp826=0,Cyc_Tcutil_terr(_tmp1FB->loc,((_tmp827="expression may not be initialized",
_tag_dyneither(_tmp827,sizeof(char),34))),_tag_dyneither(_tmp826,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp32F=Cyc_NewControlFlow_add_subscript_reln(_tmp172->r,
_tmp32C,_tmp1FA,_tmp1FB);if(_tmp32C != _tmp32F)_tmp328=Cyc_CfFlowInfo_ReachableFL(
_tmp32B,_tmp32F);goto _LL1D5;};_LL1D8:;_LL1D9: goto _LL1D5;_LL1D5:;}{void*_tmp330=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FA->topt))->v);
struct Cyc_List_List*_tmp332;struct Cyc_Absyn_PtrInfo _tmp334;struct Cyc_Absyn_PtrAtts
_tmp335;union Cyc_Absyn_Constraint*_tmp336;_LL1DB: {struct Cyc_Absyn_TupleType_struct*
_tmp331=(struct Cyc_Absyn_TupleType_struct*)_tmp330;if(_tmp331->tag != 11)goto
_LL1DD;else{_tmp332=_tmp331->f1;}}_LL1DC: {void*_tmp337=(void*)((struct Cyc_List_List*)
_check_null(_tmp327))->hd;int _tmp339;struct _dyneither_ptr _tmp33A;_LL1E2: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp338=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp337;if(_tmp338->tag != 6)goto _LL1E4;else{_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;}}
_LL1E3: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1FB)).f1;struct
_tuple13 _tmp828;return(_tmp828.f1=_tmp328,((_tmp828.f2=*((void**)
_check_dyneither_subscript(_tmp33A,sizeof(void*),(int)i)),_tmp828)));}_LL1E4:;
_LL1E5: {struct Cyc_Core_Impossible_struct _tmp82E;const char*_tmp82D;struct Cyc_Core_Impossible_struct*
_tmp82C;(int)_throw((void*)((_tmp82C=_cycalloc(sizeof(*_tmp82C)),((_tmp82C[0]=((
_tmp82E.tag=Cyc_Core_Impossible,((_tmp82E.f1=((_tmp82D="anal_Rexp: Subscript",
_tag_dyneither(_tmp82D,sizeof(char),21))),_tmp82E)))),_tmp82C)))));}_LL1E1:;}
_LL1DD: {struct Cyc_Absyn_PointerType_struct*_tmp333=(struct Cyc_Absyn_PointerType_struct*)
_tmp330;if(_tmp333->tag != 5)goto _LL1DF;else{_tmp334=_tmp333->f1;_tmp335=_tmp334.ptr_atts;
_tmp336=_tmp335.bounds;}}_LL1DE: {union Cyc_CfFlowInfo_FlowInfo _tmp340;void*
_tmp341;struct _tuple13 _tmp33F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp326,
_tmp1FA,(void*)((struct Cyc_List_List*)_check_null(_tmp327))->hd);_tmp340=_tmp33F.f1;
_tmp341=_tmp33F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp342=_tmp340;int _tmp343;
_LL1E7: if((_tmp342.BottomFL).tag != 1)goto _LL1E9;_tmp343=(int)(_tmp342.BottomFL).val;
_LL1E8: {struct _tuple13 _tmp82F;return(_tmp82F.f1=_tmp340,((_tmp82F.f2=_tmp341,
_tmp82F)));}_LL1E9:;_LL1EA: {struct _tuple13 _tmp830;return(_tmp830.f1=_tmp328,((
_tmp830.f2=_tmp341,_tmp830)));}_LL1E6:;};}_LL1DF:;_LL1E0: {struct Cyc_Core_Impossible_struct
_tmp836;const char*_tmp835;struct Cyc_Core_Impossible_struct*_tmp834;(int)_throw((
void*)((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((_tmp836.tag=Cyc_Core_Impossible,((
_tmp836.f1=((_tmp835="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp835,
sizeof(char),33))),_tmp836)))),_tmp834)))));}_LL1DA:;};};}_LL158: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1FC=(struct Cyc_Absyn_Datatype_e_struct*)_tmp191;if(_tmp1FC->tag != 31)goto
_LL15A;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;}}_LL159: _tmp200=_tmp1FD;
goto _LL15B;_LL15A: {struct Cyc_Absyn_Tuple_e_struct*_tmp1FF=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp191;if(_tmp1FF->tag != 25)goto _LL15C;else{_tmp200=_tmp1FF->f1;}}_LL15B: {
struct _RegionHandle*_tmp349=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp34B;struct Cyc_List_List*
_tmp34C;struct _tuple19 _tmp34A=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp349,
env,inflow,_tmp200,1,1);_tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;{unsigned int
_tmp83C;unsigned int _tmp83B;struct _dyneither_ptr _tmp83A;void**_tmp839;
unsigned int _tmp838;struct _dyneither_ptr aggrdict=(_tmp838=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp200),((_tmp839=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp838)),((_tmp83A=
_tag_dyneither(_tmp839,sizeof(void*),_tmp838),((((_tmp83B=_tmp838,_tmp83D(&
_tmp83C,& _tmp83B,& _tmp839,& _tmp34C))),_tmp83A)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp843;struct Cyc_CfFlowInfo_Aggregate_struct _tmp842;struct _tuple13 _tmp841;
return(_tmp841.f1=_tmp34B,((_tmp841.f2=(void*)((_tmp843=_region_malloc(env->r,
sizeof(*_tmp843)),((_tmp843[0]=((_tmp842.tag=6,((_tmp842.f1=0,((_tmp842.f2=
aggrdict,_tmp842)))))),_tmp843)))),_tmp841)));};}_LL15C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp201=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp191;if(_tmp201->tag != 30)goto
_LL15E;else{_tmp202=_tmp201->f2;}}_LL15D: {struct Cyc_List_List*fs;{void*_tmp355=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp357;_LL1EC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp356=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp355;if(_tmp356->tag != 13)goto _LL1EE;
else{_tmp357=_tmp356->f2;}}_LL1ED: fs=_tmp357;goto _LL1EB;_LL1EE:;_LL1EF: {struct
Cyc_Core_Impossible_struct _tmp849;const char*_tmp848;struct Cyc_Core_Impossible_struct*
_tmp847;(int)_throw((void*)((_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847[0]=((
_tmp849.tag=Cyc_Core_Impossible,((_tmp849.f1=((_tmp848="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp848,sizeof(char),35))),_tmp849)))),_tmp847)))));}_LL1EB:;}
_tmp204=_tmp202;_tmp207=Cyc_Absyn_StructA;_tmp20A=fs;goto _LL15F;}_LL15E: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp203=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp191;if(_tmp203->tag != 29)goto _LL160;else{_tmp204=_tmp203->f3;_tmp205=_tmp203->f4;
if(_tmp205 == 0)goto _LL160;_tmp206=*_tmp205;_tmp207=_tmp206.kind;_tmp208=_tmp206.impl;
if(_tmp208 == 0)goto _LL160;_tmp209=*_tmp208;_tmp20A=_tmp209.fields;}}_LL15F: {
struct _RegionHandle*_tmp35B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp35D;struct Cyc_List_List*
_tmp35E;struct _tuple19 _tmp35C=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp35B,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp35B,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp204),1,1);_tmp35D=_tmp35C.f1;_tmp35E=_tmp35C.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp172,
_tmp20A,_tmp172->unknown_all);{int i=0;for(0;_tmp35E != 0;(((_tmp35E=_tmp35E->tl,
_tmp204=_tmp204->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple21*)((struct
Cyc_List_List*)_check_null(_tmp204))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp35F=(void*)ds->hd;struct _dyneither_ptr*_tmp362;_LL1F1: {struct Cyc_Absyn_ArrayElement_struct*
_tmp360=(struct Cyc_Absyn_ArrayElement_struct*)_tmp35F;if(_tmp360->tag != 0)goto
_LL1F3;}_LL1F2: {struct Cyc_Core_Impossible_struct _tmp84F;const char*_tmp84E;
struct Cyc_Core_Impossible_struct*_tmp84D;(int)_throw((void*)((_tmp84D=_cycalloc(
sizeof(*_tmp84D)),((_tmp84D[0]=((_tmp84F.tag=Cyc_Core_Impossible,((_tmp84F.f1=((
_tmp84E="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp84E,sizeof(char),22))),
_tmp84F)))),_tmp84D)))));}_LL1F3: {struct Cyc_Absyn_FieldName_struct*_tmp361=(
struct Cyc_Absyn_FieldName_struct*)_tmp35F;if(_tmp361->tag != 1)goto _LL1F0;else{
_tmp362=_tmp361->f1;}}_LL1F4: {int _tmp366=Cyc_CfFlowInfo_get_field_index_fs(
_tmp20A,_tmp362);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp366))=(void*)_tmp35E->hd;if(_tmp207 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp367=(*((struct _tuple21*)_tmp204->hd)).f2;_tmp35D=Cyc_NewControlFlow_use_Rval(
env,_tmp367->loc,_tmp35D,(void*)_tmp35E->hd);}}_LL1F0:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp855;struct Cyc_CfFlowInfo_Aggregate_struct _tmp854;struct _tuple13 _tmp853;
return(_tmp853.f1=_tmp35D,((_tmp853.f2=(void*)((_tmp855=_region_malloc(env->r,
sizeof(*_tmp855)),((_tmp855[0]=((_tmp854.tag=6,((_tmp854.f1=_tmp207 == Cyc_Absyn_UnionA,((
_tmp854.f2=aggrdict,_tmp854)))))),_tmp855)))),_tmp853)));};};}_LL160: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp20B=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp191;if(_tmp20B->tag != 29)goto
_LL162;}_LL161: {struct Cyc_Core_Impossible_struct _tmp85B;const char*_tmp85A;
struct Cyc_Core_Impossible_struct*_tmp859;(int)_throw((void*)((_tmp859=_cycalloc(
sizeof(*_tmp859)),((_tmp859[0]=((_tmp85B.tag=Cyc_Core_Impossible,((_tmp85B.f1=((
_tmp85A="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp85A,sizeof(char),31))),
_tmp85B)))),_tmp859)))));}_LL162: {struct Cyc_Absyn_Array_e_struct*_tmp20C=(
struct Cyc_Absyn_Array_e_struct*)_tmp191;if(_tmp20C->tag != 27)goto _LL164;else{
_tmp20D=_tmp20C->f1;}}_LL163: {struct _RegionHandle*_tmp36E=env->r;struct Cyc_List_List*
_tmp36F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp36E,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp20D);union Cyc_CfFlowInfo_FlowInfo _tmp371;
struct Cyc_List_List*_tmp372;struct _tuple19 _tmp370=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp36E,env,inflow,_tmp36F,1,1);_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;for(0;
_tmp372 != 0;(_tmp372=_tmp372->tl,_tmp36F=_tmp36F->tl)){_tmp371=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36F))->hd)->loc,
_tmp371,(void*)_tmp372->hd);}{struct _tuple13 _tmp85C;return(_tmp85C.f1=_tmp371,((
_tmp85C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp85C)));};}_LL164: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp20E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp191;if(_tmp20E->tag != 28)
goto _LL166;else{_tmp20F=_tmp20E->f1;_tmp210=_tmp20E->f2;_tmp211=_tmp20E->f3;
_tmp212=_tmp20E->f4;}}_LL165: {union Cyc_CfFlowInfo_FlowInfo _tmp375;void*_tmp376;
struct _tuple13 _tmp374=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp210);_tmp375=
_tmp374.f1;_tmp376=_tmp374.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp377=_tmp375;int
_tmp378;struct _tuple12 _tmp379;struct Cyc_Dict_Dict _tmp37A;struct Cyc_List_List*
_tmp37B;_LL1F6: if((_tmp377.BottomFL).tag != 1)goto _LL1F8;_tmp378=(int)(_tmp377.BottomFL).val;
_LL1F7: {struct _tuple13 _tmp85D;return(_tmp85D.f1=_tmp375,((_tmp85D.f2=_tmp172->unknown_all,
_tmp85D)));}_LL1F8: if((_tmp377.ReachableFL).tag != 2)goto _LL1F5;_tmp379=(struct
_tuple12)(_tmp377.ReachableFL).val;_tmp37A=_tmp379.f1;_tmp37B=_tmp379.f2;_LL1F9:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp37A,_tmp376)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp860;void*_tmp85F;(_tmp85F=0,Cyc_Tcutil_terr(_tmp210->loc,((_tmp860="expression may not be initialized",
_tag_dyneither(_tmp860,sizeof(char),34))),_tag_dyneither(_tmp85F,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp37B;comp_loop: {void*_tmp37F=_tmp210->r;struct
Cyc_Absyn_Exp*_tmp381;void*_tmp383;struct Cyc_Absyn_Vardecl*_tmp385;void*_tmp387;
struct Cyc_Absyn_Vardecl*_tmp389;void*_tmp38B;struct Cyc_Absyn_Vardecl*_tmp38D;
void*_tmp38F;struct Cyc_Absyn_Vardecl*_tmp391;union Cyc_Absyn_Cnst _tmp393;struct
_tuple6 _tmp394;int _tmp395;enum Cyc_Absyn_Primop _tmp397;struct Cyc_List_List*
_tmp398;struct Cyc_List_List _tmp399;struct Cyc_Absyn_Exp*_tmp39A;_LL1FB: {struct
Cyc_Absyn_Cast_e_struct*_tmp380=(struct Cyc_Absyn_Cast_e_struct*)_tmp37F;if(
_tmp380->tag != 15)goto _LL1FD;else{_tmp381=_tmp380->f2;}}_LL1FC: _tmp210=_tmp381;
goto comp_loop;_LL1FD:{struct Cyc_Absyn_Var_e_struct*_tmp382=(struct Cyc_Absyn_Var_e_struct*)
_tmp37F;if(_tmp382->tag != 1)goto _LL1FF;else{_tmp383=(void*)_tmp382->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp384=(struct Cyc_Absyn_Global_b_struct*)_tmp383;if(
_tmp384->tag != 1)goto _LL1FF;else{_tmp385=_tmp384->f1;}};}}if(!(!_tmp385->escapes))
goto _LL1FF;_LL1FE: _tmp389=_tmp385;goto _LL200;_LL1FF:{struct Cyc_Absyn_Var_e_struct*
_tmp386=(struct Cyc_Absyn_Var_e_struct*)_tmp37F;if(_tmp386->tag != 1)goto _LL201;
else{_tmp387=(void*)_tmp386->f2;{struct Cyc_Absyn_Local_b_struct*_tmp388=(struct
Cyc_Absyn_Local_b_struct*)_tmp387;if(_tmp388->tag != 4)goto _LL201;else{_tmp389=
_tmp388->f1;}};}}if(!(!_tmp389->escapes))goto _LL201;_LL200: _tmp38D=_tmp389;goto
_LL202;_LL201:{struct Cyc_Absyn_Var_e_struct*_tmp38A=(struct Cyc_Absyn_Var_e_struct*)
_tmp37F;if(_tmp38A->tag != 1)goto _LL203;else{_tmp38B=(void*)_tmp38A->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp38C=(struct Cyc_Absyn_Pat_b_struct*)_tmp38B;if(_tmp38C->tag
!= 5)goto _LL203;else{_tmp38D=_tmp38C->f1;}};}}if(!(!_tmp38D->escapes))goto _LL203;
_LL202: _tmp391=_tmp38D;goto _LL204;_LL203:{struct Cyc_Absyn_Var_e_struct*_tmp38E=(
struct Cyc_Absyn_Var_e_struct*)_tmp37F;if(_tmp38E->tag != 1)goto _LL205;else{
_tmp38F=(void*)_tmp38E->f2;{struct Cyc_Absyn_Param_b_struct*_tmp390=(struct Cyc_Absyn_Param_b_struct*)
_tmp38F;if(_tmp390->tag != 3)goto _LL205;else{_tmp391=_tmp390->f1;}};}}if(!(!
_tmp391->escapes))goto _LL205;_LL204:{struct Cyc_CfFlowInfo_Reln*_tmp863;struct Cyc_List_List*
_tmp862;new_relns=((_tmp862=_region_malloc(env->r,sizeof(*_tmp862)),((_tmp862->hd=((
_tmp863=_region_malloc(env->r,sizeof(*_tmp863)),((_tmp863->vd=_tmp20F,((_tmp863->rop=
Cyc_CfFlowInfo_LessVar(_tmp391,_tmp391->type),_tmp863)))))),((_tmp862->tl=
_tmp37B,_tmp862))))));}goto _LL1FA;_LL205: {struct Cyc_Absyn_Const_e_struct*
_tmp392=(struct Cyc_Absyn_Const_e_struct*)_tmp37F;if(_tmp392->tag != 0)goto _LL207;
else{_tmp393=_tmp392->f1;if((_tmp393.Int_c).tag != 4)goto _LL207;_tmp394=(struct
_tuple6)(_tmp393.Int_c).val;_tmp395=_tmp394.f2;}}_LL206:{struct Cyc_CfFlowInfo_Reln*
_tmp866;struct Cyc_List_List*_tmp865;new_relns=((_tmp865=_region_malloc(env->r,
sizeof(*_tmp865)),((_tmp865->hd=((_tmp866=_region_malloc(env->r,sizeof(*_tmp866)),((
_tmp866->vd=_tmp20F,((_tmp866->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp395),_tmp866)))))),((_tmp865->tl=_tmp37B,_tmp865))))));}goto _LL1FA;_LL207: {
struct Cyc_Absyn_Primop_e_struct*_tmp396=(struct Cyc_Absyn_Primop_e_struct*)
_tmp37F;if(_tmp396->tag != 3)goto _LL209;else{_tmp397=_tmp396->f1;_tmp398=_tmp396->f2;
if(_tmp398 == 0)goto _LL209;_tmp399=*_tmp398;_tmp39A=(struct Cyc_Absyn_Exp*)_tmp399.hd;}}
_LL208:{void*_tmp39F=_tmp39A->r;void*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A3;
void*_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A7;void*_tmp3A9;struct Cyc_Absyn_Vardecl*
_tmp3AB;void*_tmp3AD;struct Cyc_Absyn_Vardecl*_tmp3AF;_LL20C:{struct Cyc_Absyn_Var_e_struct*
_tmp3A0=(struct Cyc_Absyn_Var_e_struct*)_tmp39F;if(_tmp3A0->tag != 1)goto _LL20E;
else{_tmp3A1=(void*)_tmp3A0->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3A2=(struct
Cyc_Absyn_Global_b_struct*)_tmp3A1;if(_tmp3A2->tag != 1)goto _LL20E;else{_tmp3A3=
_tmp3A2->f1;}};}}if(!(!_tmp3A3->escapes))goto _LL20E;_LL20D: _tmp3A7=_tmp3A3;goto
_LL20F;_LL20E:{struct Cyc_Absyn_Var_e_struct*_tmp3A4=(struct Cyc_Absyn_Var_e_struct*)
_tmp39F;if(_tmp3A4->tag != 1)goto _LL210;else{_tmp3A5=(void*)_tmp3A4->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3A6=(struct Cyc_Absyn_Local_b_struct*)_tmp3A5;if(
_tmp3A6->tag != 4)goto _LL210;else{_tmp3A7=_tmp3A6->f1;}};}}if(!(!_tmp3A7->escapes))
goto _LL210;_LL20F: _tmp3AB=_tmp3A7;goto _LL211;_LL210:{struct Cyc_Absyn_Var_e_struct*
_tmp3A8=(struct Cyc_Absyn_Var_e_struct*)_tmp39F;if(_tmp3A8->tag != 1)goto _LL212;
else{_tmp3A9=(void*)_tmp3A8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3AA=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3A9;if(_tmp3AA->tag != 5)goto _LL212;else{_tmp3AB=_tmp3AA->f1;}};}}if(!(!
_tmp3AB->escapes))goto _LL212;_LL211: _tmp3AF=_tmp3AB;goto _LL213;_LL212:{struct Cyc_Absyn_Var_e_struct*
_tmp3AC=(struct Cyc_Absyn_Var_e_struct*)_tmp39F;if(_tmp3AC->tag != 1)goto _LL214;
else{_tmp3AD=(void*)_tmp3AC->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3AE=(struct
Cyc_Absyn_Param_b_struct*)_tmp3AD;if(_tmp3AE->tag != 3)goto _LL214;else{_tmp3AF=
_tmp3AE->f1;}};}}if(!(!_tmp3AF->escapes))goto _LL214;_LL213:{struct Cyc_CfFlowInfo_Reln*
_tmp869;struct Cyc_List_List*_tmp868;new_relns=((_tmp868=_region_malloc(env->r,
sizeof(*_tmp868)),((_tmp868->hd=((_tmp869=_region_malloc(env->r,sizeof(*_tmp869)),((
_tmp869->vd=_tmp20F,((_tmp869->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3AF),_tmp869)))))),((
_tmp868->tl=_tmp37B,_tmp868))))));}goto _LL20B;_LL214:;_LL215: goto _LL20B;_LL20B:;}
goto _LL1FA;_LL209:;_LL20A: goto _LL1FA;_LL1FA:;}if(_tmp37B != new_relns)_tmp375=Cyc_CfFlowInfo_ReachableFL(
_tmp37A,new_relns);{void*_tmp3B2=_tmp376;_LL217: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3B3=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3B2;if(_tmp3B3->tag != 0)goto
_LL219;}_LL218: {struct _tuple13 _tmp86A;return(_tmp86A.f1=_tmp375,((_tmp86A.f2=
_tmp172->unknown_all,_tmp86A)));}_LL219: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3B4=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3B2;if(_tmp3B4->tag != 2)
goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3B5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3B2;if(_tmp3B5->tag != 1)
goto _LL21D;}_LL21C: goto _LL21E;_LL21D: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3B6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3B2;if(_tmp3B6->tag != 5)goto
_LL21F;}_LL21E: {struct Cyc_List_List _tmp86B;struct Cyc_List_List _tmp3B8=(_tmp86B.hd=
_tmp20F,((_tmp86B.tl=0,_tmp86B)));_tmp375=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp375,(struct Cyc_List_List*)& _tmp3B8,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3BA;void*_tmp3BB;struct _tuple13 _tmp3B9=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp375,_tmp211);_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B9.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3BC=_tmp3BA;int _tmp3BD;struct _tuple12 _tmp3BE;struct Cyc_Dict_Dict _tmp3BF;
_LL222: if((_tmp3BC.BottomFL).tag != 1)goto _LL224;_tmp3BD=(int)(_tmp3BC.BottomFL).val;
_LL223: {struct _tuple13 _tmp86C;return(_tmp86C.f1=_tmp3BA,((_tmp86C.f2=_tmp172->unknown_all,
_tmp86C)));}_LL224: if((_tmp3BC.ReachableFL).tag != 2)goto _LL221;_tmp3BE=(struct
_tuple12)(_tmp3BC.ReachableFL).val;_tmp3BF=_tmp3BE.f1;_LL225: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3BF,_tmp3BB)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp86F;void*_tmp86E;(
_tmp86E=0,Cyc_Tcutil_terr(_tmp210->loc,((_tmp86F="expression may not be initialized",
_tag_dyneither(_tmp86F,sizeof(char),34))),_tag_dyneither(_tmp86E,sizeof(void*),0)));}{
struct _tuple13 _tmp870;return(_tmp870.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp870.f2=
_tmp172->unknown_all,_tmp870)));};}_LL221:;}_tmp375=_tmp3BA;goto _LL220;};}_LL21F:;
_LL220: while(1){struct Cyc_List_List _tmp871;struct Cyc_List_List _tmp3C5=(_tmp871.hd=
_tmp20F,((_tmp871.tl=0,_tmp871)));_tmp375=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp375,(struct Cyc_List_List*)& _tmp3C5,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3C7;void*_tmp3C8;struct _tuple13 _tmp3C6=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp375,_tmp211);_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3C9=_tmp3C7;int _tmp3CA;struct _tuple12 _tmp3CB;struct Cyc_Dict_Dict _tmp3CC;
_LL227: if((_tmp3C9.BottomFL).tag != 1)goto _LL229;_tmp3CA=(int)(_tmp3C9.BottomFL).val;
_LL228: goto _LL226;_LL229: if((_tmp3C9.ReachableFL).tag != 2)goto _LL226;_tmp3CB=(
struct _tuple12)(_tmp3C9.ReachableFL).val;_tmp3CC=_tmp3CB.f1;_LL22A: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3CC,_tmp3C8)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp874;void*_tmp873;(
_tmp873=0,Cyc_Tcutil_terr(_tmp210->loc,((_tmp874="expression may not be initialized",
_tag_dyneither(_tmp874,sizeof(char),34))),_tag_dyneither(_tmp873,sizeof(void*),0)));}{
struct _tuple13 _tmp875;return(_tmp875.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp875.f2=
_tmp172->unknown_all,_tmp875)));};}_LL226:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3D0=
Cyc_CfFlowInfo_join_flow(_tmp172,env->all_changed,_tmp375,_tmp3C7);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3D0,_tmp375))break;_tmp375=_tmp3D0;};};}{struct _tuple13 _tmp876;return(
_tmp876.f1=_tmp375,((_tmp876.f2=_tmp172->unknown_all,_tmp876)));};_LL216:;};};
_LL1F5:;};}_LL166: {struct Cyc_Absyn_StmtExp_e_struct*_tmp213=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp191;if(_tmp213->tag != 37)goto _LL168;else{_tmp214=_tmp213->f1;}}_LL167: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp214);_LL168: {struct
Cyc_Absyn_Var_e_struct*_tmp215=(struct Cyc_Absyn_Var_e_struct*)_tmp191;if(_tmp215->tag
!= 1)goto _LL16A;else{_tmp216=(void*)_tmp215->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp217=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp216;if(_tmp217->tag != 0)goto
_LL16A;};}}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp218=(
struct Cyc_Absyn_UnknownId_e_struct*)_tmp191;if(_tmp218->tag != 2)goto _LL16C;}
_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp219=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp191;if(_tmp219->tag != 10)goto _LL16E;}_LL16D:
goto _LL16F;_LL16E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp21A=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp191;if(_tmp21A->tag != 36)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp21B=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp191;if(_tmp21B->tag != 26)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Valueof_e_struct*_tmp21C=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp191;if(_tmp21C->tag != 39)goto _LL10B;}
_LL173: {struct Cyc_Core_Impossible_struct _tmp87C;const char*_tmp87B;struct Cyc_Core_Impossible_struct*
_tmp87A;(int)_throw((void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((
_tmp87C.tag=Cyc_Core_Impossible,((_tmp87C.f1=((_tmp87B="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp87B,sizeof(char),31))),_tmp87C)))),_tmp87A)))));}_LL10B:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3D6=env->fenv;void*_tmp3D7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_PtrAtts _tmp3DB;union Cyc_Absyn_Constraint*
_tmp3DC;union Cyc_Absyn_Constraint*_tmp3DD;_LL22C: {struct Cyc_Absyn_PointerType_struct*
_tmp3D8=(struct Cyc_Absyn_PointerType_struct*)_tmp3D7;if(_tmp3D8->tag != 5)goto
_LL22E;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=_tmp3D9.elt_typ;_tmp3DB=_tmp3D9.ptr_atts;
_tmp3DC=_tmp3DB.bounds;_tmp3DD=_tmp3DB.zero_term;}}_LL22D: {union Cyc_CfFlowInfo_FlowInfo
_tmp3DE=f;int _tmp3DF;struct _tuple12 _tmp3E0;struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*
_tmp3E2;_LL231: if((_tmp3DE.BottomFL).tag != 1)goto _LL233;_tmp3DF=(int)(_tmp3DE.BottomFL).val;
_LL232: {struct _tuple14 _tmp87D;return(_tmp87D.f1=f,((_tmp87D.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp87D)));}_LL233: if((_tmp3DE.ReachableFL).tag != 2)goto _LL230;_tmp3E0=(struct
_tuple12)(_tmp3DE.ReachableFL).val;_tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;_LL234:
if(Cyc_Tcutil_is_bound_one(_tmp3DC)){void*_tmp3E4=r;struct Cyc_CfFlowInfo_Place*
_tmp3E8;struct Cyc_CfFlowInfo_Place _tmp3E9;void*_tmp3EA;struct Cyc_List_List*
_tmp3EB;enum Cyc_CfFlowInfo_InitLevel _tmp3EE;_LL236: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3E5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E4;if(_tmp3E5->tag != 1)
goto _LL238;}_LL237: goto _LL239;_LL238: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3E6=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E4;if(_tmp3E6->tag != 2)
goto _LL23A;}_LL239:{struct Cyc_CfFlowInfo_NotZero_struct _tmp880;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp87F;e->annot=(void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((
_tmp880.tag=Cyc_CfFlowInfo_NotZero,((_tmp880.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E2),_tmp880)))),_tmp87F))));}goto _LL235;_LL23A: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3E7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3E4;if(_tmp3E7->tag != 5)goto
_LL23C;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=*_tmp3E8;_tmp3EA=_tmp3E9.root;_tmp3EB=
_tmp3E9.fields;}}_LL23B:{struct Cyc_CfFlowInfo_NotZero_struct _tmp883;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp882;e->annot=(void*)((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=((
_tmp883.tag=Cyc_CfFlowInfo_NotZero,((_tmp883.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E2),_tmp883)))),_tmp882))));}{struct Cyc_CfFlowInfo_Place*_tmp886;struct
_tuple14 _tmp885;return(_tmp885.f1=f,((_tmp885.f2=Cyc_CfFlowInfo_PlaceL(((_tmp886=
_region_malloc(_tmp3D6->r,sizeof(*_tmp886)),((_tmp886->root=_tmp3EA,((_tmp886->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3D6->r,_tmp3EB,flds),_tmp886))))))),_tmp885)));};_LL23C: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3EC=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3E4;if(_tmp3EC->tag != 0)goto _LL23E;}_LL23D: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp887;return(_tmp887.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp887.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp887)));};_LL23E: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3ED=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E4;if(_tmp3ED->tag != 3)goto
_LL240;else{_tmp3EE=_tmp3ED->f1;}}_LL23F: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3EE);goto _LL241;_LL240:;_LL241: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp88A;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp889;e->annot=(void*)((_tmp889=
_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((_tmp88A.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp88A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3E2),_tmp88A)))),
_tmp889))));}_LL235:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp88D;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp88C;e->annot=(void*)((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C[0]=((
_tmp88D.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp88D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E2),_tmp88D)))),_tmp88C))));}if(Cyc_CfFlowInfo_initlevel(_tmp3D6,_tmp3E1,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(
e->loc,((_tmp890="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp890,sizeof(char),46))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}{struct
_tuple14 _tmp891;return(_tmp891.f1=f,((_tmp891.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp891)));};_LL230:;}_LL22E:;_LL22F: {struct Cyc_Core_Impossible_struct _tmp897;
const char*_tmp896;struct Cyc_Core_Impossible_struct*_tmp895;(int)_throw((void*)((
_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp897.tag=Cyc_Core_Impossible,((
_tmp897.f1=((_tmp896="left deref of non-pointer-type",_tag_dyneither(_tmp896,
sizeof(char),31))),_tmp897)))),_tmp895)))));}_LL22B:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp400=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp401=inflow;int _tmp402;struct _tuple12
_tmp403;struct Cyc_Dict_Dict _tmp404;struct Cyc_List_List*_tmp405;_LL243: if((
_tmp401.BottomFL).tag != 1)goto _LL245;_tmp402=(int)(_tmp401.BottomFL).val;_LL244: {
struct _tuple14 _tmp898;return(_tmp898.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp898.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp898)));}_LL245: if((_tmp401.ReachableFL).tag != 2)
goto _LL242;_tmp403=(struct _tuple12)(_tmp401.ReachableFL).val;_tmp404=_tmp403.f1;
_tmp405=_tmp403.f2;_LL246: d=_tmp404;_LL242:;}{void*_tmp407=e->r;struct Cyc_Absyn_Exp*
_tmp409;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp*_tmp40D;void*_tmp40F;
struct Cyc_Absyn_Vardecl*_tmp411;void*_tmp413;struct Cyc_Absyn_Vardecl*_tmp415;
void*_tmp417;struct Cyc_Absyn_Vardecl*_tmp419;void*_tmp41B;struct Cyc_Absyn_Vardecl*
_tmp41D;struct Cyc_Absyn_Exp*_tmp41F;struct _dyneither_ptr*_tmp420;struct Cyc_Absyn_Exp*
_tmp422;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*
_tmp427;struct _dyneither_ptr*_tmp428;_LL248: {struct Cyc_Absyn_Cast_e_struct*
_tmp408=(struct Cyc_Absyn_Cast_e_struct*)_tmp407;if(_tmp408->tag != 15)goto _LL24A;
else{_tmp409=_tmp408->f2;}}_LL249: _tmp40B=_tmp409;goto _LL24B;_LL24A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp40A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp407;if(_tmp40A->tag != 13)
goto _LL24C;else{_tmp40B=_tmp40A->f1;}}_LL24B: _tmp40D=_tmp40B;goto _LL24D;_LL24C: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp40C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp407;if(_tmp40C->tag != 14)goto _LL24E;else{_tmp40D=_tmp40C->f1;}}_LL24D: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp40D,flds);_LL24E: {
struct Cyc_Absyn_Var_e_struct*_tmp40E=(struct Cyc_Absyn_Var_e_struct*)_tmp407;if(
_tmp40E->tag != 1)goto _LL250;else{_tmp40F=(void*)_tmp40E->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp410=(struct Cyc_Absyn_Param_b_struct*)_tmp40F;if(_tmp410->tag != 3)goto _LL250;
else{_tmp411=_tmp410->f1;}};}}_LL24F: _tmp415=_tmp411;goto _LL251;_LL250: {struct
Cyc_Absyn_Var_e_struct*_tmp412=(struct Cyc_Absyn_Var_e_struct*)_tmp407;if(_tmp412->tag
!= 1)goto _LL252;else{_tmp413=(void*)_tmp412->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp414=(struct Cyc_Absyn_Local_b_struct*)_tmp413;if(_tmp414->tag != 4)goto _LL252;
else{_tmp415=_tmp414->f1;}};}}_LL251: _tmp419=_tmp415;goto _LL253;_LL252: {struct
Cyc_Absyn_Var_e_struct*_tmp416=(struct Cyc_Absyn_Var_e_struct*)_tmp407;if(_tmp416->tag
!= 1)goto _LL254;else{_tmp417=(void*)_tmp416->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp418=(struct Cyc_Absyn_Pat_b_struct*)_tmp417;if(_tmp418->tag != 5)goto _LL254;
else{_tmp419=_tmp418->f1;}};}}_LL253: {struct Cyc_CfFlowInfo_Place*_tmp8A2;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8A1;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8A0;
struct _tuple14 _tmp89F;return(_tmp89F.f1=inflow,((_tmp89F.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8A2=_region_malloc(env->r,sizeof(*_tmp8A2)),((_tmp8A2->root=(void*)((_tmp8A0=
_region_malloc(env->r,sizeof(*_tmp8A0)),((_tmp8A0[0]=((_tmp8A1.tag=0,((_tmp8A1.f1=
_tmp419,_tmp8A1)))),_tmp8A0)))),((_tmp8A2->fields=flds,_tmp8A2))))))),_tmp89F)));}
_LL254: {struct Cyc_Absyn_Var_e_struct*_tmp41A=(struct Cyc_Absyn_Var_e_struct*)
_tmp407;if(_tmp41A->tag != 1)goto _LL256;else{_tmp41B=(void*)_tmp41A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp41C=(struct Cyc_Absyn_Global_b_struct*)_tmp41B;if(
_tmp41C->tag != 1)goto _LL256;else{_tmp41D=_tmp41C->f1;}};}}_LL255: {struct
_tuple14 _tmp8A3;return(_tmp8A3.f1=inflow,((_tmp8A3.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8A3)));}_LL256: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp41E=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp407;if(_tmp41E->tag != 23)goto _LL258;else{_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;}}
_LL257:{void*_tmp42E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp41F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp430;void*_tmp431;
_LL261: {struct Cyc_Absyn_PointerType_struct*_tmp42F=(struct Cyc_Absyn_PointerType_struct*)
_tmp42E;if(_tmp42F->tag != 5)goto _LL263;else{_tmp430=_tmp42F->f1;_tmp431=_tmp430.elt_typ;}}
_LL262: if(!Cyc_Absyn_is_nontagged_union_type(_tmp431)){struct Cyc_List_List*
_tmp8A4;flds=((_tmp8A4=_region_malloc(env->r,sizeof(*_tmp8A4)),((_tmp8A4->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp431,_tmp420),((_tmp8A4->tl=flds,_tmp8A4))))));}
goto _LL260;_LL263:;_LL264: {struct Cyc_Core_Impossible_struct _tmp8AA;const char*
_tmp8A9;struct Cyc_Core_Impossible_struct*_tmp8A8;(int)_throw((void*)((_tmp8A8=
_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8AA.tag=Cyc_Core_Impossible,((
_tmp8AA.f1=((_tmp8A9="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8A9,sizeof(
char),25))),_tmp8AA)))),_tmp8A8)))));}_LL260:;}_tmp422=_tmp41F;goto _LL259;_LL258: {
struct Cyc_Absyn_Deref_e_struct*_tmp421=(struct Cyc_Absyn_Deref_e_struct*)_tmp407;
if(_tmp421->tag != 21)goto _LL25A;else{_tmp422=_tmp421->f1;}}_LL259: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp422->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp437;union Cyc_CfFlowInfo_AbsLVal
_tmp438;struct _tuple14 _tmp436=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp422);
_tmp437=_tmp436.f1;_tmp438=_tmp436.f2;{struct _tuple14 _tmp8AB;struct _tuple14
_tmp43A=(_tmp8AB.f1=_tmp437,((_tmp8AB.f2=_tmp438,_tmp8AB)));union Cyc_CfFlowInfo_FlowInfo
_tmp43B;struct _tuple12 _tmp43C;struct Cyc_Dict_Dict _tmp43D;struct Cyc_List_List*
_tmp43E;union Cyc_CfFlowInfo_AbsLVal _tmp43F;struct Cyc_CfFlowInfo_Place*_tmp440;
_LL266: _tmp43B=_tmp43A.f1;if((_tmp43B.ReachableFL).tag != 2)goto _LL268;_tmp43C=(
struct _tuple12)(_tmp43B.ReachableFL).val;_tmp43D=_tmp43C.f1;_tmp43E=_tmp43C.f2;
_tmp43F=_tmp43A.f2;if((_tmp43F.PlaceL).tag != 1)goto _LL268;_tmp440=(struct Cyc_CfFlowInfo_Place*)(
_tmp43F.PlaceL).val;_LL267: {void*_tmp441=Cyc_CfFlowInfo_lookup_place(_tmp43D,
_tmp440);void*_tmp442=_tmp441;_LL26B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp443=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp442;if(_tmp443->tag != 5)goto
_LL26D;}_LL26C: return Cyc_NewControlFlow_anal_derefL(env,_tmp437,_tmp422,_tmp437,
_tmp441,flds);_LL26D:;_LL26E: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp400,_tmp43D,_tmp441);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp400->unknown_all:
_tmp400->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp400,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp422->topt))->v),leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8AE;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8AD;void*new_root=(void*)((
_tmp8AD=_region_malloc(_tmp400->r,sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AE.tag=1,((
_tmp8AE.f1=e,((_tmp8AE.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8AE)))))),_tmp8AD))));struct Cyc_CfFlowInfo_Place*_tmp8AF;struct Cyc_CfFlowInfo_Place*
place=(_tmp8AF=_region_malloc(_tmp400->r,sizeof(*_tmp8AF)),((_tmp8AF->root=
new_root,((_tmp8AF->fields=0,_tmp8AF)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp8B2;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8B1;void*res=(void*)((_tmp8B1=
_region_malloc(_tmp400->r,sizeof(*_tmp8B1)),((_tmp8B1[0]=((_tmp8B2.tag=5,((
_tmp8B2.f1=place,_tmp8B2)))),_tmp8B1))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp43D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp43D,new_root,new_rval);_tmp43D=Cyc_CfFlowInfo_assign_place(
_tmp400,e->loc,_tmp43D,env->all_changed,_tmp440,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp444=Cyc_CfFlowInfo_ReachableFL(_tmp43D,_tmp43E);return Cyc_NewControlFlow_anal_derefL(
env,_tmp444,_tmp422,_tmp444,res,flds);};}_LL26A:;}_LL268:;_LL269: goto _LL265;
_LL265:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp44B;void*_tmp44C;struct _tuple13
_tmp44A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp422);_tmp44B=_tmp44A.f1;
_tmp44C=_tmp44A.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp422,
_tmp44B,_tmp44C,flds);};_LL25A: {struct Cyc_Absyn_Subscript_e_struct*_tmp423=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp407;if(_tmp423->tag != 24)goto _LL25C;
else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}_LL25B: {void*_tmp44D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp424->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp450;struct Cyc_Absyn_PtrAtts _tmp451;union Cyc_Absyn_Constraint*_tmp452;_LL270: {
struct Cyc_Absyn_TupleType_struct*_tmp44E=(struct Cyc_Absyn_TupleType_struct*)
_tmp44D;if(_tmp44E->tag != 11)goto _LL272;}_LL271: {unsigned int _tmp453=(Cyc_Evexp_eval_const_uint_exp(
_tmp425)).f1;struct Cyc_List_List*_tmp8B3;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp424,((_tmp8B3=_region_malloc(env->r,sizeof(*_tmp8B3)),((
_tmp8B3->hd=(void*)((int)_tmp453),((_tmp8B3->tl=flds,_tmp8B3)))))));}_LL272: {
struct Cyc_Absyn_PointerType_struct*_tmp44F=(struct Cyc_Absyn_PointerType_struct*)
_tmp44D;if(_tmp44F->tag != 5)goto _LL274;else{_tmp450=_tmp44F->f1;_tmp451=_tmp450.ptr_atts;
_tmp452=_tmp451.bounds;}}_LL273: {struct _RegionHandle*_tmp455=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp458;struct Cyc_List_List*_tmp459;struct Cyc_Absyn_Exp*_tmp8B4[2];struct
_tuple19 _tmp457=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp455,env,inflow,((
_tmp8B4[1]=_tmp425,((_tmp8B4[0]=_tmp424,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp455,_tag_dyneither(
_tmp8B4,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp458=_tmp457.f1;_tmp459=
_tmp457.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp45A=_tmp458;{union Cyc_CfFlowInfo_FlowInfo
_tmp45B=_tmp458;struct _tuple12 _tmp45C;struct Cyc_Dict_Dict _tmp45D;struct Cyc_List_List*
_tmp45E;_LL277: if((_tmp45B.ReachableFL).tag != 2)goto _LL279;_tmp45C=(struct
_tuple12)(_tmp45B.ReachableFL).val;_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;_LL278:
if(Cyc_CfFlowInfo_initlevel(_tmp400,_tmp45D,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp459))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,Cyc_Tcutil_terr(_tmp425->loc,((_tmp8B7="expression may not be initialized",
_tag_dyneither(_tmp8B7,sizeof(char),34))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp461=Cyc_NewControlFlow_add_subscript_reln(_tmp400->r,
_tmp45E,_tmp424,_tmp425);if(_tmp45E != _tmp461)_tmp45A=Cyc_CfFlowInfo_ReachableFL(
_tmp45D,_tmp461);goto _LL276;};_LL279:;_LL27A: goto _LL276;_LL276:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp463;union Cyc_CfFlowInfo_AbsLVal _tmp464;struct _tuple14 _tmp462=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp424,_tmp458,(void*)((struct Cyc_List_List*)_check_null(_tmp459))->hd,
flds);_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp465=
_tmp463;int _tmp466;_LL27C: if((_tmp465.BottomFL).tag != 1)goto _LL27E;_tmp466=(int)(
_tmp465.BottomFL).val;_LL27D: {struct _tuple14 _tmp8B8;return(_tmp8B8.f1=_tmp463,((
_tmp8B8.f2=_tmp464,_tmp8B8)));}_LL27E:;_LL27F: {struct _tuple14 _tmp8B9;return(
_tmp8B9.f1=_tmp45A,((_tmp8B9.f2=_tmp464,_tmp8B9)));}_LL27B:;};};};}_LL274:;
_LL275: {struct Cyc_Core_Impossible_struct _tmp8BF;const char*_tmp8BE;struct Cyc_Core_Impossible_struct*
_tmp8BD;(int)_throw((void*)((_tmp8BD=_cycalloc(sizeof(*_tmp8BD)),((_tmp8BD[0]=((
_tmp8BF.tag=Cyc_Core_Impossible,((_tmp8BF.f1=((_tmp8BE="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp8BE,sizeof(char),33))),_tmp8BF)))),_tmp8BD)))));}_LL26F:;}
_LL25C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp426=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp407;if(_tmp426->tag != 22)goto _LL25E;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}
_LL25D: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp427->topt))->v)){struct _tuple14 _tmp8C0;return(_tmp8C0.f1=inflow,((
_tmp8C0.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C0)));}{struct Cyc_List_List*_tmp8C1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp427,((
_tmp8C1=_region_malloc(env->r,sizeof(*_tmp8C1)),((_tmp8C1->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp427->topt))->v,_tmp428),((_tmp8C1->tl=
flds,_tmp8C1)))))));};_LL25E:;_LL25F: {struct _tuple14 _tmp8C2;return(_tmp8C2.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp8C2.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C2)));}
_LL247:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp470;union Cyc_CfFlowInfo_AbsLVal _tmp471;struct
_tuple14 _tmp46F=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{struct _tuple14 _tmp8C3;return(_tmp8C3.f1=
_tmp470,((_tmp8C3.f2=_tmp471,_tmp8C3)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp473=env->fenv;{void*_tmp474=e->r;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Exp*
_tmp477;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_Absyn_Exp*
_tmp47B;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*
_tmp480;struct Cyc_Absyn_Exp*_tmp481;enum Cyc_Absyn_Primop _tmp483;struct Cyc_List_List*
_tmp484;struct Cyc_List_List _tmp485;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_List_List*
_tmp487;enum Cyc_Absyn_Primop _tmp489;struct Cyc_List_List*_tmp48A;_LL281: {struct
Cyc_Absyn_Conditional_e_struct*_tmp475=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp474;if(_tmp475->tag != 6)goto _LL283;else{_tmp476=_tmp475->f1;_tmp477=_tmp475->f2;
_tmp478=_tmp475->f3;}}_LL282: {union Cyc_CfFlowInfo_FlowInfo _tmp48C;union Cyc_CfFlowInfo_FlowInfo
_tmp48D;struct _tuple15 _tmp48B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp476);
_tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp48F;union
Cyc_CfFlowInfo_FlowInfo _tmp490;struct _tuple15 _tmp48E=Cyc_NewControlFlow_anal_test(
env,_tmp48C,_tmp477);_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp492;union Cyc_CfFlowInfo_FlowInfo _tmp493;struct _tuple15 _tmp491=Cyc_NewControlFlow_anal_test(
env,_tmp48D,_tmp478);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{struct _tuple15
_tmp8C4;return(_tmp8C4.f1=Cyc_CfFlowInfo_join_flow(_tmp473,env->all_changed,
_tmp48F,_tmp492),((_tmp8C4.f2=Cyc_CfFlowInfo_join_flow(_tmp473,env->all_changed,
_tmp490,_tmp493),_tmp8C4)));};};};}_LL283: {struct Cyc_Absyn_And_e_struct*_tmp479=(
struct Cyc_Absyn_And_e_struct*)_tmp474;if(_tmp479->tag != 7)goto _LL285;else{
_tmp47A=_tmp479->f1;_tmp47B=_tmp479->f2;}}_LL284: {union Cyc_CfFlowInfo_FlowInfo
_tmp496;union Cyc_CfFlowInfo_FlowInfo _tmp497;struct _tuple15 _tmp495=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47A);_tmp496=_tmp495.f1;_tmp497=_tmp495.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;struct _tuple15 _tmp498=Cyc_NewControlFlow_anal_test(
env,_tmp496,_tmp47B);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{struct _tuple15
_tmp8C5;return(_tmp8C5.f1=_tmp499,((_tmp8C5.f2=Cyc_CfFlowInfo_join_flow(_tmp473,
env->all_changed,_tmp497,_tmp49A),_tmp8C5)));};};}_LL285: {struct Cyc_Absyn_Or_e_struct*
_tmp47C=(struct Cyc_Absyn_Or_e_struct*)_tmp474;if(_tmp47C->tag != 8)goto _LL287;
else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp49D;union Cyc_CfFlowInfo_FlowInfo _tmp49E;struct _tuple15 _tmp49C=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47D);_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp4A1;struct _tuple15 _tmp49F=Cyc_NewControlFlow_anal_test(
env,_tmp49E,_tmp47E);_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;{struct _tuple15
_tmp8C6;return(_tmp8C6.f1=Cyc_CfFlowInfo_join_flow(_tmp473,env->all_changed,
_tmp49D,_tmp4A0),((_tmp8C6.f2=_tmp4A1,_tmp8C6)));};};}_LL287: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp47F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp474;if(_tmp47F->tag != 9)goto _LL289;
else{_tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;}}_LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp4A4;void*_tmp4A5;struct _tuple13 _tmp4A3=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp480);_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4A4,_tmp481);}_LL289: {struct Cyc_Absyn_Primop_e_struct*_tmp482=(struct
Cyc_Absyn_Primop_e_struct*)_tmp474;if(_tmp482->tag != 3)goto _LL28B;else{_tmp483=
_tmp482->f1;if(_tmp483 != Cyc_Absyn_Not)goto _LL28B;_tmp484=_tmp482->f2;if(_tmp484
== 0)goto _LL28B;_tmp485=*_tmp484;_tmp486=(struct Cyc_Absyn_Exp*)_tmp485.hd;
_tmp487=_tmp485.tl;if(_tmp487 != 0)goto _LL28B;}}_LL28A: {union Cyc_CfFlowInfo_FlowInfo
_tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;struct _tuple15 _tmp4A6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp486);_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{struct _tuple15 _tmp8C7;
return(_tmp8C7.f1=_tmp4A8,((_tmp8C7.f2=_tmp4A7,_tmp8C7)));};}_LL28B: {struct Cyc_Absyn_Primop_e_struct*
_tmp488=(struct Cyc_Absyn_Primop_e_struct*)_tmp474;if(_tmp488->tag != 3)goto _LL28D;
else{_tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;}}_LL28C: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4AA=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4AC;struct
Cyc_List_List*_tmp4AD;struct _tuple19 _tmp4AB=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4AA,env,inflow,_tmp48A,0,0);_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4AD))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4AD->tl))->hd;f=_tmp4AC;}{union Cyc_CfFlowInfo_FlowInfo _tmp4AE=f;
int _tmp4AF;struct _tuple12 _tmp4B0;struct Cyc_Dict_Dict _tmp4B1;struct Cyc_List_List*
_tmp4B2;_LL290: if((_tmp4AE.BottomFL).tag != 1)goto _LL292;_tmp4AF=(int)(_tmp4AE.BottomFL).val;
_LL291: {struct _tuple15 _tmp8C8;return(_tmp8C8.f1=f,((_tmp8C8.f2=f,_tmp8C8)));}
_LL292: if((_tmp4AE.ReachableFL).tag != 2)goto _LL28F;_tmp4B0=(struct _tuple12)(
_tmp4AE.ReachableFL).val;_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;_LL293: {struct
Cyc_Absyn_Exp*_tmp4B4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48A))->hd;struct Cyc_Absyn_Exp*_tmp4B5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48A->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B1,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8CB;void*_tmp8CA;(_tmp8CA=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp48A->hd)->loc,((_tmp8CB="expression may not be initialized",
_tag_dyneither(_tmp8CB,sizeof(char),34))),_tag_dyneither(_tmp8CA,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B1,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp8CE;void*_tmp8CD;(_tmp8CD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48A->tl))->hd)->loc,((_tmp8CE="expression may not be initialized",
_tag_dyneither(_tmp8CE,sizeof(char),34))),_tag_dyneither(_tmp8CD,sizeof(void*),0)));}
if(_tmp489 == Cyc_Absyn_Eq  || _tmp489 == Cyc_Absyn_Neq){struct _tuple0 _tmp8CF;
struct _tuple0 _tmp4BB=(_tmp8CF.f1=r1,((_tmp8CF.f2=r2,_tmp8CF)));void*_tmp4BC;
enum Cyc_CfFlowInfo_InitLevel _tmp4BE;void*_tmp4BF;void*_tmp4C1;void*_tmp4C3;
enum Cyc_CfFlowInfo_InitLevel _tmp4C5;void*_tmp4C6;void*_tmp4C8;void*_tmp4CA;void*
_tmp4CC;void*_tmp4CE;void*_tmp4D0;void*_tmp4D2;void*_tmp4D4;void*_tmp4D6;void*
_tmp4D8;void*_tmp4DA;void*_tmp4DC;void*_tmp4DE;void*_tmp4E0;_LL295: _tmp4BC=
_tmp4BB.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4BD=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4BC;if(_tmp4BD->tag != 3)goto _LL297;else{_tmp4BE=_tmp4BD->f1;}};_tmp4BF=
_tmp4BB.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4BF;if(_tmp4C0->tag != 0)goto _LL297;};_LL296: {union Cyc_CfFlowInfo_FlowInfo
_tmp4E3;union Cyc_CfFlowInfo_FlowInfo _tmp4E4;struct _tuple15 _tmp4E2=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4B4,_tmp4BE);_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;switch(_tmp489){
case Cyc_Absyn_Eq: _LL2A9: {struct _tuple15 _tmp8D0;return(_tmp8D0.f1=_tmp4E4,((
_tmp8D0.f2=_tmp4E3,_tmp8D0)));}case Cyc_Absyn_Neq: _LL2AA: {struct _tuple15 _tmp8D1;
return(_tmp8D1.f1=_tmp4E3,((_tmp8D1.f2=_tmp4E4,_tmp8D1)));}default: _LL2AB: {
struct Cyc_Core_Impossible_struct _tmp8D7;const char*_tmp8D6;struct Cyc_Core_Impossible_struct*
_tmp8D5;(int)_throw((void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((
_tmp8D7.tag=Cyc_Core_Impossible,((_tmp8D7.f1=((_tmp8D6="anal_test, zero-split",
_tag_dyneither(_tmp8D6,sizeof(char),22))),_tmp8D7)))),_tmp8D5)))));}}}_LL297:
_tmp4C1=_tmp4BB.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C2=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C1;if(_tmp4C2->tag != 0)goto _LL299;};_tmp4C3=_tmp4BB.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4C4=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4C3;if(_tmp4C4->tag != 3)goto
_LL299;else{_tmp4C5=_tmp4C4->f1;}};_LL298: {union Cyc_CfFlowInfo_FlowInfo _tmp4EB;
union Cyc_CfFlowInfo_FlowInfo _tmp4EC;struct _tuple15 _tmp4EA=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4B5,_tmp4C5);_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;switch(_tmp489){
case Cyc_Absyn_Eq: _LL2AD: {struct _tuple15 _tmp8D8;return(_tmp8D8.f1=_tmp4EC,((
_tmp8D8.f2=_tmp4EB,_tmp8D8)));}case Cyc_Absyn_Neq: _LL2AE: {struct _tuple15 _tmp8D9;
return(_tmp8D9.f1=_tmp4EB,((_tmp8D9.f2=_tmp4EC,_tmp8D9)));}default: _LL2AF: {
struct Cyc_Core_Impossible_struct _tmp8DF;const char*_tmp8DE;struct Cyc_Core_Impossible_struct*
_tmp8DD;(int)_throw((void*)((_tmp8DD=_cycalloc(sizeof(*_tmp8DD)),((_tmp8DD[0]=((
_tmp8DF.tag=Cyc_Core_Impossible,((_tmp8DF.f1=((_tmp8DE="anal_test, zero-split",
_tag_dyneither(_tmp8DE,sizeof(char),22))),_tmp8DF)))),_tmp8DD)))));}}}_LL299:
_tmp4C6=_tmp4BB.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C7=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C6;if(_tmp4C7->tag != 0)goto _LL29B;};_tmp4C8=_tmp4BB.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C9=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C8;if(_tmp4C9->tag != 0)goto
_LL29B;};_LL29A: if(_tmp489 == Cyc_Absyn_Eq){struct _tuple15 _tmp8E0;return(_tmp8E0.f1=
f,((_tmp8E0.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8E0)));}else{struct _tuple15 _tmp8E1;
return(_tmp8E1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E1.f2=f,_tmp8E1)));}_LL29B:
_tmp4CA=_tmp4BB.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4CB=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4CA;if(_tmp4CB->tag != 0)goto _LL29D;};_tmp4CC=_tmp4BB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4CD=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4CC;if(_tmp4CD->tag != 1)
goto _LL29D;};_LL29C: goto _LL29E;_LL29D: _tmp4CE=_tmp4BB.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4CF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4CE;if(_tmp4CF->tag != 0)goto
_LL29F;};_tmp4D0=_tmp4BB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4D1=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4D0;if(_tmp4D1->tag != 2)goto _LL29F;};
_LL29E: goto _LL2A0;_LL29F: _tmp4D2=_tmp4BB.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4D3=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D2;if(_tmp4D3->tag != 0)goto
_LL2A1;};_tmp4D4=_tmp4BB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4D5=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4D4;if(_tmp4D5->tag != 5)goto _LL2A1;};
_LL2A0: goto _LL2A2;_LL2A1: _tmp4D6=_tmp4BB.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4D7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4D6;if(_tmp4D7->tag != 1)
goto _LL2A3;};_tmp4D8=_tmp4BB.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D9=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D8;if(_tmp4D9->tag != 0)goto _LL2A3;};
_LL2A2: goto _LL2A4;_LL2A3: _tmp4DA=_tmp4BB.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4DB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4DA;if(_tmp4DB->tag != 2)
goto _LL2A5;};_tmp4DC=_tmp4BB.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DD=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4DC;if(_tmp4DD->tag != 0)goto _LL2A5;};
_LL2A4: goto _LL2A6;_LL2A5: _tmp4DE=_tmp4BB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4DF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4DE;if(_tmp4DF->tag != 5)goto
_LL2A7;};_tmp4E0=_tmp4BB.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E1=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E0;if(_tmp4E1->tag != 0)goto _LL2A7;};_LL2A6: if(_tmp489 == Cyc_Absyn_Neq){
struct _tuple15 _tmp8E2;return(_tmp8E2.f1=f,((_tmp8E2.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8E2)));}else{struct _tuple15 _tmp8E3;return(_tmp8E3.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8E3.f2=f,_tmp8E3)));}_LL2A7:;_LL2A8: goto _LL294;_LL294:;}{struct _tuple0
_tmp8E4;struct _tuple0 _tmp4F7=(_tmp8E4.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B4->topt))->v),((_tmp8E4.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4B5->topt))->v),_tmp8E4)));void*_tmp4F8;enum Cyc_Absyn_Sign
_tmp4FA;void*_tmp4FB;enum Cyc_Absyn_Sign _tmp4FD;void*_tmp4FE;void*_tmp500;_LL2B2:
_tmp4F8=_tmp4F7.f1;{struct Cyc_Absyn_IntType_struct*_tmp4F9=(struct Cyc_Absyn_IntType_struct*)
_tmp4F8;if(_tmp4F9->tag != 6)goto _LL2B4;else{_tmp4FA=_tmp4F9->f1;if(_tmp4FA != Cyc_Absyn_Unsigned)
goto _LL2B4;}};_LL2B3: goto _LL2B5;_LL2B4: _tmp4FB=_tmp4F7.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4FC=(struct Cyc_Absyn_IntType_struct*)_tmp4FB;if(_tmp4FC->tag != 6)goto _LL2B6;
else{_tmp4FD=_tmp4FC->f1;if(_tmp4FD != Cyc_Absyn_Unsigned)goto _LL2B6;}};_LL2B5:
goto _LL2B7;_LL2B6: _tmp4FE=_tmp4F7.f1;{struct Cyc_Absyn_TagType_struct*_tmp4FF=(
struct Cyc_Absyn_TagType_struct*)_tmp4FE;if(_tmp4FF->tag != 20)goto _LL2B8;};_LL2B7:
goto _LL2B9;_LL2B8: _tmp500=_tmp4F7.f2;{struct Cyc_Absyn_TagType_struct*_tmp501=(
struct Cyc_Absyn_TagType_struct*)_tmp500;if(_tmp501->tag != 20)goto _LL2BA;};_LL2B9:
goto _LL2B1;_LL2BA:;_LL2BB: {struct _tuple15 _tmp8E5;return(_tmp8E5.f1=f,((_tmp8E5.f2=
f,_tmp8E5)));}_LL2B1:;}switch(_tmp489){case Cyc_Absyn_Eq: _LL2BC: goto _LL2BD;case
Cyc_Absyn_Neq: _LL2BD: goto _LL2BE;case Cyc_Absyn_Gt: _LL2BE: goto _LL2BF;case Cyc_Absyn_Gte:
_LL2BF: {struct _tuple15 _tmp8E6;return(_tmp8E6.f1=f,((_tmp8E6.f2=f,_tmp8E6)));}
case Cyc_Absyn_Lt: _LL2C0: {union Cyc_CfFlowInfo_FlowInfo _tmp504=f;union Cyc_CfFlowInfo_FlowInfo
_tmp505=f;{union Cyc_CfFlowInfo_FlowInfo _tmp506=_tmp504;int _tmp507;struct _tuple12
_tmp508;struct Cyc_Dict_Dict _tmp509;_LL2C3: if((_tmp506.BottomFL).tag != 1)goto
_LL2C5;_tmp507=(int)(_tmp506.BottomFL).val;_LL2C4: {struct Cyc_Core_Impossible_struct
_tmp8EC;const char*_tmp8EB;struct Cyc_Core_Impossible_struct*_tmp8EA;(int)_throw((
void*)((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EC.tag=Cyc_Core_Impossible,((
_tmp8EC.f1=((_tmp8EB="anal_test, Lt",_tag_dyneither(_tmp8EB,sizeof(char),14))),
_tmp8EC)))),_tmp8EA)))));}_LL2C5: if((_tmp506.ReachableFL).tag != 2)goto _LL2C2;
_tmp508=(struct _tuple12)(_tmp506.ReachableFL).val;_tmp509=_tmp508.f1;_LL2C6:
_tmp4B1=_tmp509;_LL2C2:;}{void*_tmp50D=_tmp4B4->r;void*_tmp50F;struct Cyc_Absyn_Vardecl*
_tmp511;void*_tmp513;struct Cyc_Absyn_Vardecl*_tmp515;void*_tmp517;struct Cyc_Absyn_Vardecl*
_tmp519;void*_tmp51B;struct Cyc_Absyn_Vardecl*_tmp51D;_LL2C8:{struct Cyc_Absyn_Var_e_struct*
_tmp50E=(struct Cyc_Absyn_Var_e_struct*)_tmp50D;if(_tmp50E->tag != 1)goto _LL2CA;
else{_tmp50F=(void*)_tmp50E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp510=(struct
Cyc_Absyn_Global_b_struct*)_tmp50F;if(_tmp510->tag != 1)goto _LL2CA;else{_tmp511=
_tmp510->f1;}};}}if(!(!_tmp511->escapes))goto _LL2CA;_LL2C9: _tmp515=_tmp511;goto
_LL2CB;_LL2CA:{struct Cyc_Absyn_Var_e_struct*_tmp512=(struct Cyc_Absyn_Var_e_struct*)
_tmp50D;if(_tmp512->tag != 1)goto _LL2CC;else{_tmp513=(void*)_tmp512->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp514=(struct Cyc_Absyn_Local_b_struct*)_tmp513;if(
_tmp514->tag != 4)goto _LL2CC;else{_tmp515=_tmp514->f1;}};}}if(!(!_tmp515->escapes))
goto _LL2CC;_LL2CB: _tmp519=_tmp515;goto _LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*
_tmp516=(struct Cyc_Absyn_Var_e_struct*)_tmp50D;if(_tmp516->tag != 1)goto _LL2CE;
else{_tmp517=(void*)_tmp516->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp518=(struct Cyc_Absyn_Pat_b_struct*)
_tmp517;if(_tmp518->tag != 5)goto _LL2CE;else{_tmp519=_tmp518->f1;}};}}if(!(!
_tmp519->escapes))goto _LL2CE;_LL2CD: _tmp51D=_tmp519;goto _LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*
_tmp51A=(struct Cyc_Absyn_Var_e_struct*)_tmp50D;if(_tmp51A->tag != 1)goto _LL2D0;
else{_tmp51B=(void*)_tmp51A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp51C=(struct
Cyc_Absyn_Param_b_struct*)_tmp51B;if(_tmp51C->tag != 3)goto _LL2D0;else{_tmp51D=
_tmp51C->f1;}};}}if(!(!_tmp51D->escapes))goto _LL2D0;_LL2CF: {void*_tmp51E=
_tmp4B5->r;void*_tmp520;struct Cyc_Absyn_Vardecl*_tmp522;void*_tmp524;struct Cyc_Absyn_Vardecl*
_tmp526;void*_tmp528;struct Cyc_Absyn_Vardecl*_tmp52A;void*_tmp52C;struct Cyc_Absyn_Vardecl*
_tmp52E;union Cyc_Absyn_Cnst _tmp530;struct _tuple6 _tmp531;int _tmp532;struct Cyc_Absyn_Exp*
_tmp534;struct Cyc_Absyn_Exp _tmp535;void*_tmp536;union Cyc_Absyn_Cnst _tmp538;
struct _tuple6 _tmp539;int _tmp53A;enum Cyc_Absyn_Primop _tmp53C;struct Cyc_List_List*
_tmp53D;struct Cyc_List_List _tmp53E;struct Cyc_Absyn_Exp*_tmp53F;_LL2D3:{struct Cyc_Absyn_Var_e_struct*
_tmp51F=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp51F->tag != 1)goto _LL2D5;
else{_tmp520=(void*)_tmp51F->f2;{struct Cyc_Absyn_Global_b_struct*_tmp521=(struct
Cyc_Absyn_Global_b_struct*)_tmp520;if(_tmp521->tag != 1)goto _LL2D5;else{_tmp522=
_tmp521->f1;}};}}if(!(!_tmp522->escapes))goto _LL2D5;_LL2D4: _tmp526=_tmp522;goto
_LL2D6;_LL2D5:{struct Cyc_Absyn_Var_e_struct*_tmp523=(struct Cyc_Absyn_Var_e_struct*)
_tmp51E;if(_tmp523->tag != 1)goto _LL2D7;else{_tmp524=(void*)_tmp523->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp525=(struct Cyc_Absyn_Local_b_struct*)_tmp524;if(
_tmp525->tag != 4)goto _LL2D7;else{_tmp526=_tmp525->f1;}};}}if(!(!_tmp526->escapes))
goto _LL2D7;_LL2D6: _tmp52A=_tmp526;goto _LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*
_tmp527=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp527->tag != 1)goto _LL2D9;
else{_tmp528=(void*)_tmp527->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp529=(struct Cyc_Absyn_Pat_b_struct*)
_tmp528;if(_tmp529->tag != 5)goto _LL2D9;else{_tmp52A=_tmp529->f1;}};}}if(!(!
_tmp52A->escapes))goto _LL2D9;_LL2D8: _tmp52E=_tmp52A;goto _LL2DA;_LL2D9:{struct Cyc_Absyn_Var_e_struct*
_tmp52B=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp52B->tag != 1)goto _LL2DB;
else{_tmp52C=(void*)_tmp52B->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52D=(struct
Cyc_Absyn_Param_b_struct*)_tmp52C;if(_tmp52D->tag != 3)goto _LL2DB;else{_tmp52E=
_tmp52D->f1;}};}}if(!(!_tmp52E->escapes))goto _LL2DB;_LL2DA: {struct _RegionHandle*
_tmp540=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8EF;struct Cyc_List_List*
_tmp8EE;struct Cyc_List_List*_tmp541=(_tmp8EE=_region_malloc(_tmp540,sizeof(*
_tmp8EE)),((_tmp8EE->hd=((_tmp8EF=_region_malloc(_tmp540,sizeof(*_tmp8EF)),((
_tmp8EF->vd=_tmp51D,((_tmp8EF->rop=Cyc_CfFlowInfo_LessVar(_tmp52E,_tmp52E->type),
_tmp8EF)))))),((_tmp8EE->tl=_tmp4B2,_tmp8EE)))));struct _tuple15 _tmp8F0;return(
_tmp8F0.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B1,_tmp541),((_tmp8F0.f2=_tmp505,
_tmp8F0)));}_LL2DB: {struct Cyc_Absyn_Const_e_struct*_tmp52F=(struct Cyc_Absyn_Const_e_struct*)
_tmp51E;if(_tmp52F->tag != 0)goto _LL2DD;else{_tmp530=_tmp52F->f1;if((_tmp530.Int_c).tag
!= 4)goto _LL2DD;_tmp531=(struct _tuple6)(_tmp530.Int_c).val;_tmp532=_tmp531.f2;}}
_LL2DC: _tmp53A=_tmp532;goto _LL2DE;_LL2DD: {struct Cyc_Absyn_Cast_e_struct*_tmp533=(
struct Cyc_Absyn_Cast_e_struct*)_tmp51E;if(_tmp533->tag != 15)goto _LL2DF;else{
_tmp534=_tmp533->f2;_tmp535=*_tmp534;_tmp536=_tmp535.r;{struct Cyc_Absyn_Const_e_struct*
_tmp537=(struct Cyc_Absyn_Const_e_struct*)_tmp536;if(_tmp537->tag != 0)goto _LL2DF;
else{_tmp538=_tmp537->f1;if((_tmp538.Int_c).tag != 4)goto _LL2DF;_tmp539=(struct
_tuple6)(_tmp538.Int_c).val;_tmp53A=_tmp539.f2;}};}}_LL2DE: {struct _RegionHandle*
_tmp545=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8F3;struct Cyc_List_List*
_tmp8F2;struct Cyc_List_List*_tmp546=(_tmp8F2=_region_malloc(_tmp545,sizeof(*
_tmp8F2)),((_tmp8F2->hd=((_tmp8F3=_region_malloc(_tmp545,sizeof(*_tmp8F3)),((
_tmp8F3->vd=_tmp51D,((_tmp8F3->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp53A),_tmp8F3)))))),((_tmp8F2->tl=_tmp4B2,_tmp8F2)))));struct _tuple15 _tmp8F4;
return(_tmp8F4.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B1,_tmp546),((_tmp8F4.f2=
_tmp505,_tmp8F4)));}_LL2DF: {struct Cyc_Absyn_Primop_e_struct*_tmp53B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp51E;if(_tmp53B->tag != 3)goto _LL2E1;else{_tmp53C=_tmp53B->f1;_tmp53D=_tmp53B->f2;
if(_tmp53D == 0)goto _LL2E1;_tmp53E=*_tmp53D;_tmp53F=(struct Cyc_Absyn_Exp*)_tmp53E.hd;}}
_LL2E0: {void*_tmp54A=_tmp53F->r;void*_tmp54C;struct Cyc_Absyn_Vardecl*_tmp54E;
void*_tmp550;struct Cyc_Absyn_Vardecl*_tmp552;void*_tmp554;struct Cyc_Absyn_Vardecl*
_tmp556;void*_tmp558;struct Cyc_Absyn_Vardecl*_tmp55A;_LL2E4:{struct Cyc_Absyn_Var_e_struct*
_tmp54B=(struct Cyc_Absyn_Var_e_struct*)_tmp54A;if(_tmp54B->tag != 1)goto _LL2E6;
else{_tmp54C=(void*)_tmp54B->f2;{struct Cyc_Absyn_Global_b_struct*_tmp54D=(struct
Cyc_Absyn_Global_b_struct*)_tmp54C;if(_tmp54D->tag != 1)goto _LL2E6;else{_tmp54E=
_tmp54D->f1;}};}}if(!(!_tmp54E->escapes))goto _LL2E6;_LL2E5: _tmp552=_tmp54E;goto
_LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_struct*_tmp54F=(struct Cyc_Absyn_Var_e_struct*)
_tmp54A;if(_tmp54F->tag != 1)goto _LL2E8;else{_tmp550=(void*)_tmp54F->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp551=(struct Cyc_Absyn_Local_b_struct*)_tmp550;if(
_tmp551->tag != 4)goto _LL2E8;else{_tmp552=_tmp551->f1;}};}}if(!(!_tmp552->escapes))
goto _LL2E8;_LL2E7: _tmp556=_tmp552;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_struct*
_tmp553=(struct Cyc_Absyn_Var_e_struct*)_tmp54A;if(_tmp553->tag != 1)goto _LL2EA;
else{_tmp554=(void*)_tmp553->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp555=(struct Cyc_Absyn_Pat_b_struct*)
_tmp554;if(_tmp555->tag != 5)goto _LL2EA;else{_tmp556=_tmp555->f1;}};}}if(!(!
_tmp556->escapes))goto _LL2EA;_LL2E9: _tmp55A=_tmp556;goto _LL2EB;_LL2EA:{struct Cyc_Absyn_Var_e_struct*
_tmp557=(struct Cyc_Absyn_Var_e_struct*)_tmp54A;if(_tmp557->tag != 1)goto _LL2EC;
else{_tmp558=(void*)_tmp557->f2;{struct Cyc_Absyn_Param_b_struct*_tmp559=(struct
Cyc_Absyn_Param_b_struct*)_tmp558;if(_tmp559->tag != 3)goto _LL2EC;else{_tmp55A=
_tmp559->f1;}};}}if(!(!_tmp55A->escapes))goto _LL2EC;_LL2EB: {struct _RegionHandle*
_tmp55B=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8F7;struct Cyc_List_List*
_tmp8F6;struct Cyc_List_List*_tmp55C=(_tmp8F6=_region_malloc(_tmp55B,sizeof(*
_tmp8F6)),((_tmp8F6->hd=((_tmp8F7=_region_malloc(_tmp55B,sizeof(*_tmp8F7)),((
_tmp8F7->vd=_tmp51D,((_tmp8F7->rop=Cyc_CfFlowInfo_LessNumelts(_tmp55A),_tmp8F7)))))),((
_tmp8F6->tl=_tmp4B2,_tmp8F6)))));struct _tuple15 _tmp8F8;return(_tmp8F8.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4B1,_tmp55C),((_tmp8F8.f2=_tmp505,_tmp8F8)));}_LL2EC:;_LL2ED: {struct
_tuple15 _tmp8F9;return(_tmp8F9.f1=_tmp504,((_tmp8F9.f2=_tmp505,_tmp8F9)));}
_LL2E3:;}_LL2E1:;_LL2E2: {struct _tuple15 _tmp8FA;return(_tmp8FA.f1=_tmp504,((
_tmp8FA.f2=_tmp505,_tmp8FA)));}_LL2D2:;}_LL2D0:;_LL2D1: {struct _tuple15 _tmp8FB;
return(_tmp8FB.f1=_tmp504,((_tmp8FB.f2=_tmp505,_tmp8FB)));}_LL2C7:;};}case Cyc_Absyn_Lte:
_LL2C1: {union Cyc_CfFlowInfo_FlowInfo _tmp563=f;union Cyc_CfFlowInfo_FlowInfo
_tmp564=f;{union Cyc_CfFlowInfo_FlowInfo _tmp565=_tmp563;int _tmp566;struct _tuple12
_tmp567;struct Cyc_Dict_Dict _tmp568;_LL2F0: if((_tmp565.BottomFL).tag != 1)goto
_LL2F2;_tmp566=(int)(_tmp565.BottomFL).val;_LL2F1: {struct Cyc_Core_Impossible_struct
_tmp901;const char*_tmp900;struct Cyc_Core_Impossible_struct*_tmp8FF;(int)_throw((
void*)((_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp901.tag=Cyc_Core_Impossible,((
_tmp901.f1=((_tmp900="anal_test, Lte",_tag_dyneither(_tmp900,sizeof(char),15))),
_tmp901)))),_tmp8FF)))));}_LL2F2: if((_tmp565.ReachableFL).tag != 2)goto _LL2EF;
_tmp567=(struct _tuple12)(_tmp565.ReachableFL).val;_tmp568=_tmp567.f1;_LL2F3:
_tmp4B1=_tmp568;_LL2EF:;}{void*_tmp56C=_tmp4B4->r;void*_tmp56E;struct Cyc_Absyn_Vardecl*
_tmp570;void*_tmp572;struct Cyc_Absyn_Vardecl*_tmp574;void*_tmp576;struct Cyc_Absyn_Vardecl*
_tmp578;void*_tmp57A;struct Cyc_Absyn_Vardecl*_tmp57C;_LL2F5:{struct Cyc_Absyn_Var_e_struct*
_tmp56D=(struct Cyc_Absyn_Var_e_struct*)_tmp56C;if(_tmp56D->tag != 1)goto _LL2F7;
else{_tmp56E=(void*)_tmp56D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp56F=(struct
Cyc_Absyn_Global_b_struct*)_tmp56E;if(_tmp56F->tag != 1)goto _LL2F7;else{_tmp570=
_tmp56F->f1;}};}}if(!(!_tmp570->escapes))goto _LL2F7;_LL2F6: _tmp574=_tmp570;goto
_LL2F8;_LL2F7:{struct Cyc_Absyn_Var_e_struct*_tmp571=(struct Cyc_Absyn_Var_e_struct*)
_tmp56C;if(_tmp571->tag != 1)goto _LL2F9;else{_tmp572=(void*)_tmp571->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp573=(struct Cyc_Absyn_Local_b_struct*)_tmp572;if(
_tmp573->tag != 4)goto _LL2F9;else{_tmp574=_tmp573->f1;}};}}if(!(!_tmp574->escapes))
goto _LL2F9;_LL2F8: _tmp578=_tmp574;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_struct*
_tmp575=(struct Cyc_Absyn_Var_e_struct*)_tmp56C;if(_tmp575->tag != 1)goto _LL2FB;
else{_tmp576=(void*)_tmp575->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp577=(struct Cyc_Absyn_Pat_b_struct*)
_tmp576;if(_tmp577->tag != 5)goto _LL2FB;else{_tmp578=_tmp577->f1;}};}}if(!(!
_tmp578->escapes))goto _LL2FB;_LL2FA: _tmp57C=_tmp578;goto _LL2FC;_LL2FB:{struct Cyc_Absyn_Var_e_struct*
_tmp579=(struct Cyc_Absyn_Var_e_struct*)_tmp56C;if(_tmp579->tag != 1)goto _LL2FD;
else{_tmp57A=(void*)_tmp579->f2;{struct Cyc_Absyn_Param_b_struct*_tmp57B=(struct
Cyc_Absyn_Param_b_struct*)_tmp57A;if(_tmp57B->tag != 3)goto _LL2FD;else{_tmp57C=
_tmp57B->f1;}};}}if(!(!_tmp57C->escapes))goto _LL2FD;_LL2FC: {void*_tmp57D=
_tmp4B5->r;union Cyc_Absyn_Cnst _tmp57F;struct _tuple6 _tmp580;int _tmp581;struct Cyc_Absyn_Exp*
_tmp583;struct Cyc_Absyn_Exp _tmp584;void*_tmp585;union Cyc_Absyn_Cnst _tmp587;
struct _tuple6 _tmp588;int _tmp589;enum Cyc_Absyn_Primop _tmp58B;struct Cyc_List_List*
_tmp58C;struct Cyc_List_List _tmp58D;struct Cyc_Absyn_Exp*_tmp58E;_LL300: {struct
Cyc_Absyn_Const_e_struct*_tmp57E=(struct Cyc_Absyn_Const_e_struct*)_tmp57D;if(
_tmp57E->tag != 0)goto _LL302;else{_tmp57F=_tmp57E->f1;if((_tmp57F.Int_c).tag != 4)
goto _LL302;_tmp580=(struct _tuple6)(_tmp57F.Int_c).val;_tmp581=_tmp580.f2;}}
_LL301: _tmp589=_tmp581;goto _LL303;_LL302: {struct Cyc_Absyn_Cast_e_struct*_tmp582=(
struct Cyc_Absyn_Cast_e_struct*)_tmp57D;if(_tmp582->tag != 15)goto _LL304;else{
_tmp583=_tmp582->f2;_tmp584=*_tmp583;_tmp585=_tmp584.r;{struct Cyc_Absyn_Const_e_struct*
_tmp586=(struct Cyc_Absyn_Const_e_struct*)_tmp585;if(_tmp586->tag != 0)goto _LL304;
else{_tmp587=_tmp586->f1;if((_tmp587.Int_c).tag != 4)goto _LL304;_tmp588=(struct
_tuple6)(_tmp587.Int_c).val;_tmp589=_tmp588.f2;}};}}_LL303: {struct _RegionHandle*
_tmp58F=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp904;struct Cyc_List_List*
_tmp903;struct Cyc_List_List*_tmp590=(_tmp903=_region_malloc(_tmp58F,sizeof(*
_tmp903)),((_tmp903->hd=((_tmp904=_region_malloc(_tmp58F,sizeof(*_tmp904)),((
_tmp904->vd=_tmp57C,((_tmp904->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp589),_tmp904)))))),((_tmp903->tl=_tmp4B2,_tmp903)))));struct _tuple15 _tmp905;
return(_tmp905.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B1,_tmp590),((_tmp905.f2=
_tmp564,_tmp905)));}_LL304: {struct Cyc_Absyn_Primop_e_struct*_tmp58A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp57D;if(_tmp58A->tag != 3)goto _LL306;else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58A->f2;
if(_tmp58C == 0)goto _LL306;_tmp58D=*_tmp58C;_tmp58E=(struct Cyc_Absyn_Exp*)_tmp58D.hd;}}
_LL305: {void*_tmp594=_tmp58E->r;void*_tmp596;struct Cyc_Absyn_Vardecl*_tmp598;
void*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp59C;void*_tmp59E;struct Cyc_Absyn_Vardecl*
_tmp5A0;void*_tmp5A2;struct Cyc_Absyn_Vardecl*_tmp5A4;_LL309:{struct Cyc_Absyn_Var_e_struct*
_tmp595=(struct Cyc_Absyn_Var_e_struct*)_tmp594;if(_tmp595->tag != 1)goto _LL30B;
else{_tmp596=(void*)_tmp595->f2;{struct Cyc_Absyn_Global_b_struct*_tmp597=(struct
Cyc_Absyn_Global_b_struct*)_tmp596;if(_tmp597->tag != 1)goto _LL30B;else{_tmp598=
_tmp597->f1;}};}}if(!(!_tmp598->escapes))goto _LL30B;_LL30A: _tmp59C=_tmp598;goto
_LL30C;_LL30B:{struct Cyc_Absyn_Var_e_struct*_tmp599=(struct Cyc_Absyn_Var_e_struct*)
_tmp594;if(_tmp599->tag != 1)goto _LL30D;else{_tmp59A=(void*)_tmp599->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp59B=(struct Cyc_Absyn_Local_b_struct*)_tmp59A;if(
_tmp59B->tag != 4)goto _LL30D;else{_tmp59C=_tmp59B->f1;}};}}if(!(!_tmp59C->escapes))
goto _LL30D;_LL30C: _tmp5A0=_tmp59C;goto _LL30E;_LL30D:{struct Cyc_Absyn_Var_e_struct*
_tmp59D=(struct Cyc_Absyn_Var_e_struct*)_tmp594;if(_tmp59D->tag != 1)goto _LL30F;
else{_tmp59E=(void*)_tmp59D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp59F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp59E;if(_tmp59F->tag != 5)goto _LL30F;else{_tmp5A0=_tmp59F->f1;}};}}if(!(!
_tmp5A0->escapes))goto _LL30F;_LL30E: _tmp5A4=_tmp5A0;goto _LL310;_LL30F:{struct Cyc_Absyn_Var_e_struct*
_tmp5A1=(struct Cyc_Absyn_Var_e_struct*)_tmp594;if(_tmp5A1->tag != 1)goto _LL311;
else{_tmp5A2=(void*)_tmp5A1->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5A3=(struct
Cyc_Absyn_Param_b_struct*)_tmp5A2;if(_tmp5A3->tag != 3)goto _LL311;else{_tmp5A4=
_tmp5A3->f1;}};}}if(!(!_tmp5A4->escapes))goto _LL311;_LL310: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5A5=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp908;struct Cyc_List_List*_tmp907;
struct Cyc_List_List*_tmp5A6=(_tmp907=_region_malloc(_tmp5A5->r,sizeof(*_tmp907)),((
_tmp907->hd=((_tmp908=_region_malloc(_tmp5A5->r,sizeof(*_tmp908)),((_tmp908->vd=
_tmp57C,((_tmp908->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5A4),_tmp908)))))),((
_tmp907->tl=_tmp4B2,_tmp907)))));struct _tuple15 _tmp909;return(_tmp909.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4B1,_tmp5A6),((_tmp909.f2=_tmp564,_tmp909)));}_LL311:;_LL312: {struct
_tuple15 _tmp90A;return(_tmp90A.f1=_tmp563,((_tmp90A.f2=_tmp564,_tmp90A)));}
_LL308:;}_LL306:;_LL307: {struct _tuple15 _tmp90B;return(_tmp90B.f1=_tmp563,((
_tmp90B.f2=_tmp564,_tmp90B)));}_LL2FF:;}_LL2FD:;_LL2FE: {struct _tuple15 _tmp90C;
return(_tmp90C.f1=_tmp563,((_tmp90C.f2=_tmp564,_tmp90C)));}_LL2F4:;};}default:
_LL2EE: {struct _tuple15 _tmp90D;return(_tmp90D.f1=f,((_tmp90D.f2=f,_tmp90D)));}}}
_LL28F:;};}_LL28D:;_LL28E: goto _LL280;_LL280:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp5AF;void*_tmp5B0;struct _tuple13 _tmp5AE=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5B1=_tmp5AF;int _tmp5B2;struct _tuple12 _tmp5B3;struct Cyc_Dict_Dict _tmp5B4;
_LL315: if((_tmp5B1.BottomFL).tag != 1)goto _LL317;_tmp5B2=(int)(_tmp5B1.BottomFL).val;
_LL316: {struct _tuple15 _tmp90E;return(_tmp90E.f1=_tmp5AF,((_tmp90E.f2=_tmp5AF,
_tmp90E)));}_LL317: if((_tmp5B1.ReachableFL).tag != 2)goto _LL314;_tmp5B3=(struct
_tuple12)(_tmp5B1.ReachableFL).val;_tmp5B4=_tmp5B3.f1;_LL318: {void*_tmp5B6=
_tmp5B0;enum Cyc_CfFlowInfo_InitLevel _tmp5BC;enum Cyc_CfFlowInfo_InitLevel
_tmp5BE;enum Cyc_CfFlowInfo_InitLevel _tmp5C1;_LL31A: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5B7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5B6;if(_tmp5B7->tag != 0)goto
_LL31C;}_LL31B: {struct _tuple15 _tmp90F;return(_tmp90F.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp90F.f2=_tmp5AF,_tmp90F)));}_LL31C: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp5B8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp5B6;if(_tmp5B8->tag != 2)
goto _LL31E;}_LL31D: goto _LL31F;_LL31E: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5B9=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5B6;if(_tmp5B9->tag != 1)
goto _LL320;}_LL31F: goto _LL321;_LL320: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5BA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5B6;if(_tmp5BA->tag != 5)goto
_LL322;}_LL321: {struct _tuple15 _tmp910;return(_tmp910.f1=_tmp5AF,((_tmp910.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp910)));}_LL322: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5BB=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5B6;if(_tmp5BB->tag != 3)goto
_LL324;else{_tmp5BC=_tmp5BB->f1;if(_tmp5BC != Cyc_CfFlowInfo_NoneIL)goto _LL324;}}
_LL323: goto _LL325;_LL324: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5BD=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5B6;if(_tmp5BD->tag != 4)goto _LL326;else{_tmp5BE=_tmp5BD->f1;if(_tmp5BE != Cyc_CfFlowInfo_NoneIL)
goto _LL326;}}_LL325: goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5BF=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5B6;if(_tmp5BF->tag != 7)goto
_LL328;}_LL327:{const char*_tmp913;void*_tmp912;(_tmp912=0,Cyc_Tcutil_terr(e->loc,((
_tmp913="expression may not be initialized",_tag_dyneither(_tmp913,sizeof(char),
34))),_tag_dyneither(_tmp912,sizeof(void*),0)));}{struct _tuple15 _tmp914;return(
_tmp914.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp914.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp914)));};_LL328: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5C0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5B6;if(_tmp5C0->tag != 3)goto _LL32A;else{_tmp5C1=_tmp5C0->f1;}}_LL329: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5AF,e,_tmp5C1);_LL32A: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5C2=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5B6;if(_tmp5C2->tag != 4)goto _LL32C;}
_LL32B: {struct _tuple15 _tmp915;return(_tmp915.f1=_tmp5AF,((_tmp915.f2=_tmp5AF,
_tmp915)));}_LL32C: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5C3=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5B6;if(_tmp5C3->tag != 6)goto _LL319;}_LL32D: {struct Cyc_Core_Impossible_struct
_tmp91B;const char*_tmp91A;struct Cyc_Core_Impossible_struct*_tmp919;(int)_throw((
void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp91B.tag=Cyc_Core_Impossible,((
_tmp91B.f1=((_tmp91A="anal_test",_tag_dyneither(_tmp91A,sizeof(char),10))),
_tmp91B)))),_tmp919)))));}_LL319:;}_LL314:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5CE;struct Cyc_Position_Segment*
_tmp5CF;struct Cyc_NewControlFlow_AnalEnv*_tmp5D0;struct Cyc_Dict_Dict _tmp5D1;
struct _tuple22*_tmp5CD=ckenv;_tmp5CE=*_tmp5CD;_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;
_tmp5D1=_tmp5CE.f3;{void*_tmp5D2=root;struct Cyc_Absyn_Vardecl*_tmp5D4;_LL32F: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5D3=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5D2;if(_tmp5D3->tag != 0)goto _LL331;else{_tmp5D4=_tmp5D3->f1;}}_LL330: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5D0->fenv)->r,_tmp5D4->type)){void*_tmp5D5=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5D9;_LL334: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D6=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5D5;if(_tmp5D6->tag != 7)goto _LL336;}_LL335: goto _LL337;_LL336: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5D7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5D5;if(_tmp5D7->tag != 0)goto
_LL338;}_LL337: goto _LL339;_LL338: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5D8=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5D5;if(_tmp5D8->tag != 3)goto _LL33A;
else{_tmp5D9=_tmp5D8->f1;if(_tmp5D9 != Cyc_CfFlowInfo_NoneIL)goto _LL33A;}}_LL339:
goto _LL333;_LL33A:;_LL33B:{const char*_tmp91F;void*_tmp91E[1];struct Cyc_String_pa_struct
_tmp91D;(_tmp91D.tag=0,((_tmp91D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5D4->name)),((_tmp91E[0]=& _tmp91D,Cyc_Tcutil_warn(
_tmp5CF,((_tmp91F="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp91F,sizeof(char),57))),_tag_dyneither(_tmp91E,sizeof(void*),1)))))));}
goto _LL333;_LL333:;}goto _LL32E;_LL331:;_LL332: goto _LL32E;_LL32E:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow){union Cyc_CfFlowInfo_FlowInfo _tmp5DD=flow;int _tmp5DE;struct _tuple12 _tmp5DF;
struct Cyc_Dict_Dict _tmp5E0;_LL33D: if((_tmp5DD.BottomFL).tag != 1)goto _LL33F;
_tmp5DE=(int)(_tmp5DD.BottomFL).val;_LL33E: return;_LL33F: if((_tmp5DD.ReachableFL).tag
!= 2)goto _LL33C;_tmp5DF=(struct _tuple12)(_tmp5DD.ReachableFL).val;_tmp5E0=
_tmp5DF.f1;_LL340:{struct Cyc_List_List*_tmp5E1=env->param_roots;for(0;_tmp5E1 != 
0;_tmp5E1=_tmp5E1->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5E0,Cyc_CfFlowInfo_lookup_place(
_tmp5E0,(struct Cyc_CfFlowInfo_Place*)_tmp5E1->hd))!= Cyc_CfFlowInfo_AllIL){const
char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_terr(loc,((_tmp922="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp922,sizeof(char),76))),_tag_dyneither(_tmp921,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp923;struct _tuple22
_tmp5E4=(_tmp923.f1=loc,((_tmp923.f2=env,((_tmp923.f3=_tmp5E0,_tmp923)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5E4,_tmp5E0);}
return;_LL33C:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5E6=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5E8;struct Cyc_Core_Opt*_tmp5E9;
struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Position_Segment*
_tmp5EC;struct Cyc_Absyn_Switch_clause*_tmp5E7=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5E8=*_tmp5E7;_tmp5E9=_tmp5E8.pat_vars;_tmp5EA=_tmp5E8.where_clause;
_tmp5EB=_tmp5E8.body;_tmp5EC=_tmp5E8.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5E6,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5E9))->v,_tmp5E6->unknown_all,_tmp5EC);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5EA != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5EA;union Cyc_CfFlowInfo_FlowInfo _tmp5EE;union Cyc_CfFlowInfo_FlowInfo
_tmp5EF;struct _tuple15 _tmp5ED=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;inflow=_tmp5EF;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp5EE,_tmp5EB);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp5EB);}{union Cyc_CfFlowInfo_FlowInfo _tmp5F0=clause_outflow;int
_tmp5F1;_LL342: if((_tmp5F0.BottomFL).tag != 1)goto _LL344;_tmp5F1=(int)(_tmp5F0.BottomFL).val;
_LL343: goto _LL341;_LL344:;_LL345: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp926;void*_tmp925;(_tmp925=0,Cyc_Tcutil_terr(_tmp5EB->loc,((_tmp926="switch clause may implicitly fallthru",
_tag_dyneither(_tmp926,sizeof(char),38))),_tag_dyneither(_tmp925,sizeof(void*),0)));}
else{const char*_tmp929;void*_tmp928;(_tmp928=0,Cyc_Tcutil_warn(_tmp5EB->loc,((
_tmp929="switch clause may implicitly fallthru",_tag_dyneither(_tmp929,sizeof(
char),38))),_tag_dyneither(_tmp928,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL341;_LL341:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd);static void Cyc_NewControlFlow_check_dropped_unique_vd(
struct _tuple23*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp5F7;struct Cyc_NewControlFlow_AnalEnv*
_tmp5F8;struct Cyc_Dict_Dict _tmp5F9;struct Cyc_Position_Segment*_tmp5FA;struct
_tuple23*_tmp5F6=vdenv;_tmp5F7=*_tmp5F6;_tmp5F8=_tmp5F7.f1;_tmp5F9=_tmp5F7.f2;
_tmp5FA=_tmp5F7.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5F8->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp92C;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp92B;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5FB=(_tmp92B=_region_malloc((
_tmp5F8->fenv)->r,sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92C.tag=0,((_tmp92C.f1=vd,
_tmp92C)))),_tmp92B)));void*_tmp5FC=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp5F9,(void*)_tmp5FB);void*_tmp5FD=_tmp5FC;enum Cyc_CfFlowInfo_InitLevel
_tmp601;_LL347: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5FE=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5FD;if(_tmp5FE->tag != 7)goto _LL349;}_LL348: goto _LL34A;_LL349: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5FF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5FD;if(_tmp5FF->tag != 0)goto
_LL34B;}_LL34A: goto _LL34C;_LL34B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp600=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5FD;if(_tmp600->tag != 3)goto _LL34D;
else{_tmp601=_tmp600->f1;if(_tmp601 != Cyc_CfFlowInfo_NoneIL)goto _LL34D;}}_LL34C:
goto _LL346;_LL34D:;_LL34E:{const char*_tmp930;void*_tmp92F[1];struct Cyc_String_pa_struct
_tmp92E;(_tmp92E.tag=0,((_tmp92E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp92F[0]=& _tmp92E,Cyc_Tcutil_warn(_tmp5FA,((
_tmp930="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp930,sizeof(char),68))),_tag_dyneither(_tmp92F,sizeof(void*),1)))))));}
goto _LL346;_LL346:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static void Cyc_NewControlFlow_check_dropped_unique(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo _tmp607=inflow;struct _tuple12
_tmp608;struct Cyc_Dict_Dict _tmp609;_LL350: if((_tmp607.ReachableFL).tag != 2)goto
_LL352;_tmp608=(struct _tuple12)(_tmp607.ReachableFL).val;_tmp609=_tmp608.f1;
_LL351: {struct _tuple23 _tmp931;struct _tuple23 _tmp60A=(_tmp931.f1=env,((_tmp931.f2=
_tmp609,((_tmp931.f3=decl->loc,_tmp931)))));struct Cyc_CfFlowInfo_FlowEnv*_tmp60B=
env->fenv;{void*_tmp60C=decl->r;struct Cyc_Absyn_Vardecl*_tmp60E;struct Cyc_Core_Opt*
_tmp610;struct Cyc_Core_Opt _tmp611;struct Cyc_List_List*_tmp612;struct Cyc_List_List*
_tmp614;_LL355: {struct Cyc_Absyn_Var_d_struct*_tmp60D=(struct Cyc_Absyn_Var_d_struct*)
_tmp60C;if(_tmp60D->tag != 0)goto _LL357;else{_tmp60E=_tmp60D->f1;}}_LL356: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp60A,_tmp60E);goto _LL354;_LL357: {struct Cyc_Absyn_Let_d_struct*_tmp60F=(
struct Cyc_Absyn_Let_d_struct*)_tmp60C;if(_tmp60F->tag != 2)goto _LL359;else{
_tmp610=_tmp60F->f2;if(_tmp610 == 0)goto _LL359;_tmp611=*_tmp610;_tmp612=(struct
Cyc_List_List*)_tmp611.v;}}_LL358: _tmp614=_tmp612;goto _LL35A;_LL359: {struct Cyc_Absyn_Letv_d_struct*
_tmp613=(struct Cyc_Absyn_Letv_d_struct*)_tmp60C;if(_tmp613->tag != 3)goto _LL35B;
else{_tmp614=_tmp613->f1;}}_LL35A:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp60A,_tmp614);goto _LL354;_LL35B:;_LL35C: goto _LL354;_LL354:;}goto _LL34F;}
_LL352:;_LL353: goto _LL34F;_LL34F:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp617;union Cyc_CfFlowInfo_FlowInfo*_tmp618;struct _tuple16 _tmp616=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp617=_tmp616.f1;_tmp618=_tmp616.f2;inflow=*_tmp618;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp619=env->fenv;void*_tmp61A=s->r;struct Cyc_Absyn_Exp*_tmp61D;struct Cyc_Absyn_Exp*
_tmp61F;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Stmt*_tmp623;struct Cyc_Absyn_Stmt*
_tmp624;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Stmt*_tmp627;struct Cyc_Absyn_Stmt*
_tmp628;struct _tuple9 _tmp62A;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Stmt*
_tmp62C;struct Cyc_Absyn_Stmt*_tmp62D;struct Cyc_Absyn_Stmt*_tmp62F;struct _tuple9
_tmp630;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Stmt*_tmp632;struct Cyc_Absyn_Exp*
_tmp634;struct _tuple9 _tmp635;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Stmt*
_tmp637;struct _tuple9 _tmp638;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*
_tmp63A;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63D;struct Cyc_List_List*
_tmp63F;struct Cyc_Absyn_Switch_clause**_tmp640;struct Cyc_Absyn_Switch_clause*
_tmp641;struct Cyc_Absyn_Stmt*_tmp643;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_Absyn_Stmt*
_tmp647;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_Stmt*
_tmp64C;struct Cyc_List_List*_tmp64D;struct Cyc_Absyn_Decl*_tmp64F;struct Cyc_Absyn_Decl
_tmp650;void*_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Tvar*_tmp654;
struct Cyc_Absyn_Vardecl*_tmp655;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_Absyn_Decl*
_tmp658;struct Cyc_Absyn_Stmt*_tmp659;struct Cyc_Absyn_Stmt*_tmp65B;struct Cyc_Absyn_Exp*
_tmp65D;_LL35E: {struct Cyc_Absyn_Skip_s_struct*_tmp61B=(struct Cyc_Absyn_Skip_s_struct*)
_tmp61A;if(_tmp61B->tag != 0)goto _LL360;}_LL35F: return inflow;_LL360: {struct Cyc_Absyn_Return_s_struct*
_tmp61C=(struct Cyc_Absyn_Return_s_struct*)_tmp61A;if(_tmp61C->tag != 3)goto _LL362;
else{_tmp61D=_tmp61C->f1;if(_tmp61D != 0)goto _LL362;}}_LL361: if(env->noreturn){
const char*_tmp934;void*_tmp933;(_tmp933=0,Cyc_Tcutil_terr(s->loc,((_tmp934="`noreturn' function might return",
_tag_dyneither(_tmp934,sizeof(char),33))),_tag_dyneither(_tmp933,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL362: {struct Cyc_Absyn_Return_s_struct*_tmp61E=(struct Cyc_Absyn_Return_s_struct*)
_tmp61A;if(_tmp61E->tag != 3)goto _LL364;else{_tmp61F=_tmp61E->f1;}}_LL363: if(env->noreturn){
const char*_tmp937;void*_tmp936;(_tmp936=0,Cyc_Tcutil_terr(s->loc,((_tmp937="`noreturn' function might return",
_tag_dyneither(_tmp937,sizeof(char),33))),_tag_dyneither(_tmp936,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp663;void*_tmp664;struct _tuple13 _tmp662=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp61F));_tmp663=_tmp662.f1;
_tmp664=_tmp662.f2;_tmp663=Cyc_NewControlFlow_use_Rval(env,_tmp61F->loc,_tmp663,
_tmp664);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp663);return Cyc_CfFlowInfo_BottomFL();};
_LL364: {struct Cyc_Absyn_Exp_s_struct*_tmp620=(struct Cyc_Absyn_Exp_s_struct*)
_tmp61A;if(_tmp620->tag != 1)goto _LL366;else{_tmp621=_tmp620->f1;}}_LL365: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp621)).f1;_LL366: {struct Cyc_Absyn_Seq_s_struct*
_tmp622=(struct Cyc_Absyn_Seq_s_struct*)_tmp61A;if(_tmp622->tag != 2)goto _LL368;
else{_tmp623=_tmp622->f1;_tmp624=_tmp622->f2;}}_LL367: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp623),_tmp624);_LL368: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp625=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp61A;if(_tmp625->tag != 4)goto
_LL36A;else{_tmp626=_tmp625->f1;_tmp627=_tmp625->f2;_tmp628=_tmp625->f3;}}_LL369: {
union Cyc_CfFlowInfo_FlowInfo _tmp666;union Cyc_CfFlowInfo_FlowInfo _tmp667;struct
_tuple15 _tmp665=Cyc_NewControlFlow_anal_test(env,inflow,_tmp626);_tmp666=_tmp665.f1;
_tmp667=_tmp665.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp668=Cyc_NewControlFlow_anal_stmt(
env,_tmp667,_tmp628);union Cyc_CfFlowInfo_FlowInfo _tmp669=Cyc_NewControlFlow_anal_stmt(
env,_tmp666,_tmp627);return Cyc_CfFlowInfo_join_flow(_tmp619,env->all_changed,
_tmp669,_tmp668);};}_LL36A: {struct Cyc_Absyn_While_s_struct*_tmp629=(struct Cyc_Absyn_While_s_struct*)
_tmp61A;if(_tmp629->tag != 5)goto _LL36C;else{_tmp62A=_tmp629->f1;_tmp62B=_tmp62A.f1;
_tmp62C=_tmp62A.f2;_tmp62D=_tmp629->f2;}}_LL36B: {union Cyc_CfFlowInfo_FlowInfo*
_tmp66B;struct _tuple16 _tmp66A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp62C);_tmp66B=_tmp66A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp66C=*_tmp66B;union
Cyc_CfFlowInfo_FlowInfo _tmp66E;union Cyc_CfFlowInfo_FlowInfo _tmp66F;struct
_tuple15 _tmp66D=Cyc_NewControlFlow_anal_test(env,_tmp66C,_tmp62B);_tmp66E=
_tmp66D.f1;_tmp66F=_tmp66D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp670=Cyc_NewControlFlow_anal_stmt(
env,_tmp66E,_tmp62D);Cyc_NewControlFlow_update_flow(env,_tmp62C,_tmp670);return
_tmp66F;};};}_LL36C: {struct Cyc_Absyn_Do_s_struct*_tmp62E=(struct Cyc_Absyn_Do_s_struct*)
_tmp61A;if(_tmp62E->tag != 14)goto _LL36E;else{_tmp62F=_tmp62E->f1;_tmp630=_tmp62E->f2;
_tmp631=_tmp630.f1;_tmp632=_tmp630.f2;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo
_tmp671=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp62F);union Cyc_CfFlowInfo_FlowInfo*
_tmp673;struct _tuple16 _tmp672=Cyc_NewControlFlow_pre_stmt_check(env,_tmp671,
_tmp632);_tmp673=_tmp672.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp674=*_tmp673;union
Cyc_CfFlowInfo_FlowInfo _tmp676;union Cyc_CfFlowInfo_FlowInfo _tmp677;struct
_tuple15 _tmp675=Cyc_NewControlFlow_anal_test(env,_tmp674,_tmp631);_tmp676=
_tmp675.f1;_tmp677=_tmp675.f2;Cyc_NewControlFlow_update_flow(env,_tmp62F,_tmp676);
return _tmp677;};}_LL36E: {struct Cyc_Absyn_For_s_struct*_tmp633=(struct Cyc_Absyn_For_s_struct*)
_tmp61A;if(_tmp633->tag != 9)goto _LL370;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;
_tmp636=_tmp635.f1;_tmp637=_tmp635.f2;_tmp638=_tmp633->f3;_tmp639=_tmp638.f1;
_tmp63A=_tmp638.f2;_tmp63B=_tmp633->f4;}}_LL36F: {union Cyc_CfFlowInfo_FlowInfo
_tmp678=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp634)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp67A;struct _tuple16 _tmp679=Cyc_NewControlFlow_pre_stmt_check(env,_tmp678,
_tmp637);_tmp67A=_tmp679.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp67B=*_tmp67A;union
Cyc_CfFlowInfo_FlowInfo _tmp67D;union Cyc_CfFlowInfo_FlowInfo _tmp67E;struct
_tuple15 _tmp67C=Cyc_NewControlFlow_anal_test(env,_tmp67B,_tmp636);_tmp67D=
_tmp67C.f1;_tmp67E=_tmp67C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp67F=Cyc_NewControlFlow_anal_stmt(
env,_tmp67D,_tmp63B);union Cyc_CfFlowInfo_FlowInfo*_tmp681;struct _tuple16 _tmp680=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp67F,_tmp63A);_tmp681=_tmp680.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp682=*_tmp681;union Cyc_CfFlowInfo_FlowInfo _tmp683=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp682,_tmp639)).f1;Cyc_NewControlFlow_update_flow(env,_tmp637,_tmp683);
return _tmp67E;};};};}_LL370: {struct Cyc_Absyn_Break_s_struct*_tmp63C=(struct Cyc_Absyn_Break_s_struct*)
_tmp61A;if(_tmp63C->tag != 6)goto _LL372;else{_tmp63D=_tmp63C->f1;if(_tmp63D != 0)
goto _LL372;}}_LL371: return Cyc_CfFlowInfo_BottomFL();_LL372: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp63E=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp61A;if(_tmp63E->tag != 11)goto
_LL374;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;if(_tmp640 == 0)goto _LL374;
_tmp641=*_tmp640;}}_LL373: {struct _RegionHandle*_tmp684=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp686;struct Cyc_List_List*_tmp687;struct _tuple19 _tmp685=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp684,env,inflow,_tmp63F,1,1);_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;for(0;
_tmp687 != 0;(_tmp687=_tmp687->tl,_tmp63F=_tmp63F->tl)){_tmp686=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63F))->hd)->loc,
_tmp686,(void*)_tmp687->hd);}_tmp686=Cyc_NewControlFlow_add_vars(_tmp619,_tmp686,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp641->pat_vars))->v,
_tmp619->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp641->body,_tmp686);return Cyc_CfFlowInfo_BottomFL();}_LL374: {struct Cyc_Absyn_Break_s_struct*
_tmp642=(struct Cyc_Absyn_Break_s_struct*)_tmp61A;if(_tmp642->tag != 6)goto _LL376;
else{_tmp643=_tmp642->f1;}}_LL375: _tmp645=_tmp643;goto _LL377;_LL376: {struct Cyc_Absyn_Continue_s_struct*
_tmp644=(struct Cyc_Absyn_Continue_s_struct*)_tmp61A;if(_tmp644->tag != 7)goto
_LL378;else{_tmp645=_tmp644->f1;}}_LL377: _tmp647=_tmp645;goto _LL379;_LL378: {
struct Cyc_Absyn_Goto_s_struct*_tmp646=(struct Cyc_Absyn_Goto_s_struct*)_tmp61A;
if(_tmp646->tag != 8)goto _LL37A;else{_tmp647=_tmp646->f2;}}_LL379: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp688=_tmp617->encloser;struct Cyc_Absyn_Stmt*
_tmp689=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp647)))->encloser;while(_tmp689 != _tmp688){struct Cyc_Absyn_Stmt*_tmp68A=(Cyc_NewControlFlow_get_stmt_annot(
_tmp688))->encloser;if(_tmp68A == _tmp688){{const char*_tmp93A;void*_tmp939;(
_tmp939=0,Cyc_Tcutil_terr(s->loc,((_tmp93A="goto enters local scope or exception handler",
_tag_dyneither(_tmp93A,sizeof(char),45))),_tag_dyneither(_tmp939,sizeof(void*),0)));}
break;}_tmp688=_tmp68A;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp647),inflow);return Cyc_CfFlowInfo_BottomFL();_LL37A: {struct Cyc_Absyn_Switch_s_struct*
_tmp648=(struct Cyc_Absyn_Switch_s_struct*)_tmp61A;if(_tmp648->tag != 10)goto
_LL37C;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;}}_LL37B: {union Cyc_CfFlowInfo_FlowInfo
_tmp68E;void*_tmp68F;struct _tuple13 _tmp68D=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp649);_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;_tmp68E=Cyc_NewControlFlow_use_Rval(
env,_tmp649->loc,_tmp68E,_tmp68F);return Cyc_NewControlFlow_anal_scs(env,_tmp68E,
_tmp64A);}_LL37C: {struct Cyc_Absyn_TryCatch_s_struct*_tmp64B=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp61A;if(_tmp64B->tag != 15)goto _LL37E;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;}}
_LL37D: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp64C);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp64D);{union Cyc_CfFlowInfo_FlowInfo _tmp690=scs_outflow;int
_tmp691;_LL389: if((_tmp690.BottomFL).tag != 1)goto _LL38B;_tmp691=(int)(_tmp690.BottomFL).val;
_LL38A: goto _LL388;_LL38B:;_LL38C: {const char*_tmp93D;void*_tmp93C;(_tmp93C=0,Cyc_Tcutil_terr(
s->loc,((_tmp93D="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp93D,sizeof(char),42))),_tag_dyneither(_tmp93C,sizeof(void*),0)));}_LL388:;}
outflow=s1_outflow;return outflow;};};}_LL37E: {struct Cyc_Absyn_Decl_s_struct*
_tmp64E=(struct Cyc_Absyn_Decl_s_struct*)_tmp61A;if(_tmp64E->tag != 12)goto _LL380;
else{_tmp64F=_tmp64E->f1;_tmp650=*_tmp64F;_tmp651=_tmp650.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp652=(struct Cyc_Absyn_Alias_d_struct*)_tmp651;if(_tmp652->tag != 5)goto _LL380;
else{_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=_tmp652->f3;}};_tmp656=
_tmp64E->f2;}}_LL37F: {union Cyc_CfFlowInfo_FlowInfo _tmp695;union Cyc_CfFlowInfo_AbsLVal
_tmp696;struct _tuple14 _tmp694=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp653);
_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;{struct _tuple14 _tmp93E;struct _tuple14
_tmp698=(_tmp93E.f1=_tmp695,((_tmp93E.f2=_tmp696,_tmp93E)));union Cyc_CfFlowInfo_FlowInfo
_tmp699;struct _tuple12 _tmp69A;struct Cyc_Dict_Dict _tmp69B;struct Cyc_List_List*
_tmp69C;union Cyc_CfFlowInfo_AbsLVal _tmp69D;struct Cyc_CfFlowInfo_Place*_tmp69E;
union Cyc_CfFlowInfo_FlowInfo _tmp69F;int _tmp6A0;_LL38E: _tmp699=_tmp698.f1;if((
_tmp699.ReachableFL).tag != 2)goto _LL390;_tmp69A=(struct _tuple12)(_tmp699.ReachableFL).val;
_tmp69B=_tmp69A.f1;_tmp69C=_tmp69A.f2;_tmp69D=_tmp698.f2;if((_tmp69D.PlaceL).tag
!= 1)goto _LL390;_tmp69E=(struct Cyc_CfFlowInfo_Place*)(_tmp69D.PlaceL).val;_LL38F: {
void*_tmp6A1=Cyc_CfFlowInfo_lookup_place(_tmp69B,_tmp69E);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp653->topt))->v;{void*_tmp6A2=_tmp653->r;struct Cyc_Absyn_Exp*
_tmp6A4;_LL395: {struct Cyc_Absyn_Cast_e_struct*_tmp6A3=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6A2;if(_tmp6A3->tag != 15)goto _LL397;else{_tmp6A4=_tmp6A3->f2;}}_LL396: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A4->topt))->v;goto _LL394;_LL397:;
_LL398: goto _LL394;_LL394:;}{void*_tmp6A5=Cyc_CfFlowInfo_make_unique_consumed(
_tmp619,t,_tmp653,env->iteration_num,_tmp6A1);_tmp69B=Cyc_CfFlowInfo_assign_place(
_tmp619,_tmp653->loc,_tmp69B,env->all_changed,_tmp69E,_tmp6A5);_tmp695=Cyc_CfFlowInfo_ReachableFL(
_tmp69B,_tmp69C);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp619,_tmp69B,
_tmp6A1)){case Cyc_CfFlowInfo_AllIL: _LL399: leaf=_tmp619->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL39A: leaf=_tmp619->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL39B: leaf=_tmp619->unknown_this;break;}{void*_tmp6A6=Cyc_CfFlowInfo_typ_to_absrval(
_tmp619,t,leaf);_tmp695=Cyc_NewControlFlow_use_Rval(env,_tmp653->loc,_tmp695,
_tmp6A1);{struct Cyc_List_List _tmp93F;struct Cyc_List_List _tmp6A7=(_tmp93F.hd=
_tmp655,((_tmp93F.tl=0,_tmp93F)));_tmp695=Cyc_NewControlFlow_add_vars(_tmp619,
_tmp695,(struct Cyc_List_List*)& _tmp6A7,_tmp619->unknown_all,s->loc);_tmp695=Cyc_NewControlFlow_anal_stmt(
env,_tmp695,_tmp656);{union Cyc_CfFlowInfo_FlowInfo _tmp6A8=_tmp695;struct _tuple12
_tmp6A9;struct Cyc_Dict_Dict _tmp6AA;struct Cyc_List_List*_tmp6AB;_LL39E: if((
_tmp6A8.ReachableFL).tag != 2)goto _LL3A0;_tmp6A9=(struct _tuple12)(_tmp6A8.ReachableFL).val;
_tmp6AA=_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;_LL39F: _tmp6AA=Cyc_CfFlowInfo_assign_place(
_tmp619,s->loc,_tmp6AA,env->all_changed,_tmp69E,_tmp6A6);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6AA,_tmp6AB);return outflow;_LL3A0:;_LL3A1: return _tmp695;_LL39D:;};};};};};}
_LL390: _tmp69F=_tmp698.f1;if((_tmp69F.BottomFL).tag != 1)goto _LL392;_tmp6A0=(int)(
_tmp69F.BottomFL).val;_LL391: return _tmp695;_LL392:;_LL393:{const char*_tmp942;
void*_tmp941;(_tmp941=0,Cyc_Tcutil_terr(_tmp653->loc,((_tmp942="bad alias expression--no unique path found",
_tag_dyneither(_tmp942,sizeof(char),43))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38D:;};}_LL380: {struct Cyc_Absyn_Decl_s_struct*
_tmp657=(struct Cyc_Absyn_Decl_s_struct*)_tmp61A;if(_tmp657->tag != 12)goto _LL382;
else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;}}_LL381: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp658),_tmp659);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp658);return outflow;
_LL382: {struct Cyc_Absyn_Label_s_struct*_tmp65A=(struct Cyc_Absyn_Label_s_struct*)
_tmp61A;if(_tmp65A->tag != 13)goto _LL384;else{_tmp65B=_tmp65A->f2;}}_LL383: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp65B);_LL384: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp65C=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp61A;if(_tmp65C->tag != 16)goto
_LL386;else{_tmp65D=_tmp65C->f1;}}_LL385: {union Cyc_CfFlowInfo_FlowInfo _tmp6B0;
void*_tmp6B1;struct _tuple13 _tmp6AF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp65D);_tmp6B0=_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6B2=Cyc_NewControlFlow_use_Rval(env,_tmp65D->loc,_tmp6B0,_tmp6B1);void*
_tmp6B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp65D->topt))->v);
void*_tmp6B5;_LL3A3: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6B4=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6B3;if(_tmp6B4->tag != 16)goto _LL3A5;else{_tmp6B5=(void*)_tmp6B4->f1;}}_LL3A4:
return Cyc_CfFlowInfo_kill_flow_region(_tmp619,_tmp6B0,_tmp6B5);_LL3A5:;_LL3A6: {
struct Cyc_Core_Impossible_struct _tmp948;const char*_tmp947;struct Cyc_Core_Impossible_struct*
_tmp946;(int)_throw((void*)((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946[0]=((
_tmp948.tag=Cyc_Core_Impossible,((_tmp948.f1=((_tmp947="anal_stmt -- reset_region",
_tag_dyneither(_tmp947,sizeof(char),26))),_tmp948)))),_tmp946)))));}_LL3A2:;};}
_LL386:;_LL387: {struct Cyc_Core_Impossible_struct _tmp94E;const char*_tmp94D;
struct Cyc_Core_Impossible_struct*_tmp94C;(int)_throw((void*)((_tmp94C=_cycalloc(
sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94E.tag=Cyc_Core_Impossible,((_tmp94E.f1=((
_tmp94D="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp94D,sizeof(char),56))),_tmp94E)))),_tmp94C)))));}_LL35D:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp6BC=env->fenv;
void*_tmp6BD=decl->r;struct Cyc_Absyn_Vardecl*_tmp6BF;struct Cyc_Core_Opt*_tmp6C1;
struct Cyc_Core_Opt _tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_Absyn_Exp*
_tmp6C4;struct Cyc_List_List*_tmp6C6;struct Cyc_Absyn_Fndecl*_tmp6C8;struct Cyc_Absyn_Tvar*
_tmp6CA;struct Cyc_Absyn_Vardecl*_tmp6CB;int _tmp6CC;struct Cyc_Absyn_Exp*_tmp6CD;
_LL3A8: {struct Cyc_Absyn_Var_d_struct*_tmp6BE=(struct Cyc_Absyn_Var_d_struct*)
_tmp6BD;if(_tmp6BE->tag != 0)goto _LL3AA;else{_tmp6BF=_tmp6BE->f1;}}_LL3A9: {
struct Cyc_List_List _tmp94F;struct Cyc_List_List _tmp6CE=(_tmp94F.hd=_tmp6BF,((
_tmp94F.tl=0,_tmp94F)));inflow=Cyc_NewControlFlow_add_vars(_tmp6BC,inflow,(
struct Cyc_List_List*)& _tmp6CE,_tmp6BC->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6CF=_tmp6BF->initializer;if(_tmp6CF == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6D1;void*_tmp6D2;struct _tuple13 _tmp6D0=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6CF);_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6D3=_tmp6D1;int _tmp6D4;struct _tuple12 _tmp6D5;
struct Cyc_Dict_Dict _tmp6D6;struct Cyc_List_List*_tmp6D7;_LL3B5: if((_tmp6D3.BottomFL).tag
!= 1)goto _LL3B7;_tmp6D4=(int)(_tmp6D3.BottomFL).val;_LL3B6: return Cyc_CfFlowInfo_BottomFL();
_LL3B7: if((_tmp6D3.ReachableFL).tag != 2)goto _LL3B4;_tmp6D5=(struct _tuple12)(
_tmp6D3.ReachableFL).val;_tmp6D6=_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;_LL3B8:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp955;struct Cyc_CfFlowInfo_VarRoot_struct _tmp954;struct Cyc_CfFlowInfo_Place*
_tmp953;_tmp6D6=Cyc_CfFlowInfo_assign_place(_tmp6BC,decl->loc,_tmp6D6,env->all_changed,((
_tmp953=_region_malloc(env->r,sizeof(*_tmp953)),((_tmp953->root=(void*)((_tmp955=
_region_malloc(env->r,sizeof(*_tmp955)),((_tmp955[0]=((_tmp954.tag=0,((_tmp954.f1=
_tmp6BF,_tmp954)))),_tmp955)))),((_tmp953->fields=0,_tmp953)))))),_tmp6D2);}
_tmp6D7=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6D7,_tmp6BF,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6BF->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6DB=Cyc_CfFlowInfo_ReachableFL(
_tmp6D6,_tmp6D7);Cyc_NewControlFlow_update_tryflow(env,_tmp6DB);return _tmp6DB;};
_LL3B4:;};};};}_LL3AA: {struct Cyc_Absyn_Let_d_struct*_tmp6C0=(struct Cyc_Absyn_Let_d_struct*)
_tmp6BD;if(_tmp6C0->tag != 2)goto _LL3AC;else{_tmp6C1=_tmp6C0->f2;if(_tmp6C1 == 0)
goto _LL3AC;_tmp6C2=*_tmp6C1;_tmp6C3=(struct Cyc_List_List*)_tmp6C2.v;_tmp6C4=
_tmp6C0->f3;}}_LL3AB: {union Cyc_CfFlowInfo_FlowInfo _tmp6DE;void*_tmp6DF;struct
_tuple13 _tmp6DD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6C4);_tmp6DE=
_tmp6DD.f1;_tmp6DF=_tmp6DD.f2;_tmp6DE=Cyc_NewControlFlow_use_Rval(env,_tmp6C4->loc,
_tmp6DE,_tmp6DF);return Cyc_NewControlFlow_add_vars(_tmp6BC,_tmp6DE,_tmp6C3,
_tmp6BC->unknown_all,decl->loc);}_LL3AC: {struct Cyc_Absyn_Letv_d_struct*_tmp6C5=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6BD;if(_tmp6C5->tag != 3)goto _LL3AE;else{
_tmp6C6=_tmp6C5->f1;}}_LL3AD: return Cyc_NewControlFlow_add_vars(_tmp6BC,inflow,
_tmp6C6,_tmp6BC->unknown_none,decl->loc);_LL3AE: {struct Cyc_Absyn_Fn_d_struct*
_tmp6C7=(struct Cyc_Absyn_Fn_d_struct*)_tmp6BD;if(_tmp6C7->tag != 1)goto _LL3B0;
else{_tmp6C8=_tmp6C7->f1;}}_LL3AF: Cyc_NewControlFlow_check_nested_fun(_tmp6BC,
inflow,_tmp6C8);{void*_tmp6E0=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6C8->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6E1=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6C8->fn_vardecl);
struct Cyc_List_List*_tmp956;return Cyc_NewControlFlow_add_vars(_tmp6BC,inflow,((
_tmp956=_region_malloc(env->r,sizeof(*_tmp956)),((_tmp956->hd=_tmp6E1,((_tmp956->tl=
0,_tmp956)))))),_tmp6BC->unknown_all,decl->loc);};_LL3B0: {struct Cyc_Absyn_Region_d_struct*
_tmp6C9=(struct Cyc_Absyn_Region_d_struct*)_tmp6BD;if(_tmp6C9->tag != 4)goto _LL3B2;
else{_tmp6CA=_tmp6C9->f1;_tmp6CB=_tmp6C9->f2;_tmp6CC=_tmp6C9->f3;_tmp6CD=_tmp6C9->f4;}}
_LL3B1: if(_tmp6CD != 0){struct Cyc_Absyn_Exp*_tmp6E3=(struct Cyc_Absyn_Exp*)_tmp6CD;
union Cyc_CfFlowInfo_FlowInfo _tmp6E5;void*_tmp6E6;struct _tuple13 _tmp6E4=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6E3);_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6E3->loc,_tmp6E5,_tmp6E6);}{struct Cyc_List_List _tmp957;struct Cyc_List_List
_tmp6E7=(_tmp957.hd=_tmp6CB,((_tmp957.tl=0,_tmp957)));return Cyc_NewControlFlow_add_vars(
_tmp6BC,inflow,(struct Cyc_List_List*)& _tmp6E7,_tmp6BC->unknown_all,decl->loc);};
_LL3B2:;_LL3B3: {struct Cyc_Core_Impossible_struct _tmp95D;const char*_tmp95C;
struct Cyc_Core_Impossible_struct*_tmp95B;(int)_throw((void*)((_tmp95B=_cycalloc(
sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95D.tag=Cyc_Core_Impossible,((_tmp95D.f1=((
_tmp95C="anal_decl: unexpected decl variant",_tag_dyneither(_tmp95C,sizeof(char),
35))),_tmp95D)))),_tmp95B)))));}_LL3A7:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp6EC=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp6EC;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp6ED=fenv->r;struct Cyc_Position_Segment*_tmp6EE=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6EE);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp6EF=inflow;int _tmp6F0;struct
_tuple12 _tmp6F1;struct Cyc_Dict_Dict _tmp6F2;struct Cyc_List_List*_tmp6F3;_LL3BA:
if((_tmp6EF.BottomFL).tag != 1)goto _LL3BC;_tmp6F0=(int)(_tmp6EF.BottomFL).val;
_LL3BB: {const char*_tmp960;void*_tmp95F;(_tmp95F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp960="check_fun",
_tag_dyneither(_tmp960,sizeof(char),10))),_tag_dyneither(_tmp95F,sizeof(void*),0)));}
_LL3BC: if((_tmp6EF.ReachableFL).tag != 2)goto _LL3B9;_tmp6F1=(struct _tuple12)(
_tmp6EF.ReachableFL).val;_tmp6F2=_tmp6F1.f1;_tmp6F3=_tmp6F1.f2;_LL3BD: {struct
Cyc_List_List*atts;{void*_tmp6F6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6F8;struct Cyc_List_List*
_tmp6F9;_LL3BF: {struct Cyc_Absyn_FnType_struct*_tmp6F7=(struct Cyc_Absyn_FnType_struct*)
_tmp6F6;if(_tmp6F7->tag != 10)goto _LL3C1;else{_tmp6F8=_tmp6F7->f1;_tmp6F9=_tmp6F8.attributes;}}
_LL3C0: atts=_tmp6F9;goto _LL3BE;_LL3C1:;_LL3C2: {const char*_tmp963;void*_tmp962;(
_tmp962=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp963="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp963,sizeof(char),50))),_tag_dyneither(_tmp962,sizeof(void*),0)));}_LL3BE:;}
for(0;atts != 0;atts=atts->tl){void*_tmp6FC=(void*)atts->hd;int _tmp6FE;int _tmp700;
_LL3C4: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp6FD=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp6FC;if(_tmp6FD->tag != 21)goto _LL3C6;else{_tmp6FE=_tmp6FD->f1;}}_LL3C5: {
unsigned int j=(unsigned int)_tmp6FE;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp966;
void*_tmp965;(_tmp965=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp966="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp966,sizeof(char),52))),_tag_dyneither(_tmp965,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp703=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp703->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp969;void*
_tmp968;(_tmp968=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp969="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp969,sizeof(char),52))),_tag_dyneither(_tmp968,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp706=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp96C;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp96B;struct Cyc_CfFlowInfo_InitParam_struct*_tmp707=(_tmp96B=_region_malloc(
_tmp6ED,sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96C.tag=2,((_tmp96C.f1=(int)j,((
_tmp96C.f2=(void*)t,_tmp96C)))))),_tmp96B)));struct Cyc_CfFlowInfo_Place*_tmp96D;
struct Cyc_CfFlowInfo_Place*_tmp708=(_tmp96D=_region_malloc(_tmp6ED,sizeof(*
_tmp96D)),((_tmp96D->root=(void*)_tmp707,((_tmp96D->fields=0,_tmp96D)))));
_tmp6F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F2,(void*)_tmp707,_tmp706);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp973;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp972;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp970;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp96F;_tmp6F2=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6F2,(void*)((_tmp96F=
_region_malloc(_tmp6ED,sizeof(*_tmp96F)),((_tmp96F[0]=((_tmp970.tag=0,((_tmp970.f1=
_tmp703,_tmp970)))),_tmp96F)))),(void*)((_tmp972=_region_malloc(_tmp6ED,sizeof(*
_tmp972)),((_tmp972[0]=((_tmp973.tag=5,((_tmp973.f1=_tmp708,_tmp973)))),_tmp972)))));}
goto _LL3C3;};};}_LL3C6: {struct Cyc_Absyn_Initializes_att_struct*_tmp6FF=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp6FC;if(_tmp6FF->tag != 20)goto _LL3C8;else{
_tmp700=_tmp6FF->f1;}}_LL3C7: {unsigned int j=(unsigned int)_tmp700;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp976;void*_tmp975;(_tmp975=0,
Cyc_Tcutil_terr(_tmp6EE,((_tmp976="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp976,sizeof(char),51))),_tag_dyneither(_tmp975,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp712=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp713=Cyc_Tcutil_compress(
_tmp712->type);struct Cyc_Absyn_PtrInfo _tmp715;void*_tmp716;struct Cyc_Absyn_PtrAtts
_tmp717;union Cyc_Absyn_Constraint*_tmp718;union Cyc_Absyn_Constraint*_tmp719;
union Cyc_Absyn_Constraint*_tmp71A;_LL3CB: {struct Cyc_Absyn_PointerType_struct*
_tmp714=(struct Cyc_Absyn_PointerType_struct*)_tmp713;if(_tmp714->tag != 5)goto
_LL3CD;else{_tmp715=_tmp714->f1;_tmp716=_tmp715.elt_typ;_tmp717=_tmp715.ptr_atts;
_tmp718=_tmp717.nullable;_tmp719=_tmp717.bounds;_tmp71A=_tmp717.zero_term;}}
_LL3CC: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp718)){
const char*_tmp979;void*_tmp978;(_tmp978=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp979="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp979,sizeof(char),55))),_tag_dyneither(_tmp978,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp719)){const char*_tmp97C;void*_tmp97B;(_tmp97B=0,
Cyc_Tcutil_terr(_tmp6EE,((_tmp97C="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp97C,sizeof(char),57))),_tag_dyneither(_tmp97B,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp71A)){
const char*_tmp97F;void*_tmp97E;(_tmp97E=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp97F="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp97F,sizeof(char),77))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp982;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp981;struct Cyc_CfFlowInfo_InitParam_struct*_tmp721=(_tmp981=_region_malloc(
_tmp6ED,sizeof(*_tmp981)),((_tmp981[0]=((_tmp982.tag=2,((_tmp982.f1=(int)j,((
_tmp982.f2=(void*)_tmp716,_tmp982)))))),_tmp981)));struct Cyc_CfFlowInfo_Place*
_tmp983;struct Cyc_CfFlowInfo_Place*_tmp722=(_tmp983=_region_malloc(_tmp6ED,
sizeof(*_tmp983)),((_tmp983->root=(void*)_tmp721,((_tmp983->fields=0,_tmp983)))));
_tmp6F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F2,(void*)_tmp721,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp716,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp989;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp988;struct Cyc_CfFlowInfo_VarRoot_struct _tmp986;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp985;_tmp6F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F2,(void*)((_tmp985=_region_malloc(_tmp6ED,sizeof(*_tmp985)),((_tmp985[0]=((
_tmp986.tag=0,((_tmp986.f1=_tmp712,_tmp986)))),_tmp985)))),(void*)((_tmp988=
_region_malloc(_tmp6ED,sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=5,((_tmp989.f1=
_tmp722,_tmp989)))),_tmp988)))));}{struct Cyc_List_List*_tmp98A;param_roots=((
_tmp98A=_region_malloc(_tmp6ED,sizeof(*_tmp98A)),((_tmp98A->hd=_tmp722,((_tmp98A->tl=
param_roots,_tmp98A))))));}goto _LL3CA;};_LL3CD:;_LL3CE: {const char*_tmp98D;void*
_tmp98C;(_tmp98C=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp98D="initializes attribute on non-pointer",
_tag_dyneither(_tmp98D,sizeof(char),37))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
_LL3CA:;}goto _LL3C3;};}_LL3C8:;_LL3C9: goto _LL3C3;_LL3C3:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6F2,_tmp6F3);}_LL3B9:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6ED,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp98E;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp98E=_region_malloc(_tmp6ED,sizeof(*_tmp98E)),((
_tmp98E->r=_tmp6ED,((_tmp98E->fenv=fenv,((_tmp98E->iterate_again=1,((_tmp98E->iteration_num=
0,((_tmp98E->in_try=0,((_tmp98E->tryflow=inflow,((_tmp98E->all_changed=0,((
_tmp98E->noreturn=noreturn,((_tmp98E->param_roots=param_roots,((_tmp98E->flow_table=
flow_table,_tmp98E)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp72D=outflow;int _tmp72E;_LL3D0: if((_tmp72D.BottomFL).tag
!= 1)goto _LL3D2;_tmp72E=(int)(_tmp72D.BottomFL).val;_LL3D1: goto _LL3CF;_LL3D2:;
_LL3D3: Cyc_NewControlFlow_check_init_params(_tmp6EE,env,outflow);if(noreturn){
const char*_tmp991;void*_tmp990;(_tmp990=0,Cyc_Tcutil_terr(_tmp6EE,((_tmp991="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp991,sizeof(char),46))),_tag_dyneither(_tmp990,sizeof(void*),0)));}
else{void*_tmp731=Cyc_Tcutil_compress(fd->ret_type);_LL3D5: {struct Cyc_Absyn_VoidType_struct*
_tmp732=(struct Cyc_Absyn_VoidType_struct*)_tmp731;if(_tmp732->tag != 0)goto _LL3D7;}
_LL3D6: goto _LL3D4;_LL3D7: {struct Cyc_Absyn_DoubleType_struct*_tmp733=(struct Cyc_Absyn_DoubleType_struct*)
_tmp731;if(_tmp733->tag != 8)goto _LL3D9;}_LL3D8: goto _LL3DA;_LL3D9: {struct Cyc_Absyn_FloatType_struct*
_tmp734=(struct Cyc_Absyn_FloatType_struct*)_tmp731;if(_tmp734->tag != 7)goto
_LL3DB;}_LL3DA: goto _LL3DC;_LL3DB: {struct Cyc_Absyn_IntType_struct*_tmp735=(
struct Cyc_Absyn_IntType_struct*)_tmp731;if(_tmp735->tag != 6)goto _LL3DD;}_LL3DC:{
const char*_tmp994;void*_tmp993;(_tmp993=0,Cyc_Tcutil_warn(_tmp6EE,((_tmp994="function may not return a value",
_tag_dyneither(_tmp994,sizeof(char),32))),_tag_dyneither(_tmp993,sizeof(void*),0)));}
goto _LL3D4;_LL3DD:;_LL3DE:{const char*_tmp997;void*_tmp996;(_tmp996=0,Cyc_Tcutil_terr(
_tmp6EE,((_tmp997="function may not return a value",_tag_dyneither(_tmp997,
sizeof(char),32))),_tag_dyneither(_tmp996,sizeof(void*),0)));}goto _LL3D4;_LL3D4:;}
goto _LL3CF;_LL3CF:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp73B=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp73D;
struct Cyc_List_List*_tmp73F;struct Cyc_List_List*_tmp741;struct Cyc_List_List*
_tmp743;_LL3E0: {struct Cyc_Absyn_Fn_d_struct*_tmp73C=(struct Cyc_Absyn_Fn_d_struct*)
_tmp73B;if(_tmp73C->tag != 1)goto _LL3E2;else{_tmp73D=_tmp73C->f1;}}_LL3E1: Cyc_NewControlFlow_check_fun(
_tmp73D);goto _LL3DF;_LL3E2: {struct Cyc_Absyn_ExternC_d_struct*_tmp73E=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp73B;if(_tmp73E->tag != 12)goto _LL3E4;else{_tmp73F=_tmp73E->f1;}}_LL3E3:
_tmp741=_tmp73F;goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Using_d_struct*_tmp740=(
struct Cyc_Absyn_Using_d_struct*)_tmp73B;if(_tmp740->tag != 11)goto _LL3E6;else{
_tmp741=_tmp740->f2;}}_LL3E5: _tmp743=_tmp741;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Namespace_d_struct*
_tmp742=(struct Cyc_Absyn_Namespace_d_struct*)_tmp73B;if(_tmp742->tag != 10)goto
_LL3E8;else{_tmp743=_tmp742->f2;}}_LL3E7: Cyc_NewControlFlow_cf_check(_tmp743);
goto _LL3DF;_LL3E8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp744=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp73B;if(_tmp744->tag != 13)goto _LL3EA;}_LL3E9: goto _LL3DF;_LL3EA:;_LL3EB: goto
_LL3DF;_LL3DF:;}}
