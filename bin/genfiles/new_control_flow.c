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
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,int no_init_bits_only,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp762;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp761;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp760;enclosee->annot=(void*)((_tmp760=_cycalloc(sizeof(*_tmp760)),((_tmp760[0]=((
_tmp762.tag=Cyc_NewControlFlow_CFAnnot,((_tmp762.f1=((_tmp761.encloser=encloser,((
_tmp761.visited=0,_tmp761)))),_tmp762)))),_tmp760))));}struct Cyc_NewControlFlow_AnalEnv{
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
_tmp768;const char*_tmp767;struct Cyc_Core_Impossible_struct*_tmp766;(int)_throw((
void*)((_tmp766=_cycalloc(sizeof(*_tmp766)),((_tmp766[0]=((_tmp768.tag=Cyc_Core_Impossible,((
_tmp768.f1=((_tmp767="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp767,sizeof(char),37))),_tmp768)))),_tmp766)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp769;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp769=_region_malloc(env->r,sizeof(*_tmp769)),((_tmp769[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp769)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp76A;return(_tmp76A.f1=_tmpB,((_tmp76A.f2=
_tmpC,_tmp76A)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp76E;void*_tmp76D[1];struct Cyc_String_pa_struct _tmp76C;(_tmp76C.tag=0,((
_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp76D[0]=& _tmp76C,Cyc_Tcutil_warn(_tmp11,((_tmp76E="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp76E,sizeof(char),60))),_tag_dyneither(_tmp76D,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=
Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple15 _tmp76F;struct _tuple15 _tmp22=(_tmp76F.f1=flow,((_tmp76F.f2=_tmp20,
_tmp76F)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict
_tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict
_tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(
struct _tuple12)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((
_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;
_tmp28=_tmp27.f1;_LL15: {struct _tuple17 _tmp770;struct _tuple17 _tmp29=(_tmp770.f1=
env,((_tmp770.f2=s->loc,((_tmp770.f3=_tmp28,_tmp770)))));((void(*)(void(*f)(
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
_tmp773;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp772;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp772=_region_malloc(fenv->r,sizeof(*_tmp772)),((_tmp772[0]=((_tmp773.tag=
0,((_tmp773.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp773)))),_tmp772)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,0,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);
_LL18:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp33=inflow;int _tmp34;struct _tuple12 _tmp35;struct
Cyc_Dict_Dict _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 
1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;_LL1F: return Cyc_CfFlowInfo_BottomFL();
_LL20: if((_tmp33.ReachableFL).tag != 2)goto _LL1D;_tmp35=(struct _tuple12)(_tmp33.ReachableFL).val;
_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp776;void*_tmp775;(_tmp775=0,Cyc_Tcutil_terr(
loc,((_tmp776="expression may not be fully initialized",_tag_dyneither(_tmp776,
sizeof(char),40))),_tag_dyneither(_tmp775,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp777;struct
_tuple0 _tmp3D=(_tmp777.f1=Cyc_Tcutil_compress(t),((_tmp777.f2=r,_tmp777)));void*
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
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp77A;void*
_tmp779;(_tmp779=0,Cyc_Tcutil_warn(loc,((_tmp77A="argument may still contain unique pointers",
_tag_dyneither(_tmp77A,sizeof(char),43))),_tag_dyneither(_tmp779,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp77D;void*
_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(loc,((_tmp77D="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp77D,sizeof(char),47))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp780;void*_tmp77F;(_tmp77F=0,Cyc_Tcutil_warn(
loc,((_tmp780="argument may contain live unique pointers",_tag_dyneither(_tmp780,
sizeof(char),42))),_tag_dyneither(_tmp77F,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple19 _tmp781;return(
_tmp781.f1=inflow,((_tmp781.f2=0,_tmp781)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp784;struct _tuple19 _tmp783;return(_tmp783.f1=
_tmp7C,((_tmp783.f2=((_tmp784=_region_malloc(rgn,sizeof(*_tmp784)),((_tmp784->hd=
_tmp7D,((_tmp784->tl=0,_tmp784)))))),_tmp783)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp785;env->all_changed=((
_tmp785=_region_malloc(env->r,sizeof(*_tmp785)),((_tmp785[0]=(env->fenv)->mt_place_set,
_tmp785))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp786;rvals=((_tmp786=_region_malloc(rgn,sizeof(*_tmp786)),((_tmp786->hd=
_tmp83,((_tmp786->tl=0,_tmp786))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp787;env->all_changed=((_tmp787=_region_malloc(env->r,sizeof(*_tmp787)),((
_tmp787[0]=(env->fenv)->mt_place_set,_tmp787))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp788;rvals=((_tmp788=_region_malloc(rgn,
sizeof(*_tmp788)),((_tmp788->hd=_tmp88,((_tmp788->tl=rvals,_tmp788))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp789;env->all_changed=((
_tmp789=_region_malloc(env->r,sizeof(*_tmp789)),((_tmp789[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp789))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp78A;return(_tmp78A.f1=outflow,((_tmp78A.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp78A)));};};}
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
const char*_tmp78D;void*_tmp78C;(_tmp78C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp78D="expression may not be initialized",
_tag_dyneither(_tmp78D,sizeof(char),34))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple13 _tmp78E;return(_tmp78E.f1=_tmp8E,((
_tmp78E.f2=(env->fenv)->unknown_all,_tmp78E)));};}static union Cyc_CfFlowInfo_FlowInfo
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
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp78F;return(_tmp78F.f1=outflow,((
_tmp78F.f2=outflow,_tmp78F)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp790;
return(_tmp790.f1=outflow,((_tmp790.f2=outflow,_tmp790)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp791;return(_tmp791.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp791.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp791)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
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
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp792;return(_tmp792.f1=
f,((_tmp792.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp792)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
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
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp795;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp794;e->annot=(void*)((_tmp794=_cycalloc(sizeof(*_tmp794)),((_tmp794[0]=((
_tmp795.tag=Cyc_CfFlowInfo_NotZero,((_tmp795.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp795)))),_tmp794))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp798;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp797;e->annot=(void*)((_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797[0]=((
_tmp798.tag=Cyc_CfFlowInfo_NotZero,((_tmp798.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp798)))),_tmp797))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp799;return(_tmp799.f1=f,((
_tmp799.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp799)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp79A;return(_tmp79A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp79A.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp79A)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp79D;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp79C;e->annot=(void*)((_tmp79C=
_cycalloc(sizeof(*_tmp79C)),((_tmp79C[0]=((_tmp79D.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp79D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp79D)))),
_tmp79C))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp7A0;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp79F;e->annot=(void*)((_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F[0]=((
_tmp7A0.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7A0.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp7A0)))),_tmp79F))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp7A3;void*_tmp7A2;(
_tmp7A2=0,Cyc_Tcutil_terr(e->loc,((_tmp7A3="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp7A3,sizeof(char),61))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_Tcutil_terr(
e->loc,((_tmp7A6="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7A6,sizeof(char),46))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp7A7;return(_tmp7A7.f1=
f,((_tmp7A7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp7A7)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp7A8;return(
_tmp7A8.f1=f,((_tmp7A8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_none),
_tmp7A8)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp7AE;
const char*_tmp7AD;struct Cyc_Core_Impossible_struct*_tmp7AC;(int)_throw((void*)((
_tmp7AC=_cycalloc(sizeof(*_tmp7AC)),((_tmp7AC[0]=((_tmp7AE.tag=Cyc_Core_Impossible,((
_tmp7AE.f1=((_tmp7AD="right deref of non-pointer-type",_tag_dyneither(_tmp7AD,
sizeof(char),32))),_tmp7AE)))),_tmp7AC)))));}_LL58:;}static struct Cyc_List_List*
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
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7B1;
struct Cyc_List_List*_tmp7B0;return(_tmp7B0=_region_malloc(rgn,sizeof(*_tmp7B0)),((
_tmp7B0->hd=((_tmp7B1=_region_malloc(rgn,sizeof(*_tmp7B1)),((_tmp7B1->vd=_tmp107,((
_tmp7B1->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp7B1)))))),((_tmp7B0->tl=
relns,_tmp7B0)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp7B2;return(_tmp7B2.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7B2.f2=rval,_tmp7B2)));}_LLB4: if((_tmp10E.ReachableFL).tag
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
_tmp7B5;void*_tmp7B4;(_tmp7B4=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7B5="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7B5,sizeof(char),43))),_tag_dyneither(_tmp7B4,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp7B6;return(_tmp7B6.f1=outflow,((_tmp7B6.f2=rval,_tmp7B6)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp7B7;return(_tmp7B7.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7B7.f2=rval,_tmp7B7)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
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
_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct _tuple14 _tmp7B8;struct _tuple14
_tmp150=(_tmp7B8.f1=_tmp14D,((_tmp7B8.f2=_tmp14E,_tmp7B8)));union Cyc_CfFlowInfo_FlowInfo
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
t,0,leaf);_tmp14D=Cyc_NewControlFlow_use_Rval(env,_tmp143->loc,_tmp14D,_tmp159);{
struct Cyc_List_List _tmp7B9;struct Cyc_List_List _tmp15F=(_tmp7B9.hd=_tmp145,((
_tmp7B9.tl=0,_tmp7B9)));_tmp14D=Cyc_NewControlFlow_add_vars(_tmp137,_tmp14D,(
struct Cyc_List_List*)& _tmp15F,_tmp137->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp161;void*_tmp162;struct _tuple13 _tmp160=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14D,_tmp146);_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp163=_tmp161;struct _tuple12 _tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*
_tmp166;_LLF8: if((_tmp163.ReachableFL).tag != 2)goto _LLFA;_tmp164=(struct _tuple12)(
_tmp163.ReachableFL).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LLF9: _tmp165=Cyc_CfFlowInfo_assign_place(
_tmp137,s->loc,_tmp165,env->all_changed,_tmp156,_tmp15E);{union Cyc_CfFlowInfo_FlowInfo
_tmp167=Cyc_CfFlowInfo_ReachableFL(_tmp165,_tmp166);struct _tuple13 _tmp7BA;return(
_tmp7BA.f1=_tmp167,((_tmp7BA.f2=_tmp162,_tmp7BA)));};_LLFA:;_LLFB: {struct
_tuple13 _tmp7BB;return(_tmp7BB.f1=_tmp161,((_tmp7BB.f2=_tmp162,_tmp7BB)));}_LLF7:;};};};};};};}
_LLEA: _tmp157=_tmp150.f1;if((_tmp157.BottomFL).tag != 1)goto _LLEC;_tmp158=(int)(
_tmp157.BottomFL).val;_LLEB: {struct _tuple13 _tmp7BC;return(_tmp7BC.f1=_tmp14D,((
_tmp7BC.f2=_tmp137->unknown_all,_tmp7BC)));}_LLEC:;_LLED:{const char*_tmp7BF;void*
_tmp7BE;(_tmp7BE=0,Cyc_Tcutil_terr(_tmp143->loc,((_tmp7BF="bad alias expression--no unique path found",
_tag_dyneither(_tmp7BF,sizeof(char),43))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}{
struct _tuple13 _tmp7C0;return(_tmp7C0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C0.f2=
_tmp137->unknown_all,_tmp7C0)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp147=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp147->tag != 12)goto _LLE3;
else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp148);s=_tmp149;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14A=(struct Cyc_Absyn_Exp_s_struct*)_tmp13A;if(_tmp14A->tag != 1)goto _LLE5;
else{_tmp14B=_tmp14A->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14B);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7C6;
const char*_tmp7C5;struct Cyc_Core_Impossible_struct*_tmp7C4;(int)_throw((void*)((
_tmp7C4=_cycalloc(sizeof(*_tmp7C4)),((_tmp7C4[0]=((_tmp7C6.tag=Cyc_Core_Impossible,((
_tmp7C6.f1=((_tmp7C5="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7C5,
sizeof(char),33))),_tmp7C6)))),_tmp7C4)))));}_LLDC:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp86B(unsigned int*_tmp86A,unsigned int*_tmp869,void***_tmp867,
struct Cyc_List_List**_tmp363){for(*_tmp86A=0;*_tmp86A < *_tmp869;(*_tmp86A)++){
void*_tmp865;(*_tmp867)[*_tmp86A]=((_tmp865=(void*)((struct Cyc_List_List*)
_check_null(*_tmp363))->hd,((*_tmp363=(*_tmp363)->tl,_tmp865))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp172=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp173=inflow;int _tmp174;struct _tuple12 _tmp175;struct Cyc_Dict_Dict _tmp176;
struct Cyc_List_List*_tmp177;_LLFD: if((_tmp173.BottomFL).tag != 1)goto _LLFF;
_tmp174=(int)(_tmp173.BottomFL).val;_LLFE: {struct _tuple13 _tmp7C7;return(_tmp7C7.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7C7.f2=_tmp172->unknown_all,_tmp7C7)));}_LLFF:
if((_tmp173.ReachableFL).tag != 2)goto _LLFC;_tmp175=(struct _tuple12)(_tmp173.ReachableFL).val;
_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL100: d=_tmp176;relns=_tmp177;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17A;union Cyc_CfFlowInfo_AbsLVal _tmp17B;struct _tuple14 _tmp179=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tuple14 _tmp7C8;
struct _tuple14 _tmp17D=(_tmp7C8.f1=_tmp17A,((_tmp7C8.f2=_tmp17B,_tmp7C8)));union
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
struct _tuple13 _tmp7C9;return(_tmp7C9.f1=_tmp17A,((_tmp7C9.f2=_tmp189,_tmp7C9)));}{
const char*_tmp7CC;void*_tmp7CB;(_tmp7CB=0,Cyc_Tcutil_terr(e->loc,((_tmp7CC="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7CC,sizeof(char),71))),_tag_dyneither(_tmp7CB,sizeof(void*),0)));}{
struct _tuple13 _tmp7CD;return(_tmp7CD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CD.f2=
_tmp172->unknown_all,_tmp7CD)));};_LL109:;_LL10A: if(Cyc_CfFlowInfo_is_unique_consumed(
e,env->iteration_num,_tmp184)){{const char*_tmp7D0;void*_tmp7CF;(_tmp7CF=0,Cyc_Tcutil_terr(
e->loc,((_tmp7D0="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7D0,sizeof(char),71))),_tag_dyneither(_tmp7CF,sizeof(void*),0)));}{
struct _tuple13 _tmp7D1;return(_tmp7D1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D1.f2=
_tmp172->unknown_all,_tmp7D1)));};}goto _LL106;_LL106:;}{void*_tmp191=Cyc_CfFlowInfo_make_unique_consumed(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,e,env->iteration_num,
_tmp184);struct Cyc_Dict_Dict _tmp192=Cyc_CfFlowInfo_assign_place(_tmp172,e->loc,
_tmp180,env->all_changed,_tmp183,_tmp191);struct _tuple13 _tmp7D2;return(_tmp7D2.f1=
Cyc_CfFlowInfo_ReachableFL(_tmp192,_tmp181),((_tmp7D2.f2=_tmp184,_tmp7D2)));};}
_LL104:;_LL105: goto _LL101;_LL101:;};}{void*_tmp194=e->r;struct Cyc_Absyn_Exp*
_tmp196;enum Cyc_Absyn_Coercion _tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*
_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;union Cyc_Absyn_Cnst _tmp19F;int _tmp1A0;union
Cyc_Absyn_Cnst _tmp1A2;struct _tuple6 _tmp1A3;int _tmp1A4;union Cyc_Absyn_Cnst _tmp1A6;
struct _tuple6 _tmp1A7;void*_tmp1A9;struct Cyc_List_List*_tmp1AC;void*_tmp1B4;void*
_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B9;void*_tmp1BB;struct Cyc_Absyn_Vardecl*
_tmp1BD;void*_tmp1BF;struct Cyc_Absyn_Vardecl*_tmp1C1;enum Cyc_Absyn_Primop
_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*
_tmp1C8;struct Cyc_Core_Opt*_tmp1C9;struct Cyc_Core_Opt _tmp1CA;struct Cyc_Absyn_Exp*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Core_Opt*_tmp1CE;struct Cyc_Absyn_Exp*
_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_MallocInfo
_tmp1D9;int _tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;void**_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DD;int _tmp1DE;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;
struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1EA;struct
_dyneither_ptr*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1ED;struct _dyneither_ptr*_tmp1EE;
struct Cyc_Absyn_Exp*_tmp1F0;struct _dyneither_ptr*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_List_List*
_tmp200;struct Cyc_Absyn_Datatypedecl*_tmp201;struct Cyc_List_List*_tmp203;struct
Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_Aggrdecl*
_tmp208;struct Cyc_Absyn_Aggrdecl _tmp209;enum Cyc_Absyn_AggrKind _tmp20A;struct Cyc_Absyn_AggrdeclImpl*
_tmp20B;struct Cyc_Absyn_AggrdeclImpl _tmp20C;struct Cyc_List_List*_tmp20D;struct
Cyc_List_List*_tmp210;struct Cyc_Absyn_Vardecl*_tmp212;struct Cyc_Absyn_Exp*
_tmp213;struct Cyc_Absyn_Exp*_tmp214;int _tmp215;struct Cyc_Absyn_Stmt*_tmp217;void*
_tmp219;_LL10C: {struct Cyc_Absyn_Cast_e_struct*_tmp195=(struct Cyc_Absyn_Cast_e_struct*)
_tmp194;if(_tmp195->tag != 15)goto _LL10E;else{_tmp196=_tmp195->f2;_tmp197=_tmp195->f4;
if(_tmp197 != Cyc_Absyn_NonNull_to_Null)goto _LL10E;}}_LL10D: {union Cyc_CfFlowInfo_FlowInfo
_tmp221;void*_tmp222;struct _tuple13 _tmp220=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp196);_tmp221=_tmp220.f1;_tmp222=_tmp220.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp224;void*_tmp225;struct _tuple13 _tmp223=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp221,_tmp196,_tmp222);_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;{struct
_tuple13 _tmp7D3;return(_tmp7D3.f1=_tmp224,((_tmp7D3.f2=_tmp222,_tmp7D3)));};};}
_LL10E: {struct Cyc_Absyn_Cast_e_struct*_tmp198=(struct Cyc_Absyn_Cast_e_struct*)
_tmp194;if(_tmp198->tag != 15)goto _LL110;else{_tmp199=_tmp198->f2;}}_LL10F:
_tmp19B=_tmp199;goto _LL111;_LL110: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp19A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp194;if(_tmp19A->tag != 13)
goto _LL112;else{_tmp19B=_tmp19A->f1;}}_LL111: _tmp19D=_tmp19B;goto _LL113;_LL112: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp19C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp194;if(_tmp19C->tag != 14)goto _LL114;else{_tmp19D=_tmp19C->f1;}}_LL113: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19D);_LL114: {struct Cyc_Absyn_Const_e_struct*
_tmp19E=(struct Cyc_Absyn_Const_e_struct*)_tmp194;if(_tmp19E->tag != 0)goto _LL116;
else{_tmp19F=_tmp19E->f1;if((_tmp19F.Null_c).tag != 1)goto _LL116;_tmp1A0=(int)(
_tmp19F.Null_c).val;}}_LL115: goto _LL117;_LL116: {struct Cyc_Absyn_Const_e_struct*
_tmp1A1=(struct Cyc_Absyn_Const_e_struct*)_tmp194;if(_tmp1A1->tag != 0)goto _LL118;
else{_tmp1A2=_tmp1A1->f1;if((_tmp1A2.Int_c).tag != 4)goto _LL118;_tmp1A3=(struct
_tuple6)(_tmp1A2.Int_c).val;_tmp1A4=_tmp1A3.f2;if(_tmp1A4 != 0)goto _LL118;}}
_LL117: {struct _tuple13 _tmp7D4;return(_tmp7D4.f1=inflow,((_tmp7D4.f2=_tmp172->zero,
_tmp7D4)));}_LL118: {struct Cyc_Absyn_Const_e_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_struct*)
_tmp194;if(_tmp1A5->tag != 0)goto _LL11A;else{_tmp1A6=_tmp1A5->f1;if((_tmp1A6.Int_c).tag
!= 4)goto _LL11A;_tmp1A7=(struct _tuple6)(_tmp1A6.Int_c).val;}}_LL119: goto _LL11B;
_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp1A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1A8->tag != 1)goto _LL11C;else{_tmp1A9=(void*)_tmp1A8->f2;{struct
Cyc_Absyn_Funname_b_struct*_tmp1AA=(struct Cyc_Absyn_Funname_b_struct*)_tmp1A9;
if(_tmp1AA->tag != 2)goto _LL11C;};}}_LL11B: {struct _tuple13 _tmp7D5;return(_tmp7D5.f1=
inflow,((_tmp7D5.f2=_tmp172->notzeroall,_tmp7D5)));}_LL11C: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1AB=(struct Cyc_Absyn_Datatype_e_struct*)_tmp194;if(_tmp1AB->tag != 31)goto
_LL11E;else{_tmp1AC=_tmp1AB->f1;if(_tmp1AC != 0)goto _LL11E;}}_LL11D: goto _LL11F;
_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp1AD=(struct Cyc_Absyn_Const_e_struct*)
_tmp194;if(_tmp1AD->tag != 0)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1AE=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp194;if(_tmp1AE->tag != 19)goto
_LL122;}_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1AF=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp194;if(_tmp1AF->tag != 18)goto _LL124;}
_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1B0=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp194;if(_tmp1B0->tag != 20)goto _LL126;}_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1B1=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp194;if(_tmp1B1->tag != 33)goto
_LL128;}_LL127: goto _LL129;_LL128: {struct Cyc_Absyn_Enum_e_struct*_tmp1B2=(struct
Cyc_Absyn_Enum_e_struct*)_tmp194;if(_tmp1B2->tag != 32)goto _LL12A;}_LL129: {
struct _tuple13 _tmp7D6;return(_tmp7D6.f1=inflow,((_tmp7D6.f2=_tmp172->unknown_all,
_tmp7D6)));}_LL12A: {struct Cyc_Absyn_Var_e_struct*_tmp1B3=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1B3->tag != 1)goto _LL12C;else{_tmp1B4=(void*)_tmp1B3->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1B5=(struct Cyc_Absyn_Global_b_struct*)_tmp1B4;if(
_tmp1B5->tag != 1)goto _LL12C;};}}_LL12B: {struct _tuple13 _tmp7D7;return(_tmp7D7.f1=
inflow,((_tmp7D7.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7D7)));}_LL12C: {struct Cyc_Absyn_Var_e_struct*
_tmp1B6=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1B6->tag != 1)goto _LL12E;
else{_tmp1B7=(void*)_tmp1B6->f2;{struct Cyc_Absyn_Param_b_struct*_tmp1B8=(struct
Cyc_Absyn_Param_b_struct*)_tmp1B7;if(_tmp1B8->tag != 3)goto _LL12E;else{_tmp1B9=
_tmp1B8->f1;}};}}_LL12D: _tmp1BD=_tmp1B9;goto _LL12F;_LL12E: {struct Cyc_Absyn_Var_e_struct*
_tmp1BA=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1BA->tag != 1)goto _LL130;
else{_tmp1BB=(void*)_tmp1BA->f2;{struct Cyc_Absyn_Local_b_struct*_tmp1BC=(struct
Cyc_Absyn_Local_b_struct*)_tmp1BB;if(_tmp1BC->tag != 4)goto _LL130;else{_tmp1BD=
_tmp1BC->f1;}};}}_LL12F: _tmp1C1=_tmp1BD;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp1BE=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1BE->tag != 1)goto _LL132;
else{_tmp1BF=(void*)_tmp1BE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp1C0=(struct Cyc_Absyn_Pat_b_struct*)
_tmp1BF;if(_tmp1C0->tag != 5)goto _LL132;else{_tmp1C1=_tmp1C0->f1;}};}}_LL131: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7DD;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7DC;struct _tuple13 _tmp7DB;return(_tmp7DB.f1=inflow,((_tmp7DB.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7DD=_region_malloc(
env->r,sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DC.tag=0,((_tmp7DC.f1=_tmp1C1,
_tmp7DC)))),_tmp7DD))))),_tmp7DB)));}_LL132: {struct Cyc_Absyn_Primop_e_struct*
_tmp1C2=(struct Cyc_Absyn_Primop_e_struct*)_tmp194;if(_tmp1C2->tag != 3)goto _LL134;
else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL133: {union Cyc_CfFlowInfo_FlowInfo
_tmp22F;void*_tmp230;struct _tuple13 _tmp22E=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1C4);_tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;{struct _tuple13 _tmp7DE;
return(_tmp7DE.f1=_tmp22F,((_tmp7DE.f2=_tmp230,_tmp7DE)));};}_LL134: {struct Cyc_Absyn_Increment_e_struct*
_tmp1C5=(struct Cyc_Absyn_Increment_e_struct*)_tmp194;if(_tmp1C5->tag != 5)goto
_LL136;else{_tmp1C6=_tmp1C5->f1;}}_LL135: {struct Cyc_List_List _tmp7DF;struct Cyc_List_List
_tmp232=(_tmp7DF.hd=_tmp1C6,((_tmp7DF.tl=0,_tmp7DF)));union Cyc_CfFlowInfo_FlowInfo
_tmp234;struct _tuple13 _tmp233=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp232);_tmp234=_tmp233.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp236;struct _tuple14 _tmp235=Cyc_NewControlFlow_anal_Lexp(env,_tmp234,0,_tmp1C6);
_tmp236=_tmp235.f2;{struct _tuple20 _tmp7E0;struct _tuple20 _tmp238=(_tmp7E0.f1=
_tmp236,((_tmp7E0.f2=_tmp234,_tmp7E0)));union Cyc_CfFlowInfo_AbsLVal _tmp239;
struct Cyc_CfFlowInfo_Place*_tmp23A;union Cyc_CfFlowInfo_FlowInfo _tmp23B;struct
_tuple12 _tmp23C;struct Cyc_Dict_Dict _tmp23D;struct Cyc_List_List*_tmp23E;_LL175:
_tmp239=_tmp238.f1;if((_tmp239.PlaceL).tag != 1)goto _LL177;_tmp23A=(struct Cyc_CfFlowInfo_Place*)(
_tmp239.PlaceL).val;_tmp23B=_tmp238.f2;if((_tmp23B.ReachableFL).tag != 2)goto
_LL177;_tmp23C=(struct _tuple12)(_tmp23B.ReachableFL).val;_tmp23D=_tmp23C.f1;
_tmp23E=_tmp23C.f2;_LL176: _tmp23E=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp23E,_tmp1C6);_tmp234=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C6->loc,_tmp23D,env->all_changed,_tmp23A,_tmp172->unknown_all),
_tmp23E);goto _LL174;_LL177:;_LL178: goto _LL174;_LL174:;}{struct _tuple13 _tmp7E1;
return(_tmp7E1.f1=_tmp234,((_tmp7E1.f2=_tmp172->unknown_all,_tmp7E1)));};};}
_LL136: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C7=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1C7->tag != 4)goto _LL138;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;
if(_tmp1C9 == 0)goto _LL138;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1C7->f3;}}_LL137: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7E4;void*_tmp7E3;(_tmp7E3=0,Cyc_Tcutil_terr(
e->loc,((_tmp7E4="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7E4,sizeof(char),58))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}{
struct _tuple13 _tmp7E5;return(_tmp7E5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E5.f2=
_tmp172->unknown_all,_tmp7E5)));};}{struct Cyc_List_List _tmp7E6;struct Cyc_List_List
_tmp244=(_tmp7E6.hd=_tmp1CB,((_tmp7E6.tl=0,_tmp7E6)));struct Cyc_List_List _tmp7E7;
struct Cyc_List_List _tmp245=(_tmp7E7.hd=_tmp1C8,((_tmp7E7.tl=(struct Cyc_List_List*)&
_tmp244,_tmp7E7)));union Cyc_CfFlowInfo_FlowInfo _tmp247;struct _tuple13 _tmp246=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp245);_tmp247=_tmp246.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp249;struct _tuple14 _tmp248=Cyc_NewControlFlow_anal_Lexp(env,_tmp247,0,_tmp1C8);
_tmp249=_tmp248.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp24A=_tmp247;struct _tuple12
_tmp24B;struct Cyc_Dict_Dict _tmp24C;struct Cyc_List_List*_tmp24D;_LL17A: if((
_tmp24A.ReachableFL).tag != 2)goto _LL17C;_tmp24B=(struct _tuple12)(_tmp24A.ReachableFL).val;
_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;_LL17B:{union Cyc_CfFlowInfo_AbsLVal _tmp24E=
_tmp249;struct Cyc_CfFlowInfo_Place*_tmp24F;int _tmp250;_LL17F: if((_tmp24E.PlaceL).tag
!= 1)goto _LL181;_tmp24F=(struct Cyc_CfFlowInfo_Place*)(_tmp24E.PlaceL).val;_LL180:
_tmp24D=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,_tmp24D,_tmp1C8);_tmp24C=Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C8->loc,_tmp24C,env->all_changed,_tmp24F,_tmp172->unknown_all);
_tmp247=Cyc_CfFlowInfo_ReachableFL(_tmp24C,_tmp24D);goto _LL17E;_LL181: if((
_tmp24E.UnknownL).tag != 2)goto _LL17E;_tmp250=(int)(_tmp24E.UnknownL).val;_LL182:
goto _LL17E;_LL17E:;}goto _LL179;_LL17C:;_LL17D: goto _LL179;_LL179:;}{struct
_tuple13 _tmp7E8;return(_tmp7E8.f1=_tmp247,((_tmp7E8.f2=_tmp172->unknown_all,
_tmp7E8)));};};};_LL138: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1CC=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1CC->tag != 4)goto _LL13A;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;
if(_tmp1CE != 0)goto _LL13A;_tmp1CF=_tmp1CC->f3;}}_LL139: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp7EB;void*_tmp7EA;(_tmp7EA=0,Cyc_Tcutil_terr(e->loc,((_tmp7EB="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7EB,sizeof(char),58))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}{
struct _tuple13 _tmp7EC;return(_tmp7EC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EC.f2=
_tmp172->unknown_all,_tmp7EC)));};}{struct Cyc_Dict_Dict*_tmp257=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7ED;env->all_changed=((_tmp7ED=_region_malloc(
env->r,sizeof(*_tmp7ED)),((_tmp7ED[0]=_tmp172->mt_place_set,_tmp7ED))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp25A;union Cyc_CfFlowInfo_AbsLVal _tmp25B;struct _tuple14
_tmp259=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1CD);_tmp25A=_tmp259.f1;
_tmp25B=_tmp259.f2;{struct Cyc_Dict_Dict _tmp25C=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7EE;env->all_changed=((
_tmp7EE=_region_malloc(env->r,sizeof(*_tmp7EE)),((_tmp7EE[0]=_tmp172->mt_place_set,
_tmp7EE))));}{union Cyc_CfFlowInfo_FlowInfo _tmp25F;void*_tmp260;struct _tuple13
_tmp25E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CF);_tmp25F=_tmp25E.f1;
_tmp260=_tmp25E.f2;{struct Cyc_Dict_Dict _tmp261=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp262=Cyc_CfFlowInfo_after_flow(
_tmp172,(struct Cyc_Dict_Dict*)& _tmp25C,_tmp25A,_tmp25F,_tmp25C,_tmp261);union Cyc_CfFlowInfo_FlowInfo
_tmp263=Cyc_CfFlowInfo_join_flow(_tmp172,_tmp257,inflow,_tmp262);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp263,inflow)){if(_tmp257 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp257,Cyc_CfFlowInfo_union_place_set(
_tmp25C,_tmp261,0),0);}return Cyc_NewControlFlow_do_assign(_tmp172,env,_tmp262,
_tmp1CD,_tmp25B,_tmp1CF,_tmp260,e->loc);}inflow=_tmp263;};};};};}};_LL13A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1D0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp194;if(_tmp1D0->tag != 9)goto _LL13C;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}
_LL13B: {union Cyc_CfFlowInfo_FlowInfo _tmp265;void*_tmp266;struct _tuple13 _tmp264=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1D1);_tmp265=_tmp264.f1;_tmp266=
_tmp264.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp265,_tmp1D2);}
_LL13C: {struct Cyc_Absyn_Throw_e_struct*_tmp1D3=(struct Cyc_Absyn_Throw_e_struct*)
_tmp194;if(_tmp1D3->tag != 12)goto _LL13E;else{_tmp1D4=_tmp1D3->f1;}}_LL13D: {
union Cyc_CfFlowInfo_FlowInfo _tmp268;void*_tmp269;struct _tuple13 _tmp267=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1D4);_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1D4->loc,_tmp268,_tmp269);{struct _tuple13 _tmp7EF;return(_tmp7EF.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7EF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7EF)));};}_LL13E: {struct
Cyc_Absyn_FnCall_e_struct*_tmp1D5=(struct Cyc_Absyn_FnCall_e_struct*)_tmp194;if(
_tmp1D5->tag != 11)goto _LL140;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}
_LL13F: {struct _RegionHandle*_tmp26B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp26E;
struct Cyc_List_List*_tmp26F;struct Cyc_List_List*_tmp7F0;struct _tuple19 _tmp26D=
Cyc_NewControlFlow_anal_unordered_Rexps(_tmp26B,env,inflow,((_tmp7F0=
_region_malloc(_tmp26B,sizeof(*_tmp7F0)),((_tmp7F0->hd=_tmp1D6,((_tmp7F0->tl=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp26B,_tmp1D7),_tmp7F0)))))),0,1);_tmp26E=_tmp26D.f1;_tmp26F=_tmp26D.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp270=Cyc_NewControlFlow_use_Rval(env,_tmp1D6->loc,
_tmp26E,(void*)((struct Cyc_List_List*)_check_null(_tmp26F))->hd);_tmp26F=_tmp26F->tl;{
struct Cyc_List_List*init_params=0;struct Cyc_List_List*nolive_unique_params=0;{
void*_tmp271=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1D6->topt))->v);struct Cyc_Absyn_PtrInfo _tmp273;void*_tmp274;_LL184: {struct
Cyc_Absyn_PointerType_struct*_tmp272=(struct Cyc_Absyn_PointerType_struct*)
_tmp271;if(_tmp272->tag != 5)goto _LL186;else{_tmp273=_tmp272->f1;_tmp274=_tmp273.elt_typ;}}
_LL185:{void*_tmp275=Cyc_Tcutil_compress(_tmp274);struct Cyc_Absyn_FnInfo _tmp277;
struct Cyc_List_List*_tmp278;_LL189: {struct Cyc_Absyn_FnType_struct*_tmp276=(
struct Cyc_Absyn_FnType_struct*)_tmp275;if(_tmp276->tag != 10)goto _LL18B;else{
_tmp277=_tmp276->f1;_tmp278=_tmp277.attributes;}}_LL18A: for(0;_tmp278 != 0;
_tmp278=_tmp278->tl){void*_tmp279=(void*)_tmp278->hd;int _tmp27B;int _tmp27D;
_LL18E: {struct Cyc_Absyn_Initializes_att_struct*_tmp27A=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp279;if(_tmp27A->tag != 20)goto _LL190;else{_tmp27B=_tmp27A->f1;}}_LL18F:{
struct Cyc_List_List*_tmp7F1;init_params=((_tmp7F1=_region_malloc(_tmp26B,sizeof(*
_tmp7F1)),((_tmp7F1->hd=(void*)_tmp27B,((_tmp7F1->tl=init_params,_tmp7F1))))));}
goto _LL18D;_LL190: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp27C=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp279;if(_tmp27C->tag != 21)goto _LL192;else{_tmp27D=_tmp27C->f1;}}_LL191:{
struct Cyc_List_List*_tmp7F2;nolive_unique_params=((_tmp7F2=_region_malloc(
_tmp26B,sizeof(*_tmp7F2)),((_tmp7F2->hd=(void*)_tmp27D,((_tmp7F2->tl=
nolive_unique_params,_tmp7F2))))));}goto _LL18D;_LL192:;_LL193: goto _LL18D;_LL18D:;}
goto _LL188;_LL18B:;_LL18C: {const char*_tmp7F5;void*_tmp7F4;(_tmp7F4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F5="anal_Rexp: bad function type",
_tag_dyneither(_tmp7F5,sizeof(char),29))),_tag_dyneither(_tmp7F4,sizeof(void*),0)));}
_LL188:;}goto _LL183;_LL186:;_LL187: {const char*_tmp7F8;void*_tmp7F7;(_tmp7F7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7F8="anal_Rexp: bad function type",_tag_dyneither(_tmp7F8,sizeof(char),29))),
_tag_dyneither(_tmp7F7,sizeof(void*),0)));}_LL183:;}{int i=1;for(0;_tmp26F != 0;(((
_tmp26F=_tmp26F->tl,_tmp1D7=((struct Cyc_List_List*)_check_null(_tmp1D7))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp270=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->loc,_tmp270,(void*)_tmp26F->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp270=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->topt))->v,_tmp270,(void*)_tmp26F->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp284=_tmp26E;int _tmp285;struct _tuple12 _tmp286;
struct Cyc_Dict_Dict _tmp287;_LL195: if((_tmp284.BottomFL).tag != 1)goto _LL197;
_tmp285=(int)(_tmp284.BottomFL).val;_LL196: goto _LL194;_LL197: if((_tmp284.ReachableFL).tag
!= 2)goto _LL194;_tmp286=(struct _tuple12)(_tmp284.ReachableFL).val;_tmp287=
_tmp286.f1;_LL198: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp287,(void*)_tmp26F->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd)->loc,((
_tmp7FB="expression may not be initialized",_tag_dyneither(_tmp7FB,sizeof(char),
34))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp28A=_tmp270;int _tmp28B;struct _tuple12 _tmp28C;struct Cyc_Dict_Dict _tmp28D;
struct Cyc_List_List*_tmp28E;_LL19A: if((_tmp28A.BottomFL).tag != 1)goto _LL19C;
_tmp28B=(int)(_tmp28A.BottomFL).val;_LL19B: goto _LL199;_LL19C: if((_tmp28A.ReachableFL).tag
!= 2)goto _LL199;_tmp28C=(struct _tuple12)(_tmp28A.ReachableFL).val;_tmp28D=
_tmp28C.f1;_tmp28E=_tmp28C.f2;_LL19D: {struct Cyc_Dict_Dict _tmp28F=Cyc_CfFlowInfo_escape_deref(
_tmp172,_tmp28D,env->all_changed,(void*)_tmp26F->hd);{void*_tmp290=(void*)
_tmp26F->hd;struct Cyc_CfFlowInfo_Place*_tmp292;_LL19F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp291=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp290;if(_tmp291->tag != 5)goto
_LL1A1;else{_tmp292=_tmp291->f1;}}_LL1A0:{void*_tmp293=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp295;void*
_tmp296;_LL1A4: {struct Cyc_Absyn_PointerType_struct*_tmp294=(struct Cyc_Absyn_PointerType_struct*)
_tmp293;if(_tmp294->tag != 5)goto _LL1A6;else{_tmp295=_tmp294->f1;_tmp296=_tmp295.elt_typ;}}
_LL1A5: _tmp28F=Cyc_CfFlowInfo_assign_place(_tmp172,((struct Cyc_Absyn_Exp*)
_tmp1D7->hd)->loc,_tmp28F,env->all_changed,_tmp292,Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,_tmp296,0,_tmp172->esc_all));goto _LL1A3;_LL1A6:;_LL1A7: {const char*
_tmp7FE;void*_tmp7FD;(_tmp7FD=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FE="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7FE,sizeof(char),39))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}
_LL1A3:;}goto _LL19E;_LL1A1:;_LL1A2: goto _LL19E;_LL19E:;}_tmp270=Cyc_CfFlowInfo_ReachableFL(
_tmp28F,_tmp28E);goto _LL199;}_LL199:;}goto _LL194;_LL194:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6->topt))->v)){struct _tuple13
_tmp7FF;return(_tmp7FF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FF.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp7FF)));}else{struct _tuple13 _tmp800;return(_tmp800.f1=_tmp270,((_tmp800.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0,_tmp172->unknown_all),_tmp800)));}};};}_LL140: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D8=(struct Cyc_Absyn_Malloc_e_struct*)_tmp194;if(_tmp1D8->tag != 34)goto
_LL142;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D9.is_calloc;_tmp1DB=_tmp1D9.rgn;
_tmp1DC=_tmp1D9.elt_type;_tmp1DD=_tmp1D9.num_elts;_tmp1DE=_tmp1D9.fat_result;}}
_LL141: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp803;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp802;void*root=(void*)((_tmp802=_region_malloc(_tmp172->r,sizeof(*_tmp802)),((
_tmp802[0]=((_tmp803.tag=1,((_tmp803.f1=_tmp1DD,((_tmp803.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp803)))))),_tmp802))));struct
Cyc_CfFlowInfo_Place*_tmp804;struct Cyc_CfFlowInfo_Place*place=(_tmp804=
_region_malloc(_tmp172->r,sizeof(*_tmp804)),((_tmp804->root=root,((_tmp804->fields=
0,_tmp804)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp807;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp806;void*rval=(void*)((_tmp806=_region_malloc(_tmp172->r,sizeof(*_tmp806)),((
_tmp806[0]=((_tmp807.tag=5,((_tmp807.f1=place,_tmp807)))),_tmp806))));void*
place_val;if(_tmp1DE)place_val=_tmp172->notzeroall;else{if(_tmp1DA)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),0,_tmp172->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),0,_tmp172->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1DB != 0){struct _RegionHandle*_tmp29B=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp29E;struct Cyc_List_List*_tmp29F;struct Cyc_Absyn_Exp*_tmp808[2];struct
_tuple19 _tmp29D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp29B,env,inflow,((
_tmp808[1]=_tmp1DD,((_tmp808[0]=(struct Cyc_Absyn_Exp*)_tmp1DB,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp29B,
_tag_dyneither(_tmp808,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp29E=_tmp29D.f1;
_tmp29F=_tmp29D.f2;outflow=_tmp29E;}else{union Cyc_CfFlowInfo_FlowInfo _tmp2A1;
struct _tuple13 _tmp2A0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);_tmp2A1=
_tmp2A0.f1;outflow=_tmp2A1;}{union Cyc_CfFlowInfo_FlowInfo _tmp2A2=outflow;int
_tmp2A3;struct _tuple12 _tmp2A4;struct Cyc_Dict_Dict _tmp2A5;struct Cyc_List_List*
_tmp2A6;_LL1A9: if((_tmp2A2.BottomFL).tag != 1)goto _LL1AB;_tmp2A3=(int)(_tmp2A2.BottomFL).val;
_LL1AA: {struct _tuple13 _tmp809;return(_tmp809.f1=outflow,((_tmp809.f2=rval,
_tmp809)));}_LL1AB: if((_tmp2A2.ReachableFL).tag != 2)goto _LL1A8;_tmp2A4=(struct
_tuple12)(_tmp2A2.ReachableFL).val;_tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_LL1AC: {
struct _tuple13 _tmp80A;return(_tmp80A.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A5,root,place_val),
_tmp2A6),((_tmp80A.f2=rval,_tmp80A)));}_LL1A8:;};};}_LL142: {struct Cyc_Absyn_Swap_e_struct*
_tmp1DF=(struct Cyc_Absyn_Swap_e_struct*)_tmp194;if(_tmp1DF->tag != 35)goto _LL144;
else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LL143: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp2AE=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B1;struct Cyc_List_List*_tmp2B2;struct Cyc_Absyn_Exp*
_tmp80B[2];struct _tuple19 _tmp2B0=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AE,
env,inflow,((_tmp80B[1]=_tmp1E1,((_tmp80B[0]=_tmp1E0,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AE,
_tag_dyneither(_tmp80B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2B1=_tmp2B0.f1;
_tmp2B2=_tmp2B0.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2B2->tl))->hd;outflow=
_tmp2B1;}{union Cyc_CfFlowInfo_AbsLVal _tmp2B4;struct _tuple14 _tmp2B3=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1E0);_tmp2B4=_tmp2B3.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2B6;
struct _tuple14 _tmp2B5=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1E1);
_tmp2B6=_tmp2B5.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B7=outflow;struct _tuple12
_tmp2B8;struct Cyc_Dict_Dict _tmp2B9;struct Cyc_List_List*_tmp2BA;_LL1AE: if((
_tmp2B7.ReachableFL).tag != 2)goto _LL1B0;_tmp2B8=(struct _tuple12)(_tmp2B7.ReachableFL).val;
_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;_LL1AF:{union Cyc_CfFlowInfo_AbsLVal _tmp2BB=
_tmp2B4;struct Cyc_CfFlowInfo_Place*_tmp2BC;int _tmp2BD;_LL1B3: if((_tmp2BB.PlaceL).tag
!= 1)goto _LL1B5;_tmp2BC=(struct Cyc_CfFlowInfo_Place*)(_tmp2BB.PlaceL).val;_LL1B4:
_tmp2B9=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1E0->loc,_tmp2B9,env->all_changed,
_tmp2BC,right_rval);goto _LL1B2;_LL1B5: if((_tmp2BB.UnknownL).tag != 2)goto _LL1B2;
_tmp2BD=(int)(_tmp2BB.UnknownL).val;_LL1B6: goto _LL1B2;_LL1B2:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2BE=_tmp2B6;struct Cyc_CfFlowInfo_Place*_tmp2BF;int _tmp2C0;_LL1B8: if((_tmp2BE.PlaceL).tag
!= 1)goto _LL1BA;_tmp2BF=(struct Cyc_CfFlowInfo_Place*)(_tmp2BE.PlaceL).val;_LL1B9:
_tmp2B9=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1E1->loc,_tmp2B9,env->all_changed,
_tmp2BF,left_rval);goto _LL1B7;_LL1BA: if((_tmp2BE.UnknownL).tag != 2)goto _LL1B7;
_tmp2C0=(int)(_tmp2BE.UnknownL).val;_LL1BB: goto _LL1B7;_LL1B7:;}_tmp2BA=Cyc_CfFlowInfo_reln_kill_exp(
_tmp172->r,_tmp2BA,_tmp1E0);_tmp2BA=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp2BA,_tmp1E1);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B9,_tmp2BA);goto _LL1AD;
_LL1B0:;_LL1B1: goto _LL1AD;_LL1AD:;}{struct _tuple13 _tmp80C;return(_tmp80C.f1=
outflow,((_tmp80C.f2=_tmp172->unknown_all,_tmp80C)));};};};}_LL144: {struct Cyc_Absyn_New_e_struct*
_tmp1E2=(struct Cyc_Absyn_New_e_struct*)_tmp194;if(_tmp1E2->tag != 17)goto _LL146;
else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL145: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp80F;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp80E;void*root=(void*)((_tmp80E=
_region_malloc(_tmp172->r,sizeof(*_tmp80E)),((_tmp80E[0]=((_tmp80F.tag=1,((
_tmp80F.f1=_tmp1E4,((_tmp80F.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp80F)))))),_tmp80E))));struct Cyc_CfFlowInfo_Place*_tmp810;
struct Cyc_CfFlowInfo_Place*place=(_tmp810=_region_malloc(_tmp172->r,sizeof(*
_tmp810)),((_tmp810->root=root,((_tmp810->fields=0,_tmp810)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp813;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp812;void*rval=(void*)((_tmp812=
_region_malloc(_tmp172->r,sizeof(*_tmp812)),((_tmp812[0]=((_tmp813.tag=5,((
_tmp813.f1=place,_tmp813)))),_tmp812))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1E3 != 0){struct _RegionHandle*_tmp2C2=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2C5;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Exp*_tmp814[2];struct
_tuple19 _tmp2C4=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2C2,env,inflow,((
_tmp814[1]=_tmp1E4,((_tmp814[0]=(struct Cyc_Absyn_Exp*)_tmp1E3,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2C2,
_tag_dyneither(_tmp814,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C5=_tmp2C4.f1;
_tmp2C6=_tmp2C4.f2;outflow=_tmp2C5;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C6))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2C8;void*_tmp2C9;struct _tuple13 _tmp2C7=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1E4);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;outflow=_tmp2C8;
place_val=_tmp2C9;}{union Cyc_CfFlowInfo_FlowInfo _tmp2CA=outflow;int _tmp2CB;
struct _tuple12 _tmp2CC;struct Cyc_Dict_Dict _tmp2CD;struct Cyc_List_List*_tmp2CE;
_LL1BD: if((_tmp2CA.BottomFL).tag != 1)goto _LL1BF;_tmp2CB=(int)(_tmp2CA.BottomFL).val;
_LL1BE: {struct _tuple13 _tmp815;return(_tmp815.f1=outflow,((_tmp815.f2=rval,
_tmp815)));}_LL1BF: if((_tmp2CA.ReachableFL).tag != 2)goto _LL1BC;_tmp2CC=(struct
_tuple12)(_tmp2CA.ReachableFL).val;_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;_LL1C0: {
struct _tuple13 _tmp816;return(_tmp816.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CD,root,place_val),
_tmp2CE),((_tmp816.f2=rval,_tmp816)));}_LL1BC:;};};}_LL146: {struct Cyc_Absyn_Address_e_struct*
_tmp1E5=(struct Cyc_Absyn_Address_e_struct*)_tmp194;if(_tmp1E5->tag != 16)goto
_LL148;else{_tmp1E6=_tmp1E5->f1;}}_LL147: {union Cyc_CfFlowInfo_FlowInfo _tmp2D7;
union Cyc_CfFlowInfo_AbsLVal _tmp2D8;struct _tuple14 _tmp2D6=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1E6);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2D9=_tmp2D8;int _tmp2DA;struct Cyc_CfFlowInfo_Place*_tmp2DB;_LL1C2: if((_tmp2D9.UnknownL).tag
!= 2)goto _LL1C4;_tmp2DA=(int)(_tmp2D9.UnknownL).val;_LL1C3: {struct _tuple13
_tmp817;return(_tmp817.f1=_tmp2D7,((_tmp817.f2=_tmp172->notzeroall,_tmp817)));}
_LL1C4: if((_tmp2D9.PlaceL).tag != 1)goto _LL1C1;_tmp2DB=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D9.PlaceL).val;_LL1C5: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp81D;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp81C;struct _tuple13 _tmp81B;return(
_tmp81B.f1=_tmp2D7,((_tmp81B.f2=(void*)((_tmp81D=_region_malloc(env->r,sizeof(*
_tmp81D)),((_tmp81D[0]=((_tmp81C.tag=5,((_tmp81C.f1=_tmp2DB,_tmp81C)))),_tmp81D)))),
_tmp81B)));}_LL1C1:;};}_LL148: {struct Cyc_Absyn_Deref_e_struct*_tmp1E7=(struct
Cyc_Absyn_Deref_e_struct*)_tmp194;if(_tmp1E7->tag != 21)goto _LL14A;else{_tmp1E8=
_tmp1E7->f1;}}_LL149: {union Cyc_CfFlowInfo_FlowInfo _tmp2E1;void*_tmp2E2;struct
_tuple13 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E8);_tmp2E1=
_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2E1,_tmp1E8,_tmp2E2);}_LL14A: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E9=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp194;if(_tmp1E9->tag != 22)goto _LL14C;
else{_tmp1EA=_tmp1E9->f1;_tmp1EB=_tmp1E9->f2;}}_LL14B: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E4;void*_tmp2E5;struct _tuple13 _tmp2E3=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1EA);_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1EA->topt))->v)){struct _tuple13
_tmp81E;return(_tmp81E.f1=_tmp2E4,((_tmp81E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp81E)));}{void*_tmp2E7=_tmp2E5;struct Cyc_CfFlowInfo_UnionRInfo _tmp2E9;int
_tmp2EA;int _tmp2EB;struct _dyneither_ptr _tmp2EC;_LL1C7: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2E8=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E7;if(_tmp2E8->tag != 6)goto
_LL1C9;else{_tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E9.is_union;_tmp2EB=_tmp2E9.fieldnum;
_tmp2EC=_tmp2E8->f2;}}_LL1C8: {int _tmp2ED=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1EA->topt))->v,_tmp1EB);if((_tmp2EA  && 
_tmp2EB != - 1) && _tmp2EB != _tmp2ED){struct _tuple13 _tmp81F;return(_tmp81F.f1=
_tmp2E4,((_tmp81F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,1,_tmp172->unknown_none),_tmp81F)));}{struct _tuple13
_tmp820;return(_tmp820.f1=_tmp2E4,((_tmp820.f2=*((void**)
_check_dyneither_subscript(_tmp2EC,sizeof(void*),_tmp2ED)),_tmp820)));};}_LL1C9:;
_LL1CA: {struct Cyc_Core_Impossible_struct _tmp82D;const char*_tmp82C;void*_tmp82B[
1];struct Cyc_String_pa_struct _tmp82A;struct Cyc_Core_Impossible_struct*_tmp829;(
int)_throw((void*)((_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82D.tag=
Cyc_Core_Impossible,((_tmp82D.f1=(struct _dyneither_ptr)((_tmp82A.tag=0,((_tmp82A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp82B[
0]=& _tmp82A,Cyc_aprintf(((_tmp82C="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp82C,sizeof(char),26))),_tag_dyneither(_tmp82B,sizeof(void*),1)))))))),
_tmp82D)))),_tmp829)))));}_LL1C6:;};}_LL14C: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1EC=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp194;if(_tmp1EC->tag != 38)goto
_LL14E;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LL14D: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F6;void*_tmp2F7;struct _tuple13 _tmp2F5=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1ED);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v)){struct _tuple13
_tmp82E;return(_tmp82E.f1=_tmp2F6,((_tmp82E.f2=_tmp172->unknown_all,_tmp82E)));}{
void*_tmp2F9=_tmp2F7;struct Cyc_CfFlowInfo_UnionRInfo _tmp2FB;int _tmp2FC;int
_tmp2FD;struct _dyneither_ptr _tmp2FE;_LL1CC: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2FA=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2F9;if(_tmp2FA->tag != 6)goto
_LL1CE;else{_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FB.is_union;_tmp2FD=_tmp2FB.fieldnum;
_tmp2FE=_tmp2FA->f2;}}_LL1CD: {int _tmp2FF=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v,_tmp1EE);if(_tmp2FC  && 
_tmp2FD != - 1){if(_tmp2FD != _tmp2FF){struct _tuple13 _tmp82F;return(_tmp82F.f1=
_tmp2F6,((_tmp82F.f2=_tmp172->zero,_tmp82F)));}else{struct _tuple13 _tmp830;return(
_tmp830.f1=_tmp2F6,((_tmp830.f2=_tmp172->notzeroall,_tmp830)));}}else{struct
_tuple13 _tmp831;return(_tmp831.f1=_tmp2F6,((_tmp831.f2=_tmp172->unknown_all,
_tmp831)));}}_LL1CE:;_LL1CF: {struct Cyc_Core_Impossible_struct _tmp83E;const char*
_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_struct _tmp83B;struct Cyc_Core_Impossible_struct*
_tmp83A;(int)_throw((void*)((_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=((
_tmp83E.tag=Cyc_Core_Impossible,((_tmp83E.f1=(struct _dyneither_ptr)((_tmp83B.tag=
0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp83C[0]=& _tmp83B,Cyc_aprintf(((_tmp83D="anal_Rexp: TagCheck_e: %s",
_tag_dyneither(_tmp83D,sizeof(char),26))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))))),
_tmp83E)))),_tmp83A)))));}_LL1CB:;};}_LL14E: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1EF=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp194;if(_tmp1EF->tag != 23)goto
_LL150;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LL14F: {union Cyc_CfFlowInfo_FlowInfo
_tmp309;void*_tmp30A;struct _tuple13 _tmp308=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1F0);_tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp30C;void*_tmp30D;struct _tuple13 _tmp30B=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp309,_tmp1F0,_tmp30A);_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;{void*
_tmp30E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F0->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp310;void*_tmp311;_LL1D1: {struct Cyc_Absyn_PointerType_struct*
_tmp30F=(struct Cyc_Absyn_PointerType_struct*)_tmp30E;if(_tmp30F->tag != 5)goto
_LL1D3;else{_tmp310=_tmp30F->f1;_tmp311=_tmp310.elt_typ;}}_LL1D2: if(Cyc_Absyn_is_nontagged_union_type(
_tmp311)){struct _tuple13 _tmp83F;return(_tmp83F.f1=_tmp30C,((_tmp83F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp83F)));}{void*_tmp313=_tmp30D;struct Cyc_CfFlowInfo_UnionRInfo _tmp315;int
_tmp316;int _tmp317;struct _dyneither_ptr _tmp318;_LL1D6: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp314=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp313;if(_tmp314->tag != 6)goto
_LL1D8;else{_tmp315=_tmp314->f1;_tmp316=_tmp315.is_union;_tmp317=_tmp315.fieldnum;
_tmp318=_tmp314->f2;}}_LL1D7: {int _tmp319=Cyc_CfFlowInfo_get_field_index(_tmp311,
_tmp1F1);if((_tmp316  && _tmp317 != - 1) && _tmp317 != _tmp319){struct _tuple13
_tmp840;return(_tmp840.f1=_tmp30C,((_tmp840.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,1,_tmp172->unknown_none),
_tmp840)));}{struct _tuple13 _tmp841;return(_tmp841.f1=_tmp30C,((_tmp841.f2=*((
void**)_check_dyneither_subscript(_tmp318,sizeof(void*),_tmp319)),_tmp841)));};}
_LL1D8:;_LL1D9: {struct Cyc_Core_Impossible_struct _tmp847;const char*_tmp846;
struct Cyc_Core_Impossible_struct*_tmp845;(int)_throw((void*)((_tmp845=_cycalloc(
sizeof(*_tmp845)),((_tmp845[0]=((_tmp847.tag=Cyc_Core_Impossible,((_tmp847.f1=((
_tmp846="anal_Rexp: AggrArrow",_tag_dyneither(_tmp846,sizeof(char),21))),_tmp847)))),
_tmp845)))));}_LL1D5:;};_LL1D3:;_LL1D4: {struct Cyc_Core_Impossible_struct _tmp84D;
const char*_tmp84C;struct Cyc_Core_Impossible_struct*_tmp84B;(int)_throw((void*)((
_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84D.tag=Cyc_Core_Impossible,((
_tmp84D.f1=((_tmp84C="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp84C,sizeof(
char),25))),_tmp84D)))),_tmp84B)))));}_LL1D0:;};};}_LL150: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1F2=(struct Cyc_Absyn_Conditional_e_struct*)_tmp194;if(_tmp1F2->tag != 6)goto
_LL152;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;_tmp1F5=_tmp1F2->f3;}}_LL151: {
union Cyc_CfFlowInfo_FlowInfo _tmp323;union Cyc_CfFlowInfo_FlowInfo _tmp324;struct
_tuple15 _tmp322=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F3);_tmp323=_tmp322.f1;
_tmp324=_tmp322.f2;{struct _tuple13 _tmp325=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp323,_tmp1F4);struct _tuple13 _tmp326=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp324,_tmp1F5);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,
env->all_changed,_tmp325,_tmp326);};}_LL152: {struct Cyc_Absyn_And_e_struct*
_tmp1F6=(struct Cyc_Absyn_And_e_struct*)_tmp194;if(_tmp1F6->tag != 7)goto _LL154;
else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}_LL153: {union Cyc_CfFlowInfo_FlowInfo
_tmp328;union Cyc_CfFlowInfo_FlowInfo _tmp329;struct _tuple15 _tmp327=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F7);_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp32B;void*_tmp32C;struct _tuple13 _tmp32A=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp328,_tmp1F8);_tmp32B=_tmp32A.f1;_tmp32C=_tmp32A.f2;{struct _tuple13
_tmp84E;struct _tuple13 _tmp32D=(_tmp84E.f1=_tmp32B,((_tmp84E.f2=_tmp32C,_tmp84E)));
struct _tuple13 _tmp84F;struct _tuple13 _tmp32E=(_tmp84F.f1=_tmp329,((_tmp84F.f2=
_tmp172->zero,_tmp84F)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp32D,_tmp32E);};};}_LL154: {struct Cyc_Absyn_Or_e_struct*_tmp1F9=(struct Cyc_Absyn_Or_e_struct*)
_tmp194;if(_tmp1F9->tag != 8)goto _LL156;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}}
_LL155: {union Cyc_CfFlowInfo_FlowInfo _tmp332;union Cyc_CfFlowInfo_FlowInfo _tmp333;
struct _tuple15 _tmp331=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1FA);_tmp332=
_tmp331.f1;_tmp333=_tmp331.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp335;void*_tmp336;
struct _tuple13 _tmp334=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp333,_tmp1FB);
_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;{struct _tuple13 _tmp850;struct _tuple13
_tmp337=(_tmp850.f1=_tmp335,((_tmp850.f2=_tmp336,_tmp850)));struct _tuple13
_tmp851;struct _tuple13 _tmp338=(_tmp851.f1=_tmp332,((_tmp851.f2=_tmp172->notzeroall,
_tmp851)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp337,_tmp338);};};}_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp1FC=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp194;if(_tmp1FC->tag != 24)goto _LL158;
else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;}}_LL157: {struct _RegionHandle*
_tmp33B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp33E;struct Cyc_List_List*_tmp33F;
struct Cyc_Absyn_Exp*_tmp852[2];struct _tuple19 _tmp33D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp33B,env,inflow,((_tmp852[1]=_tmp1FE,((_tmp852[0]=_tmp1FD,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp33B,
_tag_dyneither(_tmp852,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp33E=_tmp33D.f1;
_tmp33F=_tmp33D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp340=_tmp33E;{union Cyc_CfFlowInfo_FlowInfo
_tmp341=_tmp33E;struct _tuple12 _tmp342;struct Cyc_Dict_Dict _tmp343;struct Cyc_List_List*
_tmp344;_LL1DB: if((_tmp341.ReachableFL).tag != 2)goto _LL1DD;_tmp342=(struct
_tuple12)(_tmp341.ReachableFL).val;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;_LL1DC:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp343,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp33F))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp855;void*_tmp854;(_tmp854=0,Cyc_Tcutil_terr(_tmp1FE->loc,((_tmp855="expression may not be initialized",
_tag_dyneither(_tmp855,sizeof(char),34))),_tag_dyneither(_tmp854,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp347=Cyc_NewControlFlow_add_subscript_reln(_tmp172->r,
_tmp344,_tmp1FD,_tmp1FE);if(_tmp344 != _tmp347)_tmp340=Cyc_CfFlowInfo_ReachableFL(
_tmp343,_tmp347);goto _LL1DA;};_LL1DD:;_LL1DE: goto _LL1DA;_LL1DA:;}{void*_tmp348=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FD->topt))->v);
struct Cyc_List_List*_tmp34A;struct Cyc_Absyn_PtrInfo _tmp34C;struct Cyc_Absyn_PtrAtts
_tmp34D;union Cyc_Absyn_Constraint*_tmp34E;_LL1E0: {struct Cyc_Absyn_TupleType_struct*
_tmp349=(struct Cyc_Absyn_TupleType_struct*)_tmp348;if(_tmp349->tag != 11)goto
_LL1E2;else{_tmp34A=_tmp349->f1;}}_LL1E1: {void*_tmp34F=(void*)((struct Cyc_List_List*)
_check_null(_tmp33F))->hd;struct _dyneither_ptr _tmp351;_LL1E7: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp350=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp34F;if(_tmp350->tag != 6)goto
_LL1E9;else{_tmp351=_tmp350->f2;}}_LL1E8: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(
_tmp1FE)).f1;struct _tuple13 _tmp856;return(_tmp856.f1=_tmp340,((_tmp856.f2=*((
void**)_check_dyneither_subscript(_tmp351,sizeof(void*),(int)i)),_tmp856)));}
_LL1E9:;_LL1EA: {struct Cyc_Core_Impossible_struct _tmp85C;const char*_tmp85B;
struct Cyc_Core_Impossible_struct*_tmp85A;(int)_throw((void*)((_tmp85A=_cycalloc(
sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85C.tag=Cyc_Core_Impossible,((_tmp85C.f1=((
_tmp85B="anal_Rexp: Subscript",_tag_dyneither(_tmp85B,sizeof(char),21))),_tmp85C)))),
_tmp85A)))));}_LL1E6:;}_LL1E2: {struct Cyc_Absyn_PointerType_struct*_tmp34B=(
struct Cyc_Absyn_PointerType_struct*)_tmp348;if(_tmp34B->tag != 5)goto _LL1E4;else{
_tmp34C=_tmp34B->f1;_tmp34D=_tmp34C.ptr_atts;_tmp34E=_tmp34D.bounds;}}_LL1E3: {
union Cyc_CfFlowInfo_FlowInfo _tmp357;void*_tmp358;struct _tuple13 _tmp356=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp33E,_tmp1FD,(void*)((struct Cyc_List_List*)_check_null(_tmp33F))->hd);
_tmp357=_tmp356.f1;_tmp358=_tmp356.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp359=
_tmp357;int _tmp35A;_LL1EC: if((_tmp359.BottomFL).tag != 1)goto _LL1EE;_tmp35A=(int)(
_tmp359.BottomFL).val;_LL1ED: {struct _tuple13 _tmp85D;return(_tmp85D.f1=_tmp357,((
_tmp85D.f2=_tmp358,_tmp85D)));}_LL1EE:;_LL1EF: {struct _tuple13 _tmp85E;return(
_tmp85E.f1=_tmp340,((_tmp85E.f2=_tmp358,_tmp85E)));}_LL1EB:;};}_LL1E4:;_LL1E5: {
struct Cyc_Core_Impossible_struct _tmp864;const char*_tmp863;struct Cyc_Core_Impossible_struct*
_tmp862;(int)_throw((void*)((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862[0]=((
_tmp864.tag=Cyc_Core_Impossible,((_tmp864.f1=((_tmp863="anal_Rexp: Subscript -- bad type",
_tag_dyneither(_tmp863,sizeof(char),33))),_tmp864)))),_tmp862)))));}_LL1DF:;};};}
_LL158: {struct Cyc_Absyn_Datatype_e_struct*_tmp1FF=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp194;if(_tmp1FF->tag != 31)goto _LL15A;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;}}
_LL159: _tmp203=_tmp200;goto _LL15B;_LL15A: {struct Cyc_Absyn_Tuple_e_struct*
_tmp202=(struct Cyc_Absyn_Tuple_e_struct*)_tmp194;if(_tmp202->tag != 25)goto _LL15C;
else{_tmp203=_tmp202->f1;}}_LL15B: {struct _RegionHandle*_tmp360=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp362;struct Cyc_List_List*_tmp363;struct _tuple19 _tmp361=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp360,env,inflow,_tmp203,1,1);_tmp362=_tmp361.f1;_tmp363=_tmp361.f2;{
unsigned int _tmp86A;unsigned int _tmp869;struct _dyneither_ptr _tmp868;void**
_tmp867;unsigned int _tmp866;struct _dyneither_ptr aggrdict=(_tmp866=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp203),((_tmp867=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp866)),((_tmp868=
_tag_dyneither(_tmp867,sizeof(void*),_tmp866),((((_tmp869=_tmp866,_tmp86B(&
_tmp86A,& _tmp869,& _tmp867,& _tmp363))),_tmp868)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp875;struct Cyc_CfFlowInfo_UnionRInfo _tmp874;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp873;struct _tuple13 _tmp872;return(_tmp872.f1=_tmp362,((_tmp872.f2=(void*)((
_tmp875=_region_malloc(env->r,sizeof(*_tmp875)),((_tmp875[0]=((_tmp873.tag=6,((
_tmp873.f1=((_tmp874.is_union=0,((_tmp874.fieldnum=- 1,_tmp874)))),((_tmp873.f2=
aggrdict,_tmp873)))))),_tmp875)))),_tmp872)));};}_LL15C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp204=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp194;if(_tmp204->tag != 30)goto
_LL15E;else{_tmp205=_tmp204->f2;}}_LL15D: {struct Cyc_List_List*fs;{void*_tmp36D=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp36F;_LL1F1: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36E=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp36D;if(_tmp36E->tag != 13)goto _LL1F3;
else{_tmp36F=_tmp36E->f2;}}_LL1F2: fs=_tmp36F;goto _LL1F0;_LL1F3:;_LL1F4: {struct
Cyc_Core_Impossible_struct _tmp87B;const char*_tmp87A;struct Cyc_Core_Impossible_struct*
_tmp879;(int)_throw((void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((
_tmp87B.tag=Cyc_Core_Impossible,((_tmp87B.f1=((_tmp87A="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp87A,sizeof(char),35))),_tmp87B)))),_tmp879)))));}_LL1F0:;}
_tmp207=_tmp205;_tmp20A=Cyc_Absyn_StructA;_tmp20D=fs;goto _LL15F;}_LL15E: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp206=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp194;if(_tmp206->tag != 29)goto _LL160;else{_tmp207=_tmp206->f3;_tmp208=_tmp206->f4;
if(_tmp208 == 0)goto _LL160;_tmp209=*_tmp208;_tmp20A=_tmp209.kind;_tmp20B=_tmp209.impl;
if(_tmp20B == 0)goto _LL160;_tmp20C=*_tmp20B;_tmp20D=_tmp20C.fields;}}_LL15F: {
struct _RegionHandle*_tmp373=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp375;struct Cyc_List_List*
_tmp376;struct _tuple19 _tmp374=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp373,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp373,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp207),1,1);_tmp375=_tmp374.f1;_tmp376=_tmp374.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp172,
_tmp20D,_tmp20A == Cyc_Absyn_UnionA,_tmp20A == Cyc_Absyn_UnionA?_tmp172->unknown_none:
_tmp172->unknown_all);int field_no=- 1;{int i=0;for(0;_tmp376 != 0;(((_tmp376=
_tmp376->tl,_tmp207=_tmp207->tl)),++ i)){struct Cyc_List_List*ds=(*((struct
_tuple21*)((struct Cyc_List_List*)_check_null(_tmp207))->hd)).f1;for(0;ds != 0;ds=
ds->tl){void*_tmp377=(void*)ds->hd;struct _dyneither_ptr*_tmp37A;_LL1F6: {struct
Cyc_Absyn_ArrayElement_struct*_tmp378=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp377;if(_tmp378->tag != 0)goto _LL1F8;}_LL1F7: {struct Cyc_Core_Impossible_struct
_tmp881;const char*_tmp880;struct Cyc_Core_Impossible_struct*_tmp87F;(int)_throw((
void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((_tmp881.tag=Cyc_Core_Impossible,((
_tmp881.f1=((_tmp880="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp880,sizeof(char),
22))),_tmp881)))),_tmp87F)))));}_LL1F8: {struct Cyc_Absyn_FieldName_struct*
_tmp379=(struct Cyc_Absyn_FieldName_struct*)_tmp377;if(_tmp379->tag != 1)goto
_LL1F5;else{_tmp37A=_tmp379->f1;}}_LL1F9: field_no=Cyc_CfFlowInfo_get_field_index_fs(
_tmp20D,_tmp37A);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
field_no))=(void*)_tmp376->hd;if(_tmp20A == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp37E=(*((struct _tuple21*)_tmp207->hd)).f2;_tmp375=Cyc_NewControlFlow_use_Rval(
env,_tmp37E->loc,_tmp375,(void*)_tmp376->hd);}_LL1F5:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp88B;struct Cyc_CfFlowInfo_UnionRInfo _tmp88A;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp889;struct _tuple13 _tmp888;return(_tmp888.f1=_tmp375,((_tmp888.f2=(void*)((
_tmp88B=_region_malloc(env->r,sizeof(*_tmp88B)),((_tmp88B[0]=((_tmp889.tag=6,((
_tmp889.f1=((_tmp88A.is_union=_tmp20A == Cyc_Absyn_UnionA,((_tmp88A.fieldnum=
field_no,_tmp88A)))),((_tmp889.f2=aggrdict,_tmp889)))))),_tmp88B)))),_tmp888)));};};}
_LL160: {struct Cyc_Absyn_Aggregate_e_struct*_tmp20E=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp194;if(_tmp20E->tag != 29)goto _LL162;}_LL161: {struct Cyc_Core_Impossible_struct
_tmp891;const char*_tmp890;struct Cyc_Core_Impossible_struct*_tmp88F;(int)_throw((
void*)((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp891.tag=Cyc_Core_Impossible,((
_tmp891.f1=((_tmp890="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp890,
sizeof(char),31))),_tmp891)))),_tmp88F)))));}_LL162: {struct Cyc_Absyn_Array_e_struct*
_tmp20F=(struct Cyc_Absyn_Array_e_struct*)_tmp194;if(_tmp20F->tag != 27)goto _LL164;
else{_tmp210=_tmp20F->f1;}}_LL163: {struct _RegionHandle*_tmp386=env->r;struct Cyc_List_List*
_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp386,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp210);union Cyc_CfFlowInfo_FlowInfo _tmp389;
struct Cyc_List_List*_tmp38A;struct _tuple19 _tmp388=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp386,env,inflow,_tmp387,1,1);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;for(0;
_tmp38A != 0;(_tmp38A=_tmp38A->tl,_tmp387=_tmp387->tl)){_tmp389=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp387))->hd)->loc,
_tmp389,(void*)_tmp38A->hd);}{struct _tuple13 _tmp892;return(_tmp892.f1=_tmp389,((
_tmp892.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp892)));};}_LL164: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp211=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp194;if(_tmp211->tag != 28)goto _LL166;else{_tmp212=_tmp211->f1;_tmp213=_tmp211->f2;
_tmp214=_tmp211->f3;_tmp215=_tmp211->f4;}}_LL165: {union Cyc_CfFlowInfo_FlowInfo
_tmp38D;void*_tmp38E;struct _tuple13 _tmp38C=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp213);_tmp38D=_tmp38C.f1;_tmp38E=_tmp38C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp38F=_tmp38D;int _tmp390;struct _tuple12 _tmp391;struct Cyc_Dict_Dict _tmp392;
struct Cyc_List_List*_tmp393;_LL1FB: if((_tmp38F.BottomFL).tag != 1)goto _LL1FD;
_tmp390=(int)(_tmp38F.BottomFL).val;_LL1FC: {struct _tuple13 _tmp893;return(
_tmp893.f1=_tmp38D,((_tmp893.f2=_tmp172->unknown_all,_tmp893)));}_LL1FD: if((
_tmp38F.ReachableFL).tag != 2)goto _LL1FA;_tmp391=(struct _tuple12)(_tmp38F.ReachableFL).val;
_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_LL1FE: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp392,_tmp38E)== Cyc_CfFlowInfo_NoneIL){const char*_tmp896;void*_tmp895;(
_tmp895=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp896="expression may not be initialized",
_tag_dyneither(_tmp896,sizeof(char),34))),_tag_dyneither(_tmp895,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp393;comp_loop: {void*_tmp397=_tmp213->r;struct
Cyc_Absyn_Exp*_tmp399;void*_tmp39B;struct Cyc_Absyn_Vardecl*_tmp39D;void*_tmp39F;
struct Cyc_Absyn_Vardecl*_tmp3A1;void*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A5;
void*_tmp3A7;struct Cyc_Absyn_Vardecl*_tmp3A9;union Cyc_Absyn_Cnst _tmp3AB;struct
_tuple6 _tmp3AC;int _tmp3AD;enum Cyc_Absyn_Primop _tmp3AF;struct Cyc_List_List*
_tmp3B0;struct Cyc_List_List _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;_LL200: {struct
Cyc_Absyn_Cast_e_struct*_tmp398=(struct Cyc_Absyn_Cast_e_struct*)_tmp397;if(
_tmp398->tag != 15)goto _LL202;else{_tmp399=_tmp398->f2;}}_LL201: _tmp213=_tmp399;
goto comp_loop;_LL202:{struct Cyc_Absyn_Var_e_struct*_tmp39A=(struct Cyc_Absyn_Var_e_struct*)
_tmp397;if(_tmp39A->tag != 1)goto _LL204;else{_tmp39B=(void*)_tmp39A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39C=(struct Cyc_Absyn_Global_b_struct*)_tmp39B;if(
_tmp39C->tag != 1)goto _LL204;else{_tmp39D=_tmp39C->f1;}};}}if(!(!_tmp39D->escapes))
goto _LL204;_LL203: _tmp3A1=_tmp39D;goto _LL205;_LL204:{struct Cyc_Absyn_Var_e_struct*
_tmp39E=(struct Cyc_Absyn_Var_e_struct*)_tmp397;if(_tmp39E->tag != 1)goto _LL206;
else{_tmp39F=(void*)_tmp39E->f2;{struct Cyc_Absyn_Local_b_struct*_tmp3A0=(struct
Cyc_Absyn_Local_b_struct*)_tmp39F;if(_tmp3A0->tag != 4)goto _LL206;else{_tmp3A1=
_tmp3A0->f1;}};}}if(!(!_tmp3A1->escapes))goto _LL206;_LL205: _tmp3A5=_tmp3A1;goto
_LL207;_LL206:{struct Cyc_Absyn_Var_e_struct*_tmp3A2=(struct Cyc_Absyn_Var_e_struct*)
_tmp397;if(_tmp3A2->tag != 1)goto _LL208;else{_tmp3A3=(void*)_tmp3A2->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3A4=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A3;if(_tmp3A4->tag
!= 5)goto _LL208;else{_tmp3A5=_tmp3A4->f1;}};}}if(!(!_tmp3A5->escapes))goto _LL208;
_LL207: _tmp3A9=_tmp3A5;goto _LL209;_LL208:{struct Cyc_Absyn_Var_e_struct*_tmp3A6=(
struct Cyc_Absyn_Var_e_struct*)_tmp397;if(_tmp3A6->tag != 1)goto _LL20A;else{
_tmp3A7=(void*)_tmp3A6->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A8=(struct Cyc_Absyn_Param_b_struct*)
_tmp3A7;if(_tmp3A8->tag != 3)goto _LL20A;else{_tmp3A9=_tmp3A8->f1;}};}}if(!(!
_tmp3A9->escapes))goto _LL20A;_LL209:{struct Cyc_CfFlowInfo_Reln*_tmp899;struct Cyc_List_List*
_tmp898;new_relns=((_tmp898=_region_malloc(env->r,sizeof(*_tmp898)),((_tmp898->hd=((
_tmp899=_region_malloc(env->r,sizeof(*_tmp899)),((_tmp899->vd=_tmp212,((_tmp899->rop=
Cyc_CfFlowInfo_LessVar(_tmp3A9,_tmp3A9->type),_tmp899)))))),((_tmp898->tl=
_tmp393,_tmp898))))));}goto _LL1FF;_LL20A: {struct Cyc_Absyn_Const_e_struct*
_tmp3AA=(struct Cyc_Absyn_Const_e_struct*)_tmp397;if(_tmp3AA->tag != 0)goto _LL20C;
else{_tmp3AB=_tmp3AA->f1;if((_tmp3AB.Int_c).tag != 4)goto _LL20C;_tmp3AC=(struct
_tuple6)(_tmp3AB.Int_c).val;_tmp3AD=_tmp3AC.f2;}}_LL20B:{struct Cyc_CfFlowInfo_Reln*
_tmp89C;struct Cyc_List_List*_tmp89B;new_relns=((_tmp89B=_region_malloc(env->r,
sizeof(*_tmp89B)),((_tmp89B->hd=((_tmp89C=_region_malloc(env->r,sizeof(*_tmp89C)),((
_tmp89C->vd=_tmp212,((_tmp89C->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp3AD),_tmp89C)))))),((_tmp89B->tl=_tmp393,_tmp89B))))));}goto _LL1FF;_LL20C: {
struct Cyc_Absyn_Primop_e_struct*_tmp3AE=(struct Cyc_Absyn_Primop_e_struct*)
_tmp397;if(_tmp3AE->tag != 3)goto _LL20E;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;
if(_tmp3B0 == 0)goto _LL20E;_tmp3B1=*_tmp3B0;_tmp3B2=(struct Cyc_Absyn_Exp*)_tmp3B1.hd;}}
_LL20D:{void*_tmp3B7=_tmp3B2->r;void*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BB;
void*_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Vardecl*
_tmp3C3;void*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C7;_LL211:{struct Cyc_Absyn_Var_e_struct*
_tmp3B8=(struct Cyc_Absyn_Var_e_struct*)_tmp3B7;if(_tmp3B8->tag != 1)goto _LL213;
else{_tmp3B9=(void*)_tmp3B8->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3BA=(struct
Cyc_Absyn_Global_b_struct*)_tmp3B9;if(_tmp3BA->tag != 1)goto _LL213;else{_tmp3BB=
_tmp3BA->f1;}};}}if(!(!_tmp3BB->escapes))goto _LL213;_LL212: _tmp3BF=_tmp3BB;goto
_LL214;_LL213:{struct Cyc_Absyn_Var_e_struct*_tmp3BC=(struct Cyc_Absyn_Var_e_struct*)
_tmp3B7;if(_tmp3BC->tag != 1)goto _LL215;else{_tmp3BD=(void*)_tmp3BC->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3BE=(struct Cyc_Absyn_Local_b_struct*)_tmp3BD;if(
_tmp3BE->tag != 4)goto _LL215;else{_tmp3BF=_tmp3BE->f1;}};}}if(!(!_tmp3BF->escapes))
goto _LL215;_LL214: _tmp3C3=_tmp3BF;goto _LL216;_LL215:{struct Cyc_Absyn_Var_e_struct*
_tmp3C0=(struct Cyc_Absyn_Var_e_struct*)_tmp3B7;if(_tmp3C0->tag != 1)goto _LL217;
else{_tmp3C1=(void*)_tmp3C0->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3C2=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3C1;if(_tmp3C2->tag != 5)goto _LL217;else{_tmp3C3=_tmp3C2->f1;}};}}if(!(!
_tmp3C3->escapes))goto _LL217;_LL216: _tmp3C7=_tmp3C3;goto _LL218;_LL217:{struct Cyc_Absyn_Var_e_struct*
_tmp3C4=(struct Cyc_Absyn_Var_e_struct*)_tmp3B7;if(_tmp3C4->tag != 1)goto _LL219;
else{_tmp3C5=(void*)_tmp3C4->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3C6=(struct
Cyc_Absyn_Param_b_struct*)_tmp3C5;if(_tmp3C6->tag != 3)goto _LL219;else{_tmp3C7=
_tmp3C6->f1;}};}}if(!(!_tmp3C7->escapes))goto _LL219;_LL218:{struct Cyc_CfFlowInfo_Reln*
_tmp89F;struct Cyc_List_List*_tmp89E;new_relns=((_tmp89E=_region_malloc(env->r,
sizeof(*_tmp89E)),((_tmp89E->hd=((_tmp89F=_region_malloc(env->r,sizeof(*_tmp89F)),((
_tmp89F->vd=_tmp212,((_tmp89F->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3C7),_tmp89F)))))),((
_tmp89E->tl=_tmp393,_tmp89E))))));}goto _LL210;_LL219:;_LL21A: goto _LL210;_LL210:;}
goto _LL1FF;_LL20E:;_LL20F: goto _LL1FF;_LL1FF:;}if(_tmp393 != new_relns)_tmp38D=Cyc_CfFlowInfo_ReachableFL(
_tmp392,new_relns);{void*_tmp3CA=_tmp38E;_LL21C: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3CB=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3CA;if(_tmp3CB->tag != 0)goto
_LL21E;}_LL21D: {struct _tuple13 _tmp8A0;return(_tmp8A0.f1=_tmp38D,((_tmp8A0.f2=
_tmp172->unknown_all,_tmp8A0)));}_LL21E: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3CC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3CA;if(_tmp3CC->tag != 2)
goto _LL220;}_LL21F: goto _LL221;_LL220: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3CD=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3CA;if(_tmp3CD->tag != 1)
goto _LL222;}_LL221: goto _LL223;_LL222: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3CE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3CA;if(_tmp3CE->tag != 5)goto
_LL224;}_LL223: {struct Cyc_List_List _tmp8A1;struct Cyc_List_List _tmp3D0=(_tmp8A1.hd=
_tmp212,((_tmp8A1.tl=0,_tmp8A1)));_tmp38D=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp38D,(struct Cyc_List_List*)& _tmp3D0,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3D2;void*_tmp3D3;struct _tuple13 _tmp3D1=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38D,_tmp214);_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3D4=_tmp3D2;int _tmp3D5;struct _tuple12 _tmp3D6;struct Cyc_Dict_Dict _tmp3D7;
_LL227: if((_tmp3D4.BottomFL).tag != 1)goto _LL229;_tmp3D5=(int)(_tmp3D4.BottomFL).val;
_LL228: {struct _tuple13 _tmp8A2;return(_tmp8A2.f1=_tmp3D2,((_tmp8A2.f2=_tmp172->unknown_all,
_tmp8A2)));}_LL229: if((_tmp3D4.ReachableFL).tag != 2)goto _LL226;_tmp3D6=(struct
_tuple12)(_tmp3D4.ReachableFL).val;_tmp3D7=_tmp3D6.f1;_LL22A: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3D7,_tmp3D3)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8A5;void*_tmp8A4;(
_tmp8A4=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp8A5="expression may not be initialized",
_tag_dyneither(_tmp8A5,sizeof(char),34))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}{
struct _tuple13 _tmp8A6;return(_tmp8A6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A6.f2=
_tmp172->unknown_all,_tmp8A6)));};}_LL226:;}_tmp38D=_tmp3D2;goto _LL225;};}_LL224:;
_LL225: while(1){struct Cyc_List_List _tmp8A7;struct Cyc_List_List _tmp3DD=(_tmp8A7.hd=
_tmp212,((_tmp8A7.tl=0,_tmp8A7)));_tmp38D=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp38D,(struct Cyc_List_List*)& _tmp3DD,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3DF;void*_tmp3E0;struct _tuple13 _tmp3DE=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38D,_tmp214);_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3E1=_tmp3DF;int _tmp3E2;struct _tuple12 _tmp3E3;struct Cyc_Dict_Dict _tmp3E4;
_LL22C: if((_tmp3E1.BottomFL).tag != 1)goto _LL22E;_tmp3E2=(int)(_tmp3E1.BottomFL).val;
_LL22D: goto _LL22B;_LL22E: if((_tmp3E1.ReachableFL).tag != 2)goto _LL22B;_tmp3E3=(
struct _tuple12)(_tmp3E1.ReachableFL).val;_tmp3E4=_tmp3E3.f1;_LL22F: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3E4,_tmp3E0)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8AA;void*_tmp8A9;(
_tmp8A9=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp8AA="expression may not be initialized",
_tag_dyneither(_tmp8AA,sizeof(char),34))),_tag_dyneither(_tmp8A9,sizeof(void*),0)));}{
struct _tuple13 _tmp8AB;return(_tmp8AB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8AB.f2=
_tmp172->unknown_all,_tmp8AB)));};}_LL22B:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3E8=
Cyc_CfFlowInfo_join_flow(_tmp172,env->all_changed,_tmp38D,_tmp3DF);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3E8,_tmp38D))break;_tmp38D=_tmp3E8;};};}{struct _tuple13 _tmp8AC;return(
_tmp8AC.f1=_tmp38D,((_tmp8AC.f2=_tmp172->unknown_all,_tmp8AC)));};_LL21B:;};};
_LL1FA:;};}_LL166: {struct Cyc_Absyn_StmtExp_e_struct*_tmp216=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp194;if(_tmp216->tag != 37)goto _LL168;else{_tmp217=_tmp216->f1;}}_LL167: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp217);_LL168: {struct
Cyc_Absyn_Var_e_struct*_tmp218=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp218->tag
!= 1)goto _LL16A;else{_tmp219=(void*)_tmp218->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp21A=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp219;if(_tmp21A->tag != 0)goto
_LL16A;};}}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp21B=(
struct Cyc_Absyn_UnknownId_e_struct*)_tmp194;if(_tmp21B->tag != 2)goto _LL16C;}
_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp21C=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp194;if(_tmp21C->tag != 10)goto _LL16E;}_LL16D:
goto _LL16F;_LL16E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp21D=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp194;if(_tmp21D->tag != 36)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp21E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp194;if(_tmp21E->tag != 26)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Valueof_e_struct*_tmp21F=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp194;if(_tmp21F->tag != 39)goto _LL10B;}
_LL173: {struct Cyc_Core_Impossible_struct _tmp8B2;const char*_tmp8B1;struct Cyc_Core_Impossible_struct*
_tmp8B0;(int)_throw((void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((
_tmp8B2.tag=Cyc_Core_Impossible,((_tmp8B2.f1=((_tmp8B1="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp8B1,sizeof(char),31))),_tmp8B2)))),_tmp8B0)))));}_LL10B:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3EE=env->fenv;void*_tmp3EF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_PtrAtts _tmp3F3;union Cyc_Absyn_Constraint*
_tmp3F4;union Cyc_Absyn_Constraint*_tmp3F5;_LL231: {struct Cyc_Absyn_PointerType_struct*
_tmp3F0=(struct Cyc_Absyn_PointerType_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto
_LL233;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F1.elt_typ;_tmp3F3=_tmp3F1.ptr_atts;
_tmp3F4=_tmp3F3.bounds;_tmp3F5=_tmp3F3.zero_term;}}_LL232: {union Cyc_CfFlowInfo_FlowInfo
_tmp3F6=f;int _tmp3F7;struct _tuple12 _tmp3F8;struct Cyc_Dict_Dict _tmp3F9;struct Cyc_List_List*
_tmp3FA;_LL236: if((_tmp3F6.BottomFL).tag != 1)goto _LL238;_tmp3F7=(int)(_tmp3F6.BottomFL).val;
_LL237: {struct _tuple14 _tmp8B3;return(_tmp8B3.f1=f,((_tmp8B3.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8B3)));}_LL238: if((_tmp3F6.ReachableFL).tag != 2)goto _LL235;_tmp3F8=(struct
_tuple12)(_tmp3F6.ReachableFL).val;_tmp3F9=_tmp3F8.f1;_tmp3FA=_tmp3F8.f2;_LL239:
if(Cyc_Tcutil_is_bound_one(_tmp3F4)){void*_tmp3FC=r;struct Cyc_CfFlowInfo_Place*
_tmp400;struct Cyc_CfFlowInfo_Place _tmp401;void*_tmp402;struct Cyc_List_List*
_tmp403;enum Cyc_CfFlowInfo_InitLevel _tmp406;_LL23B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3FD=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3FC;if(_tmp3FD->tag != 1)
goto _LL23D;}_LL23C: goto _LL23E;_LL23D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FC;if(_tmp3FE->tag != 2)
goto _LL23F;}_LL23E:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8B6;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8B5;e->annot=(void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((
_tmp8B6.tag=Cyc_CfFlowInfo_NotZero,((_tmp8B6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FA),_tmp8B6)))),_tmp8B5))));}goto _LL23A;_LL23F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FC;if(_tmp3FF->tag != 5)goto
_LL241;else{_tmp400=_tmp3FF->f1;_tmp401=*_tmp400;_tmp402=_tmp401.root;_tmp403=
_tmp401.fields;}}_LL240:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8B9;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8B8;e->annot=(void*)((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8[0]=((
_tmp8B9.tag=Cyc_CfFlowInfo_NotZero,((_tmp8B9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FA),_tmp8B9)))),_tmp8B8))));}{struct Cyc_CfFlowInfo_Place*_tmp8BC;struct
_tuple14 _tmp8BB;return(_tmp8BB.f1=f,((_tmp8BB.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8BC=
_region_malloc(_tmp3EE->r,sizeof(*_tmp8BC)),((_tmp8BC->root=_tmp402,((_tmp8BC->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3EE->r,_tmp403,flds),_tmp8BC))))))),_tmp8BB)));};_LL241: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp404=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3FC;if(_tmp404->tag != 0)goto _LL243;}_LL242: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp8BD;return(_tmp8BD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BD.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8BD)));};_LL243: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp405=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FC;if(_tmp405->tag != 3)goto
_LL245;else{_tmp406=_tmp405->f1;}}_LL244: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp406);goto _LL246;_LL245:;_LL246: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8C0;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8BF;e->annot=(void*)((_tmp8BF=
_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF[0]=((_tmp8C0.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8C0.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8C0)))),
_tmp8BF))));}_LL23A:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8C3;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8C2;e->annot=(void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((
_tmp8C3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8C3.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FA),_tmp8C3)))),_tmp8C2))));}if(Cyc_CfFlowInfo_initlevel(_tmp3EE,_tmp3F9,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8C6;void*_tmp8C5;(_tmp8C5=0,Cyc_Tcutil_terr(
e->loc,((_tmp8C6="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp8C6,sizeof(char),46))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}{struct
_tuple14 _tmp8C7;return(_tmp8C7.f1=f,((_tmp8C7.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8C7)));};_LL235:;}_LL233:;_LL234: {struct Cyc_Core_Impossible_struct _tmp8CD;
const char*_tmp8CC;struct Cyc_Core_Impossible_struct*_tmp8CB;(int)_throw((void*)((
_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CD.tag=Cyc_Core_Impossible,((
_tmp8CD.f1=((_tmp8CC="left deref of non-pointer-type",_tag_dyneither(_tmp8CC,
sizeof(char),31))),_tmp8CD)))),_tmp8CB)))));}_LL230:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp418=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp419=inflow;int _tmp41A;struct _tuple12
_tmp41B;struct Cyc_Dict_Dict _tmp41C;struct Cyc_List_List*_tmp41D;_LL248: if((
_tmp419.BottomFL).tag != 1)goto _LL24A;_tmp41A=(int)(_tmp419.BottomFL).val;_LL249: {
struct _tuple14 _tmp8CE;return(_tmp8CE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8CE.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8CE)));}_LL24A: if((_tmp419.ReachableFL).tag != 2)
goto _LL247;_tmp41B=(struct _tuple12)(_tmp419.ReachableFL).val;_tmp41C=_tmp41B.f1;
_tmp41D=_tmp41B.f2;_LL24B: d=_tmp41C;_LL247:;}{void*_tmp41F=e->r;struct Cyc_Absyn_Exp*
_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;void*_tmp427;
struct Cyc_Absyn_Vardecl*_tmp429;void*_tmp42B;struct Cyc_Absyn_Vardecl*_tmp42D;
void*_tmp42F;struct Cyc_Absyn_Vardecl*_tmp431;void*_tmp433;struct Cyc_Absyn_Vardecl*
_tmp435;struct Cyc_Absyn_Exp*_tmp437;struct _dyneither_ptr*_tmp438;struct Cyc_Absyn_Exp*
_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*
_tmp43F;struct _dyneither_ptr*_tmp440;_LL24D: {struct Cyc_Absyn_Cast_e_struct*
_tmp420=(struct Cyc_Absyn_Cast_e_struct*)_tmp41F;if(_tmp420->tag != 15)goto _LL24F;
else{_tmp421=_tmp420->f2;}}_LL24E: _tmp423=_tmp421;goto _LL250;_LL24F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp422=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp41F;if(_tmp422->tag != 13)
goto _LL251;else{_tmp423=_tmp422->f1;}}_LL250: _tmp425=_tmp423;goto _LL252;_LL251: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp424=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp41F;if(_tmp424->tag != 14)goto _LL253;else{_tmp425=_tmp424->f1;}}_LL252: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp425,flds);_LL253: {
struct Cyc_Absyn_Var_e_struct*_tmp426=(struct Cyc_Absyn_Var_e_struct*)_tmp41F;if(
_tmp426->tag != 1)goto _LL255;else{_tmp427=(void*)_tmp426->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp428=(struct Cyc_Absyn_Param_b_struct*)_tmp427;if(_tmp428->tag != 3)goto _LL255;
else{_tmp429=_tmp428->f1;}};}}_LL254: _tmp42D=_tmp429;goto _LL256;_LL255: {struct
Cyc_Absyn_Var_e_struct*_tmp42A=(struct Cyc_Absyn_Var_e_struct*)_tmp41F;if(_tmp42A->tag
!= 1)goto _LL257;else{_tmp42B=(void*)_tmp42A->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp42C=(struct Cyc_Absyn_Local_b_struct*)_tmp42B;if(_tmp42C->tag != 4)goto _LL257;
else{_tmp42D=_tmp42C->f1;}};}}_LL256: _tmp431=_tmp42D;goto _LL258;_LL257: {struct
Cyc_Absyn_Var_e_struct*_tmp42E=(struct Cyc_Absyn_Var_e_struct*)_tmp41F;if(_tmp42E->tag
!= 1)goto _LL259;else{_tmp42F=(void*)_tmp42E->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp430=(struct Cyc_Absyn_Pat_b_struct*)_tmp42F;if(_tmp430->tag != 5)goto _LL259;
else{_tmp431=_tmp430->f1;}};}}_LL258: {struct Cyc_CfFlowInfo_Place*_tmp8D8;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8D7;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8D6;
struct _tuple14 _tmp8D5;return(_tmp8D5.f1=inflow,((_tmp8D5.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8D8=_region_malloc(env->r,sizeof(*_tmp8D8)),((_tmp8D8->root=(void*)((_tmp8D6=
_region_malloc(env->r,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=0,((_tmp8D7.f1=
_tmp431,_tmp8D7)))),_tmp8D6)))),((_tmp8D8->fields=flds,_tmp8D8))))))),_tmp8D5)));}
_LL259: {struct Cyc_Absyn_Var_e_struct*_tmp432=(struct Cyc_Absyn_Var_e_struct*)
_tmp41F;if(_tmp432->tag != 1)goto _LL25B;else{_tmp433=(void*)_tmp432->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp434=(struct Cyc_Absyn_Global_b_struct*)_tmp433;if(
_tmp434->tag != 1)goto _LL25B;else{_tmp435=_tmp434->f1;}};}}_LL25A: {struct
_tuple14 _tmp8D9;return(_tmp8D9.f1=inflow,((_tmp8D9.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8D9)));}_LL25B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp436=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp41F;if(_tmp436->tag != 23)goto _LL25D;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}
_LL25C:{void*_tmp446=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp437->topt))->v);struct Cyc_Absyn_PtrInfo _tmp448;void*_tmp449;
_LL266: {struct Cyc_Absyn_PointerType_struct*_tmp447=(struct Cyc_Absyn_PointerType_struct*)
_tmp446;if(_tmp447->tag != 5)goto _LL268;else{_tmp448=_tmp447->f1;_tmp449=_tmp448.elt_typ;}}
_LL267: if(!Cyc_Absyn_is_nontagged_union_type(_tmp449)){struct Cyc_List_List*
_tmp8DA;flds=((_tmp8DA=_region_malloc(env->r,sizeof(*_tmp8DA)),((_tmp8DA->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp449,_tmp438),((_tmp8DA->tl=flds,_tmp8DA))))));}
goto _LL265;_LL268:;_LL269: {struct Cyc_Core_Impossible_struct _tmp8E0;const char*
_tmp8DF;struct Cyc_Core_Impossible_struct*_tmp8DE;(int)_throw((void*)((_tmp8DE=
_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8E0.tag=Cyc_Core_Impossible,((
_tmp8E0.f1=((_tmp8DF="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8DF,sizeof(
char),25))),_tmp8E0)))),_tmp8DE)))));}_LL265:;}_tmp43A=_tmp437;goto _LL25E;_LL25D: {
struct Cyc_Absyn_Deref_e_struct*_tmp439=(struct Cyc_Absyn_Deref_e_struct*)_tmp41F;
if(_tmp439->tag != 21)goto _LL25F;else{_tmp43A=_tmp439->f1;}}_LL25E: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43A->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_AbsLVal
_tmp450;struct _tuple14 _tmp44E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp43A);
_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;{struct _tuple14 _tmp8E1;struct _tuple14
_tmp452=(_tmp8E1.f1=_tmp44F,((_tmp8E1.f2=_tmp450,_tmp8E1)));union Cyc_CfFlowInfo_FlowInfo
_tmp453;struct _tuple12 _tmp454;struct Cyc_Dict_Dict _tmp455;struct Cyc_List_List*
_tmp456;union Cyc_CfFlowInfo_AbsLVal _tmp457;struct Cyc_CfFlowInfo_Place*_tmp458;
_LL26B: _tmp453=_tmp452.f1;if((_tmp453.ReachableFL).tag != 2)goto _LL26D;_tmp454=(
struct _tuple12)(_tmp453.ReachableFL).val;_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;
_tmp457=_tmp452.f2;if((_tmp457.PlaceL).tag != 1)goto _LL26D;_tmp458=(struct Cyc_CfFlowInfo_Place*)(
_tmp457.PlaceL).val;_LL26C: {void*_tmp459=Cyc_CfFlowInfo_lookup_place(_tmp455,
_tmp458);void*_tmp45A=_tmp459;_LL270: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp45B=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp45A;if(_tmp45B->tag != 5)goto
_LL272;}_LL271: return Cyc_NewControlFlow_anal_derefL(env,_tmp44F,_tmp43A,_tmp44F,
_tmp459,flds);_LL272:;_LL273: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp418,_tmp455,_tmp459);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp418->unknown_all:
_tmp418->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp418,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43A->topt))->v),0,leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8E4;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8E3;void*new_root=(void*)((
_tmp8E3=_region_malloc(_tmp418->r,sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=1,((
_tmp8E4.f1=e,((_tmp8E4.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8E4)))))),_tmp8E3))));struct Cyc_CfFlowInfo_Place*_tmp8E5;struct Cyc_CfFlowInfo_Place*
place=(_tmp8E5=_region_malloc(_tmp418->r,sizeof(*_tmp8E5)),((_tmp8E5->root=
new_root,((_tmp8E5->fields=0,_tmp8E5)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp8E8;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8E7;void*res=(void*)((_tmp8E7=
_region_malloc(_tmp418->r,sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=5,((
_tmp8E8.f1=place,_tmp8E8)))),_tmp8E7))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp455=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp455,new_root,new_rval);_tmp455=Cyc_CfFlowInfo_assign_place(
_tmp418,e->loc,_tmp455,env->all_changed,_tmp458,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp45C=Cyc_CfFlowInfo_ReachableFL(_tmp455,_tmp456);return Cyc_NewControlFlow_anal_derefL(
env,_tmp45C,_tmp43A,_tmp45C,res,flds);};}_LL26F:;}_LL26D:;_LL26E: goto _LL26A;
_LL26A:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp463;void*_tmp464;struct _tuple13
_tmp462=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43A);_tmp463=_tmp462.f1;
_tmp464=_tmp462.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43A,
_tmp463,_tmp464,flds);};_LL25F: {struct Cyc_Absyn_Subscript_e_struct*_tmp43B=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp41F;if(_tmp43B->tag != 24)goto _LL261;
else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}_LL260: {void*_tmp465=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp468;struct Cyc_Absyn_PtrAtts _tmp469;union Cyc_Absyn_Constraint*_tmp46A;_LL275: {
struct Cyc_Absyn_TupleType_struct*_tmp466=(struct Cyc_Absyn_TupleType_struct*)
_tmp465;if(_tmp466->tag != 11)goto _LL277;}_LL276: {unsigned int _tmp46B=(Cyc_Evexp_eval_const_uint_exp(
_tmp43D)).f1;struct Cyc_List_List*_tmp8E9;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp43C,((_tmp8E9=_region_malloc(env->r,sizeof(*_tmp8E9)),((
_tmp8E9->hd=(void*)((int)_tmp46B),((_tmp8E9->tl=flds,_tmp8E9)))))));}_LL277: {
struct Cyc_Absyn_PointerType_struct*_tmp467=(struct Cyc_Absyn_PointerType_struct*)
_tmp465;if(_tmp467->tag != 5)goto _LL279;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.ptr_atts;
_tmp46A=_tmp469.bounds;}}_LL278: {struct _RegionHandle*_tmp46D=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp470;struct Cyc_List_List*_tmp471;struct Cyc_Absyn_Exp*_tmp8EA[2];struct
_tuple19 _tmp46F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp46D,env,inflow,((
_tmp8EA[1]=_tmp43D,((_tmp8EA[0]=_tmp43C,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp46D,_tag_dyneither(
_tmp8EA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp470=_tmp46F.f1;_tmp471=
_tmp46F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp472=_tmp470;{union Cyc_CfFlowInfo_FlowInfo
_tmp473=_tmp470;struct _tuple12 _tmp474;struct Cyc_Dict_Dict _tmp475;struct Cyc_List_List*
_tmp476;_LL27C: if((_tmp473.ReachableFL).tag != 2)goto _LL27E;_tmp474=(struct
_tuple12)(_tmp473.ReachableFL).val;_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_LL27D:
if(Cyc_CfFlowInfo_initlevel(_tmp418,_tmp475,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp471))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,Cyc_Tcutil_terr(_tmp43D->loc,((_tmp8ED="expression may not be initialized",
_tag_dyneither(_tmp8ED,sizeof(char),34))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp479=Cyc_NewControlFlow_add_subscript_reln(_tmp418->r,
_tmp476,_tmp43C,_tmp43D);if(_tmp476 != _tmp479)_tmp472=Cyc_CfFlowInfo_ReachableFL(
_tmp475,_tmp479);goto _LL27B;};_LL27E:;_LL27F: goto _LL27B;_LL27B:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp47B;union Cyc_CfFlowInfo_AbsLVal _tmp47C;struct _tuple14 _tmp47A=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp43C,_tmp470,(void*)((struct Cyc_List_List*)_check_null(_tmp471))->hd,
flds);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp47D=
_tmp47B;int _tmp47E;_LL281: if((_tmp47D.BottomFL).tag != 1)goto _LL283;_tmp47E=(int)(
_tmp47D.BottomFL).val;_LL282: {struct _tuple14 _tmp8EE;return(_tmp8EE.f1=_tmp47B,((
_tmp8EE.f2=_tmp47C,_tmp8EE)));}_LL283:;_LL284: {struct _tuple14 _tmp8EF;return(
_tmp8EF.f1=_tmp472,((_tmp8EF.f2=_tmp47C,_tmp8EF)));}_LL280:;};};};}_LL279:;
_LL27A: {struct Cyc_Core_Impossible_struct _tmp8F5;const char*_tmp8F4;struct Cyc_Core_Impossible_struct*
_tmp8F3;(int)_throw((void*)((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3[0]=((
_tmp8F5.tag=Cyc_Core_Impossible,((_tmp8F5.f1=((_tmp8F4="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp8F4,sizeof(char),33))),_tmp8F5)))),_tmp8F3)))));}_LL274:;}
_LL261: {struct Cyc_Absyn_AggrMember_e_struct*_tmp43E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp41F;if(_tmp43E->tag != 22)goto _LL263;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}
_LL262: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43F->topt))->v)){struct _tuple14 _tmp8F6;return(_tmp8F6.f1=inflow,((
_tmp8F6.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F6)));}{struct Cyc_List_List*_tmp8F7;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp43F,((
_tmp8F7=_region_malloc(env->r,sizeof(*_tmp8F7)),((_tmp8F7->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43F->topt))->v,_tmp440),((_tmp8F7->tl=
flds,_tmp8F7)))))));};_LL263:;_LL264: {struct _tuple14 _tmp8F8;return(_tmp8F8.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp8F8.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F8)));}
_LL24C:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp488;union Cyc_CfFlowInfo_AbsLVal _tmp489;struct
_tuple14 _tmp487=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;{struct _tuple14 _tmp8F9;return(_tmp8F9.f1=
_tmp488,((_tmp8F9.f2=_tmp489,_tmp8F9)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp48B=env->fenv;{void*_tmp48C=e->r;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*
_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*
_tmp493;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*
_tmp498;struct Cyc_Absyn_Exp*_tmp499;enum Cyc_Absyn_Primop _tmp49B;struct Cyc_List_List*
_tmp49C;struct Cyc_List_List _tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_List_List*
_tmp49F;enum Cyc_Absyn_Primop _tmp4A1;struct Cyc_List_List*_tmp4A2;_LL286: {struct
Cyc_Absyn_Conditional_e_struct*_tmp48D=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp48C;if(_tmp48D->tag != 6)goto _LL288;else{_tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;
_tmp490=_tmp48D->f3;}}_LL287: {union Cyc_CfFlowInfo_FlowInfo _tmp4A4;union Cyc_CfFlowInfo_FlowInfo
_tmp4A5;struct _tuple15 _tmp4A3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48E);
_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4A7;union
Cyc_CfFlowInfo_FlowInfo _tmp4A8;struct _tuple15 _tmp4A6=Cyc_NewControlFlow_anal_test(
env,_tmp4A4,_tmp48F);_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;struct _tuple15 _tmp4A9=Cyc_NewControlFlow_anal_test(
env,_tmp4A5,_tmp490);_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{struct _tuple15
_tmp8FA;return(_tmp8FA.f1=Cyc_CfFlowInfo_join_flow(_tmp48B,env->all_changed,
_tmp4A7,_tmp4AA),((_tmp8FA.f2=Cyc_CfFlowInfo_join_flow(_tmp48B,env->all_changed,
_tmp4A8,_tmp4AB),_tmp8FA)));};};};}_LL288: {struct Cyc_Absyn_And_e_struct*_tmp491=(
struct Cyc_Absyn_And_e_struct*)_tmp48C;if(_tmp491->tag != 7)goto _LL28A;else{
_tmp492=_tmp491->f1;_tmp493=_tmp491->f2;}}_LL289: {union Cyc_CfFlowInfo_FlowInfo
_tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;struct _tuple15 _tmp4AD=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp492);_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B1;union Cyc_CfFlowInfo_FlowInfo _tmp4B2;struct _tuple15 _tmp4B0=Cyc_NewControlFlow_anal_test(
env,_tmp4AE,_tmp493);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;{struct _tuple15
_tmp8FB;return(_tmp8FB.f1=_tmp4B1,((_tmp8FB.f2=Cyc_CfFlowInfo_join_flow(_tmp48B,
env->all_changed,_tmp4AF,_tmp4B2),_tmp8FB)));};};}_LL28A: {struct Cyc_Absyn_Or_e_struct*
_tmp494=(struct Cyc_Absyn_Or_e_struct*)_tmp48C;if(_tmp494->tag != 8)goto _LL28C;
else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;}}_LL28B: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B5;union Cyc_CfFlowInfo_FlowInfo _tmp4B6;struct _tuple15 _tmp4B4=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp495);_tmp4B5=_tmp4B4.f1;_tmp4B6=_tmp4B4.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B8;union Cyc_CfFlowInfo_FlowInfo _tmp4B9;struct _tuple15 _tmp4B7=Cyc_NewControlFlow_anal_test(
env,_tmp4B6,_tmp496);_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;{struct _tuple15
_tmp8FC;return(_tmp8FC.f1=Cyc_CfFlowInfo_join_flow(_tmp48B,env->all_changed,
_tmp4B5,_tmp4B8),((_tmp8FC.f2=_tmp4B9,_tmp8FC)));};};}_LL28C: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp497=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp48C;if(_tmp497->tag != 9)goto _LL28E;
else{_tmp498=_tmp497->f1;_tmp499=_tmp497->f2;}}_LL28D: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BC;void*_tmp4BD;struct _tuple13 _tmp4BB=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp498);_tmp4BC=_tmp4BB.f1;_tmp4BD=_tmp4BB.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4BC,_tmp499);}_LL28E: {struct Cyc_Absyn_Primop_e_struct*_tmp49A=(struct
Cyc_Absyn_Primop_e_struct*)_tmp48C;if(_tmp49A->tag != 3)goto _LL290;else{_tmp49B=
_tmp49A->f1;if(_tmp49B != Cyc_Absyn_Not)goto _LL290;_tmp49C=_tmp49A->f2;if(_tmp49C
== 0)goto _LL290;_tmp49D=*_tmp49C;_tmp49E=(struct Cyc_Absyn_Exp*)_tmp49D.hd;
_tmp49F=_tmp49D.tl;if(_tmp49F != 0)goto _LL290;}}_LL28F: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BF;union Cyc_CfFlowInfo_FlowInfo _tmp4C0;struct _tuple15 _tmp4BE=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp49E);_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;{struct _tuple15 _tmp8FD;
return(_tmp8FD.f1=_tmp4C0,((_tmp8FD.f2=_tmp4BF,_tmp8FD)));};}_LL290: {struct Cyc_Absyn_Primop_e_struct*
_tmp4A0=(struct Cyc_Absyn_Primop_e_struct*)_tmp48C;if(_tmp4A0->tag != 3)goto _LL292;
else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}_LL291: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4C2=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4C4;struct
Cyc_List_List*_tmp4C5;struct _tuple19 _tmp4C3=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4C2,env,inflow,_tmp4A2,0,0);_tmp4C4=_tmp4C3.f1;_tmp4C5=_tmp4C3.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4C5))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4C5->tl))->hd;f=_tmp4C4;}{union Cyc_CfFlowInfo_FlowInfo _tmp4C6=f;
int _tmp4C7;struct _tuple12 _tmp4C8;struct Cyc_Dict_Dict _tmp4C9;struct Cyc_List_List*
_tmp4CA;_LL295: if((_tmp4C6.BottomFL).tag != 1)goto _LL297;_tmp4C7=(int)(_tmp4C6.BottomFL).val;
_LL296: {struct _tuple15 _tmp8FE;return(_tmp8FE.f1=f,((_tmp8FE.f2=f,_tmp8FE)));}
_LL297: if((_tmp4C6.ReachableFL).tag != 2)goto _LL294;_tmp4C8=(struct _tuple12)(
_tmp4C6.ReachableFL).val;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_LL298: {struct
Cyc_Absyn_Exp*_tmp4CC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4A2))->hd;struct Cyc_Absyn_Exp*_tmp4CD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A2->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4C9,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp901;void*_tmp900;(_tmp900=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp4A2->hd)->loc,((_tmp901="expression may not be initialized",
_tag_dyneither(_tmp901,sizeof(char),34))),_tag_dyneither(_tmp900,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4C9,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp904;void*_tmp903;(_tmp903=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2->tl))->hd)->loc,((_tmp904="expression may not be initialized",
_tag_dyneither(_tmp904,sizeof(char),34))),_tag_dyneither(_tmp903,sizeof(void*),0)));}
if(_tmp4A1 == Cyc_Absyn_Eq  || _tmp4A1 == Cyc_Absyn_Neq){struct _tuple0 _tmp905;
struct _tuple0 _tmp4D3=(_tmp905.f1=r1,((_tmp905.f2=r2,_tmp905)));void*_tmp4D4;
enum Cyc_CfFlowInfo_InitLevel _tmp4D6;void*_tmp4D7;void*_tmp4D9;void*_tmp4DB;
enum Cyc_CfFlowInfo_InitLevel _tmp4DD;void*_tmp4DE;void*_tmp4E0;void*_tmp4E2;void*
_tmp4E4;void*_tmp4E6;void*_tmp4E8;void*_tmp4EA;void*_tmp4EC;void*_tmp4EE;void*
_tmp4F0;void*_tmp4F2;void*_tmp4F4;void*_tmp4F6;void*_tmp4F8;_LL29A: _tmp4D4=
_tmp4D3.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4D5=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4D4;if(_tmp4D5->tag != 3)goto _LL29C;else{_tmp4D6=_tmp4D5->f1;}};_tmp4D7=
_tmp4D3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D7;if(_tmp4D8->tag != 0)goto _LL29C;};_LL29B: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FB;union Cyc_CfFlowInfo_FlowInfo _tmp4FC;struct _tuple15 _tmp4FA=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4CC,_tmp4D6);_tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;switch(_tmp4A1){
case Cyc_Absyn_Eq: _LL2AE: {struct _tuple15 _tmp906;return(_tmp906.f1=_tmp4FC,((
_tmp906.f2=_tmp4FB,_tmp906)));}case Cyc_Absyn_Neq: _LL2AF: {struct _tuple15 _tmp907;
return(_tmp907.f1=_tmp4FB,((_tmp907.f2=_tmp4FC,_tmp907)));}default: _LL2B0: {
struct Cyc_Core_Impossible_struct _tmp90D;const char*_tmp90C;struct Cyc_Core_Impossible_struct*
_tmp90B;(int)_throw((void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((
_tmp90D.tag=Cyc_Core_Impossible,((_tmp90D.f1=((_tmp90C="anal_test, zero-split",
_tag_dyneither(_tmp90C,sizeof(char),22))),_tmp90D)))),_tmp90B)))));}}}_LL29C:
_tmp4D9=_tmp4D3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D9;if(_tmp4DA->tag != 0)goto _LL29E;};_tmp4DB=_tmp4D3.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4DC=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4DB;if(_tmp4DC->tag != 3)goto
_LL29E;else{_tmp4DD=_tmp4DC->f1;}};_LL29D: {union Cyc_CfFlowInfo_FlowInfo _tmp503;
union Cyc_CfFlowInfo_FlowInfo _tmp504;struct _tuple15 _tmp502=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4CD,_tmp4DD);_tmp503=_tmp502.f1;_tmp504=_tmp502.f2;switch(_tmp4A1){
case Cyc_Absyn_Eq: _LL2B2: {struct _tuple15 _tmp90E;return(_tmp90E.f1=_tmp504,((
_tmp90E.f2=_tmp503,_tmp90E)));}case Cyc_Absyn_Neq: _LL2B3: {struct _tuple15 _tmp90F;
return(_tmp90F.f1=_tmp503,((_tmp90F.f2=_tmp504,_tmp90F)));}default: _LL2B4: {
struct Cyc_Core_Impossible_struct _tmp915;const char*_tmp914;struct Cyc_Core_Impossible_struct*
_tmp913;(int)_throw((void*)((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913[0]=((
_tmp915.tag=Cyc_Core_Impossible,((_tmp915.f1=((_tmp914="anal_test, zero-split",
_tag_dyneither(_tmp914,sizeof(char),22))),_tmp915)))),_tmp913)))));}}}_LL29E:
_tmp4DE=_tmp4D3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DE;if(_tmp4DF->tag != 0)goto _LL2A0;};_tmp4E0=_tmp4D3.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E1=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E0;if(_tmp4E1->tag != 0)goto
_LL2A0;};_LL29F: if(_tmp4A1 == Cyc_Absyn_Eq){struct _tuple15 _tmp916;return(_tmp916.f1=
f,((_tmp916.f2=Cyc_CfFlowInfo_BottomFL(),_tmp916)));}else{struct _tuple15 _tmp917;
return(_tmp917.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp917.f2=f,_tmp917)));}_LL2A0:
_tmp4E2=_tmp4D3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E2;if(_tmp4E3->tag != 0)goto _LL2A2;};_tmp4E4=_tmp4D3.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4E5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4E4;if(_tmp4E5->tag != 1)
goto _LL2A2;};_LL2A1: goto _LL2A3;_LL2A2: _tmp4E6=_tmp4D3.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E6;if(_tmp4E7->tag != 0)goto
_LL2A4;};_tmp4E8=_tmp4D3.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4E9=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4E8;if(_tmp4E9->tag != 2)goto _LL2A4;};
_LL2A3: goto _LL2A5;_LL2A4: _tmp4EA=_tmp4D3.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4EB=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4EA;if(_tmp4EB->tag != 0)goto
_LL2A6;};_tmp4EC=_tmp4D3.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4ED=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4EC;if(_tmp4ED->tag != 5)goto _LL2A6;};
_LL2A5: goto _LL2A7;_LL2A6: _tmp4EE=_tmp4D3.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4EF=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4EE;if(_tmp4EF->tag != 1)
goto _LL2A8;};_tmp4F0=_tmp4D3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F1=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F0;if(_tmp4F1->tag != 0)goto _LL2A8;};
_LL2A7: goto _LL2A9;_LL2A8: _tmp4F2=_tmp4D3.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4F3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4F2;if(_tmp4F3->tag != 2)
goto _LL2AA;};_tmp4F4=_tmp4D3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F5=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F4;if(_tmp4F5->tag != 0)goto _LL2AA;};
_LL2A9: goto _LL2AB;_LL2AA: _tmp4F6=_tmp4D3.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4F7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4F6;if(_tmp4F7->tag != 5)goto
_LL2AC;};_tmp4F8=_tmp4D3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4F8;if(_tmp4F9->tag != 0)goto _LL2AC;};_LL2AB: if(_tmp4A1 == Cyc_Absyn_Neq){
struct _tuple15 _tmp918;return(_tmp918.f1=f,((_tmp918.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp918)));}else{struct _tuple15 _tmp919;return(_tmp919.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp919.f2=f,_tmp919)));}_LL2AC:;_LL2AD: goto _LL299;_LL299:;}{struct _tuple0
_tmp91A;struct _tuple0 _tmp50F=(_tmp91A.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4CC->topt))->v),((_tmp91A.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4CD->topt))->v),_tmp91A)));void*_tmp510;enum Cyc_Absyn_Sign
_tmp512;void*_tmp513;enum Cyc_Absyn_Sign _tmp515;void*_tmp516;void*_tmp518;_LL2B7:
_tmp510=_tmp50F.f1;{struct Cyc_Absyn_IntType_struct*_tmp511=(struct Cyc_Absyn_IntType_struct*)
_tmp510;if(_tmp511->tag != 6)goto _LL2B9;else{_tmp512=_tmp511->f1;if(_tmp512 != Cyc_Absyn_Unsigned)
goto _LL2B9;}};_LL2B8: goto _LL2BA;_LL2B9: _tmp513=_tmp50F.f2;{struct Cyc_Absyn_IntType_struct*
_tmp514=(struct Cyc_Absyn_IntType_struct*)_tmp513;if(_tmp514->tag != 6)goto _LL2BB;
else{_tmp515=_tmp514->f1;if(_tmp515 != Cyc_Absyn_Unsigned)goto _LL2BB;}};_LL2BA:
goto _LL2BC;_LL2BB: _tmp516=_tmp50F.f1;{struct Cyc_Absyn_TagType_struct*_tmp517=(
struct Cyc_Absyn_TagType_struct*)_tmp516;if(_tmp517->tag != 20)goto _LL2BD;};_LL2BC:
goto _LL2BE;_LL2BD: _tmp518=_tmp50F.f2;{struct Cyc_Absyn_TagType_struct*_tmp519=(
struct Cyc_Absyn_TagType_struct*)_tmp518;if(_tmp519->tag != 20)goto _LL2BF;};_LL2BE:
goto _LL2B6;_LL2BF:;_LL2C0: {struct _tuple15 _tmp91B;return(_tmp91B.f1=f,((_tmp91B.f2=
f,_tmp91B)));}_LL2B6:;}switch(_tmp4A1){case Cyc_Absyn_Eq: _LL2C1: goto _LL2C2;case
Cyc_Absyn_Neq: _LL2C2: goto _LL2C3;case Cyc_Absyn_Gt: _LL2C3: goto _LL2C4;case Cyc_Absyn_Gte:
_LL2C4: {struct _tuple15 _tmp91C;return(_tmp91C.f1=f,((_tmp91C.f2=f,_tmp91C)));}
case Cyc_Absyn_Lt: _LL2C5: {union Cyc_CfFlowInfo_FlowInfo _tmp51C=f;union Cyc_CfFlowInfo_FlowInfo
_tmp51D=f;{union Cyc_CfFlowInfo_FlowInfo _tmp51E=_tmp51C;int _tmp51F;struct _tuple12
_tmp520;struct Cyc_Dict_Dict _tmp521;_LL2C8: if((_tmp51E.BottomFL).tag != 1)goto
_LL2CA;_tmp51F=(int)(_tmp51E.BottomFL).val;_LL2C9: {struct Cyc_Core_Impossible_struct
_tmp922;const char*_tmp921;struct Cyc_Core_Impossible_struct*_tmp920;(int)_throw((
void*)((_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((_tmp922.tag=Cyc_Core_Impossible,((
_tmp922.f1=((_tmp921="anal_test, Lt",_tag_dyneither(_tmp921,sizeof(char),14))),
_tmp922)))),_tmp920)))));}_LL2CA: if((_tmp51E.ReachableFL).tag != 2)goto _LL2C7;
_tmp520=(struct _tuple12)(_tmp51E.ReachableFL).val;_tmp521=_tmp520.f1;_LL2CB:
_tmp4C9=_tmp521;_LL2C7:;}{void*_tmp525=_tmp4CC->r;void*_tmp527;struct Cyc_Absyn_Vardecl*
_tmp529;void*_tmp52B;struct Cyc_Absyn_Vardecl*_tmp52D;void*_tmp52F;struct Cyc_Absyn_Vardecl*
_tmp531;void*_tmp533;struct Cyc_Absyn_Vardecl*_tmp535;_LL2CD:{struct Cyc_Absyn_Var_e_struct*
_tmp526=(struct Cyc_Absyn_Var_e_struct*)_tmp525;if(_tmp526->tag != 1)goto _LL2CF;
else{_tmp527=(void*)_tmp526->f2;{struct Cyc_Absyn_Global_b_struct*_tmp528=(struct
Cyc_Absyn_Global_b_struct*)_tmp527;if(_tmp528->tag != 1)goto _LL2CF;else{_tmp529=
_tmp528->f1;}};}}if(!(!_tmp529->escapes))goto _LL2CF;_LL2CE: _tmp52D=_tmp529;goto
_LL2D0;_LL2CF:{struct Cyc_Absyn_Var_e_struct*_tmp52A=(struct Cyc_Absyn_Var_e_struct*)
_tmp525;if(_tmp52A->tag != 1)goto _LL2D1;else{_tmp52B=(void*)_tmp52A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp52C=(struct Cyc_Absyn_Local_b_struct*)_tmp52B;if(
_tmp52C->tag != 4)goto _LL2D1;else{_tmp52D=_tmp52C->f1;}};}}if(!(!_tmp52D->escapes))
goto _LL2D1;_LL2D0: _tmp531=_tmp52D;goto _LL2D2;_LL2D1:{struct Cyc_Absyn_Var_e_struct*
_tmp52E=(struct Cyc_Absyn_Var_e_struct*)_tmp525;if(_tmp52E->tag != 1)goto _LL2D3;
else{_tmp52F=(void*)_tmp52E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp530=(struct Cyc_Absyn_Pat_b_struct*)
_tmp52F;if(_tmp530->tag != 5)goto _LL2D3;else{_tmp531=_tmp530->f1;}};}}if(!(!
_tmp531->escapes))goto _LL2D3;_LL2D2: _tmp535=_tmp531;goto _LL2D4;_LL2D3:{struct Cyc_Absyn_Var_e_struct*
_tmp532=(struct Cyc_Absyn_Var_e_struct*)_tmp525;if(_tmp532->tag != 1)goto _LL2D5;
else{_tmp533=(void*)_tmp532->f2;{struct Cyc_Absyn_Param_b_struct*_tmp534=(struct
Cyc_Absyn_Param_b_struct*)_tmp533;if(_tmp534->tag != 3)goto _LL2D5;else{_tmp535=
_tmp534->f1;}};}}if(!(!_tmp535->escapes))goto _LL2D5;_LL2D4: {void*_tmp536=
_tmp4CD->r;void*_tmp538;struct Cyc_Absyn_Vardecl*_tmp53A;void*_tmp53C;struct Cyc_Absyn_Vardecl*
_tmp53E;void*_tmp540;struct Cyc_Absyn_Vardecl*_tmp542;void*_tmp544;struct Cyc_Absyn_Vardecl*
_tmp546;union Cyc_Absyn_Cnst _tmp548;struct _tuple6 _tmp549;int _tmp54A;struct Cyc_Absyn_Exp*
_tmp54C;struct Cyc_Absyn_Exp _tmp54D;void*_tmp54E;union Cyc_Absyn_Cnst _tmp550;
struct _tuple6 _tmp551;int _tmp552;enum Cyc_Absyn_Primop _tmp554;struct Cyc_List_List*
_tmp555;struct Cyc_List_List _tmp556;struct Cyc_Absyn_Exp*_tmp557;_LL2D8:{struct Cyc_Absyn_Var_e_struct*
_tmp537=(struct Cyc_Absyn_Var_e_struct*)_tmp536;if(_tmp537->tag != 1)goto _LL2DA;
else{_tmp538=(void*)_tmp537->f2;{struct Cyc_Absyn_Global_b_struct*_tmp539=(struct
Cyc_Absyn_Global_b_struct*)_tmp538;if(_tmp539->tag != 1)goto _LL2DA;else{_tmp53A=
_tmp539->f1;}};}}if(!(!_tmp53A->escapes))goto _LL2DA;_LL2D9: _tmp53E=_tmp53A;goto
_LL2DB;_LL2DA:{struct Cyc_Absyn_Var_e_struct*_tmp53B=(struct Cyc_Absyn_Var_e_struct*)
_tmp536;if(_tmp53B->tag != 1)goto _LL2DC;else{_tmp53C=(void*)_tmp53B->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp53D=(struct Cyc_Absyn_Local_b_struct*)_tmp53C;if(
_tmp53D->tag != 4)goto _LL2DC;else{_tmp53E=_tmp53D->f1;}};}}if(!(!_tmp53E->escapes))
goto _LL2DC;_LL2DB: _tmp542=_tmp53E;goto _LL2DD;_LL2DC:{struct Cyc_Absyn_Var_e_struct*
_tmp53F=(struct Cyc_Absyn_Var_e_struct*)_tmp536;if(_tmp53F->tag != 1)goto _LL2DE;
else{_tmp540=(void*)_tmp53F->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp541=(struct Cyc_Absyn_Pat_b_struct*)
_tmp540;if(_tmp541->tag != 5)goto _LL2DE;else{_tmp542=_tmp541->f1;}};}}if(!(!
_tmp542->escapes))goto _LL2DE;_LL2DD: _tmp546=_tmp542;goto _LL2DF;_LL2DE:{struct Cyc_Absyn_Var_e_struct*
_tmp543=(struct Cyc_Absyn_Var_e_struct*)_tmp536;if(_tmp543->tag != 1)goto _LL2E0;
else{_tmp544=(void*)_tmp543->f2;{struct Cyc_Absyn_Param_b_struct*_tmp545=(struct
Cyc_Absyn_Param_b_struct*)_tmp544;if(_tmp545->tag != 3)goto _LL2E0;else{_tmp546=
_tmp545->f1;}};}}if(!(!_tmp546->escapes))goto _LL2E0;_LL2DF: {struct _RegionHandle*
_tmp558=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp925;struct Cyc_List_List*
_tmp924;struct Cyc_List_List*_tmp559=(_tmp924=_region_malloc(_tmp558,sizeof(*
_tmp924)),((_tmp924->hd=((_tmp925=_region_malloc(_tmp558,sizeof(*_tmp925)),((
_tmp925->vd=_tmp535,((_tmp925->rop=Cyc_CfFlowInfo_LessVar(_tmp546,_tmp546->type),
_tmp925)))))),((_tmp924->tl=_tmp4CA,_tmp924)))));struct _tuple15 _tmp926;return(
_tmp926.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C9,_tmp559),((_tmp926.f2=_tmp51D,
_tmp926)));}_LL2E0: {struct Cyc_Absyn_Const_e_struct*_tmp547=(struct Cyc_Absyn_Const_e_struct*)
_tmp536;if(_tmp547->tag != 0)goto _LL2E2;else{_tmp548=_tmp547->f1;if((_tmp548.Int_c).tag
!= 4)goto _LL2E2;_tmp549=(struct _tuple6)(_tmp548.Int_c).val;_tmp54A=_tmp549.f2;}}
_LL2E1: _tmp552=_tmp54A;goto _LL2E3;_LL2E2: {struct Cyc_Absyn_Cast_e_struct*_tmp54B=(
struct Cyc_Absyn_Cast_e_struct*)_tmp536;if(_tmp54B->tag != 15)goto _LL2E4;else{
_tmp54C=_tmp54B->f2;_tmp54D=*_tmp54C;_tmp54E=_tmp54D.r;{struct Cyc_Absyn_Const_e_struct*
_tmp54F=(struct Cyc_Absyn_Const_e_struct*)_tmp54E;if(_tmp54F->tag != 0)goto _LL2E4;
else{_tmp550=_tmp54F->f1;if((_tmp550.Int_c).tag != 4)goto _LL2E4;_tmp551=(struct
_tuple6)(_tmp550.Int_c).val;_tmp552=_tmp551.f2;}};}}_LL2E3: {struct _RegionHandle*
_tmp55D=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp929;struct Cyc_List_List*
_tmp928;struct Cyc_List_List*_tmp55E=(_tmp928=_region_malloc(_tmp55D,sizeof(*
_tmp928)),((_tmp928->hd=((_tmp929=_region_malloc(_tmp55D,sizeof(*_tmp929)),((
_tmp929->vd=_tmp535,((_tmp929->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp552),_tmp929)))))),((_tmp928->tl=_tmp4CA,_tmp928)))));struct _tuple15 _tmp92A;
return(_tmp92A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C9,_tmp55E),((_tmp92A.f2=
_tmp51D,_tmp92A)));}_LL2E4: {struct Cyc_Absyn_Primop_e_struct*_tmp553=(struct Cyc_Absyn_Primop_e_struct*)
_tmp536;if(_tmp553->tag != 3)goto _LL2E6;else{_tmp554=_tmp553->f1;_tmp555=_tmp553->f2;
if(_tmp555 == 0)goto _LL2E6;_tmp556=*_tmp555;_tmp557=(struct Cyc_Absyn_Exp*)_tmp556.hd;}}
_LL2E5: {void*_tmp562=_tmp557->r;void*_tmp564;struct Cyc_Absyn_Vardecl*_tmp566;
void*_tmp568;struct Cyc_Absyn_Vardecl*_tmp56A;void*_tmp56C;struct Cyc_Absyn_Vardecl*
_tmp56E;void*_tmp570;struct Cyc_Absyn_Vardecl*_tmp572;_LL2E9:{struct Cyc_Absyn_Var_e_struct*
_tmp563=(struct Cyc_Absyn_Var_e_struct*)_tmp562;if(_tmp563->tag != 1)goto _LL2EB;
else{_tmp564=(void*)_tmp563->f2;{struct Cyc_Absyn_Global_b_struct*_tmp565=(struct
Cyc_Absyn_Global_b_struct*)_tmp564;if(_tmp565->tag != 1)goto _LL2EB;else{_tmp566=
_tmp565->f1;}};}}if(!(!_tmp566->escapes))goto _LL2EB;_LL2EA: _tmp56A=_tmp566;goto
_LL2EC;_LL2EB:{struct Cyc_Absyn_Var_e_struct*_tmp567=(struct Cyc_Absyn_Var_e_struct*)
_tmp562;if(_tmp567->tag != 1)goto _LL2ED;else{_tmp568=(void*)_tmp567->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp569=(struct Cyc_Absyn_Local_b_struct*)_tmp568;if(
_tmp569->tag != 4)goto _LL2ED;else{_tmp56A=_tmp569->f1;}};}}if(!(!_tmp56A->escapes))
goto _LL2ED;_LL2EC: _tmp56E=_tmp56A;goto _LL2EE;_LL2ED:{struct Cyc_Absyn_Var_e_struct*
_tmp56B=(struct Cyc_Absyn_Var_e_struct*)_tmp562;if(_tmp56B->tag != 1)goto _LL2EF;
else{_tmp56C=(void*)_tmp56B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp56D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp56C;if(_tmp56D->tag != 5)goto _LL2EF;else{_tmp56E=_tmp56D->f1;}};}}if(!(!
_tmp56E->escapes))goto _LL2EF;_LL2EE: _tmp572=_tmp56E;goto _LL2F0;_LL2EF:{struct Cyc_Absyn_Var_e_struct*
_tmp56F=(struct Cyc_Absyn_Var_e_struct*)_tmp562;if(_tmp56F->tag != 1)goto _LL2F1;
else{_tmp570=(void*)_tmp56F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp571=(struct
Cyc_Absyn_Param_b_struct*)_tmp570;if(_tmp571->tag != 3)goto _LL2F1;else{_tmp572=
_tmp571->f1;}};}}if(!(!_tmp572->escapes))goto _LL2F1;_LL2F0: {struct _RegionHandle*
_tmp573=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp92D;struct Cyc_List_List*
_tmp92C;struct Cyc_List_List*_tmp574=(_tmp92C=_region_malloc(_tmp573,sizeof(*
_tmp92C)),((_tmp92C->hd=((_tmp92D=_region_malloc(_tmp573,sizeof(*_tmp92D)),((
_tmp92D->vd=_tmp535,((_tmp92D->rop=Cyc_CfFlowInfo_LessNumelts(_tmp572),_tmp92D)))))),((
_tmp92C->tl=_tmp4CA,_tmp92C)))));struct _tuple15 _tmp92E;return(_tmp92E.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4C9,_tmp574),((_tmp92E.f2=_tmp51D,_tmp92E)));}_LL2F1:;_LL2F2: {struct
_tuple15 _tmp92F;return(_tmp92F.f1=_tmp51C,((_tmp92F.f2=_tmp51D,_tmp92F)));}
_LL2E8:;}_LL2E6:;_LL2E7: {struct _tuple15 _tmp930;return(_tmp930.f1=_tmp51C,((
_tmp930.f2=_tmp51D,_tmp930)));}_LL2D7:;}_LL2D5:;_LL2D6: {struct _tuple15 _tmp931;
return(_tmp931.f1=_tmp51C,((_tmp931.f2=_tmp51D,_tmp931)));}_LL2CC:;};}case Cyc_Absyn_Lte:
_LL2C6: {union Cyc_CfFlowInfo_FlowInfo _tmp57B=f;union Cyc_CfFlowInfo_FlowInfo
_tmp57C=f;{union Cyc_CfFlowInfo_FlowInfo _tmp57D=_tmp57B;int _tmp57E;struct _tuple12
_tmp57F;struct Cyc_Dict_Dict _tmp580;_LL2F5: if((_tmp57D.BottomFL).tag != 1)goto
_LL2F7;_tmp57E=(int)(_tmp57D.BottomFL).val;_LL2F6: {struct Cyc_Core_Impossible_struct
_tmp937;const char*_tmp936;struct Cyc_Core_Impossible_struct*_tmp935;(int)_throw((
void*)((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935[0]=((_tmp937.tag=Cyc_Core_Impossible,((
_tmp937.f1=((_tmp936="anal_test, Lte",_tag_dyneither(_tmp936,sizeof(char),15))),
_tmp937)))),_tmp935)))));}_LL2F7: if((_tmp57D.ReachableFL).tag != 2)goto _LL2F4;
_tmp57F=(struct _tuple12)(_tmp57D.ReachableFL).val;_tmp580=_tmp57F.f1;_LL2F8:
_tmp4C9=_tmp580;_LL2F4:;}{void*_tmp584=_tmp4CC->r;void*_tmp586;struct Cyc_Absyn_Vardecl*
_tmp588;void*_tmp58A;struct Cyc_Absyn_Vardecl*_tmp58C;void*_tmp58E;struct Cyc_Absyn_Vardecl*
_tmp590;void*_tmp592;struct Cyc_Absyn_Vardecl*_tmp594;_LL2FA:{struct Cyc_Absyn_Var_e_struct*
_tmp585=(struct Cyc_Absyn_Var_e_struct*)_tmp584;if(_tmp585->tag != 1)goto _LL2FC;
else{_tmp586=(void*)_tmp585->f2;{struct Cyc_Absyn_Global_b_struct*_tmp587=(struct
Cyc_Absyn_Global_b_struct*)_tmp586;if(_tmp587->tag != 1)goto _LL2FC;else{_tmp588=
_tmp587->f1;}};}}if(!(!_tmp588->escapes))goto _LL2FC;_LL2FB: _tmp58C=_tmp588;goto
_LL2FD;_LL2FC:{struct Cyc_Absyn_Var_e_struct*_tmp589=(struct Cyc_Absyn_Var_e_struct*)
_tmp584;if(_tmp589->tag != 1)goto _LL2FE;else{_tmp58A=(void*)_tmp589->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp58B=(struct Cyc_Absyn_Local_b_struct*)_tmp58A;if(
_tmp58B->tag != 4)goto _LL2FE;else{_tmp58C=_tmp58B->f1;}};}}if(!(!_tmp58C->escapes))
goto _LL2FE;_LL2FD: _tmp590=_tmp58C;goto _LL2FF;_LL2FE:{struct Cyc_Absyn_Var_e_struct*
_tmp58D=(struct Cyc_Absyn_Var_e_struct*)_tmp584;if(_tmp58D->tag != 1)goto _LL300;
else{_tmp58E=(void*)_tmp58D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp58F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp58E;if(_tmp58F->tag != 5)goto _LL300;else{_tmp590=_tmp58F->f1;}};}}if(!(!
_tmp590->escapes))goto _LL300;_LL2FF: _tmp594=_tmp590;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_struct*
_tmp591=(struct Cyc_Absyn_Var_e_struct*)_tmp584;if(_tmp591->tag != 1)goto _LL302;
else{_tmp592=(void*)_tmp591->f2;{struct Cyc_Absyn_Param_b_struct*_tmp593=(struct
Cyc_Absyn_Param_b_struct*)_tmp592;if(_tmp593->tag != 3)goto _LL302;else{_tmp594=
_tmp593->f1;}};}}if(!(!_tmp594->escapes))goto _LL302;_LL301: {void*_tmp595=
_tmp4CD->r;union Cyc_Absyn_Cnst _tmp597;struct _tuple6 _tmp598;int _tmp599;struct Cyc_Absyn_Exp*
_tmp59B;struct Cyc_Absyn_Exp _tmp59C;void*_tmp59D;union Cyc_Absyn_Cnst _tmp59F;
struct _tuple6 _tmp5A0;int _tmp5A1;enum Cyc_Absyn_Primop _tmp5A3;struct Cyc_List_List*
_tmp5A4;struct Cyc_List_List _tmp5A5;struct Cyc_Absyn_Exp*_tmp5A6;_LL305: {struct
Cyc_Absyn_Const_e_struct*_tmp596=(struct Cyc_Absyn_Const_e_struct*)_tmp595;if(
_tmp596->tag != 0)goto _LL307;else{_tmp597=_tmp596->f1;if((_tmp597.Int_c).tag != 4)
goto _LL307;_tmp598=(struct _tuple6)(_tmp597.Int_c).val;_tmp599=_tmp598.f2;}}
_LL306: _tmp5A1=_tmp599;goto _LL308;_LL307: {struct Cyc_Absyn_Cast_e_struct*_tmp59A=(
struct Cyc_Absyn_Cast_e_struct*)_tmp595;if(_tmp59A->tag != 15)goto _LL309;else{
_tmp59B=_tmp59A->f2;_tmp59C=*_tmp59B;_tmp59D=_tmp59C.r;{struct Cyc_Absyn_Const_e_struct*
_tmp59E=(struct Cyc_Absyn_Const_e_struct*)_tmp59D;if(_tmp59E->tag != 0)goto _LL309;
else{_tmp59F=_tmp59E->f1;if((_tmp59F.Int_c).tag != 4)goto _LL309;_tmp5A0=(struct
_tuple6)(_tmp59F.Int_c).val;_tmp5A1=_tmp5A0.f2;}};}}_LL308: {struct _RegionHandle*
_tmp5A7=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp93A;struct Cyc_List_List*
_tmp939;struct Cyc_List_List*_tmp5A8=(_tmp939=_region_malloc(_tmp5A7,sizeof(*
_tmp939)),((_tmp939->hd=((_tmp93A=_region_malloc(_tmp5A7,sizeof(*_tmp93A)),((
_tmp93A->vd=_tmp594,((_tmp93A->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp5A1),_tmp93A)))))),((_tmp939->tl=_tmp4CA,_tmp939)))));struct _tuple15 _tmp93B;
return(_tmp93B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C9,_tmp5A8),((_tmp93B.f2=
_tmp57C,_tmp93B)));}_LL309: {struct Cyc_Absyn_Primop_e_struct*_tmp5A2=(struct Cyc_Absyn_Primop_e_struct*)
_tmp595;if(_tmp5A2->tag != 3)goto _LL30B;else{_tmp5A3=_tmp5A2->f1;_tmp5A4=_tmp5A2->f2;
if(_tmp5A4 == 0)goto _LL30B;_tmp5A5=*_tmp5A4;_tmp5A6=(struct Cyc_Absyn_Exp*)_tmp5A5.hd;}}
_LL30A: {void*_tmp5AC=_tmp5A6->r;void*_tmp5AE;struct Cyc_Absyn_Vardecl*_tmp5B0;
void*_tmp5B2;struct Cyc_Absyn_Vardecl*_tmp5B4;void*_tmp5B6;struct Cyc_Absyn_Vardecl*
_tmp5B8;void*_tmp5BA;struct Cyc_Absyn_Vardecl*_tmp5BC;_LL30E:{struct Cyc_Absyn_Var_e_struct*
_tmp5AD=(struct Cyc_Absyn_Var_e_struct*)_tmp5AC;if(_tmp5AD->tag != 1)goto _LL310;
else{_tmp5AE=(void*)_tmp5AD->f2;{struct Cyc_Absyn_Global_b_struct*_tmp5AF=(struct
Cyc_Absyn_Global_b_struct*)_tmp5AE;if(_tmp5AF->tag != 1)goto _LL310;else{_tmp5B0=
_tmp5AF->f1;}};}}if(!(!_tmp5B0->escapes))goto _LL310;_LL30F: _tmp5B4=_tmp5B0;goto
_LL311;_LL310:{struct Cyc_Absyn_Var_e_struct*_tmp5B1=(struct Cyc_Absyn_Var_e_struct*)
_tmp5AC;if(_tmp5B1->tag != 1)goto _LL312;else{_tmp5B2=(void*)_tmp5B1->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp5B3=(struct Cyc_Absyn_Local_b_struct*)_tmp5B2;if(
_tmp5B3->tag != 4)goto _LL312;else{_tmp5B4=_tmp5B3->f1;}};}}if(!(!_tmp5B4->escapes))
goto _LL312;_LL311: _tmp5B8=_tmp5B4;goto _LL313;_LL312:{struct Cyc_Absyn_Var_e_struct*
_tmp5B5=(struct Cyc_Absyn_Var_e_struct*)_tmp5AC;if(_tmp5B5->tag != 1)goto _LL314;
else{_tmp5B6=(void*)_tmp5B5->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5B7=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5B6;if(_tmp5B7->tag != 5)goto _LL314;else{_tmp5B8=_tmp5B7->f1;}};}}if(!(!
_tmp5B8->escapes))goto _LL314;_LL313: _tmp5BC=_tmp5B8;goto _LL315;_LL314:{struct Cyc_Absyn_Var_e_struct*
_tmp5B9=(struct Cyc_Absyn_Var_e_struct*)_tmp5AC;if(_tmp5B9->tag != 1)goto _LL316;
else{_tmp5BA=(void*)_tmp5B9->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5BB=(struct
Cyc_Absyn_Param_b_struct*)_tmp5BA;if(_tmp5BB->tag != 3)goto _LL316;else{_tmp5BC=
_tmp5BB->f1;}};}}if(!(!_tmp5BC->escapes))goto _LL316;_LL315: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5BD=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp93E;struct Cyc_List_List*_tmp93D;
struct Cyc_List_List*_tmp5BE=(_tmp93D=_region_malloc(_tmp5BD->r,sizeof(*_tmp93D)),((
_tmp93D->hd=((_tmp93E=_region_malloc(_tmp5BD->r,sizeof(*_tmp93E)),((_tmp93E->vd=
_tmp594,((_tmp93E->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5BC),_tmp93E)))))),((
_tmp93D->tl=_tmp4CA,_tmp93D)))));struct _tuple15 _tmp93F;return(_tmp93F.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4C9,_tmp5BE),((_tmp93F.f2=_tmp57C,_tmp93F)));}_LL316:;_LL317: {struct
_tuple15 _tmp940;return(_tmp940.f1=_tmp57B,((_tmp940.f2=_tmp57C,_tmp940)));}
_LL30D:;}_LL30B:;_LL30C: {struct _tuple15 _tmp941;return(_tmp941.f1=_tmp57B,((
_tmp941.f2=_tmp57C,_tmp941)));}_LL304:;}_LL302:;_LL303: {struct _tuple15 _tmp942;
return(_tmp942.f1=_tmp57B,((_tmp942.f2=_tmp57C,_tmp942)));}_LL2F9:;};}default:
_LL2F3: {struct _tuple15 _tmp943;return(_tmp943.f1=f,((_tmp943.f2=f,_tmp943)));}}}
_LL294:;};}_LL292:;_LL293: goto _LL285;_LL285:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp5C7;void*_tmp5C8;struct _tuple13 _tmp5C6=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5C9=_tmp5C7;int _tmp5CA;struct _tuple12 _tmp5CB;struct Cyc_Dict_Dict _tmp5CC;
_LL31A: if((_tmp5C9.BottomFL).tag != 1)goto _LL31C;_tmp5CA=(int)(_tmp5C9.BottomFL).val;
_LL31B: {struct _tuple15 _tmp944;return(_tmp944.f1=_tmp5C7,((_tmp944.f2=_tmp5C7,
_tmp944)));}_LL31C: if((_tmp5C9.ReachableFL).tag != 2)goto _LL319;_tmp5CB=(struct
_tuple12)(_tmp5C9.ReachableFL).val;_tmp5CC=_tmp5CB.f1;_LL31D: {void*_tmp5CE=
_tmp5C8;enum Cyc_CfFlowInfo_InitLevel _tmp5D4;enum Cyc_CfFlowInfo_InitLevel
_tmp5D6;enum Cyc_CfFlowInfo_InitLevel _tmp5D9;_LL31F: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5CF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5CE;if(_tmp5CF->tag != 0)goto
_LL321;}_LL320: {struct _tuple15 _tmp945;return(_tmp945.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp945.f2=_tmp5C7,_tmp945)));}_LL321: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp5D0=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp5CE;if(_tmp5D0->tag != 2)
goto _LL323;}_LL322: goto _LL324;_LL323: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5D1=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5CE;if(_tmp5D1->tag != 1)
goto _LL325;}_LL324: goto _LL326;_LL325: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5D2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5CE;if(_tmp5D2->tag != 5)goto
_LL327;}_LL326: {struct _tuple15 _tmp946;return(_tmp946.f1=_tmp5C7,((_tmp946.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp946)));}_LL327: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5D3=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5CE;if(_tmp5D3->tag != 3)goto
_LL329;else{_tmp5D4=_tmp5D3->f1;if(_tmp5D4 != Cyc_CfFlowInfo_NoneIL)goto _LL329;}}
_LL328: goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5D5=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5CE;if(_tmp5D5->tag != 4)goto _LL32B;else{_tmp5D6=_tmp5D5->f1;if(_tmp5D6 != Cyc_CfFlowInfo_NoneIL)
goto _LL32B;}}_LL32A: goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5D7=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5CE;if(_tmp5D7->tag != 7)goto
_LL32D;}_LL32C:{const char*_tmp949;void*_tmp948;(_tmp948=0,Cyc_Tcutil_terr(e->loc,((
_tmp949="expression may not be initialized",_tag_dyneither(_tmp949,sizeof(char),
34))),_tag_dyneither(_tmp948,sizeof(void*),0)));}{struct _tuple15 _tmp94A;return(
_tmp94A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp94A.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp94A)));};_LL32D: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5D8=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5CE;if(_tmp5D8->tag != 3)goto _LL32F;else{_tmp5D9=_tmp5D8->f1;}}_LL32E: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5C7,e,_tmp5D9);_LL32F: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5DA=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5CE;if(_tmp5DA->tag != 4)goto _LL331;}
_LL330: {struct _tuple15 _tmp94B;return(_tmp94B.f1=_tmp5C7,((_tmp94B.f2=_tmp5C7,
_tmp94B)));}_LL331: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5DB=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5CE;if(_tmp5DB->tag != 6)goto _LL31E;}_LL332: {struct Cyc_Core_Impossible_struct
_tmp951;const char*_tmp950;struct Cyc_Core_Impossible_struct*_tmp94F;(int)_throw((
void*)((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F[0]=((_tmp951.tag=Cyc_Core_Impossible,((
_tmp951.f1=((_tmp950="anal_test",_tag_dyneither(_tmp950,sizeof(char),10))),
_tmp951)))),_tmp94F)))));}_LL31E:;}_LL319:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5E6;struct Cyc_Position_Segment*
_tmp5E7;struct Cyc_NewControlFlow_AnalEnv*_tmp5E8;struct Cyc_Dict_Dict _tmp5E9;
struct _tuple22*_tmp5E5=ckenv;_tmp5E6=*_tmp5E5;_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;
_tmp5E9=_tmp5E6.f3;{void*_tmp5EA=root;struct Cyc_Absyn_Vardecl*_tmp5EC;_LL334: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5EB=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5EA;if(_tmp5EB->tag != 0)goto _LL336;else{_tmp5EC=_tmp5EB->f1;}}_LL335: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5E8->fenv)->r,_tmp5EC->type)){void*_tmp5ED=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5F1;_LL339: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5EE=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5ED;if(_tmp5EE->tag != 7)goto _LL33B;}_LL33A: goto _LL33C;_LL33B: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5EF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5ED;if(_tmp5EF->tag != 0)goto
_LL33D;}_LL33C: goto _LL33E;_LL33D: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5F0=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5ED;if(_tmp5F0->tag != 3)goto _LL33F;
else{_tmp5F1=_tmp5F0->f1;if(_tmp5F1 != Cyc_CfFlowInfo_NoneIL)goto _LL33F;}}_LL33E:
goto _LL338;_LL33F:;_LL340:{const char*_tmp955;void*_tmp954[1];struct Cyc_String_pa_struct
_tmp953;(_tmp953.tag=0,((_tmp953.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5EC->name)),((_tmp954[0]=& _tmp953,Cyc_Tcutil_warn(
_tmp5E7,((_tmp955="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp955,sizeof(char),57))),_tag_dyneither(_tmp954,sizeof(void*),1)))))));}
goto _LL338;_LL338:;}goto _LL333;_LL336:;_LL337: goto _LL333;_LL333:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow){union Cyc_CfFlowInfo_FlowInfo _tmp5F5=flow;int _tmp5F6;struct _tuple12 _tmp5F7;
struct Cyc_Dict_Dict _tmp5F8;_LL342: if((_tmp5F5.BottomFL).tag != 1)goto _LL344;
_tmp5F6=(int)(_tmp5F5.BottomFL).val;_LL343: return;_LL344: if((_tmp5F5.ReachableFL).tag
!= 2)goto _LL341;_tmp5F7=(struct _tuple12)(_tmp5F5.ReachableFL).val;_tmp5F8=
_tmp5F7.f1;_LL345:{struct Cyc_List_List*_tmp5F9=env->param_roots;for(0;_tmp5F9 != 
0;_tmp5F9=_tmp5F9->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5F8,Cyc_CfFlowInfo_lookup_place(
_tmp5F8,(struct Cyc_CfFlowInfo_Place*)_tmp5F9->hd))!= Cyc_CfFlowInfo_AllIL){const
char*_tmp958;void*_tmp957;(_tmp957=0,Cyc_Tcutil_terr(loc,((_tmp958="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp958,sizeof(char),76))),_tag_dyneither(_tmp957,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp959;struct _tuple22
_tmp5FC=(_tmp959.f1=loc,((_tmp959.f2=env,((_tmp959.f3=_tmp5F8,_tmp959)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5FC,_tmp5F8);}
return;_LL341:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5FE=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp600;struct Cyc_Core_Opt*_tmp601;
struct Cyc_Absyn_Exp*_tmp602;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Position_Segment*
_tmp604;struct Cyc_Absyn_Switch_clause*_tmp5FF=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp600=*_tmp5FF;_tmp601=_tmp600.pat_vars;_tmp602=_tmp600.where_clause;
_tmp603=_tmp600.body;_tmp604=_tmp600.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5FE,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp601))->v,_tmp5FE->unknown_all,_tmp604);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp602 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp602;union Cyc_CfFlowInfo_FlowInfo _tmp606;union Cyc_CfFlowInfo_FlowInfo
_tmp607;struct _tuple15 _tmp605=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp606=_tmp605.f1;_tmp607=_tmp605.f2;inflow=_tmp607;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp606,_tmp603);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp603);}{union Cyc_CfFlowInfo_FlowInfo _tmp608=clause_outflow;int
_tmp609;_LL347: if((_tmp608.BottomFL).tag != 1)goto _LL349;_tmp609=(int)(_tmp608.BottomFL).val;
_LL348: goto _LL346;_LL349:;_LL34A: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp95C;void*_tmp95B;(_tmp95B=0,Cyc_Tcutil_terr(_tmp603->loc,((_tmp95C="switch clause may implicitly fallthru",
_tag_dyneither(_tmp95C,sizeof(char),38))),_tag_dyneither(_tmp95B,sizeof(void*),0)));}
else{const char*_tmp95F;void*_tmp95E;(_tmp95E=0,Cyc_Tcutil_warn(_tmp603->loc,((
_tmp95F="switch clause may implicitly fallthru",_tag_dyneither(_tmp95F,sizeof(
char),38))),_tag_dyneither(_tmp95E,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL346;_LL346:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd);static void Cyc_NewControlFlow_check_dropped_unique_vd(
struct _tuple23*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp60F;struct Cyc_NewControlFlow_AnalEnv*
_tmp610;struct Cyc_Dict_Dict _tmp611;struct Cyc_Position_Segment*_tmp612;struct
_tuple23*_tmp60E=vdenv;_tmp60F=*_tmp60E;_tmp610=_tmp60F.f1;_tmp611=_tmp60F.f2;
_tmp612=_tmp60F.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp610->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp962;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp961;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp613=(_tmp961=_region_malloc((
_tmp610->fenv)->r,sizeof(*_tmp961)),((_tmp961[0]=((_tmp962.tag=0,((_tmp962.f1=vd,
_tmp962)))),_tmp961)));void*_tmp614=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp611,(void*)_tmp613);void*_tmp615=_tmp614;enum Cyc_CfFlowInfo_InitLevel
_tmp619;_LL34C: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp616=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp615;if(_tmp616->tag != 7)goto _LL34E;}_LL34D: goto _LL34F;_LL34E: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp617=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp615;if(_tmp617->tag != 0)goto
_LL350;}_LL34F: goto _LL351;_LL350: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp618=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp615;if(_tmp618->tag != 3)goto _LL352;
else{_tmp619=_tmp618->f1;if(_tmp619 != Cyc_CfFlowInfo_NoneIL)goto _LL352;}}_LL351:
goto _LL34B;_LL352:;_LL353:{const char*_tmp966;void*_tmp965[1];struct Cyc_String_pa_struct
_tmp964;(_tmp964.tag=0,((_tmp964.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp965[0]=& _tmp964,Cyc_Tcutil_warn(_tmp612,((
_tmp966="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp966,sizeof(char),68))),_tag_dyneither(_tmp965,sizeof(void*),1)))))));}
goto _LL34B;_LL34B:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static void Cyc_NewControlFlow_check_dropped_unique(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo _tmp61F=inflow;struct _tuple12
_tmp620;struct Cyc_Dict_Dict _tmp621;_LL355: if((_tmp61F.ReachableFL).tag != 2)goto
_LL357;_tmp620=(struct _tuple12)(_tmp61F.ReachableFL).val;_tmp621=_tmp620.f1;
_LL356: {struct _tuple23 _tmp967;struct _tuple23 _tmp622=(_tmp967.f1=env,((_tmp967.f2=
_tmp621,((_tmp967.f3=decl->loc,_tmp967)))));struct Cyc_CfFlowInfo_FlowEnv*_tmp623=
env->fenv;{void*_tmp624=decl->r;struct Cyc_Absyn_Vardecl*_tmp626;struct Cyc_Core_Opt*
_tmp628;struct Cyc_Core_Opt _tmp629;struct Cyc_List_List*_tmp62A;struct Cyc_List_List*
_tmp62C;_LL35A: {struct Cyc_Absyn_Var_d_struct*_tmp625=(struct Cyc_Absyn_Var_d_struct*)
_tmp624;if(_tmp625->tag != 0)goto _LL35C;else{_tmp626=_tmp625->f1;}}_LL35B: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp622,_tmp626);goto _LL359;_LL35C: {struct Cyc_Absyn_Let_d_struct*_tmp627=(
struct Cyc_Absyn_Let_d_struct*)_tmp624;if(_tmp627->tag != 2)goto _LL35E;else{
_tmp628=_tmp627->f2;if(_tmp628 == 0)goto _LL35E;_tmp629=*_tmp628;_tmp62A=(struct
Cyc_List_List*)_tmp629.v;}}_LL35D: _tmp62C=_tmp62A;goto _LL35F;_LL35E: {struct Cyc_Absyn_Letv_d_struct*
_tmp62B=(struct Cyc_Absyn_Letv_d_struct*)_tmp624;if(_tmp62B->tag != 3)goto _LL360;
else{_tmp62C=_tmp62B->f1;}}_LL35F:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp622,_tmp62C);goto _LL359;_LL360:;_LL361: goto _LL359;_LL359:;}goto _LL354;}
_LL357:;_LL358: goto _LL354;_LL354:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp62F;union Cyc_CfFlowInfo_FlowInfo*_tmp630;struct _tuple16 _tmp62E=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;inflow=*_tmp630;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp631=env->fenv;void*_tmp632=s->r;struct Cyc_Absyn_Exp*_tmp635;struct Cyc_Absyn_Exp*
_tmp637;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Stmt*
_tmp63C;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_Absyn_Stmt*
_tmp640;struct _tuple9 _tmp642;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Stmt*
_tmp644;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_Absyn_Stmt*_tmp647;struct _tuple9
_tmp648;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Stmt*_tmp64A;struct Cyc_Absyn_Exp*
_tmp64C;struct _tuple9 _tmp64D;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Stmt*
_tmp64F;struct _tuple9 _tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Stmt*
_tmp652;struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_Absyn_Stmt*_tmp655;struct Cyc_List_List*
_tmp657;struct Cyc_Absyn_Switch_clause**_tmp658;struct Cyc_Absyn_Switch_clause*
_tmp659;struct Cyc_Absyn_Stmt*_tmp65B;struct Cyc_Absyn_Stmt*_tmp65D;struct Cyc_Absyn_Stmt*
_tmp65F;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_List_List*_tmp662;struct Cyc_Absyn_Stmt*
_tmp664;struct Cyc_List_List*_tmp665;struct Cyc_Absyn_Decl*_tmp667;struct Cyc_Absyn_Decl
_tmp668;void*_tmp669;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Tvar*_tmp66C;
struct Cyc_Absyn_Vardecl*_tmp66D;struct Cyc_Absyn_Stmt*_tmp66E;struct Cyc_Absyn_Decl*
_tmp670;struct Cyc_Absyn_Stmt*_tmp671;struct Cyc_Absyn_Stmt*_tmp673;struct Cyc_Absyn_Exp*
_tmp675;_LL363: {struct Cyc_Absyn_Skip_s_struct*_tmp633=(struct Cyc_Absyn_Skip_s_struct*)
_tmp632;if(_tmp633->tag != 0)goto _LL365;}_LL364: return inflow;_LL365: {struct Cyc_Absyn_Return_s_struct*
_tmp634=(struct Cyc_Absyn_Return_s_struct*)_tmp632;if(_tmp634->tag != 3)goto _LL367;
else{_tmp635=_tmp634->f1;if(_tmp635 != 0)goto _LL367;}}_LL366: if(env->noreturn){
const char*_tmp96A;void*_tmp969;(_tmp969=0,Cyc_Tcutil_terr(s->loc,((_tmp96A="`noreturn' function might return",
_tag_dyneither(_tmp96A,sizeof(char),33))),_tag_dyneither(_tmp969,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL367: {struct Cyc_Absyn_Return_s_struct*_tmp636=(struct Cyc_Absyn_Return_s_struct*)
_tmp632;if(_tmp636->tag != 3)goto _LL369;else{_tmp637=_tmp636->f1;}}_LL368: if(env->noreturn){
const char*_tmp96D;void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_terr(s->loc,((_tmp96D="`noreturn' function might return",
_tag_dyneither(_tmp96D,sizeof(char),33))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp67B;void*_tmp67C;struct _tuple13 _tmp67A=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp637));_tmp67B=_tmp67A.f1;
_tmp67C=_tmp67A.f2;_tmp67B=Cyc_NewControlFlow_use_Rval(env,_tmp637->loc,_tmp67B,
_tmp67C);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp67B);return Cyc_CfFlowInfo_BottomFL();};
_LL369: {struct Cyc_Absyn_Exp_s_struct*_tmp638=(struct Cyc_Absyn_Exp_s_struct*)
_tmp632;if(_tmp638->tag != 1)goto _LL36B;else{_tmp639=_tmp638->f1;}}_LL36A: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp639)).f1;_LL36B: {struct Cyc_Absyn_Seq_s_struct*
_tmp63A=(struct Cyc_Absyn_Seq_s_struct*)_tmp632;if(_tmp63A->tag != 2)goto _LL36D;
else{_tmp63B=_tmp63A->f1;_tmp63C=_tmp63A->f2;}}_LL36C: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp63B),_tmp63C);_LL36D: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp63D=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp632;if(_tmp63D->tag != 4)goto
_LL36F;else{_tmp63E=_tmp63D->f1;_tmp63F=_tmp63D->f2;_tmp640=_tmp63D->f3;}}_LL36E: {
union Cyc_CfFlowInfo_FlowInfo _tmp67E;union Cyc_CfFlowInfo_FlowInfo _tmp67F;struct
_tuple15 _tmp67D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp63E);_tmp67E=_tmp67D.f1;
_tmp67F=_tmp67D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp680=Cyc_NewControlFlow_anal_stmt(
env,_tmp67F,_tmp640);union Cyc_CfFlowInfo_FlowInfo _tmp681=Cyc_NewControlFlow_anal_stmt(
env,_tmp67E,_tmp63F);return Cyc_CfFlowInfo_join_flow(_tmp631,env->all_changed,
_tmp681,_tmp680);};}_LL36F: {struct Cyc_Absyn_While_s_struct*_tmp641=(struct Cyc_Absyn_While_s_struct*)
_tmp632;if(_tmp641->tag != 5)goto _LL371;else{_tmp642=_tmp641->f1;_tmp643=_tmp642.f1;
_tmp644=_tmp642.f2;_tmp645=_tmp641->f2;}}_LL370: {union Cyc_CfFlowInfo_FlowInfo*
_tmp683;struct _tuple16 _tmp682=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp644);_tmp683=_tmp682.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp684=*_tmp683;union
Cyc_CfFlowInfo_FlowInfo _tmp686;union Cyc_CfFlowInfo_FlowInfo _tmp687;struct
_tuple15 _tmp685=Cyc_NewControlFlow_anal_test(env,_tmp684,_tmp643);_tmp686=
_tmp685.f1;_tmp687=_tmp685.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp688=Cyc_NewControlFlow_anal_stmt(
env,_tmp686,_tmp645);Cyc_NewControlFlow_update_flow(env,_tmp644,_tmp688);return
_tmp687;};};}_LL371: {struct Cyc_Absyn_Do_s_struct*_tmp646=(struct Cyc_Absyn_Do_s_struct*)
_tmp632;if(_tmp646->tag != 14)goto _LL373;else{_tmp647=_tmp646->f1;_tmp648=_tmp646->f2;
_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;}}_LL372: {union Cyc_CfFlowInfo_FlowInfo
_tmp689=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp647);union Cyc_CfFlowInfo_FlowInfo*
_tmp68B;struct _tuple16 _tmp68A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp689,
_tmp64A);_tmp68B=_tmp68A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp68C=*_tmp68B;union
Cyc_CfFlowInfo_FlowInfo _tmp68E;union Cyc_CfFlowInfo_FlowInfo _tmp68F;struct
_tuple15 _tmp68D=Cyc_NewControlFlow_anal_test(env,_tmp68C,_tmp649);_tmp68E=
_tmp68D.f1;_tmp68F=_tmp68D.f2;Cyc_NewControlFlow_update_flow(env,_tmp647,_tmp68E);
return _tmp68F;};}_LL373: {struct Cyc_Absyn_For_s_struct*_tmp64B=(struct Cyc_Absyn_For_s_struct*)
_tmp632;if(_tmp64B->tag != 9)goto _LL375;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;
_tmp64E=_tmp64D.f1;_tmp64F=_tmp64D.f2;_tmp650=_tmp64B->f3;_tmp651=_tmp650.f1;
_tmp652=_tmp650.f2;_tmp653=_tmp64B->f4;}}_LL374: {union Cyc_CfFlowInfo_FlowInfo
_tmp690=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp64C)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp692;struct _tuple16 _tmp691=Cyc_NewControlFlow_pre_stmt_check(env,_tmp690,
_tmp64F);_tmp692=_tmp691.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp693=*_tmp692;union
Cyc_CfFlowInfo_FlowInfo _tmp695;union Cyc_CfFlowInfo_FlowInfo _tmp696;struct
_tuple15 _tmp694=Cyc_NewControlFlow_anal_test(env,_tmp693,_tmp64E);_tmp695=
_tmp694.f1;_tmp696=_tmp694.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp697=Cyc_NewControlFlow_anal_stmt(
env,_tmp695,_tmp653);union Cyc_CfFlowInfo_FlowInfo*_tmp699;struct _tuple16 _tmp698=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp697,_tmp652);_tmp699=_tmp698.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp69A=*_tmp699;union Cyc_CfFlowInfo_FlowInfo _tmp69B=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp69A,_tmp651)).f1;Cyc_NewControlFlow_update_flow(env,_tmp64F,_tmp69B);
return _tmp696;};};};}_LL375: {struct Cyc_Absyn_Break_s_struct*_tmp654=(struct Cyc_Absyn_Break_s_struct*)
_tmp632;if(_tmp654->tag != 6)goto _LL377;else{_tmp655=_tmp654->f1;if(_tmp655 != 0)
goto _LL377;}}_LL376: return Cyc_CfFlowInfo_BottomFL();_LL377: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp656=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp632;if(_tmp656->tag != 11)goto
_LL379;else{_tmp657=_tmp656->f1;_tmp658=_tmp656->f2;if(_tmp658 == 0)goto _LL379;
_tmp659=*_tmp658;}}_LL378: {struct _RegionHandle*_tmp69C=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp69E;struct Cyc_List_List*_tmp69F;struct _tuple19 _tmp69D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp69C,env,inflow,_tmp657,1,1);_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;for(0;
_tmp69F != 0;(_tmp69F=_tmp69F->tl,_tmp657=_tmp657->tl)){_tmp69E=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp657))->hd)->loc,
_tmp69E,(void*)_tmp69F->hd);}_tmp69E=Cyc_NewControlFlow_add_vars(_tmp631,_tmp69E,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp659->pat_vars))->v,
_tmp631->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp659->body,_tmp69E);return Cyc_CfFlowInfo_BottomFL();}_LL379: {struct Cyc_Absyn_Break_s_struct*
_tmp65A=(struct Cyc_Absyn_Break_s_struct*)_tmp632;if(_tmp65A->tag != 6)goto _LL37B;
else{_tmp65B=_tmp65A->f1;}}_LL37A: _tmp65D=_tmp65B;goto _LL37C;_LL37B: {struct Cyc_Absyn_Continue_s_struct*
_tmp65C=(struct Cyc_Absyn_Continue_s_struct*)_tmp632;if(_tmp65C->tag != 7)goto
_LL37D;else{_tmp65D=_tmp65C->f1;}}_LL37C: _tmp65F=_tmp65D;goto _LL37E;_LL37D: {
struct Cyc_Absyn_Goto_s_struct*_tmp65E=(struct Cyc_Absyn_Goto_s_struct*)_tmp632;
if(_tmp65E->tag != 8)goto _LL37F;else{_tmp65F=_tmp65E->f2;}}_LL37E: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp6A0=_tmp62F->encloser;struct Cyc_Absyn_Stmt*
_tmp6A1=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp65F)))->encloser;while(_tmp6A1 != _tmp6A0){struct Cyc_Absyn_Stmt*_tmp6A2=(Cyc_NewControlFlow_get_stmt_annot(
_tmp6A0))->encloser;if(_tmp6A2 == _tmp6A0){{const char*_tmp970;void*_tmp96F;(
_tmp96F=0,Cyc_Tcutil_terr(s->loc,((_tmp970="goto enters local scope or exception handler",
_tag_dyneither(_tmp970,sizeof(char),45))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}
break;}_tmp6A0=_tmp6A2;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp65F),inflow);return Cyc_CfFlowInfo_BottomFL();_LL37F: {struct Cyc_Absyn_Switch_s_struct*
_tmp660=(struct Cyc_Absyn_Switch_s_struct*)_tmp632;if(_tmp660->tag != 10)goto
_LL381;else{_tmp661=_tmp660->f1;_tmp662=_tmp660->f2;}}_LL380: {union Cyc_CfFlowInfo_FlowInfo
_tmp6A6;void*_tmp6A7;struct _tuple13 _tmp6A5=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp661);_tmp6A6=_tmp6A5.f1;_tmp6A7=_tmp6A5.f2;_tmp6A6=Cyc_NewControlFlow_use_Rval(
env,_tmp661->loc,_tmp6A6,_tmp6A7);return Cyc_NewControlFlow_anal_scs(env,_tmp6A6,
_tmp662);}_LL381: {struct Cyc_Absyn_TryCatch_s_struct*_tmp663=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp632;if(_tmp663->tag != 15)goto _LL383;else{_tmp664=_tmp663->f1;_tmp665=_tmp663->f2;}}
_LL382: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp664);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp665);{union Cyc_CfFlowInfo_FlowInfo _tmp6A8=scs_outflow;int
_tmp6A9;_LL38E: if((_tmp6A8.BottomFL).tag != 1)goto _LL390;_tmp6A9=(int)(_tmp6A8.BottomFL).val;
_LL38F: goto _LL38D;_LL390:;_LL391: {const char*_tmp973;void*_tmp972;(_tmp972=0,Cyc_Tcutil_terr(
s->loc,((_tmp973="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp973,sizeof(char),42))),_tag_dyneither(_tmp972,sizeof(void*),0)));}_LL38D:;}
outflow=s1_outflow;return outflow;};};}_LL383: {struct Cyc_Absyn_Decl_s_struct*
_tmp666=(struct Cyc_Absyn_Decl_s_struct*)_tmp632;if(_tmp666->tag != 12)goto _LL385;
else{_tmp667=_tmp666->f1;_tmp668=*_tmp667;_tmp669=_tmp668.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp66A=(struct Cyc_Absyn_Alias_d_struct*)_tmp669;if(_tmp66A->tag != 5)goto _LL385;
else{_tmp66B=_tmp66A->f1;_tmp66C=_tmp66A->f2;_tmp66D=_tmp66A->f3;}};_tmp66E=
_tmp666->f2;}}_LL384: {union Cyc_CfFlowInfo_FlowInfo _tmp6AD;union Cyc_CfFlowInfo_AbsLVal
_tmp6AE;struct _tuple14 _tmp6AC=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp66B);
_tmp6AD=_tmp6AC.f1;_tmp6AE=_tmp6AC.f2;{struct _tuple14 _tmp974;struct _tuple14
_tmp6B0=(_tmp974.f1=_tmp6AD,((_tmp974.f2=_tmp6AE,_tmp974)));union Cyc_CfFlowInfo_FlowInfo
_tmp6B1;struct _tuple12 _tmp6B2;struct Cyc_Dict_Dict _tmp6B3;struct Cyc_List_List*
_tmp6B4;union Cyc_CfFlowInfo_AbsLVal _tmp6B5;struct Cyc_CfFlowInfo_Place*_tmp6B6;
union Cyc_CfFlowInfo_FlowInfo _tmp6B7;int _tmp6B8;_LL393: _tmp6B1=_tmp6B0.f1;if((
_tmp6B1.ReachableFL).tag != 2)goto _LL395;_tmp6B2=(struct _tuple12)(_tmp6B1.ReachableFL).val;
_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B0.f2;if((_tmp6B5.PlaceL).tag
!= 1)goto _LL395;_tmp6B6=(struct Cyc_CfFlowInfo_Place*)(_tmp6B5.PlaceL).val;_LL394: {
void*_tmp6B9=Cyc_CfFlowInfo_lookup_place(_tmp6B3,_tmp6B6);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp66B->topt))->v;{void*_tmp6BA=_tmp66B->r;struct Cyc_Absyn_Exp*
_tmp6BC;_LL39A: {struct Cyc_Absyn_Cast_e_struct*_tmp6BB=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6BA;if(_tmp6BB->tag != 15)goto _LL39C;else{_tmp6BC=_tmp6BB->f2;}}_LL39B: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6BC->topt))->v;goto _LL399;_LL39C:;
_LL39D: goto _LL399;_LL399:;}{void*_tmp6BD=Cyc_CfFlowInfo_make_unique_consumed(
_tmp631,t,_tmp66B,env->iteration_num,_tmp6B9);_tmp6B3=Cyc_CfFlowInfo_assign_place(
_tmp631,_tmp66B->loc,_tmp6B3,env->all_changed,_tmp6B6,_tmp6BD);_tmp6AD=Cyc_CfFlowInfo_ReachableFL(
_tmp6B3,_tmp6B4);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp631,_tmp6B3,
_tmp6B9)){case Cyc_CfFlowInfo_AllIL: _LL39E: leaf=_tmp631->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL39F: leaf=_tmp631->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL3A0: leaf=_tmp631->unknown_this;break;}{void*_tmp6BE=Cyc_CfFlowInfo_typ_to_absrval(
_tmp631,t,0,leaf);_tmp6AD=Cyc_NewControlFlow_use_Rval(env,_tmp66B->loc,_tmp6AD,
_tmp6B9);{struct Cyc_List_List _tmp975;struct Cyc_List_List _tmp6BF=(_tmp975.hd=
_tmp66D,((_tmp975.tl=0,_tmp975)));_tmp6AD=Cyc_NewControlFlow_add_vars(_tmp631,
_tmp6AD,(struct Cyc_List_List*)& _tmp6BF,_tmp631->unknown_all,s->loc);_tmp6AD=Cyc_NewControlFlow_anal_stmt(
env,_tmp6AD,_tmp66E);{union Cyc_CfFlowInfo_FlowInfo _tmp6C0=_tmp6AD;struct _tuple12
_tmp6C1;struct Cyc_Dict_Dict _tmp6C2;struct Cyc_List_List*_tmp6C3;_LL3A3: if((
_tmp6C0.ReachableFL).tag != 2)goto _LL3A5;_tmp6C1=(struct _tuple12)(_tmp6C0.ReachableFL).val;
_tmp6C2=_tmp6C1.f1;_tmp6C3=_tmp6C1.f2;_LL3A4: _tmp6C2=Cyc_CfFlowInfo_assign_place(
_tmp631,s->loc,_tmp6C2,env->all_changed,_tmp6B6,_tmp6BE);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6C2,_tmp6C3);return outflow;_LL3A5:;_LL3A6: return _tmp6AD;_LL3A2:;};};};};};}
_LL395: _tmp6B7=_tmp6B0.f1;if((_tmp6B7.BottomFL).tag != 1)goto _LL397;_tmp6B8=(int)(
_tmp6B7.BottomFL).val;_LL396: return _tmp6AD;_LL397:;_LL398:{const char*_tmp978;
void*_tmp977;(_tmp977=0,Cyc_Tcutil_terr(_tmp66B->loc,((_tmp978="bad alias expression--no unique path found",
_tag_dyneither(_tmp978,sizeof(char),43))),_tag_dyneither(_tmp977,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL392:;};}_LL385: {struct Cyc_Absyn_Decl_s_struct*
_tmp66F=(struct Cyc_Absyn_Decl_s_struct*)_tmp632;if(_tmp66F->tag != 12)goto _LL387;
else{_tmp670=_tmp66F->f1;_tmp671=_tmp66F->f2;}}_LL386: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp670),_tmp671);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp670);return outflow;
_LL387: {struct Cyc_Absyn_Label_s_struct*_tmp672=(struct Cyc_Absyn_Label_s_struct*)
_tmp632;if(_tmp672->tag != 13)goto _LL389;else{_tmp673=_tmp672->f2;}}_LL388: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp673);_LL389: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp674=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp632;if(_tmp674->tag != 16)goto
_LL38B;else{_tmp675=_tmp674->f1;}}_LL38A: {union Cyc_CfFlowInfo_FlowInfo _tmp6C8;
void*_tmp6C9;struct _tuple13 _tmp6C7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp675);_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6CA=Cyc_NewControlFlow_use_Rval(env,_tmp675->loc,_tmp6C8,_tmp6C9);void*
_tmp6CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp675->topt))->v);
void*_tmp6CD;_LL3A8: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6CC=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6CB;if(_tmp6CC->tag != 16)goto _LL3AA;else{_tmp6CD=(void*)_tmp6CC->f1;}}_LL3A9:
return Cyc_CfFlowInfo_kill_flow_region(_tmp631,_tmp6C8,_tmp6CD);_LL3AA:;_LL3AB: {
struct Cyc_Core_Impossible_struct _tmp97E;const char*_tmp97D;struct Cyc_Core_Impossible_struct*
_tmp97C;(int)_throw((void*)((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=((
_tmp97E.tag=Cyc_Core_Impossible,((_tmp97E.f1=((_tmp97D="anal_stmt -- reset_region",
_tag_dyneither(_tmp97D,sizeof(char),26))),_tmp97E)))),_tmp97C)))));}_LL3A7:;};}
_LL38B:;_LL38C: {struct Cyc_Core_Impossible_struct _tmp984;const char*_tmp983;
struct Cyc_Core_Impossible_struct*_tmp982;(int)_throw((void*)((_tmp982=_cycalloc(
sizeof(*_tmp982)),((_tmp982[0]=((_tmp984.tag=Cyc_Core_Impossible,((_tmp984.f1=((
_tmp983="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp983,sizeof(char),56))),_tmp984)))),_tmp982)))));}_LL362:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp6D4=env->fenv;
void*_tmp6D5=decl->r;struct Cyc_Absyn_Vardecl*_tmp6D7;struct Cyc_Core_Opt*_tmp6D9;
struct Cyc_Core_Opt _tmp6DA;struct Cyc_List_List*_tmp6DB;struct Cyc_Absyn_Exp*
_tmp6DC;struct Cyc_List_List*_tmp6DE;struct Cyc_Absyn_Fndecl*_tmp6E0;struct Cyc_Absyn_Tvar*
_tmp6E2;struct Cyc_Absyn_Vardecl*_tmp6E3;int _tmp6E4;struct Cyc_Absyn_Exp*_tmp6E5;
_LL3AD: {struct Cyc_Absyn_Var_d_struct*_tmp6D6=(struct Cyc_Absyn_Var_d_struct*)
_tmp6D5;if(_tmp6D6->tag != 0)goto _LL3AF;else{_tmp6D7=_tmp6D6->f1;}}_LL3AE: {
struct Cyc_List_List _tmp985;struct Cyc_List_List _tmp6E6=(_tmp985.hd=_tmp6D7,((
_tmp985.tl=0,_tmp985)));inflow=Cyc_NewControlFlow_add_vars(_tmp6D4,inflow,(
struct Cyc_List_List*)& _tmp6E6,_tmp6D4->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6E7=_tmp6D7->initializer;if(_tmp6E7 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6E9;void*_tmp6EA;struct _tuple13 _tmp6E8=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6E7);_tmp6E9=_tmp6E8.f1;_tmp6EA=_tmp6E8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6EB=_tmp6E9;int _tmp6EC;struct _tuple12 _tmp6ED;
struct Cyc_Dict_Dict _tmp6EE;struct Cyc_List_List*_tmp6EF;_LL3BA: if((_tmp6EB.BottomFL).tag
!= 1)goto _LL3BC;_tmp6EC=(int)(_tmp6EB.BottomFL).val;_LL3BB: return Cyc_CfFlowInfo_BottomFL();
_LL3BC: if((_tmp6EB.ReachableFL).tag != 2)goto _LL3B9;_tmp6ED=(struct _tuple12)(
_tmp6EB.ReachableFL).val;_tmp6EE=_tmp6ED.f1;_tmp6EF=_tmp6ED.f2;_LL3BD:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp98B;struct Cyc_CfFlowInfo_VarRoot_struct _tmp98A;struct Cyc_CfFlowInfo_Place*
_tmp989;_tmp6EE=Cyc_CfFlowInfo_assign_place(_tmp6D4,decl->loc,_tmp6EE,env->all_changed,((
_tmp989=_region_malloc(env->r,sizeof(*_tmp989)),((_tmp989->root=(void*)((_tmp98B=
_region_malloc(env->r,sizeof(*_tmp98B)),((_tmp98B[0]=((_tmp98A.tag=0,((_tmp98A.f1=
_tmp6D7,_tmp98A)))),_tmp98B)))),((_tmp989->fields=0,_tmp989)))))),_tmp6EA);}
_tmp6EF=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6EF,_tmp6D7,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6D7->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6F3=Cyc_CfFlowInfo_ReachableFL(
_tmp6EE,_tmp6EF);Cyc_NewControlFlow_update_tryflow(env,_tmp6F3);return _tmp6F3;};
_LL3B9:;};};};}_LL3AF: {struct Cyc_Absyn_Let_d_struct*_tmp6D8=(struct Cyc_Absyn_Let_d_struct*)
_tmp6D5;if(_tmp6D8->tag != 2)goto _LL3B1;else{_tmp6D9=_tmp6D8->f2;if(_tmp6D9 == 0)
goto _LL3B1;_tmp6DA=*_tmp6D9;_tmp6DB=(struct Cyc_List_List*)_tmp6DA.v;_tmp6DC=
_tmp6D8->f3;}}_LL3B0: {union Cyc_CfFlowInfo_FlowInfo _tmp6F6;void*_tmp6F7;struct
_tuple13 _tmp6F5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6DC);_tmp6F6=
_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;_tmp6F6=Cyc_NewControlFlow_use_Rval(env,_tmp6DC->loc,
_tmp6F6,_tmp6F7);return Cyc_NewControlFlow_add_vars(_tmp6D4,_tmp6F6,_tmp6DB,
_tmp6D4->unknown_all,decl->loc);}_LL3B1: {struct Cyc_Absyn_Letv_d_struct*_tmp6DD=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6D5;if(_tmp6DD->tag != 3)goto _LL3B3;else{
_tmp6DE=_tmp6DD->f1;}}_LL3B2: return Cyc_NewControlFlow_add_vars(_tmp6D4,inflow,
_tmp6DE,_tmp6D4->unknown_none,decl->loc);_LL3B3: {struct Cyc_Absyn_Fn_d_struct*
_tmp6DF=(struct Cyc_Absyn_Fn_d_struct*)_tmp6D5;if(_tmp6DF->tag != 1)goto _LL3B5;
else{_tmp6E0=_tmp6DF->f1;}}_LL3B4: Cyc_NewControlFlow_check_nested_fun(_tmp6D4,
inflow,_tmp6E0);{void*_tmp6F8=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6E0->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6F9=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6E0->fn_vardecl);
struct Cyc_List_List*_tmp98C;return Cyc_NewControlFlow_add_vars(_tmp6D4,inflow,((
_tmp98C=_region_malloc(env->r,sizeof(*_tmp98C)),((_tmp98C->hd=_tmp6F9,((_tmp98C->tl=
0,_tmp98C)))))),_tmp6D4->unknown_all,decl->loc);};_LL3B5: {struct Cyc_Absyn_Region_d_struct*
_tmp6E1=(struct Cyc_Absyn_Region_d_struct*)_tmp6D5;if(_tmp6E1->tag != 4)goto _LL3B7;
else{_tmp6E2=_tmp6E1->f1;_tmp6E3=_tmp6E1->f2;_tmp6E4=_tmp6E1->f3;_tmp6E5=_tmp6E1->f4;}}
_LL3B6: if(_tmp6E5 != 0){struct Cyc_Absyn_Exp*_tmp6FB=(struct Cyc_Absyn_Exp*)_tmp6E5;
union Cyc_CfFlowInfo_FlowInfo _tmp6FD;void*_tmp6FE;struct _tuple13 _tmp6FC=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6FB);_tmp6FD=_tmp6FC.f1;_tmp6FE=_tmp6FC.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6FB->loc,_tmp6FD,_tmp6FE);}{struct Cyc_List_List _tmp98D;struct Cyc_List_List
_tmp6FF=(_tmp98D.hd=_tmp6E3,((_tmp98D.tl=0,_tmp98D)));return Cyc_NewControlFlow_add_vars(
_tmp6D4,inflow,(struct Cyc_List_List*)& _tmp6FF,_tmp6D4->unknown_all,decl->loc);};
_LL3B7:;_LL3B8: {struct Cyc_Core_Impossible_struct _tmp993;const char*_tmp992;
struct Cyc_Core_Impossible_struct*_tmp991;(int)_throw((void*)((_tmp991=_cycalloc(
sizeof(*_tmp991)),((_tmp991[0]=((_tmp993.tag=Cyc_Core_Impossible,((_tmp993.f1=((
_tmp992="anal_decl: unexpected decl variant",_tag_dyneither(_tmp992,sizeof(char),
35))),_tmp993)))),_tmp991)))));}_LL3AC:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp704=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp704;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp705=fenv->r;struct Cyc_Position_Segment*_tmp706=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp706);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp707=inflow;int _tmp708;struct
_tuple12 _tmp709;struct Cyc_Dict_Dict _tmp70A;struct Cyc_List_List*_tmp70B;_LL3BF:
if((_tmp707.BottomFL).tag != 1)goto _LL3C1;_tmp708=(int)(_tmp707.BottomFL).val;
_LL3C0: {const char*_tmp996;void*_tmp995;(_tmp995=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp996="check_fun",
_tag_dyneither(_tmp996,sizeof(char),10))),_tag_dyneither(_tmp995,sizeof(void*),0)));}
_LL3C1: if((_tmp707.ReachableFL).tag != 2)goto _LL3BE;_tmp709=(struct _tuple12)(
_tmp707.ReachableFL).val;_tmp70A=_tmp709.f1;_tmp70B=_tmp709.f2;_LL3C2: {struct
Cyc_List_List*atts;{void*_tmp70E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp710;struct Cyc_List_List*
_tmp711;_LL3C4: {struct Cyc_Absyn_FnType_struct*_tmp70F=(struct Cyc_Absyn_FnType_struct*)
_tmp70E;if(_tmp70F->tag != 10)goto _LL3C6;else{_tmp710=_tmp70F->f1;_tmp711=_tmp710.attributes;}}
_LL3C5: atts=_tmp711;goto _LL3C3;_LL3C6:;_LL3C7: {const char*_tmp999;void*_tmp998;(
_tmp998=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp999="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp999,sizeof(char),50))),_tag_dyneither(_tmp998,sizeof(void*),0)));}_LL3C3:;}
for(0;atts != 0;atts=atts->tl){void*_tmp714=(void*)atts->hd;int _tmp716;int _tmp718;
_LL3C9: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp715=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp714;if(_tmp715->tag != 21)goto _LL3CB;else{_tmp716=_tmp715->f1;}}_LL3CA: {
unsigned int j=(unsigned int)_tmp716;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp99C;
void*_tmp99B;(_tmp99B=0,Cyc_Tcutil_terr(_tmp706,((_tmp99C="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp99C,sizeof(char),52))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp71B=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp71B->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp99F;void*
_tmp99E;(_tmp99E=0,Cyc_Tcutil_terr(_tmp706,((_tmp99F="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp99F,sizeof(char),52))),_tag_dyneither(_tmp99E,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp71E=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp9A2;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9A1;struct Cyc_CfFlowInfo_InitParam_struct*_tmp71F=(_tmp9A1=_region_malloc(
_tmp705,sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=2,((_tmp9A2.f1=(int)j,((
_tmp9A2.f2=(void*)t,_tmp9A2)))))),_tmp9A1)));struct Cyc_CfFlowInfo_Place*_tmp9A3;
struct Cyc_CfFlowInfo_Place*_tmp720=(_tmp9A3=_region_malloc(_tmp705,sizeof(*
_tmp9A3)),((_tmp9A3->root=(void*)_tmp71F,((_tmp9A3->fields=0,_tmp9A3)))));
_tmp70A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70A,(void*)_tmp71F,_tmp71E);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp9A9;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp9A8;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp9A6;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp9A5;_tmp70A=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp70A,(void*)((_tmp9A5=
_region_malloc(_tmp705,sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=0,((_tmp9A6.f1=
_tmp71B,_tmp9A6)))),_tmp9A5)))),(void*)((_tmp9A8=_region_malloc(_tmp705,sizeof(*
_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=5,((_tmp9A9.f1=_tmp720,_tmp9A9)))),_tmp9A8)))));}
goto _LL3C8;};};}_LL3CB: {struct Cyc_Absyn_Initializes_att_struct*_tmp717=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp714;if(_tmp717->tag != 20)goto _LL3CD;else{
_tmp718=_tmp717->f1;}}_LL3CC: {unsigned int j=(unsigned int)_tmp718;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9AC;void*_tmp9AB;(_tmp9AB=0,
Cyc_Tcutil_terr(_tmp706,((_tmp9AC="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9AC,sizeof(char),51))),_tag_dyneither(_tmp9AB,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp72A=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp72B=Cyc_Tcutil_compress(
_tmp72A->type);struct Cyc_Absyn_PtrInfo _tmp72D;void*_tmp72E;struct Cyc_Absyn_PtrAtts
_tmp72F;union Cyc_Absyn_Constraint*_tmp730;union Cyc_Absyn_Constraint*_tmp731;
union Cyc_Absyn_Constraint*_tmp732;_LL3D0: {struct Cyc_Absyn_PointerType_struct*
_tmp72C=(struct Cyc_Absyn_PointerType_struct*)_tmp72B;if(_tmp72C->tag != 5)goto
_LL3D2;else{_tmp72D=_tmp72C->f1;_tmp72E=_tmp72D.elt_typ;_tmp72F=_tmp72D.ptr_atts;
_tmp730=_tmp72F.nullable;_tmp731=_tmp72F.bounds;_tmp732=_tmp72F.zero_term;}}
_LL3D1: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp730)){
const char*_tmp9AF;void*_tmp9AE;(_tmp9AE=0,Cyc_Tcutil_terr(_tmp706,((_tmp9AF="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9AF,sizeof(char),55))),_tag_dyneither(_tmp9AE,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp731)){const char*_tmp9B2;void*_tmp9B1;(_tmp9B1=0,
Cyc_Tcutil_terr(_tmp706,((_tmp9B2="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9B2,sizeof(char),57))),_tag_dyneither(_tmp9B1,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp732)){
const char*_tmp9B5;void*_tmp9B4;(_tmp9B4=0,Cyc_Tcutil_terr(_tmp706,((_tmp9B5="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9B5,sizeof(char),77))),_tag_dyneither(_tmp9B4,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp9B8;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9B7;struct Cyc_CfFlowInfo_InitParam_struct*_tmp739=(_tmp9B7=_region_malloc(
_tmp705,sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=2,((_tmp9B8.f1=(int)j,((
_tmp9B8.f2=(void*)_tmp72E,_tmp9B8)))))),_tmp9B7)));struct Cyc_CfFlowInfo_Place*
_tmp9B9;struct Cyc_CfFlowInfo_Place*_tmp73A=(_tmp9B9=_region_malloc(_tmp705,
sizeof(*_tmp9B9)),((_tmp9B9->root=(void*)_tmp739,((_tmp9B9->fields=0,_tmp9B9)))));
_tmp70A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70A,(void*)_tmp739,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp72E,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp9BF;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp9BE;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9BC;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9BB;_tmp70A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70A,(void*)((_tmp9BB=_region_malloc(_tmp705,sizeof(*_tmp9BB)),((_tmp9BB[0]=((
_tmp9BC.tag=0,((_tmp9BC.f1=_tmp72A,_tmp9BC)))),_tmp9BB)))),(void*)((_tmp9BE=
_region_malloc(_tmp705,sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=5,((_tmp9BF.f1=
_tmp73A,_tmp9BF)))),_tmp9BE)))));}{struct Cyc_List_List*_tmp9C0;param_roots=((
_tmp9C0=_region_malloc(_tmp705,sizeof(*_tmp9C0)),((_tmp9C0->hd=_tmp73A,((_tmp9C0->tl=
param_roots,_tmp9C0))))));}goto _LL3CF;};_LL3D2:;_LL3D3: {const char*_tmp9C3;void*
_tmp9C2;(_tmp9C2=0,Cyc_Tcutil_terr(_tmp706,((_tmp9C3="initializes attribute on non-pointer",
_tag_dyneither(_tmp9C3,sizeof(char),37))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));}
_LL3CF:;}goto _LL3C8;};}_LL3CD:;_LL3CE: goto _LL3C8;_LL3C8:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp70A,_tmp70B);}_LL3BE:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp705,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9C4;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9C4=_region_malloc(_tmp705,sizeof(*_tmp9C4)),((
_tmp9C4->r=_tmp705,((_tmp9C4->fenv=fenv,((_tmp9C4->iterate_again=1,((_tmp9C4->iteration_num=
0,((_tmp9C4->in_try=0,((_tmp9C4->tryflow=inflow,((_tmp9C4->all_changed=0,((
_tmp9C4->noreturn=noreturn,((_tmp9C4->param_roots=param_roots,((_tmp9C4->flow_table=
flow_table,_tmp9C4)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp745=outflow;int _tmp746;_LL3D5: if((_tmp745.BottomFL).tag
!= 1)goto _LL3D7;_tmp746=(int)(_tmp745.BottomFL).val;_LL3D6: goto _LL3D4;_LL3D7:;
_LL3D8: Cyc_NewControlFlow_check_init_params(_tmp706,env,outflow);if(noreturn){
const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,Cyc_Tcutil_terr(_tmp706,((_tmp9C7="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9C7,sizeof(char),46))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
else{void*_tmp749=Cyc_Tcutil_compress(fd->ret_type);_LL3DA: {struct Cyc_Absyn_VoidType_struct*
_tmp74A=(struct Cyc_Absyn_VoidType_struct*)_tmp749;if(_tmp74A->tag != 0)goto _LL3DC;}
_LL3DB: goto _LL3D9;_LL3DC: {struct Cyc_Absyn_DoubleType_struct*_tmp74B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp749;if(_tmp74B->tag != 8)goto _LL3DE;}_LL3DD: goto _LL3DF;_LL3DE: {struct Cyc_Absyn_FloatType_struct*
_tmp74C=(struct Cyc_Absyn_FloatType_struct*)_tmp749;if(_tmp74C->tag != 7)goto
_LL3E0;}_LL3DF: goto _LL3E1;_LL3E0: {struct Cyc_Absyn_IntType_struct*_tmp74D=(
struct Cyc_Absyn_IntType_struct*)_tmp749;if(_tmp74D->tag != 6)goto _LL3E2;}_LL3E1:{
const char*_tmp9CA;void*_tmp9C9;(_tmp9C9=0,Cyc_Tcutil_warn(_tmp706,((_tmp9CA="function may not return a value",
_tag_dyneither(_tmp9CA,sizeof(char),32))),_tag_dyneither(_tmp9C9,sizeof(void*),0)));}
goto _LL3D9;_LL3E2:;_LL3E3:{const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,Cyc_Tcutil_terr(
_tmp706,((_tmp9CD="function may not return a value",_tag_dyneither(_tmp9CD,
sizeof(char),32))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}goto _LL3D9;_LL3D9:;}
goto _LL3D4;_LL3D4:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp753=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp755;
struct Cyc_List_List*_tmp757;struct Cyc_List_List*_tmp759;struct Cyc_List_List*
_tmp75B;_LL3E5: {struct Cyc_Absyn_Fn_d_struct*_tmp754=(struct Cyc_Absyn_Fn_d_struct*)
_tmp753;if(_tmp754->tag != 1)goto _LL3E7;else{_tmp755=_tmp754->f1;}}_LL3E6: Cyc_NewControlFlow_check_fun(
_tmp755);goto _LL3E4;_LL3E7: {struct Cyc_Absyn_ExternC_d_struct*_tmp756=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp753;if(_tmp756->tag != 12)goto _LL3E9;else{_tmp757=_tmp756->f1;}}_LL3E8:
_tmp759=_tmp757;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Using_d_struct*_tmp758=(
struct Cyc_Absyn_Using_d_struct*)_tmp753;if(_tmp758->tag != 11)goto _LL3EB;else{
_tmp759=_tmp758->f2;}}_LL3EA: _tmp75B=_tmp759;goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Namespace_d_struct*
_tmp75A=(struct Cyc_Absyn_Namespace_d_struct*)_tmp753;if(_tmp75A->tag != 10)goto
_LL3ED;else{_tmp75B=_tmp75A->f2;}}_LL3EC: Cyc_NewControlFlow_cf_check(_tmp75B);
goto _LL3E4;_LL3ED: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp75C=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp753;if(_tmp75C->tag != 13)goto _LL3EF;}_LL3EE: goto _LL3E4;_LL3EF:;_LL3F0: goto
_LL3E4;_LL3E4:;}}
